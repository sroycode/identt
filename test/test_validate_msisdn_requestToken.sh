#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export phone_number=${phone_number:="9999999999"};
export country=${country:="IN"};
export client_secret=${client_secret:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export next_link=${next_link:="http://google.com"};
export send_attempt=${send_attempt:="1"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/validate/msisdn/requestToken \
	phone_number:="\"${phone_number}\"" \
	country:="\"${country}\"" \
	client_secret:="\"${client_secret}\"" \
	next_link:="\"${next_link}\"" \
	send_attempt:=${send_attempt}
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/validate/msisdn/requestToken \
	phone_number="${phone_number}" \
	country="${country}" \
	client_secret="${client_secret}" \
	next_link="${next_link}" \
	send_attempt=${send_attempt}
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/validate/msisdn/requestToken \
	phone_number=="${phone_number}" \
	country=="${country}" \
	client_secret=="${client_secret}" \
	next_link=="${next_link}" \
	send_attempt==${send_attempt}
fi
