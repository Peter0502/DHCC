#ifndef HVNET_H
#define HVNET_H
#include"stdlib.h"
#define HV_ITS 0
#define HV_SERVER 1
#define HV_CLIENT 2
typedef struct _t_comm{
	short Me;
	int WaitId;				/*守候的socket id*/
	int Timeout;
	int Maxtry;				/*最大重试次数*/
	char ServAddr[32];
	int  ServPort;
	char ClientAddr[32];
	int ClientPort;
}TCOMM;
extern TCOMM g_COMM;
extern TCOMM *g_poComm;
int hv_initPara(char *pIniFile);
int hv_netOpen();
int hv_connect(int orient);
int hv_accept();
void hv_netClose(int iSocketID);
int   hv_send(int iSocketID,char *caBuf);
int   hv_recv(int iSocketID,char *caBuf);	
#endif

