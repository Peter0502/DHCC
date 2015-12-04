/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2008-09-09                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : mobsend_bat.c                                            ***
 *** 程序作用 : 短信批量发送程序                                         ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <varargs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sys/shm.h"
#include "sys/msg.h"
#include "sys/types.h"
int (*func)();
struct _t_global{
	char prog[128];
	char addr[64];
	int port;
}param;
/**
函数名:nTransParam
功能  :解析参数                 
参数  :
返回值:
      -1         失败
      0          成功 
**/ 
int nTransParam(char *buf[]){
	char *ptr;
	int id=0;
	memset(&param,'\0',sizeof(param));
	ptr=buf[id++];
	if(ptr!=NULL && ptr[0]!='\0'){
		strncpy(param.prog,ptr,sizeof(param.prog)-1);
	}
	while(ptr!=NULL && ptr[0]!='\0'){
		ptr=buf[id++];
		if(ptr==NULL && ptr[0]!='\0'){
			break;
		}
		if(strcmp(ptr,"-p")==0){
			ptr=buf[id++];
			if(ptr!=NULL && ptr[0]!='\0'){
				param.port=atoi(ptr);	
				printf("port=%d\n",param.port);
			}
		}
		if(strcmp(ptr,"-i")==0){
			ptr=buf[id++];
			if(ptr!=NULL && ptr[0]!='\0'){
				strncpy(param.addr,ptr,sizeof(param.addr));	
				printf("addr=%s\n",param.addr);
			}
		}
	}
	if(param.addr[0]=='\0'){
		printf("未指定短信平台地址\n");
		return(-1);
	}
	if(param.port<=0){
		printf("未指定短信平台端口号\n");
		return(-1);
	}
	return(0);
}
int main(int argc,char *argv[]){
	int iRc=0;
	char sMsgBuf[256];
	int  nMsgLength;
	func=NULL;
	putenv("LOGFILE=mob.log");
	if(nTransParam(argv)<0){
		exit(1);
	}
	if(db_open()<0){
		printf("打开数据库错误!\n");
		exit(1);
	}
	if(nMobMqOpen()<0){
		vtcp_log("初始化消息队列失败[%d]\n",errno);
		exit(1);
	}	
	nMobMqInit();
	/****首先把未发送的全部发送出去 ****/
	nMobSendAll(param.addr,param.port);	
	vtcp_log("[%s][%d] 滞留发送完毕!!",__FILE__,__LINE__);
	return(0);
}

