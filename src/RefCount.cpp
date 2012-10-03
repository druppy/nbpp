#include <nb++/RefCount.hpp>

namespace nbpp {
    RefCounted::RefCounted( void ) : _refcount( 0 ), _destroying( false ) 
    {
    }
    
	RefCounted::RefCounted( const RefCounted & ) : _refcount( 0 ), _destroying( false ) 
    {
    }

	void RefCounted::lock( void ) 
    {
#ifdef NBPP_HAVE_ATOMIC
        if( _destroying )
            throw RefCountException( "Can't lock a destructed data object." );
            
        __sync_fetch_and_add( &_refcount, 1 );
#else
		Lock sync( _mutex );

		if( _destroying )
			throw RefCountException( "Can't lock a destructed data object." ); 

		_refcount++;
#endif
	}

	void RefCounted::release( void ) 
    {
#ifdef NBPP_HAVE_ATOMIC
        if( __sync_sub_and_fetch( &_refcount, 1 ) == 0) {
                _destroying = true;
                delete this; 
        }
#else
		Lock sync( _mutex );

		if( --_refcount == 0 ) {
			_destroying = true;
			sync.release();
			delete this;    // Are we in danger here, or does'nt out refcounter help us out ?
		}
#endif
	}

    BaseRefHandle::BaseRefHandle( RefCounted *ref ) : _ref( ref ) 
    {
		if( _ref ) {
			_ref->lock();
		}
	}

	BaseRefHandle::~BaseRefHandle() 
    {
		if( _ref  )
			_ref->release();
	}
	
	BaseRefHandle::BaseRefHandle( const BaseRefHandle &refHndl ) 
    {
		_ref = refHndl._ref;

		if( _ref )
		  	_ref->lock();
	}

	BaseRefHandle &BaseRefHandle::operator=( const BaseRefHandle &refHndl ) 
    {
		if( _ref != refHndl._ref ) {
			RefCounted *old = _ref;
			
			_ref = refHndl._ref;

			if( _ref )
				_ref->lock();

			if( old )
				old->release();			
		}
		return *this;
	}
}
