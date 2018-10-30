
/** Standard and driver types */
#include "Fr_UNIFIED_types.h"  

/** UNIFIED driver implementation */         
#include "Fr_UNIFIED.h"        
         
/** Configuration data for the FlexRay node */
#include "Fr_UNIFIED_cfg.h"

//#include "VT4_Device.h"
//#include "VT4_Examples.h"
//#include "defineAddress.h"

/*
uint16 tx_data_1[6];//FR_WP_1_5ms   
uint16 tx_data_2[6];//FR_WP_2_5ms
*/

/** Return values */
Fr_return_type           return_value;      
/** Current protocol state */      
Fr_POC_state_type        protocol_state;       
/** Current wakeup status */
Fr_wakeup_state_type     wakeup_status;     
/** Current cycle value */
//uint8    current_cycle;   
//uint16   current_macrotick;

/** Definition of the variables used for updating of the transmit MBs,IN WORD*/


/** Variable used for storing of the return values */
Fr_tx_MB_status_type tx_return_value;   
/** Variable to determine if data has been tranmsitted */
Fr_tx_status_type    tx_status;             

/** Variables used for storing data and status from the receive MBs,IN WORD */

             
/** Received data length */
uint8 rx_data_length = 0;               
/** Received frame status */
uint16 rx_status_slot = 0;              
/** Variable used for storing of the return values */
Fr_rx_MB_status_type rx_return_value;   
/** Variable to determine if a frame has been received */
Fr_rx_status_type rx_status;            


//--------------------------20120606
/*******************************************************************************/
/**
* \brief    Function for FlexRay cycle start interrupt
* \author   R62779
* \param    void
* \return   void
*/
/*
void CC_interrupt_cycle_start(void)
{
    uint8 Fr_cycle_ptr;
    // Get the global time 
    Fr_get_global_time(&current_cycle, &current_macrotick); 
  //  tx_data_1[11] = current_cycle;  // Store current cycle value 
     Fr_check_cycle_start(&Fr_cycle_ptr);
}
//--------------------------20120606
*/
/*******************************************************************************/

int  vfnFlexRay_Init(void)
{
   unsigned long i;
    if(Fr_get_POC_state() == FR_POCSTATE_NORMAL_ACTIVE) 
	   return  0;
    /* Enable the FlexRay CC and force it into FR_POCSTATE_CONFIG */
    return_value = Fr_init(&Fr_HW_cfg_00, &Fr_low_level_cfg_set_00);
    if(return_value == FR_NOT_SUCCESS) 
       // Failed(1);   /* Call debug function in case of any error */
        return 1;
    /* Initialization of the FlexRay CC with protocol configuration parameter */
    Fr_set_configuration(&Fr_HW_cfg_00, &Fr_low_level_cfg_set_00);
    
    /* Initialization of all message buffers, receive shadow buffers 
       and FIFO storages */
    
	Fr_buffers_init(&Fr_buffer_cfg_00[0], &Fr_buffer_cfg_set_00[0]);
  
    return_value = Fr_leave_configuration_mode();
    if(return_value == FR_NOT_SUCCESS)
       // Failed(2);   /* Call debug function in case of any error */
       return  2;
    /* Retrieve the wakeup state */
    wakeup_status = Fr_get_wakeup_state();
    
    /* Check whether a wakeup pattern has been received */
   
    if(wakeup_status == FR_WAKEUPSTATE_UNDEFINED)
    {   /* No wakeup pattern has been received */
        /* Initiate wakeup procedure */
        return_value = Fr_send_wakeup();
        if(return_value == FR_NOT_SUCCESS)
          //  Failed(3);   /* Call debug function in case of any error */    
        return 3;
    }
    protocol_state = Fr_get_POC_state();    /* Load current POC state */
    
    /* Wait till the FR CC is not in the FR_POCSTATE_READY */
    i=0;
    while(Fr_get_POC_state() != FR_POCSTATE_READY)  //20110411
    {
        protocol_state = Fr_get_POC_state();       
         i++;
         if(i>=32767)  
         return  5;   
    }  
    /* Initialize startup */
    return_value = Fr_start_communication();
    if(return_value == FR_NOT_SUCCESS)
      //  Failed(4);   /* Call debug function in case of any error */
      return 4;
    protocol_state = Fr_get_POC_state();    /* Load current POC state */
    

    /* Wait till the FR CC is not in the FR_POCSTATE_NORMAL_ACTIVE */
    i=0;
    while(Fr_get_POC_state() != FR_POCSTATE_NORMAL_ACTIVE) //20110411 
    {
        protocol_state = Fr_get_POC_state();
        i++;
        if(i>=32767)  
         return  5;  
    } 
    	  
    protocol_state = Fr_get_POC_state();    /* Load current POC state */

      /* The  initialization of the MB  */
//  	tx_return_value = Fr_transmit_data(TX_WP1_ID4, &tx_data_1[0], 6);
//	tx_return_value = Fr_transmit_data(TX_WP2_ID3, &tx_data_2[0], 6);
 	wakeup_status = Fr_get_wakeup_state();  /* Load current wakeup status */
    
    //-------------------20120611
     /* Enable appropriate interrupts */
	Fr_CC_reg_ptr[FrPIFR0] = FR_CYCLE_START_IRQ;//Enable 前先清中断标识位
    Fr_enable_interrupts((FrGIFER_MIE|FR_PROTOCOL_IRQ) , FR_CYCLE_START_IRQ, 0);
	//-------------------20120611
    return  0;
}
  

/******************************************************************************/



