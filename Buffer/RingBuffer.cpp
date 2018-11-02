/*
 * RingBuffer.cpp
 *
 *  Created on: 2018Äê10ÔÂ3ÈÕ
 *      Author: LiuBin
 */
#include "RingBuffer.h"


RingBuffer::RingBuffer(void)
: m_pBuffer_b(NULL)
, m_nIn_t(0)
, m_nHead_t(0)
, m_nFac_t2b(0)
, m_State(INVALID)
{
}

/*
* for example:
*   long a;     // element to save into buffer.
*   RingBuffer Stack(32, sizeof(a));
* or 
*   RingBuffer Stack(32, sizeof(long));
*/
RingBuffer::RingBuffer(unsigned int nSize, unsigned int nSizeOfElemType)
: m_pBuffer_b(NULL)
, m_nIn_t(0)
, m_nHead_t(0)
, m_State(INVALID)
{
    SetSize(nSize, nSizeOfElemType);
}

RingBuffer::~RingBuffer(void)
{
	Release();
}

void RingBuffer::SetSize(unsigned int nSize_t, unsigned int nSizeOfElemType)
{
	m_nFac_t2b = nSizeOfElemType / sizeof(unsigned char);

	m_nSize_t = nSize_t;

    if (!Is_Power_Of_2(m_nSize_t))
    {
		m_nSize_t = roundup_power_of_two(m_nSize_t);
    }

}

int RingBuffer::Initialize()
{
	Release();

    m_pBuffer_b = new unsigned char[m_nSize_t * m_nFac_t2b];

    if (!m_pBuffer_b)
    {
        return 0;
    }

	m_nIn_t = m_nHead_t = 0;

	m_State = EMPTY;

    return 1;
}

void RingBuffer::Release()
{
	if (NULL != m_pBuffer_b)
	{
		delete[] m_pBuffer_b;
		m_pBuffer_b = NULL;
		m_State = INVALID;
	}
}

unsigned int RingBuffer::ChangeState(ChangeType_T type)
{	
	unsigned int isChanged = 0;
	unsigned int nUsed = GetUsedSize();
	unsigned int nFree = GetFreeSize();

	if (CUT == type)
	{
		switch (m_State)
		{
		case EMPTY:
			break;
		case NORMAL:
			if (nUsed == 0)
			{
				m_State = EMPTY;
				isChanged = 1;
			}
			break;
		case FULL:
			if (0 == nFree) { break; }	// Cut Zero
			m_State = ((nUsed != 0) ? NORMAL : EMPTY);
			isChanged = 1;
			break;
		case OVERFLOWED:
			if (0 == nFree) { break; }	// Cut Zero
			m_State = ((nUsed != 0) ? NORMAL : EMPTY);
			isChanged = 1;
			break;
		case LOCKED:
			break;
		case INVALID:
			break;
		default:
			break;
		}
	}
	else if (ADD == type)
	{
		switch (m_State)
		{
		case EMPTY:
			if (0 == nUsed) { break; } // cut zero
			m_State = ((nFree != 0) ? NORMAL : FULL);
			isChanged = 1;
			break;
		case NORMAL:
			if (nFree == 0)
			{
				m_State = FULL;
				isChanged = 1;
			}
			break;
		case FULL:
			m_State = OVERFLOWED;
			isChanged = 1;
			break;
		case OVERFLOWED:
			break;
		case LOCKED:
			break;
		case INVALID:
			break;
		default:
			break;
		}
	}
	return isChanged;
}

unsigned int RingBuffer::PutIn_t(const void *src, unsigned int nLen_t)
{
	/* *
	*  Overwrite old data if buffer filed full
	* */
	if (FULL == m_State || OVERFLOWED == m_State)
	{
		nLen_t = min(nLen_t, m_nSize_t);
		m_nHead_t += nLen_t;
	}
	/* Prevent over range */
	nLen_t = min(nLen_t, m_nSize_t - m_nIn_t + m_nHead_t);

	/*
	* Ensure that we sample the m_nOut index -before- we
	* start putting bytes into the UnlockQueue.
	*/
	//  __sync_synchronize();

	/* first put the data starting from fifo->in to buffer end */
	unsigned int n_t = min(nLen_t, m_nSize_t - mod(m_nIn_t));
	memcpy(m_pBuffer_b + t2b(mod(m_nIn_t)), src, t2b(n_t));

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(m_pBuffer_b, ((unsigned char*)src + t2b(n_t)), t2b(nLen_t - n_t));

	/*
	* Ensure that we add the bytes to the kfifo -before-
	* we update the fifo->in index.
	*/
	//  __sync_synchronize();

	m_nIn_t += nLen_t;

	ChangeState(ADD);

	return nLen_t;
}

unsigned int RingBuffer::GetOut_t(void *dest, unsigned int nLen_t, Dir_T dir)
{
	nLen_t = min(nLen_t, m_nIn_t - m_nHead_t);	// necessary!!!
	return Delete_t(Copy_t(
		dest, (dir == FRONT ? 0 : m_nIn_t - m_nHead_t - nLen_t), nLen_t),
	dir
	);
}

unsigned int RingBuffer::Copy_t(void *dest, unsigned int nIndex_t, unsigned int nLen_t)
{
	/* Prevent over range */
	nLen_t = min(nLen_t, m_nIn_t - (m_nHead_t + nIndex_t));

	/* TODO:
	* Ensure that we sample the fifo->in index -before- we
	* start removing bytes from the kfifo.
	*/
	//  __sync_synchronize();

	/* first get the data from out-beginning until the end of the buffer */
	unsigned int n_t = min(nLen_t, m_nSize_t - mod(m_nHead_t + nIndex_t));
	memcpy(dest, m_pBuffer_b + t2b(mod(m_nHead_t + nIndex_t)), t2b(n_t));

	/* then get the rest (if any) from the buffer-beginning of the buffer */
	memcpy(((unsigned char*)dest + t2b(n_t)), m_pBuffer_b, t2b(nLen_t - n_t));

	/* TODO:
	* Ensure that we remove the bytes from the kfifo -before-
	* we update the fifo->out index.
	*/
	//  __sync_synchronize();
	return nLen_t;
}

unsigned int RingBuffer::Delete_t(unsigned int nLen_t, Dir_T dir)
{
	nLen_t = min(nLen_t, m_nIn_t - m_nHead_t);

	(dir == FRONT) ? (m_nHead_t += nLen_t) : (m_nIn_t -= nLen_t);

	ChangeState(CUT);

	return nLen_t;
}

int RingBuffer::Find(const void *src, unsigned int nLen_t)
{
	for (unsigned int i = m_nHead_t; i < m_nIn_t; i++)
	{
		if (0 == memcmp(src, (unsigned char*)(m_pBuffer_b + t2b(mod(i))), t2b(nLen_t)))
		{
			/* Get matched index in size of data type */
			return (i - m_nHead_t);
		}
	}
	/* No matched data return -1*/
	return -1;
}

unsigned int RingBuffer::ByteCheckXOR(unsigned int nIndex, unsigned int nLen)
{
	unsigned int XOR = 0;
	unsigned int end = mod(m_nHead_t + nLen);
	for (int i = mod(m_nHead_t + nIndex); i < end; i++)
	{
		XOR ^= *(m_pBuffer_b + t2b(i));
	}
	return XOR;
}
// End of file.