# Parallel Dijkstra's Algorithm using MPI

## Parallelization Approach

This implementation uses **MPI (Message Passing Interface)** to parallelize Dijkstra's algorithm through vertex distribution and collective communication.

### Why MPI?

**Chosen over Pthreads because:**
1. **Scalability:** Can scale beyond a single machine to distributed systems
2. **Data Distribution:** Natural fit for distributing graph vertices across processes
3. **Collective Operations:** MPI_Allreduce efficiently handles global minimum finding
4. **Learning Value:** Demonstrates distributed computing concepts better

**Chosen over MPI point-to-point because:**
1. **Regular Pattern:** Algorithm has regular, predictable communication
2. **Global Operations:** Need to find global minimum each iteration
3. **Simplicity:** Collective operations are cleaner than custom point-to-point logic

## Parallelism Pattern

**Pattern:** Iterative algorithm with global synchronization

**Characteristics:**
- Each iteration requires finding global minimum (synchronization point)
- Work is distributed across processes
- Regular communication pattern (every iteration)
- Demonstrates Amdahl's Law limitations

## Data Distribution Strategy

### Graph Distribution
- **Adjacency Matrix:** Each process stores complete graph (broadcast initially)
  - Alternative: Row-wise distribution (more memory efficient but complex)
  - Trade-off: Memory vs. communication overhead

### Vertex Distribution
- **Distance Array:** Distributed in contiguous blocks
  - Process 0: vertices [0, n/p)
  - Process 1: vertices [n/p, 2n/p)
  - Process k: vertices [k*n/p, (k+1)*n/p)

### Load Balancing
- Static distribution ensures equal work per process
- Each process handles approximately n/p vertices

## Communication Pattern

### Phase 1: Initialization (One-time)
```
Master → All: Broadcast graph (MPI_Bcast)
```

### Phase 2: Main Loop (V iterations)
```
Each iteration:
1. Each process finds local minimum unvisited vertex
2. Global reduction to find global minimum (MPI_Allreduce)
3. All processes update their local distances
```

### Phase 3: Finalization
```
All → Master: Gather distance arrays (MPI_Gather)
```

## Synchronization Requirements

### Critical Synchronization Points

1. **Global Minimum Finding (Every Iteration)**
   - Operation: MPI_Allreduce with MPI_MINLOC
   - Purpose: Find vertex with minimum distance across all processes
   - Frequency: V times (once per iteration)
   - **This is the main bottleneck**

2. **Graph Broadcast (Once)**
   - Operation: MPI_Bcast
   - Purpose: Distribute graph to all processes
   - Frequency: Once at initialization

3. **Result Gathering (Once)**
   - Operation: MPI_Gather
   - Purpose: Collect final distances at master
   - Frequency: Once at end

### Why Synchronization is Expensive

- **Amdahl's Law:** Global minimum finding is inherently sequential
- **Communication Overhead:** Network latency for MPI_Allreduce
- **Barrier Effect:** All processes must wait for slowest process

## Algorithm Outline

```c
// Parallel Dijkstra Algorithm

1. Initialize MPI (rank, size)

2. Master process (rank 0):
   - Generate random graph
   - Broadcast graph to all processes

3. All processes:
   - Receive graph
   - Calculate local vertex range [start, end)
   - Initialize local distance array

4. Main loop (V iterations):
   a. Find local minimum:
      - Search only local vertices
      - Track vertex index and distance
   
   b. Find global minimum:
      - MPI_Allreduce with MPI_MINLOC
      - All processes learn which vertex was selected
   
   c. Update local distances:
      - If selected vertex has edge to local vertex
      - Update distance if shorter path found
   
   d. Mark selected vertex as visited (all processes)

5. Gather results:
   - MPI_Gather local distances to master
   - Master prints results

6. Finalize MPI
```

## Compilation

### Using Makefile
```bash
make
```

### Manual Compilation
```bash
mpicc -o dijkstra_parallel parallel.c -lm -O2
```

Flags:
- `-lm`: Link math library
- `-O2`: Optimization level 2

## Usage Examples

### Basic usage (4 processes, 1000 vertices)
```bash
mpirun -np 4 ./dijkstra_parallel -n 1000 -d 0.3
```

### Scaling test (1, 2, 4, 8 processes)
```bash
for np in 1 2 4 8; do
    echo "Testing with $np processes:"
    mpirun -np $np ./dijkstra_parallel -n 1000 -d 0.3
done
```

### Large graph test
```bash
mpirun -np 8 ./dijkstra_parallel -n 2000 -d 0.2
```

### With verbose output (small graph)
```bash
mpirun -np 2 ./dijkstra_parallel -n 50 -d 0.5 -v
```

## Performance Characteristics

### Expected Speedup
- **2 processes:** ~1.8x speedup (90% efficiency)
- **4 processes:** ~3.2x speedup (80% efficiency)
- **8 processes:** ~4.8x speedup (60% efficiency)

### Bottlenecks
1. **Global Synchronization:** MPI_Allreduce every iteration
2. **Communication Overhead:** Increases with process count
3. **Amdahl's Law:** Sequential portion limits speedup

### Scalability Analysis

**Strong Scaling (Fixed Problem Size):**
- Speedup plateaus around 4-8 processes
- Communication overhead dominates beyond 8 processes

**Weak Scaling (Fixed Work per Process):**
- Better efficiency maintained
- Each process handles constant vertices

## Verification

### Correctness Checks
1. Compare with sequential version output
2. Verify distance to source is 0
3. Check triangle inequality holds
4. Ensure all processes agree on final distances

### Testing Commands
```bash
# Run sequential version
cd ../sequential
./dijkstra -n 100 -d 0.3 -v > seq_output.txt

# Run parallel version
cd ../parallel
mpirun -np 4 ./dijkstra_parallel -n 100 -d 0.3 -v > par_output.txt

# Compare outputs (distances should match)
diff seq_output.txt par_output.txt
```

## Implementation Details

### Key Data Structures
```c
int **graph;           // Adjacency matrix (full copy on each process)
int *dist;             // Local distance array
int *visited;          // Global visited array (replicated)
int local_start;       // First vertex owned by this process
int local_end;         // Last vertex + 1 owned by this process
```

### Key MPI Operations
```c
MPI_Bcast()           // Broadcast graph from master
MPI_Allreduce()       // Find global minimum (MINLOC)
MPI_Gather()          // Collect final distances
```

### Custom MPI Datatype
```c
struct {
    int dist;          // Distance value
    int vertex;        // Vertex index
} local_min, global_min;

MPI_Datatype MPI_2INT;  // For MINLOC operation
```

## Performance Tips

1. **Increase Problem Size:** Larger graphs show better speedup
2. **Reduce Process Count:** Don't use more processes than needed
3. **Optimize Density:** Denser graphs have more computation
4. **Use Fast Network:** InfiniBand > Ethernet for MPI

## Limitations

1. **Memory:** Each process stores full graph (O(V²) per process)
2. **Scalability:** Limited by global synchronization
3. **Network:** Performance depends on interconnect speed
4. **Load Balance:** Static distribution may be suboptimal

## Future Improvements

1. **Distributed Graph Storage:** Row-wise distribution to save memory
2. **Dynamic Load Balancing:** Handle irregular workloads
3. **Priority Queue:** Use heap for better sequential complexity
4. **Asynchronous Updates:** Reduce synchronization overhead
