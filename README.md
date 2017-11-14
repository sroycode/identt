# Project Identt

Matrix Identity Server

## Code is WIP

This code is work in progress and is to change frequently.
Major change: server-to-server port is now separate from matrix port

## What is it

This project seeks to take some of the pain out of setting up a matrix identity server
and should be a drop-in replacement for sydent based on standalone rocksdb
(or leveldb , just change the flag in CMakeLists.txt). 

Supports rudimentary replication. All write queries go to master, read queries are on slave.

Most of the queries closely follow https://matrix.org/docs/spec/identity_service/unstable.html

In addition there are some utilities

- identt_keygen is a keypair generator currently only for ed_25519 signing key
- identt_dumpdb is a db dumper ( dont use in production)
- identt_getmail is a dummy program to extract mail/sms to send 

One main difference is handling of mails and sms to users is handled by an external program. 
See README.MAILS.md for details

For replication , the service listens to two ports , you can expose the main port to public, the second port is for
communication between servers.

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

On Centos with cmake3 and custom Boost ( See README.LINUX.md )

```
# Debug
cmake3 -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=/opt/local/boost .. && make
# Release
cmake3 -DBOOST_ROOT=/opt/local/boost .. && make
```

Note the number of concurrent connections is limited by number of active threads, you can adjust it
by setting `LIBASYNC_NUM_THREADS` as an environment variable before compiling.

```
export LIBASYNC_NUM_THREADS=30
```

## Running

1. You need to have a config file , see README.CONFIG.md for details. There is a sample in etc for you to edit

2. Start the server ( default starts in master mode) . Default config listens 9091 for matrix + 9092 for intermachine

```
./identtserver --config ../etc/identt.conf
```

3. To start another server in slave mode (optional) pointing to master intermachine port started at port 9092 of same machine

```
./identtserver --config ../etc/identt2.conf --master="http://127.0.0.1:9092"
```

## Testing

Look at the folder test for expected query.
The test/test_ scripts are for testing, you will need httpie.

For example , mobile no verification looks like this ( run from build dir , for simplicity cut-pasted variables from log)

```
sh ../test/test_validate_msisdn_requestToken.sh 
sh ../test/test_validate_msisdn_submitToken.sh # fails
sh ../test/test_tools_getmail.sh # get token and sid
phone_number="919999999999" token="512048" sid="1112" sh ../test/test_validate_msisdn_submitToken.sh
phone_number="919999999999" token="512048" sid="1112" sh ../test/test_3pid_getValidated3pid.sh 
phone_number="919999999999" token="512048" sid="1112" sh ../test/test_3pid_bind.sh 
```

## Acknowledgements

This project uses code from

- https://github.com/Tencent/rapidjson
- https://github.com/Amanieu/asyncplusplus
- https://github.com/whoshuu/cpr

- https://github.com/eidheim/Simple-Web-Server ( web server is modified from HTTP Server )
- https://github.com/yinqiwen/pbjson (Protobuf<->Json)

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
