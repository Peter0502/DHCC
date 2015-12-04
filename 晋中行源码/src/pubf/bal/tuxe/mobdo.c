#include "bal.h"
#include "global.h"
#include"pub_tx.h"
extern struct pub_tx g_pub_tx;              /*通用记帐结构*/
#define TRACE vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
int mobdo(){
	/****Add by SSH,20080909,通知mobsend发送短消息****/
	vtcp_log("取到流水号:[%d]\n",g_pub_tx.trace_no);
	if(g_pub_tx.trace_no>0){
		char buf[128];
		/****TODO:
			首先测试每次都连接消息队列,这样队列出错时不用重新启动服务
		   如果效率受到明显影响则应在初始化时连接
		****/
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"%d|%d",g_pub_tx.tx_date,g_pub_tx.trace_no);
		if(nMobMqOpen()<0){
			vtcp_log("连接短信队列失败,但程序继续![%d]\n",errno);
			return(0);
		}
		
		vtcp_log("连接短信队列open,程序继续!\n");
		if(nMobMqPut(buf,strlen(buf),1)<0){
			vtcp_log("发送数据到短信队列失败,但程序继续![%d]\n",errno);
			return(0);
		}
		vtcp_log("发送数据到短信队列成功\n");
	}		
	return(0);
}

