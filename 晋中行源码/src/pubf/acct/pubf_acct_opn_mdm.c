/***************************************************************
* 文 件 名:     pubf_acct_opn_mdm.c
* 				pub_acct_opn_mdm(int mode)
* 功能描述：	开介质，即从无到有，没有任何介质而获得。
*              	1、取客户信息；
*              	2、检查介质属性，凭证销号；
*              	3、登记介质账号对照表；
*              	4、登记账户序号ID表；
*              	5、登记交易流水；
* 证件g_pub_tx.id_type,g_pub_tx.id_no
*
* 入口参数：	类型（1-客户；2-内部）
* 作    者:     xxxxx
* 完成日期：    2003年2月6日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
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
*  客户帐开介质
*/
pub_acct_opn_cif_mdm()
{
	char cif_type[2];
	struct mdm_attr_c	mdm_attr_c;
	struct mdm_ac_rel_c	mdm_ac_rel_c;
	struct mdm_ac_rel_c	v_mdm_ac_rel;
	struct ac_seqn_id_c	ac_seqn_id_c;
	int Z_F_flag=0;	/*** 主负卡标志 ***/
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
	/*** 检查客户资料 ***/
	if( !g_pub_tx.cif_no )
	{
		if( strlen(g_pub_tx.id_type) )
		{
			if( pub_cif_CifCheck(g_pub_tx.id_type,g_pub_tx.id_no,
				&g_pub_tx.cif_no,cif_type) )
			{
				sprintf(acErrMsg,"该客户在客户中心未登记![%s][%s]",
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

	/*** 应该在开户中检查 ***
	if( cif_type[0]=='2' )
	{
		ret=sql_count("dd_mst","cif_no=%ld and ac_type='1'",g_pub_tx.cif_no);
		if( ret )
		{
			sprintf(acErrMsg,"该客户已存在基本账户[%ld][%d]",g_pub_tx.cif_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C082");
			goto ErrExit;
		}
	}
	***********************/

	/*** 读取介质属性 ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s' ", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该介质属性不存在![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取介质属性异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);

vtcp_log("PQAAA[%s]",g_pub_tx.beg_note_no );TRACE
	sprintf(acErrMsg,"mdm_attr_c.note_type=[%s]",mdm_attr_c.note_type);
	WRITEMSG

	/*需要凭卡号开户，如卡类：开户时需要输入卡号*/
	if( mdm_attr_c.no_ind[0]=='Y' )
	{
		if( !strlen(g_pub_tx.crd_no) )
		{
			sprintf(acErrMsg,"需要输入卡号");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D111" );
			goto ErrExit;
		}

		sprintf(acErrMsg,"输入的卡号为[%s]",g_pub_tx.crd_no);
		WRITEMSG

		/**检查卡号是否已经开过**/
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&v_mdm_ac_rel,
			"ac_no='%s'",g_pub_tx.crd_no ) ;
		if( ret && ret!=100 ) goto ErrExit;
		else if( !ret )
		{
			sprintf(acErrMsg,"需要输入卡号[%s]",g_pub_tx.crd_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D113" );
			goto ErrExit;
		}
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no );

		pub_base_strpack(g_pub_tx.beg_note_no);
		if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no) )
		{
			/*** 根据卡号取凭证号 ***/
			pub_card_CardToNote(g_pub_tx.crd_no,g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
			vtcp_log("%s,%d PQAAA[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no );
		}
		vtcp_log("%s,%d PAAAA[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no );
		/* add by LiuHuafeng 20070120 增加卡磁道等相关信息 */
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
				sprintf(acErrMsg,"写卡登记簿错误[%s]",g_pub_tx.crd_no);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D113" );
				goto ErrExit;
			}
		}
	}
	else
	{
		/*** 生成显示账号 ***/
		ret=pub_base_CrtCifAc(g_pub_tx.cif_no,g_pub_tx.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"生成客户显示账号错误!cif_no=[%ld]", \
					g_pub_tx.cif_no);
			WRITEMSG
			goto ErrExit;
		}
	}

	/* 需要凭证销号 */ 
	if( mdm_attr_c.only_ind[0]=='Y' && g_pub_tx.ac_wrk_ind[0]!='0' ) 
	{
		ret=pub_com_NoteUse(0,0,g_pub_tx.note_type,
			g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"柜员使用凭证错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*生成账户ID*/
	if(g_pub_tx.ac_id==0)
	{
		ret=pub_base_crt_cltid(&g_pub_tx.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"生成客户账号ID错误");
			WRITEMSG
			goto ErrExit;
		}
		Z_F_flag=1;
	}

	/* 需要检查是否存在主卡 */
	if( Z_F_flag==0 )
	{
		if( pub_base_Exist_MainCrd(g_pub_tx.ac_id,&Z_F_flag) )
		{
			sprintf(acErrMsg,"检查是否存在主卡错误");
			WRITEMSG
			goto ErrExit;
		}
	}

	/**单张户介质**/
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

	/**设定支取方式**/
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
		sprintf(acErrMsg,"支取方式不符合介质规定");
		WRITEMSG
		goto ErrExit;
	}

	/*** 登记介质账号关系 ***/
	strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no);
	/**add by zyl 20110427 七天乐虚拟账户.开在主账户的开户机构上.**/
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
	if(!strncmp(mdm_attr_c.mdm_code,"0016",4))			/***定期一本通***/
		mdm_ac_rel_c.prt_line=0;
	else
		mdm_ac_rel_c.prt_line=1;
	if(Z_F_flag==1) strcpy(mdm_ac_rel_c.main_ind,"1");
	else if(Z_F_flag==0) strcpy(mdm_ac_rel_c.main_ind,"2");

	g_reply_int=Mdm_ac_rel_Ins(mdm_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记介质账号对照表错误![%ld][%d][%d]", \
				g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}
	
	/*** 登记账户序号ID表 ***/
	if( Z_F_flag && g_pub_tx.ac_seqn==ALL_SEQN )
	{
		ac_seqn_id_c.ac_id=g_pub_tx.ac_id;	
		ac_seqn_id_c.ac_seqn_id=BEG_SEQN;
		ret=Ac_seqn_id_Ins(ac_seqn_id_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记账户序号ID!id[%d]seq[%d]ret[%d]",
				ac_seqn_id_c.ac_id,ac_seqn_id_c.ac_seqn_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P139");
			goto ErrExit;
		}
	}
	
	/********del by xxxxx 20030620 **************
	*** 登记交易流水 ***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	********************del end****************/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开介质成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"开介质失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
/*
*  内部帐开介质
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

	/* 查询内部产品参数 */
	ret =In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
		g_pub_tx.prdt_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询内部产品参数错误!![%d][%s]",
				ret,g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W035");
		goto ErrExit;
	}

	/**内部帐无介质**/
	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);

	/*** 生成显示账号 ***/
	ret=pub_base_CrtInAc(g_in_parm.acc_hrt,g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,
			"生成内部显示账号错误!g_in_parm.acc_hrt=[%s],g_pub_tx.ac_no=[%s]",
			g_in_parm.acc_hrt,g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	ret=pub_base_get_inid(&g_pub_tx.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"生成内部账号ID错误");
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=ZERO_SEQN;

	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("mdm_code=[%s],cur=[%s]",g_pub_tx.mdm_code,g_pub_tx.cur_no);

		/**设定支取方式**/
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
			sprintf(acErrMsg,"支取方式不符合介质规定");
			WRITEMSG
			goto ErrExit;
		}

	/*** 登记介质账号关系 ***/
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
		sprintf(acErrMsg,"登记介质账号对照表错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** 登记账户序号ID表 ***/
	if( g_pub_tx.ac_seqn==ALL_SEQN )
	{
		ac_seqn_id_c.ac_id=g_pub_tx.ac_id;	
		ac_seqn_id_c.ac_seqn_id=BEG_SEQN;
		ret=Ac_seqn_id_Ins(ac_seqn_id_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记账户序号ID!id[%d]seq[%d]ret[%d]",
				ac_seqn_id_c.ac_id,ac_seqn_id_c.ac_seqn_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P139");
			goto ErrExit;
		}
	}
	
	/*** 登记交易流水 ***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	********************/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开介质成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"开介质失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

/*
*  贴现账号开介质
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

	/* 查询贷款产品参数 */
	ret =Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",
		g_pub_tx.prdt_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询贷款产品参数错误!![%d][%s]",
				ret,g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L153");
		goto ErrExit;
	}

	/**贴现账户无介质**/
	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);

	/* 查询会计代码对应科目 */
	ret =Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel_c,"dc_code='%s'",
		g_ln_parm.dc_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询会计代码对应科目错误!![%d][%s]",
				ret,g_ln_parm.dc_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L154");
		goto ErrExit;
	}

	/*** 生成显示账号 ***/
	ret=pub_base_CrtDiscntAc(dc_acc_rel_c.acc_hrt,g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,
			"生成贴现账号错误!dc_acc_rel_c.acc_hrt=[%s],g_pub_tx.ac_no=[%s]",
			dc_acc_rel_c.acc_hrt,g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	ret=pub_base_get_cltid(&g_pub_tx.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"生成贴现账号ID错误");
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=0;

	strcpy(mdm_ac_rel_c.ac_no,g_pub_tx.ac_no);
	mdm_ac_rel_c.ac_id=g_pub_tx.ac_id;
	mdm_ac_rel_c.ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("mdm_code=[%s],cur=[%s]",g_pub_tx.mdm_code,g_pub_tx.cur_no);

		/**设定支取方式**/
		strcpy(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no);
		strcpy(mdm_ac_rel_c.draw_uncon_yn,"N");
		strcpy(mdm_ac_rel_c.draw_pwd_yn,"N");
		strcpy(mdm_ac_rel_c.draw_id_yn,"N");
		strcpy(mdm_ac_rel_c.draw_seal_yn,"N");
		strcpy(mdm_ac_rel_c.pwd_mach_yn,"N");
		if( pub_base_mdm_draw(mdm_attr_c,mdm_ac_rel_c) )
		{
			sprintf(acErrMsg,"支取方式不符合介质规定");
			WRITEMSG
			goto ErrExit;
		}

	/*** 登记介质账号关系 ***/
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
		sprintf(acErrMsg,"登记介质账号对照表错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** 登记交易流水 ***
	****---- xxx ---- 20030818 ---- 只登记开户的流水就可以了 ----***
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	****-----------------------------------------------------------***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开介质成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"开介质失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
