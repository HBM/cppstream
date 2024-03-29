# $${\color{red}MOVED}$$
# https://github.com/hbkworld/cppstream


# HBM Daq Stream Client Examples
Implements an HBM Daq Stream Client and some simple example programs using C++.

## License
Copyright (c) 2014 Hottinger Baldwin Messtechnik. See the [LICENSE](LICENSE) file for license rights and limitations.

## Build Status
[![Travis CI](https://travis-ci.org/HBM/cppstream.svg?branch=master)](https://travis-ci.org/HBM/cppstream)

[![Coverity](https://scan.coverity.com/projects/6402/badge.svg)](https://scan.coverity.com/projects/6402)

## Prerequisites

### Sources 
All sources of the example programs are to be found on the [HBM Daq Stream Examples github project page](https://github.com/HBM/cppstream "").

### Used Libraries
We try to use as much existing and prooved software as possbile in order to keep implementation and testing effort as low as possible. All libraries used carry a generous license. See the licenses for details.

The open source project jsoncpp is being used as JSON composer and parser. Version 1.6.2 is included as submodule. Use the following git command in order to get the code:
`git submodule update --init --recursive`. Please make sure to choose Visual Studio 2013 as platform toolset (to be found in the project configuration properties under "General").

The unit tests provided do use the Boost libraries (1.55). Refer to [boost](http://www.boost.org/ "") for details.
For Linux, simply install the Boost development packages of your distribution. For Windows, the projects are tailored to link against the prebuilt boost binaries from [boost](http://www.boost.org/ "").
Download and install the binaries and set the '`BOOST_ROOT`' environment variable to the installation directory.


### Build System
#### Linux
Under Linux the cmake build system is being used. Install it using your distribution package system. Create a sub directory inside the project directory. Change into this subdirectory and call '`cmake ..`'. Execute '`make`' afterwards to build all libraries and executables.
Tested with gcc 4.8.2 and 4.9.2.


#### Windows
A solution for MSVC2012 is provided.

## Library
Center of all is a library containing everything needed to implement an HBM Daq Stream Client. Sources are to be found below `lib`. Headers needed to interface with the library are to be found under `include`.

The most important class is `StreamClient`. It encapsulates the communication with a Daq Stream server. It collects and keeps stream related meta information received from the Daq Stream Server. It provides methods for subscribing and unsubscribing signals from the Daq Stream Server. 

`Signals` holds a container of objects representing all subscribed signals from a Daq Stream Server. The `SubScribedSignal` class processes signal related meta information and measured values.

## Examples
There are simple example programs using the library.

### Example Client
This program connects to a HBM Daq Streaming socket, receives everything that arrives, subscribes all signals, interprets stream and signal related meta information and measured data. It is started with at least one paramter telling about the address of the HBM Daq Stream server. 
'`./exampleclient.bin 172.19.3.4`'

Call without parameter or with parameter '`-h`' to get help.

### Reconnect
Works as example client but stops and restarts the streaming client periodically.
