#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define INF 999999
#define MAX_WEIGHT 100

// Function prototypes
void generate_graph(int **graph, int n, double density);
void dijkstra(int **graph, int *dist, int n, int source);
int find_min_distance(int *dist, int *visited, int n);
void print_distances(int *dist, int n, int source);
double get_time();

int main(int argc, char *argv[]) {
    int n = 100;              // Default number of vertices
    double density = 0.3;     // Default edge density
    int source = 0;           // Default source vertex
    int verbose = 0;          // Verbose output flag
    
    // Parse command line arguments
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
        fprintf(stderr, "Error: Number of vertices must be between 1 and 10000\n");
        return 1;
    }
    if (density < 0.0 || density > 1.0) {
        fprintf(stderr, "Error: Density must be between 0.0 and 1.0\n");
        return 1;
    }
    if (source < 0 || source >= n) {
        fprintf(stderr, "Error: Source vertex must be between 0 and %d\n", n - 1);
        return 1;
    }
    
    printf("=== Sequential Dijkstra's Algorithm ===\n");
    printf("Vertices: %d\n", n);
    printf("Edge Density: %.2f\n", density);
    printf("Source Vertex: %d\n", source);
    
    // Allocate memory for graph (adjacency matrix)
    int **graph = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph[i] = (int *)malloc(n * sizeof(int));
    }
    
    // Allocate memory for distance array
    int *dist = (int *)malloc(n * sizeof(int));
    
    // Generate random graph
    printf("Generating random graph...\n");
    generate_graph(graph, n, density);
    
    // Run Dijkstra's algorithm with timing
    printf("Running Dijkstra's algorithm...\n");
    double start_time = get_time();
    dijkstra(graph, dist, n, source);
    double end_time = get_time();
    
    double elapsed_time = end_time - start_time;
    printf("Execution Time: %.6f seconds\n", elapsed_time);
    
    // Print distances if verbose
    if (verbose) {
        print_distances(dist, n, source);
    }
    
    // Calculate some statistics
    int reachable = 0;
    long long total_dist = 0;
    for (int i = 0; i < n; i++) {
        if (dist[i] != INF) {
            reachable++;
            total_dist += dist[i];
        }
    }
    printf("Reachable vertices: %d/%d\n", reachable, n);
    if (reachable > 1) {
        printf("Average distance: %.2f\n", (double)total_dist / (reachable - 1));
    }
    
    // Free memory
    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);
    free(dist);
    
    return 0;
}

// Generate a random weighted graph
void generate_graph(int **graph, int n, double density) {
    srand(time(NULL));
    
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
                    // Create edge with random weight [1, MAX_WEIGHT]
                    graph[i][j] = (rand() % MAX_WEIGHT) + 1;
                }
            }
        }
    }
}

// Dijkstra's algorithm implementation
void dijkstra(int **graph, int *dist, int n, int source) {
    int *visited = (int *)malloc(n * sizeof(int));
    
    // Initialize distances and visited array
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[source] = 0;
    
    // Main loop: process all vertices
    for (int count = 0; count < n; count++) {
        // Find unvisited vertex with minimum distance
        int u = find_min_distance(dist, visited, n);
        
        if (u == -1 || dist[u] == INF) {
            break;  // No more reachable vertices
        }
        
        // Mark vertex as visited
        visited[u] = 1;
        
        // Update distances to neighbors
        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] != 0) {
                int new_dist = dist[u] + graph[u][v];
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                }
            }
        }
    }
    
    free(visited);
}

// Find unvisited vertex with minimum distance
int find_min_distance(int *dist, int *visited, int n) {
    int min_dist = INF;
    int min_vertex = -1;
    
    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] < min_dist) {
            min_dist = dist[i];
            min_vertex = i;
        }
    }
    
    return min_vertex;
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

// Get current time in seconds
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
