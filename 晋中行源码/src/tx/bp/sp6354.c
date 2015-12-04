/*****************************************************
* 文 件 名:  sp6354.c
* 功能描述： 实时借记回执信息查询
* 作    者:  xyy 
* 完成日期： 2008-4-21 10:58 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "public.h"
#include "lvnet.h"
#include "lv_pub.h"
#include "lv_pkgreg_c.h"
int sp6354()
{
    int ret=0,i;
    char cBuff[2001];
    char cDate[9];
    char cCpDate[9];
    char cPono[21];
    char cKinbr[BRNO_LEN+1];
    char cOCashname[61];
    char cFilename[1000];
    double dCpamt=0.00;
    struct lv_pkgreg_c  nlv_pkgreg;
    struct lv_pkgreg_c  lv_pkgreg;
    
    LV_FD123_AREA fd123;
    
    memset(cPono    , 0 , sizeof(cPono));
    memset(cBuff    , 0 , sizeof(cBuff));
    memset(cDate    , 0 , sizeof(cDate));
    memset(cKinbr   , 0 , sizeof(cKinbr));
    memset(cCpDate  , 0 , sizeof(cCpDate));
    memset(cOCashname,0, sizeof(cOCashname));
    memset(cFilename, 0 , sizeof(cFilename));
    memset(&fd123   , ' ',sizeof(fd123));
    memset(&lv_pkgreg  , 0 ,sizeof(lv_pkgreg));
    memset(&nlv_pkgreg , 0 , sizeof(nlv_pkgreg));
    
    vtcp_log("sp6354 开始！[%s][%d]",__FILE__,__LINE__);
     
    pub_base_sysinit();

    get_zd_data("0030",cKinbr);
    get_fd_data("1230",(char*)&fd123);
    vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
    vtcp_log("[%s,%d]1230-----=[%s]",__FILE__,__LINE__,(char*)&fd123);

    /******获取三个索引的值*******/
    lv_pkgreg.wt_date = apatoi(fd123.wt_date,8);
    memcpy(lv_pkgreg.orderno,fd123.orderno,sizeof(lv_pkgreg.orderno)-1);
    memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
    vtcp_log("+++++lv_pkgreg.or_br_no==[%s]!+++++",lv_pkgreg.or_br_no);
    zip_space(lv_pkgreg.or_br_no);
    if(strlen(lv_pkgreg.or_br_no)==0)
    {
        GetOr_br_noByBr_no(cKinbr, lv_pkgreg.or_br_no );
    }

    vtcp_log("[%s][%d]根据这里锁定小额支付表:wt_date=[%ld],or_br_no=[%s],orderno=[%s]"\
    ,__FILE__,__LINE__,lv_pkgreg.wt_date,lv_pkgreg.or_br_no,lv_pkgreg.orderno);

    ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "orderno='%s' and or_br_no='%s' \
    and wt_date=%ld and pkgno='004' and lw_ind='1' ", lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date,cKinbr);
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    if ( ret==100 )
    {
        sprintf(acErrMsg,"无此小额支付交易!",50);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    else if ( ret )
    {
        sprintf(acErrMsg,"查询小额支付交易基本信息出错",50);
        vtcp_log("%s,%d 查询小额支付交易基本信息出错",__FILE__,__LINE__,cKinbr);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    if(lv_pkgreg.lv_sts[0] !='4') /**往帐发送状态**/
    {
        sprintf(acErrMsg,"原业务非发送状态",50);
        vtcp_log("%s,%d 原业务非发送状态",__FILE__,__LINE__,cKinbr);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    ret = Lv_pkgreg_Sel(g_pub_tx.reply,&nlv_pkgreg, "o_orderno='%s' and o_or_br_no='%s' \
    and o_wt_date=%ld and pkgno='010' and lw_ind='2' ", lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date,cKinbr);
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    if ( ret!=100 && ret!=0 )
    {
        sprintf(acErrMsg,"查询小额支付交易基本信息出错!!",50);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        strncpy(g_pub_tx.reply,"8888",4);
        goto    ErrExit;
    }
    else if ( !ret )
    {
        sprintf(acErrMsg,"已经收到应答,请重新确认",50);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
    apitoa(lv_pkgreg.wt_date,sizeof(fd123.wt_date),fd123.wt_date);
    vtcp_log("[%s][%d]fd123.wt_date=[%.8s] ",__FL__,fd123.wt_date );
    memcpy(fd123.or_br_no        ,lv_pkgreg.or_br_no        ,sizeof(lv_pkgreg.or_br_no   )-1);
    vtcp_log("[%s][%d]fd123.or_br_no=[%.12s] ",__FL__,fd123.or_br_no );
    memcpy(fd123.pkgno       ,lv_pkgreg.pkgno       ,sizeof(lv_pkgreg.pkgno  )-1);   
    vtcp_log("[%s][%d]fd123.pkgno=[%.3s] ",__FL__,fd123.pkgno );
    memcpy(fd123.pay_qs_no       ,lv_pkgreg.pay_qs_no       ,sizeof(lv_pkgreg.pay_qs_no  )-1);   
    vtcp_log("[%s][%d]fd123.pay_qs_no=[%.12s] ",__FL__,fd123.pay_qs_no );
    memcpy(fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(lv_pkgreg.pay_ac_no  )-1);   
    vtcp_log("[%s][%d]fd123.pay_ac_no=[%.32s] ",__FL__,fd123.pay_ac_no );
    memcpy(fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(lv_pkgreg.pay_name   )-1);
    vtcp_log("[%s][%d]fd123.pay_name=[%.60s] ",__FL__,fd123.pay_name );
    memcpy(fd123.pay_addr        ,lv_pkgreg.pay_addr        ,sizeof(lv_pkgreg.pay_addr   )-1);
    vtcp_log("[%s][%d]fd123.pay_addr=[%.60s] ",__FL__,fd123.pay_addr );
    memcpy(fd123.ac_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(lv_pkgreg.ac_br_no   )-1);
    vtcp_log("[%s][%d]fd123.ac_br_no=[%.12s] ",__FL__,fd123.ac_br_no );
    memcpy(fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(lv_pkgreg.cash_ac_no  )-1);
    vtcp_log("[%s][%d]fd123.cash_ac_no=[%.32s] ",__FL__,fd123.cash_ac_no );
    memcpy(fd123.cash_qs_no      ,lv_pkgreg.cash_qs_no      ,sizeof(lv_pkgreg.cash_qs_no  )-1);
    vtcp_log("[%s][%d]fd123.cash_qs_no=[%.12s] ",__FL__,fd123.cash_qs_no );
    memcpy(fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(lv_pkgreg.cash_name   )-1);
    vtcp_log("[%s][%d]fd123.cash_name=[%.60s] ",__FL__,fd123.cash_name );
    memcpy(fd123.cash_addr       ,lv_pkgreg.cash_addr       ,sizeof(lv_pkgreg.cash_addr   )-1);
    vtcp_log("[%s][%d]fd123.cash_addr=[%.60s] ",__FL__,fd123.cash_addr );                 
    memcpy(fd123.orderno         ,lv_pkgreg.orderno         ,sizeof(lv_pkgreg.orderno     )-1);
    vtcp_log("[%s][%d]fd123.orderno=[%.8s] ",__FL__,fd123.orderno );                    
    memcpy(fd123.o_orderno       ,lv_pkgreg.o_orderno       ,sizeof(lv_pkgreg.o_orderno   )-1);
    vtcp_log("[%s][%d]fd123.o_orderno=[%.8s] ",__FL__,fd123.o_orderno );
    memcpy(fd123.pay_opn_br_no   ,lv_pkgreg.pay_opn_br_no   ,sizeof(lv_pkgreg.pay_opn_br_no)-1);
    vtcp_log("[%s][%d]fd123.pay_opn_br_no=[%.12s] ",__FL__,fd123.pay_opn_br_no );
    memcpy(fd123.cash_opn_br_no  ,lv_pkgreg.cash_opn_br_no ,sizeof(lv_pkgreg.cash_opn_br_no)-1);
    vtcp_log("[%s][%d]fd123.cash_opn_br_no=[%.12s] ",__FL__,fd123.cash_opn_br_no );
    memcpy(fd123.txnum           ,lv_pkgreg.txnum           ,sizeof(lv_pkgreg.txnum       )-1);
    vtcp_log("[%s][%d]fd123.txnum=[%.5s] ",__FL__,fd123.txnum );
    memcpy(fd123.yw_type           ,lv_pkgreg.lv_yw_ind     ,sizeof(lv_pkgreg.lv_yw_ind   )-1);
    vtcp_log("[%s][%d]fd123.yw_type=[%.12s] ",__FL__,fd123.yw_type );
    memcpy(fd123.brf           ,lv_pkgreg.lv_brf     ,sizeof(lv_pkgreg.lv_brf       )-1);
    vtcp_log("[%s][%d]fd123.brf=[%.60s] ",__FL__,fd123.brf );
    memcpy(fd123.rcp_sts    ,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));/**应答状态**/
    memcpy(fd123.ret_code    ,lv_pkgreg.retcode,sizeof(fd123.ret_code));/**应答状态**/
    memcpy(fd123.pack_id     ,lv_pkgreg.packid,sizeof(fd123.pack_id));/**原包号**/
    memcpy(fd123.o_or_br_no,lv_pkgreg.o_or_br_no,sizeof(fd123.o_or_br_no));
    vtcp_log("[%s][%d]fd123.o_or_br_no=[%.12s] ",__FL__,fd123.o_or_br_no );
    memcpy(fd123.o_ac_br_no,lv_pkgreg.o_ac_br_no,sizeof(fd123.o_ac_br_no));
    vtcp_log("[%s][%d]fd123.o_ac_br_no=[%.12s] ",__FL__,fd123.o_ac_br_no );
    apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
    apitoa(lv_pkgreg.o_wt_date,sizeof(fd123.o_wt_date),fd123.o_wt_date);/**原包日期**/
    vtcp_log("[%s][%d]lv_pkgreg.o_pack_date=[%.8d] ",__FL__,lv_pkgreg.o_pack_date);
    vtcp_log("[%s][%d]fd123.pack_id=[%.8s] ",__FL__,fd123.pack_id );
    vtcp_log("[%s][%d]fd123.rcp_sts=[%.2s] ",__FL__,fd123.rcp_sts );
    fd123.chrg_ind[0]  = lv_pkgreg.tx_chrg_ind[0];
    fd123.lw_ind[0] = lv_pkgreg.lw_ind[0];
    fd123.lv_sts[0] = lv_pkgreg.lv_sts[0];
    vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );    
    vtcp_log("[%s][%d]fd123.lv_pkgreg.tx_amt*100=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt*100);
    {
        double dTmp1=0.0;
        dTmp1=lv_pkgreg.tx_amt*100.0;
        dbl2str(&dTmp1,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
    }
    vtcp_log("[%s][%d]fd123.tx_amt=[%s]",__FILE__,__LINE__,fd123.tx_amt);
    vtcp_log("[%s][%d]lv_pkgreg.tx_amt=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt);
    vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );
    apitoa(lv_pkgreg.o_wt_date, sizeof(fd123.o_wt_date),fd123.o_wt_date);
    apitoa(lv_pkgreg.wt_date,sizeof(fd123.wt_date),fd123.wt_date);
    vtcp_log("[%s][%d]fd123.o_wt_date=[%s] ",__FL__,fd123.o_wt_date );
    vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
    pub_base_full_space((char*)&fd123,sizeof(fd123));
    vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
    fd123.buff[sizeof(fd123)-1]='\0';
    set_zd_long("0470",lv_pkgreg.wt_date);
    set_zd_data("1230",(char*)&fd123);
    vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
    vtcp_log("&fd123==[%s]\n",&fd123);
    if(!memcmp(lv_pkgreg.txnum,TXNUM_TYJL,sizeof(lv_pkgreg.txnum)-1))
    {
        FILE *fpw;
        PAY_IN_AREA payin;
        
        TYJL1_ADD_AREA lv_tyjl1;
        TYJL4_ADD_AREA lv_tyjl4;
        
        memset(&lv_tyjl1, 0 , sizeof(lv_tyjl1));
        memset(&lv_tyjl4, 0 , sizeof(lv_tyjl4));
        memset(&payin   , 0 , sizeof(payin));
        
        ret=pub_base_AllDwnFilName(cFilename);
        if(ret)
        {
            vtcp_log("[%s][%d] 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
        }
        fpw=fopen(cFilename,"wb+");
        if(fpw==NULL)
        {
            vtcp_log("[%s][%d] 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
        }
        vtcp_log("[%s][%d] 文件名为=[%s]",__FILE__,__LINE__,cFilename);
        memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(lv_pkgreg.addid)-1);
        apitoa(lv_pkgreg.wt_date,sizeof(payin.F_wtday),payin.F_wtday);
        iLvGetAddit(&payin,"001",cBuff,0);
        vtcp_log("[%s][%d] 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
        
        memcpy((char *)&lv_tyjl1,cBuff, sizeof(lv_tyjl1));
        memcpy(cCpDate,lv_tyjl1.cpday,sizeof(lv_tyjl1.cpday));
        vtcp_log("[%s][%d] 出票日期=[%s]",__FL__,cCpDate);
        memcpy(cPono  ,lv_tyjl1.cpno ,sizeof(lv_tyjl1.cpno));
        vtcp_log("[%s][%d] 出票号码=[%s]",__FL__,cPono);
        str2dbl(lv_tyjl1.cpamt,sizeof(lv_tyjl1.cpamt),0,&dCpamt);
        dCpamt = dCpamt/100;
        vtcp_log("[%s][%d] 出票金额=[%f]",__FL__,dCpamt);
        
        set_zd_data("0290",cCpDate);
        set_zd_data("0380",cPono);
        set_zd_double("0390",dCpamt);
        
        vtcp_log("[%s][%d] 写入附加域文件1=[%s]",__FL__,(char *)&lv_tyjl1);
        memset(cBuff, 0 , sizeof(cBuff));
        memcpy(cBuff,(char *)&lv_tyjl1,sizeof(lv_tyjl1));
        fprintf(fpw,"%s",cBuff);
        /**附加域1处理完毕**/

        memset(cBuff    , 0 , sizeof(cBuff));
        memset(&payin   , 0 , sizeof(payin));
        memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(lv_pkgreg.addid)-1);
        apitoa(lv_pkgreg.wt_date,sizeof(payin.F_wtday),payin.F_wtday);
        iLvGetAddit(&payin,"004",cBuff,0);
        vtcp_log("[%s][%d] 得到的附加域4=[%s]",__FILE__,__LINE__,cBuff);
        memcpy((char *)&lv_tyjl4,cBuff, sizeof(lv_tyjl4));
        memcpy(cOCashname,lv_tyjl4.content,sizeof(cOCashname)-1);

        vtcp_log("[%s][%d] 原收款人名称=[%s]",__FL__,cOCashname);
        set_zd_data("0270",cOCashname);
        
        vtcp_log("[%s][%d] 写入附加域文件4=[%s]",__FL__,(char *)&lv_tyjl4);
        memset(cBuff, 0 , sizeof(cBuff));
        memcpy(cBuff,(char *)&lv_tyjl4,sizeof(lv_tyjl4));
        fprintf(fpw,"%s",cBuff);
        
        /**附加域4处理完毕**/

        fclose(fpw);
    }
        
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        memcpy(g_pub_tx.reply,"T063",4);
    }
    sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
