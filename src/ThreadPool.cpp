//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <iostream>
using namespace std;

#include <nb++/ThreadPool.hpp>

namespace nbpp
{
    /*
     * A predicate that returns true if a CommandThread is done.
     */
    class IsDone
    {
    public:
        bool operator()(const CommandThread& thread) const
            throw(AssertException, exception)
        {
            return thread.isDone();
        }
    };

    ThreadPool::ThreadPool() throw(AssertException, exception) :
        startThreads(5), minSpare(5), maxSpare(10),
        maxRequestsPerThread(0), maxThreads(15), idleThreads(0),
        mustCleanUp(false) { }

    ThreadPool::~ThreadPool() throw()
    {
        try
        {
            for (ThreadList::iterator iter = threads.begin(); iter != threads.end(); ++iter)
            {
                if (!iter->isDone() && !iter->wasCanceled())
                {
                    try
                    {
                        iter->cancel();
                    }
                    catch (...) { }
                }
            }
            
            while (!threads.empty())
            {
                Lock lock(mutex);
                while (!mustCleanUp) lock.wait();
    
                mustCleanUp = false;
                threads.remove_if(IsDone());
            }
        }
        catch (...) { }
    }

    void ThreadPool::init() throw(ThreadControlException, AssertException, exception)
    {
        for (unsigned int i = startThreads; i > 0; --i)
        {
            threads.push_back(CommandThread(*this, commandQueue, maxRequestsPerThread));
            threads.back().start();
        }
    }

    void ThreadPool::queue(const Command& cmd)
        throw(ThreadControlException, AssertException, exception)
    {
        // cout << "ThreadPool::queue(): Queueing command" << endl;

        commandQueue.push(cmd);

        // cout << "ThreadPool::queue(): getting lock" << endl;
        Lock lock(mutex);

        if (mustCleanUp)
        {
            mustCleanUp = false;
            // cout << "ThreadPool::queue(): removing finished threads" << endl;
            threads.remove_if(IsDone());
        }

        // If we don't have enough idle threads, add some.
        if (minSpare >= idleThreads &&
            (maxThreads == 0 || threads.size() < maxThreads))
        {
            // cout << "ThreadPool::queue(): adding threads" << endl;
            for (unsigned int i = minSpare - idleThreads + 1; i > 0; --i)
            {
                // cout << "ThreadPool::queue(): adding new threads" << endl;
                threads.push_back(CommandThread(*this, commandQueue, maxRequestsPerThread));
                threads.back().start();
            }
        }
    }

    CommandQueue ThreadPool::getCommandQueue() const throw(AssertException, exception)
    {
        return commandQueue;
    }

    unsigned int ThreadPool::getIdleThreads() const throw(AssertException, exception)
    {
        return idleThreads;
    }

    void ThreadPool::switchState(State oldState, State newState)
        throw(AssertException, exception)
    {
        Lock lock(mutex);

        if (oldState == Waiting)
        {
            --idleThreads;
        }

        if (newState == Waiting)
        {
            ++idleThreads;
        }
        else if (newState == Finished)
        {
            mustCleanUp = true;
            lock.notifyAll();
        }
    }

    bool ThreadPool::canWait() throw(AssertException, exception)
    {
        Lock lock(mutex);
        return !maxSpare || !commandQueue.empty() || (idleThreads < maxSpare);
    }

    unsigned int ThreadPool::getStartThreads() const throw(AssertException, exception)
    {
        Lock lock(mutex);
        return startThreads;
    }

    ThreadPool& ThreadPool::setStartThreads(unsigned int arg_startThreads)
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        startThreads = arg_startThreads;
        return *this;
    }

    unsigned int ThreadPool::getMinSpare() const throw(AssertException, exception)
    {
        Lock lock(mutex);
        return minSpare;
    }

    ThreadPool& ThreadPool::setMinSpare(unsigned int arg_minSpare)
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        minSpare = arg_minSpare;
        return *this;
    }

    unsigned int ThreadPool::getMaxSpare() const throw(AssertException, exception)
    {
        Lock lock(mutex);
        return maxSpare;
    }

    ThreadPool& ThreadPool::setMaxSpare(unsigned int arg_maxSpare)
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        maxSpare = arg_maxSpare;
        return *this;
    }

    unsigned int ThreadPool::getMaxRequestsPerThread() const
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        return maxRequestsPerThread;
    }

    ThreadPool& ThreadPool::setMaxRequestsPerThread(unsigned int arg_maxRqPerThr)
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        maxRequestsPerThread = arg_maxRqPerThr;
        return *this;
    }

    unsigned int ThreadPool::getMaxThreads(unsigned int arg_maxThreads) const
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        return maxThreads;
    }

    ThreadPool& ThreadPool::setMaxThreads(unsigned int arg_maxThreads)
        throw(AssertException, exception)
    {
        Lock lock(mutex);
        maxThreads = arg_maxThreads;
        return *this;        
    }
}
