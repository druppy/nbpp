PROJECT( nbpp )

INCLUDE( FindPkgConfig )

pkg_check_modules( CURL libcurl ) 

INCLUDE_DIRECTORIES( ../include ${CURL_INCLUDE_DIRS})
LINK_DIRECTORIES( ${CURL_LIBRARY_DIRS})
ADD_DEFINITIONS( ${CURL_CFLAGS} )

ADD_LIBRARY( nbpp SHARED 
             Exception.cpp 
             Thread.cpp ThreadImpl.cpp 
             Base64.cpp Command.cpp 
             CommandQueue.cpp CommandQueueImpl.cpp 
             CommandThread.cpp 
             ThreadObserver.cpp 
             ThreadPool.cpp 
             Threader.cpp ThreaderImpl.cpp 
             ForkThreader.cpp ForkThreaderImpl.cpp 
             SingleThreader.cpp SingleThreaderImpl.cpp 
             MultiThreader.cpp MultiThreaderImpl.cpp 
             ThreadPerCommand.cpp ThreadPerCommandImpl.cpp 
             Mutex.cpp RWLocker.cpp 
             NetworkAddress.cpp NetworkAddressImpl.cpp 
             InetAddress.cpp InetAddressImpl.cpp 
             UnixAddress.cpp UnixAddressImpl.cpp 
             SocketBaseImpl.cpp 
             Socket.cpp SocketImpl.cpp 
             ServerSocket.cpp ServerSocketImpl.cpp 
             Regex.cpp RegexImpl.cpp 
             RegexResult.cpp RegexResultImpl.cpp 
             RegexSubResult.cpp String.cpp 
             SignalManager.cpp 
             DaemonBase.cpp Daemon.cpp 
             ConfigFile.cpp 
             TempFile.cpp TempFileImpl.cpp 
             CommandLineOptions.cpp 
             ExceptionGuard.cpp 
             osdep.cpp Log.cpp 
             HTTPServer.cpp Url.cpp 
             CurlScheme.cpp
             SmtpClient.cpp
             Multipart.cpp )

SET_TARGET_PROPERTIES( nbpp PROPERTIES VERSION ${NBPP_VERSION} SOVERSION ${NBPP_VERSION_MAJOR} )
TARGET_LINK_LIBRARIES( nbpp -lpthread ${CURL_LDFLAGS} )

INSTALL(TARGETS nbpp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib )
