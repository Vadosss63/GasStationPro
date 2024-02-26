#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"
BUILD_DIR=${SCRIPT_PATH}/build/

if [ -d ${BUILD_DIR} ]; then
    rm -rf ${BUILD_DIR}
fi

mkdir -p ${BUILD_DIR}
qmake -o ${BUILD_DIR} -r ${SCRIPT_PATH}
make -C ${BUILD_DIR} -j4

${BUILD_DIR}/LoggerTest 2>&1 | tee ${BUILD_DIR}/LoggerTestResults.txt

exit_code=$?

exit "$exit_code"
