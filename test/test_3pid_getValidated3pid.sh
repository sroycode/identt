#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export client_secret=${client_secret:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export sid=${sid:="12679"};
export token=${token:="499b11fe-2505-49d9-a872-83b7a9b9c303"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/3pid/getValidated3pid \
	token:="\"${token}\"" \
	sid:="\"${sid}\"" \
	client_secret:="\"${client_secret}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/3pid/getValidated3pid \
	token="${token}" \
	sid="${sid}" \
	client_secret="${client_secret}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/3pid/getValidated3pid \
	token=="${token}" \
	sid=="${sid}" \
	client_secret=="${client_secret}"
fi
