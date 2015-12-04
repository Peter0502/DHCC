/**************************************************
模块名称:批量处理程序
函数功能:普通贷记借记交易批量拼包发送
函数原型:
输入参数:
输出参数:
使用说明:
编 写 者:xyy 2006-10-11 15:31
**************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "string.h"
#include "stdlib.h"
#include "lv_pkgreg_c.h"
#include "hv_define.h"
#include "lv_wbctl_c.h"
#include "com_sys_parm_c.h"
#include "lv_define.h"
#include "lv_sysctl_c.h"
#include "time.h"
#include "timeb.h"
#include "public.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <varargs.h>

extern char * pcLvGetDay();
extern char * pub_base_daynumLAT(char *date, int num);
/**定义游标**/
static PAY_IN_AREA pay_in;
/**
static struct lv_wbctl_c lv_wbctl;
**/
static struct lv_pkgreg_c lv_pkgreg;
int bh_ErrProce(char * cashqsactno,char * pkgno,char * orderno,char * orbrno,char * packid,char * packday,char * msg,int line);

int iLvGetAutoPkgConfig(int *sleeptime ,int* cyctime,char *);
/**全局变量定义**/
#define TIME        3600   /** 程序循环停顿参数,单位秒  **/
#define CYCTIMES    5      /** 循环此数                 **/
#define HZDATE      2      /** 普通借记业务回执期限     **/

static  int     cnt;
static  int     tep;
static  int     iErrFlag=0;
static  int     iTime   =0;
static  int     iCyc    =0;

static  char    cHzdate [3];
static  char    cPackday[9];
static  char    cPackid [9];
static  char    cTlrno  [7];
static  char    cTxday  [9];
static  char    cQsbrno [BRNO_LEN+1]; 

static  char    STAT1      [2];
static  char    STAT2      [2];
static  char    STAT3      [2];
static  char    CASHQSACTNO[13];
static  char    PAYACTNO   [20];
static  char    CASHACTNO  [20];
static  char    WTDAY      [9];

static  char    cSqlbuf1[1024];
static  char    cSqlbuf5[1024];
static  char    cOpFlag [2];
static  char    cOldKinbr[BRNO_LEN+1];

