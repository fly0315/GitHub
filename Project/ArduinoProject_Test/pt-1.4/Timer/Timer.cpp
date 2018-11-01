#include "Timer.h"
#include <Arduino.h>
#include "../PtThreadEx.h"
extern PtThreadEx ThreadQ[];

void OnTimer1()
{
	noInterrupts();
	// The CPU acknowledges the interrupt.

	for (int i = 0; (ThreadQ[i].ID != THREAD_NULL); i++)
	{
		ThreadQ[i].TimerRemarks();
		ThreadQ[i].SleeperRemarks_MS();
	}
	interrupts();
}

void OnTimer2()
{
	noInterrupts();
	// The CPU acknowledges the interrupt.

	for (int i = 0; (ThreadQ[i].ID != THREAD_NULL); i++)
	{
		ThreadQ[i].TimerRemarks();
		ThreadQ[i].SleeperRemarks_MS();
	}
	interrupts();
}
