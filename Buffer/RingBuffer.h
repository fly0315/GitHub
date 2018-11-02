/*
 * RingBuffer.h
 *
 *  Created on: 2018年10月3日
 *      Author: LiuBin
 *      ------------------------------------------
 *      DSP-C2000内存形式为小端（低尾端/小尾端）模式。
 *      例如：UINT32 num = 0x12345678,
 *      按内存地址由低至高排序为  0x5678 0x1234
 *      ------------------------------------------
 */
#include <string.h>
#include <assert.h>
#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#pragma once

#ifndef NOMINMAX

    #ifndef min
        #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif

	#ifndef max
		#define max(a,b)            (((a) > (b)) ? (a) : (b))
	#endif

#endif  /* NOMINMAX */


typedef enum ST_T
{
	INVALID = 0,
	NORMAL	= 1,
	EMPTY	= 2,
	FULL	= 3,
	OVERFLOWED = 4,
	LOCKED	= 5,
};

/* More information see link: http://stereopsis.com/log2.html */
inline long long fastlog2_64b(double x)
{
	assert(x != 0);
	return (long long((unsigned long long&)x >> 52) - 1023);
}

inline long fastlog2_32b(float x)
{
	assert(x != 0);
	return (long((unsigned long&)x >> 23) - 127);
}

inline bool Is_Power_Of_2(unsigned long n) { return (n != 0 && ((n & (n - 1)) == 0)); }

inline unsigned long roundup_power_of_two(unsigned long val)
{
	if ((val & (val - 1)) == 0)
		return val;

	unsigned long maxulong = (unsigned long)((unsigned long)~0);
	unsigned long andv = ~(maxulong & (maxulong >> 1));
	while ((andv & val) == 0)
		andv = andv >> 1;

	return andv << 1;
}

class RingBuffer
{
public:
    RingBuffer();
    RingBuffer(unsigned int nSize, unsigned int nSizeOfElemType);
    virtual ~RingBuffer();

protected:
	ST_T			m_State;		// ring buffer state
	unsigned char   *m_pBuffer_b;     // the buffer holding the data
	unsigned int	m_nSize_t;		// m_nSize_t = m_nSize * m_nFac
	unsigned int	m_nIn_t;
	unsigned int	m_nHead_t;
	unsigned int    m_nFac_t2b;         // m_nFac = sizeof(element) / sizeof(char)

	typedef enum Dir_T
	{
		FRONT = 0,
		BACK = 1,
	};

	typedef enum ChangeType_T
	{
		CUT = 0,
		ADD = 1,
	};
protected:
	/**
	 * Only basic function PutIn() and Delete() cause buffer state change.
	 * Functions like GetOut() used Delete() to change buffer state.
	 */
	unsigned int ChangeState(ChangeType_T type);
	
	unsigned int PutIn_t(const void *src, unsigned int nLen_t);
	unsigned int Delete_t(unsigned int nLen_t, Dir_T dir);

	unsigned int GetOut_t(void *dest, unsigned int nLen_t, Dir_T dir);
	unsigned int Copy_t(void *dest, unsigned int nIndex_t, unsigned int nLen_t);

	inline unsigned int mod(unsigned int index_t) { return (index_t) & (m_nSize_t - 1); }	
	inline unsigned int t2b(unsigned int index_t) { return (index_t * m_nFac_t2b); }
public:
	int Initialize();
	void Release();
	inline const void* GetBuffer() { return (void*)m_pBuffer_b; }
	void SetSize(unsigned int nSize_t, unsigned int nSizeOfElemType);
	inline ST_T GetState() { return m_State; }

    inline void Clean()	
	{ 
		m_nIn_t = m_nHead_t = 0;
		m_State = EMPTY;
	}
	inline unsigned int GetSize()		const 
	{ 
		return m_nSize_t; 
	}
	inline unsigned int GetUsedSize()	const 
	{ 
		return (m_nIn_t - m_nHead_t);
	}
    inline unsigned int GetFreeSize()	const 
	{ 
		return GetSize() - GetUsedSize(); 
	}

	inline unsigned int Add(const void	*src, unsigned int nLen_t)
	{
		return PutIn_t(src, nLen_t);
	}
	
	inline unsigned int PickFront(void *dest, unsigned int nLen_t)
	{
		return GetOut_t(dest, nLen_t, FRONT);
	}
	inline unsigned int PickBack(void *dest, unsigned int nLen_t)
	{
		return GetOut_t(dest, nLen_t, BACK);
	}

	inline unsigned int CopyFront(void *dest, unsigned nLen_t)
	{
		return Copy_t(dest, 0, nLen_t);
	}
	inline unsigned int CopyBack(void *dest, unsigned nLen_t)
	{
		return Copy_t(dest, m_nIn_t - m_nHead_t - nLen_t, nLen_t);
	}
	
	inline unsigned int ReadRange(unsigned int nIndex_t, unsigned int nLen_t, void *dest)
	{
		return Copy_t(dest, nIndex_t, nLen_t);
	}
	inline unsigned int ReadFrontAt(unsigned int nIndex_t, void *dest)
	{
		return Copy_t(dest, nIndex_t, 1);
	}
	inline unsigned int ReadBackAt(unsigned int nIndex_t, void *dest)
	{
		return Copy_t(dest, m_nIn_t - m_nHead_t - (nIndex_t + 1), 1);
	}

	inline void ModifyFrontAt(unsigned int nIndex_t, void *src)
	{
		memcpy(m_pBuffer_b + t2b(nIndex_t), src, m_nFac_t2b);
	}
	inline void ModifyBackAt(unsigned int nIndex_t, void *src)
	{
		memcpy(m_pBuffer_b + t2b(mod(m_nIn_t - (nIndex_t + 1))), src, m_nFac_t2b);
	}
	
	inline unsigned int CutFront(unsigned int nLen_t)
	{
		return Delete_t(nLen_t, FRONT);
	}
	inline unsigned int CutBack(unsigned int nLen_t)
	{
		return Delete_t(nLen_t, BACK);
	}

	int Find(const void *src, unsigned int nLen_t);

	unsigned int ByteCheckXOR(unsigned int nIndex,unsigned int nLen);
};

#endif /* RINGBUFFER_H_ */
