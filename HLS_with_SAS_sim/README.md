# HLS with SAS Simulator

This project is a High-Level Synthesis (HLS) simulator that uses Simulated Annealing Scheduling (SAS) to optimize the scheduling, resource allocation, and resource binding of a Data Flow Graph (DFG).

The simulator aims to minimize a cost function defined as:
`Cost = α × Latency + β × Area`

## Features

- **DFG Parsing**: Reads custom DFG descriptions.
- **Initial Scheduling**: Supports ASAP (As Soon As Possible) and List Scheduling as baselines.
- **Simulated Annealing Optimizer**: Optimizes latency and area through various move types:
    1. Move operation in time (forward/backward).
    2. Rebind operation to a different resource.
    3. Add a new resource.
    4. Remove an unused or redundant resource.
- **Multi-run Analysis**: Performs multiple independent SA runs to provide statistical insights (Best, Worst, Average, Median, Std Dev).
- **Visualization**: Generates Gantt-style CSVs and convergence logs, with an optional Python script for interactive plots.

## Requirements

- **C++ Compiler**: Supports C++26 (e.g., GCC 14+, Clang 18+).
- **CMake**: Version 3.25 or higher.
- **Python 3**: (Optional, for visualization) with `pandas` and `plotly` installed.
- **Doxygen**: (Optional, for generating documentation).

## Building the Project

### Using CLion
The project is configured to work with CLion. 
- Open the project folder in CLion.
- Select the **Release** profile.
- Build the `HLS_with_SAS_sim` target.
- The executable will be placed directly in the project root directory.

### Using Command Line
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
*Note: If built via command line with `-DCMAKE_BUILD_TYPE=Release`, the executable will also be placed in the project root.*

## Usage

Run the simulator from the project root:

```bash
./HLS_with_SAS_sim --dfg example.dfg --config config.json
```

### Arguments
- `--dfg <path>`: Path to the DFG file (default: `example.dfg`).
- `--config <path>`: Path to the JSON configuration file (default: `config.json`).

## Configuration

The `config.json` file controls the SA parameters and resource costs:

```json
{
  "sa": {
    "initial_temperature": 1000.0,
    "cooling_rate": 0.95,
    "min_temperature": 0.01,
    "iterations_per_temp": 1000,
    "runs": 30,
    "alpha": 1.0,
    "beta": 0.5,
    "seed": 42
  },
  "resources": {
    "LOAD": { "count": 1, "area": 1, "latency": 1 },
    "MUL": { "count": 1, "area": 3, "latency": 2 },
    ...
  }
}
```

## Results and Outputs

The simulator produces several output files in the project root:

1.  **Console Output**: Summary of ASAP/List scheduling, progress of SA runs, and final statistics.
2.  **`run_summary.csv`**: Data for each SA run (cost, latency, area, time).
3.  **`convergence.csv`**: Iteration-by-iteration log of the best run (temperature, current cost, best cost).
4.  **`gantt.csv`**: Resource-based schedule of the best solution, suitable for Gantt chart visualization.

### Visualization

To generate interactive HTML plots (Cost Distribution, Convergence, Temperature Curve), run:

```bash
python3 visualize.py
```

This will create `cost_distribution.html`, `convergence.html`, and `temperature.html`.

## Documentation

Comprehensive code documentation can be generated using Doxygen:

```bash
doxygen Doxyfile
```

The documentation will be available in `docs/html/index.html`.
