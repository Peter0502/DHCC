/*************************************************
* �� �� ��: sp9718.c
* �������������ݹ�Ա�Ż��Թ�Ա����
*
* ��    ��: rob
* ������ڣ�2003��03��31��
* �޸ļ�¼�� 
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

sp9718()  
{ 		
	int ret=0;
	char tmp_title[41];
	char cTx_code[5];

	struct com_tel_c	g_com_tel;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(cTx_code,0x00,sizeof(cTx_code));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0920",g_pub_tx.tel);	/*��Ա����*/
	get_zd_data("0650",cTx_code);	/*���״���   Ŀǰֻ�����ֽ��������*/
	
	if( strlen(g_pub_tx.tel)==0 )
	{
		goto OkExit;
	}
	
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",g_pub_tx.tel);
	if(ret==100)
	{
		sprintf(acErrMsg,"�ù�Ա������");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O120");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	/****�����ֽ��������ʱ���Է�ûǩ�������������  add by wanglei 20070429****/
	/****���ֽ����ʱ���Է�����ǩ��״̬��������� modify at 20100722
	if(g_com_tel.csts[0] != '2')
	****/
	vtcp_log("[%s][%d] TX_CODE==[%s]",__FILE__,__LINE__,cTx_code);
	if(memcmp(cTx_code,"5203",4)==0)
	{
		if(g_com_tel.csts[0] == '1')
		{
			sprintf(acErrMsg,"�ù�Ա״̬��ǩ��! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O046");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '2')
		{
			sprintf(acErrMsg,"�ù�Ա״̬��ǩ��! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O046");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '3')
		{
			sprintf(acErrMsg,"�ù�Ա״̬������! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O063");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '4')
		{
			sprintf(acErrMsg,"�ù�Ա״̬��ɾ��! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O062");
			goto ErrExit;
		}
	}
	/**** end by wanglei ****/

	set_zd_data("0860",g_com_tel.name);
	vtcp_log("[%s][%d] br_no = [%s]\n",__FILE__,__LINE__,g_com_tel.br_no);
	set_zd_data("0910",g_com_tel.br_no); /****���ػ�����   20070425****/


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
