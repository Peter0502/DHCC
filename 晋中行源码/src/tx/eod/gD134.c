 /***************************************************************************
 *** 程序作者 : Han Xichao                                                ***
 *** 日    期 : 2010-11-23                                                ***
 *** 所属模块 :                                                           ***
 *** 程序名称 : gD134.c                                                   ***
 *** 程序作用 : 翻牌后向平台发送开启服务请求                              ***
 ***                                                                      ***
 *** 使用注意 : 翻牌后向平台发送开启服务请求                              ***
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
#include"com_sys_parm_c.h"
#ifdef SCO
#define socklen_t size_t
#endif
#define EXTERN
#include "public.h"
static int netGetPeer(int iSocketID, char *pbyIPAddr );
static int FileSock_waitfor_read(int iSocketID,int waitseconds);
static int FileSock_waitfor_write(int iSocketID,int waitseconds);
#include"errlog.h"
#define WAITSECONDS 10          /****TODO:测试缩短时间****/
#define TESTADDR    "172.168.99.35"
#define TESTPORT    11188
gD134()
{
  	int iSockId,i=0;
  	char buf[1024];
  	FILE *fp;
  	int  iLength;
  	int  iRc;
  	char ptaddr[20];
  	int ptport;
  	char envptaddr[20];
	char envptport[20];
  	char sendbuf[100];
	struct com_sys_parm_c com_sys_parm;
	
	memset(ptaddr,0x00,sizeof(ptaddr));
	memset(sendbuf,0x00,sizeof(sendbuf));
	memset(envptaddr,0x00,sizeof(envptaddr));
	memset(envptport,0x00,sizeof(envptport));
	memset(&com_sys_parm, 0x00, sizeof(struct com_sys_parm_c) );
	
	pub_base_sysinit();
	
  	/**取IP地址和PORT**/
  	memset(envptaddr,0x00,sizeof(envptaddr));
	memset(envptport,0x00,sizeof(envptport));
	strcpy(envptaddr,getenv("PTADDR"));
	strcpy(envptport,getenv("PTPORT"));
	vtcp_log("%s,%d,envptaddr=[%s],envptport=[%s])",__FILE__,__LINE__,envptaddr,envptport);
	memset(ptaddr,0x00,sizeof(ptaddr));
	if(envptaddr[0] == 0x00)
	{
		vtcp_log("%s,%d,PTADDR未赋值,取默认值)",__FILE__,__LINE__);
		memcpy(ptaddr,TESTADDR,strlen(TESTADDR));
	  	vtcp_log("%s,%d,默认地址ptaddr=[%s]",__FILE__,__LINE__,ptaddr);
  	}
	else
	{
	  vtcp_log("%s,%d,getenvaddr=[%s]",__FILE__,__LINE__,envptaddr);
	  memcpy(ptaddr,envptaddr,strlen(envptaddr));
	  vtcp_log("%s,%d,ptaddr=[%s]",__FILE__,__LINE__,ptaddr);
  	}
	
	if(envptport[0] == 0x00)
	{
		vtcp_log("%s,%d,PTPORT未赋值,取默认值",__FILE__,__LINE__);
		ptport = TESTPORT;
		vtcp_log("%s,%d,默认端口ptport=[%d]",__FILE__,__LINE__,ptport);
	}
	else
	{
	    	vtcp_log("%s,%d,getenvport=[%s]",__FILE__,__LINE__,envptport);
	    	ptport = atoi(envptport);
		vtcp_log("%s,%d,ptport=[%d]",__FILE__,__LINE__,ptport);
    	}
	vtcp_log("%s,%d,ptaddr=[%s],ptport=[%d]",__FILE__,__LINE__,ptaddr,ptport);
	
	/**取核心系统时间**/
	iRc = pub_base_GetSysparm(&com_sys_parm);
	if(iRc)
		{
		  vtcp_log("[%s][%d],取核心系统时间出错[%d]!",__FILE__,__LINE__,iRc);
		  strcpy ( g_pub_tx.reply , "G001" );
		  goto ErrExit;
	}
	
	vtcp_log("[%s][%d],取核心上一日期[%ld],核心当前时间[%ld]!",__FILE__,__LINE__,com_sys_parm.lst_date,com_sys_parm.sys_date);
	
	iRc= sql_begin();
	     if(iRc)
	      {
		      vtcp_log("%s,%d,打开事务错误!",__FILE__,__LINE__);
		      strcpy(g_pub_tx.reply,"9999");
		      goto ErrExit;
	      }


        /* add by LiuHufeng 恢复系统状态2表示翻盘结束或未翻盘 */
        iRc = sql_execute("update com_sys_parm set sys_sts = 2 ");
        if (iRc) {
                vtcp_log("修改系统状态为日终时出错[%d]!",iRc);
                WRITEMSG
                strcpy(g_pub_tx.reply, "9999");
                goto ErrExit;
        }
        /* end by LiuHuafeng 20080501 */
        
        for(i=0;i<3;)
        {
        	iSockId= FileSock_connect(ptaddr,ptport);
        	if(iSockId<0){
                	perror("Connect:");
			            vtcp_log("%s,%d,连接平台错误!\n",__FILE__,__LINE__);
                	strcpy(g_pub_tx.reply, "9999");
			            goto ErrExit;
        	}
        	memset(sendbuf,0x00,sizeof(sendbuf));
          sprintf(sendbuf,"0022602901%ld%ld",com_sys_parm.lst_date,com_sys_parm.sys_date); /**设定交易代码为6029,标志位为01,报文体长度为22**/
				  iRc=FileSock_send(iSockId,sendbuf,26,-1);
          if(iRc){
			         vtcp_log("%s,%d,发送信息到平台错误!\n",__FILE__,__LINE__);
               perror("Send:");
               strcpy(g_pub_tx.reply, "9999");
			         goto ErrExit;
              }
		       memset(buf,0,sizeof(buf));
           iRc=FileSock_recv(iSockId,buf,4,-1);
           if(iRc){
                  perror("Send:");
			            vtcp_log("%s,%d,接收平台信息错误!\n",__FILE__,__LINE__);
                	strcpy(g_pub_tx.reply, "9999");
			            goto ErrExit;
                }
                buf[4]='\0';
                if(memcmp(buf,"0000",4)==0)
                {
                   vtcp_log("%s,%d,收到正确应答!\n",__FILE__,__LINE__);
			             FileSock_close(iSockId);
                   break;
                }else{
			                  FileSock_close(iSockId);
                        vtcp_log("%s,%d,收到错误应答,重新请求!i[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
                        i++;
                        continue;
                }
                /***
                iLength=atoi(buf);
                iRc=FileSock_recv(iSockId,buf+4,iLength,-1);
                if(iRc){
                        perror("Send:");
                        exit(1);
                }
                **/
        }
	/***
	vtcp_log("%s,%d,进入等待时间!WYW_TEST2",__FILE__,__LINE__);
	sleep(30);
	vtcp_log("%s,%d,等待时间过去!WYW_TEST2",__FILE__,__LINE__);
	***/
	if(i>=3)
	{
     vtcp_log("修改平台系统状态为正常时出错[%d]!",i);
     strcpy(g_pub_tx.reply, "9999");
     goto ErrExit;
	}
        vtcp_log("%s,%d,接收数据长度[%d]\n",__FILE__,__LINE__,strlen(buf));
OkExit:
	     sql_commit();
       strcpy(g_pub_tx.reply, "0000");
       vtcp_log("成功接收平台应答信息[%s]", g_pub_tx.reply);
       set_zd_data("0120", g_pub_tx.reply);
       return 0;
ErrExit:
	     sql_rollback();
       vtcp_log("平台处理请求失败[%s]", g_pub_tx.reply);
       set_zd_data(DC_GET_MSG,acErrMsg);
       set_zd_data("0120", "9999");
       return 1;
}

/**********************************************************************
*  int FileSock_bind(char *addr,int port);                                  *
*  功能：初始化本地服务                                               *
*  参数：                                                             *
*      addr-本地地址                                                  *
*      port-本地端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/         
int FileSock_bind(char *addr,int port){
        
        /*初始化linehandler的socket服务*/
        int iSocketID;
        struct sockaddr_in stSockAddr;
        struct linger TimeOut;
        int iLength;
        int iRc;
        iSocketID=socket(AF_INET, SOCK_STREAM, 0);
        if(iSocketID<0){
            vtcp_log("%s,%d,Creat socket error\n",__FILE__,__LINE__);
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
            vtcp_log("%s,%d,Bind socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        iRc = listen(iSocketID, 1);
        if(iRc<0){
            vtcp_log("%s,%d,Listen socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        vtcp_log("%s,%d,Bind over\n",__FILE__,__LINE__);
        return(iSocketID);
}

/**********************************************************************
*  int FileSock_accept(int socketid);                                       *
*  功能：接受远程连接                                                 *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/                     
int FileSock_accept(int socketid){
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
                        vtcp_log("%s,%d,Accept err,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }else{
                        if (iAcceptedID == 0) continue;
                        vtcp_log("Accept,socketId=%d\n",iAcceptedID);
                        TimeOut.l_onoff = 1;
                        TimeOut.l_linger = 10;
                        setsockopt(iAcceptedID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
                        
                        setsockopt(iAcceptedID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
                        netGetPeer(iAcceptedID, caIP);
                        vtcp_log("%s,%d,Accept socket,remote addr: %s\n",__FILE__,__LINE__,caIP);
                        return(iAcceptedID);
                }
        }while(1);
        
}
/**********************************************************************
*  int FileSock_close(int socketid);                                        *
*  功能：关闭远程连接                                                 *
*  参数：连接id                                                       *
*  返回值：                                                           *
*                                                                     *
**********************************************************************/                 
int FileSock_close(int socketid){
        int iRc;
        if(socketid==0)
                return(0);
        iRc=close(socketid);
        return(0);
}

/**********************************************************************
*  int FileSock_send                                                        *
*  功能：发送数据                                                     *
*  参数：iSocketID-远端socket id                                      *
*        ht-发送数据的哈希表结构                                      *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int FileSock_send(int iSocketID,char *buf,int length,int timeout){
        int  nTotalLen;
        int  nSubLen;
        int  nShouldLen;
        int iRc;
        iRc=nTotalLen=nSubLen=nShouldLen=0;
        nShouldLen=length;
        while(nTotalLen<nShouldLen){    
                if(timeout<=0){
                        iRc=FileSock_waitfor_write(iSocketID,WAITSECONDS);
                }else{
                        iRc=FileSock_waitfor_write(iSocketID,timeout);
                }
                if(iRc<=0){
                        vtcp_log("%s,%d,等待发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nTotalLen+=nSubLen;
        }
        return(0);
}

/**********************************************************************
*  int FileSock_recv                                                        *
*  功能：接收远端数据                                                 *
*  参数：iSocketID-远端socket id                                      *
*        ht-接收哈希表结构                                            *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/ 
int FileSock_recv(int iSocketID,char *buf,int length,int timeout){/***接收指定长度数据***/
        int  nTotalLen;
        int  nSubLen;
        int  nShouldLen;
        int iRc;
        iRc=nTotalLen=nSubLen=nShouldLen=0;
        nShouldLen=length;
        while(nTotalLen<nShouldLen){    
                if(timeout<=0){
                        iRc=FileSock_waitfor_read(iSocketID,WAITSECONDS);
                }else{
                        iRc=FileSock_waitfor_read(iSocketID,timeout);
                }
                if(iRc<=0){
                        vtcp_log("%s,%d,等待接收数据错误,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,接收数据错误,errno=%d,len=%d\n",__FILE__,__LINE__,errno,nSubLen);
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
*  int FileSock_connect(char *addr,int port);                               *
*  功能：连接远程服务                                                 *
*  参数：                                                             *
*      addr-远程地址                                                  *
*      port-远程端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/                 
int FileSock_connect(char *addr,int port){
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
            vtcp_log("%s,%d,Creat socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        memset((char *)&stSockAddr,'\0',sizeof(stSockAddr));
        hp = gethostbyname(caServerName);
        if (hp != NULL){
                lInterAddr = *((unsigned long *)hp->h_addr);
        }else{
            if ((lInterAddr = inet_addr(caServerName)) == -1) {
                    vtcp_log("%s,%d,Get server address error[%d]\n",__FILE__,__LINE__,errno);
                        close(iSocketID);
                        return(-1);
            }
        }
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(iPort);
        /*Add by SSH,2004.7.27,长整形地址的最后4位必须为0，然而却有时不是*/
        if(sizeof(lInterAddr)>4){
                memcpy(((char *)&lInterAddr)+4,(char *)&lInterAddr,4);
                memset((char *)&lInterAddr,'\0',4);
        }
        stSockAddr.sin_addr.s_addr = lInterAddr;
        iRc = connect(iSocketID,(struct sockaddr*)(&stSockAddr),sizeof(stSockAddr));
        if(iRc){
                vtcp_log("%s,%d,Connect socket error,[%s],[%d],errno=%d\n",__FILE__,__LINE__,addr,port,errno);
                close(iSocketID);
		strcpy(g_pub_tx.reply,"9999");
                return(-1);
        }
        TimeOut.l_onoff = 1;
        TimeOut.l_linger = 10;
        setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
        setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
        netGetPeer(iSocketID, caIP);
        return(iSocketID);
        
}
static int FileSock_waitfor_write(int iSocketID,int waitseconds){
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

static int FileSock_waitfor_read(int iSocketID,int waitseconds){
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
