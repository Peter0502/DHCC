/*************************************************
* 文 件 名: sp6683.c
* 功能描述：此交易完成信贷审批入库
*
* 作    者: chenchao
* 完成日期：2011-8-4 15:20:22
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6683','信贷审批入库','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6683','0','6683','#$');
insert into tx_sub_def values ('6683','信贷审批入库','sp6683','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_gage_reg_c.h"
int sp6683()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	ln_gage_reg_c		sLnGageReg;
	struct	td_mst_c		sTd_mst;
	struct	cms_rollback_c		sCms_rollback;
	
	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	memset(&sLnGageReg,0x00,sizeof(sLnGageReg));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sTd_mst,0x00,sizeof(sTd_mst));
	
	pub_base_sysinit();
	
	get_zd_data("0380",cGet_data);			/** 贷款合同号 || 承兑协议编号 **/
	zip_space(cGet_data);
	memcpy(sLn_gage_reg.pact_no,cGet_data,sizeof(sLn_gage_reg.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	iRet = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sLn_gage_reg.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "定义贷款登记薄游标出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"查询贷款登记薄出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	iRet = Ln_gage_reg_Fet_Upd(&sLnGageReg,g_pub_tx.reply);
	if(iRet && iRet != 100)
	{
		sprintf( acErrMsg, "查询贷款登记薄出错!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"查询贷款登记薄出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if (iRet == 100)
	{
		get_zd_data("0670",sLn_gage_reg.gage_type);	/** 抵押物类型 1:抵押 2:质押 **/
		sLn_gage_reg.sts[0] = 'X';			/** 信贷发起入库 **/
		sLn_gage_reg.ac_id=0;
		iRet = Ln_gage_reg_Ins(sLn_gage_reg,g_pub_tx.reply);
		if(iRet)
		{
			sprintf( acErrMsg, "插入抵质押物登记薄出错!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"插入抵质押物登记薄出错!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
		get_zd_data("0580",cGet_data);	/** 授权号码 **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'I';
		sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' ",sLn_gage_reg.pact_no);
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
	
	}else
	{
		vtcp_log("[%s][%d]进去更新阶段",__FILE__,__LINE__);
		sprintf(sCms_rollback.rolbk_sql," sts='%s' and gage_type='%s' ",sLnGageReg.sts,sLnGageReg.gage_type);
		get_zd_data("0670",sLnGageReg.gage_type);	/** 抵押物类型 1:抵押 2:质押 **/
		sLnGageReg.sts[0] = 'X';			/** 信贷发起入库 **/
		iRet = Ln_gage_reg_Upd_Upd(sLnGageReg);
		if(iRet)
		{
			vtcp_log("[%s][%d]更新贷款登记薄出错Ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,"更新贷款登记薄出错!!!"); 
			goto ErrExit;
		}
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));

		get_zd_data("0580",cGet_data);	/** 授权号码 **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'U';
		sCms_rollback.sts[0] = '0';
		sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' and gage_type='%s'",sLnGageReg.pact_no,sLnGageReg.gage_type);
		get_zd_data("1230",sCms_rollback.filler);
		iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
			goto ErrExit;
		}
	}
	Ln_gage_reg_Clo_Upd();
	
	
	/*** 开始整理 pact_gaga_rel **/
	memcpy(sPact_gaga_rel.pact_no,sLn_gage_reg.pact_no,sizeof(sPact_gaga_rel.pact_no)-1);	/** 合同号 **/
	get_zd_data("0680",sPact_gaga_rel.pact_type);		/** 类型:1:贷款 2:承兑 **/
	get_zd_data("0370",cGet_data);				/** 抵质押物编号 **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.gaga_id,cGet_data,sizeof(sPact_gaga_rel.gaga_id)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_double("0390",&sPact_gaga_rel.gaga_bal);		/** 所抵押金额 **/
	get_zd_double("0400",&sPact_gaga_rel.gaga_val);		/** 所抵押金额 **/	
	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);

	get_zd_data("0250",cGet_data);				/** 户名 **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.name,cGet_data,sizeof(sPact_gaga_rel.name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0300",cGet_data);				/** 本行存单号码 **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.bill_no,cGet_data,sizeof(sPact_gaga_rel.bill_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sPact_gaga_rel.bill_no);

	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);
	sPact_gaga_rel.stsvar[0] = 'X';				/** 信贷发起入库 **/
	if(sLn_gage_reg.gage_type[0] == '2' && strlen(sPact_gaga_rel.bill_no) >0 )
	{
		iRet = Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s' )",sPact_gaga_rel.bill_no);
		if(iRet)
		{
			sprintf( acErrMsg, "查询定期主文件失败!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"查询定期主文件失败!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		memcpy(sPact_gaga_rel.io_ind,"120",3);
	}
	
	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);
	iRet = Pact_gaga_rel_Ins(sPact_gaga_rel,g_pub_tx.reply);
	if(iRet)
	{
		sprintf( acErrMsg, "合同号和抵质押品明细关联关系表失败!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"插入合同号和抵质押品明细关联关系表失败!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," pact_gaga_rel ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," stsvar='X' and pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
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
OkExit:
	sprintf( acErrMsg, "信贷审批入库成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷审批入库失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
