/***************************************************************
* 文 件 名:  spD313.c
* 功能描述:  遂心存签约	
* 作    者:  nx

* 完成日期：    
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"

int spD313()
{
     int iRet=0;
     
     char cFlag[2];
     struct mo_sxc_c     sMo_sxc;
     struct mdm_ac_rel_c sMdm_ac_rel;
     struct dd_mst_c     sDd_mst;
     
     memset(cFlag,       0x00, sizeof(cFlag));
     memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
     memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
     memset(&sDd_mst,     0x00, sizeof(sDd_mst));

     pub_base_sysinit();
     
     get_zd_data("0300", g_pub_tx.ac_no);
     pub_base_strpack(g_pub_tx.ac_no);
     get_zd_int("0490",  &g_pub_tx.ac_seqn);
     get_zd_data("0700", cFlag);     /**1为开户签约专用，其他交易不可传值1**/
     vtcp_log("ac_no[%s], ac_seqn[%d]", g_pub_tx.ac_no, g_pub_tx.ac_seqn);
        
 
     /*根据ac_no从表mdm_ac_rel中获取ac_id*/
     iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
     if(iRet)
     {
     	vtcp_log("获取ac_id失败，iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "D101");
     	goto ErrExit;	
     }
     
     /**随心存只针对金太阳卡和复合测试卡2开放，其他卡不能签约随心存 20151128**/
     if(strcmp(sMdm_ac_rel.mdm_code,"0027")!=0 && strcmp(sMdm_ac_rel.mdm_code,"0032")!=0)
     	goto OkExit;
     
     if(cFlag[0] != '1')  /*开户时不走此分支*/
     {
     	 iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld and ac_seqn=%d", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
         if(iRet)
         {
     	     vtcp_log(" Dd_mst_Sel错误 账户[%ld][%d]不存在iRet[%d]", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn, iRet);
     	     strcpy(g_pub_tx.reply, "P102");
     	     goto ErrExit;
         }
     	 if(sDd_mst.ac_type[0]!='5')
         {
             vtcp_log("对公活期不允许签约");
             strcpy(g_pub_tx.reply, "S035");
             goto ErrExit;
         }
         
         /*根据当前获取的ac_id判断当前账户是否签约遂心存*/
         iRet = sql_count("mo_sxc", "ac_id=%ld and ac_seqn=%d and sts='1'", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
         if(iRet > 0)
         {
       	     set_zd_data("0130", "当前账户已签约遂心存");
       	     vtcp_log("当前用户已签约！,iRet=[%d]", iRet);
       	     strcpy(g_pub_tx.reply, "S035");
       	     goto ErrExit;
         }
         
         /*****签约之前先滚动积数 ***/
         g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
         g_pub_tx.tx_amt1 = 0.00;
         strcpy( g_pub_tx.add_ind, "1" );  /**增加**/;
         strcpy( g_pub_tx.cur_no, "01" );
         strcpy( g_pub_tx.ct_ind, "2" );
         strcpy( g_pub_tx.prt_ind, "0" );  /*是否打折标志*/  
         g_pub_tx.svc_ind = 1001;          /**存取款***/
         strcpy(g_pub_tx.hst_ind, "1");
         strcpy(g_pub_tx.brf, "遂心存签约");
         strcpy(g_pub_tx.ac_wrk_ind, "000000000");
         g_reply_int = pub_acct_trance();
         if(g_reply_int != 0)
         {
             vtcp_log("滚动积数失败!");
             strcpy(g_pub_tx.reply, "S035");
             goto ErrExit;
         }
         
         sMo_sxc.qy_type[0] = '2'; /**签约交易标志**/
     }
     else
     {
     	sMo_sxc.qy_type[0] = '1';  /**开户签约标志1**/
     }
     
     /*登记遂心存登记薄*/
     sMo_sxc.ac_id = sMdm_ac_rel.ac_id;
     sMo_sxc.ac_seqn = g_pub_tx.ac_seqn;
     if(sMo_sxc.ac_seqn != 1)
     {
     	/**友情提示：目前活期序号都为1，如果以后加上子卡后存在序号不为1的账户，请注意修改查询交易sp8314,sp8315中的账户序号1**/
        vtcp_log("ac_seqn[%d]序号不为1，不允许遂心存签约", sMo_sxc.ac_seqn);
        strcpy(g_pub_tx.reply, "S035");
        goto ErrExit;
     }
     sMo_sxc.bal = 0.00;
     sMo_sxc.intst = 0.00;
     sMo_sxc.un_intst = 0.00;
     sMo_sxc.tx_date   = g_pub_tx.tx_date;
     sMo_sxc.trace_no  = g_pub_tx.trace_no;
     strcpy(sMo_sxc.tx_br_no, g_pub_tx.tx_br_no);
     strcpy(sMo_sxc.tx_tel, g_pub_tx.tel);
     strcpy(sMo_sxc.opn_br_no, sMdm_ac_rel.opn_br_no);
     sMo_sxc.sts[0]  = '1';   /**1-签约**/
     sMo_sxc.flag[0] = '1';   /**1-当日发生业务 日终执行批量**/
     
     iRet = Mo_sxc_Ins(sMo_sxc, g_pub_tx.reply);
     if(iRet)
     {
        vtcp_log("登记遂心存登记薄失败，iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D102");
        goto ErrExit;	
     }
      
     /**** 登记记流水 *******/
     g_pub_tx.ac_wrk_ind[1] = '1'; /**1-记流水**/
     g_pub_tx.ac_id = sMo_sxc.ac_id;
     strcpy(g_pub_tx.brf, "遂心存签约");
     iRet = pub_ins_trace_log();
     if(iRet)
     {
     	vtcp_log("登记流水失败，iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "S035");
     	goto ErrExit;	
     }
     /**发送签约短信**/
     if(cFlag[0] != '1')
     {
         iRet = pub_mob_sendmail(g_pub_tx.brf, g_pub_tx.tx_code, g_pub_tx.ac_no, "2", 0, 0);	
     }
     
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(strlen(g_pub_tx.reply)==0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply, "S035");	
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;

}
