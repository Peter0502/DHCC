/***************************************************************
* �� �� ��: spD204.c
* ����������������
* ��    ��: jack
* ������ڣ�2004��1��17��
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

spD204()
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
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_data("1025",g_pub_tx.draw_uncon_yn);	
	get_zd_data("1026",g_pub_tx.draw_pwd_yn);	
	get_zd_data("1027",g_pub_tx.qry_pwd);
	get_zd_data("1028",g_pub_tx.draw_pwd);
	get_zd_data("1029",g_pub_tx.draw_id_yn);	
	get_zd_data("102A",g_pub_tx.id_no);	
	get_zd_data("102B",g_pub_tx.draw_seal_yn);	
	get_zd_data("102C",g_pub_tx.pwd_mach_yn);	
	get_zd_data("102D",g_pub_tx.draw_mach);	
	get_zd_long("102E",&g_pub_tx.mach_date);
	/** ���� **/

	g_reply_int=pub_acct_cls_mdm();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"����������ģ���!");
		WRITEMSG
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
