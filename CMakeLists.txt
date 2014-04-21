CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT( nbpp )

INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckLibraryExists)
INCLUDE( FindDoxygen )
INCLUDE( CheckCSourceCompiles )

CHECK_C_SOURCE_COMPILES("int main() {int x = 0; __sync_fetch_and_add(&x, 1);__sync_sub_and_fetch( &x, 1 );} " NBPP_HAVE_ATOMIC )

# Set the library version
SET( NBPP_VERSION_MAJOR 0 )
SET( NBPP_VERSION_MINOR 7 )
SET( NBPP_VERSION_REV 8 )
SET( NBPP_VERSION ${NBPP_VERSION_MAJOR}.${NBPP_VERSION_MINOR}.${NBPP_VERSION_REV} )

SET( CMAKE_CXX_FLAGS "-std=c++98 -pthread" )

#SET( _REENTRANT 1 )
#SET( _GNU_SOURCE 1 )  # GNU source extentions

# Check the system and make config.h

CHECK_INCLUDE_FILES( fcntl.h NBPP_HAVE_FCNTL_H )
CHECK_INCLUDE_FILES( unistd.h NBPP_HAVE_UNISTD_H )
CHECK_INCLUDE_FILES( time.h NBPP_HAVE_TIME_H )
CHECK_INCLUDE_FILES( sys/time.h NBPP_HAVE_SYS_TIME_H )
CHECK_INCLUDE_FILES( sys/select.h NBPP_HAVE_SYS_SELECT_H )
CHECK_INCLUDE_FILES( sys/socket.h NBPP_HAVE_SYS_SOCKET_H )
CHECK_INCLUDE_FILES( pthread.h NBPP_HAVE_PTHREAD_H )
CHECK_INCLUDE_FILES( sys/resource.h NBPP_HAVE_SYS_RESOURCE_H )
CHECK_INCLUDE_FILES( getopt.h NBPP_HAVE_GETOPT_H )
CHECK_INCLUDE_FILES( curl/curl.h NBPP_HAVE_CURL_CURL_H )

CHECK_FUNCTION_EXISTS( regcomp NBPP_HAVE_REGCOMP )
CHECK_FUNCTION_EXISTS( select NBPP_HAVE_SELECT )
CHECK_FUNCTION_EXISTS( socket NBPP_HAVE_SOCKET )
CHECK_FUNCTION_EXISTS( uname NBPP_HAVE_UNAME )
CHECK_FUNCTION_EXISTS( hstrerror NBPP_HAVE_HSTRERROR )
CHECK_FUNCTION_EXISTS( pthread_attr_setdetachstate NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE )
CHECK_FUNCTION_EXISTS( pthread_attr_getdetachstate NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE )
CHECK_FUNCTION_EXISTS( getrlimit NBPP_HAVE_GETRLIMIT )
CHECK_FUNCTION_EXISTS( setsid NBPP_HAVE_SETSID )
CHECK_FUNCTION_EXISTS( setpgrp NBPP_HAVE_SETPGRP )
CHECK_FUNCTION_EXISTS( sigaction NBPP_HAVE_SIGACTION )
CHECK_FUNCTION_EXISTS( signal NBPP_HAVE_SIGNAL )
CHECK_FUNCTION_EXISTS( getopt_long NBPP_HAVE_GETOPT_LONG )
CHECK_FUNCTION_EXISTS( sigprocmask NBPP_HAVE_SIGPROCMASK )
CHECK_FUNCTION_EXISTS( sigblock NBPP_HAVE_SIGBLOCK )

#CHECK_LIBRARY_EXISTS(gcc main "" _CMA_NOWRAPPERS_) # HP ux stuff
CHECK_LIBRARY_EXISTS( pthread pthread_create "" NBPP_HAVE_LIBPTHREAD )
CHECK_LIBRARY_EXISTS( dce pthread_create "" NBPP_HAVE_LIBDCE )
CHECK_LIBRARY_EXISTS( cma pthread_create "" NBPP_HAVE_LIBCMA )

