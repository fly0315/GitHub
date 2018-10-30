#include "UartFrameFilter.h"

UartFrameFilter::UartFrameFilter
(
	unsigned int nFrameLen,
	unsigned int byteHead,
	unsigned int nCheckSumIndex,
	unsigned int nCheckSumLen,
	unsigned int nCheckStartIndex,
	CheckType CheckType,
	unsigned int FrameBufferDepth,
	unsigned int byteTail,
	unsigned int byteFlag,
	unsigned int nflagIndex,
	FlagHook pfun,
	RcvMode rcvMode
)
	: m_frameLen(nFrameLen)
	, m_head(byteHead)
	, m_checkByteIndex(nCheckSumIndex)
	, m_checkLen(nCheckSumLen)
	, m_checkStartIndex(nCheckStartIndex)
	, m_checkType(CheckType)
	, m_FrameBufferDepth(roundup_power_of_two(FrameBufferDepth))
	, m_tail(byteTail)
	, m_flag(byteFlag)
	, m_flagIndex(nflagIndex)
	, m_flagFun(pfun)
	, m_analyzedFrameCnt(0)
	, m_rcvByteCnt(0)
	, m_rcvByteBuffer(nFrameLen * m_FrameBufferDepth, sizeof(unsigned char))
	, m_FrameBuffer(m_FrameBufferDepth, sizeof(unsigned char) * nFrameLen)
{
	m_rcvByteBuffer.Initialize();
	m_FrameBuffer.Initialize();
}

UartFrameFilter::~UartFrameFilter()
{
}

unsigned int UartFrameFilter::PushIn(const unsigned char* src, unsigned int Len_b)
{
	unsigned int cnt = 0;
	while (cnt != Len_b)
	{
		cnt += m_rcvByteBuffer.Add(src + cnt , Len_b - cnt);
		m_rcvByteCnt += cnt;
		if (m_rcvByteBuffer.GetState() == FULL
			|| m_rcvByteBuffer.GetState() == OVERFLOWED) // no chance into overflow state, just make me felling well.
		{
			FrameScreening();
		}
	}
	return cnt;
}

unsigned int UartFrameFilter::FrameScreening()
{
	static unsigned char pByteArray[MAX_BYTE_CNT_IN_FRAME] = {0};
	unsigned int cnt = 0;

	while (m_rcvByteBuffer.GetUsedSize() >= m_frameLen)
	{
		do
		{
			if (1 != HeadShoot()) { break; }
			if (1 != FlagMatch()) { break; }
			if (1 != TailHold()) { break; }
			if (1 != Check()) { break; }
			m_rcvByteBuffer.PickFront(pByteArray, m_frameLen);
			m_FrameBuffer.Add(pByteArray, 1);
			cnt++;
		} while (0);
	}
	m_analyzedFrameCnt += cnt;
	return cnt;
}

int UartFrameFilter::HeadShoot()
{
	unsigned int i = m_rcvByteBuffer.Find(&m_head, 1);
	if (-1 == i) { return 0; }
	m_errorByteCnt += m_rcvByteBuffer.CutFront(i);
	return 1;
}

int UartFrameFilter::TailHold()
{
	if (m_tail == 0) { return 1; }
	else if (m_rcvByteBuffer.Find(&m_tail, 1) == (m_frameLen - 1)) { return 1; }
	else { CutFrontByte(); return 0; }
}

int UartFrameFilter::FlagMatch()
{
	if (m_flag == 0) { return 1; }
	else if (m_rcvByteBuffer.Find(&m_flag,1) == m_flagIndex) 
	{ 
		if (NULL != m_flagFun)
		{
			unsigned char byte;
			m_rcvByteBuffer.ReadFrontAt(m_flagIndex, &byte);
			return m_flagFun(byte);
		}
		return 1; 
	}
	else { CutFrontByte(); return 0; }
}

int UartFrameFilter::Check()
{
	unsigned char byte = 0;
	m_rcvByteBuffer.ReadFrontAt(m_checkByteIndex, &byte);
	if (byte == m_rcvByteBuffer.ByteCheckXOR(m_checkStartIndex, m_checkLen)) { return 1; }
	else { CutFrontByte(); return 0; }
}

void UartFrameFilter::CutFrontByte()
{
	m_rcvByteBuffer.CutFront(1);
	m_errorByteCnt++;
}
// end of file