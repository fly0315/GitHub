/*
 * UnlockFifoBuffer.h
 *
 *  Created on: 2018年1月8日
 *      Author: LiuBin
 *      ------------------------------------------
 *      DSP-C2000内存形式为小端（低尾端/小尾端）模式。
 *      例如：UINT32 num = 0x12345678,
 *      按内存地址由低至高排序为  0x5678 0x1234
 *      ------------------------------------------
 */

#ifndef UNLOCKFIFOBUFFER_H_
#define UNLOCKFIFOBUFFER_H_

#pragma once

#ifndef NOMINMAX

    #ifndef max
        #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
        #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif

#endif  /* NOMINMAX */

#define BUFFER_SIZE 32

class UnlockFifoBuffer
{
public:
    UnlockFifoBuffer();
    UnlockFifoBuffer(int nSize);
    virtual ~UnlockFifoBuffer();
public:
    bool Initialize();
	void SetSize(int nSize);

    unsigned int Put(const unsigned int *pBuffer, unsigned int nLen);
    unsigned int Get(unsigned int *pBuffer, unsigned int nLen);
    unsigned int Copy(unsigned int *pBuffer, unsigned int nLen);
    unsigned int Delete(unsigned int nLen);

	unsigned int Put_32(const unsigned long *pBuffer, unsigned int nLen);
	unsigned int Get_32(unsigned long *pBuffer, unsigned int nLen);
	unsigned int Copy_32(unsigned long *pBuffer, unsigned int nLen);
	unsigned int Delete_32(unsigned int nLen);

    inline void Clean() { m_nIn = m_nOut = 0; }
    inline unsigned int GetDataLen() const { return  m_nIn - m_nOut; }
    inline unsigned int GetSize() const { return m_nSize; }
    inline unsigned int GetEmpty() const { return m_nSize - GetDataLen(); }

private:
	inline bool Is_Power_Of_2(unsigned long n) {return (n != 0 && ((n & (n - 1)) == 0));}
    unsigned long roundup_power_of_two(unsigned long val);
private:
    unsigned int    *m_pBuffer;     // the buffer holding the data
    unsigned int    m_nSize;        // the size of the allocated buffer
    unsigned int    m_nIn;          // data is added at offset. (in % size)
    unsigned int    m_nOut;         // data is extracted from off.(out % size)

};

#endif /* UNLOCKFIFOBUFFER_H_ */
