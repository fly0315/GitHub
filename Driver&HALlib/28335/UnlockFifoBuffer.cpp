/*
 * UnlockFifoBuffer.cpp
 *
 *  Created on: 2018Äê1ÔÂ8ÈÕ
 *      Author: LiuBin
 */

#include "UnlockFifoBuffer.h"
#include <string.h>
//#include <stdio.h>

UnlockFifoBuffer::UnlockFifoBuffer(void)
: m_pBuffer(NULL)
, m_nIn(0)
, m_nOut(0)
{
}

UnlockFifoBuffer::UnlockFifoBuffer(int nSize)
: m_pBuffer(NULL)
    ,m_nSize(nSize)
    ,m_nIn(0)
    ,m_nOut(0)
{
    //round up to the next power of 2
    if (!Is_Power_Of_2(nSize))
    {
        m_nSize = roundup_power_of_two(nSize);
    }
}

UnlockFifoBuffer::~UnlockFifoBuffer(void)
{
    if(NULL != m_pBuffer)
    {
        delete[] m_pBuffer;
        m_pBuffer = NULL;
    }
}

bool UnlockFifoBuffer::Initialize()
{
    m_pBuffer = new unsigned int[m_nSize];
    if (!m_pBuffer)
    {
        return false;
    }

    m_nIn = m_nOut = 0;

    return true;
}

unsigned long UnlockFifoBuffer::roundup_power_of_two(unsigned long val)
{
    if((val & (val-1)) == 0)
        return val;

    unsigned long maxulong = (unsigned long)((unsigned long)~0);
    unsigned long andv = ~(maxulong&(maxulong>>1));
    while((andv & val) == 0)
        andv = andv>>1;

    return andv<<1;
}

unsigned int UnlockFifoBuffer::Put(const unsigned int *buffer, unsigned int len)
{
    unsigned int l;

    len = min(len, m_nSize - m_nIn + m_nOut);

    /*
    * Ensure that we sample the m_nOut index -before- we
    * start putting bytes into the UnlockQueue.
    */
//  __sync_synchronize();

    /* first put the data starting from fifo->in to buffer end */
    l = min(len, m_nSize - (m_nIn  & (m_nSize - 1)));
    memcpy(m_pBuffer + (m_nIn & (m_nSize - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(m_pBuffer, buffer + l, len - l);

    /*
    * Ensure that we add the bytes to the kfifo -before-
    * we update the fifo->in index.
    */
//  __sync_synchronize();

    m_nIn += len;

    return len;
}

unsigned int UnlockFifoBuffer::Get(unsigned int *buffer, unsigned int len)
{
    unsigned int l;

    len = min(len, m_nIn - m_nOut);

    /*
    * Ensure that we sample the fifo->in index -before- we
    * start removing bytes from the kfifo.
    */
//  __sync_synchronize();

    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, m_nSize - (m_nOut & (m_nSize - 1)));
    memcpy(buffer, m_pBuffer + (m_nOut & (m_nSize - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, m_pBuffer, len - l);

    /*
    * Ensure that we remove the bytes from the kfifo -before-
    * we update the fifo->out index.
    */
//  __sync_synchronize();

    m_nOut += len;

    return len;
}

unsigned int UnlockFifoBuffer::Copy(unsigned int *buffer, unsigned int len)
{
    unsigned int l;

    len = min(len, m_nIn - m_nOut);

    /*
    * Ensure that we sample the fifo->in index -before- we
    * start removing bytes from the kfifo.
    */
//  __sync_synchronize();

    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, m_nSize - (m_nOut & (m_nSize - 1)));
    memcpy(buffer, m_pBuffer + (m_nOut & (m_nSize - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, m_pBuffer, len - l);

    /*
    * Ensure that we remove the bytes from the kfifo -before-
    * we update the fifo->out index.
    */
//  __sync_synchronize();

    return len;
}

unsigned int UnlockFifoBuffer::Delete(unsigned int len)
{
    len = min(len, m_nIn - m_nOut);

    m_nOut += len;

    return len;
}

void UnlockFifoBuffer::SetSize(int nSize)
{
	m_nSize = nSize;
}

