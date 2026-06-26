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
    
    # Group by temperature and calculate required stats
    stats = combined.groupby('temperature').agg(
        best_cost=('min_cost', 'min'),
        worst_cost=('max_cost', 'max'),
        avg_cost=('avg_cost', 'mean'),
        avg_best=('min_cost', 'mean'),
        avg_worst=('max_cost', 'mean')
    ).reset_index()
    
    # Sort by temperature descending for cooling schedule feel
    stats = stats.sort_values('temperature', ascending=False)
    
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['best_cost'], name='Best', mode='lines+markers'))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['avg_best'], name='Avg Best', mode='lines', line=dict(dash='dash')))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['avg_cost'], name='Plain Average', mode='lines', line=dict(width=4)))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['avg_worst'], name='Avg Worst', mode='lines', line=dict(dash='dash')))
    fig.add_trace(go.Scatter(x=stats['temperature'], y=stats['worst_cost'], name='Worst', mode='lines+markers'))
    
    fig.update_layout(
        title='Cost vs Temperature (All Runs Combined)',
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
    plot_gantt()
