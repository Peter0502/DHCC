/*************************************************************
* 文 件 名: gDCLL.c
* 功能描述：按揭还款计划调整(换所账号)
*
* 作    者: jack
* 完成日期: 2003年9月19日
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

gDCLL()
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
	ret = Ln_mst_Dec_Sel(g_pub_tx.reply," opn_date=ic_date and repay_type in ('3','4') ");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret= Ln_mst_Fet_Sel(&S_ln_mst, g_pub_tx.reply);
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
		TRACE
		ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst,"ac_id=%ld and ac_seqn=%d and tx_code='2498' ",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret&&ret!=100)
		{
			sprintf(acErrMsg,"查询贷款明细失败 ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if(ret==100){
			TRACE
			continue;
		}
							
		month_now = pub_base_CalMouths( S_ln_mst.ic_date , 20050921 ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"现期数[%d]",month_now);
		WRITEMSG
		if( S_ln_mst.opn_date%100 >21 )
		{
			month_now=month_now-1;
		}				
		ret= sql_execute( " UPDATE ln_pay_pln SET curr_cnt=%d WHERE ac_id=%ld and ac_seqn=%d ",month_now,S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		cnt++;
		
		TRACE
	        ret= Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
	        if(ret)		goto ErrExit;
		sprintf(acErrMsg,"----------------------－－－－－--------------------------------");
		WRITEMSG	         
		sprintf(acErrMsg,"－－－－－－－－－－－第【%d】条记录－－－－－－－－－－－－－－",cnt);
		WRITEMSG
		sprintf(acErrMsg,"ac_no=【%s】到期日=【%d】户名=【%s】",g_mdm_ac_rel.ac_no,S_ln_mst.mtr_date,S_ln_mst.name);
		WRITEMSG
		sprintf(acErrMsg,"ac_id=【%ld】起息日=【%d】现期数=【%d】",S_ln_mst.ac_id,S_ln_mst.ic_date,month_now);
		WRITEMSG
		sprintf(acErrMsg,"-----------------------------------------------------------------");
		WRITEMSG
	}
	Ln_mst_Clo_Sel( );    		                         
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