/*******************************************************************
*                                                                  *
*   FUNCTIONS:      main                                           *
*                                                                  *
*  DESCRIPTION:    1.时间控制                                      *
*          2.批量处理未组包的普通贷记业务                          *
*          3.批量处理未组包的普通借记业务                          *
*******************************************************************/
looppkg()
{
  int ret = 0;
  iTime=0;
  iCyc=0;
  tep=0;
  cnt=0;
  iErrFlag=0;
  struct com_sys_parm_c com_sys_parm;
  struct lv_sysctl_c lv_sysctl;
  memset(cOldKinbr, 0 , sizeof(cOldKinbr));
  memset(cOpFlag  , 0 , sizeof(cOpFlag));
  memset(&com_sys_parm,0,sizeof(com_sys_parm));
  memset(&lv_sysctl   ,0,sizeof(lv_sysctl));
  ret=sql_begin();
  if ( ret )
  {
      sprintf( acErrMsg, "打开事务失败!!!" );
      WRITEMSG
      goto ErrExit;
  }
  /**------- 初始化全局变量 --------**/
  pub_base_sysinit();

  vtcp_log("[%s][%d] Begin of lv_batch",__FILE__,__LINE__);
  cOpFlag[0]='1';
  iTime=TIME;
  iCyc=CYCTIMES;/* 循环两次退出 */
  vtcp_log("[%s][%d] 默认循环次数=[%d],循环时间=[%d]",__FILE__,__LINE__,iCyc,iTime);
  set_zd_data(DC_TX_BR_NO,QS_BR_NO);

  if (iLvGetAutoPkgConfig(&iTime ,&iCyc,cOpFlag))
  {
      vtcp_log("[%s][%d]here 1",__FILE__,__LINE__);
      iTime=TIME;
      iCyc=CYCTIMES;/* 循环两次退出 */
      vtcp_log("[%s][%d] 得到配置的循环次数=[%d],循环时间=[%d]",__FILE__,__LINE__,iCyc,iTime);
  }
  vtcp_log("[%s][%d]here 11",__FILE__,__LINE__);
  /**
  sleep(10);
  **/
  memset(cTxday , 0 , sizeof(cTxday));
  memset(cQsbrno, 0 , sizeof(cQsbrno));
  memset(cTlrno , 0 , sizeof(cTlrno));
  vtcp_log("[%s][%d]here 2",__FILE__,__LINE__);
  get_fd_data("0030",cQsbrno);  /**交易机构**/
  vtcp_log("[%s][%d]cQsbrno=[%s]",__FILE__,__LINE__,cQsbrno);
  if (memcmp(cQsbrno,QS_BR_NO,BRNO_LEN)!=0 &&  memcmp(cQsbrno,"59999",BRNO_LEN)!=0)
  {
      sprintf( acErrMsg,"非清算中心不能做此业务[%s][%d][%s]",__FILE__,__LINE__,cQsbrno);
      set_zd_data(DC_GET_MSG,"非清算中心不能做此业务");
      strncpy(g_pub_tx.reply,"P047",4);
      WRITEMSG
      goto ErrExit;
  }
  memcpy(cTlrno,"0098",4);/**发送操作员暂500098**/
  vtcp_log("[%s][%d]cTlrno=[%s]",__FILE__,__LINE__,cTlrno);
  g_reply_int = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
  if (g_reply_int)
  {
      sprintf( acErrMsg,"得到系统时间错误[%s][%d]",__FILE__,__LINE__);
      set_zd_data(DC_GET_MSG,"得到系统时间错误");
      strncpy(g_pub_tx.reply,"S039",4);
      WRITEMSG
      goto ErrExit;
  }
  vtcp_log("[%s][%d]com_sys_parm.sys_date=[%ld]",__FILE__,__LINE__,com_sys_parm.sys_date);
  g_pub_tx.tx_date = com_sys_parm.sys_date;/**系统时间**/ 

  while (1)
  {
    /*add by xyy 2006-12-14 15:09 for 停运*/
    g_reply_int = Lv_sysctl_Sel(g_pub_tx.reply,&lv_sysctl,"1=1");
    if (g_reply_int)
    {
        sprintf( acErrMsg,"查询小额控制表错误[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"查询小额控制表错误");
        strncpy(g_pub_tx.reply,"S039",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log(" [%s][%d]当前小额系统状态为[%c]",__FILE__,__LINE__,lv_sysctl.stat[0]);
    if (lv_sysctl.stat[0]=='0')
    {
        vtcp_log(" [%s][%d]当前小额系统状态为停运[%c]",__FILE__,__LINE__,lv_sysctl.stat[0]);
        break;
    }
    tep++;
    vtcp_log("第[%d]次循环处理 [%s][%d]",tep, __FILE__,__LINE__);
    if (cOpFlag[0]!='1')
    {
        /***modify by xyy 2006-11-22 13:32 如果读出的配置文件为'0'，那么直接退出，不再等待
        vtcp_log("第[%d]次循环,标志为[%c]关闭[%s][%d]",tep,cOpFlag[0],__FILE__,__LINE__);
        sleep(iTime);
        if (iLvGetAutoPkgConfig(&iTime ,&iCyc,cOpFlag))
        {
        iTime=TIME;
        iCyc=CYCTIMES;
        }
        continue;
        *****/
        break;
    }
    /************************************************
    **     变量初始化                              **
    ************************************************/
    vtcp_log("[%s][%d] lv_batch_Initial begin",__FILE__,__LINE__);
    lv_batch_Initial();
    vtcp_log("[%s][%d]lv_batch_Initial over", __FILE__,__LINE__);
    /************************************************
    **    批量处理已组包的业务                     **
    ************************************************/

    vtcp_log("[%s][%d]pack_without_send begin", __FILE__,__LINE__);
    g_reply_int = pack_without_send();
    if (g_reply_int)
    {
        sprintf( acErrMsg,"pack_without_send[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]pack_without_send over", __FILE__,__LINE__);
    
    /************************************************
    **    批量处理未组包的普通借记的支票业         **
    ************************************************/
    vtcp_log(" 批量处理未组包的普通借记的支票业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_ptjjzphz(PKGNO_PTJJHZ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_ptjjzphz[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] lv_batch_ptjjzphz over", __FILE__,__LINE__);
    
    /************************************************
    **    批量处理未组包的普通贷记业务             **
    ************************************************/
    vtcp_log(" 批量处理未组包的普通贷记业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_ptdj(PKGNO_PTDJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_ptdj[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] lv_batch_ptdj over", __FILE__,__LINE__);
    /************************************************
    **    批量处理未组包的定期贷记业务             **
    ************************************************/
    vtcp_log(" 批量处理未组包的定期贷记业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_dqdj(PKGNO_DQDJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_dqdj[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] lv_batch_dqdj over", __FILE__,__LINE__);
    /************************************************
    **    批量处理未组包的贷记退汇业务             **
    ************************************************/
    vtcp_log(" 批量处理未组包的贷记退汇业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_ptdj(PKGNO_DJTH);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_ptdj[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] lv_batch_ptdj over", __FILE__,__LINE__);
    /************************************************
    **    批量处理未组包的普通借记业务             **
    ************************************************/
    vtcp_log("批量处理未组包的普通借记业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_ptjj(PKGNO_PTJJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_ptjj[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] lv_batch_ptjj over", __FILE__,__LINE__);
    /************************************************
    **    批量处理未组包的定期借记业务             **
    ************************************************/
    vtcp_log("批量处理未组包的定期借记业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_dqjj(PKGNO_DQJJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_dqjj[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]lv_batch_dqjj over", __FILE__,__LINE__);
    /************************************************
    **    批量处理未组包的普通借记回执业务             **
    ************************************************/
    vtcp_log("批量处理未组包的普通借记回执业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_jjhz(PKGNO_PTJJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_jjhz[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]lv_batch_jjhz over", __FILE__,__LINE__);

    /************************************************
    **    批量处理未组包的定期借记回执业务             **
    ************************************************/
    vtcp_log("批量处理未组包的定期借记回执业务[%s][%d]", __FILE__,__LINE__);
    g_reply_int = lv_batch_jjhz(PKGNO_DQJJ);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"lv_batch_jjhz[%s][%d]",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"L192",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]lv_batch_jjhz over", __FILE__,__LINE__);
    /************************************************ 
    **    时间控制,参数为秒   用宏定义            ** 
    ************************************************/
    vtcp_log("时间控制,时间间隔[%d]秒[%s][%d]",iTime, __FILE__,__LINE__);
    sleep(iTime);
    if (iLvGetAutoPkgConfig(&iTime ,&iCyc,cOpFlag))
    {
        iTime=TIME;
        iCyc=CYCTIMES;/* 循环两次退出 */
    }
    vtcp_log("[%s][%d] tep===[%d] iCyc=[%d]",__FILE__,__LINE__,tep,iCyc);
    if (tep>=iCyc)
    {
        break;   /** 此处测试用,循环2次跳出 **/
    }     
    if (cOpFlag[0]!='1')
    {
        break;
    }
  }
  OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"小额自动拼包成功!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
  ErrExit:
    sql_rollback(); /*--事务回滚--*/
    sprintf(acErrMsg,"小额自动拼包失败!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
/********************************************************
*   FUNCTION:           lv_batch_Initial()              *
*   DESCRIPTION:        1.变量初始化                    *
********************************************************/
int lv_batch_Initial()
{
  memset(&pay_in,'\0',sizeof(pay_in));
  vtcp_log("[%s][%d]",__FILE__,__LINE__);  
  memset(&lv_pkgreg,'\0',sizeof(lv_pkgreg));
  
  memset(STAT1,'\0',sizeof(STAT1));
  memset(STAT2,'\0',sizeof(STAT2));
  memset(STAT3,'\0',sizeof(STAT3));
  memset(WTDAY,'\0',sizeof(WTDAY));
  memset(CASHQSACTNO,'\0',sizeof(CASHQSACTNO));
  
  memset(cPackday,'\0',sizeof(cPackday));
  memset(cPackid ,'\0',sizeof(cPackid));
  memset(cHzdate ,'\0',sizeof(cHzdate));
  
  memcpy(WTDAY,pcLvGetDay() ,8);
  memcpy(cPackday,pcLvGetDay(),8);
  vtcp_log("[%s][%d]WTDAY=[%s]",__FILE__,__LINE__,WTDAY);
  vtcp_log("[%s][%d]cPackday=[%s]",__FILE__,__LINE__,cPackday);
  
  STAT1[0]=STAT_WZCHECK2;
  STAT2[0]=STAT_WZLR;
  STAT3[0]=PSTAT_PACKED;
  return(0);
}
/****************************************************
*  FUNCTIONS:      lv_batch_ptdj                    *
*  DESCRIPTIPN:                                     *
****************************************************/
int lv_batch_ptdj(char * pkgno)
{
  char cPkgno[4];
  int iRc=0;
  iRc=0;
  vtcp_log("[%s][%d]Begin of lv_batch_ptdj begin",__FILE__,__LINE__);

  memset(cPkgno, 0 , sizeof(cPkgno));
  memset(cSqlbuf1 , 0 , sizeof(cSqlbuf1));
  memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);

  distinct_dec_sel( "lv_pkgreg", "cash_qs_no", " lv_sts='%c' and pkgno='%s' and lw_ind='1' and packid is null", STAT1[0],cPkgno);

  while (1)
  {
    memset(CASHQSACTNO, 0 , sizeof(CASHQSACTNO));
    g_reply_int = distinct_fet_sel(CASHQSACTNO);
    if (g_reply_int==100) break;
    if (g_reply_int!=0)
    {
        vtcp_log("[%s][%d] error fetch ptdj1 sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
        break;
    }
    cnt=0;
    vtcp_log("[%s][%d]=====CASHQSACTNO=[%s]",__FILE__,__LINE__,CASHQSACTNO);

    cnt = sql_count("lv_pkgreg","lv_sts='%c' and cash_qs_no='%s' and lw_ind='1' and pkgno='%s' and packid is null",STAT1[0],CASHQSACTNO,cPkgno);

    vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt);
    vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
    if (cnt!=0)
    {
      iRc = lv_batchptdj_package(cPkgno);

      if (iRc!=0)
      {
          bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"拼包错误",__LINE__);
          vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
      }
      else 
      {
          vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
          iRc = lv_batchdj_send(cPkgno);
      }
      if (iRc!=0)
      {
          bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"发包错误",__LINE__);
      }
    }
  }
  distinct_clo_sel();
  vtcp_log("[%s][%d]", __FILE__,__LINE__);
  return(0);
}
/********************************************************
*  FUNCTION:      lv_batch_package()                      *
*  DESCRIPTION:    1.组包                                *
*                  2.封包                                *
********************************************************/
int lv_batchptdj_package(char *pkgno)
{
  int iRc;
  int recid=0;
  char cPkgday[9];
  char cPkgno [4];
  char cTmpday[9];
  char cOldBrno[BRNO_LEN+1];
  char wherelist[1024];  /**查询条件**/
  struct lv_pkgreg_c lv_pkgreg;
  struct lv_wbctl_c lv_wbctl;
  memset(cOldBrno , 0 , sizeof(cOldBrno));
  memset(cPackid  , 0 , sizeof(cPackid));
  memset(cPackday , 0 , sizeof(cPackday));
  memset(cTmpday  , 0 , sizeof(cTmpday));
  memset(cPkgno   , 0 , sizeof(cPkgno));
  memset(wherelist, 0 , sizeof(wherelist));
  memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
  memset(&lv_pkgreg, 0 , sizeof(lv_pkgreg));

  memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);
  vtcp_log("[%s][%d]cPkgno==[%s]",__FILE__,__LINE__,cPkgno);

  /* 保存原理交易机构号 */
  get_zd_data("0030",cOldBrno);
  if (memcmp(cPkgno,PKGNO_PTDJ,sizeof(cPkgno)-1)==0 || memcmp(cPkgno,PKGNO_DJTH,sizeof(cPkgno)-1)==0)/**普通贷记或贷记退汇**/
  {
      sprintf(wherelist,"cash_qs_no='%s' and lv_sts='%c' and lw_ind='1' and packid is null and pkgno='%s'",CASHQSACTNO,STAT1[0],cPkgno);
  }
  else if (memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0)/*定期贷记*/
  {
      sprintf(wherelist,"cash_qs_no='%s' and lv_sts='%c' and lw_ind='1' and packid is null and pkgno='%s' and pay_ac_no='%s'",CASHQSACTNO,STAT1[0],cPkgno,PAYACTNO);
  }
  g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, wherelist);
  if (g_reply_int!=DB_OK && g_reply_int!=100)
  {
      vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
      sprintf( acErrMsg,"数据库错误[%s][%d]",__FILE__,__LINE__);
      bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
      return(g_reply_int);
  }
  vtcp_log("[%s][%d]", __FILE__,__LINE__);
  if (g_reply_int==100)
  {
      vtcp_log("[%s][%d],没有可拼包的记录",__FILE__,__LINE__);
      sprintf( acErrMsg,"没有可拼包的记录[%s][%d]",__FILE__,__LINE__);
      bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
      return(g_reply_int);
  }
  while (1)
  {
    memset(&pay_in,'\0',sizeof(pay_in));
    vtcp_log("同一接收行第[%d]次循环子处理 [%s][%d]",recid, __FILE__,__LINE__);
    g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
    recid++;
    vtcp_log("同一接收行第[%d]次循环子处理 [%s][%d]",recid, __FILE__,__LINE__);

    if (g_reply_int!=DB_OK && g_reply_int!=100)
    {
        vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"访问数据库错误");
        WRITEMSG
        return (g_reply_int);
    }
    if (g_reply_int==100)
    {
        if (recid==1)
        {
          vtcp_log("[%s][%d],没有可拼包的记录sqlcode=100",__FILE__,__LINE__);
          sprintf( acErrMsg,"没有可拼包的记录[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
          return(g_reply_int);
        }
        vtcp_log("[%s][%d],记录循环处理完毕",__FILE__,__LINE__);
        break;
    }
    set_zd_data("0020",lv_pkgreg.br_no);/**开户机构**/
    set_zd_data("0030",lv_pkgreg.br_no);/**交易机构**/

    vtcp_log("[%s][%d]iLvPayinAutoInfo begin", __FILE__,__LINE__);
    pay_in.F_crdb[0]='1';
    iLvPayinAutoInfo(&pay_in);
    vtcp_log("[%s][%d]iLvPayinAutoInfo over", __FILE__,__LINE__);

    memcpy(pay_in.F_opcd,OPCD_LR,sizeof(pay_in.F_opcd));
    vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD over!", __FILE__,__LINE__);
    LV_CP_PAY_IN_FIELD(&pay_in,(&lv_pkgreg));
    vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD over!", __FILE__,__LINE__);
    iLvResetPayin(&pay_in);
    vtcp_log("[%s][%d]iLvResetPayin over!", __FILE__,__LINE__);
    vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,pay_in.F_pkgno);
    /****正交易****/
    if (pay_in.T_stat[0]!=STAT_WZCHECK2)
    {
        vtcp_log("[%s][%d][%.8s]支付交易状态[%c]不符",__FILE__,__LINE__,pay_in.F_orderno,pay_in.T_stat[0]);
        sprintf(acErrMsg,"支付交易状态不符[%s][%d]",__FILE__,__LINE__);
        memcpy(cTmpday,pay_in.T_wtday,sizeof(cTmpday)-1);
        bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,(char *)NULL,cTmpday,acErrMsg,__LINE__);
        return(-1);
    }
    vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,pay_in.F_pkgno);
    /****产生一个新包   如果不是第一条记录,将packid赋值****/
    if (recid!=1)
    {
        memcpy(pay_in.F_packid,cPackid,sizeof(pay_in.F_packid));
        memcpy(pay_in.F_packday,cPackday,sizeof(pay_in.F_packday));
        vtcp_log("[%s][%d]=================cPackid=[%s]",__FILE__,__LINE__,cPackid);
        vtcp_log("[%s][%d]======pay_in.F_packday=[%s]",__FILE__,__LINE__,pay_in.F_packday);
    }
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    /****产生新包的第一条记录,设置新包内容,以后就不再设置了***/
    /* 这里好像有问题，好像是这样每条明细都会产生一个包号 */
    get_zd_data("0030",cOldKinbr);
    set_zd_data("0030",cQsbrno);
    vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,pay_in.F_pkgno);
    iRc=iLvPayinCheck8(&pay_in,&lv_wbctl,NULL);
    vtcp_log("[%s][%d] very CDlv_wbctl.packday =[%ld] ",__FL__,lv_wbctl.pack_date);

    set_zd_data("0030",cOldKinbr);
    vtcp_log("[%s][%d]cOldKinbr =[%s]", __FILE__,__LINE__,cOldKinbr);
    if (iRc!=PCHK_NEW && iRc!=PCHK_UPDATE)
    {
        vtcp_log("[%s][%d],新生成包时[%d]出错",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"更新支付交易登记簿错[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
        return(iRc);
    }
    vtcp_log("[%s][%d] packid==[%.8s]=[%.8s]",__FILE__,__LINE__,pay_in.F_packid,lv_wbctl.packid);
    vtcp_log("[%s][%d] very CD  lv_wbctl.pkgno==[%.3s]",__FILE__,__LINE__,lv_wbctl.pkgno);
    vtcp_log("[%s][%d] very CDlv_wbctl.packid xyy ===[%s] ",__FL__,lv_wbctl.packid);
    vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
    /**  将新生成的包序号传出   **/
    if (iRc==PCHK_UPDATE)
    {
        memcpy(pay_in.F_packid,lv_wbctl.packid,sizeof(pay_in.F_packid));
        memcpy(pay_in.T_packid,lv_wbctl.packid,sizeof(pay_in.T_packid));
    }
    memcpy(cPackid,pay_in.F_packid,sizeof(pay_in.F_packid));
    memcpy(cPackday,pay_in.F_packday,sizeof(pay_in.F_packday));
    vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
    vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
    LV_CP_PAY_IN_TABLE(&pay_in,(&lv_pkgreg));
    vtcp_log("[%s][%d]LV_CP_PAY_IN_TABLE over", __FILE__,__LINE__,LV_CP_PAY_IN_TABLE);

    memcpy(pay_in.F_packday, pcLvGetDay(), 8);
    memcpy(pay_in.F_packid, lv_wbctl.packid,8);

    vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
    memcpy(lv_pkgreg.packid,cPackid,sizeof(cPackday)-1);
    lv_pkgreg.pack_date = apatoi(cPackday,8);
    memcpy(lv_pkgreg.send_tel,cTlrno,sizeof(cTlrno)-1);
    vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
    vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
    g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);

    if (g_reply_int!=DB_OK && g_reply_int!=100)
    {
        vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
        return(iRc);
    }
    if (g_reply_int==100)
    {
        vtcp_log("[%s][%d]pkgno[%s]", __FILE__,__LINE__,cPkgno);
        sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d] lv_pkgreg.pkgno           =[%s]",__FL__,lv_pkgreg.pkgno         );
    vtcp_log("[%s][%d] lv_pkgreg.orderno         =[%s]",__FL__,lv_pkgreg.orderno       );
    vtcp_log("[%s][%d] lv_pkgreg.txnum           =[%s]",__FL__,lv_pkgreg.txnum         );
    vtcp_log("[%s][%d] lv_pkgreg.or_br_no        =[%s]",__FL__,lv_pkgreg.or_br_no      );
    vtcp_log("[%s][%d] lv_pkgreg.pay_qs_no       =[%s]",__FL__,lv_pkgreg.pay_qs_no     );
    vtcp_log("[%s][%d] lv_pkgreg.sendco          =[%s]",__FL__,lv_pkgreg.sendco        );
    vtcp_log("[%s][%d] lv_pkgreg.ac_br_no        =[%s]",__FL__,lv_pkgreg.ac_br_no      );
    vtcp_log("[%s][%d] lv_pkgreg.cash_qs_no      =[%s]",__FL__,lv_pkgreg.cash_qs_no    );
    vtcp_log("[%s][%d] lv_pkgreg.receco          =[%s]",__FL__,lv_pkgreg.receco        );
    vtcp_log("[%s][%d] lv_pkgreg.pay_opn_br_no   =[%s]",__FL__,lv_pkgreg.pay_opn_br_no );
    vtcp_log("[%s][%d] lv_pkgreg.pay_ac_no       =[%s]",__FL__,lv_pkgreg.pay_ac_no     );
    vtcp_log("[%s][%d] lv_pkgreg.pay_name        =[%s]",__FL__,lv_pkgreg.pay_name      );
    vtcp_log("[%s][%d] lv_pkgreg.pay_addr        =[%s]",__FL__,lv_pkgreg.pay_addr      );
    vtcp_log("[%s][%d] lv_pkgreg.cash_opn_br_no  =[%s]",__FL__,lv_pkgreg.cash_opn_br_no);
    vtcp_log("[%s][%d] lv_pkgreg.cash_ac_no      =[%s]",__FL__,lv_pkgreg.cash_ac_no    );
    vtcp_log("[%s][%d] lv_pkgreg.cash_name       =[%s]",__FL__,lv_pkgreg.cash_name     );
    vtcp_log("[%s][%d] lv_pkgreg.cash_addr       =[%s]",__FL__,lv_pkgreg.cash_addr     );
    vtcp_log("[%s][%d] lv_pkgreg.lv_yw_ind       =[%s]",__FL__,lv_pkgreg.lv_yw_ind     );
    vtcp_log("[%s][%d] lv_pkgreg.lv_brf          =[%s]",__FL__,lv_pkgreg.lv_brf        );
    vtcp_log("[%s][%d] lv_pkgreg.rcpstat         =[%s]",__FL__,lv_pkgreg.rcpstat       );
    vtcp_log("[%s][%d] lv_pkgreg.resp1           =[%s]",__FL__,lv_pkgreg.resp1         );
    vtcp_log("[%s][%d] lv_pkgreg.resp2           =[%s]",__FL__,lv_pkgreg.resp2         );
    vtcp_log("[%s][%d] lv_pkgreg.note_type       =[%s]",__FL__,lv_pkgreg.note_type     );
    vtcp_log("[%s][%d] lv_pkgreg.note_no         =[%s]",__FL__,lv_pkgreg.note_no       );
    vtcp_log("[%s][%d] lv_pkgreg.addid           =[%s]",__FL__,lv_pkgreg.addid         );
    vtcp_log("[%s][%d] lv_pkgreg.br_no           =[%s]",__FL__,lv_pkgreg.br_no         );
    vtcp_log("[%s][%d] lv_pkgreg.input_tel       =[%s]",__FL__,lv_pkgreg.input_tel     );
    vtcp_log("[%s][%d] lv_pkgreg.check_tel       =[%s]",__FL__,lv_pkgreg.check_tel     );
    vtcp_log("[%s][%d] lv_pkgreg.auth_tel        =[%s]",__FL__,lv_pkgreg.auth_tel      );
    vtcp_log("[%s][%d] lv_pkgreg.send_tel        =[%s]",__FL__,lv_pkgreg.send_tel      );
    vtcp_log("[%s][%d] lv_pkgreg.resp_tel        =[%s]",__FL__,lv_pkgreg.resp_tel      );
    vtcp_log("[%s][%d] lv_pkgreg.deal_tel        =[%s]",__FL__,lv_pkgreg.deal_tel      );
    vtcp_log("[%s][%d] lv_pkgreg.input_trace_no  =[%s]",__FL__,lv_pkgreg.input_trace_no);
    vtcp_log("[%s][%d] lv_pkgreg.check_trace_no  =[%s]",__FL__,lv_pkgreg.check_trace_no);
    vtcp_log("[%s][%d] lv_pkgreg.send_trace_no   =[%s]",__FL__,lv_pkgreg.send_trace_no );
    vtcp_log("[%s][%d] lv_pkgreg.resp_trace_no   =[%s]",__FL__,lv_pkgreg.resp_trace_no );
    vtcp_log("[%s][%d] lv_pkgreg.deal_trace_no   =[%s]",__FL__,lv_pkgreg.deal_trace_no );
    vtcp_log("[%s][%d] lv_pkgreg.lv_sts          =[%c]",__FL__,lv_pkgreg.lv_sts[0]     );
    vtcp_log("[%s][%d] lv_pkgreg.beg_sts         =[%c]",__FL__,lv_pkgreg.beg_sts[0]    );
    vtcp_log("[%s][%d] lv_pkgreg.operlevel       =[%c]",__FL__,lv_pkgreg.operlevel[0]  );
    vtcp_log("[%s][%d] lv_pkgreg.tx_chrg_ind     =[%c]",__FL__,lv_pkgreg.tx_chrg_ind[0]);
    vtcp_log("[%s][%d] lv_pkgreg.checkflag       =[%c]",__FL__,lv_pkgreg.checkflag[0]  );
    vtcp_log("[%s][%d] lv_pkgreg.brprtflag       =[%s]",__FL__,lv_pkgreg.brprtflag     );
    vtcp_log("[%s][%d] lv_pkgreg.qsprtflag       =[%s]",__FL__,lv_pkgreg.qsprtflag     );
    vtcp_log("[%s][%d] lv_pkgreg.packid          =[%s]",__FL__,lv_pkgreg.packid        );
    vtcp_log("[%s][%d] lv_pkgreg.retcode         =[%s]",__FL__,lv_pkgreg.retcode       );
    vtcp_log("[%s][%d] lv_pkgreg.o_txnum         =[%s]",__FL__,lv_pkgreg.o_txnum       );
    vtcp_log("[%s][%d] lv_pkgreg.ywdtlid         =[%s]",__FL__,lv_pkgreg.ywdtlid       );
    vtcp_log("[%s][%d] lv_pkgreg.protno          =[%s]",__FL__,lv_pkgreg.protno        );
    vtcp_log("[%s][%d] lv_pkgreg.ac_type         =[%c]",__FL__,lv_pkgreg.ac_type[0]    );
    vtcp_log("[%s][%d] lv_pkgreg.refno           =[%s]",__FL__,lv_pkgreg.refno         );
    vtcp_log("[%s][%d] lv_pkgreg.o_or_br_no      =[%s]",__FL__,lv_pkgreg.o_or_br_no    );
    vtcp_log("[%s][%d] lv_pkgreg.o_orderno       =[%s]",__FL__,lv_pkgreg.o_orderno     );
    vtcp_log("[%s][%d] lv_pkgreg.o_packid        =[%s]",__FL__,lv_pkgreg.o_packid      );
    vtcp_log("[%s][%d] lv_pkgreg.o_ac_br_no      =[%s]",__FL__,lv_pkgreg.o_ac_br_no    );
    vtcp_log("[%s][%d] lv_pkgreg.lv_o_sts        =[%c]",__FL__,lv_pkgreg.lv_o_sts[0]   );
    vtcp_log("[%s][%d] lv_pkgreg.lw_ind          =[%c]",__FL__,lv_pkgreg.lw_ind[0]     );
    vtcp_log("[%s][%d] lv_pkgreg.cur_no          =[%s]",__FL__,lv_pkgreg.cur_no        );
    vtcp_log("[%s][%d] lv_pkgreg.filler          =[%s]",__FL__,lv_pkgreg.filler        );                                                                                           
    vtcp_log("[%s][%d] lv_pkgreg.tx_time         =[%ld]",__FL__,lv_pkgreg.tx_time       );
    vtcp_log("[%s][%d] lv_pkgreg.jz_date         =[%ld]",__FL__,lv_pkgreg.jz_date       );
    vtcp_log("[%s][%d] lv_pkgreg.in_date         =[%ld]",__FL__,lv_pkgreg.in_date       );
    vtcp_log("[%s][%d] lv_pkgreg.wt_date         =[%ld]",__FL__,lv_pkgreg.wt_date       );
    vtcp_log("[%s][%d] lv_pkgreg.pack_date       =[%ld]",__FL__,lv_pkgreg.pack_date     );
    vtcp_log("[%s][%d] lv_pkgreg.resp_date       =[%ld]",__FL__,lv_pkgreg.resp_date     );
    vtcp_log("[%s][%d] lv_pkgreg.o_wt_date       =[%ld]",__FL__,lv_pkgreg.o_wt_date     );
    vtcp_log("[%s][%d] lv_pkgreg.o_pack_date     =[%ld]",__FL__,lv_pkgreg.o_pack_date   );
    vtcp_log("[%s][%d] lv_pkgreg.df_chrg_amt     =[%f]",__FL__,lv_pkgreg.df_chrg_amt   );
    vtcp_log("[%s][%d] lv_pkgreg.chrg_amt        =[%f]",__FL__,lv_pkgreg.chrg_amt      );
    vtcp_log("[%s][%d] lv_pkgreg.tx_amt          =[%f]",__FL__,lv_pkgreg.tx_amt        );
    vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
    vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
    vtcp_log("同一接收行第[%d]次循环子处理结束 [%s][%d]",recid,__FILE__,__LINE__);
  }
  Lv_pkgreg_Clo_Upd();
  vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);

  /****把包记录封闭******/
  g_reply_int = sql_execute("update lv_wbctl set stat='%c' where packid='%s' and pack_date='%s'",STAT3[0],cPackid,cPackday);
  if (g_reply_int)
  {
      vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
      sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
      bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
      return(g_reply_int);
  }
  vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
  sql_commit();/*提交事务*/
  /* 恢复交易机构 */
  set_zd_data("0030",cOldBrno);
  vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
  vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
  return 0;
}
/********************************************************
*                                                        *
*  FUNCTION:      lv_batchdj_send()                        *
*                                                        *
*  DESCRIPTION:    发送普通贷记包                        *
********************************************************/
int lv_batchdj_send(char * pkgno)
{
  int ret=0;
  char cPkgno[4];
  memset(cPkgno, 0 , sizeof(cPkgno));

  vtcp_log("[%s][%d]cPackid=[%s]",__FILE__,__LINE__,cPackid);
  vtcp_log("[%s][%d]cPackday=[%s]",__FILE__,__LINE__,cPackday);
  memcpy(cPkgno,pkgno,sizeof(cPkgno));
  if (memcmp(cPkgno,PKGNO_PTDJ,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_ptdj_tag(cPackday,cPackid);  /** PKG001普通贷记业务包 **/
      vtcp_log("[%s][%d]-----发送普通贷记包包,ret=[%d]\n",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]发送普通贷记包[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  if (memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_dqdj_tag_ssh(cPackday,cPackid);  /** PKG005定期贷记业务包 **/
      vtcp_log("[%s][%d]-----发送定期贷记业务包,ret=[%d]",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]发送定期贷记包[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  if (memcmp(cPkgno,PKGNO_DJTH,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_djth_tag(cPackday,cPackid);  /**  PKG007贷记退汇业务包 **/
      vtcp_log("[%s][%d]-----发送贷记退汇业务包,ret=[%d]",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]发送贷记退汇包[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  if (ret!=0)
  {
      sprintf( acErrMsg,"[%s][%d]发送贷记类业务失败packday=[%s]packid=[%s]ret==[%d]\n",__FILE__,__LINE__,cPackday,cPackid,ret);
      bh_ErrProce(CASHQSACTNO,cPkgno,(char*)NULL,(char*)NULL,cPackid,cPackday,acErrMsg,__LINE__);
      return ret;
  }
  sql_commit();/*提交事务*/
  vtcp_log("[%s][%d]==cPackday=[%s]",__FILE__,__LINE__,cPackday);
  vtcp_log("[%s][%d]该包为packid=[%s]",__FILE__,__LINE__,cPackid);
  return 0;
}
/********************************************************
*                                                       *
*  FUNCTION:      lv_batchptjj_send()                     *
*                                                        *
*  DESCRIPTION:    发送普通借记包                        *
********************************************************/
int lv_batchptjj_send(char * pkgno)
{
  int ret=0;
  char cPkgno[4];
  struct lv_pkgreg_c lv_pkgreg;
  
  memset(cPkgno, 0 , sizeof(cPkgno));
  memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);
  memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
  vtcp_log("[%s][%d]===cPackid=[%s]",__FILE__,__LINE__,cPackid);
  vtcp_log("[%s][%d]===cPackday=[%s]",__FILE__,__LINE__,cPackday);
    
  if (memcmp(cPkgno,PKGNO_PTJJ,sizeof(cPkgno)-1)==0)
  {
      memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
      ret = lv_get_ptjj_tag(cPackday,cPackid);  /** PKG002普通借记业务包 **/
      sprintf( acErrMsg,"[%s][%d]拼普通借记发送[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  else if (memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_dqjj_tag(cPackday,cPackid);  /** PKG006定期借记业务包 **/
      vtcp_log("[%s][%d]-----拼定期借记发送包,ret=[%d]",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]拼定期借记业务发送[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  else if (memcmp(cPkgno,PKGNO_PTJJHZ,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_ptjjhz_tag(cPackday,cPackid);  /** PKG008普通借记业务回执包 **/
      vtcp_log("[%s][%d]-----拼普通借记回执发送包,ret=[%d]",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]拼普通借记回执发送[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  else if (memcmp(cPkgno,PKGNO_DQJJHZ,sizeof(cPkgno)-1)==0)
  {
      ret = lv_get_dqjjhz_tag(cPackday,cPackid);  /** PKG011定期借记业务回执包 **/
      vtcp_log("[%s][%d]-----拼定期借记回执发送包,ret=[%d]",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"[%s][%d]拼定期借记回执发送[%s]成功\n",__FILE__,__LINE__,cPackid);
  }
  if (ret!=0)
  {
      vtcp_log("%s,%d send error ret=%d\n",__FILE__,__LINE__,ret);
      sprintf( acErrMsg,"发送借记类业务失败[%s][%d]packday=[%s]packid=[%s]ret=[%d]\n",__FILE__,__LINE__,cPackday,cPackid,ret);
      bh_ErrProce(CASHQSACTNO,cPkgno,(char*)NULL,(char*)NULL,cPackid,cPackday,acErrMsg,__LINE__);
      return ret;
  }
  vtcp_log("[%s][%d]-----拼普通借记发送包,ret=[%d]",__FILE__,__LINE__,ret);
  sql_commit();/*提交事务*/
  vtcp_log("[%s][%d]==cPackday=[%s]",__FILE__,__LINE__,cPackday);
  return 0;
}

int iLvGetAutoPkgConfig(int *sleeptime ,int* cyctime,char *opflg)
{
  FILE * fpr;
  char Filename[80];
  char cTmpvalue[50];
  char cValues[50];
  char *p;
  int iSleepTime=0;
  int iCycTime=0;
  char cOpenFlag[10];
  struct com_sys_parm_c com_sys_parm;
  vtcp_log("[%s][%d]Config  1",__FILE__,__LINE__);
  memset(cOpenFlag, 0 , sizeof(cOpenFlag));
  memset(cValues  , 0 , sizeof(cValues));
  memset(cTmpvalue, 0 , sizeof(cTmpvalue));
  memset(Filename , 0 , sizeof(Filename));
  sprintf(Filename, "%s/etc/autopkg.cfg", getenv("HOME"));
  vtcp_log("[%s][%d]Filename=[%s]",__FILE__,__LINE__,Filename);
  iSleepTime=0;
  iCycTime=0;
  g_reply_int = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
  if (g_reply_int)
  {
      sprintf( acErrMsg,"得到系统时间错误[%s][%d]",__FILE__,__LINE__);
      set_zd_data(DC_GET_MSG,"得到系统时间错误");
      WRITEMSG
      return (g_reply_int);
  }
  g_pub_tx.tx_date = com_sys_parm.sys_date;/**系统时间**/ 
  vtcp_log("[%s][%d]g_pub_tx.tx_date=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_date);
  fpr = fopen(Filename, "a+");
  if (fpr==NULL)
  {
      vtcp_log("[%s][%d] open [%s] error",__FILE__,__LINE__,Filename);
      sleep(10);
      return -1;
  }
  fgets(cTmpvalue,sizeof(cTmpvalue)-1,fpr);

  p=strtok(cTmpvalue,"|");
  memcpy(cValues,p,sizeof(cValues)-1);
  iSleepTime=atoi(cValues);
  *sleeptime=iSleepTime;

  p=strtok(NULL,"|"); 
  memcpy(cValues,p,sizeof(cValues)-1);
  iCycTime=atoi(cValues);

  p=strtok(NULL,"|"); 
  memcpy(cOpenFlag,p,sizeof(cOpenFlag)-1);
  /****Modified by SSH,2006.5.23****/
  if(*sleeptime<=0)
  {
    int sp1,sp2,sp3,sp4,spwork;
  #ifdef __JINZHONG__
    /****如果配置文件中的间隔秒数<=0,则按晋中规定时间拼包****/
    sp1=iCountTimeSpace("103000");
    sp2=iCountTimeSpace("140000");
    sp3=iCountTimeSpace("154000");
    sp4=iCountTimeSpace("173000");
    if (sp1<=0)sp1=3600*24;  
    if (sp2<=0)sp1=3600*24;  
    if (sp3<=0)sp1=3600*24;  
    if (sp4<=0)sp1=3600*24;  
    spwork=sp1;
    if (sp2<spwork)spwork=sp2;
    if (sp3<spwork)spwork=sp3;
    if (sp4<spwork)spwork=sp4;
    *sleeptime=spwork;  
  #else
    /****如果配置文件中的间隔秒数<=0,则按扎差场次提前20分钟拼包****/
    sp1=iCountTimeSpace("102000");
    sp2=iCountTimeSpace("135000");
    sp3=iCountTimeSpace("152000");
    if (sp1<=0)sp1=3600*24;  
    if (sp2<=0)sp1=3600*24;  
    if (sp3<=0)sp1=3600*24;  
    spwork=sp1;
    if (sp2<spwork)spwork=sp2;
    if (sp3<spwork)spwork=sp3;
    *sleeptime=spwork;  
  #endif
  }  
  *cyctime=iCycTime;
  opflg[0]=cOpenFlag[0];
  vtcp_log("[%s][%d] 循环次数==[%d],循环时间=[%d],自动拼包开关标志[%c]0关，1开",__FILE__,__LINE__,iCycTime,iSleepTime,cOpenFlag[0]);
  return 0;
}
/****Add by SSH,2006.5.23,计算某个时间点和当前时间相差的秒数***/
int iCountTimeSpace(char *pcNextTimeStr)
{
  struct  tm stTime;
  struct  timeb stTimeB_Now;
  struct  timeb stTimeB_Next;
  char    pcTimeStr[128];
  memset((char *)&stTimeB_Now,'\0',sizeof(struct  timeb));
  memset((char *)&stTimeB_Next,'\0',sizeof(struct  timeb));
  memset((char *)&stTime ,'\0',sizeof(struct tm));
  memset(pcTimeStr,'\0',sizeof(pcTimeStr));  
  /****算出当前的时间值****/
  ftime(&stTimeB_Now);
  memcpy(&stTime,localtime((time_t *)&(stTimeB_Now.time)),sizeof(struct tm));
  stTimeB_Now.time=mktime(&stTime);
  /****算出指定的时间值****/
  sscanf(pcNextTimeStr,"%2d%2d%2d",&(stTime.tm_hour),&(stTime.tm_min),&(stTime.tm_sec));
  stTimeB_Next.time = mktime(&stTime);
  if (stTimeB_Next.time>stTimeB_Now.time)
  {
      return(stTimeB_Next.time-stTimeB_Now.time);
  }
  else
  {
      return(stTimeB_Next.time-stTimeB_Now.time+3600*24);
  }    
}
/*****************************************************************************************/
/* 函数名称:  int lvsd_logwrite(struct  wd_lv_wbctl_area  *wp_lv_wbctl, char *string)    */
/* 功能描述: 将wd_lv_wbctl_area结构体中重要的信息写在log文件里面                         */
/* 创建日期: 2006-03-27                                                                  */
/* 作者:     卿苏德                                                                      */
/* 说明:     请注意修改 FileName的路径                                                   */
/*****************************************************************************************/
int lvsd_logwrite(struct lv_wbctl_c *wp_lv_wbctl, char *string,int times)
{
    FILE * fp;
    char Filename[80];
    char cTxday[9];

    memset(cTxday  , 0 , sizeof(cTxday));
    memset(Filename, 0 , sizeof(Filename));

    apitoa(g_pub_tx.tx_date,sizeof(cTxday)-1,cTxday);
    vtcp_log("[%s][%d] txday==[%s]",__FILE__,__LINE__,cTxday);

    sprintf(Filename, "%s/lvsd%s.txt", getenv("LOGDIR"), cTxday);

    vtcp_log("[%s][%d] 文件 ===[%s]",__FILE__,__LINE__,Filename);
    fp = fopen(Filename, "ab+");
    if (fp == NULL)
    {
        vtcp_log("[%s][%d] 打开准备写入的文件错误",__FILE__,__LINE__);
        return -1;
    }
    /*　因为wd_lv_wbctl结构体的每个成员都是以'\0'结尾的　所以直接COPY　*/
    vtcp_log("[%s][%d] txday==[%s]",__FILE__,__LINE__,cTxday);
    fprintf(fp, "\n");
    fprintf(fp, "PACKID[%s] |", wp_lv_wbctl->packid);
    fprintf(fp, "PACKDAY[%ld] |", wp_lv_wbctl->pack_date);
    fprintf(fp, "STAT[%s] |", wp_lv_wbctl->stat);
    fprintf(fp, "TOTAMT[%.0f] |", wp_lv_wbctl->totamt);
    fprintf(fp, "DTLCNT[%.0f] |", wp_lv_wbctl->dtlcnt);
    fprintf(fp, "PKGNO[%s] |", wp_lv_wbctl->pkgno);
    fprintf(fp, "CASHQSACTNO[%s] |", wp_lv_wbctl->cash_qs_no);
    fprintf(fp, "OPACKID[%s] |", wp_lv_wbctl->o_packid);
    fprintf(fp, "INFO:  [%s]", string);
    fprintf(fp, "times:  [%d]", times);
    fprintf(fp, "\n");
    fclose(fp);
    return 0;
}
/************************************************************************************/
/* 函数名称:  int pack_without_send( )                                              */
/* 功能描述: 遍历lv_wbctl找出满足已组包的兄弟，并且将它发送                         */
/* 创建日期: 2006-06-06                                                             */
/* 作者:     xyy                                                                    */
/************************************************************************************/

int pack_without_send()
{
    char cOldPackid[9];
    char cPkgno[4];
    struct lv_wbctl_c lv_wbctl;
    int iRc=0,cnt=0;
    memset(&lv_wbctl,0,sizeof(lv_wbctl));
    iRc=0;
    vtcp_log("[%s][%d]Begin of find pack_without_send",__FILE__,__LINE__);
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
    memset(cOldPackid,0,sizeof(cOldPackid));
    memcpy(cOldPackid,cPackid,sizeof(cPackid)-1);

    Lv_wbctl_Dec_Sel(g_pub_tx.reply, "stat='%c'",STAT3[0]);
    while (1)
    {
      memset(cPackid , 0 , sizeof(cPackid));
      memset(cPackday, 0 , sizeof(cPackday));
      memset(cPkgno  , 0 , sizeof(cPkgno));
      g_reply_int = Lv_wbctl_Fet_Sel(&lv_wbctl, g_pub_tx.reply);
      if (g_reply_int==100)break;
      if (g_reply_int!=0)
      {
          vtcp_log("[%s][%d]error fetch PWSEND g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
          sprintf( acErrMsg,"FETCH游标出错[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,(char*)NULL,(char*)NULL,cPackid,cPackday,acErrMsg,__LINE__);
          return g_reply_int;
      }
      cnt=0;
      memcpy(cPkgno,lv_wbctl.pkgno,sizeof(cPkgno)-1);
      memcpy(cPackid,lv_wbctl.packid,sizeof(cPackid)-1);
      apitoa(lv_wbctl.pack_date,sizeof(cPackday)-1,cPackday);
      vtcp_log("[%s][%d]cPkgno  =[%s]",__FILE__,__LINE__,cPkgno);
      vtcp_log("[%s][%d]cPackday=[%s]",__FILE__,__LINE__,cPackday);
      vtcp_log("[%s][%d]cPackid =[%s]",__FILE__,__LINE__,cPackid);

      cnt = sql_count("lv_wbctl","stat='%c'",STAT3[0]);
      vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt);
      if (cnt!=0)
      {
        if (memcmp(cPkgno,"002",3)==0 || memcmp(cPkgno,"006",3)==0 || memcmp(cPkgno,"008",3)==0 || memcmp(cPkgno,"011",3)==0 )
        {
            iRc=lv_batchptjj_send(cPkgno);
            if (iRc!=0)
            {
                bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,cPackid,cPackday,"发包错误",__LINE__);
            }
        }
        else if (memcmp(cPkgno,"001",3)==0 || memcmp(cPkgno,"005",3)==0 || memcmp(cPkgno,"007",3)==0)
        {
            iRc=lv_batchdj_send(cPkgno);
            if (iRc!=0)
            {
                bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,cPackid,cPackday,"发包错误",__LINE__);   
            }
        }
        else
        {
            vtcp_log("[%s][%d] The cPkgno is  wrong=[%s]",__FILE__,__LINE__,cPkgno);
            sprintf( acErrMsg,"没有这个包号[%s][%d]",__FILE__,__LINE__);
        }
      }
      else
      {
          vtcp_log("[%s][%d] 没有满足已组包但未发送的情况!",__FILE__,__LINE__,cPkgno);
      }
    }
    Lv_wbctl_Clo_Sel();
    memcpy(cPackday,pcLvGetDay(),8);/**把日期还原**/
    memcpy(cPackid,cOldPackid,sizeof(cPackid)-1);
    return(0);
}
int bh_ErrProce(char * cashqsactno,char * pkgno,char * orderno,char * orbrno,char * packid,char * packday,char * msg,int line)
{
    char cMsg[1000];
    struct lv_wbctl_c lv_wbctl;
    memset(cMsg, 0 , sizeof(cMsg));
    memset(&lv_wbctl, 0 ,sizeof(lv_wbctl));
    memcpy(lv_wbctl.pkgno,pkgno,sizeof(lv_wbctl.pkgno)-1);
    memcpy(lv_wbctl.cash_qs_no,cashqsactno,sizeof(lv_wbctl.cash_qs_no)-1);
    memcpy(lv_wbctl.packid,packid,sizeof(lv_wbctl.packid)-1);
    lv_wbctl.pack_date = apatoi(packday,8);

    memcpy(cMsg,msg,999);
    zip_space(cMsg);
    sprintf(cMsg+strlen(cMsg),"orderno=%s,orbrno=%s,line=%d",orderno,orbrno,line);
    printf("[%s][%d] msg=[%s]",__FILE__,__LINE__,cMsg);
    lvsd_logwrite(&lv_wbctl,cMsg,tep);
    sql_rollback();
    iErrFlag=1;
}
/****************************************************
*  FUNCTIONS:      lv_batch_dqdj                    *
*  DESCRIPTIPN:                                     *
****************************************************/
int lv_batch_dqdj(char * pkgno)
{
    char cPkgno[4];
    char cOldCashqsactno[13];
    int iRc=0;
    int cnt1=0;
    iRc=0;
    vtcp_log("[%s][%d]Begin of lv_batch_dqdj",__FILE__,__LINE__);

    vtcp_log("[%s][%d]cPackday=======[%s]",__FILE__,__LINE__,cPackday);
    memset(cPkgno, 0 , sizeof(cPkgno));
    memset(cOldCashqsactno,0,sizeof(cOldCashqsactno)); 
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
     
    memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);
    memcpy(cOldCashqsactno,CASHQSACTNO,sizeof(CASHQSACTNO)-1);

    distinct_dec_sel( "lv_pkgreg", "cash_qs_no","lv_sts='%c' and pkgno='%s' and lw_ind='1' and packid is null", STAT1[0],cPkgno);
    while (1)
    {
      memset(CASHQSACTNO, 0 , sizeof(CASHQSACTNO));
      g_reply_int = distinct_fet_sel(CASHQSACTNO);
      if (g_reply_int==100) 
      {
          break;
      }
      if (g_reply_int!=0)
      {
          vtcp_log("[%s][%d] error fetch dqdj sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
          break;
      }
      cnt=0;

      cnt = sql_count("lv_pkgreg","lv_sts='%c' and pkgno='%s' and lw_ind='1' and packid is null",STAT1[0],cPkgno);

      vtcp_log("[%s][%d]cnt=[%d]",__FILE__,__LINE__,cnt);
      if (cnt!=0)
      {
        distinct_dec_sel2( "lv_pkgreg", "pay_ac_no"," lv_sts='%c' and pkgno='%s' and cash_qs_no='%s' and lw_ind='1' and pkgno='%s' and packid is null", STAT1[0],cPkgno,CASHQSACTNO,cPkgno);
        while (1)
        {
          memset(PAYACTNO   , 0 , sizeof(PAYACTNO));

          g_reply_int = distinct_fet_sel2(PAYACTNO);
          if (g_reply_int==100)
          { 
              break;
          }
          if (g_reply_int!=0)
          {
              vtcp_log("[%s][%d] error fetch dqdj code=[%d]",__FILE__,__LINE__,g_reply_int);
              break;
          }
          zip_space(PAYACTNO);
          cnt1=0;

          cnt1 = sql_count("lv_pkgreg","lv_sts='%c' and cash_qs_no='%s' and pay_ac_no='%s' and lw_ind='1' and pkgno='%s' and packid is null",STAT1[0],CASHQSACTNO,PAYACTNO,cPkgno);

          vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt1);

          if (cnt1!=0)
          {
              iRc = lv_batchptdj_package(cPkgno);
              if (iRc!=0)
              {
                  bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"拼包错误",__LINE__);
              }
              else
              { 
                  iRc = lv_batchdj_send(cPkgno);
              }
              if (iRc!=0)
              {
                  bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"发包错误",__LINE__);
              }
          }
        }
        distinct_clo_sel2();
      }
    }
    distinct_clo_sel();
    memcpy(CASHQSACTNO,cOldCashqsactno,sizeof(CASHQSACTNO)-1);
    vtcp_log("[%s][%d]cPackday=======[%s]",__FILE__,__LINE__,cPackday);
    return(0);
}
/***************************************************
*  FUNCTIONS:      lv_batch_ptjj                   *
*  DESCRIPTIPN:                                    *
****************************************************/
int lv_batch_ptjj(char * pkgno)
{
    char cPkgno[4];
    int iRc=0;
    iRc=0;
    vtcp_log("[%s][%d]Begin of lv_batch_ptjj",__FILE__,__LINE__);
    memset(cPkgno, 0 , sizeof(cPkgno));
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
    memset(cSqlbuf1 , 0 , sizeof(cSqlbuf1));
    
    memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);

    distinct_dec_sel( "lv_pkgreg", "cash_qs_no", " lv_sts='%c' and pkgno='%s' and  txnum not in ('30102','30103') and lw_ind='1' and packid is null", STAT2[0],cPkgno);

    while (1)
    {
      memset(CASHQSACTNO, 0 , sizeof(CASHQSACTNO));
      g_reply_int = distinct_fet_sel(CASHQSACTNO);
      if (g_reply_int==100) break;
      if (g_reply_int!=0)
      {
          vtcp_log("[%s][%d] error fetch ptjj1 sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
          break;
      }
      cnt=0;
      vtcp_log("[%s][%d]=====CASHQSACTNO=[%s]",__FILE__,__LINE__,CASHQSACTNO);

      cnt = sql_count("lv_pkgreg","lv_sts='%c' and cash_qs_no='%s' and txnum not in ('30102','30103') and lw_ind='1' and pkgno='%s' and packid is null",STAT2[0],CASHQSACTNO,cPkgno);

      vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt);
      if (cnt!=0)
      {
          iRc = lv_batchptjj_package(cPkgno);
          if (iRc!=0)
          {
              bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"拼包错误",__LINE__);
          }
          else
          { 
              iRc = lv_batchptjj_send(cPkgno);
          }
          if (iRc!=0)
          {
              bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"发包错误",__LINE__);
          }
      }
    }
    distinct_clo_sel();
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    return(0);
}
/****************************************************
*  FUNCTIONS:      lv_batch_dqjj                    *
*  DESCRIPTIPN:                                     *
****************************************************/
int lv_batch_dqjj(char * pkgno)
{
    char cPkgno[4];
    char cOldCashqsactno[13];
    int iRc=0;
    int cnt1=0;
    iRc=0;
    vtcp_log("[%s][%d]Begin of lv_batch_dqjj",__FILE__,__LINE__);

    vtcp_log("[%s][%d]cPackday=======[%s]",__FILE__,__LINE__,cPackday);
    memset(cPkgno, 0 , sizeof(cPkgno));
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
    memset(cOldCashqsactno,0,sizeof(cOldCashqsactno));  

    memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);
    memcpy(cOldCashqsactno,CASHQSACTNO,sizeof(CASHQSACTNO)-1);

    distinct_dec_sel( "lv_pkgreg", "cash_qs_no"," lv_sts='%c' and pkgno='%s' and lw_ind='1' and packid is null", STAT2[0],cPkgno);
    while (1)
    {
      memset(CASHQSACTNO, 0 , sizeof(CASHQSACTNO));
      g_reply_int = distinct_fet_sel(CASHQSACTNO);
      if (g_reply_int==100) break;
      if (g_reply_int!=0)
      {
          vtcp_log("[%s][%d] error fetch dqjj sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
          break;
      }
      cnt=0;
      cnt = sql_count("lv_pkgreg","lv_sts='%c' and pkgno='%s' and lw_ind='1' and packid is null",STAT2[0],cPkgno);

      vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt);
      if (cnt!=0)
      {
        distinct_dec_sel2( "lv_pkgreg", "cash_ac_no"," lv_sts='%c' and pkgno='%s' and cash_qs_no='%s' and lw_ind='1' and packid is null", STAT2[0],cPkgno,CASHQSACTNO);
        while (1)
        {
          memset(CASHACTNO   , 0 , sizeof(CASHACTNO));

          g_reply_int = distinct_fet_sel2(CASHACTNO);
          if (g_reply_int==100)
          {
              break;
          }
          if (g_reply_int!=0)
          {
              vtcp_log("[%s][%d] error fetch dqjj sqlcode=[%d]",__FL__,g_reply_int);
              break;
          }
          zip_space(CASHACTNO);
          cnt1=0;

          cnt1 = sql_count("lv_pkgreg","lv_sts='%c' and cash_qs_no='%s' and cash_ac_no='%s' and lw_ind='1' and pkgno='%s' and packid is null",STAT2[0],CASHQSACTNO,CASHACTNO,cPkgno);

          vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt1);

          if (cnt1!=0)
          {
              iRc = lv_batchptjj_package(cPkgno);
              if (iRc!=0)
              {
                  bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"拼包错误",__LINE__);
              }
              else
              { 
                  iRc = lv_batchptjj_send(cPkgno);
              }
              if (iRc!=0)
              {
                  bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"发包错误",__LINE__);
              }
          }
        }
        distinct_clo_sel2();
      }
    }
    distinct_clo_sel();
    memcpy(CASHQSACTNO,cOldCashqsactno,sizeof(CASHQSACTNO)-1);
    vtcp_log("[%s][%d]cPackday=======[%s]",__FILE__,__LINE__,cPackday);
    return(0);
}
/********************************************************
*                                                       *
*  FUNCTION:      lv_batch_jjhz()                       *
*                                                       *
*  DESCRIPTION:  普通类型的借记业务回执                 *
********************************************************/
int lv_batch_jjhz(char * pkgno_jj)
{
    int  iFhcnt=0;
    int  recid=0;
    int  iTemp=0;
    int  iCnt =0;
    int  iChgFlag=0;
    char cPkgno_hz[4];
    char cPkgno_jj[4];
    char cPkgid   [9];
    char cPkgday  [9];
    char cCashqsno[13];
    char cTmpPackid[9];
    char cTmpPackday[9];
    struct lv_lbctl_c lv_lbctl;
		struct lv_wbctl_c lv_wbctl;
    int iRc=0;
		memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
    memset(cCashqsno, 0 , sizeof(cCashqsno));
    memset(cPkgid   , 0 , sizeof(cPkgid));
    memset(cPkgday  , 0 , sizeof(cPkgday));
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
    memset(cPkgno_hz, 0 , sizeof(cPkgno_hz));
    memset(cPkgno_jj, 0 , sizeof(cPkgno_jj));
    memset(cTmpPackid,0 , sizeof(cTmpPackid));
    memset(cTmpPackday,0, sizeof(cTmpPackday));
    memset(&lv_lbctl, 0 , sizeof(lv_lbctl));

    memcpy(cPkgno_jj, pkgno_jj , sizeof(cPkgno_jj)-1);
    
    iTemp = sql_count("lv_pkgreg","pkgno='%s' and o_pack_date=0 and lw_ind ='2' and o_packid is null  ",cPkgno_jj);
    if (iTemp==0)
    {
        return (0);
    }

    vtcp_log("[%s][%d]cPkgno==[%s]",__FILE__,__LINE__,cPkgno_jj);
    Lv_lbctl_Dec_Sel(g_pub_tx.reply, "pkgno='%s' and o_pack_date=0 and o_packid is null order by pack_date",cPkgno_jj);
    while(1)
    {
        vtcp_log("[%s][%d]lv_batch_jjhz begin!",__FILE__,__LINE__);
        memset(&lv_lbctl, 0 , sizeof(lv_lbctl));
        g_reply_int = Lv_lbctl_Fet_Sel(&lv_lbctl, g_pub_tx.reply);
        vtcp_log("[%s][%d]Lv_lbctl_Fet_Sel over!",__FILE__,__LINE__);
        if (g_reply_int==100 && iCnt ==0)
        {
              vtcp_log("[%s][%d]一条记录没有找到\n",__FILE__,__LINE__,iCnt);
              return 0;
        }
        if (g_reply_int!=100 && g_reply_int !=0)
        {
            vtcp_log("[%s][%d] fetch lbctl error sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"读取借记来帐游标错误[%s][%d]",__FILE__,__LINE__);
            bh_ErrProce((char*)NULL,cPkgno_jj,(char*)NULL,(char*)NULL,(char*)NULL,(char*)NULL,acErrMsg,__LINE__);
            return -1;
        }
        if(g_reply_int==100)
        {
        	  vtcp_log("[%s][%d]已经取到最后一条记录\n",__FILE__,__LINE__,iCnt);
        	  break;
        }
        memcpy(cPkgid, lv_lbctl.packid ,sizeof(cPkgid)-1);
        memcpy(cCashqsno, lv_lbctl.pay_qs_no ,sizeof(cCashqsno)-1);
        apitoa(lv_lbctl.pack_date,sizeof(cPkgday)-1,cPkgday);
        
        /**保存来帐的包号和包日期**/
        apitoa(lv_lbctl.pack_date,sizeof(cTmpPackday)-1,cTmpPackday);
        memcpy(cTmpPackid ,lv_lbctl.packid ,sizeof(cTmpPackid )-1);
        vtcp_log("[%s][%d]来包序号 cTmpPackid =[%s]",__FILE__,__LINE__,cTmpPackid);
        vtcp_log("[%s][%d]来包日期 cTmpPackday=[%s]",__FILE__,__LINE__,cTmpPackday);
        
        vtcp_log("[%s][%d]lv_lbctl.dtlcnt =[%ld]",__FILE__,__LINE__,lv_lbctl.dtlcnt);
        if(!memcmp(cPkgno_jj,PKGNO_PTJJ,sizeof(cPkgno_jj)-1))
        {
            memcpy(cPkgno_hz,PKGNO_PTJJHZ,sizeof(cPkgno_hz)-1);
        }
        if(!memcmp(cPkgno_jj,PKGNO_DQJJ,sizeof(cPkgno_jj)-1))
        {
            memcpy(cPkgno_hz,PKGNO_DQJJHZ,sizeof(cPkgno_hz)-1);
        }
        vtcp_log("[%s][%d]cPkgno_hz=[%s]",__FILE__,__LINE__,cPkgno_hz);
        vtcp_log("[%s][%d]cPkgid  =[%s]",__FILE__,__LINE__,cPkgid);
        vtcp_log("[%s][%d]cPkgday =[%s]",__FILE__,__LINE__,cPkgday);
        iFhcnt=0;
        iFhcnt = sql_count("lv_pkgreg"," pkgno='%s' and cash_qs_no='%s' and o_packid='%s' and o_pack_date='%s' and lv_sts='3'",cPkgno_hz,cCashqsno,cPkgid,cPkgday);
        vtcp_log("[%s][%d]iFhcnt =[%d]",__FILE__,__LINE__,iFhcnt);
        if ((double)iFhcnt!=lv_lbctl.dtlcnt)
        {
          vtcp_log("[%s][%d][%ld],[%s]您有未复核的交易，或者您没有全部回执，不能组包%d,%.0f ",
            __FILE__,__LINE__,lv_lbctl.pack_date,lv_lbctl.packid,iFhcnt,lv_lbctl.dtlcnt);
          continue;
        }
        vtcp_log("[%s][%d]lv_lbctl.o_packid=[%s]",__FILE__,__LINE__,cPackid);
        vtcp_log("[%s][%d]lv_lbctl.o_pack_date=[%s]",__FILE__,__LINE__,cPackday);
        vtcp_log("[%s][%d]cCashqsno=[%s]\n",__FILE__,__LINE__,cCashqsno);
        recid=0;
        if(memcmp(PKGNO_PTJJHZ,"008",3)==0)
        {
            Lv_pkgreg_Dec_Upd(g_pub_tx.reply, " pkgno='%s' and lw_ind='1' and txnum not in ('30104','30105') and cash_qs_no='%s' and o_packid='%s' and o_pack_date='%s' order by orderno",cPkgno_hz,cCashqsno,cPkgid,cPkgday);
        }
        else
        {
            Lv_pkgreg_Dec_Upd(g_pub_tx.reply, " pkgno='%s' and lw_ind='1' and cash_qs_no='%s' and o_packid='%s' and o_pack_date='%s'  order by orderno",cPkgno_hz,cCashqsno,cPkgid,cPkgday);
        }
        vtcp_log("[%s][%d]pack_date = [%ld]packid = [%s] \n",__FILE__,__LINE__,lv_lbctl.pack_date,lv_lbctl.packid);
        vtcp_log("[%s][%d]lv_lbctl.totcnt=[%.0f] \n",__FILE__,__LINE__,lv_lbctl.dtlcnt);
        while (1)
        {
          recid++;
          vtcp_log("[%s][%d]这是第[%d]次",__FILE__,__LINE__,recid);
          memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
          g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
          if (g_reply_int==100)
          {
              if (recid==1)
              {
                  return 0;/**没有记录就不要往下执行了**/
              }
              else
              {
                  vtcp_log("[%s][%d],没有可拼包的记录了",__FILE__,__LINE__);
                  break;
              }
              vtcp_log("[%s][%d],记录循环处理完毕",__FILE__,__LINE__);
              break;
          }
          if (g_reply_int)
          {
              vtcp_log("[%s][%d] fetch lv_ptjjs_hz error sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
              sprintf( acErrMsg,"读取游标错误[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce((char*)NULL,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
              break;
          }
          memset(&pay_in , 0 , sizeof(pay_in));
          memcpy(pay_in.F_opcd,OPCD_LR,sizeof(pay_in.F_opcd));
          vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD begin!", __FILE__,__LINE__);
          LV_CP_PAY_IN_FIELD(&pay_in,(&lv_pkgreg));
          vtcp_log("[%s][%d]payin.orderno=[%s]",__FILE__,__LINE__,pay_in.T_orderno);
          vtcp_log("[%s][%d]payin.wt_date=[%ld]",__FILE__,__LINE__,pay_in.T_wtday);
          vtcp_log("[%s][%d]payin.or_br_no=[%s]",__FILE__,__LINE__,pay_in.T_orbrno);
          vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD over!", __FILE__,__LINE__);
          iLvResetPayin(&pay_in);
          iLvPayinAutoInfo(&pay_in);/**得到日期和流水号**/
          if (pay_in.T_stat[0]!=STAT_WZCHECK2)/**如果不是复核状态就不允许**/
          {
              vtcp_log("[%s][%d],[%.8s]支付交易状态[%c]不符",__FILE__,__LINE__,pay_in.F_orderno,pay_in.T_stat[0]);
              sprintf( acErrMsg,"支付交易状态不符[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce(cCashqsno,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
              continue;
          }
          if (recid!=1)
          {
              memcpy(pay_in.F_packid,cPackid,sizeof(pay_in.F_packid));
              memcpy(pay_in.F_packday,cPackday,sizeof(pay_in.F_packday));
              vtcp_log("[%s][%d]=================cPackid=[%s]",__FILE__,__LINE__,cPackid);
              vtcp_log("[%s][%d]======pay_in.F_packday=[%s]",__FILE__,__LINE__,pay_in.F_packday);
          }
          /****记帐放在拼包中,方便修改交易的处理**/
          get_zd_data("0030",cOldKinbr);
          set_zd_data("0030",cQsbrno);
          iRc=iLvPayinCheck8(&pay_in,&lv_wbctl,NULL);
          set_zd_data("0030",cOldKinbr);
          if (iRc!=PCHK_NEW && iRc!=PCHK_UPDATE)
          {
              vtcp_log("[%s][%d],新生成包时[%d]出错",__FILE__,__LINE__,iRc);
              sprintf(acErrMsg,"更新支付交易登记簿错[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
              continue;/* 继续下一次循环 */
          }
          /**  将新生成的包序号传出   **/
          if (iRc==PCHK_UPDATE)
          {
              memcpy(pay_in.F_packid,lv_wbctl.packid,sizeof(pay_in.F_packid));
              memcpy(pay_in.T_packid,lv_wbctl.packid,sizeof(pay_in.T_packid));
          }
          memcpy(cPackid,pay_in.F_packid,sizeof(pay_in.F_packid));
          memcpy(cPackday,pay_in.F_packday,sizeof(pay_in.F_packday));
          vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
          vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
          LV_CP_PAY_IN_TABLE(&pay_in,(&lv_pkgreg));
          vtcp_log("[%s][%d]LV_CP_PAY_IN_TABLE over", __FILE__,__LINE__,LV_CP_PAY_IN_TABLE);
          
          memcpy(pay_in.F_packday, pcLvGetDay(), 8);
          memcpy(pay_in.F_packid, lv_wbctl.packid,8);
          
          vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
          memcpy(lv_pkgreg.packid,cPackid,sizeof(cPackday)-1);
          lv_pkgreg.pack_date = apatoi(cPackday,8);
          memcpy(lv_pkgreg.send_tel,cTlrno,sizeof(cTlrno)-1);
          vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
          vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);      
          vtcp_log("[%s][%d]lv_pkgreg.packid=[%s]", __FILE__,__LINE__,lv_pkgreg.packid);
          vtcp_log("[%s][%d]lv_pkgreg.pack_date=[%ld]", __FILE__,__LINE__,lv_pkgreg.pack_date);
          vtcp_log("[%s][%d]lv_pkgreg.send_tel=[%s]", __FILE__,__LINE__,lv_pkgreg.send_tel);

          g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
          if (g_reply_int!=DB_OK && g_reply_int!=100)
          {
              vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
              sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
              continue;/* 继续下一次循环 */
          }
          if (g_reply_int==100)
          {
              vtcp_log("[%s][%d]cPkgno_hz=[%s]", __FILE__,__LINE__,cPkgno_hz);
              sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
              continue;/* 继续下一次循环 */
          }
        }
        Lv_pkgreg_Clo_Upd();
        vtcp_log("%s,%d 自动拼出包,packid=[%s],packdate=[%s]",__FILE__,__LINE__,cTmpPackid,cTmpPackday);
        /***更新原来包的o_packid和o_pack_date**/
        g_reply_int = sql_execute("update lv_lbctl set o_packid='%s' ,o_pack_date='%s' where packid='%s' and pack_date='%s'",cPackid,cPackday,cTmpPackid,cTmpPackday);
        if (g_reply_int)
        {
            vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
            bh_ErrProce(cCashqsno,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
            continue;/* 继续下一次循环 */
        }
        /***更新原记录的o_packid和o_pack_date**/
        g_reply_int = sql_execute("update lv_pkgreg set o_packid='%s' ,o_pack_date='%s' where packid='%s' and pack_date='%s' and lw_ind='2' ",cPackid,cPackday,cTmpPackid,cTmpPackday);
        if (g_reply_int)
        {
            vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
            bh_ErrProce(cCashqsno,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
            continue;/* 继续下一次循环 */
        }
        /****把包记录封闭******/
        g_reply_int = sql_execute("update lv_wbctl set stat='%c' where packid='%s' and pack_date='%s'",STAT3[0],cPackid,cPackday);
        if (g_reply_int)
        {
            vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
            bh_ErrProce(cCashqsno,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
            continue;/* 继续下一次循环 */
        }
        /* note 这里为了测试直接rollback 同时continue下一个循环,看看是否会出现游标错误 */
        sql_commit();/*提交事务*/
        vtcp_log("[%s][%d]wbctl.totamt=[%f]",__FILE__,__LINE__,lv_wbctl.totamt);
        vtcp_log("[%s][%d]wbctl.dtlcnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlcnt);
        vtcp_log("[%s][%d]wbctl.dtlsuccnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlsuccnt);
        vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,lv_wbctl.dtlsucamt);
        vtcp_log("[%s][%d]wbctl.packid=[%s]",__FILE__,__LINE__,lv_wbctl.packid);
        
        vtcp_log("[%s][%d]已经到了这里喽兄弟   pkgno=[%s]",__FILE__,__LINE__,cPkgno_hz);
        vtcp_log("[%s][%d]已经到了这里喽兄弟cPackday=[%s]",__FILE__,__LINE__,cPackday);
        vtcp_log("[%s][%d]已经到了这里喽兄弟 cPackid=[%s]",__FILE__,__LINE__,cPackid);
        if (memcmp(cPkgno_hz,PKGNO_PTJJHZ,sizeof(cPkgno_hz)-1)==0)
        {
            vtcp_log("[%s][%d]lv_get_ptjjhz_tag begin",__FILE__,__LINE__);
            iRc = lv_get_ptjjhz_tag(cPackday,cPackid); /**PKG008普通借记回执包**/
            vtcp_log("[%s][%d]-拼普通借记回执发送包,iRc=[%d]",__FILE__,__LINE__,iRc);
            sprintf(acErrMsg,"拼普通借记回执发送包[%s][%d]",__FILE__,__LINE__);
        }
        else
        {
            vtcp_log("[%s][%d]lv_get_dqjjhz_tag begin",__FILE__,__LINE__);
            iRc = lv_get_dqjjhz_tag(cPackday,cPackid); /** PKG011定期借记业务包 **/
            vtcp_log("[%s][%d]-拼定期借记回执发送包,iRc=[%d]",__FILE__,__LINE__,iRc);
            sprintf(acErrMsg,"拼普通借记回执发送包[%s][%d]",__FILE__,__LINE__);
        }
        if (iRc!=0)
        {
            vtcp_log("[%s][%d]-发包错误,iRc=[%d]",__FILE__,__LINE__,iRc);
            bh_ErrProce(cCashqsno,cPkgno_jj,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
        }
				else
        	sql_commit();/*提交事务*/
        iCnt++;
    }
    Lv_lbctl_Clo_Sel();
    vtcp_log("[%s][%d] 开始找往帐业务！",__FILE__,__LINE__);
    
    vtcp_log("[%s][%d]lv_batch_jjhz over",__FILE__,__LINE__);
    return 0;
}
/********************************************************
*  FUNCTION:      lv_batchptjj_package()          *
*  DESCRIPTION:    1.组包                  *
*              2.封包                      *
********************************************************/
int lv_batchptjj_package(char * pkgno)
{
    int iRc;
    int recid=0;
    int iHzdate=0;
    char cPkgday[9];
    char cPkgno [4];
    char cWtday[9];
    char cOldBrno[BRNO_LEN+1];
    char wherelist[1024];  /**查询条件**/
    struct lv_pkgreg_c lv_pkgreg;
    struct lv_wbctl_c lv_wbctl;
    memset(cOldBrno , 0 , sizeof(cOldBrno));
    memset(cPackid  , 0 , sizeof(cPackid));
    memset(cPackday , 0 , sizeof(cPackday));
    memset(cPkgday  , 0 , sizeof(cPkgday));
    memset(cPkgno   , 0 , sizeof(cPkgno));
    memset(cWtday   , 0 , sizeof(cWtday));
    memset(wherelist, 0 , sizeof(wherelist));
    memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
    memset(&lv_pkgreg, 0 , sizeof(lv_pkgreg));
    iHzdate=HZDATE;
    memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);

    vtcp_log("[%s][%d]cPkgno==[%s]",__FILE__,__LINE__,cPkgno);
    /* 保存原理交易机构号 */
    get_zd_data("0030",cOldBrno);
    if (memcmp(cPkgno,PKGNO_PTJJ,sizeof(cPkgno)-1)==0 )/**普通借记**/
    {
        sprintf(wherelist,"cash_qs_no='%s' and lv_sts='%c' and lw_ind='1' and txnum not in ('30102','30103') and packid is null and pkgno='%s'",CASHQSACTNO,STAT2[0],cPkgno);
    }
    else if (memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)/*定期借记*/
    {
        sprintf(wherelist,"cash_qs_no='%s' and lv_sts='%c' and lw_ind='1' and packid is null and pkgno='%s' and cash_ac_no='%s'",CASHQSACTNO,STAT2[0],cPkgno,CASHACTNO);
    }
    g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, wherelist);
    if (g_reply_int!=DB_OK && g_reply_int!=100)
    {
        vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"数据库错误[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    if (g_reply_int==100)
    {
        vtcp_log("[%s][%d],没有可拼包的记录",__FILE__,__LINE__);
        sprintf( acErrMsg,"没有可拼包的记录[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    while (1)
    {
      memset(&pay_in,'\0',sizeof(pay_in));
      g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
      recid++;
      vtcp_log("同一接收行第[%d]次循环子处理 [%s][%d]",recid, __FILE__,__LINE__);

      if (g_reply_int!=DB_OK && g_reply_int!=100)
      {
          vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
          sprintf( acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,"访问数据库错误");
          WRITEMSG
          return (g_reply_int);
      }
      if (g_reply_int==100)
      {
          if (recid==1)
          {
              vtcp_log("[%s][%d],没有可拼包的记录sqlcode=100",__FILE__,__LINE__);
              sprintf( acErrMsg,"没有可拼包的记录[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
              return(g_reply_int);
          }
          vtcp_log("[%s][%d],记录循环处理完毕",__FILE__,__LINE__);
          break;
      }
      set_zd_data("0020",lv_pkgreg.br_no);/**开户机构**/
      set_zd_data("0030",lv_pkgreg.br_no);/**交易机构**/

      vtcp_log("[%s][%d]", __FILE__,__LINE__);

      pay_in.F_crdb[0]='2';
      iLvPayinAutoInfo(&pay_in);
      vtcp_log("[%s][%d]", __FILE__,__LINE__);

      memcpy(pay_in.F_opcd,OPCD_LR,sizeof(pay_in.F_opcd));

      LV_CP_PAY_IN_FIELD(&pay_in,(&lv_pkgreg));
      vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD over!", __FILE__,__LINE__);

      vtcp_log("[%s][%d]iLvFetchRecToPayin over!", __FILE__,__LINE__);
      iLvResetPayin(&pay_in);
      vtcp_log("[%s][%d]iLvResetPayin over!", __FILE__,__LINE__);

      memcpy(cWtday,pay_in.F_wtday,sizeof(cWtday)-1);
      memcpy(pay_in.F_respday , pub_base_daynumLAT(cWtday,iHzdate),sizeof(pay_in.F_respday));
      vtcp_log("[%s][%d]回执日期==[%s]",__FILE__,__LINE__,pay_in.F_respday);

      g_reply_int=iCheckHzdate(pay_in.T_cashqsactno,iHzdate);
      if (g_reply_int)
      {
          vtcp_log("[%s][%d],检查回执日期错误[%d]",__FILE__,__LINE__,g_reply_int);
          sprintf(acErrMsg,"检查回执日期错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,(char *)NULL,cWtday,acErrMsg,__LINE__);
          return(g_reply_int);
      }
      if (pay_in.T_stat[0]!=STAT_WZLR)
      {
          vtcp_log("[%s][%d][%.8s]支付交易状态[%c]不符",__FILE__,__LINE__,pay_in.F_orderno,pay_in.T_stat[0]);
          sprintf(acErrMsg,"支付交易状态不符[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,(char *)NULL,cWtday,acErrMsg,__LINE__);
          return(-1);
      }
      vtcp_log("[%s][%d]", __FILE__,__LINE__);
      /****产生一个新包   如果不是第一条记录,将packid赋值****/
      if (recid!=1)
      {
          memcpy(pay_in.F_packid,cPackid,sizeof(pay_in.F_packid));
          memcpy(pay_in.F_packday,cPackday,sizeof(pay_in.F_packday));
          vtcp_log("[%s][%d]======cPackid=[%s]",__FILE__,__LINE__,cPackid);
          vtcp_log("[%s][%d]======pay_in.F_packday=[%s]",__FILE__,__LINE__,pay_in.F_packday);
      }
      vtcp_log("[%s][%d]", __FILE__,__LINE__);
      /****产生新包的第一条记录,设置新包内容,以后就不再设置了***/
      get_zd_data("0030",cOldKinbr);
      set_zd_data("0030",cQsbrno);
      iRc=iLvPayinCheck8(&pay_in,&lv_wbctl,NULL);
      set_zd_data("0030",cOldKinbr);
      apitoa(lv_wbctl.pack_date,sizeof(cPkgday)-1,cPkgday);
      vtcp_log("[%s][%d]cOldKinbr =[%s]", __FILE__,__LINE__,cOldKinbr);
      if (iRc!=PCHK_NEW && iRc!=PCHK_UPDATE)
      {
          vtcp_log("[%s][%d],新生成包时[%d]出错",__FILE__,__LINE__,iRc);
          sprintf(acErrMsg,"更新支付交易登记簿错[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
          return(iRc);
      }
      vtcp_log("[%s][%d] packid==[%.8s]=[%.8s]",__FILE__,__LINE__,pay_in.F_packid,lv_wbctl.packid);
      /**  将新生成的包序号传出   **/
      if (iRc==PCHK_UPDATE)
      {
          memcpy(pay_in.F_packid,lv_wbctl.packid,sizeof(pay_in.F_packid));
          memcpy(pay_in.T_packid,lv_wbctl.packid,sizeof(pay_in.T_packid));
      }
      memcpy(cPackid,pay_in.F_packid,sizeof(pay_in.F_packid));
      memcpy(cPackday,pay_in.F_packday,sizeof(pay_in.F_packday));
      vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
      vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);

      LV_CP_PAY_IN_TABLE(&pay_in,(&lv_pkgreg));
      vtcp_log("[%s][%d]LV_CP_PAY_IN_TABLE over", __FILE__,__LINE__,LV_CP_PAY_IN_TABLE);

      memcpy(pay_in.F_packday, pcLvGetDay(), 8);
      memcpy(pay_in.F_packid, lv_wbctl.packid,8);

      memcpy(lv_pkgreg.packid,cPackid,sizeof(cPackid)-1);
      lv_pkgreg.pack_date = apatoi(cPackday,8);
      memcpy(lv_pkgreg.send_tel,cTlrno,sizeof(cTlrno)-1);
      g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
      if (g_reply_int!=DB_OK && g_reply_int!=100)
      {
          vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
          sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
          return(iRc);
      }
      if (g_reply_int==100)
      {
          vtcp_log("[%s][%d]pkgno[%s]", __FILE__,__LINE__,cPkgno);
          sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
          return(g_reply_int);
      }
      vtcp_log("同一接收行第[%d]次循环子处理结束 [%s][%d]",recid,__FILE__,__LINE__);
    }
    Lv_pkgreg_Clo_Upd();
    /****把包记录封闭******/
    /****这里还需要改，先找lv_parmd看看回执日期，如果没有的话再用宏定义***/
    sprintf(cHzdate,"%02d",HZDATE);
    g_reply_int = sql_execute("update lv_wbctl set stat='%c' ,hz_date ='%s' where packid='%s' and pack_date='%s'",STAT3[0],cHzdate,cPackid,cPackday);
    if (g_reply_int)
    {
        vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    sql_commit();/*提交事务*/
    /* 恢复交易机构 */
    set_zd_data("0030",cOldBrno);
    return 0;
}

vmbfe_log( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
    va_list ap;
    int     year, mon, day, hour, min, sec;
    FILE *    fp;
    char    tmpstr[100];
    char    tmpname[100];
    char    comm_t[100];
    char    log_time[9];
    long    logsize;
    struct stat lfsts;

    memset(tmpstr,0x00,sizeof(tmpstr));
    memset(tmpname,0x00,sizeof(tmpname));
    memset(log_time,0x00,sizeof(log_time));

    /*  struct timeb Tp; */
    va_start(ap);

    pub_base_get_date(&year, &mon, &day);
    pub_base_GetTime2(&hour, &min, &sec);

    sprintf(log_time,"%02d%02d%02d",day,hour,min);
    if ( getenv("LOGDIR") )
    {
        strcpy( tmpstr, getenv("LOGDIR") );
    }
    else
    {
        strcpy(tmpstr,"/tmp");
    }
    strcat( tmpstr, "/" );

    if ( getenv("LOGFILE") )
    {
        strcat( tmpstr, getenv("LOGFILE") );
    }
    else
    {
        strcat(tmpstr,"trad.log");
    }
    fp = fopen(tmpstr, "a");

    stat(tmpstr, &lfsts);
    if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
    {
        logsize=500000;
    }
    else
    {
      logsize=atol( getenv( "LOGSIZE" ) );
    }
    if ( lfsts.st_size >= logsize ) 
    {
        fclose(fp);
        strcpy(tmpname, tmpstr);
        memcpy(tmpname+strlen(tmpname)-3, "old", 3);
        umask(0);
        rename(tmpstr, tmpname);
        fp = fopen(tmpstr, "a");
    }
    if (fp)
    {
        fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d ",year, mon, day, hour, min ,sec);
        vfprintf(fp, fmtstr, ap);
        fprintf(fp, "\n");
        fclose(fp);
    }
    va_end(ap);    
    if ( lfsts.st_size >= logsize ) 
    {
        sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
        system(comm_t);
    }
    return 0;
}

int lv_batch_ptjjzphz(char * pkgno)
{
  char cPkgno[4];
  int iRc=0;
  iRc=0;
  vtcp_log("[%s][%d]Begin of lv_batch_ptjjzphz begin",__FILE__,__LINE__);

  memset(cPkgno, 0 , sizeof(cPkgno));
  memset(cSqlbuf1 , 0 , sizeof(cSqlbuf1));
  memset(cPackid  , 0 , sizeof(cPackid));
  memset(cPackday , 0 , sizeof(cPackday));
  
  memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);

  distinct_dec_sel( "lv_pkgreg", "cash_qs_no", " lv_sts='%c' and pkgno='%s' and lw_ind='1' and txnum in ('30104','30105') and packid is null", STAT1[0],cPkgno);

  while (1)
  {
    memset(CASHQSACTNO, 0 , sizeof(CASHQSACTNO));
    g_reply_int = distinct_fet_sel(CASHQSACTNO);
    if (g_reply_int==100) break;
    if (g_reply_int!=0)
    {
        vtcp_log("[%s][%d] error fetch ptjjzp sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
        break;
    }
    cnt=0;
    vtcp_log("[%s][%d]=====CASHQSACTNO=[%s]",__FILE__,__LINE__,CASHQSACTNO);

    cnt = sql_count("lv_pkgreg","lv_sts='%c' and cash_qs_no='%s' and lw_ind='1' and pkgno='%s' and txnum in ('30104','30105') and packid is null",STAT1[0],CASHQSACTNO,cPkgno);

    vtcp_log("[%s][%d]=========cnt=[%d]",__FILE__,__LINE__,cnt);
    vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
    if (cnt!=0)
    {
      iRc=lv_batch_zphz(cPkgno);

      if (iRc!=0)
      {
          bh_ErrProce(CASHQSACTNO,cPkgno,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,"拼包错误",__LINE__);
          vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
      }
    }
  }
  distinct_clo_sel();
  vtcp_log("[%s][%d]", __FILE__,__LINE__);
  return(0);
}
/**处理支票业务的拼包**/
int lv_batch_zphz(char * pkgno_ptjj)
{
    int  recid=0;
    char cPkgno_hz[4];
    char cTxcode  [5];
    char cPkgid   [9];
    char cPkgday  [9];
    char cCashqsno[13];
    char cTmpPackday[9];
    char cTmpPackid [9];
    
    struct lv_lbctl_c lv_lbctl;
    struct lv_wbctl_c lv_wbctl;

    int iRc=0;
    memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
    memset(cCashqsno, 0 , sizeof(cCashqsno));
    memset(cPkgid   , 0 , sizeof(cPkgid));
    memset(cTxcode  , 0 , sizeof(cTxcode));
    memset(cPkgday  , 0 , sizeof(cPkgday));
    memset(cPkgno_hz, 0 , sizeof(cPkgno_hz));
    memset(cTmpPackid,0 , sizeof(cTmpPackid));
    memset(cTmpPackday,0, sizeof(cTmpPackday));
    memset(&lv_lbctl, 0 , sizeof(lv_lbctl));

    memcpy(cPkgno_hz, pkgno_ptjj , sizeof(cPkgno_hz)-1);
    /****石总说实际的生产环境支票业务的回执，是在lbctl表中找不到记录的***/
    g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, " pkgno='%s' and cash_qs_no='%s' and lv_sts='%c'  and  txnum in ('30104','30105') and lw_ind='1' and packid is null order by orderno",cPkgno_hz,CASHQSACTNO,STAT1[0]);
    if (g_reply_int!=DB_OK && g_reply_int!=100)
    {
        vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"数据库错误[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(CASHQSACTNO,cPkgno_hz,(char *)NULL,(char *)NULL,(char *)NULL,(char *)NULL,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    recid=0;
    while (1)
    {
      recid++;
      vtcp_log("[%s][%d]这是第[%d]次",__FILE__,__LINE__,recid);
      memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
      g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
      if (g_reply_int==100)
      {
          if (recid==1)
          {
              vtcp_log("[%s][%d],数据库错误",__FILE__,__LINE__);
              sprintf( acErrMsg,"没有可拼包的记录[%s][%d]",__FILE__,__LINE__);
              bh_ErrProce((char*)NULL,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
              break;
          }
          vtcp_log("[%s][%d],记录循环处理完毕",__FILE__,__LINE__);
          break;
      }
      if (g_reply_int)
      {
          vtcp_log("[%s][%d] fetch lv_ptjjs_hz error sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
          sprintf( acErrMsg,"读取游标错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce((char*)NULL,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
          break;
      }
      memset(&pay_in , 0 , sizeof(pay_in));
      memcpy(pay_in.F_opcd,OPCD_LR,sizeof(pay_in.F_opcd));

      vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD begin!", __FILE__,__LINE__);
      LV_CP_PAY_IN_FIELD(&pay_in,(&lv_pkgreg));
      vtcp_log("[%s][%d]payin.orderno=[%s]",__FILE__,__LINE__,pay_in.T_orderno);
      vtcp_log("[%s][%d]payin.wt_date=[%ld]",__FILE__,__LINE__,pay_in.T_wtday);
      vtcp_log("[%s][%d]payin.or_br_no=[%s]",__FILE__,__LINE__,pay_in.T_orbrno);
      vtcp_log("[%s][%d]LV_CP_PAY_IN_FIELD over!", __FILE__,__LINE__);
      iLvResetPayin(&pay_in);
      iLvPayinAutoInfo(&pay_in);/**得到日期和流水号**/

      if (pay_in.T_stat[0]!=STAT_WZCHECK2)/**如果不是复核状态就不允许**/
      {
          vtcp_log("[%s][%d],[%.8s]支付交易状态[%c]不符",__FILE__,__LINE__,pay_in.F_orderno,pay_in.T_stat[0]);
          sprintf( acErrMsg,"支付交易状态不符[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(cCashqsno,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
          break;
      }
      if (recid!=1)
      {
          memcpy(pay_in.F_packid,cPackid,sizeof(pay_in.F_packid));
          memcpy(pay_in.F_packday,cPackday,sizeof(pay_in.F_packday));
          vtcp_log("[%s][%d]=================cPackid=[%s]",__FILE__,__LINE__,cPackid);
          vtcp_log("[%s][%d]======pay_in.F_packday=[%s]",__FILE__,__LINE__,pay_in.F_packday);
      }
      /****记帐放在拼包中,方便修改交易的处理**/
      get_zd_data("0030",cOldKinbr);
      set_zd_data("0030",cQsbrno);
      vtcp_log("[%s][%d]pay_in.F_txnum=[%.5s]\n",__FILE__,__LINE__,pay_in.F_txnum);
      vtcp_log("[%s][%d]pay_in.T_txnum=[%.5s]\n",__FILE__,__LINE__,pay_in.T_txnum);
      memset(cTxcode  , 0 , sizeof(cTxcode));
      memcpy(cTxcode,g_pub_tx.tx_code,sizeof(cTxcode)-1);
      vtcp_log("[%s][%d]cTxcode=[%s]\n",__FILE__,__LINE__,cTxcode);
      memcpy(g_pub_tx.tx_code,"5926",4);
      iRc=iLvPayinCheck8(&pay_in,&lv_wbctl,NULL);
      set_zd_data("0030",cOldKinbr);
      
      if (iRc!=PCHK_NEW && iRc!=PCHK_UPDATE)
      {
          vtcp_log("[%s][%d],新生成包时[%d]出错",__FILE__,__LINE__,iRc);
          sprintf(acErrMsg,"更新支付交易登记簿错[%s][%d]",__FILE__,__LINE__);
          memcpy(g_pub_tx.tx_code,cTxcode,sizeof(cTxcode)-1);/*恢复原来交易号 by LiuHuafeng 2007-5-5 0:38*/
          bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPkgday,acErrMsg,__LINE__);
          return(iRc);
      }
      memcpy(g_pub_tx.tx_code,cTxcode,sizeof(cTxcode)-1);
      vtcp_log("[%s][%d]cTxcode=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
      
      vtcp_log("[%s][%d] packid==[%.8s]=[%.8s]",__FILE__,__LINE__,pay_in.F_packid,lv_wbctl.packid);
      vtcp_log("[%s][%d] very CD  lv_wbctl.pkgno==[%.3s]",__FILE__,__LINE__,lv_wbctl.pkgno);
      vtcp_log("[%s][%d] very CDlv_wbctl.packid xyy ===[%s] ",__FL__,lv_wbctl.packid);
      vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
      /**  将新生成的包序号传出   **/
      if (iRc==PCHK_UPDATE)
      {
          memcpy(pay_in.F_packid,lv_wbctl.packid,sizeof(pay_in.F_packid));
          memcpy(pay_in.T_packid,lv_wbctl.packid,sizeof(pay_in.T_packid));
      }
      memcpy(cPackid,pay_in.F_packid,sizeof(pay_in.F_packid));
      memcpy(cPackday,pay_in.F_packday,sizeof(pay_in.F_packday));
      vtcp_log("[%s][%d]cPackid=[%s]", __FILE__,__LINE__,cPackid);
      vtcp_log("[%s][%d]cPackday=[%s]", __FILE__,__LINE__,cPackday);
      LV_CP_PAY_IN_TABLE(&pay_in,(&lv_pkgreg));
      vtcp_log("[%s][%d]LV_CP_PAY_IN_TABLE over", __FILE__,__LINE__,LV_CP_PAY_IN_TABLE);
      
      memcpy(pay_in.F_packday, pcLvGetDay(), 8);
      memcpy(pay_in.F_packid, lv_wbctl.packid,8);
      
      vtcp_log("[%s][%d] very CDlv_wbctl.packday xyy =[%ld] ",__FL__,lv_wbctl.pack_date);
      memcpy(lv_pkgreg.packid,cPackid,sizeof(cPackday)-1);
      lv_pkgreg.pack_date = apatoi(cPackday,8);
      memcpy(lv_pkgreg.send_tel,cTlrno,sizeof(cTlrno)-1);
      
      
      /**保存来帐的包号和包日期**/
      apitoa(lv_pkgreg.o_pack_date,sizeof(cTmpPackday)-1,cTmpPackday);
      memcpy(cTmpPackid ,lv_pkgreg.o_packid,sizeof(cTmpPackid )-1);
      
      vtcp_log("[%s][%d]lv_pkgreg.packid=[%s]", __FILE__,__LINE__,lv_pkgreg.packid);
      vtcp_log("[%s][%d]lv_pkgreg.pack_date=[%ld]", __FILE__,__LINE__,lv_pkgreg.pack_date);
      vtcp_log("[%s][%d]lv_pkgreg.o_packid=[%s]", __FILE__,__LINE__,lv_pkgreg.o_packid);
      vtcp_log("[%s][%d]lv_pkgreg.o_pack_date=[%ld]", __FILE__,__LINE__,lv_pkgreg.o_pack_date);
      vtcp_log("[%s][%d]lv_pkgreg.send_tel=[%s]", __FILE__,__LINE__,lv_pkgreg.send_tel);
      g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
      if (g_reply_int!=DB_OK && g_reply_int!=100)
      {
          vtcp_log("[%s][%d],数据库错误sqlcode=%d",__FILE__,__LINE__,g_reply_int);
          sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
          return(iRc);
      }
      if (g_reply_int==100)
      {
          vtcp_log("[%s][%d]cPkgno_hz=[%s]", __FILE__,__LINE__,cPkgno_hz);
          sprintf(acErrMsg,"访问数据库错误[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(CASHQSACTNO,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
          return(g_reply_int);
      }
      /***更新原记录的o_packid和o_pack_date**/
      g_reply_int = sql_execute("update lv_pkgreg set o_packid='%s' ,o_pack_date='%s' where packid='%s' and pack_date='%s'",cPackid,cPackday,cTmpPackid,cTmpPackday);
      if (g_reply_int)
      {
          vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
          sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
          bh_ErrProce(cCashqsno,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
          return(g_reply_int);
      }
    }
    Lv_pkgreg_Clo_Upd();
    
    /****把包记录封闭******/
    g_reply_int = sql_execute("update lv_wbctl set stat='%c' where packid='%s' and pack_date='%s'",STAT3[0],cPackid,cPackday);
    if (g_reply_int)
    {
        vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"更新往包控制表失败[%s][%d]",__FILE__,__LINE__);
        bh_ErrProce(cCashqsno,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_wbctl.packid,cPackday,acErrMsg,__LINE__);
        return(g_reply_int);
    }
    sql_commit();/*提交事务*/
    vtcp_log("[%s][%d]wbctl.totamt=[%f]",__FILE__,__LINE__,lv_wbctl.totamt);
    vtcp_log("[%s][%d]wbctl.dtlcnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlcnt);
    vtcp_log("[%s][%d]wbctl.dtlsuccnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlsuccnt);
    vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,lv_wbctl.dtlsucamt);
    vtcp_log("[%s][%d]wbctl.packid=[%s]",__FILE__,__LINE__,lv_wbctl.packid);
    
    vtcp_log("[%s][%d]已经到了这里喽兄弟   pkgno=[%s]",__FILE__,__LINE__,cPkgno_hz);
    vtcp_log("[%s][%d]已经到了这里喽兄弟cPackday=[%s]",__FILE__,__LINE__,cPackday);
    vtcp_log("[%s][%d]已经到了这里喽兄弟 cPackid=[%s]",__FILE__,__LINE__,cPackid);
    if (memcmp(cPkgno_hz,PKGNO_PTJJHZ,sizeof(cPkgno_hz)-1)==0)
    {
        vtcp_log("[%s][%d]lv_get_ptjjhz_tag begin",__FILE__,__LINE__);
        iRc = lv_get_ptjjhz_tag(cPackday,cPackid); /**PKG008普通借记业务包**/
        vtcp_log("[%s][%d]-拼普通借记回执发送包,iRc=[%d]",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"拼普通借记回执发送包[%s][%d]",__FILE__,__LINE__);
    }
    if (iRc!=0)
    {
        vtcp_log("[%s][%d]-发包错误,iRc=[%d]",__FILE__,__LINE__,iRc);
        bh_ErrProce(cCashqsno,cPkgno_hz,lv_pkgreg.orderno,lv_pkgreg.or_br_no,cPackid,cPackday,acErrMsg,__LINE__);
    }
    sql_commit();/*提交事务*/
    
    vtcp_log("[%s][%d]lv_batch_zphz over",__FILE__,__LINE__);
    return 0;
}
