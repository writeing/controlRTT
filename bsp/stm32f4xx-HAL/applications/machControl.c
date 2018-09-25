#include "machControl.h"

stuMacnControl g_stumach;

void setFucode(char funcode)
{
	g_stumach.FuncCode = funcode;
}
void setSendData(char data1,char data2)
{
	g_stumach.sbuff[0] = data1;
	g_stumach.sbuff[1] = data2;
}
void getCrcData()
{
	unsigned char crc = 0;
	crc+=g_stumach.FuncCode;
	crc+= g_stumach.dataLen;
	crc+=g_stumach.sbuff[0];
	crc+=g_stumach.sbuff[1];
	g_stumach.crc = crc;
}
char *getSendData()
{
	g_stumach.sendbuff[0] = g_stumach.HEAD;
	g_stumach.sendbuff[1] = g_stumach.FuncCode;
	g_stumach.sendbuff[2] = g_stumach.dataLen;
	g_stumach.sendbuff[3] = g_stumach.sbuff[0];
	g_stumach.sendbuff[4] = g_stumach.sbuff[1];
	g_stumach.sendbuff[5] = g_stumach.crc;
	g_stumach.sendbuff[6] = g_stumach.title;
	return g_stumach.sendbuff;
}

