/*
 * PtThreadEx.h
 *
 *  Created on: 2018年3月26日
 *      Author: LiuBin
 */

#ifndef PTTHREADEX_H_
#define PTTHREADEX_H_
#include <string.h>
#include "pt-sem.h"
#include "BSP_cfg.h"
typedef enum _STATE
{
	WAIT = 0,
	READY = 1,
	TIMEOUT = 2,
}STATE;


typedef enum _SLEEP_ST
{
	ASLEEP = 0,
	AWAKE = 1,
}SLEEP_ST;

typedef int(*TASK)(struct pt *pt);

inline unsigned long TickCnt_T2_mS(unsigned int ThreadPeriod)
{
	return (unsigned long)(ThreadPeriod / PERIOD_TIMER2_MS);
}
inline unsigned long TickCnt_T1_uS(unsigned int time_us)
{
	return (unsigned long)(time_us / PERIOD_TIMER1_US);
}

class PtThreadEx {
public:
	PtThreadEx();

    virtual ~PtThreadEx();
	void CreateTask(
		TASK myTask,
		unsigned long Period_ms = 0,
		STATE State = WAIT,
		void(*TimerHook)(void) = NULL
	);
	void TimerRemarks();
	int Schedule();
	inline void SetPeriod(unsigned int time_ms) 
	{ 
		TIMER.TickCNT = TickCnt_T2_mS(time_ms);
	}
	inline void ResetTimer()
	{
		TIMER.Ticks = TIMER.TickCNT;;
	}
	inline void SetState(STATE flag)
	{
		m_State = flag;
	}
	inline STATE GetState() { return m_State; }

	SLEEP_ST SLEEP_US(unsigned int time_us);
	SLEEP_ST SLEEP_MS(unsigned int time_ms);

	void SleeperRemarks_US();
	void SleeperRemarks_MS();	
	
	inline SLEEP_ST IsAwake_US() { return SLEEPER_US.SleepST; }
	inline SLEEP_ST IsAwake_MS() { return SLEEPER_MS.SleepST; }
private:
	struct pt m_pt;
	STATE m_State;
	struct _TIMER
	{
		unsigned long Ticks;         // timer tick-tack
		unsigned long TickCNT;       // time interval
		void(*FunHook)(void);       // function pointer
		inline void TimerProcess(void)
		{
			if (FunHook != NULL)
			{
				FunHook();
			}
		}
	} TIMER;       // 任务定义
	struct _SLEEPER 
	{
		unsigned long Ticks;
		unsigned long TickCNT;
		SLEEP_ST SleepST;
	} SLEEPER_US, SLEEPER_MS;
	TASK m_TaskThread;
};

#endif /* PTTHREADEX_H_ */
