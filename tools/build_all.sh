#!/usr/bin/env bash

set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/../"

function build_psp() {
    docker run --rm -u $UID --init -v ${ROOT_DIR}:/home/user/workspace:Z -e LANG=$LANG \
        -w /home/user/workspace -it zie87/pspdev ./tools/psp/psp_build_meson.sh $1
}

function build_mingw_32() {
    docker run --rm -u $UID --init -v ${ROOT_DIR}:/home/user/workspace:Z -e LANG=$LANG \
        -w /home/user/workspace -it zie87/mingw ./tools/mingw/i686_build_meson.sh $1
}

function build_mingw_64() {
    docker run --rm -u $UID --init -v ${ROOT_DIR}:/home/user/workspace:Z -e LANG=$LANG \
        -w /home/user/workspace -it zie87/mingw ./tools/mingw/x86_64_build_meson.sh $1
}

function build_linux_run() {
    BUILD_BASE_DIR="${ROOT_DIR}/build/sdl"
    
    BUILD_DIR="${BUILD_BASE_DIR}/$1"
    rm -rf ${BUILD_DIR}
    meson setup \
        --buildtype $1 "${BUILD_DIR}" \
        -Denable_testsuite=$2
    meson compile -v -C "${BUILD_DIR}"
    
}

function build_linux() {
    if [ "${1}" = "Release" ]; then
        build_linux_run release "false"
    elif [ "${1}" = "Size" ]; then
        build_linux_run minsize "false"
    else
        build_linux_run debug "true"
    fi
}

function meson_build() {
    build_psp $1
    build_mingw_32 $1
    build_mingw_64 $1
    build_linux $1
}

cd ${ROOT_DIR}

if [ "${1}" = "Release" ]; then
    meson_build "Release"
elif [ "${1}" = "Size" ]; then
    meson_build "Size"
else
    meson_build "Debug"
fi
