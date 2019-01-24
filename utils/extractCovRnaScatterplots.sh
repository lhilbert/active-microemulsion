#!/bin/bash

# Simple wrapper:
# [mandatory] $1 = target run directory, which must contain all the Out_* directories.
# [optional ] $2 = a quoted string containing all extra opts to pass down to the "covRnaAnalysis.py" script
#

EXT="png"
#EXT="svg"

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
analyze="${utilsDir}/covRnaAnalysis.py ${extraArgs}"

plotBasename="$(basename ${targetDir})_CovRna"

pushd ${targetDir}

controlDirs="Out_*m_t1_E[0-9]_* Out_*m_t1_E10_* Out_*m_t1*_activate10_E* Out_*m_t1*_activate1099_E*"
flavopiridolDirs="Out_*m_t1_E[0-9]_* Out_*m_t1_E10_* Out_*m_t1*flavopiridol*"
actinomycinDDirs="$(ls | grep -v "activate10" | grep "E[34][0-9]") Out_*_m_t1_E* Out_*m_t1*actinomycin-D*"

for f in ${controlDirs}; do
	[[ -d "$f" ]] && ${analyze} -S ${controlDirs} -p ${plotBasename}_control.${EXT} && break
done
for f in ${flavopiridolDirs}; do
#    echo Testing ${f} #debug
	[[ -d "$f" ]] && ${analyze} -S ${flavopiridolDirs} -p ${plotBasename}_flavopiridol.${EXT} && break
done
for f in ${actinomycinDDirs}; do
	[[ -d "$f" ]] && ${analyze} -S ${actinomycinDDirs} -p ${plotBasename}_actinomycin-D.${EXT} && break
done
popd

#eof

