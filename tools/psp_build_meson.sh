#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/.."

BUILD_BASE_DIR="build/psp"

function meson_build() {
    BUILD_DIR="${BUILD_BASE_DIR}/$1"
    rm -rf ${BUILD_DIR}
    meson setup --cross-file ./tools/meson/psp.build --buildtype $1 "${BUILD_DIR}"
    meson compile -v -C "${BUILD_DIR}"
}

cd ${ROOT_DIR}

if [ "${1}" = "Release" ]; then
    meson_build release
elif [ "${1}" = "Size" ]; then
    meson_build minsize
else
    meson_build debug
fi
