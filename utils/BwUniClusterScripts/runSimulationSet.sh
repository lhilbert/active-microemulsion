#!/bin/bash

# Email notifications
EMAIL_ADDRESS="foo@bar"
NOTIFY_EVENTS="bea" # b=begin, e=end, a=abort

# Time expressed in minutes

ACTIVATION=10
TRX_RANGE=40

#RT_HOURS=( 14 38 38 )
RT_HOURS=( 42 ) #max is 72h=3d

INHIBITED_FRACTION="0.4" #Default value is 0.4

BASE_OPTS_NAMES=( "SparserInitTest" )

BASE_OPTS=( 
    "--threads 1 -W 200 -H 200 --chain-generator-sparse --chain-generator-I ${INHIBITED_FRACTION} --chain-generator-n 25 -w 0.25 -s 3000 --kRnaPlus 4.16666e-2 --kRnaMinusRbp 4.16666e-4 --kChromPlus 4.4444e-3 --kChromMinus 1.1111e-3 -m -t 1 --flavopiridol 0" 
)

###
ACTIVATE_OPTS="--txn-spike ${ACTIVATION} --activate ${ACTIVATION}.99" # This is a hack to do a one-minute txn spike at the activation time.
#ACTIVATE_OPTS="--activate ${ACTIVATION}"
#ACTIVATE_OPTS=""
###
#SPECIFIC_OPTS=( "" "--flavopiridol <time>" "--actinomycin-D <time>" "<relaxation>" ) # One extra run with empty ones is performed anyway as a control
SPECIFIC_OPTS=( "<relaxation>" "" "--actinomycin-D <time>" "--flavopiridol <time>" ) # Relaxation must always be first!
#SPECIFIC_OPTS=( "" )
#SPECIFIC_OPTS=( "<relaxation>" )
#SPECIFIC_OPTS=( "" "--flavopiridol <time>" )
#SPECIFIC_OPTS=( "--actinomycin-D <time>" )
#SPECIFIC_OPTS=( "--flavopiridol <time>" )
###

NICKNAMES=( "relaxation" "control" "actinomycinD" "flavopiridol" ) # Nicknames to be used for job name
#NICKNAMES=( "actinomycinD" ) # Nicknames to be used for job name

REPEATS_START=0
#REPEATS_START=11
REPEATS=50
#REPEATS=30
RELAXATION_REPEATS=30 # Must be divisible by 2
STEP=1
#STEP=2
SHIFT=1 # This must be > 0 !!!

SIM_PER_JOB=10 # Must divide RELAXATION_REPEATS/STEP and be < maxPPN

JOBS_DIR="Jobs"
mkdir -p ${JOBS_DIR}

initialRelaxDone=false
counter=0
for ((k = 0; k < ${#BASE_OPTS[@]}; k++)); do
currentBaseOpts="${BASE_OPTS[$k]}"
currentBOName="${BASE_OPTS_NAMES[$k]}"
currentRunTimeHours=${RT_HOURS[$k]}
for  ((i = 0; i < ${#SPECIFIC_OPTS[@]}; i++)); do
    for  ((J = ${REPEATS_START}; J < ${REPEATS}; J+=$[STEP*SIM_PER_JOB])); do
        #time="$[(ACTIVATION + TRX_RANGE) * RANDOM / 32767]" # Picking a random transcription duration here
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
	TASK="hostname; ./moab-job.sh --job-folder-name SimSet_${currentBOName} ${OPTS}"
	echo ${TASK} >> ${JOBFILE}
    done
	if [[ ! -s ${JOBFILE} ]]; then
	    #echo "Skipping empty JOBFILE=${JOBFILE}" #Debug
	    rm ${JOBFILE}
	else
	    echo "" >> ${JOBFILE}
            chmod +x ${JOBFILE}
	    CMD="msub -m ${NOTIFY_EVENTS} -M ${EMAIL_ADDRESS} -q singlenode -N ${NICKNAMES[i]} -l pmem=1024mb,nodes=1:ppn=${SIM_PER_JOB},walltime=${currentRunTimeHours}:00:00 -v SCRIPT_FLAGS=${JOBFILE} parbatch-wrapper.sh"
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

