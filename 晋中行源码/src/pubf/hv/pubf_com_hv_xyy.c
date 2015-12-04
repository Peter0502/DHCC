#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "hv_poinfo_c.h"
#include "lv_define.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "svrpub_c.h"
#include <fcntl.h>
#include   "data_dic_c.h"
#include "hvstat_c.h"
#include "find_debug.h"

extern char *cTmpfp;
extern char *cTmpfp1;
#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
int iGetHvFzfOrderno(char * orderno);
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);
char *find_qs_no(char *brno,char *qsactno)
{
    /*根据参与者号或联行号得到清算行行行号*/
    char sql_bakcode[13];
    struct hv_uniontab_c hv_uniontab;
    MEMSET_DEBUG(&hv_uniontab,0,sizeof(hv_uniontab));
    MEMSET_DEBUG(&sql_bakcode,'\0',sizeof(sql_bakcode));

    if (brno==NULL)
    {        
        sprintf( acErrMsg,"取当前行的清算行行号[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"取当前行的清算行行号");
        WRITEMSG
        memcpy(qsactno,HV_QSBRNO,strlen(HV_QSBRNO));
        return(qsactno);
    }else
    {
        sprintf( acErrMsg,"根据参与者号得到清算行号[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"根据参与者号得到清算行号");
        WRITEMSG
        vtcp_log("[%s][%d]根据参与者号得到清算行号brno=[%s]",__FILE__,__LINE__,sql_bakcode);
    }    
    vtcp_log("[%s][%d]brno=[%s]",__FILE__,__LINE__,brno);
    memcpy(sql_bakcode,brno,sizeof(sql_bakcode)-1);
    vtcp_log("[%s][%d]sql_bakcode=[%s]",__FILE__,__LINE__,sql_bakcode);

    g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' and br_sts ='1' ",sql_bakcode);
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"取清算行行号出错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"取清算行行号出错");
        WRITEMSG
        return (NULL);
    }
    memcpy(qsactno,hv_uniontab.qs_no,12);
    vtcp_log("得到清算行号[%s][%d]qs_no=[%s]",__FILE__,__LINE__,qsactno);
    return(qsactno);
}
char *find_sendco(char *brno,char *sendco)
{
    int ret=0;
    char sql_bakcode[13];
    struct hv_uniontab_c hv_uniontab;
    MEMSET_DEBUG(&hv_uniontab,0,sizeof(hv_uniontab));
    MEMSET_DEBUG(&sql_bakcode,'\0',sizeof(sql_bakcode));
    vtcp_log("[%s][%d]根据参与者号brno==[%s]得到收报中心",__FILE__,__LINE__,brno);
    memcpy(sql_bakcode,brno,sizeof(sql_bakcode)-1);
    vtcp_log("[%s][%d]根据参与者号[%s]得到收报中心",__FILE__,__LINE__,sql_bakcode);
    if (strlen(sql_bakcode)==0)
    {
        vtcp_log("取当前行的收报中心代码[%s][%d]sql_bakcode=[%s]",__FILE__,__LINE__,sql_bakcode);
        memcpy(sendco,HV_SENDCO,strlen(HV_SENDCO));
        return(sendco);
    }else
    {
        vtcp_log("[%s][%d]根据参与者号[%s]得到收报中心",__FILE__,__LINE__,sql_bakcode);
    }    
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' and br_sts ='1' ",sql_bakcode);
    if ( ret )
    {
        sprintf( acErrMsg,"取出收报中心出错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"取出收报中心出错");
        WRITEMSG
            return (NULL);
    }
    memcpy(sendco,hv_uniontab.sendco,12);
    vtcp_log("[%s][%d]根据参与者号得到收报中心[%s]",__FILE__,__LINE__,sendco);
    return(sendco);
}
/****************************************************************************/
/* 函数名称: dg_hv_zfhtr(HV_PAYIN_AREA * hv_payin,double txamt)             */
/* 功能描述: 插入支付明细表                                                 */
/* 创建日期: 2006-8-15 18:22                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/
int dg_hv_zfhtr(HV_PAYIN_AREA * hv_payin,char * txamt)
{
    char cTxamt[17];
    struct hv_zfhtr_c hv_zfhtr;

    vtcp_log("[%s,%d] txamt====[%s]",__FILE__,__LINE__,txamt);
    MEMSET_DEBUG(cTxamt   ,0,sizeof(cTxamt));
    MEMSET_DEBUG(&hv_zfhtr,0,sizeof(struct hv_zfhtr_c));
    memcpy(cTxamt,txamt,sizeof(cTxamt)-1);
    vtcp_log("[%s,%d] cTxamt====[%s]",__FILE__,__LINE__,cTxamt);
    memcpy(hv_zfhtr.br_no       ,hv_payin->T_tx_br_no ,BRNO_LEN);
    memcpy(hv_zfhtr.orderno     ,hv_payin->T_orderno  ,sizeof(hv_zfhtr.orderno )-1);
    memcpy(hv_zfhtr.cur_no      ,hv_payin->T_cur_no,sizeof(hv_zfhtr.cur_no  )-1);
    memcpy(hv_zfhtr.note_type   ,hv_payin->T_note_type,sizeof(hv_zfhtr.note_type)-1);
    memcpy(hv_zfhtr.note_no     ,hv_payin->T_note_no ,sizeof(hv_zfhtr.note_no)-1);
    memcpy(hv_zfhtr.pay_qs_no   ,hv_payin->T_pay_qs_no,sizeof(hv_zfhtr.pay_qs_no)-1);
    memcpy(hv_zfhtr.or_br_no    ,hv_payin->T_or_br_no ,sizeof(hv_zfhtr.or_br_no )-1);
    memcpy(hv_zfhtr.pay_opn_br_no ,hv_payin->T_pay_opn_br_no \
        ,sizeof(hv_zfhtr.pay_opn_br_no )-1);
    memcpy(hv_zfhtr.pay_ac_no   ,hv_payin->T_pay_ac_no,sizeof(hv_zfhtr.pay_ac_no)-1);
    memcpy(hv_zfhtr.pay_name    ,hv_payin->T_pay_name,sizeof(hv_zfhtr.pay_name)-1);
    memcpy(hv_zfhtr.pay_addr    ,hv_payin->T_pay_addr ,sizeof(hv_zfhtr.pay_addr  )-1);
    memcpy(hv_zfhtr.cash_qs_no  ,hv_payin->T_cash_qs_no,sizeof(hv_zfhtr.cash_qs_no)-1);
    memcpy(hv_zfhtr.ac_br_no    ,hv_payin->T_ac_br_no ,sizeof(hv_zfhtr.ac_br_no )-1);
    memcpy(hv_zfhtr.cash_opn_br_no,hv_payin->T_cash_opn_br_no \
        ,sizeof(hv_zfhtr.cash_opn_br_no)-1);
    memcpy(hv_zfhtr.cash_ac_no, hv_payin->T_cash_ac_no,sizeof(hv_zfhtr.cash_ac_no )-1);
    memcpy(hv_zfhtr.cash_name , hv_payin->T_cash_name ,sizeof(hv_zfhtr.cash_name)-1);
    memcpy(hv_zfhtr.cash_addr , hv_payin->T_cash_addr ,sizeof(hv_zfhtr.cash_addr)-1);
    memcpy(hv_zfhtr.yw_type   , hv_payin->T_yw_type,sizeof(hv_zfhtr.yw_type  )-1);
    memcpy(hv_zfhtr.sendco    , hv_payin->T_sendco,sizeof(hv_zfhtr.sendco)-1);
    memcpy(hv_zfhtr.receco    , hv_payin->T_receco,sizeof(hv_zfhtr.receco)-1);
    memcpy(hv_zfhtr.cmtno     , hv_payin->T_cmtno,sizeof(hv_zfhtr.cmtno    )-1);
    memcpy(hv_zfhtr.hv_prt_ind, hv_payin->T_hv_prt_ind,sizeof(hv_zfhtr.hv_prt_ind)-1);
    memcpy(hv_zfhtr.o_orderno , hv_payin->T_o_orderno,sizeof(hv_zfhtr.o_orderno)-1);
    memcpy(hv_zfhtr.o_cmtno   , hv_payin->T_o_cmtno,sizeof(hv_zfhtr.o_cmtno)-1);
    memcpy(hv_zfhtr.addid     , hv_payin->T_addid,sizeof(hv_zfhtr.addid)-1);
    memcpy(hv_zfhtr.hv_brf    , hv_payin->T_hv_brf,sizeof(hv_zfhtr.hv_brf)-1);
    memcpy(hv_zfhtr.refno     , hv_payin->T_refno,sizeof(hv_zfhtr.refno)-1);
    memcpy(hv_zfhtr.resp1     , hv_payin->T_resp1,sizeof(hv_zfhtr.resp1)-1);
    
    vtcp_log("[%s,%d] cTxamt====[%s]",__FILE__,__LINE__,cTxamt);
    vtcp_log("[%s,%d] hv_payin->T_chrg_amt====[%s]",__FILE__,__LINE__,hv_payin->T_chrg_amt);      
    dot_str2dbl(cTxamt,sizeof(cTxamt)-1,&hv_zfhtr.tx_amt);
    dot_str2dbl(hv_payin->T_chrg_amt,sizeof(hv_payin->T_chrg_amt),&hv_zfhtr.chrg_amt);

    vtcp_log("[%s,%d] hv_zfhtr.tx_amt====[%f]",__FILE__,__LINE__,hv_zfhtr.tx_amt);
    vtcp_log("[%s,%d] hv_zfhtr.chrg_amt====[%f]",__FILE__,__LINE__,hv_zfhtr.chrg_amt);      

    hv_zfhtr.tx_date=apatoi(hv_payin->T_tx_date,8);
    hv_zfhtr.wt_date=apatoi(hv_payin->T_wt_date,8);
    hv_zfhtr.resend_date = apatoi(hv_payin->T_resend_date,8);
    hv_zfhtr.o_tx_date   = apatoi(hv_payin->T_o_tx_date,8);
    hv_zfhtr.o_wt_date   = apatoi(hv_payin->T_o_wt_date,8);
    if (memcmp(hv_payin->F_opcd,"10",2)==0)
    {
        hv_zfhtr.trace_no=apatoi(hv_payin->T_trace_no,6);
        memcpy(hv_zfhtr.tel , hv_payin->T_tel ,sizeof(hv_zfhtr.tel    )-1);
    }        
    if (memcmp(hv_payin->F_opcd,"30",2)==0)
    {
        hv_zfhtr.trace_no     =  apatoi(hv_payin->T_chk_trace_no,6);
        hv_zfhtr.chk_trace_no =  apatoi(hv_payin->T_chk_trace_no,6);
        memcpy(hv_zfhtr.tel , hv_payin->T_chk ,sizeof(hv_zfhtr.tel    )-1);
        memcpy(hv_zfhtr.chk , hv_payin->T_chk ,sizeof(hv_zfhtr.chk    )-1);
    }
    if (memcmp(hv_payin->F_opcd,"40",2)==0)
    {
        hv_zfhtr.trace_no          =  apatoi(hv_payin->T_send_tel_trace_no,6);
        hv_zfhtr.send_tel_trace_no =  apatoi(hv_payin->T_send_tel_trace_no,6);
        memcpy(hv_zfhtr.tel      , hv_payin->T_send_tel,sizeof(hv_zfhtr.tel    )-1);
        memcpy(hv_zfhtr.send_tel , hv_payin->T_send_tel ,sizeof(hv_zfhtr.send_tel    )-1);
    }
    vtcp_log("[%s][%d]     tel==[%s]",__FILE__,__LINE__,hv_zfhtr.tel);
    vtcp_log("[%s][%d]trace_no==[%d]",__FILE__,__LINE__,hv_zfhtr.trace_no);
    get_zd_long(DC_TRACE_NO,&hv_zfhtr.trace_no);
    hv_zfhtr.tx_time=apatoi(hv_payin->T_tx_time,6);
    /**网银交易大额发送交易走到这里把流水号+1，否则插入大额交易历史明细表重复 hzh 20130612**/
    if(memcmp( g_pub_tx.tx_code,"6389",sizeof( g_pub_tx.tx_code))==0 &&
    memcmp(g_pub_tx.sub_tx_code,"J009",sizeof(g_pub_tx.sub_tx_code))==0)
    {
        hv_zfhtr.trace_no++;
        hv_zfhtr.send_tel_trace_no++;
        vtcp_log("%s %d 网银交易大额发送交易走到这里把流水号+1,hv_zfhtr.trace_no=[%d],hv_zfhtr.send_tel_trace_no=[%d]",__FILE__,__LINE__,hv_zfhtr.trace_no,hv_zfhtr.send_tel_trace_no);
    }
    /**网银交易大额发送交易走到这里把流水号+1，否则插入大额交易历史明细表重复 hzh 20130612**/
    hv_zfhtr.lw_ind[0]      = hv_payin->T_lw_ind[0];
    hv_zfhtr.hv_sts[0]      = hv_payin->T_hv_sts[0];
    hv_zfhtr.beg_sts[0]     = hv_payin->T_beg_sts[0];
    hv_zfhtr.tx_chrg_ind[0] = hv_payin->T_tx_chrg_ind[0];
    hv_zfhtr.operlevel[0]   = hv_payin->T_operlevel[0];
    hv_zfhtr.hv_osts[0]     = hv_payin->T_hv_osts[0];
    hv_zfhtr.checkflag[0]   = hv_payin->T_checkflag[0];
    hv_zfhtr.resend_ind[0]  = hv_payin->T_resend_ind[0];
    vtcp_log("[%s,%d] hv_zfhtr.lw_ind====[%s]",__FILE__,__LINE__,hv_zfhtr.lw_ind);      

    if (Hv_zfhtr_Ins(hv_zfhtr ,g_pub_tx.reply ))
    {
        strncpy(acErrMsg,"插大额交易历史明细表错误!",sizeof(acErrMsg));
        set_zd_data(DC_GET_MSG,"插大额交易历史明细表错误");
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        WRITEMSG
            return -1;
    }
    return 0;
}
/****************************************************************************/
/* 函数名称: dg_hv_fzfhtr(HV_NOTPAY_AREA * hv_notpay)                       */
/* 功能描述: 插入非支付明细表                                           */
/* 创建日期: 2006-8-15 18:22                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/
int dg_hv_fzfhtr(HV_NOTPAY_AREA * hv_notpay)
{
    struct hv_fzfhtr_c hv_fzfhtr;
    MEMSET_DEBUG(&hv_fzfhtr,0,sizeof( struct hv_fzfhtr_c));
    memcpy(hv_fzfhtr.or_br_no     ,hv_notpay->T_or_br_no,sizeof(hv_fzfhtr.or_br_no)-1);  
    memcpy(hv_fzfhtr.orderno     ,hv_notpay->T_orderno,sizeof(hv_fzfhtr.orderno)-1);  
    memcpy(hv_fzfhtr.br_no        ,hv_notpay->T_br_no  ,BRNO_LEN);  
    hv_fzfhtr.hv_fzfhtr_sts[0]  = hv_notpay->T_hv_fzf_sts[0];
    hv_fzfhtr.o_tx_type[0]      = hv_notpay->T_o_tx_type[0]; 
    hv_fzfhtr.res_sts[0]        = hv_notpay->T_respcode[0]; 
    hv_fzfhtr.lw_ind[0]         = hv_notpay->T_lw_ind[0];
    memcpy(hv_fzfhtr.cmtno        ,hv_notpay->T_cmtno  ,sizeof(hv_fzfhtr.cmtno  )-1);  
    memcpy(hv_fzfhtr.pay_qs_no  ,hv_notpay->T_pay_qs_no,sizeof(hv_fzfhtr.pay_qs_no  )-1);  
    memcpy(hv_fzfhtr.ac_br_no   ,hv_notpay->T_ac_br_no,sizeof(hv_fzfhtr.ac_br_no)-1);  
    memcpy(hv_fzfhtr.cash_qs_no ,hv_notpay->T_cash_qs_no ,sizeof(hv_fzfhtr.cash_qs_no )-1);  
    memcpy(hv_fzfhtr.o_or_br_no ,hv_notpay->T_o_or_br_no ,sizeof(hv_fzfhtr.o_or_br_no )-1);  
    memcpy(hv_fzfhtr.o_ac_br_no ,hv_notpay->T_o_ac_br_no ,sizeof(hv_fzfhtr.o_ac_br_no )-1);  
    memcpy(hv_fzfhtr.o_orderno  ,hv_notpay->T_o_payin_orderno,sizeof(hv_fzfhtr.o_orderno)-1);  
    memcpy(hv_fzfhtr.o_cur_ind  ,hv_notpay->T_o_cur_no,sizeof(hv_fzfhtr.o_cur_ind  )-1);  
    memcpy(hv_fzfhtr.o_cmtno    ,hv_notpay->T_o_cmtno,sizeof(hv_fzfhtr.o_cmtno)-1);  

    memcpy(hv_fzfhtr.sendco  ,hv_notpay->T_sendco ,sizeof(hv_fzfhtr.sendco )-1);  
    memcpy(hv_fzfhtr.receco  ,hv_notpay->T_receco ,sizeof(hv_fzfhtr.receco )-1);  
    memcpy(hv_fzfhtr.req_br_no,hv_notpay->T_req_br_no  ,sizeof(hv_fzfhtr.req_br_no  )-1);  
    memcpy(hv_fzfhtr.req_content,hv_notpay->T_req_content,sizeof(hv_fzfhtr.req_content)-1);  
    memcpy(hv_fzfhtr.req_orderno,hv_notpay->T_req_orderno,sizeof(hv_fzfhtr.req_orderno)-1);  
    memcpy(hv_fzfhtr.res_br_no  ,hv_notpay->T_res_br_no  ,sizeof(hv_fzfhtr.res_br_no  )-1);  
    memcpy(hv_fzfhtr.res_content,hv_notpay->T_res_content,sizeof(hv_fzfhtr.res_content)-1);  
    memcpy(hv_fzfhtr.res_orderno,hv_notpay->T_res_orderno,sizeof(hv_fzfhtr.res_orderno)-1);  

    memcpy(hv_fzfhtr.addid        ,hv_notpay->T_addid      ,sizeof(hv_fzfhtr.addid  )-1);  
    memcpy(hv_fzfhtr.refno        ,hv_notpay->T_refno      ,sizeof(hv_fzfhtr.refno  )-1);  
    memcpy(hv_fzfhtr.hv_prt_ind ,hv_notpay->T_hv_prt_ind ,sizeof(hv_fzfhtr.hv_prt_ind )-1);  
    memcpy(hv_fzfhtr.hv_brf     ,hv_notpay->T_hv_brf     ,sizeof(hv_fzfhtr.hv_brf )-1);  
    memcpy(hv_fzfhtr.tel          ,hv_notpay->T_tel        ,sizeof(hv_fzfhtr.tel )-1);

    hv_fzfhtr.tx_date     = apatoi(hv_notpay->T_tx_date,8 );
    hv_fzfhtr.wt_date     = apatoi(hv_notpay->T_wt_date,8);
    vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
    vtcp_log("[%s][%d]hv_fzfhtr.wt_date=[%.8ld]",__FILE__,__LINE__,hv_fzfhtr.wt_date);
    hv_fzfhtr.tx_time     = apatoi(hv_notpay->T_tx_time ,6);
    hv_fzfhtr.o_tx_date    = apatoi(hv_notpay->T_o_wt_date,8);
    hv_fzfhtr.req_date    = apatoi(hv_notpay->T_req_date,8);
    hv_fzfhtr.res_date    = apatoi(hv_notpay->T_res_date,8);
    hv_fzfhtr.trace_no    = apatoi(hv_notpay->T_trace_no,6);

    dot_str2dbl(hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt),&hv_fzfhtr.o_tx_amt);

    vtcp_log("[%s][%d] =========== Hv_fzfhtr_Debug  ======= ",__FILE__,__LINE__);
    Hv_fzfhtr_Debug(&hv_fzfhtr);    
    if (Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply))
    {
        strncpy(acErrMsg,"插入大额非支付交易历史明细表错误!!",sizeof(acErrMsg));
        set_zd_data(DC_GET_MSG,"插入大额非支付交易历史明细表错误");
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        WRITEMSG
            return -1;
    }
    return 0;
}
/****************************************************************************/
/* 函数名称: iCheckDownloadFile()                                     */
/* 功能描述: 查看明细下载是否成功                                           */
/* 创建日期: 2006-8-16 14:42                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/ 
int iCheckDownloadFile()
{
    long lTx_date;
    int iCount;
    int ret=0;
    iCount=0;

    get_zd_long("0050",&lTx_date);
    ret=sql_count("ch_chkagemr","tx_date= %s" ,lTx_date);
    if (iCount==0)
    {
        sprintf(acErrMsg,"今天明细未下载成功 %d",iCount);
        set_zd_data(DC_GET_MSG,"今天明细未下载成功");
        WRITEMSG
            return -1;
    }
    return 0;
}
/*************************************************************************************************/
/* 函数名称:hv_insert_sendlist(*pHvvPack,char *brno,char *tlrno,char *wssrno,char *orderno)       */
/* 功能描述:  根据条件插入发送登记簿                                                              */
/* 创建日期: 2006-8-15 10:52                                                                      */
/* 作者:     xyy                                                                                  */
/**************************************************************************************************/ 
int hv_insert_sendlist(THVPACKAGE *pHvPack,char *brno,char *tlrno,char *wssrno,char *orderno)
{
    /*插入发送登记簿*/
    struct hv_sendlist_c hv_sendlist;
    char buf[32];
    char cBrf[256];
    char cOrbrno[13];
    char cAcbrno[13];
    char cCmtno[4];
    char cWssrno[7];
    char cBrno[BRNO_LEN+1];
    int  ret=0;

    vtcp_log("[%s,%d]hv_insert_sendlist",__FILE__,__LINE__);
    MEMSET_DEBUG(&hv_sendlist,'\0',sizeof(hv_sendlist));
    MEMSET_DEBUG(cBrf,   0,sizeof(cBrf));
    MEMSET_DEBUG(cOrbrno,0,sizeof(cOrbrno));
    MEMSET_DEBUG(cWssrno,0,sizeof(cWssrno));
    MEMSET_DEBUG(cAcbrno,0,sizeof(cAcbrno));
    MEMSET_DEBUG(cCmtno,0,sizeof(cCmtno));
    MEMSET_DEBUG(cBrno  ,0,sizeof(cBrno));
    vtcp_log("body==[%s]",pHvPack->body);
    memcpy(hv_sendlist.br_no  ,brno ,BRNO_LEN);
    vtcp_log("[%s,%d]brno=[%s]",__FILE__,__LINE__,hv_sendlist.br_no);
    memcpy(hv_sendlist.tel    ,tlrno ,sizeof(hv_sendlist.tel  )-1);
    vtcp_log("[%s,%d]hv_sendlist.tel=[%s]",__FILE__,__LINE__,hv_sendlist.tel);

    vtcp_log("[%s][%d]wssrno=[%s]",__FILE__,__LINE__,wssrno);
    memcpy(cWssrno,wssrno,sizeof(cWssrno)-1);
    hv_sendlist.trace_no=apatoi(cWssrno,6);
    vtcp_log("[%s,%d]hv_sendlist.trace_no=[%d]",__FILE__,__LINE__,hv_sendlist.trace_no);

    memcpy(hv_sendlist.ref_no ,pHvPack->reNo     ,sizeof(hv_sendlist.ref_no)-1);
    vtcp_log("[%s,%d]ref_no=[%s]",__FILE__,__LINE__,hv_sendlist.ref_no);

    /*得到报文参考号，主键中的主键*/
    memcpy(cCmtno  ,pHvPack->CMTCode  ,sizeof(cCmtno)-1);
    memcpy(hv_sendlist.cmtno,cCmtno,sizeof(cCmtno)-1);
    vtcp_log("[%s,%d]cmtno=[%s]",__FILE__,__LINE__,hv_sendlist.cmtno);
    memcpy(hv_sendlist.cmt_title,hv_getcmtdesc(pHvPack->CMTCode),sizeof(hv_sendlist.cmt_title)-1);
    memcpy(hv_sendlist.orderno ,orderno ,sizeof(hv_sendlist.orderno)-1);
    vtcp_log("[%s,%d]hv_sendlist.orderno=[%s]",__FILE__,__LINE__,hv_sendlist.orderno);
    zip_space(hv_sendlist.br_no);
    zip_space(hv_sendlist.tel);

    /**现在的orderno统一了，所以接收从前台传过来的就OK了
    if(hv_get_tag(pHvPack,"0BC",hv_sendlist.orderno)<0)
    if(hv_get_tag(pHvPack,"056",hv_sendlist.orderno)<0)
    if(hv_get_tag(pHvPack,"043",hv_sendlist.orderno)<0)
    hv_get_tag(pHvPack,"C47",hv_sendlist.orderno);
    **/

    vtcp_log("[%s,%d]hv_sendlist.orderno=[%s]",__FILE__,__LINE__,hv_sendlist.orderno);

    /**这里对不同的报文类型，对接收行号和发起行号做不同的处理**/
    vtcp_log("[%s,%d]cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
    if (memcmp(cCmtno,"301",3)==0 )
    {
        hv_get_tag(pHvPack,"01C",cOrbrno  );
        memcpy(hv_sendlist.or_br_no,cOrbrno,sizeof(cOrbrno)-1);
        hv_get_tag(pHvPack,"01D",cAcbrno  );
        memcpy(hv_sendlist.ac_br_no,cAcbrno,sizeof(cAcbrno)-1);
    }
    else if (memcmp(cCmtno,"302",3)==0)
    {
        hv_get_tag(pHvPack,"01D",cOrbrno  );
        memcpy(hv_sendlist.or_br_no,cOrbrno,sizeof(cOrbrno)-1);
        hv_get_tag(pHvPack,"CCB",cAcbrno  );
        memcpy(hv_sendlist.ac_br_no,cAcbrno,sizeof(cAcbrno)-1);
    }
    else if (memcmp(cCmtno,"314",3)==0)
    {
        hv_get_tag(pHvPack,"CCE",cOrbrno  );
        memcpy(hv_sendlist.or_br_no,cOrbrno,sizeof(cOrbrno)-1);
        hv_get_tag(pHvPack,"58A",cAcbrno  );
        memcpy(hv_sendlist.ac_br_no,cAcbrno,sizeof(cAcbrno)-1);
    }
    else
    {
        hv_get_tag(pHvPack,"52A",cOrbrno  );
        memcpy(hv_sendlist.or_br_no,cOrbrno,sizeof(cOrbrno)-1);
        hv_get_tag(pHvPack,"58A",cAcbrno  );
        memcpy(hv_sendlist.ac_br_no,cAcbrno,sizeof(cAcbrno)-1);
    }
    vtcp_log("[%s,%d]hv_sendlist.ac_br_no=[%s]",__FILE__,__LINE__,hv_sendlist.ac_br_no);
    vtcp_log("[%s,%d]hv_sendlist.or_br_no=[%s]",__FILE__,__LINE__,hv_sendlist.or_br_no);

    hv_get_tag(pHvPack,"50C",hv_sendlist.pay_ac_no  );
    vtcp_log("[%s,%d]hv_sendlist.tel=[%s]",__FILE__,__LINE__,hv_sendlist.tel);
    vtcp_log("[%s,%d]hv_sendlist.pay_ac_no=[%s]",__FILE__,__LINE__,hv_sendlist.pay_ac_no);
    hv_get_tag(pHvPack,"50A",hv_sendlist.pay_name   );
    vtcp_log("[%s,%d]hv_sendlist.pay_name=[%s]",__FILE__,__LINE__,hv_sendlist.pay_name);
    hv_get_tag(pHvPack,"59C",hv_sendlist.cash_ac_no );
    vtcp_log("[%s,%d]hv_sendlist.cash_ac_no=[%s]",__FILE__,__LINE__,hv_sendlist.cash_ac_no);
    hv_get_tag(pHvPack,"59A",hv_sendlist.cash_name  );
    vtcp_log("[%s,%d]hv_sendlist.ordercash_name=[%s]",__FILE__,__LINE__,hv_sendlist.cash_name);
    vtcp_log("[%s,%d]hv_sendlist.tel=[%s]",__FILE__,__LINE__,hv_sendlist.tel);
    MEMSET_DEBUG(buf,'\0',sizeof(buf));   
    hv_get_tag(pHvPack,"32A",buf  );
    dot_str2dbl( buf+3,sizeof(buf)-3,&hv_sendlist.tx_amt);
    vtcp_log("[%s,%d]hv_sendlist.tel=[%s]",__FILE__,__LINE__,hv_sendlist.tel);
    vtcp_log("[%s,%d]hv_sendlist.tx_amt=[%f]",__FILE__,__LINE__,hv_sendlist.tx_amt);
    hv_sendlist.resp_flg[0]='0';
    hv_get_tag(pHvPack,"72A",cBrf );
    vtcp_log("[%s,%d]cBrf=[%s]",__FILE__,__LINE__,cBrf); 

    zip_space(cBrf);
    memcpy(hv_sendlist.brf,cBrf,30);
    vtcp_log("[%s,%d]hv_sendlist.brf  =[%s]",__FILE__,__LINE__,hv_sendlist.brf); 

    get_fd_data("0030",cBrno);/*发起行就是当前机构号的行号*/
    vtcp_log("[%s,%d]cBrno=[%s]",__FILE__,__LINE__,cBrno);

    vtcp_log("[%s,%d]坚持就是胜利     ref_no=[%s]",__FILE__,__LINE__,hv_sendlist.ref_no);

    if (Hv_sendlist_Ins(hv_sendlist,g_pub_tx.reply))
    {
        sprintf(acErrMsg,"插入发送报文登记簿错误![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"插入发送报文登记簿错误");
        WRITEMSG
            return(-1);
    }
    return(0);
}
/****************************************************************************/
/* 函数名称: hv_getcmtdesc(char *cmtno)                                     */
/* 功能描述: 根据cmt号得到cmt名称                                           */
/* 创建日期: 2006-8-15 10:52                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/     
char* hv_getcmtdesc(char *cmtno)
{
    static char desc[30]; /*xyy 私自改成30位，以便能顺利的放到cmt_title字段*/
    MEMSET_DEBUG(desc,'\0',sizeof(desc));
    if (memcmp(cmtno,"100",3)==0)
    {
        strcpy(desc,"汇兑支付");
    }else if(memcmp(cmtno,"101",3)==0)
    {
        strcpy(desc,"委托收款");
    }else if(memcmp(cmtno,"102",3)==0)
    {
        strcpy(desc,"托收承付");
    }else if(memcmp(cmtno,"105",3)==0)
    {
        strcpy(desc,"同业拆借");
    }else if(memcmp(cmtno,"108",3)==0)
    {
        strcpy(desc,"退汇支付");
    }else if(memcmp(cmtno,"109",3)==0)
    {
        strcpy(desc,"电子联行专用汇兑");
    }else if(memcmp(cmtno,"301",3)==0)
    {
        strcpy(desc,"大额查询");
    }else if(memcmp(cmtno,"302",3)==0)
    {
        strcpy(desc,"大额查复");
    }else if(memcmp(cmtno,"303",3)==0)
    {
        strcpy(desc,"自由格式");
    }else if(memcmp(cmtno,"311",3)==0)
    {
        strcpy(desc,"撤销申请");
    }else if(memcmp(cmtno,"313",3)==0)
    {
        strcpy(desc,"申请退回");
    }else if(memcmp(cmtno,"314",3)==0)
    {
        strcpy(desc,"业务退回应答");
    }else if(memcmp(cmtno,"651",3)==0)
    {
        strcpy(desc,"支付业务查询");
    }else if(memcmp(cmtno,"659",3)==0)
    {
        strcpy(desc,"业务明细请求");
    }else if(memcmp(cmtno,"121",3)==0)
    {
        strcpy(desc,"银行汇票资金移存");
    }else if(memcmp(cmtno,"721",3)==0)
    {
        strcpy(desc,"申请清算银行汇票");
    }else if(memcmp(cmtno,"724",3)==0)
    {
        strcpy(desc,"银行汇票申请退回");
    }else
    {
        sprintf(desc,"未知业务[%.3s]",desc);
    }
    return(desc);
}
/* 找到汇票处理中心喽 */
char *find_hpcenter(char *hpcenter)
{
    vtcp_log("[%s][%d]调用汇票处理中心",__FILE__,__LINE__); 
    memcpy(hpcenter,HV_HPCENTER,strlen(HV_HPCENTER));
    vtcp_log("[%s][%d]调用汇票处理中心hpcenter=[%s]",__FILE__,__LINE__,hpcenter); 
    return(hpcenter);
}

/****************************************************************************/
/* 函数名称: iHvTistoNotpay(HV_NOTPAY_AREA * hv_notpay)                     */
/* 功能描述: 从前台得到数据到F_字段                                         */
/* 创建日期: 2006-8-19 15:58                                                */
/* 作者:     xyy                                                           */
/****************************************************************************/ 
int iHvTistoNotpay(HV_NOTPAY_AREA * hv_notpay)
{
    HV_FD123_AREA fd123;
    char cOrderno[9];
    char cInd[2];
    char cCmtno[4];
    char cCur_no[3];
    char cRececo[5];
    int iRet=0;
    int iOrderno = 0;/*by liuyue 20060913*/
    MEMSET_DEBUG(cCur_no, 0 , sizeof(cCur_no));
    MEMSET_DEBUG(cInd, 0 , sizeof(cInd));
    MEMSET_DEBUG(cRececo,0,sizeof(cRececo));
    MEMSET_DEBUG(cOrderno, 0 , sizeof(cOrderno));
    MEMSET_DEBUG(cCmtno,0,sizeof(cCmtno));
    MEMSET_DEBUG(&fd123, 0 , sizeof(fd123));
    get_fd_data("1230",(char*)&fd123);

    vtcp_log("123域=[%s]",(char*)&fd123.ac_br_no); 

    vtcp_log("123域=[%s]",(char*)&fd123.cmtno); 
    vtcp_log("o_notpay_orderno==[%s]",(char *)&fd123.o_notpay_orderno);
    memcpy(hv_notpay->F_cmtno,fd123.cmtno,sizeof(hv_notpay->F_cmtno));
    vtcp_log("[%s][%d]!!!sizeof(fd123)====[%d]\n",__FILE__,__LINE__,sizeof(fd123));
    vtcp_log("[%s][%d]!!!sizeof(fd123)====[%d]\n",__FILE__,__LINE__,sizeof(fd123));
    /* 交易机构 */ 
    get_fd_data("0030",hv_notpay->F_tx_br_no);
    vtcp_log("前台传来的brno=[%.5s]",hv_notpay->F_tx_br_no); 
    /*操作代码***/
    vtcp_log("前台传来的fd123.opcd=[%.2s]",fd123.opcd); 
    memcpy(hv_notpay->F_opcd,fd123.opcd,sizeof(hv_notpay->F_opcd));
    vtcp_log("前台传来的opcd=[%.2s]",hv_notpay->F_opcd); 
    /**委托日期**/
    get_fd_data("0440",hv_notpay->F_wt_date);
    vtcp_log("前台传来的wt_date=[%.8s]",hv_notpay->F_wt_date); 
    get_fd_data("0050",hv_notpay->F_tx_date);
    /* 币种 */
    get_fd_data("0210",cCur_no);
    vCur_dhcc_mbfe(cCur_no,hv_notpay->F_o_cur_no);
    /* 交易金额 */
    get_fd_data("0390",hv_notpay->F_o_txamt);
    vtcp_log("前台传来的txamt=[%.16s]",hv_notpay->F_o_txamt);TRACE 
    /**操作员号**/
    if (memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
    {
        get_fd_data("0070",hv_notpay->F_tel);
    }
    else if (memcmp(hv_notpay->F_opcd,"40",sizeof(hv_notpay->F_opcd))==0)
    {
        get_fd_data("0070",hv_notpay->F_send_tel);
    }
    vtcp_log("前台传来的tel=[%s]",hv_notpay->F_tel); 
    vtcp_log("前台传来的tel=[%s]",hv_notpay->F_send_tel); 

    /**汇票是否核押**/
    hv_notpay->F_hp_add_pwd[0] = fd123.hp_add_pwd[0];
    /**原业务类型**/
    vtcp_log("[%s,%d] fd123.tx_type===[%s]\n",__FILE__,__LINE__, &fd123.tx_type[0]);
    hv_notpay->F_o_tx_type[0] = fd123.tx_type[0];
    vtcp_log("[%s,%d]ywtype===[%c]\n",__FILE__,__LINE__,hv_notpay->F_o_tx_type[0]);
    /* 原非支付交易日期,查复中用到的查询日期 */    
    get_fd_data("0460",hv_notpay->F_o_fzf_date);    
    vtcp_log("前台传来的F_o_fzf_date=[%.8s]",hv_notpay->F_o_fzf_date); 
    memcpy(cCmtno,hv_notpay->F_cmtno,sizeof(hv_notpay->F_cmtno));
    /**选择大小额**/
    hv_notpay->F_otype[0] = fd123.otype[0];
    vtcp_log("前台传来的hv_notpay->F_otype[0]=[%c]",hv_notpay->F_otype[0]); 
    /**撤消或退回应答**/
    hv_notpay->F_respcode[0] = fd123.resp[0];
    /**不同的报文，交易序号放在不同的字段**/
    if (memcmp(cCmtno,"301",sizeof(cCmtno)-1)==0)
    {
        memcpy(cOrderno ,fd123.qr_tx_num    ,sizeof(cOrderno)-1);
        vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);
    }
    else if (memcmp(cCmtno,"302",sizeof(cCmtno)-1)==0)
    {
        memcpy(cOrderno ,fd123.cr_tx_num    ,sizeof(cOrderno)-1);
        vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);
    }
    else if (memcmp(cCmtno,"303",sizeof(cCmtno)-1)==0 
        || memcmp(cCmtno,"721",sizeof(cCmtno)-1)==0 
        || memcmp(cCmtno,"724",sizeof(cCmtno)-1)==0 )/*721、724 暂时放在这个字段下*/
    {
        memcpy(cOrderno  ,fd123.orderno     ,sizeof(cOrderno)-1);
        vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);
    }
    else if (memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0)
    {/***by liuyue 20060828 20:52*******/
        memcpy(cOrderno,"00000",5);
        memcpy(&cOrderno[0]+5 ,fd123.o_notpay_orderno ,3);
        vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);
    }
    else  /* 311、314的交易序号只有三位*/
    {
        memcpy(cOrderno,"00000",5);
        memcpy(cOrderno+5 ,fd123.notpay_orderno   ,3);
        vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);
    }
    zip_space(cOrderno);
    vtcp_log("[%s,%d]cOrderno===[%s]\n",__FILE__,__LINE__,cOrderno);

    if (strlen(cOrderno)!=8 && strlen(cOrderno)!=3)/**change by liuyue 20060913  在这里可以取小额的序号***/
    {
	/****
        vtcp_log("[%s][%d]fd123.otype==[%c]\n",fd123.otype[0]);
	****/
        vtcp_log("[%s][%d]fd123.otype==[%c]\n",__FILE__,__LINE__,fd123.otype[0]);
        if (fd123.otype[0]=='1')
        {
            vtcp_log("[%s][%d]开始生成大额的交易序号!\n",__FILE__,__LINE__);
            iRet=iGetHvFzfOrderno(cOrderno);
            if (iRet)
            {
                vtcp_log("[%s,%d]得到支付交易序号错误=%d",__FILE__,__LINE__,iRet); 
                sprintf( acErrMsg,"得到支付交易序号错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
                set_zd_data(DC_GET_MSG,"得到支付交易序号错误!");
                return iRet;
            }
            memcpy(fd123.orderno,cOrderno,sizeof(cOrderno)-1);
            set_zd_data("1230",(char*)&fd123);
        }
        else if (fd123.otype[0]=='2')
        {
            vtcp_log("[%s][%d]小额的交易序号到小额交易中取生成!\n",__FILE__,__LINE__);
            /****小额序号要到小额交易里面来生成****/
        }
        else
        {
            vtcp_log("[%s][%d]非大小额业务,请您查证!\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"非大小额业务,请您查证!");
            return (-1);
        }
    }
    vtcp_log("%s,%d 前台传来的cOrderno=[%8s]",__FILE__,__LINE__,cOrderno); 
    /**往来表示***/
    hv_notpay->F_lw_ind[0]=fd123.lw_ind[0];
    vtcp_log("%s,%d lw_ind前台传来的附加域=[%c]",__FILE__,__LINE__,fd123.lw_ind[0]); 
    memcpy(hv_notpay->F_orderno ,cOrderno ,sizeof(hv_notpay->F_orderno ));
    vtcp_log("%s,%d F_orderno=[%.8s]",__FILE__,__LINE__,hv_notpay->F_orderno); 
    vtcp_log("%s,%d cmtno前台传来的附加域=[%s]",__FILE__,__LINE__,hv_notpay->F_cmtno); 
    /**原CMT编号**/
    memcpy(hv_notpay->F_o_cmtno   ,fd123.o_cmtno   ,sizeof(hv_notpay->F_o_cmtno));
    vtcp_log("%s,%d o_cmtno前台传来的附加域=[%s]",__FILE__,__LINE__,hv_notpay->F_o_cmtno); 

    /**发起行行号**/
    vtcp_log("[%s,%d]交易行号 br_no=[%s]",__FILE__,__LINE__,hv_notpay->F_tx_br_no); 
    vtcp_log("[%s,%d]交易行号 cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
    vtcp_log("[%s,%d]交易行号 F_lw_ind[0]=[%c]",__FILE__,__LINE__,hv_notpay->F_lw_ind[0]);
    if (hv_notpay->F_lw_ind[0]=='1')/*如果是往*/
    {
        if ( memcmp(cCmtno,"302",3)==0)
        {
            /*查询行行号*/
            memcpy(hv_notpay->F_or_br_no,fd123.qr_br_no,sizeof(hv_notpay->F_or_br_no));
            vtcp_log("[%s,%d]or_br_no=[%.12s]",__FILE__,__LINE__,hv_notpay->F_or_br_no);
        }
        else
        {
            GetOr_br_noByBr_no(hv_notpay->F_tx_br_no,hv_notpay->F_or_br_no);
            vtcp_log("[%s,%d]or_br_no=[%.12s]",__FILE__,__LINE__,hv_notpay->F_or_br_no);
        }
    }
    else
    {
        memcpy(hv_notpay->F_or_br_no,fd123.or_br_no,sizeof(hv_notpay->F_or_br_no));
        vtcp_log("[%s,%d]or_br_no=[%.12s]",__FILE__,__LINE__,hv_notpay->F_or_br_no);
    }
    vtcp_log("[%s,%d]发起行号 or_br_no=[%.12s]",__FILE__,__LINE__,hv_notpay->F_or_br_no); 
    /**接收行行号**/

    memcpy(cRececo,fd123.receco,sizeof(cRececo)-1);
    zip_space(cRececo);/*自由格式需要用到收报中心*/
    if (strlen(cRececo)==4)
    {
        memcpy(hv_notpay->F_ac_br_no,fd123.receco,sizeof(hv_notpay->F_ac_br_no));
        vtcp_log("[%s,%d]F_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay->F_ac_br_no); 
    }
    else
    {
        if (memcmp(cCmtno,"301",3)==0 )
        {
            /*查复行行号*/
            memcpy(hv_notpay->F_ac_br_no,fd123.cr_br_no,sizeof(hv_notpay->F_ac_br_no));
        }
        else if ((memcmp(cCmtno,"721",3))==0 || (memcmp(cCmtno,"724",3))==0)
            memcpy(hv_notpay->F_ac_br_no,"904290099992",sizeof(hv_notpay->F_ac_br_no));
        else
            memcpy(hv_notpay->F_ac_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_ac_br_no));
        vtcp_log("[%s,%d]F_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay->F_ac_br_no); 
    }    
    vtcp_log("%s,%d 前台传来的ac_br_no=[%.12s]",__FILE__,__LINE__,hv_notpay->F_ac_br_no); 
    /*附言*/
    memcpy(hv_notpay->F_content,fd123.notpay_content,sizeof(hv_notpay->F_content)); 
    memcpy(hv_notpay->F_hv_brf,fd123.brf,sizeof(hv_notpay->F_hv_brf));
    vtcp_log("%s,%d ************F_content*************=[%s]",__FILE__,__LINE__,hv_notpay->F_content);
    vtcp_log("%s,%d F_hv_brf=[%s]",__FILE__,__LINE__,hv_notpay->F_hv_brf);

    /* 原委托日期 */
    get_fd_data("0450",hv_notpay->F_o_wt_date);
    vtcp_log("%s,%d 前台传来的o_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->F_o_wt_date); 
    /**原支付交易序号**/
    memcpy(hv_notpay->F_o_payin_orderno,fd123.o_orderno,sizeof(hv_notpay->F_o_payin_orderno));
    vtcp_log("%s,%d 前台传来的o_orderno=[%.8s]",__FILE__,__LINE__,hv_notpay->F_o_payin_orderno); 
    /**处理号码**/
    memcpy(hv_notpay->F_resp1,fd123.procode,sizeof(hv_notpay->F_resp1));
    vtcp_log("[%s][%d]hv_notpay->F_resp1[%.8s]",__FILE__,__LINE__,hv_notpay->F_resp1);
    /*非支付交易序号*/
    memcpy(hv_notpay->F_o_notpay_orderno,fd123.qr_tx_num,sizeof(hv_notpay->F_o_notpay_orderno));
    vtcp_log("[%s][%d]F_o_notpay_orderno=[%s]",__FILE__,__LINE__,hv_notpay->F_o_notpay_orderno);    
    if (memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
    {
        iGetHvRefno(HV_QS_BR_NO,hv_notpay->F_refno);
    }
    vtcp_log("[%s][%d]hv_notpay->F_refno=[%s]",__FILE__,__LINE__,hv_notpay->F_refno);
    if (memcmp(cCmtno,"301",3)==0 )/**需要时再添加**/
    {
        /*原支付交易发起行*/
        memcpy(hv_notpay->F_o_or_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_o_or_br_no));
        /*原支付交易接收行*/
        memcpy(hv_notpay->F_o_ac_br_no,fd123.or_br_no,sizeof(hv_notpay->F_o_ac_br_no));
    }
    return 0;
}
/****************************************************************************/
/* 函数名称: iHvfFrontToTable(HV_NOTPAY_AREA * hv_notpay)                   */
/* 功能描述: 从NOTPAY的F_字段赋值给T字段                                    */
/* 创建日期: 2006-8-19 19:12                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/
int iHvfFrontToTable(HV_NOTPAY_AREA * hv_notpay,char * cbuff)
{    
    int iRet=0;
    char cFilename[1000];
    char cBuff[2000];
    char cCmtno[4];
    FILE * fpr;

    Gl_file_first=0;
    MEMSET_DEBUG(cFilename, 0  , sizeof(cFilename));
    MEMSET_DEBUG(cBuff,0,sizeof(cBuff));    
    MEMSET_DEBUG(cCmtno,0,sizeof(cCmtno)-1);
    get_fd_data("0060",hv_notpay->T_tx_time);/* 交易时间*/
    get_fd_data("0040",hv_notpay->T_trace_no);/* 流水号*/
    vtcp_log("[%s][%d]hv_notpay->T_tx_time=[%s]",__FILE__,__LINE__,hv_notpay->T_tx_time);
    vtcp_log("[%s][%d]hv_notpay->T_trace_no=[%s]",__FILE__,__LINE__,hv_notpay->T_trace_no);
    hv_notpay->T_otype[0] = hv_notpay->F_otype[0];/*0大额 1小额*/
    vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay->T_otype[0]);
    hv_notpay->T_lw_ind[0] = hv_notpay->F_lw_ind[0]; /* 来往标识1-往2来 */
    hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/* 原交易类型 */
    vtcp_log("[%s,%d]T_o_tx_type===[%c]\n",__FILE__,__LINE__,hv_notpay->T_o_tx_type[0]);
    hv_notpay->T_respcode[0] = hv_notpay->F_respcode[0];/* 退回/撤销应答,0, 可以 1，不可以 */
    vtcp_log("[%s][%d]hv_notpay->F_respcode=[%c]",__FILE__,__LINE__,hv_notpay->F_respcode[0]);
    memcpy(hv_notpay->T_orderno,hv_notpay->F_orderno ,sizeof(hv_notpay->T_orderno));/*交易序号*/
    memcpy(hv_notpay->T_tx_date,hv_notpay->F_tx_date ,sizeof(hv_notpay->T_tx_date));/*交易日期*/
    memcpy(hv_notpay->T_wt_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_wt_date));/*委托日期*/
    memcpy(hv_notpay->T_br_no  ,hv_notpay->F_tx_br_no ,sizeof(hv_notpay->T_br_no));/*交易机构*/
    GetOr_br_noByBr_no(hv_notpay->T_br_no  ,hv_notpay->T_or_br_no);  /* 交易行号 */
    memcpy(cCmtno,hv_notpay->F_cmtno,sizeof(cCmtno)-1);
    memcpy(hv_notpay->T_cmtno ,hv_notpay->F_cmtno ,sizeof(hv_notpay->T_cmtno));/* 交易类型 */
    vtcp_log("[%s][%d]hv_notpay->T_or_br_no=[%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
    /*得到发报中心和发送行清算中心*/
    iGet_full_hv_no(hv_notpay->T_or_br_no,hv_notpay->T_sendco,hv_notpay->T_pay_qs_no);

    vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
    /* 原委托日期 */
    memcpy(hv_notpay->T_o_wt_date  ,hv_notpay->F_o_wt_date  ,sizeof(hv_notpay->T_o_wt_date ));
    /* 原发起行行号 */
    memcpy(hv_notpay->T_o_or_br_no ,hv_notpay->F_o_or_br_no ,sizeof(hv_notpay->T_o_or_br_no));
    /***Modified by SSH,2006.8.28,如果接行4位表示是收报中心****/
    pub_base_full_anti_zero(hv_notpay->F_ac_br_no,sizeof(hv_notpay->F_ac_br_no));
    if (hv_notpay->F_ac_br_no[5]=='\0')
    {    
        MEMSET_DEBUG(hv_notpay->T_ac_br_no  ,'\0',sizeof(hv_notpay->T_ac_br_no));/* 接收行行号 */
    }else
    {
        /* 接收行行号 */
        memcpy(hv_notpay->T_ac_br_no  ,hv_notpay->F_ac_br_no   ,sizeof(hv_notpay->T_ac_br_no));
    }
    /*得到收中心和接收行清算中心*/
    iGet_full_hv_no(hv_notpay->T_ac_br_no,hv_notpay->T_receco,hv_notpay->T_cash_qs_no);
    /* 原接收行行号 */
    memcpy(hv_notpay->T_o_ac_br_no ,hv_notpay->F_o_ac_br_no ,sizeof(hv_notpay->T_o_ac_br_no));
    /* 原支付交易序号 */
    memcpy(hv_notpay->T_o_payin_orderno ,hv_notpay->F_o_payin_orderno ,sizeof(hv_notpay->T_o_payin_orderno ));
    /* 原币种 */
    memcpy(hv_notpay->T_o_cur_no   ,hv_notpay->F_o_cur_no   ,sizeof(hv_notpay->T_o_cur_no));
    /* 原交易金额 */
    memcpy(hv_notpay->T_o_tx_amt   ,hv_notpay->F_o_txamt    ,sizeof(hv_notpay->T_o_tx_amt));
    /* 原业务类型 */
    memcpy(hv_notpay->T_o_cmtno    ,hv_notpay->F_o_cmtno    ,sizeof(hv_notpay->T_o_cmtno));
    vtcp_log("[%s][%d]F_content=[%s]",__FILE__,__LINE__,hv_notpay->F_content);
    vtcp_log("[%s][%d]F_cmtno=[%s]",__FILE__,__LINE__,hv_notpay->F_cmtno);
    vtcp_log("[%s][%d]cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
    /*****对申请类和应答类分别处理*****//**by liuyue 20060913***/
    if (memcmp(cCmtno,"301",3)==0 ||memcmp(cCmtno,"311",3)==0 ||memcmp(cCmtno,"313",3)==0
        ||memcmp(cCmtno,"721",3)==0 ||memcmp(cCmtno,"724",3)==0 ||memcmp(cCmtno,"303",3)==0
        ||memcmp(cCmtno,"319",3)==0 ||memcmp(cCmtno,"321",3)==0 ||memcmp(cCmtno,"323",3)==0
        ||memcmp(cCmtno,"327",3)==0)
    {
        /* 申请交易序号 */
        memcpy(hv_notpay->T_req_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_req_orderno));
        /* 申请日期 */
        memcpy(hv_notpay->T_req_date,hv_notpay->F_tx_date ,sizeof(hv_notpay->T_req_date));
        /* 申请机构 */
        memcpy(hv_notpay->T_req_br_no,hv_notpay->F_or_br_no,sizeof(hv_notpay->T_req_br_no));
        /* 申请内容 */
        memcpy(hv_notpay->T_req_content,hv_notpay->F_content,sizeof(hv_notpay->T_req_content));
        vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
    }
    if (memcmp(cCmtno,"302",3)==0 ||memcmp(cCmtno,"314",3)==0||memcmp(cCmtno,"320",3)==0 ||memcmp(cCmtno,"322",3)==0 
        ||memcmp(cCmtno,"324",3)==0 ||memcmp(cCmtno,"328",3)==0  )
    {    /* 应答交易序号 */
        memcpy(hv_notpay->T_res_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_res_orderno));
        /* 应答日期 */
        memcpy(hv_notpay->T_res_date,hv_notpay->F_tx_date ,sizeof(hv_notpay->T_res_date));
        /* 应答机构 */
        memcpy(hv_notpay->T_res_br_no,hv_notpay->F_or_br_no,sizeof(hv_notpay->T_res_br_no));
        /* 申请内容 */
        memcpy(hv_notpay->T_res_content,hv_notpay->F_content,sizeof(hv_notpay->T_res_content));
        vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
    }
    vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
    vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
    hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/**原业务种类**/
    memcpy(hv_notpay->T_refno ,hv_notpay->F_refno  ,sizeof(hv_notpay->T_refno));/* 报文参考号 */
    memcpy(hv_notpay->T_tel      ,hv_notpay->F_tel    ,sizeof(hv_notpay->T_tel));/* 操作员号 */
    memcpy(hv_notpay->T_send_tel,hv_notpay->F_send_tel,sizeof(hv_notpay->F_send_tel));
    memcpy(hv_notpay->T_hv_brf ,hv_notpay->F_hv_brf,sizeof(hv_notpay->T_hv_brf)); /* 备注 */

    if (memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
    {/* 01录入 */
        hv_notpay->T_hv_fzf_sts[0]=SEND_REGIST;
    }
    else if (memcmp(hv_notpay->F_opcd,"30",sizeof(hv_notpay->F_opcd))==0)
    {/* 03发送 */
        hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_WAIT;
    }
    else if (memcmp(hv_notpay->F_opcd,"60",sizeof(hv_notpay->F_opcd))==0)
    {/* 04接收 */
        if (!memcmp(g_pub_tx.reply,"0000",4))
        {
            hv_notpay->T_hv_fzf_sts[0]=RECEIVE_ACCT_QS;
        }
        else
        {
            /* 记账失败了，要先判断接收机构是否存在，存在挂支行，不存在挂清算 */
            hv_notpay->T_hv_fzf_sts[0]=RECEIVE_CUSTNAME_ERR;
        }
    }
    else if (memcmp(hv_notpay->F_opcd,"50",sizeof(hv_notpay->F_opcd))==0)
    {/* 05应答 */
        hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_QS;
    }
    else
    {
        vtcp_log("[%s][%d]非支付交易好象没有此种交易类型啊！F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
        sprintf(acErrMsg,"非支付交易好象没有此种交易类型啊！F_opcd!\n");
        set_zd_data(DC_GET_MSG,"非支付交易好象没有此种交易类型啊");
        WRITEMSG
            return 1;
    }
    vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,hv_notpay->T_cmtno);

    /*以下 对附加域进行处理 */
    if (memcmp(hv_notpay->T_cmtno , "721" , sizeof(hv_notpay->T_cmtno))==0 || \
        memcmp(hv_notpay->T_cmtno , "724" , sizeof(hv_notpay->T_cmtno))==0 )
    {
        vtcp_log("有附加域cmtno=[%s]",hv_notpay->T_cmtno); 
        Gl_file_first=0;/* add by LiuHuafeng 2006-8-26 14:32 */
        pub_base_AllDwnFilName( cFilename);
        vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,cFilename);
        fpr=fopen(cFilename,"rb");
        if (fpr==NULL)
        {
            sprintf(acErrMsg,"得到附加域文件错误![%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"得到附加域文件错误");
            WRITEMSG
                return -1;
        }
        fgets(cBuff,sizeof(cBuff),fpr);
        vtcp_log("%s,%d file=[%s]\n",__FILE__,__LINE__,cBuff);
        fclose(fpr);
        iRet=iHvSetAddit(hv_notpay->F_tx_date,hv_notpay->F_wt_date,hv_notpay->T_addid,"000",cBuff,strlen(cBuff));
        if (iRet)
        {
            vtcp_log("写入附加域错误");TRACE 
                sprintf(acErrMsg,"写入附加域错误![%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"写入附加域错误");
            WRITEMSG
                return -1;
        }
        memcpy(cbuff,cBuff,sizeof(cBuff)-1);
        vtcp_log("%s,%d cBuff=[%s]\n",__FILE__,__LINE__,cBuff);
    }
    vtcp_log("[%s][%d]F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
    vtcp_log("[%s][%d]fzf_sts=[%c]",__FILE__,__LINE__,hv_notpay->T_hv_fzf_sts[0]);
    return 0;
}
/****************************************************************************/
/* 函数名称: cIns_hp(HV_NOTPAY_AREA * hv_notpay,HV_721_ADD  * hv_721_add)   */
/* 功能描述: 把NOTPAY和hv_721_add的信息插入到poinfo表中                     */
/* 创建日期: 2006-8-26 16:21                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/

int cIns_hp(HV_NOTPAY_AREA hv_notpay,HV_721_ADD hv_721_add)
{
    struct hv_poinfo_c hv_poinfo;
    char cPay_qs_no[13];
    char cTxank[13];
    char cHp_del_no[13];
    char cHp_del_center[13];
    char cFlag[2];
    long iEndday=0;
    MEMSET_DEBUG(cFlag     ,0,sizeof(cFlag));
    MEMSET_DEBUG(&hv_poinfo,0,sizeof(hv_poinfo));
    MEMSET_DEBUG(cPay_qs_no,0,sizeof(cPay_qs_no));

    vtcp_log("[%s][%d]   hv_721_add  =[%s]",__FILE__,__LINE__,&hv_721_add);
    /* 根据签发行行号得到支付行清算行号 */
    memcpy(cTxank,hv_721_add.pay_opn_br_no,sizeof(hv_721_add.pay_opn_br_no));
    vtcp_log("[%s][%d]pay_opn_br_no  =[%s]",__FILE__,__LINE__,hv_721_add.pay_opn_br_no);
    vtcp_log("[%s][%d]   cTxank  =[%s]",__FILE__,__LINE__,cTxank);
    if (find_qs_no(cTxank,cPay_qs_no)==NULL)
    {
        sprintf(acErrMsg,"签发行的清算行号不存在[%s][%d]!",__FILE__,__LINE__);
        WRITEMSG
            return 1;
    }
    vtcp_log("[%s][%d]   hv_721_add.pj_date  =[%.8ld]",__FILE__,__LINE__,hv_721_add.pj_date);
    vtcp_log("[%s][%d]   hv_notpay.F_tx_date  =[%s]",__FILE__,__LINE__,hv_notpay.F_tx_date);
    if (apatoi(hv_721_add.pj_date,8)-apatoi(hv_notpay.F_tx_date,8)>0)
    {
        sprintf(acErrMsg,"输入的票据日期太大![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"输入的票据日期太大");
        WRITEMSG
            return 1;
    }
    /* 得到汇票中心代码 */
    find_hpcenter(cHp_del_no);
    /* 得到汇票中心清算行行号 */
    vtcp_log("[%s][%d]   cHp_del_no  =[%s]",__FILE__,__LINE__,cHp_del_no);

    /* 得到收报中心代码和汇票处理中心号码 */
    iGet_full_hv_no(cHp_del_no,hv_notpay.T_receco,cHp_del_center);
    vtcp_log("发起清算行号cHp_del_center=[%.12s]",cHp_del_center);

    /* 得到发报中心代码 */
    if (find_sendco(hv_notpay.F_or_br_no,hv_notpay.T_sendco)==NULL)
    {
        sprintf(acErrMsg,"交易机构的发报中心代码不存在[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"交易机构的发报中心代码不存在");
        WRITEMSG
            return 1;
    }
    /* 票据号码 */
    /* rem by LiuHuafeng 20061003
    memcpy(hv_poinfo.po_no,hv_721_add.pj_num,sizeof(hv_poinfo.po_no)-1);
    ***************replace by next line*********************************/
    memcpy(hv_poinfo.po_no,hv_721_add.pj_num,HV_POLEN);
    vtcp_log("[%s][%d]hv_poinfo.po_no=[%.20s]",__FILE__,__LINE__,hv_poinfo.po_no);
    /* 交易机构 */
    memcpy(hv_poinfo.br_no,hv_notpay.F_tx_br_no,sizeof(hv_poinfo.br_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.or_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.or_br_no);
    /* 发起行行号 */
    memcpy(hv_poinfo.or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_poinfo.or_br_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.or_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.or_br_no);
    /* 支付行清算行号 */
    memcpy(hv_poinfo.pay_qs_no,cPay_qs_no,sizeof(hv_poinfo.pay_qs_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.pay_qs_no=[%s]",__FILE__,__LINE__,hv_poinfo.pay_qs_no);
    /* 上次交易日期 */
    hv_poinfo.l_tx_date=apatoi(hv_notpay.F_tx_date,8);
    vtcp_log("[%s][%d]hv_poinfo.l_tx_date=[%.08ld]",__FILE__,__LINE__,hv_poinfo.l_tx_date);
    /* 票面金额 */
    dot_str2dbl( hv_721_add.cp_amt,sizeof(hv_721_add.cp_amt),&hv_poinfo.po_bal);
    hv_poinfo.po_bal = hv_poinfo.po_bal/100.0;
    vtcp_log("[%s][%d]hv_poinfo.po_bal=[%f]",__FILE__,__LINE__,hv_poinfo.po_bal);
    /* 实际结算金额 */
    dot_str2dbl( hv_721_add.real_js_amt,sizeof(hv_721_add.real_js_amt),&hv_poinfo.cash_amt);
    hv_poinfo.cash_amt = hv_poinfo.cash_amt/100.0;
    vtcp_log("[%s][%d]hv_poinfo.cash_amt=[%f]",__FILE__,__LINE__,hv_poinfo.cash_amt);
    /* 多余金额 */
    vtcp_log("[%s][%d]hv_721_add.dy_amt==[%s]\n",__FILE__,__LINE__,hv_721_add.dy_amt);
    dot_str2dbl( hv_721_add.dy_amt,sizeof(hv_721_add.dy_amt),&hv_poinfo.rem_amt);
    hv_poinfo.rem_amt = hv_poinfo.rem_amt/100.0;
    vtcp_log("[%s][%d]hv_poinfo.rem_amt=[%f]",__FILE__,__LINE__,hv_poinfo.rem_amt);
    /* 付款人开户行 */
    memcpy(hv_poinfo.pay_br_no,hv_721_add.pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.pay_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.pay_br_no);
    /* 兑付行行号 */
    memcpy(hv_poinfo.cash_br_no,hv_notpay.F_ac_br_no,sizeof(hv_poinfo.cash_br_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.cash_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no);
    /* 最后持票人姓名 */
    memcpy(hv_poinfo.hold_ac_no,hv_721_add.last_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);
    vtcp_log("[%s][%d]hv_poinfo.hold_ac_no=[%s]",__FILE__,__LINE__,hv_poinfo.hold_ac_no);
    /* 最后持票人姓名 */
    memcpy(hv_poinfo.hold_name,hv_721_add.last_name,sizeof(hv_poinfo.hold_name)-1);
    vtcp_log("[%s][%d]hv_poinfo.hold_name=[%s]",__FILE__,__LINE__,hv_poinfo.hold_name);
    /* 汇票当前状态 */
    hv_poinfo.po_sts[0] = HV_HP_ASKCASH;
    vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
    /* 登记日期 */
    hv_poinfo.reg_date=apatoi(hv_notpay.F_tx_date,8);
    vtcp_log("[%s][%d]hv_poinfo.reg_date=[%.08ld]",__FILE__,__LINE__,hv_poinfo.reg_date);
    /* 签发日期 */
    hv_poinfo.sign_date=apatoi(hv_721_add.pj_date,8);
    vtcp_log("[%s][%d]hv_poinfo.sign_date=[%.08ld]",__FILE__,__LINE__,hv_poinfo.sign_date);
    /* 提示付款日期 */

    /*判断汇票是否逾期-**/
    pub_base_deadlineM( hv_poinfo.sign_date, 1, &iEndday);/*得到到期日期*/
    vtcp_log("[%s][%d]iEndday=[%.08ld]",__FILE__,__LINE__,iEndday);
    if (iEndday-apatoi(hv_notpay.F_tx_date,8)<0)
    {
        sprintf(acErrMsg,"该汇票已经逾期![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"该汇票已经逾期");
        WRITEMSG
            return 1;
    }
    /**提示付款日期**/
    hv_poinfo.pay_date= iEndday;
    vtcp_log("[%s][%d]iEndday=[%.08ld]",__FILE__,__LINE__,iEndday);
    vtcp_log("[%s][%d]hv_poinfo.pay_date=[%.08ld]",__FILE__,__LINE__,hv_poinfo.pay_date);
    /* 密押 */
    memcpy(hv_poinfo.pwd,hv_721_add.hp_miya,sizeof(hv_poinfo.pwd)-1);
    /* 汇票类型 */
    hv_poinfo.po_ind[0] = hv_721_add.hp_type[0];
    /* 行别标志 */
    if (memcmp(cPay_qs_no,LV_QSBRNO,sizeof(cPay_qs_no)-1)==0)
        hv_poinfo.bank_type[0]='0';/*本行汇票*/
    else
        hv_poinfo.bank_type[0]='1';/*他行汇票*/
    vtcp_log("[%s][%d]hv_poinfo.bank_type[0]=[%c]",__FILE__,__LINE__,hv_poinfo.bank_type[0]);
    /**币种**/
    memcpy(hv_poinfo.cur_id,"01",sizeof(hv_poinfo.cur_id)-1);
    vtcp_log("[%s][%d]hv_poinfo.cur_id=[%s]",__FILE__,__LINE__,hv_poinfo.cur_id);
    /* 附言 */
    memcpy(hv_poinfo.content,hv_notpay.F_content,sizeof(hv_poinfo.content)-1);
    memcpy(hv_poinfo.brf,hv_notpay.F_content,sizeof(hv_poinfo.brf)-1);
    vtcp_log("[%s][%d]hv_poinfo.brf=[%s]",__FILE__,__LINE__,hv_poinfo.brf);
    hv_poinfo.td_cnt=0;
    hv_poinfo.add_pwd_ind[0]='0';
    get_zd_data("0680",cFlag);
    vtcp_log("[%s][%d]您选择的核押方式是[%c]",__FILE__,__LINE__,cFlag[0]);
    /* 在这里进行汇票的核押 ****/
    if (cFlag[0]=='0')
    {
        printf("[%s][%d]开始汇票核押!\n",__FILE__,__LINE__);
        char OtherInfo[61];
        char cCashbrno[13];
        char Hptype;
        char cSignDate[9];
        int  iRet;
        iRet=0;
        char cHpamt[18];
        MEMSET_DEBUG(cHpamt, 0 , sizeof(cHpamt));
        MEMSET_DEBUG(cSignDate, 0 , sizeof(cSignDate));
        MEMSET_DEBUG(OtherInfo,0,sizeof(OtherInfo));
        MEMSET_DEBUG(cCashbrno, 0, sizeof(cCashbrno));

        /* 连接汇票服务器验证汇票密押是否正确 */

        if (hv_poinfo.po_ind[0] == '0')
        {
            Hptype = 2;
            memcpy(cCashbrno,"000000000000",12);
        }
        else if (hv_poinfo.po_ind[0] == '1')
        {
            Hptype = 3;
            memcpy(cCashbrno,"000000000000",12);
        }
        else if (hv_poinfo.po_ind[0] == '2')
        {
            Hptype = 1;
            /* rem by LiuHuafeng 2006-11-5 12:46
            memcpy(cCashbrno,hv_poinfo.cash_br_no,12);
            ************************************/
            memcpy(cCashbrno,hv_notpay.F_or_br_no,sizeof(cCashbrno)-1);
        }
        vtcp_log("%s,%d hptype=[%d],cashbrno=[%s]",__FILE__,__LINE__,Hptype,cCashbrno);
        vtcp_log("%s,%d HP_SERVERNAME1=[%s],HP_SERVERNAME2=[%s]",__FILE__,__LINE__,HP_SERVERNAME1,HP_SERVERNAME2);
        iRet = SetParamAll(HP_SERVERNAME1,HP_SERVERNAME2,TIME_OUT);
        if (iRet)
        {
            sprintf(acErrMsg,"设置网络参数失败[%s][%d]!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"P013");
            set_zd_data("0130","汇票密押服务器连接失败");
            WRITEMSG
                return 1;
        }
        sprintf(cSignDate,"%08d",hv_poinfo.sign_date);
        memcpy(cHpamt,hv_721_add.cp_amt,sizeof(hv_721_add.cp_amt));
        pub_base_full_zero(cHpamt,sizeof(hv_721_add.cp_amt));
        vtcp_log("%s,%d hptype=[%d],cashbrno=[%s],signdate=[%s]",__FILE__,__LINE__,Hptype,cCashbrno,cSignDate);
        vtcp_log("%s,%d amt=[%s],paybrno=[%s]",__FILE__,__LINE__,cHpamt,hv_poinfo.pay_br_no);
        g_reply_int = DraftEncrypt(1,Hptype,cSignDate,hv_poinfo.po_no
            ,cHpamt,hv_poinfo.pay_br_no,cCashbrno,OtherInfo,hv_poinfo.pwd);
        if (g_reply_int)
        {
            sprintf(acErrMsg,"核押错误[%s][%d]!",__FILE__,__LINE__);
            vtcp_log("%s,%d errcode=[%d] ",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"P014");
            set_zd_data("0130","汇票密押错误");
            WRITEMSG
                return 1;
        }
    }
    /*vtcp_log("[%s][%d]hv_poinfo.pwd=[%s]",__FILE__,__LINE__,hv_poinfo.pwd);*/
    hv_poinfo.add_pwd_ind[0]='1';
    /*vtcp_log("[%s][%d]add_pwd_ind[0]=[%c]",__FILE__,__LINE__,hv_poinfo.add_pwd_ind[0]);*/
    /******************************
    * 检查是否已经存在该笔汇票    *
    ******************************/

    g_reply_int =  Hv_poinfo_Dec_Sel ( g_pub_tx.reply ,"po_no='%s' and pay_br_no ='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
    vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
    vtcp_log("[%s][%d]g_reply_int=========================[%d]",__FILE__,__LINE__,g_reply_int);
    if ( g_reply_int)
    {
        sprintf(acErrMsg,"声明游标出错![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"声明游标出错");
        WRITEMSG
            return (NULL);
    }
    vtcp_log("[%s][%d]声明游标Hv_poinfo_Dec_Sel OVER",__FILE__,__LINE__);
    g_reply_int = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
    vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
    vtcp_log("[%s][%d]g_reply_int=========================[%d]",__FILE__,__LINE__,g_reply_int);
    if ( g_reply_int ==100)
    {
        if (Hv_poinfo_Ins(hv_poinfo ,g_pub_tx.reply ))
        {
            sprintf( acErrMsg,"插入一条新记录出错！[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"插入一条新记录出错！");
            WRITEMSG
                return (NULL);
        }
    }
    else
    {
        vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
        memcpy(hv_poinfo.or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_poinfo.or_br_no)-1);
        memcpy(hv_poinfo.pay_qs_no,cPay_qs_no,sizeof(hv_poinfo.pay_qs_no)-1);
        vtcp_log("[%s][%d]hv_poinfo.l_tx_date***********=[%ld]",__FILE__,__LINE__,hv_poinfo.l_tx_date);
        if (hv_poinfo.l_tx_date-apatoi(hv_notpay.F_tx_date,8))
        {
            hv_poinfo.l_tx_date=apatoi(hv_notpay.F_tx_date,8);
            vtcp_log("[%s][%d]hv_poinfo.l_tx_date***********=[%ld]",__FILE__,__LINE__,hv_poinfo.l_tx_date);
            hv_poinfo.td_cnt=1;
        }
        else
            hv_poinfo.td_cnt++;
        dot_str2dbl( hv_721_add.real_js_amt,sizeof(hv_721_add.real_js_amt),&hv_poinfo.cash_amt);
        hv_poinfo.cash_amt = hv_poinfo.cash_amt/100.0;
        dot_str2dbl( hv_721_add.dy_amt,sizeof(hv_721_add.dy_amt),&hv_poinfo.rem_amt);
        hv_poinfo.rem_amt = hv_poinfo.rem_amt/100.0;
        memcpy(hv_poinfo.cash_br_no,hv_notpay.F_ac_br_no,sizeof(hv_poinfo.cash_br_no)-1);
        memcpy(hv_poinfo.hold_ac_no,hv_721_add.last_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);
        memcpy(hv_poinfo.hold_name,hv_721_add.last_name,sizeof(hv_poinfo.hold_name)-1);
        vtcp_log("[%s][%d]hv_poinfo.l_tx_date***********=[%ld]",__FILE__,__LINE__,hv_poinfo.l_tx_date);
        /*如果是本行汇票则判断只能是移存状态*/
        if (memcmp(hv_poinfo.pay_qs_no, cPay_qs_no, sizeof(hv_poinfo.pay_qs_no)-1)==0)
        {
            vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
            if (hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER && hv_poinfo.po_sts[0] != 
                HV_HP_CANCEL && hv_poinfo.po_sts[0] !=HV_HP_UNCASH)
            {
                sprintf(acErrMsg,"汇票状态错[%s][%d]!",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"汇票状态错");
                WRITEMSG
                    return 1;
            }
        }
        else
        {
            /*如果是他行汇票只能是汇票注销状态才能申请清算*/
            vtcp_log("[%s][%d]hv_poinfo.po_sts[0]=[%c]",__FILE__,__LINE__,hv_poinfo.po_sts[0]);
            if (hv_poinfo.po_sts[0] != HV_HP_CANCEL)
            {
                sprintf(acErrMsg,"汇票状态错[%s][%d]!",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"汇票状态错");
                WRITEMSG
                    return 1;
            }
        }
        hv_poinfo.po_sts[0] = HV_HP_ASKCASH;
        hv_poinfo.pay_date = apatoi(hv_notpay.F_tx_date,8);
        hv_poinfo.add_pwd_ind[0]='1';
        vtcp_log("[%s][%d]po_no         =[%s]",__FILE__,__LINE__,hv_poinfo.po_no        ); 
        vtcp_log("[%s][%d]br_no         =[%s]",__FILE__,__LINE__,hv_poinfo.br_no        ); 
        vtcp_log("[%s][%d]bank_type     =[%s]",__FILE__,__LINE__,hv_poinfo.bank_type    ); 
        vtcp_log("[%s][%d]or_br_no      =[%s]",__FILE__,__LINE__,hv_poinfo.or_br_no     ); 
        vtcp_log("[%s][%d]pay_qs_no     =[%s]",__FILE__,__LINE__,hv_poinfo.pay_qs_no    ); 
        vtcp_log("[%s][%d]l_tx_date     =[%ld]",__FILE__,__LINE__,hv_poinfo.l_tx_date    );
        vtcp_log("[%s][%d]td_cnt        =[%ld]",__FILE__,__LINE__,hv_poinfo.td_cnt       );
        vtcp_log("[%s][%d]lost_cnt      =[%ld]",__FILE__,__LINE__,hv_poinfo.lost_cnt     );
        vtcp_log("[%s][%d]note_type     =[%s]",__FILE__,__LINE__,hv_poinfo.note_type    ); 
        vtcp_log("[%s][%d]note_no       =[%s]",__FILE__,__LINE__,hv_poinfo.note_no      ); 
        vtcp_log("[%s][%d]o_note_type   =[%s]",__FILE__,__LINE__,hv_poinfo.o_note_type  ); 
        vtcp_log("[%s][%d]o_note_no     =[%s]",__FILE__,__LINE__,hv_poinfo.o_note_no    ); 
        vtcp_log("[%s][%d]po_bal        =[%f]",__FILE__,__LINE__,hv_poinfo.po_bal       ); 
        vtcp_log("[%s][%d]cash_amt      =[%f]",__FILE__,__LINE__,hv_poinfo.cash_amt     ); 
        vtcp_log("[%s][%d]fee_amt       =[%f]",__FILE__,__LINE__,hv_poinfo.fee_amt      ); 
        vtcp_log("[%s][%d]rem_amt       =[%f]",__FILE__,__LINE__,hv_poinfo.rem_amt      ); 
        vtcp_log("[%s][%d]cur_id        =[%s]",__FILE__,__LINE__,hv_poinfo.cur_id       ); 
        vtcp_log("[%s][%d]pay_br_no     =[%s]",__FILE__,__LINE__,hv_poinfo.pay_br_no    ); 
        vtcp_log("[%s][%d]pay_ac_no     =[%s]",__FILE__,__LINE__,hv_poinfo.pay_ac_no    ); 
        vtcp_log("[%s][%d]pay_name      =[%s]",__FILE__,__LINE__,hv_poinfo.pay_name     ); 
        vtcp_log("[%s][%d]pay_br_name   =[%s]",__FILE__,__LINE__,hv_poinfo.pay_br_name  ); 
        vtcp_log("[%s][%d]pay_addr      =[%s]",__FILE__,__LINE__,hv_poinfo.pay_addr     ); 
        vtcp_log("[%s][%d]cash_ac_no    =[%s]",__FILE__,__LINE__,hv_poinfo.cash_ac_no   ); 
        vtcp_log("[%s][%d]cash_br_no    =[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no   ); 
        vtcp_log("[%s][%d]cash_br_name  =[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_name ); 
        vtcp_log("[%s][%d]cash_addr     =[%s]",__FILE__,__LINE__,hv_poinfo.cash_addr    ); 
        vtcp_log("[%s][%d]cash_name     =[%s]",__FILE__,__LINE__,hv_poinfo.cash_name    ); 
        vtcp_log("[%s][%d]hold_ac_no    =[%s]",__FILE__,__LINE__,hv_poinfo.hold_ac_no   ); 
        vtcp_log("[%s][%d]hold_name     =[%s]",__FILE__,__LINE__,hv_poinfo.hold_name    ); 
        vtcp_log("[%s][%d]l_po_sts      =[%s]",__FILE__,__LINE__,hv_poinfo.l_po_sts     ); 
        vtcp_log("[%s][%d]po_sts        =[%s]",__FILE__,__LINE__,hv_poinfo.po_sts       ); 
        vtcp_log("[%s][%d]reg_date      =[%ld]",__FILE__,__LINE__,hv_poinfo.reg_date     );
        vtcp_log("[%s][%d]sign_date     =[%ld]",__FILE__,__LINE__,hv_poinfo.sign_date    );
        vtcp_log("[%s][%d]pay_date      =[%ld]",__FILE__,__LINE__,hv_poinfo.pay_date     );
        /*vtcp_log("[%s][%d]pwd           =[%s]",__FILE__,__LINE__,hv_poinfo.pwd          ); */           
        vtcp_log("[%s][%d]po_ind        =[%s]",__FILE__,__LINE__,hv_poinfo.po_ind    );    
        vtcp_log("[%s][%d]deal_mode     =[%s]",__FILE__,__LINE__,hv_poinfo.deal_mode    ); 
        vtcp_log("[%s][%d]drsubject     =[%s]",__FILE__,__LINE__,hv_poinfo.drsubject    ); 
        vtcp_log("[%s][%d]crsubject     =[%s]",__FILE__,__LINE__,hv_poinfo.crsubject    ); 
        vtcp_log("[%s][%d]content       =[%s]",__FILE__,__LINE__,hv_poinfo.content      );  
        vtcp_log("[%s][%d]tx_chrg_type  =[%s]",__FILE__,__LINE__,hv_poinfo.tx_chrg_type );  
        vtcp_log("[%s][%d]checkflag     =[%s]",__FILE__,__LINE__,hv_poinfo.checkflag    );  
        vtcp_log("[%s][%d]op_tel        =[%s]",__FILE__,__LINE__,hv_poinfo.op_tel       );  
        vtcp_log("[%s][%d]chk_tel       =[%s]",__FILE__,__LINE__,hv_poinfo.chk_tel      );  
        vtcp_log("[%s][%d]pass_tel      =[%s]",__FILE__,__LINE__,hv_poinfo.pass_tel     );  
        vtcp_log("[%s][%d]add_pwd_ind   =[%s]",__FILE__,__LINE__,hv_poinfo.add_pwd_ind  );  
        g_reply_int = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);    
        vtcp_log("[%s][%d]",__FILE__,__LINE__);  
        if ( g_reply_int)
        {
            vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"更新一条新记录出错！[%s][%d]g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
            set_zd_data(DC_GET_MSG,"更新一条新记录出错！");
            WRITEMSG
                return (NULL);
        }
        vtcp_log("[%s][%d]呵呵，马上就要Ok了",__FILE__,__LINE__);
    }   
    Hv_poinfo_Clo_Upd( );
    return 0;
}
/****Add by SSH,2006.8.28,把发送处理做成一个公共函数****/
int hv_sendpack(THVPACKAGE *pHv)
{
    int iRc=0;    
    int ret=0;
    int iSocketID;
    THVPACKAGE respack;
    char tmpBuf[300];
    char cTmpbuff[9];
    char filename[128];
    char cOrderno[9];
    char cTxday[9];
    char cTx_date[9];
    char cCmtno[4];
    char cOrbrno[13];
    char cTmpbuff2[9];
    char cTemp1[9];
    char cTemp2[9];
    char tmpBuf0[6];
    char tmpBuf1[11];
    char tmpBuf2[11];
    
    int     recdnum=0;
    long    length=0;
    struct hvstat_c hv_stat;
    struct  data_dic_c  data_dic;
    
    memcpy(tmpBuf0,Pu_Val[0].sjbm,sizeof(tmpBuf0)-1);/*第一个数据编码的值*/
    sprintf(tmpBuf1,"%ld",Pu_Val);               /*c_8583_buf首地址*/
    sprintf(tmpBuf2,"%ld",sizeof(THVPACKAGE));    /*pack_zf的长度*/

    iChkSendRecord("100",tmpBuf0,tmpBuf1,"打印变量",tmpBuf2);
    MEMSET_DEBUG(&data_dic,0,sizeof(data_dic));
    MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
    MEMSET_DEBUG(cTx_date,0,sizeof(cTx_date));
    
    recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
    if (recdnum<=0) 
    {
        vtcp_log("数据字典无内容!!ERROR %d",recdnum);
        return -1;
    }

    ret = sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
    if ( ret )
    {
        vtcp_log("数据字典sum!!ERROR");
        return -1;
    }
    ret=Hvstat_Sel(g_pub_tx.reply,&hv_stat,"1=1");
    if(ret!=0){
            vtcp_log("%s,%d,取得营业状态错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
            strncpy(acErrMsg,"取得营业状态错误",40);
            set_zd_data("0130",acErrMsg);
            return(-1);
    }
    
    apitoa(hv_stat.tx_date,sizeof(cTx_date)-1,cTx_date);
    vtcp_log("[%s][%d]tx_date=[%s]",__FILE__,__LINE__,cTx_date);
    char *c_8583_buf2 =  (unsigned char *)malloc (sizeof(struct PVal_St) *recdnum);        /**存放8583域***/
    char *c_8583_buf3 =  (unsigned char *)malloc (sizeof(unsigned char)*length+2);        /**存放8583域***/
    
    MEMSET_DEBUG(c_8583_buf2,0x20,sizeof(struct PVal_St) *recdnum);
    MEMSET_DEBUG(c_8583_buf3,0x20,length+2);
    
    memcpy(c_8583_buf2,cTmpfp,sizeof(struct PVal_St) *recdnum);
    memcpy(c_8583_buf3,cTmpfp1,length+2);

    iChkSendMemory(cTx_date,c_8583_buf2,c_8583_buf3,"sendpack");

    MEMSET_DEBUG(&respack,'\0',sizeof(respack));
    MEMSET_DEBUG(tmpBuf,'\0',sizeof(tmpBuf));
    MEMSET_DEBUG(cTmpbuff,'\0',sizeof(cTmpbuff));
    MEMSET_DEBUG(cTmpbuff2,'\0',sizeof(cTmpbuff2));
    MEMSET_DEBUG(cOrderno,0,sizeof(cOrderno));
    MEMSET_DEBUG(cCmtno  ,0,sizeof(cCmtno));
    MEMSET_DEBUG(cOrbrno ,0,sizeof(cOrbrno));
    MEMSET_DEBUG(cTxday  ,0,sizeof(cTxday));
    MEMSET_DEBUG(cTemp1  ,0,sizeof(cTemp1));
    MEMSET_DEBUG(cTemp2  ,0,sizeof(cTemp2));

    memcpy(cCmtno ,pHv->CMTCode,sizeof(cCmtno)-1);
    hv_get_tag(pHv,"0BC",cOrderno);
    hv_get_tag(pHv,"52A",cOrbrno);
    hv_get_tag(pHv,"30A",cTxday);
    
    memcpy(cTemp1,"11111111",sizeof(cTemp1)-1);
    set_zd_data("1280",cTemp1);
    get_fd_data("1280",cTemp2);
    if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
    {
        /****重新设置8583****/
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"这里开始重新设置8583");
        iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
        iReset_8583(c_8583_buf2,c_8583_buf3);
        vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
    }
    else
    {
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"1111开始保存8583内容");
        iGet_8583(c_8583_buf2,c_8583_buf3);
        vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
    }  
    iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
    
    if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
        || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
        || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
    {
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"1111开始调用sendpack");
    } 
    iRc=hv_initPara(NULL);
    if (iRc<0)
    {
        sprintf(acErrMsg,"初始化网络参数失败!\n");
        set_zd_data(DC_GET_MSG,"初始化网络参数失败");
        WRITEMSG
        free(c_8583_buf2);
        free(c_8583_buf3);
        return(-1);
    }
    memcpy(cTemp1,"22222222",sizeof(cTemp1)-1);
    set_zd_data("1280",cTemp1);
    get_fd_data("1280",cTemp2);
    if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
    {
        /****重新设置8583****/
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"2222开始重新设置8583");
        iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
        iReset_8583(c_8583_buf2,c_8583_buf3);    
        vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
    }
    else
    {
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"2222开始保存8583内容");
        iGet_8583(c_8583_buf2,c_8583_buf3);
        vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
    }  
    iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
    
    /*发送报文到前置机**/
    if ((char *)getenv("HVSENDTYPE")!=NULL && !memcmp((char*)getenv("HVSENDTYPE"),"NO_SEND",8))
    {
        /****模拟,不实际发送****/
        free(c_8583_buf2);
        free(c_8583_buf3);
        return(0);    
    }
    else
    {
        memcpy(cTemp1,"33333333",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"3333开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"3333开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
    
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"**hv_connect begin**");
        }
        
        
        memcpy(cTemp1,"44444444",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"4444开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"4444开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        
        iSocketID=hv_connect(HV_SERVER);
        if (iSocketID<=0) 
        {
            char filename[64];
            fcopy(filename,pHv,FLAG_SENDER);
            sprintf(acErrMsg,"无法连接大额支付前置机!\n");
            set_zd_data(DC_GET_MSG,"无法连接大额支付前置机");
            WRITEMSG
            free(c_8583_buf2);
            free(c_8583_buf3);
            return(-1);    
        }
        
        
        memcpy(cTemp1,"55555555",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
       if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
       {
           /****重新设置8583****/
           iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"5555开始重新设置8583");
           iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
           iReset_8583(c_8583_buf2,c_8583_buf3);    
           vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
       }
       else
       {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"5555开始保存8583内容");
           iGet_8583(c_8583_buf2,c_8583_buf3);
           vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
       } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        
        vtcp_log("[%s][%d]cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"**hv_connect  over**");
        }
        
        memcpy(cTemp1,"66666666",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"6666开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"6666开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        iRc=hv_send(iSocketID,pHv->body);
        if (iRc<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"无法发送报文到大额支付前置机!\n");
            set_zd_data(DC_GET_MSG,"无法发送报文到大额支付前置机");
            WRITEMSG
            free(c_8583_buf2);
            free(c_8583_buf3);
            return(-1);    
        }
        
        memcpy(cTemp1,"00000000",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"0000开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"0000开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        
        
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"***hv_send over***");
        }
        
        memcpy(cTemp1,"88888888",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"8888开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"8888开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        iRc=hv_recv(iSocketID,respack.body);
        if (iRc<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"接收大额支付前置机响应失败!\n");
            set_zd_data(DC_GET_MSG,"接收大额支付前置机响应失败");
            WRITEMSG
            free(c_8583_buf2);
            free(c_8583_buf3);
            return(-1);    
        }
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"***hv_recv over***");
        }
        
        memcpy(cTemp1,"99999999",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"9999开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"9999开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
        hv_netClose(iSocketID);
        iRc=hv_analyze_pack(&respack);
        if (iRc<0)
        {
            sprintf(acErrMsg,"接收大额支付前置机响应报文非法!\n");
            set_zd_data(DC_GET_MSG,"接收大额支付前置机响应报文非法");
            WRITEMSG
            free(c_8583_buf2);
            free(c_8583_buf3);
            return(-1);    
        }
        
        memcpy(cTemp1,"77777777",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"7777开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"7777开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        }
        
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
    
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0 \
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0 \
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"hv_analyze_pack over");
        }
        
        
        memcpy(cTemp1,"1515151a",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
        if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"1515开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
              iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"1515开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
      
        iRc=hv_get_tag(&respack,"C42",tmpBuf);
        
        
        memcpy(cTemp1,"bbbbbbbb",sizeof(cTemp1)-1);
        set_zd_data("1280",cTemp1);
        get_fd_data("1280",cTemp2);
         if(memcmp(cTemp1,cTemp2,sizeof(cTemp2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"bbbb开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTemp1);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
        }
        else
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"bbbb开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        } 
        
        
        iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTemp2);
    
        memcpy(cTmpbuff,tmpBuf,sizeof(cTmpbuff)-1);
        
        
        set_zd_data("108J",cTmpbuff);
        get_fd_data("108J",cTmpbuff2);
        if(memcmp(cTmpbuff,cTmpbuff2,sizeof(cTmpbuff2)-1)!=0)
        {
            /****重新设置8583****/
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"108J开始重新设置8583");
            iChkSendMemory(cTxday,c_8583_buf2,c_8583_buf3,cTmpbuff);
            iReset_8583(c_8583_buf2,c_8583_buf3);    
            vtcp_log("[%s][%d]重新设置8583域完毕!\n",__FILE__,__LINE__);
            set_zd_data("108J",cTmpbuff);
            get_fd_data("108J",cTmpbuff2);
            if(memcmp(cTmpbuff,cTmpbuff2,sizeof(cTmpbuff2)-1)!=0)
            {
            	  iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"这里开始重新cTmpbuff1");
            	  iReset_8583(c_8583_buf2,c_8583_buf3); 
            	  set_zd_data("108J",cTmpbuff);
                get_fd_data("108J",cTmpbuff2); 
                if(memcmp(cTmpbuff,cTmpbuff2,sizeof(cTmpbuff2)-1)!=0)
                {
                	 iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"重新cTmpbuff失败2");
                }
                else
                {
                	 iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"重新cTmpbuff成功2");
                }
            }
            else
            {
            	 iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"重新cTmpbuff成功0");
            }
        }
        else
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"这里开始保存8583内容");
            iGet_8583(c_8583_buf2,c_8583_buf3);
            vtcp_log("[%s][%d]c_8583_buf=[%s]",__FILE__,__LINE__,c_8583_buf2);
        }
    
    
        vtcp_log("[%s][%d]cTmpbuff=[%s]",__FILE__,__LINE__,cTmpbuff);
        if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0\
            || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0\
            || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
        {
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTmpbuff);
            iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,cTmpbuff2);
        }    
        if (tmpBuf[3]!='I')
        {
            MEMSET_DEBUG(tmpBuf,'\0',sizeof(tmpBuf));
            hv_get_tag(&respack,"72A",tmpBuf);
            if (memcmp(cCmtno,"100",3)==0 || memcmp(cCmtno,"101",3)==0 || memcmp(cCmtno,"102",3)==0\
                || memcmp(cCmtno,"103",3)==0 || memcmp(cCmtno,"104",3)==0 || memcmp(cCmtno,"105",3)==0\
                || memcmp(cCmtno,"108",3)==0 || memcmp(cCmtno,"109",3)==0 || memcmp(cCmtno,"121",3)==0)
            {
                iChkSendRecord(cCmtno,cOrderno,cOrbrno,cTxday,"前置机回应了错误信息");
            }
            sprintf(acErrMsg,"前置机回应了错误信息[%s]!",cTmpbuff);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            free(c_8583_buf2);
            free(c_8583_buf3);
            return(-1);    
        }
    }
    free(c_8583_buf2);
    free(c_8583_buf3);
    return(0);
}
/****************************************************************************/
/* 函数名称: cGetSysDate                                                                                                    */
/* 功能描述: 得到系统日期，char 型                                                              */
/* 创建日期: 2006-9-8 17:49                                                 */
/* 作者:     xyy                                                            */
/****************************************************************************/
char * cGetSysDate()
{
    int iYear=0,iMon=0,iDay=0;
    static char cDate[9];
    char cYear[5];
    char cMon[3];
    char cDay[3];
    MEMSET_DEBUG(cDate,0,sizeof(cDate));
    MEMSET_DEBUG(cYear,0,sizeof(cYear));
    MEMSET_DEBUG(cMon,0,sizeof(cMon));
    MEMSET_DEBUG(cDay,0,sizeof(cDay));

    pub_base_get_date(&iYear,&iMon,&iDay);

    apitoa(iYear,sizeof(cYear)-1,cYear);
    apitoa(iMon,sizeof(cMon)-1,cMon);
    apitoa(iDay,sizeof(cDay)-1,cDay);

    sprintf(cDate,"%s%s%s",cYear,cMon,cDay);

    return(cDate);
}
/****************************************************************************/
/* 函数名称: iFzfhtrFromLvdef(NOTPAY_IN_AREA * lv_notpay)                                  */
/* 功能描述: 从小额的lv_define的非支付交易字段，给大额非支付历史明细表赋值  */
/* 创建日期: 2006-9-12 13:54                                                */
/* 作者:     xyy                                                            */
/****************************************************************************/
int iFzfhtrFromLvdef(NOTPAY_IN_AREA lv_notpay)
{
    struct hv_fzfhtr_c hv_fzfhtr;
    MEMSET_DEBUG(&hv_fzfhtr,0,sizeof(hv_fzfhtr));
    memcpy(hv_fzfhtr.or_br_no    ,lv_notpay.orbrno       ,sizeof(hv_fzfhtr.or_br_no   )-1);
    memcpy(hv_fzfhtr.pay_qs_no   ,lv_notpay.payqsactno   ,sizeof(hv_fzfhtr.pay_qs_no  )-1);
    memcpy(hv_fzfhtr.ac_br_no    ,lv_notpay.acbrno              ,sizeof(hv_fzfhtr.ac_br_no   )-1); 
    memcpy(hv_fzfhtr.cash_qs_no  ,lv_notpay.cashqsactno  ,sizeof(hv_fzfhtr.cash_qs_no )-1);
    memcpy(hv_fzfhtr.orderno     ,lv_notpay.orderno             ,sizeof(hv_fzfhtr.orderno    )-1);             
    memcpy(hv_fzfhtr.cmtno       ,lv_notpay.cmtno               ,sizeof(hv_fzfhtr.cmtno      )-1); 
    memcpy(hv_fzfhtr.sendco      ,lv_notpay.sendco       ,sizeof(hv_fzfhtr.sendco     )-1);
    memcpy(hv_fzfhtr.receco      ,lv_notpay.receco       ,sizeof(hv_fzfhtr.receco     )-1);
    memcpy(hv_fzfhtr.addid       ,lv_notpay.addid        ,sizeof(hv_fzfhtr.addid      )-1);
    memcpy(hv_fzfhtr.hv_brf      ,lv_notpay.content             ,sizeof(hv_fzfhtr.hv_brf     )-1); 
    memcpy(hv_fzfhtr.o_or_br_no  ,lv_notpay.oorbrno          ,sizeof(hv_fzfhtr.o_or_br_no )-1); 
    memcpy(hv_fzfhtr.o_ac_br_no  ,lv_notpay.oacbrno          ,sizeof(hv_fzfhtr.o_ac_br_no )-1); 
    memcpy(hv_fzfhtr.o_orderno   ,lv_notpay.oorderno         ,sizeof(hv_fzfhtr.o_orderno  )-1); 
    memcpy(hv_fzfhtr.o_cur_ind   ,lv_notpay.cour              ,sizeof(hv_fzfhtr.o_cur_ind  )-1); 
    memcpy(hv_fzfhtr.o_cmtno     ,lv_notpay.opkgno       ,sizeof(hv_fzfhtr.o_cmtno    )-1);
    memcpy(hv_fzfhtr.req_orderno ,lv_notpay.qorderno         ,sizeof(hv_fzfhtr.req_orderno)-1); 
    memcpy(hv_fzfhtr.req_br_no   ,lv_notpay.qorbrno             ,sizeof(hv_fzfhtr.req_br_no  )-1); 
    memcpy(hv_fzfhtr.req_content ,lv_notpay.content             ,sizeof(hv_fzfhtr.req_content)-1); 
    memcpy(hv_fzfhtr.res_orderno ,lv_notpay.rorderno         ,sizeof(hv_fzfhtr.res_orderno)-1); 
    memcpy(hv_fzfhtr.res_br_no   ,lv_notpay.racbrno             ,sizeof(hv_fzfhtr.res_br_no  )-1);  
    memcpy(hv_fzfhtr.res_content ,lv_notpay.content             ,sizeof(hv_fzfhtr.res_content)-1); 
    hv_fzfhtr.res_sts[0]        =  lv_notpay.respcode;     
    hv_fzfhtr.o_tx_type[0]          =  lv_notpay.otxtype;        
    hv_fzfhtr.hv_fzfhtr_sts[0]  =  lv_notpay.stat;    

    /******整型变量之间的转换*******/
    hv_fzfhtr.tx_date   = apatoi(lv_notpay.txday ,8);
    hv_fzfhtr.wt_date   = apatoi(lv_notpay.txday ,8);
    hv_fzfhtr.trace_no  = apatoi(lv_notpay.wssrno,6);
    hv_fzfhtr.o_tx_date = apatoi(lv_notpay.owtday,8);
    hv_fzfhtr.req_date  = apatoi(lv_notpay.qwtday,8);
    hv_fzfhtr.res_date  = apatoi(lv_notpay.rwtday,8);
    str2dbl(lv_notpay.otxamt,sizeof(lv_notpay.otxamt),0,&hv_fzfhtr.o_tx_amt);
}
/******************************************
*监控 hv_sendpack程序
* add by xyy 2006-11-21 23:14
******************************************/
int iChkSendRecord(char *cmtno,char* orderno,char *orbrno ,char * txday,char *errmsg)
{
    int  iFd=0,iSize=0;
    char cFilename[513];
    char cBasename[15];
    char cTxday[9];
    struct STR_ChkSend str_chkRecord;
    memcpy(cTxday, 0 , sizeof(cTxday));
    memcpy(cTxday,txday,sizeof(cTxday)-1);
    MEMSET_DEBUG(&str_chkRecord, 0 , sizeof(str_chkRecord));
    MEMSET_DEBUG(cFilename, 0 ,sizeof(cFilename));
    MEMSET_DEBUG(cBasename, 0 ,sizeof(cBasename));
    memcpy(cBasename,"ch",2);
    memcpy(cBasename+2,cTxday,8);
    memcpy(cBasename+10,".txt",4);
    sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
    vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
    iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
    if (iFd==-1)
    {
        vtcp_log("[%s][%d]文件描述符号==[%d] \n",__FILE__,__LINE__,iFd);
        vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
        vtcp_log("[%s][%d] 打开准备写入的文件[%s]错误 \n",__FILE__,__LINE__,cFilename);
        return -1;
    }
    vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,orderno);
    vtcp_log("[%s][%d]orbrno =[%s]",__FILE__,__LINE__,orbrno);
    vtcp_log("[%s][%d]txday  =[%s]",__FILE__,__LINE__,txday);
    vtcp_log("[%s][%d]errmsg =[%s]",__FILE__,__LINE__,errmsg);
    memcpy(str_chkRecord.cmtno   ,cmtno,sizeof(str_chkRecord.cmtno  ));
    str_chkRecord.separator1='|';
    memcpy(str_chkRecord.orderno ,orderno,sizeof(str_chkRecord.orderno  ));
    str_chkRecord.separator2='|';
    memcpy(str_chkRecord.orbrno  ,orbrno ,sizeof(str_chkRecord.orbrno  ));
    str_chkRecord.separator3='|';
    memcpy(str_chkRecord.txday   ,txday  ,sizeof(str_chkRecord.txday  ));
    str_chkRecord.separator4='|';
    memcpy(str_chkRecord.errmsg  ,errmsg ,sizeof(str_chkRecord.errmsg));
    str_chkRecord.separator5='|';
    str_chkRecord.nextline='\n';

    iSize=write(iFd,(char *)&str_chkRecord,sizeof(str_chkRecord));
    if (iSize<=0)
    {
        vtcp_log("没有数据被写入 size=[%d] \n",iSize);
        return -1;
    }
    if (close(iFd))
    {
        vtcp_log("[%s][%d] close id error \n",__FILE__,__LINE__);
        return -1;
    }
    return 0;
}


