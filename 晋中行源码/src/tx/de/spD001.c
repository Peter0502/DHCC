/***************************************************************
* �� �� ��:     spD001.c
* ����������	���ս���
*
* ��    ��:     jack
* ������ڣ�    2004��1��6��
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
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_id_code_rel_c.h"

spD001()
{
	/*** ��ʼ��ȫ�ֱ��� ***/
	double amt;
	pub_base_sysinit();
	set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140110**/
	get_zd_data("1161",g_pub_tx.mdm_code);	
	get_zd_data("1162",g_pub_tx.crd_no);	
	get_zd_data("1164",g_pub_tx.id_type);	
	get_zd_data("1165",g_pub_tx.id_no);	
	get_zd_data("1166",g_pub_tx.name);
	vtcp_log("[%s][%d] beg_note =[%s]\n",__FILE__,__LINE__,g_pub_tx.beg_note_no);	
	get_zd_data("1168",g_pub_tx.beg_note_no);	
	vtcp_log("[%s][%d] beg_note =[%s]\n",__FILE__,__LINE__,g_pub_tx.beg_note_no);	
	get_zd_data("1151",g_pub_tx.draw_uncon_yn);
	get_zd_data("1152",g_pub_tx.draw_pwd_yn);
	get_zd_data("1153",g_pub_tx.qry_pwd);
	get_zd_data("1154",g_pub_tx.draw_pwd);
	get_zd_data("1155",g_pub_tx.draw_id_yn);
	get_zd_data("1157",g_pub_tx.draw_seal_yn);
	get_zd_data("1159",g_pub_tx.pwd_mach_yn);		/*֧������*/
	get_zd_double("102F",&amt);
    vtcp_log("grt------lj[%.2lf]",amt);

	strcpy(g_pub_tx.brf,"����");

	pub_base_strpack(g_pub_tx.crd_no);
	pub_base_strpack(g_pub_tx.beg_note_no);
    vtcp_log("wyb--[%s][%s][%s]",g_pub_tx.mdm_code,g_pub_tx.id_type,g_pub_tx.beg_note_no);
	get_zd_data("0191",g_pub_tx.using_ind);
    vtcp_log("wyb--[%s]",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
	g_pub_tx.svc_ind=1002;
	if(g_pub_tx.id_type[0]=='Z')/**���ʻ� 2007-6-8 9:33**/
  {
  	g_reply_int=pub_acc_crt_cif_yzh(g_pub_tx.id_type);
  	if(g_reply_int)
  	{
  		sprintf(acErrMsg,"������ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
		  WRITEMSG
		  goto ErrExit;
  	}
  	
  }
	g_reply_int=pub_acct_opn_cif_mdm();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"������ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*** ������� ***/
	vtcp_log("[%s]",g_pub_tx.ac_no);
	set_zd_data("1167",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int pub_acc_crt_cif_yzh(char * id_type)
{
		long cif_no,ret=0;
	static struct cif_alias_inf_c    cif_alias_inf;
	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_id_code_rel_c  cif_id_code_rel;	
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	strcpy(cif_basic_inf.type,"2");		  /* �ͻ����� */
	strcpy(cif_id_code_rel.id_type,id_type);/* ֤������ */
	strcpy(cif_basic_inf.name,g_pub_tx.name);		/* �ͻ����� */
  strcpy(cif_basic_inf.local,"1");			/* ���ر�־1-����2-���3-���� */
	strcpy(cif_basic_inf.poten,"Y");			/* Ǳ�ڱ�־Y-Ǳ�� N-��Ǳ��*/
	strcpy(cif_basic_inf.shareholder_ind,"N");	/* ���йɶ�Y-�� N-�� */
	strcpy(cif_basic_inf.lvl,"4");
	ret = pub_cif_CreYzh_cif_no( &cif_no );
	if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
		return 1;
		}
		cif_basic_inf.cif_no = cif_no ;
	  cif_id_code_rel.cif_no = cif_no;
	  sprintf(cif_id_code_rel.id_no,"%ld",cif_no);
	  sprintf(g_pub_tx.id_no,"%ld",cif_no);
	  ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
	  if ( ret )
	 {
		sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C004");
		return 1;
	 }
	 /* �Ǽǿͻ�������Ϣ�� */
	cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
	cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
	strcpy( cif_basic_inf.sts , "1" ) ;         /* �ͻ�״̬��1���� 2ע�� */
	cif_basic_inf.cif_seqn = 0 ;                /* �ͻ���� */
	ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C005");
		return 1;
	}
	strcpy(cif_alias_inf.alias,cif_basic_inf.name);		/* �ͻ����� */
	cif_alias_inf.cif_no = cif_basic_inf.cif_no ; 		/* �� �� �� */
	cif_alias_inf.alias_seqn = 1 ;                		/* ������� */
	ret=Cif_alias_inf_Ins(cif_alias_inf,g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C083");
		return 1;
	}
	/* �Ǽǽ�����ˮ */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		return 1;
	} 
	return 0;
}
