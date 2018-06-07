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
if [ "$TOTAL_NUMBER" == "0" ]; then
    echo '
 _____ _   _ ___ ____    ___ ____
|_   _| | | |_ _/ ___|  |_ _/ ___|
  | | | |_| || |\___ \   | |\___ \
  | | |  _  || | ___) |  | | ___) |
  |_| |_| |_|___|____/  |___|____/
 ____  _   _ ____  ____ ___ ____ ___ ___  _   _ ____
/ ___|| | | / ___||  _ \_ _/ ___|_ _/ _ \| | | / ___|
\___ \| | | \___ \| |_) | | |    | | | | | | | \___ \
 ___) | |_| |___) |  __/| | |___ | | |_| | |_| |___) |
|____/ \___/|____/|_|  |___\____|___\___/ \___/|____(_)
 _   _    ___     _____ _   _  ____   _   _  ___
| | | |  / \ \   / /_ _| \ | |/ ___| | \ | |/ _ \
| |_| | / _ \ \ / / | ||  \| | |  _  |  \| | | | |
|  _  |/ ___ \ V /  | || |\  | |_| | | |\  | |_| |
|_| |_/_/   \_\_/  |___|_| \_|\____| |_| \_|\___/
 _____ _____ ____ _____ ____    ___ ____
|_   _| ____/ ___|_   _/ ___|  |_ _/ ___|
  | | |  _| \___ \ | | \___ \   | |\___ \
  | | | |___ ___) || |  ___) |  | | ___) |
  |_| |_____|____/ |_| |____/  |___|____/
 ____  _____    _    _     _  __   __     _      ____    _    ____
|  _ \| ____|  / \  | |   | | \ \ / /    / \    | __ )  / \  |  _ \
| |_) |  _|   / _ \ | |   | |  \ V /    / _ \   |  _ \ / _ \ | | | |
|  _ <| |___ / ___ \| |___| |___| |    / ___ \  | |_) / ___ \| |_| |
|_| \_\_____/_/   \_\_____|_____|_|   /_/   \_\ |____/_/   \_\____/
 ____ ___ ____ _   _
/ ___|_ _/ ___| \ | |
\___ \| | |  _|  \| |
 ___) | | |_| | |\  |
|____/___\____|_| \_|

'
    echo "THIS IS SUSPICIOUS. HAVING NO TESTS IS REALLY A BAD SIGN"
fi
echo "Tests passed ( $PASSED_TESTS_NUM ):"
find "${ROOT_TESTS_DIR}"  -name test.passed -exec cat {} ';' | while read f; do
    readlink -f $f | sed -e "s#${ROOT_TESTS_DIR}##g"
done

echo "Tests failed ( $FAILED_TESTS_NUM ) :"
find "${ROOT_TESTS_DIR}"  -name test.failed -exec cat {} ';' | sed -e "s#${ROOT_TESTS_DIR}##g"

# This is BAD.
if [ "$TOTAL_NUMBER" == "0" ]; then
   exit 255
fi

exit $FAILED_TESTS_NUM
