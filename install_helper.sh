#!/bin/bash

function run_systemd_command_by_user() {
    local USER_NAME="$1"
    local COMMAND="$2"

    sudo -u ${USER_NAME}  XDG_RUNTIME_DIR=/run/user/$(id -u ${USER_NAME}) systemctl --user $COMMAND
}

function check_if_service_exists() {
    local n=$1
    if [[ $(systemctl list-units --all -t service --full --no-legend "$n" | sed 's/^\s*//g' | cut -f1 -d' ') == $n ]]; then
        return 0
    else
        return 1
    fi
}

function check_if_user_service_exists() {
    local SERVICE_NAME="$1"
    local USER_NAME="$2"

    if [[ $(run_systemd_command_by_user "$USER_NAME" "list-units --all -t service --full --no-legend "$SERVICE_NAME"" | sed 's/^\s*//g' | cut -f1 -d' ') == $SERVICE_NAME ]]; then
        return 0
    else
        return 1
    fi
}

function disable_user_service() {
    local SERVICE_NAME="$1"
    local SERVICE_PATH="$2"
    local USER_NAME="$3"

    if check_if_user_service_exists "$SERVICE_NAME" "$USER_NAME"; then
        run_systemd_command_by_user "$USER_NAME" "stop $SERVICE_NAME"
        run_systemd_command_by_user "$USER_NAME" "disable $SERVICE_NAME"
        rm -rf "$SERVICE_PATH"
        run_systemd_command_by_user "$USER_NAME" "daemon-reload"
    fi
}

function disable_service() {
    local SERVICE_NAME="$1"
    local SERVICE_PATH="$2"

    if check_if_service_exists "$SERVICE_NAME"; then
        systemctl stop "$SERVICE_NAME"
        systemctl disable "$SERVICE_NAME"
        rm -rf "$SERVICE_PATH"
        systemctl daemon-reload
    fi
}

function add_service_to_autostart() {
    local SERVICE_NAME="$1"
    local HOME_DIR="$2"
    local AUTOSTART_FILE="$HOME_DIR/.config/lxsession/Lubuntu/autostart"
    local LINE_TO_ADD="systemctl --user start $SERVICE_NAME"

    if [ -f "$AUTOSTART_FILE" ]; then
        if ! grep -q "$LINE_TO_ADD" "$AUTOSTART_FILE"; then
            echo "$LINE_TO_ADD" >> "$AUTOSTART_FILE"
            echo "Service $SERVICE_NAME was added to autostart"
        fi
    fi
}

function remove_service_from_autostart() {
    local SERVICE_NAME="$1"
    local HOME_DIR="$2"
    local AUTOSTART_FILE="$HOME_DIR/.config/lxsession/Lubuntu/autostart"
    local LINE_TO_REMOVE="systemctl --user start $SERVICE_NAME"

    if [ -f "$AUTOSTART_FILE" ]; then
        sed -i "/$LINE_TO_REMOVE/d" "$AUTOSTART_FILE"
    fi
}

function verify_user_service() {
    local SERVICE_NAME="$1"
    local USER_NAME="$2"
    sleep 3
    is_active=$(run_systemd_command_by_user "$USER_NAME" "is-active $SERVICE_NAME")

    if [ "$is_active" = "active" ]; then
        echo "The $SERVICE_NAME was successfully installed"
    else
        echo "Failed to install $SERVICE_NAME"
        exit 1
    fi
}

function verify_service() {
    local SERVICE_NAME="$1"

    sleep 3
    is_active=$(systemctl is-active "$SERVICE_NAME")

    if [ "$is_active" = "active" ]; then
        echo "The $SERVICE_NAME was successfully installed"
    else
        echo "Failed to install $SERVICE_NAME"
        exit 1
    fi
}

function enable_user_service() {
    local SERVICE_NAME="$1"
    local USER_NAME="$2"

    run_systemd_command_by_user "$USER_NAME" "enable $SERVICE_NAME"
    run_systemd_command_by_user "$USER_NAME" "daemon-reload"
    run_systemd_command_by_user "$USER_NAME" "restart $SERVICE_NAME"

    add_service_to_autostart $SERVICE_NAME "/home/$USER_NAME"
}

function enable_service() {
    local SERVICE_NAME="$1"

    systemctl enable "$SERVICE_NAME"
    systemctl daemon-reload
    systemctl restart "$SERVICE_NAME"
}

function get_user() {
    who | awk '{print $1}' | sort | uniq | head -n 1;
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

function cleanup_directory() {
    remove_directories "$1"
    create_directories "$1"
}

function build_project() {
    local BUILD_DIR="$1"
    local SOURCE_DIR="$2"

    cleanup_directory "$BUILD_DIR"

    qmake -o ${BUILD_DIR}/Makefile -r ${SOURCE_DIR}
    make -C "$BUILD_DIR"

    if [ $? -ne 0 ]; then
        echo "Failed to build a project" >&2
        remove_directories "$BUILD_DIR"
        exit 1
    fi
}

function install_project() {
    local INSTALL_PROJECT_DIR="$1"
    local BUILD_PROJECT_DIR="$2"

    cleanup_directory "$INSTALL_PROJECT_DIR"

    make -C "$BUILD_PROJECT_DIR" install
    remove_directories "$BUILD_PROJECT_DIR"
}

function install_service_file() {
    echo "$1" > "$2"
}

function install_file_if_not_exist() {
    if [ ! -f ${2} ]; then
        cp ${1} ${2}
    fi
}

function create_symbolic_link() {
    ln -sf ${1} ${2}
}

function copy_dir_content() {
    cp -r ${1}/* ${2}
}
function change_folder_owner() {
    local NEW_OWNER="$1"
    local FOLDER_PATH="$2"

    chown -R $NEW_OWNER: $FOLDER_PATH
}

function backup_project_if_exist() {

    local INSTALL_PROJECT_DIR="$1"
    local BACKUP_PROJECT_DIR="$2"

    if [ ! -d "$INSTALL_PROJECT_DIR" ]; then
        echo "Nothing to backup, directory $INSTALL_PROJECT_DIR does not exist"
        return 1
    fi

    copy_dir_content "$INSTALL_PROJECT_DIR" "$BACKUP_PROJECT_DIR"
    return 0
}

function restore_backup() {
    local EXIT_CODE=$?
    local INSTALL_PROJECT_DIR="$1"
    local BACKUP_PROJECT_DIR="$2"
    local SERVICE_NAME="$3"
    local USER_NAME="$4"
    local IS_USER="$5"

    if [ "$EXIT_CODE" = 0 ]; then
        exit 0
    fi

    echo "Instalation of $SERVICE_NAME was ended with error"
    echo "Restoring previous version ..."

    cleanup_directory "$INSTALL_PROJECT_DIR"

    copy_dir_content "$BACKUP_PROJECT_DIR" "$INSTALL_PROJECT_DIR"

    if $IS_USER; then
        enable_user_service "$SERVICE_NAME" "$USER_NAME"
    else
        enable_service "$SERVICE_NAME"
    fi

    remove_directories "$BACKUP_PROJECT_DIR"
    exit $EXIT_CODE
}
