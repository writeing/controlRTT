#include <rtthread.h>
#include <board.h>
#include "app.h"
#include "bluetooth.h"
#include "blueApp.h"
rt_mq_t blue_rx_mq;
rt_err_t blue_uart_input(rt_device_t dev, rt_size_t size);
rt_device_t Bluewrite_device;
int blue_data_type = 0;
// 1  cmd   0  data
uint8_t uart_rx_buffer[512];	

rt_mq_t rx_mq;
//接收蓝牙数据，计算数据，设置当前速度

void getBlueCmdData()
{
	rt_err_t result;
	struct rx_msg msg;
	if(blue_data_type == 1)
	{
		//cmd
		//rt_kprintf("blue cmd mode;");
		result = rt_mq_recv(blue_rx_mq, &msg, sizeof(struct rx_msg),50);
		if(result == RT_EOK)
		{
			//rt_kprintf("blue had rev cmd data;%d",msg.size);
			rt_thread_delay(50);
			rt_uint32_t rx_length = 0;
			rt_tick_t starttime = rt_tick_get();
			char ch;
			while(rx_length != msg.size && rt_tick_get() - starttime < 3*1000)
			{				
				rt_device_read(msg.dev, rx_length, &(ch),1);
				rx_length ++;
				input_blueTooth_cmd(ch);
				rt_thread_delay(10);
				rt_kprintf("%c",ch);
			}
			if(rx_length != 0)
			{
				input_blueTooth_cmd(0xAA);
			}
			rx_length = 0;
		}
	}//rev  blue data      
}
rt_err_t blue_uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg bluemsg;
    bluemsg.dev = dev;
    bluemsg.size = size;
	if(blue_data_type == 1)
	{
		//cmd
		rt_mq_send(blue_rx_mq, &bluemsg, sizeof(struct rx_msg));
	}else
	{
		//data
		rt_mq_send(rx_mq, &bluemsg, sizeof(struct rx_msg));
	}    	
    return RT_EOK;
}

/********************
@brief get run step status is direct speed
@note void
@param 
@retval 
********************/
void getGatcherInfo(int *direct,int *speed)
{
	*direct = 	getBlueMacnStatus().run >> 16;
	*speed = (	getBlueMacnStatus().run) & 0x0FFFF;
}
#define BLUE_CMD_MODE       		0
#define BLUE_DATA_MODE      		1

void beginSendBlueCmd(void)
{
	blue_data_type = BLUE_CMD_MODE;
}
void beginRevBlueData(void)
{
	blue_data_type = BLUE_DATA_MODE;
}

void app_thread_entry(void *parameter)
{
	//init usart blueTooth
	int rx_length = 0;
	struct rx_msg msg;
	rt_device_t device;
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart3");// blue
    if (device != RT_NULL)
    {		
        rt_device_set_rx_indicate(device, blue_uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);  //
		rt_kprintf("usart3 init true\r\n");
    }
	else
	{
		rt_kprintf("usart3 init false\r\n");
	}
	Bluewrite_device = device;


	initBlue(Bluewrite_device);
//鍚庨潰鏀惧埌鎬荤殑鍒濆鍖栧湴鏂癸紝鍦ㄨ繖涓嚎绋嬪惎鍔ㄤ箣鍚庣殑鍦版柟	
	beginSendBlueCmd();
	initBlueSet();	  	  //
	beginRevBlueData();

	
	while(1)
	{
		//rev blue data
		result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg),10);  		
		if(result != RT_EOK)
		{
			rt_thread_delay(3);
			continue;
		}		
		//read blue data
		g_stublueOpt.read(uart_rx_buffer,&rx_length);
		//ansy blue data
		for(int i = 0 ; i < rx_length ; i ++ )
		{					
			input_blueTooth(uart_rx_buffer[i]);
			if(uart_rx_buffer[i] == 0xcc)
			{
				setMacbWorkStatus(getBlueMacnStatus());
			}
		}
		rt_thread_delay(3);		
	}
}

void rt_BlueApp_application_init()
{
	rt_thread_t tid;
	//defien mq to rev usart data
	rx_mq = rt_mq_create("mq1",200,100,RT_IPC_FLAG_FIFO);
	// define mq to rev blue data
	blue_rx_mq = rt_mq_create("blue_mql",200,100,RT_IPC_FLAG_FIFO);
	
	tid = rt_thread_create("app", app_thread_entry, RT_NULL,
                           RT_APP_THREAD_STACK_SIZE, RT_APP_THREAD_PRIORITY, 20);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
}

