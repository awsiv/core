#!/bin/sh

SERVICE_NAME="NGINX Web Server"

ROOT=$(dirname $(pwd))/
LOGDIR=${ROOT}tests/log

mkdir -p $LOGDIR

echo "Starting ${SERVICE_NAME}"
exec nginx -p $ROOT -c tests/conf/http-nginx.conf 2> ${LOGDIR}/nginx-stderr.log
