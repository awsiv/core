#!/bin/sh -e

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777
export CFENGINE_TEST_OVERRIDE_HOSTKEY="SHA=305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283"

if [ -n "$1" ]
then
    export CFENGINE_TEST_OVERRIDE_TEST_FILES=$1
else
    export CFENGINE_TEST_OVERRIDE_TEST_FILES=../cfmod/tests/*.php
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