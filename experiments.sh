#!/usr/bin/env bash

#
# Script for automatically running experiments with active-microemulsion
# and for getting also nice figures out of them.
#
# Additional requirements/dependencies are:
# - Imagemagick ("convert" command)
#
EXPERIMENT_NAME="chainIntegrity_orthogonalChains_centered"
END_TIME="1e3"

OUTIMG_NAMEFMT="microemulsion_%d.pgm"

# Format is used by printf, so use the same type of format specifiers.
FORMAT="./Out_v.%s.%s_w%s_T%s"
PARAM_1_version="0.2a"
PARAM_2_expName="${EXPERIMENT_NAME}"
PARAM_4_endTime="${END_TIME}"

OMEGAS="-0.50 -0.25 0.00 0.25 0.50"
FIGURE_SEQ="0 1 25 100"

function expandParams { # Receiving a list of variable names to expand
    PARAMS=""
    PLIST="$@"
    for p in $PLIST; do
        PARAMS="${PARAMS}${!p} "
    done
    echo ${PARAMS}
}

for omega in ${OMEGAS}; do
    PARAM_3_omega="${omega}"
    echo "Running experiment with omega=${omega}"
    PARAMS=$(expandParams ${!PARAM_*})
#    echo ${PARAMS} #debug
#    exit 1
    out_dir=$(printf ${FORMAT} ${PARAMS})
    CMD="./active-microemulsion -T ${END_TIME} -w ${omega} --output-dir ${out_dir}"
    echo ${CMD}
    ${CMD}
    mkdir -p ${out_dir}/Figure
    targetFigures=""
    for i in ${FIGURE_SEQ}; do
        targetFigures="${targetFigures}$(printf ${out_dir}/${OUTIMG_NAMEFMT} ${i}) "
    done
    CMD="montage ${targetFigures} -geometry +5+5 -tile 4x1 -size 100x100 ${out_dir}/Figure/figure.png"
    echo ${CMD}
    ${CMD}
done


#eof
