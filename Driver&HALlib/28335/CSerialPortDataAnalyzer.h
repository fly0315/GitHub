/*
 * CSerialPortDataAnalyzer.h
 *
 *  Created on: 2018年1月10日
 *      Author: LiuBin
 */

#ifndef CSERIALPORTDATAANALYZER_H_
#define CSERIALPORTDATAANALYZER_H_

#include "UnlockFifoBuffer.h"

#define FRAME_BUFFER_SIZE 20
class CSerialPortDataAnalyzer 
{
public:
    CSerialPortDataAnalyzer
    (
        unsigned int nFrameLen,				// 初始化设置的帧长度，在多种帧长度时，应设置为最小帧长度
        unsigned int byteHead,				// 帧头
        unsigned int nCheckSumIndex,        // 校验和字节索引
        unsigned int nCheckSumLen,          // 校验和长度
        unsigned int nCheckSumBeginIndex=0,
        unsigned int nCheckSumFlag=0,       // 0 抑或求和  1 求和取低八位 
        unsigned int byteFlag=0,
        unsigned int byteTail=0
        
    );

    virtual ~CSerialPortDataAnalyzer();

public:
	// Get a frame return 1 else return 0. 
    int Unpackage(UnlockFifoBuffer* pBuffer);
    inline unsigned int* GetFrameBufferAddr()   { return m_Frame; }
    inline const unsigned int GetFrameLen()     { return m_nFrameLen; }
private:
    unsigned int m_CNT;
    unsigned int m_Frame[FRAME_BUFFER_SIZE];

    unsigned int m_nFrameLen;			// 帧缓冲区长度，需要大于最长帧长度
    unsigned int m_Head;
    unsigned int m_Tail;
    unsigned int m_Flag;		// 帧号+帧长度位。可以有[N]种，如果Flag[0]初始化为0，则该位为数据位。
    unsigned int m_CheckSumBeginIndex;
    unsigned int m_CheckSumLen;
    unsigned int m_CheckSumIndex;
    unsigned int m_CheckSumFlag;

private:
	int HeadShoot(UnlockFifoBuffer* pBuffer);
	int TailHold(UnlockFifoBuffer* pBuffer);
	int FlagMatch(UnlockFifoBuffer* pBuffer); 

    unsigned int CalculateCheckSum();
    unsigned int CalculateSum();
};

class FrameAnalyzer_Auto: public CSerialPortDataAnalyzer

{
public:
    FrameAnalyzer_Auto
    (
        unsigned int nFrameLen,             // 初始化设置的帧长度，在多种帧长度时，应设置为最小帧长度
        unsigned int byteHead,              // 帧头
        unsigned int nCheckSumIndex,        // 校验和字节索引
        unsigned int nCheckSumLen,          // 校验和长度
        unsigned int nCheckSumBeginIndex=0,
        unsigned int nCheckSumFlag=0,       // 0 抑或求和  1 求和取低八位
        unsigned int byteFlag=0,
        unsigned int byteTail=0

    );
    virtual ~FrameAnalyzer_Auto();
public:
	// returns Number of Frame. If no frame unpackaged returns 0.
	unsigned int UnpackageFrames_Auto(unsigned int* RxData,unsigned int nLen);
	inline unsigned int* GetMultiFramesBufferAddr() {return m_MultiFrames;}
	inline unsigned int GetFramesCntInBuffer() { return m_nFrames_Cnt; }
private:
	UnlockFifoBuffer m_RxBuffer;
	unsigned int m_MultiFrames[32];
	unsigned int m_nFrames_Cnt;
private:
	
};
#endif /* CSERIALPORTDATAANALYZER_H_ */
