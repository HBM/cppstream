#!/usr/bin/env sh
# This is called by `.travis.yml` via Travis CI.

# -e: fail on error
# -v: show commands
# -x: show expanded commands
set -vex

git submodule update --init --recursive --remote
cd jsoncpp
pwd
cmake .
make
sudo make install
cd ..
rm -rf build
mkdir build
cd build
cmake ../
make
if [ "$COVERITY_SCAN_BRANCH" != 1 ]; then
  valgrind ./test/teststreamclient.bin
fi

