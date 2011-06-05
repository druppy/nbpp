//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREADER
#define _NBPP_THREADER

#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
    /**
     * Represents a strategy for using threads or processes to execute Command objects.
     * This class is not meant to be used directly; use one of its derived classes
     * instead.
     *
     * This class is reference-counted.
     */
    template<class ImplHandle> class ThreaderTemplate
    {
    public:
        /**
         * Destructor.
         */
        virtual ~ThreaderTemplate() throw() { }

        /**
         * Must be called before any commands are queued.
         *
         * @exception ThreadControlException if a thread-related error occurs.
         */
        void init() throw(ThreadControlException, AssertException, exception)
        {
            impl->init();
        }

        /**
         * Queues a command to be executed according to the strategy of this Threader.
         *
         * @exception ThreadControlException if a thread-related error occurs.
         */
        void queue(Command command)
            throw(ThreadControlException, AssertException, exception)
        {
            impl->queue(command);
        }

        /**
         * Creates a null object.
         */
        ThreaderTemplate() throw(exception) { }

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw()
        {
            return impl;
        }

        /**
         * @return true if this object has been initialized.
         */
        bool isInitialized() const throw(AssertException, exception)
        {
            return impl->isInitialized();
        }

        // Not intended for general use
        const ImplHandle& getImpl () const throw(AssertException, exception)
        {
            return impl;
        }

    protected:
        ImplHandle impl;
        ThreaderTemplate(const ImplHandle& arg_impl)
            throw(AssertException, exception) : impl(arg_impl) { }
    };


    /**
     * Encapsulates a strategy for using threads or processes to execute Command objects.
     * Threader doesn't have real derived classes, but implicit conversion of specialized
     * types to Threader is supported.
     *
     * This class is reference-counted.
     *
     * @see ThreaderTemplate
     * @see SingleThreader
     * @see ForkThreader
     * @see ThreadPerCommand
     * @see MultiThreader
     */
    class Threader : public ThreaderTemplate<ThreaderHandle>
    {
    public:
        /**
         * Creates a null object.
         */
        Threader() throw(exception);

        // Not intended for general use.
        template<class C> Threader(const C& threader) throw(AssertException, exception) :
            ThreaderTemplate<ThreaderHandle>(threader.getImpl()) { }

        // Not intended for general use.
        Threader(ThreaderImpl* arg_impl) throw(AssertException, exception);
    };
}

#endif /* _NBPP_THREADER */
