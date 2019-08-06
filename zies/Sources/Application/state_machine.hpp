// =================================================================
// state_machine.hpp
// Header for state machine framework implementation.
// =================================================================

#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

#include "registers.hpp"
#include "time.hpp"

class StateMachine;
// =================================================================
class State
{
	public:
		inline State () :
				Id(0)
		{
		}
		virtual State * Poll ( StateMachine * theSM ) const = 0;
		virtual ~State ()
		{
		}
	private:
		uint8_t Id;
};
// =================================================================
class StateMachine
{
		static StateMachine * SMlist[];
		static int SMlistCount;
		static const int SMlistCountMax = 100u;
	protected:
		State * CurrentState;
	public:
		const char * Name;
		Error * HostErrorCode;
		int32_t * HostCommand;

		StateMachine ( const char * theName, State * theInitialState, int32_t * theHostCommand,
		        Error * theHostErrorCode ) :
				CurrentState(theInitialState), Name(theName), HostErrorCode(theHostErrorCode), HostCommand(
				        theHostCommand)
		{
			if (theName)
			{
				ASSERT(SMlistCount < SMlistCountMax);
				SMlist[SMlistCount++] = this;
			}
		}
		void Poll ();
		static void PollAll ();
		static bool SmIsBusyOrError ( RegisterIds theAddress, int32_t theCommand );
		static void PrintAllErrorCodes ();
};
// =================================================================
#define SM_POLL( stateName ) State * stateName ## _class::Poll (StateMachine * theSM) const
#define SM_STATE( stateName ) \
  class stateName ## _class : public State \
  { public: \
    stateName ## _class () {} \
    State * Poll (StateMachine * theSM) const; \
    virtual ~ stateName  ## _class () {} \
  }; \
  stateName ## _class stateName;

// =================================================================
class SM_Msg
{
	public:
		bool Pending;

		SM_Msg () :
				Pending(false)
		{
		}
		inline void Send ()
		{
			Pending = true;
		}
		Error TestAndSend ();
};

// =================================================================
class SM_Int32ParamMsg : public SM_Msg
{
	public:
		int32_t Param1;

		inline SM_Int32ParamMsg () :
				Param1(0), InProgress(false), Request(-1)
		{
		}
		Error Query ( int32_t theRequest, int32_t * theResult ); // multi-threaded callers should use unique Request Ids
		void Send1Param ( int32_t theParam );
		Error TestAndSendParam ( int32_t theParam );
	protected:
		bool InProgress = false;
		int32_t Request = -1;
};
// =================================================================
class SM_2Int32ParamMsg : public SM_Int32ParamMsg
{
	public:
		int32_t Param2;

		inline SM_2Int32ParamMsg () :
				Param2(0)
		{
		}
		void Send2Param ( int32_t theParam1, int32_t theParam2 );
};
// =================================================================
class SM_3Int32ParamMsg : public SM_2Int32ParamMsg
{
	public:
		int32_t Param3;

		inline SM_3Int32ParamMsg () :
				Param3(0)
		{
		}
		void Send3Param ( int32_t theParam1, int32_t theParam2, int32_t theParam3 );
};
// =================================================================
class SM_4Int32ParamMsg : public SM_3Int32ParamMsg
{
	public:
		int32_t Param4;

		inline SM_4Int32ParamMsg () :
				Param4(0)
		{
		}
		void Send4Param ( int32_t theParam1, int32_t theParam2, int32_t theParam3, int32_t theParam4 );
};
// =================================================================
class SM_TimeParamMsg : public SM_Msg
{
	public:
		Time Param1;

		inline SM_TimeParamMsg () :
				Param1(0)
		{
		}
		void SendTimeParam ( const Time & theParam );
		Error TestAndSendTimeParam ( const Time & theParam );
};
// =================================================================
class SM_StringParamMsg : public SM_Msg
{
	public:
		char * String;
		int MaxSize;

		inline SM_StringParamMsg () :
				String(NULL), MaxSize(0)
		{
		}
		void SendStrParam ( char * theString, int theMaxSize );
};

#endif /* STATEMACHINE_HPP_ */
