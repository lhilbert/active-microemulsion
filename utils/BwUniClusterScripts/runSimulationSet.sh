#!/bin/bash

# Email notifications
EMAIL_ADDRESS="foo@bar"
NOTIFY_EVENTS="bea" # b=begin, e=end, a=abort

# Time expressed in minutes

ACTIVATION=10
END_TIME=90

#INHIBITED_FRACTION="0.4" #Default value is 0.4

RT_HOURS=( 18 18 ) #max is 72h=3d
BASE_OPTS_NAMES=( "Run1_name" "Run2_name" )
BASE_OPTS=( 
    "--threads 1 -W 100 -H 100 --chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25 -w 0.25 -s 4500 --kRnaPlus 1.1111e-1 --kRnaMinusRbp 5.5555e-4 --kChromPlus 0.075 --kChromMinus 1.6666e-3 --kOn 1.852e-4 --kOff 1.6666e-3 --kRnaTransfer 5.5555e-4 --txn-spike-factor 100 -m -t 1 --additional-snapshots 10.1 10.2 10.3 10.4 10.5 10.6 10.7 10.8 10.9 11.25 11.5 11.75 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 --flavopiridol 0"
    "--threads 1 -W 100 -H 100 --chain-generator-sparse --chain-generator-I 0.6 --chain-generator-n 25 -w 0.25 -s 4500 --kRnaPlus 1.1111e-1 --kRnaMinusRbp 5.5555e-4 --kChromPlus 0.075 --kChromMinus 1.6666e-3 --kOn 1.852e-4 --kOff 1.6666e-3 --kRnaTransfer 5.5555e-4 --txn-spike-factor 100 -m -t 1 --additional-snapshots 10.1 10.2 10.3 10.4 10.5 10.6 10.7 10.8 10.9 11.25 11.5 11.75 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 --flavopiridol 0"
)

###
ACTIVATE_OPTS="--txn-spike ${ACTIVATION} --activate ${ACTIVATION}.99" # This is a hack to do a one-minute txn spike at the activation time.
#ACTIVATE_OPTS="--activate ${ACTIVATION}"
#ACTIVATE_OPTS=""
###
NICKNAMES=( "relaxation" "control" "actinomycinD" "flavopiridol" ) # Nicknames to be used for job name
SPECIFIC_OPTS=( "<relaxation>" "" "--actinomycin-D <time>" "--flavopiridol <time>" ) # Relaxation must always be first!
###

#### End of settings

RELAXATION_REPEATS=30 # Must be divisible by 2
STEP=1
#STEP=2
SHIFT=1 # This must be > 0 !!!

#SIM_PER_JOB=10 # Must divide RELAXATION_REPEATS/STEP and END_TIME/STEP and be < maxPPN
SIM_PER_JOB=15 # Must divide RELAXATION_REPEATS/STEP and END_TIME/STEP and be < maxPPN

JOBS_DIR="Jobs"
mkdir -p ${JOBS_DIR}

initialRelaxDone=false
counter=0
for ((k = 0; k < ${#BASE_OPTS[@]}; k++)); do
currentBaseOpts="${BASE_OPTS[$k]}"
currentBOName="${BASE_OPTS_NAMES[$k]}"
currentRunTimeHours=${RT_HOURS[$k]}
for  ((i = 0; i < ${#SPECIFIC_OPTS[@]}; i++)); do
    for  ((J = 0; J < ${END_TIME}; J+=$[STEP*SIM_PER_JOB])); do
    endJ=$[J+(STEP*SIM_PER_JOB)]
    JOBFILE="${JOBS_DIR}/joblist.$(date +%s.%N).txt"
    if [[ -f ${JOBFILE} ]]; then
    rm ${JOBFILE}
    fi
    touch ${JOBFILE}
    for  ((j = ${J}; j < ${endJ}; j+=1)); do
        time=$[j+SHIFT] # Here testing all the possible times
        spec="${SPECIFIC_OPTS[$i]}"
        activate="${ACTIVATE_OPTS}"
    if [[ "${spec}" == "<relaxation>" ]]; then # Relaxations
        if [[ ${time} -gt ${RELAXATION_REPEATS} ]]; then
        break;
            fi
        activate=""
        evt=""
    else                       # Normal case
            if [[ ${time} -le ${ACTIVATION} ]]; then
                activate=""
            fi
            evt=$(echo ${spec} | sed s/"<time>"/"${time}"/g)
    fi
    # Do inital relaxations 1-10 just once!
    if [[ ${time} -le ${ACTIVATION} && ${initialRelaxDone} == true ]]; then
        echo "Skipping redundant initial relaxations..."
            continue # This is to avoid redoing 1-10 relaxations!
        fi
    if [[ ${initialRelaxDone} != true  && ${time} -gt ${ACTIVATION} ]]; then
        echo "Finished initial relaxations..."
        initialRelaxDone=true
    fi
    # Manage end time
        if [[ "${evt}" ]]; then
        if [[ ${time} -gt $[END_TIME-30] ]]; then
        break;
            fi
            T=$[30+time]
        else
            T=${time}
        fi
        OPTS="${currentBaseOpts} ${activate} ${evt} -E ${T} -T ${T}"
    TASK="hostname; ./moab-job.sh --job-folder-name SimSet_${currentBOName} ${OPTS}"
    echo ${TASK} >> ${JOBFILE}
    done
    if [[ ! -s ${JOBFILE} ]]; then
        #echo "Skipping empty JOBFILE=${JOBFILE}" #Debug
        rm ${JOBFILE}
    else
        echo "" >> ${JOBFILE}
            chmod +x ${JOBFILE}
        CMD="msub -m bea -M ${EMAIL_ADDRESS} -q singlenode -N ${NICKNAMES[i]} -l pmem=1024mb,nodes=1:ppn=${SIM_PER_JOB},walltime=${currentRunTimeHours}:00:00 -v SCRIPT_FLAGS=${JOBFILE} parbatch-wrapper.sh"
            #echo "k=$k; i=$i; J=$J" #debug
        echo ${CMD}
        cat ${JOBFILE}
            ${CMD}
            counter=$[counter+1]
    fi
    done
done
done

echo "Total number of jobs spawn: ${counter}"

#qstat

#eof

