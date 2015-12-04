/*************************************************
* 文 件 名: spL318.c
* 功能描述：   
*              贷款本金核销
*
* 作    者: lance 
* 完成日期：   2003年09月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_verify_reg_c.h"
#include "trace_log_c.h"

struct ln_verify_reg_c ln_verify_reg;

spL318()
{

 int ret=0;
 char tmp_sts[2];
 

  pub_base_sysinit();
  
  memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
  memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
  memset (&ln_verify_reg,0x00,sizeof(struct ln_verify_reg_c));
  memset (&tmp_sts, 0x00, sizeof(tmp_sts));
 
  get_zd_data("0670",tmp_sts);
  get_zd_data("1081",g_pub_tx.ac_no);		/* 帐号 */
  get_zd_int("1082",&g_pub_tx.ac_seqn);		/* 账户序号 */
  get_zd_double("1084",&g_pub_tx.tx_amt1);	/* 核销金额 */
  get_zd_double("1085",&g_pub_tx.tx_amt2);
  get_zd_double("1086",&g_pub_tx.tx_amt3);
  get_zd_double("1087",&g_pub_tx.tx_amt4);
  
  pub_base_strpack(g_pub_tx.ac_no);
  
  ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		          &g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
 if( ret ) 
 {
   sprintf(acErrMsg,"gh------读取贷款户信息出错！" );
   WRITEMSG
   strcpy(g_pub_tx.reply,"W018");
   goto ErrExit;
 }	
 
 switch(tmp_sts[0])
	{
	case '0':	/*1-增加*/
         strcpy(ln_verify_reg.name,g_ln_mst.name);
         strcpy(ln_verify_reg.ac_no,g_pub_tx.ac_no);
         strcpy(ln_verify_reg.verify_brno,g_pub_tx.tx_br_no);
         strcpy(ln_verify_reg.verify_tel,g_pub_tx.tel);
         strcpy(ln_verify_reg.opn_brno,g_ln_mst.opn_br_no);
         
         ln_verify_reg.mtr_date=g_ln_mst.mtr_date;
         ln_verify_reg.opn_date=g_ln_mst.opn_date;
         ln_verify_reg.verify_date=g_pub_tx.tx_date;
       	
	 ln_verify_reg.ac_id=g_ln_mst.ac_id;
	 ln_verify_reg.ac_seqn=g_ln_mst.ac_seqn;
		 
         ln_verify_reg.bal_amt=g_pub_tx.tx_amt1;
         ln_verify_reg.in_lo_amt=g_pub_tx.tx_amt2;
         ln_verify_reg.out_lo_amt=g_pub_tx.tx_amt3;
         ln_verify_reg.cmpd_lo_amtt=g_pub_tx.tx_amt4;
         
         ln_verify_reg.rate=g_ln_mst.rate;
         ln_verify_reg.over_rate=g_ln_mst.over_rate;
         ln_verify_reg.fine_rate=g_ln_mst.fine_rate;
         ln_verify_reg.verify_trace_no=g_pub_tx.trace_no;

	 vtcp_log("name[%s] ac_no[%s] brno[%s]",ln_verify_reg.name,ln_verify_reg.ac_no,ln_verify_reg.verify_brno);
	 vtcp_log("opn_brno[%s]",ln_verify_reg.opn_brno);
	 vtcp_log("mtr_date[%d]",ln_verify_reg.mtr_date);
	 vtcp_log("opn_date[%d]",ln_verify_reg.opn_date);
	 vtcp_log("verify_date[%d]",ln_verify_reg.verify_date);
	 vtcp_log("ac_id[%d]",ln_verify_reg.ac_id);
	 vtcp_log("ac_seqn[%d]",ln_verify_reg.ac_seqn);
	 vtcp_log("bal_amt[%f] [%f]",ln_verify_reg.bal_amt,g_pub_tx.tx_amt1);
	 vtcp_log("in_lo_amt[%f] [%f]",ln_verify_reg.in_lo_amt,g_pub_tx.tx_amt2);
	 vtcp_log("out_lo_amt[%f] [%f]",ln_verify_reg.out_lo_amt,g_pub_tx.tx_amt3);
	 vtcp_log("cmpd_lo_amt[%f] [%f]",ln_verify_reg.cmpd_lo_amtt,g_pub_tx.tx_amt4);
	 vtcp_log("rate[%f] over_rate[%f] fine_rate[%f] verify_trace_no[%d]",ln_verify_reg.rate,ln_verify_reg.over_rate,ln_verify_reg.fine_rate,g_pub_tx.trace_no);
 
         ret=Ln_verify_reg_Ins(ln_verify_reg,g_pub_tx.reply);
         if(ret)
         {
          sprintf(acErrMsg,"登记核销登记簿异常![%s]",
		g_pub_tx.reply);
	  WRITEMSG
	  strcpy(g_pub_tx.reply,"L235");
	  ERR_DEAL;
	 }
        strcpy( g_pub_tx.brf,"贷款核销登记簿登记");
        break;
	case '1':          /*2-修改*/
	  
	    ret=Ln_verify_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and ac_seqn='%d'",\
				      g_pub_tx.ac_no, g_pub_tx.ac_seqn);
	    if (ret)
	     {
		sprintf(acErrMsg,"DECLARE ln_verify_reg游标异常![%s]",
			g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			ERR_DEAL;
	     }
           
          
	   ret=Ln_verify_reg_Fet_Upd(&ln_verify_reg,g_pub_tx.reply);
	   if (ret&&ret!=100)
	   {
	    sprintf(acErrMsg,"gh------>FETCH ln_verify_reg游标异常![%s]",
	            g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"D102");
	    ERR_DEAL;
	   }else if(ret==100){
	    sprintf(acErrMsg,"无记录![%s]",g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"L236");
	    ERR_DEAL;
	  }	   	
	   
         strcpy(ln_verify_reg.verify_brno,g_pub_tx.opn_br_no);
         strcpy(ln_verify_reg.verify_tel,g_pub_tx.tel);
         ln_verify_reg.verify_date=g_pub_tx.tx_date;
       	 ln_verify_reg.verify_trace_no=g_pub_tx.trace_no;
       	 
	 ln_verify_reg.bal_amt=g_pub_tx.tx_amt1;
         ln_verify_reg.in_lo_amt=g_pub_tx.tx_amt2;
         ln_verify_reg.out_lo_amt=g_pub_tx.tx_amt3;
         ln_verify_reg.cmpd_lo_amtt=g_pub_tx.tx_amt4;
         
         ret=Ln_verify_reg_Upd_Upd(ln_verify_reg,g_pub_tx.reply);
	  if (ret)
	   {
	    sprintf(acErrMsg,"gh------>UPDATE Ln_verify_reg游标异常![%s]",
		   g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"D102");
	    ERR_DEAL;
	   }
	 
	 Ln_verify_reg_Clo_Upd( );						
	 strcpy( g_pub_tx.brf,"贷款核销登记簿修改");
		break;	
	default: 
		break;
	}

	/* 登记交易流水 */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款核销维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款核销维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
