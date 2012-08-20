//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <nb++/RWLocker.hpp>

namespace nbpp
{
    RWLocker::RWLocker() throw(AssertException, exception) :
        _readerCount(0), _mutex(new Mutex) { }
    
    RWLocker::RWLocker(Mutex* arg_mutex) throw(AssertException, exception) :
        _readerCount(0), _mutex(arg_mutex) { }

    RWLocker::~RWLocker() throw()
    {
        delete _mutex;
    }

    void RWLocker::getReadLock() throw(AssertException, exception)
    {
        // Lock the mutex.
        Lock lock(*_mutex);

        // Increment the number of readers.
        ++_readerCount;
    }

    void RWLocker::releaseReadLock() throw(AssertException, exception)
    {
        // Lock the mutex.
        Lock lock(*_mutex);

        // Decrement the number of readers.
        DEBUG(Assert (_readerCount != 0,
                      AssertException ("nbpp::RWLocker::releaseReadLock(): "
                                       "RWLocker already unlocked")));
        --_readerCount;

        // Notify other threads waiting for this mutex.
        lock.notifyAll();
    }

    void RWLocker::getWriteLock(Lock& lock) throw(AssertException, exception)
    {
        // Get the write lock.
        lock.get();

        // Wait until there are no more readers.
        while (_readerCount > 0)
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
        _rwLocker(arg_rwLocker), _locked(false)
    {
        if (autoLock) get();
    }

    ReadLock::~ReadLock() throw()
    {
        try {
            release();
        } catch (...) { }
    }

    void ReadLock::get() throw(AssertException, exception)
    {
        if (!_locked) {
            _rwLocker.getReadLock();
            _locked = true;
        }
    }

    void ReadLock::release() throw(AssertException, exception)
    {
        if (_locked) {
            _rwLocker.releaseReadLock();
            _locked = false;
        }
    }

    WriteLock::WriteLock(RWLocker& arg_rwLocker, bool autoLock)
        throw(AssertException, exception) :
        _rwLocker(arg_rwLocker), _lock(*_rwLocker._mutex, false), _locked(false)
    {
        if (autoLock) get();
    }

    WriteLock::~WriteLock() throw()
    {
        try {
            release();
        } catch (...) { }
    }

    void WriteLock::get() throw(AssertException, exception)
    {
        if (!_locked) {
            _rwLocker.getWriteLock(_lock);
            _locked = true;
        }
    }

    void WriteLock::release() throw(AssertException, exception)
    {
        if (_locked) {
            _rwLocker.releaseWriteLock(_lock);
            //~ _locked = false;
        }
    }
}
