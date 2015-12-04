/*************************************************
* 文 件 名:  spB413.c
* 功能描述： 完成中心打印传真明细功能。
*
* 作    者:  jane
* 完成日期： 2003年1月30日
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
#include "mo_fax_c.h"
#include "com_branch_c.h"

int spB413()
{
	struct	mo_fax_c	sMoFax;		/* 传真登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系 */
	struct	com_branch_c	sComBranch; 	/* 公共机构码表 */
	char	cFileName[50];			/* 临时文件名 */
	char	cOperCode[6];			/* 业务代码 */
	char	cBrNo[6];			/* 机构号 */
	int	ret, num=0;
	FILE 	*fp;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cBrNo, 0x00, sizeof( cBrNo ) );
	
	strcpy( cOperCode, "10005" );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0910", cBrNo );
	sprintf( acErrMsg, "aaaaaaacbrno = [%s]", cBrNo );		WRITEMSG
	/* 根据柜员号、终端号生成临时文件 */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "生成下传全路经文件名(批量)错" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "临时文件错误!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	/* 检查输入机构号是否为空 */
	pub_base_strpack( cBrNo );
	
	sprintf( acErrMsg, "aaaaaaaaaaaaaaaaaaaaaaa" );		WRITEMSG
	/* 如果输入机构号不为空，则打印此输入机构的传真明细、汇总表 */
	if( strlen( cBrNo ) != 0 )
	{
		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaa111111" );		WRITEMSG	
		/* 根据输入的机构号、当日日期查找传真登记薄，写入临时文件中 */
		/* 声明打开游标 */
		ret = Mo_fax_Dec_Upd( g_pub_tx.reply , "tx_br_no='%s' \
				and tx_date=%d ", cBrNo, g_pub_tx.tx_date );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_fax_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaa222222222" );		WRITEMSG	
		fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"交易日期", "交易机构号", "传真编号", "申请人全称", \
			"付款人开户行行号", "付款人开户行行名", "付款人账号", \
			"付款人账号序号", "付款人全称", "收款人开户行行号", \
			"收款人开户行行名", "收款人账号", "收款人全称", \
			"传真金额", "传真状态", "已打印次数" );
	
		while(1)
		{
			memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
			memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
		
			ret = Mo_fax_Fet_Upd( &sMoFax, g_pub_tx.reply );
			if ( ret == 100 && num == 0)
			{
				sprintf( acErrMsg, "查询传真登记簿错，无记录!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B081" );
				goto ErrExit;
			}else if ( ret == 100 && num > 0 )
			{
				break;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mo_fax_Fet_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		
			/* 检查交易机构是否是中心机构(即签发机构) */
			ret = pub_sttl_ChkSignBrno( cOperCode, sMoFax.tx_br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "检查机构权限错!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			
			/* 根据付款人账号ID和付款人账号序号返回付款人账号 */
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdmAcRel , "ac_id=%ld \
						and ac_seqn=%d", sMoFax.payer_ac_id, \
						sMoFax.payer_ac_seqn );
			if( ret == 100 )
    			{
        			sprintf(acErrMsg,"取介质与账户关系表mdm_ac_rel错!无此账户信息!\
        			ac_id=[%ld], ac_seqn=[%d]", sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
    			}else if ( ret )
    			{
        			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret );
        			WRITEMSG
        			goto ErrExit;
    			}

			fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|%.2f|%s|%d|\n", \
				g_pub_tx.tx_date, g_pub_tx.tx_br_no, sMoFax.fax_no, \
				sMoFax.requ_name, sMoFax.payer_br_no, sMoFax.payer_br_name, \
				sMdmAcRel.ac_no, sMoFax.payer_ac_seqn, sMoFax.payer_name, \
				sMoFax.payee_br_no, sMoFax.payee_br_name, sMoFax.payee_ac_no, \
				sMoFax.payee_name, sMoFax.fax_amt, sMoFax.fax_sts, sMoFax.pr_cnt );

			num++;	
		}
	}
	/* 如果为空，则打印所有网点的传真明细、汇总表 */
	else
	{
		/* 查询公共机构码表,取出机构号 */
		ret = Com_branch_Dec_Sel( g_pub_tx.reply, "1='%s'", "1" );
		if ( ret )
    		{
        		sprintf( acErrMsg, "执行Com_branch_Dec_Sel错误!ret=[%d]", ret );
        		WRITEMSG
        		goto ErrExit;
   		}
   		   		
   		while(1)	/*　机构循环　*/
   		{
   			
   			ret = Com_branch_Fet_Sel ( &sComBranch , g_pub_tx.reply );
   			if ( ret == 100 )
   			{
   				break;
   			}else if ( ret )
   			{
	   			sprintf( acErrMsg, "执行Com_branch_Fet_Sel错误!ret=[%d]", ret );
	        		WRITEMSG
	        		goto ErrExit;
   			}
   		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaasComBranch.br_no=[%s]",sComBranch.br_no );		WRITEMSG	
   			/* 根据公共机构码表中的机构号、当日日期查找传真登记薄，写入临时文件中 */
			/* 声明打开游标 */
			ret = Mo_fax_Dec_Upd( g_pub_tx.reply , "tx_br_no='%s' \
						and tx_date=%ld ", sComBranch.br_no,\
						g_pub_tx.tx_date );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_fax_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

   			num = 0;
   			while(1)
			{
				memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
				memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
		
				ret = Mo_fax_Fet_Upd( &sMoFax, g_pub_tx.reply );
				if ( ret == 100  )
				{
					break;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "执行Mo_fax_Fet_Upd错[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				
				if( num == 0 )
				{
					fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
					"交易日期", "交易机构号", "传真编号", "申请人全称", \
					"付款人开户行行号", "付款人开户行行名", "付款人账号", \
					"付款人账号序号", "付款人全称", "收款人开户行行号", \
					"收款人开户行行名", "收款人账号", "收款人全称", \
					"传真金额", "传真状态", "已打印次数" );				
				}
				
				/* 检查交易机构是否是中心机构(即签发机构) */
				ret = pub_sttl_ChkSignBrno( cOperCode, sMoFax.tx_br_no, g_pub_tx.tx_br_no );
				if ( ret )
				{
					sprintf( acErrMsg, "检查机构权限错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				
				/* 根据付款人账号ID和付款人账号序号返回付款人账号 */
				ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdmAcRel , "ac_id=%ld \
							and ac_seqn=%d", sMoFax.payer_ac_id, \
							sMoFax.payer_ac_seqn );
				if( ret == 100 )
    				{
        				sprintf(acErrMsg,"取介质与账户关系表mdm_ac_rel错!无此账户信息!\
        				ac_id=[%ld], ac_seqn=[%d]", sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
        				WRITEMSG
        				strcpy( g_pub_tx.reply, "M003" );
        				goto ErrExit;
    				}else if ( ret )
    				{
        				sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret );
        				WRITEMSG
        				goto ErrExit;
    				}

				fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|%.2f|%s|%d|\n", \
					g_pub_tx.tx_date, g_pub_tx.tx_br_no, sMoFax.fax_no, \
					sMoFax.requ_name, sMoFax.payer_br_no, sMoFax.payer_br_name, \
					sMdmAcRel.ac_no, sMoFax.payer_ac_seqn, sMoFax.payer_name, \
					sMoFax.payee_br_no, sMoFax.payee_br_name, sMoFax.payee_ac_no, \
					sMoFax.payee_name, sMoFax.fax_amt, sMoFax.fax_sts, sMoFax.pr_cnt );

				num++;
			}	/* end while */
   		}	/* end while */
   		Com_branch_Clo_Sel ();
	}	/* end if */
	
	Mo_fax_Clo_Upd( );
	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

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
