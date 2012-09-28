#!/bin/sh -e

ROOT=$(pwd)/..
API=${ROOT}/api/.libs/cfengine-enterprise-api.so
LOGDIR=${ROOT}/tests/log
LOGFILE="${LOGDIR}/test-api-memory.log"

echo "Removing log file: $LOGFILE"
rm -f $LOGFILE

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
./load-data.sh $ROOT/api/http/tests/fixtures

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777

CFENGINE_TEST_HOSTKEY="SHA=305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283"

if [ -n "$CFENGINE_TEST_OVERRIDE_USERNAME" ]
then
    CFENGINE_TEST_USERNAME=$CFENGINE_TEST_OVERRIDE_USERNAME
else
    CFENGINE_TEST_USERNAME="admin"
fi

if [ -n "$CFENGINE_TEST_OVERRIDE_PASSWORD" ]
then
    CFENGINE_TEST_PASSWORD=$CFENGINE_TEST_OVERRIDE_PASSWORD
else
    CFENGINE_TEST_PASSWORD="admin"
fi

if [ -n "$CFENGINE_TEST_OVERRIDE_TEST_FILES" ]
then
    CFENGINE_TEST_FILES=$CFENGINE_TEST_OVERRIDE_TEST_FILES
else
    CFENGINE_TEST_FILES=${ROOT}/api/tests/*.php
fi

TEST_FAILED_MEMORY=""
TEST_FAILED_SPEED=""
TEST_FAILED_FATAL_ERROR=""

export CFENGINE_TEST_USERNAME
export CFENGINE_TEST_PASSWORD
export CFENGINE_TEST_HOSTKEY


TIME_START_TEST_ALL=$(date +%s)
echo -e "\n\n=== Staring tests at $TIME_START_TEST_ALL ===" >> $LOGFILE
echo -e "Testing with params $CFENGINE_TEST_USERNAME, $CFENGINE_TEST_HOSTKEY, $CFENGINE_TEST_FILES" >> $LOGFILE
echo -e "Using cfengine-enterprise-api = $API" >> $LOGFILE

runtest()
{
    PHPFILE=$1
    
    echo "Testing $PHPFILE..."
    echo -e "php -n -d extension=$API -f $PHPFILE"
    
    TIME_START_TEST=$(date +%s)
    #OUTPUT_PHP=$(php -n -d extension=$API -f $PHPFILE) # 2>&1)
    TIME_END_TEST=$(date +%s)

    SECONDS_USED_TEST=$(($TIME_END_TEST - $TIME_START_TEST))
    
    OUTPUT_VALGRIND=$(valgrind -q --main-stacksize=10000000 --max-stackframe=10000000 --leak-check=full --log-fd=1 php -n -d extension=$API -f $PHPFILE 2>/dev/null)
    echo "done"
    
    if [ -n "$OUTPUT_VALGRIND" ]
    then
        echo "Memory error:"
        echo "$OUTPUT_VALGRIND"
        TEST_FAILED_MEMORY="$TEST_FAILED_MEMORY $PHPFILE"
    fi
    
    if [ "$SECONDS_USED_TEST" -gt 1 ]  # NOTE: may be up to two seconds (1-second resolution)
    then
        echo "Too slow (used $SECONDS_USED_TEST seconds)"
        TEST_FAILED_SPEED="$TEST_FAILED_SPEED $PHPFILE"
    fi

    if [[ "$OUTPUT_PHP" == *"PHP Fatal error"* ]]
    then
        echo "Fatal php error:"
        echo "$OUTPUT_PHP"
        TEST_FAILED_FATAL_ERROR="$TEST_FAILED_FATAL_ERROR $PHPFILE"
    fi
    
    echo "--- $PHPFILE ---" >> $LOGFILE
    echo -e "time: $SECONDS_USED_TEST seconds" >> $LOGFILE
    echo -e "php:\n$OUTPUT_PHP" >> $LOGFILE
    echo -e "valgrind:\n$OUTPUT_VALGRIND\n" >> $LOGFILE
}

echo "Starting tests"

for TESTFILE in $CFENGINE_TEST_FILES
do
    runtest "$TESTFILE"
done


TIME_END_TEST_ALL=$(date +%s)
SECONDS_USED_TEST_ALL=$(($TIME_END_TEST_ALL - $TIME_START_TEST_ALL))

echo -e "Total time: $SECONDS_USED_TEST_ALL seconds\n\n" >> $LOGFILE

echo -e "\nSUMMARY (total time: $SECONDS_USED_TEST_ALL seconds)"
echo "======================"

echo "Failed memory checks:"
echo "$TEST_FAILED_MEMORY"

echo "----------------------"
echo "Failed speed checks:"
echo "$TEST_FAILED_SPEED"

echo "----------------------"
echo "Tests giving php fatal error (is test wrong?):"
echo "$TEST_FAILED_FATAL_ERROR"
