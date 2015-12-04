/*************************************************
* 文 件 名: sp6684.c
* 功能描述：此交易完成信贷审批出库
*
* 作    者: chenchao
* 完成日期：2011-8-4 17:50:31
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6684','信贷审批出库','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6684','0','6684','#$');
insert into tx_sub_def values ('6684','信贷审批出库','sp6684','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_gage_reg_c.h"
#include "ln_mst_c.h"

int sp6684()
{
	int	iRet = 0;
	int	iUpd_flag = 0;
	char	cGet_data[201];
	
	
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	cms_rollback_c		sCms_rollback;
	struct	ln_mst_c		sLn_mst;
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	
	memset(&sLn_mst,0x00,sizeof(sLn_mst));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	
	get_zd_data("0380",cGet_data);			/** 贷款合同号 || 承兑协议编号 **/
	zip_space(cGet_data);
	memcpy(sLn_gage_reg.pact_no,cGet_data,sizeof(sLn_gage_reg.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0670",sLn_gage_reg.gage_type);	/** 抵押物类型 1:抵押 2:质押 **/
	
	get_zd_data("0370",cGet_data);			/** 抵质押物编号 **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.gaga_id,cGet_data,sizeof(sPact_gaga_rel.gaga_id)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	/*************************************************************/
	/***处理合同与抵质押物关系表Pact_gaga_rel *****************/
	/*************************************************************/
	
	iRet = Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
	if(iRet)
	{
		sprintf( acErrMsg, "定义游标出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"定义游标出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	iRet = Pact_gaga_rel_Fet_Upd(&sPact_gaga_rel,g_pub_tx.reply);
	if(iRet == 100)
	{
		sprintf( acErrMsg, "没有该笔合同或协议关联的抵押物的入库记录!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"没有该笔合同或协议关联的抵押物的入库记录!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if(iRet)
	{
		sprintf( acErrMsg, "取游标出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"取游标出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	if(sPact_gaga_rel.stsvar[0] != '1')
	{
		sprintf( acErrMsg, "该合同或协议下抵押物非入库状态!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"该合同或协议下抵押物非入库状态!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	sPact_gaga_rel.stsvar[0] = 'Y';		/**信贷发起出库 **/
	iRet = Pact_gaga_rel_Upd_Upd(sPact_gaga_rel,g_pub_tx.reply);
	if(iRet == 100)
	{
		sprintf( acErrMsg, "更新抵质押登记薄出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"更新抵质押登记薄出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	Pact_gaga_rel_Clo_Upd();
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," pact_gaga_rel ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.beg_sql," stsvar='Y' and pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
	sCms_rollback.sts[0] = '0';
	memcpy(sCms_rollback.rolbk_sql," stsvar='1' ",sizeof(sCms_rollback.rolbk_sql)-1);
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	
	
	/*************************************************************/
	/*******   贷款合同借据关系表 Ln_gage_reg    *****************/
	/*************************************************************/
	
	iRet = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sLn_gage_reg.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "定义游标出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"定义游标出错!!!");
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}
	while(1)		/** 有可能一笔合同对以多比借据(贷款),因此在这儿要全部处理 **/
	{
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		iRet = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);
		if(iRet == 100)
		{
			if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "没有该笔合同或协议的入库记录!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"没有该笔合同或协议的入库记录!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}else if(iRet)
		{
			sprintf( acErrMsg, "取游标出错!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"取游标出错!!!");
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
		if(sPact_gaga_rel.pact_type[0] == '1') /*此处有点问题啊*/
		{
			iRet = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"ac_id = %ld ",sLn_gage_reg.ac_id);
			if(iRet)
			{
				sprintf( acErrMsg, "没有该笔贷款信息!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"没有该笔贷款信息!!!");
				strcpy( g_pub_tx.reply, "B130" );
				goto ErrExit;
			}
			if(pub_base_CompDblVal(sLn_mst.bal+sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst,0.00)>0)
			{
				sprintf( acErrMsg, "ac_id[%ld]未还清,不允许出库!!!",sLn_mst.ac_id);
				WRITEMSG
				sprintf( acErrMsg, "[%s]未还清,不允许出库!!",sLn_mst.name);
				WRITEMSG
				set_zd_data(DC_GET_MSG,acErrMsg);
				strcpy( g_pub_tx.reply, "B130" );
				goto ErrExit;
			}
		}
	
		if(sLn_gage_reg.sts[0] != '0' && sLn_gage_reg.sts[0] != 'Y')
		{
			sprintf( acErrMsg, "该笔合同或协议的非入库状态!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"该笔合同或协议为非入库状态!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;		
		}
		sprintf(sCms_rollback.rolbk_sql," sts='%s'  ",sLn_gage_reg.sts);
		sLn_gage_reg.sts[0] = 'Y';	/**信贷发起出库 **/
		iRet = Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
		if(iRet == 100)
		{
			sprintf( acErrMsg, "更新抵质押登记薄出错!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"更新抵质押登记薄出错!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
		get_zd_data("0580",cGet_data);	/** 授权号码 **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'U';
		sprintf(sCms_rollback.beg_sql," sts='Y' and pact_no='%s' ",sLn_gage_reg.pact_no);
		sCms_rollback.sts[0] = '0';
		get_zd_data("1230",sCms_rollback.filler);
		iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
			goto ErrExit;
		}
		iUpd_flag ++;
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	}
	Ln_gage_reg_Clo_Upd();
	
	

	
OkExit:
	sprintf( acErrMsg, "信贷审批出库成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷审批出库失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
