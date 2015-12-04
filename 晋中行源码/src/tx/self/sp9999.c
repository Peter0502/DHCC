/*******************************************************
 * 程 序 名: 
 * 程序功能：
 * 作    者：ligl2008
 * 开发日期：
 * 修 改 人：
 * 修改日期：
 *******************************************************/

#define EXTERN
#include	<stdio.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<stropts.h>
#include 	<poll.h>
#include 	<sys/ipc.h>
#include 	<sys/msg.h>
#include 	<errno.h>
#include  <stdlib.h>
#include	<memory.h>
#include	<fcntl.h>
#include	<sys/socket.h>
#include	<time.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>

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

#include"sys/errno.h"
#include <sys/timeb.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>


#include "public.h"
#include "tx_def_c.h"
#include "svrpub_c.h"

#define BAL_COMM_PASSWD       "dhccnew"

sp9999()
{
	char c_addr[16];
	char c_port[5];
	char cCardno[20];
	char sendbuf[4000];
	char g_pack[2096];
	int g_packlen=0,ret=0;
	double tx_amt=0.00;
	char jgbm[6];
	char tty[5];
	char tmpstr[10];
	char serv[10];
	char tx_code[5];
	struct tx_def_c tx_def;
	char g_filenames[1000];
	int  sockfd;
		
	memset(c_addr,0x0,sizeof(c_addr));
	memset(g_filenames,0x0,sizeof(g_filenames));
	memset(&tx_def,0x0,sizeof(struct tx_def_c));
	memset(c_port,0x0,sizeof(c_port));
	memset(cCardno,0x0,sizeof(cCardno));
	memset(g_pack,0x0,sizeof(g_pack));
	memset(jgbm,0x0,sizeof(jgbm));
	memset(tty,0x0,sizeof(tty));
	memset(serv,0x0,sizeof(serv));
	memset(tx_code,0x0,sizeof(tx_code));
	memset(sendbuf,0x0,sizeof(sendbuf));	
	pub_base_sysinit();
	strcpy(c_addr,"192.168.1.3");
	/*strcpy(c_addr,"127.0.0.1");*/
	strcpy(c_port,"7803");
	get_zd_data("0300",cCardno);
	get_zd_double("0400",&tx_amt);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'",cCardno);
	if(ret)
	{
    		sprintf(acErrMsg,"查询介质帐号关系表!!![%d]",ret);
			WRITEMSG
			goto ErrExit;
	}
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if(ret)
	{
    		sprintf(acErrMsg,"查询活期主表错误!!![%d]",ret);
				WRITEMSG
				goto ErrExit;
	}
	set_zd_data("0250",g_dd_mst.name);
	set_zd_double("0400",tx_amt);
	set_zd_data("0300",cCardno);
   	strcpy(tx_code,"1799");
	vtcp_log("渠道交易码:trcode = [%s]", tx_code);
	ret = Tx_def_Sel(g_pub_tx.reply, &tx_def, "tx_code='%s'", tx_code);
		if (ret != 0)
		{
		  sprintf( acErrMsg,"取btmap错误!!! [%d],[%s] ", ret, g_pub_tx.reply );
		  goto ErrExit;
		}
		Rec_Bitp[sizeof(Rec_Bitp)-1]=0;
		vtcp_log("组包前:Rec_Bitp=[%s]", Rec_Bitp);
		
		vtcp_log("组包前:btmap=[%s]", tx_def.bit_map);
   	vtcp_log("组包前:pack=[%s]", g_pack);
    ret=pub_comm_crea_pack(tx_def.bit_map, g_pack ,&g_packlen);
    if (ret != 0)
		{
		  vtcp_log("----渠道-------->向主机组包错误!!!" );
		  strcpy( g_pub_tx.reply, "9999" );
		  goto ErrExit;
		}
		g_pack[g_packlen] = 0;
		vtcp_log("\n 组包后位图为: [%.21s] \n", g_pack);
		vtcp_log("\n 组包后正文为: \n[%s] ", g_pack+21);    
    
    /*nt_Encrypt(g_pack, g_packlen, BAL_COMM_PASSWD,1);  加密
		g_pack[g_packlen] = 0;
		vtcp_log("\n 加密后位图为: [%.21s] \n", g_pack);
		vtcp_log("\n 加密后正文为: \n[%s] ", g_pack+21);*/
    
		/* 组包头*/ 
  	get_zd_data("0030",jgbm);
		get_zd_data("0070",tty);
		memcpy(sendbuf, tx_code, 4);
    vtcp_log("组包后:pack=+++++");
		memcpy(sendbuf+5, serv, strlen(serv));
    vtcp_log("组包后:pack=+++++");
		memcpy(sendbuf+20, "0", 1);
    vtcp_log("组包后:pack=+++++");
		memcpy(sendbuf+21, jgbm, 5);
    vtcp_log("组包后:pack=+++++");
	  memcpy(sendbuf+26, tty, strlen(tty));
    vtcp_log("组包后:pack=+++++");
		memcpy(sendbuf+46, "0", 1);
		sprintf(tmpstr,"%04d",g_packlen);
		memcpy(sendbuf+56, tmpstr, 4);
    vtcp_log("组包后:pack=+++++");
		memcpy(sendbuf+60, g_pack, g_packlen);
    vtcp_log("组包后:pack=+++++");
		g_packlen = g_packlen + 60;
		memset(g_pack,0x0,sizeof(g_pack));
    vtcp_log("组包后:pack=+++++");
		memcpy(g_pack, sendbuf, g_packlen);
    vtcp_log("组包后:pack=+++++");
		g_pack[g_packlen] = 0;
    vtcp_log("============ 开始和主机通讯============"); 
    
 	  /*********** 发送 ***********/	
	
	  ret = TcpSendpack(c_port, c_addr, g_pack, g_packlen, g_filenames);
		if (ret != 0)
		{
					vtcp_log("\n -------------->渠道和主机通讯出错!! \n");
					strcpy( g_pub_tx.reply, "9999" );
					goto ErrExit;
	  }  

		  
    
    /*
		ret = tcpclt(c_port, c_addr, g_pack, &g_packlen, g_filenames);
		if (ret != 0)
		{
			vtcp_log("\n -------------->渠道和主机通讯出错!! \n");
			strcpy( g_pub_tx.reply, "9999" );
			goto ErrExit;
		}
		g_pack[g_packlen] = 0;
		
		vtcp_log("\n接受返回报文: \n[%s] ",g_pack);
		
	  memset(sendbuf,0x0, sizeof(sendbuf));
	  memcpy(sendbuf, g_pack+60, g_packlen-60);
	  sendbuf[g_packlen - 60] = 0;*/
	  
    /*********** Print For Test ********************
    char cTmpPack[4001];
    int  iTst=0;
    memcpy(cTmpPack, sendbuf, sizeof(cTmpPack)-1 );
    cTmpPack[sizeof(cTmpPack)-1]=0;
    for(iTst=0; iTst<g_packlen - 60; iTst++ )
    {
    	  if( cTmpPack[iTst] == 0 ) cTmpPack[iTst] = '@';  	  
    }  
    cTmpPack[g_packlen - 60]=0;
    vtcp_log("后台已加密: \n [%s] \n", cTmpPack);
    ********** Print For Test *********************/

    /*nt_Encrypt(sendbuf, g_packlen-60, BAL_COMM_PASSWD,0); 解密*/
		/*sendbuf[g_packlen-60] = 0;
		
    memcpy(cTmpPack, sendbuf, sizeof(cTmpPack)-1 );
    cTmpPack[sizeof(cTmpPack)-1]=0;
    for(iTst=0; iTst<g_packlen-60; iTst++ )
    {
    	  if( cTmpPack[iTst] == 0 ) cTmpPack[iTst] = '@';  	  
    }  
    cTmpPack[g_packlen-60]=0;
    vtcp_log("后台已加密: \n [%s] \n", cTmpPack); 
    *********** Print For Test *********************/
    
    /*		
		ret=pub_comm_chan_pack(sendbuf);
		if (ret != 0)
		{
			vtcp_log("----渠道-------->向主机拆包错误!!!" );
			strcpy( g_pub_tx.reply, "9999" );
			goto ErrExit;
		}*/
		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


int TcpSendpack(char *PORT, char *IP, char *buffer, int *i_buflen, char *tfile)
{

	struct sockaddr_in server;
	int sock = 0, i_rtn = 0;
	int n_flag;
	char ip[16],port[5];

	unsigned short portnum;

	memset(ip,0x0,sizeof(ip));
	memset(port,0x0,sizeof(port));
	
	strcpy(ip, IP);
	strcpy(port, PORT);
	portnum = atoi(port);

	vtcp_log("传进来的 ip[%s] port[%s] \n\n", ip, port);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = inet_addr( ip );

	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0 )
	{
		vtcp_log("[%s][%d]创建SOCKET失败!\n",__FILE__,__LINE__);
		return 1;
	}
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		vtcp_log("[%s][%d]建立数据通讯失败!\n",__FILE__,__LINE__);
		return 2;
	}
	vtcp_log("[%s][%d]已经与后台建立连接! file=[%s]\n",__FILE__,__LINE__,tfile);

	i_rtn = TcpSnd(sock, buffer, *i_buflen, tfile);
	if(i_rtn != 0)
	{
		shutdown (sock,2);
		close(sock);
		vtcp_log("[%s][%d]发送数据失败!\n",__FILE__,__LINE__);
		return 3;
	}
	vtcp_log("[%s][%d]发送数据成功 ! file=[%s]\n",__FILE__,__LINE__,tfile);
	shutdown (sock,2);
	close(sock);
	return 0;
}



