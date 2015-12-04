/*************************************************************
* 文 件 名: gD085.c
* 功能描述：日终－贷款还款计划表每日检查
*
* 作    者: lance
* 完成日期: 2003年3月31日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"

gD085()
{
	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0,pay_flag=0;
	int    ac_seqn=0,parm_seqn=0,t_cnt=0;

	double amt_reg=0.00,amt=0.00,pun_intst=0.00,t_intst=0.00,t_pun_intst=0.00;
	char ln_ac_no[20],dd_ac_no[20],flag[2],parm_code[6];  
	long day_tmp=0,be_date=0;
	double now_ln_bal=0.00;
	double lo_tol_amt=0.00;
	double lo_pay_amt=0.00;
	long ttl_cnt=0;
	long curr_cnt=0;
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;
	struct ln_lo_c ln_lo; 
	struct ln_lo_c ln_lo_tmp;       
	struct ln_pay_pln_c ln_pay_pln; 
	struct ln_pay_pln_c t_ln_pay_pln;   
	struct ln_pay_pln_c ln_pay_pln_curr;  
	struct ln_pay_pln_c ln_pay_pln_next;   
	struct ln_pay_pln_c ln_pay_pln_temp;   
	int first=0;

	memset( vln_intst,0,sizeof(vln_intst) );    
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
	memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 
	memset(&ln_lo_tmp, 0x00, sizeof(struct ln_lo_c));    
	memset(&ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&t_ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_curr, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_next, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_temp, 0x00, sizeof(struct ln_pay_pln_c)); 

	ret=sql_begin(); /*打开事务*/
	if (ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
			goto ErrExit;
	}
	TRACE

	/* 初始化全局变量 */
		pub_base_sysinit();

	show_log( " BEGIN [ pay_ind='0' and beg_date<=%ld and end_date >=%ld ]",
		g_pub_tx.tx_date,g_pub_tx.tx_date);

	/* 将当前还款计划导入临时表中 */
	ret=sql_execute("delete from ln_pay_pln_tmp");
	if ( ret) goto ErrExit;

	ret=sql_execute("insert into ln_pay_pln_tmp select * from ln_pay_pln");
	if ( ret) goto ErrExit;

	/* 读贷款还款计划表(处于还款周期内且状态为未还清) */    
	ret = Ln_pay_pln_Dec_Upd1(g_pub_tx.reply,
		" pay_ind='0' and beg_date<=%ld and end_date >=%ld ",
	/*rem by LiuHuafeng replace by next line 20070120
		" pay_ind='0' and beg_date<=%ld and end_date >=%ld and ac_id in (select ac_id from ln_mst where ac_sts<>'*') ",
	****************************************/
		g_pub_tx.tx_date,g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	TRACE

	while (1)
	{
		/*数据初始化*/
		ret=0;bj_flag=0;chg_flag=0;i=0;dc_flag=0;pay_flag=0;
		ac_seqn=0;parm_seqn=0;t_cnt=0;

		amt_reg=0.00;amt=0.00;pun_intst=0.00;t_intst=0.00;t_pun_intst=0.00;

		now_ln_bal=0.00;
		lo_tol_amt=0.00;
		lo_pay_amt=0.00;
		ttl_cnt=0;
		curr_cnt=0;
		memset( vln_intst,0,sizeof(vln_intst) );    
		strcpy( g_pub_tx.tx_code, "G085");

		ret = Ln_pay_pln_Fet_Upd1(&ln_pay_pln, g_pub_tx.reply);
		if (ret && ret!=100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
				goto ErrExit;
			}else if (ret==100)
			{
				vtcp_log("无相应记录[%d]ret=[%d]",g_pub_tx.tx_date,ret);
				break;
			}   
			first++;
			show_log( "gD085[%d]-[%d]",first,ln_pay_pln.ac_id );

			/* 取流水号 */
			if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
			{
				sprintf(acErrMsg,"Get trace_no error [%d]",g_pub_tx.trace_no);
				WRITEMSG
					goto ErrExit;
			}
			set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
			set_zd_long( DC_TRACE_CNT,1 );

			/* 取贷款信息账号 */
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
			if (ret)
			{
				sprintf(acErrMsg,"读取贷款户主文件失败[%d-%d]",
					ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
				WRITEMSG
					goto ErrExit;   
			}
			ret= Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'",
				g_ln_mst.prdt_no);
			if ( ret )
			{
				sprintf(acErrMsg,"读取贷款参数表信息失败[%s]",g_ln_mst.prdt_no);
				WRITEMSG
					goto ErrExit;   
			}  
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_id=%ld and ac_seqn=%d",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
			if (ret)
			{
				sprintf(acErrMsg,"读取介质账号对照表失败[%d-%d]",
					ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
				WRITEMSG
					goto ErrExit;
			}
			strcpy( ln_ac_no,g_mdm_ac_rel.ac_no );

			sprintf(acErrMsg,"SS [%ld][%d][%s]",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn,ln_ac_no);
			WRITEMSG

			/*由于数据不准确,重新计算还款计划*/
			/*取欠款表数据*/
				ret= sql_sum_double("ln_lo", "lo_amt", &lo_tol_amt, 
				"ac_id=%ld and ac_seqn=%d and pay_type='0'",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if (ret)	goto ErrExit;

			ret= sql_sum_double ("ln_lo", "pay_lo_amt", &lo_pay_amt,
				"ac_id=%ld and ac_seqn=%d and pay_type='0'",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if (ret)	goto ErrExit;	

			now_ln_bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);

			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*总期数*/

			curr_cnt= ln_pay_pln.curr_cnt;		/*当前期数*/

			ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
				now_ln_bal, &ln_pay_pln_curr );
			if (ret)
			{
				sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
				WRITEMSG
					goto ErrExit;
			}
			sprintf(acErrMsg,"重新生成的还款计划!当前[%d]期",ln_pay_pln.curr_cnt);
			WRITEMSG

				sprintf(acErrMsg,"账号序号[%d]",ln_pay_pln_curr.ac_seqn);
			WRITEMSG
				sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln_curr.curr_cnt);
			WRITEMSG
				sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln_curr.wrk_date);
			WRITEMSG
				sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln_curr.pay_ind);
			WRITEMSG

				sprintf(acErrMsg,"总期数[%d]",ln_pay_pln_curr.ttl_cnt);
			WRITEMSG
				sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln_curr.curr_amt);
			WRITEMSG
				sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln_curr.curr_intst);
			WRITEMSG
				sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln_curr.beg_date);
			WRITEMSG
				sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln_curr.end_date);
			WRITEMSG

				sprintf(acErrMsg,"原还款计划!!!!!!!!!!!!!!!!!!!");
			sprintf(acErrMsg,"账号序号[%d]",ln_pay_pln.ac_seqn);
			WRITEMSG
				sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln.curr_cnt);
			WRITEMSG
				sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln.wrk_date);
			WRITEMSG
				sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln.pay_ind);
			WRITEMSG

				sprintf(acErrMsg,"总期数[%d]",ln_pay_pln.ttl_cnt);
			WRITEMSG
				sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln.curr_amt);
			WRITEMSG
				sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln.curr_intst);
			WRITEMSG
				sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln.beg_date);
			WRITEMSG
				sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln.end_date);
			WRITEMSG

      Ln_pay_pln_Debug(&ln_pay_pln_curr);
			/*替换原有还款计划*/
			memcpy(&ln_pay_pln, &ln_pay_pln_curr, sizeof( struct ln_pay_pln_c ));
      Ln_pay_pln_Debug(&ln_pay_pln);
			/*交易机构赋值*/
			strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

			/* 是否自动还款 */
			if ( g_ln_parm.auto_pay_ind[0]!='Y')
			{
				goto NoPay;
			}
			/**xxx*6lines*/
			if ( g_ln_mst.repay_ac_id<=0 )
			{
				sprintf(acErrMsg,"无归还账号[%ld]",g_ln_mst.repay_ac_id);
				WRITEMSG
					goto NoPay;
			}
			if ( g_dd_parm.draw_ind[0]=='N' )
			{
				sprintf(acErrMsg,"不可续取[%s]!",g_dd_parm.draw_ind);
				WRITEMSG
					goto NoPay;
			}
			/* 检查活期账号状态 */
			/* 根据币种生成活期户的账号ID */
			ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
			if ( ret )
			{
				sprintf(acErrMsg,"根据币种生成账户序号错误[%s]!",g_ln_parm.cur_no);
				WRITEMSG
					goto ErrExit;	
			}
			ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst );
			if ( ret )
			{
				sprintf(acErrMsg,"账号不存在[%ld|%d]",g_ln_mst.repay_ac_id,ac_seqn);
				WRITEMSG
					goto NoPay;
			}
			sprintf(acErrMsg,"账号[%ld]",g_dd_mst.ac_id);
			WRITEMSG
			/* 活期显示账号 */
				ret = pub_base_acid_acno( g_dd_mst.ac_id,ac_seqn,&dd_ac_no );	
			if( ret ) {	goto ErrExit; }

			sprintf(acErrMsg,"账号[%ld|%d][%s]",g_dd_mst.ac_id,ac_seqn,dd_ac_no);
			WRITEMSG

				ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
				g_dd_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"不存在该产品编号!prdt_no=[%s]",g_dd_mst.prdt_no);
				WRITEMSG
					strcpy(g_pub_tx.reply,"P145");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
				WRITEMSG
					goto ErrExit;
			}
			if ( g_dd_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"账号状态非正常状态!");
				WRITEMSG
					strcpy(g_pub_tx.reply,"P109");
				goto NoPay;
			}
			/* 金额检查 */
			g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;

			ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00);
			if ( ret<=0 )
			{	
				sprintf(acErrMsg,"无足够金额[%.2lf]",g_dd_mst.bal);
				WRITEMSG
					goto NoPay;
			}
			/* 考虑冻结状态 */
			if ( g_dd_mst.hold_sts[0]!='0' )
			{
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

				/**完全冻结或只进不出视可用为0.00**/
				if ( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
					g_pub_tx.tmp_amt1=0.00;
				else
					/*g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt; 这一段代码中，g_pub_tx.tmp_amt1 变量重复减了g_dd_mst.hold_amt一次，正确的应该删除 20140428_wjl*/	

				ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1 , 0.00);
				if ( ret<=0 )
				{
					sprintf(acErrMsg,"无足够金额[%s][%.2lf]",
						g_dd_mst.hold_sts,g_pub_tx.tmp_amt1);
					WRITEMSG
						goto NoPay;
				}
			}
			TRACE
			/* 更新贷款主文件 */
				ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1, 
				ln_pay_pln.curr_amt+ln_pay_pln.curr_intst );
			if ( ret<0 )
			{
				/*金额不够还*/
				/*不是到期日*/
				if (g_pub_tx.tx_date < ln_pay_pln.end_date)
				{
					goto NoPay;
				}
				else
				{
					/* 到期日 */
					/*调用还款计划函数确定各项欠息的还款金额*/
					i=0;
					vln_intst[i].intst_amt=ln_pay_pln.curr_amt; 
					vln_intst[i].type=0;
					i++;
					vln_intst[i].intst_amt=ln_pay_pln.curr_intst; 
					vln_intst[i].type=1;
					i++;
					ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
						&g_pub_tx.tmp_amt1 );
					if ( ret ) goto ErrExit;
					g_pub_tx.tx_amt1=vln_intst[0].pay_amt;
					g_pub_tx.tx_amt2=vln_intst[1].pay_amt;
					/****
					ln_pay_pln.curr_amt-=g_pub_tx.tx_amt1;
					ln_pay_pln.curr_intst-=g_pub_tx.tx_amt2;
					****/

					sprintf(acErrMsg,"总欠款金额:本金=[%lf],利息=[%lf] ",
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst);
					WRITEMSG
						sprintf(acErrMsg,"可归还金额:本金tx_amt1=[%lf],利息tx_amt2=[%lf] ",
						g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
					WRITEMSG

					/* 登记按揭欠款表 */
						ret = pub_ln_Ins_ln_lo( g_ln_mst , ln_pay_pln , "0" ,
						ln_pay_pln.curr_amt,ln_pay_pln.curr_intst,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2 ); 	
					if (ret)
					{
						sprintf(acErrMsg,"登记贷款欠款表失败");
						WRITEMSG
							strcpy(g_pub_tx.reply,"L083");
						goto ErrExit;
					}
					/* 判断是否是按揭 */      
					if (g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
					{
						/* 是否是最后一期 */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							/* 生成新的还款计划 */    
							now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*总期数*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*当前期数*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
								now_ln_bal, &ln_pay_pln_next );
							if (ret)
							{
								sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
								WRITEMSG
									goto ErrExit;
							}
							sprintf(acErrMsg,"生成的下一期还款计划!下一期第[%d]期",ln_pay_pln.curr_cnt);
							WRITEMSG

								sprintf(acErrMsg,"账号序号[%d]",ln_pay_pln_next.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln_next.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln_next.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln_next.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"总期数[%d]",ln_pay_pln_next.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln_next.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln_next.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln_next.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln_next.end_date);
							WRITEMSG

								sprintf(acErrMsg,"当前期次还款计划!!!!!!!!!!!!!!!!!!!");
							sprintf(acErrMsg,"账号序号[%d]",ln_pay_pln.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"总期数[%d]",ln_pay_pln.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln.end_date);
							WRITEMSG															

							/* 删除此条记录 *
							ret = sql_execute("delete from ln_pay_pln where \
							ac_id=%ld and ac_seqn=%d",\
							ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if(ret)
							{
							sprintf(acErrMsg,"登记还款计划表失败");
							WRITEMSG
							strcpy(g_pub_tx.reply,"L077");
							goto ErrExit;
							}
							***********************/
								vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* 添加还款计划表 *
							ret = Ln_pay_pln_Ins( ln_pay_pln_next , g_pub_tx.reply );
							if(ret)
							{
							sprintf(acErrMsg,"登记还款计划表失败");
							WRITEMSG
							strcpy(g_pub_tx.reply,"L077");
							goto ErrExit;
							}
							*************************/
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln_next.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_next.rowid)-1);
							 
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln_next, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							}else
							{
								strcpy( ln_pay_pln.pay_ind , "1" );    /* 已还 */
								
								/**add by xyy 2006-12-1 22:28**/
							  memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							  ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							  if ( ret )
							  {
							  	sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
							  	WRITEMSG
							  		goto ErrExit;
							  }
							  else
							    memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
								
								vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
								
								ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
								if ( ret )
								{
									sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
									WRITEMSG
										goto ErrExit;
								}
							}
						}
						else	/**hao @20050817@ 已经登记欠款表了，还款计划表已还 **/
						{
							strcpy( ln_pay_pln.pay_ind , "1" );    /* 已还 */
							
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							  
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}	
						}
					}
				}
				else
				{
					/* 金额够还本期 */
					g_pub_tx.tx_amt1 = ln_pay_pln.curr_amt;
					g_pub_tx.tx_amt2 = ln_pay_pln.curr_intst;           
					g_pub_tx.tmp_amt1=ln_pay_pln.curr_amt+ln_pay_pln.curr_intst;
					strcpy( ln_pay_pln.pay_ind , "1" );    /* 已还 */
					ln_pay_pln.de_ac_id = g_dd_mst.ac_id;
					ln_pay_pln.de_ac_seqn = ac_seqn;

					sprintf( acErrMsg,"贷款类型[%s]",g_ln_parm.ln_pay_type );
					WRITEMSG

					/* 若非按揭贷款 */
					if (g_ln_parm.ln_pay_type[0]!='3'&&g_ln_parm.ln_pay_type[0]!='4') 
					{
						if ( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)<0 )
						{
							ln_pay_pln.curr_amt = g_ln_mst.bal ;
						}
					}
					/* 判断是否是按揭，若按揭贷款 */      
					if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
					{
						/* 是否是最后一期 */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							/* 生成新的还款计划 */                        		
							now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*总期数*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*当前期数*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
								now_ln_bal, &ln_pay_pln_next );
							Ln_pay_pln_Debug(&ln_pay_pln_next);
							if (ret)
							{
								sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
								WRITEMSG
									goto ErrExit;
							}
							Ln_pay_pln_Debug(&ln_pay_pln_next);
							sprintf(acErrMsg,"生成的下一期还款计划!下一期第[%d]期",ln_pay_pln.curr_cnt);
							WRITEMSG

								vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* 更新本期还款计划为已还 */
							
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln_next.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_next.rowid)-1);
							  
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);  
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln_next, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							Ln_pay_pln_Debug(&ln_pay_pln_next);
						}
						else
						{
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							vtcp_log("[%s][%d] ln_pay_pln.ac_id==[%ld]  ln_pay_pln.ac_seqn=[%ld]",__FILE__,__LINE__,ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							
							/* 更新本期还款计划为已还 */
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;	
							}	
						}
					}
					else
					{
						/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
						
						vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
						
						/* 更新本期还款计划为已还 */
						ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
						if ( ret )
						{
							sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
							WRITEMSG
								goto ErrExit;	
						}	
					}
				}
				goto YOUPay;
				NoPay:
					g_pub_tx.tmp_amt1=0.00;
				/*金额不够还*/
				if (g_pub_tx.tx_date < ln_pay_pln.end_date)
				{
					continue;
				}
				else
				{
					/* 到期日 */
					TRACE
					/*调用还款计划函数确定各项欠息的还款金额*/
						i=0;
					vln_intst[i].intst_amt=ln_pay_pln.curr_amt; 
					vln_intst[i].type=0;
					i++;
					vln_intst[i].intst_amt=ln_pay_pln.curr_intst; 
					vln_intst[i].type=1;
					i++;
					ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
						&g_pub_tx.tmp_amt1 );
					if ( ret ) goto ErrExit;
					g_pub_tx.tx_amt1=vln_intst[0].pay_amt;
					g_pub_tx.tx_amt2=vln_intst[1].pay_amt;

					sprintf(acErrMsg,"总欠款金额:本金=[%lf],利息=[%lf] ",
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst);
					WRITEMSG
					/* 登记按揭欠款表 */
						ret = pub_ln_Ins_ln_lo( g_ln_mst , ln_pay_pln , "0" ,
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst , g_pub_tx.tx_amt1 , g_pub_tx.tx_amt2 ); 	
					if (ret)
					{
						sprintf(acErrMsg,"登记贷款欠款表失败");
						WRITEMSG
							strcpy(g_pub_tx.reply,"L083");
						goto ErrExit;
					}
					TRACE
					/* 判断是否是按揭 */      
					if (g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
					{
						/* 是否是最后一期 */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							TRACE
							/* 生成新的还款计划 */    
								now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*总期数*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*当前期数*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, 
								curr_cnt, ttl_cnt, now_ln_bal, &ln_pay_pln_next );
							if (ret)
							{
								sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
								WRITEMSG
									goto ErrExit;
							}
							sprintf(acErrMsg,"生成的下一期还款计划!下一期第[%d]期",ln_pay_pln.curr_cnt);
							WRITEMSG

								sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln_next.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln_next.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln_next.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"总期数[%d]",ln_pay_pln_next.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln_next.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln_next.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln_next.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln_next.end_date);
							WRITEMSG

								sprintf(acErrMsg,"当前期次还款计划!!!!!!!!!!!!!!!!!!!");
							sprintf(acErrMsg,"账号序号[%d]",ln_pay_pln.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"当前期次[%d]",ln_pay_pln.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"登记日期[%ld]",ln_pay_pln.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"还款状态0[%s]",ln_pay_pln.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"总期数[%d]",ln_pay_pln.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还本金[%lf]",ln_pay_pln.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"当前期次应还利息[%lf]",ln_pay_pln.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"开始归还日期[%ld]",ln_pay_pln.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"到期归还日期[%ld]",ln_pay_pln.end_date);
							WRITEMSG															
							/* 删除此条记录 */
								ret = sql_execute("delete from ln_pay_pln where \
								ac_id=%ld and ac_seqn=%d",\
								ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if (ret)
							{
								sprintf(acErrMsg,"登记还款计划表失败");
								WRITEMSG
									strcpy(g_pub_tx.reply,"L077");
								goto ErrExit;
							}
							vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* 添加还款计划表 */	
								ret = Ln_pay_pln_Ins( ln_pay_pln_next , g_pub_tx.reply );
							if (ret)
							{
								sprintf(acErrMsg,"登记还款计划表失败");
								WRITEMSG
									strcpy(g_pub_tx.reply,"L077");
								goto ErrExit;
							}		
							}else
							{
								strcpy( ln_pay_pln.pay_ind , "1" );    /* 已还 */
								
								/**add by xyy 2006-12-1 22:28**/
							  memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							  ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							  if ( ret )
							  {
							  	sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
							  	WRITEMSG
							  		goto ErrExit;
							  }
							  else
							    memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
								
								vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
								
								ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
								if ( ret )
								{
									sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
									WRITEMSG
										goto ErrExit;
								}
							}
						}
						else	/**hao @20050817@ 已经登记欠款表了，还款计划表已还 **/
						{
							strcpy( ln_pay_pln.pay_ind , "1" );    /* 已还 */
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd 出错[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}	
						}
					}
					continue;

					YOUPay:
					/*登记贷款登记簿*/
					/* 贷款登记簿--本金 */
						ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"1",g_pub_tx.tx_amt1,0.00,"0",1 );
					if ( ret ) 	goto ErrExit;
					/* 贷款登记簿--利息 */
					ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
					if ( ret ) 	goto ErrExit;	

					TRACE
					/* 记流水和记账 */
					/* 贷款方本金部分－流水 */
					strcpy( g_pub_tx.ac_no , ln_ac_no);
					g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
					g_pub_tx.ac_id = g_ln_mst.ac_id;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
					strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
					strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
					strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
					strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
					g_pub_tx.svc_ind=3019;	
					strcpy ( g_pub_tx.brf,"按揭归还--本金" );
					strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*为日终传票打印赋值ln_ac_no*/

					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"交易记帐处理错误!");
						WRITEMSG
							goto ErrExit;
					}
					TRACE
					/* 贷款方利息部分－流水 */
						strcpy( g_pub_tx.ac_no , ln_ac_no);
					g_pub_tx.ac_id = g_ln_mst.ac_id;
					g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
					strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
					strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
					strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/ 
					strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
					g_pub_tx.svc_ind=3059;	
					strcpy ( g_pub_tx.brf,"按揭归还--利息和罚息" );
					strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*为日终传票打印赋值ln_ac_no*/
					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"交易记帐处理错误!");
						WRITEMSG
							goto ErrExit;
					}
					/* 贷款方部分－会计记帐 */
					set_zd_data("0210",g_ln_parm.cur_no);    
					set_zd_double("0430",g_pub_tx.tx_amt2); 
					set_zd_double("0400",g_pub_tx.tx_amt1);                                               
					strcpy(g_pub_tx.sub_tx_code,"G085");
					strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
					ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
					if (ret != 0)
					{
						sprintf(acErrMsg,"会计记帐不成功!!");
						WRITEMSG
							goto ErrExit;
					}
					/* 委托协议贷款(还本) 
					ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "2" );
					if ( ret )  goto ErrExit;*/
					/* 委托协议贷款(还息) 
					ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
					if ( ret )  goto ErrExit;*/

					/* 取款方－流水 */
					strcpy( g_pub_tx.ac_no , dd_ac_no );
					g_pub_tx.ac_id = g_dd_mst.ac_id;
					g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;

					sprintf(acErrMsg,"!![%s],[%ld][%d]",
						g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG

						g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+g_pub_tx.tx_amt2;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-活期 */
					strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
					strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
					strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
					strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
					g_pub_tx.ac_wrk_ind[9]='1';		/*对于非结算户强行扣款*/
					g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */
					g_pub_tx.svc_ind=1001;	
					strcpy ( g_pub_tx.brf,"按揭扣款" );
					strcpy( g_pub_tx.beg_note_no, dd_ac_no);    /*为日终传票打印赋值dd_ac_no*/

					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"交易记帐处理错误!");
						WRITEMSG
							goto ErrExit;
					}   
					/* 取款方－会计记帐 */
					set_zd_data("102J",g_ln_parm.cur_no);    
					set_zd_double("102F",g_pub_tx.tx_amt1);                     
					strcpy(g_pub_tx.sub_tx_code,"D003");
					strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
					ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
					if (ret != 0)
					{
						sprintf(acErrMsg,"会计记帐不成功!!");
						WRITEMSG
							goto ErrExit;
					}            
				}
				Ln_pay_pln_Clo_Upd1( );    		                         
				sprintf(acErrMsg,"日终归还按揭欠款表记录 PASS");
				WRITEMSG	

					OkExit:
					sql_commit();	/*--提交事务--*/
				strcpy(g_pub_tx.reply,"0000");
				sprintf(acErrMsg,"日终归还按揭欠款表记录处理程序成功!!!!![%s]",g_pub_tx.reply);
				WRITEMSG
					set_zd_data("0120",g_pub_tx.reply);
				return 0;
				ErrExit:
				if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
				{
					strcpy(g_pub_tx.reply,"G009");/* 待修改 */
				}
				sql_rollback();	/*--事务回滚--*/
				sprintf(acErrMsg,"日终归还按揭欠款表记录处理程序失败!!!!![%s]",g_pub_tx.reply);
				WRITEMSG
					set_zd_data("0120",g_pub_tx.reply);
				return 1;
			}

