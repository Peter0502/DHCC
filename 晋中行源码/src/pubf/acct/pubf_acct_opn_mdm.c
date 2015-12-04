/***************************************************************
* �� �� ��:     pubf_acct_opn_mdm.c
* 				pub_acct_opn_mdm(int mode)
* ����������	�����ʣ������޵��У�û���κν��ʶ���á�
*              	1��ȡ�ͻ���Ϣ��
*              	2�����������ԣ�ƾ֤���ţ�
*              	3���Ǽǽ����˺Ŷ��ձ�
*              	4���Ǽ��˻����ID��
*              	5���Ǽǽ�����ˮ��
* ֤��g_pub_tx.id_type,g_pub_tx.id_no
*
* ��ڲ�����	���ͣ�1-�ͻ���2-�ڲ���
* ��    ��:     xxxxx
* ������ڣ�    2003��2��6��
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
#include "mdm_attr_c.h"
#include "ac_seqn_id_c.h"
#include "dc_acc_rel_c.h"
#include "card_reg_c.h"
#include "card.h"
/*
*  �ͻ��ʿ�����
*/
pub_acct_opn_cif_mdm()
{
	char cif_type[2];
	struct mdm_attr_c	mdm_attr_c;
	struct mdm_ac_rel_c	mdm_ac_rel_c;
	struct mdm_ac_rel_c	v_mdm_ac_rel;
	struct ac_seqn_id_c	ac_seqn_id_c;
	int Z_F_flag=0;	/*** ��������־ ***/
	int ret;

	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&mdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&ac_seqn_id_c,0x00,sizeof(struct ac_seqn_id_c));

	pub_base_strpack(g_pub_tx.mdm_code);
	pub_base_strpack(g_pub_tx.crd_no);
    vtcp_log("wyb-[%f][%f]",g_pub_tx.mdm_code,g_pub_tx.crd_no);
	if( !strlen(g_pub_tx.mdm_code) )
		strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    vtcp_log("[%s][%d][%d]\n",__FILE__,__LINE__,g_pub_tx.cif_no);
	/*** ���ͻ����� ***/
	if( !g_pub_tx.cif_no )
	{
		if( strlen(g_pub_tx.id_type) )
		{
			if( pub_cif_CifCheck(g_pub_tx.id_type,g_pub_tx.id_no,
				&g_pub_tx.cif_no,cif_type) )
			{
				sprintf(acErrMsg,"�ÿͻ��ڿͻ�����δ�Ǽ�![%s][%s]",
					g_pub_tx.id_type,g_pub_tx.id_no);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			g_pub_tx.cif_no=0; 
			strcpy( cif_type,"" );
		}
	}

	/*** Ӧ���ڿ����м�� ***
	if( cif_type[0]=='2' )
	{
		ret=sql_count("dd_mst","cif_no=%ld and ac_type='1'",g_pub_tx.cif_no);
		if( ret )
		{
			sprintf(acErrMsg,"�ÿͻ��Ѵ��ڻ����˻�[%ld][%d]",g_pub_tx.cif_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C082");
			goto ErrExit;
		}
	}
	***********************/

	/*** ��ȡ�������� ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s' ", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ���������쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);

vtcp_log("PQAAA[%s]",g_pub_tx.beg_note_no );TRACE
	sprintf(acErrMsg,"mdm_attr_c.note_type=[%s]",mdm_attr_c.note_type);
	WRITEMSG

	/*��Ҫƾ���ſ������翨�ࣺ����ʱ��Ҫ���뿨��*/
	if( mdm_attr_c.no_ind[0]=='Y' )
	{
		if( !strlen(g_pub_tx.crd_no) )
		{
			sprintf(acErrMsg,"��Ҫ���뿨��");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D111" );
			goto ErrExit;
		}

		sprintf(acErrMsg,"����Ŀ���Ϊ[%s]",g_pub_tx.crd_no);
		WRITEMSG

		/**��鿨���Ƿ��Ѿ�����**/
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&v_mdm_ac_rel,
			"ac_no='%s'",g_pub_tx.crd_no ) ;
		if( ret && ret!=100 ) goto ErrExit;
		else if( !ret )
		{
			sprintf(acErrMsg,"��Ҫ���뿨��[%s]",g_pub_tx.crd_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D113" );
			goto ErrExit;
		}
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no );

		pub_base_strpack(g_pub_tx.beg_note_no);
		if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no) )
		{
			/*** ���ݿ���ȡƾ֤�� ***/
			pub_card_CardToNote(g_pub_tx.crd_no,g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
			vtcp_log("%s,%d PQAAA[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no );
		}
		vtcp_log("%s,%d PAAAA[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no );
		/* add by LiuHuafeng 20070120 ���ӿ��ŵ��������Ϣ */
		{
			struct card_reg_c sCard_reg;
			memset(&sCard_reg , 0 , sizeof(sCard_reg));
			get_zd_data("0740",sCard_reg.msr1);
			get_zd_data("0750",sCard_reg.msr2);
			get_zd_data("0760",sCard_reg.msr3);
			memcpy(sCard_reg.ac_no,g_pub_tx.ac_no,sizeof(sCard_reg.ac_no)-1);
			memcpy(sCard_reg.cvn1,sCard_reg.msr2+30,sizeof(sCard_reg.cvn1)-1);
			sCard_reg.use_flag[0]=CARD_USE_FLAG_ON;
			get_zd_long(DC_DATE,&sCard_reg.opn_date);
			sCard_reg.l_tx_date=sCard_reg.opn_date;
			sCard_reg.tdcnt++;
			sCard_reg.atm_td_dr_cash_tx_amt = 0;
			sCard_reg.atm_tm_dr_cash_tx_amt = 0;
			sCard_reg.atm_ts_dr_cash_tx_amt = 0;
			sCard_reg.atm_ty_dr_cash_tx_amt = 0;
			sCard_reg.atm_td_cr_cash_tx_amt = 0;
			sCard_reg.atm_tm_cr_cash_tx_amt = 0;
			sCard_reg.atm_ts_cr_cash_tx_amt = 0;
			sCard_reg.atm_ty_cr_cash_tx_amt = 0;
			sCard_reg.atm_td_dr_cash_tx_cnt = 0;
			sCard_reg.atm_tm_dr_cash_tx_cnt = 0;
			sCard_reg.atm_ts_dr_cash_tx_cnt = 0;
			sCard_reg.atm_ty_dr_cash_tx_cnt = 0;
			sCard_reg.atm_td_cr_cash_tx_cnt = 0;
			sCard_reg.atm_tm_cr_cash_tx_cnt = 0;
			sCard_reg.atm_ts_cr_cash_tx_cnt = 0;
			sCard_reg.atm_ty_cr_cash_tx_cnt = 0;
			sCard_reg.atm_td_dr_tran_tx_amt = 0;
			sCard_reg.atm_tm_dr_tran_tx_amt = 0;
			sCard_reg.atm_ts_dr_tran_tx_amt = 0;
			sCard_reg.atm_ty_dr_tran_tx_amt = 0;
			sCard_reg.atm_td_cr_tran_tx_amt = 0;
			sCard_reg.atm_tm_cr_tran_tx_amt = 0;
			sCard_reg.atm_ts_cr_tran_tx_amt = 0;
			sCard_reg.atm_ty_cr_tran_tx_amt = 0;
			sCard_reg.atm_td_dr_tran_tx_cnt = 0;
			sCard_reg.atm_tm_dr_tran_tx_cnt = 0;
			sCard_reg.atm_ts_dr_tran_tx_cnt = 0;
			sCard_reg.atm_ty_dr_tran_tx_cnt = 0;
			sCard_reg.atm_td_cr_tran_tx_cnt = 0;
			sCard_reg.atm_tm_cr_tran_tx_cnt = 0;
			sCard_reg.atm_ts_cr_tran_tx_cnt = 0;
			sCard_reg.atm_ty_cr_tran_tx_cnt = 0;
			sCard_reg.atm_td_qy_cnt = 0;
			sCard_reg.atm_tm_qy_cnt = 0;
			sCard_reg.atm_ts_qy_cnt = 0;
			sCard_reg.atm_ty_qy_cnt = 0;
			sCard_reg.pos_td_tx_amt = 0;
			sCard_reg.pos_tm_tx_amt = 0;
			sCard_reg.pos_ts_tx_amt = 0;
			sCard_reg.pos_ty_tx_amt = 0;
			sCard_reg.pos_td_tx_cnt = 0;
			sCard_reg.pos_tm_tx_cnt = 0;
			sCard_reg.pos_ts_tx_cnt = 0;
			sCard_reg.pos_ty_tx_cnt = 0;
			sCard_reg.pos_td_qy_cnt = 0;
			sCard_reg.pos_tm_qy_cnt = 0;
			sCard_reg.pos_ts_qy_cnt = 0;
			sCard_reg.pos_ty_qy_cnt = 0;
			sCard_reg.wh_atm_td_dr_cash_tx_amt = 0;
			sCard_reg.wh_atm_tm_dr_cash_tx_amt = 0;
			sCard_reg.wh_atm_ts_dr_cash_tx_amt = 0;
			sCard_reg.wh_atm_ty_dr_cash_tx_amt = 0;
			sCard_reg.wh_atm_td_dr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_tm_dr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_ts_dr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_ty_dr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_td_dr_tran_tx_amt = 0;
			sCard_reg.wh_atm_tm_dr_tran_tx_amt = 0;
			sCard_reg.wh_atm_ts_dr_tran_tx_amt = 0;
			sCard_reg.wh_atm_ty_dr_tran_tx_amt = 0;
			sCard_reg.wh_atm_td_dr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_tm_dr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_ts_dr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_ty_dr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_td_cr_cash_tx_amt = 0;
			sCard_reg.wh_atm_tm_cr_cash_tx_amt = 0;
			sCard_reg.wh_atm_ts_cr_cash_tx_amt = 0;
			sCard_reg.wh_atm_ty_cr_cash_tx_amt = 0;
			sCard_reg.wh_atm_td_cr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_tm_cr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_ts_cr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_ty_cr_cash_tx_cnt = 0;
			sCard_reg.wh_atm_td_cr_tran_tx_amt = 0;
			sCard_reg.wh_atm_tm_cr_tran_tx_amt = 0;
			sCard_reg.wh_atm_ts_cr_tran_tx_amt = 0;
			sCard_reg.wh_atm_ty_cr_tran_tx_amt = 0;
			sCard_reg.wh_atm_td_cr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_tm_cr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_ts_cr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_ty_cr_tran_tx_cnt = 0;
			sCard_reg.wh_atm_td_qy_cnt = 0;
			sCard_reg.wh_atm_tm_qy_cnt = 0;
			sCard_reg.wh_atm_ts_qy_cnt = 0;
			sCard_reg.wh_atm_ty_qy_cnt = 0;
			sCard_reg.wh_pos_td_tx_amt = 0;
			sCard_reg.wh_pos_tm_tx_amt = 0;
			sCard_reg.wh_pos_ts_tx_amt = 0;
			sCard_reg.wh_pos_ty_tx_amt = 0;
			sCard_reg.wh_pos_td_tx_cnt = 0;
			sCard_reg.wh_pos_tm_tx_cnt = 0;
			sCard_reg.wh_pos_ts_tx_cnt = 0;
			sCard_reg.wh_pos_ty_tx_cnt = 0;
			sCard_reg.wh_pos_td_qy_cnt = 0;
			sCard_reg.wh_pos_tm_qy_cnt = 0;
			sCard_reg.wh_pos_ts_qy_cnt = 0;
			sCard_reg.wh_pos_ty_qy_cnt = 0;
			sCard_reg.atm_max_dr_cash_tx_amt =CARD_ATM_PER_DAY_MAX_TX_AMT;
			sCard_reg.atm_max_dr_cash_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
			sCard_reg.atm_max_dr_tran_tx_amt =CARD_ATM_PER_DAY_TRAN_MAX_TX_AMT;
			sCard_reg.atm_max_dr_tran_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
			sCard_reg.atm_max_cr_cash_tx_amt =CARD_ATM_PER_DAY_MAX_TX_AMT;
			sCard_reg.atm_max_cr_cash_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
			sCard_reg.atm_max_cr_tran_tx_amt =CARD_ATM_PER_DAY_TRAN_MAX_TX_AMT;
			sCard_reg.atm_max_cr_tran_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
			sCard_reg.wh_atm_tx_free_cnt = CARD_ATM_TX_FREE_CNT;
			sCard_reg.wh_atm_qy_free_cnt = CARD_ATM_QY_FREE_CNT;
			sCard_reg.wh_pos_tx_free_cnt = CARD_POS_TX_FREE_CNT;
			sCard_reg.wh_pos_qy_free_cnt = CARD_POS_QY_FREE_CNT;
			Card_reg_Debug(sCard_reg);
			ret=Card_reg_Ins(sCard_reg ,g_pub_tx.reply );
			if(ret)
			{
				sprintf(acErrMsg,"д���Ǽǲ�����[%s]",g_pub_tx.crd_no);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D113" );
				goto ErrExit;
			}
		}
	}
	else
	{
		/*** ������ʾ�˺� ***/
		ret=pub_base_CrtCifAc(g_pub_tx.cif_no,g_pub_tx.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"���ɿͻ���ʾ�˺Ŵ���!cif_no=[%ld]", \
					g_pub_tx.cif_no);
			WRITEMSG
			goto ErrExit;
		}
	}

	/* ��Ҫƾ֤���� */ 
	if( mdm_attr_c.only_ind[0]=='Y' && g_pub_tx.ac_wrk_ind[0]!='0' ) 
	{
		ret=pub_com_NoteUse(0,0,g_pub_tx.note_type,
			g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"��Աʹ��ƾ֤����!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*�����˻�ID*/
	if(g_pub_tx.ac_id==0)
	{
		ret=pub_base_crt_cltid(&g_pub_tx.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"���ɿͻ��˺�ID����");
			WRITEMSG
			goto ErrExit;
		}
		Z_F_flag=1;
	}

	/* ��Ҫ����Ƿ�������� */
	if( Z_F_flag==0 )
	{
		if( pub_base_Exist_MainCrd(g_pub_tx.ac_id,&Z_F_flag) )
		{
			sprintf(acErrMsg,"����Ƿ������������");
			WRITEMSG
			goto ErrExit;
		}
	}

	/**���Ż�����**/
	if( mdm_attr_c.ac_num_ind[0]=='Y' )
	{
		g_pub_tx.ac_seqn=ZERO_SEQN;
	}
	else
	{
		g_pub_tx.ac_seqn=ALL_SEQN;
	}

	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("mdm_code=[%s],cur=[%s]",g_pub_tx.mdm_code,g_pub_tx.cur_no);

	/**�趨֧ȡ��ʽ**/
	strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(mdm_ac_rel_c.draw_uncon_yn,g_pub_tx.draw_uncon_yn);
	strcpy(mdm_ac_rel_c.draw_pwd_yn,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.qry_pwd);
	strcpy(mdm_ac_rel_c.qry_pwd,g_pub_tx.qry_pwd);
	pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd);
	strcpy(mdm_ac_rel_c.draw_pwd,g_pub_tx.draw_pwd);
	strcpy(mdm_ac_rel_c.draw_id_yn,g_pub_tx.draw_id_yn);
vtcp_log("MDM_CODE DDDD =[%s]",g_pub_tx.id_type);
	strcpy(mdm_ac_rel_c.id_type,g_pub_tx.id_type);
	strcpy(mdm_ac_rel_c.id_no,g_pub_tx.id_no);
	strcpy(mdm_ac_rel_c.draw_seal_yn,g_pub_tx.draw_seal_yn);
	strcpy(mdm_ac_rel_c.pwd_mach_yn,"N");
	if( pub_base_mdm_draw(mdm_attr_c,mdm_ac_rel_c) )
	{
		sprintf(acErrMsg,"֧ȡ��ʽ�����Ͻ��ʹ涨");
		WRITEMSG
		goto ErrExit;
	}

	/*** �Ǽǽ����˺Ź�ϵ ***/
	strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no);
	/**add by zyl 20110427 �����������˻�.�������˻��Ŀ���������.**/
	if(strcmp(g_pub_tx.tx_code,"0008") == 0)
	{
		strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.opn_br_no);
	}
	/**end by zyl 20110427**/
	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
   	sprintf(acErrMsg,"!!!!!!!!ac_no=%s", mdm_ac_rel_c.ac_no);
   	WRITEMSG

	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(mdm_ac_rel_c.mdm_code,g_pub_tx.mdm_code);
	strcpy(mdm_ac_rel_c.note_no,g_pub_tx.beg_note_no);
	strcpy(mdm_ac_rel_c.note_sts,"0");
	strcpy(mdm_ac_rel_c.coll_sts,"0");
	mdm_ac_rel_c.beg_date=g_pub_tx.tx_date;
	mdm_ac_rel_c.end_date=99999999;
	mdm_ac_rel_c.book_bal=0.00;
	if(!strncmp(mdm_attr_c.mdm_code,"0016",4))			/***����һ��ͨ***/
		mdm_ac_rel_c.prt_line=0;
	else
		mdm_ac_rel_c.prt_line=1;
	if(Z_F_flag==1) strcpy(mdm_ac_rel_c.main_ind,"1");
	else if(Z_F_flag==0) strcpy(mdm_ac_rel_c.main_ind,"2");

	g_reply_int=Mdm_ac_rel_Ins(mdm_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�Ǽǽ����˺Ŷ��ձ����![%ld][%d][%d]", \
				g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}
	
	/*** �Ǽ��˻����ID�� ***/
	if( Z_F_flag && g_pub_tx.ac_seqn==ALL_SEQN )
	{
		ac_seqn_id_c.ac_id=g_pub_tx.ac_id;	
		ac_seqn_id_c.ac_seqn_id=BEG_SEQN;
		ret=Ac_seqn_id_Ins(ac_seqn_id_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽ��˻����ID!id[%d]seq[%d]ret[%d]",
				ac_seqn_id_c.ac_id,ac_seqn_id_c.ac_seqn_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P139");
			goto ErrExit;
		}
	}
	
	/********del by xxxxx 20030620 **************
	*** �Ǽǽ�����ˮ ***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	********************del end****************/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
/*
*  �ڲ��ʿ�����
*/
pub_acct_opn_in_mdm()
{
	char cif_type[2];
	struct mdm_attr_c	mdm_attr_c;
	struct mdm_ac_rel_c	mdm_ac_rel_c;
	struct ac_seqn_id_c	ac_seqn_id_c;
	int ret;

	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&mdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&ac_seqn_id_c,0x00,sizeof(struct ac_seqn_id_c));

	/* ��ѯ�ڲ���Ʒ���� */
	ret =In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
		g_pub_tx.prdt_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ�ڲ���Ʒ��������!![%d][%s]",
				ret,g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W035");
		goto ErrExit;
	}

	/**�ڲ����޽���**/
	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);

	/*** ������ʾ�˺� ***/
	ret=pub_base_CrtInAc(g_in_parm.acc_hrt,g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,
			"�����ڲ���ʾ�˺Ŵ���!g_in_parm.acc_hrt=[%s],g_pub_tx.ac_no=[%s]",
			g_in_parm.acc_hrt,g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	ret=pub_base_get_inid(&g_pub_tx.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڲ��˺�ID����");
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=ZERO_SEQN;

	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("mdm_code=[%s],cur=[%s]",g_pub_tx.mdm_code,g_pub_tx.cur_no);

		/**�趨֧ȡ��ʽ**/
		strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.opn_br_no);
		strcpy(mdm_ac_rel_c.draw_uncon_yn,"Y");
		strcpy(mdm_ac_rel_c.draw_pwd_yn,"N");
		pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.qry_pwd);
		strcpy(mdm_ac_rel_c.qry_pwd,g_pub_tx.qry_pwd);
		pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd);
		strcpy(mdm_ac_rel_c.draw_pwd,g_pub_tx.draw_pwd);
		strcpy(mdm_ac_rel_c.draw_id_yn,g_pub_tx.draw_id_yn);
		strcpy(mdm_ac_rel_c.id_type,g_pub_tx.id_type);
		strcpy(mdm_ac_rel_c.id_no,g_pub_tx.id_no);
		strcpy(mdm_ac_rel_c.draw_seal_yn,g_pub_tx.draw_seal_yn);
		strcpy(mdm_ac_rel_c.pwd_mach_yn,"N");
		if( pub_base_mdm_draw(mdm_attr_c,mdm_ac_rel_c) )
		{
			sprintf(acErrMsg,"֧ȡ��ʽ�����Ͻ��ʹ涨");
			WRITEMSG
			goto ErrExit;
		}

	/*** �Ǽǽ����˺Ź�ϵ ***/
	strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no);
	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
