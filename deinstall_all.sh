#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"

"$SCRIPT_PATH"/deinstall_log_uploader.sh "$@"
"$SCRIPT_PATH"/deinstall_t_azs.sh "$@"
"$SCRIPT_PATH"/deinstall_updater.sh "$@"
