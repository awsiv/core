#!/bin/bash

SERVICE_NAME="PHP-FPM (cfmod)"
PIDFILE=cfmod/cfmod-php-fpm.pid

if [ -e $PIDFILE ]
then
    PID=$(cat $PIDFILE)
    echo "Found ${SERVICE_NAME} running with PID ${PID}, stopping.."
    kill $PID
    sleep 1s
    if [ -e $PIDFILE ]
    then
	echo "Failed to kill running ${SERVICE_NAME}, try deleting ${PIDFILE}, aborting"
	exit 1
    else
	echo "${SERVICE_NAME} stopped"
    fi
fi

CWD=`pwd`/
SERVICE=php5-fpm
CFMOD=cfmod/.libs/cfmod.so
CONFIG=cfmod/cfmod-php-fpm.conf

echo "Starting ${SERVICE_NAME}"
$SERVICE -p $CWD -y $CONFIG -d extension=$CFMOD
sleep 1s

if [ -e $PIDFILE ]
then
    echo "${SERVICE_NAME} started!"
else
    echo "Failed to start ${SERVICE_NAME}, aborting"
    exit 1
fi
