#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export threepids=\
'['\
'["msisdn","919999999999"]'\
',["msisdn","919999999998"]'\
',["msisdn","919999999997"]'\
',["msisdn","919999999996"]'\
']'

export accesskey=${accesskey:="msisdn:919999999999:JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/bulk_lookup_params \
	accesskey:="\"${accesskey}\"" \
	threepids:=${threepids}
fi
