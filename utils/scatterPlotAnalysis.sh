#!/usr/bin/env bash

UTILS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )" # Get the script's directory (from https://stackoverflow.com/a/246128)
OUT_DIR="AnalysisOut"

SCRIPTS=( "covRnaAnalysis.py" "trxRnaAnalysis.py" )
FILTERS=( "$(ls -1 . | grep -v "flavopiridol\|actinomycin\|\.csv$\|\.svg$\|AnalysisOut")" '*flavopiridol*' '*actinomycin-D*' )
FILTER_NAMES=( 'control' 'flavopiridol' 'actinomycin-D' )

getScriptBasename ()
{
    sed s/"Analysis\.py$"/""/
}

EXTRA_OPTS="$@"

mkdir -p ${OUT_DIR}

for analysis in ${SCRIPTS[@]}; do
    for  ((i = 0; i < ${#FILTERS[@]}; i++)); do
        scriptName=$(echo ${analysis} | getScriptBasename)
        CMD="${UTILS_DIR}/${analysis} -S ${FILTERS[$i]} -p ${OUT_DIR}/${scriptName}_${FILTER_NAMES[$i]}.svg -d ${OUT_DIR}/${scriptName}_${FILTER_NAMES[$i]}.csv ${EXTRA_OPTS}"
        echo ${CMD}
        ${CMD}
    done
done

#eof
