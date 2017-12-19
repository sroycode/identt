#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh
export TESTURL=`echo ${TESTURL} | sed 's/\/_matrix\//\/_identt\//'`

## ---- variables

export accesskey=${accesskey:="JcEZP9gXd2MYGqAXp2B9eF6KNGz8cDpj"};
export tag=${tag:="testdata"};
export jsondata=${jsondata:='{"name":"john","age":10}'};
export textdata=${textdata:="aaa bbb ccc"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/blobdata/setBlobData \
	tag:="\"${tag}\"" \
	jsondata:=${jsondata} \
	textdata:="\"${textdata}\"" \
	accesskey:="\"${accesskey}\""
else
	echo "ONLY Json allowed"
fi
