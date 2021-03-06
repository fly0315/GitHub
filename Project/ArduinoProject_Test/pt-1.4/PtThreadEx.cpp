/*
 * PtThreadEx.cpp
 *
 *  Created on: 2018��3��26��
 *      Author: LiuBin
 */

#include "PtThreadEx.h"
static unsigned int g_SleeperCnt_US = 0;
extern PtThreadEx ThreadQ[];
PtThreadEx::PtThreadEx()
	: m_State(WAIT), m_TaskThread(NULL),ID(THREAD_NULL)
{
	TIMER.FunHook = NULL;
	TIMER.TickCNT = TIMER.Ticks = 0;

	SLEEPER_US.TickCNT = SLEEPER_US.Ticks = 0;
	SLEEPER_US.SleepST = AWAKE;
	SLEEPER_MS.TickCNT = SLEEPER_MS.Ticks = 0;
	SLEEPER_MS.SleepST = AWAKE;
}

PtThreadEx::~PtThreadEx() 
{
    // TODO Auto-generated destructor stub
}

THID PtThreadEx::CreateTask(
	THID nId,
	TASK myTask,
	unsigned long Period_ms,
	STATE State,
	void(*TimerHook)(void)
)
{
	ID = nId;
	m_State = State;
	TIMER.Ticks = TIMER.TickCNT = TickCnt_T2_mS(Period_ms);
	PT_INIT(&m_pt);
	TIMER.FunHook = TimerHook;
	m_TaskThread = myTask;

	return ID;
}

void PtThreadEx::TimerRemarks()
{
	if (TIMER.TickCNT == 0) {return;}	// Ensure timer has opened.
	if (--TIMER.Ticks == 0)				// timeout
	{
		// reset timer
		TIMER.TimerProcess();
		m_State = TIMEOUT;
		ResetTimer();
	}

}

int PtThreadEx::Schedule()
{
	return m_TaskThread(&m_pt);
}

SLEEP_ST PtThreadEx::SLEEP_US(unsigned int time_us)
{
	if (0 == time_us) { return SLEEPER_US.SleepST = AWAKE; }
	if (AWAKE == SLEEPER_US.SleepST)
	{
		SLEEPER_US.SleepST = ASLEEP;
		SLEEPER_US.Ticks = SLEEPER_US.TickCNT = TickCnt_T1_uS(time_us);
		if (0 == g_SleeperCnt_US)
		{
			StartCpuTimer1();
		}
		g_SleeperCnt_US++;
	}
	return SLEEPER_US.SleepST;
}

SLEEP_ST PtThreadEx::SLEEP_MS(unsigned int time_ms)
{
	if (0 == time_ms) { return SLEEPER_MS.SleepST = AWAKE; }
	if (AWAKE == SLEEPER_MS.SleepST)
	{
		SLEEPER_MS.SleepST = ASLEEP;
		SLEEPER_MS.Ticks = SLEEPER_MS.TickCNT = TickCnt_T2_mS(time_ms);
		StartCpuTimer1();
	}
	return SLEEPER_MS.SleepST;
}

void PtThreadEx::SleeperRemarks_US()
{
	if (AWAKE == SLEEPER_US.SleepST) { return; }
	if (--SLEEPER_US.Ticks == 0) 
	{
		SLEEPER_US.SleepST = AWAKE;
		g_SleeperCnt_US--;
		if (0 == g_SleeperCnt_US)
		{
			StopCpuTimer1();
		}
	}
}

void PtThreadEx::SleeperRemarks_MS()
{
	if (AWAKE == SLEEPER_MS.SleepST) { return; }
	if (--SLEEPER_MS.Ticks == 0)
	{
		SLEEPER_MS.SleepST = AWAKE;
		StopCpuTimer1();
	}
}