/**************打印出8583域的内容***********/
int print_8583_area()
{
    char cFilename[150];
    FILE *fp;
    int i_fp;
    long length;
    char fmt[10];
    char cTmplist[1024];
    MEMSET_DEBUG(fmt,0x00,sizeof(fmt));
    
    MEMSET_DEBUG(cFilename,0x00,sizeof(cFilename));
    MEMSET_DEBUG(cTmplist,0x00,sizeof(cTmplist));

    int recordnum = 0;
    int index = 0;
    
    recordnum = sql_count ( "data_dic" , "%s" , "1=1" );
    sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
    
    sprintf(cFilename,"%s/%s",(char *) getenv("HOME"),"8583.txt");
    vtcp_log("[%s][%d]文件名字=[%s]\n",__FILE__,__LINE__,cFilename);
    
    vtcp_log("[%s][%d]8585域中一共有[%d]个数据编码!总的8585域的长度=[%ld]\n",__FILE__,__LINE__,recordnum,length);
    
    i_fp = open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
    if(i_fp==-1)
    {
        vtcp_log("[%s][%d]打开或者创建文件失败!fp==[%d]\n",__FILE__,__LINE__,i_fp);
        
        return (-1);
    }
    
    close(i_fp);
    
    length = 1;
    fp = fopen(cFilename,"a+");
    if(fp==NULL)
    {
        vtcp_log("[%s][%d]打开文件失败!\n",__FILE__,__LINE__);
    }    
    fprintf(fp,"Pub_Buf===[%s]\n",Pub_Buf);
    fprintf(fp,"Old_Pub_Buf===[%s]\n",Old_Pub_Buf);
    for(index=0;index<recordnum;index++)
    {
        MEMSET_DEBUG(cTmplist,0x00,sizeof(cTmplist));
        MEMSET_DEBUG(fmt,    0x00,sizeof(fmt));
            
        sprintf(fmt,"%%.%ds",Pu_Val[index].length);
        sprintf(cTmplist,fmt,Pu_Val[index].val);
        fprintf(fp,"---[%d]-----[%d]----------------------\n",length,length+Pu_Val[index].length);
        fprintf(fp,"数据编码=[%s]\n",Pu_Val[index].sjbm);
        fprintf(fp,"数据长度=[%d]\n",Pu_Val[index].length);
        fprintf(fp,"数据精度=[%d]\n",Pu_Val[index].prec);
        fprintf(fp,"数据类型=[%d]\n",Pu_Val[index].lx);
        fprintf(fp,"数据值域=[%s]\n\n\n",cTmplist);
        
        length += Pu_Val[index].length;
            
    }
    fclose(fp);
    
    vtcp_log("[%s][%d]写文件成功!\n",__FILE__,__LINE__);
    return 0;
}

