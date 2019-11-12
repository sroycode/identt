# Project Identt

Matrix Identity Server

[![Build Status](https://travis-ci.com/sroycode/identt.svg?branch=master)](https://travis-ci.com/sroycode/identt)
[![GitHub (pre-)release](https://img.shields.io/github/release-pre/sroycode/identt.svg)](https://github.com/sroycode/identt)

## Code is WIP

This code is work in progress and is to change frequently.

## What is it

This project seeks to take some of the pain out of setting up a matrix identity server
and should be a drop-in replacement for sydent based on standalone rocksdb
(or leveldb , just change the flag in CMakeLists.txt). 

Supports rudimentary replication. All write queries go to master, read queries are on slave.

Most of the queries closely follow https://matrix.org/docs/spec/identity_service/unstable.html
One main difference is handling of mails and sms to users is handled by an external program. 
There are some additional endpoints to store arbitrary data with accesskey.
See README.EXTRA.md for details

In addition there are some utilities 

- identt_keygen is a keypair generator currently only for ed_25519 signing key
- identt_dumpdb is a db dumper ( dont use in production)
- identt_getmail is a dummy program to extract mail/sms to send 

For replication , the service listens to two ports , you can expose the main port to public, the second port is for
communication between servers.

## Run using Docker

The easiest way to take it for a spin is by using docker , follow the instructions in README.DOCKER.md


## Compiling

For instructions on setting up the development environment see README.COMPILE.md

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

## Changelog

- 0.50: changed compilation docs and added docker , base to ubuntu 
- 0.47: last stable built for centos based systems

## Acknowledgements

This project uses code from

- https://github.com/Tencent/rapidjson
- https://github.com/Amanieu/asyncplusplus
- https://github.com/whoshuu/cpr

- https://github.com/eidheim/Simple-Web-Server ( web server is modified from HTTP Server )
- https://github.com/yinqiwen/pbjson (Protobuf<->Json)

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
