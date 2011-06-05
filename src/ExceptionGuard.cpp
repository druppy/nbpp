//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#if __GNUG__ > 2
#include <new>
#endif

#include <nb++/ExceptionGuard.hpp>

namespace nbpp
{
	void assertUnexpected() throw(UnexpectedException)
    {
		try
        {
			throw;
		}
        catch (Exception& e)
        {
			throw UnexpectedException(e.toString());
		}
        catch (exception& e)
        {
			throw UnexpectedException(string("Unexpected system exception: ") + e.what());
		}
        catch (...)
        {
			throw UnexpectedException("Exception thrown of unknown type");
		}
	}

	ExceptionGuard::ExceptionGuard() throw()
    {
#if __GNUG__ < 3
		old = set_unexpected(&assertUnexpected);
#else 
		old = std::set_new_handler(&assertUnexpected);
#endif
	}

	ExceptionGuard::~ExceptionGuard() throw()
    {
#if __GNUG__ < 3
		set_unexpected(old);
#else 
		std::set_new_handler(&assertUnexpected);
#endif
	}
}
