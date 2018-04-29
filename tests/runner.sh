#!/bin/bash 
#
# run test images through libnssvg and count results
#
# Licensed under the MIT License,
#                http://opensource.org/licenses/mit-license.php
# Copyright (C) 2016 Vincent Sanders <vince@netsurf-browser.org>
# Copyright (C) 2018 Jim Huang <jserv.tw@gmail.com>

TEST_PATH=$1
TEST_OUT=$(mktemp -d)/mvg
TEST_LOG=${TEST_PATH}/tests/log

mkdir -p ${TEST_OUT}

echo "SVGirl tests" > ${TEST_LOG}
echo >> ${TEST_LOG}

SVGTESTS="tests/data/*.svg"

svgdecode()
{
    OUTF=$(basename ${1} .svg)
    CMPF=$(dirname  ${1})/${OUTF}.mvg
    echo "Input: ${1}" >> ${TEST_LOG}
    ${TEST_PATH}/tests/decode_svg ${1} 1.0 ${TEST_OUT}/${OUTF}.mvg 2>> ${TEST_LOG}
    ECODE=$?

    echo "Exit code:${ECODE}" >> ${TEST_LOG}
    echo >> ${TEST_LOG}
    if [ "${ECODE}" -gt 0 ];then
        echo "    => $OUTF"
	return ${ECODE}
    fi

    if [ -f "${CMPF}" ]; then
	cmp ${CMPF} ${TEST_OUT}/${OUTF}.ppm >> ${TEST_LOG} 2>> ${TEST_LOG}
	if [ "$?" -ne 0 ]; then
	    return 128
	fi
    fi

    return 0
}

SVGTESTTOTC=0
SVGTESTPASSC=0
SVGTESTFAILC=0
SVGTESTERRC=0

echo "Testing SVG decode"

for SVG in $(ls ${SVGTESTS});do
    SVGTESTTOTC=$((SVGTESTTOTC+1))
    svgdecode ${SVG}
    ECODE=$?
    if [ "${ECODE}" -gt 127 ];then
	SVGTESTERRC=$((SVGTESTERRC+1))
    else
	if [ "${ECODE}" -gt 0 ];then
	    SVGTESTFAILC=$((SVGTESTFAILC+1))
	else
	    SVGTESTPASSC=$((SVGTESTPASSC+1))
	fi
    fi
done

echo
echo "Tests:${SVGTESTTOTC} Pass:${SVGTESTPASSC} Fail:${SVGTESTFAILC} Error:${SVGTESTERRC}"

# exit code
if [ "${SVGTESTERRC}" -gt 0 ]; then
    exit 1
fi

exit 0
