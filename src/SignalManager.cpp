//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/SignalManager.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    SignalEvent::SignalEvent (int arg_signum) throw(AssertException, exception) :
        signum(arg_signum) { }

    SignalEvent::~SignalEvent() throw() { }

    int SignalEvent::getSigNum() const throw(AssertException, exception)
    {
        return signum;
    }

    SignalManager::SignalHandlerData::SignalHandlerData() throw(AssertException, exception) :
        asyncHandler(NULL), pending(false) { }
    
    SignalManager::SignalHandlerData::SignalHandlerData(const SignalHandlerData& right)
        throw(AssertException, exception) :
        asyncHandler(right.asyncHandler), event(right.event),
        pending(right.pending)
    {
        right.syncHandlers.copyTo(syncHandlers);
    }

    SignalManager& theSignalManager() throw(AssertException, exception)
    {
        static SignalManager instance;
        return instance;
    }

    SignalManager::SignalManager() throw(AssertException, exception) :
        pendingSignals(false) { }

    void SignalManager::setAsyncHandler(int signum, SignalHandler* handler)
        throw(AssertException, exception)
    {
        handlerMap[signum].asyncHandler = handler;
        setSignal(signum, realHandler);
    }

    void SignalManager::ignoreSignal(int signum) throw(AssertException, exception)
    {
        handlerMap[signum].asyncHandler = NULL;
        removeAllSyncHandlers(signum);
        setSignal(signum, SIG_IGN);
    }

    void SignalManager::defaultSignal(int signum) throw(AssertException, exception)
    {
        handlerMap[signum].asyncHandler = NULL;
        removeAllSyncHandlers(signum);
        setSignal(signum, SIG_DFL);
    }

    void SignalManager::blockSignal(int signum) throw(AssertException, exception)
    {
#ifdef NBPP_HAVE_SIGPROCMASK
        sigset_t sset;
        sigemptyset(&sset);
        sigaddset(&sset, signum);
        ::sigprocmask(SIG_BLOCK, &sset, NULL);
#else   // !NBPP_HAVE_SIGPROCMASK
#ifdef NBPP_HAVE_SIGBLOCK
        ::sigblock(::sigmask (signum));
#else   // !NBPP_HAVE_SIGBLOCK
        throw UnsupportedOperationException("SignalManager::blockAsyncSignal: "
                                            "Don't know how to block signals "
                                            "on this OS.");
#endif  // !NBPP_HAVE_SIGBLOCK
#endif  // !NBPP_HAVE_SIGPROCMASK
    }

    void SignalManager::unblockSignal(int signum) throw(AssertException, exception)
    {
#ifdef NBPP_HAVE_SIGPROCMASK
        sigset_t sset;
        sigemptyset(&sset);
        sigaddset(&sset, signum);
        ::sigprocmask(SIG_UNBLOCK, &sset, NULL);
#else   // !NBPP_HAVE_SIGPROCMASK
#ifdef NBPP_HAVE_SIGBLOCK
        ::sigsetmask(::siggetmask() & ~sigmask (signum))
#else   // !NBPP_HAVE_SIGBLOCK
              throw UnsupportedOperationException("SignalManager::blockAsyncSignal: "
                                                  "Don't know how to unblock signals "
                                                  "on this OS.");
#endif  // !NBPP_HAVE_SIGBLOCK
#endif  // !NBPP_HAVE_SIGPROCMASK
    }

    void SignalManager::addSyncHandler(int signum, SignalHandler* handler)
        throw(AssertException, exception)
    {
        handlerMap[signum].syncHandlers.addListener(handler);
        setSignal(signum, realHandler);
    }

    void SignalManager::removeSyncHandler(int signum, SignalHandler* handler)
        throw(AssertException, exception)
    {
        handlerMap[signum].syncHandlers.removeListener(handler);
    }

    void SignalManager::removeSyncHandler(SignalHandler* handler)
        throw(AssertException, exception)
    {
        for(map<int, SignalHandlerData>::iterator i = handlerMap.begin();
            i != handlerMap.end(); i++)
            (*i).second.syncHandlers.removeListener(handler);
    }

    void SignalManager::removeAllSyncHandlers(int signum)
        throw(AssertException, exception)
    {
        handlerMap[signum].syncHandlers.removeAllListeners();
    }

    void SignalManager::deliver() throw(AssertException, exception)
    {
        if (pendingSignals)
        {
            pendingSignals = false;
            for(map<int, SignalHandlerData>::iterator i = handlerMap.begin();
                i != handlerMap.end();
                ++i)
            {
                if ((*i).second.pending)
                {
                    (*i).second.pending = false;
                    (*i).second.syncHandlers.sendEvent(*(*i).second.event,
                                                       &SignalHandler::handleSignal);
                    (*i).second.event = SignalEventHandle();
                }
            }
        }
    }
    
    NBPP_RETSIGTYPE SignalManager::realHandler(int signum) throw()
    {
        try
        {
            SignalManager& manager = theSignalManager();
            manager.pendingSignals = manager.handlerMap[signum].pending = true;
            manager.handlerMap[signum].event = SignalEventHandle(new SignalEvent(signum));

            SignalHandler* handler = manager.handlerMap[signum].asyncHandler;
            if (handler != NULL)
                handler->handleSignal(*manager.handlerMap[signum].event);
#ifndef NBPP_HAVE_SIGACTION
#ifdef NBPP_HAVE_SIGNAL
            ::signal (signum, realHandler);
#endif  // NBPP_HAVE_SIGNAL
#endif  // !NBPP_HAVE_SIGACTION
        }
        catch(...) { }
    }

    void SignalManager::setSignal (int signum, NBPP_RETSIGTYPE (*func)(int signum))
        throw(AssertException, exception)
    {
#ifdef NBPP_HAVE_SIGACTION
        struct sigaction sa;
        memset(&sa, 0, sizeof (sa));
        sa.sa_handler = func;
        sigemptyset(&sa.sa_mask);

        ::sigaction(signum, &sa, NULL);
#else   // !NBPP_HAVE_SIGACTION
#ifdef NBPP_HAVE_SIGNAL
        ::signal(signum, func);
#endif  // NBPP_HAVE_SIGNAL
#endif  // !NBPP_HAVE_SIGACTION
    }
}
