#!/bin/bash

## check if $SCRIPT_FLAGS is "set"
if [ -n "${SCRIPT_FLAGS}" ] ; then
   ## but if positional parameters are already present
   ## we are going to ignore $SCRIPT_FLAGS
   if [ -z "${*}"  ] ; then
      set -- ${SCRIPT_FLAGS}
   fi
fi

module load system/parbatch

parbatch ${@}

