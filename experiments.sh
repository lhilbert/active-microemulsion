#!/usr/bin/env bash

#
# Script for automatically running experiments with active-microemulsion
# and for getting also nice figures out of them.
#
# Use the "--no-sim" command line option to not perform simulations and just
# building the figures.
#
# Additional requirements/dependencies are:
# - Imagemagick ("convert" command)
#
EXPERIMENT_NAME="chainIntegrity_orthogonalChains_centered"
END_TIME="1e3"

OUTIMG_NAMEFMT="microemulsion_%d.pgm"
COMMON_OUTDIR="ExperimentsOut"

# Format is used by printf, so use the same type of format specifiers.
OUTDIR_BASE="./Out_"
FORMAT="v.%s.%s_w%s_T%s"
PARAM_1_version="0.2a"
PARAM_2_expName="${EXPERIMENT_NAME}"
PARAM_4_endTime="${END_TIME}"

OMEGAS="-0.50 -0.25 0.00 0.25 0.50" # Must consist of exactly 5 items #todo: remove this constraint
FIGURE_SEQ="0 1 25 100" # Must consist of exactly 4 items #todo: remove this constraint

SIMULATE="true"
if [[ "${1}" == "--no-sim" ]]; then
    SIMULATE="false"
fi

function execute { # Print and execute the input command in a subshell (and return output)
    CMD="$@"
    echo ${CMD}
    echo $(${CMD})
}

function expandParams { # Receiving a list of variable names to expand
    PARAMS=""
    PLIST="$@"
    for p in $PLIST; do
        PARAMS="${PARAMS}${!p} "
    done
    echo ${PARAMS}
}

mkdir -p ${COMMON_OUTDIR}
counter=0
for omega in ${OMEGAS}; do
    PARAM_3_omega="${omega}"
    echo "Running experiment with omega=${omega}"
    PARAMS=$(expandParams ${!PARAM_*})
    item=$(printf ${FORMAT} ${PARAMS})
    out_dir="${OUTDIR_BASE}${item}"
    if [[ "${SIMULATE}" == "true" ]]; then
        execute "./active-microemulsion -T ${END_TIME} -w ${omega} --output-dir ${out_dir}"
    fi
    mkdir -p ${out_dir}/Figure
    targetFigures=""
    for i in ${FIGURE_SEQ}; do
        targetFigures="${targetFigures}$(printf ${out_dir}/${OUTIMG_NAMEFMT} ${i}) "
    done
    execute "montage ${targetFigures} -geometry +5+5 -tile 4x1 -size 100x100 ${out_dir}/Figure/${counter}_${item}.png"
    execute "convert ${out_dir}/Figure/${counter}_${item}.png -resize 400% -filter point -gravity east -extent 1150x300 -gravity southwest -font Arial -pointsize 40 -annotate +25+127 w=${omega} -flatten -gravity center -extent 1200x300 ${out_dir}/Figure/${counter}_${item}_labeled.png"
    # Now copy the figure into the common ExperimentsOut folder
    cp ${out_dir}/Figure/${counter}_${item}*.png ${COMMON_OUTDIR}/
    counter=$[counter+1]
done
PARAM_3_omega="Range"
PARAMS=$(expandParams ${!PARAM_*})
item=$(printf ${FORMAT} ${PARAMS})
execute "montage ${COMMON_OUTDIR}/*_labeled.png -geometry +0-20 -tile 1x5 -size 100x100 ${COMMON_OUTDIR}/${item}_mergedFigure.png"

#eof
