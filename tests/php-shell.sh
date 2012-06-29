#!/bin/sh

ROOT=$(pwd)
CFMOD=${ROOT}/../cfmod/.libs/cfmod.so
CFAPI=${ROOT}/../api/.libs/cfengine-enterprise-api.so
CONFIG=${ROOT}/conf/php-fpm.conf
LOGDIR=${ROOT}/log

mkdir -p $LOGDIR
mkdir -p ${ROOT}/var

CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777 exec gdb --args php -a -d extension=mongo.so -d extension=$CFMOD -d extension=$CFAPI
run