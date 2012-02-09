#!/bin/sh -e

if [ -z $1 ]
then
    echo "Usage: load-data.sh <data-directory>"
    exit 1;
fi

PORT=27777

echo "Importing data from $1"

mongoimport --port ${PORT} -d phpcfengine -c users --drop --file $1/phpcfengine.users.json --jsonArray

mongoimport --port ${PORT} -d cfreport -c hosts --drop --file $1/cfreport.hosts.json --jsonArray
mongoimport --port ${PORT} -d cfreport -c logs_rep --drop --file $1/cfreport.logs_rep.json --jsonArray
mongoimport --port ${PORT} -d cfreport -c logs_nk --drop --file $1/cfreport.logs_nk.json --jsonArray



echo "Importing data complete"