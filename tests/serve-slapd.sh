#!/bin/sh

SERVICE_NAME="OpenLDAP (slapd)"

ROOT=$(dirname $(pwd))/
LOGDIR=${ROOT}tests/log
CONFDIR=${ROOT}tests/conf
VARDIR=${ROOT}tests/var
PORT=1025

mkdir -p $LOGDIR
mkdir -p $VARDIR/ldap

echo "Starting ${SERVICE_NAME}"
exec slapd -d 1 -h ldap://localhost:${PORT} -f $CONFDIR/slapd.conf 1> $LOGDIR/slapd-stdout.log 2> $LOGDIR/slapd-stderr.log