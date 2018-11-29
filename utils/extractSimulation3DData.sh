#!/bin/bash

# Simple wrapper:
# [mandatory] $1 = target run directory, which must contain all the Out_* directories.
# [optional ] $2 = a quoted string containing all extra opts to pass down to the "covRnaTxn3dAnalysis.py" script
#

if [[ $# -lt 1 || $# -gt 2 ]]; then
	# Wrong number of parameters
	echo "ERROR: You need to pass one mandatory argument (target directory) and one optional argument (extra parameters for underlying script)."
	exit 1
else
	targetDir="$1"
	extraArgs="-s"
	if [[ "$2" != "" ]]; then
		extraArgs+=" $2"
	fi
fi

utilsDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
analyze="${utilsDir}/covRnaTxn3dAnalysis.py ${extraArgs}"

csvBasename=$(basename ${targetDir})

pushd ${targetDir}

controlDirs="$(ls | grep -v "activate10" | grep "E[34][0-9]") Out_*m_t1_E* Out_*m_t1_activate10_E*"
flavopiridolDirs="$(ls | grep -v "activate10" | grep "E[34][0-9]") Out_*m_t1*flavopiridol*"
actinomycinDDirs="$(ls | grep -v "activate10" | grep "E[34][0-9]") Out_*_m_t1_E* Out_*m_t1*actinomycin-D*"

for f in ${controlDirs}; do
	[[ -d "$f" ]] && ${analyze} -S ${controlDirs} -d ${csvBasename}_control.csv && sed -i s/"SnapshotNumber,X,Y,Z"/"SnapshotNumber,TXN,RNA,CoV(DNA)"/ ${csvBasename}_control.csv
	break
done
for f in ${flavopiridolDirs}; do
	[[ -d "$f" ]] && ${analyze} -S ${flavopiridolDirs} -d ${csvBasename}_flavopiridol.csv && sed -i s/"SnapshotNumber,X,Y,Z"/"SnapshotNumber,TXN,RNA,CoV(DNA)"/ ${csvBasename}_flavopiridol.csv
	break
done
for f in ${actinomycinDDirs}; do
	[[ -d "$f" ]] && ${analyze} -S ${actinomycinDDirs} -d ${csvBasename}_actinomycin-D.csv && sed -i s/"SnapshotNumber,X,Y,Z"/"SnapshotNumber,TXN,RNA,CoV(DNA)"/ ${csvBasename}_actinomycin-D.csv
	break
done
popd

#eof

