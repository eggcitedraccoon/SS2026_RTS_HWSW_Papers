import pandas as pd
import numpy as np
from typing import List, Dict, Any, Optional
from tasks import Job, AperiodicRequest
from dss import DynamicSporadicServer

def collect_metrics(
    periodic_tasks: List[Any], 
    completed_jobs: List[Job], 
    ready_jobs: List[Job],
    deadline_misses: int,
    aperiodic_requests: List[AperiodicRequest],
    dss: Optional[DynamicSporadicServer],
    duration: int,
    mode_name: str
) -> Dict[str, Any]:
    
    # Periodic metrics
    total_released = len(completed_jobs) + len(ready_jobs)
    periodic_utilization = sum(job.wcet for job in completed_jobs) / duration
    
    # Aperiodic metrics
    completed_aperiodic = [r for r in aperiodic_requests if r.completion_time is not None]
    total_aperiodic = len(aperiodic_requests)
    
    response_times = [r.response_time for r in completed_aperiodic if r.response_time is not None]
    waiting_times = [r.waiting_time for r in completed_aperiodic if r.waiting_time is not None]
    
    avg_response = np.mean(response_times) if response_times else 0
    max_response = np.max(response_times) if response_times else 0
    avg_waiting = np.mean(waiting_times) if waiting_times else 0
    
    metrics = {
        "Mode": mode_name,
        "Periodic Released": total_released,
        "Periodic Completed": len(completed_jobs),
        "Periodic Deadline Misses": deadline_misses,
        "Periodic Utilization": periodic_utilization,
        "Aperiodic Total": total_aperiodic,
        "Aperiodic Completed": len(completed_aperiodic),
        "Avg Aperiodic Response Time": avg_response,
        "Max Aperiodic Response Time": max_response,
        "Avg Aperiodic Waiting Time": avg_waiting
    }
    
    if dss:
        metrics.update({
            "DSS Exec Time": dss.total_execution_time,
            "DSS Exhaustions": dss.budget_exhaustion_count,
            "DSS Replenishments": dss.replenishment_count
        })
        
    return metrics

def print_summary(metrics_list: List[Dict[str, Any]]):
    df = pd.DataFrame(metrics_list)
    print("\nSimulation Comparison Summary:")
    print(df.to_string(index=False))

def export_to_csv(metrics_list: List[Dict[str, Any]], filename: str):
    df = pd.DataFrame(metrics_list)
    df.to_csv(filename, index=False)
