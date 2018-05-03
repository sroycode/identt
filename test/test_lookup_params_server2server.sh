#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export medium=${medium:="msisdn"};
export address=${address:="919999999999"};
export shared_secret=${shared_secret:="qu++AtusT++FVHqvQdPXJlgPtxeo678gXRcGfnTLqKk"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/lookup_params \
	medium:="\"${medium}\"" \
	address:="\"${address}\"" \
	"Shared-Secret: $shared_secret"
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/lookup_params \
	medium="${medium}" \
	address="${address}" \
	"Shared-Secret: $shared_secret"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/lookup_params \
	medium=="${medium}" \
	address=="${address}" \
	"Shared-Secret: $shared_secret"
fi
