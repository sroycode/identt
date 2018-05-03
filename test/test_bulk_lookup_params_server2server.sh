#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables
export shared_secret=${shared_secret:="qu++AtusT++FVHqvQdPXJlgPtxeo678gXRcGfnTLqKk"};

export threepids=\
'['\
'["msisdn","919999999999"]'\
',["msisdn","919999999998"]'\
',["msisdn","919999999997"]'\
',["msisdn","919999999996"]'\
']'

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/bulk_lookup_params \
	accesskey:="\"${accesskey}\"" \
	threepids:=${threepids} \
	"Shared-Secret: $shared_secret"
fi
