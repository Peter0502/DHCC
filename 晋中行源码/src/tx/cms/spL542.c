/***************************************************************
* 文 件 名: spL542.c
* 功    能: 抵质押品入库
* 作    者: chenchao 
* 完成日期：2011-8-5 17:03:17
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
insert into tx_def values('4542','抵质押品入库','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','2','2');
insert into tx_sub_def values('L542','抵质押品入库','spL542','1','0');
insert into tx_flow_def values('4542','0','L542','#$');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "pact_gaga_rel_c.h"
#include "com_item_c.h"

int spL542()
{
	int	ret=0;
	char	cPact_no[21];	/*合同号*/
	double	dValue=0.00;     /* 抵质押总价值 */
	double	dIn_amt=0.00;    /*入库金额 */
	double	dGaga_tot_bal=0.00;
	int	iUpd_flag=0;

	struct pact_gaga_rel_c sPact_gaga_rel;
	struct ln_gage_reg_c sLn_gage_reg;

	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	memset(cPact_no,0x00,sizeof(cPact_no));

	pub_base_sysinit();

	get_zd_data("0370", cPact_no);	/* 合同号 */
	get_zd_double("0430", &dValue);	/* 抵质押总价值*/
	get_zd_double("0420", &dIn_amt);/* 抵质押总金额*/	

	vtcp_log(" %s, %d, 合同号[%s] 抵质押总价值[%.2lf] 入库金额[%.2lf] ",__FILE__,__LINE__,cPact_no,dValue,dIn_amt);
	ret=Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='X'",cPact_no);
	if ( ret )
	{
		sprintf(acErrMsg,"定义贷款抵押品登记簿游标错误");
		WRITEMSG
			strcpy(g_pub_tx.reply, "L342");
		goto ErrExit;
	}
	while(1)
	{
		ret=Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);
		if (ret && ret!=100)
		{
			Ln_gage_reg_Clo_Upd();
			sprintf(acErrMsg,"取游标出错!!!");
			WRITEMSG
				strcpy(g_pub_tx.reply, "L211");
			goto ErrExit;
		}else if(ret == 100)
		{
			if(iUpd_flag)
			{
				break;
			}
			Ln_gage_reg_Clo_Upd();
			sprintf(acErrMsg,"没有该借据!!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L211");
			goto ErrExit;
		}
		if(memcmp(g_pub_tx.tx_code,"4345",4)==0)
		{
			ret = sql_count("pact_gaga_rel"," pact_no='%s' and stsvar ='X' and (io_ind<>'120' or io_ind is null)",cPact_no);
			if(ret==0)
			{
				strcpy(sLn_gage_reg.sts,"0");
			}else{
				strcpy(sLn_gage_reg.sts,"X");
			}
		}else{
			strcpy(sLn_gage_reg.sts,"0");
		}
		sLn_gage_reg.amt=dIn_amt;
		sLn_gage_reg.in_trace_no = g_pub_tx.trace_no;
		sLn_gage_reg.in_tx_date=g_pub_tx.tx_date;
		vtcp_log(" %s, %d, 抵质押总金额[%lf] ",__FILE__,__LINE__,sLn_gage_reg.amt);
        	
		ret=Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"更新贷款抵押品登记簿错误",ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "L344");
			Ln_gage_reg_Clo_Upd();
			goto ErrExit;
		}
		iUpd_flag++;
	}
	Ln_gage_reg_Clo_Upd();
	if(memcmp(g_pub_tx.tx_code,"4345",4)==0)
	{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and stsvar='X' and io_ind='120' and pact_type='2'",cPact_no);
	}else{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and stsvar='X' ",cPact_no);
	}
		
	if ( ret )
	{
		sprintf(acErrMsg,"定义借据号和抵质押品明细关联关系表游标错误");
		WRITEMSG
		strcpy(g_pub_tx.reply, "L328");
		goto ErrExit;
	}
	while (1)
	{
		memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
		ret=Pact_gaga_rel_Fet_Upd(&sPact_gaga_rel,g_pub_tx.reply);
		if (ret && ret != 100)
		{
			sprintf(acErrMsg,"取得借据号和抵质押品明细关联关系表游标错误");
			WRITEMSG
				strcpy(g_pub_tx.reply, "L329");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}
		strcpy(sPact_gaga_rel.stsvar,"1");

		ret=Pact_gaga_rel_Upd_Upd(sPact_gaga_rel,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"更新借据号和抵质押品明细关联关系表错误",ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "L345");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}
		dGaga_tot_bal+=sPact_gaga_rel.gaga_bal;
	}
	Pact_gaga_rel_Clo_Upd();

	if (pub_base_CompDblVal(dGaga_tot_bal,dIn_amt) && sPact_gaga_rel.pact_type[0]=='1')
	{
		vtcp_log(" %s, %d, 入库金额不符:[%lf] [%lf] ",__FILE__,__LINE__,dGaga_tot_bal,dIn_amt);
		strcpy(g_pub_tx.reply,"SN05");
		goto ErrExit;
	}

	/* 记账 借70400 */
	set_zd_data("0210","01");
	strcpy(g_pub_tx.cur_no,"01");
	set_zd_double("0420",dIn_amt);

	g_pub_tx.tx_amt1 = dIn_amt; 
	/* pub_tx结构赋值 */

	strcpy ( g_pub_tx.add_ind , "1" );        /* 增减:0减1加2销户 */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.ct_ind , "2" );        /* 现转:1现2转 */
	strcpy ( g_pub_tx.prt_ind , "0" );	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3061;            /* 贷款抵押品金额入库 */
	strcpy ( g_pub_tx.brf,"抵押品入库" );

	strcpy(g_pub_tx.ac_no,cPact_no);
	vtcp_log(" %s, %d, 合同号[%s]登记流水! ",__FILE__,__LINE__,cPact_no);
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"写流水错误!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "交易成功![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "交易失败![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
