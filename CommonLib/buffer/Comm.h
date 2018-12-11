#pragma once

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned short UINT16;


inline unsigned int ParityOdd_16b(UINT16 v)
{
/*

	//1
	UINT16 v = input;
	unsigned int r = 0;
	while (v)
	{
		r = !r;
		v = v & (v - 1);
	}
	//2
	v = input;
	v ^= v >> 4;
	v &= 0xf;
	r = (0x6996 >> v) & 1;
*/

	//3
	static const bool ParityTableOdd_8b[256] =
	{
	#   define P2(n) n, n^1, n^1, n
	#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
	#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
		P6(0), P6(1), P6(1), P6(0)
	};
	unsigned char * p = (unsigned char *)&v;
	return ParityTableOdd_8b[p[0] ^ p[1]];

}			

inline unsigned int ParityEven_16b(UINT16 v)
{
	static const bool ParityTableEven_8b[256] =
	{
	#   define P2(n) n^1, n, n, n^1
	#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
	#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
		P6(0), P6(1), P6(1), P6(0)
	};
	unsigned char * p = (unsigned char *)&v;
	return ParityTableEven_8b[p[0] ^ p[1]];

}
/*
static unsigned int table[256];
void InitTable()
{
    unsigned int n;
    table[0] = 0;
    for (int i = 0; i < 8; i++)
    {
        n = 1 << i;
        for (int j = 0; j < n; j++)
        {
            table[n + j] = i;
        }
    }
}
*/
#ifdef __cplusplus
}
#endif