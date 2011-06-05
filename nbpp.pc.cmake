prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_PREFIX@/lib
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: nb++
Description: nb++ is a nuts-and-bolts library for C++  
Requires: 
Version: @NBPP_VERSION@
Libs: -L@CMAKE_INSTALL_PREFIX@/lib -lnbpp -lpthread
Cflags: -I@CMAKE_INSTALL_PREFIX@/include @CPPFLAGS@
