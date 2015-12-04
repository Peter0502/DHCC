#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_sock.c
* 功能描述：   
*              pub_comm_GetSetting     取得系统设置
*              pub_comm_Dojy     
*
* 作    者:	jack
* 完成日期：   2003年12月16日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>

#define MSGMAXLEN 342
#define RESULTADDR 41

#ifndef DEBUG
#define DEBUG
#endif

int   msgid1,msgid2;

struct msqid_ds msg1status,msg2status;

struct mesg
{
	long mtype;
	char mtext [518];
}msg1,msg2;

int TimeOut;
char TimeFlag;

/**********************************************************************
 * 函 数 名：  pub_comm_GetSetting
 * 函数功能：  取得系统设置
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：FileName:  文件名
 *             Key:       键值
 *
 *     输  出：serv_addr: 主机信息
 *
 *     返回值： 0 成功
 *            非0 失败
 *
 * 修改历史：
 *                   
********************************************************************/

int pub_comm_GetSetting(char *FileName,char *Key,char *serv_addr)
{
	int i,ret,j;
	char Buffer[200],String[200];
	FILE *fp;

	if((fp=fopen(FileName,"r"))==NULL)
	{
		printf("Read %s File Error!!!\n",FileName);
		fclose(fp);
		return -1;
	}
    
	MEMSET_DEBUG(Buffer,0,200);

	for(;;)
	{
		for(i=0;;i++)
		{
			Buffer[i] = fgetc(fp);
			if(Buffer[i] == EOF)
			{
				printf("Read %s configer Error,i=[%d] !!\n",Key,i);
				fclose(fp);
				return -2;
			}
			if(Buffer[i] == 0x0a || Buffer[i] == 0x0d) break;
			if(Buffer[i] == ' ') continue;
		}
		Buffer[i] = '\0';

		for(i=0;;i++)
		{
			if(Buffer[i] == '=' || Buffer[i] == '\0' || Buffer[i] == ' ')
				break;
			String[i] = Buffer[i];
		}
 
		String[i] = '\0';

		if(Buffer[i] != '=' && strlen(Buffer) != 0)
		{ 
			return -3;
		}

		if(strcmp(String,Key)==0)
		{
			ret = strlen(Buffer);
			i = strlen(String)+1;
			ret = ret - i;

			for(j=0;j<ret;j++)
				String[j] = Buffer[j+i];

			String[j] = '\0';

			strcpy(serv_addr,String);

			fclose(fp);

			return 0;
		}
	}
}


/* ------- 取UNIX检验前置机服务器的地址 ------ */
int getserveraddr(struct sockaddr_in *serv_addr)
{
	int ret = 0;
	char   serv_ip[50];
	char   Port[50];
	long   port;  
    /*added by wu david*/
    char    sFileName[128];
    MEMSET_DEBUG(sFileName, '\0', sizeof(sFileName));
    strcpy(sFileName, getenv("DG_HOME"));
    strcat(sFileName, "/lib/cliip.ini");

	ret = pub_comm_GetSetting(sFileName,"Server",serv_ip);
	if(ret != 0) 
	{
		#ifdef DEBUG
			fprintf(stderr,"Read Server From cliip.ini Err!!!\n");
		#endif
		return -1;
	}
	ret = pub_comm_GetSetting(sFileName,"ClientTxPort",Port);
	if(ret != 0) 
	{
		#ifdef DEBUG
			fprintf(stderr,"Read ClientTxPort From cliip.ini Err!!!\n");
		#endif
		return -1;
	}
	port = (long)atoi(Port);

	MEMSET_DEBUG(serv_addr,0,sizeof(*serv_addr));
	serv_addr->sin_family=AF_INET;
	serv_addr->sin_addr.s_addr = inet_addr(serv_ip);
	serv_addr->sin_port = htons(port);
	return 0;
}

unsigned char GetCRC(char *data,int Len)
{
	int i;
	unsigned char temp;

	temp = 0;
	for(i=1;i<Len-3;i++)
	temp = temp + data[i];

	return temp;
}

void catch(int signo)
{
	TimeFlag = 1;
	signal(SIGALRM,catch);
}

int WriteSocket(int sockfd,char *data,int datalen)
{
	int ret=0;
	int sendlen=0;

	do
	{
		ret = write(sockfd,data+sendlen,datalen-sendlen);
		sendlen = sendlen+ret;
	}while (ret > 0 && sendlen < datalen);
	return sendlen;
}

