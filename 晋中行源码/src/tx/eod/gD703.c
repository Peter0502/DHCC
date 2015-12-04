/*************************************************************
* 文 件 名: gD703.c
* 功能描述: 反洗钱监测分析系统 (大额转帐部分) 
*
* 作    者: xxxx
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
#include "mas_hvpt_c.h"
#include "mas_pbto_c.h"
#include "mas_pato_c.h"
#include "trace_log_bk_c.h"
#include "mbfebrbkno_c.h"
#include "mbfebillopnreg_c.h"
#include "mbfeghvsendlist_c.h"
#include "mbfeghvrecvlist_c.h"

struct mas_hvpt_c  g_mas_hvpt;
struct mas_pbto_c  g_mas_pbto;
struct mas_pato_c  g_mas_pato;
struct trace_log_bk_c trace_log_bk;
struct mbfebillopnreg_c mbfebillopnreg;
struct mbfeghvsendlist_c mbfeghvsendlist;
struct mbfeghvrecvlist_c mbfeghvrecvlist;
struct mbfebrbkno_c mbfebrbkno;
 
gD703()
{
 int pa_flag,pb_flag,ret,dt_flag,mdm_flag;
 long qry_date=0;
 ret=dt_flag=mdm_flag=0;
 memset(&g_pub_tx, 0x00, sizeof(g_pub_tx));
 ret = sql_begin();
 if ( ret )
 {
	sprintf( acErrMsg, "打开事务失败!!" );
	WRITEMSG
	goto ErrExit;
 }
 
 pub_base_sysinit();
 pub_base_deadlineD(g_pub_tx.tx_date, -1 ,&qry_date );
/***JYW大额部分先不考虑***/  
#if 0
 ret=Mbfebillopnreg_Dec_Sel(g_pub_tx.reply ,"amount>=200000.00 and signdate='%ld'",qry_date);
 if(ret)
   {
    sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"D101");
    goto ErrExit;
   }  
 
 while(1)
 {
 	memset(&mbfebillopnreg,0x00,sizeof(struct mbfebillopnreg_c));
  ret=Mbfebillopnreg_Fet_Sel(&mbfebillopnreg,g_pub_tx.reply);
  if (ret==100)
       break;
  else ERR_DEAL
  pb_flag=0;
    
  memset(&g_mas_hvpt,0x00,sizeof(struct mas_hvpt_c));
  sprintf(g_mas_hvpt.trace_no,"%ld%s",mbfebillopnreg.tx_date,mbfebillopnreg.billcode);
  strcpy(g_mas_hvpt.cur_no,"RMB");
  sprintf(g_mas_hvpt.tx_amt,"%15.0lf",mbfebillopnreg.amount*100);
  pub_base_strpack(mbfebillopnreg.brf);
  if(strlen(mbfebillopnreg.brf)==0)
  	strcpy(g_mas_hvpt.use,"未登");
  else strncpy(g_mas_hvpt.use,mbfebillopnreg.brf,20);
  strcpy(g_mas_hvpt.txcode,"4140");
  strcpy(g_mas_hvpt.payfictype,"1");
  strcpy(g_mas_hvpt.origrdfictype,"1"); 
  
  strcpy(g_mas_hvpt.payer_name,mbfebillopnreg.payername);
  strcpy(g_mas_hvpt.payer_ac_no,mbfebillopnreg.payeracc);
  strcpy(g_mas_hvpt.payficcode,mbfebillopnreg.payeropenaccbkcode);
  strcpy(g_mas_hvpt.payee_name,mbfebillopnreg.recipientname);
  strcpy(g_mas_hvpt.payee_ac_no,mbfebillopnreg.recipientacc);
  strcpy(g_mas_hvpt.origrdficode,mbfebillopnreg.cashbillbkno);
  strcpy(g_mas_hvpt.notecode,mbfebillopnreg.billcode);
  
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                     g_mas_hvpt.payer_ac_no);
  if(ret==0)
  {
  	if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
    	 {
    	 	strcpy(g_mas_hvpt.payer_br_no,g_mdm_ac_rel.id_no);
    	 	str_display(g_mas_hvpt.payer_br_no);
    	 }
    else pb_flag=1;
  }
  	
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                     g_mas_hvpt.payee_ac_no);
  if(ret==0)
  {
  	if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
    	{
    		strcpy(g_mas_hvpt.payee_br_no,g_mdm_ac_rel.id_no);
    		str_display(g_mas_hvpt.payee_br_no);
    	}
    else pb_flag=1;
  }
  
  strcpy(g_mas_hvpt.sttl_type,"50");
  sprintf(g_mas_hvpt.paydate,"%ld",mbfebillopnreg.signdate);
  sprintf(g_mas_hvpt.recvdate,"%ld",mbfebillopnreg.signdate); 
  
  mas_hvpt_check();
  ret=Mas_hvpt_Ins(g_mas_hvpt,g_pub_tx.reply);
  if(ret)
  {
  sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
  WRITEMSG
  ERR_DEAL;
  }  
 
