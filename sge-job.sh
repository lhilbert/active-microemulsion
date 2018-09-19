#!/bin/bash -i
#$ -S /bin/bash
#
# MPI-PKS script for job submission script with ’qsub’.
# Syntax is Bash with special qsub-instructions that begin with ’#$’.
# For more detailed documentation, see
#     https://start.pks.mpg.de/dokuwiki/doku.php/getting-started:queueing_system
#

# Active microemulsion command line parameters are passed from outside
# Schedule this with:
# qsub sge-job.sh <active-microemulsion_arguments>
CONFIG_FLAGS="$@"

# OutFolder naming filter from flags
flagsToNameFilter()
{
    sed s/" -\| --"/"_"/g \
    | sed s,"_P \([^ ]\+/\|\)\([^ ]\+\)\.[^ ]\+","_P\2",g \
    | sed s/"[.]\|^-"/""/g \
    | sed s,"_[a-zA-Z] [^ ]\+/[^ ]\+","",g \
    | sed s/" "/""/g \
    | sed s/"_\(flavopiridol\|actinomycin-D\|activate\)0_"/"_"/g
}

timestamp="$(date +%Y%m%d_%H%M%S)"

OUT_DIR="Out_$(echo ${CONFIG_FLAGS} | flagsToNameFilter)_sge_${timestamp}_$$"

# Settings for config data and shared libraries
REPO_BASE_DIR=${HOME}/Repo/active-microemulsion
REPO_ITEMS_TO_COPY=( "active-microemulsion" "cmake-build-*" "lib" "ChainConfigs" "sequenceFigureBuilder.sh" "utils" ) # Will be copied with "cp -r"
DEST_DIR="${REPO_BASE_DIR}/SgeOut/RNADepletionRateTests/${OUT_DIR}"

# Creating the destination folder
mkdir -p ${DEST_DIR}

# --- Mandatory qsub arguments
# Hardware requirements.
#$ -l h_rss=256M,h_fsize=100M,h_cpu=24:00:00,hw=x86_64

# --- Optional qsub arguments
# Change working directory - your job will be run from the directory
# that you call qsub in.  So stdout and stderr will end up there.
#$ -cwd
# #$ -o ${DEST_DIR}/
# #$ -e ${DEST_DIR}/
#
# Split stdout and stderr to 2 files
#$ -j n
#
# Job name - purely cosmetic (changes name in ‘qstat‘).
#$ -N active-microemulsion
#
# Specific queue where to schedule the job
# #$ -q <queue_name>
# #$ -q anubis
#

################
#
# copying your data to /scratch
#
################

# create local folder on ComputeNode
scratch=/scratch/${USER}/sge_$$
mkdir -p ${scratch}

# copy all your NEEDED data to ComputeNode
for item in ${REPO_ITEMS_TO_COPY[@]}; do
    CMD="cp -r ${REPO_BASE_DIR}/${item} ${scratch}/."
    echo ${CMD}
    ${CMD}
done

# Entering scratch directory
pushd ${scratch}
# dont access /home after this line

# if needed load modules here
#module load <module_name>

# if needed add export variables here

# Run the simulation
echo "Running on $(hostname)"
echo "We are in $(pwd)"
${scratch}/active-microemulsion ${CONFIG_FLAGS} -o ${OUT_DIR}

# copy results to data
#cp -r . /data/<my_dir>
cp -r ${OUT_DIR}/* ${DEST_DIR}/.

# Return to initial folder
popd

# clean up scratch
rm -rf ${scratch}
unset scratch

exit 0

#EOF

