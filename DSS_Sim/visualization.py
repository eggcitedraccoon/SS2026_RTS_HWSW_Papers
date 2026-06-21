import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
from typing import List, Dict, Any

def plot_gantt(execution_trace: List[str], duration: int, title: str, filename: str, released_jobs: List[Any] = None):
    plt.figure(figsize=(15, 6))

    # Process trace to separate row name from task instance name
    processed_trace = []
    for entry in execution_trace:
        if entry == "IDLE":
            processed_trace.append(("IDLE", "IDLE"))
        elif ":" in entry:
            row, instance = entry.split(":")
            processed_trace.append((row, instance))
        else:
            processed_trace.append((entry, entry))

    present_rows = set(r for r, _ in processed_trace if r != "IDLE")
    unique_instances = sorted(list(set(i for r, i in processed_trace if r != "IDLE")))

    # Row order (bottom -> top): periodic tasks ascending (P1, P2, P3), then DSS/APERIODIC on top
    periodic_rows = sorted(r for r in present_rows if r not in ("DSS", "APERIODIC"))
    server_rows = [r for r in ("DSS", "APERIODIC") if r in present_rows]
    ordered_rows = periodic_rows + server_rows
    row_to_y = {row: i for i, row in enumerate(ordered_rows)}

    # Color map for instances
    colors = plt.cm.tab20(np.linspace(0, 1, len(unique_instances)))
    color_map = {inst: colors[i] for i, inst in enumerate(unique_instances)}
    color_map["IDLE"] = "white"

    for t, (row, inst) in enumerate(processed_trace):
        if row != "IDLE":
            hatch = None
            if row in ["DSS", "APERIODIC"]:
                hatches = ['', '//', '\\\\', 'xx', '++', 'oo', '..', '**', '||', '--']
                aperiodic_instances = [i for i in unique_instances if i.startswith('A')]
                if inst in aperiodic_instances:
                    hatch = hatches[aperiodic_instances.index(inst) % len(hatches)]

            plt.barh(row_to_y[row], 1, left=t, color=color_map[inst], edgecolor='black', alpha=0.8, hatch=hatch)

    # Plot deadlines
    if released_jobs:
        for job in released_jobs:
            if job.task_name in row_to_y:
                y = row_to_y[job.task_name]
                plt.vlines(job.absolute_deadline,
                           y - 0.4,
                           y + 0.4,
                           colors='red', linestyles='solid', linewidth=2, label='Deadline' if 'Deadline' not in plt.gca().get_legend_handles_labels()[1] else "")

    plt.yticks(list(row_to_y.values()), list(row_to_y.keys()))
    plt.xlabel('Time (ms)')
    plt.ylabel('Task')
    plt.title(title)
    plt.grid(axis='x', linestyle='--', alpha=0.7)

    # Add legend if we have deadlines
    handles, labels = plt.gca().get_legend_handles_labels()
    if labels:
        plt.legend(handles, labels, loc='upper right')

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

def _gaussian_kde(data: np.ndarray, x: np.ndarray) -> np.ndarray:
    n = len(data)
    std = data.std(ddof=1) if n > 1 else 0.0
    # Silverman's rule of thumb; fall back to a small bandwidth for degenerate data
    bw = 1.06 * std * n ** (-1 / 5) if std > 0 else 0.5
    diff = (x[:, None] - data[None, :]) / bw
    return np.exp(-0.5 * diff ** 2).sum(axis=1) / (n * bw * np.sqrt(2 * np.pi))


def plot_response_histogram(aperiodic_requests_a: List[Any], aperiodic_requests_b: List[Any], filename: str):
    resp_a = np.array([r.response_time for r in aperiodic_requests_a if r.completion_time is not None], dtype=float)
    resp_b = np.array([r.response_time for r in aperiodic_requests_b if r.completion_time is not None], dtype=float)

    if len(resp_a) == 0 and len(resp_b) == 0:
        return

    all_resp = np.concatenate([resp_a, resp_b])
    x_min = max(0.0, float(all_resp.min()) - 2.0)
    x_max = float(all_resp.max()) + 2.0
    x = np.linspace(x_min, x_max, 500)

    plt.figure(figsize=(10, 6))
    for data, label, color in [
        (resp_a, 'Mode A (No DSS)', 'tab:red'),
        (resp_b, 'Mode B (With DSS)', 'tab:blue'),
    ]:
        if len(data) == 0:
            continue
        # Underlying empirical histogram (normalised so area = 1)
        plt.hist(data, bins='auto', density=True, alpha=0.2, color=color)
        if len(data) >= 2:
            y = _gaussian_kde(data, x)
            plt.plot(x, y, color=color, linewidth=2, label=f'{label} (μ={data.mean():.2f} ms)')
            plt.fill_between(x, y, alpha=0.25, color=color)
        else:
            plt.axvline(data[0], color=color, linewidth=2, label=f'{label} (n=1, x={data[0]:.0f} ms)')
        plt.axvline(data.mean(), color=color, linestyle='--', alpha=0.8)

    plt.xlabel('Response Time (ms)')
    plt.ylabel('Probability Density')
    plt.title('Aperiodic Response Time Distribution Comparison')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()
