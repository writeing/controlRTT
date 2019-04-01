#include <rtthread.h>
#include <board.h>
#include "app.h"
#include "finsh.h"
#include <drivers/pin.h>
#include "machControl.h"
#include "bluetooth.h"
#include "debugUsart.h"
static struct rt_timer timer_ledNum;
// 串口接收数据信号量
rt_mq_t debug_rx_mq;
uint8_t deviceStatus = DEVICE_BEGIN;


void rt_app_application_init()
{
	//blue rev and init
	rt_MacnApp_application_init();
	rt_BlueApp_application_init();
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
    setNumLed(ge,3);
}
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
	int showNum = getSetShowNum(RT_NULL);
	rt_mutex_release(ledMutex);
	while(1)
	{
		// get blue data ,,in init blue data
		getBlueCmdData();
		//get switch key to control device
        int rpy = getSconKey();			
		if(rpy == MODE_KEY)
		{
			deviceStatus = DEVICE_END;
		}
		rt_thread_delay(50);
		rt_kprintf("%d\r\n",getBlueConnectStatus());
        if(getBlueConnectStatus() == 0)
        {			
			rt_kprintf("blue connect false reconncet %d\r\n",getBlueConnectStatus());
        	deviceStatus = DEVICE_END;	
			while(1)
			{
				if(getBlueConnectStatus() == 1)
				{
					deviceStatus = DEVICE_BEGIN;
					break;
				}
				rt_thread_delay(1000);				
			}
        }		
	}
}

void rt_led_num_application_init()
{	
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




