/*************************************************
* 文 件 名:  spB373.c
* 功能描述： 保证金取款功能
*
* 作    者:  
* 完成日期： 2003年11月6日
*
* 修改记录： 
*   日   期: 20110909
*   修 改 人: chenchao
*   修改内容: 晋中行内需求,在取款时通过信贷审批,取款金额为信贷审批金额,应该为撤销保证金金额或是到期处理的金额
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"
#include "dd_mst_c.h"

int spB373()
{
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* 承兑汇票基本信息登记簿 */
	struct	mo_bail_rate_c		sMoBailRate;	/* 保证金利率登记薄 */
	struct	mo_bail_rate_c		sMoBailRate1;	/* 保证金利率登记薄 */
	struct	mo_bail_rate_c		sMoBailRate_cc;
	struct	dd_mst_c		sDd_mst;
	double	dTempAmt=0.00;						/* 退票时应该从保证金户扣划到结算户的金额 */
	int		ret;
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	dSum_bail_amt=0.00;
	double	HQdealval=0.00;
	char	bail_ac_no[25];
	char	intst_ac_no[25];
	char	intst_flag[2];
	char	cBail_type[2];

	/* 数据初始化 */
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate_cc, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	memset (cBail_type,0x00,sizeof(cBail_type) );
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoBankAccPo.pact_no );				/* 承兑协议编号 */
	get_zd_data( "0320", bail_ac_no );					/* 保证金帐号 */
	get_zd_data( "0310", intst_ac_no );					/* 收息帐号 */
	get_zd_double( "0390", &dTempAmt );					/* 取款金额 */
	get_zd_data( "0670" , intst_flag );					/* 计息标志 */
	get_zd_data( "0680", cBail_type );					/* 保证金种类 */

	/** 旧帐号 **/
	pub_base_old_acno( bail_ac_no );
	pub_base_old_acno( intst_ac_no );
	
	zip_space(sMoBankAccPo.pact_no);
	
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",bail_ac_no);
	if( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Sel错![%d][%s]",ret,bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "非保证金账户不允许用此交易![%d][%s]",ret,bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(cBail_type[0] == '1')
	{	
		if(strlen(sMoBankAccPo.pact_no)>0 )
		{
			/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
			ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			
			ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "此承兑协议编号不存在!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B199" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		
		}else{
			ret = sql_sum_double("mo_bail_rate","amt",&dSum_bail_amt,"bail_ac_no='%s' and sts='1'",bail_ac_no);
			if(ret)
			{
				sprintf( acErrMsg, "执行sql_sum_double错![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "AT03" );
				goto ErrExit;
			}
			if(sDd_mst.bal-dSum_bail_amt-dTempAmt<-0.005)
			{
				sprintf( acErrMsg, "取款金额过大![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "A030" );
				goto ErrExit;
			}
		
			/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
			ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "bail_ac_no='%s' and acc_po_sts!='2' and acc_po_sts!='3'", bail_ac_no );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "没有签订协议不需要修改记录,直接记账!" );
				WRITEMSG
				set_zd_double("0410",dTempAmt);

				goto OkExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		/*****
		double dPoAmt=0.00; **已签发承兑汇票金额**
		double dYqBzamt =0.00;**签发承兑汇票对于保证金比例**
		ret = sql_sum_double("mo_bank_po","po_amt",&dPoAmt,"pact_no='%s' and po_sts='1'",sMoBankAccPo.pact_no);
		if (ret) {
			sprintf(acErrMsg, "执行sql_sum_double错[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"已签发的汇票金额总额=[%lf],sign_amt=[%lf]",dPoAmt,sMoBankAccPo.sign_amt);
        	WRITEMSG
        	***  保证金已签发并且没到期  不允许做保证金取款 ***
		if(dPoAmt > 0.005 && sMoBankAccPo.matr_date > g_pub_tx.tx_date)
		{
			sprintf( acErrMsg, "保证金未到期不允许取款!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B302" );
			goto ErrExit;
		}
		dYqBzamt = pub_base_PubDround_1(dPoAmt*sMoBankAccPo.bail_amt_ratio/100.00);
		sprintf(acErrMsg,"已签发保证金金额=[%lf],bail_amt=[%lf]",dYqBzamt,sMoBankAccPo.bail_amt);
        	WRITEMSG
        	
		if( pub_base_CompDblVal(dTempAmt,sMoBankAccPo.bail_amt - dYqBzamt) > 0 )
		{
			sprintf( acErrMsg, "此承兑协议取款金额错误![%.2f][%.2f]",dTempAmt,dYqBzamt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B342" );
			goto ErrExit;
		}
        	
		if( pub_base_CompDblVal(dTempAmt,sMoBankAccPo.bail_amt) > 0 )
		{
			sprintf( acErrMsg, "此承兑协议取款金额错误![%.2f][%.2f]",dTempAmt,sMoBankAccPo.bail_amt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B342" );
			goto ErrExit;
		}
		**************end by chenchao 经信贷审批 而且存在多存入的情况 要全部取出 不校验 20110908**************************/
		pub_base_strpack(sMoBankAccPo.bail_ac_no);
		pub_base_strpack(bail_ac_no);
		/*	不比较了，因为扣款是从输入的保证金户扣钱的 2006-12-11 liuxuan*//*晋中进行判断 2009/12/23 12:03:27 modify by martin */
		if( strcmp(sMoBankAccPo.bail_ac_no,bail_ac_no) )
		{
			sprintf( acErrMsg, "协议编号对应的保证金帐号与录入保证金帐号不一致!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B329" );
			goto ErrExit;
		}
		
		/* 修改承兑汇票基本信息登记簿 */
		if( pub_base_CompDblVal(dTempAmt,0.00) > 0 )
			sMoBankAccPo.bail_amt=sMoBankAccPo.bail_amt-dTempAmt;
        	
		if(pub_base_CompDblVal(sMoBankAccPo.bail_amt,0.00) < 0)
		{
			sMoBankAccPo.bail_amt=0.00;
		}else{
			sMoBankAccPo.bail_amt_ratio=pub_base_PubDround_1(sMoBankAccPo.bail_amt*100/sMoBankAccPo.sign_amt);
		}
		ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Mo_bank_acc_po_Clo_Upd();
	}else if (cBail_type[0] == '2')
	{
		/***jk20120615
			if(strlen(sMoBankAccPo.pact_no)==0 )
		{
			ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply, "bail_ac_no='%s' and sts='1' and amt=%lf",bail_ac_no ,dTempAmt );
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = Mo_bail_rate_Fet_Upd(&sMoBailRate_cc,g_pub_tx.reply);
			if( ret ) 
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			sMoBailRate_cc.sts[0]='*';
			ret = Mo_bail_rate_Upd_Upd(sMoBailRate_cc,g_pub_tx.reply);
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			Mo_bail_rate_Clo_Upd();
			ret = sql_sum_double("mo_bail_rate","amt",&dSum_bail_amt,"bail_ac_no='%s' and sts='1'",bail_ac_no);
			if(ret)
			{
				sprintf( acErrMsg, "执行sql_sum_double错![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "AT03" );
				goto ErrExit;
			}
			if(sDd_mst.bal-dSum_bail_amt-dTempAmt<-0.005)
			{
				sprintf( acErrMsg, "取款金额过大![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "A030" );
				goto ErrExit;
			}
		}jk20120615***/
	}else{
		sprintf( acErrMsg, "标志错误!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** 撤销必须要有协议编号 **/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	g_pub_tx.tx_amt1 = dTempAmt;
	strcpy( g_pub_tx.beg_note_no, bail_ac_no );
	strcpy( g_pub_tx.brf, "保证金取款" );
	g_pub_tx.svc_ind=0;
	strcpy( g_pub_tx.note_type,intst_flag );	/** 撤销用是否计息了 **/
	strcpy(g_pub_tx.no_show,"1");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");

	if( intst_flag[0]=='Y' )
	{
		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no );
		if( ret )
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret ) 
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
	
		memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );
	
		pub_base_strpack(sMoBailRate.ac_no);
		pub_base_strpack(intst_ac_no);
		if( strcmp(sMoBailRate.ac_no,intst_ac_no) )
		{
			sprintf( acErrMsg, "输入的收息帐号与输入的不一致![%s][%s]",sMoBailRate.ac_no,intst_ac_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B343" );
			goto ErrExit;
		}
	
		ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
		if( ret )
		{
			sprintf( acErrMsg, "取利率错误!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}
	
		/** 判断是否到期 **/
		if( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** 提前支取 **/
		{
			/** 计算天数 **/
			if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
			{
				sprintf( acErrMsg, "计算天数错误!");
				WRITEMSG
				goto ErrExit;
			}
			day=0;
			/** 计算利息 **/
			if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; 晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * dTempAmt * HQday / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
				sMoBailRate.amt=dTempAmt;
			}
		}
		else
		{
			/** 计算天数 **/
			if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
			{
				sprintf( acErrMsg, "计算天数错误!");
				WRITEMSG
				goto ErrExit;
			}
						/** 计算利息 **/
			if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; 晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * dTempAmt * day / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
				sMoBailRate.amt=dTempAmt;
			}
		}
	
		sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);
	
		g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
		if ( pub_base_CompDblVal(g_pub_intst.dealval,0.00)<0 )
		{
			sprintf(acErrMsg,"计算利息错误错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}
	
		/** 更新保证金计息金额 **/
		sMoBailRate.intst_date=g_pub_tx.tx_date;
		sMoBailRate.end_trace_no=g_pub_tx.trace_no;
		sMoBailRate.intst_amt=g_pub_intst.dealval;
		strcpy(sMoBailRate.sts,"*");
		ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		Mo_bail_rate_Clo_Upd();
	
		/** 插入新的保证金利息登记薄 **/
		if( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
		{
			ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Ins错![%d]",ret);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
		}
		g_pub_intst.tax_intst=g_pub_intst.dealval;	/** 应税利息 **/
		g_pub_intst.factval=g_pub_intst.dealval;	/** 实付利息 **/
		g_pub_intst.val=0.00;						/** 利息税 **/
		g_pub_intst.keepval=0.00;					/** 保值利息 **/
	
		dTempAmt=dTempAmt+g_pub_intst.dealval;
		set_zd_data("0153","52101");				/** 保证金利息 **/
		set_zd_double("102P",g_pub_intst.dealval);
	}
	else if( intst_flag[0]=='N' )
	{
		g_pub_intst.dealval=0.00;
		g_pub_intst.tax_intst=0.00;					/** 应税利息 **/
		g_pub_intst.factval=0.00;					/** 实付利息 **/
		g_pub_intst.val=0.00;						/** 利息税 **/
		g_pub_intst.keepval=0.00;					/** 保值利息 **/
		set_zd_data("0153","52101");				/** 保证金利息 **/
		set_zd_double("102P",g_pub_intst.dealval);
	}else{
		sprintf(acErrMsg,"利息标志错误!!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D999");
		goto ErrExit;
	}
	Mo_bail_rate_Clo_Upd();
	set_zd_double("0410",dTempAmt);
	
	
OkExit:
	if(strlen(sMoBankAccPo.pact_no)>0)
	{/*20110811*/
		ret=sql_execute("update ln_auth set sts='1' where pact_no='%s' and sts='X' and type='13' ",sMoBankAccPo.pact_no);
	}else{
		ret=sql_execute("update ln_auth set sts='1' where pay_ac_no='%s' and sts='X' and type='13' ",bail_ac_no);
	}
	if(ret){
		sprintf(acErrMsg,"更改ln_auth状态失败");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
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
