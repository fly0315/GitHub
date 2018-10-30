/*
 * CSerialPortDataAnalyzer.cpp
 *
 *  Created on: 2018年1月10日
 *      Author: LiuBin
 */

#include "CSerialPortDataAnalyzer.h"
#include <string.h>
CSerialPortDataAnalyzer::CSerialPortDataAnalyzer
(
    unsigned int nFrameLen,				// 初始化设置的帧长度，在多种帧长度时，应设置为最小帧长度
    unsigned int byteHead,				// 帧头
    unsigned int nCheckSumIndex,        // 校验和字节索引
    unsigned int nCheckSumLen,          // 校验和长度
    unsigned int nCheckSumBeginIndex,
    unsigned int nCheckSumFlag,       // 0 抑或求和  1 求和取低八位 
    unsigned int byteFlag,
    unsigned int byteTail
) : m_nFrameLen(nFrameLen),
    m_Head(byteHead),
    m_CheckSumIndex(nCheckSumIndex),
    m_CheckSumLen(nCheckSumLen),
    m_CheckSumBeginIndex(nCheckSumBeginIndex),
    m_CheckSumFlag(nCheckSumFlag),
    m_Flag(byteFlag),
    m_Tail(byteTail),
    m_CNT(0)
   
{
    // TODO Auto-generated constructor stub
    for (int i=0; i<FRAME_BUFFER_SIZE; ++i)
    {
        m_Frame[i]=0;
    }
    
}

CSerialPortDataAnalyzer::~CSerialPortDataAnalyzer() 
{
    // TODO Auto-generated destructor stub
}

int CSerialPortDataAnalyzer::Unpackage(UnlockFifoBuffer* pBuffer)
{
    static int rtnValue;
	rtnValue = 0;
	// 1.Find Head
	rtnValue = HeadShoot(pBuffer);			// not match return -1
	if (1 != rtnValue) { return rtnValue; }
	// 2.If flag Enable check flag
	if (m_Flag != 0)
	{
		rtnValue = FlagMatch(pBuffer);
		if (1 != rtnValue) { return rtnValue; }		// not match return -1
	}
	// 3.Check tail and check sum
	rtnValue = TailHold(pBuffer);
	if (1 != rtnValue) { return rtnValue; }				// not match return -1

	// 4.complete return 1
	return 1;
}

int CSerialPortDataAnalyzer::HeadShoot(UnlockFifoBuffer* pBuffer)
{
    pBuffer->Copy(m_Frame,1);

    if (m_Head == m_Frame[m_CNT])
    {
        m_CNT++;
    }
    else
    {
        pBuffer->Delete(1);
        m_CNT=0;
        return -1;
    }
		
    return 1; 
}

int CSerialPortDataAnalyzer::TailHold(UnlockFifoBuffer* pBuffer)
{
    // Copy the rest date according to the frame length.
	pBuffer->Copy(m_Frame, m_nFrameLen);

    if (m_Tail != 0)
    {
        if (m_Tail != m_Frame[m_nFrameLen-1])
        {
            pBuffer->Delete(1);
            m_CNT=0;
            return -3;
        }
    }
	// checksum
    if (0==m_CheckSumFlag) // XOR check
    {
        if (CalculateCheckSum() != m_Frame[m_CheckSumIndex])
        {
            pBuffer->Delete(1);
            m_CNT=0;
            return -4;
        }
    }
    else if (1==m_CheckSumFlag) // SUM check
    {
        if (CalculateSum() != m_Frame[m_CheckSumIndex])
        {
            pBuffer->Delete(1);
            m_CNT=0;
            return -1;
        }
    }
	
    // 
    pBuffer->Delete(m_nFrameLen);
	m_CNT=0;
	return 1;
}

int CSerialPortDataAnalyzer::FlagMatch(UnlockFifoBuffer* pBuffer)
{
    pBuffer->Copy(m_Frame,2);
	
    if (m_Flag == m_Frame[m_CNT])
    {
        m_CNT++;
    }
    else
    {
        if (m_Head == m_Flag)
        {
            pBuffer->Delete(2);
        }
        else
        {
            pBuffer->Delete(1);
        }
        m_CNT=0;
        return -2;
    }
	return 1;
}

unsigned int CSerialPortDataAnalyzer::CalculateCheckSum()
{
    unsigned int XOR=0;
    for (int i=0; i < m_CheckSumLen; i++)
    {
        XOR^=m_Frame[m_CheckSumBeginIndex+i];
    }
    return XOR;
}

unsigned int CSerialPortDataAnalyzer::CalculateSum()
{
    unsigned int SUM=0;
    for (int i=0;i<m_CheckSumLen;i++)
    {
        SUM += m_Frame[m_CheckSumBeginIndex+i];
    }
    return (SUM & 0xFF);
}

FrameAnalyzer_Auto::FrameAnalyzer_Auto
(
	unsigned int nFrameLen,				// 初始化设置的帧长度，在多种帧长度时，应设置为最小帧长度
	unsigned int byteHead,				// 帧头
	unsigned int nCheckSumIndex,        // 校验和字节索引
	unsigned int nCheckSumLen,          // 校验和长度
	unsigned int nCheckSumBeginIndex,
	unsigned int nCheckSumFlag,       // 0 抑或求和  1 求和取低八位 
	unsigned int byteFlag,
	unsigned int byteTail
) :
	CSerialPortDataAnalyzer
	(
		nFrameLen,
		byteHead,
		nCheckSumIndex,
		nCheckSumLen,
		nCheckSumBeginIndex,
		nCheckSumFlag,
		byteFlag,
		byteTail
	),
	m_RxBuffer(nFrameLen*2),
	m_nFrames_Cnt(0)

{
	// TODO Auto-generated constructor stub
	m_RxBuffer.Initialize();
	for (int i = 0; i < 32; ++i)
	{
		m_MultiFrames[i] = 0;
	}
}

FrameAnalyzer_Auto::~FrameAnalyzer_Auto()
{

}

unsigned int FrameAnalyzer_Auto::UnpackageFrames_Auto(unsigned int* RxData, unsigned int nLen)
{
	
	m_nFrames_Cnt = 0;

	// put in circle buffer
	m_RxBuffer.Put(RxData, nLen);

	// analyze frame
	while (m_RxBuffer.GetDataLen() >= GetFrameLen())
	{
		if (1 == Unpackage(&m_RxBuffer))
		{
			m_nFrames_Cnt++;
			memcpy((m_MultiFrames + (m_nFrames_Cnt * GetFrameLen())),
				GetFrameBufferAddr(),
				GetFrameLen());
		}
	}
	if (m_nFrames_Cnt==0)
	{
	    int i=0;
	    ++i;
	}
	return m_nFrames_Cnt;
}

// end of file
