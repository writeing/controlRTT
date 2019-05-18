#include <rthw.h>
#include <rtthread.h>
#include "app.h"

rt_mq_t mach_rx_mq;


struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};

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


void machBuffWrite(rt_device_t dev,char *buff,int len)
{
	if (dev != RT_NULL)
	{
		rt_device_write(dev, 0,buff,len);
	}
}

void app_thread_entry(void *parameter)
{
	int len = 0;
	char buff[100];
	struct rx_msg msg;
	rt_err_t result;
	rt_device_t device;	
	device= rt_device_find("uart1");   // mach usart      
	if (device != RT_NULL)
	{
		rt_device_set_rx_indicate(device, macn_uart_input);        
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	}
	for(;;)
	{
		result = rt_mq_recv(mach_rx_mq, &msg, sizeof(struct rx_msg),10);
		if(result == RT_EOK)
		{
			for(int i = 0;  i < msg.size ; i ++ )
			{
				len = rt_device_read(device, 0, &(buff[0]),1);
				if(len != 1)
				{
					rt_kprintf("rev error\r\n");
					break;					
				}
			}
			machBuffWrite(device,buff,msg.size);
			rt_thread_delay(100);				
		}
		
	}
}

void rt_own_application_init(void)
{
		mach_rx_mq = rt_mq_create("mach_mq",200,100,RT_IPC_FLAG_FIFO);
    rt_thread_t tid;
    tid = rt_thread_create("app", app_thread_entry, RT_NULL,
                           RT_MAIN_THREAD_STACK_SIZE, RT_MAIN_THREAD_PRIORITY + 1, 20);
    RT_ASSERT(tid != RT_NULL);
		rt_thread_startup(tid);	
}

