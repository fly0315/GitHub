#include "IODebounce.h"
IoDebounce::IoDebounce(IOBITS_u64 InitIOs,
				unsigned int Depth, 
				unsigned int Threshold,
				IOBITS_u64 MASK)
	: m_InitValue(InitIOs)
	, m_OutPutValue(InitIOs)
	, m_Mask(MASK)
{
	if (0 == Depth) { Depth = 1; }
	if (Threshold > Depth) { Threshold = Depth; }
	unsigned int depth[64];
	unsigned int threshold[64];
	for (int i = 0; i < 64; i++)
	{
		depth[i] = Depth;
		threshold[i] = Threshold;
	}
	SetFilters(depth, threshold);
}
IoDebounce::~IoDebounce()
{

}

IOBITS_u64 IoDebounce::JitterControl(IOBITS_u64 IOs)
{
	static IOBITS_u64 ChangeBits;
	static IOBITS_u64 CB, temp, LHB;
	static unsigned int nDepth, nThreshold, bi, sum;
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
		LHB = ChangeBits - temp;						// find lowest 1-bit
		// fast-log2(x)-search,starting from LSB.LiuBin.20181027.
		ChangeBits = temp;							// for rest loops if any
		/* 2.calculate sum with bit index */
		bi = fastlog2_64b(LHB);		// lowest 1-bit index
		nThreshold = *(m_Filters.arrayThreshold + bi);
		for (nDepth = *(m_Filters.arrayDepths + bi), sum = 0; nDepth; nDepth--)
		{
			m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
			sum += ((CB & LHB) != 0);
		}
		// 3.and toggle changed bit index
		if (sum >= nThreshold)
		{
			m_OutPutValue ^= LHB;
			for (nDepth = *(m_Filters.arrayDepths + bi); nDepth; nDepth--)
			{
				m_ChangedBitBuffer.ReadBackAt(nDepth - 1, &CB);
				CB ^= LHB;
				m_ChangedBitBuffer.ModifyBackAt(nDepth - 1, &CB);
			}
		}
	}
	m_OutPutValue = (m_OutPutValue & (~m_Mask)) + (IOs & m_Mask);
	return m_OutPutValue;
}

void IoDebounce::SetFilters(unsigned int* depthArray, unsigned int* thresholdArray)
{
	if (NULL == depthArray || NULL == thresholdArray) { return; }
	unsigned int maxDepth = *(depthArray);
	for (int i = 0; i < 64; i++)
	{
		if (*(depthArray) == 0)
		{
			*(depthArray) = 1;
		}
		if (*(thresholdArray) > *(depthArray))
		{
			*(thresholdArray) = *(depthArray);
		}
		m_Filters.arrayDepths[i] = *(depthArray);
		m_Filters.arrayThreshold[i] = *(thresholdArray);
		maxDepth = max(m_Filters.arrayDepths[i], maxDepth);
		depthArray++;
		thresholdArray++;
	}
	m_ChangedBitBuffer.SetSize(maxDepth, sizeof(IOBITS_u64));
	m_ChangedBitBuffer.Initialize();
	IOBITS_u64 cb = 0;
	while (m_ChangedBitBuffer.GetState() != FULL) { m_ChangedBitBuffer.Add(&cb, 1); }
}