/***************************************************************************/
/* 文件名称：小额支付公共函数（非支付交易）。                              */
/*                                                                         */
/* 功能描述：                                                              */
/*                                                                         */
/* 日期：  2005.07.20                                                      */
/* 作者：  zhudaowen                                                  */
/***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define EXTERN

#include "hv_define.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "hv_addit_c.h"
#include "hv_fd123.h"
#include "seqctl_c.h"
#include "lv_define.h"
#include "lv_tito.h"
#include "lv_lbctl_c.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "errlog.h"
#include "lvpack.h"
#include "public.h"

/*注意记入时增加状态*/
int notpay_lzin_to_table(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cCmtno[4]; 
  char  cPkgno[4];
  int     iPkgno=0;
  int  iCmtno=0,ret;
  char    cBrno[6];
  char    cTime[7];
  char    cTlrno[5];
  char  cBakcode[13];
  char  cOrderno[9];

  struct hv_fzf_c         sHvFzf;
  struct hv_fzf_c         sHvFzf_Ser;
  struct hv_fzf_c         hv_fzf_c_s;
  struct hv_fzf_c         hv_fzf_c;

  memset(&sHvFzf, '\0', sizeof(struct hv_fzf_c));
  memset(&sHvFzf_Ser, '\0', sizeof(struct hv_fzf_c));
  memset(&hv_fzf_c_s, '\0', sizeof(struct hv_fzf_c));
  memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));
  memset(cCmtno, '\0', sizeof(cCmtno));
  memset(cPkgno, '\0', sizeof(cPkgno));
  memset(cBrno, 0, sizeof(cBrno));
  memset(cTime, 0, sizeof(cTime));
  memset(cTlrno, 0, sizeof(cTlrno));
  memset(cBakcode, 0, sizeof(cBakcode));
  memset(cOrderno, 0, sizeof(cOrderno));

  /* 交易机构 */
  get_fd_data("0030",cBrno);
  /* 发生时间 */
  get_fd_data("0060",cTime);
  /* 操作员 */
  get_fd_data("0070",cTlrno);

  vtcp_log("[%s][%d]--------------wp_notpayin->cmtno=[%s]\n",__FILE__,__LINE__,wp_notpayin->cmtno);
  memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
  memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);

  iCmtno = atoi(cCmtno);
  iPkgno = atoi(cPkgno);

  vtcp_log("[%s][%d]--------------iCmtno=[%d]\n",__FILE__,__LINE__,iCmtno);
  vtcp_log("[%s][%d]--------------iPkgno=[%d]\n",__FILE__,__LINE__,iPkgno);
  vtcp_log("[%s][%d]--------------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);

  /*对cmt报文*/
  switch(iCmtno)
  {
  case 319:
  case 320:
  case 322:
  case 324:
  case 325:
  case 327:
  case 328:
    /*记入撤销退回冲正止付接收登记簿*/
    {
      memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);
      memcpy(hv_fzf_c.br_no, cBrno, BRNO_LEN);
      vtcp_log("[%s][%d]stat===[%c] \n",__FILE__,__LINE__,wp_notpayin->stat);
      vtcp_log("[%s][%d]wp_notpayin->cmtno===[%s] \n",__FILE__,__LINE__,wp_notpayin->cmtno);
      hv_fzf_c_s.tx_date = apatoi(pcLvGetDay(),8);
      vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
      if (memcmp(wp_notpayin->cmtno,"322",3) == 0 || memcmp(wp_notpayin->cmtno,"324",3) == 0)
      {
        hv_fzf_c_s.wt_date = apatoi(pcLvGetDay(),8);
      }
      else  /** 321,320,327,328,325 **/
      {
        hv_fzf_c_s.wt_date = apatoi(wp_notpayin->txday, sizeof(wp_notpayin->txday));
      }
      vtcp_log("[%s][%d] orbrno==[%.12s]\n",__FILE__,__LINE__,wp_notpayin->orbrno);

      memcpy(hv_fzf_c.pay_qs_no,wp_notpayin->payqsactno,sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no,wp_notpayin->orbrno,  sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno,wp_notpayin->orderno,  sizeof(hv_fzf_c.orderno)-1);
      vtcp_log("[%s][%d] +++++wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno,sizeof(hv_fzf_c.cash_qs_no)-1);
      memcpy(hv_fzf_c.ac_br_no,   wp_notpayin->acbrno,  sizeof(hv_fzf_c.ac_br_no)-1);
      hv_fzf_c.reqtype[0] = wp_notpayin->rettype;
      vtcp_log("[%s][%d]========wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
      hv_fzf_c.res_sts[0] = wp_notpayin->respcode;
      vtcp_log("[%s][%d]========hv_fzf_c.res_sts[0]=[%c]\n",__FILE__,__LINE__,hv_fzf_c.res_sts[0]);
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;

      memcpy(hv_fzf_c.o_cmtno,  wp_notpayin->opkgno,  sizeof(hv_fzf_c.o_cmtno)-1);
      memcpy(hv_fzf_c.o_packid, wp_notpayin->opackid,  sizeof(hv_fzf_c.o_packid)-1);
      memcpy(hv_fzf_c.otxnum,   wp_notpayin->otxnum,  sizeof(hv_fzf_c.otxnum)-1);
      vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
      /** dxy 20060123 320来帐时，没有owtday,所以需要从原往帐中查 **/
      if (memcmp(wp_notpayin->cmtno,"320",3) == 0)
      {
        memcpy(cBakcode, wp_notpayin->oorbrno, sizeof(cBakcode)-1);
        memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
        ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf_Ser," otype = '1' and or_br_no = '%s' and wt_date =%d and orderno = '%s' and cmtno = '319'", cBakcode,apatoi(wp_notpayin->owtday,8),cOrderno);
        if ( ret == 100 )
        {
          sprintf( acErrMsg, "原查询记录不存在!" );WRITEMSG
          WRITEMSG
            return -1;
        }else if( ret )
        {
          sprintf( acErrMsg, "执行Lv_wbctl_Sel错![%d]", ret );
          WRITEMSG
            return -1;
        }
        vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
        hv_fzf_c.o_wt_date=sHvFzf_Ser.o_wt_date;
        hv_fzf_c.o_pack_date=sHvFzf_Ser.o_pack_date;
      }
      else if (memcmp(wp_notpayin->cmtno,"328",3) == 0)
      {
        memcpy(cBakcode, wp_notpayin->acbrno, sizeof(cBakcode)-1);
        memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
        ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf_Ser," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '327'", cBakcode,apatoi(wp_notpayin->owtday,8),cOrderno);
        if ( ret == 100 )
        {
          sprintf( acErrMsg, "原查询记录不存在!" );
          WRITEMSG
            return -1;
        }else if( ret )
        {
          sprintf( acErrMsg, "执行Hv_fzf_Sel错![%d]", ret );
          WRITEMSG
            return -1;
        }
        vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
        hv_fzf_c.o_wt_date=sHvFzf_Ser.o_wt_date;
        hv_fzf_c.o_pack_date=sHvFzf_Ser.o_pack_date;
      }
      else
      {
        hv_fzf_c.o_wt_date=sHvFzf_Ser.o_wt_date;
        hv_fzf_c.o_pack_date=sHvFzf_Ser.o_pack_date;
      }
      vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
      hv_fzf_c.req_date=apatoi(wp_notpayin->owtday,8); 

      memcpy(hv_fzf_c.o_orderno,wp_notpayin->oorderno,sizeof(hv_fzf_c.o_orderno)-1);
      if (iCmtno==319 || iCmtno==327)
        memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
      if (iCmtno==320 || iCmtno==322 || iCmtno==328 || iCmtno==324 || iCmtno==325)
        memcpy(hv_fzf_c.res_content,wp_notpayin->content,sizeof(hv_fzf_c.res_content)-1);
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "执行Hv_fzf_Ins错误!ret=[%d]", ret );
        WRITEMSG
          return -1;
      }
    }
    break;
  case 301:
    /*记入查询报文接收登记簿*/
    {
      char Otxamt [16];
      memset(Otxamt,0,sizeof(Otxamt));
      vtcp_log("[%s][%d]=========开始插入c301r\n",__FILE__,__LINE__);
      memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);
      hv_fzf_c.wt_date = apatoi(pcLvGetDay(),8);
      vtcp_log("[%s][%d]--------------\n",__FILE__,__LINE__);
      /** dxy 20060123 ,收到查询，日期是报文中的
      memcpy(hv_fzf_c.qday,pcLvGetDay(),sizeof(hv_fzf_c.qday)-1);
      **/
      hv_fzf_c.tx_date=apatoi( wp_notpayin->txday,sizeof(wp_notpayin->txday));
      memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
      memcpy(hv_fzf_c.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no,  wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno,wp_notpayin->orderno,sizeof(hv_fzf_c.orderno)-1);
      memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
      memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);
      hv_fzf_c.o_wt_date=apatoi(wp_notpayin->owtday,8);
      memcpy(hv_fzf_c.o_or_br_no,wp_notpayin->oorbrno,sizeof(hv_fzf_c.o_or_br_no)-1);
      memcpy(hv_fzf_c.o_ac_br_no,wp_notpayin->oacbrno,sizeof(hv_fzf_c.o_ac_br_no)-1);
      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
      vtcp_log("[%s][%d]--------------\n",__FILE__,__LINE__);

      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

      vtcp_log("[%s][%d]wp_notpayin->otxamt=[%s]\n",__FILE__,__LINE__,wp_notpayin->otxamt);
      vtcp_log("[%s][%d]Otxamt=[%s]\n",__FILE__,__LINE__,Otxamt);
      vtcp_log("[%s][%d]hv_fzf_c.o_tx_amt=[%f]--\n",__FILE__,__LINE__,hv_fzf_c.o_tx_amt);
      memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
      hv_fzf_c.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
      hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));
      vtcp_log("[%s][%d]--------------\n",__FILE__,__LINE__);
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "执行Hv_fzf_Ins错误!ret=[%d]", ret );
        WRITEMSG
          return -1;
      }
      vtcp_log("[%s][%d]--------------\n",__FILE__,__LINE__);
    }
    break;

  case 302:
    /*记入查复报文接收登记簿*/
    {
      char Otxamt [16];
      memset(Otxamt,0,sizeof(Otxamt));
      vtcp_log("[%s][%d]=========开始插入c302r\n",__FILE__,__LINE__);
      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);
      hv_fzf_c.wt_date = apatoi(pcLvGetDay(),8);
      hv_fzf_c.tx_date=apatoi( wp_notpayin->txday,sizeof(wp_notpayin->txday));
      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
      hv_fzf_c.req_date=apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
      hv_fzf_c.o_wt_date=apatoi(wp_notpayin->owtday,8);
      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
      /*原查询书号*/
      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
      /*原支付交易序号*/
      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
      zip_space(Otxamt);
      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

      vtcp_log("[%s][%d]wp_notpayin->otxamt=[%s]\n",__FILE__,__LINE__,wp_notpayin->otxamt);
      vtcp_log("[%s][%d]Otxamt=[%s]\n",__FILE__,__LINE__,Otxamt);
      vtcp_log("[%s][%d]hv_fzf_c.o_tx_amt=[%f]--\n",__FILE__,__LINE__,hv_fzf_c.o_tx_amt);

      memcpy(hv_fzf_c.res_content, wp_notpayin->content, sizeof(hv_fzf_c.res_content)-1);
      hv_fzf_c.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
      hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));

      vtcp_log("[%s][%d]--------------\n",__FILE__,__LINE__);
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "执行Hv_fzf_Ins错误!ret=[%d]", ret );
        WRITEMSG
          return -1;
      }
      memcpy(cBakcode, wp_notpayin->oorbrno, sizeof(cBakcode)-1);
      memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
      ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '301'", cBakcode,apatoi(wp_notpayin->qwtday,8),cOrderno);
      if (ret)
      {
        vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
        WRITEMSG
          return -1;
      }
      ret = Hv_fzf_Fet_Upd( &sHvFzf_Ser, g_pub_tx.reply );
      if ( ret == 100 )
      {
        sprintf( acErrMsg, "原查询记录不存在!" );
        WRITEMSG
      }else if( ret )
      {
        sprintf( acErrMsg, "执行Hv_fzf_Fet_Upd错![%d]", ret );
        WRITEMSG
          return -1;
      }
      sHvFzf_Ser.hv_fzf_sts[0] = STAT_WZBACK;
      sHvFzf_Ser.res_date=apatoi(wp_notpayin->txday,8);
      ret = Hv_fzf_Upd_Upd( sHvFzf_Ser, g_pub_tx.reply );
      if ( ret )
      {
        sprintf( acErrMsg, "修改非支付登记薄错[%d]", ret );
        WRITEMSG
          return -1;
      }
      Hv_fzf_Clo_Upd();
    }
    break;
  case 303: /*记入自由格式报文接收登记簿*/
    {
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,  sizeof(hv_fzf_c.cmtno)-1);
      hv_fzf_c.wt_date = apatoi(pcLvGetDay(),8);
      hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
      memcpy(hv_fzf_c.sendco,  wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
      memcpy(hv_fzf_c.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no,  wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno, wp_notpayin->orderno,sizeof(hv_fzf_c.orderno)-1);
      memcpy(hv_fzf_c.receco,  wp_notpayin->receco, sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
      memcpy(hv_fzf_c.ac_br_no,  wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);
      memcpy(hv_fzf_c.req_content, wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
      hv_fzf_c.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
      hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "执行Hv_fzf_Ins错误!ret=[%d]", ret );
        WRITEMSG
          return -1;
      }
    }
    break;
  case 920: /*小额批量包处理确认通知报文*/
    {
      /*****************未改
      struct wd_lv_c920r_area    wd_lv_c920r;
      memset(&wd_lv_c920r, 0, sizeof(wd_lv_c920r));
      memcpy(wd_lv_c920r.cmtno,   wp_notpayin->cmtno,   sizeof(wd_lv_c920r.cmtno)-1);
      memcpy(wd_lv_c920r.inday,   pcLvGetDay(),      sizeof(wd_lv_c920r.inday)-1);
      memcpy(wd_lv_c920r.wtday,   pcLvGetDay(),      sizeof(wd_lv_c920r.wtday)-1);
      memcpy(wd_lv_c920r.osendco, wp_notpayin->sendco,   sizeof(wd_lv_c920r.osendco)-1);
      memcpy(wd_lv_c920r.orbrno,  wp_notpayin->orbrno,  sizeof(wd_lv_c920r.orbrno)-1);
      memcpy(wd_lv_c920r.acbrno,  wp_notpayin->acbrno,  sizeof(wd_lv_c920r.acbrno)-1);
      memcpy(wd_lv_c920r.opkgno,  wp_notpayin->opkgno,  sizeof(wd_lv_c920r.opkgno)-1);  
      memcpy(wd_lv_c920r.owtday,  wp_notpayin->owtday,  sizeof(wd_lv_c920r.owtday)-1);
      memcpy(wd_lv_c920r.opackid,  wp_notpayin->opackid,   sizeof(wd_lv_c920r.opackid)-1);
      wd_lv_c920r.stat[0] = wp_notpayin->stat;
      memcpy(wd_lv_c920r.txtime,  it_txcom.txtime,    sizeof(wd_lv_c920r.txtime)-1);
      memcpy(wd_lv_c920r.brno,  TITA.kinbr,        sizeof(wd_lv_c920r.brno)-1);
      memcpy(wd_lv_c920r.recvno,  TITA.tlrno,        sizeof(wd_lv_c920r.recvno)-1);
      memcpy(wd_lv_c920r.recvwssrno,wp_notpayin->wssrno,sizeof(wd_lv_c920r.recvwssrno)-1);

      if ((it_txcom.rtncd = DbsLV_C920R(DBS_INSERT,&wd_lv_c920r)) != DB_OK)
      {
      vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
      DbsLV_C920RDebug(&wd_lv_c920r);
      memcpy(acErrMsg,"插入查询报文接收登记簿错误!",40);
      return it_txcom.rtncd;
      }
      *********************/
    }
    break;
  }
  vtcp_log("[%s][%d]+++++++++++++执行到这里来了!+++ipkgno=[%d]\n",__FILE__,__LINE__,iPkgno);
  /*对pkg报文*/
  /***************************未改
  switch(iPkgno)
  {
  case 12: 
  {
  struct wd_lv_tyxxr_area    wd_lv_tyxxr;
  memset(&wd_lv_tyxxr, 0, sizeof(wd_lv_tyxxr));

  vtcp_log("[%s][%d]+++++++++++++执行到这里来了!+++\n",__FILE__,__LINE__);
  memcpy(wd_lv_tyxxr.pkgno,  wp_notpayin->pkgno,  sizeof(wd_lv_tyxxr.pkgno)-1);
  memcpy(wd_lv_tyxxr.inday,  pcLvGetDay(),     sizeof(wd_lv_tyxxr.inday)-1);
  memcpy(wd_lv_tyxxr.wtday,  wd_lv_tyxxr.inday,   sizeof(wd_lv_tyxxr.wtday)-1);
  memcpy(wd_lv_tyxxr.packday,wd_lv_tyxxr.inday,   sizeof(wd_lv_tyxxr.packday)-1);
  memcpy(wd_lv_tyxxr.orderno,wp_notpayin->orderno,sizeof(wd_lv_tyxxr.orderno)-1);
  memcpy(wd_lv_tyxxr.txtime, it_txcom.txtime,   sizeof(wd_lv_tyxxr.txtime)-1);
  memcpy(wd_lv_tyxxr.txnum,  wp_notpayin->txnum,  sizeof(wd_lv_tyxxr.txnum)-1);
  memcpy(wd_lv_tyxxr.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_tyxxr.orbrno)-1);
  memcpy(wd_lv_tyxxr.payqsactno, wp_notpayin->payqsactno, sizeof(wd_lv_tyxxr.payqsactno)-1);
  memcpy(wd_lv_tyxxr.sendco, wp_notpayin->sendco, sizeof(wd_lv_tyxxr.sendco)-1);
  memcpy(wd_lv_tyxxr.acbrno, wp_notpayin->acbrno, sizeof(wd_lv_tyxxr.acbrno)-1);
  memcpy(wd_lv_tyxxr.cashqsactno, wp_notpayin->cashqsactno, sizeof(wd_lv_tyxxr.cashqsactno)-1);
  memcpy(wd_lv_tyxxr.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
  wd_lv_tyxxr.datatype[0] = wp_notpayin->datatype;
  memcpy(wd_lv_tyxxr.content, wp_notpayin->content, sizeof(wd_lv_tyxxr.content)-1);
  if(!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_DF,TXNUM_LEN))
  {
  if(!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN))
  wd_lv_tyxxr.sfflag[0] = '0';
  else
  wd_lv_tyxxr.sfflag[0] = '1';
  memcpy(wd_lv_tyxxr.sfunit, wp_notpayin->unit,   sizeof(wd_lv_tyxxr.sfunit)-1);
  memcpy(wd_lv_tyxxr.sftype, wp_notpayin->sftype, sizeof(wd_lv_tyxxr.sftype)-1);
  }
  memcpy(wd_lv_tyxxr.addid,  wp_notpayin->addid,  sizeof(wd_lv_tyxxr.addid)-1);
  memcpy(wd_lv_tyxxr.brno,   TITA.kinbr, sizeof(wd_lv_tyxxr.brno)-1);
  memcpy(wd_lv_tyxxr.recvno, TITA.tlrno, sizeof(wd_lv_tyxxr.recvno)-1);
  memcpy(wd_lv_tyxxr.recvwssrno,wp_notpayin->wssrno,sizeof(wd_lv_tyxxr.recvwssrno)-1);
  memcpy(wd_lv_tyxxr.packid,wp_notpayin->opackid,sizeof(wd_lv_tyxxr.packid)-1);
  vtcp_log("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);

  if(memcmp(wp_notpayin->optype, OPCD_LR, 2) == 0)
  wd_lv_tyxxr.stat[0] = STAT_WZLR;
  else if(memcmp(wp_notpayin->optype, OPCD_SEND, 2) == 0)
  wd_lv_tyxxr.stat[0] = STAT_WZSEND;
  else
  wd_lv_tyxxr.stat[0]=STAT_LZRECV1; 

  if ((it_txcom.rtncd = DbsLV_TYXXR(DBS_INSERT,&wd_lv_tyxxr)) != DB_OK)
  {
  vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
  DbsLV_TYXXRDebug(&wd_lv_tyxxr);
  memcpy(acErrMsg,"插入通用信息接收登记簿错误!",40);
  return it_txcom.rtncd;
  }
  }
  break;
  case 13: 
  {

  struct wd_lv_ssxxr_area    wd_lv_ssxxr;
  memset(&wd_lv_ssxxr, 0, sizeof(wd_lv_ssxxr));

  vtcp_log("[%s][%d]+++++++++++++执行到这里来了!+++\n",__FILE__,__LINE__);
  memcpy(wd_lv_ssxxr.inday,   pcLvGetDay(),       sizeof(wd_lv_ssxxr.inday)-1);
  memcpy(wd_lv_ssxxr.wtday,   wd_lv_ssxxr.inday,     sizeof(wd_lv_ssxxr.wtday)-1);
  memcpy(wd_lv_ssxxr.packday, wd_lv_ssxxr.inday,     sizeof(wd_lv_ssxxr.packday)-1);
  memcpy(wd_lv_ssxxr.orderno, wp_notpayin->orderno,  sizeof(wd_lv_ssxxr.orderno)-1);
  memcpy(wd_lv_ssxxr.txtime,   it_txcom.txtime,     sizeof(wd_lv_ssxxr.txtime)-1);
  memcpy(wd_lv_ssxxr.txnum,   wp_notpayin->txnum,   sizeof(wd_lv_ssxxr.txnum)-1);
  memcpy(wd_lv_ssxxr.orbrno,   wp_notpayin->orbrno,   sizeof(wd_lv_ssxxr.orbrno)-1);
  memcpy(wd_lv_ssxxr.payqsactno, wp_notpayin->payqsactno, sizeof(wd_lv_ssxxr.payqsactno)-1);
  memcpy(wd_lv_ssxxr.sendco,   wp_notpayin->sendco,   sizeof(wd_lv_ssxxr.sendco)-1);
  memcpy(wd_lv_ssxxr.acbrno,   wp_notpayin->acbrno,   sizeof(wd_lv_ssxxr.acbrno)-1);
  memcpy(wd_lv_ssxxr.cashqsactno, wp_notpayin->cashqsactno, sizeof(wd_lv_ssxxr.cashqsactno)-1);
  memcpy(wd_lv_ssxxr.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
  memcpy(wd_lv_ssxxr.pkgno,   wp_notpayin->pkgno,   sizeof(wd_lv_ssxxr.pkgno)-1);
  wd_lv_ssxxr.datatype[0] = wp_notpayin->datatype;
  wd_lv_ssxxr.stat[0] = wp_notpayin->stat;
  memcpy(wd_lv_ssxxr.content, wp_notpayin->content,   sizeof(wd_lv_ssxxr.content)-1);
  if(!memcmp(wp_notpayin->txnum,TXNUM_ZHCX,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_CXYD,TXNUM_LEN))
  {
  vtcp_log("[%s][%d]---------wp_notpayin->actno=[%s]\n", __FILE__,__LINE__,wp_notpayin->actno);
  vtcp_log("[%s][%d]---------wp_notpayin->name=[%s]\n",__FILE__,__LINE__,wp_notpayin->name);

  wd_lv_ssxxr.qacttype[0] = wp_notpayin->acttype;
  memcpy(wd_lv_ssxxr.qactno,   wp_notpayin->actno, sizeof(wd_lv_ssxxr.qactno)-1);
  memcpy(wd_lv_ssxxr.qname,   wp_notpayin->name,   sizeof(wd_lv_ssxxr.qname)-1);
  wd_lv_ssxxr.qpswdcd[0] = wp_notpayin->pswdcd;
  memcpy(wd_lv_ssxxr.qpasswd, wp_notpayin->pascode,   sizeof(wd_lv_ssxxr.qpasswd)-1);
  wd_lv_ssxxr.qtype[0] = wp_notpayin->qtype;

  memcpy(wd_lv_ssxxr.opackday,wp_notpayin->opackday, sizeof(wd_lv_ssxxr.opackday)-1);
  memcpy(wd_lv_ssxxr.opackid,  wp_notpayin->opackid,  sizeof(wd_lv_ssxxr.opackid)-1);
  str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0,&wd_lv_ssxxr.qavbal);
  }
  memcpy(wd_lv_ssxxr.addid,   wp_notpayin->addid,    sizeof(wd_lv_ssxxr.addid)-1);
  memcpy(wd_lv_ssxxr.brno,   TITA.kinbr,        sizeof(wd_lv_ssxxr.brno)-1);
  memcpy(wd_lv_ssxxr.recvno,   TITA.tlrno,        sizeof(wd_lv_ssxxr.recvno)-1);
  memcpy(wd_lv_ssxxr.recvwssrno,wp_notpayin->wssrno, sizeof(wd_lv_ssxxr.recvwssrno)-1);
  memcpy(wd_lv_ssxxr.packid,wp_notpayin->opackid,sizeof(wd_lv_ssxxr.packid)-1);
  if ((it_txcom.rtncd = DbsLV_SSXXR(DBS_INSERT,&wd_lv_ssxxr)) != DB_OK)
  {
  vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
  it_txcom.txrsut=TX_REJECT;
  DbsLV_SSXXRDebug(&wd_lv_ssxxr);
  memcpy(acErrMsg,"插入实时信息接收登记簿错误!",40);
  return it_txcom.rtncd;
  }
  }
  break;

  }
  *********************/
  return 0;
}

