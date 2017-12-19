#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export client_secret=${client_secret:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export sid=${sid:="00000"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/blobdata/getAccessKey \
	sid:="\"${sid}\"" \
	client_secret:="\"${client_secret}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/blobdata/getAccessKey \
	sid="${sid}" \
	client_secret="${client_secret}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/blobdata/getAccessKey \
	sid=="${sid}" \
	client_secret=="${client_secret}"
fi
