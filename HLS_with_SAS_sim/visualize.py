import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import os
import glob
import numpy as np

# Create visuals directory if it doesn't exist
os.makedirs('visuals', exist_ok=True)

def plot_cost_distribution():
    summary_path = 'results/run_summary.csv'
    if not os.path.exists(summary_path):
        print(f"Skipping cost distribution: {summary_path} not found")
        return
    df = pd.read_csv(summary_path)
    fig = px.histogram(df, x='cost', nbins=20, title='Cost Distribution across Runs',
                       labels={'cost': 'Final Cost', 'count': 'Frequency'})
    
    avg_cost = df['cost'].mean()
    best_cost = df['cost'].min()
    
    fig.add_vline(x=avg_cost, line_dash="dash", line_color="red", annotation_text=f"Avg: {avg_cost:.2f}")
    fig.add_vline(x=best_cost, line_dash="dot", line_color="green", annotation_text=f"Best: {best_cost:.2f}")
    
    fig.write_image('visuals/cost_distribution.png')
    print("Generated visuals/cost_distribution.png")

def plot_combined_cost_vs_temp():
    files = glob.glob('results/run_*_temp_stats.csv')
    if not files:
        print("Skipping cost vs temp: No temp_stats files found in results/")
        return
    
    all_dfs = []
    for f in files:
        df = pd.read_csv(f)
        all_dfs.append(df)
    
    combined = pd.concat(all_dfs)

    # Each run contributes exactly one resulting cost per temperature level (the
    # state it carries forward into the next, cooler level). Aggregate that
    # single value across runs.
    stats = combined.groupby('temperature').agg(
        best_cost=('resulting_cost', 'min'),
        worst_cost=('resulting_cost', 'max'),
        mean_cost=('resulting_cost', 'mean')
    ).reset_index()

    # Sort by temperature descending for cooling schedule feel
    stats = stats.sort_values('temperature', ascending=False)

    fig = go.Figure()
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['best_cost'], name='Best', mode='lines+markers'))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['mean_cost'], name='Mean', mode='lines', line=dict(width=4)))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['worst_cost'], name='Worst', mode='lines+markers'))
    
    fig.update_layout(
        title='Cost vs Temperature (Resulting Solution per Run)',
        xaxis_title='Temperature',
        yaxis_title='Cost',
        xaxis_autorange='reversed'  # Showing cooling from right to left or left to right? usually T decreases.
    )
    
    fig.write_image('visuals/cost_vs_temp_combined.png')
    print("Generated visuals/cost_vs_temp_combined.png")

def plot_convergence():
    """
    Plots SA convergence behavior for all runs:
      - light: per-run current_cost (exploration noise, can go up & down)
      - bold:  per-run best_cost so far (must be monotonically non-increasing)
      - black: mean best_cost across all runs (the true convergence curve)
    """
    files = sorted(glob.glob('results/run_*_convergence.csv'))
    if not files:
        print("Skipping convergence: No convergence files found in results/")
        return

    fig = go.Figure()
    best_curves = []

    for f in files:
        df = pd.read_csv(f)
        if df.empty:
            continue
        run_name = os.path.basename(f).replace('_convergence.csv', '')

        # Exploration (current cost) — translucent
        fig.add_trace(go.Scatter(
            x=df['iteration'], y=df['current_cost'],
            name=f'{run_name} current', mode='lines',
            line=dict(width=1), opacity=0.25,
            legendgroup=run_name, showlegend=False
        ))

        # Best-so-far — bold per-run
        if 'best_cost' in df.columns:
            best_series = df['best_cost']
        else:
            # Fallback for older CSVs without the best_cost column.
            best_series = df['current_cost'].cummin()

        fig.add_trace(go.Scatter(
            x=df['iteration'], y=best_series,
            name=f'{run_name} best', mode='lines',
            line=dict(width=2),
            legendgroup=run_name
        ))

        best_curves.append(df[['iteration']].assign(best_cost=best_series.values))

    # Aggregate mean best-so-far across runs (align on iteration)
    if best_curves:
        merged = best_curves[0].rename(columns={'best_cost': 'b0'})
        for i, c in enumerate(best_curves[1:], start=1):
            merged = merged.merge(
                c.rename(columns={'best_cost': f'b{i}'}),
                on='iteration', how='outer'
            )
        merged = merged.sort_values('iteration').ffill()
        mean_best = merged.drop(columns='iteration').mean(axis=1)

        fig.add_trace(go.Scatter(
            x=merged['iteration'], y=mean_best,
            name='Mean best-so-far',
            mode='lines',
            line=dict(width=4, color='black')
        ))

    fig.update_layout(
        title='SA Convergence — current cost (light) vs best-so-far (bold)',
        xaxis_title='Iteration',
        yaxis_title='Cost'
    )
    fig.write_image('visuals/convergence.png')
    print("Generated visuals/convergence.png")

