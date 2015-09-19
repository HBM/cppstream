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
if [ "$COVERITY_SCAN_BRANCH" != 1 ]; then
  cmake ../ && make && make test ;
fi

