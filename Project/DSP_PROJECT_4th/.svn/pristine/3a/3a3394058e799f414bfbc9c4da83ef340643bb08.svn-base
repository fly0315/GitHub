/*
 * UnlockFifoBuffer.h
 *
 *  Created on: 2018Äê1ÔÂ8ÈÕ
 *      Author: LiuBin
 */

#ifndef MAKEDATA_H_
#define MAKEDATA_H_

#pragma once

class MakeData
{
public:
	MakeData();
    virtual ~MakeData();
public:
    inline void Apart(unsigned long *u32Data, unsigned int *u16High, unsigned int *u16Low)
    {
		u32.all = *u32Data;
		*u16High = u32.part.high;
		*u16Low = u32.part.low;
    }
	inline unsigned long Together(unsigned int *u16High, unsigned int *u16Low)
	{
		u32.part.high = *u16High;
		u32.part.low = *u16Low;
		return u32.all;
	}
protected:

private:
public:
	union _U32
	{
		unsigned long all;
		struct _part
		{
			unsigned int low	: 16;
			unsigned int high	: 16;
		}part;
	}u32;
	unsigned int m_u16Data;
};

#endif /* UNLOCKFIFOBUFFER_H_ */
