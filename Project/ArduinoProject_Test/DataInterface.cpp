#include "DataInterface.h"
#include "pt-1.4/PtThreadEx.h"
#include <math.h>
PtThreadEx ThreadQ[TASKS_MAX];
DisplayInfo TubeShowInfo;
SPEED_PARAM SpeedParams;
E2PROM_Params RamData;
ConfigST AdjustST;
DisplayType ShowType;
DataInterface::DataInterface()
{
}


DataInterface::~DataInterface()
{
}

SPEED_PARAM::SPEED_PARAM()
	:d(0), L(0), Speed(0)
{

}
SPEED_PARAM::~SPEED_PARAM()
{

}

DisplayInfo::DisplayInfo()
	:CNT(0)
{
	//code of number 0~9         
	const unsigned char  arr[10] = 
		{ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };
	for (int i = 0; i < 10; i++)
	{
		Numbers[i] = arr[i];
	}
}
DisplayInfo::~DisplayInfo()
{

}

void DisplayInfo::MakeInfo(unsigned long MSG)
{
	for (int i = 0; i < 10; i++)
	{
		InfoArray[i] = 0x00;
	}

	/**
	 * Make Time Information.
	 */
	CNT = 0;
	// 将数据按十进制位拆分，由高至低temp[0]~temp[9]
	while (MSG > 0)
	{
		InfoArray[9 - CNT] = Numbers[MSG % 10];
		CNT++;
		MSG *= 0.1;
	}
	/**
	* 补“0”。
	* CNT值必然小于9:uTime最大值为2^32(um)=4294967296(um)=4294.967296(s)=71.582(min)
	* 所以设备在上电连续工作1小时且中间没有复位过的情况下，应断电重置或复位重置。
	* uTime*0.01后最大值为8位
	*
	* i=5,个位补0;
	* i=4,0.1位补0;
	* i=3,0.01位补0;
	* i=2,0.001位补0
	*/
	for (int i = CNT; i < 5; i++)
	{
		if (i != 1)
		{
			InfoArray[9 - i] = Numbers[0];
		}

	}
	InfoArray[5] += 0x80;	// 在第6位后加小数点,显示精度为0.1ms
}

// return (%s)
double SPEED_PARAM::Caculate(double t)
{
// 	return Speed = (2 * (atan(d / (2 * L)) * 180 / PI) / t);
	return Speed = 2 * atan2(d * 0.5, L) * 180 * 0.31830988618379067153776752674503 / t;
}



static unsigned int g_CurTaskIndex = 0;
void ThreadSchedule()
{
	for (g_CurTaskIndex = 0; ThreadQ[g_CurTaskIndex].ID != THREAD_NULL; g_CurTaskIndex++)
	{
		ThreadQ[g_CurTaskIndex].Schedule();
	}
}
