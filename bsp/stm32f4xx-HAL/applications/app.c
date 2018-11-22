#include <rtthread.h>
#include <board.h>
#include "easyflash.h"
#include "app.h"
#include "finsh.h"
#include <drivers/pin.h>
#include "machControl.h"
#include "bluetooth.h"

static struct rt_timer timer_ledNum;

rt_mq_t rx_mq;
rt_mq_t mach_rx_mq;
rt_mq_t blue_rx_mq;
int blue_data_type = 0;
// 1  cmd   0  data
uint8_t uart_rx_buffer[512];		
struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};	
rt_device_t Bluewrite_device;

rt_err_t blue_uart_input(rt_device_t dev, rt_size_t size);
int getSetShowNum(int showNum);
void app_thread_entry(void *parameter)
{	
	//init usb stick
	
	//init usart blueTooth
	struct rx_msg msg;
	rt_device_t device;
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart3");//blue
    if (device != RT_NULL)
    {		
        rt_device_set_rx_indicate(device, blue_uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);  //
		rt_kprintf("usart3 init true\r\n");
    }
	Bluewrite_device = device;
	blue_data_type = 1;   //blue data mode
	initBlueSet();
	blue_data_type = 0;
	int rx_length = 0;
	while(1)
	{
		//get usart data for bluetooth		
		result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg),10);  
		//rt_kprintf("msg.size = %ld\r\n",msg.size);
		if(result != RT_EOK)
		{
			rt_thread_delay(3);
			continue;
		}
		//rt_kprintf("msg.size = %x\r\n",msg.size);
		rx_length = rt_device_read(Bluewrite_device, 0, &(uart_rx_buffer[0]),12);
		//rt_kprintf("rx_length = %ld\r\n",rx_length);		
