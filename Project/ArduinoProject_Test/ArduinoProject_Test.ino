#include <tm1638.h>
#include "pt-1.4/BSP_cfg.h"
#include "pt-1.4/PtThreadEx.h"
#include "UnlockFifoBuffer/UnlockFifoBuffer.h"
#include "DataInterface.h"
#include <IRremote.h>;
#include <EEPROM.h>

unsigned long uTime = 0;	// 扫过两列传感器的时间间隔
unsigned long curTimeCnt = 0;
unsigned long preTimeCnt = 0;

enum AD_INDEX
{
	CH_A = 0,
	CH_B = 1,
	CHANNEL_NUM
}adIndex;

struct AD_CHANNEL
{
	AD_INDEX INDEX;
	unsigned int PIN;
	int BASE_VALUE;		// the base line of the sensor value
};

AD_CHANNEL adChannels[] =
{
	{CH_A, A1, 0},
	{CH_B, A0, 0}
};

int ThreshHold = 70;	// IF (curValue - BaseValue > Threshold): trige time-meter
const int TIME_INTERVAL = 1000;	// 两次计时间隔ms

TimeMeterST tmST; // STOP=0;START_A=1;START_B=2;INIT=3
TimeMeterST showST; // SHOW_TIME = 10; SHOW_SPEED = 11;
/**************************
** tm1638 tube
**************************/
/**
**	     0x01
**	      -
**  0x20 | | 0x02
**  0x40  -
**  0x10 | | 0x04
**        -       .  0x80
**	     0x08
*/
//each segment of a tube
const unsigned char  Seg_test[8] = { 0x20,0x01,0x02,0x04,0x08,0x10,0x40,0x80 };
//temp arrary for tube to show
unsigned char initShow_Welcome[10] = { 0x76,0x79,0x38,0x38,0xbf,0x88,0x5b,0x3f,0x86,0x88 };	 // HELLO._.201._.
//from 0x00~0x07
enum DISPLAY_LIGHT_LEVEL
{
	LOW_L = 1,
	MEDIUM_L = 3,
	HIGH_L = 5,
	HIGHEST_L = 7,
};
DISPLAY_LIGHT_LEVEL lightLevel = LOW_L;
const int pin_STB = 12;
const int pin_CLK = 11;
const int pin_DIO = 10;
TM1638 tm1638(pin_STB, pin_CLK, pin_DIO, (unsigned char)lightLevel);
extern DisplayInfo TubeShowInfo;
extern SPEED_PARAM SpeedParams;
// 蜂鸣器
const int pin_BUZZER = 13;
// 遥控器
int RECV_PIN = 5;

IRrecv irrecv(RECV_PIN);
decode_results results;
IrRemoteKey Key_Cur;

/**
 * NEW
 */
extern PtThreadEx ThreadQ[];
extern DisplayInfo TubeShowInfo;
extern E2PROM_Params RamData;
extern ConfigST AdjustST;
extern DisplayType ShowType;
void InitData();
static int Task_TimeMeter(struct pt *pt);
static int Task_Display(struct pt *pt);
static int Task_Speaker(struct pt *pt);

/*STOP = 0,START_A=1,START_B=2,INIT=3,
	CONFIG_d=4,
	CONFIG_L=5,
	SHOW_TIME=6,
	SHOW_SPEED=7,*/

void TMMT_Stop();
void TMMT_CHA();
void TMMT_CHB();
void TMMT_Init();
void TMMT_Config_d();
void TMMT_Config_L();
void TMMT_Config_V();
void KeyBeep();
void KeyFun(unsigned int Key,unsigned long* data);
pFUN TimeMeterFuns[] =
{
	TMMT_Stop,
	TMMT_CHA,
	TMMT_CHB,
	TMMT_Init,
	TMMT_Config_d,
	TMMT_Config_L,
	TMMT_Config_V
};

