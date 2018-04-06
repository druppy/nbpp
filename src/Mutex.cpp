//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <nb++/Mutex.hpp>

#include <assert.h>
#include <errno.h>

#include <list>
#include <algorithm>

namespace nbpp
{
	using namespace std;
	/*
	  This is an internal class that maintain a list of all allocated mutexes
	  to keep track of them, if a fork or popen function will be called to
	  make it possible to lock and unlock all at ones, for the fock process to
	  succed.

	  Im still not sure this is the way to do this, and I really like to eliminate
	  the use of fork anyware in nb++, but just in case :-)
	 */
	class MutexManager {
		typedef list<pthread_mutex_t *> mutexes_t;
		mutexes_t _mutexes;
        pthread_mutex_t _mutex;
        pthread_mutexattr_t _mutexAttr;

		static MutexManager *_pManager;
		MutexManager() {
			reg_funcs();
			pthread_mutexattr_init( &_mutexAttr );
			pthread_mutex_init( &_mutex, &_mutexAttr );
		}

		~MutexManager() {
			pthread_mutex_destroy( &_mutex );
			pthread_mutexattr_destroy( &_mutexAttr );
		}

		void reg_funcs( void );
	public:
		static MutexManager &getInst( void ) {
			if( !_pManager )
				_pManager = new MutexManager();

			return *_pManager;
		}

		void add( pthread_mutex_t &mutex ) {
			pthread_mutex_lock( &_mutex );

			_mutexes.push_back( &mutex );

			pthread_mutex_unlock( &_mutex );
		}

		void remove( pthread_mutex_t &mutex ) {
			pthread_mutex_lock( &_mutex );
			mutexes_t::iterator i = find( _mutexes.begin(), _mutexes.end(), &mutex );

			if( i != _mutexes.end())
				_mutexes.erase( i );

			pthread_mutex_unlock( &_mutex );
		}

		void initAll( void ) {
			pthread_mutex_lock( &_mutex );

			// cout << "Init all " << _mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = _mutexes.begin(); i != _mutexes.end(); i++ )
				::pthread_mutex_init( *i, NULL );

			pthread_mutex_unlock( &_mutex );
		}

		void lockAll( void ) {
			pthread_mutex_lock( &_mutex );
			// cout << "Lock all " << _mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = _mutexes.begin(); i != _mutexes.end(); i++ )
				::pthread_mutex_lock( *i );

			pthread_mutex_unlock( &_mutex );
		}

		void releaseAll( void ) {
			pthread_mutex_lock( &_mutex );
			// cout << "Release all " << _mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = _mutexes.begin(); i != _mutexes.end(); i++ )
				::pthread_mutex_unlock( *i );

			pthread_mutex_unlock( &_mutex );
		}
	};

	void mutex_init_prepare( void ) {
	}

	void mutex_init_parent( void ) {
	}

	void mutex_init_child( void ) {
		MutexManager::getInst().initAll();
	}

	MutexManager *MutexManager::_pManager = NULL;

	void MutexManager::reg_funcs( void )
	{
		int res = ::pthread_atfork( mutex_init_prepare, mutex_init_parent, mutex_init_child );

		assert( res == 0 );
	}

	////////////////////
	// Impl. of Mutex

    Mutex::Mutex() throw(AssertException, exception)
    {
        pthread_mutexattr_init( &_mutexAttr );
        pthread_mutex_init( &_mutex, &_mutexAttr );
        pthread_condattr_init( &_condAttr );
        pthread_cond_init( &_cond, &_condAttr );

		// MutexManager::getInst().add( _mutex );
    }

    Mutex::~Mutex() throw()
    {
		// MutexManager::getInst().remove( _mutex );

		pthread_mutexattr_destroy( &_mutexAttr );
        pthread_mutex_destroy( &_mutex );
		pthread_condattr_destroy( &_condAttr );
        pthread_cond_destroy( &_cond );
    }

    void Mutex::lock() throw(AssertException, exception)
    {
        int ret;
        if( ret = pthread_mutex_lock( &_mutex ))
            throw AssertException( "nbpp::Mutex::lock() error" );
    }

    void Mutex::unlock() throw(AssertException, exception)
    {
        int ret;
        if( ret = pthread_mutex_unlock( &_mutex ))
            throw AssertException( "nbpp::Mutex::unlock() error" );
    }

