# Makefile for Parallel Dijkstra's Algorithm

CC = mpicc
CFLAGS = -Wall -O2
LDFLAGS = -lm
TARGET = dijkstra_parallel
SRC = parallel.c

# Default target
all: $(TARGET)

# Compile the parallel program
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Run with default parameters (4 processes, 1000 vertices)
run: $(TARGET)
	mpirun -np 4 ./$(TARGET) -n 1000 -d 0.3

# Run scaling test (1, 2, 4, 8 processes)
test-scaling: $(TARGET)
	@echo "=== Scaling Test ==="
	@echo "1 process:"
	@mpirun -np 1 ./$(TARGET) -n 1000 -d 0.3
	@echo ""
	@echo "2 processes:"
	@mpirun -np 2 ./$(TARGET) -n 1000 -d 0.3
	@echo ""
	@echo "4 processes:"
	@mpirun -np 4 ./$(TARGET) -n 1000 -d 0.3
	@echo ""
	@echo "8 processes:"
	@mpirun -np 8 ./$(TARGET) -n 1000 -d 0.3

# Run with different problem sizes
test-sizes: $(TARGET)
	@echo "=== Problem Size Test (4 processes) ==="
	@echo "N = 500:"
	@mpirun -np 4 ./$(TARGET) -n 500 -d 0.3
	@echo ""
	@echo "N = 1000:"
	@mpirun -np 4 ./$(TARGET) -n 1000 -d 0.3
	@echo ""
	@echo "N = 1500:"
	@mpirun -np 4 ./$(TARGET) -n 1500 -d 0.3
	@echo ""
	@echo "N = 2000:"
	@mpirun -np 4 ./$(TARGET) -n 2000 -d 0.3

# Run small test with verbose output
test-verbose: $(TARGET)
	mpirun -np 2 ./$(TARGET) -n 20 -d 0.5 -v

# Clean build artifacts
clean:
	rm -f $(TARGET) *.o

# Help target
help:
	@echo "Available targets:"
	@echo "  make              - Build the parallel program"
	@echo "  make run          - Run with default parameters (4 processes, N=1000)"
	@echo "  make test-scaling - Test with 1, 2, 4, 8 processes"
	@echo "  make test-sizes   - Test with different problem sizes"
	@echo "  make test-verbose - Run small test with verbose output"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make help         - Show this help message"

.PHONY: all run test-scaling test-sizes test-verbose clean help
