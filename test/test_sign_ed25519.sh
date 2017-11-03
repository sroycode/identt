#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export sender=${sender:="myname"};
export mxid=${mxid:="@sroycode:matrix.justdis.com"};
export token=${token:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export private_key=${private_key:="1UfbvW0r7HYk2tavQW+Iy4E8JSYpESIlTR7GxeFzqQmq774C26xP74VUeq9B09cmWA+3F6jrvyBdFwZ+dMuoqQ"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST "${TESTURL}/sign-ed25519:test" \
	sender:="\"${sender}\"" \
	mxid:="\"${mxid}\"" \
	token:="\"${token}\"" \
	private_key:="\"${private_key}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST "${TESTURL}/sign-ed25519:test" \
	sender="${sender}" \
	mxid="${mxid}" \
	token="${token}" \
	private_key="${private_key}"
fi
