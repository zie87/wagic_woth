#!/usr/bin/env bash

set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/../../"

BUILD_BASE_DIR="build/psp"

function meson_build() {
    BUILD_DIR="${BUILD_BASE_DIR}/$1"
    rm -rf ${BUILD_DIR}
    meson setup --cross-file ./tools/psp/mips_psp.build \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -C "${BUILD_DIR}"
}

cd ${ROOT_DIR}

if [ "${1}" = "Release" ]; then
    meson_build release "false"
elif [ "${1}" = "Size" ]; then
    meson_build minsize "false"
else
    meson_build debug "true"
fi
