#!/usr/bin/env bash

set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/../../"
WINE_ROOT_DIR="${ROOT_DIR}/.wine/"

BUILD_BASE_DIR="build/mingw"

function mingw_build() {
    BUILD_DIR="${BUILD_BASE_DIR}64/$1"
    rm -rf ${BUILD_DIR}

    export WINEPREFIX="${WINE_ROOT_DIR}/wine64" 
    export WINEPATH=/usr/x86_64-w64-mingw32/sys-root/mingw/bin
    
    meson setup --cross-file ./tools/mingw/x86_64-w64-mingw32.build \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -C "${BUILD_DIR}"
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
