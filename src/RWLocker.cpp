//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <nb++/RWLocker.hpp>

namespace nbpp
{
    RWLocker::RWLocker() throw(AssertException, exception) :
        readerCount(0), mutex(new Mutex) { }
    
    RWLocker::RWLocker(Mutex* arg_mutex) throw(AssertException, exception) :
        readerCount(0), mutex(arg_mutex) { }

    RWLocker::~RWLocker() throw()
    {
        delete mutex;
    }

    void RWLocker::getReadLock() throw(AssertException, exception)
    {
        // Lock the mutex.
        Lock lock(*mutex);

        // Increment the number of readers.
        ++readerCount;
    }

    void RWLocker::releaseReadLock() throw(AssertException, exception)
    {
        // Lock the mutex.
        Lock lock(*mutex);

        // Decrement the number of readers.
        DEBUG(Assert (readerCount != 0,
                      AssertException ("nbpp::RWLocker::releaseReadLock(): "
                                       "RWLocker already unlocked")));
        --readerCount;

        // Notify other threads waiting for this mutex.
        lock.notifyAll();
    }

    void RWLocker::getWriteLock(Lock& lock) throw(AssertException, exception)
    {
        // Get the write lock.
        lock.get();

        // Wait until there are no more readers.
        while (readerCount > 0)
            lock.wait();
    }

    void RWLocker::releaseWriteLock(Lock& lock) throw(AssertException, exception)
    {
        lock.release();
    }

    RecursiveRWLocker::RecursiveRWLocker() throw(AssertException, exception) :
        RWLocker(new RecursiveMutex()) { }

    RecursiveRWLocker::~RecursiveRWLocker() throw() { }

    ReadLock::ReadLock(RWLocker& arg_rwLocker, bool autoLock)
        throw(AssertException, exception) :
        rwLocker(arg_rwLocker), locked(false)
    {
        if (autoLock) get();
    }

    ReadLock::~ReadLock() throw()
    {
        try
        {
            release();
        }
        catch (...) { }
    }

    void ReadLock::get() throw(AssertException, exception)
    {
        if (!locked)
        {
            rwLocker.getReadLock();
            locked = true;
        }
    }

    void ReadLock::release() throw(AssertException, exception)
    {
        if (locked)
        {
            rwLocker.releaseReadLock();
            locked = false;
        }
    }

    WriteLock::WriteLock(RWLocker& arg_rwLocker, bool autoLock)
        throw(AssertException, exception) :
        rwLocker(arg_rwLocker), lock(*rwLocker.mutex, false), locked(false)
    {
        if (autoLock) get();
    }

    WriteLock::~WriteLock() throw()
    {
        try
        {
            release();
        }
        catch (...) { }
    }

    void WriteLock::get() throw(AssertException, exception)
    {
        if (!locked)
        {
            rwLocker.getWriteLock(lock);
            locked = true;
        }
    }

    void WriteLock::release() throw(AssertException, exception)
    {
        if (locked)
        {
            rwLocker.releaseWriteLock(lock);
            locked = false;
        }
    }
}
