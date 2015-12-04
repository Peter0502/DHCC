/*************************************************
* 文 件 名:     pubf_eod_put_intst.c
* 功能描述：    日终预提利息主控(每月21预提)
* 作    者:     power
* 完成日期：    2004年07月05日
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int pub_eod_put_intst()
{
	int	ret=0,year=0,mouth=0,day=0;
	long put_beg_date=0;
	long put_end_date=0;
	struct com_branch_c	sCom_branch;
	memset(&sCom_branch,0x00,sizeof(struct com_branch_c));

	year=g_pub_tx.tx_date/10000;
	mouth=g_pub_tx.tx_date%10000/100;
	day=g_pub_tx.tx_date%100;

	pub_base_deadlineD(g_pub_tx.tx_date,-1,&put_end_date);
	mouth=mouth-1;
	if( mouth<0 )
	{
		mouth=mouth+12;
		year--;
	}
	put_beg_date=year*10000+mouth*100+day;

	ret=Com_branch_Dec_Sel(g_pub_tx.reply," br_type!='0' and wrk_sts!='*' ");
	if( ret )
	{
		sprintf(acErrMsg," com_branch_dec_sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O092");
		return 1;
	}

	while(1)
	{
		ret=Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
		if( ret==100 )	break;
		if( ret )
		{
			sprintf(acErrMsg," com_branch_fet_sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O092");
			return 1;
		}
		strcpy(g_pub_tx.tx_br_no,sCom_branch.br_no);
		strcpy(g_pub_tx.opn_br_no,sCom_branch.br_no);

		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
         	 sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
         	 WRITEMSG
			 return 1;
		}
		set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);   /* 流水号 */

		ret=pub_sub_put_intst_dd(sCom_branch.br_no,put_beg_date,put_end_date);
		if( ret )
		{
			sprintf(acErrMsg," pub_sub_put_intst_dd erro");
			WRITEMSG
			return 1;
		}

		ret=pub_sub_put_intst_td(sCom_branch.br_no,put_beg_date,put_end_date);
		if( ret )
		{
			sprintf(acErrMsg," pub_sub_put_intst_td erro");
			WRITEMSG
			return 1;
		}
	}
	Com_branch_Clo_Sel();
	set_zd_data("1204","");
	set_zd_data("1205","");
	set_zd_double("1208",0.00);
	set_zd_data("1214","");
	set_zd_data("1215","");
	set_zd_double("1218",0.00);

	return 0;
}

