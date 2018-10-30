#pragma once
#include <FlexiTimer2.h>
#include <TimerOne.h>
void OnTimer1();
void OnTimer2();
static unsigned int Timer1UserCnt;
static unsigned int Timer2UserCnt;
inline void StartCpuTimer1()
{
	if (Timer1UserCnt == 0)
	{
		Timer1.start();
	}
	Timer1UserCnt++;
	
}
inline void StopCpuTimer1()
{	
	Timer1UserCnt--;
	if (Timer1UserCnt == 0)
	{
		Timer1.stop();
	}
}

inline void ConfigCpuTimer1(unsigned long MS)
{
	Timer1UserCnt = 0;
	Timer1.initialize(MS * 1000);
	Timer1.attachInterrupt(OnTimer1);
}

inline void StartCpuTimer2()
{
	if (Timer2UserCnt == 0)
	{
		FlexiTimer2::start();
	}
	Timer2UserCnt++;

}
inline void StopCpuTimer2()
{
	Timer2UserCnt--;
	if (Timer2UserCnt == 0)
	{
		FlexiTimer2::stop();
	}
}

inline void ConfigCpuTimer2(unsigned long MS)
{
	Timer2UserCnt = 0;
	Timer1.initialize(MS * 1000);
	Timer1.attachInterrupt(OnTimer1);
	FlexiTimer2::set(MS, OnTimer2);
}
