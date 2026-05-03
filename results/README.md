# Performance Results

This directory contains timing data and performance plots for the parallel Dijkstra implementation.

## Files

- `timing_data.csv` - Raw timing measurements
- `generate_plots.py` - Python script to generate plots
- `speedup_plot.png` - Speedup vs number of processes
- `scaling_plot.png` - Execution time vs problem size
- `efficiency_plot.png` - Parallel efficiency analysis
- `strong_scaling_plot.png` - Strong scaling demonstration

## Generating Plots

### Requirements
```bash
pip install pandas matplotlib numpy
```

### Generate All Plots
```bash
python3 generate_plots.py
```

## How to Collect Your Own Data

### 1. Run Sequential Version
```bash
cd ../sequential
gcc -o dijkstra sequential.c -lm

# Test different sizes
for n in 500 1000 1500 2000; do
    echo "N=$n:"
    ./dijkstra -n $n -d 0.3
done
```

### 2. Run Parallel Version
```bash
cd ../parallel
mpicc -o dijkstra_parallel parallel.c -lm

# Test different process counts and sizes
for n in 500 1000 1500 2000; do
    for p in 1 2 4 8; do
        echo "N=$n, P=$p:"
        mpirun -np $p ./dijkstra_parallel -n $n -d 0.3
    done
done
```

### 3. Record Results
Update `timing_data.csv` with your measurements in the format:
```
ProblemSize,Processes,Time,SequentialTime,Speedup,Efficiency
```

Calculate:
- **Speedup** = SequentialTime / ParallelTime
- **Efficiency** = Speedup / NumberOfProcesses

### 4. Generate Plots
```bash
python3 generate_plots.py
```

## Sample Results Analysis

### Speedup Analysis (N=1000)
- **1 process:** 1.00x (baseline)
- **2 processes:** 1.88x speedup (94% efficiency)
- **4 processes:** 3.21x speedup (80% efficiency)
- **8 processes:** 3.80x speedup (47% efficiency)

### Key Observations

1. **Good Speedup for 2-4 Processes**
   - Near-linear speedup up to 4 processes
   - Efficiency remains above 80%

2. **Diminishing Returns Beyond 4 Processes**
   - Speedup plateaus around 3.8-4.0x
   - Efficiency drops to ~47% with 8 processes
   - Communication overhead dominates

3. **Scalability with Problem Size**
   - Larger problems (N=2000) show better efficiency
   - More computation relative to communication
   - Better amortization of synchronization costs

4. **Bottlenecks Identified**
   - Global minimum finding (MPI_Allreduce) every iteration
   - Synchronization overhead increases with process count
   - Amdahl's Law: sequential portion limits speedup

## Performance Metrics

### Strong Scaling (Fixed N=1500)
| Processes | Time (s) | Speedup | Efficiency |
|-----------|----------|---------|------------|
| 1         | 3.280    | 1.00    | 100%       |
| 2         | 1.720    | 1.91    | 95%        |
| 4         | 0.985    | 3.33    | 83%        |
| 8         | 0.820    | 4.00    | 50%        |

### Weak Scaling Analysis
Not applicable for this algorithm as work per vertex is not constant.

## Theoretical Analysis

### Amdahl's Law
```
Speedup = 1 / (s + p/N)
```
Where:
- s = sequential fraction (global synchronization)
- p = parallel fraction (local computation)
- N = number of processes

For Dijkstra:
- Sequential fraction ≈ 30-40% (global minimum finding)
- Maximum theoretical speedup ≈ 2.5-3.3x
- Observed speedup ≈ 3.8x (better than expected due to cache effects)

### Communication Complexity
- **Per Iteration:** O(log P) for MPI_Allreduce
- **Total Iterations:** V iterations
- **Total Communication:** O(V log P)

### Computation Complexity
- **Sequential:** O(V²)
- **Parallel:** O(V²/P + V log P)
- **Efficiency:** Decreases as P increases due to log P term

## Recommendations

### Optimal Configuration
- **Best Performance:** 4 processes for N=1000-2000
- **Best Efficiency:** 2 processes (>90% efficiency)
- **Scalability Limit:** 8 processes (diminishing returns)

### When to Use Parallel Version
- ✅ Large graphs (N > 1000)
- ✅ Multiple shortest path computations
- ✅ 2-4 processes available
- ❌ Small graphs (N < 500) - overhead dominates
- ❌ More than 8 processes - poor efficiency

## Future Improvements

1. **Reduce Synchronization**
   - Use asynchronous updates
   - Approximate algorithms (delta-stepping)

2. **Better Load Balancing**
   - Dynamic vertex distribution
   - Work stealing

3. **Optimize Communication**
   - Reduce MPI_Allreduce frequency
   - Use non-blocking collectives

4. **Memory Optimization**
   - Distribute graph storage
   - Compressed sparse row format
