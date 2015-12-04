/***************************************************************
* 文 件 名: sp4541.c
* 功    能: 查询贷款抵押品登记簿，返回抵押金额和估计金额。
	    查询合同下的抵押品信息。
* 作    者: chenchao
* 完成日期：2011-8-5 16:01:25
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
insert into tx_def values('4541','查合同下的抵押品信息','10000000000000000000000000000000000000111110000000000000000000001010000000000000000000000000000000000000000000000000000000000000','0','6','2');
insert into tx_sub_def values('4541','查合同下的抵押品信息','sp4541','0','0');
insert into tx_flow_def values('4541','0','4541','#$');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "pact_gaga_rel_c.h"

int sp4541()
{
	int	ret=0;
	int	iUpd_flag=0;
	char	cPact_no[22];/*合同号*/
	char	cTxno[9];
	char	filename[300];
	char	cOutOrIn[2];	/**出入库标志 **/
	FILE	*fp = NULL;
	double	dTot_outamt=0.00; /* 入库总金额 */
	double	dTot_val=0.00;	/** 总价值 **/
	char	gaga_sts[16];

	struct ln_gage_reg_c sLn_gage_reg;
	struct pact_gaga_rel_c sPact_gaga_rel;

	memset(&sPact_gaga_rel, 0x00, sizeof(sPact_gaga_rel));
	memset(&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	memset(cPact_no, 0x00, sizeof(cPact_no));
	memset(cTxno, 0x00, sizeof(cTxno));
	memset(cOutOrIn, 0x00, sizeof(cOutOrIn));
	memset(gaga_sts, 0x00, sizeof(gaga_sts));
	memset(filename, 0x00, sizeof(filename));

	pub_base_sysinit();

	get_zd_data("0370", cPact_no); /**合同号 */
	get_zd_data("0650", cTxno);/* 交易代码 */

	vtcp_log(" %s, %d, 合同号[%s] 交易代码[%s] ",__FILE__,__LINE__,cPact_no,cTxno);
	if( memcmp( cTxno,"4543",4 ) == 0 )
	{
		cOutOrIn[0] = '0';
	}else{
		cOutOrIn[0] = 'X';
	}
	ret = Ln_gage_reg_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and sts='%s' ",cPact_no,cOutOrIn);
	if(ret)
	{
		sprintf(acErrMsg,"定义游标失败!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"定义游标失败!!!");
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	ret = Ln_gage_reg_Fet_Sel(&sLn_gage_reg,g_pub_tx.reply);
	if(ret != 100 && ret != 0)
	{
		sprintf(acErrMsg,"查询贷款抵押品登记簿错误");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"查询贷款抵押品登记簿错误");
		strcpy(g_pub_tx.reply, "D102");
		goto ErrExit;
	}
	else if(ret == 100)
	{
		sprintf(acErrMsg,"贷款抵押品登记簿中无此账户记录");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"贷款抵押品登记簿中无此账户记录");
		strcpy(g_pub_tx.reply, "L175");
		goto ErrExit;
	}
	vtcp_log(" %s, %d, 抵押金额[%lf] ",__FILE__,__LINE__,sLn_gage_reg.amt);

	pub_base_AllDwnFilName(filename);
	fp = fopen(filename,"w");
	if(fp == NULL)
	{
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	if( memcmp(cTxno,"4543",4) == 0 )
	{
		ret = Pact_gaga_rel_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and stsvar = '1'",cPact_no);
	}
	else
	{
		ret = Pact_gaga_rel_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and stsvar = 'X'",cPact_no);
	}
	if(ret)
	{
		sprintf(acErrMsg, "定义合同号和抵质押品明细关联关系表游标错误!cPact_no=[%s]",cPact_no);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"定义合同号和抵质押品明细关联关系表游标错误!");
		strcpy(g_pub_tx.reply, "L328");
		goto ErrExit;
	}
	while(1)
	{
		memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
		ret = Pact_gaga_rel_Fet_Sel(&sPact_gaga_rel, g_pub_tx.reply);
		if(ret == 100)
		{
			if(iUpd_flag)
			{
	 			break;
	 		}
	 		sprintf(acErrMsg, "没有找到抵押物信息");
			WRITEMSG
			set_zd_data(DC_GET_MSG, "没有找到抵押物信息");
			strcpy(g_pub_tx.reply, "L329");
			goto ErrExit;
		}
		else if(ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "取得借据号和抵质押品明细关联关系表游标错误！！");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"取得借据号和抵质押品明细关联关系表游标错误！！");
			strcpy(g_pub_tx.reply, "L329");
			goto ErrExit;
		}
		pub_base_strpack(sPact_gaga_rel.bill_no);
		if( sPact_gaga_rel.pact_type[0] == '2' && strlen(sPact_gaga_rel.bill_no) >0 && memcmp(sPact_gaga_rel.io_ind,"120",3)==0)
		{
			if( memcmp(cTxno,"4543",4) == 0 )
			{
				memcpy(gaga_sts,"待出库状态",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				fprintf(fp,"%s|%s|%lf|%lf|%s|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts,sPact_gaga_rel.bill_no);
			}else{
				memcpy(gaga_sts,"待入库状态",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				/* 添加状态已做部分抵押或新入库 */
				fprintf(fp,"%s|%s|%lf|%lf|%s|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts,sPact_gaga_rel.bill_no);
			}
			set_zd_data("0670","1");/*** 此处代表承兑的质押存单入库,需要跳往质押存单录入或出库 **/
		}else{
			if( memcmp(cTxno,"4543",4) == 0 )
			{
				memcpy(gaga_sts,"待出库状态",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				fprintf(fp,"%s|%s|%lf|%lf|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts);
			}else{
				memcpy(gaga_sts,"待入库状态",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				/* 添加状态已做部分抵押或新入库 */
				fprintf(fp,"%s|%s|%lf|%lf|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts);
			}
		}
		iUpd_flag++;
	}
	Pact_gaga_rel_Clo_Sel();

	set_zd_double("0420",dTot_outamt);/*入库金额 */
	set_zd_double("0430",dTot_val);/*入库总价值*/

	/*4543抵质押品出库不需要传文件*/
	if( fp!=NULL )
	{
		set_zd_data(DC_FILE_SND, "1");
		vtcp_log("[%s][%d]传文件结束，关闭文件",__FILE__,__LINE__);
		fclose(fp);
	}

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "交易成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}	
	sprintf(acErrMsg, "交易失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
