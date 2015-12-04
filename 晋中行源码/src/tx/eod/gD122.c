/*************************************************
* 文 件 名: gD122.c
* 功能描述: 支行考核贷款表统计
*
* 作    者:xxxxx
* 完成日期:2003年12月21日
* 修改记录：   
* 日    期:
* 修 改 人:
* 修改内容:
* 
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		} 
#define EXTERN
#include "public.h"
#include "subbr_ln_c.h"
/**JYW#include "com_branch_c.h" **/
#include "cif_mger_rel_c.h"
#include "trace_log_bk_c.h"

gD122()
{
 int ret,mb_flag,sb_flag,yb_flag,lcnt;
 long y,m,d,bz;
 int year,month,day;
 struct subbr_ln_c subbr_ln;
 struct com_branch_c g_com_branch;
 struct cif_mger_rel_c cif_mger_rel;
 struct trace_log_bk_c trace_log_bk;
 
 ret=mb_flag=sb_flag=yb_flag=lcnt=0;
 year=month=day=0;
 memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
 memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
 
 ret = sql_begin();
 if ( ret )
 {
	 sprintf( acErrMsg, "打开事务失败!!" );
	 WRITEMSG
	 goto ErrExit;
 }
 pub_base_sysinit();
 ret = pub_base_get_ymd(&y,&m,&d,g_pub_tx.tx_date,&bz);	
 if( ret )
 {
   sprintf(acErrMsg,"调用判断月末函数出错!![%d]",ret);
   WRITEMSG
   return ret;
 }
 
 if(bz==3) mb_flag=1;
 	else if(bz==5) sb_flag=1;
 		else if(bz==9) yb_flag=1;
 			
 pub_base_get_date(&year,&month,&day);
 ret=Ln_mst_Dec_Sel(g_pub_tx.reply,"1=1");
 if (ret)
 {
 	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
 	WRITEMSG
 	goto ErrExit;
 }			
 
 while(1)
 {
 	ret=Ln_mst_Fet_Sel(&g_ln_mst, g_pub_tx.reply );
 	if( ret==100 )
 	{
 		sprintf( acErrMsg,"无记录[%d]",ret);
 		WRITEMSG
 		break;
 	}else ERR_DEAL
 		
 	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_ln_mst.ac_id);	 
  if(ret==100)
	 {
	  sprintf(acErrMsg,"该账号不存在,请检查!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"L161");
    goto ErrExit;		
	 }else ERR_DEAL	
	
	memset(&subbr_ln,0x00,sizeof(struct subbr_ln_c));
	ret=Subbr_ln_Sel(g_pub_tx.reply,&subbr_ln,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn); 	
	if(ret==100)
	{
	 strcpy(subbr_ln.opn_br_no,g_ln_mst.opn_br_no);
   ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_ln_mst.opn_br_no);	
   strcpy(subbr_ln.up_br_no,g_com_branch.up_br_no);
   subbr_ln.ex_year=year;
   subbr_ln.ex_month=month;
   subbr_ln.ac_id=g_mdm_ac_rel.ac_id;
   subbr_ln.ac_seqn=g_mdm_ac_rel.ac_seqn;
   strcpy(subbr_ln.acc_sts,g_ln_mst.ac_sts);
   subbr_ln.monthb_bal=g_ln_mst.bal;
   subbr_ln.bal=g_ln_mst.bal;
   
   if(mb_flag==1)
   	{
   	 subbr_ln.mon_increment=0.00;
   	 subbr_ln.month_addbal=0.00;
   	}
   else {
   	 if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	  	    subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(31-day);
  	  	 else if((month==4)||(month==6)||(month==9)||(month==11))
  	  	 	         subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(30-day);
  	  	 	     else if(month==2)
  	  	 	     	  {
  	  	 	     	   if(pub_base_chk_leap(year))
  	  	 	     	  	    subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(28-day);
  	  	 	     	   else subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(29-day);
  	  	 	        }
  	 subbr_ln.month_addbal+=g_ln_mst.bal;
   	}
   
   if(sb_flag==1)
  	  	subbr_ln.season_addbal=0.00;
   else subbr_ln.season_addbal+=g_ln_mst.bal;
  	  
   if(yb_flag==1)
  	    subbr_ln.year_addbal=0.00;
   else subbr_ln.year_addbal+=g_ln_mst.bal;
   
   memset(&cif_mger_rel,0x00,sizeof(struct cif_mger_rel_c));
   ret=Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel,"cif_no=%ld",g_td_mst.cif_no); 
   strcpy(subbr_ln.client_mgr,cif_mger_rel.mang);
   
   /*
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G083' and add_ind='0'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G085' and add_ind='0' and svc_ind='3059'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G086' and add_ind='0' and svc_ind='3069' and sub_tx_code='Z086' and svc_ind!='3010'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3202' and add_ind='0' and svc_ind!='1001' and sub_tx_code ='L202'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));	
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3203' and add_ind='0' and svc_ind!='1001' and sub_tx_code ='L203' and svc_ind!='3010'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G088' and add_ind='0' and svc_ind not in ('3060','3063') and prdt_no[1]='3'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
   
   subbr_ln.accrual_tadd+=subbr_ln.true_accrual;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G083' and add_ind='1'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G085' and add_ind='0' and svc_ind='3059'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G086' and add_ind='0' and svc_ind='3069' and sub_tx_code='Z086' and svc_ind!='3010'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;	
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3202' and add_ind='1' and sub_tx_code='L202'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
   
   memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));	
   ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G088' and add_ind='1' and svc_ind not in ('3060','3063') and prdt_no[1]='3'",\
                        g_ln_mst.ac_id,g_ln_mst.ac_seqn);
   if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;	
   */
   subbr_ln.true_accrual=0.00;
   subbr_ln.accrual_tadd=0.00;
   subbr_ln.jaccrual=0.00;
   lcnt++;
   ret=Subbr_ln_Ins(subbr_ln,g_pub_tx.reply);
   if(ret)
   {
    sprintf(acErrMsg,"登记支行考核贷款异常![%s]",g_pub_tx.reply);
	  WRITEMSG
	  ERR_DEAL;
	 }
	 else if(ret==0)
	 	{
	 	 sprintf(acErrMsg,"登记支行考核贷款成功![%d]",lcnt);
	   WRITEMSG
	 	}	
	}else if(ret==0)
		{
			ret=Subbr_ln_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",subbr_ln.ac_id,subbr_ln.ac_seqn);
      if (ret)
      {
        sprintf(acErrMsg,"DECLARE subbr_ln游标异常![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"L224");
        goto ErrExit;
      }
      
      memset(&subbr_ln,0x00,sizeof(struct subbr_ln_c));  
      ret=Subbr_ln_Fet_Upd(&subbr_ln,g_pub_tx.reply);
      if (ret&&ret!=100)
      {
       sprintf(acErrMsg,"FETCH subbr_ln游标异常![%s]",g_pub_tx.reply);
       WRITEMSG
       strcpy(g_pub_tx.reply,"L225");
       goto ErrExit;
      }
      
      strcpy(subbr_ln.acc_sts,g_ln_mst.ac_sts);
      subbr_ln.bal=g_ln_mst.bal;
      if(mb_flag==1)
   	  {
   	   subbr_ln.monthb_bal=g_ln_mst.bal;
   	   subbr_ln.mon_increment=0.00;
   	   subbr_ln.month_addbal=0.00;
   	   subbr_ln.ex_month=month;
   	  }
      else {
     	 if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	    	    subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(31-day);
  	    	 else if((month==4)||(month==6)||(month==9)||(month==11))
  	    	 	         subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(30-day);
  	    	 	     else if(month==2)
  	    	 	     	  {
  	    	 	     	   if(pub_base_chk_leap(year))
  	    	 	     	  	    subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(28-day);
  	    	 	     	   else subbr_ln.mon_increment+=(g_ln_mst.bal-g_ln_mst.ys_bal)*(29-day);
  	    	 	        }
  	   subbr_ln.month_addbal+=g_ln_mst.bal;
      } 
      if(sb_flag==1)
  	       subbr_ln.season_addbal=0.00; 	   
      else subbr_ln.season_addbal+=g_ln_mst.bal;
  	     
      if(yb_flag==1)
  	  {
  	  	subbr_ln.year_addbal=0.00;
  	  	subbr_ln.ex_year=year;
  	  }
      else subbr_ln.year_addbal+=g_ln_mst.bal;
      /*
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G083' and add_ind='0'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G085' and add_ind='0' and svc_ind='3059'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G086' and add_ind='0' and svc_ind='3069' and sub_tx_code='Z086' and svc_ind!='3010'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3202' and add_ind='0' and svc_ind!='1001' and sub_tx_code ='L202'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));	
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3203' and add_ind='0' and svc_ind!='1001' and sub_tx_code ='L203' and svc_ind!='3010'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G088' and add_ind='0' and svc_ind not in ('3060','3063') and prdt_no[1]='3'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.true_accrual+=trace_log_bk.amt;	
      
      subbr_ln.accrual_tadd+=subbr_ln.true_accrual;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G083' and add_ind='1'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G085' and add_ind='0' and svc_ind='3059'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G086' and add_ind='0' and svc_ind='3069' and sub_tx_code='Z086' and svc_ind!='3010'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;	
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='3202' and add_ind='1' and sub_tx_code='L202'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;
      
      memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));	
      ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log_bk,"ac_id=%ld and ac_seqn=%d and sts='0' and tx_code='G088' and add_ind='1' and svc_ind not in ('3060','3063') and prdt_no[1]='3'",\
                           g_ln_mst.ac_id,g_ln_mst.ac_seqn);
      if(ret==0)	subbr_ln.jaccrual+=trace_log_bk.amt;	
      */  
	    ret=Subbr_ln_Upd_Upd(subbr_ln,g_pub_tx.reply); 
 	    if(ret)
      {
      sprintf(acErrMsg,"修改支行考核贷款异常![%s]",g_pub_tx.reply);
	    WRITEMSG
	    ERR_DEAL;
	    }
	}
	else ERR_DEAL
 }
Ln_mst_Clo_Sel();
Subbr_ln_Clo_Upd();		
OkExit:
    sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"支行考核贷款表统计成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	  sql_rollback();
    sprintf(acErrMsg,"支行考核贷款表统计失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return -1;
}
