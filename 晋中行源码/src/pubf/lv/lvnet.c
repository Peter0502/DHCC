/***************************************************************************
 *** �������� : Shi shenghui ʯ����				      ***
 *** ��    �� : 2004-09-06					       ***
 *** ����ģ�� :							  ***
 *** �������� : lvnet.c						  ***
 *** �������� : ���紫��⺯��					   ***
 ***								     ***
 *** ʹ��ע�� :							  ***
 *** ����     ��1 .2005-11-24���޸�Ϊ������С�� Τ��			 ***
 ***	      :	2 .2005-12--8  �޸�lv_netOpen(),ǿ������time_wait�˿ڡ�  ***
 				Τ��	 				 ***
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
#include"lvnet.h"
#include"errno.h"
#include"hvini.h"
#include"dmem.h"
#include"lvpack.h"
int netGetPeer(int iSocketID, char *pbyIPAddr );
#define MTU_LEN 512
#define WAITSECONDS 60
static int Sock_waitfor_read(int iSocketID,int waitseconds);
static int Sock_waitfor_write(int iSocketID,int waitseconds);
/**********************************************************************
*  int lv_initPara()						  *
*  ���ܣ���ʼ���������					       *
*  ����ֵ��0/-1						       *
*								     *
**********************************************************************/	 
TLVCOMM g_lv_COMM;
TLVCOMM *g_lv_poComm;
int lv_initPara(char *pIniFile){
	char caValue[256];
	char pcFileName[128];
	int iRc=0;
	g_lv_poComm=&g_lv_COMM;
	memset((char *)g_lv_poComm,'\0',sizeof(TLVCOMM));
	memset(caValue,'\0',sizeof(caValue));
	if(pIniFile==NULL)
		sprintf(pcFileName,"%s/etc/lvnet.ini",getenv("WORKDIR"));
	else
		sprintf(pcFileName,"%s/etc/%s",getenv("WORKDIR"),pIniFile);

	iRc=hv_readIni(pcFileName,"Common","Me",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){
		vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not read lvnet.ini",__FILE__,__LINE__);
		return(iRc);
	}
	if(strcmp(caValue,"LV_ITS")==0){
		g_lv_poComm->Me=LV_ITS;
	}else if(strcmp(caValue,"LV_SERVER")==0){
		g_lv_poComm->Me=LV_SERVER;
	}else if(strcmp(caValue,"LV_CLIENT")==0){
		g_lv_poComm->Me=LV_CLIENT;
	}else{
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Me:[%s]",__FILE__,__LINE__,caValue);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"Common","Timeout",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){
		g_lv_poComm->Timeout=60;
	}else{
		g_lv_poComm->Timeout=atoi(caValue);
	}
	iRc=hv_readIni(pcFileName,"Common","Maxtry",caValue);
	if(iRc<0){
		g_lv_poComm->Maxtry=4;
	}else{
		g_lv_poComm->Maxtry=atoi(caValue);
	}
	/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"------------%s",caValue);*/
	iRc=hv_readIni(pcFileName,"LV_SERVER","Address",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){				
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Address",__FILE__,__LINE__);
		return(-1);
	}else{
		strncpy(g_lv_poComm->ServAddr,caValue,sizeof(g_lv_poComm->ServAddr)-1);
	}
	iRc=hv_readIni(pcFileName,"LV_SERVER","Port",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Port",__FILE__,__LINE__);
		return(-1);
	}else{
		g_lv_poComm->ServPort=atoi(caValue);
	}
	iRc=hv_readIni(pcFileName,"LV_CLIENT","Address",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Address",__FILE__,__LINE__);
		return(-1);
	}else{
		strncpy(g_lv_poComm->ClientAddr,caValue,sizeof(g_lv_poComm->ClientAddr)-1);
	}
	iRc=hv_readIni(pcFileName,"LV_CLIENT","Port",caValue);
	vtcp_log("[%s][%d]caValue==[%s]\n",__FILE__,__LINE__,caValue);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,InValid value of Port",__FILE__,__LINE__);
		return(-1);
	}else{
		g_lv_poComm->ClientPort=atoi(caValue);
	}
	vtcp_log("[%s][%d]��ȡ�����ļ��ɹ�!\n",__FILE__,__LINE__);
	return(0);
}
/**********************************************************************
*  int lv_netOpen()						   *
*  ���ܣ���ʼ�����ط���					       *
*  ����ֵ��0/-1						       *
*								     *
**********************************************************************/	 
int lv_netOpen(){
	int iRc=0;
	
	/*ʹ�� SO_REUSEADDR �׽���ѡ������ַʹ�ô��� add by weizhao 2005-12-08*/
	int on=1;
	/*add end*/
	
	if(g_lv_poComm->Me==LV_ITS){
		return(0);
	}else if(g_lv_poComm->Me==LV_SERVER){
		/*��ʼ��ǰ�û�������socket����*/
		int iSocketID;
		struct sockaddr_in stSockAddr;
		int iLength;
		iSocketID=socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		/*ʹ�� SO_REUSEADDR �׽���ѡ������ַʹ�ô��� add by weizhao 2005-12-08*/
		iRc = setsockopt( iSocketID, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
		/*add end*/
		
		iLength = sizeof(stSockAddr);
		stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(g_lv_poComm->ServPort);
		iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Bind socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		g_lv_poComm->WaitId=iSocketID;
		iRc = listen(iSocketID, 5);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Listen socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		return(0);
	}else if(
		g_lv_poComm->Me==LV_CLIENT){
		/*��ʼ��ǰ�û��ͻ���socket����*/
		int iSocketID;
		struct sockaddr_in stSockAddr;
		int iLength;
		iSocketID=socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		/*ʹ�� SO_REUSEADDR �׽���ѡ������ַʹ�ô��� add by weizhao 2005-12-08*/
		iRc = setsockopt( iSocketID, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
		/*add end*/
		iLength = sizeof(stSockAddr);
		stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(g_lv_poComm->ClientPort);
		iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
		if(iRc<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Bind socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		g_lv_poComm->WaitId=iSocketID;
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
*  int lv_connect(int orient)					 *
*  ���ܣ�����Զ�̷���						 *
*  ������orient-����Ŀ��(LV_SERVER/LV_CLIENT)			 *
*  ����ֵ��socket id/-1					       *
*								     *
**********************************************************************/		 
int lv_connect(int orient){
	struct hostent *hp;
	struct sockaddr_in stSockAddr;
	struct linger TimeOut;
	unsigned long lInterAddr;
	int  iRc=0,iSocketID=0;
	char caIP[4*4];
	char *caServerName;
	int iPort=0;
	if(orient==LV_SERVER){
		caServerName=g_lv_poComm->ServAddr;
		iPort=g_lv_poComm->ServPort;
	}else if(orient==LV_SERVER_RT){
								caServerName=g_lv_poComm->ServAddr;
								iPort=g_lv_poComm->ServPort+1;
				}else if(orient==LV_CLIENT){
		caServerName=g_lv_poComm->ClientAddr;
		iPort=g_lv_poComm->ClientPort;
	}
	
	
	switch(orient){
	case LV_ITS:
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,ITS can not accept connection",__FILE__,__LINE__);
		return(-1);
	case LV_SERVER:
	case LV_SERVER_RT:
	case LV_CLIENT:
		iSocketID = socket(AF_INET, SOCK_STREAM, 0);
		if(iSocketID<0){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Creat socket error",__FILE__,__LINE__);
			return(-1);
		}
		memset((char *)&stSockAddr,'\0',sizeof(stSockAddr));
		hp = gethostbyname(caServerName);
		if (hp != NULL){
			lInterAddr = *((unsigned long *)hp->h_addr);
		}else{
			if ((lInterAddr = inet_addr(caServerName)) == -1) {
				errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Get server address error",__FILE__,__LINE__);
				close(iSocketID);
				return(-1);
			}
		}
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(iPort);
		/*Add by SSH,2004.7.27,�����ε�ַ�����4λ����Ϊ0��Ȼ��ȴ��ʱ����*/
		if(sizeof(lInterAddr)>4){
			memcpy(((char *)&lInterAddr)+4,(char *)&lInterAddr,4);
			memset((char *)&lInterAddr,'\0',4);

		}
		stSockAddr.sin_addr.s_addr = lInterAddr;
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket,num addr: %x,port=%d",__FILE__,__LINE__,lInterAddr,iPort);
		iRc = connect(iSocketID,(struct sockaddr*)(&stSockAddr),sizeof(stSockAddr));
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket error",__FILE__,__LINE__);
			close(iSocketID);
			return(-1);
		}
		TimeOut.l_onoff = 1;
		TimeOut.l_linger = 10;
		setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
		setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
		
		netGetPeer(iSocketID, caIP);
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Connect socket,remote addr: %s",__FILE__,__LINE__,caIP);
		return(iSocketID);
	default:
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Invalid orient",__FILE__,__LINE__);
		return(-1);
	}
}
/**********************************************************************
*  int lv_accept( )						   *
*  ���ܣ�����Զ������						 *
*  ����ֵ��socket id/-1					       *
*								     *
**********************************************************************/		     
int lv_accept(){
	int iAcceptedID=0;
	struct linger TimeOut;
	char   caIP[4*4];
	socklen_t iLength;
	struct sockaddr stSockAddr;
	do {
		iAcceptedID = accept(g_lv_poComm->WaitId,(struct sockaddr*)&stSockAddr,
			(socklen_t      *)&iLength);
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
*  void  lv_close( int iSocketID)				     *
*  ���ܣ��ر�Զ������						 *
*  ����������id						       *
*  ����ֵ��							   *
*								     *
**********************************************************************/		 
void lv_netClose(int iSocketID){
	int iRc=0;
	iRc=close(iSocketID);
	if(iSocketID==g_lv_poComm->WaitId){
		g_lv_poComm->WaitId=0;
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
	int  nTotalLen=0;
	int  nSubLen=0;
	int  nShouldLen=0;
	int iRc=0;
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
*  int   lv_send(int iSocketID,TLVPACKAGE *lv)			   *
*  ���ܣ��������ݵ�Զ��					       *
*  ������iSocketID-Զ��socket id				      *
*	lv-���������ݣ���\0����				   *
*  ����ֵ:0/-1							*
*								     *
**********************************************************************/			
int   lv_send(int iSocketID,TLVPACKAGE *lv){
	char caSendBuf[MTU_LEN+1];    /*1k���ݣ�8�ֽ�ͷ��\0*/
	char pcHead[9];
	int  nTotalLen=0;
	int iRc=0;
	int nPackId=0;
	int nPackLen=0;

	memset(pcHead,'\0',sizeof(pcHead));
	memset(caSendBuf,'\0',sizeof(caSendBuf));
	nTotalLen = lv->dm.length;
	sprintf(pcHead,"%08d",nTotalLen);
	caSendBuf[MTU_LEN]='\0';
	iRc=Sock_send(iSocketID,pcHead,8);
	if(iRc<0){
		return(-1);
	}
	while(1){
		if((nPackId+1)*MTU_LEN>nTotalLen){
			/****ʣ�಻������****/
		/*** rem by LiuHuafeng 20060924 
			nPackLen=(nPackId+1)*MTU_LEN-nTotalLen;
		***/
			nPackLen=nTotalLen-(nPackId)*MTU_LEN;
		}else{
			nPackLen=MTU_LEN;
		}
		dm_dmemcpy2(caSendBuf,&(lv->dm),nPackId*MTU_LEN+1,nPackLen);
		caSendBuf[nPackLen+1]='\0';
		iRc=Sock_send(iSocketID,caSendBuf,nPackLen);
		if(iRc<0){
			return(-1);
		}	
		nPackId++;
		if(nPackId*MTU_LEN>=nTotalLen){
			/****�������****/
			break;
		}
	}/*end while*/
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
	int  nTotalLen=0;
	int  nSubLen=0;
	int  nShouldLen=0;
	int iRc=0;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		iRc=Sock_waitfor_read(iSocketID,WAITSECONDS);
		if(iRc<=0){
			vtcp_log("%s,%d,�ȴ��������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			vtcp_log("[%s][%d]recv(%d,buf+%d,%d,0)\n",__FILE__,__LINE__,iSocketID,nTotalLen,nShouldLen-nTotalLen);
			vtcp_log("%s,%d,�������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}
/**********************************************************************
*  int   lv_recv(int iSocketID,TLVPACKAGE *lv)			   *
*  ���ܣ�����Զ������						 *
*  ������iSocketID-Զ��socket id				      *
*	caBuf-�������ݻ�����					 *
*  ����ֵ:0/-1							*
*								     *
**********************************************************************/			 		
int   lv_recv(int iSocketID,TLVPACKAGE *lv){
	char caRecvBuf[MTU_LEN+1];    /*1k���ݣ�8�ֽ�ͷ��\0*/
	char pcHead[9];
	int  nTotalLen;
	int iRc=0;
	int nPackId=0;
	int nPackLen=0;

	memset(pcHead,'\0',sizeof(pcHead));
	memset(caRecvBuf,'\0',sizeof(caRecvBuf));
	iRc=Sock_recv(iSocketID,pcHead,8);
	if(iRc<0){
		return(-1);
	}
	nTotalLen = atoi(pcHead);
	sprintf(pcHead,"%08d",nTotalLen);
	while(1){
		if((nPackId+1)*MTU_LEN>nTotalLen){
			/****ʣ�಻������****/
			/***
			nPackLen=(nPackId+1)*MTU_LEN-nTotalLen;
			****/
			nPackLen=nTotalLen-(nPackId)*MTU_LEN;
		}else{
			nPackLen=MTU_LEN;
		}
		iRc=Sock_recv(iSocketID,caRecvBuf,nPackLen);
		if(iRc<0){
			return(-1);
		}	
		dm_dmemcpy(&(lv->dm),nPackId*MTU_LEN+1,caRecvBuf,nPackLen);
		nPackId++;
		if(nPackId*MTU_LEN>=nTotalLen){
			/****�������****/
			break;
		}
	}/*end while*/
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
	int iRc=0;
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
	int iRc=0;
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
