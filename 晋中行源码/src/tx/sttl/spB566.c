/*************************************************
* 文 件 名:  spB566.c
* 功能描述：贴现赎回预处理通知    
*
* 作    者: yindianheng
* 完成日期： 2010年4月8日
*----------------------------转账部分参考电话银行转账
* 修改记录：
*   日   期:
*   修 改 人: 
*   修改内容:

insert into tx_def values ('4966',' 贴现赎回预处理通知','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null);
insert into tx_flow_def  values ('4966',0,'B566','#=0001@1@#$');
insert into tx_flow_def  values ('4966',1,'D003','#=0000@2@#$');
insert into tx_flow_def  values ('4966',2,'D099','#$');
insert into tx_sub_def values ('B566','  贴现赎回预处理通知','spB566','0','0');
      
 

        
*************************************************/

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "dd_mst_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "trace_log_c.h"
#include "mdm_attr_c.h"

int spB566()
{
    struct mdm_attr_c z_mdm_attr,v_mdm_attr;
    
    struct mo_discnt_c    sMoDiscnt,sMoDiscnt1;    /* 贴现登记薄 */
    struct ecds_recv_reg_c    sEcdsRecvReg;    
    struct dd_mst_c        sDdMst;
    struct dd_mst_c        sDdMst1;
    struct mdm_ac_rel_c    sMdmAcRel,sMdmAcRel1;
    struct com_parm_c    sComParm;
    struct trace_log_c    sTraceLog;
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    
    int           ret;    
    double        damt;
    
    char        cpn_ac_no[33];        /* 贴出人账号 */
    char        cpayee_ac_no[33];    /* 入账账号 */
    char        cinitTyp[2];/*预处理类型 0预处理 1撤销预处理*/
   
    char    cbuff[61];
         
    memset(&z_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
    memset(&v_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
    
    memset(&sEcdsRecvReg1, 0x00, sizeof(struct ecds_recv_reg_c));
    memset(&sMoDiscnt1, 0x00, sizeof(struct mo_discnt_c));    
    
    memset(&sMoDiscnt, 0x00, sizeof(struct mo_discnt_c));
    memset(&sEcdsRecvReg, 0x00, sizeof(struct ecds_recv_reg_c));    
    memset(&sDdMst, 0x00, sizeof(struct dd_mst_c));
    memset(&sDdMst1, 0x00, sizeof(struct dd_mst_c));
    memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sMdmAcRel1, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sComParm, 0x00, sizeof(struct com_parm_c));
    memset(&sTraceLog, 0x00, sizeof(struct trace_log_c));        
    memset(cpn_ac_no, 0x00, sizeof(cpn_ac_no));
    memset(cpayee_ac_no, 0x00, sizeof(cpayee_ac_no));    
    memset(cinitTyp, 0x00, sizeof(cinitTyp));
    
   
    memset(cbuff, 0x00, sizeof(cbuff));
    

    
    /* 数据初始化 */
    pub_base_sysinit();
    
    
    get_zd_long("0450", &sEcdsRecvReg.duedt);       /* 到期日期、 */ 
      
    get_zd_data("0300", cbuff);    /* 原贴出人账号 */
    zip_space(cbuff);
    memcpy(cpn_ac_no,cbuff,sizeof(cpn_ac_no));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0670", cinitTyp);/*预处理类型 */
    get_zd_double("0410",&damt);/*应付金额*/    
    get_zd_long("0460",&sEcdsRecvReg.ecds_tx_date);/*交易日期*/   
         
    get_zd_data("0810", cbuff);    /* 交易流水号 */
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.ecds_trace_no,cbuff,sizeof(sEcdsRecvReg.ecds_trace_no));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0680",sEcdsRecvReg.drftcls); /* 票据分类 */         
    get_zd_long("0520",&sTraceLog.pt_trace_no);
    get_zd_long("0440",&sTraceLog.tx_date);
    
    get_zd_data("0910", cbuff);    /* 票据类型 */
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drfttyp,cbuff,sizeof(sEcdsRecvReg.drfttyp));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0270", cbuff);    /* 票据号码 */
    vtcp_log(" [%s][%d] 票据号码buff:[%s] ",__FILE__,__LINE__,cbuff);
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drftnb,cbuff,sizeof(sEcdsRecvReg.drftnb));
    memset(cbuff, 0x00, sizeof(cbuff));
   
    
    vtcp_log("%s.%d   -------cdrftNb=%s  ...cdrftTyp=%s", __FILE__,__LINE__,sEcdsRecvReg.drftnb,sEcdsRecvReg.drftcls);
    vtcp_log(" [%s][%d] 日期:[%ld]",__FILE__,__LINE__,sEcdsRecvReg.ecds_tx_date);
    vtcp_log(" [%s][%d] 票据号码:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drftnb);
    vtcp_log(" [%s][%d] 到期日期:[%ld] ",__FILE__,__LINE__,sEcdsRecvReg.duedt);
    vtcp_log(" [%s][%d] 原贴出人账号:[%s] ",__FILE__,__LINE__,cpn_ac_no);
    vtcp_log(" [%s][%d] 票据类型:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    vtcp_log(" [%s][%d] 预处理类型:[%s] ",__FILE__,__LINE__,cinitTyp);
    vtcp_log(" [%s][%d] 交易流水号:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.ecds_trace_no);
    vtcp_log(" [%s][%d] 票据分类:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drftcls);
    vtcp_log(" [%s][%d] 应付金额:[%f] ",__FILE__,__LINE__,damt);
    vtcp_log(" [%s][%d] sTraceLog.tx_date:[%d] ",__FILE__,__LINE__,sTraceLog.tx_date);
    vtcp_log(" [%s][%d] sTraceLog.pt_trace_no:[%d] ",__FILE__,__LINE__,sTraceLog.pt_trace_no);
     
    
    
    sEcdsRecvReg.issedt=0;    /* 出票日 */     
    sEcdsRecvReg.trace_no=g_pub_tx.trace_no;    /* 核心流水号 */    
    sEcdsRecvReg.tx_date=g_pub_tx.tx_date;    /* 核心交易日期 */
    sEcdsRecvReg.bf_flag[0]='0';
    memcpy(sEcdsRecvReg.tag_name,"4956",sizeof(sEcdsRecvReg.tag_name));    /* 报文类型 */
    memcpy(sEcdsRecvReg.sts,"1",sizeof(sEcdsRecvReg.sts));
    memcpy(sEcdsRecvReg.check_flag,"2",sizeof(sEcdsRecvReg.check_flag)-1);/*对账标志*/
      
         
          /*---------判断转换票据类型-------------------*/
    vtcp_log("[%s][%d]--------票据类型:[%s]",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    if(!memcmp(sEcdsRecvReg.drfttyp,"AC01",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"111",4);
    }
    else if(!memcmp(sEcdsRecvReg.drfttyp,"AC02",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"112",4);        
    }
    else
    {
        sprintf(acErrMsg, "---票据类型错误!! ");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
        
    }
   
    /*预处理类型 0 预处理 1撤销预处理*/
    if(cinitTyp[0]=='0')
    {
        ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            sEcdsRecvReg.ecds_tx_date, sEcdsRecvReg.ecds_trace_no);
        if(ret==0)
        { 
            int iflag=0;
            sprintf(acErrMsg,"Ecds_recv_reg该记录存在!! ret [%d],trace_no=[%s],date=[%ld] ",\
                   ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
            WRITEMSG  	        	
            if(sEcdsRecvReg1.bf_flag[0]!='1')
            {
        	sprintf(acErrMsg," 该记录已经处理过!! bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                        sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG
                   /*ret=0表示已经预处理过，sts=2表示电票撤销，sts=1预处理*/
                 if(sEcdsRecvReg1.sts[0]=='1')
                 {
                     sprintf(acErrMsg,"该电票已经处理!! ret [%d],trace_no=[%s],date=[%ld] ",\
                        ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "NXDP");       
                     goto ErrExit; 
                 } 
                 vtcp_log(" [%s][%d] ret=[%d] Ecds_recv_reg.sts=[%s]!!",__FILE__,__LINE__,ret,sEcdsRecvReg1.sts);  
                 iflag=1;      
            }
            /*更改Ecds_recv_reg状态*/
             ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                     sEcdsRecvReg.ecds_trace_no,sEcdsRecvReg.ecds_tx_date);
             if (ret) 
             {
                 sprintf(acErrMsg, "Ecds_recv_reg_Dec_Upd![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg,g_pub_tx.reply);
             if (ret) 
             {
                 sprintf(acErrMsg, "Ecds_recv_reg_Fet_Upd![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
            }
            if(iflag==1)/*bf_flag=(0,2)时iflag=1，不需要更新bf_flag，若sts=2表可以预处理，更新sts=1*/   
            {
                  memcpy(sEcdsRecvReg.sts,"1",sizeof(sEcdsRecvReg.sts)); 
            }
            else/*bf_flag=1时iflag=0，需要更新bf_flag=2，*/   
            {
                  memcpy(sEcdsRecvReg.bf_flag,"2",sizeof(sEcdsRecvReg1.bf_flag));    
            }  
            ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg,g_pub_tx.reply);
            if (ret)
            {
                 sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd错![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
            }
            Ecds_recv_reg_Clo_Upd();
            
            
        }
        else if(ret==100)
        {
            vtcp_log(" [%s][%d] ret=[%d] 没有查到相应的信息!!",__FILE__,__LINE__,ret); 
            /*-----------Ecds_recv_reg_Ins------------*/
            ret=Ecds_recv_reg_Ins(sEcdsRecvReg,g_pub_tx.reply);    
            if (ret) 
            {
                sprintf(acErrMsg, "插入Ecds_recv_reg错![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }   
        }
        else
        {
             sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! ret [%s]",ret);
             WRITEMSG
             strcpy ( g_pub_tx.reply , "AT06" );
             goto ErrExit;
        }
         
        /* 检查贴现登记薄中  */
        ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and pn_type='1' and note_type='%s'  and sts ='3'", \
              sEcdsRecvReg.drftnb,sEcdsRecvReg.drfttyp );    
        if (ret ==100) 
        {
        	/********************如何编造一个再贴现协议编号************************/
            char cTmpBuff[9];
            char cTraceNo[10];
            char cRpact_no[21];
            memset(cTmpBuff , 0 , sizeof(cTmpBuff));
            memset(cTraceNo , 0 , sizeof(cTraceNo));
            memset(cRpact_no , 0 , sizeof(cRpact_no));
            
            sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
            vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);    
            sprintf(cRpact_no ,"%08d" , g_pub_tx.tx_date);
            memcpy(cRpact_no , cTmpBuff+2 , 6);
            vtcp_log("[%s][%d] cRpact_no=[%s] ",__FILE__,__LINE__,cRpact_no);
            
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel,"ac_no='%s'",cpn_ac_no);
            if (ret ==100)
            {
                  sprintf(acErrMsg, "介质与账户表中不存在此帐号!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "B174");
                  goto ErrExit;
            } 
            else if (ret)
            {
                  sprintf(acErrMsg, "执行Mdm_ac_rel错![%d]", ret);
                  WRITEMSG
                  goto ErrExit;
            }        
 	     strcat(cRpact_no , sMdmAcRel.opn_br_no );
 	    
 	     vtcp_log("[%s][%d] cRpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cRpact_no,g_pub_tx.trace_no);
 	     sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
 	     strcat(cRpact_no , cTraceNo );
 	     vtcp_log("[%s][%d]再贴现协议编号=[%s]", __FILE__,__LINE__,cRpact_no);
 	     memcpy(sMoDiscnt.pact_no  , cRpact_no , sizeof(sMoDiscnt.pact_no)-1);
 	     memcpy(sMoDiscnt.br_no,sMdmAcRel.opn_br_no,sizeof(sMoDiscnt.br_no)-1); 	    
 	     memcpy(sMoDiscnt.pn_type ,"1",sizeof(sMoDiscnt.pn_type)); 	    
 	     sMoDiscnt.ic_date=sEcdsRecvReg.ecds_tx_date;
 	     sMoDiscnt.pn_date=sEcdsRecvReg.ecds_tx_date;
 	     memcpy(sMoDiscnt.pn_ac_no,cpn_ac_no,sizeof(sMoDiscnt.pn_ac_no));
 	     memcpy(sMoDiscnt.pay_ac_no,cpn_ac_no,sizeof(sMoDiscnt.pn_ac_no));
 	     memcpy(sMoDiscnt.sts ,"3",sizeof(sMoDiscnt.sts ));
 	     sMoDiscnt.not_matr_date=sEcdsRecvReg.duedt;
 	     memcpy(sMoDiscnt.note_type,sEcdsRecvReg.drfttyp,sizeof(sMoDiscnt.note_type));
             memcpy(sMoDiscnt.pnote_no,sEcdsRecvReg.drftnb,sizeof(sMoDiscnt.pnote_no));
             sMoDiscnt.wrk_date=g_pub_tx.tx_date;
             sMoDiscnt.trace_no=g_pub_tx.trace_no;
             memcpy(sMoDiscnt.po_knd,"E",sizeof(sMoDiscnt.po_knd));
             get_zd_long("1261",&sMoDiscnt.tt_no);
             
             ret = Mo_discnt_Ins(sMoDiscnt, g_pub_tx.reply);
             if (ret)
             {
                  sprintf(acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret);
                  WRITEMSG
                  goto ErrExit;
             }
                     	
            memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c)); 
            
        } else if (ret) {
            sprintf(acErrMsg, "执行Mo_discnt_Sel错![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        if(sMoDiscnt.pt_knd[0]=='0') 
        {
            sprintf(acErrMsg, "该票据已经预处理过,错! ");
            WRITEMSG
            strcpy(g_pub_tx.reply, "0174");
            goto ErrExit;            
        }
    }
    else if(cinitTyp[0]=='1')/*预处理类型 0 预处理 1撤销预处理*/
    {
        ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            sEcdsRecvReg.ecds_tx_date, sEcdsRecvReg.ecds_trace_no);
        if(ret==0)
        {              
            sprintf(acErrMsg," 撤销预处理检查Ecds_recv_reg，该记录存在!! ret [%d],trace_no=[%s],date=[%ld] ",\
                   ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
            WRITEMSG  	        	
            if(sEcdsRecvReg1.bf_flag[0]=='1')
            {
        	sprintf(acErrMsg," 错误!!，该记录未预处理 bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                        sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG  
                goto ErrExit; 
            } 
              /*ret=0表示已经预处理过，sts=2表示电票撤销，sts=1未撤销*/
            if(sEcdsRecvReg1.sts[0]=='2')
            {
                sprintf(acErrMsg,"该电票已经撤销,直接返回!! ret [%d],trace_no=[%s],date=[%ld] ",\
                       ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG
                strcpy(g_pub_tx.reply, "NXDP");       
                goto ErrExit; 
            }
            vtcp_log(" [%s][%d] ret=[%d] Ecds_recv_reg.sts=[%s]!!",__FILE__,__LINE__,ret,sEcdsRecvReg1.sts);  
                         
        }
        else if(ret==100)
        {
            sprintf(acErrMsg," 无记录，电票没有预处理，无法撤销，错误!! ret=[%d]",ret); 
            WRITEMSG 
            goto ErrExit;      
        }
        else
        {
            sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! ret [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
                
        /*-----更新Ecds_recv_reg.sts==2------*/
        ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                 sEcdsRecvReg.ecds_trace_no,sEcdsRecvReg.ecds_tx_date);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Dec_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg,g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Fet_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        memcpy(sEcdsRecvReg.sts,"2",sizeof(sEcdsRecvReg.sts));    
        ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg,g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd错![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        Ecds_recv_reg_Clo_Upd();
        /*-----更新Ecds_recv_reg------*/         
        
        ret=Trace_log_Sel(g_pub_tx.reply,&sTraceLog,"tx_date=%ld and trace_no=%ld",sTraceLog.tx_date,sTraceLog.pt_trace_no);
        if (ret ==100)
        {
            ret=Trace_log_bk_Sel(g_pub_tx.reply,&sTraceLog,"tx_date=%ld and trace_no=%ld",sTraceLog.tx_date,sTraceLog.pt_trace_no);
            if (ret) 
            {
                sprintf(acErrMsg, "无此交易流水![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            
        } 
        else if (ret) {
            sprintf(acErrMsg, "执行Trace_log错![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        /*欠缺的：由sTraceLog.trace_no检查赎回登记簿中的状态，
        如果是赎回预处理状态可撤销，并设置状态为撤销。
        Mo_discnt_call_back未建立         
        */  
        
        damt=-damt;
        
        
        /* 检查贴现登记薄中  */
        ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and pn_type='1' and note_type='%s'  and sts ='3'", \
            sEcdsRecvReg.drftnb,sEcdsRecvReg.drfttyp );    
        if (ret ==100) {
             sprintf(acErrMsg, "贴现登记薄中不存在此 票据号! ");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B174");
             goto ErrExit;
        } 
        else if (ret) {
             sprintf(acErrMsg, "执行Mo_discnt_Sel错![%d]", ret);
             WRITEMSG
             goto ErrExit;
        }
        if(sMoDiscnt.pt_knd[0]!='0') 
        {
             sprintf(acErrMsg, "该票据已经预处理撤销过,直接返回! ");
             WRITEMSG
             goto OkExit;            
        }        
    }
    else
    {
        sprintf(acErrMsg, "预处理类型错误：[%s]",cinitTyp);
        WRITEMSG
        goto ErrExit;
        
    } 
    /*-----以cinitTyp对sMoDiscnt1.pt_knd更新----------*/
    ret = Mo_discnt_Dec_Upd(g_pub_tx.reply, "pact_no='%s' and note_type='%s' and pnote_no='%s' and pn_type='1' and sts ='3' ",\
            sMoDiscnt.pact_no,sEcdsRecvReg.drfttyp,sEcdsRecvReg.drftnb);
    if (ret) 
    {
        sprintf(acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_discnt_Fet_Upd(&sMoDiscnt1, g_pub_tx.reply);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "贴现登记薄中不存在此票据信息![%s],[%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B130");
        goto ErrExit;
    } 
    else if (ret) {
       sprintf(acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret);
       WRITEMSG
       goto ErrExit;
    } 
    memcpy(sMoDiscnt1.pt_knd,cinitTyp,sizeof(sMoDiscnt1.pt_knd));
    ret =Mo_discnt_Upd_Upd(sMoDiscnt1, g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg, "修改贴现登记薄错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_discnt_Clo_Upd();  
    
 
    /* -cpn_ac_no='0'时处理- */
    if((strlen(cpn_ac_no)==1)&&(cpn_ac_no[0]=='0'))
    {
        sprintf(acErrMsg, "本行贴出本行贴入的贴现!原贴出人账号:%s",cpn_ac_no);
        WRITEMSG        
        memcpy(cpn_ac_no,sMoDiscnt.pay_ac_no,sizeof(cpn_ac_no));    
    }
    /*----贴出人的还款账户查询--*/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel,"ac_no='%s'",cpn_ac_no);
    if (ret ==100) {
        sprintf(acErrMsg, "介质与账户表中不存在此帐号!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "执行Mdm_ac_rel错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
        
    ret=Dd_mst_Sel(g_pub_tx.reply,&sDdMst,"ac_id='%ld' ",sMdmAcRel.ac_id);
    if (ret ==100) {
        sprintf(acErrMsg, "活期存款表Dd_mst中不存在此帐号!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "执行Dd_mst错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    if(sDdMst.bal<damt)
    {
        sprintf(acErrMsg, "余额不足![%s]",cpn_ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "0174");
        goto ErrExit;
        
    }        
    /*----贴出人的还款账户查询--*/    
    
    /*----应解汇款账户----- 机构号??-*/
    ret=Com_parm_Sel(g_pub_tx.reply,&sComParm,"parm_code='%s' and parm_seqn='%d' ",sMoDiscnt.br_no,PARM_ECDS_TC_AC);
    if (ret ==100) {
        sprintf(acErrMsg, "公共参数表中不存在此帐号!");
        WRITEMSG
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "执行Com_parm错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }        
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel1,"ac_no='%s'",sComParm.val);
    if (ret ==100) {
        sprintf(acErrMsg, "介质与账户表中不存在此帐号!");
        WRITEMSG
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "执行Mdm_ac_rel错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    
    /*欠缺：写入一个贴现赎回登记簿中，并设状态为赎回预处理状态，
    记录平台、农信银的流水和日期。*/
    
    
    
    /*-------为调下一个交易做准备--------------------*/
    
    /****根据mdm_code查找转出方的note_type***/
    ret=Mdm_attr_Sel(g_pub_tx.reply,&v_mdm_attr," mdm_code='%s' ",sMdmAcRel.mdm_code );
    if(ret)
    {
        sprintf(acErrMsg,"取mdm_attr失败![%s]",sMdmAcRel.mdm_code);
        WRITEMSG
        strcpy( g_pub_tx.reply,"N055" );
        goto ErrExit;
    }
    
    ret=Mdm_attr_Sel(g_pub_tx.reply,&z_mdm_attr," mdm_code='%s' ",sMdmAcRel1.mdm_code );
    if(ret)
    {
        sprintf(acErrMsg,"取mdm_attr失败![%s]",sMdmAcRel1.mdm_code);
        WRITEMSG
        strcpy( g_pub_tx.reply,"N055" );
        goto ErrExit;
    }
    /*--------------------------*/
     memcpy(g_pub_tx.ac_no,cpn_ac_no,sizeof(g_pub_tx.ac_no));
     vtcp_log("%s,%d 看看转出方帐户g_pub_tx.ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
   
  
    vtcp_log("[%s][%d]--- 金额=%f", __FILE__,__LINE__,damt);  
     
    /*****end****/
    {/****开始为spD003.c做数据准备******/
        vtcp_log("[%s][%d]--- 转出账号=%s", __FILE__,__LINE__,g_pub_tx.ac_no);    
        
        set_zd_data("1021",cpn_ac_no);/**转出账号**/
        set_zd_data("1023",v_mdm_attr.note_type);/**凭证类型**/
        set_zd_data("1024",sMdmAcRel.note_no);/**凭证号码**/
        set_zd_data("1022","0001");/**帐户序号**/         
        set_zd_data("1025","");
        set_zd_data("1026","N");/**凭密码支取??**/
        set_zd_data("1027","");/**查询密码**/
        
        set_zd_data("1029","N");/**证件支取**/
        set_zd_data("102A","");/**证件号码**/
        set_zd_data("102B","");
        set_zd_data("102C","");
        set_zd_data("102D","");
        g_pub_tx.mach_date=0;
        set_zd_double("102F",damt);
        set_zd_data("102K","2");/**现转标志**/
        set_zd_data("102Y","");
        set_zd_data("0191","0");/**不用19域**/
        set_zd_data("102O","1");/**打折标志**/
        set_zd_data("102J","01");/**币种**/
        set_zd_data("0910",g_pub_tx.tx_br_no);
    }
    /****根据mdm_code查找转入方的note_type***/
    vtcp_log("[%s][%d]--- ---", __FILE__,__LINE__); 
    {/****为调用spD099.c准备数据***/
        vtcp_log("[%s][%d]--- 转入账号=%s", __FILE__,__LINE__,sComParm.val);   
        set_zd_data("1011",sComParm.val);/**转入账号**/  
        set_zd_data("1014",z_mdm_attr.note_type);/**凭证类型**/
        set_zd_data("1015",sMdmAcRel1.note_no);      
        get_zd_data("0310","");        
        set_zd_data("1012","0000");/**帐户序号**/        
        set_zd_data("101A","01");/**币种**/
       
        set_zd_data("101B","2");/***现转标志***/
        set_zd_double("1013",damt);/***转入金额***/        
    }
    vtcp_log("[%s][%d] reply=[%s] ",__FILE__,__LINE__,g_pub_tx.reply);
OkExit1:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B566");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
