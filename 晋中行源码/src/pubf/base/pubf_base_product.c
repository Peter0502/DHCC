#include "find_debug.h" 
/*************************************************
* 文 件 名:  pubf_base_product.c
* 功能描述：
*           包括 活期产品属性检查函数 pub_base_check_current();	
* 				 定期产品属性检查函数 pub_base_check_terminate();			
*                活期产品变更函数 pub_base_change_current();	
* 				 检查客户类型与购买产品种类是否相符函数 pub_base_check_cltprdt;
* 				 生成帐户ID并添加账户序号ID表函数 pub_base_get_cltid();			
* 				 生成内部帐帐号ID函数 pub_base_get_inid();			
*		 		 取客户账户序号 pub_base_get_cltidseq();
*				 根据产品代码取产品参数函数	pub_base_get_prdtparm();�
*				 显示账号转换成账号ID的函数 pub_base_disid_cltid();
*				 旧账号转换成新账号函数：pub_base_oldac_newac();
*                由账户ID和账户序号查询产品名称    pub_base_get_prdt_name
* 				 由产品号查询产品名称	pub_base_prdt_name    
* 				 根据科目控制字取科目信息	pub_base_item_inf    
* 				 根据凭证类型取凭证名称    	pub_base_get_note_name   
* 作    者: rob
* 完成日期：2003/12/27
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"
#include "clt_id_seq_c.h"
#include "in_id_seq_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mo_merge_ac_c.h"
#include "mdm_unprt_hst_c.h"
#include "ac_seqn_id_c.h"
#include "com_item_c.h"
#include "note_parm_c.h"

struct  pub_tx f_pub_tx;
struct  dd_parm_c f_dd_parm;
struct  dd_mst_c f_dd_mst;
struct  td_mst_c f_td_mst;
struct  td_mst_hst_c f_td_mst_hst;
struct  clt_id_seq_c g_clt_id_seq;					/*客户账户id结构*/
struct  in_id_seq_c g_in_id_seq;					/*内部账户id结构*/
struct  cif_basic_inf_c g_cif_basic_inf;			/*客户基本信息结构*/
struct  cif_id_code_rel_c g_cif_id_code_rel;		/*客户证件与客户号对照结构*/
struct  prdt_ac_id_c g_prdt_ac_id;					/*产品账号对照结构*/
struct  mo_merge_ac_c g_mo_merge_ac;				/*新旧账号对照结构*/
struct  mdm_unprt_hst_c g_mdm_unprt_hst;			/*未打印明细账结构*/
struct  ac_seqn_id_c ac_seqn_id;					/*账户序号ID表结构*/
struct  com_item_c com_item;						/* 公共科目表 */
struct	note_parm_c	note_parm;						/* 凭证参数表 */

static int ret=0;

