/*************************************************
* 文 件 名:     pubf_eod_prot.c
* 功能描述：    日终理财处理
* 作    者:     rob
* 完成日期：    2004年03月22日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_prot_parm_c.h"
#include "mo_prdt_prot_c.h"
#include "prot_hst_c.h"
#include "mo_prot_ac_rel_c.h"
#include "mo_infrm_c.h"

/**********************************************************************
 * 函 数 名：   pub_eod_prot
 * 函数功能：   日终理财处理 
 * 作者/时间：  
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/

int pub_eod_prot(mo_prdt_prot_c)
struct mo_prdt_prot_c mo_prdt_prot_c;
{
    int ret;
	int turn_flag;		/*** 零表示不够化转条件，壹表示满足化转条件 ***/
	int	unchain_flag;	/*** 解除协议理财关系标志 ***/
	double Tamt=0.00;
	struct prdt_prot_parm_c prdt_prot_parm_c;
	struct mo_infrm_c mo_infrm_c;
	struct mo_prot_ac_rel_c mo_prot_ac_rel_c;
	struct td_mst_c g_td_mst1;
	struct pub_tx v_pub_tx;
	
	ret=0;
	turn_flag=0;
	unchain_flag=0;
	memset(&prdt_prot_parm_c,0x00,sizeof(struct prdt_prot_parm_c) );
	memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c) );
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c) );
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c) );
	memset(&g_td_mst1,0x00,sizeof(struct td_mst_c) );

	ret=Prdt_prot_parm_Dec_Sel(g_pub_tx.reply,"prot_type='%s' and deal_seq=1",mo_prdt_prot_c.prot_type);
	if( ret )
	{
		sprintf(acErrMsg,"prdt_prot_parm_dec_sel erro ![%d]",ret );
		WRITEMSG
		return 1;
	}

	ret=Prdt_prot_parm_Fet_Sel( &prdt_prot_parm_c,g_pub_tx.reply );
	if( ret )
	{
		sprintf(acErrMsg,"prdt_prot_parm_fet_sel erro ![%d]",ret );
		WRITEMSG
		return 1;
	}

	Prdt_prot_parm_Clo_Sel();
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/

	switch(prdt_prot_parm_c.turn_cond[0])
	{
		case 'A':	/** 协定划款、理财套餐 **/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%d and ac_seqn=%d",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
				WRITEMSG
				return 1;
			}
			strcpy(g_pub_tx.name,g_dd_mst.name);
			if( g_dd_mst.bal-mo_prdt_prot_c.leave_amt-prdt_prot_parm_c.min_amt>0.005)
			{
				turn_flag=1;
				/** 应化转总金额 **/
				g_pub_tx.tx_amt1=((g_dd_mst.bal-mo_prdt_prot_c.leave_amt)/
					prdt_prot_parm_c.turn_unit)*prdt_prot_parm_c.turn_unit;
			}
			else
			{
				/**add by ligl 2009-4-15 17:24 B划转A**/
				if(pub_base_CompDblVal(g_dd_mst.bal-mo_prdt_prot_c.leave_amt,0.00)<0)
				{
						double need_return_amt=0.00;
						need_return_amt=mo_prdt_prot_c.leave_amt-g_dd_mst.bal;
	          vtcp_log("%s,%dAAAAAAAAAAAAAAAA need_return_amt=%.2lf,g_dd_mst.bal=%.2lf\n",__FILE__,__LINE__,need_return_amt,g_dd_mst.bal);
			  		ret=pub_eod_prot1(mo_prdt_prot_c,need_return_amt);
            /**不管ret 为0还是1 都要下面赋值，这样保证A不划转B**/
			  		turn_flag=0;
			  		g_pub_tx.tx_amt1=0.00;
			  }
				else
				{
				  turn_flag=0;
				  g_pub_tx.tx_amt1=0.00;
			  }
			}
			break;
		case 'B':	/** 活期转零整 **/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
				"ac_id=%d and ac_seqn=%d",
				mo_prdt_prot_c.main_ac_id, mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
				WRITEMSG
				return 1;
			}
			ret=Mo_prot_ac_rel_Dec_Sel(g_pub_tx.reply,"main_ac_id=%d and main_ac_seqn=%d and sts='1' ",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取从账号错误![%d]",ret);
				WRITEMSG
				return 1;
			}
			ret=Mo_prot_ac_rel_Fet_Sel(&mo_prot_ac_rel_c,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"取从账号错误![%d]",ret);
				WRITEMSG
				return 1;
			}
			Mo_prot_ac_rel_Clo_Sel();

			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%d and ac_seqn=%d",mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
			    mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,ret);
				WRITEMSG
				return 1;
			}
			g_pub_tx.tx_amt1=g_td_mst.td_amt;

			if(g_dd_mst.bal-mo_prdt_prot_c.turn_amt<mo_prdt_prot_c.leave_amt)
				{
					turn_flag=0;
				}
			else
			{
			if( g_dd_mst.bal-g_td_mst.td_amt-prdt_prot_parm_c.min_bal>0.005 
				&& pub_chk_td_mst_hst()==0 )
			{
				turn_flag=1;
				g_pub_tx.tx_amt1=g_td_mst.td_amt;
			}
			else {
				turn_flag=0;
				g_pub_tx.tx_amt1=0.00;
				sprintf(acErrMsg,"[%.2f],[%.2f],[%ld],[%ld]",
			g_dd_mst.bal,g_td_mst.td_amt,g_td_mst.lst_date,g_pub_tx.tx_date);
				WRITEMSG
			}
			}
			break;
		case 'C':	/** 定期到期类型 **/
				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%d and ac_seqn=%d",mo_prdt_prot_c.main_ac_id, mo_prdt_prot_c.main_ac_seqn);
				if( ret )
				{
					sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
				if( g_td_mst.mtr_date==g_pub_tx.tx_date )
				{
					turn_flag=1;
					g_pub_tx.tx_amt1=g_td_mst.bal;
				}
				else
				{
					turn_flag=0;
					g_pub_tx.tx_amt1=0.00;
					sprintf(acErrMsg,"[%ld],[%ld]",g_td_mst.mtr_date,g_pub_tx.tx_date);
					WRITEMSG
				}
				break;
		case 'D':	/** 存本取息转零整类型 **/
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%d and ac_seqn=%d",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
				WRITEMSG
				return 1;
			}
			g_pub_tx.tx_amt1=g_td_mst.td_amt;

			ret=Mo_prot_ac_rel_Dec_Sel(g_pub_tx.reply,"main_ac_id=%d and main_ac_seqn=%d and sts='1' ",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取从账号错误![%d]",ret);
				WRITEMSG
				return 1;
			}
			ret=Mo_prot_ac_rel_Fet_Sel(&mo_prot_ac_rel_c,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"取从账号错误![%d]",ret);
				WRITEMSG
				return 1;
			}
			Mo_prot_ac_rel_Clo_Sel();

			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst1,"ac_id=%d and ac_seqn=%d",mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
			    mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,ret);
				WRITEMSG
				return 1;
			}

				if(pub_base_CalMouths(g_td_mst.lst_date,g_pub_tx.tx_date)>0.9 
				&& pub_base_CompDblVal(g_td_mst.td_amt,g_td_mst1.td_amt)==0 
				&& pub_base_CalMouths(g_td_mst1.lst_date,g_pub_tx.tx_date)>0.9)
				{
					turn_flag=1;
					g_pub_tx.tx_amt1=g_td_mst.td_amt;
				}
				else
				{
					turn_flag=0;
					g_pub_tx.tx_amt1=0.00;
					sprintf(acErrMsg,"[%.2f],[%.2f],[%ld],[%ld],[%ld]",g_td_mst.td_amt,g_td_mst1.td_amt,g_td_mst.lst_date,g_td_mst1.lst_date,g_pub_tx.tx_date);
					WRITEMSG
				}
				break;
		case 'E':	/** 存本取息转活期类型 **/
				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%d and ac_seqn=%d",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
				if( ret )
				{
					sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
					WRITEMSG
					return 1;
				}
				if(pub_base_CalMouths(g_td_mst.lst_date,g_pub_tx.tx_date)>0.9) 
				{
					turn_flag=1;
					g_pub_tx.tx_amt1=g_td_mst.td_amt;
				}
				else
				{
					turn_flag=0;
					g_pub_tx.tx_amt1=0.00;
					sprintf(acErrMsg,"[%ld],[%ld]",g_td_mst.mtr_date,
							g_pub_tx.tx_date);
					WRITEMSG
				}
				break;
		case 'F':	/** 结计利息入本户（通知套餐A） **/
				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
					"ac_id=%d and ac_seqn=%d",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
				if( ret )
				{
					sprintf(acErrMsg,"取定期主文件错误 [%d][%d][%d]",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
					WRITEMSG
					return 1;
				}
				ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
					"prdt_no='%s'", g_td_mst.prdt_no );
				if( ret )
				{
					sprintf(acErrMsg,"取定期参数文件错误 prdt_no[%s]",
						g_td_mst.prdt_no);
					WRITEMSG
					return 1;
				}
				ret=Mo_infrm_Sel( g_pub_tx.reply,&mo_infrm_c,
					"ac_id=%d and ac_seqn=%d and sts='0' ",g_td_mst.ac_id,g_td_mst.ac_seqn);
				if( ret==100 )
					;
				else if( ret==0 )
				{
					sprintf(acErrMsg,"TZA已经登记支取不转存" );
					WRITEMSG
					turn_flag=0;
					break;
				}
				else
				{
					sprintf(acErrMsg,"取mo_infrm文件错误 prdt_no[%s]",
						g_td_mst.prdt_no);
					WRITEMSG
					return 1;
				}

