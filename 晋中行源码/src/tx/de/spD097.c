/***************************************************************
* 文 件 名: spD097.c
* 功能描述:	转存单交易
* 作    者: jack
* 完成日期：2004年1月11日
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_rpl_note_c.h"
#include "mo_opn_cls_c.h"
#include "td_mst_hst_c.h"

spD097()
{
	char ac_no[25];
	char cur_name[17];
	char deac_type[15];
	char term_type[15];
	char draw_type[21];
	char tmpname[101];
	char mdm_code[5];
	char ac_num_ind[2];
	int  ac_seqn=0,ret=0;
	int  tx_cnt=0,hst_cnt=0;
	char beg_note_no[17];
	char end_note_no[17];
	char note_type[4];
	char opn_br_no[6];
	char check_no[21];
	char acc_hrt[8];
	char br_name[6];
	char auto_depo[15];
	double bal,amt,rate_val;
	long ac_id;
	long opn_date;
	long mtr_date;
	long tmp_date;
	long ic_date;
	int book_line;
	int	 tmp_term;
	FILE *fp;

	struct cif_email_inf_c  	g_cif_email_inf;
	struct mdm_attr_c  			mdm_attr_c;
	struct mdm_ac_rel_c			f_mdm_ac_rel;
	struct prdt_ac_id_c  		g_prdt_ac_id;
	struct td_mst_c  			f_td_mst;
	struct td_mst_c  			h_td_mst;
	struct td_mst_c  			m_td_mst;
	struct td_mst_hst_c  		g_td_mst_hst;
	struct cif_id_code_rel_c  	g_cif_id_code_rel;
	struct mo_rpl_note_c  		g_mo_rpl_note;
	struct mo_opn_cls_c  		g_mo_opn_cls;

	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&f_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&h_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&m_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_mst_hst,0x00,sizeof(struct td_mst_hst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_mo_rpl_note,0x00,sizeof(struct mo_rpl_note_c));
	memset(&g_mo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));

	/*** 初始化全局变量***/
	pub_base_sysinit();

	get_zd_data("1021",ac_no);
	get_zd_int("0340",&ac_seqn);
	get_zd_data("0240",g_pub_tx.prdt_code);
	get_zd_double("0420",&g_pub_tx.tx_amt1);
	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1144",g_pub_tx.draw_pwd);
	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);
	
	 sprintf(acErrMsg,"不存在该账户!!ac_no=[%s]",ac_no);
   	 WRITEMSG

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld]",g_mdm_ac_rel.ac_id);
	   	 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]=='1')
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='2')
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='3')
	{
		sprintf(acErrMsg,"该介质已正常换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已销户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已做部提，请输入新帐号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O217");
		goto ErrExit;
	}

	ac_id = g_mdm_ac_rel.ac_id;							/* 原帐号ID */
	strcpy( mdm_code,g_mdm_ac_rel.mdm_code );			/* 原介质 */

	/*** 检查支取方式是否合法 ***/
	ret = pub_base_check_draw("001");
	if( ret )
	{
		sprintf(acErrMsg,"调用检查支取方式是否合法函数出错");
	   	WRITEMSG
		goto ErrExit;
	}

	/*** 取介质属性 ***/
	ret = Mdm_attr_Sel( g_pub_tx.reply,&mdm_attr_c,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该介质!!mdm_code=[%s]",g_mdm_ac_rel.mdm_code);
	   	 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy( ac_num_ind,mdm_attr_c.ac_num_ind );

	/*** 多帐户介质，即储蓄户做转存单 ***/
	if( ac_num_ind[0]=='N' && ac_seqn<200 )
		ac_seqn = 200;
	else if( ac_num_ind[0]=='Y' )
		ac_seqn = 0;

	/*** 取户名 ***/
	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", \
					 ac_id,ac_seqn);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld]",g_mdm_ac_rel.ac_id);
   		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_td_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该帐户已销");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	/*** 登记定期明细文件 ***/
	/*** hao ----- @20050907@ ---- 只有多介质单账户帐号新开户 ----**/
	if( ac_num_ind[0]=='N' )
	{
		g_td_mst_hst.trace_no = g_pub_tx.trace_no;
		strcpy(g_td_mst_hst.opn_br_no,g_mdm_ac_rel.opn_br_no);
		strcpy(g_td_mst_hst.tx_br_no,g_pub_tx.tx_br_no);
		g_td_mst_hst.ac_id = ac_id;
		g_td_mst_hst.ac_seqn = ac_seqn;
		strcpy(g_td_mst_hst.tx_code,g_pub_tx.tx_code);
		strcpy(g_td_mst_hst.sub_tx_code,g_pub_tx.sub_tx_code);
		strcpy(g_td_mst_hst.add_ind,"0");
		strcpy(g_td_mst_hst.ct_ind,"1");
		g_td_mst_hst.tx_amt = g_td_mst.bal;
		g_td_mst_hst.bal = 0.00;
		g_td_mst_hst.acbo_bal = 0.00;
		g_td_mst_hst.intst_acm = g_td_mst.intst_acm;
		g_td_mst_hst.intst = 0.00;
		g_td_mst_hst.tx_date = g_pub_tx.tx_date;
		g_td_mst_hst.tx_time = g_pub_tx.tx_time;
		strcpy( g_td_mst_hst.note_type,mdm_attr_c.note_type );
		strcpy( g_td_mst_hst.note_no,g_mdm_ac_rel.note_no );
		strcpy( g_td_mst_hst.brf,"转存单" );
		strcpy( g_td_mst_hst.tel,g_pub_tx.tel );
		strcpy( g_td_mst_hst.chk,g_pub_tx.chk );
		strcpy( g_td_mst_hst.auth,g_pub_tx.auth );

		ret = Td_mst_hst_Ins( g_td_mst_hst,g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"登记定期明细文件出错[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P157" );
	 		goto ErrExit;
		}
	}

	/*** 打印USE ***/
	memset(&h_td_mst,0x00,sizeof(struct td_mst_c));
	memcpy(&h_td_mst,&g_td_mst,sizeof(struct td_mst_c));

	/*** 多帐户介质，即储蓄户做转存单 ***/
	amt = g_td_mst.bal;
	opn_date = g_td_mst.opn_date;
	mtr_date = g_td_mst.mtr_date;
	ic_date  = g_td_mst.ic_date;
	strcpy(opn_br_no,g_td_mst.opn_br_no);
	strcpy(g_pub_tx.name,g_td_mst.name);

	/***  转存单必须在开户机构办理 ***/
	if( strncmp(opn_br_no,g_pub_tx.tx_br_no,5) )
	{
		if( pub_base_getbrname( opn_br_no,br_name ) )
		{
			sprintf(acErrMsg,"取行名函数错!");
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack(br_name);
		strcpy(g_pub_tx.reply,"D211");
	 	sprintf( acErrMsg,"开户机构为:(%s).",br_name);
	 	set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		goto ErrExit;
	}

	ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
					  g_td_mst.prdt_no);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该账户!!prdt_no=[%s]",g_td_mst.prdt_no);
	   	 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*** 转到存单后的信息 ***/
	get_zd_data("1161",g_pub_tx.mdm_code);				/* 介质代码 */
	get_zd_data("1168",g_pub_tx.beg_note_no);			/* 凭证号码 */
	strcpy(beg_note_no,g_pub_tx.beg_note_no);
	strcpy(end_note_no,beg_note_no);
	get_zd_data("1152",g_pub_tx.draw_pwd_yn);
	get_zd_data("1154",g_pub_tx.draw_pwd);
	get_zd_data("1155",g_pub_tx.draw_id_yn);
	get_zd_data("1158",g_pub_tx.id_type);
	get_zd_data("1156",g_pub_tx.id_no);
	strcpy( g_pub_tx.ac_wrk_ind,"01111" );
