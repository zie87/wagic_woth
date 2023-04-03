#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_DIR="${SCRIPT_DIR}/.."

JGE_DIR="${ROOT_DIR}/JGE"
WTH_DIR="${ROOT_DIR}/projects/mtg"

function clean_psp() {
    cd "${JGE_DIR}"

    make -f Makefile.hge clean
    make clean
    
    cd "${WTH_DIR}"
    make clean
}

function build_psp() {
    cd "${JGE_DIR}"
    
    make VERBOSE=1 -f Makefile.hge -j $(nproc)
    mv libhgetools.a lib/psp/libhgetools.a
    
    make VERBOSE=1 -j $(nproc)
    
    cd "${WTH_DIR}"
    mkdir -p {objs,deps}
    make VERBOSE=1 -j $(nproc)
}

function build_psp_debug() {
    cd "${JGE_DIR}"
    
    make debug VERBOSE=1 -f Makefile.hge -j $(nproc)
    mv libhgetools.a lib/psp/libhgetools.a
    
    make debug VERBOSE=1 -j $(nproc)
    
    cd "${WTH_DIR}"
    mkdir -p {objs,deps}
    make debug VERBOSE=1 -j $(nproc)
}

if [ "${1}" = "Release" ]; then
    clean_psp 
    build_psp
elif [ "${1}" = "Clean" ]; then
    clean_psp 
else
    clean_psp 
    build_psp_debug
fi
