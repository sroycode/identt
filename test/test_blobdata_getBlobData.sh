#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export accesskey=${accesskey:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export tag=${tag:="testdata"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/blobdata/getBlobData \
	tag:="\"${tag}\"" \
	accesskey:="\"${accesskey}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/blobdata/getBlobData \
	tag="${tag}" \
	accesskey="${accesskey}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/blobdata/getBlobData \
	tag=="${tag}" \
	accesskey=="${accesskey}"
fi