if(pb_flag==1)
 {
  memset(&g_mas_pbto,0x00,sizeof(struct mas_pbto_c));
  sprintf(g_mas_pbto.trace_no,"B%s",g_mas_hvpt.trace_no);
  strcpy(g_mas_pbto.cur_no,"RMB");
  strcpy(g_mas_pbto.tx_amt,g_mas_hvpt.tx_amt);
  strcpy(g_mas_pbto.use,g_mas_hvpt.use);
  strcpy(g_mas_pbto.txcode,g_mas_hvpt.txcode);
  strcpy(g_mas_pbto.payfictype,"1");
  strcpy(g_mas_pbto.origrdfictype,"1");
  strcpy(g_mas_pbto.payficcode,g_mas_hvpt.payficcode);
  strcpy(g_mas_pbto.payer_name,g_mas_hvpt.payer_name);
  strcpy(g_mas_pbto.payer_ac_no,g_mas_hvpt.payer_ac_no);
  strcpy(g_mas_pbto.payer_br_no,g_mas_hvpt.payer_br_no);
  strcpy(g_mas_pbto.origrdficode,g_mas_hvpt.origrdficode);
  strcpy(g_mas_pbto.payee_name,g_mas_hvpt.payee_name);
  strcpy(g_mas_pbto.payee_ac_no,g_mas_hvpt.payee_ac_no);
  strcpy(g_mas_pbto.payee_br_no,g_mas_hvpt.payee_br_no);
  strcpy(g_mas_pbto.notecode,g_mas_hvpt.notecode);
  strcpy(g_mas_pbto.paydate,g_mas_hvpt.paydate);
  strcpy(g_mas_pbto.recvdate,g_mas_hvpt.recvdate);
     
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
  	                  g_mas_pbto.payee_ac_no);
  if((ret==0)&&(strcmp(g_mas_pbto.payee_ac_no,"00000000")))
  	{
  	 if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
  	 	  {
  	 	    strcpy(g_mas_pbto.payee_type,"02");
  	 	    if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2'))
  			    strcpy(g_mas_pbto.per_id_type,"01");
  			  else if(g_mdm_ac_rel.id_type[0]=='4')
  		       strcpy(g_mas_pbto.per_id_type,"02");
  		      else if(g_mdm_ac_rel.id_type[0]=='5')
  		      	    strcpy(g_mas_pbto.per_id_type,"03");
  		      	   else if(g_mdm_ac_rel.id_type[0]=='3')
  		      	   	    strcpy(g_mas_pbto.per_id_type,"04");
  		      	   	   else strcpy(g_mas_pbto.per_id_type,"05");
  	    strcpy(g_mas_pbto.per_id_no,g_mdm_ac_rel.id_no);
  	 	  }
  	 else	strcpy(g_mas_pbto.payee_type,"01");
  	}
 else 
  	{
  	 strcpy(g_mas_pbto.payee_type,"02");
  	 strcpy(g_mas_pbto.per_id_type,"05");
  	}
 strcpy(g_mas_pato.sttl_type,"20");
 mas_pbto_check();
 ret=Mas_pbto_Ins(g_mas_pbto,g_pub_tx.reply);
 if(ret)
  {
  sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
  WRITEMSG
  ERR_DEAL;
  }
 }
}

