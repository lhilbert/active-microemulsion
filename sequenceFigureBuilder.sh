#!/usr/bin/env bash

#
# Script for automatically building figures of 3 channel sequences.
# Target directory containing the output sequences must be passed as first argument!
#
# Additional requirements/dependencies are:
# - Imagemagick ("convert" command)
#

EXPERIMENT_DIR="${1%/}"
EXPERIMENT_NAME=$(basename ${EXPERIMENT_DIR})
EXPERIMENT_NAME=${EXPERIMENT_NAME##Out}
EXPERIMENT_NAME=${EXPERIMENT_NAME##_}
EXPERIMENT_NAME=${EXPERIMENT_NAME:-microemulsion_testRun}
CHANNELS=( DNA RNA Transcription )
IMGFMT="microemulsion_%s_%d.pgm"
size=( $(identify -ping -format '%w %h\n' ${EXPERIMENT_DIR}/$(printf ${IMGFMT} ${CHANNELS[0]} 0)) )
WIDTH=${size[0]}
HEIGHT=${size[1]}

echo "Experiment: ${EXPERIMENT_NAME}"
echo "Image size: ${WIDTH}x${HEIGHT}"
#exit 1

#FIGURE_SEQ=( 1 8 17 37 58 100 ) # For cutoff @1/6
#FIGURE_SEQ=( 1 8 17 31 58 100 ) # For cutoff @1/6
#FIGURE_SEQ=( 1 12 25 44 62 100 ) # For cutoff @1/4
#FIGURE_SEQ=( 1 12 25 38 62 100 ) # For cutoff @1/4
#FIGURE_SEQ=( 0 16 33 50 66 83 100 ) # For cutoff @1/2
#FIGURE_SEQ=( 0 11 22 33 56 78 100 ) # For cutoff @1/3
#FIGURE_SEQ=( 0 1 25 100 ) # For simple phase separation simulations

FIGURE_SEQ=( 0 10 22 52 )

BLUR_PARAMS="0x0.5" # This should be tuned carefully

OUTDIR="${EXPERIMENT_DIR}/Figures"

function execute { # Log and execute the input command
    CMD="$@"
    echo ${CMD}
    ${CMD}
}

function expandParams { # Receiving a list of variable names to expand
    PARAMS=""
    PLIST="$@"
    for p in $PLIST; do
        PARAMS="${PARAMS}${!p} "
    done
    echo ${PARAMS}
}

mkdir -p ${OUTDIR}

for channel in ${CHANNELS[@]}; do
    echo "Building sequence for channel=${channel}"

    targetFigures=""
    for i in ${FIGURE_SEQ[@]}; do
        targetFigures="${targetFigures}$(printf ${EXPERIMENT_DIR}/${IMGFMT} ${channel} ${i}) "
    done
#    echo "Target figures: ${targetFigures}"
    sequenceName="${OUTDIR}/${EXPERIMENT_NAME}_${channel}.png"
    labeledSequenceName="${OUTDIR}/${EXPERIMENT_NAME}_${channel}_labeled.png"
    execute "montage ${targetFigures} -blur ${BLUR_PARAMS} -geometry +5+5 -tile ${#FIGURE_SEQ[@]}x1 -size 100x100 ${sequenceName}"
    width=$[((${WIDTH} + 10) * 4) * (${#FIGURE_SEQ[@]} + 1)]
    width=$[${width} + ((${WIDTH} + 10) * 2)]
    height=$[(${HEIGHT} + 10) * (4+1)]
    echo "New dimensions: ${width}x${height}"
    execute "convert ${sequenceName} -resize 400% -filter point -gravity east -extent $[${width} - 50]x${height} -gravity southwest -font Arial -pointsize 40 -annotate +25+127 ${channel} -flatten -gravity center -extent ${width}x${height} ${labeledSequenceName}"
#    # Now copy the figure into the common ExperimentsOut folder
#    cp ${out_dir}/Figure/${counter}_${item}*.png ${COMMON_OUTDIR}/
done
execute "montage ${OUTDIR}/*_labeled.png -geometry +0-20 -tile 1x3 -size 100x100 ${OUTDIR}/${EXPERIMENT_NAME}_${#CHANNELS[@]}Channels.png"

#eof
