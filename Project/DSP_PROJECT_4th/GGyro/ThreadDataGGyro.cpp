/*
 * ThreadDataGGyro.cpp
 *
 *  Created on: 2018Äê4ÔÂ3ÈÕ
 *      Author: LiuBin
 */

#include "ThreadDataGGyro.h"

static unsigned int g_CurTaskIndex = 0;
PtThreadEx ThreadQ[SCHED_MAX];
Semaphore SMPHR[SCHED_MAX];

Semaphore::Semaphore()
	:m_Sem(0)
{
}
void Semaphore::Init(unsigned sem)
{
	m_Sem = sem;
}
void Semaphore::Set()
{
	m_Sem++;
}
unsigned int Semaphore::Get()
{
	return ((m_Sem > 0) ? m_Sem-- : 0);
}

void ThreadSchedule()
{
	for (g_CurTaskIndex = 0; g_CurTaskIndex < SCHED_MAX; g_CurTaskIndex++)
	{
		ThreadQ[g_CurTaskIndex].Schedule();
	}
}

void OSSchedule()
{
	g_CurTaskIndex = SCHED_MAX;
}
