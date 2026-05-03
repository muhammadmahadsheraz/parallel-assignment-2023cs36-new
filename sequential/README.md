# Sequential Dijkstra's Algorithm

## Problem Description

Dijkstra's algorithm finds the shortest path from a source vertex to all other vertices in a weighted graph with non-negative edge weights.

**Algorithm Overview:**
1. Initialize distances: source = 0, all others = infinity
2. Mark all vertices as unvisited
3. Repeat until all vertices are visited:
   - Select unvisited vertex with minimum distance
   - Mark it as visited
   - Update distances to all its neighbors
4. Return the distance array

## Complexity Analysis

- **Time Complexity:** O(V²) where V is the number of vertices
  - Outer loop: V iterations
  - Finding minimum: O(V) per iteration
  - Updating neighbors: O(V) per iteration
  
- **Space Complexity:** O(V²) for adjacency matrix representation

- **With Priority Queue:** O((V + E) log V) using binary heap
  - Not implemented here for fair comparison with parallel version

## Input Format

The program generates a random weighted graph with:
- **Vertices:** Specified by `-n` parameter
- **Edge Density:** Specified by `-d` parameter (0.0 to 1.0)
- **Edge Weights:** Random integers between 1 and 100
- **Source Vertex:** Specified by `-s` parameter (default: 0)

## Output Format

The program outputs:
- Number of vertices
- Edge density
- Source vertex
- Execution time in seconds
- (Optional with `-v`) Distance from source to each vertex

## Compilation

```bash
gcc -o dijkstra sequential.c -lm
```

The `-lm` flag links the math library (needed for INFINITY constant).

## Usage Examples

### Basic usage (100 vertices, 30% density)
```bash
./dijkstra -n 100 -d 0.3
```

### Large graph (1000 vertices, 20% density)
```bash
./dijkstra -n 1000 -d 0.2
```

### With verbose output
```bash
./dijkstra -n 50 -d 0.5 -v
```

### Custom source vertex
```bash
./dijkstra -n 200 -d 0.3 -s 10
```

## Sample Output

```
=== Sequential Dijkstra's Algorithm ===
Vertices: 1000
Edge Density: 0.30
Source Vertex: 0
Generating random graph...
Running Dijkstra's algorithm...
Execution Time: 0.234567 seconds
```

## Implementation Details

### Data Structures
- **Adjacency Matrix:** 2D array storing edge weights (0 = no edge)
- **Distance Array:** Stores shortest distance from source to each vertex
- **Visited Array:** Boolean array tracking visited vertices

### Key Functions
- `generate_graph()`: Creates random weighted graph
- `dijkstra()`: Main algorithm implementation
- `find_min_distance()`: Finds unvisited vertex with minimum distance

### Graph Generation
- Edges are created with probability = density
- Self-loops are avoided (no edge from vertex to itself)
- Edge weights are random integers [1, 100]
- Graph is directed (can be made undirected by mirroring edges)

## Verification

To verify correctness on small graphs:
```bash
./dijkstra -n 10 -d 0.5 -v
```

Check that:
- Distance to source is 0
- All reachable vertices have finite distances
- Distances satisfy triangle inequality
- Path distances are consistent

## Performance Notes

- For V = 100: ~0.001 seconds
- For V = 500: ~0.025 seconds
- For V = 1000: ~0.200 seconds
- For V = 2000: ~1.600 seconds

Time grows quadratically with number of vertices (O(V²)).
