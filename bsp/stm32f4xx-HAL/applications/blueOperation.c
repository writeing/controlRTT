
#include "blueOperation.h"

/********************
@brief 
@note void
@param void
@retval SUCCESS/ERROR
********************/


stublueOperation g_stublueOpt = 
{
	.open = initblue,
	.write = blueSendBuffToDev,
	.read = blueRevBuffForDev,
	.ioctl = blueIoctl,
};


/********************
@brief init blue control set some data 
@note void
@param void
@retval SUCCESS/ERROR
********************/
ErrorStatus initblue()
{	

	return SUCCESS;
}


/********************
@brief send buff to device 
@note set direct and speed to device
@param direct: 0/1   speed 0~60
@retval SUCCESS/ERROR
********************/
ErrorStatus blueSendBuffToDev(char *buff,int len)
{
	blueBuffWrite(buff,len);
	return SUCCESS;
}


/********************
@brief rev buff for device
@note get device retuen data
@param 
@retval SUCCESS/ERROR
********************/
ErrorStatus blueRevBuffForDev(char *revBuff,int *revlen)
{	
	assert_param(revBuff != RT_NULL);
	blueBuffRead(revBuff,revlen);
	return SUCCESS;
}

/********************
@brief ioclt 
@note get device retuen data
@param 
@retval SUCCESS/ERROR
********************/
ErrorStatus blueIoctl(void)
{	
	return SUCCESS;
}






































