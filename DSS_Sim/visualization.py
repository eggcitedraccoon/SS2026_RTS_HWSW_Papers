import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import plotly.express as px
import plotly.io as pio
import os
from typing import List, Dict, Any

def plot_gantt_plotly(execution_trace: List[str], duration: int, title: str, filename: str):
    df_list = []
    if not execution_trace:
        return
        
    start_time = 0
    current_task = execution_trace[0]
    
    for t in range(1, len(execution_trace)):
        if execution_trace[t] != current_task:
            if current_task != "IDLE":
                df_list.append(dict(Task=current_task, Start=start_time, Finish=t))
            start_time = t
            current_task = execution_trace[t]
    
    # Last task
    if current_task != "IDLE":
        df_list.append(dict(Task=current_task, Start=start_time, Finish=len(execution_trace)))
        
    if not df_list:
        return
        
    df = pd.DataFrame(df_list)
    # Convert ticks to some pseudo-dates because plotly express timeline expects dates
    # or use bar chart
    fig = px.timeline(df, x_start="Start", x_end="Finish", y="Task", color="Task", title=title)
    fig.update_yaxes(autorange="reversed")
    fig.layout.xaxis.type = 'linear'
    # Set tick format
    for i in range(len(fig.data)):
        fig.data[i].x = fig.data[i].x
        
    fig.write_html(filename)

def plot_gantt(execution_trace: List[str], duration: int, title: str, filename: str):
    plt.figure(figsize=(15, 6))
    
    unique_tasks = sorted(list(set(execution_trace) - {"IDLE"}))
    colors = plt.cm.tab10(np.linspace(0, 1, len(unique_tasks)))
    color_map = {task: colors[i] for i, task in enumerate(unique_tasks)}
    color_map["IDLE"] = "white"
    
    for t in range(len(execution_trace)):
        task = execution_trace[t]
        if task != "IDLE":
            plt.barh(task, 1, left=t, color=color_map[task], edgecolor='black', alpha=0.8)
            
    plt.xlabel('Time (ms)')
    plt.ylabel('Task')
    plt.title(title)
    plt.grid(axis='x', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

def plot_budget(budget_trace: List[int], duration: int, title: str, filename: str):
    plt.figure(figsize=(15, 4))
    plt.step(range(len(budget_trace)), budget_trace, where='post', color='blue', linewidth=1.5)
    plt.fill_between(range(len(budget_trace)), budget_trace, step='post', alpha=0.2, color='blue')
    
    plt.xlabel('Time (ms)')
    plt.ylabel('Remaining Budget')
    plt.title(title)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

def plot_response_times(aperiodic_requests: List[Any], title: str, filename: str):
    completed = [r for r in aperiodic_requests if r.completion_time is not None]
    if not completed:
        return
        
    response_times = [r.response_time for r in completed]
    arrival_times = [r.arrival_time for r in completed]
    
    plt.figure(figsize=(10, 6))
    plt.scatter(arrival_times, response_times, alpha=0.6, color='red', edgecolor='black')
    plt.axhline(y=np.mean(response_times), color='blue', linestyle='--', label=f'Avg: {np.mean(response_times):.2f}ms')
    
    plt.xlabel('Arrival Time (ms)')
    plt.ylabel('Response Time (ms)')
    plt.title(title)
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

def plot_response_histogram(aperiodic_requests_a: List[Any], aperiodic_requests_b: List[Any], filename: str):
    resp_a = [r.response_time for r in aperiodic_requests_a if r.completion_time is not None]
    resp_b = [r.response_time for r in aperiodic_requests_b if r.completion_time is not None]
    
    plt.figure(figsize=(10, 6))
    plt.hist([resp_a, resp_b], label=['Mode A (No DSS)', 'Mode B (With DSS)'], bins=20, alpha=0.7)
    plt.xlabel('Response Time (ms)')
    plt.ylabel('Frequency')
    plt.title('Aperiodic Response Time Distribution Comparison')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()
