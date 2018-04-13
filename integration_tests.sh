#!/bin/bash

CK=$(tput setaf 0)
BLUE=$(tput setaf 1)
GREEN=$(tput setaf 2)
CYAN=$(tput setaf 3)
RED=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
YELLOW=$(tput setaf 6)
WHITE=$(tput setaf 7)

BOLD=$(tput bold)
RESET=$(tput sgr0)

function summary {
    echo -e "$BOLD$GREEN==>$WHITE ${1}$RESET"
}

failed=0

function compile_and_run {
    local test_program=$1

    # Compile the file.
    ./yabfc -s tests/$test_program
    if [[ $? -ne 0 ]]; then
        echo "Compilation failed!"
        failed=1
        return
    fi

    # Run it, saving output.
    local executable=tests/"${test_program%.*}"
    local input=tests/${test_program}.in

    if [ -f $input ]; then
        valgrind -q --tool=none ./$executable < $input > output.txt
    else
        valgrind -q --tool=none ./$executable > output.txt
    fi
    if [[ $? -ne 0 ]]; then
        echo "Program crashed!"
        failed=1
        return
    fi

    # Compare output.
    local expected_output=tests/${test_program}.out
    if [ -f $expected_output ]; then
        diff output.txt $expected_output > diff.txt
        if [[ $? -ne 0 ]]; then
            echo "Output differs!"
            cat diff.txt
            failed=1
            return
        fi
    fi
}

function check_program {
    summary "Testing $1"
    compile_and_run $1

    # Cleanup.
    rm -f ${1%.*} output.txt diff.txt
}

check_program primes.bf
check_program hello.bf
check_program mandelbrot.bf

exit $failed

