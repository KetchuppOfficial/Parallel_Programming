from matplotlib import pyplot as plt
import numpy as np
import subprocess
import re
import argparse
import math

def is_power_of_two(n):
    return (n != 0) and (n & (n-1) == 0)

def parse_options():
    parser = argparse.ArgumentParser(description="Matrix multiplication statistics")

    parser.add_argument("--build-dir", help="path to the build directory",
                        required=True, action="store", dest="build_dir", type=str)
    parser.add_argument("--min", help="minimum size of the matrices",
                        default=256, action="store", type=int)
    parser.add_argument("--max", help="maximum size of the matrices",
                        default=2048, action="store", type=int)
    parser.add_argument("--parallel", action="store_true")

    args = parser.parse_args()

    if not is_power_of_two(args.min):
        raise Exception("minimum size of the matrices shall be a power of 2")

    if not is_power_of_two(args.max):
        raise Exception("maximum size of the matrices shall be a power of 2")

    if args.min >= args.max:
        raise Exception("minimum size of the matrices shall be less than the maximum size")

    return args

args = parse_options()

n_sizes = int(math.log2(args.max)) - int(math.log2(args.min)) + 1
sizes = np.geomspace(start=args.min, stop=args.max, num=n_sizes, dtype=int)

results = {"naive" : [], "transpose-second" : [], "block" : []}

for algo in results.keys():
    for size in sizes:
        prog = "parallel" if args.parallel else "sequential"
        cmd = [f"./{args.build_dir}/{prog}", "--side", f"{size}", "--algorithm", algo]
        proc = subprocess.run(cmd, capture_output=True)
        time = re.findall(r"\d+", str(proc.stdout))[0]
        results[algo].append(int(time))

title = "Matrix multiplication with OpenMP" if args.parallel else "Matrix multiplication"

plt.figure(figsize = (21, 9), dpi = 100)
plt.title(title, fontsize=30)

plt.xscale("linear")
plt.yscale("linear")

plt.xlabel("matrix side", fontsize = 24)
plt.xticks(ticks=sizes, fontsize = 12, ha = "center", va = "top")

plt.ylabel("multiplication time, mcs", fontsize = 24)
plt.yticks(fontsize = 12, rotation = 30, ha = "right", va = "top")

for algo in results.keys():
    plt.scatter(sizes, np.array(results[algo]), s=15, label=algo)
    plt.plot(sizes, np.array(results[algo]), linewidth=1)

plt.legend(loc = "best", fontsize = 14)

plt.grid(color = "black", linewidth = 0.45, linestyle = "dotted")
plt.grid(which = "minor", color = "grey", linewidth = 0.25, linestyle = "dashed")

plt.show()
