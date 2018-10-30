#pragma once
#ifndef DATAINTERFACE_H
#define  DATAINTERFACE_H

typedef enum _TASK_INDEX
{
	// add your task here
	TKIN_TMMT = 0,
	TKIN_DISPLAY,
	TKIN_SPEAKER,
	TKIN_CONFIG,
	//////////////////////////////////////////////////////////////////////////
	TASKS_MAX                       // total task number = 6
} TASK_INDEX;
#define SCHED_MAX (TASKS_MAX)

enum TimeMeterST
{
	STOP = 0,
	START_A,
	START_B,
	INIT,
	CONFIG_d,
	CONFIG_L,
	CONFIG_VIEW,
	SHOW_TIME,
	SHOW_SPEED,
};

enum DisplayType
{
	TIMEMETER = 1,
	SPEED = 2,
};

enum IrRemoteKey
{
	KEY_0 = 0xFF6897,
	KEY_1 = 0xFF30CF,
	KEY_2 = 0xFF18E7,
	KEY_3 = 0xFF7A85,
	KEY_4 = 0xFF10EF,
	KEY_5 = 0xFF38C7,
	KEY_6 = 0xFF5AA5,
	KEY_7 = 0xFF42BD,
	KEY_8 = 0xFF4AB5,
	KEY_9 = 0xFF52AD,
	KEY_PLAY = 0xFFC23D,
	KEY_PRE = 0xFF22DD,
	KEY_NEXT = 0xFF02FD,
	KEY_PLUS = 0xFFA857,
	KEY_MINUS = 0xFFE01F,
	KEY_CH_PLUS = 0xFFE21D,
	KEY_CH = 0xFF629D,
	KEY_CH_MINUS = 0xFFA25D,
	KEY_100_PLUS = 0xFF9867,
	KEY_200_PLUS = 0xFFB04F,
	KEY_EQ = 0xFF906F
};
enum ConfigST
{
	NO_ADJ = 0,
	ADJ_ING
};
class SPEED_PARAM
{
public:
	SPEED_PARAM();
	~SPEED_PARAM();
public:
	double d;
	double L;
	double Speed;
	double Caculate(double t);
private:


};
struct E2PROM_Params
{
	unsigned long d_01mm;
	unsigned long L_01mm;
	unsigned int ShowType;
};

class DisplayInfo
{
public:
	DisplayInfo();
	~DisplayInfo();
public:
	unsigned int CNT;	
	unsigned char InfoArray[10];
private:
	//code of number 0~9         
	unsigned char Numbers[10];

public:
	void MakeInfo(unsigned long MSG);

};

class DataInterface
{
public:
	DataInterface();
	~DataInterface();
};

void ThreadSchedule();

#endif