ret=Mbfeghvsendlist_Dec_Sel(g_pub_tx.reply ,"amount>=200000.00 and consigndate='%ld'",qry_date);
if(ret)
  {
   sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
   WRITEMSG
   strcpy(g_pub_tx.reply,"D101");
   goto ErrExit;
  }  
while(1)
{
 pa_flag=0;
 memset(&mbfeghvsendlist,0x00,sizeof(struct mbfeghvsendlist_c));
 ret=Mbfeghvsendlist_Fet_Sel(&mbfeghvsendlist,g_pub_tx.reply);	
 if (ret==100)
        break;
 else ERR_DEAL
 
 memset(&g_mas_hvpt,0x00,sizeof(struct mas_hvpt_c));
 sprintf(g_mas_hvpt.trace_no,"%ld%s",mbfeghvsendlist.consigndate,mbfeghvsendlist.txssno);
 strcpy(g_mas_hvpt.cur_no,"RMB");
 sprintf(g_mas_hvpt.tx_amt,"%15.0lf",mbfeghvsendlist.amount*100);
 pub_base_strpack(mbfeghvsendlist.strinfo);
 if(strlen(mbfeghvsendlist.strinfo)==0)
 	 strcpy(g_mas_hvpt.use,"未登");
 else strncpy(g_mas_hvpt.use,mbfeghvsendlist.strinfo,20);
 strcpy(g_mas_hvpt.txcode,"4110");
 strcpy(g_mas_hvpt.payfictype,"1");
 strcpy(g_mas_hvpt.origrdfictype,"1");
 
 strcpy(g_mas_hvpt.payer_name,mbfeghvsendlist.payername);
 strcpy(g_mas_hvpt.payer_ac_no,mbfeghvsendlist.payeracc);
 strcpy(g_mas_hvpt.payficcode,mbfeghvsendlist.odficode);
 strcpy(g_mas_hvpt.payee_name,mbfeghvsendlist.recipientname);
 strcpy(g_mas_hvpt.payee_ac_no,mbfeghvsendlist.recipientacc);
 strcpy(g_mas_hvpt.origrdficode,mbfeghvsendlist.rdficode);
 strcpy(g_mas_hvpt.notecode,mbfeghvsendlist.warrantno); 
 
 memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                    g_mas_hvpt.payer_ac_no);
 if(ret==0)
 {
 	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
 		pa_flag=1;
 	else if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
   	{
   	 strcpy(g_mas_hvpt.payer_br_no,g_mdm_ac_rel.id_no);
   	 str_display(g_mas_hvpt.payer_br_no);
   	}
 }     
       
 memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                    g_mas_hvpt.payee_ac_no);
 if(ret==0)
 {     
 	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
 		pa_flag=1;
 	else if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
   	{
   		strcpy(g_mas_hvpt.payee_br_no,g_mdm_ac_rel.id_no);
   		str_display(g_mas_hvpt.payee_br_no);
   	}
 }
 	
 sprintf(g_mas_hvpt.paydate,"%ld",mbfeghvsendlist.consigndate); 
 strcpy(g_mas_hvpt.recvdate,g_mas_hvpt.paydate); 
 	
 if (!strcmp(mbfeghvsendlist.cmtno,"101"))
 	   strcpy(g_mas_hvpt.sttl_type,"55");
 else if (!strcmp(mbfeghvsendlist.cmtno,"102"))
 	     strcpy(g_mas_hvpt.sttl_type,"54");
 	  else strcpy(g_mas_hvpt.sttl_type,"53");

mas_hvpt_check();
ret=Mas_hvpt_Ins(g_mas_hvpt,g_pub_tx.reply);
if(ret)
 {
  sprintf(acErrMsg,"登记mas_hvpt异常![%s]",g_pub_tx.reply);
  WRITEMSG
  ERR_DEAL;
 }
 
 if(pa_flag==1)
  {
   mas_pato_ins(); 
   mas_pato_check();
   ret=Mas_pato_Ins(g_mas_pato,g_pub_tx.reply);
   if(ret)
   {
    sprintf(acErrMsg,"登记mas_pato异常![%s]",g_pub_tx.reply);
	  WRITEMSG
	  ERR_DEAL;
	 }
	} 
}


