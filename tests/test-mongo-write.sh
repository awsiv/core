#!/bin/sh -e

WORKDIR=./db-writes-workdir

if [ -n "$1" ]
then
    IMPORT_FILE_PATH=$1
else
    IMPORT_FILE_PATH=./data/report_hub_full.nov
fi

CLIENT_COUNT=200

MONGO_PID=

kill_servers()
{
  retcode=$?
  echo "Killing servers"
  kill $MONGO_PID 2>/dev/null || :
  exit $retcode
}

trap kill_servers INT QUIT 0

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777
export CFENGINE_TEST_OVERRIDE_MONGO_DBPATH=./db-writes
export CFENGINE_TEST_OVERRIDE_MONGO_LOGPATH=./db-writes/mongod.log

./serve-mongo.sh&
MONGO_PID=$!

# Wait for Mongo to come online
while ! mongo --port 27777 --eval 'db.ping'; do
  sleep 0.01
done

cf-hub -i

mkdir -p $WORKDIR

generate_report_files()
{
    REPORT_FILES_COUNT=$1
    echo "Generating $REPORT_FILES_COUNT report-files..."
    i=0
    while [  $i -lt $REPORT_FILES_COUNT ]
    do
        DESTINATION=$WORKDIR/$i.nov
        cp $IMPORT_FILE_PATH $DESTINATION
        
        HEADER="NOVA_EXPORT full SHA=265e5eb7abf2bb45a4e82b1753a95d047815f8ad2966d2cc8d205a027c50$i 10.0.0.$i generated-name-$i"
        
        sed -i 1d $DESTINATION
        sed -i "1i $HEADER" $DESTINATION

        i=$(($i+1))
    done
}

import_report_files()
{
    REPORT_FILES_COUNT=$1
    REPORTS_PER_PROCESS=50
    PROCESS_COUNT=$((($REPORT_FILES_COUNT + ($REPORTS_PER_PROCESS - 1))/$REPORTS_PER_PROCESS))
    
    if [ $PROCESS_COUNT -eq 0 ]
    then
        PROCESS_COUNT=1
    fi
    
    echo "Creating $PROCESS_COUNT processes..."
    
    i=0
    while [ $i -lt $PROCESS_COUNT ]
    do
        START=$(($i*$REPORTS_PER_PROCESS))
        END=$(($START + $REPORTS_PER_PROCESS - 1))
        
        if [ $END -ge $REPORT_FILES_COUNT ]
        then
            END=$(($REPORT_FILES_COUNT - 1))
        fi
        
        ./test-mongo-write-import.sh $WORKDIR $START $END &
        i=$(($i+1))
    done
}

generate_report_files $CLIENT_COUNT

import_report_files $CLIENT_COUNT

# arbitrary sleep to wait for children
sleep 1000

echo "Finished all"