#!/bin/bash;

if [ ! "`which http`" -o ! "`which jq`" ] ; then
	echo "httpie or jq not installed";
	exit 1;
fi

if [ -z "${SRCDIR}" -o -z "${SRCFIL}" ] ; then
	echo "define SRCDIR and SRCFIL in test program";
	exit 1;
fi


# if testbatch is not set , set 0
export TESTBATCH=${TESTBATCH:=0};

# if testheaders is not set , set 0
export TESTHEADERS=${TESTHEADERS:=0};

# test directory
export TESTDIR=${TESTDIR:="./testdir"};

HTTPIE="http"
if [ ${TESTBATCH} -eq 1 ] ; then
	if [ ! -d ${TESTDIR} ] ; then mkdir ${TESTDIR} ; fi
	HTTPIE="${HTTPIE} -o ${TESTDIR}/${SRCFIL}.out";
elif [ ${TESTHEADERS} -eq 1 ] ; then
	HTTPIE="${HTTPIE} --print BHbh";
fi
export HTTPIE

# if postform is not set , set 0 i.e. posts are json
export POSTFORM=${POSTFORM:=0};

export TESTURL=${TESTURL:="http://localhost:9091/_matrix/identity/api/v1"};
#export TESTURL="matrix.org/_matrix/identity/api/v1";

