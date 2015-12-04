/*************************************************
* 文 件 名:  spB924.c
* 功能描述： 完成回购式转贴现记账（转入）,分别挂提出挂账子交易、
*	    异地大额子交易;开贴现户(即贷款户129);修改登记簿状态。
*
* 作    者:  jane
* 完成日期： 2003年4月14日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "in_mst_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"
static	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */

int spB924()
{
	char    cAccHrt[6];                     /* 同城科目控制字 */
	int		ret;
  char  skno[24];
  struct	mdm_ac_rel_c	sMdm_ac_rel;
  struct	dd_mst_c	sDd_mst;
  struct	dd_parm_c	sDd_parm;
  struct	dc_acc_rel_c	sDc_acc_rel;
  
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset ( &sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );
	memset ( &sDd_mst, 0x00, sizeof(struct dd_mst_c) );
	memset ( &sDd_parm, 0x00, sizeof(struct dd_parm_c) );
	memset ( &sDc_acc_rel, 0x00, sizeof(struct dc_acc_rel_c) );
	memset ( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );		/* 贷款主文件 */
	memset ( &g_in_mst, 0x00, sizeof(struct in_mst_c) );		/* 内部账主文件 */
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( skno, 0x00, sizeof( skno ) );

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*空介质介质代码*/
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* 转贴现协议编号 */
  get_zd_data( "0300", skno );	  /*收款账号*/
	/* 根据转贴现协议编号查询贴现登记簿 */
	/**------- xxx ------- @20030817@ ---------**
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此转贴现协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	***--------------------------------------------**/
	/* 根据转贴现协议编号和贴现类型修改贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ",sMoDiscnt.pact_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此转贴现协议编号或状态不正常!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B135" );
		goto ErrExit;
    }else if( ret )
    {
    	sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	get_zd_data( "0700", sMoDiscnt.get_ac_type );   /* 收款账户类型 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 流水日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 主机流水号 */

	if( sMoDiscnt.pn_type[0] != '3' )
	{
		sprintf( acErrMsg, "此协议非回购式转贴现转入类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B161" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "此笔贴现已记帐!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已到期回购!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}

	/* 调用贷款开户函数，产生帐号id，帐号序号 *
	if( ln_open_ac() )
	*/
	if( in_open_ac() )
	{
		sprintf(acErrMsg,"调用贷款开户函数错误!");
		WRITEMSG
		goto ErrExit;
	}

	/* 修改贴现登记薄，状态为正常 */
	sMoDiscnt.ac_id = g_in_mst.ac_id;	/* 贷款账号ID */
	sMoDiscnt.ac_seqn = g_in_mst.ac_seqn;	/* 贷款账号序号 */
	strcpy( sMoDiscnt.sts, "3" );		/* 标志：3正常 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();
  vtcp_log("%s,%看看类型===[%s]\n",__FILE__,__LINE__,sMoDiscnt.get_ac_type);
  {
	/*if( sMoDiscnt.get_ac_type[0] == '1' )
	{
		* 取同城科目产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) *
		ret = pub_base_GetParm_Str( "TCKM", 2, cAccHrt );
		if( ret )
		{
			sprintf( acErrMsg, "取同城科目产品编号错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B257" );
			goto ErrExit;
		}

		* 同城时传科目值到内部账贷方子交易 *//*20151127   cfq*/
		/*ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", skno );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "读取介质账号对照表失败,请与中心机房联系!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C114" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel有误![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		ret =Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id=%ld",sMdm_ac_rel.ac_id);
		if( ret == 100 )
		{
			sprintf( acErrMsg, "该账户非活期户![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "M018" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Dd_mst_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no=%ld",sDd_mst.prdt_no);
   	if( ret == 100 )
		{
			sprintf( acErrMsg, "活期产品参数表中不存在该纪录![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "W024" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Dd_parm_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	ret = Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code=%ld",sDd_parm.dc_code);
   	if( ret == 100 )
		{
			sprintf( acErrMsg, "不存在该会计代码![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "O169" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Dc_acc_rel_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	strcpy(cAccHrt,sDc_acc_rel.acc_hrt);*/
		set_zd_data( "0610", skno );
sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
WRITEMSG
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int ln_open_ac()
{
	int	ret1;

	get_zd_data("0620",g_ln_mst.pact_no);		/* 合同号(此处为转贴现协议编号) */
	get_zd_long("0460",&g_ln_mst.mtr_date); 	/**到期日期**/
sprintf(acErrMsg,"到期日期=[%ld]",g_ln_mst.mtr_date);
WRITEMSG
	get_zd_double("0840",&g_ln_mst.rate); 		/**利率**/
	get_zd_data("0250",g_pub_tx.name); 			/**户名**/
	get_zd_double("1002",&g_ln_mst.amt_lmt);	/* 计划发放额，贷款限额 */

	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );			/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );			/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );			/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );			/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3001;
	strcpy(g_pub_tx.brf,"转贴现转入开户");
	strcpy( g_pub_tx.prdt_code, sMoDiscnt.bkrl_code );/* 产品代码 */

	/* 根据贷款产品编号取贷款参数 */
	ret1 = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret1 == 100 )
	{
		sprintf( acErrMsg, "此贴现产品编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B240" );
		return 1;
   	}
	else if( ret1 )
   	{
		sprintf( acErrMsg, "执行Ln_parm_Sel错![%d]", ret1 );
		WRITEMSG
		return 1;
   	}

	strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );	/* 计息类型 */
	strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );	/* 贷款支付方式 */
	/**add by jane 2003-09-02********/
	strcpy( g_ln_mst.cal_code , "0000000000" );	/* 指标体系代码 */
	strcpy( g_ln_mst.cmpd_intst_ind , g_ln_parm.cmpd_intst_ind );	/*是否记复利标志*/

	/* 贴现账号开介质 */
	ret1 = pub_acct_opn_discnt_mdm();
	if( ret1 )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/***开户***/
	ret1 = pub_acct_opn_ac();
	if( ret1 )
	{
		sprintf(acErrMsg,"开户错误![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("0330",g_pub_tx.ac_no);

	return 0;
}

int in_open_ac()
{
	int ret=0;

	strcpy( g_pub_tx.prdt_code, sMoDiscnt.bkrl_code );/* 产品代码 */
	g_pub_tx.mach_date=0; 					/**到期日期**/
	strcpy(g_pub_tx.tmp_ind,"0"); 			/**计息类型**/
	g_pub_tx.over_rate=0.00; 				/**逾期利率**/
	get_zd_double("0840",&g_pub_tx.rate); 		/**利率**/
	get_zd_data("0250",g_pub_tx.name); 			/**户名**/
	strcpy(g_pub_tx.brf,"转贴现转入开户");

	ret=In_parm_Sel( g_pub_tx.reply , &g_ln_parm ,"prdt_no='%s'" , g_pub_tx.prdt_code );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "查询内部帐产品参数表出错!prdt_no=[%s]", g_pub_tx.prdt_code );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W058" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行In_parm_Sel错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret=pub_acct_opn_in_mdm();
	if( ret )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("0330",g_pub_tx.ac_no);

	ret=pub_acct_opn_ac();
	if( ret )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
