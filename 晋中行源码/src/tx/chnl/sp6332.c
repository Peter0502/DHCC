/*******************************************************
 * 程 序 名:  sp6332()
 * 程序功能： 网络管理报文 签到、签退、线路测试 (我行发起)
 * 作    者： ChenMing
 * 开发日期： 20070129
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
#include    <stdlib.h>
#include	<memory.h>
#include	<fcntl.h>
#include	<sys/socket.h>
#include	<time.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>
#include "const.h"

#include "public.h"
#include "tx_def_c.h"


int sp6332()
{
		pub_base_sysinit();
		
		/* 组包，发送，接收应答报文，拆包*/
    if(comm_center()) 
    {
    	goto ErrExit;
    }
		
		get_zd_data(DC_REPLY, g_pub_tx.reply);
		vtcp_log("g_pub_tx.reply====== [%s] ", g_pub_tx.reply);   
		if(memcmp(g_pub_tx.reply, "0000", 4))
		{
			 goto ErrExit;
		}
		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
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


/* 组包，发送，接收应答报文，拆包*/
int comm_center()
{
    char sendbuf[4000];
    char g_pack[2096];
    int g_packlen=0,ret=0;
  	char c_addr[16];
    char c_port[5];

    char jgbm[6];
		char tty[5];
		char tmpstr[10];
		char serv[10];
		char tx_code[5];
		struct tx_def_c tx_def;
		char g_filenames[1000];
		
    memset(c_addr,0x0,sizeof(c_addr));
    memset(g_filenames,0x0,sizeof(g_filenames));
    memset(&tx_def,0x0,sizeof(struct tx_def_c));
    memset(c_port,0x0,sizeof(c_port));

    memset(g_pack,0x0,sizeof(g_pack));
    memset(jgbm,0x0,sizeof(jgbm));
    memset(tty,0x0,sizeof(tty));
    memset(serv,0x0,sizeof(serv));    
		strcpy(c_addr,"192.168.1.3");
    strcpy(c_port,"7802");

    memset(tx_code,0x0,sizeof(tx_code));
		memset(sendbuf,0x0,sizeof(sendbuf));	       
       
    get_zd_data("0160", tx_code);    
		vtcp_log("渠道交易码:trcode = [%s]", tx_code);
		ret = Tx_def_Sel(g_pub_tx.reply, &tx_def, "tx_code='%s'", tx_code);
		if (ret != 0)
		{
		  sprintf( acErrMsg,"取btmap错误!!! [%d],[%s] ", ret, g_pub_tx.reply );
		  return -1;
		}
		vtcp_log("组包前:btmap=[%s]", tx_def.bit_map);
   	vtcp_log("组包前:pack=[%s]",g_pack);
    ret=pub_comm_crea_pack(tx_def.bit_map, g_pack ,&g_packlen);
    if (ret != 0)
		{
		    vtcp_log("----渠道-------->向主机组包错误!!!" );
		    strcpy( g_pub_tx.reply, "9999" );
		    return -1;
		}
    vtcp_log("组包后: 加密前  pack= [%s] ",g_pack+21);	    
	  nt_Encrypt(g_pack, g_packlen, BAL_COMM_PASSWD, 1);    
    vtcp_log("组包后: 加密后  pack= [%s] ",g_pack+21);	
    
		/* 组包头*/ 
  	get_zd_data("0030",jgbm);
		get_zd_data("0070",tty);
		memcpy(sendbuf, tx_code,4);
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
		memcpy(g_pack,sendbuf,g_packlen);
    vtcp_log("组包后:pack=+++++");
    
		ret = tcpclt(c_port, c_addr, g_pack, &g_packlen, g_filenames);
		if (ret != 0)
		{
			vtcp_log("\n -------------->渠道和主机通讯出错!! \n");
			strcpy( g_pub_tx.reply, "9999" );
			return -1;
		}

	  nt_Encrypt(g_pack, g_packlen, BAL_COMM_PASSWD, 0);    
		
		vtcp_log("解密后: \n");
		
    /*********** Print For Test *********************/
    char cTmpPack[4001];
    int  iTst=0;
    int i;
    memset(cTmpPack, 0, sizeof(cTmpPack));  
    for(iTst=0; iTst<g_packlen/50+1; iTst++ )
    {
    	 memset(cTmpPack, 0, sizeof(cTmpPack));
       for(i=0; i<50; i++)
       {  	 
      	  if( g_pack[50*iTst+i] == 0 ) 
      	  {
      	  	cTmpPack[i] = '@';  	
      	  }
      	  else
      	  {
      	  	cTmpPack[i] = g_pack[50*iTst+i] ;
      	  }
       }  
    	 vtcp_log("[%.50s]", cTmpPack);
    }  
    /*********** Print For Test 	
	  memset(sendbuf,0x0,sizeof(sendbuf));
	  memcpy(sendbuf, g_pack+60, g_packlen-60);
	  sendbuf[g_packlen - 60] = 0;
		vtcp_log("\n 返回正文为: [%s] \n",sendbuf);*********************/	
		vtcp_log("开始拆渠道返回包！！ \n");
		ret = pub_comm_chan_pack(g_pack);
		if (ret != 0)
		{
			vtcp_log("----拆渠道返回包错误!!!" );
			strcpy( g_pub_tx.reply, "9999" );
			return -1;
		}	
		vtcp_log("pub_comm_chan_pack() 拆渠道返回包成功！！ \n");
	
	return 0;
}


