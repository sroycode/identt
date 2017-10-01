#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export threepids=\
'['\
'{"medium":"msisdn","address":"919999999999"}'\
',{"medium":"msisdn","address":"919999999998"}'\
',{"medium":"msisdn","address":"919999999997"}'\
',{"medium":"msisdn","address":"919999999996"}'\
']'

echo $threepids

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/bulk_lookup \
	threepids:=${threepids}
fi
