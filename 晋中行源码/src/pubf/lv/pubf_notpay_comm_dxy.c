/***************************************************************************/
/* 文件名称：小额支付公共函数（非支付交易）。                              */
/*                                                                         */
/* 功能描述：                                                              */
/*                                                                         */
/* 日期：  2005.07.20                                                      */
/* 作者：  WenShuanghua                                                    */
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
#include "hv_brno_orno_c.h"
#include "errlog.h"
#include "lvpack.h"
#include "public.h"

/*准备输出包*/
/*****************未改
int notpayin_to_notpay_out(NOTPAY_IN_AREA *wp_notpayin,NOTPAY_OUT_AREA *wp_notpay_out)
{
NOTPAY_IN_AREA   wd_notpayin;
NOTPAY_OUT_AREA  wd_notpay_out;
memset(&wd_notpayin,  '\0', sizeof(wd_notpayin));
memset(&wd_notpay_out, '\0', sizeof(wd_notpay_out));

memcpy(&wd_notpayin, wp_notpayin, sizeof(wd_notpayin));
memcpy(wd_notpay_out.txday, wd_notpayin.txday, sizeof(wd_notpay_out.txday));
memcpy(wd_notpay_out.brno,  TITA.kinbr, sizeof(wd_notpay_out.brno));
memcpy(wd_notpay_out.orderno, wd_notpayin.orderno, sizeof(wd_notpay_out.orderno));
strcpy(wd_notpay_out.txtype, pcLvPayinTxtype(wd_notpayin.txnum));
memcpy(wd_notpay_out.orbrno, wd_notpayin.orbrno, sizeof(wd_notpay_out.orbrno));
memcpy(wd_notpay_out.acbrno, wd_notpayin.acbrno, sizeof(wd_notpay_out.acbrno));

memcpy(wd_notpay_out.content, wd_notpayin.content, sizeof(wd_notpay_out.content));
if (TITA.hcode == TX_HCODE_NORMAL)
{
strcpy(wd_notpay_out.wssrno,get_format_seq( 6,dg_get_wssrno()));
}
else
{
memcpy(wd_notpay_out.wssrno,TITA.hejfno,6);
memcpy(wd_notpay_out.wssrno,"X",1);
}
if (!memcmp(wd_notpayin.txnum,TXNUM_DS,TXNUM_LEN))
wd_notpay_out.sfflag = '0';
else if (!memcmp(wd_notpayin.txnum,TXNUM_DF,TXNUM_LEN))
wd_notpay_out.sfflag = '1';

wd_notpay_out.acttype = wd_notpayin.acttype;
wd_notpay_out.pswdcd = wd_notpayin.pswdcd;
wd_notpay_out.qtype=wd_notpayin.qtype;

memcpy(wd_notpay_out.unit     , wd_notpayin.unit     , sizeof(wd_notpay_out.unit    ));
memcpy(wd_notpay_out.belongcyc, wd_notpayin.belongcyc, sizeof(wd_notpay_out.belongcyc));
memcpy(wd_notpay_out.sftype   , wd_notpayin.sftype   , sizeof(wd_notpay_out.sftype  ));
memcpy(wd_notpay_out.actno    , wd_notpayin.actno    , sizeof(wd_notpay_out.actno   ));
memcpy(wd_notpay_out.name     , wd_notpayin.name     , sizeof(wd_notpay_out.name    ));
memcpy(wd_notpay_out.passwd   , wd_notpayin.passwd   , sizeof(wd_notpay_out.passwd  ));

memcpy(wd_notpay_out.qcontent, wd_notpayin.reason  , sizeof(wd_notpay_out.qcontent));
memcpy(wd_notpay_out.opackday, wd_notpayin.opackday, sizeof(wd_notpay_out.opackday));
memcpy(wd_notpay_out.opackid , wd_notpayin.opackid , sizeof(wd_notpay_out.opackid ));
memcpy(wd_notpay_out.oorderno, wd_notpayin.oorderno, sizeof(wd_notpay_out.oorderno));
memcpy(wd_notpay_out.opkgno  , wd_notpayin.opkgno  , sizeof(wd_notpay_out.opkgno));
memcpy(wd_notpay_out.avbal   , wd_notpayin.otxamt  , sizeof(wd_notpay_out.avbal   ));

strcpy(wd_notpay_out.otxtype, pcLvPayinTxtype(wd_notpayin.otxnum));
memcpy(wp_notpay_out, &wd_notpay_out, sizeof(wd_notpay_out));
#if 0
notpay_out_printf(&wd_notpay_out);
#endif
vtcp_log("[%s][%d]avbal=[%s]otxamt=[%.15s]\n\n\n",__FILE__,__LINE__,wd_notpay_out.avbal,wd_notpayin.otxamt);

return 0;
}
**************/
/*得到非支付交易的描述*/
/***********未改
char * lv_notpay_txname(NOTPAY_IN_AREA *wd_notpayin)
{
char txname[20];
memset(txname, 0, sizeof(txname));
*对pkg报文*
if (memcmp(wd_notpayin->pkgno, PKGNO_TYXX, 3)==0)
memcpy(txname, "通用信息业务", 12);
else if (memcmp(wd_notpayin->pkgno, PKGNO_SSXX, 3)==0)
memcpy(txname, "实时信息业务", 12);

*对cmt报文*
if (memcmp(wd_notpayin->cmtno, "301", 3)==0)
memcpy(txname, "小额查询业务", 12);
else if (memcmp(wd_notpayin->cmtno, "302", 3)==0)
memcpy(txname, "小额查复业务", 12);
else if (memcmp(wd_notpayin->cmtno, "303", 3)==0)
memcpy(txname, "小额自由格式", 12);
else if (memcmp(wd_notpayin->cmtno, "319", 3)==0)
memcpy(txname, "小额申请退回", 12);
else if (memcmp(wd_notpayin->cmtno, "320", 3)==0)
memcpy(txname, "小额退回应答", 12);
else if (memcmp(wd_notpayin->cmtno, "321", 3)==0)
memcpy(txname, "小额申请撤销", 12);
else if (memcmp(wd_notpayin->cmtno, "322", 3)==0)
memcpy(txname, "小额撤销应答", 12);
else if (memcmp(wd_notpayin->cmtno, "323", 3)==0)
memcpy(txname, "小额冲正申请", 12);
else if (memcmp(wd_notpayin->cmtno, "324", 3)==0)
memcpy(txname, "小额冲正应答", 12);
else if (memcmp(wd_notpayin->cmtno, "325", 3)==0)
memcpy(txname, "小额冲正通知", 12);
else if (memcmp(wd_notpayin->cmtno, "327", 3)==0)
memcpy(txname, "小额止付申请", 12);
else if (memcmp(wd_notpayin->cmtno, "328", 3)==0)
memcpy(txname, "小额止付应答", 12);

return(txname);
}
****************/
/****
int notpay_out_printf(NOTPAY_OUT_AREA *wp_notpay_out)
{
printf("[%s][%d]filename =[%16s]\n" , __FILE__,__LINE__,wp_notpay_out->filename );
printf("[%s][%d]txday    =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->txday    );
printf("[%s][%d]brno     =[%4s] \n" , __FILE__,__LINE__,wp_notpay_out->brno     );
printf("[%s][%d]orderno  =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->orderno  );
printf("[%s][%d]txtype   =[%12s]\n" , __FILE__,__LINE__,wp_notpay_out->txtype   );
printf("[%s][%d]orbrno   =[%12s]\n" , __FILE__,__LINE__,wp_notpay_out->orbrno   );
printf("[%s][%d]acbrno   =[%12s]\n" , __FILE__,__LINE__,wp_notpay_out->acbrno   );
printf("[%s][%d]wssrno   =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->wssrno   );
printf("[%s][%d]sfflag   =[%c]  \n" , __FILE__,__LINE__,wp_notpay_out->sfflag   );
printf("[%s][%d]unit     =[%60s]\n" , __FILE__,__LINE__,wp_notpay_out->unit     );
printf("[%s][%d]belongcyc=[%16s]\n" , __FILE__,__LINE__,wp_notpay_out->belongcyc);
printf("[%s][%d]pdate    =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->pdate    );
printf("[%s][%d]pnum     =[%20s]\n" , __FILE__,__LINE__,wp_notpay_out->pnum     );
printf("[%s][%d]paybrname=[%60s]\n" , __FILE__,__LINE__,wp_notpay_out->paybrname);
printf("[%s][%d]bsnum    =[%2s] \n" , __FILE__,__LINE__,wp_notpay_out->bsnum    );
printf("[%s][%d]datalen  =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->datalen  );
printf("[%s][%d]reason   =[%60s]\n" , __FILE__,__LINE__,wp_notpay_out->reason   );
printf("[%s][%d]sftype   =[%c]  \n" , __FILE__,__LINE__,wp_notpay_out->sftype   );
printf("[%s][%d]acttype  =[%c]  \n" , __FILE__,__LINE__,wp_notpay_out->acttype  );
printf("[%s][%d]actno    =[%32s]\n" , __FILE__,__LINE__,wp_notpay_out->actno    );
printf("[%s][%d]name     =[%60s]\n" , __FILE__,__LINE__,wp_notpay_out->name     );
printf("[%s][%d]pswdcd   =[%c]  \n" , __FILE__,__LINE__,wp_notpay_out->pswdcd   );
printf("[%s][%d]qtype    =[%c]  \n" , __FILE__,__LINE__,wp_notpay_out->qtype    );
printf("[%s][%d]respday  =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->respday  );
printf("[%s][%d]qcontent =[%60s]\n" , __FILE__,__LINE__,wp_notpay_out->qcontent );
printf("[%s][%d]opackday =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->opackday );
printf("[%s][%d]opackid  =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->opackid  );
printf("[%s][%d]opkgno   =[%3s] \n" , __FILE__,__LINE__,wp_notpay_out->opkgno   );
printf("[%s][%d]otxtype  =[%12s]\n" , __FILE__,__LINE__,wp_notpay_out->otxtype  );
printf("[%s][%d]oorderno =[%8s] \n" , __FILE__,__LINE__,wp_notpay_out->oorderno );
printf("[%s][%d]avbal    =[%15s]\n" , __FILE__,__LINE__,wp_notpay_out->avbal    );

printf("[%s][%d]content =[%255s]\n" , __FILE__,__LINE__,wp_notpay_out->content  );
printf("[%s][%d]passwd  =[%128s]\n" , __FILE__,__LINE__,wp_notpay_out->passwd   );
return 0;
}
*********/
/*拼包函数*/

