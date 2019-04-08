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
		for(int i = 0 ; i < 8 ; i ++ )
	{
		if((SEG_A_List[num] & (0x01 << i)) > 0)
		{
			HAL_GPIO_WritePin(GPIOB,(0x01 << i),GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB,(0x01 << i),GPIO_PIN_RESET);
		}
	}
	switch(index)
	{
		case 4:
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
			break;
		case 3:	
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);						
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);		
			break;
		case 2:
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
		break;
		case 1:
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
			break;
	}

}
int getSetShowNum(int showNum)
{
	static int s_showNum = 1234;
	if(showNum == RT_NULL)
	{
		return s_showNum;
	}
	s_showNum = showNum;
	return showNum;	
}

void lightNumLed(int speed,int index)
{
	switch(index)
	{
		case 1:
			speed = speed %10;
			break;
		case 2:
			speed = (speed/10)%10;
			break;
		case 3:
			speed = (speed/100)%10;
			break;
		case 4:
			speed = (speed/1000)%10;
			break;		
	}	
	setNumLed(speed,index);
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
		
		//numled com define 
		 //com1-4
 		rt_pin_mode(81,PIN_MODE_OUTPUT);
		rt_pin_mode(82,PIN_MODE_OUTPUT);
		rt_pin_mode(83,PIN_MODE_OUTPUT);
		rt_pin_mode(84,PIN_MODE_OUTPUT);
		//com5~8
		rt_pin_mode(85,PIN_MODE_OUTPUT);
		rt_pin_mode(86,PIN_MODE_OUTPUT);
		rt_pin_mode(87,PIN_MODE_OUTPUT);
		rt_pin_mode(88,PIN_MODE_OUTPUT);
		//9~12
		rt_pin_mode(55,PIN_MODE_OUTPUT);
		rt_pin_mode(56,PIN_MODE_OUTPUT);
		rt_pin_mode(57,PIN_MODE_OUTPUT);
		rt_pin_mode(58,PIN_MODE_OUTPUT);
		//all switch
		rt_pin_mode(59,PIN_MODE_OUTPUT);

		//lednum data
		rt_pin_mode(35,PIN_MODE_OUTPUT);
		rt_pin_mode(36,PIN_MODE_OUTPUT);
		rt_pin_mode(37,PIN_MODE_OUTPUT);
		rt_pin_mode(59,PIN_MODE_OUTPUT);
		rt_pin_mode(90,PIN_MODE_OUTPUT);
		rt_pin_mode(91,PIN_MODE_OUTPUT);
		rt_pin_mode(92,PIN_MODE_OUTPUT);
		rt_pin_mode(93,PIN_MODE_OUTPUT);
			
			
		
		
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
			rt_thread_mdelay(20);
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
	//lightNumLed(showNum);
}

void LED_NUM_thread_entry(void *parameter)
{
	int showNum = getSetShowNum(RT_NULL);
	rt_mutex_release(ledMutex);
	static char numLedIndex = 1;
	static int count = 0;
	while(1)
	{
	  showNum = getSetShowNum(RT_NULL);		
		numLedIndex = numLedIndex % 5;
		if(numLedIndex == 0)
			numLedIndex  = 1;

		lightNumLed(showNum,numLedIndex);
		numLedIndex ++;
		rt_thread_delay(5);
	}
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
	rt_pin_write(59,1); // open all switch 
	if (ledMutex == RT_NULL)
	{
	rt_kprintf("ledMutex init false\r\n");
	return ;
	}
	rt_thread_t tid;
	tid = rt_thread_create("lednum", LED_NUM_thread_entry, RT_NULL,
                           RT_LED_NUM_THREAD_STACK_SIZE, RT_LED_NUM_THREAD_PRIORITY, 1);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
		
//	rt_timer_init(&timer_ledNum, "timer1",  /* ?????? timer1 */
//                    timeout1, /* ?????????? */
//                    RT_NULL, /* ????????? */
//                    10, /* ????,?OS Tick???,?10?OS Tick */
//                    RT_TIMER_FLAG_PERIODIC); /* ?????? */
//	
//	 rt_timer_start(&timer_ledNum);
}
/***************************************LED NUM****************************************/




