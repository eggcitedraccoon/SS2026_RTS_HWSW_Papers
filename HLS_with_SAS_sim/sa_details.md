# Simulated Annealing Details: Moves and Acceptance

This document explains the two primary random components of the Simulated Annealing (SA) scheduler in this project: the neighbor generation (moves) and the Metropolis acceptance criterion.

## 1. Random Neighbor Generation (Moves)

The `NeighborGenerator` class is responsible for perturbing the current schedule to find a "neighbor" solution. It randomly selects one of five types of moves:

1.  **Move in Time (Forward)**: Randomly selects an operation and increments its start cycle by 1.
2.  **Move in Time (Backward)**: Randomly selects an operation and decrements its start cycle by 1 (clamped to 0).
3.  **Rebind Resource**: Randomly selects an operation and changes its binding to another available resource instance of the same compatible type.
4.  **Add Resource**: Randomly selects an operation type (e.g., ADD, MUL) and adds a new resource instance of that type to the available pool. This allows the scheduler to explore solutions with more parallelism.
5.  **Remove Resource**: Randomly selects an existing resource instance and removes it from the pool, provided it's not the last resource of its type. Operations bound to the removed resource are automatically rebound to another instance of the same type.

### Repair Logic
After every move, a `repair` function is called to ensure that the Data Flow Graph (DFG) constraints are still satisfied:
- **Dependency Repair**: If an operation's start time changes, all its successors are recursively shifted forward to maintain causality. If an operation is moved backward into its predecessors, it is pushed forward again.
- **Resource Overlap Repair**: If multiple operations are bound to the same resource instance at overlapping times, they are shifted forward in time to resolve the conflict.

## 2. Worse Schedule Acceptance (Metropolis Criterion)

In the `SimulatedAnnealingScheduler`, we decide whether to move from the `currentState` to the `nextState` (neighbor) based on the change in cost ($\Delta = \text{cost}_{next} - \text{cost}_{current}$):

- If $\Delta < 0$ (the new schedule is better), we **always** accept it.
- If $\Delta \ge 0$ (the new schedule is worse or equal), we accept it with a certain probability $P$.

The probability $P$ is calculated using the Metropolis criterion:
$$P = e^{-\frac{\Delta}{T}}$$
where $T$ is the current **Temperature**.

### How Random Acceptance Works
To decide if we accept a worse schedule, we:
1.  Calculate $P = e^{-\frac{\Delta}{T}}$.
2.  Generate a random number $r$ from a uniform distribution between 0 and 1.
3.  If $r < P$, we accept the worse schedule; otherwise, we stay with the current one.

### Behavior over time
- **High Temperature**: $P$ is close to 1, meaning almost all moves are accepted. This allows the algorithm to explore the search space and escape local minima.
- **Low Temperature**: $P$ becomes very small for large $\Delta$. The algorithm becomes "greedier," accepting only small increases in cost or mostly improvements, eventually settling into a (hopefully global) minimum.