/************************************************************
*  根据接收的cmt报文,对需要退回撤销的业务进行状态的更改  *
*  本函数只根据能否退回或撤销等修改ctczs和wbctl的状态    *
*  函数名: Rev_Pkg_Stat                  *
************************************************************/
int Rev_Pkg_Stat(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cCmtno[4];
  char  Hvf_Cmtno[4];
  char  HvW_Packid[9];
  int   iCmtno=0,ret=0;

  struct  hv_fzf_c  sHvFzf;
  struct  lv_wbctl_c  sLvWbctl;

  memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
  memset(&sLvWbctl,   '\0', sizeof(struct lv_wbctl_c));

  memset(cCmtno, '\0', sizeof(cCmtno));
  memset(Hvf_Cmtno, '\0', sizeof(cCmtno));
  memset(HvW_Packid, '\0', sizeof(HvW_Packid));
  memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);

  iCmtno = apatoi(cCmtno,3);
  vtcp_log("[%s][%d]进入改写原交易状态函数,传入报文是cmt[%d]\n",__FILE__,__LINE__,iCmtno);

  if (memcmp(wp_notpayin->cmtno,"320", 3) == 0)
  {
    memcpy(Hvf_Cmtno, "319", sizeof(Hvf_Cmtno)-1);
  }
  else if (memcmp(wp_notpayin->cmtno,"322", 3) == 0)
  {
    memcpy(Hvf_Cmtno, "321", sizeof(Hvf_Cmtno)-1);
  }
  else if (memcmp(wp_notpayin->cmtno,"324", 3) == 0)
  {
    memcpy(Hvf_Cmtno, "323", sizeof(Hvf_Cmtno)-1);
  }
  else if (memcmp(wp_notpayin->cmtno,"328", 3) == 0)
  {
    memcpy(Hvf_Cmtno, "327", sizeof(Hvf_Cmtno)-1);
  }
  vtcp_log("[%s][%d]====notpay发起行=[%.12s],委托日期=[%.8s],序号=[%.8s]\n",
    __FILE__,__LINE__,wp_notpayin->oorbrno,wp_notpayin->owtday,wp_notpayin->oorderno);
