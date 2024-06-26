#!/bin/bash

set -u

SCRIPT_PATH="$(dirname "$0")"
source "$SCRIPT_PATH/install_helper.sh"

SETTINGS_FILE=${SCRIPT_PATH}/GasTeminal/gas_station/settings/settings.json
SOURCE_DIR=${SCRIPT_PATH}/GasTeminal/gas_station
INSTALL_DIR="/opt/GasStationPro/gas_station"
SERVICE_NAME="gas-station.service"
BUILD_DIR="$SOURCE_DIR/build"
SERVICE_FILE_DIR="$INSTALL_DIR/service"
SERVICE_FILE_PATH="$SERVICE_FILE_DIR/$SERVICE_NAME"
SERVICE_FILE_LINK_PATH="/etc/systemd/user/$SERVICE_NAME"

COMPONENT_USER=$(get_user)
HOME_DIR="/home/$COMPONENT_USER"
WORK_DIR="$HOME_DIR/.GasStationPro"
SETTINGS_DEST=${WORK_DIR}/settings.json

SERVICE_FILE_TEMPLATE="[Unit]
Description=gas-station service
After=network.target

[Service]
ExecStart=$INSTALL_DIR/bin/gas_station
WorkingDirectory=$WORK_DIR
Restart=always

[Install]
WantedBy=graphical-session.target"

usermod -a -G dialout $COMPONENT_USER
build_project ${BUILD_DIR} ${SOURCE_DIR}


BACKUP_DIR=$(mktemp -d)
if backup_project_if_exist "$INSTALL_DIR" "$BACKUP_DIR"; then
    trap 'restore_backup "$INSTALL_DIR" "$BACKUP_DIR" "$SERVICE_NAME" "$COMPONENT_USER" true' EXIT
    echo "Backup was saved to $BACKUP_DIR"
fi

disable_user_service ${SERVICE_NAME} ${SERVICE_FILE_LINK_PATH} ${COMPONENT_USER}

install_project ${INSTALL_DIR} ${BUILD_DIR}
create_directories ${WORK_DIR} ${SERVICE_FILE_DIR}
install_file_if_not_exist ${SETTINGS_FILE} ${SETTINGS_DEST}
change_folder_owner ${COMPONENT_USER} ${WORK_DIR}

install_service_file "$SERVICE_FILE_TEMPLATE" "$SERVICE_FILE_PATH"
create_symbolic_link ${SERVICE_FILE_PATH} ${SERVICE_FILE_LINK_PATH}

enable_user_service ${SERVICE_NAME} ${COMPONENT_USER} 
verify_user_service ${SERVICE_NAME} ${COMPONENT_USER}
