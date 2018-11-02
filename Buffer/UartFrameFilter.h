/*
 * UartFrameFilter.h
 *
 *  Created on: 2018Äê10ÔÂ25ÈÕ
 *      Author: LiuBin

 */
#ifndef _UARTANALYZER_H
#define  _UARTANALYZER_H

#pragma once
#include "RingBuffer.h"
static const unsigned int MAX_FRAME_CNT_IN_BUFFER = 4;
static const unsigned int MAX_BYTE_CNT_IN_FRAME = 30;
typedef int(*FlagHook)(unsigned char flag);
typedef enum CheckType
{
	XOR = 0,
	SUM = 1
};
typedef enum RcvMode
{
	AUTO = 0,
	MANUAL = 1
};
class UartFrameFilter
{
public:
	UartFrameFilter
	(
		unsigned int nFrameLen,				
		unsigned int byteHead,				
		unsigned int nCheckSumIndex,        
		unsigned int nCheckSumLen,          
		unsigned int nCheckStartIndex = 0,
		CheckType nCheckType = XOR,       // XOR & SUM &...
		unsigned int FrameBufferDepth = MAX_FRAME_CNT_IN_BUFFER,
		unsigned int byteTail = 0,
		unsigned int byteFlag = 0,
		unsigned int flagIndex = 1,
		FlagHook pfun = NULL,
		RcvMode rcvMode = MANUAL
	);
	~UartFrameFilter();
private:
	unsigned int m_frameLen;
	unsigned int m_head;
	unsigned int m_checkByteIndex;
	unsigned int m_checkLen;

	unsigned int m_checkStartIndex;
	CheckType m_checkType;
	unsigned int m_FrameBufferDepth;
	unsigned int m_tail;
	unsigned int m_flag;
	unsigned int m_flagIndex;
	FlagHook m_flagFun;

	unsigned long m_analyzedFrameCnt;
	unsigned long m_rcvByteCnt;
	unsigned long m_errorByteCnt;
	
	RingBuffer m_rcvByteBuffer;
	RingBuffer m_FrameBuffer;	
public:
	unsigned int PushIn(const unsigned char* src, unsigned int Len_b);
	unsigned int FrameScreening();
	inline int PickFrontFrame(unsigned char *pFrame) { return m_FrameBuffer.PickFront(pFrame, 1); }
	inline int PickLatestFrame(unsigned char *pFrame) { return m_FrameBuffer.PickBack(pFrame, 1); }
	inline unsigned int GetFrameCntInBuffer() { return m_FrameBuffer.GetUsedSize(); }
	inline void ClearBuffer() { m_FrameBuffer.Clean(); }

	inline unsigned int GetRcvBufferLen() { return m_rcvByteBuffer.GetUsedSize(); }

	inline void SetFrameLen(unsigned int Len) { m_frameLen = Len; }
	inline void SetFlag(unsigned int flag) { m_flag = flag; }
	inline void SetFlagHook(FlagHook fun) { m_flagFun = fun; }

private:
	int HeadShoot();
	int TailHold();
	int FlagMatch();
	int Check();
	void CutFrontByte();
};

#endif