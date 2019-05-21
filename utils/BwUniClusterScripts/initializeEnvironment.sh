#!/usr/bin/env bash

# This script has to be run from the repository main folder
# It links all the scripts which are necessary for scheduling into the main folder

targetDir="utils/BwUniClusterScripts"
targets=('parbatch-wrapper.sh' 'moab-job.sh' \
        'runSimulationSet.py' 'schedulingLib.py' \
        'initializeEnvironment.sh')

###

if [[ ! -d ${targetDir} ]]; then
    echo "Cannot find ${targetDir}, make sure to be in the repo main directory!" 1>&2
    exit 1
fi

# Link all to base directory
echo "Linking scripts..."
for item in ${targets[@]}; do
    [[ ! -e ${item} ]] && ln -s ${targetDir}/${item} ./
done

# Load python3 module
echo "Loading python3 module..."
module load devel/python/3.5.2
