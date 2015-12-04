/***************************************************************
* 文 件 名:     spD292.c
* 功能描述：	打印本行提出汇总报单
* 作    者:     jack
* 完成日期：    2004年1月24日
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
#include "com_branch_c.h"

spD292()
{
	 double sum1,sum2,sum3,sum4;
	 int count1,count2,count3,count4,tmp_int;
	 FILE *fp;
	 char filename[50];
	 char tmpname[101];

	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct com_branch_c	g_com_branch;
	struct dd_parm_c	g_dd_parm;
	struct td_parm_c	g_td_parm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	sprintf(acErrMsg,"g_pub_tx.tx_br_no=[%s]!!",g_pub_tx.tx_br_no);
	WRITEMSG

	g_reply_int=Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",
									g_pub_tx.tx_br_no);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"定义公共机构表游标异常!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O090");
		goto ErrExit;
	}

	g_reply_int=Com_branch_Fet_Upd(&g_com_branch,g_pub_tx.reply);
	if (g_reply_int==100)
	{
		sprintf(acErrMsg,"公共机构表中不存在该记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O093");
		goto ErrExit;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"FETCH查询公共机构表异常!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O089");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	/****************************手工传递票据汇总*************************/
	/*取提出借方总笔数*/
	count1=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and trans_mode='1' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count1<0)
	{
		sprintf(acErrMsg,"取提出借方总笔数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出借方总金额*/
	sql_sum_double("mo_city","stll_amt",&sum1,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='1' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum1,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取提出借方总金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出贷方总笔数*/
	count2=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='2' and trans_mode='1' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count2<0)
	{
		sprintf(acErrMsg,"取提出贷方总笔数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出贷方总金额*/
	sql_sum_double("mo_city","stll_amt",&sum2,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum2,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取提出贷方总金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOOOKOKOKKOKK");
	WRITEMSG

	/****************************自动传递票据汇总*************************/
	/*取提出借方总笔数*/
	count3=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and trans_mode='2' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count1<0)
	{
		sprintf(acErrMsg,"取提出借方总笔数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出借方总金额*/
	sql_sum_double("mo_city","stll_amt",&sum3,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum3,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取提出借方总金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出贷方总笔数*/
	count4=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='2' and trans_mode='2' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count2<0)
	{
		sprintf(acErrMsg,"取提出贷方总笔数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取提出贷方总金额*/
	sql_sum_double("mo_city","stll_amt",&sum4,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='2' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum4,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取提出贷方总金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

 /*打印显示部分*/
 pub_base_GenDwnFilName(filename);
 pub_base_strpack(filename);
 sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
 fp=fopen(tmpname,"w");

 fprintf(fp,"*************************同城提出汇总报单************************|\n");
 fprintf(fp,"机构编码:%s		网点名称:%s				日期:%ld		场次:%d|\n",
  g_pub_tx.tx_br_no,g_com_branch.br_name,g_pub_tx.tx_date,g_com_branch.city_no);
 fprintf(fp,"-----------------------------------------------------------------|\n");
 fprintf(fp,"------------------------人工传递票据汇总-------------------------|\n");
 fprintf(fp,"提出借方笔数    提出借方总金额	    提出贷方笔数    提出贷方总金额|\n");
 fprintf(fp,"%d				  %lf				  %d			 %lf 		  |\n",
			count1,sum1,count2,sum2);

 fprintf(fp,"-----------------------------------------------------------------|\n|\n");
 fprintf(fp,"------------------------自动传递票据汇总-------------------------|\n");
 fprintf(fp,"提出借方笔数	  提出借方总金额	   提出贷方笔数	   提出贷方总金额\n");
 fprintf(fp,"%d				%lf				%d				%lf|\n",
			count3,sum3,count4,sum4);
 fprintf(fp,"-----------------------------------------------------------------|\n");
 fprintf(fp,"操作员:%s									          打印日期:%ld|\n",
			g_pub_tx.tel,g_pub_tx.tx_date);

	g_com_branch.city_no++;						/*同城批次加一*/

	g_reply_int=Com_branch_Upd_Upd(g_com_branch,g_pub_tx.reply);				
	if (g_reply_int)
	{
		sprintf(acErrMsg,"UPDATE公共机构码表出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Com_branch_Clo_Upd();

 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"登记交易流水错误!");
  		WRITEMSG
    	goto ErrExit;
 	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印本行提出汇总报单成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"打印本行提出汇总报单失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
