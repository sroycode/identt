# Project Identt

Matrix Identity Server

## Code is WIP

This code is work in progress and is to change frequently.

## What is it

This project seeks to take some of the pain out of setting up a matrix identity server
and should be a drop-in replacement for sydent based on standalone rocksdb
(or leveldb , just change the flag in CMakeLists.txt). 

Most of the queries closely follow https://matrix.org/docs/spec/identity_service/unstable.html

In addition there are some utilities

- identt_keygen is a keypair generator currently only for ed_25519 signing key
- identt_dumpdb is a db dumper ( dont use in production)
- identt_getmail is a dummy program to extract mail/sms to send 

One main difference is handling of mails and sms to users is handled by an external program. 
See README.MAILS.md for details

## Creating Development Environment

You need to install these tools using standard utils.
- cmake 3+,
- boost 1.58+
- clang++/gcc5 ( supporting C++11)
- autoconf automake etc ( for thirdparty tools )
- icu4c
- glog and gflags.
- openssl 1.0.0+ ( for https)
- libcurl 7.0+ ( for tools)
- httpie for testing

The ones below can be compiled from source if you need,there is a helper `scripts/thirdparty.sh`
- gperftools 2.5
- protobuf 3.1.0
- snappy 1.1.3
- rocksdb compression libs zlib , bzip2 , lz4 - preferably static (*)
- rocksdb 5.2.1  ( script installs above headers ) (*)

Items marked with asterix (*) should preferably be compiled from source
On mac the homebrew versions work. For Centos 7 see README.LINUX.md.

## Installing thirdparty packages using scripts/thirdparty.sh

Please set the environment ( See .profile.centos7 and .profile.mac ). There are two environment variables
`IDENTT_SOURCE` should be set to the top level directory of the package. 
`IDENTT_TPSRC` is where the source code packages are downloaded ( defaults to /opt/backup ).
Order is important since rocksdb needs snappy.

## Compiling

Create a build directory

```
mkdir build
cd build
# Debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
# Release
cmake .. && make
```

On Linux with cmake3 and custom Boost ( See README.LINUX.md )

```
# Debug
cmake3 -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=/opt/local/boost .. && make
# Release
cmake3 -DBOOST_ROOT=/opt/local/boost .. && make
```

## Running

1. You need to have a config file , see README.CONFIG.md for details. There is a sample in etc for you to edit

2. Start the server 


```
./identtserver --config ../etc/identt.conf
```

## Testing

Look at the folder test for expected query.
The test/test_ scripts are for testing, you will need httpie.

For example , mobile no verification looks like this ( run from build dir )

```
sh ../test/test_validate_msisdn_requestToken.sh 
sh ../test/test_validate_msisdn_submitToken.sh # fails
sh ../test/test_tools_getmail.sh # get token and sid
token=3c512048-2ef9-43e4-8e7b-1ce72584b337 sid=2 sh ../test/test_validate_msisdn_submitToken.sh
token=3c512048-2ef9-43e4-8e7b-1ce72584b337 sid=2 sh ../test/test_3pid_getValidated3pid.sh 
token=3c512048-2ef9-43e4-8e7b-1ce72584b337 sid=2 sh ../test/test_3pid_bind.sh 
```

## Acknowledgements

This project uses code from

- https://github.com/Tencent/rapidjson
- https://github.com/Amanieu/asyncplusplus
- https://github.com/whoshuu/cpr ( tools only)
- https://github.com/eidheim/Simple-Web-Server ( web server is modified from HTTP Server )
- https://github.com/yinqiwen/pbjson (Protobuf<->Json)
- https://github.com/r-lyeh/sole

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
