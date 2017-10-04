#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export client_secret=${client_secret:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export sid=${sid:="12679"};
export mxid=${mxid:="@sroycode:jadoo.mobi"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/3pid/bind \
	mxid:="\"${mxid}\"" \
	sid:=${sid} \
	client_secret:="\"${client_secret}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/3pid/bind \
	mxid="${mxid}" \
	sid=${sid} \
	client_secret="${client_secret}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/3pid/bind \
	mxid=="${mxid}" \
	sid==${sid} \
	client_secret=="${client_secret}"
fi