ret=Mbfeghvrecvlist_Dec_Sel(g_pub_tx.reply ,"amount>=200000.00 and consigndate='%ld'",qry_date);
if(ret)
  {
   sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
   WRITEMSG
   strcpy(g_pub_tx.reply,"D101");
   goto ErrExit;
  }  
while(1)
{
 memset(&mbfeghvrecvlist,0x00,sizeof(struct mbfeghvrecvlist_c));
 ret=Mbfeghvrecvlist_Fet_Sel(&mbfeghvrecvlist,g_pub_tx.reply);	
 if (ret==100)
     break;
 else ERR_DEAL
 pa_flag=0;
 
 memset(&g_mas_hvpt,0x00,sizeof(struct mas_hvpt_c));
 sprintf(g_mas_hvpt.trace_no,"%ld%s",mbfeghvrecvlist.consigndate,mbfeghvrecvlist.txssno);
 strcpy(g_mas_hvpt.cur_no,"RMB");
 sprintf(g_mas_hvpt.tx_amt,"%15.0lf",mbfeghvrecvlist.amount*100);
 
 pub_base_strpack(mbfeghvrecvlist.strinfo);
 if(strlen(mbfeghvrecvlist.strinfo)==0)
 	 strcpy(g_mas_hvpt.use,"未登");
 else strncpy(g_mas_hvpt.use,mbfeghvrecvlist.strinfo,20);
 strcpy(g_mas_hvpt.txcode,"7901");
 strcpy(g_mas_hvpt.payfictype,"1");
 strcpy(g_mas_hvpt.origrdfictype,"1");
 
 strcpy(g_mas_hvpt.payer_name,mbfeghvrecvlist.payername);
 strcpy(g_mas_hvpt.payer_ac_no,mbfeghvrecvlist.payeracc);
 strcpy(g_mas_hvpt.payficcode,mbfeghvrecvlist.odficode);
 strcpy(g_mas_hvpt.payee_name,mbfeghvrecvlist.recipientname);
 strcpy(g_mas_hvpt.payee_ac_no,mbfeghvrecvlist.recipientacc);
 strcpy(g_mas_hvpt.origrdficode,mbfeghvrecvlist.rdficode);
 strcpy(g_mas_hvpt.notecode,mbfeghvrecvlist.warrantno); 
 sprintf(g_mas_hvpt.paydate,"%ld",mbfeghvrecvlist.consigndate); 
 strcpy(g_mas_hvpt.recvdate,g_mas_hvpt.paydate);
 
 memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
               g_mas_hvpt.payer_ac_no);
 if(ret==0)
 {
 	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
 		pa_flag=1;
 else if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
   	{
   		strcpy(g_mas_hvpt.payer_br_no,g_mdm_ac_rel.id_no);
   		str_display(g_mas_hvpt.payer_br_no);
   	}
 }
 
 memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                    g_mas_hvpt.payee_ac_no);
 if(ret==0)
 {
 	if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
 		pa_flag=1;
 else if((g_mdm_ac_rel.id_type[0]=='8')||(g_mdm_ac_rel.id_type[0]=='9')||(g_mdm_ac_rel.id_type[0]=='A')||(g_mdm_ac_rel.id_type[0]=='B')||(g_mdm_ac_rel.id_type[0]=='C'))
   {
   	strcpy(g_mas_hvpt.payee_br_no,g_mdm_ac_rel.id_no);
   	str_display(g_mas_hvpt.payee_br_no);
   }
 }    
    
 if (!strcmp(mbfeghvsendlist.cmtno,"101"))
 	   strcpy(g_mas_hvpt.sttl_type,"55");
 else if (!strcmp(mbfeghvsendlist.cmtno,"102"))
 	     strcpy(g_mas_hvpt.sttl_type,"54");
 	  else strcpy(g_mas_hvpt.sttl_type,"53");
 
 mas_hvpt_check();
 ret=Mas_hvpt_Ins(g_mas_hvpt,g_pub_tx.reply);
 if(ret)
 {
  sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
  WRITEMSG
  ERR_DEAL;
 }
  
 if(pa_flag==1)
  {
   mas_pato_ins(); 
   mas_pato_check();
   ret=Mas_pato_Ins(g_mas_pato,g_pub_tx.reply);
   if(ret)
   {
    sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
	  WRITEMSG
	  ERR_DEAL;
	 }
	} 
}
Mbfebillopnreg_Clo_Sel();
Mbfeghvsendlist_Clo_Sel();
Mbfeghvrecvlist_Clo_Sel();
#endif
ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply ,"amt>=200000.00 and prdt_no[1] in ('1','2') and ct_ind='2'");
if(ret)
   {
    sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"D101");
    goto ErrExit;
   }
