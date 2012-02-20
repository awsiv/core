#!/bin/sh

SERVICE_NAME="PHP-FPM (+cfmod)"

ROOT=$(pwd)
CFMOD=${ROOT}/../cfmod/.libs/cfmod.so
CONFIG=${ROOT}/conf/php-fpm.conf
PHPINI=${ROOT}/conf/php.ini

echo "Starting ${SERVICE_NAME}"
exec php5-fpm -n -p $ROOT -y $CONFIG -d extension=mongo.so -d extension=$CFMOD 2> ${ROOT}/log/php-fpm-stderr.log
