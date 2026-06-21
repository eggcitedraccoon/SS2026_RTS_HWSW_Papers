# EDF Simulator with Dynamic Sporadic Server (DSS)

This application simulates an Earliest Deadline First (EDF) scheduler with a Dynamic Sporadic Server (DSS) for handling aperiodic tasks. It is designed for educational and research purposes to demonstrate how a DSS protects periodic real-time tasks while providing service to aperiodic requests.

## Features

- **EDF Scheduling**: Implements core EDF logic for periodic jobs.
- **Dynamic Sporadic Server**: Handles aperiodic requests with budget management, deadline assignment, and replenishment.
- **Comparison Mode**: Compare EDF with DSS against EDF with low-priority aperiodic tasks.
- **Visualizations**: Generates publication-quality Gantt charts, budget traces, and response time plots.
- **Metrics**: Collects detailed statistics on task completion, deadline misses, and server performance.

## Project Structure

- `main.py`: Orchestrates the simulation and comparison.
- `tasks.py`: Data structures for Periodic Tasks, Jobs, and Aperiodic Requests.
- `dss.py`: Implementation of the Dynamic Sporadic Server.
- `scheduler.py`: EDF Scheduler logic.
- `metrics.py`: Metrics collection and reporting.
- `visualization.py`: Plotting utilities.
- `config.py`: Simulation parameters.
- `results/`: Output directory for plots and statistics.

## Usage

Ensure you have the required libraries installed:
```bash
pip install pandas numpy matplotlib plotly
```

Run the simulation:
```bash
python main.py
```

Results will be saved in the `results/` directory.
