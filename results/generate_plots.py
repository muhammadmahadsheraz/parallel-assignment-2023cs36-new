#!/usr/bin/env python3
"""
Generate performance plots for Dijkstra's parallel implementation
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read timing data
df = pd.read_csv('timing_data_collected.csv')

# Set style
plt.style.use('seaborn-v0_8-darkgrid')
colors = ['#2E86AB', '#A23B72', '#F18F01', '#C73E1D']

# ============================================================================
# Plot 1: Speedup vs Number of Processes (for N=1000)
# ============================================================================
fig, ax = plt.subplots(figsize=(10, 6))

# Filter data for N=1000
df_1000 = df[df['ProblemSize'] == 1000]

processes = df_1000['Processes'].values
speedup = df_1000['Speedup'].values

# Plot actual speedup
ax.plot(processes, speedup, 'o-', linewidth=2, markersize=8, 
        label='Actual Speedup', color=colors[0])

# Plot ideal speedup
ideal_speedup = processes
ax.plot(processes, ideal_speedup, '--', linewidth=2, 
        label='Ideal Speedup', color=colors[2], alpha=0.7)

ax.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
ax.set_ylabel('Speedup', fontsize=12, fontweight='bold')
ax.set_title('Speedup vs Number of Processes (N=1000 vertices)', 
             fontsize=14, fontweight='bold')
ax.legend(fontsize=11)
ax.grid(True, alpha=0.3)
ax.set_xticks(processes)

# Add efficiency annotations
for i, (p, s) in enumerate(zip(processes, speedup)):
    efficiency = (s / p) * 100
    ax.annotate(f'{efficiency:.1f}%', 
                xy=(p, s), 
                xytext=(5, 5), 
                textcoords='offset points',
                fontsize=9,
                bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.3))

plt.tight_layout()
plt.savefig('speedup_plot.png', dpi=300, bbox_inches='tight')
print("Generated: speedup_plot.png")

# ============================================================================
# Plot 2: Execution Time vs Problem Size
# ============================================================================
fig, ax = plt.subplots(figsize=(10, 6))

# Get unique process counts
process_counts = sorted(df['Processes'].unique())

for i, proc in enumerate(process_counts):
    df_proc = df[df['Processes'] == proc]
    sizes = df_proc['ProblemSize'].values
    times = df_proc['Time'].values
    
    label = f'{proc} process' if proc == 1 else f'{proc} processes'
    ax.plot(sizes, times, 'o-', linewidth=2, markersize=8, 
            label=label, color=colors[i % len(colors)])

ax.set_xlabel('Problem Size (Number of Vertices)', fontsize=12, fontweight='bold')
ax.set_ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
ax.set_title('Execution Time vs Problem Size', fontsize=14, fontweight='bold')
ax.legend(fontsize=11)
ax.grid(True, alpha=0.3)
ax.set_yscale('log')

plt.tight_layout()
plt.savefig('scaling_plot.png', dpi=300, bbox_inches='tight')
print("Generated: scaling_plot.png")

# ============================================================================
# Plot 3: Efficiency vs Number of Processes
# ============================================================================
fig, ax = plt.subplots(figsize=(10, 6))

# Plot efficiency for different problem sizes
problem_sizes = sorted(df['ProblemSize'].unique())

for i, size in enumerate(problem_sizes):
    df_size = df[df['ProblemSize'] == size]
    processes = df_size['Processes'].values
    efficiency = df_size['Efficiency'].values * 100  # Convert to percentage
    
    ax.plot(processes, efficiency, 'o-', linewidth=2, markersize=8, 
            label=f'N={size}', color=colors[i % len(colors)])

ax.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
ax.set_ylabel('Efficiency (%)', fontsize=12, fontweight='bold')
ax.set_title('Parallel Efficiency vs Number of Processes', 
             fontsize=14, fontweight='bold')
ax.legend(fontsize=11)
ax.grid(True, alpha=0.3)
ax.set_xticks(processes)
ax.axhline(y=100, color='gray', linestyle='--', alpha=0.5, label='Ideal (100%)')
ax.set_ylim([0, 110])

plt.tight_layout()
plt.savefig('efficiency_plot.png', dpi=300, bbox_inches='tight')
print("Generated: efficiency_plot.png")

# ============================================================================
# Plot 4: Strong Scaling (Fixed Problem Size)
# ============================================================================
fig, ax = plt.subplots(figsize=(10, 6))

# Use N=1500 for strong scaling
df_strong = df[df['ProblemSize'] == 1500]
processes = df_strong['Processes'].values
times = df_strong['Time'].values

ax.bar(processes, times, color=colors[1], alpha=0.7, edgecolor='black')

# Add value labels on bars
for p, t in zip(processes, times):
    ax.text(p, t + 0.1, f'{t:.3f}s', ha='center', va='bottom', fontweight='bold')

ax.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
ax.set_ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
ax.set_title('Strong Scaling: Fixed Problem Size (N=1500)', 
             fontsize=14, fontweight='bold')
ax.set_xticks(processes)
ax.grid(True, alpha=0.3, axis='y')

plt.tight_layout()
plt.savefig('strong_scaling_plot.png', dpi=300, bbox_inches='tight')
print("Generated: strong_scaling_plot.png")

# ============================================================================
# Generate Summary Statistics
# ============================================================================
print("\n=== Performance Summary ===")
print(f"\nBest Speedup: {df['Speedup'].max():.2f}x")
best_speedup_row = df.loc[df['Speedup'].idxmax()]
print(f"  Achieved with {int(best_speedup_row['Processes'])} processes, N={int(best_speedup_row['ProblemSize'])}")

print(f"\nBest Efficiency: {df['Efficiency'].max()*100:.1f}%")
best_eff_row = df.loc[df['Efficiency'].idxmax()]
print(f"  Achieved with {int(best_eff_row['Processes'])} processes, N={int(best_eff_row['ProblemSize'])}")

print("\n=== Speedup by Process Count (N=1000) ===")
df_1000 = df[df['ProblemSize'] == 1000]
for _, row in df_1000.iterrows():
    print(f"  {int(row['Processes'])} processes: {row['Speedup']:.2f}x speedup, {row['Efficiency']*100:.1f}% efficiency")

print("\nAll plots generated successfully!")
