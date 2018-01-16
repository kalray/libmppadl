#!/usr/bin/env bash

ROOT_TESTS_DIR="$(readlink -f $1)"

TOTAL_NUMBER=$(find "${ROOT_TESTS_DIR}" -name "test.log" | wc -l)

PASSED_TESTS_NUM=$(find "${ROOT_TESTS_DIR}"  -name test.passed |wc -l)
FAILED_TESTS_NUM=$(find "${ROOT_TESTS_DIR}"  -name test.failed |wc -l)

if [ ! -z ${FAILED_TESTS_NUM} ]; then
    echo "Log for failed tests :"

    find "${ROOT_TESTS_DIR}" -name test.failed |
	while read failed; do
	    generated_dir=$(dirname $failed)
	    echo "Log result for $generated_dir"
	    echo "8<---8<---8<---8<---8<---8<---"
	    cat $generated_dir/test.log
	    echo "8<---8<---8<---8<---8<---8<---"
	    echo ""
	done
fi

echo "total number of tests:" $TOTAL_NUMBER
echo "Tests passed ( $PASSED_TESTS_NUM ):"
find "${ROOT_TESTS_DIR}"  -name test.passed -exec cat {} ';' | while read f; do
    readlink -f $f | sed -e "s#${ROOT_TESTS_DIR}##g"
done

echo "Tests failed ( $FAILED_TESTS_NUM ) :"
find "${ROOT_TESTS_DIR}"  -name test.failed -exec cat {} ';' | sed -e "s#${ROOT_TESTS_DIR}##g"

exit $FAILED_TESTS_NUM
