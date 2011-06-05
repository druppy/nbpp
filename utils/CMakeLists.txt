PROJECT( nbpp_utils )

LINK_LIBRARIES( nbpp )
INCLUDE_DIRECTORIES( ../include )

ADD_EXECUTABLE( tl tl.cpp )
ADD_EXECUTABLE( nblog nblog.cpp )

INSTALL( TARGETS tl nblog 
         RUNTIME DESTINATION bin )