int ReadSocket(int sockfd,char *data,int datalen)
{
	int ret=0;
	int recvlen=0;

	do
	{
		ret = read(sockfd,data+recvlen,datalen-recvlen);
		recvlen = recvlen+ret;
	}while (ret > 0 && recvlen < datalen);
	return recvlen;
}

/* ------- Connect Dos Server ------ */
int  ConnectServer(int *sockfd)
{
	int    ret = 0;
	struct sockaddr_in cli_addr,serv_addr;
 
	ret = getserveraddr(&serv_addr);
	if(ret != 0)
		return -2;

	if(( *sockfd = socket(AF_INET,SOCK_STREAM,0) ) <  0  ) 
	{
		ret = -1;
		goto ENDCONNECT;
	}

	if( connect( *sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr) ) < 0 )
	{
		ret = - 2 ;
		goto ENDCONNECT;
	}
	else
	{
		return 0;
	}   
      
ENDCONNECT:
	shutdown(*sockfd,2);
	close(*sockfd);

	return  -1;
}

int CommMain(char *data)
{
	int ret = 0 ; 
	int   sockfd;
	int MessageType; /* 校验机返回数据包类型*/
	int i;
	char buf[20];
	long MsgType;
	int pid;
	int MsgLength;
	char temp[3];
	unsigned char CRC; 

	ret = ConnectServer(&sockfd); 
	if(ret != 0)
		return -1;

	ret = WriteSocket(sockfd,data,MSGMAXLEN);
	if(ret != MSGMAXLEN)
	{
		shutdown(sockfd,2);
		close(sockfd);
		return -2;
	}

	ret = ReadSocket(sockfd,data,MSGMAXLEN); 

	shutdown(sockfd,2);
	close(sockfd);
 
	if(ret < 0 || ret != MSGMAXLEN)
		return -3;
 
	CRC = GetCRC(data,MSGMAXLEN-1);
	sprintf(temp,"%02X",CRC);
	if(data[339] != temp[0] || data[340] != temp[1])
		return -4;
	return 0;
}


/**********************************************************************
 * 函 数 名：  pub_comm_Dojy
 * 函数功能：  
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：PayAcc:  
 *             CheckNo: 
 *             Date:  
 *             Amount: 
 *             PayCode:  
 *
 *     输  出：无
 *
 *     返回值：ret：
 *
 * 修改历史：
 *                   
********************************************************************/

int pub_comm_Dojy(char PayAcc[],char CheckNo[],char Date[], char Amount[], char PayCode[])
{
	int   ret;
	int   pid;
	int   i;
	int   j;
	unsigned char plus;
	char temp[3];
	char MsgStr[10];
	char data[500];
	char RcvAcc[21];
	char CheckType[3];
	char SysTime[15];

	/*added by wu david*/
	char	sFileName[128];
	MEMSET_DEBUG(sFileName, '\0', sizeof(sFileName));
	strcpy(sFileName, getenv("DG_HOME"));
	strcat(sFileName, "/lib/cliip.ini");
	/* 
	ret = pub_comm_GetSetting("cliip.ini","TradeTimeOut",MsgStr);
	*/
	ret = pub_comm_GetSetting(sFileName, "TradeTimeOut",MsgStr);
	if(ret != 0)
	{
		ret = 156;
		return ret;
	}
	TimeOut= (int)atoi(MsgStr);

	data[0] = '2';
	data[1] = 'V';
	data[2] = 'S';
	for(i=3;i<342;i++)
		data[i] = '0';

	data[36] = '4';
	data[37] = 'B';
	data[338] = '3';

	sprintf(CheckType,"%02s","00"); 
	sprintf(RcvAcc,"%020s","000"); 

	j = 42;
	for(i=0;i<20;i++)
		data[j++] = PayAcc[i];
	for(i=0;i<2;i++)
		 data[j++] = CheckType[i];
	for(i=0;i<8;i++)
		 data[j++] = CheckNo[i];
	for(i=0;i<8;i++)
		 data[j++] = Date[i];
	for(i=0;i<20;i++)
		 data[j++] = RcvAcc[i];
	for(i=0;i<14;i++)
		 data[j++] = Amount[i];
	for(i=0;i<10;i++)
		 data[16+(j++)] = PayCode[i];

	plus = 0; 
	for(i=1;i<338;i++)
		plus = plus + data[i];

	sprintf(temp,"%02X",plus);
	data[339] = temp[0];
	data[340] = temp[1]; 
	data[341] = '\0';

	alarm(TimeOut);
	signal(SIGALRM,catch);
	ret = CommMain(data);
	alarm(0);
	if(ret == 0)
		ret = pub_base_Combine((unsigned char *)(data)+40);
	else
		ret = 156;
	return ret;
}
