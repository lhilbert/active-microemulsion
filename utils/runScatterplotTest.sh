#!/bin/bash

# Time expressed in minutes

ACTIVATION=10
TRX_RANGE=40

#RT_HOURS=( 14 38 38 )
RT_HOURS=( 38 38 38 )
#RT_HOURS=( 38 )
BASE_OPTS_NAMES=( "FinalRun_w025_25chains_Set1" "FinalRun_w025_25chains_Set2" "FinalRun_w025_25chains_Set3")
#BASE_OPTS_NAMES=( "FinalRun_w025_25chains_Relaxations" )
#BASE_OPTS_NAMES=( "TestNumChains" )
#BASE_OPTS=( "-W 100 -H 100 -w 0.15 -s 666 --no-sticky-boundary --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 -m -t 1 --flavopiridol 0" )
#BASE_OPTS=( "-W 100 -H 100 -w 0.15 -s 666 --no-sticky-boundary --kRnaPlus 2.7777e-3 --kRnaMinus 2.7777e-4 -m -t 1 --flavopiridol 0" )
#BASE_OPTS=( 
#    "-W 60 -H 60 -w 0.15 -s 666 --no-sticky-boundary --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#    "-W 60 -H 60 -w 0.25 -s 1810 --no-sticky-boundary --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#)
#BASE_OPTS=( "-W 60 -H 60 -w 0.5 -s 22055 --no-sticky-boundary --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" )
#BASE_OPTS=( "-W 60 -H 60 -w 0.25 -s 1810 --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" )
#BASE_OPTS=( "-W 60 -H 60 -w 0.5 -s 22055 --kRnaPlus 5.5555e-3 --kRnaMinus 5.5555e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" )
#BASE_OPTS=( 
#    "-W 200 -H 200 --chain-generator-n 25 -w 0.2 -s 666 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#    "-W 200 -H 200 -w 0.3 -s 1810 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#    "-W 200 -H 200 --chain-generator-n 25 -w 0.3 -s 1810 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#)
#BASE_OPTS=( 
#    "-W 200 -H 200 --chain-generator-n 25 -w 0.2 -s 666 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
#)
BASE_OPTS=( 
    "-W 200 -H 200 --chain-generator-n 25 -w 0.25 -s 1098 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
    "-W 200 -H 200 --chain-generator-n 25 -w 0.25 -s 1098 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
    "-W 200 -H 200 --chain-generator-n 25 -w 0.25 -s 1098 --kRnaPlus 8.33333e-2 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
)

###
BASE_EVT_OPTS="--activate ${ACTIVATION}"
#BASE_EVT_OPTS=""
###
SPECIFIC_OPTS=( "" "--flavopiridol <time>" "--actinomycin-D <time>" ) # One extra run with empty ones is performed anyway as a control
#SPECIFIC_OPTS=( "" )
#SPECIFIC_OPTS=( "" "--flavopiridol <time>" )
#SPECIFIC_OPTS=( "--actinomycin-D <time>" )
#SPECIFIC_OPTS=( "--flavopiridol <time>" )
###

REPEATS_START=0
#REPEATS_START=11
REPEATS=50
#REPEATS=30
STEP=1
#STEP=2

counter=0
for ((k = 0; k < ${#BASE_OPTS[@]}; k++)); do
currentBaseOpts="${BASE_OPTS[$k]}"
currentBOName="${BASE_OPTS_NAMES[$k]}"
currentRunTimeHours=${RT_HOURS[$k]}
for  ((i = 0; i < ${#SPECIFIC_OPTS[@]}; i++)); do
    for  ((j = ${REPEATS_START}; j < ${REPEATS}; j++)); do
        #time="$[(ACTIVATION + TRX_RANGE) * RANDOM / 32767]" # Picking a random transcription duration here
        time=$[j+STEP] # Here testing all the possible times
        spec="${SPECIFIC_OPTS[$i]}"
        activate="${BASE_EVT_OPTS}"
        if [[ ${time} -le ${ACTIVATION} ]]; then
            activate=""
        fi
        evt=$(echo ${spec} | sed s/"<time>"/"${time}"/g)
        if [[ "${spec}" ]]; then
            T=$[30+time]
        else
            T=${time}
        fi
        OPTS="${currentBaseOpts} ${activate} ${evt} -E ${T} -T ${T}"
        # CMD="qsub -l h_rss=512M,h_fsize=100M,h_cpu=36:00:00,hw=x86_64 sge-job.sh --sge-folder-name ScatterplotExperiment_${currentBOName} --chain-generator-I 0.4 ${OPTS}"
        CMD="qsub -l h_rss=1024M,h_fsize=512M,h_rt=${currentRunTimeHours}:00:00,hw=x86_64 sge-job.sh --sge-folder-name ScatterplotExperiment_${currentBOName} --chain-generator-I 0.4 ${OPTS}"
        echo ${CMD}
        ${CMD}
        counter=$[counter+1]
    done
done
done

echo "Total number of jobs spawn: ${counter}"

#qstat

#eof

