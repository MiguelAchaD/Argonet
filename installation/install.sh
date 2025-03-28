#!/bin/bash

# ASCII art generator: https://patorjk.com/software/taag/#p=display&f=DiamFont&t=TEST
INSTALLATION_ASCII="
▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▄▄▄▖▗▄▖ ▗▖   ▗▖    ▗▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▖ ▗▖  ▗▖
  █  ▐▛▚▖▐▌▐▌     █ ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌ █    █  ▐▌ ▐▌▐▛▚▖▐▌
  █  ▐▌ ▝▜▌ ▝▀▚▖  █ ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▜▌ █    █  ▐▌ ▐▌▐▌ ▝▜▌
▗▄█▄▖▐▌  ▐▌▗▄▄▞▘  █ ▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▌ ▐▌ █  ▗▄█▄▖▝▚▄▞▘▐▌  ▐▌
"
DEPENDENCIES_ASCII="
▗▄▄▄  ▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖▗▄▄▄  ▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▄▖
▐▌  █ ▐▌   ▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌  █ ▐▌   ▐▛▚▖▐▌▐▌     █  ▐▌   ▐▌   
▐▌  █ ▐▛▀▀▘▐▛▀▘ ▐▛▀▀▘▐▌ ▝▜▌▐▌  █ ▐▛▀▀▘▐▌ ▝▜▌▐▌     █  ▐▛▀▀▘ ▝▀▚▖
▐▙▄▄▀ ▐▙▄▄▖▐▌   ▐▙▄▄▖▐▌  ▐▌▐▙▄▄▀ ▐▙▄▄▖▐▌  ▐▌▝▚▄▄▖▗▄█▄▖▐▙▄▄▖▗▄▄▞▘
"

###################################################
## AUTOMATIC INSTALLATION SCRIPT ##################
###################################################

CURRENT_DATE=$(date +"%Y%m%d%H%M%s")

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
PROJECT_DIR="$(dirname $SCRIPT_DIR)"
LOG_DIR="$PROJECT_DIR/logs"
LOG_FILE="$LOG_DIR/log$CURRENT_DATE"
DEPENDENCIES_FILE="$SCRIPT_DIR/dependencies.txt"

WHITE="\e[38;5;15m"
RED="\e[38;5;196m"
BLUE="\e[38;5;21m"
GREEN="\e[38;5;46m"
YELLOW="\e[38;5;226m"
ENDCOLOUR=$WHITE

###################################################
## LOGGING METHODS ################################
###################################################

function prepareLogs {
    if [[ ! -d "$LOGS_DIR" ]]; then
        mkdir -p $LOG_DIR
    fi

    touch $LOG_FILE
}

function section {
    echo -e "$1"
}

function log {
    local type=$1
    local message=$2

    case "$type" in
        "LOG")
            echo -e "${WHITE}:: $message${ENDCOLOUR}" | tee -a $LOG_FILE
        ;;
        "WARNING")
            echo -e "${YELLOW}:! $message${ENDCOLOUR}" | tee -a $LOG_FILE
        ;;
        "ERROR")
            echo -e "${RED}!! $message${ENDCOLOUR}" | tee -a $LOG_FILE
        ;;
        "SUCCESS")
            echo -e "${GREEN}// $message${ENDCOLOUR}" | tee -a $LOG_FILE
        ;;
        *)
            echo -e ":: $message${ENDCOLOUR}" | tee -a $LOG_FILE
        ;;
    esac
}

function askForPermission {
    while true; do
        read -rp "Do you want to continue? (y/n): " answer
        case "$answer" in
            [yY][eE][sS]|[yY])
                echo -e "${BLUE}Continuing...${ENDCOLOUR}"
                break
                ;;
            [nN][oO]|[nN])
                echo -e "${RED}Exiting...${ENDCOLOUR}"
                exit 1
                ;;
            *)
                echo -e "${RED}Invalid input. Please enter 'y' or 'n'.${ENDCOLOUR}"
                ;;
        esac
    done
}

###################################################
## INSTALLATION METHODS ###########################
###################################################

function detectPackageManager {
    if command -v pacman &>/dev/null; then
        log "SUCCESS" "Detected pacman (Arch-based distribution)"
        PACKAGE_MANAGER="pacman"
        INSTALL_CMD="sudo pacman -S --noconfirm"
    elif command -v apt &>/dev/null; then
        log "SUCCESS" "Detected apt (Debian/Ubuntu-based distribution)"
        PACKAGE_MANAGER="apt"
        INSTALL_CMD="sudo apt install -y"
    elif command -v dnf &>/dev/null; then
        log "SUCCESS" "Detected dnf (Fedora-based distribution)"
        PACKAGE_MANAGER="dnf"
        INSTALL_CMD="sudo dnf install -y"
    elif command -v zypper &>/dev/null; then
        log "SUCCESS" "Detected zypper (openSUSE-based distribution)"
        PACKAGE_MANAGER="zypper"
        INSTALL_CMD="sudo zypper install -y"
    elif command -v yay &>/dev/null; then
        log "SUCCESS" "Detected yay (AUR helper)"
        PACKAGE_MANAGER="yay"
        INSTALL_CMD="yay -S --noconfirm"
    else
        log "ERROR" "No supported package manager found"
        exit 1
    fi
}

###################################################
## FILE METHODS ###################################
###################################################

function readDependenciesAndInstall {
    if [[ ! -n "$DEPENDENCIES_FILE" ]]; then
        log "ERROR" "No dependencies file found"
        exit 1
    fi

    log "SUCCESS" "Found dependencies file, prepared for installation..."
    askForPermission

    while read -r line
    do
        if [[ -n "$line" ]]; then
            log "LOG" "Trying to install \"$line\""

            case "$PACKAGE_MANAGER" in
                pacman|apt|dnf|zypper|yay)
                    if command -v "$PACKAGE_MANAGER" &>/dev/null; then
                        log "LOG" "Installing \"$line\" with $PACKAGE_MANAGER."
                        $INSTALL_CMD "$line"
                    else
                        log "ERROR" "Package \"$line\" not found in $PACKAGE_MANAGER."
                    fi
                ;;
                *)
                    log "ERROR" "Unsupported package manager \"$PACKAGE_MANAGER\""
                    exit 1
                ;;
            esac
        fi
    done < "$DEPENDENCIES_FILE"
}

###################################################
## MAIN ###########################################
###################################################
prepareLogs
section "${BLUE}$INSTALLATION_ASCII"
detectPackageManager
section "${BLUE}$DEPENDENCIES_ASCII"
readDependenciesAndInstall
