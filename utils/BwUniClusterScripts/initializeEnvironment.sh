#!/usr/bin/env bash

# This script has to be run from the repository main folder
# It links all the scripts which are necessary for scheduling into the main folder

targetDir="utils/BwUniClusterScripts"
targetExecs=('parbatch-wrapper.sh' 'moab-job.sh' 'runSimulationSet.py')
targetLibs=('schedulingLib.py')

###
all=("${targetExecs[@]}" "${targetLibs[@]}")

if [[ ! -d ${targetDir} ]]; then
    echo "Cannot find ${targetDir}, make sure to be in the repo main directory!" 1>&2
    exit 1
fi

# Set exec permissions on execs
for item in ${targetExecs[@]}; do
    chmod +x ${targetDir}/${item}
done

# Link all to base directory
for item in ${all[@]}; do
    ln -s ${targetDir}/${item} ./
done

# Load python3 module
module load devel/python/3.5.2
