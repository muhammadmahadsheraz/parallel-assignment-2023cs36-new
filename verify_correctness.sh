#!/bin/bash

# Script to verify correctness of parallel implementation
# Compares output of sequential and parallel versions

echo "=========================================="
echo "Dijkstra Correctness Verification"
echo "=========================================="
echo ""

# Test configurations
TEST_SIZES=(10 50 100)
TEST_PROCS=(1 2 4)

# Compile if needed
if [ ! -f "sequential/dijkstra" ]; then
    echo "Compiling sequential version..."
    cd sequential && gcc -o dijkstra sequential.c -lm && cd ..
fi

if [ ! -f "parallel/dijkstra_parallel" ]; then
    echo "Compiling parallel version..."
    cd parallel && mpicc -o dijkstra_parallel parallel.c -lm && cd ..
fi

echo "Running correctness tests..."
echo ""

PASSED=0
FAILED=0

for size in "${TEST_SIZES[@]}"; do
    echo "Testing N=$size vertices:"
    echo "------------------------"
    
    # Run sequential version
    echo -n "  Running sequential... "
    ./sequential/dijkstra -n $size -d 0.3 -v > /tmp/seq_output_$size.txt 2>&1
    echo "Done"
    
    for procs in "${TEST_PROCS[@]}"; do
        echo -n "  Running parallel (P=$procs)... "
        mpirun -np $procs ./parallel/dijkstra_parallel -n $size -d 0.3 -v > /tmp/par_output_${size}_${procs}.txt 2>&1
        echo "Done"
        
        # Extract distance arrays
        seq_distances=$(grep -A $size "Shortest Distances" /tmp/seq_output_$size.txt | tail -n +2 | awk '{print $2}' | sort -n)
        par_distances=$(grep -A $size "Shortest Distances" /tmp/par_output_${size}_${procs}.txt | tail -n +2 | awk '{print $2}' | sort -n)
        
        # Compare
        if [ "$seq_distances" == "$par_distances" ]; then
            echo "  ✓ PASS: P=$procs matches sequential output"
            ((PASSED++))
        else
            echo "  ✗ FAIL: P=$procs does NOT match sequential output"
            ((FAILED++))
            echo "    Sequential distances: $(echo $seq_distances | head -c 50)..."
            echo "    Parallel distances:   $(echo $par_distances | head -c 50)..."
        fi
    done
    echo ""
done

# Summary
echo "=========================================="
echo "Verification Summary"
echo "=========================================="
echo "Tests Passed: $PASSED"
echo "Tests Failed: $FAILED"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "✓ All tests passed! Parallel implementation is correct."
    exit 0
else
    echo "✗ Some tests failed. Please review the implementation."
    exit 1
fi
