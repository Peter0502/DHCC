/*******************************************************
 * �� �� ��: 
 * �����ܣ�
 * ��    �ߣ�ligl2008
 * �������ڣ�
 * �� �� �ˣ�
 * �޸����ڣ�
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
#include "public.h"
#include "tx_def_c.h"
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
    strcpy(c_port,"7802");
    get_zd_data("0300",cCardno);
    get_zd_double("0400",&tx_amt);
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cCardno);
    if(ret)
    {
    		sprintf(acErrMsg,"��ѯ�����ʺŹ�ϵ��!!![%d]",ret);
				WRITEMSG
				goto ErrExit;
    }
    ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
    if(ret)
    {
    		sprintf(acErrMsg,"��ѯ�����������!!![%d]",ret);
				WRITEMSG
				goto ErrExit;
    }
    set_zd_data("0250",g_dd_mst.name);
    set_zd_double("0400",tx_amt);
    set_zd_data("0300",cCardno);
   	strcpy(tx_code,"1799");
		vtcp_log("����������:trcode = [%s]", tx_code);
		ret = Tx_def_Sel(g_pub_tx.reply, &tx_def, "tx_code='%s'", tx_code);
		if (ret != 0)
		{
		  sprintf( acErrMsg,"ȡbtmap����!!! [%d],[%s] ", ret, g_pub_tx.reply );
		  goto ErrExit;
		}
		vtcp_log("���ǰ:btmap=[%s]", tx_def.bit_map);
   	vtcp_log("���ǰ:pack=[%s]",g_pack);
    ret=pub_comm_crea_pack(tx_def.bit_map, g_pack ,&g_packlen);
    if (ret != 0)
		{
		vtcp_log("----����-------->�������������!!!" );
		strcpy( g_pub_tx.reply, "9999" );
		goto ErrExit;
		}
    vtcp_log("�����:pack= [%s] ",g_pack+21);	
		/* ���ͷ*/ 
  	get_zd_data("0030",jgbm);
		get_zd_data("0070",tty);
		memcpy(sendbuf, tx_code,4);
    vtcp_log("�����:pack=+++++");
		memcpy(sendbuf+5, serv, strlen(serv));
    vtcp_log("�����:pack=+++++");
		memcpy(sendbuf+20, "0", 1);
    vtcp_log("�����:pack=+++++");
		memcpy(sendbuf+21, jgbm, 5);
    vtcp_log("�����:pack=+++++");
	  memcpy(sendbuf+26, tty, strlen(tty));
    vtcp_log("�����:pack=+++++");
		memcpy(sendbuf+46, "0", 1);
		sprintf(tmpstr,"%04d",g_packlen);
		memcpy(sendbuf+56, tmpstr, 4);
    vtcp_log("�����:pack=+++++");
		memcpy(sendbuf+60, g_pack, g_packlen);
    vtcp_log("�����:pack=+++++");
		g_packlen = g_packlen + 60;
		memset(g_pack,0x0,sizeof(g_pack));
    vtcp_log("�����:pack=+++++");
		memcpy(g_pack,sendbuf,g_packlen);
    vtcp_log("�����:pack=+++++");
		ret = tcpclt(c_port, c_addr, g_pack, &g_packlen, g_filenames);
		if (ret != 0)
		{
			vtcp_log("\n -------------->����������ͨѶ����!! \n");
			strcpy( g_pub_tx.reply, "9999" );
			goto ErrExit;
		}
		vtcp_log("\n���ܷ��ر���: [%s] \n",g_pack);
	  memset(sendbuf,0x0,sizeof(sendbuf));
	  memcpy(sendbuf,g_pack+60,g_packlen-60);
	  sendbuf[g_packlen - 60] = 0;
		vtcp_log("\n ��������Ϊ: [%s] \n",sendbuf);
		ret=pub_comm_chan_pack(sendbuf);
		if (ret != 0)
		{
			vtcp_log("----����-------->�������������!!!" );
			strcpy( g_pub_tx.reply, "9999" );
			goto ErrExit;
		}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
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

	vtcp_log("��������ip[%s]port[%s]\n\n",ip,port);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = inet_addr( ip );

	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0 )
	{
		vtcp_log("[%s][%d]����SOCKETʧ��!\n",__FILE__,__LINE__);
		return 1;
	}
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		vtcp_log("[%s][%d]��������ͨѶʧ��!\n",__FILE__,__LINE__);
		return 2;
	}
	vtcp_log("[%s][%d]�Ѿ����̨��������! file=[%s]\n",__FILE__,__LINE__,tfile);

	i_rtn = TcpSnd(sock, buffer, *i_buflen, tfile);
	if(i_rtn != 0)
	{
		shutdown (sock,2);
		close(sock);
		vtcp_log("[%s][%d]��������ʧ��!\n",__FILE__,__LINE__);
		return 3;
	}

	memset(buffer,0x0,sizeof(buffer));
	memset(tfile,0x0,sizeof(tfile));

	i_rtn = TcpRcv(sock, buffer, i_buflen, tfile);
	if(i_rtn != 0)
	{
		shutdown (sock,2);
		close(sock);
		vtcp_log("[%s][%d]��������ʧ��!\n",__FILE__,__LINE__);
		return 3;
	}
	vtcp_log("[%s][%d]�Ѿ����̨�������ӽ����������!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]  weijian = [%s] \n",__FILE__,__LINE__,tfile);

	shutdown (sock,2);
	close(sock);
	return 0;
}
