/*************************************************
* 文 件 名: pubf_ln_check.c
* 功能描述：   
* 			pub_ln_check     贷款产品属性检查
* 			pub_ln_chk_clear 查看贷款是否还清
*			pub_ln_lndis_file根据贷款显示账号和账号序号取贷款主文件
*			pub_ln_lnid_file 根据贷款账号ID和账号序号取贷款主文件
*			pub_ln_check_sts 检查贷款账户状态
*			pub_ln_check_stsdate 检查贷款账户状态(检查临界状态)
*
* 作    者:    rob
* 完成日期：   2004年12月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "dc_acc_rel_c.h"
#include "com_sect_acm_c.h"
#include "com_item_c.h"
/**********************************************************************
* 函 数 名：  pub_ln_check
* 函数功能：  贷款产品属性检查,检查台账内容是否合乎产品参数定义的要求
* 作    者：  rob
* 完成时间：  2004年12月28日
*
* 参    数： 
*     输  入：tx_type   char(1)   交易类型(1-开户 2-展期 3-还款 4-销户)    
*      
*     输  出:           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_check(char *tx_type)
{
	struct mo_infrm_c mo_infrm_c;

	int ret = 0;
	long min_day , max_day ;
	double min_rate , max_rate , rate;
	double rate_val,over_rate_val,fine_rate_val;
	long infrm_date;

	memset( &g_ln_parm,0x00,sizeof(struct ln_parm_c) );
	vtcp_log("%s,%d ",__FILE__,__LINE__);
    /* 根据产品代码查贷款产品参数表，查看是否有该种产品 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm, "prdt_no='%s'", 
						g_ln_mst.prdt_no);
		
	if( ret==100 )
	{
		vtcp_log("%s,%d g_ln_mst.prdt_no=[%s] ",__FILE__,__LINE__,g_ln_mst.prdt_no);
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%d]",ret);
			vtcp_log("%s,%d g_ln_mst.prdt_no=[%s] ",__FILE__,__LINE__,g_ln_mst.prdt_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    		}
	switch( atoi(tx_type) )
	{
	  case 1:	/* 开户时检查 */

		/* 检查开户日期是否在启用日期和止用日期之间 */	
		if ( (g_ln_parm.beg_date > g_ln_mst.opn_date) || \
			(g_ln_mst.opn_date > g_ln_parm.end_date))
		{	
	        strcpy(acErrMsg,"开户日期不在在启用日期和止用日期之间!");
		    WRITEMSG
		    strcpy ( g_pub_tx.reply , "A001" );
		    goto ErrExit;
		}
		
		/* 检查贷款的最小期限，最大期限 */	
		
		if ( g_ln_parm.term_type[0]=='Y' )      /* 期限类型：年 */
		{	
			 TRACE
			g_ln_parm.min_term = g_ln_parm.min_term*12;
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* 期限类型：月 */
          {
			 TRACE
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' )  /* 期限类型：日 */
		  {
			 TRACE
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
			 TRACE
		if ( g_ln_mst.mtr_date<min_day )
		  {
              	sprintf( acErrMsg,"贷款期限小于规定的最小期限![%ld][%ld][%s]",
							g_ln_mst.mtr_date , min_day , g_ln_parm.term_type );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L003" );
				goto ErrExit;
		  }
        if ( g_ln_mst.mtr_date>max_day )
		  {
              	sprintf( acErrMsg,"贷款期限大于规定的最大期限![%ld][%ld][%s]",
							g_ln_mst.mtr_date , max_day , g_ln_parm.term_type );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }
	sprintf(acErrMsg,"检查贷款的最小期限，最大期限(开户) PASS!");
	WRITEMSG	
	
		/* 检查该产品最低余额和最高限额 */
		if( pub_base_CompDblVal( g_ln_parm.min_bal , 0.00)>0 )
		{
			if ( g_ln_mst.bal<g_ln_parm.min_bal )
			{
				strcpy(acErrMsg,"贷款余额低于最低余额！");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A003" );
				goto ErrExit;
			}
		}
		if( pub_base_CompDblVal( g_ln_parm.min_bal , 0.00)>0 )		
		{
			if ( g_ln_mst.bal>g_ln_parm.max_bal)
			{
				strcpy(acErrMsg,"贷款余额超过最高限额！");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A004" );
				goto ErrExit;
			}
		}
	sprintf(acErrMsg,"检查该产品最低余额和最高限额(开户) PASS!");
	WRITEMSG	
	
		/* 检查利率是否超过规定利率上浮和下浮后的界限*/
		/* 调用函数根据利率编号取得利率 */
	   /***取利率编号对应的利率***/
	   if( g_ln_mst.opn_date>=20030827 )
	   {
		   ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
						g_pub_tx.tx_date,&rate_val);
		   if( ret ) goto ErrExit;
		   sprintf(acErrMsg,"转换前利率[%lf]",rate_val);
		   WRITEMSG
	
		   rate_val = rate_val/1.2;/* 转换成月利率 */ 
		   sprintf(acErrMsg,"转换结束后利率[%lf]",rate_val);
		   WRITEMSG
	
		   /** 取逾期利率 **/
		   if( g_ln_parm.over_rate_type[0]=='0' )/* 取逾期利率编号 */
		   {
		   		TRACE
		   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&over_rate_val);
		   		if( ret ) goto ErrExit;					
		   		over_rate_val = over_rate_val/1.2;/* 转换成月利率 */
		   }else{
				TRACE
				over_rate_val = g_ln_mst.rate ;
		   }
		   /** 取罚息利率 **/
		   if( g_ln_parm.fine_rate_type[0]=='0' )/* 取逾期利率编号 */
		   {
		   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&fine_rate_val);
		   		if( ret ) goto ErrExit;					
		   		fine_rate_val = fine_rate_val/1.2;/* 转换成月利率 */
		   }else{
				fine_rate_val = g_ln_mst.rate ;
		   }
			sprintf(acErrMsg,"逾期利率[%s]",g_ln_parm.over_rate_no);
			WRITEMSG
			sprintf(acErrMsg,"罚息利率[%s]",g_ln_parm.fine_rate_no);
			WRITEMSG		
			sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
			WRITEMSG
		 	sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
			WRITEMSG
	
	
			if( g_ln_parm.get_rate_type[0]=='0' ) /**挂牌利率**/
			{
				g_ln_mst.rate=rate_val;
			}else{		/**手输利率检查上浮下浮比率**/  
				/*正常利率检查*/
				if( pub_base_CompDblVal(g_ln_mst.rate,
						rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
						rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
								,g_ln_parm.max_flt_ratio );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A006" );
					goto ErrExit;
				}
	            sprintf(acErrMsg,"最高利率正确时[%lf][%lf][%d] ", rate_val,
				rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
				WRITEMSG
	
				if( pub_base_CompDblVal(g_ln_mst.rate,
						rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
				{
				sprintf(acErrMsg,"最低利率错误时[%lf][%lf][%d] ", rate_val,
			 rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
							 WRITEMSG
					strcpy ( g_pub_tx.reply , "A005" );
					goto ErrExit;
				}
				sprintf(acErrMsg,"最低利率正确时[%lf][%lf][%d] ", rate_val,
			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
				WRITEMSG
	
				/**逾期利率检查**/
				if( pub_base_CompDblVal(g_ln_mst.over_rate,
						over_rate_val*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.over_rate,g_ln_parm.over_rate_max,
					over_rate_val*(100+g_ln_parm.over_rate_max)/100.00
								 );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L062" );
					goto ErrExit;
				}
				if( pub_base_CompDblVal(g_ln_mst.over_rate,
						over_rate_val*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
				{
					sprintf(acErrMsg,"over_rate rate is too high[%lf]over_rate_val=[%lf]g_ln_mst.rate=[%lf]!!",
							over_rate_val*((100+g_ln_parm.over_rate_min)/100.00),over_rate_val,g_ln_mst.over_rate);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L061" );
					goto ErrExit;
				}
				
				/**罚息利率检查**/
				if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
					fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00
								 );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L163" );
					goto ErrExit;
				}
				if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00) )<0 )
				{
					sprintf(acErrMsg,"current rate is too low[%lf][%lf]!!",
							g_ln_mst.fine_rate,fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00));
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L162" );
					goto ErrExit;
				}						
			}
			sprintf(acErrMsg,"检查正常/逾期/罚息利率(开户) PASS!");
			WRITEMSG
		}
	
		/* 检查开户限制条件编码 (调用函数)
		if ( g_ln_parm.opn_lmt == 000 )
		{
			goto ErrExit;
		}
		*/
		break;
	sprintf(acErrMsg,"检查开户限制条件编码(开户) PASS!");
	WRITEMSG	
	
   case 2:   /* 展期时检查 */
   		/*检查可否展期标志*/
		if( g_ln_parm.exp_ind[0]=='N' )
    		{
			strcpy(acErrMsg,"该贷款产品不可以展期!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L007" );
		    	goto ErrExit;
		}else{
			if( atoi(g_ln_mst.exp_ind)>=(atoi(g_ln_parm.exp_ind)+1) )
		        {
				strcpy(acErrMsg,"该贷款产品已超过展期次数，不可展期!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L008" );
				goto ErrExit;
			}
		}

		/* 检查贷款展期后是否超过贷款的最大期限 */	
		
		if ( g_ln_parm.term_type[0]=='Y' )     /* 期限类型：年 */
		{	
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* 期限类型：月 */
          {
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' ) /* 期限类型：日 */
		  {
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }

        if ( g_ln_mst.mtr_date>max_day )
		  {
                strcpy( acErrMsg,"贷款期限大于规定的最大期限!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }

		/* 检查利率是否超过规定利率上浮和下浮后的界限*/
		/* 调用函数根据利率编号取得利率(只取利率) */
		/* 展期利率必须大于贷款合同利率 */
		/******************8
		min_rate=g_ln_mst.rate-g_ln_parm.min_flt_ratio;
		max_rate=g_ln_mst.rate+g_ln_parm.max_flt_ratio;
	
		if ( g_ln_mst.rate < min_rate )
		{
			strcpy(acErrMsg,"贷款展期利率低于规定的最低利率！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L009" );
			goto ErrExit;
		}
		else if ( g_ln_mst.rate > max_rate )
		{
			strcpy(acErrMsg,"贷款展期利率高于规定的最高利率！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L010" );
			goto ErrExit;
		}
		***********************/
		if( pub_base_CompDblVal(g_ln_mst.exp_rate,g_ln_mst.rate )<0 )
		{
			sprintf(acErrMsg,"展期利率过低[%lf][%lf]!!",g_ln_mst.exp_rate,g_ln_mst.rate);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L124" );
			goto ErrExit;
		}		
		break;

   case 3:   /* 还款时检查 */

     if( g_pub_tx.tx_date < g_ln_mst.mtr_date )
	  {
        ret = Mo_infrm_Sel(g_pub_tx.reply, &mo_infrm_c , \
				  "ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld" , \
			       g_ln_mst.ac_id , g_ln_mst.ac_seqn , g_pub_tx.tx_date );

     	  if( ret==100 )
			{
				sprintf( acErrMsg,"通知登记薄中无此记录 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L011" );
				goto ErrExit;
			}else if( ret !=0 )
				{
					sprintf( acErrMsg,"取通知登记薄表异常 erro code=[%d]",ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"D103" );
			    	goto ErrExit;
				}
		
     if( g_pub_tx.tx_amt1> g_ln_parm.fdrq_lmt_amt )
 	 {
		  strcpy(acErrMsg,"还款金额超过提前还款限额!");
		  WRITEMSG
		  strcpy ( g_pub_tx.reply , "L012" );
		  goto ErrExit;
	 }
	}

		break;

   case 4:   /* 销户时检查 */   

		/* 检查销户限制条件编码 (调用函数) */
		if ( g_ln_parm.cls_lmt == 000 )
		{
			goto ErrExit;
		}		

		break;

   default: /* 交易类型入口参数错误  */
	   
		strcpy( acErrMsg,"交易类型入口参数错误" );
		WRITEMSG
		goto ErrExit;

}

 return 0;

/* 错误出口 */
ErrExit:

		return 1;

}
/**********************************************************************
* 函 数 名：  pub_ln_lndis_file
* 函数功能：  根据贷款显示账号和账号序号取贷款主文件
* 作    者：  rob
* 完成时间：  2004年12月30日
*
* 参    数： 
*     输  入：  ac_no       char(19)  显示账号  	
*               ac_seqn     smallint  账号序号	       
*
*     输  出:                         介质帐号对照文件结构
*                                     贷款主文件结构
*                                     贷款参数文件结构
*                           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_lndis_file(char *ac_no,int ac_seqn , 
	struct mdm_ac_rel_c *mdm_ac_rel,struct ln_mst_c *ln_mst,
	struct ln_parm_c *ln_parm )
{
    int ret = 0 ;
    
    /* 根据显示账号，在凭证(介质)与账户关系mdm_ac_rel表中查询，取出账户ID */
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_no='%s'", ac_no );
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录[%d][%s]",ret,ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常[%d][%s]",ret,ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
    	        
	/******
	if( mdm_ac_rel->ac_seqn && ac_seqn && ac_seqn!=mdm_ac_rel->ac_seqn )
	{
    	sprintf( acErrMsg,"账户序号异常mdm[%d]input[%d]",
			ac_seqn,mdm_ac_rel->ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D033" );
    	return -1;
	}
	****/

   	/* 取贷款主文件信息 */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        mdm_ac_rel->ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* 取贷款参数信息 */
	ret = Ln_parm_Sel(g_pub_tx.reply, ln_parm , " prdt_no='%s' " ,
				        ln_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"贷款参数文件中无此记录 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款参数文件异常 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
  return 0 ;
}


/**********************************************************************
* 函 数 名：  pub_ln_lnid_file
* 函数功能：  根据贷款账号ID和账号序号取贷款主文件
* 作    者：  rob
* 完成时间：  2004年12月30日
*
* 参    数： 
*     输  入：  ac_id       integer   账号ID  	
*               ac_seqn     smallint  账号序号	       
*
*     输  出:                         贷款主文件结构
*                           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_lnid_file ( long ac_id , int ac_seqn , struct ln_mst_c *ln_mst )
{

	int ret = 0 ;
	
	memset( ln_mst, 0x00, sizeof( struct ln_mst_c ) );
    	        
    /* 取贷款主文件信息 */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
			        ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
  return 0 ;
}
/**********************************************************************
* 函 数 名：  pub_ln_check_sts
* 函数功能：  检查贷款账户状态
* 作    者：  rob
* 完成时间：  2004年04月30日
*
* 参    数： 
*     输  入：  ln_mst_c       贷款主文件结构   	
*               mode     int   检查模式	       
*
*     输  出:       
*
*     返回值： 0 成功
*             -1 失败
* 
* 修改历史：
*                   
********************************************************************/
int pub_ln_check_sts( struct ln_mst_c ln_mst,int mode )
{
	int ret=0;

	if( ln_mst.ac_sts[0]=='*' )
	{
    	sprintf( acErrMsg,"取贷款状态异常[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L035" );
    	return -1;
	}
	if( ln_mst.ac_sts[0]=='9' )
	{
    	sprintf( acErrMsg,"取贷款状态异常[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L036" );
    	return -1;
	}
	if( ln_mst.ac_sts[0]!='1' && ln_mst.ac_sts[0]!='2' 
		&& ln_mst.ac_sts[0]!='3' && ln_mst.ac_sts[0]!='4' && ln_mst.ac_sts[0]!='5' )
	{
    	sprintf( acErrMsg,"取贷款状态异常[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L036" );
    	return -1;
	}

	return 0;
}
/**********************************************************************
* 函 数 名：  pub_ln_check_stsdate
* 函数功能：  检查贷款账户状态(检查临界状态)
* 作    者：  rob
* 完成时间：  2004年04月30日
*
* 参    数： 
*     输  入： 	       
*
*     输  出:       
*
*     返回值： 2－正常最后一天,需改逾期
*		3－逾期最后一天,需改呆滞
* 
* 修改历史：
*                   
********************************************************************/
int pub_ln_check_stsdate( )
{
	int ret=0;
	long date,dqr; 
	
	/* 转逾期标志 */
	if ( g_ln_parm.turn_ill_ind[0]=='n' )	
	{	
		return -1;
	}
	/* 判断展期标志 */
	if ( g_ln_mst.exp_ind[0]!='0' )
	{
		date=g_ln_mst.exp_mtr_date;
	}else{
		date=g_ln_mst.mtr_date;
	}
	/* 当前状态为正常 */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
		
	vtcp_log("JJJJJJJJJJJJJJJgpub_tx.txdate=[%d]",g_pub_tx.tx_date);
		if( date<=g_pub_tx.tx_date )
		
		/***
		if( date==g_pub_tx.tx_date )		
		****/
		{
			return 2;
		}else{
			return -1;
		}
	}
	/**add by ligl2006-9-25 15:57**/
	/*当前状态为逾期90内**/
	if(g_ln_mst.ac_sts[0]=='2')
	{
     ret=pub_base_deadlineD(date,g_ln_parm.turn_ill_term,&dqr );
     if(ret)	return -1;
     sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
			WRITEMSG							
		if( dqr==g_pub_tx.tx_date )
     {
				return 5;
		 }
		 else
		 {
				return -1;
		 }		
	}
	/**当前状态为逾期90天外**/
	if ( g_ln_mst.ac_sts[0]=='5' )
	{
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -1;
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG
		if( dqr==g_pub_tx.tx_date )		
		{
				return 3;
		}else
		{
				return -1;
		}
	}
	/**end**/
	
	/* 当前状态为逾期 */
	/**if ( g_ln_mst.ac_sts[0]=='2' )
	{
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -1;
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG	**/	
		/******
		if( dqr<=g_pub_tx.tx_date )
		******/
		/**if( dqr==g_pub_tx.tx_date )		
		{
			return 3;
		}else{
			return -1;
		}
	}**/
	return -1;		
}
/**********************************************************************
* 函 数 名：  pub_ln_check_acsts
* 函数功能：  返还贷款账户状态
* 作    者：  rob
* 完成时间：  2004年04月30日
*
* 参    数： 
*     输  入： 	      
*
*     输  出:  ln_flag 		1-正常
*				Z-展期(字母Z)
*				2-逾期     
*				3-呆滞
*				4-呆帐
*
*     返回值： 
* 
* 修改历史：
*                   
********************************************************************/	
int pub_ln_check_acsts( char *ln_flag )
{ 		
	strcpy( ln_flag , g_ln_mst.ac_sts );
	/* 当前状态为正常 */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
		if ( g_ln_mst.exp_ind[0]!='0' && g_pub_tx.tx_date>g_ln_mst.mtr_date )
		{
			strcpy( ln_flag , "Z" );		
		}
	}

	return 0;		
}		
/**********************************************************************
* 函 数 名：  pub_ln_chk_clear
* 函数功能：  判断贷款是否已还清
* 作    者：  rob
* 完成时间：  2003年06月07日
*
* 参    数：
*     输  入：   p_ln_mst            贷款主文件结构
*		 		 p_ln_parm 	    	 贷款参数结构
*      
*     输  出:    clear_flg	     char(1)	 是否还清标志	0-还清 1-未还清
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_chk_clear(struct ln_mst_c p_ln_mst, struct ln_parm_c p_ln_parm, 
												char *clear_flg )
{
  	int ret=0;

  	struct ln_reg_c	ln_reg;	
  	struct ln_lo_c	ln_lo;
  	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	memset( &ln_lo,0x00,sizeof(struct ln_lo_c) );	
	
	/* 普通贷款 */
	if( p_ln_parm.ln_pay_type[0]!='3' && p_ln_parm.ln_pay_type[0]!='4' )
	{
		/* 查询贷款主文件中是否有欠款记录 */
		/* 若不计息 */
		if( p_ln_mst.intst_type[0]=='0' )
		{
			if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) )
			{
				strcpy( clear_flg ,"1" );	/*有欠款*/
				return 0;
			}else{
				strcpy( clear_flg ,"0" );   /*无欠款*/
				return 0;
			}
		}
		/* 若不计算复利 */
		if( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.intst_acm , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.in_lo_intst , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.out_lo_intst , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.cmpd_lo_intst , 0.00) )
			{
				strcpy( clear_flg ,"1" );   /*有欠款*/
				return 0;
			}else{
				strcpy( clear_flg ,"0" );   /*无欠款*/
				return 0;
			}
		}

		if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.intst_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.in_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.in_lo_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.out_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.out_lo_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.cmpd_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.cmpd_lo_acm , 0.00) )
		{	
			strcpy( clear_flg , "1" );	/* 有欠款 */
			return 0;
		}else{
			strcpy( clear_flg ,"0" );   /*无欠款*/
			return 0;
		}
		 	
		/* 查询贷款登记簿,查看是否有未还记录 *
		ret = Ln_reg_Sel ( g_pub_tx.reply , &ln_reg , "ac_id=%ld and \
					ac_seqn=%d and sts='0'", p_ln_mst.ac_id, p_ln_mst.ac_seqn);
		if ( ret && ret!=100 )
		{
			sprintf( acErrMsg, "执行Ln_reg_Sel错!ret=[%d]", ret);
			WRITEMSG					
			return 1;
		}else if( ret==100 ){
	 		sprintf( acErrMsg,"无欠息记录!!");
	 		WRITEMSG
			strcpy( clear_flg , "0" );	* 无欠款 *
 		}else if( !ret ){ 
			strcpy( clear_flg , "1" );	* 有欠款 *
			return 0;
		}
		*****/
	}
	
	/* 按揭贷款 */	
	if( p_ln_parm.ln_pay_type[0]=='3' || p_ln_parm.ln_pay_type[0]=='4' )
	{
		/* 查询贷款主文件中是否有欠款记录 */
		if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) )
		{
			strcpy( clear_flg , "1" );	/* 有欠款 */
			return 0;
		}
		/* 查询按揭欠款表,查看是否有未还记录 */						
		ret = Ln_lo_Sel( g_pub_tx.reply, &ln_lo, "ac_id=%ld and ac_seqn=%d \
					and pay_type='0'", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
		if ( ret && ret!=100 )
		{
			sprintf( acErrMsg, "执行Ln_lo_Sel错!ret=[%d]", ret);
			WRITEMSG					
			return 1;	
		}else if( ret==100 ){
	 		sprintf( acErrMsg,"无欠息记录!!");
	 		WRITEMSG
			strcpy( clear_flg , "0" );	/* 无欠款 */
 		}else if( !ret ){ 
			strcpy( clear_flg , "1" );	/* 有欠款 */
			return 0;
		}
	}		
			
	return 0;
}
/**********************************************************************
* 函 数 名：  pub_ln_check_mtr_date
* 函数功能：  判断贷款账户是否到期
* 作    者：  rob
* 完成时间：  2004年04月30日
*
* 参    数： 
*     输  入： 	      
*
*     输  出:      
*
*     返回值：1－正常最后一天
*		     2－逾期最后一天
*		     -1----不处理 
* 
* 修改历史：
*                   
********************************************************************/	
int pub_ln_check_mtr_date( )
{
	int ret=0;
	long date,dqr; 
	
	/* 判断展期标志 */
	if ( g_ln_mst.exp_ind[0]!='0' )
	{
		TRACE
		date=g_ln_mst.exp_mtr_date;
	}else{
		TRACE
		date=g_ln_mst.mtr_date;
	}
	TRACE
	/* 当前状态为正常 */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
	TRACE
		if( date==g_pub_tx.tx_date )
		{
		TRACE
			return 1;
		}else{
		TRACE
			return -1;
		}
	}
	/* 当前状态为逾期 90天内*/
	if ( g_ln_mst.ac_sts[0]=='2' )
	{
		TRACE
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -2;
		TRACE
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG		
		if( dqr==g_pub_tx.tx_date )
		{
		TRACE
			return 2;
		}else{
		TRACE
			return -1;
		}
	}
	/**当前状态为逾期 90天外**/
	if(g_ln_mst.ac_sts[0]=='5')
  {
  	TRACE
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -5;
		TRACE
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG		
		if( dqr==g_pub_tx.tx_date )
		{
		TRACE
			return 5;
		}else{
		TRACE
			return -1;
		}
  }
  /**eend**/
		TRACE
	return -1;		
}		

