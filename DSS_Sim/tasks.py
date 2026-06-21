from dataclasses import dataclass
from typing import Optional

@dataclass
class Job:
    task_name: str
    release_time: int
    absolute_deadline: int
    wcet: int
    remaining_time: int
    completion_time: Optional[int] = None
    deadline_missed: bool = False

@dataclass
class PeriodicTask:
    name: str
    period: int
    wcet: int
    relative_deadline: int
    next_release_time: int = 0

    def release_job(self, current_time: int) -> Job:
        job = Job(
            task_name=self.name,
            release_time=current_time,
            absolute_deadline=current_time + self.relative_deadline,
            wcet=self.wcet,
            remaining_time=self.wcet
        )
        self.next_release_time += self.period
        return job

@dataclass
class AperiodicRequest:
    arrival_time: int
    execution_time: int
    remaining_time: int
    completion_time: Optional[int] = None
    response_time: Optional[int] = None
    waiting_time: Optional[int] = None