vtcp_log("================%s",mdm_ac_rel_c.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(mdm_ac_rel_c.mdm_code,g_pub_tx.mdm_code);
	strcpy(mdm_ac_rel_c.note_no,g_pub_tx.beg_note_no);
	strcpy(mdm_ac_rel_c.note_sts,"0");
	strcpy(mdm_ac_rel_c.coll_sts,"0");
	mdm_ac_rel_c.beg_date=g_pub_tx.tx_date;
	mdm_ac_rel_c.end_date=99999999;
	mdm_ac_rel_c.book_bal=0.00;
	mdm_ac_rel_c.prt_line=0;
	g_reply_int=Mdm_ac_rel_Ins(mdm_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�Ǽǽ����˺Ŷ��ձ����![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** �Ǽ��˻����ID�� ***/
	if( g_pub_tx.ac_seqn==ALL_SEQN )
	{
		ac_seqn_id_c.ac_id=g_pub_tx.ac_id;	
		ac_seqn_id_c.ac_seqn_id=BEG_SEQN;
		ret=Ac_seqn_id_Ins(ac_seqn_id_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽ��˻����ID!id[%d]seq[%d]ret[%d]",
				ac_seqn_id_c.ac_id,ac_seqn_id_c.ac_seqn_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P139");
			goto ErrExit;
		}
	}
	
	/*** �Ǽǽ�����ˮ ***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	********************/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

/*
*  �����˺ſ�����
*/
pub_acct_opn_discnt_mdm()
{
	char cif_type[2];
	struct mdm_attr_c	mdm_attr_c;
	struct mdm_ac_rel_c	mdm_ac_rel_c;
	struct ac_seqn_id_c	ac_seqn_id_c;
	struct dc_acc_rel_c	dc_acc_rel_c;
	int ret;

	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&mdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&ac_seqn_id_c,0x00,sizeof(struct ac_seqn_id_c));
	memset(&dc_acc_rel_c,0x00,sizeof(struct dc_acc_rel_c));

	/* ��ѯ�����Ʒ���� */
	ret =Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",
		g_pub_tx.prdt_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ�����Ʒ��������!![%d][%s]",
				ret,g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L153");
		goto ErrExit;
	}

	/**�����˻��޽���**/
	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);

	/* ��ѯ��ƴ����Ӧ��Ŀ */
	ret =Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel_c,"dc_code='%s'",
		g_ln_parm.dc_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ��ƴ����Ӧ��Ŀ����!![%d][%s]",
				ret,g_ln_parm.dc_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L154");
		goto ErrExit;
	}

	/*** ������ʾ�˺� ***/
	ret=pub_base_CrtDiscntAc(dc_acc_rel_c.acc_hrt,g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,
			"���������˺Ŵ���!dc_acc_rel_c.acc_hrt=[%s],g_pub_tx.ac_no=[%s]",
			dc_acc_rel_c.acc_hrt,g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	ret=pub_base_get_cltid(&g_pub_tx.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"���������˺�ID����");
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=0;

	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("mdm_code=[%s],cur=[%s]",g_pub_tx.mdm_code,g_pub_tx.cur_no);

		/**�趨֧ȡ��ʽ**/
		strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no);
		strcpy(mdm_ac_rel_c.draw_uncon_yn,"N");
		strcpy(mdm_ac_rel_c.draw_pwd_yn,"N");
		strcpy(mdm_ac_rel_c.draw_id_yn,"N");
		strcpy(mdm_ac_rel_c.draw_seal_yn,"N");
		strcpy(mdm_ac_rel_c.pwd_mach_yn,"N");
		if( pub_base_mdm_draw(mdm_attr_c,mdm_ac_rel_c) )
		{
			sprintf(acErrMsg,"֧ȡ��ʽ�����Ͻ��ʹ涨");
			WRITEMSG
			goto ErrExit;
		}

	/*** �Ǽǽ����˺Ź�ϵ ***/
	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(mdm_ac_rel_c.mdm_code,g_pub_tx.mdm_code);
	strcpy(mdm_ac_rel_c.note_no,"");
	strcpy(mdm_ac_rel_c.note_sts,"0");
	strcpy(mdm_ac_rel_c.coll_sts,"0");
	mdm_ac_rel_c.beg_date=g_pub_tx.tx_date;
	mdm_ac_rel_c.end_date=99999999;
	mdm_ac_rel_c.book_bal=0.00;
	mdm_ac_rel_c.prt_line=0;
	g_reply_int=Mdm_ac_rel_Ins(mdm_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�Ǽǽ����˺Ŷ��ձ����![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** �Ǽǽ�����ˮ ***
	****---- xxx ---- 20030818 ---- ֻ�Ǽǿ�������ˮ�Ϳ����� ----***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	****-----------------------------------------------------------***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
