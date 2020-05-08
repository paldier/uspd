#!/bin/bash

echo "preparation script"
pwd

echo "Running the unit test cases"
make unit-test -C ./src/
ret=$?

#report part
#GitLab-CI output
gcovr -r .
# Artefact
gcovr -r . --xml -o ./unit-test-coverage.xml
date +%s > timestamp.log

echo "$0 Return status ${ret}"
exit ${ret}
