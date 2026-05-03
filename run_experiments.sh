#!/bin/bash

# Script to run performance experiments for Dijkstra's parallel implementation
# This script runs both sequential and parallel versions with various configurations
# and collects timing data for analysis

echo "=========================================="
echo "Dijkstra Performance Experiment Runner"
echo "=========================================="
echo ""

# Configuration
SIZES=(5000 8000 10000 12000)
PROCESSES=(1 2 4)
DENSITY=0.3
RUNS=3  # Number of runs to average

# MPI extra flags (empty for MPICH, use --oversubscribe for OpenMPI if needed)
MPI_EXTRA_FLAGS=""

# Output file
OUTPUT_FILE="results/timing_data_collected.csv"
echo "ProblemSize,Processes,Time,SequentialTime,Speedup,Efficiency" > $OUTPUT_FILE

# Compile programs
echo "Step 1: Compiling programs..."
echo "----------------------------"

cd sequential
gcc -o dijkstra sequential.c -lm -O2
if [ $? -ne 0 ]; then
    echo "Error: Sequential compilation failed"
    exit 1
fi
echo "✓ Sequential version compiled"
cd ..

cd parallel
mpicc -o dijkstra_parallel parallel.c -lm -O2
if [ $? -ne 0 ]; then
    echo "Error: Parallel compilation failed"
    exit 1
fi
echo "✓ Parallel version compiled"
cd ..

echo ""
echo "Step 2: Running sequential baseline (using parallel with P=1)..."
echo "----------------------------------------------------------------"

# Store sequential times (using parallel version with 1 process for consistency)
declare -A seq_times

for size in "${SIZES[@]}"; do
    echo -n "Testing N=$size... "
    
    total_time=0
    for run in $(seq 1 $RUNS); do
        # Run parallel version with 1 process as baseline
        output=$(mpirun $MPI_EXTRA_FLAGS -np 1 ./parallel/dijkstra_parallel -n $size -d $DENSITY 2>&1)
        time=$(echo "$output" | grep "Execution Time:" | awk '{print $3}')
        total_time=$(echo "$total_time + $time" | bc -l)
    done
    
    # Calculate average
    avg_time=$(echo "scale=6; $total_time / $RUNS" | bc -l)
    seq_times[$size]=$avg_time
    
    echo "Average time: ${avg_time}s"
done

echo ""
echo "Step 3: Running parallel experiments..."
echo "---------------------------------------"

for size in "${SIZES[@]}"; do
    seq_time=${seq_times[$size]}
    echo ""
    echo "Problem Size: N=$size (Sequential: ${seq_time}s)"
    
    for procs in "${PROCESSES[@]}"; do
        echo -n "  Testing P=$procs... "
        
        total_time=0
        for run in $(seq 1 $RUNS); do
            # Run and extract time
            output=$(mpirun $MPI_EXTRA_FLAGS -np $procs ./parallel/dijkstra_parallel -n $size -d $DENSITY 2>&1)
            time=$(echo "$output" | grep "Execution Time:" | awk '{print $3}')
            total_time=$(echo "$total_time + $time" | bc -l)
        done
        
        # Calculate average
        avg_time=$(echo "scale=6; $total_time / $RUNS" | bc -l)
        
        # Calculate speedup and efficiency
        speedup=$(echo "scale=2; $seq_time / $avg_time" | bc -l)
        efficiency=$(echo "scale=4; $speedup / $procs" | bc -l)
        
        # Write to CSV
        echo "$size,$procs,$avg_time,$seq_time,$speedup,$efficiency" >> $OUTPUT_FILE
        
        echo "Time: ${avg_time}s, Speedup: ${speedup}x, Efficiency: $(echo "$efficiency * 100" | bc -l | xargs printf "%.1f")%"
    done
done

echo ""
echo "=========================================="
echo "Experiments Complete!"
echo "=========================================="
echo ""
echo "Results saved to: $OUTPUT_FILE"
echo ""
echo "Next steps:"
echo "1. Review the timing data in $OUTPUT_FILE"
echo "2. Generate plots: cd results && python3 generate_plots.py"
echo "3. Analyze the performance in your report"
echo ""

# Display summary
echo "Performance Summary:"
echo "-------------------"
echo ""
echo "Best Speedup:"
best_speedup=$(tail -n +2 $OUTPUT_FILE | sort -t',' -k5 -rn | head -1)
echo "  $(echo $best_speedup | awk -F',' '{printf "N=%d, P=%d: %.2fx speedup (%.1f%% efficiency)\n", $1, $2, $5, $6*100}')"

echo ""
echo "Best Efficiency:"
best_efficiency=$(tail -n +2 $OUTPUT_FILE | sort -t',' -k6 -rn | head -1)
echo "  $(echo $best_efficiency | awk -F',' '{printf "N=%d, P=%d: %.1f%% efficiency (%.2fx speedup)\n", $1, $2, $6*100, $5}')"

echo ""
echo "Speedup by Process Count (N=1000):"
grep "^1000," $OUTPUT_FILE | awk -F',' '{printf "  %d processes: %.2fx speedup (%.1f%% efficiency)\n", $2, $5, $6*100}'

echo ""
