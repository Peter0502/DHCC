/*************************************************************
* 文 件 名: gD082.c
* 功能描述：日终－普通贷款还款计划检查
*
* 作    者: lance
* 完成日期: 2003年11月18日
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
#include "ln_pln_inf_c.h"

gD082()
{
	int ret=0;
	long ac_seqn=0;
	double shld_pay_amt=0.00;
	char ln_ac_no[21];
	char dd_ac_no[21];
	char comm[100];
	struct ln_pln_inf_c	ln_pln_inf;
	struct com_holiday_c	com_holiday;		
	
	memset( &com_holiday,0x00,sizeof(struct com_holiday_c )); 
	
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/* 初始化全局变量 */
	pub_base_sysinit();
	TRACE
	
	sprintf( comm," pay_ind='0' and beg_date<=%ld and end_date>=%ld ",g_pub_tx.tx_date,g_pub_tx.tx_date);
	ret= Ln_pln_inf_Dec_Upd( g_pub_tx.reply , comm );
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		/*** 数据初始化 ***/
		memset( &ln_pln_inf,0x00,sizeof(struct ln_pln_inf_c ));    
		memset( dd_ac_no, 0x00, sizeof(dd_ac_no));
		memset( ln_ac_no, 0x00, sizeof(ln_ac_no));
		strcpy( g_pub_tx.tx_code, "G082");		

		ret = Ln_pln_inf_Fet_Upd(&ln_pln_inf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}       
		else if (ret == 100)
		{
			break;
		}
		strcpy( ln_pln_inf.content,"");
		sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
		WRITEMSG
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );
				
		ret= Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst,"ac_id=%ld and ac_seqn=%d",ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"ret=%d 读取贷款户主文件失败[%d-%d]",
					ret,ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
			WRITEMSG
			goto ErrExit;   
		}
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;
		strcpy(g_pub_tx.tx_br_no,g_ln_mst.opn_br_no);

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",ln_pln_inf.ac_id);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W010");
			goto ErrExit;
		}
		else if ( ret && ret!=100)
		{
			sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W011");
			goto ErrExit;
		}
		strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
		memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
		TRACE
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
		if( ret ) 
		{
			strcpy(acErrMsg,"取贷款产品参数表错误!");
			WRITEMSG
			goto ErrExit;
		}

		/* 判断节假日是否处理 */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ){continue;}
		TRACE
		
		/* 按揭贷款不处理 */
            	if(g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
            	{
			strcpy(acErrMsg,"按揭贷款不在此处理!");
			WRITEMSG
			goto ErrExit;
		}            		
				sprintf(acErrMsg,"shld_pay_amt[%.2lf][%.2lf]",
					ln_pln_inf.curr_amt-ln_pln_inf.true_amt,g_ln_mst.bal);
				WRITEMSG
		
		/* 是否自动还款 */
		if ( g_ln_parm.auto_pay_ind[0]!='Y')
		{
			goto NoPay;
		}
		if( g_ln_mst.repay_ac_id<=0 )
		{
			sprintf(acErrMsg,"无归还账号[%ld]",g_ln_mst.repay_ac_id);
			WRITEMSG
			goto NoPay;
		}
		if( g_dd_parm.draw_ind[0]=='N' )
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
		if( ret )
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
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该产品编号!prdt_no=[%s]",g_dd_mst.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		if( g_dd_mst.ac_sts[0]!='1' )
		{
			sprintf(acErrMsg,"账号状态非正常状态!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
			goto NoPay;
		}
		/* 金额检查 */
		g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;

		ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00);
		if( ret<=0 )
		{	
			sprintf(acErrMsg,"无足够金额[%.2lf]",g_dd_mst.bal);
			WRITEMSG
			goto NoPay;
		}
	
		/* 考虑冻结状态 */
		if( g_dd_mst.hold_sts[0]!='0' )
		{
			g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

			/**完全冻结或只进不出视可用为0.00**/
			if( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
				g_pub_tx.tmp_amt1=0.00;
			else
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

			ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1 , 0.00);
			if( ret<=0 )
			{
				sprintf(acErrMsg,"无足够金额[%s][%.2lf]",
					g_dd_mst.hold_sts,g_pub_tx.tmp_amt1);
				WRITEMSG
				goto NoPay;
			}
		}

		TRACE
				sprintf(acErrMsg,"shld_pay_amt[%.2lf][%.2lf]",
					ln_pln_inf.curr_amt-ln_pln_inf.true_amt,g_ln_mst.bal);
				WRITEMSG
		/* 几种扣款情况 */
		/* 若贷款余额未达到本期需还金额 */
		shld_pay_amt=0.00;
		if( pub_base_CompDblVal( (ln_pln_inf.curr_amt-ln_pln_inf.true_amt),g_ln_mst.bal )>0 )
		{
			shld_pay_amt=g_ln_mst.bal;
			/* 不够还,则不还 */
			if( pub_base_CompDblVal( g_pub_tx.tmp_amt1,shld_pay_amt )<0 )
			{
				goto NoPay;
			}else{
				strcpy( ln_pln_inf.pay_ind,"1" );
				strcpy( ln_pln_inf.content,"贷款余额低于计划还款额");
				ln_pln_inf.true_amt=ln_pln_inf.true_amt+shld_pay_amt;
			}
		}else{
			shld_pay_amt=ln_pln_inf.curr_amt-ln_pln_inf.true_amt;
			/* 不够还,则不还 */
			if( pub_base_CompDblVal( g_pub_tx.tmp_amt1,shld_pay_amt )<0 )
			{
				goto NoPay;
			}else{
				strcpy( ln_pln_inf.pay_ind,"1" );
				ln_pln_inf.true_amt=ln_pln_inf.true_amt+shld_pay_amt;
			}
		}

		/*写贷款登记簿  0登记欠款 1回收已登记欠款 2登记并回收*/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"1",shld_pay_amt,0.00,"0",1 );
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"写贷款登记簿PASS");
		WRITEMSG
		
		set_zd_data( "0660" , "2" );	
		/* 还贷款本金 */
		strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*未读取了mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.note_type,"299");
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
   		sprintf(acErrMsg,"*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	   	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	   	g_pub_tx.svc_ind=3010;
	   	g_pub_tx.tx_amt1=shld_pay_amt;	
		strcpy ( g_pub_tx.brf,"贷款扣本--还款计划" );
	
	   	/*** 交易记帐处理 ***/
	   	if( pub_acct_trance() )
	   	{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
	   	}
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	/* 进行会计记帐 */
		set_zd_data("0210",g_ln_parm.cur_no);  
		set_zd_double("107F",g_pub_tx.tx_amt1);       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_amt1=0.00;
		set_zd_double("107F",0.00);    					
			
		/* 扣客户存款账 */
		strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
		g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*未读取了mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-活期 */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/ 
 		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*为日终传票打印赋值dd_ac_no*/
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.ac_wrk_ind[9]='1';    	    /*对于非结算户强行扣款*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */

	   	g_pub_tx.svc_ind=1001;
	   	g_pub_tx.tx_amt1=shld_pay_amt;	
		strcpy ( g_pub_tx.brf,"贷款还本" );
	
	   	/*** 交易记帐处理 ***/
	   	if( pub_acct_trance() )
	   	{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
	   	}
		/* 进行会计记帐 */
		set_zd_data("102J",g_ln_parm.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);             
		strcpy(g_pub_tx.sub_tx_code,"D003");/**取款子交易**/
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
		}
		set_zd_double("102F",0.00);             
	   	g_pub_tx.tx_amt1=0.00;
	   	
	   	goto Youpay;

NoPay:    	
    		strcpy( ln_pln_inf.pay_ind,"0" );
		strcpy( ln_pln_inf.content,"存款余额不足");
Youpay:    	             	               
		sprintf(acErrMsg,"!!!!!!!!!ac_id=[%ld],curr_amt=[%.2lf],true_amt=[%.2lf],shld_amt=[%.2lf]",ln_pln_inf.ac_id,ln_pln_inf.curr_amt,ln_pln_inf.true_amt,shld_pay_amt);
		WRITEMSG		
		ln_pln_inf.lst_date=g_pub_tx.tx_date;
		ret = Ln_pln_inf_Upd_Upd(ln_pln_inf,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"更新普通贷款还款计划表错误!!");
			WRITEMSG
			goto ErrExit;
		} 
    }
    Ln_pln_inf_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终－普通贷款还款计划检查成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* 待修改 */
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终－普通贷款还款计划检查失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
