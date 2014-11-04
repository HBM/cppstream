# HBM Daq Stream Client Examples
Implements an HBM Daq Stream Client and some simple example programs using C++

## License

Copyright (c) 2014 Matthias Loy. See the LICENSE file for license rights and limitations (AFL 3.0).

## Prerequisites

### Sources 
All sources of the example programs are to be found on the [HBM Daq Stream Examples github project page](https://github.com/HBM-Team/cppstream "")

### Used Libraries
We try to use as much existing and prooved software as possbile in order to keep implementation and testing effort as low as possible. All libraries used carry a generous license. See the licenses for details.

The open source project jsoncpp is being used as JSON composer and parser. For Linux, simply install the jsoncpp development package of your distribution. For Windows, download the source from the [jsoncpp github project page](https://github.com/open-source-parsers/jsoncpp "") and put them beside the directory of this project.

Some example programs do use the Boost libraries. Refer to [boost](http://www.boost.org/ "") for details.
For Linux, simply install the Boost development packages of your distribution. For Windows, the projects are tailored to link against the prebuilt boost binaries from [boost](http://www.boost.org/ "").
Download and install the binaries and set the '`BOOST_ROOT`' environment variable to the installation directory.


### Build System
#### Linux
Under Linux the cmake build system is being used. Install it using your distribution package system. Create a sub directory inside the project directory. change into this subdirectory and call '`cmake ..`'. Execute '`make`' afterwards to build all libraries and executables.
Tested with gcc 4.6


#### Windows
A solution for MSVC2010 is provided.

## Library
Center of all is a library containing everything needed to implement an HBM Daq Stream Client. Sources are to be found below `lib`. Headers with the interface headers are to be found under `include`.

The most important class is `StreamClient`. It encapsulates the communication with a Daq Stream server. It collects and keeps stream related meta information received from the Daq Stream Server. It provides methods for subscribing and unsubscribing signals from the Daq Stream Server. 

`Signals` holds a container of objects representing all subscribed signals from a Daq Stream Server. The `SubScribedSignal` class processes signal related meta information and measured values.

## Examples
There are simple example programs using the library.

### Example Client
This program connects to a HBM Daq Streaming socket, receives everything that arrives, subscribes all signals, interprets stream and signal related meta information and measured data. It is started with at least one paramter telling about the address of the HBM Daq Stream server. 
'`./exampleclient.bin 172.19.3.4`'

call without parameter or with parameter '`-h`' to get help.

### Reconnect
Works as example client but stops and restarts the streaming client periodically. This program relies on the boost thread library.

### Multiple
Works as example client but creates several instances of the streaming client in order to connect to several streaming servers. This program relies on the boost thread library.