int iReset_8583(char *buf,char *buf1)
{
    vtcp_log("[%s][%d]开始重新设置8583域!\n",__FILE__,__LINE__);
    int     ret=0;
    int     recdnum=0;
    long    length=0;
    struct  data_dic_c  data_dic;
    MEMSET_DEBUG(&data_dic,0,sizeof(data_dic));

    recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
    if (recdnum<=0) 
    {
        vtcp_log("数据字典无内容!!ERROR %d",recdnum);
        return -1;
    }

    ret = sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
    if ( ret )
    {
        vtcp_log("数据字典sum!!ERROR");
        return -1;
    }
    memcpy(Pu_Val   ,buf  , sizeof(struct PVal_St) *recdnum);
    memcpy(Pub_Buf  ,buf1 , length+2);
        
    vtcp_log("[%s][%d]重新设置8583域成功!\n",__FILE__,__LINE__);
    return 0;
    
}

int iGet_8583(char *buf,char *buf1)
{
    vtcp_log("[%s][%d]开始iGet_8583设置8583域!\n",__FILE__,__LINE__);
    int     ret=0;
    int     recdnum=0;
    long    length=0;
    struct  data_dic_c  data_dic;
    MEMSET_DEBUG(&data_dic,0,sizeof(data_dic));

    recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
    if (recdnum<=0) 
    {
        vtcp_log("数据字典无内容!!ERROR %d",recdnum);
        return -1;
    }

    ret = sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
    if ( ret )
    {
        vtcp_log("数据字典sum!!ERROR");
        return -1;
    }
    memcpy(buf  ,Pu_Val   , sizeof(struct PVal_St) *recdnum);
    memcpy(buf1 ,Pub_Buf  , length+2);
    vtcp_log("[%s][%d]Pu_Val =[%s]",__FILE__,__LINE__,Pu_Val);
    vtcp_log("[%s][%d]Pub_Buf=[%s]",__FILE__,__LINE__,Pub_Buf);
    vtcp_log("[%s][%d]buf    =[%s]",__FILE__,__LINE__,buf);
    vtcp_log("[%s][%d]buf1   =[%s]",__FILE__,__LINE__,buf1);
    
    vtcp_log("[%s][%d]设置8583域成功!\n",__FILE__,__LINE__);
    return 0;
    
}

