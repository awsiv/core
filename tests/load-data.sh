#!/bin/sh -e

DATADIR=$1

if [ -z $DATADIR ]; then
    echo "Usage: load-data.sh <data-directory>"
    exit 1
fi

PORT=27777

echo "Importing data from $1"

load_data() {
    local DB=$1
    local COLL=$2
    local FILE=${DATADIR}/${DB}.${COLL}.json
    echo "Importing ${FILE}"
    if [ -f ${FILE} ]; then
	mongoimport --port ${PORT} -d phpcfengine -c $COLL --drop --file ${FILE} --jsonArray
    fi
}

load_data phpcfengine users
load_data phpcfengine roles
load_data phpcfengine appsettings

load_data cfreport hosts
load_data cfreport logs_rep
load_data cfreport logs_nk

echo "Importing data complete"
