/***************************************************************
* �� �� ��:     spD304.c
* ����������	�������ȡ���ӽ���
* ��    ��:     jack
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

spD399()
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
	get_zd_data("0370",g_pub_tx.crd_no);
	strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
	g_pub_tx.ac_seqn=0;
	get_zd_int("0340",&g_pub_tx.ac_seqn);
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	get_zd_data("0790",g_pub_tx.draw_pwd);
	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);
	get_zd_data("0620",g_pub_tx.id_no);

	g_reply_int=pub_base_prot("",4);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���Э���Ǽ����س���!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�������ȡ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�������ȡ��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
