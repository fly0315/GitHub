/** Standard and driver types */
#include "Fr_UNIFIED_types.h"  

/** UNIFIED driver implementation */         
#include "Fr_UNIFIED.h"
         
/** Configuration data for the FlexRay node */
#include "Fr_UNIFIED_cfg.h"
#include "FlexRayInterface.h"
extern Fr_tx_status_type Fr_check_tx_status(uint8 Fr_buffer_idx);
extern Fr_tx_MB_status_type Fr_transmit_data(uint16 Fr_buffer_idx, const uint16 *Fr_data_ptr, uint8 Fr_data_length);
extern "C"
Fr_POC_state_type Fr_get_POC_state();
FlexRayTxData FrTxData_GG_Slot02(0, 10);
FlexRayTxData FrTxData_GG_Slot12(2, 10);

FlexRayTxData FrTxData_CZT_Slot31(0, 10);
FlexRayTxData::FlexRayTxData(uint8 Fr_idx,uint8 len)
:m_nTxCnt(0),m_Fr_ID(Fr_idx),m_u8Len(len)

{
	for (int i = 0; i < 10; i++)
		{m_u16TxDataArry[i] = 0;}
}

void FlexRayTxData :: txData()
{

	int tx_status1;
	int tx_return_value1;

	m_u16TxDataArry[0] = (m_nTxCnt << 8);

	tx_status1 = Fr_check_tx_status(m_Fr_ID); //ÏÈ²éÑ¯·¢ËÍ×´Ì¬                  
	//	if(tx_status1 == FR_TRANSMITTED)
	{
		tx_return_value1 = Fr_transmit_data(m_Fr_ID, m_u16TxDataArry, m_u8Len);
	}
	m_nTxCnt++; 
	m_nTxCnt &= 0xFF;
}
