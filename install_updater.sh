#!/bin/bash

SCRIPT_PATH="$(dirname "$0")"
SOURCE_DIR=${SCRIPT_PATH}/GasTeminal/gas_station/app_updater
BUILD_DIR=${SOURCE_DIR}/build
INSTALL_DIR="/opt/GasStationPro/app_updater"
SERVICE_NAME="app-updater.service"
SERVICE_FILE_PATH="/etc/systemd/system/${SERVICE_NAME}"

ORIGINAL_USER=${SUDO_USER}
if [ -z "$ORIGINAL_USER" ]; then
    ORIGINAL_USER="$USER"
fi

WORK_DIR="/home/${ORIGINAL_USER}/.GasStationPro"

SERVICE_FILE_TEMPLATE="[Unit]
Description=GasStationPro updater service

[Service]
ExecStart=$INSTALL_DIR/bin/app_updater
WorkingDirectory=$WORK_DIR
Restart=always

[Install]
WantedBy=graphical-session.target"

function disable_service() {
    if systemctl list-units --full --all | grep -Fq ${1}; then
        systemctl stop ${1}
        systemctl disable ${1}
        rm -f ${2}
    fi
}

function enable_service() {
    systemctl enable ${1}
    systemctl daemon-reload
    systemctl start ${1}
}

function remove_directories() {
    for dir in "$@"; do
        if [ -d ${dir} ]; then
            rm -rf ${dir}
        fi
    done
}

function create_directories() {
    for dir in "$@"; do
        mkdir -p  $dir
    done
}

function build_project() {
    remove_directories ${1}
    create_directories ${1}

    qmake -o ${1} -r ${2}
    make -C ${1} -j4

    if [ $? -ne 0 ]; then
        echo "Failed to build a project" >&2
        remove_directories ${1}
        exit 1
    fi
}

function install_project() {
    if [ ! -d "${1}" ]; then
        echo "Failed to install a project, directory ${1} does not exist"
        exit 1
    fi

    make -C ${1} -j4 install
    exit_code=$?

    remove_directories ${1}

    if [ exit_code -ne 0 ]; then
        echo "Failed to install a project"
        exit 1
    fi
}

function install_service_file() {
    echo "$1" > "$2"
}


build_project ${BUILD_DIR} ${SOURCE_DIR}

disable_service ${SERVICE_NAME} ${SERVICE_FILE_PATH}

remove_directories ${INSTALL_DIR}
create_directories ${INSTALL_DIR} ${WORK_DIR}

install_project ${BUILD_DIR}

install_service_file "$SERVICE_FILE_TEMPLATE" "$SERVICE_FILE_PATH"

enable_service ${SERVICE_NAME}

is_active=$(systemctl is-active "$SERVICE_NAME")

if [ "$is_active" = "active" ]; then
    echo "The $SERVICE_NAME was successfully installed"
else
    echo "Failed to install $SERVICE_NAME"
    exit 1
fi
