#!/bin/sh -e

WORKDIR=$1
START=$2
END=$3

export CFENGINE_TEST=""

i=$START

TIME_START=$(date +%s)

while [ $i -le $END ]
do
    REPORT_FILE=$WORKDIR/$i.nov
    cf-report -i $REPORT_FILE
    echo "$$: $i / $END"
    i=$(($i+1))
done

TIME_END=$(date +%s)

SECONDS_USED=$(($TIME_END - $TIME_START))
echo "$$: Finished ($SECONDS_USED seconds)"