//		if(rx_length <=12)
//		{
//			rt_kprintf("rev:%02x\r\n",uart_rx_buffer[0]);
//			continue;
//		}
		//rx_length ++;				
		for(int i = 0 ; i < rx_length ; i ++ )
		{					
			input_blueTooth(uart_rx_buffer[i]);
			if(uart_rx_buffer[i] == 0xcc || uart_rx_buffer[i] == 0xCC)
			{
		//		rt_kprintf("size = %d,%d\r\n",rx_length,i);	
				//rx_length = 0;
				setMacbWorkStatus(getBlueMacnStatus());
				getSetShowNum(getBlueMacnStatus()&0x0FFFF);
				rt_thread_delay(3);
				//break;
			}			
		}
		rt_thread_delay(3);		
	}
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
	initMacninfo(0x01);
	rt_device_t device, write_device;
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart2");
    if (device != RT_NULL)
    {
        rt_device_set_rx_indicate(device, macn_uart_input);        
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
	write_device = device;	
	while(1)
	{
		result = rt_mq_recv(mach_rx_mq, &msg, sizeof(struct rx_msg),50);
		if(result == RT_EOK)
		{
			rx_length = rt_device_read(write_device, 0, &(mach_rx_buffer[0]),9);
			for(int i = 0 ; i <rx_length ; i ++ )
			{
				checkMacnData(mach_rx_buffer[i]);
			}
		}
		sendbuff = getSendData();
		if (write_device != RT_NULL)
		{
			rt_device_write(write_device, 0,sendbuff,MACN_SEND_DATA_LEN);
			for(int i = 0 ; i < MACN_SEND_DATA_LEN ; i ++)
			{
				rt_kprintf(" %02x ",sendbuff[i]);
			}
		}
		while(getMacnRunStatus() != MACN_BEGIN)
		{
			sendbuff = getSendData();
			if (write_device != RT_NULL)
			{
				rt_device_write(write_device, 0,sendbuff,MACN_SEND_DATA_LEN);
				
			}
			rt_thread_delay(50);
		}	
		rt_thread_delay(50);
	}
}
void rt_app_application_init()
{
	//blue rev and init
	rt_thread_t tid;
	rx_mq = rt_mq_create("mq1",200,100,RT_IPC_FLAG_FIFO);
	blue_rx_mq = rt_mq_create("blue_mql",200,100,RT_IPC_FLAG_FIFO);
	mach_rx_mq = rt_mq_create("mach_mq",200,100,RT_IPC_FLAG_FIFO);
	tid = rt_thread_create("app", app_thread_entry, RT_NULL,
                           RT_APP_THREAD_STACK_SIZE, RT_APP_THREAD_PRIORITY, 20);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
	//drive macn 
	tid = rt_thread_create("macn", app_macn_thread_entry, RT_NULL,
                           RT_MACN_THREAD_STACK_SIZE, RT_MACN_THREAD_PRIORITY, 21);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
}
/***************************************LED NUM****************************************/
uint8_t SEG_A_List[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

void setNumLed(int num,int index)
{		
	switch(index)
	{
		case 1:
			rt_pin_write(39,0);
			rt_pin_write(40,0);
			rt_pin_write(41,1);			
			break;
		case 2:	
			rt_pin_write(39,0);
			rt_pin_write(40,1);
			rt_pin_write(41,0);						
			break;
		case 3:
			rt_pin_write(39,1);
			rt_pin_write(40,0);
			rt_pin_write(41,0);						
			break;
		default:return;
	}	
	for(int i = 0 ; i < 8 ; i ++ )
	{
		if((SEG_A_List[num] & (0x01 << i)) > 0)
		{
			HAL_GPIO_WritePin(GPIOE,(0x01 << i),GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE,(0x01 << i),GPIO_PIN_RESET);
		}
	}	
}
int getSetShowNum(int showNum)
{
	static int s_showNum = 555;
	if(showNum == RT_NULL)
	{
		return s_showNum;
	}
	s_showNum = showNum;
	return showNum;	
}
MSH_CMD_EXPORT(getSetShowNum,set led num in func);

void lightNumLed(int speed)
{
	int bai = (int)(speed /100);
	int shi = (int)((speed - bai*100)/10);
	int ge = speed % 10;
	//printf("num = %d\r\n",ge);
    setNumLed(bai,1); 
    setNumLed(shi,2);
    setNumLed(shi,3);
}

MSH_CMD_EXPORT(lightNumLed,set led light);
void initGpioForLedNum()
{
	rt_device_t device;
	//"pin"		
	device = rt_device_find("pin");
	
    if (device!= RT_NULL)
    {	   
		//scan key
		rt_pin_mode(15,PIN_MODE_INPUT);
		rt_pin_mode(16,PIN_MODE_INPUT);
		rt_pin_mode(17,PIN_MODE_INPUT);
		rt_pin_mode(18,PIN_MODE_INPUT);
		
		//numled
		rt_pin_mode(39,PIN_MODE_OUTPUT);
		rt_pin_mode(40,PIN_MODE_OUTPUT);
		rt_pin_mode(41,PIN_MODE_OUTPUT);
		
		rt_pin_mode(97,PIN_MODE_OUTPUT);
		rt_pin_mode(98,PIN_MODE_OUTPUT);
		rt_pin_mode(1,PIN_MODE_OUTPUT);
		rt_pin_mode(2,PIN_MODE_OUTPUT);
		rt_pin_mode(3,PIN_MODE_OUTPUT);
		rt_pin_mode(4,PIN_MODE_OUTPUT);
		rt_pin_mode(5,PIN_MODE_OUTPUT);
		rt_pin_mode(38,PIN_MODE_OUTPUT);
		
		//blue
		rt_pin_mode(51,PIN_MODE_OUTPUT);  //PB12
		rt_pin_mode(52,PIN_MODE_OUTPUT);  //PB13
		rt_pin_mode(53,PIN_MODE_INPUT);   //PB14
    }
}
	
int getSconKey()
{
	for(int i = 0 ; i < 4 ; i ++)
	{
		if(HAL_GPIO_ReadPin(GPIOC,KEY_1_Pin << i) == GPIO_PIN_RESET)
		{
			HAL_Delay(20);
			while(HAL_GPIO_ReadPin(GPIOC,KEY_1_Pin << i) == GPIO_PIN_RESET);
			return i;
		}
	}
	return -1;
}

//rt_err_t rt_mutex_init (rt_mutex_t mutex, const char* name, rt_uint8_t flag);
rt_mutex_t ledMutex;
static void timeout1(void* parameter)
{
	int showNum = getSetShowNum(RT_NULL);
	lightNumLed(showNum);
}
void LED_NUM_thread_entry(void *parameter)
{
	rt_err_t result;
	int showNum = getSetShowNum(RT_NULL);
	struct rx_msg msg;
	while(1)
	{
		result = rt_mutex_take(ledMutex, 10);
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
        if (result == RT_EOK)
        {
			//showNum = getSetShowNum(RT_NULL);
			int rpy = getSconKey();
			if(rpy == 0)
			{
				showNum = getSetShowNum(showNum +1);
			}
			if(rpy == 2)
			{
				//start 
				blueCmdSend(BLUE_INITFUNCODE);
				rt_thread_delay(30);
				blueCmdSend(BLUE_STARTCODE);
			}
			if(rpy == 3)
			{
				//stop
				blueCmdSend(BLUE_STOPCODE);
			}
            rt_mutex_release(ledMutex);
        }
		rt_thread_delay(30);
		//rt_kprintf("num = %d\r\n",showNum);
	}
}

void rt_led_num_application_init()
{
	//rt_err_t err = rt_mutex_init(ledMutex,"lednum",RT_IPC_FLAG_PRIO);
	ledMutex = rt_mutex_create("lednum", RT_IPC_FLAG_PRIO);
	initGpioForLedNum();
    if (ledMutex == RT_NULL)
    {
		rt_kprintf("ledMutex init false\r\n");
		return ;
    }
	rt_thread_t tid;
	tid = rt_thread_create("lednum", LED_NUM_thread_entry, RT_NULL,
                           RT_LED_NUM_THREAD_STACK_SIZE, RT_LED_NUM_THREAD_PRIORITY, 20);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
	
	
	rt_timer_init(&timer_ledNum, "timer1",  /* ?????? timer1 */
                    timeout1, /* ?????????? */
                    RT_NULL, /* ????????? */
                    10, /* ????,?OS Tick???,?10?OS Tick */
                    RT_TIMER_FLAG_PERIODIC); /* ?????? */
	
	 rt_timer_start(&timer_ledNum);
}
/***************************************LED NUM****************************************/




