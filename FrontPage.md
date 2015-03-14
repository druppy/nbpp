# nb++ #

If you need a C++ framework to assist you in making multi threaded (replaceable thread handler, using command/execute pattern) daemons, handle both socket listers (even HTTP), and socket communication as real c++ streams, and stuff like this, nb++  may be a tool for you.

## Features ##

It includes classes for basic operating-system features, like these

  * command line options
  * daemon tool
    * Signal handling
  * config file handling
  * reference counting
  * sockets
    * real C++ stream buffer
  * Build in small http 1.1 server
    * Multiform handling
  * smtp client handler
  * threads
    * pthread c++ wrapping
      * Mutex
      * RWMutex
      * Scope Lock
    * Command / execute pattern
  * url class inspired by the Java framwork
    * may use curl to handle connections
  * regular expressions
    * Pure posix
  * String tools
    * base64
    * split,trim and more
  * replaceable threading strategies
    * Single
    * Threaded
    * Thread pool
    * Forked
  * thread safety

## Future plans ##

  * Dynamic type system
  * Json RPC and Rest handling
  * common database handling
  * [C++11 rewrite](Cpp11Plans.md)