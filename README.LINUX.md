# Environment for Centos 7

## Creating Development Environment

- You need cmake 3+, boost 1.58+, gcc5, glog,gflags, curl for compiling
- On Centos 7 you will need devtoolset-4 and cmake3 , please see the next section for details.
- use the .profile.centos7

## Creating Development Environment on Centos 7

Set up the the compiler environment

```
. .profile.centos7
mkdir -p $IDENTT_TEMP
sudo yum install centos-release-scl
sudo yum install devtoolset-4-gcc devtoolset-4-gcc-c++ cmake3 glog-devel libunwind-devel libicu-devel libcurl-devel
# Build boost 1.62 from source
. /opt/rh/devtoolset-4/enable
wget -q -O boost_1_62_0.tar.bz2 "https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.bz2/download"
tar -jxf boost_1_62_0.tar.bz2
cd boost_1_62_0
sh bootstrap.sh --prefix=/opt/local/boost
sudo ./b2 install --prefix=/opt/local/boost
```

Before setting up the thirdparty libraries and compiling this sets the path and alias for cmake

```
. .profile.centos7
```
