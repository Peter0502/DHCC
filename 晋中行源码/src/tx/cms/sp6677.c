/*************************************************
* 文 件 名: sp6677.c
* 功能描述：完成信贷承兑汇票的签发通知
*
* 作    者: chenchao
* 完成日期：2011-7-23 15:50:42
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6677','信贷承兑汇票签发通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6677','0','6677','#$');
insert into tx_sub_def values ('6677','信贷承兑汇票签发通知','sp6677','0','0'); 

*************************************************/ 

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h" 
#include "cms_rollback_c.h"
#include "mo_bank_po_fee_c.h"

int sp6677()
{ 
	int	iRet = 0;
	long	lNum_po = 0;
	long	lNum_fee = 0;
	char	cGet_data[201];
	
	struct	cms_rollback_c		sCms_rollback;
	struct	mo_bank_acc_po_c	sMoBankAcc;
	struct	mo_bank_acc_po_c	sMoBankAccPo;
	struct	mo_bank_po_fee_c	sMo_bank_po_fee;
	struct	mdm_ac_rel_c		sMdm_ac_rel;
	struct	com_branch_c		sCom_branch;
	struct	dd_mst_c		sDd_mst;
	struct	mo_bank_acc_po_c 	sMoBankAcc1;			/* 承兑汇票基本信息登记簿 */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;			/* 保兑仓登记薄 */
	struct	mdm_ac_rel_c 		sMdmAcRel;			/* 介质账号对照表 */
	struct	dd_mst_c 		sDdMst;				/* 活期主文件 */
	struct	impa_dep_reg_c 		sImpaDepReg;			/* 质押存单登记簿 */
	struct	mo_dep_reg_c 		sMoDepReg;			/* 汇票质押存单登记簿 */
	struct  mo_po_co_c 		sMoPoCo; 			/* 承兑协议编号与合同号对照表 */
	struct  mo_bail_rate_c		mo_bail_rate; 			/* 保证金利率登记薄 */
	char	cDbActno[25];						/* 担保帐号 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	double	dBail_amt = 0.00;					/* 保证金金额 */
	double	dTallAmt;						/* 承兑汇票最高限额 */
	double	dSumMortAmt;						/* 质押存单登记簿中质押编号对应余额 */
	double	dSumMortAmt1;						/* 承兑基本信息登记簿中质押编号对应余额之和 */
	double	dSumMortAmt2;						/* 承兑登记簿中质押编号对应余额之和 */
	int	ret=0, ret1=0, flag=0, num=0;

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( cDbActno, 0x00, sizeof( cDbActno ) );
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sMoBankAcc,0x00,sizeof(sMoBankAcc));
	memset(&sMo_bank_po_fee,0x00,sizeof(sMo_bank_po_fee));
	memset(&sMoBankAccPo,0x00,sizeof(sMoBankAccPo));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(cBailAcNo,0x00,sizeof(cBailAcNo));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);		/** 协议号码 **/
	zip_space(cGet_data);
	memcpy(sMoBankAcc.pact_no,cGet_data,sizeof(sMoBankAcc.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data("0030",sMoBankAcc.tx_br_no);		/** 交易机构 **/
	
		/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret1 );
		WRITEMSG
		set_zd_data(DC_GET_MSG, "执行数据库更新出错");
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret1 );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"执行数据库更新出错");
		goto ErrExit;
	}
	else if( ret1 == 0 || ret1 == 100 )
	{
		if( ret1 == 0 )
		{
			if( sMoBankAcc.acc_po_sts[0]=='2' )
			{
				strcpy( g_pub_tx.reply,"B281" );
				sprintf( acErrMsg, "sMoBankAcc.acc_po_sts错![%s]",
					sMoBankAcc.acc_po_sts);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"承兑协议状态出错！");
				goto ErrExit;
			}else if( sMoBankAcc.acc_po_sts[0]=='0' || sMoBankAcc.acc_po_sts[0]=='1')
			{
				goto CONTINUE;
			}
			/*** 设置回滚字段 ***/
			sprintf(sCms_rollback.rolbk_sql," tx_br_no='%s',req_date='%ld',payer_name='%s',payer_ac_no='%s',payer_br_no='%s',payer_br_name='%s',payee_name='%s',payee_ac_no='%s',payee_br_name='%s',sign_amt='%f',matr_date='%ld',bail_ac_no='%s',bail_amt='%f',bail_amt_ratio='%f',pact_num='%s',tot_mort_amt='%f',advance_amt='%f',over_ac_no='%s',over_rate='%f',acc_po_sts='%s',acc_po_ind='%s',acc_type='%s'",
			sMoBankAcc.tx_br_no,sMoBankAcc.req_date,
			sMoBankAcc.payer_name,sMoBankAcc.payer_ac_no,sMoBankAcc.payer_br_no,
			sMoBankAcc.payer_br_name,sMoBankAcc.payee_name,sMoBankAcc.payee_ac_no,
			sMoBankAcc.payee_br_name,sMoBankAcc.sign_amt,sMoBankAcc.matr_date,
			sMoBankAcc.bail_ac_no,sMoBankAcc.bail_amt,sMoBankAcc.bail_amt_ratio,
			sMoBankAcc.pact_num,sMoBankAcc.tot_mort_amt,sMoBankAcc.advance_amt,
			sMoBankAcc.over_ac_no,sMoBankAcc.over_rate,sMoBankAcc.acc_po_sts,
			sMoBankAcc.acc_po_ind,sMoBankAcc.acc_type);

			
		}
		get_zd_data("0300",cGet_data);				/** 付款人账号**/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payer_ac_no,cGet_data,sizeof(sMoBankAcc.payer_ac_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		sMoBankAcc.req_date = g_pub_tx.tx_date;			/** 签发日期  **/
		get_zd_data("0250",cGet_data);				/** 付款人名称 **/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payer_name,cGet_data,sizeof(sMoBankAcc.payer_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0310",cGet_data);				/** 收款人账号 **/
		zip_space(cGet_data);
		memcpy(sMoBankAccPo.payee_ac_no,cGet_data,sizeof(sMoBankAccPo.payee_ac_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		pub_base_old_acno(sMoBankAccPo.payee_ac_no);
		
		get_zd_data("0270",cGet_data);				/** 收款人名称 **/
		zip_space(cGet_data);
		memcpy(sMoBankAccPo.payee_name,cGet_data,sizeof(sMoBankAccPo.payee_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0260",cGet_data);				/** 收款人开户行名**/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payee_br_name,cGet_data,sizeof(sMoBankAcc.payee_br_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0680",cGet_data);				/** 承兑类型 **/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.acc_type,cGet_data,sizeof(sMoBankAcc.acc_type)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
        	
		get_zd_long("0450",&sMoBankAcc.matr_date);		/** 到期日期 **/
		get_zd_double("1001",&sMoBankAcc.sign_amt);		/** 票面金额 **/
        	
		get_zd_data("0320",cGet_data);				/** 保证金账号**/
		zip_space(cGet_data);
		memcpy(cBailAcNo,cGet_data,sizeof(cBailAcNo)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		
		get_zd_double("0410",&dBail_amt);			/** 保证金金额 **/
		get_zd_double("0400",&sMoBankAcc.bail_amt_ratio);	/** 保证金比例 **/
		get_zd_double("0970",&sMoBankAcc.over_rate);		/** 罚息利率 **/
		sMoBankAcc.acc_po_ind[0] = '0';				/* 汇票状态: 0未到期处理*/
		
		get_zd_double("1002",&sMoBankAcc.tot_mort_amt);		/** 质押金额 **/
		if(sMoBankAcc.tot_mort_amt > 0.005)
		{
			memcpy(sMoBankAcc.pact_num,sMoBankAcc.pact_no,sizeof(sMoBankAcc.pact_num)-1);
		}

		pub_base_old_acno(sMoBankAcc.payer_ac_no);
		
		/** 由于晋中核心不支持一个协议下多个收款人的情况,所以在这儿控制一下 **/
		if(sMoBankAcc.acc_po_sts[0] == '1')
		{
			if(memcmp(sMoBankAccPo.payee_ac_no,sMoBankAcc.payee_ac_no,sizeof(sMoBankAcc.payee_ac_no)-1)
				|| memcmp(sMoBankAccPo.payee_name,sMoBankAcc.payee_name,sizeof(sMoBankAcc.payee_name)-1))
			{
				vtcp_log("[%s][%d]同一个协议下只能为一个收款人",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"一个协议下只能为一个收款人!!!");
				goto ErrExit;
			}
		}else{
			memcpy(sMoBankAcc.payee_ac_no,sMoBankAccPo.payee_ac_no,sizeof(sMoBankAcc.payee_ac_no)-1);
			memcpy(sMoBankAcc.payee_name,sMoBankAccPo.payee_name,sizeof(sMoBankAcc.payee_name)-1);
		}

			
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sMoBankAcc.payer_ac_no);
		if(iRet)
		{
			vtcp_log("[%s][%d]查询mdm_ac_rel失败ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			set_zd_data(DC_GET_MSG,"查询介质关系表失败!!!");
			goto ErrExit;
		}
		iRet = Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sMdm_ac_rel.opn_br_no);
		if(iRet)
		{
			vtcp_log("[%s][%d]查询com_branch失败ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			set_zd_data(DC_GET_MSG,"查询机构系表失败!!!");
			goto ErrExit;
		}
		memcpy(sMoBankAcc.payer_br_no,sCom_branch.br_no,sizeof(sMoBankAcc.payer_br_no)-1);
		memcpy(sMoBankAcc.payer_br_name,sCom_branch.br_name,sizeof(sMoBankAcc.payer_br_name)-1);
	
		/** 旧帐号处理 **/
		pub_base_old_acno( sMoBankAcc.payer_ac_no );
		pub_base_old_acno( cBailAcNo );

		if(sMoBankAcc.acc_type[0] == '3' || sMoBankAcc.acc_type[0] == '1' )
		{
			memset(&mo_bail_rate,0,sizeof(mo_bail_rate));
			memcpy(mo_bail_rate.pact_no,sMoBankAcc.pact_no,sizeof(mo_bail_rate.pact_no)-1);
			zip_space(mo_bail_rate.pact_no);
			ret = Mo_bail_rate_Sel(g_pub_tx.reply,&mo_bail_rate," pact_no='%s' ",mo_bail_rate.pact_no);
			if(ret)
			{
				if(ret==100){
					/*sprintf( acErrMsg, "承兑协议编号与保证金利率维护时输入的不一致!" );*/
					sprintf( acErrMsg, "该承兑协议编号尚未进行保证金利率维护操作!" );
				  WRITEMSG
				  set_zd_data(DC_GET_MSG,"该承兑协议编号尚未进行保证金利率维护操作!");
				  strcpy( g_pub_tx.reply, "B306" );
				  goto ErrExit;
				}
				else{
					sprintf( acErrMsg, " 查找表mo_bail_rate出错!" );
				  WRITEMSG
				  set_zd_data(DC_GET_MSG, "查找表mo_bail_rate出错!");
				  strcpy( g_pub_tx.reply, "B306" );
				  goto ErrExit;
				}				
			}
			if(mo_bail_rate.end_date!=sMoBankAcc.matr_date)
			{
				sprintf( acErrMsg, "协议到期日与保证金到期日不是同一天!!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"协议到期日与保证金到期日不是同一天!!");
				strcpy( g_pub_tx.reply, "B319" );
				goto ErrExit;
			}
		}

		/* 检查付款帐号的有效性:活期账号、账户状态正常 */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBankAcc.payer_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAcc.payer_ac_no );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"付款人账号不存在!");
			strcpy( g_pub_tx.reply, "D164" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
			goto ErrExit;
		}
	
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "付款人账号不是活期账号!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"付款人账号不是活期账号!");
			strcpy( g_pub_tx.reply, "B179" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
			goto ErrExit;
		}
	
		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "此付款人账号状态不正常!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"此付款人账号状态不正常!");
			strcpy( g_pub_tx.reply, "B180" );
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "131" ) ==0 )
		{
			sprintf( acErrMsg, "出票人账号不允许为保证金账号!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"出票人账号不允许为保证金账号!");
			strcpy( g_pub_tx.reply, "B295" );
			goto ErrExit;
		}

		vtcp_log("%s,%d,Bail_amt=%lf,要签发的保证金金额=%lf",__FILE__,__LINE__,dBail_amt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00);
		if( sMoBankAcc.tot_mort_amt < 0.005 && pub_base_CompDblVal(dBail_amt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00)<0 &&sMoBankAcc.acc_type[0] != '4')
		{
			sprintf( acErrMsg, "保证金与质押金额不允许全部为零" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"保证金与质押金额不允许全部为零");
			strcpy( g_pub_tx.reply, "B181" );
			goto ErrExit;
		}

		/* 1：差额承兑；2：100%质押存单；3：100%保证金；4：担保承兑；5：大票换小票 */
		if( ret1 == 0 )
		{
			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5' || sMoBankAcc.acc_type[0] == '4')
			{
				sprintf( acErrMsg, "承兑协议编号已存在!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"承兑协议编号已存在!");
				strcpy( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}

			pub_base_strpack( sMoBankAcc.bail_ac_no );
			if( sMoBankAcc.acc_type[0] == '1' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "承兑类型为差额承兑时，保证金账号必须输入!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"承兑类型为差额承兑时，保证金账号必须输入!" );
					strcpy( g_pub_tx.reply, "B312" );
					goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"保证金账号必须与缴存保证金时录入一致!");
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'",sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"承兑协议编号与缴存保证金时输入的不一致!");
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dBail_amt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "保证金金额必须小于等于第一次缴存时输入的金额!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"保证金金额必须小于等于第一次缴存时输入的金额!");
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}
				pub_base_strpack( sMoBankAcc.pact_num );

				if( strcmp( sMoBankAcc.pact_num, "" ))
				{
					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "此质押编号不存在!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"此质押编号不存在!");
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "执行Impa_dep_reg_Sel错误![%d]",ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
						goto ErrExit;
					}
		
					/* 检查质押存单余额是否够用 */
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", 
						&dSumMortAmt,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
						goto ErrExit;
					}
					ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
						"a.tot_mort_amt", &dSumMortAmt1,\
						"pact_num='%s' and a.pact_no=b.pact_no and \
						b.po_sts!='3'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
						goto ErrExit;
					}

					sprintf(acErrMsg, "已用金额:[%lf], 质押总额:[%lf],本次金额:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
					WRITEMSG
		
					ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
					if( ret == 1 )
					{
						sprintf( acErrMsg, "质押编号对应可用余额不足!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"质押编号对应可用余额不足!" );
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}

			if( sMoBankAcc.acc_type[0] == '3' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "承兑类型为100％保证金时，保证金账号必须输入!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"承兑类型为100％保证金时，保证金账号必须输入!");
					strcpy( g_pub_tx.reply, "B314" );
					goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"保证金账号必须与缴存保证金时录入一致!");
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
				vtcp_log( "[%s][%d],sMoBankAcc.bail_amt[%lf], sMoBankAcc.sign_amt[%lf]",__FILE__,__LINE__,sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				ret = pub_base_CompDblVal( sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "缴存保证金金额必须大于等于票面总金额!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"缴存保证金金额必须大于等于票面总金额!");
					strcpy( g_pub_tx.reply, "B313" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"承兑协议编号与缴存保证金时输入的不一致!");
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
					goto ErrExit;
				}
			}

			sMoBankAcc.acc_po_sts[0]='0';		/* 承兑协议状态：0 录入 */
		
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"修改承兑汇票基本信息登记薄错！");
				goto ErrExit;
			}
			
			get_zd_data("0580",cGet_data);	/** 授权号码 **/
			zip_space(cGet_data);
			memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
			memset(cGet_data, 0x00, sizeof(cGet_data));
			
			memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
			sCms_rollback.do_type[0] = 'U';
			sprintf(sCms_rollback.beg_sql,"pact_no='%s' ",sMoBankAcc.pact_no,sMoBankAcc.acc_po_sts);
			sCms_rollback.sts[0] = '0';
			sCms_rollback.filler[0]='#';
			get_zd_data("1230",sCms_rollback.filler+1);
			iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
			if(iRet)
			{
				vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
				goto ErrExit;
			}
		}
		else if( ret1 == 100 )
		{
			if( sMoBankAcc.acc_type[0] == '1' || sMoBankAcc.acc_type[0] == '3' )
			{
				sprintf( acErrMsg, "如果选择保证金承兑，则必须先做缴存保证金交易!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"如果选择保证金承兑，则必须先做缴存保证金交易!");
				strcpy( g_pub_tx.reply, "B264" );
				goto ErrExit;
			}

			if( !strcmp( sMoBankAcc.pact_num, "" ) && sMoBankAcc.acc_type[0] != '4' )
			{
				sprintf( acErrMsg, "质押编号必须输入!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"质押编号必须输入!" );
				strcpy( g_pub_tx.reply, "B317" );
				goto ErrExit;
			}

			if( sMoBankAcc.acc_type[0] == '4' )
			{
				ret = pub_base_CompDblVal( 0.00, sMoBankAcc.bail_amt );
				if( ret )
				{
					sprintf( acErrMsg, "担保承兑的保证金金额应该为零!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"担保承兑的保证金金额应该为零!");
					strcpy( g_pub_tx.reply, "B358" );
					goto ErrExit;
				}

				memcpy(cDbActno,cBailAcNo,sizeof(cDbActno)-1);

				ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
								cDbActno );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]担保帐号不存在![%s]", __FILE__,__LINE__,cDbActno );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"担保帐号不存在!");	
					strcpy( g_pub_tx.reply, "E354" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				switch(sMdmAcRel.note_sts[0])
				{
					case '0':	/** 正常 **/
					case '2':	/** 挂失换证 **/
					case '3':	/** 正常换证 **/
					case '4':	/** 已经部提 **/
						break;
					default:
						sprintf( acErrMsg, "[%s][%d]担保帐号状态不正常!" ,__FILE__,__LINE__);
						WRITEMSG
						set_zd_data(DC_GET_MSG,"担保帐号状态不正常!");
						strcpy( g_pub_tx.reply, "B355" );
						goto ErrExit;
				}


				ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]担保帐户不是活期帐户!",__FILE__,__LINE__ );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"担保帐户不是活期帐户!");
					strcpy( g_pub_tx.reply, "B356" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询出错！");
					goto ErrExit;
				}

				if(!memcmp(sDdMst.prdt_no,"133",sizeof(sDdMst.prdt_no)-1))
				{
					sprintf( acErrMsg, "保证金帐户不能当做担保帐户!",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"保证金帐户不能当做担保帐户!");
					strcpy( g_pub_tx.reply, "B357" );
					goto ErrExit;
				}
				if(sDdMst.ac_sts[0] != '1')
				{
					sprintf( acErrMsg, "担保帐号状态不正常!",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"担保帐号状态不正常!");
					strcpy( g_pub_tx.reply, "B355" );
					goto ErrExit;
				}

				/** 担保帐户不做金额检查,检查到此结束 **/
				strcpy( sMoBankAcc.bail_ac_no, cBailAcNo );
			}

			if( sMoBankAcc.acc_type[0] == '5' )
			{

				ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "此质押汇票编号不存在!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"此质押汇票编号不存在!");
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_dep_reg_Sel错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}
	
				/* 检查质押存单余额是否够用 */
				ret = sql_sum_double( "mo_dep_reg", "po_amt", &dSumMortAmt,\
								"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po",
						"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
						acc_type='5' and acc_po_sts in ('0','1','2') ",
						sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b",
				"b.po_amt", &dSumMortAmt2, "pact_num='%s' and  \
				a.pact_no=b.pact_no and b.po_sts!='3' and a.acc_type='5' and a.acc_po_sts='3' ",
				sMoBankAcc.pact_num );
				if( ret )
				{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
						goto ErrExit;
				}
				dSumMortAmt1+=dSumMortAmt2;


				sprintf(acErrMsg, "已用金额:[%lf], 质押总额:[%lf],本次金额:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
				WRITEMSG
	
				ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "质押编号对应可用余额不足!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"质押编号对应可用余额不足!" );
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "质押金额必须大于等于票面总金额!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"质押金额必须大于等于票面总金额!");
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}

			}

			if( sMoBankAcc.acc_type[0] == '2' )
			{
				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"质押编号对应的承兑,其到期日必须为同一天!");
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}
				}
				else if( ret != 100 )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				Mo_bank_acc_po_Clo_Sel();

				ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "此质押编号不存在!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"此质押编号不存在!");
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Impa_dep_reg_Sel错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}
	
				/* 检查质押存单余额是否够用 */
				ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
										"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					acc_type='2' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
					"b.po_amt", &dSumMortAmt2, "pact_num='%s' and  \
					a.pact_no=b.pact_no and b.po_sts!='3' and a.acc_type='2' and a.acc_po_sts='3' ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}
				dSumMortAmt1+=dSumMortAmt2;

				sprintf(acErrMsg, "已用金额:[%lf], 质押总额:[%lf],本次金额:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
				WRITEMSG
	
				ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "质押编号对应可用余额不足!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"质押编号对应可用余额不足!");
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
				/*modify by gong 2011/9/17 11:07:12 晋中商行 暂时封掉
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				vtcp_log("[%s][%d] sMoBankAcc.tot_mort_amt=[%lf],sMoBankAcc.sign_amt[%lf]",__FILE__,__LINE__,sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt);
				if( ret == -1 )
				{
					sprintf( acErrMsg, "质押金额必须大于等于票面总金额!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"质押金额必须大于等于票面总金额!");
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
				end by gong 2011/9/17 11:07:34*/
			}
		
			/* 登记汇票登记薄 */
			strcpy( sMoBankAcc.acc_po_sts, "0" );       /* 承兑协议状态：0 录入 */
		
			ret = pub_sttl_RegMoBankAccPo( sMoBankAcc );
			if ( ret )
			{
				sprintf( acErrMsg, "执行pub_sttl_RegMoBankAccPo错误!ret=[%d]", ret );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
				goto ErrExit;
			}
			memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
			
			get_zd_data("0580",cGet_data);	/** 授权号码 **/
			zip_space(cGet_data);
			memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
			memset(cGet_data, 0x00, sizeof(cGet_data));
			
			memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
			sCms_rollback.do_type[0] = 'I';
			sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoBankAcc.pact_no);
			sCms_rollback.sts[0] = '0';
			sCms_rollback.filler[0]='#';
			get_zd_data("1230",sCms_rollback.filler+1);
			iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
			if(iRet)
			{
				vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
				goto ErrExit;
			}
		}
	}

	Mo_bank_acc_po_Clo_Upd();

	/*登记协议编号-合同号对照登记簿 为MIS准备数据*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "登记承兑协议编号-合同号对照表出错" );
		WRITEMSG
		set_zd_data(DC_GET_MSG, "登记承兑协议编号-合同号对照表出错" );
		goto ErrExit;
	}

	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," mo_po_co ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoPoCo.pact_no);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0]='#'; /** 表示查询时候不去查了***/
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
CONTINUE:
	/***
	iRet = sql_max_long( "mo_bank_po", "pact_no_num", &lNum_po , "pact_no='%s' ",sMoBankAcc.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "执行sql_max_long错[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	iRet = sql_max_long( "mo_bank_po_fee", "num", &lNum_fee , "pact_no='%s' ",sMoBankAcc.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "执行sql_max_long错[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	sMo_bank_po_fee.num = (lNum_po > lNum_fee)?lNum_po:lNum_fee;
	***/

	memcpy(sMo_bank_po_fee.pact_no,sMoBankAcc.pact_no,sizeof(sMo_bank_po_fee.pact_no)-1);
	get_zd_long("0530",&sMo_bank_po_fee.num);		/** 协议序号 **/
	get_zd_double("0390",&sMo_bank_po_fee.po_amt );		/** 票面金额 **/
	get_zd_double("0420",&sMo_bank_po_fee.fee_amt); 	/** 手续费 **/
	get_zd_double("0430",&sMo_bank_po_fee.prm_amt);		/** 承诺费 **/
	sMo_bank_po_fee.sts[0] = 'X';				/** 信贷发送 **/
	sMo_bank_po_fee.new_num = 0;				/** **/
	iRet = Mo_bank_po_fee_Ins(sMo_bank_po_fee,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入承兑收费登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入承兑收费登记薄出错!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_bank_po_fee",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and sts='X' and num=%ld ",sMoBankAcc.pact_no,sMo_bank_po_fee.num);
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
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.rolbk_sql,"sign_amt=sign_amt-%lf",sMo_bank_po_fee.po_amt);
	sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoBankAcc.pact_no,sMoBankAcc.acc_po_sts);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0]='#';
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	
OkExit:
	sprintf( acErrMsg, "信贷承兑汇票签发通知成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷承兑汇票签发通知失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
