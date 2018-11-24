/*
 * InDebounce.h
 *
 *  Created on: 2018Äê10ÔÂ10ÈÕ
 *      Author: LiuBin
 */
#ifndef _IODEBOUNCE_H
#define  _IODEBOUNCE_H
#pragma once
#include "RingBuffer.h"

#define IOBITS_64b uint64_t
#define BITNUMBER 64

class IoDebounce
{
public:
	/**
	 * 1-Bits in MASK means these bits shouldn't be eliminated jitters
	 * MASK = 0 means all IO bits should be eliminated jitters
	 */
	IoDebounce(IOBITS_64b InitIOs,
				unsigned int Depth, 
				unsigned int Threshold,
				IOBITS_64b MASK = 0
				 );
	~IoDebounce();

	IOBITS_64b JitterControl(IOBITS_64b IOs);
	inline IOBITS_64b GetOutput(){ return m_OutPutValue; }
	void SetFilter(unsigned int* depthArray, unsigned int* thresholdArray);
private:
	IOBITS_64b m_Mask;
	IOBITS_64b m_InitValue;
	IOBITS_64b m_OutPutValue;
	typedef struct _FILTER_PARAM
	{
		unsigned int Depths[BITNUMBER];
		unsigned int Threshold[BITNUMBER];

	};
	_FILTER_PARAM m_FilterParameters;
	RingBuffer m_ChangedBitBuffer;
};
#endif