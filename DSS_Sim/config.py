from dataclasses import dataclass, field
from typing import List

@dataclass
class PeriodicTaskConfig:
    name: str
    period: int
    wcet: int
    deadline: int

@dataclass
class DSSConfig:
    budget: int
    period: int

@dataclass
class SimulationConfig:
    duration: int = 50000
    time_resolution: int = 1  # ms
    random_seed: int = 42
    aperiodic_arrival_prob: float = 0.025
    aperiodic_exec_range: tuple = (1, 5)
    
    periodic_tasks: List[PeriodicTaskConfig] = field(default_factory=lambda: [
        PeriodicTaskConfig("P1", 10, 2, 10),
        PeriodicTaskConfig("P2", 20, 4, 20),
        PeriodicTaskConfig("P3", 50, 6, 50),
    ])
    
    dss_config: DSSConfig = field(default_factory=lambda: DSSConfig(2, 5))