/**********************************************************************
 * 函 数 名：pub_base_check_current();
 * 函数功能：活期产品属性检查函数
 *           
 * 作者/时间：
 * 
 * 参数：
 *    输入：
 *       	交易类型		char(1);	
 *		
 *     输出: 
 *           
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_check_current(tran_type)
	char *tran_type;
{
	/*根据活期产品编号取活期参数表*/
	MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期产品参数表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"查询活期产品参数表异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*根据交易类型,检查相关参数*/
	if (tran_type[0]=='1')							/*1-开户*/
	{
		/*检查开户日期是否在起始日期和终止日期之间*/	
		if ( (g_dd_parm.beg_date > g_dd_mst.opn_date) || \
			(g_dd_mst.opn_date > g_dd_parm.end_date))
		{	
			strcpy(acErrMsg,"open date is not between begin and end date!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			return 1;
		}

		/*检查开户通兑标志*/
		if (g_dd_parm.thr_opn_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A002" );
			return 1;
		}

		/*检查该产品最低开户金额和最高开户金额*/
		if ( g_dd_mst.bal<g_dd_parm.min_opn_amt )
		{
			strcpy(acErrMsg,"open bal is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			return 1;
		}
		else if ( g_dd_mst.bal>g_dd_parm.max_opn_amt)
		{
			strcpy(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			return 1;
		}

		/*检查利率是否超过规定利率上浮和下浮后的界限*/
		if ( g_dd_mst.rate < g_dd_mst.rate - g_dd_parm.min_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			return 1;
		}
		else if ( g_dd_mst.rate > g_dd_mst.rate + g_dd_parm.max_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			return 1;
		}

		/* 检查开户限制条件编码 (调用函数) */
		if ( g_dd_parm.opn_lmt == 000 )
		{
			return 1;
		}
 	}
	else if(tran_type[0]=='2')						/*2-销户*/
	{
		/*检查销户通兑标志*/
		if ( !strcmp( g_dd_parm.thr_cls_ind,"N") )
		{	
			strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A007" );
			return 1;
		}
		/* 检查销户限制条件编码 (调用函数) */
		if ( g_dd_parm.cls_lmt == 000 )
		{
			
		}
	}
	else if (tran_type[0]=='3')				/*3-存款*/
	{
		MEMSET_DEBUG( &f_dd_mst, 0x00, sizeof(struct dd_mst_c) );

		ret = Dd_mst_Sel(g_pub_tx.reply, &f_dd_mst,
						"prdt_no='%s' and ac_id=%d and ac_seqn=%d",
						g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
		if ( ret==100 )
		{
			sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W014" );
			return 1;
		}
		else if ( ret!=0 )
		{
			sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W015" );
			return 1;
		}

		/*检查存款后金额是否超过最大金额*/
		if( ( g_dd_mst.bal+g_pub_tx.tx_amt1) > g_dd_parm.max_bal )	
		{
			strcpy(acErrMsg,"amt is too high!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A008" );
			return 1;
		}

		/*检查续存标志*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*检查通存标志*/
		if ( g_dd_parm.thr_dpst_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A010" );
			return 1;
		}

		/* 检查存款限制条件编码 (调用函数)
		if ( g_dd_parm.cls_lmt == 000 )
		{

		} */
	}
	else if(tran_type[0]=='4')							/*4-取款*/
	{
		/*检查续取标志*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*检查通兑标志*/
		if (g_dd_parm.thr_draw_ind[0]=='N')
		{
			strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A011" );
			return 1;
		}

		/*检查提现标志*/
		if (g_pub_tx.ct_ind[0]=='1')
		{
			if(g_dd_parm.cash_ind[0]=='N')
			{
				strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A012" );
				return 1;
			}
		}
	}

   return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_check_terminate();
 * 函数功能：定期产品属性检查函数
 *           
 * 作者/时间：
 * 
 * 参数：
 *     输入：
 *           交易类型 				tran_type(2);
 *     输出: 
 *         	 响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_check_terminate(tran_type)
	char *tran_type;
{
	int days;

	/*根据定期产品编号取定期产品参数*/
	MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
					"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"定期产品参数表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W026" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"查询定期产品参数表异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W027" );
		return 1;
	}

	/*根据账户id,账户序号取定期产品主文件*/
	MEMSET_DEBUG( &f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret = Td_mst_Sel(g_pub_tx.reply, &f_td_mst,
 					"prdt_no='%s' and ac_id=%ld and ac_seqn=%d",
					g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
	if ( ret==100 )
	{
		sprintf(acErrMsg,"定期产品主文件中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W016" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"查询定期产品主文件异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W017" );
		return 1;
	}

	/*根据账户id,账户序号取定期存款明细表*/
	MEMSET_DEBUG( &f_td_mst_hst, 0x00, sizeof(struct td_mst_hst_c) );

	ret = Td_mst_hst_Sel(g_pub_tx.reply, &f_td_mst_hst,
						"ac_id=%ld and ac_seqn=%d and max(trace_no)",
						g_pub_tx.ac_id , g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"定期产品明细表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W028" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"查询定期产品明细表异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W029" );
		return 1;
	}

	switch( atoi(tran_type) )
	{
	  case 1:										/*1-开户*/
			/*检查开户日期是否在起始日期和终止日期之间*/	
			if ( (g_td_parm.beg_date > g_td_mst.opn_date) || \
				(g_td_mst.opn_date > g_td_parm.end_date))
			{	
				strcpy(acErrMsg,"open date is not between begin and end date!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A001" );
				break;
			}

			/*检查通兑开户标志*/
			if (g_td_parm.thr_opn_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A002" );
				break;
			}

			/*检查该产品最低开户金额和最高开户金额*/
			if ( g_td_mst.bal<g_td_parm.min_opn_amt )
			{
				strcpy(acErrMsg,"open bal is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A003" );
				break;
			}
			else if ( g_td_mst.bal>g_td_parm.max_opn_amt)
			{
				strcpy(acErrMsg,"open bal overflow!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A004" );
				break;
			}

			/*检查利率是否超过规定利率上浮和下浮后的界限 */
			if ( g_td_mst.rate < g_td_mst.rate - g_td_parm.min_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				break;
			}
			else if ( g_td_mst.rate > g_td_mst.rate + g_td_parm.max_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				break;
			}

			/* 检查开户限制条件编码 */
			if ( g_td_parm.opn_lmt == 000 )
			{
				break;
			}
			break;
	case 2:										/*2-销户*/
			/*检查预约转存标志*/
			if (g_td_parm.tfr_lmt_ind[0]=='N')
			{	
				if (g_td_mst.tfr_ind[0]=='Y')
				{
					strcpy(acErrMsg,"tfr_ind is N,transfer failed!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A013" );
					break;
				}
			}
			
			/*检查是否提前支取*/
			if (g_td_parm.fwd_draw_ind[0]=='N')
			{
				if ( g_pub_tx.tx_date < f_td_mst.mtr_date )
				{
					strcpy(acErrMsg,"advance take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A014" );
					break;
				}
			}	

			/*检查销户通兑标志*/
			if (g_td_parm.thr_cls_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A007" );
				break;
			}

			/*检查销户限制条件编码*/
			if ( g_td_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	 case 3:									/*3-存款*/
			/*检查存款后金额是否超过最大金额*/
			if( ( g_td_mst.bal + g_pub_tx.tx_amt1) > g_td_parm.max_bal )	
			{
				strcpy(acErrMsg,"amt is too high!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A008" );
				break;
			}

			/*检查续存标志*/
			if (g_td_parm.dpst_ind[0]=='N')
			{
				strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A009" );
				break;
			}
			else if ( (g_td_parm.dpst_ind[0]=='Y') || \
					 (g_td_parm.dedr_intvl != 0) )
			{
				if( g_td_parm.intvl_type[0]=='Y')	
				{
					days = 365 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='Q')
				{
					days = 90 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='M')
				{
					days = 30 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( (g_pub_tx.tx_date - f_td_mst_hst.tx_date) < days )	
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*检查最低续存金额*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.dpst_min_amt )
			{
				strcpy(acErrMsg,"save money is too low,save failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*检查通存标志*/
			if (g_dd_parm.thr_dpst_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A010" );
				break;
			}

			/* 检查存款限制条件编码*/
			if ( g_dd_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	case 4:									/*4-取款*/
			/*检查账号支取后余额是否低于产品最低余额*/
			if ( (g_td_mst.bal - g_pub_tx.tx_amt1) < g_td_parm.max_bal )
			{
				strcpy(acErrMsg,"after take,remain is not enough!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A017" );
				break;
			}

			/*检查最低续取金额*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.draw_min_amt )
			{
				strcpy(acErrMsg,"draw cash is too low,draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*检查续取期限*/
			if ( g_td_parm.dedr_intvl != 0 )
			 {
				if(g_td_parm.intvl_type[0]=='Y')
				{
				 	days = 365 * g_td_parm.dedr_intvl;
				}
			    else if(g_td_parm.intvl_type[0]=='Q')
			  	{
				    days = 90 * g_td_parm.dedr_intvl;
				}
		 		else if(g_td_parm.intvl_type[0]=='M')
			    {
				   	days = 30 * g_td_parm.dedr_intvl;
				}
				else if(g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( ( g_pub_tx.tx_date - f_td_mst_hst.tx_date )<days )
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*检查通兑标志*/
			if ( g_td_parm.thr_draw_ind[0]=='N')
			{
				strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A011" );
				break;
			}

			/*检查提现标志*/
			if (g_pub_tx.ct_ind[0]=='1')
			{
				if(g_td_parm.cash_ind[0]=='N')
				{
					strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A012" );
					break;
				}
			}	
			break;
	case 5:									/*5-部提*/
			/*检查部提标志*/
			if(g_td_parm.wdr_ind[0]=='N')
			{
				strcpy(acErrMsg,"take part cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A018" );
				break;
			}

			/*检查部提次数*/
			if (g_td_parm.wdr_lmt_cnt != 0 && \
				g_td_mst.tx_cnt == g_td_parm.wdr_lmt_cnt)
			{
				strcpy(acErrMsg,"take part cash times is full!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A019" );
				break;
			}

	default:break;		

	}
 return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_change_current();
 * 函数功能：活期产品变更函数
 *           将账户对应的活期产品改变
 * 作者/时间：
 * 
 * 参数：
 *     输入： 账户id		long;
 *     		  账户序号		int;
 *     输出: 
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_change_current( ac_id, ac_seq )
	long ac_id;
	int ac_seq; 
{
	char tran_tp[2];

	/*根据帐户ID和账户序号找到原活期产品编号*/
	MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );

	ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seq=%d",
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期产品主文件中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W014" );
		return 1;
	}
	else if (ret && ret!=100 )
	{
		sprintf(acErrMsg,"查询活期产品主文件异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W015" );
		return 1;
	}

	/*根据原活期产品编号取出原活期产品参数结构*/
	MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
						g_dd_mst.prdt_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期产品参数表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"查询活期产品参数表异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*检查旧活期产品是否符合新活期产品属性*/
	tran_tp[0]='1';
	ret = pub_base_check_current(tran_tp);

	/*根据产品编号取新产品参数*/
	MEMSET_DEBUG( &f_dd_parm, 0x00, sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &f_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期产品参数表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"查询活期产品参数表异常[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}
	
	/*修改活期产品主文件*/
	ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d", 
							g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret )
	{
		sprintf(acErrMsg,"declare Dd_mst_Dec_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D101" );
		return 1;
	}

	ret = Dd_mst_Fet_Upd( &g_dd_mst,g_pub_tx.reply );
	if ( ret&&ret!=100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D102" );
		return 1;
	}
	else if ( ret==100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D104" );
		return 1;
	}

	strcpy( g_dd_mst.prdt_no,g_pub_tx.prdt_code );

	ret = Dd_mst_Upd_Upd( g_dd_mst, g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"update Dd_mst_Upd_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D105" );
		return 1;
	}

	Dd_mst_Clo_Upd( );

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_check_cltprdt();
 * 函数功能：检查客户类型与购买产品种类是否相符函数
 *			 如个人客户不能购买对公产品，对公客户不能购买个人产品。
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：	客户类型		clt_type	char(2);
 *          	产品编号		prdt_no		char(4);
 *     输出: 
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_check_cltprdt( clt_type, prdt_no )
	char *clt_type;
	char *prdt_no;
{
	char prdt_seqn[2];
	char str_tt[41];

	struct ln_parm_c	ln_parm_cif;
	prdt_seqn[0]=prdt_no[0];

	MEMSET_DEBUG( &ln_parm_cif, 0x00, sizeof(struct ln_parm_c) );
	MEMSET_DEBUG( str_tt, 0x0 , sizeof( str_tt ) );
	
	switch(atoi(prdt_seqn))
	{
		case 1:
				/*查询活期参数表*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"prdt_no='%s' and cif_type='%s'",
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"活期产品参数表中不存在该记录[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					return 1;
				}
				else if ( ret && ret!=0 )
				{
					sprintf(acErrMsg,"查询活期产品参数表异常[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					return 1;
				}
				break;
		case 2:
				/*查询定期参数表*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );
				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
									"prdt_no='%s' and cif_type='%s'", 
									prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"定期产品参数中不存在该记录[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"查询定期产品参数表异常[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 3:
				/*查询贷款参数表*/
				if( clt_type[0]=='1' )
				strcpy( str_tt," cif_type in('1','0') " );
				if( clt_type[0]=='2' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='3' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='4' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='5' )
				strcpy( str_tt," cif_type in('1','0') " );

				sprintf(acErrMsg,"str_tt![%s]",str_tt);
				WRITEMSG
				
				ret =Ln_parm_Sel(g_pub_tx.reply, &ln_parm_cif, 
								"prdt_no='%s' and %s ", prdt_no , str_tt );
				if ( ret==100 )
				{
					sprintf(acErrMsg,"该客户客户类型与贷款产品适合的客户类型不符![%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L146" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"查询贷款产品参数表异常[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 4:
				/*查询透支参数表*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );
				ret =Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
								"prdt_no='%s' and cif_type='%s'", 
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"透支产品参数表中不存在该记录[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"查询透支产品参数表异常[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		default:
				break;
		}
return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_get_cltid();
 * 函数功能：生成客户帐户ID函数，并添加账户序号ID表
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *				客户账户ID		long;
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：lance/20050112
 * 修改原因：增加对"账户序号ID表"的添加操作
 *          
********************************************************************/
int pub_base_get_cltid( cltid )
	long	*cltid;
{
	/*取最大客户账号id,并修改，返回可用最大客户账号id*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	/* 添加账户序号ID表 */
	ac_seqn_id.ac_id = *cltid;	/* 账号ID */
	ac_seqn_id.ac_seqn_id = 200;	/* 初始化账户序号 */

	ret = Ac_seqn_id_Ins( ac_seqn_id );
	if( ret )
	{
		sprintf(acErrMsg,"添加账户序号ID表失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W042");
		return 1;
	}
	sprintf(acErrMsg,"添加账户序号ID表 PASS!");
	WRITEMSG

	return 0;

}

int pub_base_crt_cltid( cltid )
	long	*cltid;
{
	/*取最大客户账号id,并修改，返回可用最大客户账号id*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_get_inid();
 * 函数功能：生成内部帐户ID函数
 *			 
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 
 *			内部账户ID		long;
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_get_inid( inid )
	long *inid;
{
	/*取最小内部账号id,并修改，返回可用最小内部账号id*/
	ret = In_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = In_id_seq_Fet_Upd (&g_in_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_in_id_seq.ac_seqn = g_in_id_seq.ac_seqn + 1;

	ret = In_id_seq_Upd_Upd (g_in_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	In_id_seq_Clo_Upd( );

	*inid = g_in_id_seq.ac_seqn;

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_get_cltidseq();
 * 函数功能：取客户账户序号函数
 *			 
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：证件类型		
 *           证件号码
 *     输出: 
 *			客户账户序号
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_get_cltidseq(id_type,id_num,cif_seq)
	char *id_type;	
	char *id_num;
	long *cif_seq;
{
	/*取客户代码*/
	MEMSET_DEBUG( &g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c) );

	ret = Cif_id_code_rel_Sel (g_pub_tx.reply, &g_cif_id_code_rel, 
					 		"id_type='%s' and id_no='%s'", id_type , id_num);
	if ( ret && ret!=100)
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return 1;
	}
	else if( ret==100 )
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	/*取客户账号序号修改,并返回*/
	ret = Cif_basic_inf_Dec_Upd (g_pub_tx.reply, " cif_no=%d ",
									g_cif_id_code_rel.cif_no);
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Cif_basic_inf_Fet_Upd (&g_cif_id_code_rel,g_pub_tx.reply);
	if (ret && ret!=100)
	{
		sprintf(acErrMsg,"fetch cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_cif_basic_inf.cif_seqn++;

	ret = Cif_basic_inf_Upd_Upd (g_cif_basic_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Cif_basic_inf_Clo_Upd( );

	*cif_seq = g_cif_basic_inf.cif_seqn;

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_get_prdtparm();
 * 函数功能：根据产品代码取产品参数函数
 *			 
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：产品代码		
 *          
 *     输出: 
 *			产品参数数据结构
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_get_prdtparm( char *prdt_no )
{
	char prdt_type[2];

	/*取产品类型*/

	strncpy(prdt_type,prdt_no,1);

    sprintf(acErrMsg,"prdt_type[0]=[%c],[%s]",prdt_type[0],prdt_no);
    WRITEMSG

	switch ( prdt_type[0] )
	{
		case '1':										/*1-活期*/
				/*取活期参数表*/
				MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
									"prdt_no='%s' order by dc_code", prdt_no);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				break;
		case '2':										/*2-定期*/
				/*取定期参数表*/
			    sprintf(acErrMsg, "取定期产品参数表");
				WRITEMSG
				MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm,"prdt_no='%s' order by dc_code",prdt_no);

				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				break;
		case '3':										/*3-贷款*/
				/*取贷款参数表*/
				MEMSET_DEBUG( &g_ln_parm,0x00,sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm,"prdt_no='%s'", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"SELECT ln_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				break;
		case '4':										/*4-透支*/
				/*取透支参数表*/
				MEMSET_DEBUG( &g_od_parm,0x00,sizeof(struct od_parm_c) );

				ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
								prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,"SELECT od_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				break;
		case '9':										/*5-内部产品*/
				/*取内部产品参数表*/
				MEMSET_DEBUG( &g_in_parm,0x00,sizeof(struct in_parm_c) );
				/****为什么用Dd_parm_Sel  20121128
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
				****/
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				break;
		default:
			    sprintf(acErrMsg, "取产品参数表错误");
				WRITEMSG
				break;
	}
	return 0;
}
/**********************************************************************
 * 函 数 名：pub_base_disid_cltid();
 * 函数功能：显示账号转换成账号ID的函数
 *			 
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：显示账号		
 *          
 *     输出: 
 *			账号ID
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_disid_cltid( ac_no , ac_id )
	char *ac_no;
	long *ac_id;
{
	/*根据显示账号转换成账号ID*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, 
						"ac_no='%s'", ac_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mdm_ac_rel, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W010" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W011" );
		return 1;
	}

	*ac_id = g_mdm_ac_rel.ac_id;

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_oldac_newac();
 * 函数功能：旧账号转换成新账号函数�
 *			 
 *
 * 作者/时间：rob/2003/12/31
 * 
 * 参数：
 *     输入：旧账号		
 *          
 *     输出: 
 *			新账号
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_oldac_newac( old_ac,new_ac )
	char *old_ac;
	char *new_ac;
{
	/*根据显示账号转换成账号ID*/
	MEMSET_DEBUG( &g_mo_merge_ac, 0x00, sizeof(struct mo_merge_ac_c) );

	ret = Mo_merge_ac_Sel(g_pub_tx.reply, &g_mo_merge_ac, 
							"old_ac_no='%s'", old_ac);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mo_merge_ac, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W036" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,
		"SELECT mo_merge_ac error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W037" );
		return 1;
	}
	strcpy (new_ac , g_mo_merge_ac.ac_no);

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_wrt_unprt();
 * 函数功能：登记未打印明细帐函数
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：pub_tx 		公共记账结构		
 *          
 *     输出: 
 *         	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_base_wrt_unprt( f_pub_tx )
	struct pub_tx f_pub_tx;
{
	char prdt_type[2];

	g_mdm_unprt_hst.trace_no = f_pub_tx.trace_no;			/*流水号*/
	g_mdm_unprt_hst.trace_cnt = f_pub_tx.trace_cnt;			/*流水笔次*/
	strcpy (g_mdm_unprt_hst.tx_br_no,f_pub_tx.tx_br_no);	/*交易机构号*/	
	g_mdm_unprt_hst.ac_id = f_pub_tx.ac_id;					/*账号ID*/
	g_mdm_unprt_hst.ac_seqn = f_pub_tx.ac_seqn;				/*账户序号*/
	strcpy (g_mdm_unprt_hst.add_ind , f_pub_tx.add_ind);	/*增减标志*/	
	g_mdm_unprt_hst.tx_amt = f_pub_tx.tx_amt1;				/*交易金额*/
	g_mdm_unprt_hst.tx_date = f_pub_tx.tx_date;				/*交易日期*/
	strcpy (g_mdm_unprt_hst.tel , f_pub_tx.tel);			/*操作员*/
	strcpy (g_mdm_unprt_hst.brf , f_pub_tx.tel);			/*摘要*/

	/*根据账号id和账户序号ID查找产品类型*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, 
							"ac_id=%ld and ac_seqn=%d", 
							f_pub_tx.ac_id,f_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table prdt_ac_id, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W012" );
		return 1;
	}
	else if ( ret!=0 )
	{
		sprintf(acErrMsg,
		"SELECT prdt_ac_id error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W013" );
		return 1;
	}

	/*取产品类型*/
	strncpy ( prdt_type,g_prdt_ac_id.ac_id_type,1 );

	switch ( atoi(prdt_type) )
	{
		case 1:										/*1-活期*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"ac_id='%d' and ac_seqn='%d'", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				g_mdm_unprt_hst.bal = g_dd_mst.bal;				/*余额*/
				g_mdm_unprt_hst.hst_cnt = g_dd_mst.hst_cnt;		/*明细笔数*/

				break;
		case 2:										/*2-定期*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				g_mdm_unprt_hst.bal = g_td_mst.bal;				/*余额*/
				g_mdm_unprt_hst.hst_cnt = g_td_mst.hst_cnt;		/*明细笔数*/

				break;
		case 3:										/*3-贷款*/
				MEMSET_DEBUG( &g_ln_parm, 0x00, sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT ln_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				g_mdm_unprt_hst.bal = g_ln_mst.bal;				/*余额*/
				g_mdm_unprt_hst.hst_cnt = g_ln_mst.hst_cnt;		/*明细笔数*/

				break;
		case 4:										/*4-透支*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );

				ret = Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
				"ac_id=%ld and ac_seqn=%d", f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT od_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				g_mdm_unprt_hst.bal = g_od_mst.bal;				/*余额*/
				g_mdm_unprt_hst.hst_cnt = g_od_mst.hst_cnt;		/*明细笔数*/

				break;
		case 9:										/*9-内部产品*/
				MEMSET_DEBUG( &g_in_parm, 0x00, sizeof(struct in_parm_c) );

				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				g_mdm_unprt_hst.bal = g_in_mst.bal;				/*余额*/
				g_mdm_unprt_hst.hst_cnt = g_in_mst.hst_cnt;		/*明细笔数*/

				break;
		default:
				break;
	}

	return 0;
}

/**********************************************************************
* 函数名：     	pub_base_get_prdt_name
* 函数功能：    由账户ID和账户序号查询产品名称
* 作者/时间：   lance/2005年07月04日
* 
* 参数：
*     输入： ac_id       ：账户ID
*            ac_seqn     : 账户序号
*           
*     输出： prdt_name   : 产品名称
*
* 返 回 值: 0 成功,1 失败
*           
* 修改历史：
*
********************************************************************/
int pub_base_get_prdt_name( long ac_id, int ac_seqn, char *prdt_name )
{    
	
	MEMSET_DEBUG( &g_prdt_ac_id , 0x00 ,sizeof( struct prdt_ac_id_c ));
	
	g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"FETCT 产品帐号对照表异常!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"活期明细文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*产品名称*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
						"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"定期明细文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*产品名称*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"贷款明细文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*产品名称*/							
			return 0;
		default:
			break;
	}/*switch结束*/

	return 1;
}
/**********************************************************************
* 函数名：     	pub_base_prdt_name
* 函数功能：    由产品号查询产品名称
* 作者/时间：   lance/2005年07月04日
* 
* 参数：
*     输入： prdt_no     ：产品代码
*           
*     输出： prdt_name   : 产品名称
*
* 返 回 值: 0 成功,1 失败
*           
* 修改历史：
*
********************************************************************/
int pub_base_prdt_name( char *prdt_no, char *prdt_name )
{    
	
	sprintf( acErrMsg,"prdt_no[%s],prdt_name[%s]",prdt_no,prdt_name);
	WRITEMSG
	if( strcmp(prdt_no,"临")==0 )
	{
		strcpy( prdt_name , "临时名称" );
		return 0;
	}
	switch(prdt_no[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"活期参数文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*产品名称*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
									prdt_no);
			if( g_reply_int==100 )
			{
				sprintf(acErrMsg,"定期参数文件不存在该记录![%s]",prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*产品名称*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"贷款参数文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*产品名称*/							
			return 0;
		case '4':
			g_reply_int=Od_parm_Sel(g_pub_tx.reply,&g_od_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"透支参数文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_od_parm.title);			/*产品名称*/							
			return 0;
		case '9':
			g_reply_int=In_parm_Sel(g_pub_tx.reply,&g_in_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"内部账参数文件不存在该记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_in_parm.prdt_mo);			/*产品名称*/							
			return 0;
		default:
			strcpy( g_pub_tx.reply,"D103" );
			break;
	}/*switch结束*/

	return 1;
}
/**********************************************************************
* 函数名：     	pub_base_item_inf
* 函数功能：    根据科目控制字取科目信息
* 作者/时间：   lance/2005年07月05日
* 
* 参数：
*     输入： 	acc_hrt   	   ：科目控制字
*           
*     输出： 	struct	com_item   : 科目信息结构
*
* 返 回 值: 0 成功,1 失败
*           
* 修改历史：
*
********************************************************************/
int pub_base_item_inf( char *acc_hrt, struct com_item_c	*com_item )
{    
	int ret=0;
	
	MEMSET_DEBUG( com_item , 0x00 , sizeof(struct com_item_c) );
	
	sprintf( acErrMsg,"acc_hrt[%s]",acc_hrt);
	WRITEMSG


	ret = Com_item_Sel( g_pub_tx.reply , com_item , " acc_hrt='%s' " , acc_hrt );
	if( ret )
	{
		sprintf(acErrMsg,"读取科目表信息异常出错!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );			
		return 1;
	}
	
	return 0;
}

/**********************************************************************
* 函数名：     	pub_base_get_note_name
* 函数功能：    根据凭证类型取凭证名称
* 作者/时间：   lance/2005年07月06日
* 
* 参数：
*     输入： 	note_type   	   ：凭证类型
*           
*     输出： 	note_name	   : 凭证名称
*
* 返 回 值: 0 成功,1 失败
*           
* 修改历史：
*
********************************************************************/
int pub_base_get_note_name( char *note_type, char *note_name )
{    
	int ret=0;
	
	MEMSET_DEBUG( &note_parm , 0x00 , sizeof( struct note_parm_c ) );
	
	sprintf( acErrMsg,"note_type[%s]",note_type);
	WRITEMSG

	ret = Note_parm_Sel( g_pub_tx.reply , &note_parm , "note_type='%s'" , note_type );
	if( ret==100 )
	{
		sprintf(acErrMsg,"凭证参数表中无此凭证!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N085" );			
		return 1;
	}else if( ret ){
		sprintf(acErrMsg,"查询凭证参数表异常出错,请与中心机房联系!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N086" );			
		return 1;
	}else if( ret==0 ){
		
		/* 检查凭证是否处于使用状态 */
		if( note_parm.use_ind[0] == 'N' )
		{
			sprintf( acErrMsg,"凭证处于停用状态!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N025" );
			return 1;
		}		
	}
	strcpy( note_name , note_parm.name );
	
	return 0;
}
