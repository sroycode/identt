# Compile from Source

## Creating Development Environment

You need to install these tools using standard utils.
- cmake 3+,
- boost 1.62+
- clang++/gcc5 ( supporting C++11)
- autoconf automake etc ( for thirdparty tools )
- icu4c
- glog and gflags.
- libcurl 7.0+ 
- httpie for testing

The ones below can be compiled from source if you need (Centos 7) ,there is a helper `scripts/thirdparty.sh`
- gperftools 2.5
- protobuf 3.5.0
- snappy 1.1.7
- rocksdb 5.6
- libsodium 1.0.16

On mac +homebrew and ubuntu-18 +apt default versions work fine.


### For Mac +homebrew

```
brew install libsodium libicu icu4c curl-openssl rocksdb protobuf gperf snappy ctemplate
```

### For Ubuntu 18

```
sudo add-apt-repository ppa:exonum/rocksdb
sudo add-apt-repository ppa:maarten-fonville/protobuf
sudo add-apt-repository ppa:xapian-backports/ppa
sudo add-apt-repository ppa:jtv/ppa
sudo apt update -y
sudo apt install -y \
	wget ccache cmake make libtool pkg-config g++ gcc autoconf automake curl jq lcov \
	protobuf-compiler vim-common libboost-all-dev libboost-all-dev libcurl4-openssl-dev zlib1g-dev liblz4-dev libprotobuf-dev \
	libgoogle-glog-dev libgflags-dev libgoogle-perftools-dev libsnappy-dev libbz2-dev libz-dev libctemplate-dev \
	libtbb-dev libzstd-dev libsodium-dev librocksdb-dev
```

