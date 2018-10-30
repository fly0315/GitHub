#include "IODebounce.h"
IoDebounce::IoDebounce(IOBITS_64 InitIOs,
				unsigned int Depth, 
				float Prevail,
				IOBITS_64 MASK)
	: m_InitValue(InitIOs)
	, m_OutPutValue(InitIOs)
	, m_nDepth(Depth)
	, m_Mask(MASK)
	, m_LVL_H((unsigned int)(Prevail * 100.0f * Depth))
	, m_LVL_L((unsigned int)((float)Depth * 100.0f - m_LVL_H))
	, m_Buffer(m_nDepth, sizeof(IOBITS_64))
	, m_TableOfChangeBit(sizeof(IOBITS_64) * 8,sizeof(unsigned int))
{
	assert(m_nDepth != 0);
	m_Buffer.Initialize(); 
	m_TableOfChangeBit.Initialize();
	while (m_Buffer.GetState() != FULL)
	{
		m_Buffer.Add(&InitIOs, 1);
	}
}
IoDebounce::~IoDebounce()
{

}

IOBITS_64 IoDebounce::Sampling(IOBITS_64 IOs)
{
	// 1.add sample IO data in buffer.
	m_Buffer.Add(&IOs, 1);
	// 2.make a changed bits table
	// 3.change output data with changed bit table
	unsigned int TblElem;
	for (unsigned int i = MakeTable(IOs); i ; i--)
	{
		m_TableOfChangeBit.ReadFrontAt(i - 1, &TblElem);
		if ((GetSum(TblElem) * 100) >= m_LVL_H)
		{
			SetBit(m_OutPutValue, GetIndex(TblElem));
		}
		else if ((GetSum(TblElem) * 100) <= m_LVL_L)
		{
			// +0.001f for accuracy loss compensation
			ClrBit(m_OutPutValue, GetIndex(TblElem));
		}
	}
	// 4.modify the mask bit
	m_OutPutValue = (m_OutPutValue & (~m_Mask)) + (IOs & m_Mask);
	return m_OutPutValue;
}

unsigned int IoDebounce::MakeTable(IOBITS_64 IOs)
{
	IOBITS_64 ChangeBits = (IOs ^ m_OutPutValue) & (~m_Mask);
	m_TableOfChangeBit.Clean();
/**	// search from LSB.
 *	// loop number depending on the highest 1-bit index.
 *  // abandoned for Inefficient. LiuBin.20181020
 *	for (unsigned int i = 0; data; i++, data >>= 1)
 *	{
 *		if (data & 1)
 *		{
 *			m_TableOfChangeBit.Add(&i, 1);
 *		}
 *	}
 */
	// loops depending on how many 1-bit in data.
	unsigned int offsetLen = 0;
	IOBITS_64 IoData, temp;
	unsigned int n, TblElem, nDepth;
	// the number of changed IO bits always not large 
	// so make it outer loop and make sample depth inner loop .
	while (ChangeBits)
	{

		// 1.find the highest 1-bit.
		/**
		 * abandoned for Inefficient. liubin.20181027
		// bi-search, starting from MSB (also can starting from LSB).
		temp = ChangeBits;
		n = 1;
		if (0 == (temp >> 32)) { n += 32; temp <<= 32; }
		if (0 == (temp >> 48)) { n += 16; temp <<= 16; }
		if (0 == (temp >> 56)) { n += 8; temp <<= 8; }
		if (0 == (temp >> 60)) { n += 4; temp <<= 4; }
		if (0 == (temp >> 62)) { n += 2; temp <<= 2; }
		n -= (temp >> 63);
		TblElem = 63 - n - offsetLen;		// highest 1-bit index.
		offsetLen += (n + 1);				// recode offset.
		ChangeBits <<= (n + 1);
		*/
		// fast-log2(x)-search,starting from LSB.LiuBin.20181027
		temp = ChangeBits & (ChangeBits - 1);		// remaining is lowest 1-bit from right
		TblElem = fastlog2_64b(ChangeBits - temp);	// lowest 1-bit index
		ChangeBits = temp;							// loop and find rest
		// 2.calculate sum with bit index
		for (nDepth = m_nDepth - 1; nDepth ; nDepth--)
		{
			m_Buffer.ReadBackAt(nDepth, &IoData);
			MakeElement_Sum(TblElem, IoData);
		}
		// 3.and add into table
		m_TableOfChangeBit.Add(&TblElem, 1);
	}
	return m_TableOfChangeBit.GetUsedSize();
}