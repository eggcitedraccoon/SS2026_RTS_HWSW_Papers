import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import os

def plot_cost_distribution():
    if not os.path.exists('run_summary.csv'):
        return
    df = pd.read_csv('run_summary.csv')
    fig = px.histogram(df, x='cost', nbins=20, title='Cost Distribution across Runs',
                       labels={'cost': 'Final Cost', 'count': 'Frequency'})
    
    avg_cost = df['cost'].mean()
    best_cost = df['cost'].min()
    
    fig.add_vline(x=avg_cost, line_dash="dash", line_color="red", annotation_text=f"Avg: {avg_cost:.2f}")
    fig.add_vline(x=best_cost, line_dash="dot", line_color="green", annotation_text=f"Best: {best_cost:.2f}")
    
    fig.write_html('cost_distribution.html')
    print("Generated cost_distribution.html")

def plot_convergence():
    if not os.path.exists('convergence.csv'):
        return
    df = pd.read_csv('convergence.csv')
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=df['iteration'], y=df['best_cost'], mode='lines', name='Best Cost'))
    fig.update_layout(title='Optimization Convergence', xaxis_title='Iteration', yaxis_title='Cost')
    fig.write_html('convergence.html')
    print("Generated convergence.html")

def plot_temperature():
    if not os.path.exists('convergence.csv'):
        return
    df = pd.read_csv('convergence.csv')
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=df['iteration'], y=df['temp'], mode='lines', name='Temperature', line=dict(color='orange')))
    fig.update_layout(title='Cooling Schedule', xaxis_title='Iteration', yaxis_title='Temperature')
    fig.write_html('temperature.html')
    print("Generated temperature.html")

if __name__ == "__main__":
    plot_cost_distribution()
    plot_convergence()
    plot_temperature()
