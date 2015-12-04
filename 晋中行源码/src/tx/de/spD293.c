/***************************************************************
* 文 件 名:     spD293.c
* 功能描述：	总行同城提出
* 作    者:     mike
* 完成日期：    2004年1月25日
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
#include "mo_city_c.h"

spD293()
{
	 double sum1,sum2,tmp_sum1,tmp_sum2;
	 int num1,num2,tmp_num1,tmp_num2,tmp_int,no;
	 FILE *fp;
	 char filename[50],tmpname[101];
	 char br_no[7];

	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct com_branch_c	g_com_branch;
	struct mo_city_c	g_mo_city;
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

	get_zd_data("0910",br_no);					/*网点代码w*/
	get_zd_int("0490",&no);						/*批次号*/
	get_zd_int("0350",&num1);					/*提出借方总张数*/
	get_zd_double("0410",&sum1);				/*提出借方总金额*/
	get_zd_int("0360",&num2);					/*提出贷方总张数*/
	get_zd_double("0420",&sum2);				/*提出贷方总金额*/

	/*借方自动传递票据个数*/
	tmp_num1=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and cd_ind='1' \
			and in_out_ind='1' and trans_mode='2' ",
			br_no,g_pub_tx.tx_date,no);
	if (tmp_num1<0)
	{
		sprintf(acErrMsg,"取借方自动传递票据个数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	if (tmp_num1!=num1)
	{
		sprintf(acErrMsg,"输入提出借方票据总张数与实际不符!!tmp_num1=[%d],num1=[%d]",tmp_num1,num1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}


	sprintf(acErrMsg,"OKOK!!br_no=[%s],g_pub_tx.tx_date=[%ld],no=[%d]",
			br_no,g_pub_tx.tx_date,no);
	WRITEMSG
TRACE
TRACE
	/*借方自动传递票据金额数*/
	sql_sum_double("mo_city","sttl_amt",&tmp_sum1,
		" tx_br_no='%s' and tx_date=%ld and no=%d and cd_ind='1' and in_out_ind='1' and trans_mode='2' ",
					br_no,g_pub_tx.tx_date,no);
	g_reply_int=pub_base_CompDblVal(tmp_sum1,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取借方自动传递票据金额数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	g_reply_int=pub_base_CompDblVal(sum1,tmp_sum1);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"输入提出票据借方总金额与实际不符!!tmp_sum1=[%lf],sum1=[%lf]",tmp_sum1,sum1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取自动提出贷方总笔数*/
	tmp_num2=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and cd_ind='2' \
			and trans_mode='2' ",br_no,g_pub_tx.tx_date,no);
	if (tmp_num2<0)
	{
		sprintf(acErrMsg,"取自动提出贷方总笔数错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	if (tmp_num2!=num2)
	{
		sprintf(acErrMsg,"输入提出贷方票据总张数与实际不符!!tmp_num2=[%d],num2=[%d]",tmp_num2,num2);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*取自动提出贷方总金额*/
	sql_sum_double("mo_city","sttl_amt",&tmp_sum2,
			" tx_br_no='%s' and tx_date=%ld and no=%d and cd_ind='2' \
			and in_out_ind='1' \
			and trans_mode='2' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,no);
	g_reply_int=pub_base_CompDblVal(sum2,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"取自动提出贷方总金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	if (sum2!=tmp_sum2)
	{
		sprintf(acErrMsg,"输入提出票据贷方总金额与实际不符!!tmp_sum2=[%lf],sum2=[%lf]",tmp_sum2,sum2);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOOOKOKOKKOKK");
	WRITEMSG

 	g_reply_int=Mo_city_Dec_Upd(g_pub_tx.reply,
				"tx_br_no='%s' and tx_date=%ld and no=%d",br_no,g_pub_tx.tx_date,no);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"DECLARE Mo_city_Dec_Upd 游标出错!!g_reply_int=[%d]",
				g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

 /*打印开始*/
 pub_base_GenDwnFilName(filename);
 pub_base_strpack(filename);
 sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
 fp=fopen(tmpname,"w");
 fprintf(fp,"`**********总行同城提出(非人工传递票据)汇总报单************|\n");
 fprintf(fp,"`机构编码:%s|提出信息录入机构:%s|日期:%ld|场次:%d|\n",
  			g_pub_tx.tx_br_no,g_com_branch.br_name,g_pub_tx.tx_date,no);
 fprintf(fp,"`----------------------------------------------------------|\n");
 fprintf(fp,"提出借贷| 票据种类| 票据号码|	付款人账户|	付款姓名| 付款人全程| 付款人行号| 付款人行名| 收款人账户| 收款人姓名| 收款人行号| 收款人行名|\n");

	while(1)
	{
 		g_reply_int=Mo_city_Fet_Upd(&g_mo_city,g_pub_tx.reply);
		if (g_reply_int==100)break;
		else if (g_reply_int)
		{
			sprintf(acErrMsg,"FETCH Mo_city_Dec_Sel 出错!!g_reply_int=[%d]!!",
					g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O099");
			goto ErrExit;
		}

 fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",g_mo_city.cd_ind,g_mo_city.bill_type,g_mo_city.bill_no,g_mo_city.payer_ac_no,g_mo_city.payer_name,g_mo_city.payer_br_no,g_mo_city.payer_br_name,g_mo_city.payee_ac_no,g_mo_city.payee_name,g_mo_city.payee_br_no,g_mo_city.payee_br_name);

	g_mo_city.sts[0]=='0';		/*0-录入状态*/

	g_reply_int=Mo_city_Upd_Upd(g_mo_city,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"UPDATE Mo_city_Upd_Upd 出错!!g_reply_int=[%d]!!",
				g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	}	/*while结束*/

 	fprintf(fp,"`-----------------------------------------------------------------|\n|\n");
 	fprintf(fp,"`合计:提出借方笔数|提出借方总金额|提出贷方笔数|提出贷方总金额|\n");
 	fprintf(fp,"`%d|%lf|%d|%lf|\n",num1,sum1,num2,sum2);
 	fprintf(fp,"`-----------------------------------------------------------------|\n");
 	fprintf(fp,"`操作员:%s		   打印日期:%ld|\n",g_pub_tx.tel,g_pub_tx.tx_date);

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Mo_city_Clo_Sel ();

 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"登记交易流水错误!");
  		WRITEMSG
    	goto ErrExit;
 	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"总行同城提出成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"总行同城提出失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
