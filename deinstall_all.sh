#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"
source "$SCRIPT_PATH/install_helper.sh"

ROOT_PROJECT_DIR="/opt/GasStationPro"

${SCRIPT_PATH}/deinstall_log_uploader.sh "$@"
${SCRIPT_PATH}/deinstall_t_azs.sh "$@"
${SCRIPT_PATH}/deinstall_updater.sh "$@"

remove_directories ${ROOT_PROJECT_DIR}
