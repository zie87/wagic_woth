#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/../../"

BUILD_BASE_DIR="build/mingw"

function mingw_w32_build() {
    BUILD_DIR="${BUILD_BASE_DIR}32/$1"
    rm -rf ${BUILD_DIR}
    meson setup --cross-file ./tools/mingw/i686-w64-mingw32.build \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -v -C "${BUILD_DIR}"
}

function mingw_w64_build() {
    BUILD_DIR="${BUILD_BASE_DIR}64/$1"
    rm -rf ${BUILD_DIR}
    meson setup --cross-file ./tools/mingw/x86_64-w64-mingw32.build \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -v -C "${BUILD_DIR}"
}

function mingw_build() {
    mingw_w32_build $1 $2
    mingw_w64_build $1 $2
}


cd ${ROOT_DIR}

if [ "${1}" = "Release" ]; then
    mingw_build release "false"
elif [ "${1}" = "Size" ]; then
    mingw_build minsize "false"
else
    mingw_build debug "true"
fi