/*   小额otype='2'   Modified by ChenMing  2006-10-24 16:39 
  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '1' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '%s'", wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,sizeof(wp_notpayin->owtday)),wp_notpayin->oorderno,Hvf_Cmtno);
*/

  memcpy(sHvFzf.or_br_no, wp_notpayin->oorbrno, sizeof(sHvFzf.or_br_no)-1 );
  memcpy(sHvFzf.orderno, wp_notpayin->oorderno, sizeof(sHvFzf.orderno)-1 );
  sHvFzf.wt_date = apatoi(wp_notpayin->owtday,sizeof(wp_notpayin->owtday));    

    vtcp_log("[%s][%d]  sHvFzf.or_br_no====[%s] ",__FILE__,__LINE__,sHvFzf.or_br_no);
    vtcp_log("[%s][%d]  sHvFzf.orderno====[%s] ",__FILE__,__LINE__,sHvFzf.orderno);
    vtcp_log("[%s][%d]  sHvFzf.wt_date====[%d] ",__FILE__,__LINE__,sHvFzf.wt_date);    
    
  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '%s'",
   sHvFzf.or_br_no,sHvFzf.wt_date,sHvFzf.orderno,Hvf_Cmtno);
  if (ret)
  {
    vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
    WRITEMSG
      return -1;
  }
  
  memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
  ret = Hv_fzf_Fet_Upd( &sHvFzf, g_pub_tx.reply );
  if ( ret == 100 )
  {
    sprintf( acErrMsg, "原查询记录不存在!" );
    WRITEMSG
      return -1;
  }else if( ret )
  {
    sprintf( acErrMsg, "执行Hv_fzf_Fet_Upd错![%d]", ret );
    WRITEMSG
      return -1;
  }
  
  if (sHvFzf.hv_fzf_sts[0] != STAT_WZSEND)
  {
    vtcp_log("[%s][%d]应该是4,实际状态=[%c]\n",__FILE__,__LINE__,sHvFzf.hv_fzf_sts[0]);
    sprintf( acErrMsg, "状态错!" );
    WRITEMSG
      return -1;
  }
  
  /** 下列情况需要找到原止付业务编号 **/
  if ((memcmp(wp_notpayin->cmtno,"328",3)==0||memcmp(wp_notpayin->cmtno,"320",3)==0)
    && wp_notpayin->rettype == '0')
  {
    memcpy(wp_notpayin->oorderno, sHvFzf.o_orderno, sizeof(sHvFzf.o_orderno)-1);
  }
  
  if (wp_notpayin->opkgno[0]=='\0')
  {
    memcpy(wp_notpayin->opkgno,sHvFzf.o_cmtno, sizeof(sHvFzf.o_cmtno)-1);
  }
  
  if (wp_notpayin->opackid[0]=='\0')
  {
    memcpy(wp_notpayin->opackid,sHvFzf.o_packid, sizeof(sHvFzf.o_packid)-1);
  }
  lv_orderno_add0(wp_notpayin->opackid, sizeof(HvW_Packid)-1, HvW_Packid);
    
  if (wp_notpayin->opackday[0]=='\0')
  {
    apitoa(sHvFzf.o_pack_date, sizeof(wp_notpayin->opackday),wp_notpayin->opackday);
  }

  sHvFzf.res_sts[0] = wp_notpayin->respcode;  /** 是否同意退回 dxy 20060206**/

  ret = Lv_wbctl_Dec_Upd(g_pub_tx.reply," pack_date = %d and packid = '%s' ", sHvFzf.o_pack_date,HvW_Packid);
  if (ret)
  {
    vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
    WRITEMSG
      return -1;
  }
  ret = Lv_wbctl_Fet_Upd( &sLvWbctl, g_pub_tx.reply );
  if ( ret == 100 )
  {
    sprintf( acErrMsg, "原查询记录不存在!" );
    WRITEMSG
      return -1;
  }else if( ret )
  {
    sprintf( acErrMsg, "执行Lv_wbctl_Fet_Upd错![%d]", ret );
    WRITEMSG
      return -1;
  }
  if (memcmp(sLvWbctl.packstat, PACKSTAT_SEND, sizeof(sLvWbctl.packstat)-1) != 0 &&
    memcmp(sLvWbctl.packstat, PACKSTAT_SUCCESS, sizeof(sLvWbctl.packstat)-1) != 0 &&
    memcmp(sLvWbctl.packstat, PACKSTAT_INQUEUE, sizeof(sLvWbctl.packstat)-1) != 0 &&
    memcmp(sLvWbctl.packstat, PACKSTAT_QS, sizeof(sLvWbctl.packstat)-1) != 0)
  {
    vtcp_log("[%s][%d]应该是02,实际状态=[%s]\n",__FILE__,__LINE__,sLvWbctl.packstat);
    sprintf( acErrMsg, "状态错误" );
    WRITEMSG
      return -1;
  }
  vtcp_log("[%s][%d]============wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
  /** 根据能否退回等修改状态 **/
  if (wp_notpayin->respcode == '1')   /** 同意  **/
  {
    if (memcmp(wp_notpayin->cmtno,"322", 3) == 0)  /** 撤销 **/
    {
      sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
      wp_notpayin->stat = STAT_WZBACK;
      memcpy(sLvWbctl.packstat, PACKSTAT_CANCEL, sizeof(sLvWbctl.packstat)-1); 
    }
    /** added by dxy 20060206 收到退回应答时，修改原319的状态 **/
    else if (memcmp(wp_notpayin->cmtno,"320", 3) == 0)  /** 退回应答 **/
    {
      sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
      wp_notpayin->stat = STAT_WZBACK;
    }
    else if (memcmp(wp_notpayin->cmtno,"324", 3) == 0)  /** 冲正 **/
    {
      sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
      wp_notpayin->stat = STAT_WZBACK;
      memcpy(sLvWbctl.packstat, PACKSTAT_CZ, sizeof(sLvWbctl.packstat)-1); 
    }
    else if (memcmp(wp_notpayin->cmtno,"328", 3) == 0 )  /** 止付 **/
    {
      sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
      if (wp_notpayin->rettype == '1')
      {
        memcpy(sLvWbctl.packstat, PACKSTAT_STOPPAY, sizeof(sLvWbctl.packstat)-1); 
        wp_notpayin->stat = STAT_WZBACK;
      }
    }
  }
  else if (wp_notpayin->respcode == '2')  /** 不同意 **/
  {
    sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
    if (wp_notpayin->rettype == '1')
    {
      wp_notpayin->stat = STAT_WZBACK;
    }
  }
  
  vtcp_log("[%s][%d]===========退回类型0单笔,1整包rettype=[%c]\n",__FILE__,__LINE__,wp_notpayin->rettype);
  switch(iCmtno)
  {
  case 328:
    if (wp_notpayin->rettype == '0')    /** 单笔 **/
    {
      ret = Rev_Pkg_Stat_Sig(wp_notpayin);
      if (ret!=0)
      {
        vtcp_log("[%s][%d]更新单笔错误=%d\n",__FILE__,__LINE__,ret);
        sprintf( acErrMsg, "更新单笔错误" );
        WRITEMSG
          return -1;
      }
    }
    else if (wp_notpayin->rettype == '1')    /** 整包 **/
    {
      ret = Rev_Pkg_Stat_Mul(wp_notpayin);
      if (ret!=0)
      {
        vtcp_log("[%s][%d]更新整包错误=%d\n",__FILE__,__LINE__,ret);
        sprintf( acErrMsg, "更新整包错误" );
        WRITEMSG
          return -1;
      }
    }
    break;
  case 322:
  case 324:
    if (wp_notpayin->respcode == '1')  /** 不同意 **/
    {
      ret = Rev_Pkg_Stat_Mul(wp_notpayin);
      if (ret!=0)
      {
        vtcp_log("[%s][%d]更新整包错误=%d\n",__FILE__,__LINE__,ret);
        sprintf( acErrMsg, "更新整包错误" );
        WRITEMSG
          return -1;
      }
    }
    break;
  }
  ret = Lv_wbctl_Upd_Upd(sLvWbctl,g_pub_tx.reply);
  if (ret)
  {
    vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
    sprintf( acErrMsg, "更新整包错误" );
    WRITEMSG
      return -1;
  }
  Lv_wbctl_Clo_Upd();

  ret = Hv_fzf_Upd_Upd( sHvFzf, g_pub_tx.reply );
  if ( ret )
  {
    sprintf( acErrMsg, "修改非支付登记薄错[%d]", ret );
    WRITEMSG
      return -1;
  }
  Hv_fzf_Clo_Upd();
    

  return(0);
}

