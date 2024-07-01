# cpps3rv

This project aims to provide a clear, simple template for a C++ High performance Linux TCP server.

It uses state of the art (as of 2024) paradigms in order to ensure the highest possible performance regarding
connections handling and I/O management. Data processing is still up to you, 
however cpps3rv aims to simplify the work for you also on this part, 
leading through the correct design pattern.

# Specifications

* SSL Native.
* Multithreaded.
* Asychronous I/O, epoll() polling mechanism.
* Lock-free communication mechanism between I/O threads and business logics.
* Using of 'modern' c++ language paradigms as much as possible.

# Dependancies

cpps3rv has the follwong dependancies:

* libssl
* cameron314/concurrentqueue
* conan (build)
* cmake (build)

# Building

Assuming that all the dependancies are properly installed on our system, here's how to build cpps3rv:

```mkdir build
   cd build 
   cmake ../ && make```

# Usage

# Benchmarks