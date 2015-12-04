#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/select.h>
/**
//#include <sys/select.h>
**/

#include "UnionSck.h"

int	UnionCreatSocketClient(char *ipaddr,int port);
int	UnionCloseSocket(int sckid);
int     UnionSendToSocket(int sckid,unsigned char *buf,int len,long timeout);
int     UnionReceiveFromSocket(int sckid,char *buf,int len,long timeout);
int Hsm56Connect()
{
	char 	  hsmfile[1024];
	char      buf[3];
	FILE      *fp;
	char      ip[20];
	int	port;
	int sckid;
	int i=0;
	memset(buf,0,sizeof(buf));
	memset(ip,0,sizeof(ip));

	/**open tcpip.confg**/
/**
//	sprintf(hsmfile,"%s/tcpip.confg",getenv("UNIONETC"));
**/
	if((fp=fopen("tcpip.confg","r"))==NULL)
	{
	printf("Could not open tcpip.confg\n");
	return -1;	
	}
	fscanf(fp,"%s",ip);
	fscanf(fp,"%s",buf);
	port=0;
	for (i=0;i<strlen(buf);i++)
	port=port*10+(buf[i]-'0');
/**
//	printf("[port]:%d\n",port);
//	printf("[%s][%d][port]:%d\n",__FILE__, __LINE__, port);
**/
	fclose(fp);
	return (ConnectToHsmSJL(ip,port));
  }
/**Open stream socket and connect to server**/
int UnionCreatSocketClient(char *ip,int port)
{
	struct		sockaddr_in psckadd;	
	struct          linger Linger;
	int		sckcli;
	int		on=1;
/**
//  printf("[%s][%d][port]:%d\n",__FILE__, __LINE__, port);
**/
	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	
        psckadd.sin_family            = AF_INET;
        psckadd.sin_addr.s_addr       = inet_addr(ip);
        psckadd.sin_port=htons((u_short)port);

    if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        HsmErrLog("in UnionCreatSocketClient:: can not open stream socket\n");
        return(INVALID_SOCKET);
        }
   
    if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) < 0)
	{
	HsmErrLog("in UnionCreatSocketClient:: cannot connect to server! svr_ip = [%s] port = [%d]\n",ip,port);
                UnionCloseSocket(sckcli);
		return(INVALID_SOCKET);
                }
    
    Linger.l_onoff = 1;
    Linger.l_linger = 0;
    if (setsockopt(sckcli,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		HsmErrLog("in UnionCreatSocketClient:: setsockopt linger!");
		UnionCloseSocket(sckcli);
		return(INVALID_SOCKET);
	}
	if (setsockopt(sckcli, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)))
	{
		HsmErrLog("in UnionCreatSocketClient:: setsockopt SO_OOBINLINE!\n");
		return(INVALID_SOCKET);
	}
	on = 1;
	if (setsockopt(sckcli, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)))
	{
		HsmErrLog("in UnionCreatSocketClient:: setsockopt: TCP_NODELAY");
		return(INVALID_SOCKET);
	}

    /**printf("in UnionCreatSocketClient:: creat stream socket svr_ip = [%s] port = [%d]!\n",ip,port);**/

	return(sckcli);
}

/**Shutdown HSM sockfd**/
int UnionCloseSocket(int sockfd)
{
    	shutdown(sockfd, 2);
    	
	if (close(sockfd) != 0)
		{
		HsmErrLog("close client connection error!\n");
		return -2;
		}
	else
	return(0);
}

/**Send data to Hsm**/

int UnionSendToSocket(int sckid,unsigned char *buf, int len,long timeout)
{
	char errmsg[512];
        int i;
        struct timeval ptv;
        fd_set sockset;
        
        /**printf("In UnionSendToSocket\n");
        for (i=0;i<len;i++)
        printf("[buf]:%x",buf[i]);
        printf("\n");**/
      
        ptv.tv_sec=timeout;
        FD_ZERO( &sockset);     
        FD_SET(sckid, &sockset);
	select(sckid+1,NULL,&sockset,NULL,&ptv);
        if(FD_ISSET(sckid,&sockset)) 
	   {
	    if (send(sckid,buf,len,0) != len)
	       {
		perror(errmsg);
		HsmErrLog("in UnionSendToSocket:: %s\n",errmsg);
		return(-1);
	        }
             }else {
	    	   printf("[ERROR]: Send data to sjl06e timeout!");
                   return(-2);
                }                
    return(len);
   
    }

/**Receive data from  HSM**/

int UnionReceiveFromSocket(int sckid, char *buf,int len,long timeout)
{
	int rc;
	int i;
	struct timeval ptv;
	fd_set sockset;
		   
	FD_ZERO( &sockset);	
	FD_SET(sckid, &sockset);
	
	ptv.tv_sec=timeout;
        select(sckid+1,&sockset,NULL,NULL,&ptv);
        
        if(FD_ISSET(sckid,&sockset)) 
	   {
	    if ((rc = recv(sckid,buf,len,0)) < 0)
	       {
		HsmErrLog("in UnionReceiveFromSocket:: receive from server! errno = [%d]\n",errno);
		perror(buf);
		return(-1);
	        }
	    }else {
	    	   HsmErrLog("[ERROR]: Receive data from sjl06e timeout!");
                   return(-2);
                   }
	      
	
	/**printf("In UnionReceiveFromSocket\n");
	for(i=0,i<len;i++)
	printf("[outbuf]:%x",buf[i]);
	printf("\n");**/
	
    return(rc);
}