int pub_sub_put_intst_dd(char br_no[6],long put_beg_date,long put_end_date)
{
	int ret=0;
	double intst_acm=0.00;
	double rate_val=0.00;
	double put_intst=0.00;

	ret=Dd_parm_Dec_Sel(g_pub_tx.reply," put_ind='Y' order by prdt_no" );
	if( ret )
	{
		sprintf(acErrMsg," dd_parm_Dec_Sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W025");
		return 1;
	}

	while(1)
	{
		ret=Dd_parm_Fet_Sel(&g_dd_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"dd_parm_Fet_Sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W025");
			return 1;
		}
		if( g_dd_parm.intst_type[0]=='0' )	/** 不计息 **/
			continue;
		if( g_dd_parm.put_ind[0]=='N' )		/** 不预提 **/
			continue;
		strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);

		/*** 计算总利息积数 ***/
		ret=sql_sum_double("gl_prdt_dyn_hst","cr_bal",&intst_acm, \
			" \"date\"<=%ld and \"date\">=%ld and br_no='%s' and prdt_cod='%s'", \
			put_end_date,put_beg_date,br_no,g_dd_parm.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"计算利息积数错误 [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P425");
			return 1;
		}
		if( pub_base_CompDblVal(intst_acm,0.00)==0 )
			continue;

		/*** 取利率 ***/
		ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret )
		{
			sprintf(acErrMsg,"pub_base_getllz erro [%d]",ret);
			WRITEMSG
			return 1;
		}
		put_intst=intst_acm*rate_val/360.00/100.00;
		g_pub_tx.tx_amt1=pub_base_PubDround_1(put_intst);

		if( g_dd_parm.cif_type[0]=='1' || g_dd_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"52103");		/** 个人活期 **/
		else 
			strcpy(g_pub_tx.ac_no,"52101");		/** 对公活期 **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"0");			/** 借方 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证,零金额不计流水,明细*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"调用登记会计流水错误!");
			WRITEMSG
			return 1;
		}

		if( g_dd_parm.cif_type[0]=='1' || g_dd_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"26004");	/** 个人活期 **/
		else 
			strcpy(g_pub_tx.ac_no,"26003");	/** 对公活期 **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"1");	/** 贷方 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证,零金额不计流水,明细*/
		strcpy( g_pub_tx.prt_ind,"0" ); 	/*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			return 1;
		}

		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"调用登记会计流水错误!");
			WRITEMSG
			return 1;
		}
	}
	Dd_parm_Clo_Sel();

	return 0;
}

int pub_sub_put_intst_td(char br_no[6],long put_beg_date,long put_end_date)
{
	int ret=0;
	double intst_acm=0.00;
	double rate_val=0.00;
	double put_intst=0.00;

	ret=Td_parm_Dec_Sel(g_pub_tx.reply," put_ind='Y' order by prdt_no" );
	if( ret )
	{
		sprintf(acErrMsg," td_parm_Dec_Sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W027");
		return 1;
	}

	while(1)
	{
		ret=Td_parm_Fet_Sel(&g_td_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"td_parm_Fet_Sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W027");
			return 1;
		}
		if( g_td_parm.intst_type[0]=='0' )	/** 不计息 **/
			continue;
		if( g_td_parm.put_ind[0]=='N' )		/** 不预提 **/
			continue;
		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);

		/*** 计算总利息积数 ***/
		ret=sql_sum_double("gl_prdt_dyn_hst","cr_bal",&intst_acm, \
			" \"date\"<=%ld and \"date\">=%ld and br_no='%s' and prdt_cod='%s'", \
			put_end_date,put_beg_date,br_no,g_td_parm.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"计算利息积数错误 [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P425");
			return 1;
		}
		vtcp_log("Hao --- look [%lf]",intst_acm);
		if( pub_base_CompDblVal(intst_acm,0.00)==0 )
			continue;
		/*** zz移植数据少21-26的积数 ***/
		if( g_pub_tx.tx_date==20040921 )
		{
			intst_acm=intst_acm*(1+0.2);
		}

		/*** 取利率 ***/
		ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret )
		{
			sprintf(acErrMsg,"pub_base_getllz erro [%d]",ret);
			WRITEMSG
			return 1;
		}
		put_intst=intst_acm*rate_val/360.00/100.00;
		g_pub_tx.tx_amt1=pub_base_PubDround_1(put_intst);
		vtcp_log("Hao --- look [%lf]",rate_val);
		vtcp_log("Hao --- look [%lf]",g_pub_tx.tx_amt1);

		if( !strcmp(g_td_parm.prdt_no,"242") 
			|| !strcmp(g_td_parm.prdt_no,"243")
			|| !strcmp(g_td_parm.prdt_no,"244")
			|| !strcmp(g_td_parm.prdt_no,"281")
			|| !strcmp(g_td_parm.prdt_no,"282")
			|| !strcmp(g_td_parm.prdt_no,"283") )
			strcpy(g_pub_tx.ac_no,"52105");		/** 通知存款 **/
		else if( g_td_parm.cif_type[0]=='1' || g_td_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"52104");		/** 个人定期 **/
		else 
			strcpy(g_pub_tx.ac_no,"52102");		/** 对公定期 **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"0");			/** 借方 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证,零金额不计流水,明细*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"调用登记会计流水错误!");
			WRITEMSG
			return 1;
		}

		if( g_td_parm.cif_type[0]=='1' || g_td_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"26002");		/** 个人定期 **/
		else 
			strcpy(g_pub_tx.ac_no,"26001");		/** 对公定期 **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"1");			/** 贷方 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证,零金额不计流水,明细*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"调用登记会计流水错误!");
			WRITEMSG
			return 1;
		}
	}
	Dd_parm_Clo_Sel();

	return 0;
}
