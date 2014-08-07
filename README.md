HBM Daq Stream Example
=========

This is an example client for HBM Daq Stream protocol. 

It connects to a HBM Daq Streaming socket, receives everything that arrives, subscribes signals, interprets meta information and measured data.

The open source project jsoncpp is being used as JSON composer and parser. 

Some example programs do use Boost libraries. Refer to www.boost.org for details. 

For Linux, simply install the Boost development packages of your distribution. For Windows, the projects are tailored to link against the prebuilt boost binaries from www.boost.org.
Download and install the binaries and set the BOOST_ROOT environment variable to the installation directory.