/**********************************************************************
* 函 数 名：  pub_ln_get_acc_hrt
* 函数功能：  输入数据编码取贷款科目号
* 作    者：  rob
* 完成时间：  2003年07月29日
*
* 参    数：
*     输  入：   p_ln_mst            贷款主文件结构
*		 p_ln_parm 	     贷款参数结构
*                data_code           数据编码
*      
*     输  出:    acc_hrt	     char(8)	 科目号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_get_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt )
{
	int ret =0;
	char ln_dc_code[5];
	
	struct dc_acc_rel_c	dc_acc_rel_tmp;
	struct com_item_c com_item_tmp;
	memset( &com_item_tmp  , 0x00, sizeof(struct com_item_c));
	memset( &dc_acc_rel_tmp , 0x00 , sizeof( struct dc_acc_rel_c ) );
	
	if( p_ln_mst.ac_sts[0]=='1' )/* 正常 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='2' )/* 逾期90天内 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.over_dc_code);	
	}
	if(p_ln_mst.ac_sts[0]=='5')/* 逾期90天外 */
	{
		TRACE
		strcpy(ln_dc_code,p_ln_parm.over90_dc_code);
		
	}
	if( p_ln_mst.ac_sts[0]=='3' )/* 呆滞 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.sla_dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='4' )/* 呆账 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.bad_dc_code);	
	}
	
	ret = Dc_acc_rel_Sel(g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='%s' ",ln_dc_code, data_code );
	if ( ret )
	{
		sprintf(acErrMsg,"读取会计代码对照表错误!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	ret = Com_item_Sel(g_pub_tx.reply,&com_item_tmp,"acc_hrt='%s'",dc_acc_rel_tmp.acc_hrt);
	if(ret)
	{
		sprintf(acErrMsg,"读取会计代码对照表错误!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	strcpy( acc_hrt , com_item_tmp.acc_hrt);
	
	return 0;
}	
/**********************************************************************
* 函 数 名：  pub_ln_Get_SectAcm
* 函数功能：  计算分段积数
* 作    者：  rob
* 完成时间：  2003年08月30日
*
* 参    数：  
*     输  入：ac_id   	  long 	  账户id   
*      	      ac_seqn     int	  账户序号
*             intst_type  char    利息种类（0存款1贷款本金 2贷款表内欠息3贷款表外欠息4贷款复利5透支）
*      
*     输  出: acm	  double  利息积数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_Get_SectAcm( long ac_id , int ac_seqn , 
	char intst_type[2] , double *acm )
{
	int ret;	
	
	struct com_sect_acm_c	com_sect_acm;	
	
	memset( &com_sect_acm, 0x00, sizeof(struct com_sect_acm_c) );
		
	/* 更新分段积数表中状态标志 */
	ret = Com_sect_acm_Dec_Upd( g_pub_tx.reply, 
		"ac_id=%ld and ac_seqn=%d and intst_type='%s' and sts='0' ", 
		ac_id , ac_seqn , intst_type);
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Com_sect_acm_Dec_Sel 游标出错[%d]",ret);
		WRITEMSG
		return 1;
	}	
	
	while(1)
	{
		ret = Com_sect_acm_Fet_Upd( &com_sect_acm, g_pub_tx.reply );
		if( ret==100 ) break;
   		if( ret )
 		{
	 		sprintf( acErrMsg,"fetch com_sect_acm error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		return 1;
 		}
					
		/* 算利息 */
		*acm += com_sect_acm.intst_acm;
	}
	
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"更新分段积数表中状态标志 PASS[%ld]",*acm);
	WRITEMSG
	*acm = pub_base_PubDround_1( *acm );
										 
	return 0;	
}
