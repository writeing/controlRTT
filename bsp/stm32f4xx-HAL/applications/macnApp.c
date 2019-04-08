#include <rtthread.h>
#include <board.h>
#include "app.h"
#include "bluetooth.h"
#include "machControl.h"
#include "machOperation.h"
#include "blueApp.h"

rt_mq_t mach_rx_mq;


/********************
@brief rev usart data and send it
@note void
@param void
@retval SUCCESS/ERROR
********************/
rt_err_t macn_uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg bluemsg;
    bluemsg.dev = dev;
    bluemsg.size = size;	
	rt_mq_send(mach_rx_mq, &bluemsg, sizeof(struct rx_msg));  	
    return RT_EOK;
}
//macn send data to mach
void app_macn_thread_entry(void *parameter)
{
	int rx_length = 0;
	int direct = 0,speed = 0;
	struct rx_msg msg;
	uint8_t mach_rx_buffer[125];
	rt_device_t device;			
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart2");   // mach usart    
    
    if (device != RT_NULL)
    {
        rt_device_set_rx_indicate(device, macn_uart_input);        
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    //init mach func and init stumachInfo
   	machInit(device);
	while(1)
	{	
		//get data rx msg
		result = rt_mq_recv(mach_rx_mq, &msg, sizeof(struct rx_msg),50);
		if(result == RT_EOK)
		{
			//had rev rx msg
			g_stuMachOpt.read((char *)mach_rx_buffer,&rx_length);
			for(int i = 0 ; i <rx_length ; i ++ )
			{
				if(ansyMacnData(mach_rx_buffer[i]) == ERROR)		//ansy macn status and speed
				{
					//mach is error
					//close mach 
					//send mach run staut
					//close mach send
				}
			}
		}		
		//get caiji ban data is dircet and speed;
		getGatcherInfo(&direct,&speed);
		//check mach status 
		if(checkMacnStatus() == SUCCESS)
		{
			//send data to device mach
			g_stuMachOpt.write(&g_stuMachInfo,direct,speed);
		}		
		//if mach had false  ,set deviceStatus is DEVICE_END
		rt_thread_delay(50);
	}
}
void rt_MacnApp_application_init()
{
	//blue rev and init
	rt_thread_t tid;	
	//drive macn 
	mach_rx_mq = rt_mq_create("mach_mq",200,100,RT_IPC_FLAG_FIFO);	
	tid = rt_thread_create("macn", app_macn_thread_entry, RT_NULL,
                           RT_MACN_THREAD_STACK_SIZE, RT_MACN_THREAD_PRIORITY, 21);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
}