void setup() {				
	// DEFAULT: the default analog reference of 5 volts(on 5V Arduino boards) or 3.3 volts(on 3.3V Arduino boards)
	// INTERNAL : an built - in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8(not available on the Arduino Mega)
	// EXTERNAL : the voltage applied to the AREF pin(0 to 5V only) is used as the reference.
	analogReference(INTERNAL);
	// 蜂鸣器
	pinMode(pin_BUZZER, OUTPUT);

	// 红外遥控器
// 	irrecv.enableIRIn(); // Start the receiver
	/************************************************************************/
	/* AD采样Prescaler越小精度越差，默认为128
	** 所以假設 Arduino 的時脈(Clock)是 16MHz,
	**	則 ADC clock = 16MHz / 128 = 125KHz;
	**	一次 ADC 轉換要踢它13下, 就是要 13 clock (tick),
	**	於是變成 125KHz/13 = 9615Hz
	** 默认为9615Hz~10kHz
	** Prescaler=8理论频率为153.846kHz;=16理论值为77kHz;=4---307.69kHz;
	** 本程序为两通道采样,采样频率还要除2                                                                    */
	/************************************************************************/
	setPrescale(8);		// 设定预分频因子：128--111；64--110；32--101；16--100；8--011；4--010；
						// start serial port at 9600 bps:
	Serial.begin(115200);
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for native USB port only
	}
	InitData();
	ConfigCpuTimer1(PERIOD_TIMER1_MS);
	StartCpuTimer1();

	Serial.println("Enabling IRin");
	irrecv.enableIRIn(); // Start the receiver
	Serial.println("Enabled IRin");
}

void loop() {

	// Task event setting.
	ThreadQ[TKIN_TMMT].SetState(READY);
	ThreadQ[TKIN_SPEAKER].SetState(READY);
	// Multi-Threads Schedule.
	ThreadSchedule();
// 	ThreadQ[TKIN_TMMT].Schedule();
// 	ThreadQ[TKIN_DISPLAY].Schedule();
// 	ThreadQ[TKIN_SPEAKER].Schedule();
	
}

void InitData()
{
	tmST = INIT;
	AdjustST = NO_ADJ;
	uTime = 0;

	ThreadQ[TKIN_TMMT].CreateTask(
		TKIN_TMMT, Task_TimeMeter, 0, WAIT, NULL);
	ThreadQ[TKIN_DISPLAY].CreateTask(
		TKIN_DISPLAY, Task_Display, 0, WAIT, NULL);
	ThreadQ[TKIN_SPEAKER].CreateTask(
		TKIN_SPEAKER, Task_Speaker, 0, WAIT, NULL);

}

