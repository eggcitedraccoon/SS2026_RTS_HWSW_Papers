from typing import List, Optional, Dict, Any
from tasks import PeriodicTask, Job, AperiodicRequest
from dss import DynamicSporadicServer

class EDFScheduler:
    def __init__(self, periodic_tasks: List[PeriodicTask], dss: Optional[DynamicSporadicServer] = None):
        self.periodic_tasks = periodic_tasks
        self.dss = dss
        self.ready_jobs: List[Job] = []
        self.aperiodic_requests: List[AperiodicRequest] = [] # Used when NO DSS (Mode A)
        self.completed_jobs: List[Job] = []
        self.completed_aperiodic: List[AperiodicRequest] = []
        self.deadline_misses = 0
        self.execution_trace = []
        self.budget_trace = []
        self.released_jobs: List[Job] = []
        
    def step(self, current_time: int, new_aperiodic_request: Optional[AperiodicRequest]):
        # 1. Release periodic jobs
        for task in self.periodic_tasks:
            if current_time >= task.next_release_time:
                job = task.release_job(current_time)
                self.ready_jobs.append(job)
                self.released_jobs.append(job)
        
        # 2. Process new aperiodic arrivals
        if new_aperiodic_request:
            if self.dss:
                self.dss.add_request(new_aperiodic_request, current_time)
            else:
                self.aperiodic_requests.append(new_aperiodic_request)
        
        # 3. Update replenishment events (if DSS)
        if self.dss:
            self.dss.update_replenishments(current_time)
            self.budget_trace.append(self.dss.remaining_budget)
        else:
            self.budget_trace.append(0)

        # 4. Check for deadline misses
        for job in self.ready_jobs:
            if current_time >= job.absolute_deadline and not job.deadline_missed:
                self.deadline_misses += 1
                job.deadline_missed = True

        # 5. Build EDF entities
        entities = []
        for job in self.ready_jobs:
            entities.append(('JOB', job.absolute_deadline, job))
            
        if self.dss:
            dss_deadline = self.dss.get_deadline(current_time)
            if dss_deadline is not None:
                entities.append(('DSS', dss_deadline, self.dss))
        else:
            # Mode A: aperiodic requests directly in EDF
            for req in self.aperiodic_requests:
                # Assign a large deadline as per requirements
                large_deadline = 1000000 
                entities.append(('APERIODIC', large_deadline, req))
                
        # 6. Select entity with earliest deadline
        if not entities:
            self.execution_trace.append("IDLE")
            return
        
        # Sort by deadline (second element of tuple)
        entities.sort(key=lambda x: x[1])
        selected_type, deadline, entity = entities[0]
        
        # 7. Execute for one tick
        if selected_type == 'JOB':
            job = entity
            job.remaining_time -= 1
            self.execution_trace.append(job.task_name)
            if job.remaining_time == 0:
                job.completion_time = current_time + 1
                self.completed_jobs.append(job)
                self.ready_jobs.remove(job)
        elif selected_type == 'DSS':
            req_name = self.dss.execute(current_time)
            if req_name:
                self.execution_trace.append(f"DSS:{req_name}")
            else:
                self.execution_trace.append("IDLE") # Should not happen if entities selected DSS
        elif selected_type == 'APERIODIC':
            req = entity
            req.remaining_time -= 1
            self.execution_trace.append(f"APERIODIC:{req.name}")
            if req.remaining_time == 0:
                req.completion_time = current_time + 1
                req.response_time = req.completion_time - req.arrival_time
                req.waiting_time = req.response_time - req.execution_time
                self.completed_aperiodic.append(req)
                self.aperiodic_requests.remove(req)
