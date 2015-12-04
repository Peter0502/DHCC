/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2008-09-09                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : mobsend_bat.c                                            ***
 *** �������� : �����������ͳ���                                         ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
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
������:nTransParam
����  :��������                 
����  :
����ֵ:
      -1         ʧ��
      0          �ɹ� 
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
		printf("δָ������ƽ̨��ַ\n");
		return(-1);
	}
	if(param.port<=0){
		printf("δָ������ƽ̨�˿ں�\n");
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
		printf("�����ݿ����!\n");
		exit(1);
	}
	if(nMobMqOpen()<0){
		vtcp_log("��ʼ����Ϣ����ʧ��[%d]\n",errno);
		exit(1);
	}	
	nMobMqInit();
	/****���Ȱ�δ���͵�ȫ�����ͳ�ȥ ****/
	nMobSendAll(param.addr,param.port);	
	vtcp_log("[%s][%d] �����������!!",__FILE__,__LINE__);
	return(0);
}

