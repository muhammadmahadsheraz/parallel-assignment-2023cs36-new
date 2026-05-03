#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define INF 999999
#define MAX_WEIGHT 100

// Structure for finding minimum with location
typedef struct {
    int dist;
    int vertex;
} DistVertex;

// Function prototypes
void generate_graph(int **graph, int n, double density, int rank);
void parallel_dijkstra(int **graph, int *local_dist, int *visited, 
                       int n, int source, int rank, int size);
void print_distances(int *dist, int n, int source);
int* gather_distances(int *local_dist, int local_size, int n, int rank, int size);

int main(int argc, char *argv[]) {
    int rank, size;
    int n = 100;              // Default number of vertices
    double density = 0.3;     // Default edge density
    int source = 0;           // Default source vertex
    int verbose = 0;          // Verbose output flag
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Parse command line arguments (all processes)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            density = atof(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            source = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
    }
    
    // Validate inputs
    if (n <= 0 || n > 10000) {
        if (rank == 0) {
            fprintf(stderr, "Error: Number of vertices must be between 1 and 10000\n");
        }
        MPI_Finalize();
        return 1;
    }
    if (density < 0.0 || density > 1.0) {
        if (rank == 0) {
            fprintf(stderr, "Error: Density must be between 0.0 and 1.0\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Print header (master only)
    if (rank == 0) {
        printf("=== Parallel Dijkstra's Algorithm (MPI) ===\n");
        printf("Vertices: %d\n", n);
        printf("Edge Density: %.2f\n", density);
        printf("Source Vertex: %d\n", source);
        printf("Number of Processes: %d\n", size);
    }
    
    // Allocate memory for graph (full graph on each process)
    int **graph = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph[i] = (int *)malloc(n * sizeof(int));
    }
    
    // Generate graph on master and broadcast
    if (rank == 0) {
        printf("Generating random graph...\n");
        generate_graph(graph, n, density, rank);
    }
    
    // Broadcast graph to all processes
    for (int i = 0; i < n; i++) {
        MPI_Bcast(graph[i], n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    // Calculate local vertex range for this process
    int local_size = n / size;
    int remainder = n % size;
    int local_start = rank * local_size + (rank < remainder ? rank : remainder);
    if (rank < remainder) local_size++;
    int local_end = local_start + local_size;
    
    // Allocate local distance array
    int *local_dist = (int *)malloc(local_size * sizeof(int));
    
    // Allocate global visited array (replicated on all processes)
    int *visited = (int *)calloc(n, sizeof(int));
    
    // Synchronize before timing
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Run parallel Dijkstra with timing
    double start_time = MPI_Wtime();
    parallel_dijkstra(graph, local_dist, visited, n, source, rank, size);
    double end_time = MPI_Wtime();
    
    // Gather all distances to master
    int *global_dist = gather_distances(local_dist, local_size, n, rank, size);
    
    // Master process prints results
    if (rank == 0) {
        double elapsed_time = end_time - start_time;
        printf("Execution Time: %.6f seconds\n", elapsed_time);
        
        // Print distances if verbose
        if (verbose) {
            print_distances(global_dist, n, source);
        }
        
        // Calculate statistics
        int reachable = 0;
        long long total_dist = 0;
        for (int i = 0; i < n; i++) {
            if (global_dist[i] != INF) {
                reachable++;
                total_dist += global_dist[i];
            }
        }
        printf("Reachable vertices: %d/%d\n", reachable, n);
        if (reachable > 1) {
            printf("Average distance: %.2f\n", (double)total_dist / (reachable - 1));
        }
        
        free(global_dist);
    }
    
    // Free memory
    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);
    free(local_dist);
    free(visited);
    
    MPI_Finalize();
    return 0;
}

// Generate a random weighted graph (master process only)
void generate_graph(int **graph, int n, double density, int rank) {
    srand(42);  // Fixed seed for reproducibility
    
    // Initialize all edges to 0 (no edge)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = 0;
        }
    }
    
    // Create edges based on density
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {  // No self-loops
                double rand_val = (double)rand() / RAND_MAX;
                if (rand_val < density) {
                    graph[i][j] = (rand() % MAX_WEIGHT) + 1;
                }
            }
        }
    }
}

// Parallel Dijkstra's algorithm
void parallel_dijkstra(int **graph, int *local_dist, int *visited, 
                       int n, int source, int rank, int size) {
    // Calculate local vertex range
    int local_size = n / size;
    int remainder = n % size;
    int local_start = rank * local_size + (rank < remainder ? rank : remainder);
    if (rank < remainder) local_size++;
    int local_end = local_start + local_size;
    
    // Initialize local distances
    for (int i = 0; i < local_size; i++) {
        int global_vertex = local_start + i;
        if (global_vertex == source) {
            local_dist[i] = 0;
        } else {
            local_dist[i] = INF;
        }
    }
    
    // Main loop: process all vertices
    for (int count = 0; count < n; count++) {
        // Find local minimum unvisited vertex
        DistVertex local_min;
        local_min.dist = INF;
        local_min.vertex = -1;
        
        for (int i = 0; i < local_size; i++) {
            int global_vertex = local_start + i;
            if (!visited[global_vertex] && local_dist[i] < local_min.dist) {
                local_min.dist = local_dist[i];
                local_min.vertex = global_vertex;
            }
        }
        
        // Find global minimum using MPI_Allreduce with MINLOC
        // Pack into array for MPI_2INT compatibility
        int local_min_array[2] = {local_min.dist, local_min.vertex};
        int global_min_array[2];
        MPI_Allreduce(local_min_array, global_min_array, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
        
        DistVertex global_min;
        global_min.dist = global_min_array[0];
        global_min.vertex = global_min_array[1];
        
        // If no reachable vertex found, break
        if (global_min.vertex == -1 || global_min.dist == INF) {
            break;
        }
        
        int u = global_min.vertex;
        
        // Mark vertex as visited (all processes)
        visited[u] = 1;
        
        // Update local distances based on selected vertex
        for (int i = 0; i < local_size; i++) {
            int v = local_start + i;
            if (!visited[v] && graph[u][v] != 0) {
                int new_dist = global_min.dist + graph[u][v];
                if (new_dist < local_dist[i]) {
                    local_dist[i] = new_dist;
                }
            }
        }
    }
}

// Gather local distances to master process
int* gather_distances(int *local_dist, int local_size, int n, int rank, int size) {
    int *global_dist = NULL;
    
    // Calculate send counts and displacements for MPI_Gatherv
    int *sendcounts = NULL;
    int *displs = NULL;
    
    if (rank == 0) {
        global_dist = (int *)malloc(n * sizeof(int));
        sendcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
        
        int base_size = n / size;
        int remainder = n % size;
        int offset = 0;
        
        for (int i = 0; i < size; i++) {
            sendcounts[i] = base_size + (i < remainder ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }
    
    // Gather all local distances to master
    MPI_Gatherv(local_dist, local_size, MPI_INT,
                global_dist, sendcounts, displs, MPI_INT,
                0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        free(sendcounts);
        free(displs);
    }
    
    return global_dist;
}

// Print distances from source to all vertices
void print_distances(int *dist, int n, int source) {
    printf("\n=== Shortest Distances from Vertex %d ===\n", source);
    printf("Vertex\tDistance\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t", i);
        if (dist[i] == INF) {
            printf("INF\n");
        } else {
            printf("%d\n", dist[i]);
        }
    }
}
