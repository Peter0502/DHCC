/*************************************************
* 文 件 名: gD121.c
* 功能描述：支行考核存款表统计
*
* 作    者:xxx
* 完成日期:2003年12月21日
* 修改记录：   
* 日    期:
* 修 改 人: JYW 表结构变化修改
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
#include "subbr_deposit_c.h"
/**JYW#include "com_branch_c.h" **/
#include "tel_save_inf_c.h"
#include "cif_mger_rel_c.h"

gD121()
{
 int ret,mb_flag,sb_flag,yb_flag,dcnt,tcnt;
 long y,m,d,bz;
 int year,month,day;
 struct subbr_deposit_c subbr_deposit;
 struct com_branch_c g_com_branch;
 struct tel_save_inf_c tel_save_inf;
 struct cif_mger_rel_c cif_mger_rel;
 
 ret=mb_flag=sb_flag=yb_flag=dcnt=tcnt=0;
 year=month=day=0;
 memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
 memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
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
 ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"1=1");
 if (ret)
 {
 	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
 	WRITEMSG
 	goto ErrExit;
 }	
 
 while(1)
 {
 	ret=Dd_mst_Fet_Sel(&g_dd_mst, g_pub_tx.reply );
 	if( ret==100 )
 	{
 		sprintf( acErrMsg,"无记录[%d]",ret);
 		WRITEMSG
 		break;
 	}else ERR_DEAL
 
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&subbr_deposit,0x00,sizeof(struct subbr_deposit_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_dd_mst.ac_id);	 
  if(ret==100)
	 {
	  sprintf(acErrMsg,"该账号不存在,请检查!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"L161");
    goto ErrExit;		
	 }else ERR_DEAL
	  	  	
/****JYW modi 060815 change to below
  ret=Subbr_deposit_Sel(g_pub_tx.reply,&subbr_deposit,"acc_no='%s'",g_mdm_ac_rel.ac_no);		
******end JYW *****/
  ret=Subbr_deposit_Sel(g_pub_tx.reply,&subbr_deposit,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);		
  if(ret==100)
  	{
  	  strcpy(subbr_deposit.opn_br_no,g_dd_mst.opn_br_no);
  	  ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_dd_mst.opn_br_no);	
  	  strcpy(subbr_deposit.up_br_no,g_com_branch.up_br_no);
  	  subbr_deposit.ex_year=year;
  	  subbr_deposit.ex_month=month;
	  /*JYW Change begin*/
  	  subbr_deposit.ac_id=g_mdm_ac_rel.ac_id;
  	  subbr_deposit.ac_seqn=g_mdm_ac_rel.ac_seqn;
	  /*JYW Change End**/
  	  subbr_deposit.monthb_bal=g_dd_mst.bal;
  	  subbr_deposit.bal=g_dd_mst.bal;
  	  if(mb_flag==1) 
  	  {
  	   subbr_deposit.mon_increment=0.00; 
  	   subbr_deposit.month_addbal=0.00;
  	  }
  	  else{
  	  	 if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	  	    subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(31-day);
  	  	 else if((month==4)||(month==6)||(month==9)||(month==11))
  	  	 	      subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(30-day);
  	  	 	     else if(month==2)
  	  	 	     	  {
  	  	 	     	   if(pub_base_chk_leap(year))
  	  	 	     	  	 subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(28-day);
  	  	 	     	   else subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(29-day);
  	  	 	        }
  	  	subbr_deposit.month_addbal+=g_dd_mst.bal;
  	  	}
  	  if(sb_flag==1)
  	  	subbr_deposit.season_addbal=0.00;
  	  else subbr_deposit.season_addbal+=g_dd_mst.bal;
  	  
  	  if(yb_flag==1)
  	  	 subbr_deposit.year_addbal=0.00;
  	  else subbr_deposit.year_addbal+=g_dd_mst.bal;
     
     memset(&tel_save_inf,0x00,sizeof(struct tel_save_inf_c));
     memset(&cif_mger_rel,0x00,sizeof(struct cif_mger_rel_c));
     ret=Tel_save_inf_Sel(g_pub_tx.reply,&tel_save_inf,"ac_no='%s' and ac_seqn=%d",g_mdm_ac_rel.ac_no,g_mdm_ac_rel.ac_seqn);
  	 ret=Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel,"cif_no=%ld",g_dd_mst.cif_no); 
  	 strcpy(subbr_deposit.draw_deposit,tel_save_inf.tel);
  	 strcpy(subbr_deposit.client_mgr,cif_mger_rel.mang);
  	 dcnt++;
  	 ret=Subbr_deposit_Ins(subbr_deposit,g_pub_tx.reply);
     if(ret)
     {
      sprintf(acErrMsg,"登记支行考核存款表异常![%s]",g_pub_tx.reply);
	    WRITEMSG
	    ERR_DEAL;
	   }
	   else if(ret==0)
	   	{
	   	 sprintf(acErrMsg,"登记支行考核存款表成功![%d]",dcnt);
	     WRITEMSG
	   	}
  	}
 else if(ret==0)
 	    {
 	    	ret=Subbr_deposit_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",subbr_deposit.ac_id,subbr_deposit.ac_seqn);
        if (ret)
        {
          sprintf(acErrMsg,"DECLARE subbr_deposit游标异常![%s]",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"L224");
          goto ErrExit;
        }
        memset(&subbr_deposit,0x00,sizeof(struct subbr_deposit_c));  
        ret=Subbr_deposit_Fet_Upd(&subbr_deposit,g_pub_tx.reply);
        if (ret&&ret!=100)
        {
         sprintf(acErrMsg,"FETCH subbr_deposit游标异常![%s]",g_pub_tx.reply);
         WRITEMSG
         strcpy(g_pub_tx.reply,"L225");
         goto ErrExit;
        }
        
 	    	subbr_deposit.bal=g_dd_mst.bal;
 	    	if(mb_flag==1)
 	    	{
 	    		subbr_deposit.monthb_bal=g_dd_mst.bal;
 	    		subbr_deposit.mon_increment=0.00;
 	    		subbr_deposit.month_addbal=0.00;
 	    		subbr_deposit.ex_month=month;
 	    	}
 	    	else {
 	    		if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	  	    subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(31-day);
  	  	  else if	((month==4)||(month==6)||(month==9)||(month==11))
  	  	 	      subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(30-day);
  	  	 	     else if(month==2)
  	  	 	     	  {
  	  	 	     	   if(pub_base_chk_leap(year))
  	  	 	     	  	 subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(28-day);
  	  	 	     	   else subbr_deposit.mon_increment+=(g_dd_mst.bal-g_dd_mst.ys_bal)*(29-day);
  	  	 	        }
 	    		subbr_deposit.month_addbal+=g_dd_mst.bal;
 	    	}
 	    	if(sb_flag==1)
 	    		subbr_deposit.season_addbal=0.00;
 	    	else subbr_deposit.season_addbal+=g_dd_mst.bal;
 	    	
 	    	if(yb_flag==1)
 	    		{
 	    			subbr_deposit.year_addbal=0.00;
 	    			subbr_deposit.ex_year=year;
 	    		}
 	      else subbr_deposit.year_addbal+=g_dd_mst.bal;
 	     
 	     ret=Subbr_deposit_Upd_Upd(subbr_deposit,g_pub_tx.reply);
 	     if(ret)
       {
       sprintf(acErrMsg,"修改支行考核存款表异常![%s]",g_pub_tx.reply);
	     WRITEMSG
	     ERR_DEAL;
	     }
 	   }
 	else ERR_DEAL
 }	
 
 ret=Td_mst_Dec_Sel(g_pub_tx.reply,"1=1");
 if (ret)
 {
 	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
 	WRITEMSG
 	goto ErrExit;
 }	
 
 while(1)
 {
 	ret=Td_mst_Fet_Sel(&g_td_mst, g_pub_tx.reply );
 	if( ret==100 )
 	{
 		sprintf( acErrMsg,"无记录[%d]",ret);
 		WRITEMSG
 		break;
 	}else ERR_DEAL
  
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&subbr_deposit,0x00,sizeof(struct subbr_deposit_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_td_mst.ac_id);	 
  if(ret==100)
	 {
	  sprintf(acErrMsg,"该账号不存在,请检查!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"L161");
    goto ErrExit;		
	 }else ERR_DEAL
 	
 	ret=Subbr_deposit_Sel(g_pub_tx.reply,&subbr_deposit,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);		
  if(ret==100)
  	{
  		strcpy(subbr_deposit.opn_br_no,g_td_mst.opn_br_no);
  	  ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_td_mst.opn_br_no);	
  	  strcpy(subbr_deposit.up_br_no,g_com_branch.up_br_no);
  	  subbr_deposit.ex_year=year;
  	  subbr_deposit.ex_month=month;
  	  subbr_deposit.ac_id=g_mdm_ac_rel.ac_id;
  	  subbr_deposit.ac_seqn=g_mdm_ac_rel.ac_seqn;
  	  subbr_deposit.monthb_bal=g_td_mst.bal;
  	  subbr_deposit.bal=g_td_mst.bal;
  	  if(mb_flag==1) 
  	  {
  	   subbr_deposit.mon_increment=0.00; 
  	   subbr_deposit.month_addbal=0.00;
  	  }
  	  else{
  	  	 if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	  	    subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(31-day);
  	  	 else if((month==4)||(month==6)||(month==9)||(month==11))
  	  	 	      subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(30-day);
  	  	 	     else if(month==2)
  	  	 	     	  {
  	  	 	     	   if(pub_base_chk_leap(year))
  	  	 	     	  	 subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(28-day);
  	  	 	     	   else subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(29-day);
  	  	 	        }
  	  	subbr_deposit.month_addbal+=g_td_mst.bal;
  	  	}
  	  if(sb_flag==1)
  	  	subbr_deposit.season_addbal=0.00;
  	  else subbr_deposit.season_addbal+=g_td_mst.bal;
  	  
  	  if(yb_flag==1)
  	  	 subbr_deposit.year_addbal=0.00;
  	  else subbr_deposit.year_addbal+=g_td_mst.bal;
     
     memset(&tel_save_inf,0x00,sizeof(struct tel_save_inf_c));
     memset(&cif_mger_rel,0x00,sizeof(struct cif_mger_rel_c));
     ret=Tel_save_inf_Sel(g_pub_tx.reply,&tel_save_inf,"ac_no='%s' and ac_seqn=%d",g_mdm_ac_rel.ac_no,g_mdm_ac_rel.ac_seqn);
  	 ret=Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel,"cif_no=%ld",g_td_mst.cif_no); 
  	 strcpy(subbr_deposit.draw_deposit,tel_save_inf.tel);
  	 strcpy(subbr_deposit.client_mgr,cif_mger_rel.mang);
  	 tcnt++;
  	 ret=Subbr_deposit_Ins(subbr_deposit,g_pub_tx.reply);
     if(ret)
     {
      sprintf(acErrMsg,"登记支行考核定期异常![%s]",g_pub_tx.reply);
	    WRITEMSG
	    ERR_DEAL;
	   }
	   else if(ret==0)
	   	{
	   	 sprintf(acErrMsg,"登记支行考核定期成功![%d]",tcnt);
	     WRITEMSG
	   	}
  	}
 else if(ret==0)
 	    {
 	    	ret=Subbr_deposit_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",subbr_deposit.ac_id,subbr_deposit.ac_seqn);
        if (ret)
        {
          sprintf(acErrMsg,"DECLARE subbr_deposit游标异常![%s]",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"L224");
          goto ErrExit;
        }
        memset(&subbr_deposit,0x00,sizeof(struct subbr_deposit_c));  
        ret=Subbr_deposit_Fet_Upd(&subbr_deposit,g_pub_tx.reply);
        if (ret&&ret!=100)
        {
         sprintf(acErrMsg,"FETCH subbr_deposit游标异常![%s]",g_pub_tx.reply);
         WRITEMSG
         strcpy(g_pub_tx.reply,"L225");
         goto ErrExit;
        }
        subbr_deposit.bal=g_td_mst.bal;
 	    	if(mb_flag==1)
 	    	{
 	    		subbr_deposit.monthb_bal=g_td_mst.bal;
 	    		subbr_deposit.mon_increment=0.00;
 	    		subbr_deposit.month_addbal=0.00;
 	    		subbr_deposit.ex_month=month;
 	    	}
 	    	else {
 	    		if((month==1)||(month==3)||(month==5)||(month==7)||(month==8)||(month==10)||(month==12))
  	  	    subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(31-day);
  	  	  else if	((month==4)||(month==6)||(month==9)||(month==11))
  	  	 	      subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(30-day);
  	  	 	     else if(month==2)
  	  	 	     	  {
  	  	 	     	   if(pub_base_chk_leap(year))
  	  	 	     	  	 subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(28-day);
  	  	 	     	   else subbr_deposit.mon_increment+=(g_td_mst.bal-g_td_mst.ys_bal)*(29-day);
  	  	 	        }
 	    		subbr_deposit.month_addbal+=g_td_mst.bal;
 	    	}
 	    	if(sb_flag==1)
 	    		subbr_deposit.season_addbal=0.00;
 	    	else subbr_deposit.season_addbal+=g_td_mst.bal;
 	    	
 	    	if(yb_flag==1)
 	    		{
 	    			subbr_deposit.year_addbal=0.00;
 	    			subbr_deposit.ex_year=year;
 	    		}
 	      else subbr_deposit.year_addbal+=g_td_mst.bal;
 	     
 	     ret=Subbr_deposit_Upd_Upd(subbr_deposit,g_pub_tx.reply); 
 	     if(ret)
       {
       sprintf(acErrMsg,"修改支行考核定期异常![%s]",g_pub_tx.reply);
	     WRITEMSG
	     ERR_DEAL;
	     }
 	   }
 	else ERR_DEAL	
 }
Subbr_deposit_Clo_Upd();
Dd_mst_Clo_Sel();
Td_mst_Clo_Sel();
OkExit:
    sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"支行考核存款表统计成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sql_rollback();
    sprintf(acErrMsg,"支行考核存款表统计失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return -1;
}
