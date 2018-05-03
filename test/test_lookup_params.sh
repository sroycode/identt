#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export medium=${medium:="msisdn"};
export address=${address:="919999999999"};
export accesskey=${accesskey:="msisdn:919999999999:JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/lookup_params \
	medium:="\"${medium}\"" \
	address:="\"${address}\"" \
	accesskey:="\"${accesskey}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/lookup_params \
	medium="${medium}" \
	address="${address}" \
	accesskey="${accesskey}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/lookup_params \
	medium=="${medium}" \
	address=="${address}" \
	accesskey=="${accesskey}"
fi
