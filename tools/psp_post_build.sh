#!/bin/bash -e

SCRIPT_RELATIVE_DIR=$(dirname "${BASH_SOURCE[0]}") 
DATA_DIR="${SCRIPT_RELATIVE_DIR}/../projects/mtg"

INPUT=$1

PSP_PRX_NAME=${INPUT}.prx

PSP_EBOOT_TITLE="Wagic, the Homebrew?!"
PSP_EBOOT_ICON="${DATA_DIR}/icon.png"
PSP_EBOOT_UNKPNG="${DATA_DIR}/pic0.png"
PSP_EBOOT_PIC1="${DATA_DIR}/pic1.png"

psp-fixup-imports $INPUT
mksfo "${PSP_EBOOT_TITLE}" PARAM.SFO
psp-prxgen ${INPUT} ${PSP_PRX_NAME}
pack-pbp EBOOT.PBP PARAM.SFO "${PSP_EBOOT_ICON}" \
    NULL "${PSP_EBOOT_UNKPNG}" "${PSP_EBOOT_PIC1}" \
    NULL ${PSP_PRX_NAME} NULL
