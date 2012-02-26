#!/bin/sh -e

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777

if [ -n "$1" ]
then
    export CFENGINE_TEST_OVERRIDE_TEST_FILES=$1
fi

MONGO_PID=

kill_servers() {
  retcode=$?
  echo "Killing servers"
  kill $MONGO_PID 2>/dev/null || :
  exit $retcode
}

trap kill_servers INT QUIT 0

./serve-mongo.sh&
MONGO_PID=$!

# Wait for Mongo to come online
while ! mongo --port 27777 --eval 'db.ping'; do
  sleep 0.01
done

# Load Mongo state
./load-data.sh data/rest

# Run tests
../cfmod/tests/testall

echo "Testsuite finished"
