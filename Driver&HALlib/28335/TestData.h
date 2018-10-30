#ifndef TEST_DATA_H_
#define TEST_DATA_H_

#pragma once

typedef struct TestDataCnt
{
    unsigned int Right;
    unsigned int Error;
    unsigned int TimeOut;
    TestDataCnt()
    {
        Right = 0;
        Error = 0;
        TimeOut = 0;
    }
};
#endif /* UNLOCKFIFOBUFFER_H_ */
