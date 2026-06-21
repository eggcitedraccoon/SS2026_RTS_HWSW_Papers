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
        
    def step(self, current_time: int, new_aperiodic_request: Optional[AperiodicRequest]):
        # 1. Release periodic jobs
        for task in self.periodic_tasks:
            if current_time >= task.next_release_time:
                self.ready_jobs.append(task.release_job(current_time))
        
        # 2. Process new aperiodic arrivals
        if new_aperiodic_request:
            if self.dss:
                self.dss.add_request(new_aperiodic_request)
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
            self.execution_trace.append("DSS")
            self.dss.execute(current_time)
            # Aperiodic requests are managed inside DSS
        elif selected_type == 'APERIODIC':
            req = entity
            req.remaining_time -= 1
            self.execution_trace.append("APERIODIC")
            if req.remaining_time == 0:
                req.completion_time = current_time + 1
                req.response_time = req.completion_time - req.arrival_time
                req.waiting_time = req.response_time - req.execution_time
                self.completed_aperiodic.append(req)
                self.aperiodic_requests.remove(req)
