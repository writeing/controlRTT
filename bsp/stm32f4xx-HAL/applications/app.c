#include <rtthread.h>
#include <board.h>
#include "easyflash.h"
#include "app.h"
#include "finsh.h"
#include <drivers/pin.h>
#include "machControl.h"
#include "bluetooth.h"
rt_mq_t rx_mq;
char uart_rx_buffer[64];		
struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};	

rt_err_t uart_input(rt_device_t dev, rt_size_t size);
void app_thread_entry(void *parameter)
{	
	//init usb stick
	
	//init usart blueTooth
	struct rx_msg msg;
	rt_device_t device, write_device;
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart2");
    if (device != RT_NULL)
    {
        rt_device_set_rx_indicate(device, uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    }
	write_device = device;
	initBlueSet();
	while(1)
	{
		//get usart data for bluetooth
		result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg),50);
        if (result == -RT_ETIMEOUT)
        {
            //rt_kprintf("timeout count:%d\n", ++count);
        }
		if (result == RT_EOK)
        {
            rt_uint32_t rx_length;
			rt_tick_t starttime = rt_tick_get();
            while(rx_length != msg.size || rt_tick_get() - starttime < 3*1000)
			{				
				rt_device_read(msg.dev, rx_length, &(uart_rx_buffer[0]),1);
				rx_length ++;
				input_blueTooth(uart_rx_buffer[0]);
				//uart_rx_buffer[rx_length] = '\0';
			}
			rx_length = 0;
        }
		//set macn work
		setMacbWorkStatus(getBlueMacnStatus());
		//set work status to pc
		rt_kprintf("app %s\r\n",ef_get_env("wxc"));
		rt_thread_delay(100);		
	}
}
rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg bluemsg;
    bluemsg.dev = dev;
    bluemsg.size = size;
    rt_mq_send(rx_mq, &bluemsg, sizeof(struct rx_msg));

    return RT_EOK;
}
//macn send data to macb
void app_macn_thread_entry(void *parameter)
{
	char *sendbuff;
	initMacninfo(0x03);
	rt_device_t device, write_device;
	rt_err_t result = RT_EOK;
    device= rt_device_find("uart3");
    if (device != RT_NULL)
    {
        rt_device_set_rx_indicate(device, uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    }
	write_device = device;	
	while(getMacnRunStatus() != MACN_BEGIN)
	{
		rt_thread_delay(100);
	}
	while(1)
	{
		sendbuff = getSendData();
		if (write_device != RT_NULL)
			rt_device_write(write_device, 0,sendbuff,7);
		if(getMacnRunStatus() == MACN_END)
		{
			sendbuff = getSendData();
			if (write_device != RT_NULL)
				rt_device_write(write_device, 0,sendbuff,7);
			while(getMacnRunStatus() != MACN_BEGIN)
			{
				rt_thread_delay(100);
			}
		}
		rt_thread_delay(100);
	}
}
void rt_app_application_init()
{
	rt_thread_t tid;
	rx_mq = rt_mq_create("mq1",40,64,RT_IPC_FLAG_FIFO);
	tid = rt_thread_create("app", app_thread_entry, RT_NULL,
                           RT_APP_THREAD_STACK_SIZE, RT_APP_THREAD_PRIORITY, 20);
    RT_ASSERT(tid != RT_NULL);
	rt_thread_startup(tid);
	tid = rt_thread_create("macn", app_macn_thread_entry, RT_NULL,
                           RT_MACN_THREAD_STACK_SIZE, RT_MACN_THREAD_PRIORITY, 20);
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
			rt_pin_write(39,1);
			rt_pin_write(40,0);
			rt_pin_write(41,0);			
			break;
		case 2:	
			rt_pin_write(39,0);
			rt_pin_write(40,1);
			rt_pin_write(41,0);						
			break;
		case 3:
			rt_pin_write(39,0);
			rt_pin_write(40,0);
			rt_pin_write(41,1);						
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
	static int s_showNum = 1;
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
void LED_NUM_thread_entry(void *parameter)
{
	rt_err_t result;
	int showNum;
	while(1)
	{
		result = rt_mutex_take(ledMutex, 10);
        if (result == RT_EOK)
        {
			showNum = getSetShowNum(RT_NULL);
            rt_mutex_release(ledMutex);
        }
		//shownumIn NUMLED
		lightNumLed(showNum);
		rt_thread_delay(100);
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
}
/***************************************LED NUM****************************************/




