#include "IODebounce.h"
IoDebounce::IoDebounce(IOBITS_u64 InitIOs,
				unsigned int Depth, 
				float Prevail,
				IOBITS_u64 MASK)
	: m_InitValue(InitIOs)
	, m_OutPutValue(InitIOs)
	, m_nDepth(Depth)
	, m_Mask(MASK)
	, m_Threshold((unsigned int)(Prevail * Depth))
	, m_ChangedBitBuffer(m_nDepth, sizeof(IOBITS_u64))
{
	assert(m_nDepth != 0);
	m_ChangedBitBuffer.Initialize(); 
	IOBITS_u64 cb = 0;
	while (m_ChangedBitBuffer.GetState() != FULL)
	{
		m_ChangedBitBuffer.Add(&cb, 1);
	}
}
IoDebounce::~IoDebounce()
{

}

IOBITS_u64 IoDebounce::Sampling(IOBITS_u64 IOs)
{
	static IOBITS_u64 cbi;
	static IOBITS_u64 ChangeBits;
	static IOBITS_u64 CB, temp, LB;
	static unsigned int nDepth, bi, sum;
	cbi = 0;
	// 0. find changed bits
	ChangeBits = (IOs ^ m_OutPutValue) & (~m_Mask);
	// 1 add into buffer for keep change status 
	m_ChangedBitBuffer.Add(&ChangeBits, 1);
	// 2.loops depending on how many changed bits in data.
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
	// the number of changed IO bits always not large 
	// so make it outer loop and make sample depth inner loop .
	// 	unsigned int n, offsetLen = 0;
	while (ChangeBits)
	{
		/*1.find the highest 1-bit.*/
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
		temp = ChangeBits & (ChangeBits - 1);		// remove lowest 1-bit
		LB = ChangeBits - temp;						// find lowest 1-bit
		// fast-log2(x)-search,starting from LSB.LiuBin.20181027.
		// fastlog2 is not necessary.20181102
// 		bi = fastlog2_64b(LB);		// lowest 1-bit index
		ChangeBits = temp;							// for rest loops if any
		/* 2.calculate sum with bit index */
		for (nDepth = m_nDepth, sum = 0; nDepth; nDepth--)
		{
			m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
			sum += ((CB & LB) != 0);
		}
		// 3.and make changed bit index
		if (sum >= m_Threshold)
		{
			cbi += LB;
		}
	}
	if (cbi != 0)
	{
		m_OutPutValue ^= cbi;
		for (nDepth = m_nDepth; nDepth; nDepth--)
		{
			m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
			CB ^= cbi;
			m_ChangedBitBuffer.ModifyBackAt(nDepth - 1, &CB);
		}
	}
	m_OutPutValue = (m_OutPutValue & (~m_Mask)) + (IOs & m_Mask);
	return m_OutPutValue;
}

