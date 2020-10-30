#!/bin/bash
# set -x 

# TODO: Switch between the following two to disable/enable VTUNE profiling
VTUNE="vtune -collect hotspot -knob sampling-mode=hw"
#VTUNE=

# TODO: Switch among the following three for # of iterations
#ITER=10k  # small
ITER=1M # med
#ITER=10M # large

run() {
  PROG=$1
  TRACEFILE=$2
  FACTOR=$3  # #of parts=factor x # of threads

      
  rm -rf $TRACEFILE
  touch $TRACEFILE
  
  echo $1 $2 $3
  
  # TODO: Set thread counts to test here
  # for tr in 1 2 4 6 8 10 12 16 20
  for tr in 1 2 4 6 8
  #for tr in 1 2
  do 
    $VTUNE $PROG --iterations=$ITER  --threads=$tr --parts=`expr $tr \* $FACTOR` >> $TRACEFILE 2>&1   
  done
  
  cat $TRACEFILE | grep "test="
}

#####################
# biglock
run "./main" "trace-hash.txt" 1