vtcp_log("TZA ac_id=%d and ac_seqn=%d ",g_td_mst.ac_id,g_td_mst.ac_seqn);
				if( g_td_parm.td_type[0]=='1' )
				{
					if( g_td_mst.mtr_date!=g_pub_tx.tx_date )
					{
						sprintf(acErrMsg," 非到期日",g_td_mst.mtr_date);
						WRITEMSG
					}
					else
						turn_flag=1;
				}
				else if( g_td_parm.td_type[0]=='4' )
				{
TRACE
					if( pub_base_CalTrueDays(g_td_mst.ic_date,g_pub_tx.tx_date) 
						+1>=g_td_parm.term )
					{
TRACE
						turn_flag=1;
					}
					else
					{
						sprintf(acErrMsg," ic_date[%d],txdate[%d],term[%d]",
							g_td_mst.ic_date,g_pub_tx.tx_date,g_td_parm.term);
						WRITEMSG
					}
				}
				else
				{
					sprintf(acErrMsg,"不合类划转类型[%s]",g_td_parm.td_type);
					WRITEMSG
					return 1;
				}

				if( turn_flag==1 )
					g_pub_tx.tx_amt1=g_td_mst.bal;
				else
					g_pub_tx.tx_amt1=0.00;

				break;
		case 'H':	/** 通知套餐B **/
vtcp_log("TZTCB[%d]--",mo_prdt_prot_c.main_ac_id);
			ret=Prdt_prot_parm_Sel( g_pub_tx.reply,&prdt_prot_parm_c,
				"prot_type='%s'",mo_prdt_prot_c.prot_type);
			if( ret ) { TRACE; return 1; }

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
				"prdt_no='%s'", prdt_prot_parm_c.app_prdt_code );
			if( ret ) { TRACE; return 1; }

			ret=Mo_prot_ac_rel_Sel( g_pub_tx.reply,&mo_prot_ac_rel_c,
				"main_ac_id=%d and main_ac_seqn=%d",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret ) { TRACE; return 1; }

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
				"ac_id=%d and ac_seqn=%d",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
				WRITEMSG
				return 1;
			}
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
				"prdt_no='%s'", g_dd_mst.prdt_no );
			if( ret ) { TRACE; return 1; }

				strcpy(g_pub_tx.name,g_dd_mst.name);

