#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export medium=${medium:="msisdn"};
export address=${address:="919999999999"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/lookup \
	medium:="\"${medium}\"" \
	address:="\"${address}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/lookup \
	medium="${medium}" \
	address="${address}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/lookup \
	medium=="${medium}" \
	address=="${address}"
fi
