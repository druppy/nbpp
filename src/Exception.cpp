//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Exception.hpp>

namespace nbpp
{
    Exception::Exception(const string& arg_message) : m_message(arg_message) { }

    Exception::~Exception() throw() { }

    string Exception::getMessage() const
    {
        return m_message;
    }

    string Exception::toString() const
    {
        if (getMessage().empty())
            return getName();
        else
            return getName() + ": " + getMessage();
    }

	const char *Exception::what( void ) const throw()
	{
		return m_message.c_str();
	}

    string Exception::getName() const
    {
        return "nbpp::Exception";
    }

    AssertException::AssertException(const string& message) :
        Exception(message) { }

    string AssertException::getName() const
    {
        return "nbpp::AssertException";
    }

    UnexpectedException::UnexpectedException(const string& message) :
        AssertException(message) { }

    string UnexpectedException::getName() const
    {
        return "nbpp::UnexpectedException";
    }

    IOException::IOException(const string& message) throw() :
        Exception(message) { m_errno = 0; }

    IOException::IOException( int ERRNUM, const string& message ) throw() {
        if( message.empty())
            m_message = strerror_r( ERRNUM );
        else
            m_message = message;

		m_errno = ERRNUM;
	}

    string IOException::getName() const
    {
        return "nbpp::IOException";
    }

    string UnknownHostException::getName() const
    {
        return "nbpp::UnknownHostException";
    }

    string TimeoutException::getName() const
    {
        return "nbpp::TimeoutException";
    }

    string SocketException::getName() const
    {
        return "nbpp::SocketException";
    }

    string BindException::getName() const
    {
        return "nbpp::BindException";
    }

    string ConnectException::getName() const
    {
        return "nbpp::ConnectException";
    }

    RegexException::RegexException(const string& message) :
        Exception(message) { }

    string RegexException::getName() const
    {
        return "nbpp::RegexException";
    }

    NoSuchElementException::NoSuchElementException(const string& message) :
        Exception(message) { }

    string NoSuchElementException::getName() const
    {
        return "nbpp::NoSuchElementException";
    }

    UnsupportedOperationException::UnsupportedOperationException(const string& message)
        : Exception(message) { }

    string UnsupportedOperationException::getName() const
    {
        return "nbpp::UnsupportedOperationException";
    }

    ParseException::ParseException(const string& message) :
        Exception(message) { }

    string ParseException::getName() const
    {
        return "nbpp::ParseException";
    }

    ThreadControlException::ThreadControlException( const string& message ) :
        Exception(message) { }

    string ThreadControlException::getName() const
    {
        return "nbpp::ThreadControlException";
    }

	RefCountException::RefCountException( const string& message ) 
		: Exception( message ) {}

	string RefCountException::getName() const
	{
		return "nbpp::RefCountException";
	}
}
