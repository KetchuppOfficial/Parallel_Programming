# Computation of pi number: parallel and not so parallel

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Parallel_Programming/01-pi-computing/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```

**tgt** can be **pi-sequential** or **pi-parallel**.

If --target option is omitted, both targets will be built.

### 2) How to run

- Sequential program:

    ```bash
    build/sequential-pi --help
    # Allowed options:
    #   --help                produce help message
    #   --n-iterations arg    set the number of iterations
    ```

    Example of usage:

    ```bash
    build/sequential-pi --n-iterations 6000
    ```

- Parallel program:

    ```bash
    mpirun -c N build/parallel-pi --help
    # Allowed options:
    #   --help                produce help message
    #   --n-iterations arg    set the number of iterations
    ```

    **N** - the number of nodes.

    Example of usage:

    ```bash
    mpirun -c 6 build/parallel-pi --n-iterations 1000
    ```

    The number of iterations is considered per node.
