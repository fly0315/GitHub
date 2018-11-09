#include "IODebounce.h"
IoDebounce::IoDebounce(IOBITS_64b InitIOs,
				unsigned int Depth, 
				unsigned int Threshold,
				IOBITS_64b MASK)
	: m_InitValue(InitIOs)
	, m_OutPutValue(InitIOs)
	, m_Mask(MASK)
{
	if (0 == Depth) { Depth = 1; }
	if (Threshold > Depth) { Threshold = Depth; }
	unsigned int depth[BITNUMBER];
	unsigned int threshold[BITNUMBER];
	for (int i = 0; i < BITNUMBER; i++)
	{
		depth[i] = Depth;
		threshold[i] = Threshold;
	}
	SetFilter(depth, threshold);
}
IoDebounce::~IoDebounce()
{

}

IOBITS_64b IoDebounce::JitterControl(IOBITS_64b IOs)
{
	static IOBITS_64b ChangeBits;
	static IOBITS_64b CB, temp, LHB;
	static unsigned int nDepth, bi, sum;
	/* 0. find changed bits */
	ChangeBits = (IOs ^ m_OutPutValue) & (~m_Mask);
	/* 1. add into buffer for keep change status */
	m_ChangedBitBuffer.Add(&ChangeBits, 1);
	/* 2. loops depending on how many changed bits in data. */
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
		/* 2.1.find the highest 1-bit. */
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
		LHB = ChangeBits - temp;					// find lowest 1-bit
		// fast-log2(x)-search,starting from LSB.LiuBin.20181027.
		ChangeBits = temp;							// prepare for next loop if any
		/* 2.2.calculate sum with bit index */
		bi = fastlog2_64b(LHB);						// lowest 1-bit index
		for (nDepth = *(m_FilterParameters.Depths + bi), sum = 0; nDepth; nDepth--)
		{
			m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
			sum += ((CB & LHB) != 0);
		}
		/* 2.3.and toggle changed bit index */
		if (sum >= *(m_FilterParameters.Threshold + bi))
		{
			m_OutPutValue ^= LHB;
			for (nDepth = (*(m_FilterParameters.Depths + bi) - 1); nDepth; nDepth--)
			{
				m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
				CB ^= LHB;
				m_ChangedBitBuffer.ModifyBackAt(nDepth - 1, &CB);
			}
		}
	}
	/* 3.mask bit with no debounce */
	m_OutPutValue = (m_OutPutValue & (~m_Mask)) + (IOs & m_Mask);
	return m_OutPutValue;
}

void IoDebounce::SetFilter(unsigned int* depthArray, unsigned int* thresholdArray)
{
	if (NULL == depthArray || NULL == thresholdArray) { return; }
	unsigned int maxDepth = *(depthArray);
	for (int i = 0; i < BITNUMBER; i++)
	{
		if (*(depthArray) == 0)
		{
			*(depthArray) = 1;
		}
		if (*(thresholdArray) > *(depthArray))
		{
			*(thresholdArray) = *(depthArray);
		}
		m_FilterParameters.Depths[i] = *(depthArray);
		m_FilterParameters.Threshold[i] = *(thresholdArray);
		maxDepth = max(m_FilterParameters.Depths[i], maxDepth);
		depthArray++;
		thresholdArray++;
	}
	m_ChangedBitBuffer.SetSize(maxDepth, sizeof(IOBITS_64b));
	m_ChangedBitBuffer.Initialize();
	IOBITS_64b cb = 0;
	while (m_ChangedBitBuffer.GetState() != FULL) { m_ChangedBitBuffer.Add(&cb, 1); }
}