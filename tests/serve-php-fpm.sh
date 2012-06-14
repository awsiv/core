#!/bin/sh

SERVICE_NAME="PHP-FPM (+cfmod)"

ROOT=$(pwd)
CFMOD=${ROOT}/../cfmod/.libs/cfmod.so
CFAPI=${ROOT}/../api/.libs/cfengine-enterprise-api.so
CONFIG=${ROOT}/conf/php-fpm.conf
LOGDIR=${ROOT}/log

mkdir -p $LOGDIR
mkdir -p ${ROOT}/var

echo "Starting ${SERVICE_NAME}"
exec php5-fpm -n -p $ROOT -y $CONFIG -d extension=mongo.so -d extension=$CFMOD -d extension=$CFAPI 2> ${LOGDIR}/php-fpm-stderr.log
