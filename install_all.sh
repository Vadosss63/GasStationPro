#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"

"$SCRIPT_PATH"/installDep.sh
"$SCRIPT_PATH"/install_t_azs.sh
"$SCRIPT_PATH"/install_log_uploader.sh
"$SCRIPT_PATH"/install_updater.sh