/*************测试函数 add by liuyue 20051114*************************
* 功能:打印出从前台获得的notpayin的数据                             *
* 实现:打印NOTPAY_IN_AREA结构体中的各个数据断                        *
****************
int print_notpayin(NOTPAY_IN_AREA *notpayin)
{
printf("[%s][%d]Now begin to print notpayin!!! \n",__FILE__,__LINE__);
printf("optype===[%s] \n",notpayin->optype);
printf("pkgno===[%s] \n",notpayin->pkgno);
printf("cmtno===[%s] \n",notpayin->cmtno);
printf("orderno===[%s] \n",notpayin->orderno);
printf("txnum===[%s] \n",notpayin->txnum);
printf("acbrno===[%s] \n",notpayin->acbrno);
printf("rettype;===[%c] \n",notpayin->rettype);
printf("respcode;===[%c] \n",notpayin->respcode);
printf("opackday===[%s] \n",notpayin->opackday);
printf("opkgno===[%s] \n",notpayin->opkgno);
printf("opackid===[%s] \n",notpayin->opackid);
printf("otxnum===[%s] \n",notpayin->otxnum);
printf("owtday===[%s] \n",notpayin->owtday);
printf("oorderno===[%s] \n",notpayin->oorderno);
printf("qorderno===[%s] \n",notpayin->qorderno);
printf("qday===[%s] \n",notpayin->qwtday);
printf("oorbrno===[%s] \n",notpayin->oorbrno);
printf("oacbrno===[%s] \n",notpayin->oacbrno);
printf("otxtype;===[%s] \n",notpayin->otxtype);
printf("cour===[%s] \n",notpayin->cour);
printf("otxamt===[%s] \n",notpayin->otxamt);
printf("content===[%s] \n",notpayin->content);
printf("unit===[%s] \n",notpayin->unit);
printf("belongcyc===[%s] \n",notpayin->belongcyc);
printf("sftype===[%s] \n",notpayin->sftype);
printf("pdate===[%s] \n",notpayin->pdate);
printf("pnum===[%s] \n",notpayin->pnum);
printf("paybrname===[%s] \n",notpayin->paybrname);
printf("actno===[%s] \n",notpayin->actno);
printf("name===[%s] \n",notpayin->name);
printf("reason===[%s] \n",notpayin->reason);
printf("bsnum===[%s] \n",notpayin->bsnum);
printf("passwd===[%s] \n",notpayin->passwd);
printf("datalen===[%s] \n",notpayin->datalen);
printf("acttype;===[%s] \n",notpayin->acttype);
printf("pswdcd;===[%s] \n",notpayin->pswdcd);
printf("pascode===[%s] \n",notpayin->pascode);
printf("qtype;===[%s] \n",notpayin->qtype);
printf("[%s][%d]Begin to print auto part!!!! \n",__FILE__,__LINE__);
printf("orbrno===[%s] \n",notpayin->orbrno );
printf("payqsactno===[%s] \n",notpayin->payqsactno);
printf("sendco===[%s] \n",notpayin->sendco );
printf("cashqsactno===[%s] \n",notpayin->cashqsactno	);
printf("receco===[%s] \n",notpayin->receco);
printf("wssrno===[%s] \n",notpayin->wssrno);
printf("txday===[%s] \n",notpayin->txday);
printf("stat;===[%s] \n",notpayin->stat);
printf("datatype;===[%s] \n",notpayin->datatype);
printf("addid===[%s] \n",notpayin->addid);
printf("OK!print END!!! \n");
printf("[%s][%d] \n",__FILE__,__LINE__);
return(0);

} 
int print_notpay_out(NOTPAY_OUT_AREA *notpay_out)
{
printf("[%s][%d]Begin to print notpay_out!!\n",__FILE__,__LINE__);
printf("filename===[%s] \n",notpay_out->filename);
printf("txday===[%s] \n",notpay_out->txday);
printf("brno===[%s] \n",notpay_out->brno);
printf("orderno===[%s] \n",notpay_out->orderno);
printf("txtype===[%s] \n",notpay_out->txtype);
printf("orbrno===[%s] \n",notpay_out->orbrno);
printf("acbrno===[%s] \n",notpay_out->acbrno);
printf("content===[%s] \n",notpay_out->content);
printf("wssrno===[%s] \n",notpay_out->wssrno);
printf("sfflag;===[%s] \n",notpay_out->sfflag);
printf("unit===[%s] \n",notpay_out->unit);
printf("belongcyc===[%s] \n",notpay_out->belongcyc);
printf("pdate===[%s] \n",notpay_out->pdate);
printf("pnum===[%s] \n",notpay_out->pnum);
printf("paybrname===[%s] \n",notpay_out->paybrname);
printf("bsnum===[%s] \n",notpay_out->bsnum);
printf("datalen===[%s] \n",notpay_out->datalen);
printf("reason===[%s] \n",notpay_out->reason);
printf("sftype===[%s] \n",notpay_out->sftype);
printf("acttype;===[%s] \n",notpay_out->acttype);
printf("actno===[%s] \n",notpay_out->actno);
printf("name===[%s] \n",notpay_out->name);
printf("pswdcd===[%s] \n",notpay_out->pswdcd);
printf("passwd===[%s] \n",notpay_out->passwd);
printf("qtype;===[%s] \n",notpay_out->qtype);
printf("respday===[%s] \n",notpay_out->respday);
printf("qcontent===[%s] \n",notpay_out->qcontent);
printf("opackday===[%s] \n",notpay_out->opackday);
printf("opackid===[%s] \n",notpay_out->opackid);
printf("opkgno===[%s] \n",notpay_out->opkgno);
printf("otxtype===[%s] \n",notpay_out->otxtype);
printf("oorderno===[%s] \n",notpay_out->oorderno);
printf("avbal===[%s] \n",notpay_out->avbal);
printf("OK!print END!!! \n");
printf("[%s][%d] \n",__FILE__,__LINE__);
printf("+++++++++++++++++++++++++++++++++++++++++++");
return(0);
}******************************************************/
/************************add by liuyue  20051115******************************
*FUNCTION:update_source_table_record()                                      *
*ACTION  :做退回交易时候对ptdj,ssdj,dqdj等表中的stat的数据进行更新 
*         取消时将数据表中的状态恢复过来 
*DESP    :operation 1--取消交易    0--正交易
*****************************************************************************/
/*************未改
int update_source_table_record(NOTPAY_IN_AREA *wp_notpayin)
{
char	cCmtno[4];
char	cPkgno[4];
char	cTxnum[6];
int		iTxnum;
int     iPkgno;
int		iCmtno;
iCmtno = 0;
iPkgno = 0;
iTxnum = 0;
memset(cCmtno, 0, sizeof(cCmtno));
memset(cPkgno, 0, sizeof(cPkgno));
memset(cTxnum, 0, sizeof(cTxnum));
memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);
memcpy(cTxnum, wp_notpayin->otxnum, sizeof(cTxnum)-1);
iCmtno = atoi(cCmtno);
iPkgno = atoi(cPkgno);
iTxnum = atoi(cTxnum);
switch(iCmtno)
{
case 319:
*退回申请报文*
*由不同的业务类型来锁定不同的表*
*此业务只是对于往帐业务*
switch(iTxnum)
{
case 100:
*汇兑*
case 101:
*托收承付*
case 102:
*委托收款*
case 103:
*国库资金贷记划拨*
case 106:
*其他贷记业务*
case 20001:
*缴税*
case 20002: *非税收入*
case 20003: *财政直接支付退回* 
case 20004: *财政授权支付退回* 
case 20005: *国债兑付贷记划拨* 
case 30002:
*锁定普通贷记业务发送登记簿*
{
struct wd_lv_ptdjs_area		wd_lv_ptdjs;
memset(&wd_lv_ptdjs, 0, sizeof(wd_lv_ptdjs));
memcpy(wd_lv_ptdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ptdjs.orbrno)-1);
memcpy(wd_lv_ptdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ptdjs.wtday)-1);
memcpy(wd_lv_ptdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ptdjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_PTDJS(DBS_LOCK,&wd_lv_ptdjs))== DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ptdjs.stat[0] = STAT_LZBACK;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_ptdjs.stat[0] = STAT_WZSUCCESS;
}
if ((it_txcom.rtncd = DbsLV_PTDJS(DBS_UPDATE,&wd_lv_ptdjs))!= DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_ptdjs);
memcpy(it_totw.labtex.text,"锁定普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
DbsLV_PTDJSDebug(&wd_lv_ptdjs);
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_ptdjs);
memcpy(it_totw.labtex.text,"锁定普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
case 30001:
*通存业务*
*锁定实时贷记业务登记簿*
{
struct wd_lv_ssdjs_area		wd_lv_ssdjs;
memset(&wd_lv_ssdjs, 0, sizeof(wd_lv_ssdjs));
memcpy(wd_lv_ssdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ssdjs.orbrno)-1);
memcpy(wd_lv_ssdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ssdjs.wtday)-1);
memcpy(wd_lv_ssdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ssdjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_SSDJS(DBS_LOCK,&wd_lv_ssdjs))== DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ssdjs.stat[0] = STAT_LZBACK;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_ssdjs.stat[0] = STAT_WZSUCCESS;
}
if ((it_txcom.rtncd = DbsLV_SSDJS(DBS_UPDATE,&wd_lv_ssdjs))!= DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSDJSDebug(&wd_lv_ssdjs);
memcpy(it_totw.labtex.text,"锁定实时贷记发送登记簿错误!",40);
return it_txcom.rtncd;
} 
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSDJSDebug(&wd_lv_ssdjs);
memcpy(it_totw.labtex.text,"锁定实时贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
case 104:
*定期贷记业务*
*锁定定期贷记业务登记簿*
{
struct wd_lv_dqdjs_area		wd_lv_dqdjs;
memset(&wd_lv_dqdjs, 0, sizeof(wd_lv_dqdjs));
memcpy(wd_lv_dqdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_dqdjs.orbrno)-1);
memcpy(wd_lv_dqdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_dqdjs.wtday)-1);
memcpy(wd_lv_dqdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_dqdjs.orderno)-1);

if ((it_txcom.rtncd = DbsLV_DQDJS(DBS_LOCK,&wd_lv_dqdjs)) == DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_dqdjs.stat[0] = STAT_LZBACK;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_dqdjs.stat[0] = STAT_WZSUCCESS;
}
if ((it_txcom.rtncd = DbsLV_DQDJS(DBS_LOCK,&wd_lv_dqdjs)) == DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_DQDJSDebug(&wd_lv_dqdjs);
memcpy(it_totw.labtex.text,"锁定定期贷记发送登记簿错误!",40);
return it_txcom.rtncd;
} 
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_DQDJSDebug(&wd_lv_dqdjs);
memcpy(it_totw.labtex.text,"锁定定期贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
}
break;
case 321:
*撤销申请报文*
*付款清算行对已发送给CCPC的非实时贷记批量包或借记回执批量包，
可以发起撤销申请，且只能申请撤销整包业务，不能撤销单笔明细业务*
*ccpc未扎差的可以撤销，否则拒绝撤销*
switch(iTxnum)
{
case 100:
*汇兑*
case 101:
*托收承付*
case 102:
*委托收款*
case 103:
*国库资金贷记划拨*
case 106:
*其他贷记业务*
case 20001:
*缴税*
case 20002: *非税收入* 
case 20003: *财政直接支付退回* 
case 20004: *财政授权支付退回* 
case 20005: *国债兑付贷记划拨* 
case 30002:
*缴费*
*锁定普通贷记业务发送登记簿*
{
struct wd_lv_ptdjs_area		wd_lv_ptdjs;
memset(&wd_lv_ptdjs, 0, sizeof(wd_lv_ptdjs));
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
printf("[%s][%d]+++++执行到这里+++++\n",__FILE__,__LINE__);
memcpy(wd_lv_ptdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ptdjs.orbrno)-1);
memcpy(wd_lv_ptdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ptdjs.wtday)-1);
memcpy(wd_lv_ptdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ptdjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_PTDJS(DBS_LOCK,&wd_lv_ptdjs)) == DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ptdjs.stat[0] = STAT_WZCANCEL;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
}
if ((it_txcom.rtncd = DbsLV_PTDJS(DBS_UPDATE,&wd_lv_ptdjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_ptdjs);
memcpy(it_totw.labtex.text,"更新普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_ptdjs);
memcpy(it_totw.labtex.text,"锁定普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
case 104:
*定期贷记业务*
*锁定定期贷记业务登记簿*
{
struct wd_lv_dqdjs_area		wd_lv_dqdjs;
memset(&wd_lv_dqdjs, 0, sizeof(wd_lv_dqdjs));
memcpy(wd_lv_dqdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_dqdjs.orbrno)-1);
memcpy(wd_lv_dqdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_dqdjs.wtday)-1);
memcpy(wd_lv_dqdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_dqdjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_PTDJS(DBS_LOCK,&wd_lv_dqdjs)) == DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_dqdjs.stat[0] = STAT_WZCANCEL;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
}
if ((it_txcom.rtncd = DbsLV_DQDJS(DBS_UPDATE,&wd_lv_dqdjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_dqdjs);
memcpy(it_totw.labtex.text,"更新普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTDJSDebug(&wd_lv_dqdjs);
memcpy(it_totw.labtex.text,"锁定普通贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
}
break;
case 323:
*实时业务冲正申请报文*
*实时贷(借)记业务的发起方在没有收到接收行的回执而等待超时时，可发起实时业务冲正报文*
switch(iTxnum)
{
*实时贷记*
case 30001:
*通存业务*
*锁定实时贷记发送登记簿*
{
struct wd_lv_ssdjs_area		wd_lv_ssdjs;
memset(&wd_lv_ssdjs, 0, sizeof(wd_lv_ssdjs));
memcpy(wd_lv_ssdjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ssdjs.orbrno)-1);
memcpy(wd_lv_ssdjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ssdjs.wtday)-1);
memcpy(wd_lv_ssdjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ssdjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_SSDJS(DBS_LOCK,&wd_lv_ssdjs)) == DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ssdjs.stat[0]=STAT_WZCZ;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_ssdjs.stat[0] = STAT_WZSUCCESS;
}
if ((it_txcom.rtncd = DbsLV_SSDJS(DBS_LOCK,&wd_lv_ssdjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSDJSDebug(&wd_lv_ssdjs);
memcpy(it_totw.labtex.text,"更新实时贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSDJSDebug(&wd_lv_ssdjs);
memcpy(it_totw.labtex.text,"锁定实时贷记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
*实时借记*
case 20101:
*实时扣税*
case 30101:
*通兑业务*
*锁定实时借记发送登记簿*
{
struct wd_lv_ssjjs_area		wd_lv_ssjjs;
memset(&wd_lv_ssjjs, 0, sizeof(wd_lv_ssjjs));
memcpy(wd_lv_ssjjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ssjjs.orbrno)-1);
memcpy(wd_lv_ssjjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ssjjs.wtday)-1);
memcpy(wd_lv_ssjjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ssjjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_SSDJS(DBS_LOCK,&wd_lv_ssjjs)) != DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ssjjs.stat[0] = STAT_WZCZ;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_ssjjs.stat[0] = STAT_WZSUCCESS;
}
if ((it_txcom.rtncd=DbsLV_SSJJS(DBS_UPDATE,&wd_lv_ssjjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSJJSDebug(&wd_lv_ssjjs);
memcpy(it_totw.labtex.text,"更新实时借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_SSJJSDebug(&wd_lv_ssjjs);
memcpy(it_totw.labtex.text,"锁定实时借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
} 
break;
case 327:
*借记业务止付申请报文*
*非实时借记业务的发起方在没有收到接收行的回执前，
可发起止付申请报文，止付整包或其中一笔借记业务。*
switch(iTxnum)
{
*普通借记*
case 113:
*国库资金借记划拨*
case 119:
*其他借记业务*
case 20103:
*财政直接支付*
case 20104:
*财政授权支付*
case 30102:
*支票截留业务*
{
struct wd_lv_ptjjs_area		wd_lv_ptjjs;
memset(&wd_lv_ptjjs, 0, sizeof(wd_lv_ptjjs));
memcpy(wd_lv_ptjjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_ptjjs.orbrno)-1);
memcpy(wd_lv_ptjjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_ptjjs.wtday)-1);
memcpy(wd_lv_ptjjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_ptjjs.orderno)-1);
if ((it_txcom.rtncd = DbsLV_PTJJS(DBS_LOCK,&wd_lv_ptjjs))== DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_ptjjs.stat[0] = STAT_LZZF;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_ptjjs.stat[0]  = STAT_WZINQUEUE;
}
if ((it_txcom.rtncd=DbsLV_PTJJS(DBS_UPDATE,&wd_lv_ptjjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTJJSDebug(&wd_lv_ptjjs);
memcpy(it_totw.labtex.text,"更新普通借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_PTJJSDebug(&wd_lv_ptjjs);
memcpy(it_totw.labtex.text,"锁定普通借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
*定期借记*
case 114:
*定期借记业务*
case 20102:
*批量扣税*
{
struct wd_lv_dqjjs_area		wd_lv_dqjjs;
memset(&wd_lv_dqjjs, 0, sizeof(wd_lv_dqjjs));
memcpy(wd_lv_dqjjs.orbrno, wp_notpayin->orbrno, sizeof(wd_lv_dqjjs.orbrno)-1);
memcpy(wd_lv_dqjjs.wtday, wp_notpayin->owtday, sizeof(wd_lv_dqjjs.wtday)-1);
memcpy(wd_lv_dqjjs.orderno, wp_notpayin->oorderno, sizeof(wd_lv_dqjjs.acbrno)-1);
printf("[%s][%d]执行到这里来了! \n",__FILE__,__LINE__);
if ((it_txcom.rtncd = DbsLV_DQJJS(DBS_LOCK,&wd_lv_dqjjs)) == DB_OK)
{
if (TITA.hcode == TX_HCODE_NORMAL)
{
wd_lv_dqjjs.stat[0] = STAT_LZCZ;
}
else if (TITA.hcode != TX_HCODE_NORMAL)
{
wd_lv_dqjjs.stat[0] = STAT_WZINQUEUE;
}
if ((it_txcom.rtncd= DbsLV_DQJJS(DBS_UPDATE,&wd_lv_dqjjs)) != DB_OK)
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_DQJJSDebug(&wd_lv_dqjjs);
memcpy(it_totw.labtex.text,"更新定期借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
else
{
printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
DbsLV_DQJJSDebug(&wd_lv_dqjjs);
memcpy(it_totw.labtex.text,"锁定定期借记发送登记簿错误!",40);
return it_txcom.rtncd;
}
}
break;
}
break;

}
return (0);
}
**************/
/*********************************************/
/**   实时信息和通用信息生成包的函数		**/
/*********************************************/

