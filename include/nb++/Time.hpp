/**
   This is a time_t wrapper for C++, that contains noting more than 
   the normal time_t funtionality, but in a C++ way.
*/
#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <ctime>
#include <iostream>
#include <string>

namespace nbpp {

	using namespace std;
	/**
	   A thin wrapper for the std time_t C type.
	 */
	class Time {
		time_t m_time;
		struct tm m_tm;
		bool bUpdate;
	public:
		Time() {
			m_time = time( NULL );
			localtime_r( &m_time, &m_tm );
		}

		Time( time_t t ) {
			m_time = t;
			localtime_r( &m_time, &m_tm );
		}

		Time( const Time &t ) {
			*this = t;
		}

		Time &operator=( const Time &t ) {
			m_time = t.m_time;
			localtime_r( &m_time, &m_tm );
			return *this;
		}
		
		time_t operator*() const {return m_time;}
		bool operator==( const Time &t ) const {return m_time == t.m_time;}
		bool operator!=( const Time &t ) const {return m_time != t.m_time;}
		bool operator>( const Time &t ) const {return m_time > t.m_time;}
		bool operator<( const Time &t ) const {return m_time < t.m_time;}

		void setMonth( int nMonth ) {
			m_tm.tm_mon = nMonth - 1;
			m_time = mktime( &m_tm );
		}

		void setMday( int nMday ) {
			m_tm.tm_mday = nMday;
			m_time = mktime( &m_tm );
		}

		void setYear( int nYear ) {
			m_tm.tm_year = nYear - 1900;
			m_time = mktime( &m_tm );
		}

		void setTime( int nHour, int nMin, int nSec = 0 ) {
			m_tm.tm_hour = nHour;
			m_tm.tm_min = nMin;
			m_tm.tm_sec = nSec;
			m_time = mktime( &m_tm );
		}

		int getMonth( void ) const { return m_tm.tm_mon + 1; }
		int getYear( void ) const {return m_tm.tm_year + 1900; }
		int getMday( void ) const {return m_tm.tm_mday; }

		int getHour( void ) const {return m_tm.tm_hour;}
		int getMin( void ) const {return m_tm.tm_min;}
		int getSecound( void ) const {return m_tm.tm_sec;}

		/**
		   Set ISO 8601 timestamp, as the one returned by the toString
		   function.
		 */
		bool setString( const string &sTime ) {
			int cnt = sscanf( sTime.c_str(), "%4u%2u%2uT%2u%2u%2u", 
						  &m_tm.tm_year, &m_tm.tm_mon, &m_tm.tm_mday, 
						  &m_tm.tm_hour, &m_tm.tm_min, &m_tm.tm_sec );

			if( cnt == 6 ) {
				m_tm.tm_mon--;
				m_tm.tm_year = m_tm.tm_year - 1900;

				m_time = mktime( &m_tm );

				return true;
			} 
			return false;
		}

		string toString( void ) const {
			char szLine[ 40 ];
			
			strftime( szLine, sizeof( szLine ), "%Y%m%dT%H%M%S", &m_tm );
			
			return szLine;
		}
	};
}

inline std::ostream &operator<<( std::ostream &os, const nbpp::Time &t )
{
	os << t.toString();
	return os;
}

#endif
