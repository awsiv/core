#!/bin/sh -e

DATADIR=$1

if [ -z $DATADIR ]; then
    echo "Usage: load-data.sh <data-directory>"
    exit 1
fi

PORT=27777

echo "Importing data from $1"

load_data() {
    DB=$1
    COLL=$2
    FILE=${DATADIR}/${DB}.${COLL}.json
    if [ -f ${FILE} ]; then
        echo "Importing ${FILE}"
        mongoimport --port ${PORT} -d ${DB} -c ${COLL} --drop --file ${FILE} --jsonArray
    fi
}

load_data phpcfengine users
load_data phpcfengine roles
load_data phpcfengine appsettings

load_data cfreport hosts
load_data cfreport logs_rep
load_data cfreport logs_nk
load_data cfreport archive
load_data cfreport users
load_data cfreport roles

echo "Importing data complete"
