//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_MUTEX
#define _NBPP_MUTEX

#include <nb++/osdep.hpp>
#include <nb++/Exception.hpp>

extern "C"
{
#include <pthread.h>
}

namespace nbpp
{
    class Lock;

    /**
     * A fast mutex for synchronizing non-recursive methods.  Use a Lock to lock and
     * unlock a Mutex.  Use a RecursiveMutex if you need to be able to lock a mutex
     * more than once before unlocking it.
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated as a member variable by the object that uses it.
     *
     * @see Lock
     * @see RecursiveMutex
     */
    class Mutex
    {
    public:
        /**
         * Constructs a Mutex.
         */
        Mutex() throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~Mutex() throw();

    protected:
        virtual void lock() throw(AssertException, exception);
        virtual void unlock() throw(AssertException, exception);
        virtual void wait() throw(AssertException, exception);
        virtual void wait(unsigned int sec) throw(AssertException, exception);

    private:
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);

        void notify() throw(AssertException, exception);
        void notifyAll() throw(AssertException, exception);

        pthread_mutex_t _mutex;
        pthread_mutexattr_t _mutexAttr;
        pthread_cond_t _cond;
        pthread_condattr_t _condAttr;

        friend class Lock;
    };


    /**
     * A mutex for synchronizing recursive methods.  Use a Lock to lock and unlock a
     * RecursiveMutex.
     *
     * If a locked RecursiveMutex is locked again by the same thread,
     * its lock count is incremented.  If it unlocked by the same
     * thread, its lock count is decremented.  When its lock count
     * reaches 0, it is unlocked.
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated as a member variable by the object that uses it.
     *
     * @see Lock
     */
    class RecursiveMutex : public Mutex
    {
    public:
        /**
         * Constructs a RecursiveMutex.
         */
        RecursiveMutex() throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~RecursiveMutex() throw();

    protected:
        virtual void lock() throw(AssertException, exception);
        virtual void unlock() throw(AssertException, exception);
        virtual void wait() throw(AssertException, exception);
        virtual void wait(unsigned int sec) throw(AssertException, exception);

    private:
        RecursiveMutex(const RecursiveMutex&);
        RecursiveMutex& operator=(const RecursiveMutex&);

        mutable unsigned int _lockCount;
        pthread_t _owner;
    };


    /**
     * When constructed, this object acquires (by default) an
     * exclusive lock from the Mutex passed to it in its constructor.
     * When the object goes out of scope, it releases the lock.  This
     * makes it convenient to synchronize whole methods, because the
     * lock will be released even if an exception is thrown.  For
     * example:
     *
     * <pre>
     * class Foo
     * {
     * public:
     *     void bar()
     *     {
     *         Lock lock(mutex);
     *         // do some stuff that could throw exceptions...
     *     }
     *
     * private:
     *     Mutex mutex;
     * };
     * </pre>
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated by one method, as a local variable.
     *
     * @see Mutex
     * @see RecursiveMutex
     */
    class Lock
    {
    public:
        /**
         * By default, gets the Mutex's lock.
         */
        Lock(Mutex& mutex, bool autoLock = true) throw(AssertException, exception);

        /**
         * Releases the Mutex's lock.  Does nothing if this Lock is
         * not holding the lock.
         */
        ~Lock() throw();

        /**
         * Gets the Mutex's lock.  Does nothing if this Lock is
         * already holding the lock.
         */
        void get() throw(AssertException, exception);

        /**
         * Releases the Mutex's lock.  Does nothing if this Lock is
         * not holding the lock.
         */
        void release() throw(AssertException, exception);

        /**
         * Waits for another thread to call notify() or notifyAll() on
         * a Lock for the same Mutex.  The calling thread must already
         * have the lock.
         *
         * @exception AssertException if NDEBUG was not defined when
         * this class was compiled, and the calling thread does not
         * have the lock.
         *
         * @see notify
         * @see notifyAll
         */
        void wait() throw(AssertException, exception);

        /**
          Waits for another thread to call notify() or notifyAll() on
          a Lock for the same Mutex.  The calling thread must already
          have the lock. 

          If more than time have passed the wait will terminate.
         
          @exception AssertException if NDEBUG was not defined when
          this class was compiled, and the calling thread does not
          have the lock.

          @param sec sec until timeout
        */
        void wait(unsigned int sec) throw(AssertException, exception);

        /**
         * Notifies one thread waiting for this mutex.  The calling
         * thread must already have the lock.
         *
         * @exception AssertException AssertException if NDEBUG was not defined when this
         * class was compiled, and the calling thread does not have the lock.
         *
         * @see wait
         */
        void notify() throw(AssertException, exception);

        /**
         * Notifies all threads waiting for this mutex.  The calling
         * thread must already have the lock.
         *
         * @exception AssertException AssertException if NDEBUG was not defined when this
         * class was compiled, and the calling thread does not have the lock.
         *
         * @see wait
         */
        void notifyAll() throw(AssertException, exception);

    private:
        Lock();
        Lock(const Lock&);
        Lock& operator=(Lock&);

        Mutex& _mutex;
        mutable bool _locked;
    };
}

#endif /* _NBPP_MUTEX */
