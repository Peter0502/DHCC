/****************************************************************************************
*程序名: spJ95.c
*功能:  代收付业务查询交易
*
*
*编写日期: 20061213
*作者:   Jarod
**
****************************************************************************************/
#include <string.h>
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "lv_pldfr_c.h"
#include "lv_pldsr_c.h"
#include "lv_pldscx_c.h"
#include "lv_pldsfqugx_c.h"
#include "hv_orno_brno_c.h"
extern int iGetDsfStat(char * cStat,char * cString);
extern int dsf_getjjorderno(char *wt_day,char *pack_id,char *order_no,char *outarr);
extern int dsf_getdfdj(char *wt_day,char *pack_id,char *order_no,char *tx_mx_no,char *outarr);
extern int dsf_getdsjj(char *wt_day,char *pack_id,char *order_no,char *tx_mx_no,char *outarr);
extern int pub_base_full_space(char * str,int size);
int spJ095()
{
  int ret=0;
  char wherelist[256];
 
  struct dsfmxqry{
      char wt_date   [8];
      char packid    [8];
      char orderno   [8];
      char tx_mx_no  [8];
      char br_no     [12];
      char yw_type   [5];
      char pay_br_no [12];
      char pay_ac_no [32];
      char qs_no     [12];
      char cash_ac_no[32];
      char cash_br_no[12];
      char repstat   [2];
      char re_content[60];
      char repstat2  [2];
      char xyno      [60];
      char filename  [60];
      char re_content2[60];
      char stat      [16];
      char totamt    [15];
      char otherword [100];
      char pay_name  [60];
  };
  char cOrderno[9];
  char cPackid[9];
  char cWt_date[9];
  char cTx_mx_no[9];
  char cBr_no[6];
  char cTx_type[3];
  char cOr_br_no[13];
  char cOtherword[101];
  char repstat_cont[20];     /*回执状态解释*/
  char repstat2_cont[20];  /**业务回执状态解释**/ 
  char cBuff[257];
  char cStat[3];
  char tmp[9];
  int  length=0;
  char cash_name [60];

  struct lv_pldfr_c sLv_pldfr;
  struct lv_pldsr_c sLv_pldsr;
  struct lv_pldscx_c sLv_pldscx;
  struct lv_pldsfqugx_c sLv_pldsfqugx;
  struct hv_orno_brno_c sHv_orno_brno;
  struct dsfmxqry dsfmx; 

  memset(tmp,       0x00, sizeof(tmp));
  memset(wherelist, 0x00, sizeof(wherelist));
 
  memset(&sLv_pldfr,     0x00, sizeof(struct lv_pldfr_c));
  memset(&sLv_pldsr,     0x00, sizeof(struct lv_pldsr_c));
  memset(&sLv_pldscx,    0x00, sizeof(struct lv_pldscx_c));
  memset(&sLv_pldsfqugx, 0x00, sizeof(struct lv_pldsfqugx_c));
  memset(&sHv_orno_brno, 0x00, sizeof(struct hv_orno_brno_c));
  memset(&dsfmx,         0x00, sizeof(struct dsfmxqry));
  
  memset(cWt_date,      0x00, sizeof(cWt_date));
  memset(cBuff,         0x00, sizeof(cBuff));
  memset(cPackid,       0x00, sizeof(cPackid));
  memset(cBr_no,        0x00, sizeof(cBr_no));
  memset(cStat,         0x00, sizeof(cStat));
  memset(cOrderno,      0x00, sizeof(cOrderno));
  memset(cTx_type,      0x00, sizeof(cTx_type));
  memset(cTx_mx_no,     0x00, sizeof(cTx_mx_no));
  memset(cOr_br_no,     0x00, sizeof(cOr_br_no));
  memset(cOtherword,    0x00, sizeof(cOtherword));
  memset(repstat_cont,  0x00, sizeof(repstat_cont));
  memset(repstat2_cont, 0x00, sizeof(repstat2_cont));
  pub_base_sysinit();

  get_zd_data("0440", cWt_date);
  get_zd_data("0450", cPackid);
  get_zd_data("0460", cOrderno);
  get_zd_data("0280", cTx_mx_no);
  get_zd_data("0030", cBr_no);
  get_zd_data("0670", cTx_type);

  pub_base_strpack_all(cPackid);
  pub_base_strpack_all(cOrderno);
    
/*将packid,orderno的空格用字符0填充*/
  memset(tmp, '0', sizeof(tmp));
  memcpy(tmp+8-strlen(cPackid), cPackid, strlen(cPackid));
  memset(cPackid, 0x00, sizeof(cPackid));
  memcpy(cPackid, tmp, sizeof(cPackid)-1);
  memset(tmp, '0', sizeof(tmp));
  memcpy(tmp+8-strlen(cOrderno), cOrderno, strlen(cOrderno));
  memset(cOrderno, 0x00, sizeof(cOrderno));
  memcpy(cOrderno, tmp, sizeof(cOrderno)-1);
  
  vtcp_log("[%s][%d]Wt_date=[%s]"  , __FILE__,__LINE__, cWt_date );
  vtcp_log("[%s][%d]Packid=[%s]"   , __FILE__,__LINE__, cPackid  );
  vtcp_log("[%s][%d]Orderno=[%s]"  , __FILE__,__LINE__, cOrderno );
  vtcp_log("[%s][%d]Tx_type=[%s]"  , __FILE__,__LINE__, cTx_type );
  vtcp_log("[%s][%d]Tx_mx_no==[%s]", __FILE__,__LINE__, cTx_mx_no);
 
  switch(cTx_type[0])
  {
    case '0': /**代付**/
      ret=Lv_pldfr_Sel(g_pub_tx.reply, &sLv_pldfr," wt_date=%s and packid='%8s' and orderno='%8s' and tx_mx_no='%08s' ", cWt_date, cPackid, cOrderno, cTx_mx_no);
      if(ret)
      {
          sprintf(acErrMsg, "查询代付表出错!![%d]", ret);
          WRITEMSG
          goto ErrExit;
      }
      if(strlen(sLv_pldfr.repstat))
      {
          ret = iCheckStat(sLv_pldfr.repstat, repstat_cont); 
          if(ret)
          {
              sprintf(acErrMsg, "不存在的状态");
              WRITEMSG
              goto ErrExit;
          }
      }
      if(strlen(sLv_pldfr.repstat2))
      {
          ret = iCheckStat(sLv_pldfr.repstat2, repstat_cont);  
          if(ret)
          {
              sprintf(acErrMsg, "不存在的状态");
              WRITEMSG
              goto ErrExit;
          }
      }
      vtcp_log("[%s][%d]代付业务",__FILE__,__LINE__);
      dsf_getdfdj(cWt_date, cPackid, cOrderno,cTx_mx_no,cOtherword);
      apitoa(sLv_pldfr.wt_date, sizeof(dsfmx.wt_date), dsfmx.wt_date);
      memcpy(dsfmx.packid     , sLv_pldfr.packid     , sizeof(dsfmx.packid     ));
      memcpy(dsfmx.orderno    , sLv_pldfr.orderno    , sizeof(dsfmx.orderno    ));
      memcpy(dsfmx.tx_mx_no   , sLv_pldfr.tx_mx_no   , sizeof(dsfmx.tx_mx_no   ));
      memcpy(dsfmx.br_no      , sLv_pldfr.br_no      , sizeof(dsfmx.br_no      ));
      memcpy(dsfmx.yw_type    , sLv_pldfr.yw_type    , sizeof(dsfmx.yw_type    ));
      memcpy(dsfmx.pay_br_no  , sLv_pldfr.pay_br_no  , sizeof(dsfmx.pay_br_no  ));
      memcpy(dsfmx.pay_ac_no  , sLv_pldfr.pay_ac_no  , sizeof(dsfmx.pay_ac_no  ));
      memcpy(dsfmx.qs_no      , sLv_pldfr.cash_qs_no , sizeof(dsfmx.qs_no      ));
      memcpy(dsfmx.cash_ac_no , sLv_pldfr.cash_ac_no , sizeof(dsfmx.cash_ac_no ));
      memcpy(dsfmx.cash_br_no , sLv_pldfr.cash_br_no , sizeof(dsfmx.cash_br_no ));
      memcpy(dsfmx.repstat    , sLv_pldfr.repstat    , sizeof(dsfmx.repstat    ));
      memcpy(dsfmx.re_content , sLv_pldfr.re_content , sizeof(dsfmx.re_content ));
      memcpy(dsfmx.repstat2   , sLv_pldfr.repstat2   , sizeof(dsfmx.repstat2   ));
      memcpy(dsfmx.xyno       , sLv_pldfr.xyno       , sizeof(dsfmx.xyno       ));
      memcpy(cBuff            , sLv_pldfr.filename   , sizeof(cBuff)-1);
      zip_space(cBuff);
      memcpy(dsfmx.filename   , cBuff                , sizeof(dsfmx.filename   ));
      memcpy(dsfmx.re_content2, sLv_pldfr.re_content2, sizeof(dsfmx.re_content2));
      memcpy(dsfmx.otherword  , cOtherword           , sizeof(dsfmx.otherword  ));
       /* 2009年7月21日增加收款人姓名和付款人姓名*/
      memcpy(dsfmx.pay_name   , sLv_pldfr.pay_name   , sizeof(dsfmx.pay_name   ));
      set_zd_data( "0961", sLv_pldfr.cash_name);
      /*--end*/
      if(!memcmp(sLv_pldfr.tx_mx_no,"00000000",8))
      {
          sprintf(dsfmx.totamt,"%.2f",sLv_pldfr.totamt);
      }
      else
      {
      	  sprintf(dsfmx.totamt,"%.2f",sLv_pldfr.tx_amt);
      }
      vtcp_log("[%s][%d]sLv_pldfr.totamt==[%f] ", __FILE__,__LINE__, sLv_pldfr.totamt);
      vtcp_log("[%s][%d]sLv_pldfr.tx_amt==[%f] ", __FILE__,__LINE__, sLv_pldfr.tx_amt);
      vtcp_log("[%s][%d]dsfmx.totamt==[%s] ", __FILE__,__LINE__, dsfmx.totamt);
      memcpy(cStat            , sLv_pldfr.sts        , sizeof(cStat)-1);
      ret = iGetDsfStat(cStat,dsfmx.stat);
      if(ret)
      {
      	  sprintf(acErrMsg, "状态不正常啊!![%d]", ret);
          WRITEMSG
          goto ErrExit;
      }
      vtcp_log("[%s][%d]dsfmx.stat=[%s]",__FILE__,__LINE__,dsfmx.stat);
      pub_base_full_space((char*)&dsfmx, sizeof(dsfmx));
      break;

    case '1': /**代收**/
      ret = Lv_pldsr_Sel(g_pub_tx.reply, &sLv_pldsr," wt_date=%s and packid='%8s' and orderno='%8s' and tx_mx_no='%08s' ", cWt_date, cPackid, cOrderno, cTx_mx_no);
      if(ret)
      {
        sprintf(acErrMsg, "查询代收表出错!![%d]", ret);
        WRITEMSG
        goto ErrExit;
      }
      if(strlen(sLv_pldsr.repstat))
      {
        ret = iCheckStat(sLv_pldsr.repstat, repstat_cont); 
        if(ret)
        {
          sprintf(acErrMsg, "不存在的状态");
          WRITEMSG
          goto ErrExit;
        }
      }
      if(strlen(sLv_pldsr.repstat2))
      {
           ret = iCheckStat(sLv_pldsr.repstat2, repstat_cont);  
           if(ret)
           {
               sprintf(acErrMsg, "不存在的状态");
               WRITEMSG
               goto ErrExit;
            }
      }
      vtcp_log("[%s][%d]代收业务",__FILE__,__LINE__);
      dsf_getdsjj(cWt_date, cPackid, cOrderno,cTx_mx_no, cOtherword);
      apitoa(sLv_pldsr.wt_date, sizeof(dsfmx.wt_date), dsfmx.wt_date);            
      memcpy(dsfmx.packid     , sLv_pldsr.packid     , sizeof(dsfmx.packid     ));
      memcpy(dsfmx.orderno    , sLv_pldsr.orderno    , sizeof(dsfmx.orderno    ));
      memcpy(dsfmx.tx_mx_no   , sLv_pldsr.tx_mx_no   , sizeof(dsfmx.tx_mx_no   ));
      memcpy(dsfmx.br_no      , sLv_pldsr.br_no      , sizeof(dsfmx.br_no      ));
      memcpy(dsfmx.yw_type    , sLv_pldsr.yw_type    , sizeof(dsfmx.yw_type    ));
      memcpy(dsfmx.pay_br_no  , sLv_pldsr.pay_br_no  , sizeof(dsfmx.pay_br_no  ));
      memcpy(dsfmx.pay_ac_no  , sLv_pldsr.pay_ac_no  , sizeof(dsfmx.pay_ac_no  ));
      memcpy(dsfmx.qs_no      , sLv_pldsr.pay_qs_no  , sizeof(dsfmx.qs_no      ));
      memcpy(dsfmx.cash_ac_no , sLv_pldsr.cash_ac_no , sizeof(dsfmx.cash_ac_no ));
      memcpy(dsfmx.cash_br_no , sLv_pldsr.cash_br_no , sizeof(dsfmx.cash_br_no ));
      memcpy(dsfmx.repstat    , sLv_pldsr.repstat    , sizeof(dsfmx.repstat    ));
      memcpy(dsfmx.re_content , sLv_pldsr.re_content , sizeof(dsfmx.re_content ));
      memcpy(dsfmx.repstat2   , sLv_pldsr.repstat2   , sizeof(dsfmx.repstat2   ));
      memcpy(dsfmx.xyno       , sLv_pldsr.xyno       , sizeof(dsfmx.xyno       ));
      memcpy(cBuff            , sLv_pldsr.filename   , sizeof(cBuff          )-1);
      zip_space(cBuff);
      memcpy(dsfmx.filename   , cBuff                , sizeof(dsfmx.filename   ));
      memcpy(dsfmx.re_content2, sLv_pldsr.re_content2, sizeof(dsfmx.re_content2));
      memcpy(dsfmx.otherword  , cOtherword           , sizeof(dsfmx.otherword  ));
      /* 2009年7月21日增加收款人姓名和付款人姓名*/
      memcpy(dsfmx.pay_name   , sLv_pldsr.pay_name   , sizeof(dsfmx.pay_name   ));
      set_zd_data( "0961", sLv_pldsr.cash_name);
      /*--end*/
      if(!memcmp(sLv_pldsr.tx_mx_no, "00000000", 8))
      {
          sprintf(dsfmx.totamt,"%.2f",sLv_pldsr.totamt);
      }
      else
      {
          sprintf(dsfmx.totamt,"%.2f",sLv_pldsr.tx_amt);
      }
      vtcp_log("[%s][%d]dsfmx.totamt==[%s] ", __FILE__,__LINE__, dsfmx.totamt);
      memcpy(cStat            , sLv_pldsr.sts        , sizeof(cStat)-1);
      ret = iGetDsfStat(cStat,dsfmx.stat);
      if(ret)
      {
      	  sprintf(acErrMsg, "状态不正常啊!![%d]", ret);
          WRITEMSG
          goto ErrExit;
      }
      vtcp_log("[%s][%d]dsfmx.stat=[%s]",__FILE__,__LINE__,dsfmx.stat);
      pub_base_full_space((char*)&dsfmx, sizeof(dsfmx));
      break;

    case '2': /**撤销**/
      ret = Lv_pldscx_Sel(g_pub_tx.reply, &sLv_pldscx," wt_date=%s and packid='%8s' and orderno='%8s' ", cWt_date, cPackid, cOrderno);
      if(ret)
      {
           sprintf(acErrMsg, "查询撤销表出错!![%d]", ret);
           WRITEMSG
           goto ErrExit;
      }
      if(strlen(sLv_pldscx.repstat2))
      {
          ret = iCheckStat(sLv_pldscx.repstat2, repstat_cont);  
          if(ret)
          {
              sprintf(acErrMsg, "不存在的状态");
              WRITEMSG
              goto ErrExit;
          }
      }
      vtcp_log("[%s][%d]撤销交易",__FILE__,__LINE__);
      dsf_getjjorderno(cWt_date, cPackid, cOrderno, cOtherword);
      apitoa(sLv_pldscx.wt_date, sizeof(dsfmx.wt_date), dsfmx.wt_date);
      memcpy(dsfmx.packid     , sLv_pldscx.packid     , sizeof(dsfmx.packid));
      memcpy(dsfmx.orderno    , sLv_pldscx.orderno    , sizeof(dsfmx.orderno));
      memcpy(dsfmx.br_no      , sLv_pldscx.br_no      , sizeof(dsfmx.br_no));
      memcpy(dsfmx.yw_type    , sLv_pldscx.o_yw_type  , sizeof(dsfmx.yw_type));
      memcpy(dsfmx.pay_br_no  , sLv_pldscx.or_br_no   , sizeof(dsfmx.pay_br_no));
      memcpy(dsfmx.repstat2   , sLv_pldscx.repstat2   , sizeof(dsfmx.repstat2));
      memcpy(dsfmx.re_content2, sLv_pldscx.re_content2, sizeof(dsfmx.re_content2));
      apitoa(sLv_pldscx.o_wt_date, 8, dsfmx.xyno);
      memcpy(dsfmx.xyno+8     , sLv_pldscx.o_orderno  , 8);
      memcpy(dsfmx.xyno+16    , sLv_pldscx.sts        , 2);
      memcpy(dsfmx.filename   , sLv_pldscx.content    , sizeof(dsfmx.filename));
      vtcp_log("[%s][%d]gethere ",__FILE__,__LINE__);
      memcpy(cStat            , sLv_pldscx.sts        , sizeof(cStat)-1);
      ret = iGetDsfStat(cStat,dsfmx.stat);
      if(ret)
      {
      	  sprintf(acErrMsg, "状态不正常啊!![%d]", ret);
          WRITEMSG
          goto ErrExit;
      }
      vtcp_log("[%s][%d]dsfmx.stat=[%s]",__FILE__,__LINE__,dsfmx.stat);
      pub_base_full_space((char*)&dsfmx, sizeof(dsfmx));
      break;

    case '3': /**合同变更**/
      ret = Hv_orno_brno_Sel(g_pub_tx.reply, &sHv_orno_brno, "br_no = '%s'", cBr_no);
      if(ret)
      {
            sprintf(acErrMsg, "查询hv_brno_orno表出错!![%d]", ret);
            WRITEMSG
            goto ErrExit;
      }

      memcpy(cOr_br_no, sHv_orno_brno.or_br_no, sizeof(sHv_orno_brno.or_br_no));
      vtcp_log("[%s][%d]or_br_no=[%s]", __FILE__,__LINE__, cOr_br_no);

      ret = Lv_pldsfqugx_Sel(g_pub_tx.reply, &sLv_pldsfqugx," wt_date=%s and or_br_no ='%s' and orderno='%s' ", cWt_date, cOr_br_no, cOrderno);

      if(ret)
      {
           sprintf(acErrMsg, "查询合同变更表出错!![%d]", ret);
           WRITEMSG
           goto ErrExit;
      }
      if(strlen(sLv_pldsfqugx.repstat))
      {
           ret = iCheckStat(sLv_pldsfqugx.repstat, repstat_cont);  
           if(ret)
           {
                sprintf(acErrMsg, "不存在的状态");
                WRITEMSG
                goto ErrExit;
           }
      }
      vtcp_log("[%s][%d]合同变更",__FILE__,__LINE__);
      apitoa(sLv_pldsfqugx.wt_date, sizeof(dsfmx.wt_date), dsfmx.wt_date);
      memcpy(dsfmx.packid     , sLv_pldsfqugx.packid     , sizeof(dsfmx.packid));
      memcpy(dsfmx.orderno    , sLv_pldsfqugx.orderno    , sizeof(dsfmx.orderno));
      memcpy(dsfmx.br_no      , sLv_pldsfqugx.br_no      , sizeof(dsfmx.br_no));
      memcpy(dsfmx.pay_br_no  , sLv_pldsfqugx.pay_br_no  , sizeof(dsfmx.pay_br_no));
      memcpy(dsfmx.pay_ac_no  , sLv_pldsfqugx.pay_ac_no  , sizeof(dsfmx.pay_ac_no));
      memcpy(dsfmx.qs_no      , sLv_pldsfqugx.pay_qs_no  , sizeof(dsfmx.qs_no));
      memcpy(dsfmx.cash_br_no , sLv_pldsfqugx.pay_opn_br_no, sizeof(dsfmx.cash_br_no));
      memcpy(dsfmx.xyno       , sLv_pldsfqugx.xyno       , sizeof(dsfmx.xyno));
      memcpy(dsfmx.re_content , sLv_pldsfqugx.pay_name   , sizeof(dsfmx.re_content));
      memcpy(dsfmx.filename   , sLv_pldsfqugx.or_br_no, 12);
      memcpy(dsfmx.filename+12, sLv_pldsfqugx.xy_ind  , 1);
      memcpy(dsfmx.filename+13, sLv_pldsfqugx.ch_ind  , 1);
      memcpy(dsfmx.filename+14, sLv_pldsfqugx.sts     , 2);
      memcpy(dsfmx.repstat2   , sLv_pldsfqugx.repstat    , sizeof(dsfmx.repstat2));
      memcpy(dsfmx.xyno       , sLv_pldsfqugx.xyno       , sizeof(dsfmx.xyno));
      memcpy(dsfmx.re_content2, sLv_pldsfqugx.re_content2, sizeof(dsfmx.re_content2));
      memcpy(dsfmx.otherword  , sLv_pldsfqugx.content    , sizeof(dsfmx.otherword));
      vtcp_log("[%s][%d]gethere ",__FILE__,__LINE__);
      memcpy(cStat            , sLv_pldsfqugx.sts        , sizeof(cStat)-1);
      ret = iGetDsfStat(cStat,dsfmx.stat);
      if(ret)
      {
      	  sprintf(acErrMsg, "状态不正常啊!![%d]", ret);
          WRITEMSG
          goto ErrExit;
      }
      vtcp_log("[%s][%d]dsfmx.stat=[%s]",__FILE__,__LINE__,dsfmx.stat);
      pub_base_full_space((char*)&dsfmx, sizeof(dsfmx));
      break;

    default:
      sprintf(acErrMsg,"交易类型错误!![%s]", cTx_type);
      WRITEMSG
      goto ErrExit;
  }
  set_zd_data("0950", (char *)&dsfmx);

OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"OK exit!! reply is:[%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;
ErrExit:
  sprintf(acErrMsg,"fail exit!! reply is:[%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}

int iCheckStat(char *a,char *b)
{
  char istat[3];

  memset(istat,0x00,sizeof(istat));
  
  memcpy(istat,a,2);
  switch(atoi(istat))
  {
    case 0:
      strcpy(b,"成功");
      break;
    case 1:
      strcpy(b,"帐号不存在");
        break;
    case 2:
      strcpy(b,"帐号 户名不符");
      break;
    case 3:
      strcpy(b,"账户余额不足支付");
      break;
    case 10:
      strcpy(b,"账户密码错误");
      break;
    case 11:
      strcpy(b,"账户状态错误");
      break;
    case 20:
      strcpy(b,"业务已撤销");
      break;
    case 90:
      strcpy(b,"其他错误");
      break;
    default:
      strcpy(b,"不存在的状态");
      return 1; 
  }
  return 0;
}
int iGetDsfStat(char * cStat,char * cString)
{
	  if(strlen(cStat)==0)
	  {
	  	 vtcp_log("[%s][%d]错误的cStat=[%s]",__FILE__,__LINE__,cStat);
	  	 return (-1);
	  }
	  if(memcmp(cStat,"0A",2)==0)
	  {
	  	  memcpy(cString,"来账接收",8);
	  }
	  else if(memcmp(cStat,"1B",2)==0)
	  {
	  	  memcpy(cString,"来账拒绝",8);
	  }
	  else if(memcmp(cStat,"0C",2)==0)
	  {
	  	  memcpy(cString,"来账撤销",8);
	  }
	  else if(memcmp(cStat,"0D",2)==0)
	  {
	  	  memcpy(cString,"成功回执40506",13);
	  }
	  else if(memcmp(cStat,"1E",2)==0)
	  {
	  	  memcpy(cString,"失败回执40506",13);
	  }
	  else if(memcmp(cStat,"0F",2)==0)
	  {
	  	  memcpy(cString,"成功回执40503/4",15);
	  }
	  else if(memcmp(cStat,"0G",2)==0)
	  {
	  	  memcpy(cString,"失败回执40503/4",15);
	  }
	  else if(memcmp(cStat,"0H",2)==0)
	  {
	  	  memcpy(cString,"往账发送",8);
	  }
	  else if(memcmp(cStat,"1I",2)==0)
	  {
	  	  memcpy(cString,"往账拒绝",8);
	  }
	  else if(memcmp(cStat,"0J",2)==0)
	  {
	  	  memcpy(cString,"往账清算",8);
	  }
	  else if(memcmp(cStat,"1K",2)==0)
	  {
	  	  memcpy(cString,"往账失败",8);
	  }
	  else
	  {
	  	  vtcp_log("[%s][%d]wrong type!!",__FILE__,__LINE__);
	  	  return (-1);
	  }
	  return 0;
}
