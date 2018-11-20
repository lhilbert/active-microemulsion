#!/bin/bash

# Time expressed in minutes

ACTIVATION=10
TRX_RANGE=40

#RT_HOURS=( 14 38 38 )
RT_HOURS=( 50 )
#RT_HOURS=( 38 )
BASE_OPTS_NAMES=( "ExtraRun2_w025_25chains_s3k_rnaBalanced")
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
    "-W 200 -H 200 --chain-generator-n 25 -w 0.25 -s 3000 --kRnaPlus 4.16666e-2 --kRnaMinusRbp 4.16666e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
)

###
ACTIVATE_OPTS="--activate ${ACTIVATION}"
#ACTIVATE_OPTS=""
###
SPECIFIC_OPTS=( "" "--flavopiridol <time>" "--actinomycin-D <time>" "<relaxation>" ) # One extra run with empty ones is performed anyway as a control
#SPECIFIC_OPTS=( "" )
#SPECIFIC_OPTS=( "<relaxation>" )
#SPECIFIC_OPTS=( "" "--flavopiridol <time>" )
#SPECIFIC_OPTS=( "--actinomycin-D <time>" )
#SPECIFIC_OPTS=( "--flavopiridol <time>" )
###

REPEATS_START=0
#REPEATS_START=11
REPEATS=50
#REPEATS=30
RELAXATION_REPEATS=30
STEP=1
#STEP=2
SHIFT=1 # This must be > 0 !!!

initialRelaxDone=false
counter=0
for ((k = 0; k < ${#BASE_OPTS[@]}; k++)); do
currentBaseOpts="${BASE_OPTS[$k]}"
currentBOName="${BASE_OPTS_NAMES[$k]}"
currentRunTimeHours=${RT_HOURS[$k]}
for  ((i = 0; i < ${#SPECIFIC_OPTS[@]}; i++)); do
    for  ((j = ${REPEATS_START}; j < ${REPEATS}; j+=${STEP})); do
        #time="$[(ACTIVATION + TRX_RANGE) * RANDOM / 32767]" # Picking a random transcription duration here
        time=$[j+SHIFT] # Here testing all the possible times
        spec="${SPECIFIC_OPTS[$i]}"
        activate="${ACTIVATE_OPTS}"
	if [[ "${spec}" == "<relaxation>" ]]; then # Relaxations
	    if [[ ${time} -gt ${RELAXATION_REPEATS} ]]; then
		break;
            fi
	    activate=""
	    evt=""
	else 					   # Normal case
            if [[ ${time} -le ${ACTIVATION} ]]; then
                activate=""
            fi
            evt=$(echo ${spec} | sed s/"<time>"/"${time}"/g)
	fi
	# Do inital relaxations 1-10 just once!
	if [[ ${time} -le ${ACTIVATION} && ${initialRelaxDone} == true ]]; then
            continue # This is to avoid redoing 1-10 relaxations!
        fi
	if [[ ${initialRelaxDone} != true  && ${time} -gt ${ACTIVATION} ]]; then
	    initialRelaxDone=true
	fi
	# Manage end time
        if [[ "${evt}" ]]; then
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

