#!/usr/bin/env bash

set -u

# $1 = full logs
# $2 = generated directory path
# $3 = golden output

LOG_FILE="$1"
GENERATED="$2"
GOLDEN_FILE="$3"
TEST_NAME="$4"

STATUS=0

shift 4

echo "Starting at" `date` > "${LOG_FILE}"
echo "Running $*" >> "${LOG_FILE}"

if ! $* &> "${GENERATED}/sim_output"; then
    STATUS=255
else
    sed -e 's/{{.*}}/VARIABLE_VALUE_REMOVED_FROM_TEST_RESULT/g' < "${GENERATED}/sim_output" | LC_ALL=C sort > "${GENERATED}/sim_output_filtered"
    sed -e 's/^Cluster0@0.0: .\{2,5\}: \(.*\)$/\1/g' -i "${GENERATED}/sim_output_filtered"
    sed -e 's/^IO.*0@0.0: ....: \(.*\)$/\1/g' -i "${GENERATED}/sim_output_filtered"
    sed -e '/.*have no effect.*/d' -i  "${GENERATED}/sim_output_filtered"
    sed -e '/^Warning.*Cluster0$/d' -i "${GENERATED}/sim_output_filtered"
    echo "Diffing output with golden" >> "${LOG_FILE}"

    if ! diff -Nru "${GENERATED}/sim_output_filtered" "${GOLDEN_FILE}" >> "${LOG_FILE}"; then
	STATUS=255
    fi
fi

cat "${GENERATED}/sim_output" >> "${LOG_FILE}"
echo "Ending at" `date` >> "${LOG_FILE}"
if [ $STATUS != 0 ]; then
    echo Test $TEST_NAME fail
    echo "$TEST_NAME" > "${GENERATED}/test.failed"
else
    echo "$TEST_NAME" > "${GENERATED}/test.passed"
fi
exit $STATUS
