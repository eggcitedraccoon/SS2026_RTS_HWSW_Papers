import os
import random
import numpy as np
from config import SimulationConfig
from tasks import PeriodicTask, AperiodicRequest
from dss import DynamicSporadicServer
from scheduler import EDFScheduler
from metrics import collect_metrics, print_summary, export_to_csv
from visualization import plot_gantt, plot_budget, plot_response_times, plot_response_histogram, plot_gantt_plotly

def run_simulation(config: SimulationConfig, use_dss: bool):
    # Set seed for reproducibility
    random.seed(config.random_seed)
    np.random.seed(config.random_seed)
    
    # Initialize tasks
    periodic_tasks = [
        PeriodicTask(t.name, t.period, t.wcet, t.deadline)
        for t in config.periodic_tasks
    ]
    
    dss = None
    if use_dss:
        dss = DynamicSporadicServer(config.dss_config.budget, config.dss_config.period)
        
    scheduler = EDFScheduler(periodic_tasks, dss)
    
    # Generate all aperiodic requests beforehand to ensure they are identical for both modes
    all_aperiodic_requests = []
    for t in range(config.duration):
        if random.random() < config.aperiodic_arrival_prob:
            exec_time = random.randint(config.aperiodic_exec_range[0], config.aperiodic_exec_range[1])
            all_aperiodic_requests.append(AperiodicRequest(t, exec_time, exec_time))
    
    # Simulation loop
    aperiodic_idx = 0
    all_seen_aperiodic = []
    
    for t in range(config.duration):
        current_req = None
        if aperiodic_idx < len(all_aperiodic_requests) and all_aperiodic_requests[aperiodic_idx].arrival_time == t:
            current_req = all_aperiodic_requests[aperiodic_idx]
            all_seen_aperiodic.append(current_req)
            aperiodic_idx += 1
            
        scheduler.step(t, current_req)
        
    # Finalize metrics
    mode_name = "Mode B (With DSS)" if use_dss else "Mode A (No DSS)"
    
    # In Mode A, requests are in scheduler.completed_aperiodic or scheduler.aperiodic_requests
    # In Mode B, requests are in dss.request_queue or they were completed. 
    # Actually, all_seen_aperiodic contains all requests released.
    
    results = {
        "scheduler": scheduler,
        "dss": dss,
        "aperiodic_requests": all_seen_aperiodic,
        "metrics": collect_metrics(
            periodic_tasks, 
            scheduler.completed_jobs, 
            scheduler.ready_jobs,
            scheduler.deadline_misses,
            all_seen_aperiodic,
            dss,
            config.duration,
            mode_name
        )
    }
    return results

def main():
    config = SimulationConfig()
    results_dir = "results"
    if not os.path.exists(results_dir):
        os.makedirs(results_dir)
        
    print("Running Mode A: EDF without Dynamic Sporadic Server...")
    res_a = run_simulation(config, use_dss=False)
    
    print("Running Mode B: EDF with Dynamic Sporadic Server...")
    res_b = run_simulation(config, use_dss=True)
    
    # Visualization
    print("Generating visualizations...")
    plot_gantt(res_a["scheduler"].execution_trace, config.duration, 
               "Gantt Chart - Mode A (No DSS)", os.path.join(results_dir, "gantt_mode_a.png"))
    plot_gantt(res_b["scheduler"].execution_trace, config.duration, 
               "Gantt Chart - Mode B (With DSS)", os.path.join(results_dir, "gantt_mode_b.png"))
    
    plot_gantt_plotly(res_a["scheduler"].execution_trace, config.duration, 
                      "Interactive Gantt - Mode A", os.path.join(results_dir, "gantt_a_interactive.html"))
    plot_gantt_plotly(res_b["scheduler"].execution_trace, config.duration, 
                      "Interactive Gantt - Mode B", os.path.join(results_dir, "gantt_b_interactive.html"))
    
    plot_budget(res_b["scheduler"].budget_trace, config.duration,
                "DSS Budget Over Time", os.path.join(results_dir, "dss_budget.png"))
    
    plot_response_times(res_a["aperiodic_requests"], "Aperiodic Response Times - Mode A", 
                        os.path.join(results_dir, "response_times_a.png"))
    plot_response_times(res_b["aperiodic_requests"], "Aperiodic Response Times - Mode B", 
                        os.path.join(results_dir, "response_times_b.png"))
    
    plot_response_histogram(res_a["aperiodic_requests"], res_b["aperiodic_requests"], 
                            os.path.join(results_dir, "response_comparison.png"))
    
    # Summary and Export
    metrics_list = [res_a["metrics"], res_b["metrics"]]
    print_summary(metrics_list)
    export_to_csv(metrics_list, os.path.join(results_dir, "comparison_results.csv"))
    
    print(f"\nResults saved in {results_dir}/")

if __name__ == "__main__":
    main()
