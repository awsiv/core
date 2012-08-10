#!/bin/sh -e

ROOT=$(dirname $(pwd))/
DBDIR=${ROOT}tests/var/ldap

LDIF=$1

if [ -z $LDIF ]; then
    echo "Usage: load-data-ldap.sh <data.ldif>"
    exit 1
fi

PORT=1025

echo "Importing data from $1"

ldapadd -w secret -h localhost -p $PORT -x -D "cn=root,dc=localhost" -f $LDIF

echo "Importing data complete"
