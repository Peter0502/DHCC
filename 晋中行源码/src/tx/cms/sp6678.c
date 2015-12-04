/*************************************************
* 文 件 名: sp6678.c
* 功能描述：完成信贷贴现通知
*
* 作    者: chenchao
* 完成日期：2011-7-25 15:16:49
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6678','信贷贴现通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6678','0','6678','#$');
insert into tx_sub_def values ('6678','信贷贴现通知','sp6678','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_discnt_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

int sp6678()
{
	int	iRet = 0;
	char	cPo_type[2];
	char	cGet_data[201];
	char	cPnTyep[2];
	long	lPnDays=0;
	
	struct	cms_rollback_c		sCms_rollback;
	struct	mo_discnt_c		sMo_discnt;
	struct	mo_discnt_c		sMo_discnt1;
	struct	mo_discnt_c		sMoDiscnt;
	struct	mdm_ac_rel_c		sMdmAcRel;
	struct	dd_mst_c		sDdMst;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sMo_discnt,0x00,sizeof(sMo_discnt));
	memset(&sMo_discnt1,0x00,sizeof(sMo_discnt1));
	memset(&sMoDiscnt,0x00,sizeof(sMoDiscnt));
	memset(&sMdmAcRel,0x00,sizeof(sMdmAcRel));
	memset(&sDdMst,0x00,sizeof(sDdMst));
	memset(cPo_type,0x00,sizeof(cPo_type));
	memset(cPnTyep,0x00,sizeof(cPnTyep));
	memset(cGet_data,0x00,sizeof(cGet_data));
	
	pub_base_sysinit();
	
	get_zd_data("0620",cGet_data);				/** 贴现协议号码  **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pact_no,cGet_data,sizeof(sMo_discnt.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sMo_discnt.pact_no);
	memcpy( sMo_discnt.br_no, g_pub_tx.tx_br_no,sizeof(sMo_discnt.br_no)-1 );	/* 机构编码 */
	get_zd_double("0400",&sMo_discnt.par_amt);		/** 汇票金额      **/
	get_zd_double("0840",&sMo_discnt.pn_rate);		/** 贴现率        **/
	get_zd_long("0460",&sMo_discnt.pn_date);		/** 贴现日期      **/
	sMo_discnt.wrk_date = g_pub_tx.tx_date;			/* 贴现信息录入时交易日期 */
	get_zd_long("0450",&sMo_discnt.not_matr_date);		/** 汇票到期日    **/
	
	get_zd_data("0300",cGet_data);				/** 出票人账号    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.rem_ac_no,cGet_data,sizeof(sMo_discnt.rem_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0250",cGet_data);				/** 出票人姓名    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.remitter,cGet_data,sizeof(sMo_discnt.remitter)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0270",cGet_data);				/** 付款人行名    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payer_brno_name,cGet_data,sizeof(sMo_discnt.payer_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0310",cGet_data);				/** 收款人账号    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_ac_no,cGet_data,sizeof(sMo_discnt.payee_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0260",cGet_data);				/** 收款人姓名    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee,cGet_data,sizeof(sMo_discnt.payee)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0830",cGet_data);				/** 收款人行名    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_brno_name,cGet_data,sizeof(sMo_discnt.payee_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0670",cPo_type);				/** 票据种类   1-商业承兑汇票  2-银行承兑汇票  **/
	if(cPo_type[0] == '1')
	{
		memcpy(sMo_discnt.note_type,"112",sizeof(sMo_discnt.note_type)-1);
	}else if(cPo_type[0] == '2')
	{
		memcpy(sMo_discnt.note_type,"111",sizeof(sMo_discnt.note_type)-1);
	}
	
	get_zd_data("0590",cGet_data);				/** 票据号码      **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pnote_no,cGet_data,sizeof(sMo_discnt.pnote_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sMo_discnt.pnote_no);

	get_zd_data("0320",cGet_data);				/** 贴现申请人帐号**/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pn_ac_no,cGet_data,sizeof(sMo_discnt.pn_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
		
	get_zd_data("0810",cGet_data);				/** 贴现申请人    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pn_name,cGet_data,sizeof(sMo_discnt.pn_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data( "0700", sMo_discnt.local_ind );		/* 异地同城标志 */
	get_zd_long("0520",&sMo_discnt.way_days);		/** 在途天数      **/
	

	get_zd_data("0680",sMo_discnt.pn_type);				/** 买入方式      **/
	
	get_zd_data("0620",cGet_data);				/** 贴现协议号码  **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pact_no,cGet_data,sizeof(sMo_discnt.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sMo_discnt.tranx_bal = sMo_discnt.par_amt;		/* 贴现账户余额 */
	sMoDiscnt.po_knd[0] = 'P';				/** 纸票标志 **/
	
		/* 检查贴现登记薄中是否已经存在此转贴现协议编号 */
	iRet = Mo_discnt_Sel( g_pub_tx.reply, &sMo_discnt1, "pact_no='%s'", sMo_discnt.pact_no );
	if ( iRet == 0 )
	{
		sprintf( acErrMsg, "贴现登记薄中已经存在此转贴现协议编号!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"贴现登记薄中已经存在此转贴现协议编号!");
		strcpy( g_pub_tx.reply, "B132" );
		goto ErrExit;
	}
	else if ( iRet != 100 )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", iRet );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
		goto ErrExit;
	}
	/* 检查贴现登记薄中是否已经存在此票据类型和票据号 */
	iRet = Mo_discnt_Sel( g_pub_tx.reply, &sMo_discnt1, "note_type='%s' and pnote_no='%s' and sts<>'*' and pn_type='%s' ",sMo_discnt.note_type, sMo_discnt.pnote_no ,sMo_discnt.pn_type);
	if ( iRet == 0 )
	{
		sprintf( acErrMsg, "贴现登记薄中已经存在此票据类型和票据号!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"贴现登记薄中已经存在此票据类型和票据号!");
		strcpy( g_pub_tx.reply, "B174" );
		goto ErrExit;
	}
	else if ( iRet != 100 )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", iRet );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
		goto ErrExit;
	}
	if(sMo_discnt.pn_type[0] == '1')/** 贴现  **/
	{
		sMo_discnt.sts[0] = '1';				/** 录入状态 **/
		/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
		iRet = pub_base_GetParm_Str( "TXCP", 1, sMo_discnt.bkrl_code );
		if ( iRet )
		{
			sprintf( acErrMsg, "取贴现产品编号错!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"取贴现产品编号错!");
			strcpy( g_pub_tx.reply, "B126" );
			goto ErrExit;
		}
		/* 如果贴现申请人账号为本行的,检查贴现申请人账号有效性:必须是活期且账户状态必须正常,户名与输入是否相符*/
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMo_discnt.pn_ac_no);
		if (iRet) 
		{
			sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", iRet);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
			goto ErrExit;
		}
		else 
		{
			/* 根据客户帐号ID、帐户序号、户名、帐户状态为1正常 查询活期存款主文件 */
			iRet = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
				and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
			if (iRet == 100) 
			{
				sprintf(acErrMsg, "此客户帐号非活期或帐户状态不正常![%s]", sMo_discnt.pn_ac_no);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"此客户帐号非活期或帐户状态不正常!");
				strcpy(g_pub_tx.reply, "B124");
				goto ErrExit;
			}
			else if (iRet) 
			{
				sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", iRet);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
				goto ErrExit;
			}
			if (memcmp(sMo_discnt.pn_name , sDdMst.name ,sizeof(sMo_discnt.pn_name)-1)!=0)
			{
				sprintf(acErrMsg, "户名错误![%s][%s]", sMo_discnt.pn_name,sDdMst.name);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"户名错误!");
				strcpy(g_pub_tx.reply, "NXD5");
				goto ErrExit;
			}
		}

	
	}else if (sMo_discnt.pn_type[0] == '2')/** 转贴现  **/
	{
		sMo_discnt.sts[0] = '2';				/* 转贴现标志没有审批直接记账*/
		/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
		iRet = pub_base_GetParm_Str( "TXCP", 2, sMo_discnt.bkrl_code );
		if ( iRet )
		{
			sprintf( acErrMsg, "取贴现产品编号错!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"取贴现产品编号错!");
			strcpy( g_pub_tx.reply, "B126" );
			goto ErrExit;
		}
	
	}
	/** 计算利息  **/
	iRet = pub_base_CalDays( sMo_discnt.pn_date, sMo_discnt.not_matr_date, "0", &lPnDays );
	if ( iRet )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"计算贴现天数错!");
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}
	sMo_discnt.pn_int = sMo_discnt.par_amt * (lPnDays + sMo_discnt.way_days) * (sMo_discnt.pn_rate/30/1000);
		
	iRet = Mo_discnt_Ins(sMo_discnt,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入贴现登记薄失败.请检查ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入贴现登记薄失败!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and pn_type='%s' and sts='%s' ",sMo_discnt.pact_no,sMo_discnt.pn_type,sMo_discnt.sts);
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
	sprintf( acErrMsg, "信贷贴现通知成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷贴现通知失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
