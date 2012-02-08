#!/bin/sh

SERVICE_NAME="NGINX Web Server"

ROOT=$(dirname $(pwd))/

echo "Starting ${SERVICE_NAME}"
exec nginx -p $ROOT -c tests/conf/http-nginx.conf 2> ${ROOT}/tests/log/nginx-stderr.log
