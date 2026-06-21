from dataclasses import dataclass, field
from typing import List, Tuple, Optional
from tasks import AperiodicRequest

@dataclass
class ReplenishmentEvent:
    time: int
    amount: int

class DynamicSporadicServer:
    def __init__(self, budget: int, period: int):
        self.total_budget = budget
        self.period = period
        self.remaining_budget = budget
        self.current_deadline: Optional[int] = None
        self.last_deadline: int = 0
        self.replenishments: List[ReplenishmentEvent] = []
        self.request_queue: List[AperiodicRequest] = []
        
        # Statistics
        self.total_execution_time = 0
        self.budget_exhaustion_count = 0
        self.replenishment_count = 0

    def add_request(self, request: AperiodicRequest, current_time: int):
        self.request_queue.append(request)
        self._consume_budget(current_time)

    def _consume_budget(self, current_time: int):
        for request in self.request_queue:
            needed = request.remaining_time - request.reserved_budget
            if needed > 0 and self.remaining_budget > 0:
                amount = min(needed, self.remaining_budget)
                self.remaining_budget -= amount
                request.reserved_budget += amount
                self.replenishments.append(ReplenishmentEvent(current_time + self.period, amount))
                if self.remaining_budget == 0:
                    self.budget_exhaustion_count += 1

    def update_replenishments(self, current_time: int):
        new_replenishments = []
        replenished = False
        for rep in self.replenishments:
            if rep.time <= current_time:
                self.remaining_budget = min(self.total_budget, self.remaining_budget + rep.amount)
                self.replenishment_count += 1
                replenished = True
            else:
                new_replenishments.append(rep)
        self.replenishments = new_replenishments
        if replenished:
            self._consume_budget(current_time)

    def get_deadline(self, current_time: int) -> Optional[int]:
        has_ready_work = any(r.reserved_budget > 0 for r in self.request_queue)
        if not has_ready_work:
            self.current_deadline = None
            return None
        
        if self.current_deadline is None:
            # Assign new deadline
            self.current_deadline = max(current_time, self.last_deadline) + self.period
            self.last_deadline = self.current_deadline
            
        return self.current_deadline

    def execute(self, current_time: int) -> Optional[str]:
        if not self.request_queue:
            return None
        
        # Execute the first request in the queue that has reserved budget
        request = self.request_queue[0]
        if request.reserved_budget <= 0:
            return None
            
        request.remaining_time -= 1
        request.reserved_budget -= 1
        self.total_execution_time += 1
        
        req_name = request.name
        
        if request.remaining_time == 0:
            request.completion_time = current_time + 1
            request.response_time = request.completion_time - request.arrival_time
            request.waiting_time = request.response_time - request.execution_time
            self.request_queue.pop(0)
            if not self.request_queue:
                self.current_deadline = None # Reset deadline as no more work
                
        return req_name
