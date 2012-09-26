#!/bin/sh -e

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777

ROOT=$(dirname $(pwd))/

PHP_FPM_PID=
NGINX_PID=
MONGO_PID=
SLAPD_PID=

kill_servers() {
  retcode=$?
  echo "Killing servers"
  kill $PHP_FPM_PID 2>/dev/null || :
  kill $NGINX_PID 2>/dev/null || :
  kill $MONGO_PID 2>/dev/null || :
  kill $SLAPD_PID 2>/dev/null || :
  exit $retcode
}

trap kill_servers INT QUIT 0

./serve-mongo.sh&
MONGO_PID=$!

echo "Clearing LDAP data"
rm -rf ${ROOT}/tests/var/ldap/*

./serve-slapd.sh&
SLAPD_PID=$!

# Wait for Slapd startup
while ! ldapsearch -h localhost -p 1025 -x -b '' -s base '(objectclass=*)' 1>/dev/null 2>/dev/null; do
  sleep 0.01
done

# Wait for Mongo to come online
while ! mongo --port 27777 --eval 'db.ping'; do
  sleep 0.01
done

./serve-php-fpm.sh&
PHP_FPM_PID=$!

./serve-nginx.sh&
NGINX_PID=$!

./load-data-ldap.sh data/ldap/jersey.ldif

# Load Mongo state
#./load-data.sh data/rest

# Run tests
php -c conf/php.ini -d extension=mongo.so /usr/bin/phpunit ../api/http/tests

echo "Testsuite finished"


