# HBM Daq Stream Examples

## prerequisites

### Sources 
All sources of the example programs are to be found on the [HBM Daq Stream Examples github project page](https://github.com/HBM-Team/cppstream "")

### Libraries
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


## Example Client
This is a simple example client for HBM Daq Stream protocol. 

This program connects to a HBM Daq Streaming socket, receives everything that arrives, subscribes all signals, interprets stream and signal related meta information and measured data.
It is started with at least one paramter telling about the address of the HBM Daq Stream server. 
'`./exampleclient.bin 172.19.3.4`'

call without parameter or with parameter '`-h`' to get help.


## Test Reconnect
Works as example client but stops and restarts the streaming client periodically.

## Test Multiple
Works as example client but creates several instances of the streaming client in order to connect to several streaming servers.
