#include "find_debug.h"
/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2004-09-23                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : tpnet.c                                                  ***
 *** 程序作用 : 网络传输库函数                                           ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include"stdlib.h"
#include"string.h"
#include"stdio.h"
#include"errno.h"
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
#ifdef SCO
#define socklen_t size_t
#endif
static int netGetPeer(int iSocketID, char *pbyIPAddr );
static int Sock_waitfor_read(int iSocketID,int waitseconds);
static int Sock_waitfor_write(int iSocketID,int waitseconds);
#include"errlog.h"
#define WAITSECONDS 120
/**********************************************************************
*  int Sock_bind(char *addr,int port);                                  *
*  功能：初始化本地服务                                               *
*  参数：                                                             *
*      addr-本地地址                                                  *
*      port-本地端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/         
int Sock_bind(char *addr,int port){
        
	/*初始化linehandler的socket服务*/
	int iSocketID;
	struct sockaddr_in stSockAddr;
	struct linger TimeOut;
	int iLength;
	int iRc;
	iSocketID=socket(AF_INET, SOCK_STREAM, 0);
	if(iSocketID<0){
	    writeLog("%s,%d,Creat socket error\n",__FILE__,__LINE__);
	    return(-1);
	}
	iLength = sizeof(stSockAddr);
	stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(port);
	
	TimeOut.l_onoff = 1;
	TimeOut.l_linger = 10;
	setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
	setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
	/*setsockopt(iSocketID,SOL_SOCKET,SO_REUSEPORT,(char *)&TimeOut,sizeof(TimeOut));*/
	iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
	setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
	setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
	/*setsockopt(iSocketID,SOL_SOCKET,SO_REUSEPORT,(char *)&TimeOut,sizeof(TimeOut));*/
	if(iRc<0){
	    writeLog("%s,%d,Bind socket error[%d]\n",__FILE__,__LINE__,errno);
	    return(-1);
	}
	iRc = listen(iSocketID, 1);
	if(iRc<0){
	    writeLog("%s,%d,Listen socket error[%d]\n",__FILE__,__LINE__,errno);
	    return(-1);
	}
	writeLog("%s,%d,Bind over\n",__FILE__,__LINE__);
	return(iSocketID);
}

/**********************************************************************
*  int Sock_accept(int socketid);                                       *
*  功能：接受远程连接                                                 *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/                     
int Sock_accept(int socketid){
	int iAcceptedID;
	struct linger TimeOut;
	char   caIP[4*4];
	unsigned int iLength;
	struct sockaddr_in stSockAddr;
	do {
        iLength=sizeof(stSockAddr);
        iAcceptedID = accept(socketid,(struct sockaddr*)&stSockAddr,
			(socklen_t *)&iLength);
        if (iAcceptedID < 0) {
			writeLog("%s,%d,Accept err,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}else{
			if (iAcceptedID == 0) continue;
			writeLog("Accept,socketId=%d\n",iAcceptedID);
			TimeOut.l_onoff = 1;
			TimeOut.l_linger = 10;
			setsockopt(iAcceptedID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
			
			setsockopt(iAcceptedID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
			netGetPeer(iAcceptedID, caIP);
			writeLog("%s,%d,Accept socket,remote addr: %s\n",__FILE__,__LINE__,caIP);
			return(iAcceptedID);
		}
	}while(1);
	
}
/**********************************************************************
*  int Sock_close(int socketid);                                        *
*  功能：关闭远程连接                                                 *
*  参数：连接id                                                       *
*  返回值：                                                           *
*                                                                     *
**********************************************************************/                 
int Sock_close(int socketid){
	int iRc;
	if(socketid==0)
		return(0);
	iRc=close(socketid);
	return(0);
}

