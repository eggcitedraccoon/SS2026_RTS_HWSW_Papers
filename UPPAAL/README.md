# Dynamic Sporadic Server (DSS) UPPAAL Model

This folder contains the UPPAAL model for a Dynamic Sporadic Server (DSS) implemented for a Real-Time System scheduled with Earliest Deadline First (EDF).

## Files
- `DSS.xml`: The primary, fully functional model. Includes periodic tasks, DSS, queued replenishment logic, and EDF scheduling.
- `README.md`: This documentation.

## System Overview
The model simulates a system with three tasks:
1. **Periodic Tasks ($P_0, P_1$):** Standard hard real-time tasks.
2. **Dynamic Sporadic Server ($D_2$):** A server task that handles aperiodic requests. It maintains a budget that is consumed when executing. Replenishments are queued: each consumption "burst" (from activation to idleness or budget exhaustion) is restored exactly one period later.
3. **EDF Scheduler:** A centralized scheduler that assigns the CPU to the ready task with the earliest relative deadline.

## Core Logic & Recent Fixes

### Global Declarations
Open the **Global Declarations** section in UPPAAL to find:
- **Scheduling Logic:** `select_task()` picks the task with the minimum relative deadline (`rel_d[i]`) among those that are ready.
- **Dynamic State:** The model uses relative deadlines and a periodic counter (`p[id]`) to track task arrivals.
- **Sporadic Server Logic:** `update_dss()` manages a replenishment queue. It tracks consumption bursts and schedules budget recovery using a circular buffer (`repl_amt`, `repl_t`).

### Key Implementation Fixes
The model includes specific logic to ensure correct DSS behavior:
- **Bandwidth Preservation:** If a full period $T_s$ elapses while the server is still active (e.g., it was preempted but not exhausted), the consumed budget is settled immediately and a fresh replenishment window is opened. This prevents the server from replenishing the entire burst only at the end, which could violate bandwidth constraints.
- **Deadline Integrity:** The relative deadline `rel_d[id]` for the DSS is set only upon genuine request arrival. It is *not* reset when a new burst starts within the server, ensuring that the EDF urgency correctly reflects the aperiodic request's arrival time rather than the replenishment bookkeeping.

### Templates
- **`CPU`**: Tracks whether the processor is `Idle` or `Busy` based on `cpu_task`.
- **`PeriodicTask`**: Implements arrival logic, remaining computation time (`rem_c`), and transitions between `Idle`, `Ready`, and `Active`.
- **`DSS`**: Handles sporadic aperiodic requests, budget consumption, and state transitions.
- **`Ticker`**: The system heartbeat. It triggers discrete `tick` events every 1.0 time unit, calls the scheduler, and synchronizes system-wide state changes via `reschedule`.
- **`Source`**: Generates aperiodic requests for the DSS.

## Verification (Queries)
The model includes queries to verify:
- **Deadlock Absence**: The system never enters a state with no enabled transitions.
- **Periodic Deadline Satisfaction**: Tasks $P_0$ and $P_1$ always meet their deadlines.
- **Aperiodic Deadline Satisfaction**: The aperiodic requests serviced by the DSS also meet their deadlines (`rel_d[DSS_ID] >= -1`). This ensures that the server provides sufficient responsiveness.
- **Functional Correctness**: The DSS can be activated, the budget remains within `[0, MAX_BUDGET]`, and CPU assignment is consistent with task states.

## How to Run
1. Open `DSS.xml` in **UPPAAL** (tested on version 5.0+ / Flat System 1.6).
2. Use the **Simulator** tab to step through the execution and observe the Gantt-chart-like behavior of `cpu_task`.
3. Use the **Verifier** tab to check the safety and liveness properties.