vtcp_log("TCB [%.2lf] [%.2lf]",g_dd_mst.bal,g_td_parm.min_opn_amt);
			if( pub_base_CompDblVal(g_dd_mst.bal,g_td_parm.min_opn_amt)>=0 )
			{
				if( pub_base_CompDblVal(mo_prdt_prot_c.turn_amt,0.00)
					|| pub_base_CompDblVal(mo_prdt_prot_c.turn_amt,
						g_dd_mst.bal)>0 )
				{
					ret=sql_execute("update mo_prdt_prot set turn_amt=%.2lf,leave_amt=leave_amt+%.2lf where main_ac_id=%d and main_ac_seqn=%d",
						g_dd_mst.bal,g_dd_mst.bal,
						mo_prdt_prot_c.main_ac_id,
						mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
					mo_prdt_prot_c.turn_amt+=g_dd_mst.bal;
					mo_prdt_prot_c.leave_amt+=g_dd_mst.bal;
vtcp_log("TCB LEAVE[%.2lf]",mo_prdt_prot_c.leave_amt);
				}
				else
				{
					ret=sql_execute("update mo_prdt_prot set leave_amt=leave_amt+%.2lf where main_ac_id=%d and main_ac_seqn=%d",
						g_dd_mst.bal,
						mo_prdt_prot_c.main_ac_id,
						mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
					mo_prdt_prot_c.leave_amt+=g_dd_mst.bal;
				}
					if( pub_base_CalTrueDays(mo_prot_ac_rel_c.tx_date,
						g_pub_tx.tx_date)>=g_td_parm.term )
					{
						turn_flag=1;
					}
			}
			else
			{
vtcp_log("update mo_prot_ac_rel set tx_date=%d where main_ac_id=%d and main_ac_seqn=%d", g_pub_tx.tx_date, mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);

					ret=sql_execute("update mo_prot_ac_rel set tx_date=%d where main_ac_id=%d and main_ac_seqn=%d",
						g_pub_tx.tx_date,
						mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
					ret=sql_execute("update mo_prdt_prot set turn_amt=0.00,leave_amt=0.00 where main_ac_id=%d and main_ac_seqn=%d",
						mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
			}
			break;
		case 'J':	/** 公积金活期转定期，一年一转 **/
				ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
					"ac_id=%d and ac_seqn=%d",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
				if( ret )
				{
					sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
					mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.name,g_dd_mst.name);
				vtcp_log("tx_date=[%d]",g_pub_tx.tx_date);
				if( pub_base_CompDblVal(g_dd_mst.bal,0.00)>0 && 
					(g_pub_tx.tx_date%10000)==101 )
				{
					turn_flag=1;
					/** 应化转总金额 **/
					g_pub_tx.tx_amt1=g_dd_mst.bal;
				}
				else
				{
					turn_flag=0;
					g_pub_tx.tx_amt1=0.00;
				}
				break;
		default :
				turn_flag=0;
				g_pub_tx.tx_amt1=0.00;
				sprintf(acErrMsg,"无此化转条件!%s]",prdt_prot_parm_c.turn_cond);
				WRITEMSG
				return 1;
	}

	if( turn_flag==0 )	/** 不满足化转条件 **/
	{
		sprintf(acErrMsg,"不满足化转条件!");
		WRITEMSG
		return 0;
	}

	/*** 结算户划款处理 ***/
	switch(prdt_prot_parm_c.deal_func[0])
	{
		case 'A':
		case 'B':
		case 'F':
		case 'J':
				strcpy(g_pub_tx.ac_no,"");
				g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
				g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
				strcpy(g_pub_tx.add_ind,"0"); /**减少**/
				strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
				strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				g_pub_tx.svc_ind=1001; /**存取款***//* strcpy(g_pub_tx.ac_id_type,"1"); delete by liuxuan*/
				strcpy(g_pub_tx.ac_id_type,"");
				strcpy(g_pub_tx.note_type,"");
				strcpy(g_pub_tx.beg_note_no,"");
				if( pub_acct_trance() )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					return 1;
				}	
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC03");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}

				/*** 存本取息转活期转零整，取息次数到最后自动解除协议 ***/
				pub_base_strpack(mo_prdt_prot_c.prot_type);
				if( ( !strcmp(mo_prot_ac_rel_c.prot_type,"YY5") 
					|| !strcmp(mo_prot_ac_rel_c.prot_type,"YY6") )
					&& g_td_mst.ttl_cnt==g_td_mst.tx_cnt )
					unchain_flag=1;
				break;
		case 'D':	/** 结算户销户 **/
		case 'C':	/** 结算户销户 **/
		case 'E':	/** 结算户销户 **/
				g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
				g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
				strcpy(g_pub_tx.add_ind,"0");
				strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
				strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				g_pub_tx.svc_ind = 1003;
				strcpy(g_pub_tx.ac_id_type,"");
				if( pub_acct_trance() )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					return 1;
				}	
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC03");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}
				break;
		case 'G':
TRACE
			memcpy( &v_pub_tx,&g_pub_tx,sizeof(g_pub_tx) );
			g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
			g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
			strcpy(g_pub_tx.add_ind,"0");
			strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				ret=pub_eod_Intst_tza_mst();
				if( ret )
				{
					sprintf(acErrMsg,"计算利息错误!");
					WRITEMSG
					return 1;
				}
			memcpy( &g_pub_tx,&v_pub_tx,sizeof(g_pub_tx) );
				break;
		case 'H':
			memcpy( &v_pub_tx,&g_pub_tx,sizeof(g_pub_tx) );
			g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
			g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
			strcpy(g_pub_tx.add_ind,"0");
			strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				ret=pub_eod_Intst_tzb_mst( prdt_prot_parm_c.app_prdt_code,
					mo_prdt_prot_c.leave_amt );
				if( ret )
				{
					sprintf(acErrMsg,"计算利息错误!");
					WRITEMSG
					return 1;
				}
vtcp_log("UPDATE mo_prot_ac_rel set tx_date=%d where main_ac_id=%d and main_ac_seqn=%d", g_pub_tx.tx_date, mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);

					ret=sql_execute("update mo_prot_ac_rel set tx_date=%d where main_ac_id=%d and main_ac_seqn=%d",
						g_pub_tx.tx_date,
						mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
					ret=sql_execute("update mo_prdt_prot set turn_amt=0.00,leave_amt=0.00 where main_ac_id=%d and main_ac_seqn=%d",
						mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
					if( ret ) 
					{
						strcpy( g_pub_tx.reply,"BBCL" );
						return 1;
					}
			memcpy( &g_pub_tx,&v_pub_tx,sizeof(g_pub_tx) );
				break;
		default :
				sprintf(acErrMsg,"无此化转函数!%s]",prdt_prot_parm_c.deal_func);
				WRITEMSG
				return 1;
	}

	/*** 协定理财户处理 ***/
	switch(prdt_prot_parm_c.deal_func[0])
	{
		case 'A':
				ret=Mo_prot_ac_rel_Dec_Sel(g_pub_tx.reply,"main_ac_id=%d and main_ac_seqn=%d and sts='1' ",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
                if( ret )
                {
                   sprintf(acErrMsg,"取从账号错误![%d]",ret);
                   WRITEMSG
                   return 1;
                }
                ret=Mo_prot_ac_rel_Fet_Sel(&mo_prot_ac_rel_c,g_pub_tx.reply);
                if( ret )
                {
                   sprintf(acErrMsg,"取从账号错误![%d]",ret);
                   WRITEMSG
                   return 1;
                }
                Mo_prot_ac_rel_Clo_Sel();

				g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
				g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
				strcpy(g_pub_tx.ac_no,"");
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
				strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				g_pub_tx.svc_ind=1008; /**存取款***//*strcpy(g_pub_tx.ac_id_type,"2"); delete by liuxuan*/
				strcpy(g_pub_tx.ac_id_type,"");
				if( pub_acct_trance() )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					return 1;
				}	

				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}

				/*** 活期转零整，零整到期后自动解除协定 ***/
				/*** 存本取息转零整，零整到期后自动解除协定 ***/
				if(  g_td_mst.ttl_cnt==g_td_mst.tx_cnt 
					&& strcmp(prdt_prot_parm_c.prot_type,"XD1") )
					unchain_flag=1;
				break;
		case 'B':
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=9999", mo_prdt_prot_c.main_ac_id);
				if( ret )
				{
					sprintf(acErrMsg,"取显示账号错误![%ld][%d]",
						mo_prdt_prot_c.main_ac_id,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
				strcpy(g_pub_tx.cur_no,RMB);
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.ac_type,"6");
				strcpy(g_pub_tx.brf,"理财开户");
				strcpy(g_pub_tx.hst_ind,"1");
				g_pub_tx.svc_ind=1000;

				if( mo_prdt_prot_c.m3+mo_prdt_prot_c.m6+
					mo_prdt_prot_c.y1+mo_prdt_prot_c.y2!=100 )
				{
					sprintf(acErrMsg,"理财套餐百分比错误[%d],[%d],[%d],[%d]",
						mo_prdt_prot_c.m3,mo_prdt_prot_c.m6,
						mo_prdt_prot_c.y1,mo_prdt_prot_c.y2);
					WRITEMSG
					return 1;
				}

				if( mo_prdt_prot_c.m3>0 )
				{
					strcpy(g_pub_tx.prdt_code,"201");
					g_pub_tx.tx_amt1=g_pub_tx.tx_amt1*mo_prdt_prot_c.m3/100;
					if( pub_acct_opn_ac() )
					{
						sprintf(acErrMsg,"协定户开户错误!");
						WRITEMSG
						return 1;
					}
					set_zd_data("102K",g_pub_tx.ct_ind);
					/****改用100域第2子域  modify at 20130517
					set_zd_double("102I",g_pub_tx.tx_amt1);
					****/
					set_zd_double("1002",g_pub_tx.tx_amt1);
					g_reply_int=pubf_acct_proc("LC02");
					if( g_reply_int )
					{
						sprintf(acErrMsg,"登记会计流水错误!");
						WRITEMSG
						return 1;
					}
					
					if( pub_eod_ins_prot_ac_rel(mo_prdt_prot_c) )
					{
						sprintf(acErrMsg,"登记协定账号对照表错误!");
						WRITEMSG
						return 1;
					}
				}
				if( mo_prdt_prot_c.m6>0 )
				{
					strcpy(g_pub_tx.prdt_code,"202");
					g_pub_tx.tx_amt1=g_pub_tx.tx_amt1*mo_prdt_prot_c.m6/100;
					if( pub_acct_opn_ac() )
					{
						sprintf(acErrMsg,"协定户开户错误!");
						WRITEMSG
						return 1;
					}
					set_zd_data("102K",g_pub_tx.ct_ind);
					/****改用100域第2子域  modify at 20130517
					set_zd_double("102I",g_pub_tx.tx_amt1);
					****/
					set_zd_double("1002",g_pub_tx.tx_amt1);
					g_reply_int=pubf_acct_proc("LC02");
					if( g_reply_int )
					{
						sprintf(acErrMsg,"登记会计流水错误!");
						WRITEMSG
						return 1;
					}
					if( pub_eod_ins_prot_ac_rel(mo_prdt_prot_c) )
					{
						sprintf(acErrMsg,"登记协定账号对照表错误!");
						WRITEMSG
						return 1;
					}
				}
				if( mo_prdt_prot_c.y1>0 )
				{
					strcpy(g_pub_tx.prdt_code,"204");
					g_pub_tx.tx_amt1=g_pub_tx.tx_amt1*mo_prdt_prot_c.y1/100;
					if( pub_acct_opn_ac() )
					{
						sprintf(acErrMsg,"协定户开户错误!");
						WRITEMSG
						return 1;
					}
					set_zd_data("102K",g_pub_tx.ct_ind);
					/****改用100域第2子域  modify at 20130517
					set_zd_double("102I",g_pub_tx.tx_amt1);
					****/
					set_zd_double("1002",g_pub_tx.tx_amt1);
					g_reply_int=pubf_acct_proc("LC02");
					if( g_reply_int )
					{
						sprintf(acErrMsg,"登记会计流水错误!");
						WRITEMSG
						return 1;
					}
					if( pub_eod_ins_prot_ac_rel(mo_prdt_prot_c) )
					{
						sprintf(acErrMsg,"登记协定账号对照表错误!");
						WRITEMSG
						return 1;
					}
				}
				if( mo_prdt_prot_c.y2>0 )
				{
					strcpy(g_pub_tx.prdt_code,"205");
					g_pub_tx.tx_amt1=g_pub_tx.tx_amt1*mo_prdt_prot_c.y2/100;
					if( pub_acct_opn_ac() )
					{
						sprintf(acErrMsg,"协定户开户错误!");
						WRITEMSG
						return 1;
					}
					set_zd_data("102K",g_pub_tx.ct_ind);
					/****改用100域第2子域  modify at 20130517
					set_zd_double("102I",g_pub_tx.tx_amt1);
					****/
					set_zd_double("1002",g_pub_tx.tx_amt1);
					g_reply_int=pubf_acct_proc("LC02");
					if( g_reply_int )
					{
						sprintf(acErrMsg,"登记会计流水错误!");
						WRITEMSG
						return 1;
					}
					if( pub_eod_ins_prot_ac_rel(mo_prdt_prot_c) )
					{
						sprintf(acErrMsg,"登记协定账号对照表错误!");
						WRITEMSG
						return 1;
					}
				}
				break;
		case 'C':   /** 定期本金利息转入定期 **/
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",
					mo_prdt_prot_c.main_ac_id);
				if( ret )
				{
					sprintf(acErrMsg,"取显示账号错误![%ld][%d]",
						mo_prdt_prot_c.main_ac_id,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
				strcpy(g_pub_tx.cur_no,RMB);
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.brf,"理财开户");
				strcpy(g_pub_tx.hst_ind,"1");
				g_pub_tx.svc_ind=1000;
				g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval;
				if( pub_acct_opn_ac() )
				{
					sprintf(acErrMsg,"协定户开户错误!");
					WRITEMSG
					return 1;
				}
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}
				/*** 自动更新协议登记簿的结算账号 ***/
				unchain_flag=2;
				break;
		case 'D':   /** 零整本金利息转入定期 **/
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",
					mo_prdt_prot_c.main_ac_id);
				if( ret )
				{
					sprintf(acErrMsg,"取显示账号错误![%ld][%d]",
						mo_prdt_prot_c.main_ac_id,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
				strcpy(g_pub_tx.cur_no,RMB);
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.brf,"理财开户");
				strcpy(g_pub_tx.hst_ind,"1");
				g_pub_tx.svc_ind=1000;
				g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval;
				/*** 定期产品编号 暂时定为204***/
				strcpy(g_pub_tx.prdt_code,"204");
				if( pub_acct_opn_ac() )
				{
					sprintf(acErrMsg,"协定户开户错误!");
					WRITEMSG
					return 1;
				}
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}
				/*** 到期后自动解除协议 ***/
				unchain_flag=1;
				break;
		case 'E':   /** 定期本金转入定期 利息转入活期**/
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",
					mo_prdt_prot_c.main_ac_id);
				if( ret )
				{
					sprintf(acErrMsg,"取显示账号错误![%ld][%d]",
						mo_prdt_prot_c.main_ac_id,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
				strcpy(g_pub_tx.cur_no,RMB);
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.brf,"理财开户");
				strcpy(g_pub_tx.hst_ind,"1");
				g_pub_tx.svc_ind=1000;
				if( pub_acct_opn_ac() )
				{
					sprintf(acErrMsg,"协定户开户错误!");
					WRITEMSG
					return 1;
				}
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}

				g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
				g_pub_tx.ac_seqn=1;
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
				strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				g_pub_tx.svc_ind=1001; /**存取款***/
				g_pub_tx.tx_amt1=g_pub_intst.factval+g_pub_intst.keepval;
				strcpy(g_pub_tx.ac_id_type,"");
				if( pub_acct_trance() )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					return 1;
				}	
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}
				/*** 自动更新协定结算账号 ***/
				unchain_flag=2;
				break;
		case 'F':
				g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
				g_pub_tx.ac_seqn=1;
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
				strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
				g_pub_tx.svc_ind=1001; /**存取款***/
				strcpy(g_pub_tx.ac_id_type,"");
				if( pub_acct_trance() )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					return 1;
				}	
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}
				break;
		case 'J':
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",
					mo_prdt_prot_c.main_ac_id);
				if( ret )
				{
					sprintf(acErrMsg,"取显示账号错误![%ld][%d]",
						mo_prdt_prot_c.main_ac_id,ret);
					WRITEMSG
					return 1;
				}
				strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
				strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);
				strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
				strcpy(g_pub_tx.crd_no,"");
				strcpy(g_pub_tx.ac_no,"");
				strcpy(g_pub_tx.cur_no,RMB);
				strcpy(g_pub_tx.add_ind,"1");
				g_pub_tx.ac_id=0;
				g_pub_tx.ac_seqn=0;
				strcpy(g_pub_tx.brf,"公积金开户");
				Tamt=g_pub_tx.tx_amt1;
				g_pub_tx.tx_amt1=0.00;
				g_reply_int=pub_acct_opn_cif_mdm();
				if( g_reply_int )
				{
					sprintf(acErrMsg,"开介质错误!");
					WRITEMSG
					return 1;
				}

				strcpy(g_pub_tx.hst_ind,"1");
				strcpy(g_pub_tx.ac_type,"6");
				g_pub_tx.svc_ind=1000;
				g_pub_tx.tx_amt1=Tamt;
				strcpy(g_pub_tx.prdt_code,"20C");
				if( pub_acct_opn_ac() )
				{
					sprintf(acErrMsg,"协定户开户错误!");
					WRITEMSG
					return 1;
				}
				set_zd_data("102K",g_pub_tx.ct_ind);
				/****改用100域第2子域  modify at 20130517
				set_zd_double("102I",g_pub_tx.tx_amt1);
				****/
				set_zd_double("1002",g_pub_tx.tx_amt1);
				g_reply_int=pubf_acct_proc("LC02");
				if( g_reply_int )
				{
					sprintf(acErrMsg,"登记会计流水错误!");
					WRITEMSG
					return 1;
				}

				if( pub_eod_ins_prot_ac_rel(mo_prdt_prot_c) )
				{
					sprintf(acErrMsg,"登记协定账号对照表错误!");
					WRITEMSG
					return 1;
				}
				break;
		case 'G':
				break;
		case 'H':
				break;
		default :
				sprintf(acErrMsg,"无此化转函数!%s]",prdt_prot_parm_c.deal_func);
				WRITEMSG
				return 1;
	}

	if( pub_eod_prot_ac_rel(mo_prot_ac_rel_c,unchain_flag) )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}	
	
    return 0;
}

