#!/bin/bash

function usage()
{
    local message=$1
    echo "Error: $message"
    echo "Usage: /path/to/run.sh [OPTIONS]"
    echo ""
    echo "-c <number-of-processes-to-run>"
    echo "-n <iterations-per-process>"
    exit 1
}

N_ARGS=$#
if [ $N_ARGS -ne 4 ]; then # each option takes 2 command line arguments
    usage "Script requires 2 options"
fi

NUM="^[0-9]+$"

while getopts :c:n: OPT; do
    case $OPT in
    c) # -c <number-of-processes-to-run>
        N_PROC=$OPTARG
        [[ $N_PROC =~ $NUM ]] || usage "-c is followed not by a number"
        [[ $N_PROC -gt 0 ]] || usage "-c is followed by a non-positive number"
	    ;;
	n) # -n <iterations-per-process>
	    PER_PROC=$OPTARG
        [[ $PER_PROC =~ $NUM ]] || usage "-n is followed not by a number"
        [[ $PER_PROC -gt 0 ]] || usage "-n is followed by a non-positive number"
	    ;;
	*)
	    usage "Invalid command line argument $OPTARG"
	    ;;
  esac
done
