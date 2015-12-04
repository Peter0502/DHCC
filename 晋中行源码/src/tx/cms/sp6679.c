/*************************************************
* 文 件 名: sp6679.c
* 功能描述：此交易完成信贷转贴现转出功能
*
* 作    者: chenchao
* 完成日期：2011-7-20 9:50:43
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6679','信贷转贴现转出','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6679','0','6679','#$');
insert into tx_sub_def values ('6679','信贷转贴现转出','sp6679','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_discnt_c.h"

int sp6679()
{
	int	iRet = 0;
	char	cGet_data[201];	
	double	dReal_amt = 0.00;
	long	lPnDays = 0;
	
	struct cms_rollback_c	sCms_rollback;
	struct mo_discnt_c	sMo_discnt;
	struct mo_discnt_c	sMoDiscnt; /**/
	
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
	memset(&sMo_discnt,0x00,sizeof( sMo_discnt));
	memset(&sMoDiscnt,0x00,sizeof( sMoDiscnt));
	
	pub_base_sysinit();
	/* 从前台屏幕取值 */
	get_zd_data( "0370", cGet_data );		/* 转贴现协议编号 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.tpact_no,cGet_data,sizeof(sMo_discnt.tpact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
		
	memcpy( sMo_discnt.br_no,g_pub_tx.tx_br_no,sizeof(sMo_discnt.br_no)-1);	/* 机构编码 */
	sprintf( acErrMsg, "line 49现在的br_no[%s]",sMo_discnt.br_no );/*20110817 这个机构号是咱的机构号还是他给传过来的？测试用*/
	WRITEMSG

	get_zd_double( "0400", &sMo_discnt.par_amt );	/* 汇票金额 */
	get_zd_double( "0850", &sMo_discnt.tpn_rate );	/* 转贴现率 */
	get_zd_long("0470",&sMo_discnt.tpn_date);	/* 转贴现日 */
	sMo_discnt.wrk_date = g_pub_tx.tx_date;	/* 转贴现信息录入时交易日期 */
	get_zd_long( "0460",&sMo_discnt.not_matr_date );/* 汇票到期日 */
	get_zd_data( "0310", cGet_data );		/* 出票人帐号 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.rem_ac_no,cGet_data,sizeof(sMo_discnt.rem_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0260", cGet_data);		/* 出票人全称 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.remitter,cGet_data,sizeof(sMo_discnt.remitter)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data( "0250", cGet_data );		/* 付款行全称 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payer_brno_name,cGet_data,sizeof(sMo_discnt.payer_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0320", cGet_data);		/* 收款人帐号 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_ac_no,cGet_data,sizeof(sMo_discnt.payee_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data( "0270",cGet_data);			/* 收款人全称 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee,cGet_data,sizeof(sMo_discnt.payee)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data( "0810", cGet_data );		/* 收款人开户行全称 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_brno_name,cGet_data,sizeof(sMo_discnt.payee_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data( "0670", cGet_data);		/* 票据类型 */
	zip_space(cGet_data);
	if(cGet_data[0] == '1')
	{
		memcpy(sMo_discnt.note_type,"112",sizeof(sMo_discnt.note_type)-1);
	}else if(cGet_data[0] == '2')
	{
		memcpy(sMo_discnt.note_type,"111",sizeof(sMo_discnt.note_type)-1);
	}else{
		sprintf( acErrMsg, "票据类型错误!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B248" );
		goto ErrExit;
	}
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data( "0750",cGet_data);				/* 票据号 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.pnote_no,cGet_data,sizeof(sMo_discnt.pnote_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0700", sMo_discnt.local_ind );		/* 异地同城标志 */
	
	get_zd_long( "0520", &sMo_discnt.way_days );		/* 在途天数 */
	/*get_zd_data( "0810", sMo_discnt.tpn_name );*//*嘻嘻，写错了吧*/	
	get_zd_data( "0810", cGet_data );	/* 申请人名称 */
	zip_space(cGet_data);
	memcpy(sMo_discnt.tpn_name,cGet_data,sizeof(sMo_discnt.tpn_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sMo_discnt.tranx_bal = sMo_discnt.par_amt;				/* 金额 */
   	/* 根据票据类型和票据号找出记录 */
	iRet = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s' and sts='3' and br_no='%s' ",sMo_discnt.note_type, sMo_discnt.pnote_no,g_pub_tx.tx_br_no );
	if ( iRet )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	iRet = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( iRet == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息![%s],[%s]" ,sMo_discnt.note_type, sMo_discnt.pnote_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if( iRet )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	if(sMoDiscnt.pn_type[0] == '4')
	{
		sprintf( acErrMsg, "此票据已做转贴现转出!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"此票据已做转贴现转出!"); 
		strcpy( g_pub_tx.reply, "B248" );
		goto ErrExit;
	}
	/* 检查票据状态是否为正常，且贴现类型为贴现或者买断转入 */
	if( sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "此票据非贴现或买断转入所得，不能做转贴现转出!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"此票据非贴现或买断转入所得，不能做转贴现转出!"); 
		strcpy( g_pub_tx.reply, "B154" );
		goto ErrExit;
	}

	if( sMoDiscnt.pn_type[0] == '1' )	/* 贴现 */
	{
		if( sMoDiscnt.sts[0] == '1' )/*Mo_discnt_Dec_Upd的条件就是 sts='3'所以这个状态肯定是3啊 李吉祥 */
		{
			sprintf( acErrMsg, "此笔贴现未审核!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"此笔贴现未审核!"); 
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}
		else if( sMoDiscnt.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"此笔贴现未记账!"); 
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	else					/* 买断转入 */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"此笔贴现未记账!"); 
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已做还款!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"此笔贴现已做还款!"); 
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}
	strcpy(sMoDiscnt.sts, "*");    /* 标志：* 销户*/ /* 这句什么意思？ */
	sprintf(sCms_rollback.rolbk_sql," sts='3',wrk_date=%ld ",sMoDiscnt.wrk_date);
	sMoDiscnt.wrk_date=g_pub_tx.tx_date;
	iRet = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( iRet )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();
	
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.beg_sql," pn_type='%s' and pnote_no='%s' and sts='*' and br_no='%s' ",sMoDiscnt.pn_type,sMoDiscnt.pnote_no,sMoDiscnt.br_no);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0] = '#';
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	
	
	/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	iRet = pub_base_GetParm_Str( "TXCP", 5, sMo_discnt.bkrl_code );
	if( iRet )
	{
		sprintf( acErrMsg, "取贴现产品编号错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}
	
	/*****计算贴现利息：其中，贴现天数 = 票据到期日 -转贴现日期+ 在途天数,
		贴现利息 = 汇票金额 * 贴现天数 * （月转贴现率/30天/1000）） ****/
	iRet = pub_base_CalDays( sMo_discnt.tpn_date, sMo_discnt.not_matr_date, "0", &lPnDays );
	if( iRet )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}
	
	sMo_discnt.tpn_int = sMo_discnt.par_amt * (lPnDays + sMo_discnt.way_days) * (sMo_discnt.tpn_rate/30/1000);
	
	/* 登记贴现登记薄，状态为录入 */
	strcpy( sMo_discnt.sts, "1" );				/* 标志：1录入 */
	sMo_discnt.po_knd[0] = sMoDiscnt.po_knd[0] ;
	
	strcpy(sMo_discnt.pn_type, "4");                     /* 贴现类型: 4买断转出 */
	iRet = Mo_discnt_Ins( sMo_discnt, g_pub_tx.reply );
	if ( iRet )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Ins错误!iRet=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," tpact_no='%s' and pnote_no='%s' and pn_type='4' and sts='1' ",sMo_discnt.tpact_no,sMo_discnt.pnote_no );
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
	/*************************
	*到此为止，假设有这种情况：
	*今天是8月10号，库里曾经有一组A记录，6月10号做的的一笔交易。票号是123，然后
	*A记录的数据状态是    sts='*',pn_type='2', pact_no='A1',ac_id='1234' pnote_no='123' 转入
	                      sts='*',pn_type='4',tpact_no='A2',ac_id=null,  pnote_no='123' 转出
	*今天新做的一组B数据  sts='*',pn_type='2', pact_no='B1',ac_id='5678' pnote_no='123' 新的转入
	                      sts='1',pn_type='4',tpact_no='B2',ac_id=null,  pnote_no='123' 新的转出   
	*而在转出交易输入票据号回显信息的时候，调的sp9934交易，查询条件如下    
	    ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' \  //检查贴现登记薄中是否存在此票据 
			  and pnote_no='%s' and pn_type='4' and br_no ='%s'", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no ,cKinbr); 
			ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' \  //再检查贴现登记薄中此票据的基本信息 
			and pnote_no='%s' and pn_type in ('1','2') and br_no='%s'  ",sMoDiscnt1.note_type, sMoDiscnt1.pnote_no,cKinbr );
	*因为没有对sts的判断，所以就把A记录给查出来了。因为A记录和B记录的票号，pn_type 和行号都可能是一模一样的
	*但是现在还不能靠sts来判断是新的交易还是老交易。		                             
	****************************/
OkExit:
	sprintf( acErrMsg, "信贷转贴现转出成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷转贴现转出失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
