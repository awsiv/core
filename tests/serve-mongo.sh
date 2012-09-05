#!/bin/sh

SERVICE_NAME="MONGODB"
ROOT=$(pwd)/..

SERVICE=mongod
LOGPATH=${CFENGINE_TEST_OVERRIDE_MONGO_LOGPATH:-${ROOT}/tests/log/mongod.log}
DBPATH=${CFENGINE_TEST_OVERRIDE_MONGO_DBPATH:-${ROOT}/tests/test-db}
PORT=${CFENGINE_TEST_OVERRIDE_MONGO_PORT:-27777}

mkdir -p log
mkdir -p $DBPATH

echo "Starting ${SERVICE_NAME}"
exec mongod --nojournal --dbpath $DBPATH --logpath $LOGPATH --port $PORT
