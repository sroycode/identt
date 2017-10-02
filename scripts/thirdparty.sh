#!/bin/bash

#### Set up Source Path #####

if [ -z ${IDENTT_SOURCE} ] ; then echo "PLEASE DEFINE IDENTT_SOURCE"; exit 1; fi
if [ -z ${IDENTT_TEMP} ] ; then
	export IDENTT_TEMP=${IDENTT_SOURCE}/thirdparty/src
fi
if [ -z ${IDENTT_TPSRC} ] ; then
	#export IDENTT_TPSRC=${IDENTT_TEMP}
	export IDENTT_TPSRC=/opt/backup
fi

export PATH=${PATH}:${IDENTT_SOURCE}/thirdparty/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${IDENTT_SOURCE}/thirdparty/lib:${IDENTT_SOURCE}/thirdparty/lib64
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${IDENTT_SOURCE}/thirdparty/lib:${IDENTT_SOURCE}/thirdparty/lib64
export LD_RUN_PATH=${LD_RUN_PATH}:${IDENTT_SOURCE}/thirdparty/lib:${IDENTT_SOURCE}/thirdparty/lib64

if [ `uname` = 'Linux' ] ; then
	if [ -z ${BOOST_ROOT} ] ; then echo "PLEASE DEFINE BOOST_ROOT"; exit 1; fi
	alias cmake=cmake3 -DBOOST_ROOT="$BOOST_ROOT"
fi


mkdir -p ${IDENTT_SOURCE} ${IDENTT_TEMP}

#### Functions to get Sources #####

# Building gperftools 2.5

function build_gperftools () {
local MVERSION=2.5
local MSOURCEFILE=${IDENTT_TPSRC}/gperftools-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/gperftools-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/include/gperftools/tcmalloc.h ] ; then echo "gperftools already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
./configure --prefix=${IDENTT_SOURCE}/thirdparty && make && make install
}

# Build protobuf 3.1.0

function build_protobuf () {
local MVERSION=3.1.0
local MSOURCEFILE=${IDENTT_TPSRC}/protobuf-cpp-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/protobuf-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/libprotobuf.a ] ; then
	if [ $# -eq 0 ] ; then echo "protobuf already installed"; return ; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/protobuf/releases/download/v${MVERSION}/protobuf-cpp-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
	cd ${MWORKDIR}
	sh autogen.sh
fi
cd ${MWORKDIR}
./configure --prefix=${IDENTT_SOURCE}/thirdparty && make && make install
}

# Building snappy 1.1.3

function build_snappy () {
local MVERSION=1.1.3
local MSOURCEFILE=${IDENTT_TPSRC}/snappy-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/snappy-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/libsnappy.a ] ; then 
	if [ $# -eq 0 ] ; then echo "snappy already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/snappy/releases/download/${MVERSION}/snappy-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
	cd ${MWORKDIR}
	if [ `uname` = "Darwin" ] ; then
		sed -i .bkp 's/libtool/g&/' ./autogen.sh
	fi
	sh autogen.sh
fi
cd ${MWORKDIR}
./configure --prefix=${IDENTT_SOURCE}/thirdparty && make && make install
}

# Building leveldb 1.19

function build_leveldb () {
local MVERSION=1.19
local MSOURCEFILE=${IDENTT_TPSRC}/leveldb-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/leveldb-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/libleveldb.a ] ; then echo "leveldb already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/leveldb/archive/v${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
PLATFORM_CXXFLAGS="-I ${IDENTT_SOURCE}/thirdparty/include" \
PLATFORM_LDFLAGS="-L ${IDENTT_SOURCE}/thirdparty/lib" \
make \
&& cp out-static/libleveldb.a ${IDENTT_SOURCE}/thirdparty/lib/ \
&& cp -r include/leveldb ${IDENTT_SOURCE}/thirdparty/include/leveldb
}

# Building rocksdb 5.2.1

function build_rocksdb () {
build_rocks_compression 1
local MVERSION=5.2.1
local MSOURCEFILE=${IDENTT_TPSRC}/rocksdb-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/rocksdb-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/include/rocksdb/db.h ] ; then echo "rocksdb already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/facebook/rocksdb/archive/rocksdb-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
	mv rocksdb-rocksdb-${MVERSION} rocksdb-${MVERSION}
