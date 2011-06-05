//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD_PER_COMMAND_IMPL
#define _NBPP_THREAD_PER_COMMAND_IMPL

#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
	/**
	   This class handle one command in one single thread. When a command
	   is found in the queue, this class will create a new thread and make this 
	   thread make the work, and when its all done the thread will be destroyed.

	   This is a somehow expencive way to allocate threads, but it more simple 
	   and therefor may be more usable i debug situations.

	   @see MultiThreader
	 */ 
    class ThreadPerCommandImpl : public ThreaderImpl
    {
    public:
        ThreadPerCommandImpl();

		/**
		   Add a new command into the command queue, for thread processing.

		   @param command the command object to execute
		 */
		void queue( Command command );
            
    protected:
        void doInit();
    };

    typedef RefHandle<ThreadPerCommandImpl> ThreadPerCommandHandle;
}

#endif /* _NBPP_THREAD_PER_COMMAND_IMPL */
