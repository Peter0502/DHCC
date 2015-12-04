/***************************************************************
* 文 件 名:     pubf_acct_chg_mdm.c
* 功能描述：
*              1、检查账号、账号ID和账号序号的合法性；
*              2、登记换证登记薄；
*              3、登记交易流水；
*
* 入口参数：   mode 换证类型：1-正常换证；2-书挂换证；3-书挂兼密挂换证
*                             10-部提换证；
*
* 作    者:	jack
* 完成日期：    2003年01月23日
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
#include "mo_loss_c.h"
#include "trace_log_c.h"
#include "mdm_attr_c.h"
#include "mo_rpl_note_c.h"
#include "mo_opn_cls_c.h"
#include "cif_email_inf_c.h"
#include "mdm_unprt_hst_c.h"
#include "dd_mst_hst_c.h"
#include "prdt_ac_id_c.h"
/*add by ChengGX 晋中增加对卡的处理2010-1-30 19:09*/
#include "card.h"
#include "mdm_attr_c.h"
#include "card_reg_c.h"
/*add by ChengGX 晋中增加对卡的处理2010-1-30 19:09*/

struct mo_rpl_note_c mo_rpl_note_c;
struct mo_opn_cls_c g_mo_opn_cls;
struct mdm_attr_c mdm_attr_c;
struct mdm_ac_rel_c v_mdm_ac_rel;
struct dd_mst_hst_c  g_dd_mst_hst;
struct prdt_ac_id_c s_prdt_ac_id;

int ret=0;