int tcpclt(char *PORT,char *IP,char *buffer,int *i_buflen,char *tfile)
{

	struct sockaddr_in server;
	int sock = 0, i_rtn = 0;
	int n_flag;
	char ip[16],port[5];

	unsigned short portnum;

	memset(ip,0x0,sizeof(ip));
	memset(port,0x0,sizeof(port));
	strcpy(ip,IP);
	strcpy(port,PORT);
	portnum = atoi(port);

	vtcp_log("传进来的ip[%s]port[%s]\n\n",ip,port);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = inet_addr( ip );

	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0 )
	{
		vtcp_log("[%s][%d]创建SOCKET失败!\n",__FILE__,__LINE__);
		return 1;
	}
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		vtcp_log("[%s][%d]建立数据通讯失败!\n",__FILE__,__LINE__);
		return 2;
	}
	vtcp_log("[%s][%d]已经与后台建立连接! file=[%s]\n",__FILE__,__LINE__,tfile);

	i_rtn = TcpSnd(sock, buffer, *i_buflen, tfile);
	if(i_rtn != 0)
	{
		shutdown (sock,2);
		close(sock);
		vtcp_log("[%s][%d]发送数据失败!\n",__FILE__,__LINE__);
		return 3;
	}

	memset(buffer,0x0,sizeof(buffer));
	memset(tfile,0x0,sizeof(tfile));

	i_rtn = TcpRcv(sock, buffer, i_buflen, tfile);
	if(i_rtn != 0)
	{
		shutdown (sock,2);
		close(sock);
		vtcp_log("[%s][%d]接收数据失败!\n",__FILE__,__LINE__);
		return 3;
	}
	vtcp_log("[%s][%d]已经与后台建立连接接收数据完毕!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]  weijian = [%s] \n",__FILE__,__LINE__,tfile);

	shutdown (sock,2);
	close(sock);
	return 0;
}



