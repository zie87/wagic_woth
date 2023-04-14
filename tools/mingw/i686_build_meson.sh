#!/usr/bin/env bash

set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/../../"
WINE_ROOT_DIR="${ROOT_DIR}/.wine/"

BUILD_BASE_DIR="build/mingw"

function mingw_build() {
    BUILD_DIR="${BUILD_BASE_DIR}32/$1"
    rm -rf ${BUILD_DIR}

    export WINEARCH=win32
    export WINEPREFIX="${WINE_ROOT_DIR}/wine32" 
    export WINEPATH=/usr/i686-w64-mingw32/sys-root/mingw/bin

    meson setup --cross-file ./tools/mingw/i686-w64-mingw32.build \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -C "${BUILD_DIR}"
    # meson test -C "${BUILD_DIR}" --print-errorlogs
}

cd ${ROOT_DIR}

mkdir -p ${WINE_ROOT_DIR}
if [ "${1}" = "Release" ]; then
    mingw_build release "false"
elif [ "${1}" = "Size" ]; then
    mingw_build minsize "false"
else
    mingw_build debug "true"
fi
