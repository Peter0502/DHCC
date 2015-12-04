/***************************************************************
* �� �� ��:     spD301.c
* ����������	Э�����Ǽ��ӽ���
* ��    ��:     mike
* ������ڣ�    2004��1��17��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD301()
{
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_int("0340",&g_pub_tx.ac_seqn);
	get_zd_double("0390",&g_pub_tx.tx_amt1);
	get_zd_double("0840",&g_pub_tx.rate);
	/*** �����ڿ����н��� ***/
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"ȡ�����˺Ŷ��մ���![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		goto ErrExit;
	}
	if(strcmp(g_mdm_ac_rel.opn_br_no ,g_pub_tx.tx_br_no)!=0)
	{
		sprintf(acErrMsg,"�˽��ױ����ڿ����а���![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O171");
		goto ErrExit;
	}
	
	g_reply_int=pub_base_prot("XD1",1);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���Э���Ǽ����س���!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Э�����Ǽǳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Э�����Ǽ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
