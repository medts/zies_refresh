// =================================================================
// Time.hpp
// Header for generic soft timer.
// =================================================================

#ifndef TIME_HPP_
#define TIME_HPP_

#include "common.hpp"

//================================================================
class Time
{
	public:
		static const Time ValueZero;
		static const Time ValueMax;
		uint32_t ValueMS;  // SysTick count MS
		uint32_t ValueLS;  // SysTick count LS
		//----------------------------------------------------------------
		inline Time () :
				ValueMS(0), ValueLS(0)
		{
		}
		//----------------------------------------------------------------
		Time ( uint32_t theMS, uint32_t theLS ) :
				ValueMS(theMS), ValueLS(theLS)
		{
		}
		//----------------------------------------------------------------
		Time ( float theMilliSecs );
		//----------------------------------------------------------------
		inline Time ( const Time & theOther ) :
				ValueMS(theOther.ValueMS), ValueLS(theOther.ValueLS)
		{
		}
		//----------------------------------------------------------------
		float ToMilliSecF () const;
		uint32_t ToMilliSec () const;
		uint32_t ToMicroSec () const;
		uint32_t ToNanoSec () const;
		//----------------------------------------------------------------
		virtual Time& operator= ( const Time & theOther )
		{
			if (this != &theOther)
			{
				ValueMS = theOther.ValueMS;
				ValueLS = theOther.ValueLS;
			}
			return *this;
		}
		//----------------------------------------------------------------
		Time operator+ ( const Time & theOther ) const
		{
			Time temp(*this);
			temp.ValueMS += theOther.ValueMS;
			temp.ValueLS += theOther.ValueLS;
			if (ValueLS > temp.ValueLS)  // if overflow
			    temp.ValueMS++;
			return temp;
		}
		//----------------------------------------------------------------
		Time operator- ( const Time & theOther ) const
		{
			ASSERT(ValueMS >= theOther.ValueMS);
			Time temp(*this);
			temp.ValueMS -= theOther.ValueMS + (ValueLS < theOther.ValueLS ? 1 : 0);
			temp.ValueLS -= theOther.ValueLS;
			return temp;
		}
		//----------------------------------------------------------------
		bool operator>= ( const Time & theOther ) const
		{
			if (ValueMS < theOther.ValueMS) return false;
			if (ValueMS > theOther.ValueMS) return true;
			return ValueLS >= theOther.ValueLS;
		}
		//----------------------------------------------------------------
		bool operator> ( const Time & theOther ) const
		{
			if (ValueMS < theOther.ValueMS) return false;
			if (ValueMS > theOther.ValueMS) return true;
			return ValueLS > theOther.ValueLS;
		}
		//----------------------------------------------------------------
		bool operator< ( const Time & theOther ) const
		{
			if (ValueMS < theOther.ValueMS) return true;
			if (ValueMS > theOther.ValueMS) return false;
			return ValueLS < theOther.ValueLS;
		}
		//----------------------------------------------------------------
		void Print ( const char * theTitle ) const;
};
//================================================================
class CurrentTime : public Time
{
	public:
		CurrentTime ();
};
//================================================================
class TimeoutTimer : public Time
{
	public:
		inline TimeoutTimer ()
		{
		}
		//----------------------------------------------------------------
		inline TimeoutTimer ( const Time & theWaitTime )
		{
			Start(theWaitTime);
		}
		//----------------------------------------------------------------
		void Start ( const Time & theWaitTime )
		{
			Time::operator=(theWaitTime + CurrentTime());  // save end time
		}
		//----------------------------------------------------------------
		bool IsTimedOut () const
		{
			return (*this < CurrentTime());  // if end time < current time
		}
};

#endif /* TIME_HPP_ */
