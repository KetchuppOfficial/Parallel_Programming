# Measuring latency of sending messages

The program sends a given number of instances of `std::vector<int>` from the node with rank 0 to the
node with rank 1 and measures time spent on that.

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Parallel_Programming/00-latency/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

> [!NOTE]
> Your compiler must support a feature of C++23: literal suffix for size_t and std::println
(i.e. g++-14 and clang++-18 or newer)

### 2) How to run

```bash
mpirun -c N ./build/measure_latency --help
# Allowed options:
#     --help                Produce help message
#     --n-messages arg      Set the number of messages to send from node 0 to node 1
```

**N** - the number of nodes.

Example of usage:

```bash
mpirun -c 6 ./build/parallel --n-messages 1000
```

Possible output:

```bash
Sending 1000 instance of std::vector took 584 mcs
```