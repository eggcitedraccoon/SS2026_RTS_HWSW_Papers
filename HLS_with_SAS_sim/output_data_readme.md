# HLS SA Simulation Data & Visualizations

This document explains the data exported by the High-Level Synthesis (HLS) Simulated Annealing (SA) scheduler and how to interpret the generated visualizations.

## Data Exports (`results/` directory)

All raw data from the simulation is stored in the `results/` folder in CSV format.

### 1. `run_summary.csv`
Contains the summary of all independent runs performed during the simulation.
- **run_index**: Unique ID for the run.
- **cost**: Final cost achieved.
- **latency**: Total latency (cycles) of the schedule.
- **area**: Total area (resource cost) of the schedule.
- **executionTimeMs**: Real-time duration of the run in milliseconds.

### 2. `run_X_temp_stats.csv`
One row per temperature level in a specific run (where `X` is the run index).
- **temperature**: The temperature value at that level.
- **resulting_cost**: The cost of the solution this run actually carries forward into the next, cooler
  temperature level (i.e. the current state once all iterations at this level are done). This is a
  single value per level, not an average or extremum over the level's iterations.

### 3. `run_X_convergence.csv`
High-resolution log of the cost during the optimization process.
- **iteration**: The cumulative iteration number.
- **temp**: The temperature at that iteration.
- **current_cost**: The instantaneous cost of the schedule at that iteration (sampled every 100 iterations).

### 4. `best_gantt.csv`
The schedule details for the best run found across all simulations.
- **resource**: The specific resource instance name (e.g., `Res_1_ADD`).
- **start**: Start cycle of the operation (inclusive).
- **end**: End cycle of the operation (exclusive).
- **operation**: The name of the operation (from the DFG).

---

## Visualizations (`visuals/` directory)

The `visualize.py` script generates PNG images based on the exported data.

### 1. `cost_distribution.png`
A histogram showing the distribution of the final costs across all runs.
- Helps identify if the SA is consistently finding good solutions or if there is high variance.
- Includes vertical lines for the **Average** and **Best** costs.

### 2. `cost_vs_temp_combined.png`
An aggregated view of how the cost evolves as the temperature decreases (cooling). At each
temperature level, every run contributes exactly one value -- its `resulting_cost` (see above).
- **Best**: The minimum resulting cost across all runs, at each temperature.
- **Worst**: The maximum resulting cost across all runs, at each temperature.
- **Mean**: The average resulting cost across all runs, at each temperature.
- **Interpretation**: A converging set of lines indicates that runs are settling into similar (and improving) solutions as the temperature drops.

### 3. `best_gantt.png`
A horizontal bar chart representing the best schedule.
- **Y-axis**: Resource instances used.
- **X-axis**: Time cycles.
- **Labels**: Operation names assigned to each resource at each time slot.

---

## How to Generate Visuals

To generate or update the visualizations, ensure you have the required Python dependencies installed (see `requirements.txt`) and run:

```bash
python3 visualize.py
```

*Note: The script expects the `results/` directory to contain the CSV files mentioned above.*
