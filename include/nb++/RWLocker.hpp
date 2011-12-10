//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_RW_LOCKER
#define _NBPP_RW_LOCKER

#include <nb++/Mutex.hpp>

namespace nbpp
{
    class ReadLock;
    class WriteLock;

    /**
     * A write lock and multiple read locks for synchronizing non-recursive methods.  Use
     * ReadLock and WriteLock objects to lock and unlock an RWLocker.  Use a
     * RecursiveRWLocker to synchronize recursive methods using read and write locks.
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated as a member variable by the object that uses it.
     *
     * @see ReadLock
     * @see WriteLock
     */
    class RWLocker
    {
    public:
        /**
         * Constructs an RWLocker.
         */
        RWLocker() throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~RWLocker() throw();

    protected:
        RWLocker(Mutex* mutex) throw(AssertException, exception);

    private:
        void getReadLock() throw(AssertException, exception);
        void releaseReadLock() throw(AssertException, exception);
        void getWriteLock(Lock& lock) throw(AssertException, exception);
        void releaseWriteLock(Lock& lock) throw(AssertException, exception);

        RWLocker(const RWLocker&);
        RWLocker& operator=(const RWLocker&);

        mutable unsigned int readerCount; // a count of the number of readers
        Mutex* mutex;

        friend class ReadLock;
        friend class WriteLock;
    };


    /**
     * An RWLocker for synchronizing recursive methods.  Use ReadLock and WriteLock
     * objects to lock and unlock a RecursiveRWLocker.
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated as a member variable by the object that uses it.
     *
     * @see ReadLock
     * @see WriteLock
     */
    class RecursiveRWLocker : public RWLocker
    {
    public:
        /**
         * Constructs a RecursiveRWLocker.
         */
        RecursiveRWLocker() throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~RecursiveRWLocker() throw();
    };


    /**
     * When constructed, this object acquires (by default) a read lock from the
     * RWLocker passed to it in its constructor.  When the object goes out of
     * scope, it releases the lock.  This makes it convenient to synchronize whole
     * methods, because the lock will be released even if an exception is thrown.  For
     * example:
     *
     * <pre>
     * class Foo
     * {
     * public:
     *     void bar()
     *     {
     *         ReadLock readLock(rwLocker);
     *         // do some stuff that could throw exceptions...
     *     }
     *
     * private:
     *     RWLocker rwLocker;
     * };
     * </pre>
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated by one method, as a local variable.
     *
     * @see RWLocker
     * @see RecursiveRWLocker
     * @see WriteLock
     */
    class ReadLock
    {
    public:
        /**
         * By default, acquires a shared read lock.
         */
        ReadLock(RWLocker& rwLocker, bool autoLock = true)
            throw(AssertException, exception);

        /**
         * Releases a shared read lock.
         */
        ~ReadLock() throw();

        /**
         * Acquires a shared read lock.
         */
        void get() throw(AssertException, exception);

        /**
         * Releases a shared read lock.
         */
        void release() throw(AssertException, exception);

    private:
        ReadLock();
        ReadLock(const ReadLock&);
        ReadLock& operator=(ReadLock&);

        RWLocker& rwLocker;
        mutable bool locked;
    };


    /**
     * When constructed, this object acquires (by default) a write lock from the
     * RWLocker passed to it in its constructor.  When the object goes out of
     * scope, it releases the lock.  This makes it convenient to synchronize whole
     * methods, because the lock will be released even if an exception is thrown.  For
     * example:
     *
     * <pre>
     * class Foo
     * {
     * public:
     *     void bar()
     *     {
     *         WriteLock writeLock(rwLocker);
     *         // do some stuff that could throw exceptions...
     *     }
     *
     * private:
     *     RWLocker rwLocker;
     * };
     * </pre>
     *
     * This class is not reference-counted; each instance is intended to be
     * encapsulated by one method, as a local variable.
     *
     * @see RWLocker
     * @see RecursiveRWLocker
     * @see ReadLock
     */
    class WriteLock
    {
    public:
        /**
         * By default, acquires an exclusive lock.
         */
        WriteLock(RWLocker& rwLocker, bool autoLock = true)
            throw(AssertException, exception);

        /**
         * Releases an exclusive lock.
         */
        ~WriteLock() throw();

        /**
         * Acquires an exclusive lock.
         */
        void get() throw(AssertException, exception);

        /**
         * Releases an exclusive lock.
         */
        void release() throw(AssertException, exception);

    private:
        WriteLock();
        WriteLock(const WriteLock&);
        WriteLock& operator=(WriteLock&);

        RWLocker& rwLocker;
        Lock lock;
        mutable bool locked;
    };
}

#endif /* _NBPP_RW_LOCKER */
