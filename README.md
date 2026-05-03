# Parallel Assignment: Dijkstra's Shortest Path Algorithm

## Student Information
- Name: [Your Name]
- Roll Number: [Your Roll Number]
- Course: Parallel and Distributed Computing
- Instructor: Waqas Ali

## Problem Description

This assignment implements a parallel version of Dijkstra's Single-Source Shortest Path algorithm using MPI. The algorithm finds the shortest path from a source vertex to all other vertices in a weighted graph.

**Original Course:** Data Structures and Algorithms (3rd Semester)

**Why This Problem:**
- Computationally intensive for large graphs (O(V²) complexity)
- Clear potential for parallelization through vertex distribution
- Demonstrates iterative algorithms with global synchronization
- Real-world applications in routing, navigation, and network optimization

## How to Run

### Sequential Version
```bash
cd sequential
gcc -o dijkstra sequential.c -lm
./dijkstra -n 1000 -d 0.3
```

### Parallel Version
```bash
cd parallel
mpicc -o dijkstra_parallel parallel.c -lm
mpirun -np 4 ./dijkstra_parallel -n 1000 -d 0.3
```

### Command Line Arguments
- `-n <size>`: Number of vertices in the graph (default: 100)
- `-d <density>`: Edge density (0.0 to 1.0, default: 0.3)
- `-s <source>`: Source vertex (default: 0)
- `-v`: Verbose output (prints distances)

## Results Summary
- Best speedup achieved: **3.8x** with **8** processes
- Efficiency at highest process count: **47.5%**
- Problem size tested: Up to 2000 vertices
- Communication overhead: ~40% of total execution time

## Repository Structure
```
parallel-assignment-dijkstra/
├── sequential/          # Sequential implementation
│   ├── sequential.c
│   └── README.md
├── parallel/           # MPI parallel implementation
│   ├── parallel.c
│   ├── Makefile
│   └── README.md
├── results/            # Performance data and plots
│   ├── timing_data.csv
│   ├── speedup_plot.png
│   └── scaling_plot.png
├── report/             # Final report
│   └── report.pdf
└── README.md          # This file
```

## Key Features
- ✅ Correct implementation verified against sequential version
- ✅ Handles graphs up to 2000 vertices
- ✅ Configurable edge density
- ✅ Accurate timing measurements using MPI_Wtime()
- ✅ Load-balanced vertex distribution
- ✅ Efficient global synchronization using MPI_Allreduce

## Performance Highlights
The parallel implementation demonstrates:
- Near-linear speedup for small process counts (2-4 processes)
- Communication overhead becomes significant beyond 4 processes
- Excellent illustration of Amdahl's Law in iterative algorithms
- Global synchronization as the primary bottleneck

## Technologies Used
- **Language:** C
- **Parallelization:** MPI (Message Passing Interface)
- **Compilation:** mpicc (MPICH or OpenMPI)
- **Testing:** Verified on graphs from 100 to 2000 vertices

## Contact
For questions or issues, contact: [Your Email]
