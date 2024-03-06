#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"
BUILD_DIR=${SCRIPT_PATH}/build
UT_RESULTS=${BUILD_DIR}/results

RED_CODE='\033[0;31m'
GREEN_CODE='\033[0;32m'
NC_CODE='\033[00m'

function echo_error {
    echo -e "${RED_CODE}${1}${NC_CODE}"
}

function echo_info {
    echo -e "${GREEN_CODE}${1}${NC_CODE}"
}

if [ -d ${BUILD_DIR} ]; then
    rm -rf ${BUILD_DIR}
fi

mkdir -p ${BUILD_DIR}
mkdir -p ${UT_RESULTS}

qmake -o ${BUILD_DIR} -r ${SCRIPT_PATH}
make -C ${BUILD_DIR} -j4

non_zero_return_codes=()

for ut_dir_path in ${BUILD_DIR}/ut_*; do
    ut_dir=$(basename "$ut_dir_path")

    if [ -d "$ut_dir_path" ]; then
        ut_exec="$ut_dir_path/$ut_dir"

        if [ -x "$ut_exec" ]; then
            ut_suite_name=$(basename "$ut_exec")
            echo_info "Test suite: $ut_suite_name"
            ut_result="${UT_RESULTS}/${ut_dir}.txt"
            output=$("$ut_exec" 2>&1)
            return_code=$?
            echo "$output" | tee "$ut_result"

            if [ $return_code -ne 0 ]; then
                non_zero_return_codes+=("Test suite \"$ut_suite_name\", number of failed tests: $return_code")
            fi
        else
            echo_error "Cannot find executable file: $ut_exec"
        fi
    fi
done

if [ ${#non_zero_return_codes[@]} -gt 0 ]; then
    echo_error "The number of failed test suites: ${#non_zero_return_codes[@]}"
    for code in "${non_zero_return_codes[@]}"; do
        echo_error "$code"
    done
else
    echo_info "All test suites succeeded"
fi

echo_info "See UT results in $UT_RESULTS"

exit "${#non_zero_return_codes[@]}"
