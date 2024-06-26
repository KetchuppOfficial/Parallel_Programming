# Computation of pi number: parallel and not so parallel

## Formula used to compute pi

$$\pi = \sum_{k = 0}^{\infty} \left[\frac{1}{16^k}\left(\frac{4}{8k + 1} - \frac{2}{8k + 4} - \frac{1}{8k + 5} - \frac{1}{8k + 6}\right)\right]$$

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Parallel_Programming/01-pi-computing/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```

**tgt** can be **sequential** or **parallel**.

If --target option is omitted, both targets will be built.

### 2) How to run

- Sequential program:

    ```bash
    ./build/sequential --help
    # Allowed options:
    #   --help                Produce help message
    #   --n-iterations arg    Set the number of iterations
    ```

    Example of usage:

    ```bash
    ./build/sequential --n-iterations 6000
    ```

- Parallel program:

    ```bash
    mpirun -c N ./build/parallel --help
    # Allowed options:
    #   --help                Produce help message
    #   --n-iterations arg    Set the number of iterations per process
    ```

    **N** - the number of nodes.

    Example of usage:

    ```bash
    mpirun -c 6 ./build/parallel --n-iterations 1000
    ```

### 3) How to run tests

If you want to compare results of computing pi by both programs, there is a convenient script
[run.sh](/01-pi-computing/test/run.sh) provided.

Command:

```bash
test/run.sh -c <number-of-processes-to-run> -n <iterations-per-process>
```

runs both sequential and parallel programs to compute an approximation of pi. The script compares
the results of computation and prints time took to execute the programs.
