#!/bin/bash

SERVICE_NAME="MONGODB"
CWD=`pwd`/
PIDPATH=${CWD}mongod.pid

function pid {
    local RESULT=""
    if [ -e $PIDPATH ]
    then
	local PID=$(cat $PIDPATH)
	if [ "$PID" != "" ]
	then
	    RESULT=$PID
	else
	    rm $PIDPATH
	fi
    fi
    echo $RESULT
}

PID=$(pid)
if [ "$PID" != "" ]
then
    echo "Found ${SERVICE_NAME} running with PID ${PID}, stopping.."
    kill $PID
    sleep 1s
fi

PID=$(pid)
if [ "$PID" != "" ]
then
    echo "Failed to kill running ${SERVICE_NAME}, aborting"
    exit 1
else
    echo "${SERVICE_NAME} stopped"
fi

SERVICE=mongod
LOGPATH=${CWD}mongod.log
DBPATH=${CWD}tests/test-db
PORT=27777

echo "Starting ${SERVICE_NAME}"
$SERVICE --pidfilepath $PIDPATH --dbpath $DBPATH --fork --logpath $LOGPATH --port $PORT
sleep 1s

PID=$(pid)
if [ "$PID" == "" ]
then
    echo "Failed to start ${SERVICE_NAME}, aborting"
    exit 1
else
    echo "${SERVICE_NAME} started with PID = ${PID}"
fi
