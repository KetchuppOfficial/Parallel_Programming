import subprocess
from pathlib import Path
from matplotlib import pyplot as plt

N_MEASUREMENTS : int = 5
MAX_THREADS : int = 6

def measure(prog : str, tasks : list[str], max_threads=MAX_THREADS) -> dict[str, list[float]]:
    measurements : dict[str, list[float]] = {}

    for task in tasks:
        measurements[task] = []
        for n in range(max_threads):
            time : int = 0
            for _ in range(N_MEASUREMENTS):
                cmd : list[str] = [f"build/{prog}", "-t", "-m", task]
                n_threads : dict[str, str] = {"OMP_NUM_THREADS" : str(n + 1)}
                proc = subprocess.run(cmd, capture_output=True, text=True, env=n_threads)
                time += int(proc.stdout.split(" ")[3])
            measurements[task].append(time / N_MEASUREMENTS)

    return measurements


def main() -> None:
    measurements_seq : dict[str, list[float]] = \
        measure("sequential", ["reference", "task1", "task2", "task3"], max_threads=1)
    measurements_omp : dict[str, list[float]] = \
        measure("parallel_omp", ["reference", "task2", "task3"])

    for task, time in measurements_omp.items():
        plt.figure(figsize = (16, 9), dpi = 100, layout = "compressed")
        plt.title(f"{task}. absolute execution time")
        plt.xlabel("number of threads", loc = "right")
        plt.ylabel("execution time, ms")

        plt.bar(range(1, 1 + MAX_THREADS), time)
        plt.savefig(f"images/{task}.omp.time.png")

        plt.figure(figsize = (16, 9), dpi = 100, layout = "compressed")
        plt.title(f"{task}. execution boost")
        plt.xlabel("number of threads", loc = "right")
        plt.ylabel("boost")

        seq_time = measurements_seq[task][0]
        plt.bar(range(1, 1 + MAX_THREADS), [seq_time / par_time for par_time in time])
        plt.savefig(f"images/{task}.omp.boost.png")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Caught an instance of type {type(e)}.\nwhat(): {e}")
