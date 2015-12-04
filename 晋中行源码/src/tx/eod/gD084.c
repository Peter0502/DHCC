/*************************************************************
* 文 件 名: gD084.c
* 功能描述：日终－按揭贷款主文件到期
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

gD084()
{
    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    char ln_ac_no[20],dd_ac_no[20],flag[2];  
    struct com_holiday_c com_holiday;
    struct ln_mst_c ln_mst_tmp;
    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
    
    ret=sql_begin(); /*打开事务*/
    if(ret)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /* 初始化全局变量 */
    pub_base_sysinit();

    ret=Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"mtr_date<= %ld",g_pub_tx.tx_date);
    if(ret)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	TRACE
    while(1)
    {
      	ret=Eod_ln_mst_Fet_Upd(&ln_mst_tmp, g_pub_tx.reply);
        if (ret&&ret!=100)
        {
			TRACE
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if(ret==100)
        {
			TRACE
            break;
        }
        g_pub_tx.ac_seqn=ln_mst_tmp.ac_seqn;
		TRACE
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",
															ln_mst_tmp.ac_id);
        if(ret==100)
        {
            sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }else if(ret&&ret!=100){
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        
        /* 取贷款信息 */
        ret = pub_ln_lndis_file(ln_ac_no,g_pub_tx.ac_seqn,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
		if(ret) {goto ErrExit;}
		/*交易机构赋值*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

		TRACE
        if(g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4')
        {	
			TRACE
        	continue;
		}
		/* 判断节假日是否处理 */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) continue;
		TRACE
      	/* 取贷款状态 */
        chg_flag = pub_ln_check_stsdate();
		if( chg_flag==-1 )	/* 贷款不需要做到期处理 */
		{
			TRACE
			continue;
		}
		/***************转出部分*************************/	
		TRACE
		strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ;          
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
														3日终汇总入*/  
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=3060;
		g_pub_tx.tx_amt1=-g_ln_mst.bal;	   	  	   	
		strcpy ( g_pub_tx.brf,"日终到期按揭贷款转出" );
	
		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}
		TRACE
		/* 进行会计记帐 */
		set_zd_data("0210",g_ln_parm.cur_no);  
		set_zd_double("1074",g_pub_tx.tx_amt1);                                 
		strcpy(g_pub_tx.sub_tx_code,"L30B");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
		}
		TRACE
		/* 更新贷款主文件 */
		sprintf( flag , "%d" , chg_flag );
		strcpy( g_ln_mst.ac_sts,flag );               	               
		ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"更新贷款主文件错误!!");
            WRITEMSG
            goto ErrExit;
		} 
	   /**********转入部分************/
	   TRACE
       strcpy( g_pub_tx.ac_no , ln_ac_no );
       g_pub_tx.ac_id = g_ln_mst.ac_id ;            
       g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
	   strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
	   strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	   strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	   strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
       strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	   g_pub_tx.svc_ind=3060;
	   g_pub_tx.tx_amt1=g_ln_mst.bal;	   	  	   	
       strcpy ( g_pub_tx.brf,"日终到期贷款挂账" );
	
	   /*** 交易记帐处理 ***/
	   if( pub_acct_trance() )
	   {
            sprintf(acErrMsg,"交易记帐处理错误!");
            WRITEMSG
            goto ErrExit;
	   }
	   /* 进行会计记帐 */
       set_zd_data("0210",g_ln_parm.cur_no); 
       set_zd_double("107F",g_pub_tx.tx_amt1);                       
       strcpy(g_pub_tx.sub_tx_code,"L302");
       strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
       ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
       if (ret != 0)
       {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
       }                     
       TRACE       
    }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终贷款到期处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* 待修改 */
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终贷款到期处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
