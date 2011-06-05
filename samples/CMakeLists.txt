PROJECT( nbpp_samples )

LINK_LIBRARIES( nbpp )
INCLUDE_DIRECTORIES( ../include ../include/nb++ )

ADD_EXECUTABLE( handletest  handletest-main.cpp 
                Foo.cpp FooImpl.cpp SubFooImpl.cpp 
                FooClient.cpp FooCache.cpp Factory.cpp 
                Foo.hpp FooImpl.hpp SubFooImpl.hpp 
                FooClient.hpp FooCache.hpp 
                Factory.hpp )
ADD_EXECUTABLE( circular circular.cpp )
ADD_EXECUTABLE( threadtest threadtest.cpp )
ADD_EXECUTABLE( eventtest eventtest.cpp )
ADD_EXECUTABLE( hostinfotest hostinfotest.cpp )
ADD_EXECUTABLE( socktest socktest.cpp )
#ADD_EXECUTABLE( serversocktest serversocktest.cpp )
ADD_EXECUTABLE( regextest regextest.cpp )
ADD_EXECUTABLE( recursivemutextest recursivemutextest.cpp )
ADD_EXECUTABLE( configtest configtest.cpp )
ADD_EXECUTABLE( splittest splittest.cpp )
ADD_EXECUTABLE( tempfiletest tempfiletest.cpp )
#ADD_EXECUTABLE( threadtest_multi threadtest-multi.cpp )
ADD_EXECUTABLE( daemontest daemontest.cpp )
ADD_EXECUTABLE( optiontest optiontest.cpp )
ADD_EXECUTABLE( linecount linecount.cpp )
ADD_EXECUTABLE( httpsvr httpsvr.cpp )
ADD_EXECUTABLE( ThreaderTest ThreaderTest.cpp )
ADD_EXECUTABLE( nburl nburl.cpp )

