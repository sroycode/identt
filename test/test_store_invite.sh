#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export medium=${medium:="msisdn"};
export address=${address:="919999999991"};
export room_id=${room_id:="@12345"};
export sender=${sender:="@sroycode:matrix.justdis.com"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/store-invite \
	medium:="\"${medium}\"" \
	address:="\"${address}\"" \
	room_id:="\"${room_id}\"" \
	sender:="\"${sender}\""
fi
