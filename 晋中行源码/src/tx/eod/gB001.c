/*************************************************************
* 文 件 名: gB001.c
* 功能描述：中长期债券投资计提利息，摊销折价溢价
*
* 作    者: jane
* 完成日期: 20030610
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

struct	mo_invest_c	sMo_invest;

gB001()
{
	int ret = 0;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	strcpy(g_pub_tx.cur_no,RMB);
	strcpy(g_pub_tx.tx_code,"B001");
	strcpy(g_pub_tx.sub_tx_code,"B001");
	strcpy(g_pub_tx.chk,"000");
	strcpy(g_pub_tx.auth,"000");
	g_pub_tx.trace_cnt=0;
	strcpy(g_pub_tx.ct_ind,"2");

	/**------- 业务处理 --------------**/
	ret=pub_eod_invest();
	if( ret ) 
	{
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_eod_invest()
{
	int mouth=0,day=0;
	int ret;
	memset(&sMo_invest,0x00,sizeof(struct mo_invest_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	/*** 季末21日执行 ***/
	mouth=g_pub_tx.tx_date%10000/100;
	day=g_pub_tx.tx_date/100;
	if( mouth!=3 && mouth!=6 && mouth!=9 && mouth!=12 )
	{
		sprintf( acErrMsg, "未到季末不能执行[%d]!!!",mouth );
		WRITEMSG
		return 0;
	}
	if( g_pub_tx.tx_date%100!=21 )
	{
		sprintf( acErrMsg, "未到日期不能执行[%d]!!!",day );
		WRITEMSG
		return 0;
	}

	ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"bkrl_code='%s' and sts='2' ",INVEST_LONG);
	if( ret )
	{
		sprintf( acErrMsg, "打开投资登记薄错误[%d]!!!",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B036");
		goto ErrExit;
	}

	while(1)
	{
		ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf( acErrMsg, "打开投资登记薄错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( sMo_invest.abate_ac_id!=0 && sMo_invest.overflow_ac_id!=0 )
		{
			sprintf( acErrMsg, "折价账号溢价账号同时存在[%d][%d]!!!",sMo_invest.abate_ac_id,sMo_invest.overflow_ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s' ",INVEST_LONG);
		if( ret )
		{
			sprintf( acErrMsg, "取产品参数错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"W035");
			goto ErrExit;
		}

		vtcp_log("sMo_invest.pre_intst=[%lf]",sMo_invest.pre_intst);
		ret=pub_eod_invest_intst(sMo_invest);
		if( ret )
		{
			goto ErrExit;
		}
			
		vtcp_log("sMo_invest.pre_intst=[%lf]",sMo_invest.pre_intst);
		ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "打开投资登记薄错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
	}
	Mo_invest_Clo_Upd();

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_eod_invest_intst()
{
	double	intst_amt=0.00,tmp_amt=0.00;	/*** 应计提利息 ***/
	double	over_amt=0.00;					/*** 溢价摊销金额 ***/
	double	abate_amt=0.00;					/*** 折价摊销金额 ***/
	int		ret;
	int		txcs=0;							/*** 摊销次数 ***/

	if( sMo_invest.intst_type[0]=='1' )		/*** 贴现债券 ***/
		intst_amt=0.00;
	else									/*** 到期付息债券 ***/
	{
		ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%d",sMo_invest.ac_id);
		if( ret )
		{
			sprintf( acErrMsg, "取投资账号主文件错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
		if( ret==100 ) { intst_amt=0.00; goto OVER_AMT; }
		if( ret )
		{
			sprintf( acErrMsg, "取投资账号主文件错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		/**----------- 计算利息积数 ------------**/
		if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
			g_in_parm.cal_day_type,g_in_parm.acm_type,
			g_in_mst.bal,0.00,&g_in_mst.intst_acm,
			g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

		tmp_amt = sMo_invest.rate * g_in_mst.intst_acm / L360 / L100;
		intst_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		g_in_mst.lst_date=g_pub_tx.tx_date;
		g_in_mst.ic_date=g_pub_tx.tx_date;
		g_in_mst.intst_acm=0.00;		/*** 积数清零 ***/
		ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "取投资账号主文件错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		In_mst_Clo_Upd();

		/**------------ 计提利息记账 -------------**/
		strcpy(g_pub_tx.ac_no,"14204");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=intst_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"日终中长期债券投资计提利息");
		strcpy(g_pub_tx.ct_ind,"2");		/*** 转账 ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** 人民币 ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*本程序未读取分户*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*不核对凭证号，零金额不计流水、明细*/
		strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

OVER_AMT:
	sMo_invest.pre_intst=intst_amt;
	if( sMo_invest.overflow_ac_id==0 )			/*** 非溢价 ***/
		over_amt=0.00;
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_invest.overflow_ac_id);
		if( ret==100 ) { over_amt=0.00; goto ABATE_AMT; }
		if( ret )
		{
			sprintf( acErrMsg, "取投资溢价账号主文件错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( pub_base_CompDblVal(g_in_mst.bal,0.00)==0 || sMo_invest.mtr_date<g_pub_tx.tx_date )
		{ over_amt=0.00; goto ABATE_AMT; }

		/**--------- 计算摊销次数 ----------**/
		txcs=pub_CallNum(sMo_invest.mtr_date);
		if( txcs<0 )
		{
			sprintf( acErrMsg, "计算摊销次数错误[%d]!!!",txcs );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		tmp_amt = g_in_mst.bal / txcs;
		over_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		/**------------ 溢价摊销记账 -------------**/
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.overflow_ac_id;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=over_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"日终中长期债券投资溢价摊销");
		strcpy(g_pub_tx.ct_ind,"2");		/*** 转账 ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** 人民币 ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*本程序未读取分户*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*不核对凭证号，零金额不计流水、明细*/
		strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

ABATE_AMT:
	sMo_invest.overflow_amt=sMo_invest.overflow_amt-over_amt;
	if( sMo_invest.abate_ac_id==0 )			/*** 非折价 ***/
		abate_amt=0.00;
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_invest.abate_ac_id);
		if( ret==100 ) { abate_amt=0.00; goto AMT_514; }
		if( ret )
		{
			sprintf( acErrMsg, "取投资折价账号主文件错误[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( pub_base_CompDblVal(g_in_mst.bal,0.00)==0 || sMo_invest.mtr_date<g_pub_tx.tx_date )
		{ abate_amt=0.00; goto AMT_514; }

		/**--------- 计算摊销次数 ----------**/
		txcs=pub_CallNum(sMo_invest.mtr_date);
		if( txcs<0 )
		{
			sprintf( acErrMsg, "计算摊销次数错误[%d]!!!",txcs );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		tmp_amt = g_in_mst.bal / txcs;
		abate_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		/**------------ 溢价摊销记账 -------------**/
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.abate_ac_id;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=abate_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"日终中长期债券投资折价摊销");
		strcpy(g_pub_tx.ct_ind,"2");		/*** 转账 ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** 人民币 ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*本程序未读取分户*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*不核对凭证号，零金额不计流水、明细*/
		strcpy(g_pub_tx.add_ind,"2");/*贷方标志*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}
AMT_514:
	sMo_invest.abate_amt=sMo_invest.abate_amt-abate_amt;
	/**------------ 折价摊销记账 -------------**/
	strcpy(g_pub_tx.ac_no,"51401");
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=intst_amt+abate_amt-over_amt;
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy( g_pub_tx.end_note_no,"");
	strcpy(g_pub_tx.brf,"日终中长期债券投资计提摊销");
	strcpy(g_pub_tx.ct_ind,"2");		/*** 转账 ***/
	strcpy(g_pub_tx.cur_no,"01");		/*** 人民币 ***/
	strcpy( g_pub_tx.ac_id_type,"9" );	/*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" );	/*本程序未读取分户*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*不核对凭证号，零金额不计流水、明细*/
	strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("1214",g_pub_tx.cur_no);
	set_zd_data("1215",g_pub_tx.ct_ind);
	set_zd_double("1218",g_pub_tx.tx_amt1);
	ret=pubf_acct_proc("A017");
	if( ret )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( acErrMsg, "中长期债券计提利息摊销模块成功退出!!!" );
	WRITEMSG
	return 0;
ErrExit:
	sprintf( acErrMsg, "中长期债券计提利息摊销模块错误退出!!!" );
	WRITEMSG
	return 1;
}

int pub_CallNum(long date1)
{
	int Mouths=0;
	int	cs;

	Mouths=pub_base_CalMouths(g_pub_tx.tx_date,date1);
	cs=Mouths/3;
	if( cs<0 )
		return -1;
	else if( cs==0 ) cs=1;
	else if( cs>0 && date1%100>g_pub_tx.tx_date%100 )
		cs++;
	
	return cs;
}