int creat_sock(ip, port)
char *ip;
char *port;
{
	struct hostent *phe;    
	struct protoent *ppe;   
	struct sockaddr_in sin; 
	int s,sock_opt,tmp_fd;                  
	int resu;
	int package_size = 1024;

	memset(&sin,0,sizeof(sin));

	s = socket(AF_INET,SOCK_STREAM,0);
	if (s<0)
	{
		return -1;
	}
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(port));
	
	/*待补： 绑定Client端口，如果在本机，此部分必须封，注意打开!
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	sock_opt = 1;
	
	*设置socket属性，端口可重用*
	if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,( char *)&sock_opt,sizeof(sock_opt)) == -1 )
	{
		nErrLog(ComLogFile,__FILE__,__LINE__,"(C)ERR:设置socket属性失败!\n"); 
		return(-1);
		
	}
	
	绑定端口
	if ( bind(tmp_fd,(struct sockaddr *)&sin,sizeof(struct sockaddr_in)) ==-1 )
	{
		nErrLog(ComLogFile,__FILE__,__LINE__,"(C)ERR:绑定(BIND)Clinet端口失败!\n"); 
		return(-1);
	}
	*/
	
	
	
	sin.sin_addr.s_addr = inet_addr(ip);
	
	sock_opt = 1;
	if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,( char *)&sock_opt,sizeof(sock_opt)) == -1 )
	{	
		return(-1);		
	}
	
	sock_opt = 1;
	if ( setsockopt(s,IPPROTO_TCP,TCP_NODELAY,( char *)&sock_opt,sizeof(sock_opt)) == -1 )
	{
		return(-1);
	}
	
	resu = connect(s,(struct sockaddr *)&sin,sizeof(struct sockaddr_in));
	if ( resu < 0) 
	{
		close(s); 
		return -1;
	}
	return s;
}