/**********************************************************************
*  int Sock_send                                                        *
*  功能：发送数据                                                     *
*  参数：iSocketID-远端socket id                                      *
*        ht-发送数据的哈希表结构                                      *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int Sock_send(int iSocketID,char *buf,int length,int timeout){
	int  nTotalLen;
	int  nSubLen;
	int  nShouldLen;
	int iRc;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		if(timeout<=0){
			iRc=Sock_waitfor_write(iSocketID,WAITSECONDS);
		}else{
			iRc=Sock_waitfor_write(iSocketID,timeout);
		}
		if(iRc<=0){
			writeLog("%s,%d,等待发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			writeLog("%s,%d,发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}

/**********************************************************************
*  int Sock_recv                                                        *
*  功能：接收远端数据                                                 *
*  参数：iSocketID-远端socket id                                      *
*        ht-接收哈希表结构                                            *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int Sock_recv(int iSocketID,char *buf,int length,int timeout){/***接收指定长度数据***/
	int  nTotalLen;
	int  nSubLen;
	int  nShouldLen;
	int iRc;
	iRc=nTotalLen=nSubLen=nShouldLen=0;
	nShouldLen=length;
	while(nTotalLen<nShouldLen){	
		if(timeout<=0){
			iRc=Sock_waitfor_read(iSocketID,WAITSECONDS);
		}else{
			iRc=Sock_waitfor_read(iSocketID,timeout);
		}
		if(iRc<=0){
			writeLog("%s,%d,等待接收数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
		if(nSubLen<=0){
			writeLog("%s,%d,接收数据错误,errno=%d\n",__FILE__,__LINE__,errno);
			return(-1);
		}
		nTotalLen+=nSubLen;
	}
	return(0);
}

static int netGetPeer(int iSocketID, char *pbyIPAddr )
{
	struct sockaddr_in stSockAddr;
	unsigned int iLen;
	iLen = sizeof( stSockAddr );
	if (pbyIPAddr) {
		if (getpeername(iSocketID,(struct sockaddr *)&stSockAddr,(socklen_t *)&iLen))
		{
			return(-1);
		}
		strcpy(pbyIPAddr,(char *)inet_ntoa(stSockAddr.sin_addr));
	}
	return(0);
}
/**********************************************************************
*  int Sock_connect(char *addr,int port);                               *
*  功能：连接远程服务                                                 *
*  参数：                                                             *
*      addr-远程地址                                                  *
*      port-远程端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/                 
int Sock_connect(char *addr,int port){
	struct hostent *hp;
	struct sockaddr_in stSockAddr;
	struct linger TimeOut;
	unsigned long lInterAddr;
	int  iRc,iSocketID;
	char caIP[4*4];
	char *caServerName;
	int iPort;
	iPort=port;
	caServerName=addr;
	iSocketID = socket(AF_INET, SOCK_STREAM, 0);
	if(iSocketID<0){
	    writeLog("%s,%d,Creat socket error[%d]\n",__FILE__,__LINE__,errno);
	    return(-1);
	}
	MEMSET_DEBUG((char *)&stSockAddr,'\0',sizeof(stSockAddr));
	hp = gethostbyname(caServerName);
	if (hp != NULL){
		lInterAddr = *((unsigned long *)hp->h_addr);
	}else{
	    if ((lInterAddr = inet_addr(caServerName)) == -1) {
	            writeLog("%s,%d,Get server address error[%d]\n",__FILE__,__LINE__,errno);
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
	writeLog("%s,%d,SocketDtl:\n[%s,%d]\n[%d %d %d %d]\n",__FILE__,__LINE__,addr,port,
	((unsigned char *)&lInterAddr)[0],((unsigned char *)&lInterAddr)[1],((unsigned char *)&lInterAddr)[2], ((unsigned char *)&lInterAddr)[3]);
	stSockAddr.sin_addr.s_addr = lInterAddr;
	iRc = connect(iSocketID,(struct sockaddr*)(&stSockAddr),sizeof(stSockAddr));
	if(iRc){
		writeLog("%s,%d,Connect socket error,[%s],[%d],errno=%d\n",__FILE__,__LINE__,addr,port,errno);
		close(iSocketID);
		return(-1);
	}
	TimeOut.l_onoff = 1;
	TimeOut.l_linger = 10;
	setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
	setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
	netGetPeer(iSocketID, caIP);
	writeLog("%s,%d,Connect socket,remote addr: %s\n",__FILE__,__LINE__,caIP);
	return(iSocketID);
        
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
