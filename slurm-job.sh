#!/usr/bin/env bash

# Active microemulsion command line parameters
CONFIG_FLAGS="-T 18e3 -c 3 -w 0.25 -s 333 --no-sticky-boundary -P ChainConfigs/testConfig.chains"

# OutFolder naming filter from flags
flagsToNameFilter()
{
    sed s/" -\| --"/"_"/g \
    | sed s,"_P \([^ ]\+/\|\)\([^ ]\+\)\.[^ ]\+","_P\2",g \
    | sed s/"[.]\|^-"/""/g \
    | sed s,"_[a-zA-Z] [^ ]\+/[^ ]\+","",g \
    | sed s/" "/""/g
}
OUT_DIR="Out_$(echo ${CONFIG_FLAGS} | flagsToNameFilter)_slurm_${SLURM_JOB_ID}"

# Settings for config data and shared libraries
REPO_BASE_DIR=${HOME}/Repo/active-microemulsion
REPO_ITEMS_TO_COPY=( "active-microemulsion" "cmake-build-*" "lib" "ChainConfigs" "sequenceFigureBuilder.sh" ) # Will be copied with "cp -r"

################
#
# Setting slurm options
#
################

# lines starting with "#SBATCH" define your jobs parameters

# requesting the type of node on which to run job
##SBATCH --partition <patition name>

# telling slurm how many instances of this job to spawn (typically 1)
#SBATCH --ntasks 1

# setting number of CPUs per task (1 for serial jobs)
#SBATCH --cpus-per-task 1

# setting memory requirements (in MB)
#SBATCH --mem-per-cpu 512

# propagating max time for job to run
##SBATCH --time <days-hours:minute:seconds>
##SBATCH --time <hours:minute:seconds>
##SBATCH --time <minutes>
#SBATCH --time 60

# Setting the name for the job
#SBATCH --job-name active-microemulsion

# setting notifications for job
# accepted values are ALL, BEGIN, END, FAIL, REQUEUE
#SBATCH --mail-type ALL

# telling slurm where to write output and error
##SBATCH --output <path>-%j.out
##SBATCH --error <path>-%j.out

################
#
# copying your data to /scratch
#
################

# create local folder on ComputeNode
scratch=/scratch/${USER}/slurm_${SLURM_JOB_ID}
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

################
#
# run the program
#
################
echo "Running on $(hostname)"
echo "We are in $(pwd)"
${scratch}/active-microemulsion ${CONFIG_FLAGS} -o ${OUT_DIR}

# copy results to data
#cp -r . /data/<my_dir>
cp -r ${OUT_DIR} ${REPO_BASE_DIR}/.

# Return to initial folder
popd

# clean up scratch
rm -rf ${scratch}
unset scratch

exit 0

#EOF
