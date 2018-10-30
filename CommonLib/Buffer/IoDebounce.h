/*
 * InDebounce.h
 *
 *  Created on: 2018年10月10日
 *      Author: LiuBin
 */
#ifndef _IODEBOUNCE_H
#define  _IODEBOUNCE_H
#pragma once
#include "RingBuffer.h"

#define IOBITS_64 unsigned long long

inline unsigned long long GetBit(IOBITS_64 data, unsigned int i) { return ((data >> i) & 1u); }
inline void SetBit(IOBITS_64 &data, unsigned int i) { (data |= ((IOBITS_64)1 << i)); }
inline void ClrBit(IOBITS_64 &data, unsigned int i) { (data &= ~((IOBITS_64)1 << i)); }

inline unsigned int CountBit1(IOBITS_64 n)
{
	unsigned int cnt = 0;
	while (n)
	{
		cnt++;			// n != 0 then cnt++
		n  &= (n - 1);	// set the lowest high bit to low
	}
	return cnt;
}
inline unsigned int FastCountBit1(unsigned int data)
{
	unsigned int temp = data;
	temp = (temp & 0x55555555) + ((temp >> 1) & 0x55555555);  //temp相邻位相加  
	temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333);  //temp相邻（以2为单位）相加
	temp = (temp & 0x0f0f0f0f) + ((temp >> 4) & 0x0f0f0f0f);    //temp相邻（以4为单位）相加
	temp = (temp & 0xff00ff) + ((temp >> 8) & 0xff00ff);       //temp相邻（以8为单位）相加
	temp = (temp & 0xffff) + ((temp >> 16) & 0xffff);          //temp相邻（以16为单位）相加
	return temp;

}
/**
 * find first 1-bit on left.
 * nBitCnt = sizeof(unsigned int) * 8
 * for example 32bit-data:
 * int f1(unsigned x)
 * {
 *	 int n=1;
 *	 if(x==0) return -1;
 *	 if ((x>>16) == 0) {n = n+16; x = x<<16;}
 *	 if ((x>>24) == 0) {n = n+8; x = x<<8;}
 *	 if ((x>>28) == 0) {n = n+4; x = x<<4;}
 *	 if ((x>>30) == 0) {n = n+2; x = x<<2;}
 *	 n = n-(x>>31);
 *	 return 31-n;
 * }
 */
inline int f1b(IOBITS_64 Data, unsigned int nByteCnt)
{
	if (0 == Data) { return -1; }
	unsigned int n = 1;
	unsigned int temp = (nByteCnt * 8) - 1;
	for (int div = nByteCnt * 4, i = div; div > 1; div >>= 1, i += div)
	{
		if (0 == (Data >> i)) 
		{ 
			n += div; 
			Data <<= div;
		}
	}
	n -= (Data >> temp);
	return temp - n;
}

inline int f1b64(IOBITS_64 Data)
{
	if (0 == Data) { return -1; }
	unsigned int n = 1;
	if (0 == (Data >> 32)) { n += 32; Data <<= 32; }
	if (0 == (Data >> 48)) { n += 16; Data <<= 16; }
	if (0 == (Data >> 56)) { n += 8; Data <<= 8; }
	if (0 == (Data >> 60)) { n += 4; Data <<= 4; }
	if (0 == (Data >> 62)) { n += 2; Data <<= 2; }
	n -= (Data >> 63);
	return 63 - n;
}

class IoDebounce
{
public:
	/**
	 * 1-Bits in MASK means these bits shouldn't be eliminated Jitters
	 * MASK = 0 means all IO bits should be eliminated jitters
	 * Prevail = 0.75 means 75% prevail.
	 */
	IoDebounce(IOBITS_64 InitIOs,
				unsigned int Depth, 
				float Prevail,
				IOBITS_64 MASK = 0
				 );
	~IoDebounce();
	IOBITS_64 Sampling(IOBITS_64 IOs);
	inline IOBITS_64 GetOutput(){ return m_OutPutValue; }
private:
	IOBITS_64 m_Mask;
	IOBITS_64 m_InitValue;
	IOBITS_64 m_OutPutValue;
	unsigned int m_nDepth; 
	unsigned int m_LVL_H;
	unsigned int m_LVL_L;
	RingBuffer m_Buffer;
	/**
	 * Table element structure: 
	 *		lowest 8-bits for changed bit index 
	 *		and the rest for prevail-sum-value.
	 * max-element number is 64
	 */
	RingBuffer m_TableOfChangeBit; 
	unsigned int MakeTable(IOBITS_64 data);
	inline unsigned int SetIndex(unsigned int val, unsigned char index)
	{
		return (val & (~0xFF)) & (index & 0xFF);
	}
	inline unsigned int GetIndex(unsigned int val)
	{
		return (val & 0xFF);
	}
	inline void MakeElement_Sum(unsigned int& elem, IOBITS_64 IO)
	{
		elem += ((unsigned int)GetBit(IO, GetIndex(elem)) << 8);
	}
	inline unsigned int GetSum(unsigned int val)
	{
		return (val >> 8);
	}
};
#endif