fi
cd ${MWORKDIR}
CFLAGS="-I ${IDENTT_SOURCE}/thirdparty/include -L${IDENTT_SOURCE}/thirdparty/lib" \
INSTALL_PATH=${IDENTT_SOURCE}/thirdparty/ make static_lib \
&& INSTALL_PATH=${IDENTT_SOURCE}/thirdparty/ make install
}

# Building compression libs to link rocksdb
function build_rocks_compression () {
mkdir -p ${IDENTT_SOURCE}/thirdparty/lib/rocksdb
build_rocks_compression_zlib $1
build_rocks_compression_bzip2 $1
build_rocks_compression_lz4 $1
}

# Building compression libz to link rocksdb
function build_rocks_compression_zlib () {
local MVERSION=1.2.11
local MSOURCEFILE=${IDENTT_TPSRC}/zlib-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/zlib-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/libz.a ] ; then
	if [ $# -eq 0 ] ; then echo "rocksdb/libz.a already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "http://zlib.net/zlib-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CFLAGS='-fPIC' ./configure --static && make && cp libz.a ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/
}

# Building compression bzip2 to link rocksdb
function build_rocks_compression_bzip2 () {
local MVERSION=1.0.6
local MSOURCEFILE=${IDENTT_TPSRC}/bzip2-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/bzip2-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/libbz2.a ] ; then
	if [ $# -eq 0 ] ; then echo "rocksdb/libbz2.a already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "http://www.bzip.org/${MVERSION}/bzip2-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
make CFLAGS='-fPIC -O2 -g -D_FILE_OFFSET_BITS=64' && cp libbz2.a ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/
}

# Building compression lz4 to link rocksdb
function build_rocks_compression_lz4 () {
local MVERSION=r127
local MSOURCEFILE=${IDENTT_TPSRC}/lz4-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/lz4-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/liblz4.a ] ; then
	if [ $# -eq 0 ] ; then echo "rocksdb/liblz4.a already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://codeload.github.com/Cyan4973/lz4/tar.gz/${MVERSION}"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}/lib
make CFLAGS='-fPIC' all && cp liblz4.a ${IDENTT_SOURCE}/thirdparty/lib/rocksdb/
}

# Building Libsodium 1.0.12

function build_sodium () {
local MVERSION=1.0.12
local MSOURCEFILE=${IDENTT_TPSRC}/libsodium-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/libsodium-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/include/sodium.h ] ; then echo "libsodium already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/jedisct1/libsodium/releases/download/${MVERSION}/libsodium-${MVERSION}.tar.gz"
	fi
	cd ${IDENTT_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CFLAGS="-I ${IDENTT_SOURCE}/thirdparty/include -L${IDENTT_SOURCE}/thirdparty/lib" \
./configure --enable-static=yes --prefix=${IDENTT_SOURCE}/thirdparty && make && make install
}

# Building nanomsg git-5eb7320

function build_nanomsg () {
local MVERSION=git-5eb7320
local MSOURCEFILE=${IDENTT_TPSRC}/nanomsg-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/nanomsg-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/include/nanomsg/nn.h ] ; then echo "nanomsg already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${IDENTT_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/nanomsg/nanomsg.git nanomsg-${MVERSION}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		tar -zcf ${MSOURCEFILE} nanomsg-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${IDENTT_SOURCE}/thirdparty/lib/pkgconfig \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${IDENTT_SOURCE}/thirdparty \
..  && make && make install
}

# Building googletest git-f1a87d7

function build_googletest () {
local MVERSION=git-f1a87d7
local MSOURCEFILE=${IDENTT_TPSRC}/googletest-${MVERSION}.tar.gz
local MWORKDIR=${IDENTT_TEMP}/googletest-${MVERSION}
if [ -f ${IDENTT_SOURCE}/thirdparty/include/googletest/nn.h ] ; then echo "googletest already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${IDENTT_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/google/googletest.git googletest-${MVERSION}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		tar -zcf ${MSOURCEFILE} googletest-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${IDENTT_SOURCE}/thirdparty/lib/pkgconfig \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${IDENTT_SOURCE}/thirdparty \
..  && make && make install
}

### Control ####

build_gperftools
build_protobuf
build_snappy
#build_leveldb
build_rocksdb
build_rocks_compression
build_sodium
build_nanomsg
build_googletest
