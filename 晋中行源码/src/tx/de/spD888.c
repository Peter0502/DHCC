/***************************************************************
* �� �� ��:     spD888.c
* ����������	test����
* ��    ��:     mike
* ������ڣ�    2003��r31��11��
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

spD888()
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
	/*
 set_zd_data("C200","wzstest����");
 goto OkExit;
 strcpy(g_pub_tx.reply,"0010");
		sprintf(acErrMsg,"����wzs������ʧ��!");
		WRITEMSG
 strcpy(g_pub_tx.reply,"C001");
goto ErrExit ;
*/
	get_zd_data("1011",g_pub_tx.ac_no);
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	get_zd_data("1014",g_pub_tx.note_type);
	get_zd_data("1018",g_pub_tx.name);

	get_zd_double("1013",&g_pub_tx.tmp_amt1);
	get_zd_double("1019",&g_pub_tx.tmp_amt2);

	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
	sprintf(acErrMsg,"BEG ttD888%s][%s][%s][%d]\nname=[%s]",g_pub_tx.ac_no,
	g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind, g_pub_tx.name);

	WRITEMSG
    
	sprintf(acErrMsg, "\n D888 bal=%.2f, amt=%.2f\n", 
	  g_pub_tx.tmp_amt2, g_pub_tx.tmp_amt1); 
    WRITEMSG
	/** ���� **/

	set_zd_data("101A",g_pub_tx.cur_no);
    set_zd_data("1018","wi1018t����");
    set_zd_data("0820","wzstest����");
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"testw��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"wstudttest��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
