/*************************************************************
* 文 件 名: gDCLN.c
* 功能描述：按揭还款计划调整
*
* 作    者: jack
* 完成日期: 2003年3月30日
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
#include "ln_pay_pln_c.h"
#include "ln_mst_hst_c.h"
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}

gDCLN()
{      
	int ret=0;
	int cnt=0;
	long ln_mst_max_date=0;
	long ln_mst_ic_date=0;
	int month_cnt=0;
	int month_now=0;
	
	struct ln_pay_pln_c	S_ln_pay_pln;
	struct ln_mst_c		S_ln_mst;
	struct ln_mst_hst_c	S_ln_mst_hst;
	
	memset( &S_ln_pay_pln , 0x00 ,sizeof( struct ln_pay_pln_c ));
	memset( &S_ln_mst , 0x00 ,sizeof( struct ln_mst_c ));
	memset( &S_ln_mst_hst , 0x00 ,sizeof( struct ln_mst_hst_c ));
			
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
       
	/* 读贷款主文件 */    
	ret = Ln_mst_Dec_Upd(g_pub_tx.reply," opn_date!=ic_date and repay_type in ('3','4') ");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret= Ln_mst_Fet_Upd(&S_ln_mst, g_pub_tx.reply);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;			
		}else if( ret==100 ){
			sprintf(acErrMsg,"共执行【%d】次",cnt);
			WRITEMSG			
			break;
		}
		
		cnt++;
		/*对此户特殊处理*/
		if( S_ln_mst.ac_id == 100354593 )
		{
			ln_mst_max_date= 20040326;
		
			sprintf(acErrMsg,"贷款明细中最晚的提前还款日【%d】",ln_mst_max_date);
			WRITEMSG
			sprintf(acErrMsg,"贷款主文件中原有的起息日期【%d】",S_ln_mst.ic_date);
			WRITEMSG
		}else{	
			ln_mst_max_date=0;
			ret= sql_max_long("ln_mst_hst","tx_date",&ln_mst_max_date,"ac_id=%ld and ac_seqn=%d and tx_code='2486'",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
			if( ret )	goto ErrExit;
			sprintf(acErrMsg,"贷款明细中最晚的提前还款日【%d】",ln_mst_max_date);
			WRITEMSG
			sprintf(acErrMsg,"贷款主文件中原有的起息日期【%d】",S_ln_mst.ic_date);
			WRITEMSG	
		}
			
		if( ln_mst_max_date%100 < 21 )
		{
			sprintf(acErrMsg,"还款日小于21日");
			WRITEMSG	
			if( ln_mst_max_date%10000<=200 )/*若非1月份*/
			{
				sprintf(acErrMsg,"还款日为一月份");
				WRITEMSG				
				ln_mst_ic_date= (ln_mst_max_date/10000-1)*10000+1222;
			}else{
				ln_mst_ic_date= (ln_mst_max_date/100-1)*100+22;	
			}			
		}else{
			sprintf(acErrMsg,"还款日大于21日");
			WRITEMSG
		 	ln_mst_ic_date= ln_mst_max_date/100*100+22;
		}
		S_ln_mst.ic_date= ln_mst_ic_date;
		sprintf(acErrMsg,"修改后的起息日期【%d】",S_ln_mst.ic_date);
		WRITEMSG							
		
		/*修改按揭还款计划*
		ret= Ln_pay_pln_Sel( g_pub_tx.reply, &S_ln_pay_pln,"ac_id=%ld and ac_seqn=%d ",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if (ret)
		{
			sprintf(acErrMsg,"Ln_pay_pln_Sel ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}	
		****/
			
		/* 计算总期数 */
		month_cnt = pub_base_CalMouths( S_ln_mst.ic_date , S_ln_mst.mtr_date ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"总期数[%d]",month_cnt);
		WRITEMSG
		if( S_ln_mst.mtr_date%100>21 )	
		{
			sprintf(acErrMsg,"修改后总期数[%d]",month_cnt);
			WRITEMSG
			month_cnt=month_cnt+1;
		}
		sprintf(acErrMsg,"修改后总期数[%d]",month_cnt);
		WRITEMSG		
		month_now = pub_base_CalMouths( S_ln_mst.ic_date , 20050921 ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"现期数[%d]",month_now);
		WRITEMSG				
		ret= sql_execute( " UPDATE ln_pay_pln SET ttl_cnt=%d, curr_cnt=%d WHERE ac_id=%ld and ac_seqn=%d ",month_cnt,month_now,S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		TRACE
		ret = Ln_mst_Upd_Upd( S_ln_mst , g_pub_tx.reply  );
	        if (ret)
	        {
	               sprintf(acErrMsg,"更新贷款主文件错误!!");
	               WRITEMSG
	               goto ErrExit;
	        }
	        ret= Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
	        if(ret)		goto ErrExit;
		sprintf(acErrMsg,"----------------------－－－－－--------------------------------");
		WRITEMSG	         
		sprintf(acErrMsg,"－－－－－－－－－－－第【%d】条记录－－－－－－－－－－－－－－",cnt);
		WRITEMSG
		sprintf(acErrMsg,"ac_no=【%s】到期日=【%d】户名=【%s】",g_mdm_ac_rel.ac_no,S_ln_mst.mtr_date,S_ln_mst.name);
		WRITEMSG
		sprintf(acErrMsg,"ac_id=【%ld】起息日=【%d】总期数=【%d】现期数=【%d】",S_ln_mst.ac_id,S_ln_mst.ic_date,month_cnt,month_now);
		WRITEMSG
		sprintf(acErrMsg,"-----------------------------------------------------------------");
		WRITEMSG
	}
	Ln_mst_Clo_Upd( );    		                         
	sprintf(acErrMsg,"按揭还款计划调整 PASS");
	WRITEMSG	
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"按揭还款计划调整成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* 待修改 */
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"按揭还款计划调整失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