int notpayin_to_wbctl(NOTPAY_IN_AREA *notpayin)
{
	int	ret;
	char	cBrno[6];
	struct	lv_wbctl_c	sLvWbctl;

	memset(cBrno, '\0', sizeof(cBrno)-1);
	memset(&sLvWbctl,'\0',sizeof(sLvWbctl));

	/* 交易机构 */
	get_fd_data("0030",cBrno);

	sLvWbctl.pack_date=apatoi(notpayin->opackday,8);
	memcpy(sLvWbctl.packid,  notpayin->opackid,  sizeof(sLvWbctl.packid)-1);
	memcpy(sLvWbctl.pkgno,   notpayin->pkgno,   sizeof(sLvWbctl.pkgno)-1);
	memcpy(sLvWbctl.pay_qs_no,   notpayin->payqsactno,   sizeof(sLvWbctl.pay_qs_no)-1);
	memcpy(sLvWbctl.cash_qs_no,  notpayin->cashqsactno,  sizeof(sLvWbctl.cash_qs_no)-1);
	memcpy(sLvWbctl.txnum, notpayin->txnum,sizeof(sLvWbctl.txnum)-1);
	sLvWbctl.in_date=apatoi(pcLvGetDay(),8);
	sLvWbctl.dtlcnt = 1;
	sLvWbctl.stat[0] = PSTAT_PACKED;	/** 已组包 **/
	sLvWbctl.ffflag[0] = '0';
	memcpy(sLvWbctl.br_no,   cBrno,   		sizeof(sLvWbctl.br_no)-1);
	memcpy(sLvWbctl.filler, notpayin->content, sizeof(sLvWbctl.filler)-1);

	ret = Lv_wbctl_Ins(sLvWbctl,g_pub_tx.reply);
	if (ret!=DB_OK)
	{
		vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
		Lv_wbctl_Debug(&sLvWbctl);
		sprintf(acErrMsg,"插入往包控制表错误!",40);
		return ret;
	}
	return (0);
}

