#include "find_debug.h"
/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2004-09-06                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : hvnet.c                                                  ***
 *** �������� : ���紫��⺯��                                           ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
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
*  ���ܣ���ʼ���������                                               *
*  ����ֵ��0/-1                                                       *
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
*  ���ܣ���ʼ�����ط���                                               *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/		
int hv_netOpen(){
	int iRc;
	if(g_poComm->Me==HV_ITS){
		return(0);
	}else if(g_poComm->Me==HV_SERVER){
		/*��ʼ��ǰ�û�������socket����*/
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
		/*��ʼ��ǰ�û��ͻ���socket����*/
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
*  ���ܣ�����Զ�̷���                                                 *
*  ������orient-����Ŀ��(HV_SERVER/HV_CLIENT)                         *
*  ����ֵ��socket id/-1                                               *
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
		
		/*Add by SSH,2004.7.27,�����ε�ַ�����4λ����Ϊ0��Ȼ��ȴ��ʱ����*/
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
*  ���ܣ�����Զ������                                                 *
*  ����ֵ��socket id/-1                                               *
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
*  ���ܣ��ر�Զ������                                                 *
*  ����������id                                                       *
*  ����ֵ��                                                           *
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
*  ���ܣ���������                                                     *
*  ������iSocketID-Զ��socket id                                      *
*        ht-�������ݵĹ�ϣ��ṹ                                      *
*  ����ֵ:0/-1                                                        *
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
			printf("%s,%d,�ȴ��������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			printf("%s,%d,�������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}
/**********************************************************************
*  int   hv_send(int iSocketID,char *caBuf)                           *
*  ���ܣ��������ݵ�Զ��                                               *
*  ������iSocketID-Զ��socket id                                      *
*        caBuf-���������ݣ���\0����                                   *
*  ����ֵ:0/-1                                                        *
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
*  ���ܣ�����Զ������                                                 *
*  ������iSocketID-Զ��socket id                                      *
*        ht-���չ�ϣ��ṹ                                            *
*  ����ֵ:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int Sock_recv(int iSocketID,char *buf,int length){/***����ָ����������***/
	int  nTotalLen;
	int  nSubLen;
	int  nShouldLen;
	int iRc;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		iRc=Sock_waitfor_read(iSocketID,WAITSECONDS);
		if(iRc<=0){
			printf("%s,%d,�ȴ��������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			printf("%s,%d,�������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}
/**********************************************************************
*  int   hv_recv(int iSocketID,char *caBuf)                           *
*  ���ܣ�����Զ������                                                 *
*  ������iSocketID-Զ��socket id                                      *
*        caBuf-�������ݻ�����                                         *
*  ����ֵ:0/-1                                                        *
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
/***************************************�ڲ�����**********************/
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
	/*ѭ���ȴ���ֱ��socket���п�д*/
	while(1){
		iRc = select(iSocketID+1,0,&rmask,0,ptr);
		if(iRc>0){
			break;
		}else if(iRc==0){
			/*��ʱ*/
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
	/*ѭ���ȴ���ֱ��socket���пɶ�*/
	while(1){
		iRc = select(iSocketID+1,&rmask,0,0,ptr);
		if(iRc>0){
			break;
		}else if(iRc==0){
			/*��ʱ*/
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
