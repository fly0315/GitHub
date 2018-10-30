/*
 * CSerialPortDataAnalyzer.h
 *
 *  Created on: 2018��1��10��
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
        unsigned int nFrameLen,				// ��ʼ�����õ�֡���ȣ��ڶ���֡����ʱ��Ӧ����Ϊ��С֡����
        unsigned int byteHead,				// ֡ͷ
        unsigned int nCheckSumIndex,        // У����ֽ�����
        unsigned int nCheckSumLen,          // У��ͳ���
        unsigned int nCheckSumBeginIndex=0,
        unsigned int nCheckSumFlag=0,       // 0 �ֻ����  1 ���ȡ�Ͱ�λ 
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

    unsigned int m_nFrameLen;			// ֡���������ȣ���Ҫ�����֡����
    unsigned int m_Head;
    unsigned int m_Tail;
    unsigned int m_Flag;		// ֡��+֡����λ��������[N]�֣����Flag[0]��ʼ��Ϊ0�����λΪ����λ��
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
        unsigned int nFrameLen,             // ��ʼ�����õ�֡���ȣ��ڶ���֡����ʱ��Ӧ����Ϊ��С֡����
        unsigned int byteHead,              // ֡ͷ
        unsigned int nCheckSumIndex,        // У����ֽ�����
        unsigned int nCheckSumLen,          // У��ͳ���
        unsigned int nCheckSumBeginIndex=0,
        unsigned int nCheckSumFlag=0,       // 0 �ֻ����  1 ���ȡ�Ͱ�λ
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
