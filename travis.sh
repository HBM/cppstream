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
mkdir build
cd build
cmake ../
make
if [ "$COVERITY_SCAN_BRANCH" != 1 ]; then
  valgrind --error-exitcode=42 --leak-check=full ./test/teststreamclient.bin
fi