vtcp_log("!!!!!!!!beg[%s]\n",g_pub_tx.beg_note_no);
	g_pub_tx.ac_id = 0;

	memset( &mdm_attr_c,0x00,sizeof(struct mdm_attr_c) );
	ret = Mdm_attr_Sel( g_pub_tx.reply,&mdm_attr_c,"mdm_code='%s'", \
						g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy(note_type,mdm_attr_c.note_type);

	/*** 取客户证件号码 ***/
	/*** 找开户核准号、科目控制字 ***/
	ret = Mo_opn_cls_Sel(g_pub_tx.reply, &g_mo_opn_cls, \
						 "ac_id=%ld and ac_seqn=%d", ac_id, ac_seqn);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]", \
				 g_mdm_ac_rel.ac_id);
   		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy(check_no,g_mo_opn_cls.check_no);
	strcpy(acc_hrt,g_mo_opn_cls.acc_hrt);

	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
							  "cif_no=%ld",h_td_mst.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"帐户未登记!++++++++++++[%ld][%ld]", \
				h_td_mst.cif_no,g_pub_tx.cif_no);
		WRITEMSG
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy(g_pub_tx.id_type,g_cif_id_code_rel.id_type);
	strcpy(g_pub_tx.id_no,g_cif_id_code_rel.id_no);

	/*** 检查产品-介质-介质关系是否匹配 ***/
	if( pub_base_check_promedi(g_pub_tx.prdt_code,g_pub_tx.mdm_code, \
								NULL_MDM,NULL_MDM) )
	{
		sprintf(acErrMsg,"该介质不能此产品[%s][%s]!",g_pub_tx.mdm_code, \
				g_pub_tx.prdt_code);
		WRITEMSG
		goto ErrExit;
	}

	/*** 多帐户介质需要销户重开 ***/
	if( ac_num_ind[0]=='N' )
	{
		/* 修改原来账户为销户 */
		ret = Td_mst_Dec_Upd( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
								ac_id,ac_seqn);
    	if( ret )
   		{
   			sprintf(acErrMsg,"定义游标错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			goto ErrExit;
		}

		ret = Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"账户不存在!ac_id=[%ld],ac_seqn=[%d]", \
					ac_id,ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if( f_td_mst.ac_sts[0]=='*' )
		{
			 sprintf(acErrMsg,"该账户已作销户处理!!");
	   		 WRITEMSG
			 strcpy(g_pub_tx.reply,"P245");
			 goto ErrExit;
		}

		if( f_td_mst.hold_sts[0]!='0' )
		{
			 sprintf(acErrMsg,"该账户已被冻结!!");
	   		 WRITEMSG
			 strcpy(g_pub_tx.reply,"L098");
			 goto ErrExit;
		}

		mtr_date = f_td_mst.mtr_date;			/*** 开户日不变 ***/
		opn_date = f_td_mst.opn_date;			/*** 到期日不变 ***/
		ic_date  = f_td_mst.ic_date;
		tx_cnt = f_td_mst.tx_cnt;				/*** 部提数不变 ***/
		hst_cnt = f_td_mst.hst_cnt;				/*** 明细数不变 ***/
		rate_val= f_td_mst.rate;				/*** 利率不变 ***/
		if( f_td_mst.tfr_ind[0]=='Y' )
			strcpy(auto_depo,"【 自动转存 】");
		else 
			strcpy(auto_depo,"");

		f_td_mst.bal = 0.00;			
		strcpy(f_td_mst.ac_sts,"*");

		ret = Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
		if( ret )
		{
	 		sprintf(acErrMsg,"修改记录错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
			goto ErrExit;
		}

		Td_mst_Clo_Upd( );

		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
						  f_td_mst.prdt_no);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"记录不存在!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		/*** 登记开销户登记薄 ***/
		strcpy(g_mo_opn_cls.ac_no,ac_no);
		g_mo_opn_cls.ac_id = g_mdm_ac_rel.ac_id;
		g_mo_opn_cls.ac_seqn = ac_seqn;
		strcpy(g_mo_opn_cls.cur_no,g_td_parm.cur_no);
		strcpy(g_mo_opn_cls.name,f_td_mst.name);
		strcpy(g_mo_opn_cls.acc_hrt,acc_hrt);
		strcpy(g_mo_opn_cls.cif_type,g_td_parm.cif_type);
		g_mo_opn_cls.amt = f_td_mst.bal;
		strcpy(g_mo_opn_cls.opn_br_no,f_td_mst.opn_br_no);
		g_mo_opn_cls.tx_date = g_pub_tx.tx_date;
		strcpy(g_mo_opn_cls.tx_brno,g_pub_tx.tx_br_no);
		g_mo_opn_cls.trace_no = g_pub_tx.trace_no;
		strcpy(g_mo_opn_cls.tel,g_pub_tx.tel);
		strcpy(g_mo_opn_cls.chk,g_pub_tx.chk);
		strcpy(g_mo_opn_cls.auth,g_pub_tx.auth);
		strcpy(g_mo_opn_cls.check_no,check_no);
		strcpy(g_mo_opn_cls.ct_ind,"2");
		strcpy(g_mo_opn_cls.wrk_sts,"2");

		ret = Mo_opn_cls_Ins(g_mo_opn_cls,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"调用pub_acct_opn_cif_mdm开介质函数出错!!");
			WRITEMSG
			goto ErrExit;
		}

		/*** 开介质 ***/
		g_pub_tx.ac_wrk_ind[0]='1';						/*销户标志*/
		ret = pub_acct_opn_cif_mdm( );
		if( ret )
		{
			sprintf(acErrMsg,"调用开介质函数出错!!");
			WRITEMSG
			goto ErrExit;
		}

		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.hst_ind,"1");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.brf,"新转单");

		if( pub_acct_opn_ac( ) )
		{
			sprintf(acErrMsg,"开户错误!");
			WRITEMSG
			goto ErrExit;
		}

		/*** 更改新存单账户信息为原来账户信息 ***/
		memset(&f_td_mst,0x00,sizeof(struct td_mst_c));
		ret = Td_mst_Dec_Upd( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",
								g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"定义定期主文件游标异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}

		ret = Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"定期主文件不存在该账户!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		memcpy(&g_td_mst,&f_td_mst,sizeof(struct td_mst_c));
		f_td_mst.ac_id = g_pub_tx.ac_id;
		f_td_mst.ac_seqn = g_pub_tx.ac_seqn;
		f_td_mst.tx_cnt = tx_cnt;				/*** 部提次数不变 ***/
		f_td_mst.hst_cnt = hst_cnt;				/*** 帐户明细不变***/
		f_td_mst.opn_date = opn_date;			/*** 开户日不变 ***/
		f_td_mst.mtr_date = mtr_date;			/*** 到期日不变 ***/
		f_td_mst.rate = rate_val;
		f_td_mst.ic_date = ic_date;

		ret = Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"更新定期主文件异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}

		Td_mst_Clo_Upd( );
	}
	else
	{
		strcpy(g_pub_tx.ac_no,ac_no);
		g_pub_tx.ac_seqn = ac_seqn;

		/*** 凭证销号 ***/
		ret=pub_com_NoteUse(ac_id,ac_seqn,note_type,beg_note_no,end_note_no, \
							g_pub_tx.tel);
		if( ret )
		{
		  	sprintf(acErrMsg,"调用凭证销号错误!ac_id==[%ld],ac_seqn==[%d], \
					note_type==[%s],beg_note_no==[%s],end_note_no==[%s], \
					g_pub_tx.tel==[%s]",ac_id,ac_seqn,note_type, \
					beg_note_no,end_note_no,g_pub_tx.tel);
			WRITEMSG
			goto ErrExit;
		}
		
		/*更改介质帐户表信息*/
		memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and mdm_code='%s'",ac_id,ac_seqn,mdm_code);
		if( ret )
		{
			sprintf(acErrMsg,"定义定期主文件游标异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"定期主文件不存在该账户!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(f_mdm_ac_rel.mdm_code , g_pub_tx.mdm_code);
		strcpy(f_mdm_ac_rel.note_no , g_pub_tx.beg_note_no);

		ret = Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"更新活期主文件异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}
		Mdm_ac_rel_Clo_Upd( );

		memcpy(&g_td_mst,&h_td_mst,sizeof(struct td_mst_c));
	}

	/*** 登记换证登记薄 ***/
	strcpy(g_mo_rpl_note.old_ac_no,ac_no);
	strcpy(g_mo_rpl_note.br_no,g_pub_tx.tx_br_no);
	strcpy(g_mo_rpl_note.mdm_code,g_mdm_ac_rel.mdm_code);
	strcpy(g_mo_rpl_note.old_note_no,g_mdm_ac_rel.note_no);
	strcpy(g_mo_rpl_note.new_ac_no,g_pub_tx.ac_no);
	strcpy(g_mo_rpl_note.new_note_no,g_pub_tx.beg_note_no);
	g_mo_rpl_note.rpl_date = g_pub_tx.tx_date;
	strcpy(g_mo_rpl_note.tel,g_pub_tx.tel);
	g_mo_rpl_note.trace_no = g_pub_tx.trace_no;
	strcpy(g_mo_rpl_note.rmk,"3");

	ret = Mo_rpl_note_Ins(g_mo_rpl_note,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记换证登记薄出错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
	 	goto ErrExit;
	}

	/*** 打印程序段 ***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	ret =Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"+++++++++++++++++[%s]错",g_td_mst.prdt_no);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P145" );
	 	goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);
	pub_base_dic_show_str(deac_type,"deac_type",g_pub_tx.ac_type);

	ret=pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,opn_date, \
						&rate_val);
	if( ret )
	{
		sprintf(acErrMsg,"+++++++++++++++++[%d]",ret);
		WRITEMSG
	 	goto ErrExit;
	}

	if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='N')
	 	strcpy(draw_type,"【 凭密支取 】");
	else if(g_pub_tx.draw_pwd_yn[0]=='N'&&g_pub_tx.draw_id_yn[0]=='Y')
		strcpy(draw_type,"【 证件支取 】");
	else if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='Y')
		strcpy(draw_type,"【 密码兼证件支取 】");
	else
	 	strcpy(draw_type,"");

	pub_base_strpack(g_pub_tx.name);
	pub_base_strpack(g_td_parm.title);
	pub_base_strpack(g_cif_email_inf.email);

	/*** 计算到期利息 ***/
	tmp_date = g_pub_tx.tx_date;
	g_pub_tx.tx_date = mtr_date;
	g_td_mst.opn_date = opn_date;
	g_td_mst.mtr_date = mtr_date;
	g_td_mst.ic_date = ic_date;
	g_td_mst.rate = rate_val;

		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>opn_date=[%ld], \
				ic_date=[%ld],mtr_date=[%ld],rate=[%.6lf]", \
				g_td_mst.opn_date,g_td_mst.ic_date,g_td_mst.mtr_date, \
				g_td_mst.rate);
		WRITEMSG

	ret = pub_base_CalIntst(&g_pub_intst);
	if( ret )
	{
		sprintf(acErrMsg,"取到期利息值出错 ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"O194" );
		goto ErrExit;
	}

	sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>到期利息为=[%.2lf]", \
			g_pub_intst.keepval+g_pub_intst.dealval);
	WRITEMSG

	sprintf(acErrMsg,"CGG****** = [%.2lf], [%.2lf]", \
			g_pub_intst.dealval, g_pub_intst.val);
	WRITEMSG
	g_pub_tx.tx_date = tmp_date;

	if(g_td_parm.term_type[0]=='Y')
		tmp_term=g_td_parm.term*12;
	else if ( g_td_parm.term_type[0]=='M' )
		tmp_term=g_td_parm.term;
	strcpy(term_type,"月");
	vtcp_log("[%s] [%d] tmp_term=%d g_td_parm.term=%d\n",__FILE__,__LINE__,tmp_term,g_td_parm.term);

	fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%d|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", 
			g_pub_tx.ac_no,g_pub_tx.name,g_pub_tx.tx_amt1,g_td_parm.title,
			opn_date,g_td_mst.opn_br_no,g_td_mst.cif_no,
			mtr_date,tmp_term,term_type,rate_val,
			g_pub_tx.tx_amt1,g_cif_email_inf.email,draw_type,deac_type,
			cur_name,"","","",g_pub_intst.keepval+g_pub_intst.dealval, \
			"",g_pub_tx.ac_seqn,g_td_mst.ic_date,
			g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,0,g_pub_tx.brf,
			"","","","","","","","",auto_depo);

	/*** 一本通打印原介质 ***/
	if( !strncmp(mdm_code,"0016",4) )
	{
		/*** 取打印行 ***/
		ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", \
						 ac_id, ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld]",g_mdm_ac_rel.ac_id);
	   	 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"M003");
		 	goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		fprintf(fp,"YBTZ%ld|%s|%.2lf|%s|%.2lf|%s|%d|%s|tty%s|%d|%d|%d|%s|\n",
				g_pub_tx.tx_date,"转单",g_pub_tx.tx_amt1,"1",g_td_mst.bal,
				g_pub_tx.tel,0, g_pub_tx.tx_br_no,g_pub_tx.tty,ac_seqn,
				g_td_mst.book_line,1, "2401");
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");   
	/*打印段结束*/

	/** 开户回显 **/
	set_zd_data("1167",g_pub_tx.ac_no);
	set_zd_int("103D",g_pub_tx.ac_seqn);

	/***登记交易流水***/
	if( pub_ins_trace_log() )
	{
	  	sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"转存单交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"转存单交易失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
