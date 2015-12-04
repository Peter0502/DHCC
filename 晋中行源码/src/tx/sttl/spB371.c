/*************************************************
* 文 件 名:  spB371.c
* 功能描述： 保证金利率维护功能。
*
* 作    者:  xxx
* 完成日期： 2003年11月2日
*
* 修改记录： 
*   日    期:2011-8-10 13:58:07
*   修 改 人:chenchao 
*   修改内容: 增加保证金账号和协议的关联
*   日    期：2011-8-22 
*   修 改 人：gongliangliang
*   修改内容：利率信贷已经进行了校验，核心不需要进行重新最高利率最低利率的验证，注掉利率验证部分
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bail_rate_c.h"
#include "mo_bank_acc_po_c.h"
#include "protect_note_c.h"
#include "com_parm_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_sg_cash_po_c.h"
#include "dd_parm_c.h"
int spB371()
{
	double	dRate=0.00;
	struct	mo_bail_rate_c	sMoBailRate;		/* 保证金利率登记薄 */
	struct	mo_bail_rate_c	sMoBailRate1;		/* 保证金利率登记薄 */
	struct	dd_mst_c	sDdMst,sDd_mst;			/** 活期主文件 **/
	struct	mo_bank_acc_po_c 	sMo_bank_acc_po;/* 承兑汇票基本信息登记簿 */
	struct	mo_sg_cash_po_c		sMoSgCashPo;
	struct	mdm_ac_rel_c	sMdmAcRel;		/** 介质关系  ***/
	struct  com_parm_c      sComParm;           /* 公共参数 */
	char	czm[2],tmpstr[16];
	char	cOpenBrno[9];			/* 申请人帐号开户机构 */
	int	ret;
	struct  dd_parm_c	sDd_parm;
	
	/* 数据初始化 */
	memset( &sDd_parm,0x00,sizeof(sDd_parm));
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMo_bank_acc_po, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	pub_base_sysinit();
	memset( &sDd_mst, 0x00, sizeof( sDd_mst ) );
	
	/* 从前台屏幕取值 */
	get_zd_data( "0680", czm);				/* 操作码 */
	strcpy(sMoBailRate.br_no,g_pub_tx.tx_br_no);
	get_zd_data( "0670", sMoBailRate.bail_type );		/* 保证金种类 */
	get_zd_data( "0330", sMoBailRate.pact_no );		/* 协议编号 */
	get_zd_data( "0320", sMoBailRate.bail_ac_no );		/* 保证金帐号 */
	get_zd_data( "0310", sMoBailRate.ac_no );		/* 收息帐号 */
	get_zd_double( "0390",&sMoBailRate.amt );		/* 保证金金额 */
	get_zd_long( "0440", &sMoBailRate.beg_date );		/* 起息日期 */
	get_zd_long( "0450", &sMoBailRate.end_date );		/* 到期日期 */
	get_zd_double( "0840",&sMoBailRate.ic_rate );		/* 利率 */
	sMoBailRate.in_trace_no = g_pub_tx.trace_no;		/* 登记流水号 */
	sMoBailRate.intst_date=0;				/* 计息日期 */

	/** 查找旧帐号 **/
	pub_base_old_acno( sMoBailRate.ac_no );
	pub_base_old_acno( sMoBailRate.bail_ac_no );
	

	
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",sMoBailRate.bail_ac_no);
	if( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Sel错![%d][%s]",ret,sMoBailRate.bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "非保证金账户不允许用此交易![%d][%s]",ret,sMoBailRate.bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(sMoBailRate.bail_type[0] == '2')	/**  担保保证金采用更新分户的形式** modify chenchao 2012/3/20 13:27:41 **/
	{
		if( memcmp(sDd_mst.prdt_no,"142",3) )
		{
			sprintf( acErrMsg, "非保证金账户不允许用此交易![%s]",sMoBailRate.bail_ac_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		if(pub_base_CompDblVal(sDd_mst.intst_acm,0.00)>0)
		{
			sprintf( acErrMsg, "积数不为零，请先做单户结息![%lf]",sDd_mst.intst_acm);
			WRITEMSG
			sprintf( g_pub_tx.reply, "D147" );
			goto ErrExit;
		}
		ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
		if( ret )
		{
			sprintf( acErrMsg, "Dd_parm_Sel![%d][%s]",ret,sMoBailRate.bail_ac_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret = pub_base_getllz(sDd_parm.rate_no,"01",g_pub_tx.tx_date,&dRate);
		if( ret )
		{
			sprintf( acErrMsg, "取利率错误!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}
		/** add by chenchao 2011-8-10 17:36:03 晋中进行信贷审批 **/
		ret = sql_execute("update ln_auth set sts='1' where pay_ac_no ='%s' and type='12' and sts='X' ",sMoBailRate.bail_ac_no);
		if(ret)
		{
			sprintf( acErrMsg, "记流水日志错" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sMoBailRate.bail_ac_no);
		set_zd_data("0340","1");
		set_zd_double("0290",(sMoBailRate.ic_rate/dRate)-1); 
		strcpy( g_pub_tx.reply, "0001" );
		goto OkExit;
	}
	if( sMoBailRate.beg_date>=sMoBailRate.end_date )
	{
	   	sprintf( acErrMsg, "起息日期不能大于登记到期日期!beg_date=[%ld],end_date[%ld]",sMoBailRate.beg_date,sMoBailRate.end_date);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B350" );
		goto ErrExit;
	}
	/* 检查收息帐号的有效性，并返回帐号id */
	ret = pub_base_ChkAc( sMoBailRate.ac_no, 1, cOpenBrno, &g_pub_tx.ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "检查付款帐号有效性错" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B339" );
		goto ErrExit;	
	}
	ret=pub_base_ChkAc( sMoBailRate.bail_ac_no, 1, cOpenBrno, &g_pub_tx.ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "检查付款帐号有效性错" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B339" );
		goto ErrExit;	
	}

	vtcp_log("保证金开户机构:=[%s]",cOpenBrno);
	/* 检查利率是否超过限额 */
	ret = Com_parm_Sel(g_pub_tx.reply,&sComParm,"parm_code='BZJLL' and parm_seqn=1 ");
	if( ret )
	{
		sprintf( acErrMsg, "取公共参数表错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "G001" );
		goto ErrExit;	
	}
	ret = pub_base_getllz(sComParm.val,"01",g_pub_tx.tx_date,&sMoBailRate1.ic_rate);
	if( ret )
	{
		sprintf( acErrMsg, "取利率错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "O195" );
		goto ErrExit;	
	}
	sMoBailRate.ic_rate=sMoBailRate.ic_rate * L100;
	sMoBailRate1.ic_rate=sMoBailRate1.ic_rate * L100;
	/*******edit by gongliangliang 20110822 晋中商行信贷
	if( sMoBailRate.ic_rate<-0.0005 )
	{
		sprintf( acErrMsg, "当前利率低于最低浮动利率!!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "A005" );
		goto ErrExit;	
	}
	if( pub_base_CompDblVal(sMoBailRate.ic_rate,sMoBailRate1.ic_rate)>0 )
	{
		sprintf( acErrMsg, "当前利率高于最高浮动利率!!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "A006" );
		goto ErrExit;	
	}
	*********end by gongliangliang 20110822 晋中商行信贷**********/
	sMoBailRate.ic_rate=sMoBailRate.ic_rate / L100;
	sMoBailRate1.ic_rate=sMoBailRate1.ic_rate / L100;

	if( czm[0]=='0' )			/** 登记 **/
	{
		/** add by chenchao 2011-8-10 13:20:15 相关信息在利率维护里完成  ***/
		/* 检查保证金帐号的有效性:活期账号、账户状态正常 */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBailRate.bail_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "保证金账号不存在![%s]", sMoBailRate.bail_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B195" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "输入的保证金账号非活期账号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B196" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "此保证金账号状态不正常!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B197" );
			goto ErrExit;
		}
		if(sMoBailRate.bail_type[0] == '1')
		{
			/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
			ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMo_bank_acc_po, "pact_no='%s'", sMoBailRate.pact_no );
			if( ret == 0 )
			{
				sprintf( acErrMsg, "此承兑协议编号已存在!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}else if( ret != 100 && ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			memcpy(sMo_bank_acc_po.pact_no,sMoBailRate.pact_no,sizeof(sMo_bank_acc_po.pact_no)-1);
			/* 将缴存金额登记到保兑仓登记薄中已存保证金总金额一项中 */
			ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMo_bank_acc_po.pact_no );
			if( ret == 0 )
			{
				sprintf( acErrMsg, "此承兑协议编号已存在!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}else if( ret !=0 && ret != 100 )
			{
				sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			/* 登记保兑仓登记簿 */
			strcpy( sMoSgCashPo.pact_no, sMo_bank_acc_po.pact_no );/* 承兑协议编号 */
			strcpy( sMoSgCashPo.br_no, g_pub_tx.tx_br_no );		/* 交易机构 */
			sMoSgCashPo.safeg_sum = sMoBailRate.amt;		/* 已存保证金总金额 */
			sMoSgCashPo.pickup_sum = 0.00;
			
			ret = Mo_sg_cash_po_Ins( sMoSgCashPo, g_pub_tx.reply );
			if ( ret )
			{
				Mo_sg_cash_po_Debug(&sMoSgCashPo);
				sprintf( acErrMsg, "执行Mo_sg_cash_po_Ins错误!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			/* 登记汇票登记薄 */
			strcpy( sMo_bank_acc_po.bail_ac_no, sMoBailRate.bail_ac_no );	/* 保证金帐号 */
			sMo_bank_acc_po.bail_amt = sMoBailRate.amt;		/* 保证金金额 */
			strcpy( sMo_bank_acc_po.acc_po_sts, "A" );       	/* 承兑协议状态：A 存保证金 */
			sMo_bank_acc_po.req_date = g_pub_tx.tx_date;		/* 暂时记录存保证金日期 */
			
			ret = pub_sttl_RegMoBankAccPo( sMo_bank_acc_po );
			if ( ret )
			{
				sprintf( acErrMsg, "执行pub_sttl_RegMoBankAccPo错误!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		/*** end add by chenchao 2011-8-10 13:20:54 ****/
		
		strcpy( sMoBailRate.sts, "1" );						/* 传真状态0录入*/
		sMoBailRate.end_trace_no=0;
		sMoBailRate.intst_amt=0;
		sMoBailRate.seq=0;
		if( sMoBailRate.bail_type[0]=='1' )					/** 承兑保证金 **/
		{
			strcpy(g_pub_tx.brf,"登记承兑保证金利率");
			ret=sql_count("mo_bail_rate","pact_no='%s' and bail_type='1' and sts='1' ", sMoBailRate.pact_no );
		}
		else											/** 担保保证金 **/
		{
			strcpy(g_pub_tx.brf,"登记担保保证金利率");
			ret=sql_count("mo_bail_rate","pact_no='%s' and bail_type='2' and sts='1' ", sMoBailRate.pact_no );
		}
		if( ret )
	   	{
	   		sprintf( acErrMsg, "该协议编号已经登记![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B333" );
			goto ErrExit;
		}
		ret=Mo_bail_rate_Ins(sMoBailRate,g_pub_tx.reply);
		if( ret )
	   	{
	   		sprintf( acErrMsg, "登记保证金利率登记薄错误![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
	}
	else if( czm[0]=='1' || czm[0]=='2' )		/** 修改,删除 **/ 
	{
		if( sMoBailRate.bail_type[0]=='1' )				/** 承兑保证金 **/
		{
			strcpy(tmpstr,"承兑保证金利率");
			ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and bail_type='1' and sts='1' ", sMoBailRate.pact_no );
		}
		else
		{
			strcpy(tmpstr,"担保保证金利率");
			ret=Mo_bail_rate_Dec_Upd( g_pub_tx.reply,"pact_no='%s' and bail_type='2' and sts='1' ", sMoBailRate.pact_no );
		}
		if( ret )
	   	{
	   		sprintf( acErrMsg, "登记保证金利率登记薄错误![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret=Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret )
	   	{
	   		sprintf( acErrMsg, "登记保证金利率登记薄错误![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		if( sMoBailRate.sts[0]!='1' )
	   	{
	   		sprintf( acErrMsg, "登记保证金利率登记薄状态非正常![%s]",sMoBailRate.sts );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B335" );
			goto ErrExit;
		}

		if( czm[0]=='1' )
		{
			strcpy(sMoBailRate.br_no,g_pub_tx.tx_br_no);
			get_zd_data( "0670", sMoBailRate.bail_type );		/* 保证金种类 */
			get_zd_data( "0330", sMoBailRate.pact_no );		/* 协议编号 */
			get_zd_data( "0320", sMoBailRate.bail_ac_no );		/* 保证金帐号 */
			get_zd_data( "0310", sMoBailRate.ac_no );		/* 收息帐号 */
			get_zd_double( "0390",&sMoBailRate.amt );		/* 保证金金额 */
			get_zd_long( "0440", &sMoBailRate.beg_date );		/* 起息日期 */
			get_zd_long( "0450", &sMoBailRate.end_date );		/* 到期日期 */
			get_zd_double( "0840",&sMoBailRate.ic_rate );		/* 利率 */
			sMoBailRate.in_trace_no = g_pub_tx.trace_no;		/* 登记流水号 */
			strcpy(g_pub_tx.brf,"修改");
			ret=Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
	   		{
	   			sprintf( acErrMsg, "登记保证金利率登记薄错误![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = sql_execute("update mo_bank_acc_po set bail_amt='%lf',bail_amt_ratio=bail_amt*100/sign_amt where pact_no='%s' ",sMoBailRate.amt,sMoBailRate.pact_no);
			if(ret)
			{
				sprintf( acErrMsg, "修改承兑协议表出错![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = sql_execute("update mo_sg_cash_po set safeg_sum=%lf where pact_no='%s' ",sMoBailRate.pact_no,sMoBailRate.amt);
			if(ret)
			{
				sprintf( acErrMsg, "保兑仓登记薄出错![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
		}
		else
		{
			strcpy(g_pub_tx.brf,"删除");
			ret=Mo_bail_rate_Del_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
	   		{
	   			sprintf( acErrMsg, "登记保证金利率登记薄错误![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			if(sMoBailRate.bail_type[0]=='1')
			{
				ret = sql_execute("update  mo_bank_acc_po set acc_po_sts='C',bail_amt=0,bail_amt_ratio=0 where pact_no='%s' and acc_po_sts in ('A','0','1','2')",sMoBailRate.pact_no);/** 置为删除状态 **/
				if(ret)
				{
					sprintf( acErrMsg, "删除承兑协议表出错![%d]",ret );
					WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
					goto ErrExit;
				}
				ret = sql_execute("delete from mo_sg_cash_po where pact_no='%s' ",sMoBailRate.pact_no);
				if(ret)
				{
					sprintf( acErrMsg, "保兑仓登记薄出错![%d]",ret );
					WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
					goto ErrExit;
				}
			}
		}
		strcat(g_pub_tx.brf,tmpstr);
		Mo_bail_rate_Clo_Upd();
	}
	else
	{
		sprintf( acErrMsg, "操作吗类型错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B332" );
		goto ErrExit;
	}
	
	/* 记流水日志 */
	strcpy(g_pub_tx.ac_no,sMoBailRate.ac_no);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	/** add by chenchao 2011-8-10 17:36:03 晋中进行信贷审批 **/
	ret = sql_execute("update ln_auth set sts='1' where pact_no='%s' and type='12' and sts='X' ",sMoBailRate.pact_no);
	if(ret)
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( g_pub_tx.reply, "0000" );
OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
		strcpy( g_pub_tx.reply, "H034" );
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
