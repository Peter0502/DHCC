/*************************************************
* 文 件 名: spL542.c
* 功能描述：贷款抵押品出库
*
* 作    者: chenchao
* 完成日期：2011-8-4 11:01:39
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
insert into tx_def values('4543','抵质押品出库','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','2','2');
insert into tx_sub_def values('L543','抵质押品出库','spL543','1','0');
insert into tx_flow_def values('4543','0','L543','#$');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_mst_c.h"

int spL543()
{
	int	ret=0;
	char	cKinbr[BRNO_LEN+1];
	char	cPactno[20];
	int	iUpd_flag=0;
	double	intst=0.00;
	double	tmp_amt=0.00;
	double	dGaga_max_bal=0.00;
	double	dOut_bal=0.00;/*** 前台传出库金额  ***/
	
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	ln_mst_c		sLn_mst;

	memset(cKinbr  , 0 , sizeof(cKinbr));

	memset(&sPact_gaga_rel,0x00,sizeof(struct pact_gaga_rel_c));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	memset(cPactno,0x00,sizeof(cPactno)); 

	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG

	/** 取值、赋值 **/
	get_zd_data("0030",cKinbr);
	get_zd_data("0370",cPactno);
	get_zd_double("0390",&dOut_bal);

	sprintf(acErrMsg,"取值、赋值 PASS!dOut_bal=[%.2f]cPactno=[%s]",dOut_bal,cPactno);
	WRITEMSG
	/* 因为出库完全是通过信贷控制的因此不在按传回的文件处理 */
	/** 处理流程 **/
	ret = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no = '%s'",cPactno);
	if (ret != 100 && ret != 0)
	{
		sprintf(acErrMsg,"查询贷款抵押品登记簿错误");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	} 
	
	while(1)	/** 有可能一笔合同对以多比借据(贷款),因此在这儿要全部处理 **/
	{
		tmp_amt=0.00;
		intst=0.00;
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		ret = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);	
		if(ret == 100)
		{
			if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "没有该笔合同或协议的出库库记录!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"没有该笔合同或协议的出库库记录!!!");
			strcpy( g_pub_tx.reply, "L353" );
			goto ErrExit;
		}else if(ret)
		{
			sprintf( acErrMsg, "取游标出错!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"取游标出错!!!");
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
        	if(sLn_gage_reg.ac_id>0)
        	{
        		memset(&sLn_mst,0x00,sizeof(sLn_mst));
        		ret = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"ac_id=%ld",sLn_gage_reg.ac_id);
        		if(ret)
			{
				sprintf( acErrMsg, "取游标出错!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"取游标出错!!!");
				strcpy( g_pub_tx.reply, "D102" );
				goto ErrExit;
			}
			tmp_amt=sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst+sLn_mst.bal;
			if(pub_base_CompDblVal(tmp_amt,0.00)>0 && sLn_mst.intst_type[0]!='0')
			{
				sprintf( acErrMsg, "未还清不允许销户!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"未还清不允许销户!!!");
				strcpy( g_pub_tx.reply, "L409" );
				goto ErrExit;
			}
			if (pub_base_CompDblVal(sLn_mst.intst_acm+sLn_mst.in_lo_acm+sLn_mst.out_lo_acm+sLn_mst.cmpd_lo_acm , 0) && sLn_mst.intst_type[0]!='0')
			{
				sprintf( acErrMsg, "贷款有积数未还清,不能销户[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L051");
				ERR_DEAL
			}
				/* 分段积数利息 */
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "1" , &intst , 1 );
			if( ret ) goto ErrExit;
			if ( pub_base_CompDblVal(intst , 0.00) )
			{
				sprintf( acErrMsg, "贷款利息未还清,不能销户[%lf]",intst);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "2" , &intst , 1 );	
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00) )
			{
				sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "3" , &intst , 1 );
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00))
			{
				sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "4" , &intst , 1 );
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00))
			{
				sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			/* 若为按揭贷款 */
			if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf(acErrMsg,"按揭贷款");
				WRITEMSG
			
				/* 查询按揭欠款表 */
				ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_pub_tx.ac_id,g_pub_tx.ac_seqn );
				if (ret)
				{
					sprintf( acErrMsg, "该按揭贷款存在欠款,不能销户[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L053");
					ERR_DEAL
				}		
			}
	
        	}
		/*更新出库标志*/
		sLn_gage_reg.amt=0.00;
		if(memcmp(g_pub_tx.tx_code,"4374",4)==0)
		{
			ret = sql_count("pact_gaga_rel"," pact_no='%s' and stsvar ='1' and (io_ind<>'120' or io_ind is null)",cPactno);
			if(ret==0)
			{
				strcpy(sLn_gage_reg.sts,"1");
			}else if(ret>0){
				strcpy(sLn_gage_reg.sts,"0");
			}else{
					sprintf( acErrMsg, "统计数量出错!!!");
					WRITEMSG
					strcpy( g_pub_tx.reply, "D102" );
					goto ErrExit;
			}
  	
		}else{
			strcpy(sLn_gage_reg.sts,"1");
		}

		sLn_gage_reg.out_tx_date=g_pub_tx.tx_date;
		sLn_gage_reg.out_trace_no=g_pub_tx.trace_no;
		ret=Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"更新贷款抵押品登记簿错误",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D105");
			Ln_gage_reg_Clo_Upd();
			goto ErrExit;
		}
		iUpd_flag++;
	}
	Ln_gage_reg_Clo_Upd();
	iUpd_flag = 0;
	if(memcmp(g_pub_tx.tx_code,"4374",4)==0)
	{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and stsvar='1' and io_ind='120' and pact_type='2'",cPactno);
	}else{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and stsvar='1'",cPactno);
	}
	if ( ret )
	{
		sprintf(acErrMsg,"定义借据号和抵质押品明细关联关系表游标错误");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	while (1)
	{
		memset(&sPact_gaga_rel,0,sizeof(sPact_gaga_rel));
		ret=Pact_gaga_rel_Fet_Upd(&sPact_gaga_rel,g_pub_tx.reply);
		if (ret && ret != 100)
		{
			sprintf(acErrMsg,"取得借据号和抵质押品明细关联关系表游标错误");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L329");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}else if(ret == 100)
		{	if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "没有该笔合同或协议的出库库记录!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"没有该笔合同或协议的出库库记录!!!");
			strcpy( g_pub_tx.reply, "L353" );
			goto ErrExit;
		}
		memcpy(sPact_gaga_rel.stsvar,"2", 1);
		ret=Pact_gaga_rel_Upd_Upd(sPact_gaga_rel,g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"更新借据号和抵质押品明细关联关系表错误");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L345");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}
		dGaga_max_bal += sPact_gaga_rel.gaga_bal;
		iUpd_flag++;
	}
	Pact_gaga_rel_Clo_Upd();
	if(pub_base_CompDblVal(dOut_bal,dGaga_max_bal) && sPact_gaga_rel.pact_type[0] == '1')
	{
		vtcp_log("[%s][%d]出库抵押金额不符,[%lf],gaga[%lf]",__FILE__,__LINE__,dOut_bal,dGaga_max_bal);
		strcpy(g_pub_tx.reply,"SN07");
		goto ErrExit;
	}

	/* 记账 借70400 */
	set_zd_data( "0210" , "01" );
	memcpy(g_pub_tx.cur_no,"01",2);
	g_pub_tx.tx_amt1 = dOut_bal;
	memcpy(g_pub_tx.ac_no,cPactno,sizeof(g_pub_tx.ac_no)-1);
	set_zd_double("0390",dOut_bal);
	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.add_ind , "0" );        /* 增减:0减1加2销户 */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.ct_ind , "2" );        /* 现转:1现2转 */
	strcpy ( g_pub_tx.prt_ind , "0" );/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3061;            /* 贷款抵押品金额出库 */
	strcpy ( g_pub_tx.brf,"抵押品出库" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS![%.2f]",dGaga_max_bal);
	WRITEMSG

	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG

	/*** 写流水 ***/
	g_pub_tx.tx_amt1 = dGaga_max_bal;
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"写流水错误!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"贷款抵押品出库成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"H034",4);
	}	
	sprintf(acErrMsg,"贷款抵押品出库失败！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

