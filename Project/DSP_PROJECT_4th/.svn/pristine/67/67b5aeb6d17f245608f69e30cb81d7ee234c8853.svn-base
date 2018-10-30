#ifndef _FLEXRAY_INTERFACE
#define _FLEXRAY_INTERFACE

#include "Fr_UNIFIED_types.h"

/*-----------发送数据类-----------------------------------*/
class FlexRayTxData
{
  public:
    FlexRayTxData(uint8 Fr_idx,uint8 len);
    void txData(void);//单包节点
	uint16 m_u16TxDataArry[10];
	unsigned int m_nTxCnt;

  protected:

  private:
    uint8 m_Fr_ID;
    uint8 m_u8Len;
};


#endif

