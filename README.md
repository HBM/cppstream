# HBM Daq Stream Examples






## prerequisites

### Libraries
The open source project jsoncpp is being used as JSON composer and parser. For Linux, simply install the jsoncpp development package of your distribution. For Windows, download the source from 'https://github.com/open-source-parsers/jsoncpp' and put them beside the directory of this project.

Some example programs do use the Boost libraries. Refer to www.boost.org for details.
For Linux, simply install the Boost development packages of your distribution. For Windows, the projects are tailored to link against the prebuilt boost binaries from www.boost.org.
Download and install the binaries and set the BOOST_ROOT environment variable to the installation directory.


### minimum compiler version
* gcc >= 4.6
* MSVC >= 2010

### Build System
Under Linux the cmake build system is being used. A solution for MSVC2010 is provided.


## ExampleClient
This is a simple example client for HBM Daq Stream protocol. 

It connects to a HBM Daq Streaming socket, receives everything that arrives, subscribes all signals, interprets stream and signal related meta information and measured data.

