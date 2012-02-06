#!/bin/bash

SERVICE_NAME="NGINX Web Server"
PIDFILE=nginx.pid

if [ -e $PIDFILE ]
then
    PID=$(cat $PIDFILE)
    echo "Found ${SERVICE_NAME} running with PID ${PID}, stopping.."
    kill $PID
    sleep 1s
    if [ -e $PIDFILE ]
    then
	echo "Failed to kill running ${SERVICE_NAME}, aborting"
	exit 1
    else
	echo "${SERVICE_NAME} stopped"
    fi
fi

CWD=`pwd`/
SERVICE=nginx
CONFIG=http-nginx.conf

echo "Starting ${SERVICE_NAME}"
$SERVICE -p $CWD -c http-nginx.conf
sleep 1s

if [ -e $PIDFILE ]
then
    echo "${SERVICE_NAME} started!"
else
    echo "Failed to start ${SERVICE_NAME}, aborting"
    exit 1
fi