def plot_cost_landscape_3d(run_index=1, max_points=3000):
    """
    3D view of a single run's accepted-solution trajectory in
    (latency, area, cost) space, against the cost function's baseline plane
    cost = alpha * latency + beta * area. Consecutive accepted solutions are
    connected by line segments colored by log10(temperature) -- log scale
    because the cooling schedule is geometric, so a linear temperature color
    scale would show almost no variation across the (many) low-temperature
    iterations.
    """
    trace_path = f'results/run_{run_index}_accepted_trace.csv'
    if not os.path.exists(trace_path):
        print(f"Skipping 3D cost landscape: {trace_path} not found")
        return

    df = pd.read_csv(trace_path)
    if len(df) > max_points:
        df = df.iloc[np.linspace(0, len(df) - 1, max_points).astype(int)].reset_index(drop=True)

    # cost = alpha * (latency / latency_norm) + beta * (area / area_norm); the
    # norms are set at runtime from the ASAP baseline, so they must be read from
    # what the simulator exported rather than assumed.
    cost_fn_path = 'results/cost_function.csv'
    if not os.path.exists(cost_fn_path):
        print(f"Skipping 3D cost landscape: {cost_fn_path} not found "
              "(cost plane cannot be scaled correctly without it)")
        return
    cost_fn = pd.read_csv(cost_fn_path).iloc[0]
    alpha, beta = cost_fn['alpha'], cost_fn['beta']
    latency_norm, area_norm = cost_fn['latency_norm'], cost_fn['area_norm']

    fig = go.Figure()

    # Baseline cost-function plane, on the same normalized scale as the
    # trajectory's recorded cost.
    lat_range = np.linspace(0, max(df['latency'].max(), 1) * 1.05, 10)
    area_range = np.linspace(0, max(df['area'].max(), 1) * 1.05, 10)
    lat_grid, area_grid = np.meshgrid(lat_range, area_range)
    cost_grid = alpha * (lat_grid / latency_norm) + beta * (area_grid / area_norm)

    fig.add_trace(go.Surface(
        x=lat_grid, y=area_grid, z=cost_grid,
        showscale=False, opacity=0.55,
        colorscale=[[0, 'salmon'], [1, 'salmon']],
        name=f'Cost plane (alpha={alpha}, beta={beta})'
    ))

    # Plotly line traces don't support per-vertex colorscales, so each segment
    # between consecutive accepted solutions is its own single-color trace.
    log_temp = np.log10(df['temperature'].clip(lower=1e-12))
    tmin, tmax = log_temp.min(), log_temp.max()
    span = (tmax - tmin) or 1.0

    for i in range(len(df) - 1):
        t = (log_temp.iloc[i + 1] - tmin) / span
        color = px.colors.sample_colorscale('Viridis', t)[0]
        fig.add_trace(go.Scatter3d(
            x=df['latency'].iloc[i:i + 2],
            y=df['area'].iloc[i:i + 2],
            z=df['cost'].iloc[i:i + 2],
            mode='lines',
            line=dict(color=color, width=4),
            showlegend=False
        ))

    # Markers on top of the segments, colored the same way, with a colorbar.
    # showlegend is off here too: the color already carries the temperature
    # meaning via the colorbar, so a legend entry just duplicates that and
    # collides with the colorbar title in the same corner.
    fig.add_trace(go.Scatter3d(
        x=df['latency'], y=df['area'], z=df['cost'],
        mode='markers',
        marker=dict(size=3, color=log_temp, colorscale='Viridis',
                    colorbar=dict(title='log10(Temp)')),
        name=f'Run {run_index} accepted solutions',
        showlegend=False
    ))

    fig.update_layout(
        title=f'Cost Landscape (Run {run_index}): Accepted Solutions vs. Cost Plane',
        scene=dict(xaxis_title='Latency', yaxis_title='Area', zaxis_title='Cost')
    )

    # HTML only by default: static PNG export of Surface traces fails to render
    # the cost plane on this sandbox's headless kaleido/Chromium (verified --
    # only the trajectory shows up), so the interactive HTML is the reliable
    # output; screenshot it directly if a static image is needed.
    fig.write_html(f'visuals/cost_landscape_3d_run{run_index}.html')
    print(f"Generated visuals/cost_landscape_3d_run{run_index}.html")

def plot_gantt():
    gantt_path = 'results/best_gantt.csv'
    if not os.path.exists(gantt_path):
        print(f"Skipping Gantt: {gantt_path} not found")
        return
    
    df = pd.read_csv(gantt_path)
    df['duration'] = df['end'] - df['start']
    
    # Plotly timeline expects 'Start' and 'Finish' as dates, but we have cycles.
    # To use px.timeline with integer cycles, we need to convert them to dummy dates 
    # and then format the axis.
    
    # Or more simply, use a horizontal bar chart (broken bar chart)
    fig = px.bar(df, x="duration", y="resource", color="resource", base="start",
                 text="operation", orientation="h", title="Best Schedule Gantt Chart")
    
    fig.update_yaxes(autorange="reversed")
    fig.update_layout(xaxis_title="Cycle")
    
    fig.write_image('visuals/best_gantt.png')
    print("Generated visuals/best_gantt.png")

if __name__ == "__main__":
    plot_cost_distribution()
    plot_combined_cost_vs_temp()
    plot_convergence()
    plot_cost_landscape_3d()
    plot_gantt()