pub_acct_chg_mdm(int mode)
{
    char term[3];
    char term_type[3];
	char tmpname[51];
	char cur_name[21];
	char draw_type[21];
	char deac_type[11];
	char br_name[31];
	char br_tele[21];
	char auto_depo[15];
	char comm[101];
	char tmp_ac_no[20],tmp_tx_name[21];
	/***未对一本通、卡作处理 长治的零整存折与活期相同 by wanglei 20061015***/
	char acc_no[8];/***交易的凭证返回科目号 by wanglei 20061015***/
	int prtl,prt_cnt;  
	FILE *fp;
	double rate_val,tmp_amt;
	long   tmp_date;
	struct mdm_unprt_hst_c  mdm_unprt_hst_c;
	struct cif_email_inf_c  g_cif_email_inf;
	struct S_pub_intst		sTmp_pub_intst;
	double chg_book_bal=0.00;

	memset(&g_dd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	memset(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&v_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&mo_rpl_note_c,0x00,sizeof(struct mo_rpl_note_c));
	memset(&g_mo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));
	memset(&s_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));	
	g_reply_int=0;
	memset(&sTmp_pub_intst,0x00,sizeof(struct S_pub_intst));
	memset(acc_no,0x00,sizeof(acc_no));

	/**** add by xxxxx using for print 更换申请 ****/
	strcpy( tmp_ac_no , g_pub_tx.ac_no );
	strcpy( tmp_tx_name , "更换" );
	
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
							   g_pub_tx.ac_no);	
	if( g_reply_int == 100 )
	{
		sprintf(acErrMsg,"账号不存在[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg, "查询介质账号对照表错误!! [%d]", g_reply_int);
		WRITEMSG
		return g_reply_int;
	}

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
	pub_base_strpack(g_pub_tx.opn_br_no);
	pub_base_strpack(g_mdm_ac_rel.id_type);
	pub_base_strpack(g_mdm_ac_rel.id_no);
	strcpy(mo_rpl_note_c.old_ac_no,g_pub_tx.ac_no);
	strcpy(mo_rpl_note_c.old_note_no,g_mdm_ac_rel.note_no);
	pub_base_strpack(g_mdm_ac_rel.mdm_code);
	strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

	if( (mode==2 || mode==3) && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
	{
		sprintf(acErrMsg,"挂失换证请到开户所办理![%s][%s]", \
				g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P195");
		return 1;
	}

	if( !strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
	{
		sprintf(acErrMsg,"该账号没有介质不能作换证![%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P193");
		return 1;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*' 
		|| g_mdm_ac_rel.note_sts[0]=='2' || g_mdm_ac_rel.note_sts[0]=='3' 
		|| g_mdm_ac_rel.note_sts[0]=='4' || g_mdm_ac_rel.note_sts[0]=='5' 
		)
	{
		sprintf(acErrMsg,"账号已经销户![%s]",g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		return 1;
	}
	if( mode==1 && g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"账号已经LOSS![%s]",g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		return 1;
	}

	g_reply_int=Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr_c,"mdm_code='%s'", \
							 g_pub_tx.mdm_code);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"取介质属性错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		return 1;
	}

	pub_base_strpack(mdm_attr_c.note_type);
	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);

/*add by ChengGX 晋中更换介质是卡的处理 2010-1-30 19:13*/
    {
		vtcp_log("%s,%d,[%s]挂失换证是卡的[%s],介质代码[%s]",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.crd_no,g_mdm_ac_rel.mdm_code);
		struct mdm_attr_c sMdm_attr;
		memset(&sMdm_attr,0,sizeof(sMdm_attr));
		int ret =0;	
		ret = Mdm_attr_Sel(g_pub_tx.reply,&sMdm_attr,"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
		if(ret)
		{
			vtcp_log("%s,%d,该介质属性不存在!",__FILE__,__LINE__,g_mdm_ac_rel.mdm_code);
			strcpy(g_pub_tx.reply,"P132");
			return 1;
		}
		if((sMdm_attr.ac_num_ind[0]=='N' && sMdm_attr.mdm_type[0]=='3') || !memcmp(g_mdm_ac_rel.mdm_code,"0020",4) || !memcmp(g_mdm_ac_rel.mdm_code,"0024",4)) /*0024 金融IC卡*/
		{

			/* add by LiuHuafeng 20070120 增加卡磁道等相关信息 */
			{
				struct card_reg_c sCard_reg;
				struct card_reg_c sCard_reg1;
				memset(&sCard_reg , 0 , sizeof(sCard_reg));
				memset(&sCard_reg1, 0 , sizeof(sCard_reg1));
				get_zd_data("0740",sCard_reg.msr1);
				get_zd_data("0750",sCard_reg.msr2);
				get_zd_data("0760",sCard_reg.msr3);
				memcpy(sCard_reg.ac_no,g_pub_tx.crd_no,sizeof(sCard_reg.ac_no)-1);
				memcpy(sCard_reg.cvn1,sCard_reg.msr2+32,sizeof(sCard_reg.cvn1)-1);
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
				sCard_reg.atm_max_dr_tran_tx_amt =CARD_ATM_PER_DAY_MAX_TX_AMT;
				sCard_reg.atm_max_dr_tran_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
				sCard_reg.atm_max_cr_cash_tx_amt =CARD_ATM_PER_DAY_MAX_TX_AMT;
				sCard_reg.atm_max_cr_cash_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
				sCard_reg.atm_max_cr_tran_tx_amt =CARD_ATM_PER_DAY_MAX_TX_AMT;
				sCard_reg.atm_max_cr_tran_tx_cnt =CARD_ATM_PER_DAY_MAX_TX_CNT;
				sCard_reg.wh_atm_tx_free_cnt = CARD_ATM_TX_FREE_CNT;
				sCard_reg.wh_atm_qy_free_cnt = CARD_ATM_QY_FREE_CNT;
				sCard_reg.wh_pos_tx_free_cnt = CARD_POS_TX_FREE_CNT;
				sCard_reg.wh_pos_qy_free_cnt = CARD_POS_QY_FREE_CNT;
				
				/**xyy 2009-5-15 13:39:59 把原卡的filler1 - POS消费次数 filler2 年费扣除日期 也从原卡户上跟新过来**/
				ret = Card_reg_Sel(g_pub_tx.reply,&sCard_reg1,"ac_no='%s' ",g_pub_tx.ac_no);
				if(ret)
				{
					vtcp_log("%s,%d,卡号不存在",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"CU14");
					return 1;
				}
				memcpy(sCard_reg.card_lx,sCard_reg1.card_lx,sizeof(sCard_reg.card_lx)-1);
				memcpy(sCard_reg.card_zl,sCard_reg1.card_zl,sizeof(sCard_reg.card_zl)-1);
				memcpy(sCard_reg.filler1,sCard_reg1.filler1,sizeof(sCard_reg.filler1)-1);
				memcpy(sCard_reg.filler2,sCard_reg1.filler2,sizeof(sCard_reg.filler2)-1);
				
				ret=Card_reg_Ins(sCard_reg ,g_pub_tx.reply );
				if (ret)
				{
					sprintf(acErrMsg,"写卡登记簿错误[%s]",g_pub_tx.crd_no);
					WRITEMSG
					strcpy( g_pub_tx.reply,"D113" );
					return 1;
				}
			}
		}
	}
    /*add by ChengGX 晋中更换介质是卡的处理 2010-1-30 19:14*/

	if( mode==1 )
	{
		if( pub_chg_mdm() )
		{
			sprintf(acErrMsg,"正常换证函数错误!");
			WRITEMSG
			return 1;
		}
	}
	else 
	{
		if( pub_loss_chg_mdm(mode) )
		{
			sprintf(acErrMsg,"挂失换证函数错误!");
			WRITEMSG
			return 1;
		}
	}

	/*** Add by xxx for Print ***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
	   	sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		return 1;
	}

	struct prdt_ac_id_c tmp_prdt_ac_id;
	memset(&tmp_prdt_ac_id,'\0',sizeof(tmp_prdt_ac_id));
	/*根据定期一本通的要求进行更改*/
	
	if(mdm_attr_c.ac_num_ind[0]=='Y') /**mod by hxc 091020**/
	{
	sprintf(acErrMsg,"[%s][%d]mdm_attr_c.ac_num_ind[%s]",__FILE__,__LINE__,mdm_attr_c.ac_num_ind);
		g_reply_int = Prdt_ac_id_Sel(g_pub_tx.reply,&tmp_prdt_ac_id,\
					 "ac_id=%ld and ac_seqn=%d ", \
					 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( g_reply_int )
	{
  		sprintf(acErrMsg,"[%s][%d]读产品帐号对照表出错[%d],ac_id[%ld]错",__FILE__,__LINE__,g_reply_int,g_mdm_ac_rel.ac_id);
		WRITEMSG
		return 1;
	}
	}
	else{
		sprintf(acErrMsg,"[%s][%d]mdm_attr_c.ac_num_ind[%s]",__FILE__,__LINE__,mdm_attr_c.ac_num_ind);
		g_reply_int = Prdt_ac_id_Sel(g_pub_tx.reply,&tmp_prdt_ac_id, "ac_id=%ld ",g_mdm_ac_rel.ac_id);
    if( g_reply_int )
    {
        sprintf(acErrMsg,"[%s][%d]读产品帐号对照表出错[%d],ac_id[%ld]错",__FILE__,__LINE__,g_reply_int,g_mdm_ac_rel.ac_id);
        WRITEMSG
        return 1;
	}
	}
	strcpy(g_pub_tx.prdt_code,tmp_prdt_ac_id.prdt_no); /**登记簿产品代码,之前漏了20070508，lijx*/

	/**-------- 存折类，单一帐号介质  长治所有折类都用的是活期存折所以得用产品来判断--------**/
	if( mdm_attr_c.mdm_type[0]=='1' && mdm_attr_c.ac_num_ind[0]=='Y' )
	{
		/***添加了一户通的处理 add by ligl 2006-12-7 16:56***/
		if( (!strncmp(mdm_attr_c.mdm_code,"0010",4)||!strncmp(mdm_attr_c.mdm_code,"0011",4)) && (memcmp(tmp_prdt_ac_id.prdt_no,"101",3)==0 || memcmp(tmp_prdt_ac_id.prdt_no,"102",3)==0|| memcmp(tmp_prdt_ac_id.prdt_no,"103",3)==0))
		/*****end******/
		{
			g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
									 "ac_id=%ld and ac_seqn=%d ", \
									 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			if( g_reply_int )
			{
	   			sprintf(acErrMsg,"read dd_mst er==[%ld]错",g_mdm_ac_rel.ac_id);
				WRITEMSG
				return 1;
			}

			chg_book_bal=g_dd_mst.bal;

			g_reply_int = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
									"prdt_no='%s'", g_dd_mst.prdt_no);
			if( g_reply_int )
			{
	   			sprintf(acErrMsg,"read dd_parm error==[%s]错",g_dd_mst.prdt_no);
				WRITEMSG
				return 1;
			}

			strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
			pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);

		pub_base_dic_show_str(deac_type,"deac_type",g_dd_mst.sttl_type);
	
		if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='N')
				strcpy(draw_type,"凭密支取");
		else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"证件支取");
   		else if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"密码兼证件支取");
   	 	else
	   		strcpy(draw_type,"");
		/***得到存款科目号***/
		if(pub_base_prdt_accno(g_dd_mst.prdt_no,acc_no))
			strcpy(acc_no , "");

			pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
			pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
			pub_base_strpack(br_name);
			pub_base_strpack(br_tele);
			pub_base_strpack(g_cif_email_inf.email);
			pub_base_strpack(g_dd_parm.title);
			pub_base_strpack(g_dd_mst.name);
		
			fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
			,g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1
			,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no
			,g_dd_mst.cif_no,0,"","",g_dd_mst.rate,g_pub_tx.tx_amt1
			,g_cif_email_inf.email,draw_type,deac_type,cur_name
			,g_pub_tx.tx_code,"",g_pub_tx.ac_no1,0.00,""
			,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
			,g_dd_mst.prdt_no,0,g_pub_tx.brf
			,tmp_ac_no,tmp_tx_name,"","","",br_name,br_tele,
			g_mdm_ac_rel.note_no,"",acc_no);	
		}
		else 
		{
			/*** 零整存折(目前) ***/
			g_reply_int = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
									 "ac_id=%ld and ac_seqn=%d ", \
									 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			if( g_reply_int )
			{
	   			sprintf(acErrMsg,"read dd_mst er==[%ld]错",g_mdm_ac_rel.ac_id);
				WRITEMSG
				return 1;
			}

			chg_book_bal=g_td_mst.bal;

			g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
									"prdt_no='%s'", g_td_mst.prdt_no);
			if( g_reply_int )
			{
	   			sprintf(acErrMsg,"read td_parm error==[%s]错",g_td_mst.prdt_no);
				WRITEMSG
				return 1;
			}
			strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
			pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);
			pub_base_dic_show_str(deac_type,"deac_type",g_dd_mst.sttl_type);
	
		if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='N')
				strcpy(draw_type,"凭密支取");
		else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"证件支取");
   		else if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"密码兼证件支取");
   	 	else
	   		strcpy(draw_type,"");
		/***得到存款科目号***/
		if(pub_base_prdt_accno(g_td_mst.prdt_no,acc_no))
			strcpy(acc_no , "");

			pub_base_getbrname(g_td_mst.opn_br_no,br_name);
			pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
			pub_base_strpack(g_cif_email_inf.email);
			pub_base_strpack(br_name);
			pub_base_strpack(br_tele);
			pub_base_strpack(g_td_parm.title);
			pub_base_strpack(g_td_mst.name);
		
			fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%d|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
			,g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1
			,g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no
			,g_td_mst.cif_no,0,0,"",0.00,g_pub_tx.tx_amt1
			,g_cif_email_inf.email,draw_type,deac_type,cur_name
			,g_pub_tx.tx_code,"",g_pub_tx.ac_no1,0.00,""
			,g_pub_tx.ac_seqn,g_td_mst.ic_date,g_mdm_ac_rel.prt_line
			,g_td_mst.prdt_no,0,g_pub_tx.brf
			,tmp_ac_no,tmp_tx_name,"","","",br_name,br_tele,
			g_mdm_ac_rel.note_no,"",acc_no);	
		}
	}
	else if( mdm_attr_c.mdm_type[0]=='2' && mdm_attr_c.ac_num_ind[0]=='Y' )
	{
	 	/**-------- 存单类，单一帐号介质 ------------**/
		g_reply_int = Td_mst_Sel( g_pub_tx.reply,&g_td_mst, \
								 "ac_id=%ld and ac_seqn=%d ", \
								 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( g_reply_int!=100 && g_reply_int )
		{
	   		sprintf(acErrMsg,"999999999999999999[%s]错",g_mdm_ac_rel.ac_id);
			WRITEMSG
			return 1;
		}

		chg_book_bal = g_td_mst.bal;

		g_reply_int = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
									g_td_mst.prdt_no);
		if( g_reply_int )
		{
	   		sprintf(acErrMsg,"查找产品[%s]错",g_td_mst.prdt_no);
			WRITEMSG
			return 1;
		}

		/***得到存款科目 add by wanglei 20061013***/
		if(pub_base_prdt_accno(g_td_mst.prdt_no,acc_no))
			strcpy(acc_no , "");

		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
		pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);

		/***存单打印存期单位为"月" 这里处理一下, by wanglei 20061013***/
		/***
	 	if(g_td_parm.term_type[0]=='Y')
		    strcpy(term_type,"年");
		else if(g_td_parm.term_type[0]=='M')
		    strcpy(term_type,"月");
		else if(g_td_parm.term_type[0]=='D')
			strcpy(term_type,"日");
		***/
		strcpy(term_type,"月");
		if(g_td_parm.term_type[0]=='Y')
			sprintf(term,"%2d",g_td_parm.term*12);
		else if(g_td_parm.term_type[0]=='M')
			sprintf(term,"%2d",g_td_parm.term);
		vtcp_log("[%s] [%d] term=%s term_type=%s\n",__FILE__,__LINE__,g_td_parm.term,term,g_td_parm.term_type,term_type);
		/***处理完毕***/
		/**  根据晋中同业存放定期款项需求 打印CZFY利率部提后维持原利率 add by martin 2009/7/24 13:40:35 **/
		if(memcmp(g_td_parm.prdt_no,"25A",3)==0)
			{
				rate_val=g_td_mst.rate;
				vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
			}else
		{

	   	ret=pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no, \
							g_td_mst.opn_date,&rate_val);
		  if( ret ) return 1;
		}

		if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='N')
 			strcpy(draw_type,"凭密支取");
  		else if(g_pub_tx.draw_pwd_yn[0]=='N'&&g_pub_tx.draw_id_yn[0]=='Y')
   			strcpy(draw_type,"证件支取");
		else if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='Y')
		   	strcpy(draw_type,"密码兼证件支取");
	    else
	   		strcpy(draw_type,"");

		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(br_tele);
		pub_base_strpack(g_td_parm.title);
		pub_base_strpack(g_td_mst.name);

		if (g_td_parm.td_type[0] != '4')
		{
			tmp_date = g_pub_tx.tx_date;
			tmp_amt = g_pub_tx.tx_amt1;
			g_pub_tx.tx_date = g_td_mst.mtr_date;
			g_pub_tx.tx_amt1 = g_td_mst.bal;

			ret = pub_base_CalIntstTd(&sTmp_pub_intst.dealval,
				&sTmp_pub_intst.factval, \
				&sTmp_pub_intst.val,&sTmp_pub_intst.keepval,g_pub_tx.reply); 
			if( ret )
			{
				sprintf(acErrMsg,"取到期利息值出错");
				WRITEMSG
				strcpy( g_pub_tx.reply,"O194" );
				return 1;
			}

			g_pub_tx.tx_date = tmp_date;
			g_pub_tx.tx_amt1 = tmp_amt;
		}
		else
		{
			sTmp_pub_intst.dealval = 0.00;
			sTmp_pub_intst.factval = 0.00;
			sTmp_pub_intst.val = 0.00;
			sTmp_pub_intst.keepval = 0.00;
		}
		vtcp_log("[%s] [%d]  sTmp_pub_intst.keepval = %.2f sTmp_pub_intst.dealval = %.2f\n",__FILE__,__LINE__,sTmp_pub_intst.keepval,sTmp_pub_intst.dealval);

		strcpy(deac_type,"");

		if( g_td_mst.tfr_ind[0]=='Y' )
			strcpy(auto_depo,"【 自动转存 】");
		else 
			strcpy(auto_depo,"");

		fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
		,g_pub_tx.ac_no,g_td_mst.name,g_td_mst.bal
		,g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no
		,g_td_mst.cif_no,g_td_mst.mtr_date,term,term_type
		,rate_val,g_td_mst.bal
		,g_cif_email_inf.email,draw_type,deac_type,cur_name
		,g_pub_tx.tx_code,"更换",""
		,sTmp_pub_intst.keepval+sTmp_pub_intst.dealval,""
		,g_pub_tx.ac_seqn,g_td_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_td_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,"","","",br_name,br_tele,
		g_mdm_ac_rel.note_no,auto_depo,acc_no);
	} 
	else if( mdm_attr_c.mdm_type[0]=='1' && mdm_attr_c.ac_num_ind[0]=='N' )
	{
	 	/**-------- 存折类，多帐号介质 (定期一本通)------------**/
		g_reply_int = Td_mst_Sel( g_pub_tx.reply,&g_td_mst, \
								"ac_id=%ld and ac_sts='1' ",g_mdm_ac_rel.ac_id);
		if( g_reply_int==100 )
		{
	   		sprintf(acErrMsg," 查找账号[%ld][%d]错",g_mdm_ac_rel.ac_id,
					g_reply_int );
			WRITEMSG
			return 1;
		}
		else if( g_reply_int )
		{
	   		sprintf(acErrMsg," 查找账号[%ld]错[%d]",g_mdm_ac_rel.ac_id,
					g_reply_int);
			WRITEMSG
			return 1;
		}

		chg_book_bal=g_td_mst.bal;

		g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
								g_td_mst.prdt_no);
		if( g_reply_int )
		{
	   		sprintf(acErrMsg,"888888888888888888[%s]错",g_td_mst.prdt_no);
			WRITEMSG
			return 1;
		}

		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
		pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);

		strcpy(deac_type,"");

		if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='N')
			strcpy(draw_type,"凭密支取");
	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"证件支取");
   	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"密码兼证件支取");
   	 	else
	   		strcpy(draw_type,"");

		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(br_tele);
		pub_base_strpack(g_td_mst.name);
		
		fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|/%s|%s|%s|%s|%s|%s|\n"
	 	,g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1
		,g_td_parm.title,g_mdm_ac_rel.beg_date,g_mdm_ac_rel.opn_br_no
		,g_td_mst.cif_no,0,0,"",0.00,g_pub_tx.tx_amt1
	 	,g_cif_email_inf.email,draw_type,deac_type,cur_name
		,g_pub_tx.tx_code,"更换",""
		,sTmp_pub_intst.keepval+sTmp_pub_intst.dealval,""
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,"","","",br_name,br_tele,
		g_mdm_ac_rel.note_no,"");

	}/* 卡介质 */
	else if( mdm_attr_c.mdm_type[0]=='3' && mdm_attr_c.ac_num_ind[0]=='N' )
	{
		g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply , &s_prdt_ac_id , \
									 "ac_id=%ld" , g_mdm_ac_rel.ac_id );
		if( g_reply_int )
		{
	   		sprintf(acErrMsg,"取表[%ld]错",g_mdm_ac_rel.ac_id );
			WRITEMSG
			return 1;
		}
		strcpy(g_pub_tx.prdt_code,s_prdt_ac_id.prdt_no); /**为开销户登记簿准备产品代码 lijx */

		strcpy(deac_type,"");

		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_strpack(br_tele);
		pub_base_strpack(br_name);

		if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='N')
			strcpy(draw_type,"凭密支取");
	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"证件支取");
   	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&&g_mdm_ac_rel.draw_id_yn[0]=='Y')
			strcpy(draw_type,"密码兼证件支取");
   	 	else
	   		strcpy(draw_type,"");
		
		fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
	 	,g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1
		,g_td_parm.title,g_mdm_ac_rel.beg_date,g_mdm_ac_rel.opn_br_no
		,s_prdt_ac_id.cif_no,0,0,"",0.00,g_pub_tx.tx_amt1
	 	,g_cif_email_inf.email,draw_type,deac_type,cur_name
		,g_pub_tx.tx_code,"更换","",
		sTmp_pub_intst.keepval+sTmp_pub_intst.dealval,""
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,"","","",br_name,br_tele,
		g_mdm_ac_rel.note_no,"");
	}	

	if( mdm_attr_c.prt_ind[0]=='Y' && mdm_attr_c.mdm_type[0]=='1' )
	{
		sprintf(acErrMsg, "打印存折!!");
		WRITEMSG
		ret = sql_count("mdm_unprt_hst","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"TRANSFER SQL_COUNT ERROR,RET=[%d]错", ret);
			WRITEMSG
			return 1;
		}
		else if( ret==0 && mdm_attr_c.ac_num_ind[0]=='Y' )
		{
			mdm_unprt_hst_c.trace_no=g_pub_tx.trace_no;
			mdm_unprt_hst_c.trace_cnt=g_pub_tx.trace_cnt;
			strcpy(mdm_unprt_hst_c.tx_code,g_pub_tx.tx_code);
			strcpy(mdm_unprt_hst_c.tx_br_no,g_pub_tx.tx_br_no);
			mdm_unprt_hst_c.ac_id=g_pub_tx.ac_id;
			mdm_unprt_hst_c.ac_seqn=g_pub_tx.ac_seqn;
			strcpy(mdm_unprt_hst_c.add_ind,"1");
			mdm_unprt_hst_c.bal=g_mdm_ac_rel.book_bal;
			mdm_unprt_hst_c.bal=chg_book_bal;
			mdm_unprt_hst_c.tx_date=g_pub_tx.tx_date;
			strcpy(mdm_unprt_hst_c.tel,g_pub_tx.tel);
			if (mode == 1)
			{
				strcpy(mdm_unprt_hst_c.brf,"换折");
			}
			else
			{
				strcpy(mdm_unprt_hst_c.brf,"挂失换证");
			}
			ret=Mdm_unprt_hst_Ins(mdm_unprt_hst_c,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"登记未打印明细帐错误[%d]!",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P131");
				return 1;
			}
		}
		/**** add by xxx at 20050723 ***/
		else if (mdm_attr_c.ac_num_ind[0]=='Y')
		{
			mdm_unprt_hst_c.trace_no=g_pub_tx.trace_no;
			mdm_unprt_hst_c.trace_cnt=g_pub_tx.trace_cnt;
			strcpy(mdm_unprt_hst_c.tx_code,g_pub_tx.tx_code);
			strcpy(mdm_unprt_hst_c.tx_br_no,g_pub_tx.tx_br_no);
			mdm_unprt_hst_c.ac_id=g_pub_tx.ac_id;
			mdm_unprt_hst_c.ac_seqn=g_pub_tx.ac_seqn;
			strcpy(mdm_unprt_hst_c.add_ind,"1");
			mdm_unprt_hst_c.bal=g_mdm_ac_rel.book_bal;
			mdm_unprt_hst_c.tx_date=11111111;
			strcpy(mdm_unprt_hst_c.tel,g_pub_tx.tel);

			if (mode == 1)
				strcpy(mdm_unprt_hst_c.brf,"换折");
			else
				strcpy(mdm_unprt_hst_c.brf,"挂失换证");

			ret=Mdm_unprt_hst_Ins(mdm_unprt_hst_c,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"登记未打印明细帐错误[%d]!",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P131");
				return 1;
			}
		}
		/*** add end by rob ****/
	}

	fclose(fp);
 	set_zd_data(DC_FILE_SND,"1");
	/***Add Complete***/

	if( mdm_attr_c.prt_ind[0]=='Y' && mdm_attr_c.mdm_type[0]=='1' )
	{
		if( pub_base_patch_book(mdm_attr_c)<0 )
		{
			sprintf(acErrMsg,"补登折函数出错!");
			WRITEMSG
			return 1;
		}
	}

	if( pub_ins_rpl_mdm(mode) )
	{
		sprintf(acErrMsg,"登记换证登记薄出错!");
		WRITEMSG
		return 1;
	}

	return 0;
}