# Ok Try to find the propper select argument types (from the openal prj)

IF( NOT SELECT_ARGTYPES_FOUND )
  MESSAGE(STATUS "Determining select arguments")
  SET(ARG234 "fd_set *;int *;void *")
  SET(ARG1 "int;size_t;unsigned long;unsigned")
  SET(ARG5 "struct timeval *;const struct timeval *")

  FOREACH(CUR_ARG234 ${ARG234})
    IF(NOT SELECT_ARGTYPES_FOUND)
      FOREACH(CUR_ARG1 ${ARG1})
        IF(NOT SELECT_ARGTYPES_FOUND)
          FOREACH(CUR_ARG5 ${ARG5})
            IF(NOT SELECT_ARGTYPES_FOUND)

              CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/cmake/modules/SelectArgtypes.c.in
                ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp/SelectArgtypes.c IMMEDIATE)

              FILE(READ ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp/SelectArgtypes.c SELECT_SRC )

              TRY_COMPILE(SELECT_ARGTYPES_FOUND
                ${CMAKE_BINARY_DIR}
                ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp/SelectArgtypes.c
                CMAKE_FLAGS
                "-DCOMPILE_DEFINITIONS:STRING=${SOURCE_DEFINITIONS}"
                OUTPUT_VARIABLE OUTPUT)

              FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
                "Performing C SOURCE FILE Test SELECT_ARGTYPES with the following output:\n"
                "${OUTPUT}\n")

              IF(SELECT_ARGTYPES_FOUND)
                MESSAGE(STATUS "Determining select arguments - found")
                SET(SELECT_TYPE_ARG1 ${CUR_ARG1} CACHE INTERNAL "")
                SET(SELECT_TYPE_ARG234 ${CUR_ARG234} CACHE INTERNAL "")
                SET(SELECT_TYPE_ARG5 ${CUR_ARG5} CACHE INTERNAL "")
              ENDIF(SELECT_ARGTYPES_FOUND)

            ENDIF(NOT SELECT_ARGTYPES_FOUND)
          ENDFOREACH(CUR_ARG5)
        ENDIF(NOT SELECT_ARGTYPES_FOUND)
      ENDFOREACH(CUR_ARG1)
    ENDIF(NOT SELECT_ARGTYPES_FOUND)
  ENDFOREACH(CUR_ARG234)
ENDIF(NOT SELECT_ARGTYPES_FOUND)

IF(NOT SELECT_ARGTYPES_FOUND)
  MESSAGE(STATUS "Determining select arguments - using fallback")
  SET(SELECT_TYPE_ARG1 "int" CACHE INTERNAL "")
  SET(SELECT_TYPE_ARG234 "int *" CACHE INTERNAL "")
  SET(SELECT_TYPE_ARG5 "struct timeval *" CACHE INTERNAL "")
ENDIF(NOT SELECT_ARGTYPES_FOUND)

# Detect signal type
include(cmake/modules/TestSignalType.cmake)

CONFIGURE_FILE( include/nb++/config.h.cmake include/nb++/config.h )
CONFIGURE_FILE( nbpp.pc.cmake nbpp.pc )
CONFIGURE_FILE( doc/doxygen.config.cmake doc/doxygen.config )

FILE( MAKE_DIRECTORY doc/html ) 
ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN_EXECUTABLE} doc/doxygen.config )
#ADD_COMMAND( OUTPUT doc/html/index.html )

INSTALL( DIRECTORY doc/html DESTINATION share/doc/nbpp-dev/html
         PATTERN ".svn" EXCLUDE )

INSTALL( DIRECTORY include/ DESTINATION include
         PATTERN ".svn" EXCLUDE 
         PATTERN "include/*.h*" )

INSTALL( FILES nbpp.pc DESTINATION lib/pkgconfig ) 

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( utils )
ADD_SUBDIRECTORY( samples )
