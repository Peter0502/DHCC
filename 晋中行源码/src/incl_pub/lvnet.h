#ifndef LVNET_H
#define LVNET_H
#include"stdlib.h"
#include "lvpack.h"
#define LV_ITS 0
#define LV_SERVER 1
#define LV_SERVER_BATCH 1	/*批量业务端口*/
#define LV_SERVER_RT 3		/*实时业务端口*/
#define LV_CLIENT 2
#define socklen_t size_t
typedef struct _t_lv_comm{
	short Me;
	int WaitId;				/*守候的socket id*/
	int Timeout;
	int Maxtry;				/*最大重试次数*/
	char ServAddr[32];
	int  ServPort;
	char ClientAddr[32];
	int ClientPort;
}TLVCOMM;
extern TLVCOMM g_lv_COMM;
extern TLVCOMM *g_lv_poComm;
int lv_initPara(char *);
int lv_netOpen();
int lv_connect(int orient);
int lv_accept();
void lv_netClose(int iSocketID);
int   lv_send(int iSocketID,TLVPACKAGE *lv);
int   lv_recv(int iSocketID,TLVPACKAGE *lv);	
#endif

