#!/bin/bash

echo "$0 preparation script"
pwd

make coverage -C ./

supervisorctl status all
supervisorctl update
supervisorctl restart all
sleep 3
supervisorctl status all

# run API validation
ubus-api-validator -d ./test/api/json/ > ./api-result.log
ret=$?

supervisorctl stop all
supervisorctl status

#report part
gcovr -r . --xml -o ./api-test-coverage.xml
gcovr -r .
cp ./memory-report.xml ./api-test-memory-report.xml
tap-junit --input ./api-result.log --output report
date +%s > timestamp.log

echo "$0 exit status ${ret}"
exit ${ret}