/**************打印出8583域的内容***********/
int iChkSendMemory(char * txday,char *ctmpbuf1,char *ctmpbuf2,char * ctmpbuf3)
{
    char cFilename[150];
    FILE *fp;
    int i_fp;
    char fmt[10];
    char cBasename[15];
    char cTxday[9];
    char cTmplist[1024];
    char cSpace[2];
    MEMSET_DEBUG(fmt,0x00,sizeof(fmt));
    
    MEMSET_DEBUG(cFilename,0x00,sizeof(cFilename));
    MEMSET_DEBUG(cTmplist,0x00,sizeof(cTmplist));
    MEMSET_DEBUG(cBasename, 0 ,sizeof(cBasename));
    MEMSET_DEBUG(cSpace,'\n',sizeof(cSpace));
    
    int     ret=0;
    int     recdnum=0;
    long    length=0;
    struct  data_dic_c  data_dic;
    MEMSET_DEBUG(&data_dic,0,sizeof(data_dic));

    recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
    if (recdnum<=0) 
    {
        vtcp_log("数据字典无内容!!ERROR %d",recdnum);
        return -1;
    }

    ret = sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
    if ( ret )
    {
        vtcp_log("数据字典sum!!ERROR");
        return -1;
    }
    
    memcpy(cTxday, 0 , sizeof(cTxday));
    
    memcpy(cTxday,txday,sizeof(cTxday)-1);
    memcpy(cBasename,"cm",2);
    memcpy(cBasename+2,cTxday,8);
    memcpy(cBasename+10,".txt",4);
    sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
    vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
    
    i_fp = open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
    if(i_fp==-1)
    {
        vtcp_log("[%s][%d]打开或者创建文件失败!fp==[%d]\n",__FILE__,__LINE__,i_fp);
        
        return (-1);
    }
    
    close(i_fp);
    
    fp = fopen(cFilename,"a+");
    if(fp==NULL)
    {
        vtcp_log("[%s][%d]打开文件失败!\n",__FILE__,__LINE__);
    }    
    fwrite(cSpace  ,2,1,fp);
    fwrite(ctmpbuf1,sizeof(struct PVal_St)*recdnum,1,fp);
    fwrite(cSpace  ,2,1,fp);
    fwrite(ctmpbuf2,length+2,1,fp);
    fwrite(cSpace  ,2,1,fp);
    fwrite(ctmpbuf3,strlen(ctmpbuf3),1,fp);
    fwrite(cSpace  ,2,1,fp);
    fclose(fp);
    
    vtcp_log("[%s][%d]写文件成功!\n",__FILE__,__LINE__);
    return 0;
}
