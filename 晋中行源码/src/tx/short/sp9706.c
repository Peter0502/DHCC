/*************************************************
* �� �� ��: sp9706.c
* ���������������ʺŻ��������Ϣ_[��ʧ���ר��]
* ��    ��: jack
* ������ڣ�2005��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "mo_loss_c.h"

sp9706()  
{ 		
	int ret=0;
	char title[40],cur_no[3];
	char ac_no[21],name[50];
	char loss_type[4];
	struct mo_loss_c mo_loss;
	double bal;

	struct cif_basic_inf_c g_cif_basic_inf;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct prdt_ac_id_c g_prdt_ac_id;
	
	struct mdm_attr_c   g_mdm_attr;
	long tmpcs=0,tmpts=0;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0220",loss_type);				/*** ��ʧ������� ***/
	get_zd_data("0300",ac_no);					/*** �ʺ� ***/
	pub_base_old_acno( ac_no );                 /*** �Ծ��ʺŵĴ��� ***/
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%ld]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg," ���Ҵ���ret=[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg," �ý���������ʧ��֤ ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg," �ý�����������֤ ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg," �ý����Ѳ��ỻ֤ ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg," �ý����������� ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						 g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��˻�ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld", \
								g_prdt_ac_id.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸÿͻ���!!cif_no=[%ld]",g_prdt_ac_id.cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg," ���Ҵ���ret=[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Cif_basic_inf_Sel( g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							 g_prdt_ac_id.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸÿͻ���!!cif_no=[%ld]",g_prdt_ac_id.cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg," ���Ҵ���ret=[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		g_reply_int=pub_base_GetParm_int("SGTS",1,&tmpcs);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ��������");
			WRITEMSG
			goto ErrExit;
		}
		
		ret=Mo_loss_Sel(g_pub_tx.reply,&mo_loss,
		"ac_no='%s' and	unloss_ind='0' order by loss_date DESC ,trace_no DESC",
			ac_no );
		if( ret==0 )
		{
			set_zd_data("0240",mo_loss.loss_ind);
			set_zd_data("0230",mo_loss.unloss_ind);
			set_zd_long("0350",tmpcs);
			tmpts=pub_base_CalTrueDays(mo_loss.loss_date,g_pub_tx.tx_date);
			set_zd_long("0360",tmpts);
vtcp_log("tmpcs[%d][%d]",tmpcs,tmpts);
		}
		else
		{
			set_zd_data("0240","#");
			set_zd_data("0230","#");
			set_zd_long("0350",0);
			set_zd_long("0360",0);
vtcp_log("tmpcS[%d][%d]",tmpcs,tmpts);
		}

	}
	if( loss_type[0]!='1' )
	{
		/*
		strcpy( g_pub_tx.reply,"P197" );
		goto ErrExit;
		*/
	}
	/***�õ����   add by wanglei 20061023***/
	if(g_prdt_ac_id.ac_id_type[0] == '1')  /***���ڻ�***/
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%d",g_mdm_ac_rel.ac_id);
		if(ret)
		{
			strcpy(g_pub_tx.reply,"B114");
			sprintf(acErrMsg,"���һ����ʻ���Ϣ����ret=[%ld]",ret);
			WRITEMSG
			goto ErrExit;
		}
		bal = g_dd_mst.bal;
	}
	else if(g_prdt_ac_id.ac_id_type[0] == '2')  /***���ڻ�***/
	{
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%d",g_mdm_ac_rel.ac_id);
		if(ret)
		{
			strcpy(g_pub_tx.reply,"W016");
			sprintf(acErrMsg,"���Ҷ����ʻ���Ϣ����ret=[%ld]",ret);
			WRITEMSG
			goto ErrExit;
		}
		bal = g_td_mst.bal;
	}
	

	/* ������� */
	if( !strncmp(loss_type,"11",2) || !strncmp(loss_type,"13",2) || \
		!strncmp(loss_type,"14",2) )
		strcpy(g_mdm_ac_rel.draw_pwd_yn,"N");

	set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("0250",g_cif_basic_inf.name);
	/**��CZ�������޸ģ���������֪ͨһ��
	set_zd_data("0670",g_cif_id_code_rel.id_type);
	set_zd_data("0620",g_cif_id_code_rel.id_no);
	**/
	set_zd_data("0670",g_mdm_ac_rel.id_type);
	set_zd_data("0620",g_mdm_ac_rel.id_no);
	set_zd_data("0300",ac_no);
	set_zd_double("0410",bal);
	/* 20120213 by Wang Yongwei ��Ӷ����˻���־*/
	char kt_flag[11];
	memset(kt_flag,'0',10);
	kt_flag[0]=g_mdm_ac_rel.draw_pwd_chg[0];
	kt_flag[10]=0x00;
	set_zd_data("0880",kt_flag);/* �Ƿ��ж����ӻ���־ռ��һλ */
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