    void Mutex::wait() throw(AssertException, exception)
    {
        int ret;

        if((ret = pthread_cond_wait( &_cond, &_mutex )) != 0) {
            if( ret != ETIMEDOUT ) {
                string err = strerror( ret );
                throw AssertException( "nbpp::Mutex::wait() error : " + err );
            }
        }
    }

    void Mutex::wait(unsigned int sec) throw(AssertException, exception)
    {
        int ret;
        timespec now;

        clock_gettime(CLOCK_REALTIME, &now);
        now.tv_sec += sec;
        if((ret = pthread_cond_timedwait(&_cond, &_mutex, &now) != 0))  {
            if( ret != ETIMEDOUT ) {
                string err = strerror( ret );
                throw AssertException( "nbpp::Mutex::wait(unsigend int) error : " + err );
            }
        }
    }

    void Mutex::notify() throw(AssertException, exception)
    {
        if( pthread_cond_signal( &_cond ) != 0)
            throw AssertException( "nbpp::Mutex::notify() error" );
    }

    void Mutex::notifyAll() throw(AssertException, exception)
    {
        if( pthread_cond_broadcast( &_cond ) != 0)
            throw AssertException( "nbpp::Mutex::notifyAll() error" );
    }

	/////////////////////////////
	// Impl. of RecursiveMutex

    RecursiveMutex::RecursiveMutex() throw(AssertException, exception) :
        _lockCount(0)
    { }

    RecursiveMutex::~RecursiveMutex() throw() { }

    void RecursiveMutex::lock() throw(AssertException, exception)
    {
        if (!_lockCount || !pthread_equal(_owner, pthread_self())) {
            Mutex::lock();
            _owner = pthread_self();
        }
        ++_lockCount;
    }

    void RecursiveMutex::unlock() throw(AssertException, exception)
    {
        DEBUG(Assert(_lockCount > 0,
                     AssertException("nbpp::RecursiveMutex::unlock(): "
                                     "mutex already unlocked")));
        DEBUG(Assert(pthread_equal(_owner, pthread_self()),
                     AssertException("nbpp::RecursiveMutex::unlock(): "
                                     "mutex locked by another thread")));
        if (--_lockCount == 0)
            Mutex::unlock();
    }

    void RecursiveMutex::wait() throw(AssertException, exception)
    {
        unsigned int saveCount = _lockCount;
        _lockCount = 0;
        Mutex::wait();
        _owner = pthread_self();
        _lockCount = saveCount;
    }

    void RecursiveMutex::wait(unsigned int sec) throw(AssertException, exception)
    {
        unsigned int saveCount = _lockCount;
        _lockCount = 0;
        Mutex::wait(sec);
        _owner = pthread_self();
        _lockCount = saveCount;
    }

    Lock::Lock(Mutex& arg_mutex, bool autoLock) throw(AssertException, exception) :
        _mutex(arg_mutex), _locked(false)
    {
        if ( autoLock )
			get();
    }

    Lock::~Lock() throw()
    {
        try {
            release();
        } catch (...) { }
    }

    void Lock::get() throw(AssertException, exception)
    {
        if ( !_locked ) {
            _mutex.lock();
            _locked = true;
        }
    }

    void Lock::release() throw(AssertException, exception)
    {
        if ( _locked ) {
            _mutex.unlock();
            _locked = false;
        }
    }

    void Lock::wait() throw(AssertException, exception)
    {
        DEBUG(Assert(_locked,
                     AssertException("nbpp::Lock::wait(): "
                                     "wait on unlocked mutex")));
        _mutex.wait();
    }

    void Lock::wait(unsigned int sec) throw(AssertException, exception)
    {
        DEBUG(Assert(_locked,
                     AssertException("nbpp::Lock::wait(sec): "
                                     "wait on unlocked mutex")));
        _mutex.wait(sec);
    }


    void Lock::notify() throw(AssertException, exception)
    {
        DEBUG(Assert(_locked,
                     AssertException ("nbpp::Lock::notify(): "
                                      "notify on unlocked mutex")));
        _mutex.notify();
    }

    void Lock::notifyAll() throw(AssertException, exception)
    {
        DEBUG(Assert(_locked,
                     AssertException("nbpp::Lock::notifyAll(): "
                                     "notifyAll on unlocked mutex")));
        _mutex.notifyAll();
    }
}