int Task_TimeMeter(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, (ThreadQ[TKIN_TMMT].GetState()));
		//////////////////////////////////////////////////////////////////////////
		if ((tmST == START_A) || (tmST == START_B))
		{
			TimeMeterFuns[tmST]();
			// 防止刚刚停表又马上激发。两次及时间隔TIME_INTERVAL（MS）
			if (tmST == STOP)
			{
				ThreadQ[TKIN_TMMT].SLEEP_MS(TIME_INTERVAL);
				PT_WAIT_UNTIL(pt, ThreadQ[TKIN_TMMT].IsAwake_MS());
			}
		}
		else
		{
			TimeMeterFuns[tmST]();	
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadQ[TKIN_TMMT].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
int Task_Display(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, (ThreadQ[TKIN_DISPLAY].GetState()));
		//////////////////////////////////////////////////////////////////////////
		if (STOP == tmST)
		{
			if (uTime == 0)
			{
				Serial.println(adChannels[CH_A].BASE_VALUE);
				Serial.println(adChannels[CH_B].BASE_VALUE);
				tm1638.SET_LIGHT_LEVEL((unsigned char)lightLevel);
			}
			else
			{
				if (ShowType == TIMEMETER)
				{
					TubeShowInfo.MakeInfo(uTime*0.01);
					TubeShowInfo.InfoArray[0] = 0x31;	// first character 'T'
				}
				else if (ShowType == SPEED)
				{
					SpeedParams.Caculate(uTime*0.000001);
					TubeShowInfo.MakeInfo((unsigned long)(SpeedParams.Speed * 10000));
					TubeShowInfo.InfoArray[0] = 0x6D;  // first character 'S'
				}
				tm1638.DisplayArr_TM1638(1, TubeShowInfo.InfoArray, 10);
				Serial.println(uTime*0.000001, 6);
			}
		}
		else if (INIT == tmST)
		{
			static int InitCnt = 0;
			// Tube Test
			if (InitCnt < 9)
			{
				for (int j = 0; j < 10; j++)
				{
					tm1638.DisplayOneDigi_TM1638(j + 1, Seg_test[InitCnt - 1]);
				}
				delay(300);
			}
			else
			{
				tm1638.DisplayArr_TM1638(1, initShow_Welcome, 10);
			}
			InitCnt++;
		}
		else if (CONFIG_d == tmST)
		{
			
			TubeShowInfo.MakeInfo(RamData.d_01mm);
			if (AdjustST == ADJ_ING)
			{
				TubeShowInfo.InfoArray[1] = 0x6D;	// S
				TubeShowInfo.InfoArray[2] = 0x31;	// T
			}
			TubeShowInfo.InfoArray[0] = 0x5E;	// d
			tm1638.DisplayArr_TM1638(1, TubeShowInfo.InfoArray, 10);
			
		}
		else if (CONFIG_L == tmST)
		{
			TubeShowInfo.MakeInfo(RamData.L_01mm);
			if (AdjustST == ADJ_ING)
			{
				TubeShowInfo.InfoArray[1] = 0x6D;	// S
				TubeShowInfo.InfoArray[2] = 0x31;	// T
			}
			TubeShowInfo.InfoArray[0] = 0x38;	// L
			tm1638.DisplayArr_TM1638(1, TubeShowInfo.InfoArray, 10);
		}
		else if (CONFIG_VIEW == tmST)
		{
			unsigned char temp[] = 
			{
				0x6D,0x31,0x00,0x00,0x86,
				0x31,0x00,0x00,0xDB,0x6D
			};
			for (int i = 0; i < 10; i++)
			{
				TubeShowInfo.InfoArray[i] = temp[i];
			}
			if (RamData.ShowType == (unsigned int)TIMEMETER)
			{
				TubeShowInfo.InfoArray[3] = 0x5C;
			}
			else if (RamData.ShowType == (unsigned int)SPEED)
			{
				TubeShowInfo.InfoArray[7] = 0x5C;
			}
			tm1638.DisplayArr_TM1638(1, TubeShowInfo.InfoArray, 10);
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadQ[TKIN_DISPLAY].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
int Task_Speaker(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, (ThreadQ[TKIN_SPEAKER].GetState()));
		//////////////////////////////////////////////////////////////////////////
		
		if (STOP == tmST)
		{
			digitalWrite(pin_BUZZER, LOW);
		}
		else if (INIT == tmST)
		{
			digitalWrite(pin_BUZZER, HIGH);
			ThreadQ[TKIN_SPEAKER].SLEEP_MS(20);
			PT_WAIT_UNTIL(pt, ThreadQ[TKIN_SPEAKER].IsAwake_MS());
			digitalWrite(pin_BUZZER, LOW);
			ThreadQ[TKIN_SPEAKER].SLEEP_MS(20);
			PT_WAIT_UNTIL(pt, ThreadQ[TKIN_SPEAKER].IsAwake_MS());
			
		}
		else if (START_A == tmST)
		{
			digitalWrite(pin_BUZZER, HIGH);
		}
		else if (START_B == tmST)
		{
			digitalWrite(pin_BUZZER, HIGH);
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadQ[TKIN_SPEAKER].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}

void TMMT_Init()
{
	static int sampleCnt = 0;
	static int adValues[CHANNEL_NUM][11];

	if (sampleCnt < 11)
	{
		adValues[CH_A][sampleCnt] = analogRead(adChannels[CH_A].PIN);
		adValues[CH_B][sampleCnt] = analogRead(adChannels[CH_B].PIN);
		sampleCnt++;
	}
	else
	{
		tmST = STOP; sampleCnt = 0;
		adChannels[CH_A].BASE_VALUE = adChannels[CH_B].BASE_VALUE = 0;
		// 读取传感器的值作为基准值，之后读取的传感器值与该值作差值，判断结果是否大于某一阈值
		for (int i = 0; i < 10; i++)
		{
			adChannels[CH_A].BASE_VALUE += adValues[CH_A][i + 1];
			adChannels[CH_B].BASE_VALUE += adValues[CH_B][i + 1];
		}
		adChannels[CH_A].BASE_VALUE *= 0.1;	// 10次平均
		adChannels[CH_B].BASE_VALUE *= 0.1;

		EEPROM.get(0, RamData);
		SpeedParams.d = RamData.d_01mm * 0.0001;
		SpeedParams.L = RamData.L_01mm * 0.0001;
		ShowType = (DisplayType)RamData.ShowType;

		static int maxValue = max(adChannels[CH_A].BASE_VALUE, adChannels[CH_B].BASE_VALUE);
		if (maxValue < 150)
		{
			lightLevel = LOW_L;
			ThreshHold = 60;
		}
		else if ((maxValue < 200) && (maxValue >= 150))
		{
			lightLevel = MEDIUM_L;
			ThreshHold = 40;
		}
		else if ((maxValue < 250) && (maxValue >= 200))
		{
			lightLevel = HIGH_L;
			ThreshHold = 20;
		}
		else if (maxValue >= 250)
		{
			lightLevel = HIGHEST_L;
			ThreshHold = 15;
		}
	}
	
	if (irrecv.decode(&results))
	{
		Key_Cur = (IrRemoteKey)results.value;
		irrecv.resume(); // Receive the next value

		tmST = CONFIG_d;
		Serial.print("Show Config Key:");
		Serial.println(results.value, HEX);

		EEPROM.get(0, RamData);
		SpeedParams.d = RamData.d_01mm * 0.0001;
		SpeedParams.L = RamData.L_01mm * 0.0001;
		ShowType = (DisplayType)RamData.ShowType;

		tm1638.SET_LIGHT_LEVEL(HIGH_L);
		KeyBeep();
	}

	// Send Display MSG.
	ThreadQ[TKIN_DISPLAY].SetState(READY);
	
}
void TMMT_Stop()
{
	do 
	{
		if (
			analogRead(adChannels[CH_A].PIN)
			- adChannels[CH_A].BASE_VALUE > ThreshHold
			)
		{
			preTimeCnt = curTimeCnt = micros();
			tmST = START_A;
			break;
		}

		if (
			analogRead(adChannels[CH_B].PIN)
			- adChannels[CH_B].BASE_VALUE > ThreshHold
			)
		{
			preTimeCnt = curTimeCnt = micros();
			tmST = START_B;
			break;
		}
	} while (0);
}
void TMMT_CHA()
{
	if (
		analogRead(adChannels[CH_B].PIN)
		- adChannels[CH_B].BASE_VALUE < ThreshHold
		)
	{
		return;
	}
	// calculate elapse time 
	curTimeCnt = micros();
	uTime = curTimeCnt - preTimeCnt; // us
	// stop the time-meter
	tmST = STOP;
	ThreadQ[TKIN_DISPLAY].SetState(READY);
}
void TMMT_CHB()
{
	if (
		analogRead(adChannels[CH_A].PIN)
		- adChannels[CH_A].BASE_VALUE < ThreshHold
		)
	{
		return;
	}
	// calculate elapse time 
	curTimeCnt = micros();
	uTime = curTimeCnt - preTimeCnt; // us
	// stop the time-meter
	tmST = STOP;
	ThreadQ[TKIN_DISPLAY].SetState(READY);
}
void TMMT_Config_d()
{
	if (irrecv.decode(&results))
	{
		Key_Cur = (IrRemoteKey)results.value;
		irrecv.resume(); // Receive the next value
		KeyBeep();
	}
	else
	{
		return;
	}
	switch (Key_Cur)
	{
	case KEY_CH_PLUS:
		tmST = CONFIG_L;
		if (AdjustST == ADJ_ING)
		{
			if (RamData.d_01mm < 1000 || RamData.d_01mm > 10000)
			{
				RamData.d_01mm = SpeedParams.d * 10000;
			}
		}
		AdjustST = NO_ADJ;
		break;
	case KEY_PLAY:
		if (AdjustST == ADJ_ING)
		{
			if (RamData.d_01mm < 1000 || RamData.d_01mm > 10000)
			{
				RamData.d_01mm = SpeedParams.d * 10000;
			}
		}
		(AdjustST == NO_ADJ)
			? (AdjustST = ADJ_ING)
			: (AdjustST = NO_ADJ);
		break;
	default:
		KeyFun(Key_Cur, &RamData.d_01mm);
		break;
	}

	ThreadQ[TKIN_DISPLAY].SetState(READY);
}
void TMMT_Config_L()
{
	if (irrecv.decode(&results))
	{
		Key_Cur = (IrRemoteKey)results.value;
		irrecv.resume(); // Receive the next value
		KeyBeep();
	}
	else
	{
		return;
	}
	switch (Key_Cur)
	{
	case KEY_CH_MINUS:
		tmST = CONFIG_d;
		if (AdjustST == ADJ_ING)
		{
			if (RamData.L_01mm < 50000 || RamData.L_01mm > 200000)
			{
				RamData.L_01mm = SpeedParams.L * 10000;
			}
		}
		AdjustST = NO_ADJ;
		break;
	case KEY_CH_PLUS:
		tmST = CONFIG_VIEW;
		if (AdjustST == ADJ_ING)
		{
			if (RamData.L_01mm < 50000 || RamData.L_01mm > 200000)
			{
				RamData.L_01mm = SpeedParams.L * 10000;
			}
		}
		AdjustST = NO_ADJ;
		break;
	case KEY_PLAY:
		if (AdjustST == ADJ_ING)
		{
			if (RamData.L_01mm < 50000 || RamData.L_01mm > 200000)
			{
				RamData.L_01mm = SpeedParams.L * 10000;
			}
		}
		(AdjustST == NO_ADJ)
			? (AdjustST = ADJ_ING)
			: (AdjustST = NO_ADJ);
		break;
	default:
		KeyFun(Key_Cur, &RamData.L_01mm);
		break;
	}

	ThreadQ[TKIN_DISPLAY].SetState(READY);
}
void TMMT_Config_V()
{
	if (irrecv.decode(&results))
	{
		Key_Cur = (IrRemoteKey)results.value;
		irrecv.resume(); // Receive the next value
		KeyBeep();
	}
	else
	{
		return;
	}
	switch (Key_Cur)
	{
	case KEY_CH_MINUS:
		tmST = CONFIG_L;
		break;
	case KEY_CH:
		tmST = INIT;
		SpeedParams.d = RamData.d_01mm * 0.0001;
		SpeedParams.L = RamData.L_01mm * 0.0001;
		ShowType = (DisplayType)RamData.ShowType;
		EEPROM.put(0, RamData);
		break;
		
	case KEY_NEXT:
		RamData.ShowType = (unsigned int)SPEED;
		break;
	case KEY_PRE:
		RamData.ShowType = (unsigned int)TIMEMETER;
		break;

	case KEY_1:
		RamData.ShowType = (unsigned int)TIMEMETER;
		break;
	case KEY_2:
		RamData.ShowType = (unsigned int)SPEED;
		break;

	case KEY_MINUS:
		RamData.ShowType = (unsigned int)TIMEMETER;
		break;
	case KEY_PLUS:
		RamData.ShowType = (unsigned int)SPEED;
		break;
	default:
		break;
	}
	
	ThreadQ[TKIN_DISPLAY].SetState(READY);

}
void KeyFun(unsigned int Key, unsigned long* data)
{
	if (AdjustST != ADJ_ING) { return; }
	
	switch (Key)
	{
	case KEY_EQ:
		*data = 0;
		break;
	case KEY_PLUS:
		*data += 1;
		break;
	case KEY_MINUS:
		*data -= 1;
		break;
	case KEY_100_PLUS:
		*data += 100;
		break;
	case KEY_200_PLUS:
		*data += 200;
		break;
	case KEY_0:
		*data *= 10;
		*data += 0;
		break;
	case KEY_1:
		*data *= 10;
		*data += 1;
		break;
	case KEY_2:
		*data *= 10;
		*data += 2;
		break;
	case KEY_3:
		*data *= 10;
		*data += 3;
		break;
	case KEY_4:
		*data *= 10;
		*data += 4;
		break;
	case KEY_5:
		*data *= 10;
		*data += 5;
		break;
	case KEY_6:
		*data *= 10;
		*data += 6;
		break;
	case KEY_7:
		*data *= 10;
		*data += 7;
		break;
	case KEY_8:
		*data *= 10;
		*data += 8;
		break;
	case KEY_9:
		*data *= 10;
		*data += 9;
		break;
	
	default:

		break;
	}
}
void KeyBeep()
{
	digitalWrite(pin_BUZZER, HIGH);
	delay(50);
	digitalWrite(pin_BUZZER, LOW);
}
