#include "find_debug.h"
/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2004-09-06                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : hvnet.c                                                  ***
 *** 程序作用 : 网络传输库函数                                           ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include"stdlib.h"
#include"string.h"
#include"stdio.h"
#include"sys/types.h"
#include"sys/select.h"
#include"sys/socket.h"
#include"sys/ioctl.h"
#include"sys/stat.h"
#include"netinet/in.h"
#include"netinet/tcp.h"
#include"arpa/inet.h"
#include"fcntl.h"
#include"unistd.h"
#include"netdb.h"
#include"sys/errno.h"
#include"errlog.h"
#include"hvnet.h"
#include"errno.h"
#include"hvini.h"
int netGetPeer(int iSocketID, char *pbyIPAddr );
static int Sock_waitfor_write(int iSocketID,int waitseconds);
static int Sock_waitfor_read(int iSocketID,int waitseconds);
#define WAITSECONDS 60
/**********************************************************************
*  int hv_initPara()                                                  *
*  功能：初始化网络参数                                               *
*  返回值：0/-1                                                       *
*                                                                     *
**********************************************************************/		
TCOMM g_COMM;
TCOMM *g_poComm;
int hv_initPara(char *pIniFile){
	char caValue[256];
	char pcFileName[128];
	int iRc;
	g_poComm=&g_COMM;
	MEMSET_DEBUG((char *)g_poComm,'\0',sizeof(TCOMM));
	MEMSET_DEBUG(caValue,'\0',sizeof(caValue));
	if(pIniFile==NULL)
		sprintf(pcFileName,"%s/etc/hvnet.ini",getenv("WORKDIR"));
	else
		sprintf(pcFileName,"%s/etc/%s",getenv("WORKDIR"),pIniFile);		
	iRc=hv_readIni(pcFileName,"Common","Me",caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not read hvnet.ini",__FILE__,__LINE__);
		return(iRc);
	}
	if(strcmp(caValue,"HV_ITS")==0){
		g_poComm->Me=HV_ITS;
	}else if(strcmp(caValue,"HV_SERVER")==0){
		g_poComm->Me=HV_SERVER;
	}else if(strcmp(caValue,"HV_CLIENT")==0){
		g_poComm->Me=HV_CLIENT;
	}else{
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Me:[%s]",__FILE__,__LINE__,caValue);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"Common","Timeout",caValue);
	if(iRc<0){
		g_poComm->Timeout=60;
	}else{
		g_poComm->Timeout=atoi(caValue);
	}
	iRc=hv_readIni(pcFileName,"Common","Maxtry",caValue);
	if(iRc<0){
		g_poComm->Maxtry=4;
	}else{
		g_poComm->Maxtry=atoi(caValue);
	}
	/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"------------%s",caValue);*/
	iRc=hv_readIni(pcFileName,"HV_SERVER","Address",caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Address",__FILE__,__LINE__);
		return(-1);
	}else{
		strncpy(g_poComm->ServAddr,caValue,sizeof(g_poComm->ServAddr)-1);
	}
	iRc=hv_readIni(pcFileName,"HV_SERVER","Port",caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Port",__FILE__,__LINE__);
		return(-1);
	}else{
		g_poComm->ServPort=atoi(caValue);
	}
	iRc=hv_readIni(pcFileName,"HV_CLIENT","Address",caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Address",__FILE__,__LINE__);
		return(-1);
	}else{
		strncpy(g_poComm->ClientAddr,caValue,sizeof(g_poComm->ClientAddr)-1);
	}
	iRc=hv_readIni(pcFileName,"HV_CLIENT","Port",caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Port",__FILE__,__LINE__);
		return(-1);
	}else{
		g_poComm->ClientPort=atoi(caValue);
	}
	return(0);
}
/**********************************************************************
*  int hv_netOpen()                                                   *
*  功能：初始化本地服务                                               *
*  返回值：0/-1                                                       *
*                                                                     *
**********************************************************************/		
int hv_netOpen(){
	int iRc;
	if(g_poComm->Me==HV_ITS){
		return(0);
	}else if(g_poComm->Me==HV_SERVER){
		/*初始化前置机服务器socket服务*/
		int iSocketID;
		struct sockaddr_in stSockAddr;
		int iLength;
		iSocketID=socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		iLength = sizeof(stSockAddr);
		stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(g_poComm->ServPort);
		iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Bind socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		g_poComm->WaitId=iSocketID;
		iRc = listen(iSocketID, 5);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Listen socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		return(0);
	}else if(
		g_poComm->Me==HV_CLIENT){
		/*初始化前置机客户端socket服务*/
		int iSocketID;
		struct sockaddr_in stSockAddr;
		int iLength;
		iSocketID=socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		iLength = sizeof(stSockAddr);
		stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(g_poComm->ClientPort);
		iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Bind socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		g_poComm->WaitId=iSocketID;
		iRc = listen(iSocketID, 5);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Listen socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		return(0);
	}
	return(0);
}
/**********************************************************************
*  int hv_connect(int orient)                                         *
*  功能：连接远程服务                                                 *
*  参数：orient-连接目的(HV_SERVER/HV_CLIENT)                         *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/			
int hv_connect(int orient){
	struct hostent *hp;
	struct sockaddr_in stSockAddr;
	struct linger TimeOut;
	unsigned long lInterAddr;
	int  iRc,iSocketID;
	char caIP[4*4];
	char *caServerName;
	int iPort;
	if(orient==HV_SERVER){
		caServerName=g_poComm->ServAddr;
		iPort=g_poComm->ServPort;
	}else if(orient==HV_CLIENT){
		caServerName=g_poComm->ClientAddr;
		iPort=g_poComm->ClientPort;
	}
	
	
	switch(orient){
	case HV_ITS:
	  vtcp_log("%s,%d,ITS can not accept connection",__FILE__,__LINE__);		
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,ITS can not accept connection",__FILE__,__LINE__);
		return(-1);
	case HV_SERVER:
	case HV_CLIENT:
		iSocketID = socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
	    vtcp_log("%s,%d, Creat socket error ",__FILE__,__LINE__);				
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		MEMSET_DEBUG((char *)&stSockAddr,'\0',sizeof(stSockAddr));
		hp = gethostbyname(caServerName);
		if (hp != NULL){
			lInterAddr = *((unsigned long *)hp->h_addr);
		}else{
			if ((lInterAddr = inet_addr(caServerName)) == -1) {
	      vtcp_log("%s,%d, Get server address error ",__FILE__,__LINE__);				
				errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Get server address error",__FILE__,__LINE__);
				close(iSocketID);
				return(-1);
			}
		}
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(iPort);
		
		/*Add by SSH,2004.7.27,长整形地址的最后4位必须为0，然而却有时不是*/
		if(sizeof(lInterAddr)>4){
			MEMCPY_DEBUG(((char *)&lInterAddr)+4,(char *)&lInterAddr,4);
			MEMSET_DEBUG((char *)&lInterAddr,'\0',4);

		}
		stSockAddr.sin_addr.s_addr = lInterAddr;
		
	    vtcp_log("%s,%d, Creat socket error caServerName=[%d]",__FILE__,__LINE__,caServerName);		
	    vtcp_log("%s,%d, Creat socket error stSockAddr.sin_port =[%d]",__FILE__,__LINE__,stSockAddr.sin_port );		
	    vtcp_log("%s,%d, Creat socket error stSockAddr.sin_addr.s_addr=[%d]",__FILE__,__LINE__,stSockAddr.sin_addr.s_addr);		
		
	  vtcp_log("%s,%d, Connect socket,num addr: %x",__FILE__,__LINE__,lInterAddr);				
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket,num addr: %x",__FILE__,__LINE__,lInterAddr);
		iRc = connect(iSocketID,(struct sockaddr*)(&stSockAddr),sizeof(stSockAddr));
		if(iRc){
	    vtcp_log("%s,%d, Connect socket error",__FILE__,__LINE__);				
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		TimeOut.l_onoff = 1;
		TimeOut.l_linger = 10;
		setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
		setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
		
		netGetPeer(iSocketID, caIP);
	  vtcp_log("%s,%d,Connect socket,remote addr: %s",__FILE__,__LINE__,caIP);				
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket,remote addr: %s",__FILE__,__LINE__,caIP);
		return(iSocketID);
	default:
	  vtcp_log("%s,%d,Invalid orient",__FILE__,__LINE__);
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Invalid orient",__FILE__,__LINE__);
		return(-1);
	}
}
/**********************************************************************
*  int hv_accept( )                                                   *
*  功能：接受远程连接                                                 *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/			    
int hv_accept(){
	int iAcceptedID;
	struct linger TimeOut;
	char   caIP[4*4];
	socklen_t iLength;
	struct sockaddr stSockAddr;
	do {
		iAcceptedID = accept(g_poComm->WaitId,(struct sockaddr*)&stSockAddr,
			(socklen_t	*)&iLength);
		if (iAcceptedID < 0) {
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Accept err,errno=%d",__FILE__,__LINE__,errno);
			return(-1);
		}else{
			if (iAcceptedID == 0) continue;
			errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"Accept,socketId=%d",iAcceptedID);
			TimeOut.l_onoff = 1;
			TimeOut.l_linger = 10;
			setsockopt(iAcceptedID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
			setsockopt(iAcceptedID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
			netGetPeer(iAcceptedID, caIP);
			errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Accept socket,remote addr: %s",__FILE__,__LINE__,caIP);
			return(iAcceptedID);
		}
	}while(1);
}
/**********************************************************************
*  void  hv_close( int iSocketID)                                     *
*  功能：关闭远程连接                                                 *
*  参数：连接id                                                       *
*  返回值：                                                           *
*                                                                     *
**********************************************************************/			
void hv_netClose(int iSocketID){
	int iRc;
	iRc=close(iSocketID);
	if(iSocketID==g_poComm->WaitId){
		g_poComm->WaitId=0;
	}
	return;
}
/********************************************************************** 
*  int Sock_send                                                      *
*  功能：发送数据                                                     *
*  参数：iSocketID-远端socket id                                      *
*        ht-发送数据的哈希表结构                                      *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int Sock_send(int iSocketID,char *buf,int length){
	int  nTotalLen;
	int  nSubLen;
	int  nShouldLen;
	int iRc;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		iRc=Sock_waitfor_write(iSocketID,WAITSECONDS);
		if(iRc<=0){
			printf("%s,%d,等待发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			printf("%s,%d,发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}
/**********************************************************************
*  int   hv_send(int iSocketID,char *caBuf)                           *
*  功能：发送数据到远端                                               *
*  参数：iSocketID-远端socket id                                      *
*        caBuf-待发送数据，以\0结束                                   *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/			
int   hv_send(int iSocketID,char *caBuf){
	char pcBuffer[9];
	int nLength=strlen(caBuf);
	int iRc;
	MEMSET_DEBUG(pcBuffer,'\0',sizeof(pcBuffer));
	sprintf(pcBuffer,"%08d",nLength);
	iRc=Sock_send(iSocketID,pcBuffer,8);
	if(iRc<0){
		return(-1);
	}
	iRc=Sock_send(iSocketID,caBuf,nLength);
	if(iRc<0){
		return(-1);
	}
	return(0);
}
/**********************************************************************
*  int Sock_recv                                                      *
*  功能：接收远端数据                                                 *
*  参数：iSocketID-远端socket id                                      *
*        ht-接收哈希表结构                                            *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int Sock_recv(int iSocketID,char *buf,int length){/***接收指定长度数据***/
	int  nTotalLen;
	int  nSubLen;
	int  nShouldLen;
	int iRc;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		iRc=Sock_waitfor_read(iSocketID,WAITSECONDS);
		if(iRc<=0){
			printf("%s,%d,等待接收数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			printf("%s,%d,接收数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}
/**********************************************************************
*  int   hv_recv(int iSocketID,char *caBuf)                           *
*  功能：接收远端数据                                                 *
*  参数：iSocketID-远端socket id                                      *
*        caBuf-接收数据缓冲区                                         *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/			 		
int   hv_recv(int iSocketID,char *caBuf){
	char pcBuffer[9];
	int iRc;
	int nLength;
	MEMSET_DEBUG(pcBuffer,'\0',sizeof(pcBuffer));
	iRc=Sock_recv(iSocketID,pcBuffer,8);
	if(iRc<0){
		return(-1);
	}	
	nLength=atoi(pcBuffer);
	iRc=Sock_recv(iSocketID,caBuf,nLength);
	if(iRc<0){
		return(-1);
	}
	return(0);
}		
/***************************************内部函数**********************/
int netGetPeer(int iSocketID, char *pbyIPAddr )
{
	struct sockaddr_in stSockAddr;
	socklen_t iLen = sizeof( stSockAddr );
	if (pbyIPAddr) {
		if (getpeername(iSocketID,(struct sockaddr *)&stSockAddr,&iLen)) {
			return(-1);
		}
		strcpy(pbyIPAddr,(char *)inet_ntoa(stSockAddr.sin_addr));
	}
	return(0);
}
static int Sock_waitfor_write(int iSocketID,int waitseconds){
	fd_set    rmask;
	struct    timeval time_out,*ptr;
	int iRc;
	FD_ZERO(&rmask);
	FD_SET((unsigned int)iSocketID,&rmask);
	if(waitseconds>=0){
		time_out.tv_sec  = waitseconds ;
		time_out.tv_usec = 0;
	}else{
		time_out.tv_sec  = 0 ;
		time_out.tv_usec = 200000;
	}
	ptr = &time_out;
	iRc=0;
	/*循环等待，直到socket空闲可写*/
	while(1){
		iRc = select(iSocketID+1,0,&rmask,0,ptr);
		if(iRc>0){
			break;
		}else if(iRc==0){
			/*超时*/
			break;
		}else{
			if(errno==EINTR)
				continue;
			else
				break;
		}
	}
	return iRc;
}

static int Sock_waitfor_read(int iSocketID,int waitseconds){
	fd_set    rmask;
	struct    timeval time_out,*ptr;
	int iRc;
	FD_ZERO(&rmask);
	FD_SET((unsigned int)iSocketID,&rmask);
	if(waitseconds>=0){
		time_out.tv_sec  = waitseconds ;
		time_out.tv_usec = 0;
	}else{
		time_out.tv_sec  = 0 ;
		time_out.tv_usec = 200000;
	}
	ptr = &time_out;
	iRc=0;
	/*循环等待，直到socket空闲可读*/
	while(1){
		iRc = select(iSocketID+1,&rmask,0,0,ptr);
		if(iRc>0){
			break;
		}else if(iRc==0){
			/*超时*/
			break;
		}else{
			if(errno==EINTR)
				continue;
			else
				break;
		}
	}
	return iRc;
}
