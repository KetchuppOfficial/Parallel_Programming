#!/bin/bash

green="\033[1;32m"
red="\033[1;31m"
default="\033[0m"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR=${SCRIPT_DIR%/*}
BUILD_DIR=$PROJECT_DIR/build

function clean_up()
{
    rm -rf $BUILD_DIR
    rm -rf $SCRIPT_DIR/*.res
}

function build()
{
    echo -e "${green}Building project...${default}"
    cmake -S $PROJECT_DIR -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release
    cmake --build $BUILD_DIR -j12
    echo -en "\n"
}

function run()
{
    local n_proc=$1
    local per_process=$2
    local total=$((per_process * n_proc))

    echo -en "${green}Running sequential program...${default}"
    $BUILD_DIR/sequential-pi --n-iterations $total > $SCRIPT_DIR/sequential.res
    SEQUENTIAL_TIME=$(tail -n 1 $SCRIPT_DIR/sequential.res | rev | cut -d' ' -f2 | rev)
    echo -en " $SEQUENTIAL_TIME ms\n\n"

    echo -en "${green}Running parallel program...${default}"
    mpirun -c $n_proc $BUILD_DIR/parallel-pi --n-iterations $per_process > $SCRIPT_DIR/parallel.res
    PARALLEL_TIME=$(tail -n 1 $SCRIPT_DIR/parallel.res | rev | cut -d' ' -f2 | rev)
    echo -en " $PARALLEL_TIME ms\n\n"
}

function compare_results()
{
    local SEQUENTIAL_RES=$(head -n 1 $SCRIPT_DIR/sequential.res)
    local PARALLEL_RES=$(head -n 1 $SCRIPT_DIR/parallel.res)

    if [ "$SEQUENTIAL_RES" == "$PARALLEL_RES" ]
    then
        echo -e "${green}Test passed${default}"
    else
        echo -e "${red}Test failed${default}"
    fi
}

source $SCRIPT_DIR/opts.sh

clean_up
build
run $N_PROC $PER_PROC
compare_results
