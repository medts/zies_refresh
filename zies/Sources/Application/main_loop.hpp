#ifndef MAINLOOP_HPP_
#define MAINLOOP_HPP_

#include "time.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
	void MainLoop ();
#ifdef __cplusplus
} /* extern "C" */
#endif

// =================================================================
#define LoopStats_PeriodAverageF 0.0

// =================================================================
class LoopStats
{
		Time PeriodMin;
		Time PeriodMax;
		int Count;
		Time LoopStartTime;
		Time MeasurementStart;
	public:
		const float PeriodAverageF = LoopStats_PeriodAverageF;

		LoopStats () :
				Count(0), PeriodAverageF(0.0)
		{
		}
		//----------------------------------------------------------------
		void PowerUp ()
		{
			Reset();
			LoopStartTime = MeasurementStart = CurrentTime();
			LoopStartTime.Print("MainLoop starting, CurrentTime=");
		}
		//----------------------------------------------------------------
		void Reset ()
		{
			Count = 0;
			MeasurementStart = CurrentTime();
			PeriodMin = Time::ValueMax;
			PeriodMax = Time::ValueZero;
		}
		//----------------------------------------------------------------
		void Print ()
		{
			PeriodMin.Print("   LoopTime Min: ");
			if (Count > 0)
			    Time((CurrentTime() - MeasurementStart).ToMilliSecF() / static_cast<float>(Count)).Print(
			            "   LoopTime Avg: ");
			PeriodMax.Print("   LoopTime Max: ");
		}
		//----------------------------------------------------------------
		void EndLoop ()
		{
			Time loopDuration = CurrentTime() - LoopStartTime;
			if (loopDuration > PeriodMax) PeriodMax = loopDuration;
			if (loopDuration < PeriodMin) PeriodMin = loopDuration;
			Count++;
			LoopStartTime = CurrentTime();
		}
};
// =================================================================
extern LoopStats gLoopStats;

#endif /* MAINLOOP_HPP_ */
