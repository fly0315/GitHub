/*
 * ThreadDataGGyro.h
 *
 *  Created on: 2018Äê4ÔÂ3ÈÕ
 *      Author: LiuBin
 */

#ifndef GGYRO_THREADDATAGGYRO_H_
#define GGYRO_THREADDATAGGYRO_H_
#pragma once

#include "PtThreadEx.h"

typedef enum _TASK_INDEX
{
	// add your task here
	TKIN_TX_FR_FRAME = 0,
	TKIN_SCIC,
	TKIN_SCIA,
	TKIN_CANA,
	TKIN_SCIB,
//	TKIN_CANB,
	TKIN_FR_GUARANTEE,				// 5
	//////////////////////////////////////////////////////////////////////////
	TASKS_MAX                       // total task number = 6
} TASK_INDEX;
#define SCHED_MAX (TASKS_MAX)

void OSSchedule();
void ThreadSchedule();

class Semaphore
{
public:
	Semaphore();
	void Init(unsigned sem);
	void Set();
	unsigned int Get();
protected:
private:
	unsigned int m_Sem;
};

#endif /* GGYRO_THREADDATAGGYRO_H_ */