int pub_eod_prot_ac_rel(struct mo_prot_ac_rel_c mo_prot_ac_rel_c,int unchain_flag)
{
	int ret=0;

	if( unchain_flag==1 )
	{
		ret=sql_execute("update mo_prdt_prot set sts='*' where main_ac_id=%ld  and main_ac_seqn=%d",mo_prot_ac_rel_c.main_ac_id,mo_prot_ac_rel_c.main_ac_seqn); 
		if( ret )
		{
			sprintf(acErrMsg,"更新划款协议登记簿错误![%d]",ret);
			WRITEMSG
			return 1;
		}

		ret=sql_execute("update mo_prot_ac_rel set sts='*' where main_ac_id=%ld  and main_ac_seqn=%d",mo_prot_ac_rel_c.main_ac_id,mo_prot_ac_rel_c.main_ac_seqn); 
		if( ret )
		{
			sprintf(acErrMsg,"更新划款协议登记簿错误![%d]",ret);
			WRITEMSG
			return 1;
		}
		
	}
	else if( unchain_flag==2 )
	{
		ret=sql_execute("update mo_prdt_prot set main_ac_id=%ld,main_ac_seqn=%d where main_ac_id=%ld  and main_ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn,mo_prot_ac_rel_c.main_ac_id,mo_prot_ac_rel_c.main_ac_seqn); 
		if( ret )
		{
			sprintf(acErrMsg,"更新划款协议登记簿错误![%d]",ret);
			WRITEMSG
			return 1;
		}

		ret=sql_execute("update mo_prot_ac_rel set main_ac_id=%ld,main_ac_seqn=%d where main_ac_id=%ld  and main_ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn,mo_prot_ac_rel_c.main_ac_id,mo_prot_ac_rel_c.main_ac_seqn); 
		if( ret )
		{
			sprintf(acErrMsg,"更新划款协议登记簿错误![%d]",ret);
			WRITEMSG
			return 1;
		}
		
	}

	return 0;
}
int pub_eod_ins_prot_ac_rel(struct mo_prdt_prot_c mo_prdt_prot_c)
{
	int	ret=0;
	struct mo_prot_ac_rel_c	mo_prot_ac_rel_c;
	memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));

	strcpy(mo_prot_ac_rel_c.tx_br_no,mo_prdt_prot_c.tx_br_no);
	strcpy(mo_prot_ac_rel_c.prot_type,mo_prdt_prot_c.prot_type);
	mo_prot_ac_rel_c.main_ac_id=mo_prdt_prot_c.main_ac_id;
	mo_prot_ac_rel_c.main_ac_seqn=mo_prdt_prot_c.main_ac_seqn;
	mo_prot_ac_rel_c.app_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.app_ac_seqn=g_pub_tx.ac_seqn;
	mo_prot_ac_rel_c.tx_date=g_pub_tx.tx_date;
	mo_prot_ac_rel_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prot_ac_rel_c.tel,"000");
	strcpy(mo_prot_ac_rel_c.sts,"1");

	ret=Mo_prot_ac_rel_Ins(mo_prot_ac_rel_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记协议账号对照表错误![%d]",ret);
		WRITEMSG
		return 1;
	}
	return 0;
}
int pub_eod_Intst_tza_mst( char *prot_type )
{
	int ret;
	struct td_mst_c TmpTd_mst;
	struct mo_infrm_c mo_infrm_c;
	struct prot_hst_c vprot_hst;

	memset(&TmpTd_mst, 0x0, sizeof(struct td_mst_c));
	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
	memset(&vprot_hst,0x00,sizeof(vprot_hst));

	vprot_hst.tx_date=g_pub_tx.tx_date;
	vprot_hst.ac_id=g_pub_tx.ac_id;
	vprot_hst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy( vprot_hst.prot_type,prot_type );
	vprot_hst.amt=0.00;
	vprot_hst.amt2=0.00;
	vprot_hst.amt3=0.00;

vtcp_log("TZA A ac_id=%ld and ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and \
		ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期主文件错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
		if( g_td_mst.ac_sts[0]!='1' )
		{
			strcpy( vprot_hst.reply,"P109" );
			TRACE
			goto OkExit;
		}
		if( g_td_mst.hold_sts[0]!='0' )
		{
			strcpy( vprot_hst.reply,"O084" );
			TRACE
			goto OkExit;
		}
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
		"ac_id=%ld and ac_seqn=%d and note_sts='0'", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if( ret==100 )
	{
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%ld and ac_seqn=%d and note_sts='0'", g_pub_tx.ac_id, 9999 );
	}
	if( ret==100 )
	{
		TRACE
		strcpy( vprot_hst.reply,"P109" );
		goto OkExit;
	}
	else if( ret ) goto ErrExit;
	else 
	{
		if( g_mdm_ac_rel.note_sts[0]!='0' )
		{
			TRACE
			strcpy( vprot_hst.reply,"P109" );
			goto OkExit;
		}
	}
TRACE
	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
		g_td_mst.prdt_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
TRACE

	strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
	strcpy(g_pub_tx.prdt_code,g_td_parm.prdt_no);
	strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);

	strcpy(g_pub_tx.name,g_td_mst.name);
	g_pub_tx.cif_no=g_td_mst.cif_no;
	g_pub_tx.ac_id_type[0]='2';

	g_pub_tx.tx_amt1=g_td_mst.bal;

	g_pub_tx.mach_date = g_td_mst.mtr_date;
	sprintf(acErrMsg, "CGH******教育证明为:[%s]", g_pub_intst.edu_ind);
	WRITEMSG

	/*** 调用计算利息积数的函数 **** 定额续取 定额续存 教育储蓄*/ 
	if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' || 
		g_td_parm.td_type[0] == '7' ) 
	{ 
		if( pub_base_CalAcm(g_td_mst.lst_date, g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date, 
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode, 
			g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm, 
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) ) 
		{ 
			sprintf(acErrMsg,"计算利息积数错误!"); 
			WRITEMSG 
			goto ErrExit; 
		} 
		 g_td_mst.intst_acm+=g_td_mst.bal;
	}
	sprintf(acErrMsg, "CGH *********积数为:[%lf]", g_td_mst.intst_acm);
	WRITEMSG

	/***通知存款 预约下次支取***/
	if( g_td_parm.td_type[0]=='4' )
	{
		memset( &mo_infrm_c,0,sizeof(mo_infrm_c) );
		mo_infrm_c.ac_id=g_td_mst.ac_id;
		mo_infrm_c.ac_seqn=g_td_mst.ac_seqn;
		mo_infrm_c.infrm_date=g_pub_tx.tx_date;
		mo_infrm_c.prm_draw_date=g_pub_tx.tx_date;
		pub_base_deadlineD( mo_infrm_c.prm_draw_date,-7,&mo_infrm_c.infrm_date);
		mo_infrm_c.prm_draw_amt=g_td_mst.bal;
		mo_infrm_c.sts[0]='0';
		strcpy( mo_infrm_c.name,"通知套餐 A自动预约" );
		mo_infrm_c.wrk_date=g_pub_tx.tx_date;
		mo_infrm_c.trace_no=g_pub_tx.trace_no;
		ret=Mo_infrm_Ins( mo_infrm_c,g_pub_tx.reply );
		if( ret ) goto ErrExit;
	}

	/* 调用计息函数 */ 
	g_pub_intst.mode = 1;  /* 销户计息 */ 
	sprintf(acErrMsg,"CGH****教育证明[%s]",g_pub_intst.edu_ind);
	WRITEMSG
	pub_base_deadlineD(g_td_mst.ic_date,-1,&TmpTd_mst.ic_date);
	g_td_mst.ic_date=TmpTd_mst.ic_date;
	ret = pub_base_CalIntst(&g_pub_intst); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"计算利息错误!!"); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"A032"); 
		goto ErrExit; 
	}
	sprintf(acErrMsg,"教育证明[%s]",g_pub_intst.edu_ind);
	WRITEMSG
	sprintf(acErrMsg,"应付利息[%.2lf] 应税利息[%.2lf] 保值利息[%.2lf]",
	g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
	WRITEMSG
	sprintf(acErrMsg,"实付利息[%.2lf] 利息税[%.2lf] ",
	g_pub_intst.factval, g_pub_intst.val );
	WRITEMSG
	sprintf(acErrMsg, "CGH ---------->:[%lf]", g_pub_intst.tax_intst);
	WRITEMSG

	ret = Td_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and \
		ac_seqn = %d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if (ret != 0)
	{
		sprintf(acErrMsg, "Declare for select error !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = Td_mst_Fet_Upd(&TmpTd_mst, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "Fetch for select error !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	/**strcpy(TmpTd_mst.ac_sts, "#");**/
	TmpTd_mst.intst_acm = 0;
	pub_base_deadlineD(g_pub_tx.tx_date,1,&TmpTd_mst.ic_date);
	TmpTd_mst.lst_date = g_pub_tx.tx_date;

	ret = Td_mst_Upd_Upd(TmpTd_mst, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "UPDATE error !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	Td_mst_Clo_Upd();
	/*******************入账********************/
	g_pub_tx.tx_amt1 = g_pub_intst.dealval + g_pub_intst.keepval;
	vprot_hst.amt=g_pub_tx.tx_amt1;
	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
	{
		strcpy(g_pub_tx.ac_wrk_ind, "000000");
	}
	strcpy(g_pub_tx.brf, "理财息");
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ct_ind, "2");
	g_pub_tx.svc_ind=1012; /**存取款***/
	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "CGH ********* [%s]", g_pub_tx.reply);
	WRITEMSG

	g_pub_tx.tx_amt1 = g_pub_intst.val;
	vprot_hst.amt2=-g_pub_tx.tx_amt1;
	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
	{
		strcpy(g_pub_tx.ac_wrk_ind, "000000");
	}
	strcpy(g_pub_tx.brf, "理财税");
	strcpy(g_pub_tx.add_ind,"0");
	strcpy(g_pub_tx.ct_ind, "2");
	g_pub_tx.svc_ind=1012; /**存取款***/
	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	/********入会计帐***********/
    set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);	/*实付利息*/
    set_zd_double("102Q",g_pub_intst.val);							/*利 息 税*/
    set_zd_double("102R",g_pub_intst.keepval);						/*保值利息*/
    set_zd_double("102S",
    g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
																	/*应付总额*/
	g_reply_int=pubf_acct_proc("D202");
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}
    set_zd_double("102P",0.00);	/*实付利息*/
    set_zd_double("102Q",0.00);	/*利 息 税*/
    set_zd_double("102R",0.00); /*保值利息*/
    set_zd_double("102S",0.00); /*应付总额*/

OkExit:
	ret=Prot_hst_Ins( vprot_hst,g_pub_tx.reply );
	if( ret ) goto ErrExit;
	return 0;
ErrExit:
	return 1;
}
int pub_eod_Intst_tzb_mst( char *tdprdt,double acm,char *prot_type )
{
	int ret;
	struct mo_infrm_c mo_infrm_c;
	struct prot_hst_c vprot_hst;
	double hq_rate_val;
	double dq_rate_val;
	double sdsl;
	double lx=0.00;

	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
	memset(&vprot_hst,0x00,sizeof(vprot_hst));

vtcp_log("TCB acm[%.2lf]",acm);
	vprot_hst.tx_date=g_pub_tx.tx_date;
	vprot_hst.ac_id=g_pub_tx.ac_id;
	vprot_hst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy( vprot_hst.prot_type,prot_type );
	vprot_hst.amt=0.00;
	vprot_hst.amt2=0.00;
	vprot_hst.amt3=0.00;

	ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and \
		ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询dd_mst错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
		if( g_dd_mst.ac_sts[0]!='1' )
		{
			strcpy( vprot_hst.reply,"P109" );
			TRACE
			goto OkExit;
		}
		if( g_dd_mst.hold_sts[0]!='0' )
		{
			strcpy( vprot_hst.reply,"O084" );
			TRACE
			goto OkExit;
		}

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
		"ac_id=%ld and ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if( ret==100 )
	{
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%ld and ac_seqn=%d", g_pub_tx.ac_id, 9999 );
	}
	if( ret==100 )
		;
	else if( ret ) goto ErrExit;
	else 
	{
		if( g_mdm_ac_rel.note_sts[0]!='0' )
		{
			strcpy( vprot_hst.reply,"P109" );
			goto OkExit;
		}
	}
		

	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
		tdprdt);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
		g_dd_mst.prdt_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
	strcpy(g_pub_tx.prdt_code,g_dd_parm.prdt_no);
	strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

	strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;
	g_pub_tx.ac_id_type[0]='1';

	ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,g_pub_tx.tx_date,
		&dq_rate_val);
	if (ret)
	{
		sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W110");
		return ret;
	}
	ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,g_pub_tx.tx_date,
		&hq_rate_val);
	if (ret)
	{
		sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W110");
		return ret;
	}

	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
	if (ret != 0 && ret == 1)
	{
		sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W100");
		return 1;
	}
	else if (ret < 0)
	{
		sprintf(acErrMsg,"取利息税税率错误 %d",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W101");
		return 1;
	}

	if( dq_rate_val>hq_rate_val )
	{
		lx=acm*(dq_rate_val-hq_rate_val)/L360/L100;
	}
	else
	{
		lx=0.00;
	}
	g_pub_intst.dealval=pub_base_PubDround_1(lx);
	g_pub_intst.tax_intst=g_pub_intst.dealval;
	g_pub_intst.keepval=0.00;
	g_pub_intst.val=pub_base_PubDround_1(g_pub_intst.dealval*sdsl);
	g_pub_intst.factval=g_pub_intst.dealval-g_pub_intst.val;

	sprintf(acErrMsg,"应付利息[%.2lf] 应税利息[%.2lf] 保值利息[%.2lf]",
		g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
	WRITEMSG
	sprintf(acErrMsg,"实付利息[%.2lf] 利息税[%.2lf] ",
	g_pub_intst.factval, g_pub_intst.val );
	WRITEMSG
	sprintf(acErrMsg, "CGH ---------->:[%lf]", g_pub_intst.tax_intst);
	WRITEMSG
	/*******************入账********************/
	g_pub_tx.tx_amt1 = g_pub_intst.dealval + g_pub_intst.keepval;
	vprot_hst.amt=g_pub_tx.tx_amt1;
	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
	{
		strcpy(g_pub_tx.ac_wrk_ind, "000000");
	}
	strcpy(g_pub_tx.brf, "理财息");
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ct_ind, "2");
	g_pub_tx.svc_ind=1012; /**存取款***/
	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "CGH ********* [%s]", g_pub_tx.reply);
	WRITEMSG

	g_pub_tx.tx_amt1 = g_pub_intst.val;
	vprot_hst.amt2=-g_pub_tx.tx_amt1;
	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
	{
		strcpy(g_pub_tx.ac_wrk_ind, "000000");
	}
	strcpy(g_pub_tx.brf, "理财税");
	strcpy(g_pub_tx.add_ind,"0");
	strcpy(g_pub_tx.ct_ind, "2");
	g_pub_tx.svc_ind=1012; /**存取款***/
	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	/********入会计帐***********/
    set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);	/*实付利息*/
    set_zd_double("102Q",g_pub_intst.val);							/*利 息 税*/
    set_zd_double("102R",g_pub_intst.keepval);						/*保值利息*/
    set_zd_double("102S",
    g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
																	/*应付总额*/
	g_reply_int=pubf_acct_proc("D202");
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}
    set_zd_double("102P",0.00);	/*实付利息*/
    set_zd_double("102Q",0.00);	/*利 息 税*/
    set_zd_double("102R",0.00); /*保值利息*/
    set_zd_double("102S",0.00); /*应付总额*/

