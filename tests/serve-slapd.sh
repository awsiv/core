#!/bin/sh

SERVICE_NAME="OpenLDAP (slapd)"

ROOT=$(dirname $(pwd))/
LOGDIR=${ROOT}tests/log
CONFDIR=${ROOT}tests/conf

mkdir -p $LOGDIR

echo "Starting ${SERVICE_NAME}"
exec slapd -d 1 -h ldap://localhost:1025 -f $CONFDIR/ldap.conf -F $CONFDIR/slapd.d 1> $LOGDIR/slapd-stdout.log 2> $LOGDIR/slapd-stderr.log