#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export threepids=\
'['\
'["msisdn","919999999999"]'\
',["msisdn","919999999998"]'\
',["msisdn","919999999997"]'\
',["msisdn","919999999996"]'\
']'

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/bulk_lookup \
	threepids:=${threepids}
fi
