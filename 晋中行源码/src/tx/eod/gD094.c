/*************************************************
* 文 件 名: gD094.c
* 功能描述：生成普通贷款24个月还款状态
*
* 作    者: lance
* 完成日期: 2003年10月31日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "ln_paystatus_c.h"



gD094()
{
	int ret=0;
	int sum_i=0;
	int str_flag=0;
	
	long today=0;
	long date_24=0;
	long date_24_fro=0;
	long date_24_bak=0;
	long date_24_yy=0;
	long date_24_mm=0;
	long fir_spay_date=0;
	long date_bz=0;
	long lo_days=0;
	long yy=0;
	long mm=0;
	long dd=0;
	
			
	struct ln_reg_c	S_ln_reg;
	struct ln_lo_c	S_ln_lo;	
	struct ln_paystatus_c	S_ln_paystatus;	
	
	memset(&S_ln_reg,0x00,sizeof(struct ln_reg_c));	
	memset(&S_ln_lo,0x00,sizeof(struct ln_lo_c));	
	
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

   	/* 初始化全局变量 */
   	pub_base_sysinit();		
	/** 每月生成数据 **/
	if( g_pub_tx.tx_date%100!=1 )
	goto OkExit;
   	
	ret=sql_execute("delete from ln_paystatus");
	if(ret) goto ErrExit;
	
	pub_base_deadlineD(g_pub_tx.tx_date,-1,&today);
	sprintf(acErrMsg,"日期 today=[%ld]",today);
	WRITEMSG
		
	ret = Eod_ln_mst_Dec_Sel(g_pub_tx.reply," repay_type not in('3','4')  ");
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	while(1)
	{
		ret = Eod_ln_mst_Fet_Sel(&g_ln_mst, g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		sprintf(acErrMsg,"第 [%d]户 ac_id=[%ld]",sum_i,g_ln_mst.ac_id);
		WRITEMSG
		sum_i++;
		memset(&S_ln_paystatus,0x00,sizeof(struct ln_paystatus_c));	
			
		/* 确定24个月前日期 */
		date_24=(today/10000-2)*10000+today%10000;
		date_24_yy=date_24/10000;
		date_24_mm=date_24%10000/100;	
			
		sprintf( acErrMsg,"24个月前的日期为[%ld]",date_24);
		WRITEMSG	
		
		
		for( str_flag=0; str_flag<24; str_flag++ )
		{
			/*确定每个月的起始终止日期*/
			date_24_mm++;
			if( date_24_mm==13 )
			{
				date_24_yy++;
				date_24_mm=1;
			}
			date_24_fro=date_24_yy*10000+date_24_mm*100+01;
			if( date_24_mm==1||date_24_mm==3||date_24_mm==5||date_24_mm==7||date_24_mm==8||date_24_mm==10||date_24_mm==12 )
			{
				date_24_bak=date_24_yy*10000+date_24_mm*100+31;
			}else{
				date_24_bak=date_24_yy*10000+date_24_mm*100+30;
			}
			if( date_24_mm==2 )
			{
				if( pub_base_chk_leap(date_24_yy) )
				{
					date_24_bak=date_24_yy*10000+date_24_mm*100+28;	
				}else{
					date_24_bak=date_24_yy*10000+date_24_mm*100+29;	
				}
			}			
			if( g_ln_mst.opn_date> date_24_bak )/*尚未开户*/
			{
				S_ln_paystatus.pay_sts[str_flag]='/';
			}else if( g_ln_mst.opn_date>= date_24_fro && g_ln_mst.opn_date<= date_24_bak ){	/*本月开户*/
				if( g_ln_mst.mtr_date>= date_24_fro && g_ln_mst.mtr_date<= date_24_bak )
				{
					/*对于本月开户本月到期的贷款 */
					S_ln_paystatus.pay_sts[str_flag]='1';
				}else{
					S_ln_paystatus.pay_sts[str_flag]='*';
				}
			}else if( g_ln_mst.opn_date< date_24_fro ){ /*已开户*/	
				
				/*查询贷款是否结清*/
				if( pub_base_CompDblVal( g_ln_mst.bal,0.00)<=0 && pub_base_CompDblVal( g_ln_mst.in_lo_intst,0.00)<=0 &&
				    pub_base_CompDblVal( g_ln_mst.out_lo_intst,0.00)<=0 && pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst,0.00)<=0 &&
				    pub_base_CompDblVal( g_ln_mst.intst_acm,0.00)<=0 && pub_base_CompDblVal( g_ln_mst.in_lo_acm,0.00)<=0 &&
				    pub_base_CompDblVal( g_ln_mst.out_lo_acm,0.00)<=0 && pub_base_CompDblVal( g_ln_mst.cmpd_lo_acm,0.00)<=0 )
				{
					S_ln_paystatus.pay_sts[str_flag]='C';
					S_ln_paystatus.clear_sts[0]='1';	
				}else{
					/*对非按月结息户的处理*/
					ret=pub_base_get_ymd(&yy,&mm,&dd,date_24_bak,&date_bz);
					if(ret) goto ErrExit;
					/*sprintf(acErrMsg,"还款类型[%s] 标志=[%ld]",g_ln_mst.intst_type,date_bz);
					WRITEMSG					
				*/	
					if( g_ln_mst.intst_type[0]=='0' || g_ln_mst.intst_type[0]=='1' )
					{
				/*		TRACE*/
						S_ln_paystatus.pay_sts[str_flag]='*';
					}else if( g_ln_mst.intst_type[0]=='5' && date_bz!=10 ){
						TRACE
						S_ln_paystatus.pay_sts[str_flag]='*';
					}else if( g_ln_mst.intst_type[0]=='4' && ( date_bz!=10 && date_bz!=6 && date_bz!=8 ) ){
						TRACE
						S_ln_paystatus.pay_sts[str_flag]='*';
					}else{			
						TRACE
						/*查看最早欠款日期*/
						ret=sql_min_long("ln_reg","reg_date_end",&fir_spay_date,"ac_id=%ld and ac_seqn=%d and sts='0'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
						if(ret)	goto ErrExit;
						
						if( fir_spay_date> date_24_bak )/*尚未欠款*/
						{
							S_ln_paystatus.pay_sts[str_flag]='N';
						}else{/*有欠款*/	
							/*计算欠款天数*/
							if( fir_spay_date==0 )
							{
								S_ln_paystatus.pay_sts[str_flag]='N';
							}else{	
								lo_days=0;
								lo_days=pub_base_CalTrueDays( fir_spay_date, date_24_bak );
								sprintf(acErrMsg,"欠息天数[%ld]天 fir_spay_date[%ld]||date_24_bak[%ld]",lo_days,fir_spay_date,date_24_bak);
								WRITEMSG
								if( lo_days<=30 )
								{
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='1';
								}else if( lo_days<=60 ){
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='2';
								}else if( lo_days<=90 ){
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='3';
								}else if( lo_days<=120 ){
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='4';
								}else if( lo_days<=150 ){
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='5';
								}else if( lo_days<=180 ){																																
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='6';
								}else{
									TRACE
									S_ln_paystatus.pay_sts[str_flag]='7';	
								}
							}					
						}		    	
					}
				}
			}
		}

		S_ln_paystatus.pay_sts[24]='\0';
		S_ln_paystatus.ac_id=g_ln_mst.ac_id;
		S_ln_paystatus.ac_seqn=g_ln_mst.ac_seqn;
		if(S_ln_paystatus.clear_sts[0]!='1')
		{
			S_ln_paystatus.clear_sts[0]='0';
		}
                ret = Ln_paystatus_Ins(S_ln_paystatus, g_pub_tx.reply);
                if (ret)
                {
                    sprintf(acErrMsg,"Insert 24月贷款还款情况表出错! ret=[%d]",
                            ret);
                    WRITEMSG
                    goto ErrExit;
                }
	}		
	Eod_ln_mst_Clo_Sel();				

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"----->生成24月贷款还款情况表成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
    
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"----->生成24月贷款还款情况表失败！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
