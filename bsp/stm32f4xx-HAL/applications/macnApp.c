#include <rtthread.h>
#include <board.h>
#include "app.h"
#include "bluetooth.h"
#include "machControl.h"
rt_mq_t mach_rx_mq;
rt_err_t macn_uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg bluemsg;
    bluemsg.dev = dev;
    bluemsg.size = size;	
	rt_mq_send(mach_rx_mq, &bluemsg, sizeof(struct rx_msg));  	
    return RT_EOK;
}
//macn send data to macb
void app_macn_thread_entry(void *parameter)
{
	int rx_length = 0;
	char *sendbuff;
	struct rx_msg msg;
	uint8_t mach_rx_buffer[512];
	initMacninfo(0x01);			// init macn send buff ,,set device ID
	rt_device_t device, write_device;			
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart2");   //µç»ú
    if (device != RT_NULL)
    {
        rt_device_set_rx_indicate(device, macn_uart_input);        
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
	write_device = device;
	while(1)
	{		
		//device Status is devcice keyswitch		
		while(deviceStatus == DEVICE_END)
		{
			//if device stop   
			//then stop mach
			setSendData(0,0);		//send device data 0 , 0 
			sendbuff = getSendData();
			if (write_device != RT_NULL)
			{
				rt_device_write(write_device, 0,sendbuff,MACN_SEND_DATA_LEN);
			}
			rt_thread_delay(50);
		}
		result = rt_mq_recv(mach_rx_mq, &msg, sizeof(struct rx_msg),50);
		if(result == RT_EOK)
		{
			rx_length = rt_device_read(write_device, 0, &(mach_rx_buffer[0]),11);
			for(int i = 0 ; i <rx_length ; i ++ )
			{
				checkMacnData(mach_rx_buffer[i]);		//ansy macn status and speed
			}
		}
		//get macn control data
		sendbuff = getSendData();	//send caiji ban  speed
		if (write_device != RT_NULL)
		{
			rt_device_write(write_device, 0,sendbuff,MACN_SEND_DATA_LEN);
		}		
		#if DEVICE_MACH_PRINT
		for(int i= 0 ; i < MACN_SEND_DATA_LEN; i ++)
		{
			rt_kprintf("%02x ", sendbuff[i]);
		}
		#endif
		//if mach had false  ,set deviceStatus is DEVICE_END
		checkMachStatus(write_device);		
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

