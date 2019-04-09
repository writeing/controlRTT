#include <rtthread.h>
#include <board.h>
#include "app.h"
#include "finsh.h"
#include <drivers/pin.h>
#include "machControl.h"
#include "bluetooth.h"
#include "debugUsart.h"
static struct rt_timer timer_ledNum;
void rt_app_application_init()
{
	//blue rev and init
	rt_MacnApp_application_init();
	rt_BlueApp_application_init();
}
/***************************************LED NUM****************************************/

uint8_t SEG_A_List[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

static char ledSpeedBuff[4] = {81,82,83,84};

static char ledxxxxBuff[4] =  {85,86,87,88};
static char ledyyyyBuff[4] =  {55,56,57,58};

/********************
@brief show speed in num led 
@note void
@param num: speed.index:show num index
@retval 
********************/

void setNumLed(int num,int index,int mode)
{	
	char *pinBuff;
	if(mode == SHOW_SPEED_MODE)
		pinBuff = ledSpeedBuff;
	if(mode == SHOW_XXXX_MODE)
		pinBuff = ledxxxxBuff;
	if(mode == SHOW_YYYY_MODE)
		pinBuff = ledyyyyBuff;		
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
	
	for(int i= 0 ; i < 4 ; i ++)
	{
		rt_pin_write(pinBuff[i],PIN_LOW);
	}
	rt_pin_write(pinBuff[index -1 ],PIN_HIGH);
	rt_thread_delay(2);	
}
/********************
@brief set/or get speed value
@note void
@param 
@retval 
********************/

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
/********************
@brief set data 4 value 
@note void
@param 
@retval 
********************/
void lightNumLed(int data,int index,int mode)
{
	switch(index)
	{
		case 1:
			data = data %10;
			break;
		case 2:
			data = (data/10)%10;
			break;
		case 3:
			data = (data/100)%10;
			break;
		case 4:
			data = (data/1000)%10;
			break;		
	}
	setNumLed(data,index,mode);
}


/********************
@brief init gpio ,all
@note void
@param 
@retval 
********************/
void initGpioForAllDevice()
{
	rt_device_t device;
	//"pin"		
	device = rt_device_find("pin");
	
    if (device!= RT_NULL)
    {	   
		//scan key
		rt_pin_mode(39,PIN_MODE_INPUT);
		rt_pin_mode(40,PIN_MODE_INPUT);
		rt_pin_mode(41,PIN_MODE_INPUT);
		
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

		//buzz
		rt_pin_mode(95,PIN_MODE_OUTPUT);
		//red rev
		rt_pin_mode(96,PIN_MODE_INPUT);  
		//urgen lock
		rt_pin_mode(5,PIN_MODE_INPUT);   	
    }
}


/********************
@brief get key value
@note void
@param 
@retval 
********************/	
int getSconKey()
{
	//foreach three key
	for(int i = 0 ; i < 3 ; i ++)
	{
		if(rt_pin_read(DEVICE_KEY + i) == PIN_HIGH)
		{
			rt_thread_mdelay(5);
			while(rt_pin_read(DEVICE_KEY + i) == PIN_HIGH);
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




/********************
@brief show speed data
@note void
@param 
@retval 
********************/	
void ledSpeedShow(void)
{
	int showNum = getSetShowNum(RT_NULL);	
	for(int i = 1 ; i <= 4 ; i ++)
	{
		lightNumLed(showNum,i,SHOW_SPEED_MODE);
	}
}
/********************
@brief show xxxx data
@note void
@param 
@retval 
********************/
void ledxxxxShow(void)
{
	int showNum = 1234;//getSetShowNum(RT_NULL);	
	for(int i = 1 ; i <= 4 ; i ++)
	{
		lightNumLed(showNum,i,SHOW_XXXX_MODE);
	}
}
/********************
@brief show yyyy data
@note void
@param 
@retval 
********************/
void ledyyyyShow(void)
{
	int showNum = 1234;//getSetShowNum(RT_NULL);	
	for(int i = 1 ; i <= 4 ; i ++)
	{
		lightNumLed(showNum,i,SHOW_YYYY_MODE);
	}
}
/********************
@brief execture key event
@note void
@param key is which key being touch
@retval 
********************/

void execKeyEvent(int key)
{
	if(key == KEY_1)
	{
		
	}
	if(key == KEY_2)
	{
		
	}
	if(key == KEY_3)
	{
		
	}
}
void LED_NUM_thread_entry(void *parameter)
{
	int rpy = 0;
	while(1)
	{
		//show led num
			//speed show
		ledSpeedShow();
			//xxxx show
		//ledxxxxShow();
			//yyyy show
		ledyyyyShow();
		//get key value
    //rpy = getSconKey();
        //execute key event
		//execKeyEvent(rpy);        
		
	}
}

void rt_led_num_application_init()
{	
	ledMutex = rt_mutex_create("lednum", RT_IPC_FLAG_PRIO);
	//init gpio for all device
	initGpioForAllDevice();
	rt_pin_write(59,PIN_HIGH); // open all switch 
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