/************************************************
*  要求退回或止付单笔              *
*  退回的情况有ptdj/dqdj,止付的情况有ptjj,dqjj  *
*  均不用还原帐务                *
*  函数名: Rev_Pkg_Stat_Sig          *
************************************************/
int Rev_Pkg_Stat_Sig(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cPkgno[4];
  char  cOrderno[9];
  char  cWtday[9];
  char  cOrbrno[13];
  char  cStat[2];
  char  cStat2[2];
  int  ret=0;

  struct lv_pkgreg_c              sLvPkgreg;
  memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

  memset(cPkgno,  '\0', sizeof(cPkgno));
  memset(cOrderno,'\0', sizeof(cOrderno));
  memset(cWtday,  '\0', sizeof(cWtday));
  memset(cOrbrno, '\0', sizeof(cOrbrno));
  memset(cStat,  '\0', sizeof(cStat));
  memset(cStat2,  '\0', sizeof(cStat2));

  memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
  memcpy(cWtday,    wp_notpayin->owtday,   sizeof(cWtday)-1);
  memcpy(cOrbrno,  wp_notpayin->oorbrno,  sizeof(cOrbrno)-1);
  memcpy(cPkgno,   wp_notpayin->opkgno,   sizeof(cPkgno)-1);

  vtcp_log("[%s][%d]==============原包类型号=[%s]\n",__FILE__,__LINE__,cPkgno);
  vtcp_log("[%s][%d]======orbrno=[%s],orderno=[%s]wtday=[%s]\n",__FILE__,__LINE__,cOrbrno,cOrderno,cWtday);

  ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", cOrbrno,apatoi(cWtday,8),cOrderno);
  if (ret)
  {
    vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
    WRITEMSG
      return -1;
  }
  ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
  if ( ret == 100 )
  {
    sprintf( acErrMsg, "原查询记录不存在!" );
    WRITEMSG
      return -1;
  }else if( ret )
  {
    sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
    WRITEMSG
      return -1;
  }
  cStat[0] = sLvPkgreg.lv_sts[0];
  vtcp_log("[%s][%d]-----------------cStat=[%s]\n",__FILE__,__LINE__,cStat);

  if(memcmp(wp_notpayin->cmtno,"320",3)==0)
  {
    if (cStat[0] != STAT_WZSEND && cStat[0] != STAT_WZSUCCESS)
    {
      vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
      sprintf( acErrMsg, "状态错误" );
      WRITEMSG
        return -1;
    }
  }
  if (memcmp(wp_notpayin->cmtno,"324",3)==0 || memcmp(wp_notpayin->cmtno,"328",3)==0)
  {
    if (cStat[0] != STAT_WZSEND )
    {
      vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
      sprintf( acErrMsg, "状态错误" );
      WRITEMSG
        return -1;
    }
  }
  else if (memcmp( wp_notpayin->cmtno, "322", 3) == 0 )
  {
    if (cStat[0] != STAT_WZSEND && cStat[0] != STAT_WZINQUEUE)
    {
      vtcp_log("[%s][%d]=====状态应该是8,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
      sprintf( acErrMsg, "状态错误" );
      WRITEMSG
        return -1;
    }
  }
  if (wp_notpayin->respcode == '1')  /** 同意 **/
  {
    if (memcmp( wp_notpayin->cmtno, "328", 3) == 0 )
    {
      cStat2[0] = STAT_WZZF;
    }
  }
  else
  {
    if (memcmp( wp_notpayin->cmtno, "320", 3) == 0 )
    {
      cStat2[0] = STAT_WZSUCCESS;
    }
    else if (memcmp( wp_notpayin->cmtno, "328", 3) == 0 )
    {
      cStat2[0] = STAT_WZSEND;
    }
  }
  ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
  if ( ret )
  {
    sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
    WRITEMSG
      return -1;
  }
  Lv_pkgreg_Clo_Upd();

  return(0);

}

/****************************************
*  要求退回/撤销/冲正/止付整包      *
*  函数名: Rev_Pkg_Stat_Mul      *
****************************************/
int Rev_Pkg_Stat_Mul(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cPkgno[4];
  char  cStat[2];
  char  cStat2[2];
  char  cPackid[9];
  char  cPackday[9];
  char  Pkgno[4];
  char  Orbrno[13];
  char  Wtday[9];
  char  Orderno[9];
  char  sql_string[512];
  char  sql_string2[512];
  char  cLvwPackid[9];
  int    iPkgno=0,ret=0;

  struct  hv_fzf_c  sHvFzf;
  struct  lv_wbctl_c  sLvWbctl;
  struct  lv_pkgreg_c     sLvPkgreg;

  memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
  memset(&sLvWbctl,   '\0', sizeof(struct lv_wbctl_c));


  memset(cStat,    '\0', sizeof(cStat));
  memset(cStat2,  '\0', sizeof(cStat2));
  memset(cPkgno,   '\0', sizeof(cPkgno));
  memset(cPackid, '\0', sizeof(cPackid));
  memset(cLvwPackid, '\0', sizeof(cLvwPackid));
  memset(cPackday,'\0', sizeof(cPackday));
  memset(Pkgno,   '\0', sizeof(Pkgno));
  memset(Orbrno,   '\0', sizeof(Orbrno));
  memset(Wtday,   '\0', sizeof(Wtday));
  memset(Orderno, '\0', sizeof(Orderno));
  memset(sql_string, '\0', sizeof(sql_string));
  memset(sql_string2, '\0', sizeof(sql_string2));

  vtcp_log("[%s][%d]==============cPackday==[%s]\n",__FILE__,__LINE__,cPackday);
  vtcp_log("[%s][%d]==============cLvwPackid==[%s]\n",__FILE__,__LINE__,cLvwPackid);  

  cStat[0] = wp_notpayin->stat;
  memcpy(cPkgno,    wp_notpayin->opkgno,   sizeof(cPkgno)-1);
  memcpy(cPackid,  wp_notpayin->opackid,  sizeof(cPackid)-1);
  memcpy(cPackday, wp_notpayin->opackday, sizeof(cPackday)-1);

  lv_orderno_add0(wp_notpayin->opackid,sizeof(cLvwPackid)-1,cLvwPackid);

  ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLvWbctl," pack_date = %d and packid = '%s'", apatoi(cPackday,8) ,cLvwPackid);
  if ( ret == 100 )
  {
    sprintf( acErrMsg, "查询往包控制表记录不存在!" );
    WRITEMSG
      return -1;
  }else if( ret )
  {
    sprintf( acErrMsg, "执行Lv_wbctl_Sel错![%d]", ret );
    WRITEMSG
      return -1;
  }
  iPkgno = apatoi(cPkgno,3);

  vtcp_log("[%s][%d]==============stat[%s]\n",__FILE__,__LINE__,cStat);
  vtcp_log("[%s][%d]==============cPackid[%s]\n",__FILE__,__LINE__,cPackid);
  vtcp_log("[%s][%d]==============cPackday[%s]\n",__FILE__,__LINE__,cPackday);

  vtcp_log("[%s][%d]==============原包类型号=[%d]\n",__FILE__,__LINE__,iPkgno);

  vtcp_log("[%s][%d]==============opkgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);

  if (memcmp(wp_notpayin->opkgno,PKGNO_PTDJ,strlen(PKGNO_PTDJ))==0 || 
    memcmp(wp_notpayin->opkgno,PKGNO_DQDJ,strlen(PKGNO_DQDJ))==0 ||
  memcmp(wp_notpayin->opkgno,PKGNO_DJTH,strlen(PKGNO_DJTH))==0)
  {
    vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_PTDJ);
    sprintf(sql_string," pack_date=%d and packid='%s'",apatoi(cPackday,8),cPackid);
  }else if(memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_SSDJ))==0 ||
  memcmp(wp_notpayin->opkgno,PKGNO_PTJJ,strlen(PKGNO_PTJJ))==0 ||
    memcmp(wp_notpayin->opkgno,PKGNO_DQJJ,strlen(PKGNO_DQDJ))==0 ||
  memcmp(wp_notpayin->opkgno,PKGNO_SSJJ,strlen(PKGNO_SSJJ))==0)
  {
    vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_SSDJ);
    sprintf(sql_string," pack_date=%d and packid='%s' and pay_qs_no='%s'",apatoi(cPackday,8),cPackid,LV_QSBRNO);
  }else if(memcmp(wp_notpayin->opkgno,PKGNO_PTJJHZ,strlen(PKGNO_PTJJHZ))==0 ||
  memcmp(wp_notpayin->opkgno,PKGNO_DQJJHZ,strlen(PKGNO_DQJJHZ))==0)
  {
    vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_PTJJHZ);
    sprintf(sql_string," pack_date=%d  and packid='%s' and pay_qs_no='%s' ",apatoi(sLvWbctl.o_pack_date,8),sLvWbctl.o_packid,sLvWbctl.cash_qs_no);
  }
  ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,sql_string);
  if (ret)
  {
    vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
    WRITEMSG
      return -1;
  }
  
  while (1)
  {
    
    memset(&sLvPkgreg,  '\0', sizeof(struct lv_pkgreg_c));    
    ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
    if ( ret == 100 )    
    {
      vtcp_log("%s,%d,数据处理完毕!\n",__FILE__,__LINE__);
      sprintf( acErrMsg, "数据处理完毕!" );
      break;
    }  
    else  if ( ret )
    {
        sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
        WRITEMSG
        return -1;
    }


    vtcp_log("[%s][%d]=========sLvPkgreg.or_br_no=[%s]\n",__FILE__,__LINE__,sLvPkgreg.or_br_no);
    vtcp_log("[%s][%d]=========sLvPkgreg.wt_date=[%d]\n",__FILE__,__LINE__,sLvPkgreg.wt_date);
    vtcp_log("[%s][%d]=========sLvPkgreg.orderno=[%s]\n",__FILE__,__LINE__,sLvPkgreg.orderno);
    vtcp_log("[%s][%d]=========sLvPkgreg.pkgno=[%s]\n",__FILE__,__LINE__,sLvPkgreg.pkgno);
    vtcp_log("[%s][%d]=========sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,sLvPkgreg.lv_sts);
    /** 先判断状态是否正确 **/
    if(memcmp(wp_notpayin->cmtno,"320",3)==0)
    {
      if (sLvPkgreg.lv_sts[0] != STAT_WZSEND && sLvPkgreg.lv_sts[0] != STAT_WZSUCCESS )
      {
        vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat);
        sprintf( acErrMsg, "状态错误![%d]", ret );
        WRITEMSG
          return -1;
      }
    }
    if (memcmp(wp_notpayin->cmtno,"324",3)==0 || memcmp(wp_notpayin->cmtno,"328",3)==0)
    {
      if (sLvPkgreg.lv_sts[0] != STAT_WZSEND )
      {
        vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat);
        sprintf( acErrMsg, "状态错误![%d]", ret );
        WRITEMSG
          return -1;
      }
    }
    else if (memcmp(wp_notpayin->cmtno, "322", 3) == 0 )
    {
      if (sLvPkgreg.lv_sts[0] != STAT_WZSEND && sLvPkgreg.lv_sts[0] != STAT_WZINQUEUE)
      {
        vtcp_log("[%s][%d]=====状态应该是8,现在是=[%c]\n",__FILE__,__LINE__,cStat);
        sprintf( acErrMsg, "状态错误![%d]", ret );
        WRITEMSG
          return -1;
      }
    }
    vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
    vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
    vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
    vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

    memcpy(wp_notpayin->opkgno, sLvPkgreg.pkgno, sizeof(wp_notpayin->opkgno));
    memcpy(wp_notpayin->oorbrno,sLvPkgreg.or_br_no,sizeof(wp_notpayin->oorbrno));
    apitoa(sLvPkgreg.wt_date, sizeof(wp_notpayin->owtday),wp_notpayin->owtday);
    memcpy(wp_notpayin->oorderno,sLvPkgreg.orderno,sizeof(wp_notpayin->oorderno));

    vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
    vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
    vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
    vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

    /** 328只需改状态 **/
    if (memcmp(wp_notpayin->cmtno,"328",3)==0 )
    {
      cStat2[0] = STAT_WZZF;
    }
    else if (memcmp(wp_notpayin->cmtno,"324",3)==0 )
    {
      cStat2[0] = STAT_WZCZ;
      /** 实时贷记需要回滚帐务 **/
      if (memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_DQJJHZ))==0)
      {
        ret=lv_Reb_Pkg(wp_notpayin,OPCD_RESP);
        if(ret)
        {
          vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
          sprintf( acErrMsg, "处理原交易错误!" );
          WRITEMSG
            return(-1);
        }
      }
    }
    else if (memcmp(wp_notpayin->cmtno,"322",3)==0 )
    {
      cStat2[0] = STAT_WZCANCEL;
      vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
      vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
      vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
      vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);
      ret = lv_Reb_Pkg(wp_notpayin,OPCD_RESP);
      if(ret)
      {
        vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
        sprintf( acErrMsg, "处理原交易错误!" );
        WRITEMSG
          return(-1);
      }
    }
    
    if (memcmp(wp_notpayin->cmtno,"320",3)!=0 )
    {
      if (memcmp(wp_notpayin->opkgno,PKGNO_PTDJ,strlen(PKGNO_PTDJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_DQDJ,strlen(PKGNO_DQDJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_SSDJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_PTJJ,strlen(PKGNO_PTJJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_PTJJHZ,strlen(PKGNO_PTJJHZ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_DQJJ,strlen(PKGNO_DQJJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_DQJJHZ,strlen(PKGNO_DQJJHZ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_SSJJ,strlen(PKGNO_SSJJ))==0 ||
        memcmp(wp_notpayin->opkgno,PKGNO_DJTH,strlen(PKGNO_DQJJHZ))==0)
      {
        sLvPkgreg.lv_sts[0]=cStat2[0];
        ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
        if ( ret )
        {
          sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
          WRITEMSG
            return -1;
        }
      }
    }
  }
  
  Lv_pkgreg_Clo_Upd();

  return 0;
}
/************************************************************************************
*  冲正325                                      *
*  325和324的区别是:假设A已经发了一笔业务给B,这时A发起冲正,CCPC或NPC收到冲正申请时  *
*  告诉A可以或不可以冲正的是用324,告诉B要冲正是用325                *
*  所以325需要读来包控制表                              *
*  函数名: Rev_Pkg_Stat_Sig                            *
************************************************************************************/
int Rev_325_Stat_Sig(NOTPAY_IN_AREA *wp_notpayin)
{
  int  ret=0;
  struct  lv_lbctl_c   sLvLbctl;
  struct  lv_pkgreg_c  sLvPkgreg;

  memset(&sLvLbctl,  '\0', sizeof(struct  lv_lbctl_c));
  memset(&sLvPkgreg, '\0', sizeof(struct  lv_pkgreg_c));

  ret = Lv_lbctl_Dec_Upd(g_pub_tx.reply," pack_date =%d and packid = '%s' and pay_qs_no='%s'", apatoi(wp_notpayin->opackday,8), wp_notpayin->opackid,wp_notpayin->payqsactno);
  if (ret)
  {
    vtcp_log("[%s][%d]锁定来包控制表错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
    WRITEMSG
      return -1;
  }
  ret = Lv_lbctl_Fet_Upd( &sLvLbctl, g_pub_tx.reply );
  if ( ret == 100 )
  {
    sprintf( acErrMsg, "原查询记录不存在!" );
    WRITEMSG
      return -1;
  }else if( ret )
  {
    sprintf( acErrMsg, "执行Lv_lbctl_Fet_Upd错误![%d]", ret );
    WRITEMSG
      return -1;
  }
  /****NEWYX BEGIN:测试冲正通知无条件接受****/
  if(getenv("FHTEST")==NULL || strcmp(getenv("FHTEST"),"LVTEST")!=0)
  {
      if (sLvLbctl.sts[0] != PSTAT_RECEIVE)
      {
        vtcp_log("[%s][%d]应该是5,实际状态=[%s]\n",__FILE__,__LINE__,sLvLbctl.sts);
        sprintf( acErrMsg, "状态错误!");
        WRITEMSG
          return(0);
      }
      if (memcmp(sLvLbctl.packstat, PACKSTAT_SUCCESS,sizeof(sLvLbctl.packstat)-1) != 0 &&
        memcmp(sLvLbctl.packstat, PACKSTAT_QS,sizeof(sLvLbctl.packstat)-1) != 0)
      {
        vtcp_log("[%s][%d]应该是03,实际状态=[%s]\n",__FILE__,__LINE__,sLvLbctl.packstat);
        sprintf( acErrMsg, "状态错误!");
        WRITEMSG
          return -1;
      }
  }
  /****NEWYX END:测试冲正通知无条件接受****/
  sLvLbctl.sts[0] = PSTAT_LRESP;
  memcpy(sLvLbctl.packstat, PACKSTAT_CZ,sizeof(sLvLbctl.packstat)-1);

  ret = Lv_lbctl_Upd_Upd(sLvLbctl,g_pub_tx.reply);
  if ( ret )
  {
    sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
    WRITEMSG
      return -1;
  }
  Lv_lbctl_Clo_Upd();

  if (memcmp(wp_notpayin->opkgno, PKGNO_SSJJ, 3) == 0)  /** 实时借记 **/
  {
    ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date =%d and orderno = '%s' ", wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,8),wp_notpayin->oorderno);
    if (ret)
    {
      vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
      WRITEMSG
        return -1;
    }
    ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
    if ( ret == 100 )
    {
      sprintf( acErrMsg, "原查询记录不存在!" );
      WRITEMSG
        return -1;
    }else if( ret )
    {
      sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
      WRITEMSG
        return -1;
    }
    /****NEWYS BEGIN:测试冲正通知无条件接受****/
    if(getenv("FHTEST")==NULL || strcmp(getenv("FHTEST"),"LVTEST")!=0)
    {
        if (sLvPkgreg.lv_sts[0] != STAT_LZRECV2)
        {
            vtcp_log("[%s][%d]原来帐状态错误!应该是I,实际是=[%c]\n",__FILE__,__LINE__,sLvPkgreg.lv_sts[0]);
            sprintf( acErrMsg, "原来帐状态错误");
            WRITEMSG
            return (0);
        }
    }
    /****NEWYS END:测试冲正通知无条件接受****/
    vtcp_log("[%s][%d]==========\n",__FILE__,__LINE__);
    ret=lv_Reb_Pkg(wp_notpayin,OPCD_RECEIVE);
    if(ret!=0)
    {
      vtcp_log("%s,%d,处理原交易错误ret=[%d]\n",__FILE__,__LINE__,ret);
      sprintf( acErrMsg, "处理原交易错误");
      WRITEMSG
        return(0);
    }
    sLvPkgreg.lv_sts[0] = STAT_LZCZ;

    ret = Lv_pkgreg_Upd_Upd(sLvPkgreg,g_pub_tx.reply );
    if ( ret )
    {
      sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
      WRITEMSG
        return -1;
    }  
    Lv_pkgreg_Clo_Upd();
    /*** YHBP BEGIN:冲正添加本票处理 ***/
    if(memcmp(sLvPkgreg.txnum,TXNUM_TYJL,sizeof(sLvPkgreg.txnum))==0){
        ret=iLv_Reb_Bp(sLvPkgreg.in_date,sLvPkgreg.addid);
        if(ret!=0){
            vtcp_log("%s,%d,处理原交易错误ret=[%d]\n",__FILE__,__LINE__,ret);
            sprintf( acErrMsg, "处理原交易失败[%d]", ret );
            WRITEMSG
            return(0);
        }
    }
    /*** YHBP END:冲正添加本票处理 ***/
  }
  else if (memcmp(wp_notpayin->opkgno, PKGNO_SSDJ, 3) == 0)    /** 实时贷记 **/
  {
    ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date =%d and orderno = '%s' ", wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,8),wp_notpayin->oorderno);
    if (ret)
    {
      vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
      WRITEMSG
        return -1;
    }
    ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
    if ( ret == 100 )
    {
      sprintf( acErrMsg, "原查询记录不存在!" );
      WRITEMSG
        return -1;
    }else if( ret )
    {
      sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
      WRITEMSG
        return -1;
    }
    /****NEWYX BEGIN:测试冲正通知无条件接受****/
    if(getenv("FHTEST")==NULL || strcmp(getenv("FHTEST"),"LVTEST")!=0)
    {
        if (sLvPkgreg.lv_sts[0] != STAT_LZRECV2)
        {
          vtcp_log("[%s][%d]状态错误\n",__FILE__,__LINE__);
          sprintf( acErrMsg, "状态错误!");
          WRITEMSG
            return (0);
        }
    }
    /****NEWYX END:测试冲正通知无条件接受****/
    sLvPkgreg.lv_sts[0] = STAT_LZCZ;

    ret = Lv_pkgreg_Upd_Upd(sLvPkgreg,g_pub_tx.reply );
    if ( ret )
    {
      sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
      WRITEMSG
        return -1;
    }
    Lv_pkgreg_Clo_Upd();
  }
  return(0);

}
/****************************************
*  将原来帐或往帐的帐务回滚      *
*  函数:lv_Reb_Pkg            *
****************************************/
int lv_Reb_Pkg(NOTPAY_IN_AREA *wp_notpayin,char *opcd)/***322 \ 324 改往帐，325 改来帐 这个程序的目的是什么呢？？更新数据没看啊并且N多地方是错误的WSMNE***/
{
  
  int  ret=0;
  char qsbrno[8];
  PAY_IN_AREA payin;
  memset(&payin,'\0',sizeof(payin));
  memset(qsbrno,'\0',sizeof(qsbrno));

  vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%.3s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
  vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%.12s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
  vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%.8s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
  vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%.8s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

  memcpy(payin.F_pkgno, wp_notpayin->opkgno, sizeof(payin.F_pkgno));
  memcpy(payin.F_orbrno,wp_notpayin->oorbrno,sizeof(payin.F_orbrno));
  memcpy(payin.F_wtday, wp_notpayin->owtday, sizeof(payin.F_wtday));
  memcpy(payin.F_orderno,wp_notpayin->oorderno,sizeof(payin.F_orderno));
  vtcp_log("[%s][%d]==============原来帐是[%.8s]\n",__FILE__,__LINE__,payin.F_orderno);
  memcpy(payin.F_opcd,opcd,sizeof(payin.F_opcd));

  vtcp_log("[%s][%d]=======50应答,60接收==payin.F_opcd=[%.2s]\n",__FILE__,__LINE__,payin.F_opcd);
  vtcp_log("[%s][%d]------------payin.F_pkgno=[%.3s]\n",__FILE__,__LINE__,payin.F_pkgno);

  if(memcmp(payin.F_opcd,OPCD_RECEIVE,sizeof(payin.F_opcd))==0 ||
     memcpy(payin.F_opcd,OPCD_RESP,sizeof(payin.F_opcd)))
  {
      if(memcmp(payin.F_pkgno,PKGNO_SSJJ,sizeof(payin.F_pkgno))==0)
      {
          memcpy(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno));
          vtcp_log("[%s][%d]------------payin.F_pkgno=[%s]\n",__FILE__,__LINE__,payin.F_pkgno);
      }
  }
  else
  {
      vtcp_log("%s,%d,报文格式错误[%.6s]\n",__FILE__,__LINE__,payin.F_pkgno);
      strcpy(acErrMsg,"报文格式错误");WRITEMSG
      return(-1);
  }
  vtcp_log("[%s][%d]------------payin.F_pkgno=[%.3s]\n",__FILE__,__LINE__,payin.F_pkgno);
  
  ret=iLvFetchRecToPayin(&payin);
  if(ret)
  {
      vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,ret);
      strcpy(acErrMsg,"数据库错误");WRITEMSG
      return(-1);
  }

  iLvResetPayin(&payin);

  if(memcmp(payin.F_pkgno,PKGNO_SSDJ,sizeof(payin.F_pkgno))==0||
     memcmp(payin.F_pkgno,PKGNO_SSJJ,sizeof(payin.F_pkgno))==0||
     memcmp(payin.F_pkgno,PKGNO_DQDJ,sizeof(payin.F_pkgno))==0||
     memcmp(payin.F_pkgno,PKGNO_PTDJ,sizeof(payin.F_pkgno))==0||
     memcmp(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno))==0||
    memcmp(payin.F_pkgno,PKGNO_SSDJHZ,sizeof(payin.F_pkgno))==0 )
  {
      vtcp_log("[%s][%d]---------------payin.T_brno=[%.5s]\n",__FILE__,__LINE__,payin.T_brno);
      vtcp_log("[%s][%d]---------------payin.A_brno=[%.5s]\n",__FILE__,__LINE__,payin.A_brno);
      vtcp_log("[%s][%d]---------------payin.F_orbrno=[%.12s]\n",__FILE__,__LINE__,payin.F_orbrno);
      vtcp_log("[%s][%d]---------------payin.F_acbrno=[%.12s]\n",__FILE__,__LINE__,payin.F_acbrno);
      iLvGetRecvBrno(&payin);
      vtcp_log("[%s][%d]---------------payin.A_brno=[%s]\n",__FILE__,__LINE__,payin.A_brno);
      /* memcpy(TITA.kinbr,payin.A_brno,sizeof(TITA.kinbr));*/
      pcLvGetRecvno();
      /* memcpy(payin.A_wssrno,get_format_seq( 6,dg_get_wssrno()),  sizeof(payin.A_wssrno));*/
  }
  
  vtcp_log("[%s][%d]========0借1贷=====payin.F_crdb[0]=[%c]\n",__FILE__,__LINE__,payin.F_crdb[0]);
  vtcp_log("[%s][%d]=============payin.F_cashactno=[%.12s]\n",__FILE__,__LINE__,payin.F_cashactno);
  vtcp_log("[%s][%d]=============payin.F_payactno=[%.12s]\n",__FILE__,__LINE__,payin.F_payactno);
  vtcp_log("[%s][%d]---------------payin.A_brno=[%.5s]\n",__FILE__,__LINE__,payin.A_brno);

/*
  if(payin.F_crdb[0]==CRDB_DR && memcmp(wp_notpayin->cmtno,"322",3))
  {  
      ret=iLvPayinCheck3(&payin);
      vtcp_log("[%s],[%d] ret=[%d] \n",__FILE__,__LINE__,ret);
      vtcp_log("[%s],[%d] cmtno=[%s] \n",__FILE__,__LINE__,wp_notpayin->cmtno);
  }
  else
  {
      ret=iLvPayinCheck2(&payin);
      vtcp_log("[%s],[%d] ret=[%d] \n",__FILE__,__LINE__,ret);
      vtcp_log("[%s],[%d] cmtno=[%s] \n",__FILE__,__LINE__,wp_notpayin->cmtno);
  }
  if(ret<0){
  vtcp_log("%s,%d,帐号检查错误\n",__FILE__,__LINE__);
  vtcp_log("[%s],[%d] cmtno=[%s] \n",__FILE__,__LINE__,wp_notpayin->cmtno);
  strcpy(acErrMsg,"帐号检查错误");WRITEMSG
  return(-1);
  }
*/
  ret=iLvUpdRecFromPayin(&payin,0);
  if(ret)
  {
      vtcp_log("%s,%d,更新原交易表错误\n",__FILE__,__LINE__);
      strcpy(acErrMsg,"更新原交易表错误");WRITEMSG
      return(-1);
  }
  vtcp_log("[%s][%d]  lv_Reb_Pkg() Finished ==========\n",__FILE__,__LINE__);

  return 0;

}
