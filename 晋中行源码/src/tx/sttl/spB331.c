/*************************************************
* 文 件 名:  spB331.c
* 功能描述： 完成银行承兑汇票信息录入功能
*
* 作    者:  jane
* 完成日期： 2003年8月8日
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
#include "mdm_ac_rel_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h"

int spB331()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc,sMoBankAcc1;		/* 承兑汇票基本信息登记簿 */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* 保兑仓登记薄 */
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* 承兑拆分原始票据登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	dd_mst_c 			sDdMst;			/* 活期主文件 */
	struct	impa_dep_reg_c 		sImpaDepReg;	/* 质押存单登记簿 */
	struct	mo_dep_reg_c 		sMoDepReg;	/* 汇票质押存单登记簿 */
	struct  mo_po_co_c  sMoPoCo; 	/* 承兑协议编号与合同号对照表 */
	struct  mo_bail_rate_c		mo_bail_rate; 	/* 保证金利率登记薄 */
	char    cLoadName[50];       		   		/* 上传文件名 */
	char	cDbActno[25];						/* 担保帐号 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	double	dTmpAmt;							/* 保证金金额 */
	double	dTallAmt;							/* 承兑汇票最高限额 */
	double	dSumMortAmt;						/* 质押存单登记簿中质押编号对应余额 */
	double	dSumMortAmt1;						/* 承兑基本信息登记簿中质押编号对应余额之和 */
	double	dSumMortAmt2;						/* 承兑登记簿中质押编号对应余额之和 */
	int		ret, ret1, flag, num;
	char	Tmp_flag[2];

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( cDbActno, 0x00, sizeof( cDbActno ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/* 申请书编号 */

	pub_base_strpack( sMoBankAcc.pact_no );
/***协议编号的长度不做限制
	if( strlen( sMoBankAcc.pact_no ) != 16 )
	{
		strcpy( g_pub_tx.reply, "B328" );
		sprintf( acErrMsg, "承兑协议编号应为16位!" );
		WRITEMSG
		goto ErrExit;
	}
***/

	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret1 );
		WRITEMSG
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
				goto ErrExit;
			}
		}

		/* 从前台屏幕取值 */
		/*strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	 交易机构 */
		/* 2008/12/9 17:01 add by martin 审核不能与录入为同一个人*/
		strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tel );
		sMoBankAcc.req_date = g_pub_tx.tx_date;				/* 申请日期 */
		get_zd_data( "0260", sMoBankAcc.payer_name );		/* 付款人全称 */
		get_zd_data( "0910", sMoBankAcc.payer_br_no );		/* 付款人开户行行号 */
		get_zd_data( "0250", sMoBankAcc.payer_br_name );	/* 付款人开户行行名 */
		get_zd_data( "0270", sMoBankAcc.payee_name );		/* 收款人全称 */
		get_zd_data( "0830", sMoBankAcc.payee_ac_no );		/* 收款人账号 */
		get_zd_data( "0810", sMoBankAcc.payee_br_name );	/* 收款人开户行行名 */
		get_zd_double( "1001", &sMoBankAcc.sign_amt );		/* 签发票面金额 */
		get_zd_long( "0440", &sMoBankAcc.matr_date );		/* 到期日期 */
		get_zd_data( "0630", sMoBankAcc.pact_num );			/* 质押编号 */
		get_zd_double( "1002", &sMoBankAcc.tot_mort_amt );	/* 质押金额 */
		get_zd_double( "0400", &sMoBankAcc.bail_amt_ratio );/* 保证金比例 */
		strcpy( sMoBankAcc.acc_po_ind, "0" );				/* 汇票状态: 0未到期处理*/
		get_zd_data( "0380", sMoBankAcc.payer_ac_no );		/* 付款人帐号 */
		get_zd_double( "1003", &dTmpAmt );					/* 保证金金额 */
		get_zd_data( "0670", sMoBankAcc.acc_type );			/* 承兑类型 */
		get_zd_data( "0320", cBailAcNo );					/* 保证金帐号 */
		get_zd_double( "0840", &sMoBankAcc.over_rate );		/* 罚息利率 */

		/** 旧帐号处理 **/
		pub_base_old_acno( sMoBankAcc.payer_ac_no );
		pub_base_old_acno( cBailAcNo );

		/** 判断担保户利率到期日和承兑汇票到期日是否一致 **/
		/** 因为无法取出全部索引字段，所以不能保证以后不 **/
		/** 出现重复记录 **/
		if( sMoBankAcc.acc_type[0] == '3' || sMoBankAcc.acc_type[0] == '1' )
		{
			memset(&mo_bail_rate,0,sizeof(mo_bail_rate));
			memcpy(mo_bail_rate.pact_no,sMoBankAcc.pact_no,sizeof(mo_bail_rate.pact_no)-1);
			zip_space(mo_bail_rate.pact_no);
			ret = Mo_bail_rate_Sel(g_pub_tx.reply,&mo_bail_rate," pact_no='%s' ",mo_bail_rate.pact_no);
			if(ret)
			{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
			}
			if(mo_bail_rate.end_date!=sMoBankAcc.matr_date)
			{
					sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B319" );
					goto ErrExit;
			}
		}

		/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
		/* 票面总金额不能大于等于1100万元人民币 *
		*---- 登记金额不控制 -------------------------------*
		ret = pub_base_GetParm_double( "CDZGE", 1, &dTallAmt );
		if( ret )
		{
			sprintf( acErrMsg, "取承兑汇票最高限额产品编号错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B269" );
			goto ErrExit;
		}

		ret = pub_base_CompDblVal( sMoBankAcc.sign_amt, dTallAmt );
		if( ret != -1 )
		{
			sprintf( acErrMsg, "承兑汇票票面总金额不允许超过1100万元人民币![%lf]", sMoBankAcc.sign_amt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B270" );
			goto ErrExit;
		}
		*----------------------------------------------------*/

		/* 检查付款帐号的有效性:活期账号、账户状态正常 */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
							sMoBankAcc.payer_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAcc.payer_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D164" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
						sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "付款人账号不是活期账号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B179" );
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
			sprintf( acErrMsg, "此付款人账号状态不正常!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B180" );
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "131" ) ==0 )
		{
			sprintf( acErrMsg, "出票人账号不允许为保证金账号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B295" );
			goto ErrExit;
		}

		/* 判断保证金和质押存单总金额是否为0 * modify by ligl 20100426
		if( sMoBankAcc.tot_mort_amt < 0.005 && dTmpAmt < 0.005 &&sMoBankAcc.acc_type[0] != '4')**/
		vtcp_log("%s,%d,aaaaaaaaaaaaa=%lf,%lf",__FILE__,__LINE__,dTmpAmt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00);
		if( sMoBankAcc.tot_mort_amt < 0.005 && pub_base_CompDblVal(dTmpAmt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00)<0 &&sMoBankAcc.acc_type[0] != '4')
		{
			sprintf( acErrMsg, "保证金与质押金额不允许全部为零" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B181" );
			goto ErrExit;
		}

		/* 1：差额承兑；2：100%质押存单；3：100%保证金；4：担保承兑；5：大票换小票 */
		if( ret1 == 0 )
		{
			strcpy(Tmp_flag,"0");

			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5' || sMoBankAcc.acc_type[0] == '4')
			{
				sprintf( acErrMsg, "承兑协议编号已存在!" );
				WRITEMSG
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
		            	    	strcpy( g_pub_tx.reply, "B312" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dTmpAmt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "保证金金额必须小于等于第一次缴存时输入的金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}

				if( strcmp( sMoBankAcc.pact_num, "" ) )
				{
					ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
					if( ret == 0 )
					{
						if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
						{
							sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
							WRITEMSG
							strcpy( g_pub_tx.reply, "B319" );
							goto ErrExit;
						}
					}else if( ret == 100 )
					{
						sprintf( acErrMsg, "此质押编号不存在!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}

					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, 
						"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "此质押编号不存在!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "执行Impa_dep_reg_Sel错误![%d]", 
							ret );
						WRITEMSG
						goto ErrExit;
					}
		
					/* 检查质押存单余额是否够用 */
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", 
						&dSumMortAmt,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					/* modify by rob at 20031201 **
					ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", 
						&dSumMortAmt1,"pact_num='%s'", sMoBankAcc.pact_num );
					***/
					ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
						"a.tot_mort_amt", &dSumMortAmt1,\
						"pact_num='%s' and a.pact_no=b.pact_no and \
						b.po_sts!='3'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
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
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}
		
			/**	 担保承兑
			if( sMoBankAcc.acc_type[0] == '4' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "承兑类型为保兑仓时，保证金账号必须输入!" );
		                	WRITEMSG
		            	    	strcpy( g_pub_tx.reply, "B260" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dTmpAmt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "保证金金额必须小于等于第一次缴存时输入的金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}

				if( strcmp( sMoBankAcc.pact_num, "" ) )
				{
					ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
					if( ret == 0 )
					{
						if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
						{
							sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
							WRITEMSG
							strcpy( g_pub_tx.reply, "B319" );
							goto ErrExit;
						}
					}else if( ret == 100 )
					{
						sprintf( acErrMsg, "此质押编号不存在!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					Mo_bank_acc_po_Clo_Sel();

					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "质押存单对应的承兑,其到期日必须为同一天!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}

		
					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "此质押编号不存在!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "执行Impa_dep_reg_Sel错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					* 检查质押存单余额是否够用 *
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
											"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1,\
											"pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
					if( ret == 1 )
					{
						sprintf( acErrMsg, "质押编号对应可用余额不足!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}
			**/
		
			if( sMoBankAcc.acc_type[0] == '3' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "承兑类型为100％保证金时，保证金账号必须输入!" );
		                	WRITEMSG
		            	    	strcpy( g_pub_tx.reply, "B314" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
vtcp_log( "zy1  sMoBankAcc.bail_amt[%lf], sMoBankAcc.sign_amt[%lf]",sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				ret = pub_base_CompDblVal( sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "缴存保证金金额必须大于等于票面总金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B313" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "承兑协议编号与缴存保证金时输入的不一致!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_sg_cash_po_Sel错![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			}
		
			/* 修改承兑汇票基本信息登记簿 */
			/****改为直接赋值   20110128
			strcpy( sMoBankAcc.acc_po_sts, "0" );
			****/
			sMoBankAcc.acc_po_sts[0]='0';		/* 承兑协议状态：0 录入 */
		
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		else if( ret1 == 100 )
		{
			strcpy(Tmp_flag,"1");
			if( sMoBankAcc.acc_type[0] == '1' || sMoBankAcc.acc_type[0] == '3' )
			{
				sprintf( acErrMsg, "如果选择保证金承兑，则必须先做缴存保证金交易!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B264" );
				goto ErrExit;
			}

			if( !strcmp( sMoBankAcc.pact_num, "" ) && sMoBankAcc.acc_type[0] != '4' )
			{
				sprintf( acErrMsg, "质押编号必须输入!" );
				WRITEMSG
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
						strcpy( g_pub_tx.reply, "E354" );
						goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
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
						strcpy( g_pub_tx.reply, "B355" );
						goto ErrExit;
				}


				ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
								sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]担保帐户不是活期帐户!",__FILE__,__LINE__ );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B356" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					goto ErrExit;
				}

				if(!memcmp(sDdMst.prdt_no,"133",sizeof(sDdMst.prdt_no)-1))
				{
					sprintf( acErrMsg, "保证金帐户不能当做担保帐户!",__FILE__, __LINE__,ret );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B357" );
					goto ErrExit;
				}
				if(sDdMst.ac_sts[0] != '1')
				{
					sprintf( acErrMsg, "担保帐号状态不正常!",__FILE__, __LINE__,ret );
					WRITEMSG
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
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_dep_reg_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/* 检查质押存单余额是否够用 */
				ret = sql_sum_double( "mo_dep_reg", "po_amt", &dSumMortAmt,\
								"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	

				/***
				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
					"a.tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					a.pact_no=b.pact_no and b.po_sts!='3' and acc_type='5' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			   ***/

				ret = sql_sum_double( "mo_bank_acc_po",
						"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
						acc_type='5' and acc_po_sts in ('0','1','2') ",
						sMoBankAcc.pact_num );
				if( ret )
				{
					  sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					  WRITEMSG
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
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "质押金额必须大于等于票面总金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
/****************原来状态5的情况 
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "承兑类型为承兑拆分时，保证金账号必须输入!" );
					WRITEMSG
			    		strcpy( g_pub_tx.reply, "B299" );
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}

vtcp_log( "zyzyzy1   sMoBankAcc1.bail_ac_no[%s], cBailAcNo[%s]",sMoBankAcc1.bail_ac_no, cBailAcNo );
					pub_base_strpack( sMoBankAcc1.bail_ac_no );
vtcp_log( "zyzyzy2   sMoBankAcc1.bail_ac_no[%s], cBailAcNo[%s]",sMoBankAcc1.bail_ac_no, cBailAcNo );
					if( strcmp( sMoBankAcc1.bail_ac_no, cBailAcNo ) != 0 )
					{
						sprintf( acErrMsg, "保证金账号必须与拆分票据管理交易录入的一致!" );
		       				WRITEMSG
						strcpy( g_pub_tx.reply, "B300" );
						goto ErrExit;
			    		}
				}else if( ret != 100 )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				strcpy( sMoBankAcc.bail_ac_no, cBailAcNo );

				Mo_bank_acc_po_Clo_Sel();

				* 查询承兑拆分原始票据登记簿 *
				ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, 
					"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "如果为承兑拆分，必须先做拆分票据管理交易!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B297" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_bank_split_po_Sel错![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				if( sMoBankSplitPo.matr_date > sMoBankAcc.matr_date )
				{
					sprintf( acErrMsg, "原票据到期日期不允许大于新承兑到期日期!原日期[%ld]新日期[%ld]",sMoBankSplitPo.matr_date, sMoBankAcc.matr_date );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B298" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.sign_amt, sMoBankSplitPo.par_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "拆分后新开承兑票面总金额不允许大于被拆分票据金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B301" );
					goto ErrExit;
				}
********************/
			}

			/** 大票换小票和质押存单一样处理 **/
/** 大票换小票要单独处理
			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5')
**/
			if( sMoBankAcc.acc_type[0] == '2' )
			{
				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "质押编号对应的承兑,其到期日必须为同一天!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}
				}
				else if( ret != 100 )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Mo_bank_acc_po_Clo_Sel();

				ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "此质押编号不存在!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Impa_dep_reg_Sel错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/* 检查质押存单余额是否够用 */
				ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
										"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/*** modify by rob at 20031201
				ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  acc_po_sts!='3'", sMoBankAcc.pact_num );**/

				ret = sql_sum_double( "mo_bank_acc_po", 
					"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					acc_type='2' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", ret );
					WRITEMSG
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
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "质押金额必须大于等于票面总金额!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
			}
		
			/* 登记汇票登记薄 */
			strcpy( sMoBankAcc.acc_po_sts, "0" );       /* 承兑协议状态：0 录入 */
		
			ret = pub_sttl_RegMoBankAccPo( sMoBankAcc );
			if ( ret )
		   	{
			       	sprintf( acErrMsg, "执行pub_sttl_RegMoBankAccPo错误!ret=[%d]", ret );
			       	WRITEMSG
			       	goto ErrExit;
			}
		}
	}

	Mo_bank_acc_po_Clo_Upd();

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "银行承兑汇票信息录入" );	/* 备注 */

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,Tmp_flag);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	/*登记协议编号-合同号对照登记簿 为MIS准备数据*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "登记承兑协议编号-合同号对照表出错" );
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
