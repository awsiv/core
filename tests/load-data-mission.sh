#!/bin/sh -e

DATADIR=$1

if [ -z $DATADIR ]; then
    echo "Usage: load-data-mission.sh <data-directory>"
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

load_data phpcfengine appsettings
load_data phpcfengine usersettings
load_data phpcfengine custom_search

load_data cfreport hosts
load_data cfreport promises_exp
load_data cfreport promises_unexp
load_data cfreport archive
load_data cfreport monitoring_mg

load_data cfreport users
load_data cfreport roles
load_data cfreport settings


echo "Importing data complete"
