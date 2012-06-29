#!/bin/sh -e

export CFENGINE_TEST_OVERRIDE_MONGO_PORT=27777

PHP_FPM_PID=
NGINX_PID=
MONGO_PID=

kill_servers() {
  retcode=$?
  echo "Killing servers"
  kill $PHP_FPM_PID 2>/dev/null || :
  kill $NGINX_PID 2>/dev/null || :
  kill $MONGO_PID 2>/dev/null || :
  exit $retcode
}

trap kill_servers INT QUIT 0

./serve-php-fpm.sh&
PHP_FPM_PID=$!

./serve-nginx.sh&
NGINX_PID=$!

./serve-mongo.sh&
MONGO_PID=$!

# Wait for Mongo to come online
while ! mongo --port 27777 --eval 'db.ping'; do
  sleep 0.01
done

# Load Mongo state
./load-data.sh data/rest

# Run tests
php -c conf/php.ini /usr/bin/phpunit ../api/http/tests

echo "Testsuite finished"


