/*************************************************************
* 文 件 名: gD702.c
* 功能描述: 人民银行反洗钱大额可疑数据报送(现金部分) 
*
* 作    者: xxxxx
* 完成日期: 2003年10月24日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"
#include "mas_hvcs_c.h"
#include "mas_hvcd_c.h"
#include "trace_log_bk_c.h"
#include "mdm_attr_c.h"
#include "mbfebrbkno_c.h"

struct mas_hvcd_c  g_mas_hvcd;
struct mas_hvcs_c  g_mas_hvcs;
struct trace_log_bk_c trace_log_bk;
struct mdm_attr_c mdm_attr;
struct mbfebrbkno_c mbfebrbkno;
gD702()
{
 int dt_flag,mdm_flag,ret;
 
 memset(&g_pub_tx, 0x00, sizeof(g_pub_tx));
 ret = sql_begin();
 if ( ret )
 {
	sprintf( acErrMsg, "打开事务失败!!" );
	WRITEMSG
	goto ErrExit;
 }
 
 pub_base_sysinit();
 
 ret= ;
 if(ret)
   {
    sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"D101");
    goto ErrExit;
   }  
  while(1)
  {
  	dt_flag=0;
  	mdm_flag=0;
  	ret=0;
  	
  	memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
  	ret=Trace_log_bk_Fet_Sel(&trace_log_bk,g_pub_tx.reply);
    if (ret==100)
        break;
    else ERR_DEAL
    
    memset(&mbfebrbkno,0x00,sizeof(struct mbfebrbkno_c));
    ret=Mbfebrbkno_Sel(g_pub_tx.reply,&mbfebrbkno,"brno='%s'",trace_log_bk.tx_br_no);
    
    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id='%ld' and ac_seqn='%d'",\
    		               trace_log_bk.ac_id,trace_log_bk.ac_seqn);
    if(ret==0) 
    	{
    		mdm_flag=1;
    		memset(&mdm_attr,0x00,sizeof(struct mdm_attr_c));
    		ret=Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr,"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
    	}
    else mdm_flag=2;
    
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));		
    ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id='%ld' and ac_seqn='%d'",\
                   trace_log_bk.ac_id,trace_log_bk.ac_seqn);
    if(ret==0) dt_flag=1;
    else {
    	    ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id='%ld' and ac_seqn='%d'",\
                         trace_log_bk.ac_id,trace_log_bk.ac_seqn);
          if(ret==0) dt_flag=2;
         }
    memset(&g_mas_hvcs,0x00,sizeof(struct mas_hvcs_c));
    memset(&g_mas_hvcd,0x00,sizeof(struct mas_hvcd_c));
    if(trace_log_bk.add_ind[0]=='1')
    { 
    	sprintf(g_mas_hvcs.trace_no,"%ld%ld%ld",trace_log_bk.tx_date,trace_log_bk.trace_no,trace_log_bk.trace_cnt);
    	strcpy(g_mas_hvcs.origrdficode,mbfebrbkno.bkno);
    	strcpy(g_mas_hvcs.origrdfictype,"1");
    	strcpy(g_mas_hvcs.payee_ac_no,trace_log_bk.ac_no);
    	strcpy(g_mas_hvcs.cur_no,"RMB");
    	sprintf(g_mas_hvcs.tx_amt,"%15.0lf",trace_log_bk.amt*100);
    	sprintf(g_mas_hvcs.recvdate,"%ld",trace_log_bk.tx_date);
    	strcpy(g_mas_hvcs.cash_source,trace_log_bk.brf);
    	strcpy(g_mas_hvcs.txcode,trace_log_bk.tx_code);
    	
    	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2'))
    		  strcpy(g_mas_hvcs.payer_id_type,"01");
    		else if(g_mdm_ac_rel.id_type[0]=='4')
    	       strcpy(g_mas_hvcs.payer_id_type,"02");
    	      else if(g_mdm_ac_rel.id_type[0]=='5')
    	      	    strcpy(g_mas_hvcs.payer_id_type,"03");
    	      	   else if(g_mdm_ac_rel.id_type[0]=='3')
    	      	   	    strcpy(g_mas_hvcs.payer_id_type,"04");
    	      	   	   else strcpy(g_mas_hvcs.payer_id_type,"05");
    	
    	strcpy(g_mas_hvcs.payee_id_type,g_mas_hvcs.payer_id_type);
    	if(mdm_flag==1)
    		 strcpy(g_mas_hvcs.payer_id_no,g_mdm_ac_rel.id_no);
    	strcpy(g_mas_hvcs.payee_id_no,g_mas_hvcs.payer_id_no);
     
     if(mdm_attr.mdm_type[0]=='1')
    		 strcpy(g_mas_hvcs.sttl_type,"91");
    		else if(mdm_attr.mdm_type[0]=='2')
    			    strcpy(g_mas_hvcs.sttl_type,"90"); 
    			   else if((mdm_attr.mdm_type[0]=='3')||(mdm_attr.mdm_type[0]=='4')||(mdm_attr.mdm_type[0]=='5'))
    			   	 strcpy(g_mas_hvcs.sttl_type,"92");
    			   	 else strcpy(g_mas_hvcs.sttl_type,"93");
    	
    	if(dt_flag==1)
    	{
    	 strcpy(g_mas_hvcs.payer_name,g_dd_mst.name);
       strcpy(g_mas_hvcs.payee_name,g_mas_hvcs.payer_name);
       if(g_dd_mst.cal_code[0]=='1')
            strcpy(g_mas_hvcs.payee_type,"02");
       else 
       	   {
       	   	strcpy(g_mas_hvcs.payee_type,"01");
       	   	if (mdm_flag==1)
       	   		{
       	   		 strcpy(g_mas_hvcs.payee_br_no,g_mdm_ac_rel.id_no);
       	   		 str_display(g_mas_hvcs.payee_br_no);
       	   	  }	
       	   }
      }else if(dt_flag==2)
      	{
      		strcpy(g_mas_hvcs.payer_name,g_td_mst.name);
          strcpy(g_mas_hvcs.payee_name,g_mas_hvcs.payer_name);
          if(g_td_mst.cal_code[0]=='1')
               strcpy(g_mas_hvcs.payee_type,"02");
          else 
          	{
          	 strcpy(g_mas_hvcs.payee_type,"01");
          	 if (mdm_flag==1)
       	   	   {
       	   	   	strcpy(g_mas_hvcs.payee_br_no,g_mdm_ac_rel.id_no);
       	   	   	str_display(g_mas_hvcs.payee_br_no);
       	   	   }
       	    }
      }else strcpy(g_mas_hvcs.payee_type,"02");
            
     mas_hvcs_check();
     ret=Mas_hvcs_Ins(g_mas_hvcs,g_pub_tx.reply);
      if(ret)
      {
       sprintf(acErrMsg,"登记人民币现金存入业务异常![%s]",g_pub_tx.reply);
	     WRITEMSG
	     ERR_DEAL;
	    }
	 }
else if(trace_log_bk.add_ind[0]=='0')
	   {
	   	sprintf(g_mas_hvcd.trace_no,"%ld%ld%ld",trace_log_bk.tx_date,trace_log_bk.trace_no,trace_log_bk.trace_cnt);
    	strcpy(g_mas_hvcd.payficcode,mbfebrbkno.bkno);
     	strcpy(g_mas_hvcd.payfictype,"1");
     	strcpy(g_mas_hvcd.payer_ac_no,trace_log_bk.ac_no);
     	strcpy(g_mas_hvcd.cur_no,"RMB");
     	sprintf(g_mas_hvcd.tx_amt,"%15.0lf",trace_log_bk.amt*100);
     	sprintf(g_mas_hvcd.paydate,"%ld",trace_log_bk.tx_date);
     	strcpy(g_mas_hvcd.use,trace_log_bk.brf);
     	strcpy(g_mas_hvcd.txcode,trace_log_bk.tx_code);
     	
     	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2'))
     		  strcpy(g_mas_hvcd.payer_id_type,"01");
     		else if(g_mdm_ac_rel.id_type[0]=='4')
     	       strcpy(g_mas_hvcd.payer_id_type,"02");
     	      else if(g_mdm_ac_rel.id_type[0]=='5')
     	      	    strcpy(g_mas_hvcd.payer_id_type,"03");
     	      	   else if(g_mdm_ac_rel.id_type[0]=='3')
     	      	   	    strcpy(g_mas_hvcd.payer_id_type,"04");
     	      	   	   else strcpy(g_mas_hvcd.payer_id_type,"05");
     	      	   	   	
     	strcpy(g_mas_hvcd.payee_id_type,g_mas_hvcd.payer_id_type);
     	if(mdm_flag==1)
     	  strcpy(g_mas_hvcd.payer_id_no,g_mdm_ac_rel.id_no);
     	strcpy(g_mas_hvcd.payee_id_no,g_mas_hvcd.payer_id_no);
       
      if(mdm_attr.mdm_type[0]=='1')
     		 strcpy(g_mas_hvcd.sttl_type,"02");
     		else if(mdm_attr.mdm_type[0]=='2')
     			    strcpy(g_mas_hvcd.sttl_type,"00"); 
     			   else if((mdm_attr.mdm_type[0]=='3')||(mdm_attr.mdm_type[0]=='4')||(mdm_attr.mdm_type[0]=='5'))
     			   	 strcpy(g_mas_hvcd.sttl_type,"01");
     			   	 else strcpy(g_mas_hvcd.sttl_type,"04");
     	
       if(dt_flag==1)
     	 {
     	  strcpy(g_mas_hvcd.payer_name,g_dd_mst.name);
        strcpy(g_mas_hvcd.payee_name,g_mas_hvcd.payer_name);
        if(g_dd_mst.cal_code[0]=='1')
             strcpy(g_mas_hvcd.payer_type,"02");
        else 
        	   {
        	   	strcpy(g_mas_hvcd.payer_type,"01");
        	   	if (mdm_flag==1)
        	   	  {
        	   	  	strcpy(g_mas_hvcd.payer_br_no,g_mdm_ac_rel.id_no);
        	   	  	str_display(g_mas_hvcd.payer_br_no);
        	   	  }
        	   }
       }else if(dt_flag==2)
       	{
       		strcpy(g_mas_hvcd.payer_name,g_td_mst.name);
          strcpy(g_mas_hvcd.payee_name,g_mas_hvcd.payer_name);
           if(g_td_mst.cal_code[0]=='1')
                strcpy(g_mas_hvcd.payer_type,"02");
           else 
        	   {
        	   	strcpy(g_mas_hvcd.payer_type,"01");
        	   	if (mdm_flag==1)
        	   	 {
        	   	 	strcpy(g_mas_hvcd.payer_br_no,g_mdm_ac_rel.id_no);
        	   	 	str_display(g_mas_hvcd.payer_br_no);
        	   	 }
        	   }
       		}else {
        	       strcpy(g_mas_hvcd.payee_name,g_mas_hvcd.payer_name); 
                 strcpy(g_mas_hvcd.payer_type,"02");
                }
     strcpy(g_mas_hvcd.payee_type,g_mas_hvcd.payer_type);
     mas_hvcd_check(); 			
     ret=Mas_hvcd_Ins(g_mas_hvcd,g_pub_tx.reply);
       if(ret)
       {
        sprintf(acErrMsg,"登记人民币现金支取业务异常![%s]",g_pub_tx.reply);
	      WRITEMSG
	      ERR_DEAL;
	     }
	   }
}
Trace_log_bk_Clo_Sel();
OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"登记反洗钱信息成功[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"登记反洗钱信息失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/*-----------监测mas_hvcs各字段的填写情况-----------*/
mas_hvcs_check()
{
	pub_base_strpack(g_mas_hvcs.payer_name);
	if(strlen(g_mas_hvcs.payer_name)==0)
		strcpy(g_mas_hvcs.payer_name,"00000000");
	pub_base_strpack(g_mas_hvcs.payer_id_no);
	if(strlen(g_mas_hvcs.payer_id_no)==0)
		strcpy(g_mas_hvcs.payer_id_no,"00000000");
	pub_base_strpack(g_mas_hvcs.origrdficode);
	if(strlen(g_mas_hvcs.origrdficode)==0)
		strcpy(g_mas_hvcs.origrdficode,"00000000");
	pub_base_strpack(g_mas_hvcs.payee_name);
	if(strlen(g_mas_hvcs.payee_name)==0)
		strcpy(g_mas_hvcs.payee_name,"00000000");
	pub_base_strpack(g_mas_hvcs.payee_ac_no);
	if(strlen(g_mas_hvcs.payee_ac_no)==0)
		strcpy(g_mas_hvcs.payee_ac_no,"00000000");
	pub_base_strpack(g_mas_hvcs.payee_br_no);
	if(strlen(g_mas_hvcs.payee_br_no)==0)
		strcpy(g_mas_hvcs.payee_br_no,"00000000");
	pub_base_strpack(g_mas_hvcs.payee_id_no);
	if(strlen(g_mas_hvcs.payee_id_no)==0)
		strcpy(g_mas_hvcs.payee_id_no,"00000000");
	pub_base_strpack(g_mas_hvcs.extend);
	if(strlen(g_mas_hvcs.extend)==0)
		strcpy(g_mas_hvcs.extend,"00000000");
}
/*-----------监测mas_hvcd各字段的填写情况-----------*/
mas_hvcd_check()
{
	pub_base_strpack(g_mas_hvcd.payer_name);
	if(strlen(g_mas_hvcd.payer_name)==0)
		strcpy(g_mas_hvcd.payer_name,"00000000");
  pub_base_strpack(g_mas_hvcd.payer_br_no);
	if(strlen(g_mas_hvcd.payer_br_no)==0)
		strcpy(g_mas_hvcd.payer_br_no,"00000000");
	pub_base_strpack(g_mas_hvcd.payer_id_no);
	if(strlen(g_mas_hvcd.payer_id_no)==0)
		strcpy(g_mas_hvcd.payer_id_no,"00000000");
	pub_base_strpack(g_mas_hvcd.payficcode);
	if(strlen(g_mas_hvcd.payficcode)==0)
		strcpy(g_mas_hvcd.payficcode,"00000000");
	pub_base_strpack(g_mas_hvcd.payee_name);
	if(strlen(g_mas_hvcd.payee_name)==0)
		strcpy(g_mas_hvcd.payee_name,"00000000");
	pub_base_strpack(g_mas_hvcd.payer_ac_no);
	if(strlen(g_mas_hvcd.payer_ac_no)==0)
		strcpy(g_mas_hvcd.payer_ac_no,"00000000");
	pub_base_strpack(g_mas_hvcd.payee_id_no);
	if(strlen(g_mas_hvcd.payee_id_no)==0)
		strcpy(g_mas_hvcd.payee_id_no,"00000000");
	pub_base_strpack(g_mas_hvcd.extend);
	if(strlen(g_mas_hvcd.extend)==0)
		strcpy(g_mas_hvcd.extend,"00000000");
}
/*-----------字符串替换-----------*/
str_display(char *str)
{
 int i;
 pub_base_strpack(str);
 for(i=0;i<strlen(str);i++)
 if(((*(str+i)<0x30)||(*(str+i)>0x39))&&((*(str+i)<0x41)||(*(str+i)>0x5A))&&((*(str+i)<0x61)||(*(str+i)>0x7A)))
    break;
 *(str+i)='\0';	
}