OkExit:
	ret=Prot_hst_Ins( vprot_hst,g_pub_tx.reply );
	if( ret ) goto ErrExit;
	return 0;
ErrExit:
	return 1;
}
/*********add by ligl 2009-4-15 17:23 增加划回*******/
int pub_eod_prot1(struct mo_prdt_prot_c mo_prdt_prot_c,double amt)
{
	int ret=0;
	struct dd_mst_c g_dd_mst_tmp;
	struct mo_prot_ac_rel_c mo_prot_ac_rel_tmp;
	memset(&g_dd_mst_tmp,0x00,sizeof(struct dd_mst_c));
	memset(&mo_prot_ac_rel_tmp,0x00,sizeof(struct mo_prot_ac_rel_c));
	
	vtcp_log("%s,%dAAAAAAAAAAAAAAAA amt=%.2lf,\n",__FILE__,__LINE__,amt);
	ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&mo_prot_ac_rel_tmp,"main_ac_id=%d and main_ac_seqn=%d and sts='1' ",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
  if( ret )
	{
		sprintf(acErrMsg,"取从账号错误![%d]",ret);
		WRITEMSG
		return 1;
	}
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst_tmp,"ac_id=%d and ac_seqn=%d",mo_prot_ac_rel_tmp.app_ac_id,mo_prot_ac_rel_tmp.app_ac_seqn);
	if( ret )
	{
				sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
				mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
				WRITEMSG
				return 1;
	}
	vtcp_log("%s,%dAAAAAAAAAAAAAAAA amt=%.2lf,g_dd_mst_tmp.bal=%.2lf\n",__FILE__,__LINE__,amt,g_dd_mst_tmp.bal);
	/** 按财务部要求 如果从账户余额不够划回金额，就有多少划多少 20131118 wudawei **/
	/** if(pub_base_CompDblVal(g_dd_mst_tmp.bal-amt,0.00)>=0)
	{
		g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
		g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
		vtcp_log("%s,%dAAAAAAAAAAAAAAAA amt=%.2lf,g_dd_mst_tmp.bal=%.2lf\n",__FILE__,__LINE__,amt,g_dd_mst_tmp.bal);
		if ( pub_sub_prot_x(amt,mo_prdt_prot_c) )
		{
			sprintf(acErrMsg,"协定划款化回错误!");
			WRITEMSG
			return 1;
		}
	}**/
	/**mod end wudawei 20131118**/
	if(g_dd_mst_tmp.bal > 0)
	{
		/**划回**/
		g_pub_tx.ac_id=mo_prdt_prot_c.main_ac_id;
		g_pub_tx.ac_seqn=mo_prdt_prot_c.main_ac_seqn;
		vtcp_log("%s,%dAAAAAAAAAAAAAAAA amt=%.2lf,g_dd_mst_tmp.bal=%.2lf\n",__FILE__,__LINE__,amt,g_dd_mst_tmp.bal);
		if(pub_base_CompDblVal(g_dd_mst_tmp.bal-amt,0.00)>=0)
		{
			if ( pub_sub_prot_x(amt,mo_prdt_prot_c) )
			{
				sprintf(acErrMsg,"协定划款化回错误!");
				WRITEMSG
				return 1;
			}
		}else
		{
			if ( pub_sub_prot_x(g_dd_mst_tmp.bal,mo_prdt_prot_c) )
			{
				sprintf(acErrMsg,"协定划款化回错误!");
				WRITEMSG
				return 1;
			}
		}
	}
	else
	{
		/**不处理***/
		return 1;
	}
	return 0;
}
/****end****/
