#!/bin/bash

set -u

SCRIPT_PATH="$(dirname "$0")"
source "$SCRIPT_PATH/install_helper.sh"

PROJECT_DIR="/opt/GasStationPro/app_updater"
SERVICE_NAME="app-updater.service"
SERVICE_FILE_LINK_PATH="/etc/systemd/system/$SERVICE_NAME"

COMPONENT_USER=$(get_user)
HOME_DIR="/home/$COMPONENT_USER"
WORK_DIR="$HOME_DIR/.GasStationPro"

function usage() {
    echo "Usage: $0 [-f] [--help]" 1>&2
    echo "  -f    Remove work directory" 1>&2
    echo "  -h    Print help message" 1>&2
    exit 1
}

force=false

while getopts ":fh" opt; do
    case ${opt} in
        f)
            force=true
            ;;
        h)
            usage
            ;;
        \?)
            echo "The wrong argument was passed: -$OPTARG" >&2
            usage
            ;;
    esac
done

disable_service ${SERVICE_NAME} ${SERVICE_FILE_LINK_PATH}
remove_directories ${PROJECT_DIR}

if [ "$force" = true ]; then
    remove_directories ${WORK_DIR}
fi