/*********add by liuyue 20050117***********************
*******处理通用信息以及实时信息来帐时候不能对brno更新*
*******或者更新错误的问题*****************************/
int iLvGetRecvNOTPAYBrno(NOTPAY_IN_AREA *wp_notpay)
{
	int iRc=0;
	char cAcbrno[13];
	struct hv_brno_orno_c  sHvBrnoOrno;
	memset(&sHvBrnoOrno, 0, sizeof(sHvBrnoOrno));
	memset(cAcbrno, 0, sizeof(cAcbrno));
	memcpy(cAcbrno,wp_notpay->acbrno,sizeof(cAcbrno)-1);
	vtcp_log("======cAcbrno=[%s]=======",cAcbrno);

	/*  begin  Added by ChenMing   2006.09.25  */
	if(wp_notpay->acbrno[0] == '\0')
	{
			vtcp_log("%s,%d,未找到接受机构，清算中心接受\n",__FILE__,__LINE__);
			memcpy(wp_notpay->acbrno, LV_CZ_QSBRNO, sizeof(wp_notpay->acbrno) );
     	return(0);
	}
	/*  end   Added by ChenMing   2006.09.25  */
		
	iRc=Hv_brno_orno_Sel( g_pub_tx.reply, &sHvBrnoOrno, "or_br_no = '%s' ", cAcbrno );
	if ((iRc != 0 )&& (iRc != 100))
	{
		vtcp_log("[%s] [%d] 查询发起行行号错[%d]\n", __FL__,iRc);
		sprintf(acErrMsg,"数据库错误!");
		WRITEMSG
		return(-1);
	}
	else if(iRc == 100)
	{
		vtcp_log("[%s] [%d] 发起行行号不存在 [%d]\n", __FL__,iRc);
		memcpy(wp_notpay->acbrno, LV_CZ_QSBRNO, sizeof(wp_notpay->acbrno) );
		return(0);
	}
	else
	{
		if (strcmp(sHvBrnoOrno.br_no,LV_CZ_QSBRNO)==0||	sHvBrnoOrno.br_no[0]=='\0')
		{
			vtcp_log("%s,%d,清算中心接受\n",__FILE__,__LINE__);
			memcpy(sHvBrnoOrno.br_no,LV_CZ_QSBRNO,BRNO_LEN);
			/****本来机构就初始化为清算中心,所以不用做处理****/
		}
		else
		{
			/*****找到了某个支行,把机构号放到A_brno中****/
			set_zd_data("0030", sHvBrnoOrno.br_no);
			vtcp_log("%s,%d,sHvBrnoOrno.br_no=[%s]",__FILE__,__LINE__,sHvBrnoOrno.br_no);
			pcLvGetRecvno();
		}
		set_zd_data("0030", sHvBrnoOrno.br_no);
	}
	return(0);
}
/*******增加附加数据**************/
/*******1.3.7.	得到附加数据**************/


/*******增加附加数据**************/
/*******1.3.7.	得到附加数据**************/