while(1)
{
	ret=pa_flag=0;
	memset(&trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
	ret=Trace_log_bk_Fet_Sel(&trace_log_bk,g_pub_tx.reply);
  if (ret==100)
      break;
  else ERR_DEAL
 
  memset(&mbfebillopnreg,0x00,sizeof(struct mbfebillopnreg_c));
  ret=Mbfebillopnreg_Sel(g_pub_tx.reply,&mbfebillopnreg,"signdate='%ld' and payeracc='%s' and amount='%lf'",\
                         trace_log_bk.tx_date,trace_log_bk.ac_no,trace_log_bk.amt);
  if (ret==0) continue;
  
  memset(&mbfeghvsendlist,0x00,sizeof(struct mbfeghvsendlist_c));
  ret=Mbfeghvsendlist_Sel(g_pub_tx.reply,&mbfeghvsendlist,"consigndate='%ld' and payeracc='%s' and amount='%lf'",\
                         trace_log_bk.tx_date,trace_log_bk.ac_no,trace_log_bk.amt);	
  if (ret==0) continue;
  
  
  memset(&mbfeghvrecvlist,0x00,sizeof(struct mbfeghvrecvlist_c));
  ret=Mbfeghvrecvlist_Sel(g_pub_tx.reply,&mbfeghvrecvlist,"consigndate='%ld' and recipientacc='%s' and amount='%lf'",\
                         trace_log_bk.tx_date,trace_log_bk.ac_no,trace_log_bk.amt);	 
  if (ret==0)	continue;
  
  memset(&mbfebrbkno,0x00,sizeof(struct mbfebrbkno_c));
  ret=Mbfebrbkno_Sel(g_pub_tx.reply,&mbfebrbkno,"brno='%s'",trace_log_bk.tx_br_no);
  
  memset(&g_mas_hvpt,0x00,sizeof(struct mas_hvpt_c));
  sprintf(g_mas_hvpt.trace_no,"%ld%ld%ld",trace_log_bk.tx_date,trace_log_bk.trace_no,trace_log_bk.trace_cnt);
  strcpy(g_mas_hvpt.cur_no,"RMB");
  sprintf(g_mas_hvpt.tx_amt,"%15.0lf",trace_log_bk.amt*100);
  strcpy(g_mas_hvpt.use,trace_log_bk.brf);
  strcpy(g_mas_hvpt.txcode,trace_log_bk.tx_code);
  strcpy(g_mas_hvpt.payfictype,"1");
  strcpy(g_mas_hvpt.origrdfictype,"1");
  
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
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id='%ld' and ac_seqn='%d'",\
             trace_log_bk.ac_id,trace_log_bk.ac_seqn);
  if(ret==0) 
  	   mdm_flag=1;
  else mdm_flag=2;   
  	  
  if(trace_log_bk.add_ind[0]=='0')
   {
   	 strcpy(g_mas_hvpt.payee_ac_no,trace_log_bk.ac_no);
   	 strcpy(g_mas_hvpt.origrdficode,mbfebrbkno.bkno);
   	 sprintf(g_mas_hvpt.recvdate,"%ld",trace_log_bk.tx_date);
   	 strcpy(g_mas_hvpt.paydate,g_mas_hvpt.recvdate);
   	 if(dt_flag==1)
      {
       strcpy(g_mas_hvpt.payee_name,g_dd_mst.name);
       if((g_dd_mst.cal_code[0]!='1')&&(strcmp(g_dd_mst.cal_code,""))&&(mdm_flag==1))
   	  	{
   	  		strcpy(g_mas_hvpt.payee_br_no,g_mdm_ac_rel.id_no);
   	  		str_display(g_mas_hvpt.payee_br_no);
   	  	}
   	  	else pa_flag=1;
   	  }
     else if(dt_flag==2)
   	   {  
   	     strcpy(g_mas_hvpt.payee_name,g_td_mst.name);
   	     if((g_td_mst.cal_code[0]!='1')&&(strcmp(g_td_mst.cal_code,""))&&(mdm_flag==1))
   	  	   {
   	  	   	strcpy(g_mas_hvpt.payee_br_no,g_mdm_ac_rel.id_no);
   	  	   	str_display(g_mas_hvpt.payee_br_no);
   	  	   }
   	  	 else pa_flag=1;
   	  }
   strcpy(g_mas_hvpt.notecode,trace_log_bk.note_no);
   }
   else if (trace_log_bk.add_ind[0]=='1')
   	{
   	 strcpy(g_mas_hvpt.payer_ac_no,trace_log_bk.ac_no);
   	 strcpy(g_mas_hvpt.payficcode,mbfebrbkno.bkno);
   	 sprintf(g_mas_hvpt.paydate,"%ld",trace_log_bk.tx_date);
   	 strcpy(g_mas_hvpt.recvdate,g_mas_hvpt.paydate);
   	 if(dt_flag==1)
        {
         strcpy(g_mas_hvpt.payer_name,g_dd_mst.name);
         if((g_dd_mst.cal_code[0]!='1')&&(strcmp(g_dd_mst.cal_code,""))&&(mdm_flag==1))
      	 	   {
      	 	   	strcpy(g_mas_hvpt.payer_br_no,g_mdm_ac_rel.id_no);
      	 	   	str_display(g_mas_hvpt.payer_br_no);
      	 	   }
      	 else pa_flag=1;
        }
      else if(dt_flag==2)
      	  {  
      	    strcpy(g_mas_hvpt.payer_name,g_td_mst.name);
      	    if((g_td_mst.cal_code[0]!='1')&&(strcmp(g_td_mst.cal_code,""))&&(mdm_flag==1))
      	 	     {
      	 	     	strcpy(g_mas_hvpt.payer_br_no,g_mdm_ac_rel.id_no);
      	 	     	str_display(g_mas_hvpt.payer_br_no);
      	 	     }
      	 	  else pa_flag=1;
      	  }
       strcpy(g_mas_hvpt.notecode,trace_log_bk.note_no);
       }
   if ((!strcmp(trace_log_bk.tx_code,"2206"))&&((!strcmp(trace_log_bk.note_type,"001"))||(!strcmp(trace_log_bk.note_type,"002"))))
          strcpy(g_mas_hvpt.sttl_type,"52");
     else if (!strcmp(trace_log_bk.note_type,"020"))
        	      strcpy(g_mas_hvpt.sttl_type,"59");
     	    else strcpy(g_mas_hvpt.sttl_type,"61");
 
 mas_hvpt_check();
 ret=Mas_hvpt_Ins(g_mas_hvpt,g_pub_tx.reply);
 if(ret)
  {
  sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
	 WRITEMSG
	 ERR_DEAL;
	}
 if(pa_flag==1)
  {
   mas_pato_ins(); 
   mas_pato_check();
   ret=Mas_pato_Ins(g_mas_pato,g_pub_tx.reply);
   if(ret)
   {
    sprintf(acErrMsg,"登记人民币转帐业务异常![%s]",g_pub_tx.reply);
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
mas_pato_ins()
{
	int ret=0;
	memset(&g_mas_pato,0x00,sizeof(struct mas_pato_c));
  sprintf(g_mas_pato.trace_no,"A%s",g_mas_hvpt.trace_no);
  strcpy(g_mas_pato.cur_no,"RMB");
  sprintf(g_mas_pato.tx_amt,g_mas_hvpt.tx_amt);
  strcpy(g_mas_pato.use,g_mas_hvpt.use);
  strcpy(g_mas_pato.txcode,g_mas_hvpt.txcode);
  strcpy(g_mas_pato.payfictype,"1");
  strcpy(g_mas_pato.origrdfictype,"1");
  strcpy(g_mas_pato.payficcode,g_mas_hvpt.payficcode);
  strcpy(g_mas_pato.payer_name,g_mas_hvpt.payer_name);
  strcpy(g_mas_pato.payer_ac_no,g_mas_hvpt.payer_ac_no);
  strcpy(g_mas_pato.payer_br_no,g_mas_hvpt.payer_br_no);
  strcpy(g_mas_pato.origrdficode,g_mas_hvpt.origrdficode);
  strcpy(g_mas_pato.payee_name,g_mas_hvpt.payee_name);
  strcpy(g_mas_pato.payee_ac_no,g_mas_hvpt.payee_ac_no);
  strcpy(g_mas_pato.payee_br_no,g_mas_hvpt.payee_br_no);
  strcpy(g_mas_pato.notecode,g_mas_hvpt.notecode);
  strcpy(g_mas_pato.paydate,g_mas_hvpt.paydate);
  strcpy(g_mas_pato.recvdate,g_mas_hvpt.recvdate);
   
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                     g_mas_pato.payer_ac_no);
  if((ret==0)&&(strcmp(g_mas_pato.payer_ac_no,"00000000")))
  	{
  	 if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
  	 	  {
  	 	    strcpy(g_mas_pato.payer_type,"02");
  	 	    if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2'))
  			  strcpy(g_mas_pato.payer_id_type,"01");
  			  else if(g_mdm_ac_rel.id_type[0]=='4')
  		       strcpy(g_mas_pato.payer_id_type,"02");
  		      else if(g_mdm_ac_rel.id_type[0]=='5')
  		      	    strcpy(g_mas_pato.payer_id_type,"03");
  		      	   else if(g_mdm_ac_rel.id_type[0]=='3')
  		      	   	    strcpy(g_mas_pato.payer_id_type,"04");
  		      	   	   else strcpy(g_mas_pato.payer_id_type,"05");
  	    strcpy(g_mas_pato.payer_id_no,g_mdm_ac_rel.id_no);
  	 	  }
  	 else	
  	 	 {
  	 	  strcpy(g_mas_pato.payer_type,"02");
  	 	  strcpy(g_mas_pato.payer_id_type,"05");
  	 	 }
  	}
  	else 
  	{
  	 strcpy(g_mas_pato.payer_type,"02");
  	 strcpy(g_mas_pato.payer_id_type,"05");
  	 }
  
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));	
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",\
                    g_mas_pato.payee_ac_no);
  if((ret==0)&&(strcmp(g_mas_pato.payee_ac_no,"00000000")))
  	{
  	 if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2')||(g_mdm_ac_rel.id_type[0]=='3')||(g_mdm_ac_rel.id_type[0]=='4')||(g_mdm_ac_rel.id_type[0]=='5')||(g_mdm_ac_rel.id_type[0]=='6')||(g_mdm_ac_rel.id_type[0]=='7'))
  	 	  {
  	 	    strcpy(g_mas_pato.payee_type,"02");
  	 	    if((g_mdm_ac_rel.id_type[0]=='1')||(g_mdm_ac_rel.id_type[0]=='2'))
  			  strcpy(g_mas_pato.payee_id_type,"01");
  			else if(g_mdm_ac_rel.id_type[0]=='4')
  		       strcpy(g_mas_pato.payee_id_type,"02");
  		      else if(g_mdm_ac_rel.id_type[0]=='5')
  		      	    strcpy(g_mas_pato.payee_id_type,"03");
  		      	   else if(g_mdm_ac_rel.id_type[0]=='3')
  		      	   	    strcpy(g_mas_pato.payee_id_type,"04");
  		      	   	   else strcpy(g_mas_pato.payee_id_type,"05");
  	    strcpy(g_mas_pato.payee_id_no,g_mdm_ac_rel.id_no);
  	 	  }
  	 else	
  	 	 {
  	 	  strcpy(g_mas_pato.payee_type,"02");
  	 	  strcpy(g_mas_pato.payee_id_type,"05");
  	 	 }
  	}
  	else 
  	{
  	 strcpy(g_mas_pato.payee_type,"02");
  	 strcpy(g_mas_pato.payee_id_type,"05");
  	}
   	if(!strcmp(g_mas_hvpt.sttl_type,"52"))
   		strcpy(g_mas_pato.sttl_type,"30");
   	else if(!strcmp(g_mas_hvpt.sttl_type,"53"))
   		  strcpy(g_mas_pato.sttl_type,"31");
   	 else if(!strcmp(g_mas_hvpt.sttl_type,"54"))
   		     strcpy(g_mas_pato.sttl_type,"32");
   	  else if(!strcmp(g_mas_hvpt.sttl_type,"59"))
   		     strcpy(g_mas_pato.sttl_type,"35");
   		  else strcpy(g_mas_pato.sttl_type,"37");
}
mas_hvpt_check()
{
 pub_base_strpack(g_mas_hvpt.payer_name);
	if(strlen(g_mas_hvpt.payer_name)==0)
		strcpy(g_mas_hvpt.payer_name,"00000000");
 pub_base_strpack(g_mas_hvpt.payficcode);
	if(strlen(g_mas_hvpt.payficcode)==0)
		strcpy(g_mas_hvpt.payficcode,"00000000");	
 pub_base_strpack(g_mas_hvpt.payer_ac_no);
	if(strlen(g_mas_hvpt.payer_ac_no)==0)
		strcpy(g_mas_hvpt.payer_ac_no,"00000000");
 pub_base_strpack(g_mas_hvpt.payer_br_no);
	if(strlen(g_mas_hvpt.payer_br_no)==0)
		strcpy(g_mas_hvpt.payer_br_no,"00000000");
 pub_base_strpack(g_mas_hvpt.origrdficode);
	if(strlen(g_mas_hvpt.origrdficode)==0)
		strcpy(g_mas_hvpt.origrdficode,"00000000");
 pub_base_strpack(g_mas_hvpt.payee_name);
	if(strlen(g_mas_hvpt.payee_name)==0)
		strcpy(g_mas_hvpt.payee_name,"00000000");
 pub_base_strpack(g_mas_hvpt.payee_ac_no);
	if(strlen(g_mas_hvpt.payee_ac_no)==0)
		strcpy(g_mas_hvpt.payee_ac_no,"00000000");
 pub_base_strpack(g_mas_hvpt.payee_br_no);
	if(strlen(g_mas_hvpt.payee_br_no)==0)
		strcpy(g_mas_hvpt.payee_br_no,"00000000");
 pub_base_strpack(g_mas_hvpt.notecode);
	if(strlen(g_mas_hvpt.notecode)==0)
		strcpy(g_mas_hvpt.notecode,"00000000");
 pub_base_strpack(g_mas_hvpt.extend);
	if(strlen(g_mas_hvpt.extend)==0)
		strcpy(g_mas_hvpt.extend,"00000000");
}
mas_pato_check()
{
	pub_base_strpack(g_mas_pato.payer_id_no);
	if(strlen(g_mas_pato.payer_id_no)==0)
		strcpy(g_mas_pato.payer_id_no,"00000000");
	pub_base_strpack(g_mas_pato.payee_id_no);
	if(strlen(g_mas_pato.payee_id_no)==0)
		strcpy(g_mas_pato.payee_id_no,"00000000"); 
	pub_base_strpack(g_mas_pato.extend);
	if(strlen(g_mas_pato.extend)==0)
		strcpy(g_mas_pato.extend,"00000000");  	 	 	 
}
mas_pbto_check()
{
 pub_base_strpack(g_mas_pbto.per_id_no);
	if(strlen(g_mas_pbto.per_id_no)==0)
		strcpy(g_mas_pbto.per_id_no,"00000000");
 pub_base_strpack(g_mas_pbto.extend);
	if(strlen(g_mas_pbto.extend)==0)
		strcpy(g_mas_pbto.extend,"00000000");
}
str_display(char *str)
{
 int i; 
 pub_base_strpack(str);
 for(i=0;i<strlen(str);i++)
 if(((*(str+i)<0x30)||(*(str+i)>0x39))&&((*(str+i)<0x41)||(*(str+i)>0x5A))&&((*(str+i)<0x61)||(*(str+i)>0x7A)))
   break;
 *(str+i)='\0';
}
