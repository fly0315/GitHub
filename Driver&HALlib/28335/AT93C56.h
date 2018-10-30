/*
 * AT93C56.h
 *	
 *  Created on: 2018Äê2ÔÂ7ÈÕ
 *      Author: LiuBin
 */

#ifndef AT93C56_H_
#define AT93C56_H_

#include "DSP28x_Project.h"

typedef enum 
{
	DISABLE = 0,
	ENABLE = 1
}ISENABLE;
class AT93C56_SPI {
public:
	AT93C56_SPI
	(
		volatile unsigned int* pCS,
		volatile unsigned int* pSK,
		volatile unsigned int* pDI,
		volatile unsigned int* pDO
	);
    virtual ~AT93C56_SPI();
private:
	volatile unsigned int* m_pCS;
	volatile unsigned int* m_pSK;
	volatile unsigned int* m_pDI;
	volatile unsigned int* m_pDO;
public:
	void Init();
	// Erase/Write Enable
	void EWEN(void);
	// Erase/Write Disable
	void EWDS(void);
	// Erase
	void ERASE(unsigned int Addr);
	// Erase All
	void ERAL(void);
	// Write All
	void WRAL(void);
	// Write
	void WRITE(unsigned int txData, unsigned int Addr);
	// Read
    unsigned int READ(unsigned int Addr);
private:
	// Write Option Code And Address
	void WriteOPCodeAddr(unsigned int OPCode, unsigned int addr, unsigned int offset);
	// Enable/Disable CS
	inline void CSEnable(unsigned int IsEnable)	{ GpioDataRegs.GPBDAT.bit.GPIO57 = IsEnable;}
	void TCS();

	void spi_fifo_init(void);
	void spi_init(void);
	void InitGpio_Spi_A();
};

#endif /* AT93C56_H_ */