int pub_chg_mdm()								/*正常换证*/
{
	sprintf(acErrMsg,"需要输入卡号!!g_pub_tx.crd_no=[%s],g_pub.tx_code[%s]",
					g_pub_tx.crd_no,g_pub_tx.tx_code);
	WRITEMSG

	/*需要凭卡号开户，如卡类：开户时需要输入卡号*/
	if( mdm_attr_c.no_ind[0]=='Y' )					/*需卡的情况*/
	{
		if( !strlen(g_pub_tx.crd_no) )
		{
			sprintf(acErrMsg,"需要输入卡号");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D111" );
			return 1;
		}

		/**检查卡号是否已经开过**/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&v_mdm_ac_rel,"ac_no='%s'", \
							g_pub_tx.crd_no);
		if( ret && ret!=100 ) return 1;
		else if( ret==0 )
		{
			sprintf(acErrMsg,"该卡号已经开出!![%s]",g_pub_tx.crd_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D113" );
			return 1;
		}

		if( g_mdm_ac_rel.note_sts[0]=='1' )
		{
			sprintf(acErrMsg,"账号已经挂失，请选择挂失换证![%s]",
					g_mdm_ac_rel.note_sts);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P194");
			return 1;
		}

		/*** 检查支取方式函数 ***/
		/**2410交易不校验密码,wangwk-20101220**/
		if(memcmp(g_pub_tx.tx_code,"2410",4)!=0)
		   {
			if( pub_base_check_draw("001") )
			{
				sprintf(acErrMsg,"检查支取方式错误!");
				WRITEMSG
				return 1;
			}
		}

		/*修改原来帐户状态为正常换证*/
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
									   g_pub_tx.ac_no);
	    if(g_reply_int)
	   	{
			sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&v_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
		  	sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(v_mdm_ac_rel.note_sts,"3");

		g_reply_int=Mdm_ac_rel_Upd_Upd(v_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"取记录错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );

		strcpy(g_mdm_ac_rel.ac_no,g_pub_tx.crd_no);
		strcpy(g_mdm_ac_rel.note_no,g_pub_tx.beg_note_no);
		if(strlen(g_pub_tx.draw_pwd_yn)!= 0)
		{
			vtcp_log("%s,%d,原支付标记[%s],[%s]",__FILE__,__LINE__,
				g_mdm_ac_rel.draw_pwd_yn,g_mdm_ac_rel.draw_pwd_yn);
			g_mdm_ac_rel.draw_pwd_yn[0]=g_pub_tx.draw_pwd_yn[0];
		}
		/**添加密码验证-更改功能-wangwk-20110105**/
		sprintf(acErrMsg,"换卡新密码");
		WRITEMSG
		pub_base_EnDes(g_pub_tx.tx_date,g_mdm_ac_rel.ac_no,g_pub_tx.draw_pwd);
		memcpy(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd,sizeof(g_pub_tx.draw_pwd));
		sprintf(acErrMsg,"换卡新密码");
		WRITEMSG
		/**end**/
		/*登记新帐户到介质帐户对照表*/
		ret=Mdm_ac_rel_Ins(g_mdm_ac_rel,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"登记介质账户对照表失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O083");
			return 1;
		}

		/*登记正常更换后的新卡到开销户登记薄中 add by 20151110 */
		sprintf(acErrMsg,"登记开销户登记簿赋值开始!");
		WRITEMSG
		
		/*根据账户id查dd_mst,取到prdt_no,name,hst_bal,cif_no*/
		g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld  ", g_mdm_ac_rel.ac_id);
			if( g_reply_int )
			{
	   		sprintf(acErrMsg,"read dd_mst er==[%ld]错",g_mdm_ac_rel.ac_id);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O083");
				return 1;
			}

		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		strcpy(g_pub_tx.name,g_dd_mst.name);
		g_pub_tx.tx_amt1=g_dd_mst.hst_bal;
		g_pub_tx.cif_no=g_dd_mst.cif_no;
			
		/*根据查到的prdt_no查dd_parm表，取到cur_no*/
			g_reply_int = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", g_dd_mst.prdt_no);
			if( g_reply_int )
			{
	   		sprintf(acErrMsg,"read dd_parm error==[%s]错",g_dd_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O083");
				return 1;
			}
		strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
			
		strcpy ( g_pub_tx.ac_no,g_pub_tx.crd_no);		/* 新帐号 */
		g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;  
		g_pub_tx.ac_seqn =g_mdm_ac_rel.ac_seqn;		/* 账户序号 */	
		/*
		sprintf(acErrMsg,"naccno:[%ld],acid:[%ld],ac_seqn:[%ld],name:[%20s],prdt_no:[%ld],tx_amt:[%.2f],cur_no:[%s],cif_no:[%s]", \
		g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.name,g_pub_tx.prdt_code,g_pub_tx.tx_amt1,g_pub_tx.cur_no,g_pub_tx.cif_no);
		WRITEMSG
		*/
		sprintf(acErrMsg,"登记开销户登记簿赋值结束!");
		WRITEMSG
			
		g_reply_int=pub_base_InsMo_opn_cls(g_pub_tx.name,"","1");
		if( g_reply_int )
		{
			sprintf(acErrMsg,"登记开销户登记簿出错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O083");
			return 1;
		}
		/* end by 20151110 */	
		
		ret=sql_execute("update ag_peritemmap set ac_no='%s' where ac_no='%s'",
						g_mdm_ac_rel.ac_no,g_pub_tx.ac_no );
		if(ret)
		{
			sprintf(acErrMsg,"登记 ag_peritemmap 失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O083");
			return 1;
		}
		strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
	}
	else										/* 非卡的情况 */
	{
		/*** 检查支取方式函数 ***/
		/**2410交易不校验密码,wangwk-20101220**/
		if(memcmp(g_pub_tx.tx_code,"2410", 4)!=0)
		{
				if( pub_base_check_draw("001") )
				{
					sprintf(acErrMsg,"检查支取方式错误!");
					WRITEMSG
					return 1;
				}
		}
		/*修改原来帐户状态为正常换证*/
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
									   g_pub_tx.ac_no);
	    if(g_reply_int)
	   	{
			sprintf(acErrMsg,"定义游标错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&v_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
		  	sprintf(acErrMsg,"取记录错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(v_mdm_ac_rel.note_sts,"0");
		strcpy(v_mdm_ac_rel.note_no,g_pub_tx.beg_note_no);

		vtcp_log("%s,%d,原支付标记[%s],[%s]",__FILE__,__LINE__,
				g_mdm_ac_rel.draw_pwd_yn,g_mdm_ac_rel.draw_pwd_yn);
		if(strlen(g_pub_tx.draw_pwd_yn)!= 0)
		{
			v_mdm_ac_rel.draw_pwd_yn[0]=g_pub_tx.draw_pwd_yn[0];
		}
		/**添加密码验证-更改功能-wangwk-20110105**/
		sprintf(acErrMsg,"换卡新密码");
		WRITEMSG
		pub_base_EnDes(g_pub_tx.tx_date,g_mdm_ac_rel.ac_no,g_pub_tx.draw_pwd);
		memcpy(v_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd,sizeof(g_pub_tx.draw_pwd));
		sprintf(acErrMsg,"换卡新密码");
		WRITEMSG
		/**end**/
		g_reply_int=Mdm_ac_rel_Upd_Upd(v_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"取记录错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}
		Mdm_ac_rel_Clo_Upd( );
	}

	/* 需要凭证销号 */
	if( mdm_attr_c.only_ind[0]=='Y' )
	{
	   ret=pub_com_NoteUse(0,0,g_pub_tx.note_type,
			g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
	   if( ret )
	   {
		    sprintf(acErrMsg,"柜员使用凭证错误!");
		    WRITEMSG
		return 1;
	   }
  	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		return 1;
	}

	return 0;
}

int pub_loss_chg_mdm(int mode)					/*挂失换证*/
{
	char cif_type[2];
	char new_ac_no[25];

	struct mdm_ac_rel_c o_mdm_ac_rel;
	struct mdm_attr_c o_mdm_attr_c;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct pub_tx v_pub_tx;
	double bttmpdb;
	vtcp_log("[%s][%d]挂失模式是[%d]\n",__FILE__,__LINE__,mode);
	if( mode==2 || mode==3 ) 
	{ 
		if( g_mdm_ac_rel.note_sts[0]!='1' )
		{
			sprintf(acErrMsg,"该账号未挂失![%s]",g_mdm_ac_rel.note_sts);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P197");
			return 1;
		}
	}

	if( mode==2 ) 					/*** 书挂挂失换证 ***/
	{
		mode=22;					/*解除书挂*/
		/*** 检查支取方式函数 ***/
		/**2410交易不验证密码,wangwk-201220**/
		if(memcmp(g_pub_tx.tx_code,"2410", 4) != 0)
		{
			if( pub_base_check_draw("001") )
			{
				sprintf(acErrMsg,"检查支取方式错误!");
				WRITEMSG
				return 1;
			}
		}
		/*解挂*/
		if( pub_un_loss( mode , 1 ) )
		{
			sprintf(acErrMsg,"解挂函数错误!");
			WRITEMSG
			return 1;
		}
	}
	else if( mode==3 ) 				/*** 密挂兼书挂挂失换证 ***/
	{
		mode=24;					/*解除书挂兼密挂*/
		if( pub_un_loss( mode , 1 ) )
		{
			sprintf(acErrMsg,"解挂函数错误!");
			WRITEMSG
			return 1;
		}
	}
	else if( mode==10 ) 			/*** 部提换证 ***/
	{
		bttmpdb=g_pub_tx.tx_amt1;
	}
	else
	{
		sprintf(acErrMsg,"换证类型错误![%d]",mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P199");
		return 1;
	}

	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
    if(g_reply_int)
   	{
		sprintf(acErrMsg,"定义游标错误!");
			WRITEMSG
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&o_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
	  	sprintf(acErrMsg,"取记录错误!");
			WRITEMSG
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	memcpy( &v_mdm_ac_rel,&o_mdm_ac_rel,sizeof(v_mdm_ac_rel) );
	strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);

	/**--- xxx --- g_pub_tx.id_type为支取证件号，不一定是开户客户号 ----**
	if( pub_cif_CifCheck(g_pub_tx.id_type,g_pub_tx.id_no,&g_pub_tx.cif_no,
						 cif_type) )
	{
		sprintf(acErrMsg,"该客户在客户中心未登记![%s][%s]",
				g_pub_tx.id_type,g_pub_tx.id_no);
		WRITEMSG
		return 1;
	}
	**------------------------------------------------------------------*/

	ret=Prdt_ac_id_Sel( g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"取客户号错误![%d][%d]",ret,g_mdm_ac_rel.ac_id);
		WRITEMSG
		return 1;
	}
	strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no); /**为开销户登记簿准备产品代码 lijx*/

	g_pub_tx.cif_no=sPrdt_ac_id.cif_no;
 
	g_reply_int=pub_base_InsMo_opn_cls(g_pub_tx.name,"","2");
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记开销户登记簿出错!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.hst_ind,"1" );
	g_pub_tx.tx_amt1=0.00;

	/***
	if( pub_ins_dd_hst() )
	{
		sprintf(acErrMsg,"登记明细出错!");
		WRITEMSG
		return 1;
	}
	***/

	strcpy( v_pub_tx.beg_note_no,g_pub_tx.beg_note_no );
	strcpy( g_pub_tx.beg_note_no,o_mdm_ac_rel.note_no );

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		return 1;
	}

	strcpy( g_pub_tx.beg_note_no,v_pub_tx.beg_note_no );

	/*** 读取介质属性 ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&o_mdm_attr_c," mdm_code='%s' ", \
					 o_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该介质属性不存在![%d][%s]",
			g_reply_int,o_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取介质属性异常![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** 开显示账号 ***/
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
		strcpy( new_ac_no,g_pub_tx.crd_no );

		pub_base_strpack(g_pub_tx.beg_note_no);
		if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no) )
		{
			/*** 根据卡号取凭证号 ***/
			pub_card_CardToNote(g_pub_tx.crd_no,g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
		}
	}
	else
	{
		if( pub_base_CrtCifAc(g_pub_tx.cif_no,new_ac_no) )
		{
			sprintf(acErrMsg,"生成客户显示账号错误!");
			WRITEMSG
			return 1;
		}
	}

	/**----------- xxx ------- 定期部提后小于十万用普通存单---------**/
	if( mode==10 && pub_base_CompDblVal(g_td_mst.bal,100000.00)<0 
		&& g_td_mst.cif_no<50000000 && !strcmp(o_mdm_ac_rel.mdm_code,"0014") )
	{
		strcpy(g_pub_tx.note_type,"013");
		strcpy(v_mdm_ac_rel.mdm_code,"0013");
	}

	/*** 调销凭证函数 ***/
	g_reply_int=pub_com_NoteUse(0,0,g_pub_tx.note_type,g_pub_tx.beg_note_no,
								g_pub_tx.end_note_no,g_pub_tx.tel);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"柜员使用凭证错误!");
		WRITEMSG
		return 1;
	}

	/*** add by xxx at 20050723 ***/
	if (mode == 22 || mode == 24)
		strcpy(o_mdm_ac_rel.note_sts,"2");
	else if (mode == 10)
	{
		strcpy(o_mdm_ac_rel.note_sts,"4");
		o_mdm_ac_rel.end_date=g_pub_tx.tx_date;
	}
	else 
	{
		sprintf(acErrMsg,"换证类型错误![%d]",mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P199");
		return 1;
	}
	/*** end *****/

	g_reply_int=Mdm_ac_rel_Upd_Upd(o_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"取记录错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(v_mdm_ac_rel.note_no,g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.ac_no,new_ac_no);
	strcpy(v_mdm_ac_rel.ac_no,g_pub_tx.ac_no);

	if( mode==10 ) 			/*** 部提换证 ***/
	{
		bttmpdb=g_pub_tx.tx_amt1;
		g_pub_tx.tx_amt1=g_td_mst.bal;
	}

	g_reply_int=pub_base_InsMo_opn_cls(g_pub_tx.name,"","1");
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记开销户登记簿出错!");
		WRITEMSG
		goto ErrExit;
	}

	if( mode==10 ) 			/*** 部提换证 ***/
	{
		g_pub_tx.tx_amt1=bttmpdb ;
		v_mdm_ac_rel.beg_date=g_pub_tx.tx_date;
	}

	v_mdm_ac_rel.prt_line=0;
	/**更改新密码**/
	/**vtcp_log("[%s][%d]新密码[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);
	pub_base_EnDes(g_pub_tx.tx_date,v_mdm_ac_rel.ac_no,g_pub_tx.draw_pwd);
	vtcp_log("[%s][%d]新密码[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);
	memcpy(v_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd,sizeof(g_pub_tx.draw_pwd));
	**/
	g_reply_int=Mdm_ac_rel_Ins(v_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"取记录错误!");
		WRITEMSG
		return -1;
	}
	/**end**/
	/**为登折服务**/
	strcpy( g_mdm_ac_rel.ac_no,v_mdm_ac_rel.ac_no );

	Mdm_ac_rel_Clo_Upd( );

	ret=sql_execute("update mo_loss set new_ac_no='%s' where ac_no='%s' and unloss_date=%d and trace_no=%d ",
					v_mdm_ac_rel.ac_no,o_mdm_ac_rel.ac_no,g_pub_tx.tx_date,
					g_pub_tx.trace_no );
	if( ret )
	{
		sprintf(acErrMsg,"登记 ag_peritemmap 失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O083");
		return 1;
	}

	ret=sql_execute("update ag_peritemmap set ac_no='%s' where ac_no='%s'",
					v_mdm_ac_rel.ac_no,o_mdm_ac_rel.ac_no );
	if( ret )
	{
		sprintf(acErrMsg,"登记 ag_peritemmap 失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O083");
		return 1;
	}

	pub_base_strpack( o_mdm_ac_rel.ac_no );
	pub_base_strpack( v_mdm_ac_rel.ac_no );

	ret=sql_execute("update mob_acct_type set ac_no='%s' where ac_no='%s'",
					 v_mdm_ac_rel.ac_no,o_mdm_ac_rel.ac_no );
	if(ret)
	{
		sprintf(acErrMsg,"登记 mob_acct_type 失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O083");
		return 1;
	}

	/**新户记录**/
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.hst_ind,"1" );
	g_pub_tx.tx_amt1=0.00;
	/***
	if( pub_ins_dd_hst() )
	{
		sprintf(acErrMsg,"登记明细出错!");
		WRITEMSG
		return 1;
	}
	***/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_rpl_mdm(int mode)
{
	strcpy(mo_rpl_note_c.br_no,g_pub_tx.tx_br_no);
	strcpy(mo_rpl_note_c.new_ac_no,g_pub_tx.ac_no);
	strcpy(mo_rpl_note_c.new_note_no,g_pub_tx.beg_note_no);
	strcpy(mo_rpl_note_c.mdm_code,g_mdm_ac_rel.mdm_code);
	mo_rpl_note_c.rpl_date=g_pub_tx.tx_date;
	strcpy(mo_rpl_note_c.tel,g_pub_tx.tel);
	mo_rpl_note_c.trace_no=g_pub_tx.trace_no;
	if( mode==1 )	strcpy(mo_rpl_note_c.rmk,"1");
	else if( mode==2 || mode==3 )	strcpy(mo_rpl_note_c.rmk,"2");
	else if( mode==10 )  strcpy(mo_rpl_note_c.rmk,"4");
	else strcpy(mo_rpl_note_c.rmk,"9");
	
	g_reply_int=Mo_rpl_note_Ins(mo_rpl_note_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记换证登记薄出错![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P196");
		return 1;
	}

	return 0;
}
