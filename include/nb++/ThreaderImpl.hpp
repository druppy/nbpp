//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREADER_IMPL
#define _NBPP_THREADER_IMPL

#include <nb++/Command.hpp>
#include <nb++/RefCount.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    class ThreaderImpl : public RefCounted
    {
    public:
        ThreaderImpl();

        virtual ~ThreaderImpl() throw();

        void init();

        virtual void queue( Command command ) = 0;

        bool isInitialized() const;

    protected:
        virtual void doInit() = 0;

    private:
        bool initialized;
    };

    typedef RefHandle<ThreaderImpl> ThreaderHandle;
}

#endif /* _NBPP_THREADER_IMPL */
