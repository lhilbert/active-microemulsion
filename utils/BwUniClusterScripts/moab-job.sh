#!/bin/bash -i

#MSUB -S /bin/bash

#
# bwUniCluster script for job submission script with ’msub’.
# Syntax is Bash with special msub-instructions that begin with ’#MSUB ’.
# For more detailed documentation, see
#     http://www.bwhpc-c5.de/wiki/index.php/Batch_Jobs
#     http://www.bwhpc-c5.de/wiki/index.php/Batch_Jobs_-_bwUniCluster_Features
#

# Active microemulsion command line parameters are passed from outside
# Schedule this with:
# msub moab-job.sh <active-microemulsion_arguments>


### Define some functions

# OutFolder naming filter from flags
flagsToNameFilter()
{
    sed s/" -\| --"/"_"/g \
    | sed s,"_P \([^ ]\+/\|\)\([^ ]\+\)\.[^ ]\+","_P\2",g \
    | sed s/"[.]\|^-"/""/g \
    | sed s,"_[a-zA-Z] [^ ]\+/[^ ]\+","",g \
    | sed s/" "/""/g \
    | sed s/"_\(flavopiridol\|actinomycin-D\|activate\)0_"/"_"/g \
    | sed s/"-threads[0-9]\+_"/""/ \
    | sed s/"_additional-snapshots[0-9]\+_"/"_"/
}

# Extract the chain config used, if any was passed
getChainConfig()
{
    grep -o "\-P [^ ]\+" \
    | sed s/"-P "/""/
}
# Extract the resolution config used
getResolutionConfig()
{
    grep -o "\-W [0-9]\+ \-H [0-9]\+\|\-H [0-9]\+ \-W [0-9]\+"
}

### Start run

timestamp="$(date +%Y%m%d_%H%M%S)"

# Default vars
SGE_FOLDER="DefaultOut"
CHAIN_GEN_I="0.4"
CHAIN_GEN_n=""
CHAIN_GEN_s=""
REPO_BASE_DIR=${HOME}/Repo/active-microemulsion # This is overridden by the "--repo-base-dir" cmdline flag

# Capture the raw input args
RAW_CMDLINE="$@"
# Log input args
echo Input args: ${RAW_CMDLINE}

# Parse and extract special command line arguments not to be passed downstream -- Thanks https://stackoverflow.com/a/14203146 !!! :)
POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    --repo-base-dir)
    REPO_BASE_DIR="$2"
    shift
    shift
    ;;
    --chain-generator-n)
    CHAIN_GEN_n="$2"
    shift
    shift
    ;;
    --chain-generator-I)
    CHAIN_GEN_I="$2"
    shift
    shift
    ;;
    --chain-generator-s|--chain-generator-sparse)
    CHAIN_GEN_s="--sparse"
    shift
    ;;
    -O|--job-folder-name)
    SGE_FOLDER="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

CONFIG_FLAGS="${POSITIONAL[@]}"
echo Config flags: ${CONFIG_FLAGS} #debug
#

OUT_DIR="Out_$(echo ${CONFIG_FLAGS} | flagsToNameFilter)_$$"
CHAIN_CONF_FILE="$(echo ${CONFIG_FLAGS} | getChainConfig)"
CHAIN_GEN_ARGS="-C 0.5 -I ${CHAIN_GEN_I} ${CHAIN_GEN_s}" # Number of chains is automatically set below according to grid size
#CHAIN_GEN_ARGS="-C 0.5 -I 0.2" # Number of chains is automatically set below according to grid size
RESOLUTION="$(echo ${CONFIG_FLAGS} | getResolutionConfig)"

# Settings for config data and shared libraries
REPO_ITEMS_TO_COPY=( "active-microemulsion" "cmake-build-*" "lib" "ChainConfigs" "utils" ) # Will be copied with "cp -r"

# Destination directory
# todo: Make the script read an -o <dir> flag to
DEST_DIR="${REPO_BASE_DIR}/MoabOut/${SGE_FOLDER}/${OUT_DIR}"

# Creating the destination folder
mkdir -p ${DEST_DIR}

# --- Mandatory qsub arguments
# Hardware requirements.
# #$ -l h_rss=512M,h_fsize=100M,h_cpu=36:00:00,hw=x86_64

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
# copying your data to scratch folder
#
################

# create local folder on ComputeNode
#scratch=/scratch/${USER}/sge_$$
#scratch=${DEST_DIR}/scratch
#scratch=${WORK}/scratch/job_$$
scratch=${TMP}/${USER}/job_$$
mkdir -p ${scratch}

# copy all your NEEDED data to ComputeNode
for item in ${REPO_ITEMS_TO_COPY[@]}; do
    CMD="cp -r ${REPO_BASE_DIR}/${item} ${scratch}/."
    echo ${CMD}
    ${CMD}
done

# Entering scratch directory
pushd ${scratch}
mkdir -p ${OUT_DIR}
# dont access /home after this line
echo "Timestamp: ${timestamp}" | tee -a ${OUT_DIR}/metadata.txt
echo "Hostname: $(hostname)" | tee -a ${OUT_DIR}/metadata.txt
echo "CWD: $(pwd)" | tee -a ${OUT_DIR}/metadata.txt
echo "CMD: ${RAW_CMDLINE}" | tee -a ${OUT_DIR}/metadata.txt

# if needed load modules here
#module load <module_name>
#module load devel/python/3.5.2
module load devel/python numlib/python_numpy
module load lib/boost

# If needed, generate chains on-the-fly
if [[ "${CHAIN_CONF_FILE}" == "" ]]; then
    if [[ "${RESOLUTION}" == "" ]]; then
        RESOLUTION="-W 50 -H 50"
    fi
    numchains="-n 25"
    echo ChainGen_n: ${CHAIN_GEN_n} #debug
    if [[ "${CHAIN_GEN_n}" ]]; then
	echo Setting custom number of chains...
	numchains="-n ${CHAIN_GEN_n}"
    else
        if [[ "${RESOLUTION}" == "-W 60 -H 60" ]]; then
            numchains="-n 9"
        fi
        if [[ "${RESOLUTION}" == "-W 200 -H 200" ]]; then
            numchains="-n 100"
        fi
    fi
    CHAIN_CONF_FILE="./generated.chains"
    echo "Generating chains..."
    CMD="python utils/chainConfigurator.py ${RESOLUTION} ${numchains} ${CHAIN_GEN_ARGS} ${CHAIN_CONF_FILE}"
    echo ${CMD} | tee -a ${OUT_DIR}/metadata.txt
    ${CMD}
    CONFIG_FLAGS="${CONFIG_FLAGS} -P ${CHAIN_CONF_FILE}"
    cp ${CHAIN_CONF_FILE} ${OUT_DIR}/.
fi

# Run the simulation
echo "Running simulation..."
CMD="${scratch}/active-microemulsion ${CONFIG_FLAGS} -o ${OUT_DIR}"
echo ${CMD} | tee -a ${OUT_DIR}/metadata.txt
${CMD}

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

