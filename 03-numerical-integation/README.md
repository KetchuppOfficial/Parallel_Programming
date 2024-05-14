# Numerical integration

This program computes the following integral:

$$\int\limits_{a}^{b} \sin{\frac{1}{x}}dx,\ a, b > 0$$

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Parallel_Programming/03-numerical-integration/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```

**tgt** can be **sequential** or **parallel**.

If --target option is omitted, both targets will be built.

> [!NOTE]
> Your compiler must support a feature of C++23: literal suffix for size_t and std::println
(i.e. g++-14 and clang++-18 or newer)

### 2) How to run

- Sequential program:

    ```bash
    ./build/sequential --help
    # Allowed options:
    #     --help                Produce help message
    #     --from arg            Set the lower limit of integration
    #     --to arg              Set the upper limit of integration
    ```

    Example of usage:

    ```bash
    ./build/sequential --from 0.001 --to 0.1
    ```

- Parallel program:

    ```bash
    # Allowed options:
    #     --help                Produce help message
    #     --from arg            Set the lower limit of integration
    #     --to arg              Set the upper limit of integration
    #     --n-threads arg       Set the number of threads to run the program on
    ```

    Example of usage:

    ```bash
    ./build/parallel --from 0.001 --to 0.1 --n-threads 10
    ```
