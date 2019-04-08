#include "machOperation.h"


/********************
@brief 
@note void
@param void
@retval SUCCESS/ERROR
********************/


stuMachOperation g_stuMachOpt = 
{
	.open = initMach,
	.write = machSendBuffToDev,
	.read = MachRevBuffForDev,
	.ioctl = MachIoctl,
};


/********************
@brief init mach control set some data 
@note void
@param void
@retval SUCCESS/ERROR
********************/
ErrorStatus initMach(stuMacnSendInfo *stumach)
{	
	stumach->HEAD = MACH_HEAD;
	stumach->address = MACH_ADDRESS;
	stumach->FuncCode = MACH_FUNCODE;
	stumach->dataLen = MACH_LEN;
	stumach->title = MACH_TITLE;
	return SUCCESS;
}


/********************
@brief combine send buff  for stuMacnSendInfo
@note combine some direct and speed is ok
@param stumach:had send buff some info
@retval SUCCESS/ERROR
********************/
ErrorStatus machCombineSendBuff(stuMacnSendInfo *stumach,char direct,int speed,char *sendbuff)
{
	assert_param(speed > 0 && speed < 60);
	short crc = 0x01;
	int index = 0;
	sendbuff[index ++ ] = stumach->HEAD;
	sendbuff[index ++ ] = stumach->address;
	crc += stumach->address;
	sendbuff[index ++ ] = stumach->FuncCode;	
	sendbuff[index ++ ] = stumach->dataLen;
	crc += stumach->dataLen;	
	sendbuff[index ++ ] = direct;
	crc += direct;	
	sendbuff[index ++ ] = speed;
	crc += speed;
	sendbuff[index ++ ] = crc & 0x00FF;
	sendbuff[index ++ ] = (crc & 0xFF00) >> 8;
	sendbuff[index ++ ] = stumach->title;	
	return SUCCESS;
}




/********************
@brief send buff to device 
@note set direct and speed to device
@param direct: 0/1   speed 0~60
@retval SUCCESS/ERROR
********************/
ErrorStatus machSendBuffToDev(stuMacnSendInfo *stumach,char direct,int speed)
{
	char buff[MACN_SEND_DATA_LEN] = {0};
	machCombineSendBuff(stumach,direct,speed,buff);
	machBuffWrite(buff,MACN_SEND_DATA_LEN);	
	return SUCCESS;
}


/********************
@brief rev buff for device
@note get device retuen data
@param 
@retval SUCCESS/ERROR
********************/
ErrorStatus MachRevBuffForDev(char *revBuff,int *revlen)
{	
	assert_param(revBuff != RT_NULL);
	machBuffRead(revBuff,revlen);
	return SUCCESS;
}

/********************
@brief ioclt 
@note get device retuen data
@param 
@retval SUCCESS/ERROR
********************/
ErrorStatus MachIoctl(void)
{	
	return SUCCESS;
}





















