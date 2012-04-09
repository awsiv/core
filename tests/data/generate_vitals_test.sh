#/bin/bash

# Generate testdata for vitals.
# 1. Set MAX=$MAGMAX for mag, $WEEKMAX for week, or $YEARMAX for year, maybe adjust kH below to a host in your system
# 1. ./generate_vitals_test.sh > /tmp/out
# 2. Wrap contents of /tmp/out with db.monitoring_mg.insert(OLDCONTENTS) (use wk/yr instead of mg for week or year)
# 3. Import into vitals db with "mongo localhost:27017/cfreport /tmp/out"
# 4. Do the above once for mag, week and year
# 5. You now have a predictable vital with id "test"

MAGMAX=2015
WEEKMAX=167
YEARMAX=155

MAX=$MAGMAX

echo "{"

echo "\"ds\" : \"tester\","

echo "\"en\" : 0,"
echo "\"ex\" : 100,"
echo "\"gl\" : true,"
echo "\"id\" : \"test\","
echo "\"kH\" : \"SHA=265e5eb7abf2bb45a4e82b1753a95d047815f8ad2966d2cc8d205a027c50ba7d\","


echo "\"e\" : ["
for ((i = 0; i <= $((MAX-1)); i++))
do
  echo "$i,"
done
echo "$MAX"
echo "],"

echo "\"q\" : ["
for ((i = $MAX; i >= 1; i--))
do
  echo "$i,"
done
echo "0"
echo "],"

echo "\"d\" : ["
for ((i = 0; i <= $((MAX-1)); i++))
do
  echo "10,"
done
echo "10"
echo "]"

echo "}"
