/******************************************************************
模块名称: 来账报文接收
函数功能: 来账报文接收 
函数原型:   
输入参数:
输出参数:
使用说明:
编 写 者: ChenMing
修 改 者: ChenMing  2006/09/12
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"

#include "lv_pub.h"
#include "lvpack.h"
#include "lv_define.h"

#include "hvpack.h"
#include "lv_wbctl_c.h"
#include "com_parm_c.h"

#include "mo_merge_ac_c.h"
#include "prdt_ac_id_c.h"

#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "tctd_ktzh_c.h"
#include "cashier_c.h"/**YHBP**/
#include "hv_fzf_c.h"/**YHBP**/
#include "lv_addit_c.h"/**NEWBP2**/

#define MTU_LEN 512
void vLvSetBrnoAndSts_Fd(char * brno,char * stat,char * file,int line);
/****Add by SSH,2005.12.1临时定义,当get_tag函数不再后补0时可以不用****/
static char* GETTAG(TLVPACKAGE *pack,char *name,char *value)
{
  lv_get_tag(pack,name,value);
  return(value);
}

/***************************************************************************/
/* 函数名称: pub_lv_recv_check_main()                                      */
/* 功能描述: spJ098 小额来帐－挂帐检查  主函数                             */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:     g_pub_tx.reply= 0000/0010/0020                                */
/***************************************************************************/
int pubf_lv_recv_check_main()
{
  int  ret = 0;
  char Stat = '';
  char cmtno[4 ];      /** 报文编号**/
  
  TLVPACKAGE lv_pack;  
  int  hangflg = 0;     
  char cFinishflg;          /* 结束标志 '0'=PKG包还未处理完毕   '1'=PKG包已处理完毕*/
  int  iSubpackIndex = 0;   /* 子报文序号 */

  get_zd_data("0500",&cFinishflg);
  get_zd_long("0520",&iSubpackIndex);
  vtcp_log("%s,%d ========  cFinishflg=[%c]    iSubpackIndex=[%d] ========",__FL__,cFinishflg,iSubpackIndex);  
  vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
  vLvBrnoDebug(__FILE__,__LINE__);
  if ( (cFinishflg != '0') && (cFinishflg != '1') )
  {    
    set_zd_data("0500","1");       /* 出口：置为结束 */
    sprintf(acErrMsg,"cFinishflg为错误的结束标志 ! ");
    WRITEMSG  
    return -1;
  }
  if (cFinishflg == '1')
  {
    vtcp_log("%s,%d ======== 来报循环处理完毕，直接奔J099 !!!! ========",__FL__);  
    /* begin add by LiuHuafeng 20090515 for 村镇银行 */
		/* 如果是920 并且 finish flag ==1 就不再继续处理 */
		vtcp_log("%s,%d finishflag ==1  and cmtno=%s",__FILE__,__LINE__,cmtno);
    strcpy( g_pub_tx.reply, "0010" );  
    return 0;
  }
  memset(cmtno,'\0',sizeof(cmtno)); 
  memset(&lv_pack,'\0',sizeof(lv_pack));   
  lv_create(&lv_pack);
  if ( pubf_lv_read_pack(&lv_pack) )   /* 读来帐报文  */
  {
    sprintf(acErrMsg,"读来帐报文出错 ! ");
    WRITEMSG  
    lv_destroy(&lv_pack);      
    return -1;
  }
  /* pubf_lv_print_dm(&lv_pack.dm); */
  dm_show(&(lv_pack.dm));
  vLvBrnoDebug(__FILE__,__LINE__);
  ret = lv_analyze_pack(&lv_pack);
  vLvBrnoDebug(__FILE__,__LINE__);
  if ( ret )  /** 检查报文格式是否合法 **/ 
  {
    sprintf(acErrMsg,"接收到的报文格式错误 [%d] ", ret);
    WRITEMSG
    lv_destroy(&lv_pack);      
    return -1;
  }   
  vtcp_log(" ============= 接收到的报文格式正确 ============= ");    
  vtcp_log("[%s][%d]=====Recievee length==[%d]==[%d]",__FILE__,__LINE__,lv_pack.length, lv_pack.dm.length);
  vtcp_log("[%s][%d]=====此笔传入的报文是==[%.3s]  =报文类型==%d", __FILE__,__LINE__,lv_pack.CMTCode,lv_pack.pkgType);

  memcpy(cmtno,lv_pack.CMTCode,3);   /** 得到报文编号 **/   
  set_zd_data("0510",cmtno);
  vtcp_log("%s,%d,开始进入分别处理....   ",__FILE__,__LINE__);

  if (lv_pack.pkgType)  /* 是PKG包 */
  {  
    vtcp_log("[%s][%d] 是PKG包 cmtcode=[%.3s]",__FILE__,__LINE__,lv_pack.CMTCode);
        if(iLvchkstat())
        {
            vtcp_log("%s,%d 这里判断是否可以接受来帐"__FILE__,__LINE__);
            strncpy(g_pub_tx.reply,"P046",4);
            WRITEMSG
            return 9999;
        }

    {/* 这里判断是否是补发的往包，补发的往包不走帐，补发的来包照常处理 */
        char pcPayqsactno[13];
        char pcBfFlag[2];
        /****根据发起清算行判断是来帐还是往帐, 因为可能有对帐时补发的往帐****/
        memset(pcPayqsactno,'\0',sizeof(pcPayqsactno));
        memset(pcBfFlag,'\0',sizeof(pcBfFlag));          
        lv_pkg_getHeaderTag(&lv_pack,"011",pcPayqsactno);
        lv_pkg_getHeaderTag(&lv_pack,"BS4",pcBfFlag);
        vtcp_log("[%s][%d] pcPayqsactno=[%s]  pcBfFlag=[%s]  ",__FILE__,__LINE__,pcPayqsactno,pcBfFlag);
        if(pcBfFlag[0]=='1')    /****是补发的包****/
        {
          vtcp_log("[%s][%d] 是补发的包 ",__FILE__,__LINE__);   
          /**** 是补发的往帐 不记帐 ****/
          if (memcmp(pcPayqsactno,HV_QS_BR_NO,sizeof(pcPayqsactno)-1)==0)
          {
            vtcp_log("[%s][%d] 是补发的往帐 转 J099 登记 !!! \n",__FILE__,__LINE__);
            /*  往帐少帐一般不会发生，如果发生了，只插入往包控制表 */
            strcpy( g_pub_tx.reply, "0010" );  /* 转 J099  登记 !!! */ 
            set_zd_data("0510",cmtno);                         
            return 0;
          }
          else  /****  ??  补发的来帐和正常的来包一样处理  ?? ****/
          {
            /* ??  */
            vtcp_log("[%s][%d] 是补发的来帐 \n",__FILE__,__LINE__);      
            vtcp_log("[%s][%d] pcPayqsactno=[%s]  pcBfFlag=[%s]  ",__FILE__,__LINE__,pcPayqsactno,pcBfFlag);
          }
        }
    }

    /* 根据 CMTCode 分别做挂帐检查 */
    if (memcmp(lv_pack.CMTCode,PKGNO_PTDJ,3)==0)
    {
      vLvBrnoDebug(__FILE__,__LINE__);
      ret=lv_deal_ptdjlz(&lv_pack,1);   /* chkflg=1 挂帐检查 */
      vLvBrnoDebug(__FILE__,__LINE__);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_DQDJ,3)==0)
    {
      ret=lv_deal_dqdjlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_SSDJ,3)==0)
    {
      ret=lv_deal_ssdjlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_DJTH,3)==0)
    {
      ret=lv_deal_djthlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_SSDJHZ,3)==0)
    {
      ret=lv_deal_ssdjhzlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_PTJJ,3)==0)
    {
      ret=lv_deal_ptjjlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_DQJJ,3)==0)
    {
      ret=lv_deal_dqjjlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_SSJJ,3)==0)
    {
      ret=lv_deal_ssjjlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_PTJJHZ,3)==0)
    {
      ret=lv_deal_ptjjhzlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_DQJJHZ,3)==0)
    {
      ret=lv_deal_dqjjhzlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_SSJJHZ,3)==0)
    {
      ret=lv_deal_ssjjhzlz(&lv_pack,1);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_SSXX,3)==0)
    {
      vtcp_log("%s,%d,============ PKGNO:[%.3s]\n",__FILE__,__LINE__, lv_pack.CMTCode);
      ret=lv_deal_ssxxlz(&lv_pack,0);
    }
    else if(memcmp(lv_pack.CMTCode,PKGNO_TYXX,3)==0)
    {
      vtcp_log("%s,%d,============ PKGNO:[%.3s]\n",__FILE__,__LINE__, lv_pack.CMTCode);
      ret=lv_deal_tyxxlz(&lv_pack,0);
    }
    else
    {
      vtcp_log("%s,%d,不能处理的 PKGNO:[%.3s]\n",__FILE__,__LINE__, lv_pack.CMTCode);
      lv_destroy(&lv_pack);    
      return -1;
    }
    
    
    if ( (memcmp(lv_pack.CMTCode,PKGNO_SSXX,3)==0) || 
      (memcmp(lv_pack.CMTCode,PKGNO_TYXX,3)==0) )
    {
      if ( (ret != 0)  &&  (ret != 100) )      
      {
        sprintf(acErrMsg,"  挂帐检查出错 [%d] ", ret);
        WRITEMSG  
        lv_destroy(&lv_pack);           
        return -1;    
      } 
      memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了就置 0000，SSXX TYXX 不用循环  */
      set_zd_data("0500","1");         /* 报文已处理, 不用挂帐检查 已登记 */ 
      lv_destroy(&lv_pack);
      return 0;
    }  
    
    if ( ret == 0 || ret == 100 )   
    {
      vtcp_log("[%s][%d] 挂帐检查结果：转 Jtst 记帐",__FILE__,__LINE__);  
      strcpy( g_pub_tx.reply, "0000" );  /* 转到 Jtst设置8583域走帐 !!! */
    }
    else  if( ret == 999 )  /* 挂帐, 转 2 */
    {
      vtcp_log("[%s][%d] 挂帐检查结果： 转 J099 登记 !!!",__FILE__,__LINE__);     
      strcpy( g_pub_tx.reply, "0010" );  /* 转 J099  登记 !!! */
    }
    else   /*   挂帐检查出错  */
    {
      sprintf(acErrMsg,"  挂帐检查出错 [%d] ", ret);
      WRITEMSG  
      lv_destroy(&lv_pack);             
      return -1;    
    }
    
    set_zd_data("0510",cmtno);         
    set_zd_data("0210","01");
    /* begin add by LiuHuafeng 2007-1-16 12:01 */
    {
        char cTst_fd68[2];
        memset(cTst_fd68, 0 , sizeof(cTst_fd68));
        get_zd_data("0680",cTst_fd68);
        pub_base_strpack(cTst_fd68);
        if(strlen(cTst_fd68)==0) /* 只有没有找到68域的情况下在设置 */
    /* end by LiuHuafeng 2007-1-16 12:04 */
            set_zd_data("0680","1");
    }
    lv_destroy(&lv_pack);            
    return 0;
      
  }
  else  /* 是CMT包  */
  {
    char cTel[7];
    char tx_br_no[7];    
    char cKinbr[BRNO_LEN+1];
    memset(cTel, 0 , sizeof(cTel));
    memset(tx_br_no, 0 , sizeof(tx_br_no));    
    memset(cKinbr,0,sizeof(cKinbr));
    vtcp_log("[%s][%d] 是CMT包 cmtcode=[%.3s]",__FILE__,__LINE__,lv_pack.CMTCode);
    memcpy(cmtno,lv_pack.CMTCode,3);   /** 得到报文编号 **/              
    vtcp_log("[%s][%d]lv_pack.CMTCode=[%-3s]",__FILE__,__LINE__,cmtno);

        if(memcmp(cmtno,"920",3)==0 || memcmp(cmtno,"911",3)==0 
          || memcmp(cmtno,"254",3)==0 || memcmp(cmtno,"552",3)==0
          || memcmp(cmtno,"554",3)==0)
        {
            if(iLvchkstat())
            {
                vtcp_log("%s,%d 这里判断是否可以接受来帐"__FILE__,__LINE__);
                strncpy(g_pub_tx.reply,"P046",4);
                WRITEMSG
                return 9999;
            }
        }
    if (pub_hv_check_cmtno(cmtno) != 0)  /** 检查报文编号是否合法 **/ 
    {
      sprintf(acErrMsg,"cmtno=[%s] -------cmtno 错误！", cmtno);
      MBFEWRITEMSG  
      lv_destroy(&lv_pack);            
      return -1;
    } 
    
    /* ==== 有些报文需要走帐，需要特殊处理 在此设置8583域  ====== */
    if(memcmp(cmtno,"254",3) == 0)
    {
        memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        set_zd_data("0030",cKinbr); 
        iGetReceTel(cKinbr);    /*  柜员号 ==> 0070 */
         
        set_zd_data("0070",cTel);
        set_zd_data("0510",cmtno);
        
        if (pubf_handle_CMT254(&lv_pack) != 0)  /** 254报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT254() 出错！", cmtno);
            MBFEWRITEMSG 
            lv_destroy(&lv_pack);                   
            return -1;
        }
        vtcp_log("[%s][%d] pubf_handle_CMT254() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);              
        return 0;
    }  
    
    if(memcmp(cmtno,"322",3) == 0)
    {

        set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT322(&lv_pack) != 0)  /** 322报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT322() 出错！", cmtno);
            MBFEWRITEMSG  
             lv_destroy(&lv_pack);                 
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT322() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }    
    
    if(memcmp(cmtno,"324",3) == 0)
    {
        set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT324(&lv_pack) != 0)  /** 324报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT324() 出错！", cmtno);
            MBFEWRITEMSG  
            lv_destroy(&lv_pack);                  
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT324() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }        
      
    if(memcmp(cmtno,"325",3) == 0)
    {
         set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT325(&lv_pack) != 0)  /** 325报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT325() 出错！", cmtno);
            MBFEWRITEMSG  
             lv_destroy(&lv_pack);                 
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT325() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }         
    if(memcmp(cmtno,"552",3) == 0)
    {
        memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        set_zd_data("0030",cKinbr); 
        iGetReceTel(cKinbr);    /*  柜员号 ==> 0070 */
 
         /*set_zd_data("0070",cTel);*/ 
         set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT552(&lv_pack) != 0)  /** 552报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT552() 出错！", cmtno);
            MBFEWRITEMSG  
            lv_destroy(&lv_pack);      
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT552() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }  
          
    if(memcmp(cmtno,"554",3) == 0)
    {
        memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        set_zd_data("0030",cKinbr); 
        iGetReceTel(cKinbr);    /*  柜员号 ==> 0070 */
        
        /*set_zd_data("0070",cTel);*/ 
         set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT554(&lv_pack) != 0)  /** 554报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT554() 出错！", cmtno);
            MBFEWRITEMSG  
            lv_destroy(&lv_pack);      
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT554() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }  
           
    if(memcmp(cmtno,"920",3) == 0)
    {
        memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        set_zd_data("0030",cKinbr); 
        iGetReceTel(cKinbr);    /*  柜员号 ==> 0070 */
         
         set_zd_data("0510",cmtno);   
        if (pubf_handle_CMT920(&lv_pack) != 0)  /** 920报文的8583设置 **/ 
        {
            sprintf(acErrMsg,"cmtno=[%s] pubf_handle_CMT920() 出错！", cmtno);
            MBFEWRITEMSG  
            lv_destroy(&lv_pack);      
            return -1;
        } 
        vtcp_log("[%s][%d] pubf_handle_CMT920() 完毕 !!! ",__FILE__,__LINE__);
        lv_destroy(&lv_pack);      
        return 0;
    }
    /* ====  需要特殊处理的报文处理完毕  ====== */   
     
    set_zd_long("0530",-1);            /* iStep== -1  表示记帐处理结束 */
    strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐的，转到J099 登记来帐*/
    set_zd_data("0510",cmtno);  
    vLvBrnoDebug(__FILE__,__LINE__);
  }
  lv_destroy(&lv_pack);
  return 0;
}

int pubf_handle_CMT254(TLVPACKAGE *plv_pack )
{
    char cCrdb[2];
    char cTemp[19];  
    char cTxamt[16]; 
    double dTxamt=0.0;    
    memset(cCrdb,0,sizeof(cCrdb));    
    memset(cTemp,0,sizeof(cTemp));
    memset(cTxamt,0,sizeof(cTxamt));

    lv_get_tag(plv_pack,"90C",cCrdb);
    lv_get_tag(plv_pack,"32A",cTemp);
    memcpy(cTxamt,cTemp+3,sizeof(cTxamt)-1);
    dTxamt = atof(cTxamt);

    vtcp_log("cCrdb=[%s]  cTxamt=[%s]  dTxamt=[%lf]",__FILE__,__LINE__,cCrdb,cTxamt,dTxamt);
    /* 在这里设置254帐务处理的借贷标志 */
    /* 0720 指定为41403的借贷标志  0660 为110的借贷标志 */
    if(cCrdb[0]=='1')
    {
      set_zd_data("0660","1");
      set_zd_data("0720","2");
    }
    else
    {
      set_zd_data("0660","2");
      set_zd_data("0720","1");
    }

    set_zd_data("0680","6");    /* 7 为现金 */
    dTxamt=dTxamt/100.0;
    vtcp_log(" dTxamt=[%lf]",__FILE__,__LINE__,dTxamt);    
    set_zd_double("0400",dTxamt);

    set_zd_data("0210","01");    
    set_zd_data("0320", LVLZGD ); /* 设置过渡科目  LVLZGD */      
    set_zd_double("0390",0.0);
  
    vtcp_log("[%s][%d] 254报文交易金额是[%f] \n",__FILE__,__LINE__,dTxamt);
    strcpy( g_pub_tx.reply, "0000" );  /* 需要走帐，转到Jtst */      
    return 0;  
}

int pubf_handle_CMT322(TLVPACKAGE *plv_pack )
{
    long iStep=0;  
    char cTmpStr[9];
    char cOpackid[9];
    char cOpackday[9];
    char cRespcode[2];

    char cOpackstat[3];
        
    int ret,iRc;
    struct lv_pkgreg_c sLv_pkgreg;
    struct lv_wbctl_c  sLv_wbctl;
    char cAc_no [20];
    char cOpkgno [4];  
    
    memset(cTmpStr,0,sizeof(cTmpStr));
    memset(cOpackid,0,sizeof(cOpackid));    
    memset(cOpackday,0,sizeof(cOpackday));        
    memset(cRespcode,0,sizeof(cRespcode));
    memset(cOpkgno,0,sizeof(cOpkgno));    
    memset(cOpackstat,0,sizeof(cOpackstat));  
    
    GETTAG(plv_pack,"0BE",cTmpStr);
    lv_orderno_add0(cTmpStr, sizeof(cOpackid)-1,cOpackid);
    vtcp_log("[%s][%d] cOpackid==[%s] ",__FILE__,__LINE__,cOpackid);
      
    GETTAG_SIZE(plv_pack,"30I",cOpackday,8);    
    GETTAG_SIZE(plv_pack,"04C",cRespcode,1);  /* 撤销应答 */
    GETTAG_SIZE(plv_pack,"02D",cOpkgno,3);
    GETTAG_SIZE(plv_pack,"CIB",cOpackstat,2);  /* 原包处理状态 */
      
        
    vtcp_log("[%s][%d] cOpackday==[%s] ",__FILE__,__LINE__,cOpackday);
    vtcp_log("[%s][%d] cRespcode==[%s] ",__FILE__,__LINE__,cRespcode);
    vtcp_log("[%s][%d] cOpkgno==[%s] ",__FILE__,__LINE__,cOpkgno);
    vtcp_log("[%s][%d] cOpackstat==[%s] ",__FILE__,__LINE__,cOpackstat);    
    
     /* 原包处理状态为99－无此业务 不走帐    撤销应答：1 已撤销  2 不能撤销。 */
    if( (memcmp(cOpackstat,"99",2)== 0) || (cRespcode[0] == '2' ) ) 
    {
       vtcp_log("[%s][%d] cOpackstat==[%s] cRespcode==[%s] ",__FILE__,__LINE__,cOpackstat,cRespcode);    
       set_zd_long("0530",-1); /* iStep== -1  表示记帐处理结束 */
       strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */       
       return 0;      
    }  
    
    /*  检查原来的撤消申请及其状态 */  

    
    /*  找到原往包记录  */      
    memset(&sLv_wbctl , 0  ,sizeof(sLv_wbctl ));    
    ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLv_wbctl,"pack_date = %d and packid = '%s' ", apatoi(cOpackday,8), cOpackid);
    if(ret == 100)
     {
       sprintf(acErrMsg,"没有找到原往包记录 !!!! - ret==[%d] ", ret);
       WRITEMSG
       return(-1);
    }else if(ret)
     {
       sprintf(acErrMsg,"数据库错误 - ret==[%d]", ret);
       WRITEMSG
       return(-1);
    }

    /*  找到原往帐记录  */  
    vtcp_log("sLv_wbctl.pay_qs_no=[%s] pack_date=[%d] packid=[%s]",sLv_wbctl.pay_qs_no,sLv_wbctl.pack_date,sLv_wbctl.packid);
    iRc = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pay_qs_no='%s' and pack_date=%d and packid='%s' and lw_ind='1'  order by rowid ",
          sLv_wbctl.pay_qs_no, sLv_wbctl.pack_date, sLv_wbctl.packid);
    if(iRc)
    {
      vtcp_log("%s,%d,游标声明出错[%d]\n",__FILE__,__LINE__,iRc);
      strcpy(acErrMsg,"游标声明出错!!");WRITEMSG
      return(-1);
    }
    vtcp_log("%s,%d,=====  Lv_pkgreg_Dec_Sel() 成功!!! ===== [%d]\n",__FILE__,__LINE__,iRc);

    get_zd_long("0530",&iStep);  
    vtcp_log("%s,%d,=====  iStep ===== [%d]\n",__FILE__,__LINE__,iStep);

    int iTimes=0;
    while(1)
    {
        memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
        
        vtcp_log("%s,%d,=====  Before Lv_pkgreg_Fet_Sel() !!! ===== ",__FILE__,__LINE__);
        iRc = Lv_pkgreg_Fet_Sel( &sLv_pkgreg, g_pub_tx.reply );
        if(iRc == 100)
        {  
            if(iTimes == 0)
            {
                vtcp_log("[%s,%d], ====  一条记录也没有, 肯定不对 !!!!  =====",__FILE__,__LINE__);  
                strcpy(acErrMsg,"没有找到原实时贷记往帐记录");WRITEMSG
                return(-1);        
            }
            
            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }else  if(iRc)
        {
            vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,iRc);
            strcpy(acErrMsg,"数据库错误");WRITEMSG
            return(-1);
        }
        vtcp_log("%s,%d,=====  Lv_pkgreg_Fet_Sel() 取到一条记录 !!! ===== ",__FILE__,__LINE__);
        vtcp_log("%s,%d,=====  iStep ===== [%d]    iTimes ===== [%d] ",__FILE__,__LINE__,iStep,iTimes);

        if(iTimes==iStep)
        {
            /* 找到要处理的记录，进行挂帐检查，为走帐设置8583域 */
            memset(cAc_no, 0 ,sizeof(cAc_no));            
            strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1); 
            vtcp_log("[%s][%d]  sLv_pkgreg.br_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.br_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_ac_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.pay_ac_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_name=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.pay_name);

            /* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
              pubf_lv_GetBr_noTel(sLv_pkgreg.pay_ac_no);

            pub_base_old_acno(cAc_no);            
            if(cAc_no[0]=='1') set_zd_data("0680","1");
            else set_zd_data("0680","6");
            vtcp_log("[%s][%d]  cAc_no=====[%s] ",__FILE__,__LINE__,cAc_no);
            
            /* 挂帐检查 */
            if(iLvHang_Check_Acno(cAc_no, sLv_pkgreg.pay_name,sLv_pkgreg.br_no))
            {
                vtcp_log("%s,%d, pubf_lv_hang_chk_ac() 出错 ",__FILE__,__LINE__);
                strcpy(acErrMsg,"pubf_lv_hang_chk_ac() 出错 ");WRITEMSG
                return(-1);  
            }
                    
            /* 借客户       贷4110100                金额为原来交易金额的负值*/
            /* 借 4110100   贷414小额待清算过渡科目  金额为原来交易金额的负值*/          
            /*set_zd_data("0300",cAc_no); */      
            set_zd_data("0660","1");   /* 借客户 */
            set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
            set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
            set_zd_data("0720","2");       /* 贷 414等待清算科目  */                      
            set_zd_double("0400",sLv_pkgreg.tx_amt * -1);
            set_zd_double("0390",sLv_pkgreg.tx_amt * -1);            
            set_zd_data("0210", "01");     /* 币种 */

            set_zd_long("0530",iStep+1);
            break;
        }
        else if(iTimes < iStep)
        {
            iTimes++;
        }
        else
        {
            strcpy(acErrMsg,"没有找到原实时贷记往帐记录");
            WRITEMSG
            return(-1);  
        }
    }
    
    strcpy( g_pub_tx.reply, "0020" );  /* 需要走帐，不用走Jtst，直接转到XEJZ*/  
    return 0;  
}

int pubf_handle_CMT324(TLVPACKAGE *plv_pack )
{
    long iStep=0;  
    char cTmpStr[9];
    char cOpackid[9];
    char cOpackday[9];
    char cRespcode[2];
    int ret,iRc;
    struct lv_pkgreg_c sLv_pkgreg;
    struct lv_wbctl_c  sLv_wbctl;
    char cAc_no [20];
    char cOpkgno [4];  
    
    memset(cTmpStr,0,sizeof(cTmpStr));
    memset(cOpackid,0,sizeof(cOpackid));    
    memset(cOpackday,0,sizeof(cOpackday));        
    memset(cRespcode,0,sizeof(cRespcode));
    memset(cOpkgno,0,sizeof(cOpkgno));    

    GETTAG(plv_pack,"0BE",cTmpStr);
    lv_orderno_add0(cTmpStr, sizeof(cOpackid)-1,cOpackid);
    vtcp_log("[%s][%d] cOpackid==[%s] ",__FILE__,__LINE__,cOpackid);
      
    GETTAG_SIZE(plv_pack,"30I",cOpackday,8);    
    GETTAG_SIZE(plv_pack,"BS9",cRespcode,1);  
    GETTAG_SIZE(plv_pack,"02D",cOpkgno,3);
        
    vtcp_log("[%s][%d] cOpackday==[%s] ",__FILE__,__LINE__,cOpackday);
    vtcp_log("[%s][%d] cRespcode==[%s] ",__FILE__,__LINE__,cRespcode);
    vtcp_log("[%s][%d] cOpkgno==[%s] ",__FILE__,__LINE__,cOpkgno);
    
     /* 应答不成功或不是实时贷记往帐，不走帐 */
    if( (cRespcode[0] != '1') || memcmp(cOpkgno,"003",3) ) 
    {
       set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
       strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */       
       return 0;      
    }
          
    /*  找到原往包记录  */      
    memset(&sLv_wbctl , 0  ,sizeof(sLv_wbctl ));    
    ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLv_wbctl,"pack_date = %d and packid = '%s' ", apatoi(cOpackday,8), cOpackid);
    if(ret == 100)
     {
       sprintf(acErrMsg,"没有找到原往包记录 !!!! - ret==[%d] ", ret);
       WRITEMSG
       return(-1);
    }else if(ret)
     {
       sprintf(acErrMsg,"数据库错误 - ret==[%d]", ret);
       WRITEMSG
       return(-1);
    }

    /*  需不需要检查原来的冲正申请 ？？ */  
    
    

    /*  找到原实时贷记往帐记录  */  
    vtcp_log("sLv_wbctl.pay_qs_no=[%s] pack_date=[%d] packid=[%s]",sLv_wbctl.pay_qs_no,sLv_wbctl.pack_date,sLv_wbctl.packid);
    iRc = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pay_qs_no='%s' and pack_date=%d and packid='%s' and lw_ind='1' and pkgno='003' order by rowid ",
          sLv_wbctl.pay_qs_no, sLv_wbctl.pack_date, sLv_wbctl.packid);
    if(iRc)
    {
      vtcp_log("%s,%d,游标声明出错[%d]\n",__FILE__,__LINE__,iRc);
      strcpy(acErrMsg,"游标声明出错!!");WRITEMSG
      return(-1);
    }
    vtcp_log("%s,%d,=====  Lv_pkgreg_Dec_Sel() 成功!!! ===== [%d]\n",__FILE__,__LINE__,iRc);
    get_zd_long("0530",&iStep);  
    vtcp_log("%s,%d,=====  iStep ===== [%d]\n",__FILE__,__LINE__,iStep);


    int iTimes=0;
    while(1)
    {
        memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
        
        vtcp_log("%s,%d,=====  Before Lv_pkgreg_Fet_Sel() !!! ===== ",__FILE__,__LINE__);
        iRc = Lv_pkgreg_Fet_Sel( &sLv_pkgreg, g_pub_tx.reply );
        if(iRc == 100)
        {  
            if(iTimes == 0)
            {
                vtcp_log("[%s,%d], ====  一条记录也没有, 肯定不对 !!!!  =====",__FILE__,__LINE__);  
                strcpy(acErrMsg,"没有找到原实时贷记往帐记录");WRITEMSG
                return(-1);        
            }
            
            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }else  if(iRc)
        {
            vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,iRc);
            strcpy(acErrMsg,"数据库错误");WRITEMSG
            return(-1);
        }
        vtcp_log("%s,%d,=====  Lv_pkgreg_Fet_Sel() 取到一条记录 !!! ===== ",__FILE__,__LINE__);
        vtcp_log("%s,%d,=====  iStep ===== [%d]    iTimes ===== [%d] ",__FILE__,__LINE__,iStep,iTimes);

        if(iTimes==iStep)
        {
            /* 找到要处理的记录，进行挂帐检查，为走帐设置8583域 */
            memset(cAc_no, 0 ,sizeof(cAc_no));            
            strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1); 
            vtcp_log("[%s][%d]  sLv_pkgreg.br_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.br_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_ac_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.pay_ac_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_name=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.pay_name);

            pub_base_old_acno(cAc_no);            
            if(cAc_no[0]=='1') set_zd_data("0680","1");
            else set_zd_data("0680","6");
            vtcp_log("[%s][%d]  cAc_no=====[%s] ",__FILE__,__LINE__,cAc_no);
            
            /* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
              pubf_lv_GetBr_noTel(sLv_pkgreg.pay_ac_no);
              
            /* 挂帐检查 */
            if(iLvHang_Check_Acno(cAc_no, sLv_pkgreg.pay_name,sLv_pkgreg.br_no))
            {
                vtcp_log("%s,%d, pubf_lv_hang_chk_ac() 出错 ",__FILE__,__LINE__);
                strcpy(acErrMsg,"pubf_lv_hang_chk_ac() 出错 ");WRITEMSG
                return(-1);  
            }
                    
            /* 借客户       贷4110100                金额为原来交易金额的负值*/
            /* 借 4110100   贷414小额待清算过渡科目  金额为原来交易金额的负值*/          
            /*set_zd_data("0300",cAc_no); */   
            set_zd_data("0660","1");   /* 借客户 */
            /* begin NEWTCTD2 */
            /* rem by LiuHuafeng 2007-9-2 23:24:09 
            set_zd_data("0320",LVLZGD);* 设置过渡科目 *
             * end by LiuHuafeng replace by next line ***/
            set_zd_data("0320",LV_SUB02_SEND);/* 设置往帐过渡科目 */
            set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
            set_zd_data("0720","2");       /* 贷 414等待清算科目  */                      
            set_zd_double("0400",sLv_pkgreg.tx_amt * -1);
            set_zd_double("0390",sLv_pkgreg.tx_amt * -1);            
            set_zd_data("0210", "01");     /* 币种 */

            set_zd_long("0530",iStep+1);
            break;
        }
        else if(iTimes < iStep)
        {
            iTimes++;
        }
        else
        {
            strcpy(acErrMsg,"没有找到原实时贷记往帐记录");
            WRITEMSG
            return(-1);  
        }
    }
    
    strcpy( g_pub_tx.reply, "0020" );  /* 需要走帐，不用走Jtst，直接转到XEJZ*/  
    return 0;  
}

int pubf_handle_CMT325(TLVPACKAGE *plv_pack )
{
    long iStep=0;  
    char cTmpStr[9];
    char cOpackid[9];
    char cOpackday[9];
    int ret,iRc;
    struct lv_pkgreg_c sLv_pkgreg;
    struct lv_wbctl_c  sLv_wbctl;
    struct lv_lbctl_c  sLv_lbctl;    
    char cAc_no [20];
    char cOpkgno [4];  
    
    memset(cTmpStr,0,sizeof(cTmpStr));
    memset(cOpackid,0,sizeof(cOpackid));    
    memset(cOpackday,0,sizeof(cOpackday));        
    memset(cOpkgno,0,sizeof(cOpkgno));    

    GETTAG(plv_pack,"0BE",cTmpStr);
    lv_orderno_add0(cTmpStr, sizeof(cOpackid)-1,cOpackid);
    vtcp_log("[%s][%d] cOpackid==[%s] ",__FILE__,__LINE__,cOpackid);
      
    GETTAG_SIZE(plv_pack,"30I",cOpackday,8);    
    GETTAG_SIZE(plv_pack,"02D",cOpkgno,3);
        
    vtcp_log("[%s][%d] cOpackday==[%s] ",__FILE__,__LINE__,cOpackday);
    vtcp_log("[%s][%d] cOpkgno==[%s] ",__FILE__,__LINE__,cOpkgno);
    
     /* 不是实时借记来帐，不走帐 为什么实时贷记就不行呢？WSMN,如果ssdj不走账的话，那么下边为什么还要判断，为什么还要把收款人赋过去？？？*/
    if( memcmp(cOpkgno,"004",3) ) 
    {
       set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
       strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */       
       return 0; 
    }
          
    /*  找到原往包记录  */      
    memset(&sLv_wbctl , 0  ,sizeof(sLv_wbctl ));    
    /* rem by NEWTCTD2 */
    /* rem by LiuHuafeng 2007-9-3 3:12:04    
    ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLv_wbctl,"pack_date = %d and packid = '%s' ", apatoi(cOpackday,8), cOpackid);
    if(ret == 100)
     {
       sprintf(acErrMsg,"没有找到原往包记录 !!!! - ret==[%d] ", ret);
       WRITEMSG
       return(-1);
    }else if(ret)
     {
       sprintf(acErrMsg,"数据库错误 - ret==[%d]", ret);
       WRITEMSG
       return(-1);
    }
    ************************************/
    /* end by NEWTCTD2 */

    /*  需不需要检查原来的冲正申请或实时借记来帐 ？？ */  

    /*  找到原实时借记来帐记录  */  
    vtcp_log("sLv_wbctl.pay_qs_no=[%s] pack_date=[%d] packid=[%s]",sLv_wbctl.pay_qs_no,sLv_wbctl.pack_date,sLv_wbctl.packid);
    /* begin NEWTCTD2 */
    /* rem by LiuHuafeng 2007-9-3 3:06:36 
    iRc = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pay_qs_no='%s' and pack_date=%d and packid='%s' and lw_ind='2' and pkgno='004' order by rowid ",
          sLv_wbctl.pay_qs_no, sLv_wbctl.pack_date, sLv_wbctl.packid);
    ***** replace by next line  ****/
        memset(&sLv_lbctl, 0 , sizeof(sLv_lbctl));
    ret = Lv_lbctl_Sel(g_pub_tx.reply, &sLv_lbctl,"pack_date = %d and packid = '%s' ", apatoi(cOpackday,8), cOpackid);
    if(ret == 100)
     {
       /******************delete by xyy YHBP 如果我没有收到需要冲正这笔业务，那么325来了以后就不收了么？？
       sprintf(acErrMsg,"没有找到原往包记录 !!!! - ret==[%d] ", ret);
       WRITEMSG
       return(-1);
       ************add by xyy for YHBP 这里改成只接收下325**************/
       vtcp_log(" [%s,%d],====  跳出循环接收325报文!!!!  =====",__FILE__,__LINE__);  
       set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
       strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐,转到J099 */            
       set_zd_long("0500",0);
       return 0;
    }else if(ret)
     {
       sprintf(acErrMsg,"数据库错误 - ret==[%d]", ret);
       WRITEMSG
       return(-1);
    }        
    iRc = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pay_qs_no='%s' and pack_date=%d and packid='%s' and lw_ind='2' and pkgno='%s' order by rowid ",
          sLv_lbctl.pay_qs_no, sLv_lbctl.pack_date, sLv_lbctl.packid,cOpkgno);
    if(iRc)
    {
      vtcp_log("%s,%d,游标声明出错[%d]\n",__FILE__,__LINE__,iRc);
      strcpy(acErrMsg,"游标声明出错!!");WRITEMSG
      return(-1);
    }
        /* end NEWTCTD2 */

    vtcp_log("%s,%d,=====  Lv_pkgreg_Dec_Sel() 成功!!! ===== [%d]\n",__FILE__,__LINE__,iRc);
    get_zd_long("0530",&iStep);  
    vtcp_log("%s,%d,=====  iStep ===== [%d]\n",__FILE__,__LINE__,iStep);

    int iTimes=0;
    while(1)
    {
        memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
        vtcp_log("%s,%d,=====  Before Lv_pkgreg_Fet_Sel() !!! ===== ",__FILE__,__LINE__);
        iRc = Lv_pkgreg_Fet_Sel( &sLv_pkgreg, g_pub_tx.reply );
        if(iRc == 100)
        {
            if(iTimes == 0)
            {
                vtcp_log("[%s,%d], ====  一条记录也没有, 肯定不对 !!!!  =====",__FILE__,__LINE__);  
                strcpy(acErrMsg,"没有找到原实时贷记往帐记录");WRITEMSG
                return(-1);        
            }

            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }else  if(iRc)
        {
            vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,iRc);
            strcpy(acErrMsg,"数据库错误");WRITEMSG
            return(-1);
        }
        vtcp_log("%s,%d,=====  Lv_pkgreg_Fet_Sel() 取到一条记录 !!! ===== ",__FILE__,__LINE__);
        vtcp_log("%s,%d,=====  iStep ===== [%d]    iTimes ===== [%d] ",__FILE__,__LINE__,iStep,iTimes);

       /* begin NEWTCTD2 */
        if(memcmp(sLv_pkgreg.rcpstat,"00",2))
        {
            vtcp_log("%s,%d 应答状态为非00 ＝[%s] 不需要记帐处理",__FILE__,__LINE__,sLv_pkgreg.rcpstat);
            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }
        if(sLv_pkgreg.lv_sts[0]!=STAT_LZRECV2 && memcmp(cOpkgno,"003",3)==0)
        {
            vtcp_log("%s,%d 实时贷记来帐不是正常接收状态 ＝[%c] 不需要记帐处理",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }
        if(sLv_pkgreg.lv_sts[0] != STAT_YES_NOTSEND && sLv_pkgreg.lv_sts[0] != STAT_YES_SEND && memcmp(cOpkgno,"004",3)==0)/**modify by NEWBP1**/
        {
            vtcp_log("%s,%d 实时借记来帐不是临时接收状态 ＝[%c] 不需要记帐处理",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
            vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);  
            set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */
            strcpy( g_pub_tx.reply, "0010" );  /* 不需要走帐，转到J099 */            
            set_zd_long("0500",0);
            return 0;
        }
        /* end NEWTCTD2 */
        
        if(iTimes==iStep)
        {
            /* 找到要处理的记录，进行挂帐检查，为走帐设置8583域 */
            memset(cAc_no, 0 ,sizeof(cAc_no));            
            strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1); /********modify by NEWBP2*********/
            vtcp_log("[%s][%d]  sLv_pkgreg.br_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.br_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_ac_no=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.cash_ac_no);
            vtcp_log("[%s][%d]  sLv_pkgreg.pay_name=====[%s] ",__FILE__,__LINE__,sLv_pkgreg.cash_name);
            
            /****add by NEWBP2***/
            if(memcmp(sLv_pkgreg.pkgno,"004",sizeof(sLv_pkgreg.pkgno)-1)==0 && memcmp(sLv_pkgreg.txnum,"30103",sizeof(sLv_pkgreg.txnum)-1)==0 )
            {
                  int  ret=0;
                struct  cashier_c      wd_cashier;
                struct  com_parm_c     com_parm;
                struct  mdm_ac_rel_c   mdm_ac_rel;
                struct  in_mst_c       in_mst;
                struct  lv_addit_c     sLvAddit;
                
                char cBuff[2001];
                char cKinbr[BRNO_LEN+1];
                TYJL1_ADD_AREA lv_tyjl1;
                
                vtcp_log("[%s][%d] 实时借记的通用截留业务付款帐户应为[0]\n",__FILE__,__LINE__);
                if(strcmp(sLv_pkgreg.pay_ac_no,"0"))
                {
                    vtcp_log("[%s][%d] 实时借记的通用截留业务付款帐户应为[0][%s]\n",__FILE__,__LINE__,sLv_pkgreg.pay_ac_no);
                    strcpy(acErrMsg,"实时借记的通用截留业务付款帐户应为0");
                    WRITEMSG
                    return(-1);  
                }
                                 
                memset(cBuff      , 0 , sizeof(cBuff));
                memset(cKinbr     , 0 , sizeof(cKinbr));
                memset(&lv_tyjl1  , 0 , sizeof(lv_tyjl1));
                memset(&wd_cashier, 0 , sizeof(wd_cashier));
                memset(&com_parm    , 0 , sizeof(com_parm));
                memset(&in_mst      , 0 , sizeof(in_mst));
                memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));
                memset(&sLvAddit    , 0 , sizeof(struct lv_addit_c));
                
                vtcp_log("[%s][%d]in_date=[%d]\n",__FILE__,__LINE__,sLv_pkgreg.in_date);
                vtcp_log("[%s][%d]addid=[%d]\n",__FILE__,__LINE__,sLv_pkgreg.addid);
                ret = Lv_addit_Sel(g_pub_tx.reply, &sLvAddit,"tx_date = '%d' and addid = '%s' and tagname= '001' ",sLv_pkgreg.in_date,sLv_pkgreg.addid);
                if ( ret == 100 )
                {
                    sprintf( acErrMsg, "原查询记录不存在!" );
                    WRITEMSG
                    return -1;
                }else if( ret )
                {
                    sprintf( acErrMsg, "执行Lv_wbctl_Sel错![%d]", ret );
                    WRITEMSG
                    return -1;
                }
                memcpy((char *)&lv_tyjl1,sLvAddit.tagdata, sizeof(lv_tyjl1));
                vtcp_log("[%s][%d] 写入附加域文件1=[%s]",__FL__,(char *)&lv_tyjl1);
                
                vtcp_log("%s,%d 附加域信息处理完成拉\n",__FILE__,__LINE__);
                memcpy(wd_cashier.pono   ,lv_tyjl1.cpno ,sizeof(wd_cashier.pono)-1);
                wd_cashier.signday = apatoi(lv_tyjl1.cpday,8);
                vtcp_log("%s,%d pono=[%s]\n",__FILE__,__LINE__,wd_cashier.pono);
                vtcp_log("%s,%d signday=[%d]\n",__FILE__,__LINE__,wd_cashier.signday);
                ret = Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld'",\
                                      wd_cashier.pono,wd_cashier.signday);
                if (ret)
                {
                    vtcp_log("[%s][%d] 查询本票信息错误[%d]\n",__FILE__,__LINE__,ret);
                    strcpy(acErrMsg,"查询本票信息错误");
                    WRITEMSG
                    return(-1);  
                }
                vtcp_log("[%s][%d]准备FETCH 数据啦[%d]\n",__FILE__,__LINE__,ret);
                ret = Cashier_Fet_Upd(&wd_cashier ,g_pub_tx.reply);
                if (ret)
                {
                    vtcp_log("[%s][%d] 查询本票信息错误[%d]\n",__FILE__,__LINE__,ret);
                    strcpy(acErrMsg,"查询本票信息错误");
                    WRITEMSG
                    return(-1);  
                }
                vtcp_log("[%s][%d]票据状态为[%c]\n",__FILE__,__LINE__,wd_cashier.stat[0]);
                /**检查票据状态**/
                if(wd_cashier.stat[0]!='2' && wd_cashier.stat[0]!='3')
                {
                    vtcp_log("[%s][%d] 查询本票的状态错误[%c]\n",__FILE__,__LINE__,wd_cashier.stat[0]);
                    strcpy(acErrMsg,"查询本票的状态错误");
                    WRITEMSG
                    return(-1);  
                }
                wd_cashier.lactcls[0] = wd_cashier.stat[0];
                wd_cashier.stat[0] = '1' ;
                wd_cashier.holdactno[0] ='\0';
                wd_cashier.holdname[0] ='\0';
                wd_cashier.holdbrno[0] ='\0';
                wd_cashier.holdbrname[0] ='\0';
                wd_cashier.filler[0] ='\0';
                memcpy(wd_cashier.filler,"此笔兑付请求已被冲正",20);
                ret = Cashier_Upd_Upd(wd_cashier,g_pub_tx.reply);
                if (ret)
                {
                    vtcp_log("[%s][%d] 跟新本票信息错误[%d]\n",__FILE__,__LINE__,ret);
                    strcpy(acErrMsg,"跟新本票信息错误");
                    WRITEMSG
                    return(-1);  
                }
                memcpy(cKinbr,wd_cashier.brno,BRNO_LEN);
                Cashier_Clo_Upd();
                
                vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
                ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",cKinbr);
                if(ret)
                {
                    vtcp_log("[%s][%d] 查找公共关系表错误[%d]\n",__FILE__,__LINE__,ret);
                    strcpy(acErrMsg,"查找公共关系表错误");
                    WRITEMSG
                    return(-1);  
                }
                /** 检查本票签发户是否存在 **/
                ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
                if(ret)
                {
                    vtcp_log("[%s][%d] 该帐户不存在[%d][%s]\n",__FILE__,__LINE__,ret,com_parm.val);
                    strcpy(acErrMsg,"该帐户不存在");
                    WRITEMSG
                    return(-1); 
                }
                ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
                if(ret)
                {
                    vtcp_log("[%s][%d] 该帐户不存在[%d][%s]\n",__FILE__,__LINE__,ret,mdm_ac_rel.ac_id);
                    strcpy(acErrMsg,"该帐户不存在");
                    WRITEMSG
                    return(-1); 
                }
                vtcp_log("[%s][%d]xyy付款帐户状态为[%c]\n",__FILE__,__LINE__,in_mst.sts[0]);
                if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
                {
                    vtcp_log("[%s][%d] 该帐户状态不正常[%d][%s]\n",__FILE__,__LINE__,ret,mdm_ac_rel.ac_id);
                    strcpy(acErrMsg,"该帐户状态不正常");
                    WRITEMSG
                    return(-1); 
                }
                vtcp_log("[%s][%d]xyy付款帐户为[%s]\n",__FILE__,__LINE__, com_parm.val);
                memcpy(cAc_no,com_parm.val,sizeof(cAc_no)-1);
                /**内部帐户不检查户名，需要特殊处理NEWBP2**/
            }
            /*****end by NEWBP2****/
            pub_base_old_acno(cAc_no);
            if(cAc_no[0]=='1')
            {
                set_zd_data("0680","1");
            }
            else
            {
                  set_zd_data("0680","6");
            } 
            vtcp_log("[%s][%d]  cAc_no=====[%s] ",__FILE__,__LINE__,cAc_no);

            /* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
            pubf_lv_GetBr_noTel(sLv_pkgreg.pay_ac_no);

            /* 挂帐检查 */
            if(iLvHang_Check_Acno(cAc_no, sLv_pkgreg.pay_name,sLv_pkgreg.br_no))/**modify by NEWBP2**/
            {
                vtcp_log("%s,%d, pubf_lv_hang_chk_ac() 出错 ",__FILE__,__LINE__);
                strcpy(acErrMsg,"pubf_lv_hang_chk_ac() 出错 ");WRITEMSG
                return(-1);  
            }
                    
            /* 借客户       贷4110100                金额为原来交易金额的负值*/
            /* 借 4110100   贷414小额待清算过渡科目  金额为原来交易金额的负值*/          
            /*set_zd_data("0300",cAc_no); */ 
            /* begin NEWTCTD2 */
            if(memcmp(cOpkgno,PKGNO_SSDJ,sizeof(cOpkgno)-1)==0)
            {
                  set_zd_data("0660","2");   /* 借客户 */
                  set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
                  set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
                  set_zd_data("0720","1");       /* 贷 414等待清算科目  */                      
            }
            else
            {
                /* end NEWTCTD2 */  
                set_zd_data("0660","1");   /* 借客户 */
                set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
                set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
                set_zd_data("0720","2");       /* 贷 414等待清算科目  */     
             }                 
            set_zd_double("0400",sLv_pkgreg.tx_amt * -1);
            set_zd_double("0390",sLv_pkgreg.tx_amt * -1);            
            set_zd_data("0210", "01");     /* 币种 */

            set_zd_long("0530",iStep+1);
            break;
        }
        else if(iTimes < iStep)
        {
            iTimes++;
        }
        else
        {
            strcpy(acErrMsg,"没有找到原实时贷记往帐记录");
            WRITEMSG
            return(-1);  
        }
    }

    strcpy( g_pub_tx.reply, "0020" );  /* 需要走帐，不用走Jtst，直接转到XEJZ*/  
    return 0;  
}

int pubf_handle_CMT552(TLVPACKAGE *plv_pack )
{
    char cTxamt[16];
    double dTxamt=0.0;   
    char cAcno110[20];
    int iRet;
       
    memset(cAcno110, 0 , sizeof(cAcno110));
    memset(cTxamt,0,sizeof(cTxamt));

    lv_get_tag(plv_pack,"B25",cTxamt);
    vtcp_log("[%s][%d] cTxamt====[%s] \n",__FILE__,__LINE__,cTxamt);    
    dTxamt = atof(cTxamt);    
    vtcp_log("[%s][%d] 552报文 被计费行总费用==[%f] \n",__FILE__,__LINE__,dTxamt);
    
    /* 找清算中心的110账号 */
    iRet=pub_base_GetParm_Str(LV_CZ_QSBRNO,PARM_HVQS,cAcno110);
    if(iRet)
    {
      vtcp_log("%s,%d 没有配置机构%s的110帐号",__FL__,LV_CZ_QSBRNO);
      strncpy(g_pub_tx.reply,"P076",4);
      return -1;
    }
    
    set_zd_data("0300",LV_SUB02_HAND_PAYOUT); /* 手续费支出 */    
    set_zd_data("0330",cAcno110);  /* 清算中心的110账号 */
            
    set_zd_data("0660","1");   /* 借：手续费支出 */
    set_zd_data("0720","2");   /* 贷：110（清算中心的110账号）*/

    set_zd_data("0680","6");    /* 7 为现金 */
    dTxamt=dTxamt/100.0;
    vtcp_log(" dTxamt=[%lf]",__FILE__,__LINE__,dTxamt);    
    set_zd_double("0400",dTxamt);
    
    set_zd_data("0320", LVLZGD );   /* 设置过渡科目  LVLZGD */      
    set_zd_double("0390",0.0);  /* 552不走过渡科目  */  
    
    set_zd_data("0210","01");    
    set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */    
    strcpy( g_pub_tx.reply, "0020" );  /* 需要走帐，不用走Jtst，直接转到XEJZ*/    
  return 0;  
}

int pubf_handle_CMT554(TLVPACKAGE *plv_pack )
{
    char cTxamt[16];
    double dTxamt=0.0;   
    char cAcno110[20];
    int iRet;
       
    memset(cAcno110, 0 , sizeof(cAcno110));
    memset(cTxamt,0,sizeof(cTxamt));

    lv_get_tag(plv_pack,"B2E",cTxamt);
    vtcp_log("[%s][%d] cTxamt====[%s] \n",__FILE__,__LINE__,cTxamt);    
    dTxamt = atof(cTxamt);    
    vtcp_log("[%s][%d]  554报文 返还总金额==[%f] \n",__FILE__,__LINE__,dTxamt);
    
    /* 找清算中心的110账号 */
    iRet=pub_base_GetParm_Str(LV_CZ_QSBRNO,PARM_HVQS,cAcno110);
    if(iRet)
    {
      vtcp_log("%s,%d 没有配置机构%s的110帐号",__FL__,LV_CZ_QSBRNO);
      strncpy(g_pub_tx.reply,"P076",4);
      return -1;
    }
  
    set_zd_data("0300",LV_SUB02_HAND_INCOME); /* 手续费收入 */        
    set_zd_data("0330",cAcno110);  /* 清算中心的110账号 */    

    set_zd_data("0660","2");   /* 贷：手续费收入 */
    set_zd_data("0720","1");   /* 借：110（清算中心的110账号）*/

    set_zd_data("0680","6");    /* 7 为现金 */
    dTxamt=dTxamt/100.0;
    vtcp_log(" dTxamt=[%lf]",__FILE__,__LINE__,dTxamt);    
    set_zd_double("0400",dTxamt);
    
    set_zd_data("0320", LVLZGD );   /* 设置过渡科目  LVLZGD */      
    set_zd_double("0390",0.0);  /* 552不走过渡科目  */  

    set_zd_data("0210","01");    
    set_zd_long("0530",-1); /* iStep==-1表示记帐处理结束 */    
    strcpy( g_pub_tx.reply, "0020" );  /* 需要走帐，不用走Jtst，直接转到XEJZ*/  
    return 0;
}

int pubf_handle_CMT920(TLVPACKAGE *plv_pack )
{
    char cRespCode[9];
    char cOpackday[9];  
    char cTmpOpackid[9];      
    char cOpackid[9];     
    char cTemp[51];  
       
    memset(cRespCode,0,sizeof(cRespCode));    
    memset(cOpackday,0,sizeof(cOpackday));  
    memset(cOpackid,0,sizeof(cOpackid));      
    memset(cTmpOpackid,0,sizeof(cTmpOpackid));
    
    GETTAG(plv_pack,"0BE",cTmpOpackid);     
    vtcp_log("[%s][%d] cTmpOpackid==[%s] \n",__FILE__,__LINE__,cTmpOpackid);      
    lv_orderno_add0(cTmpOpackid, sizeof(cOpackid)-1, cOpackid);
    vtcp_log("[%s][%d] cOpackid==[%s] \n",__FILE__,__LINE__,cOpackid);
    
    GETTAG_SIZE(plv_pack,"30I",cOpackday,8); 
    vtcp_log("[%s][%d] cOpackday==[%s] \n",__FILE__,__LINE__,cOpackday);       
    
    lv_get_tag(plv_pack,"C42",cRespCode);/****应答码****/
    vtcp_log("[%s][%d] cRespCode==[%s] \n",__FILE__,__LINE__,cRespCode);   

    memset(cTemp,0,sizeof(cTemp));  
        
    memcpy(cTemp,cOpackid,8);
    memcpy(cTemp+8,cOpackday,8);  
    memcpy(cTemp+16,cRespCode,8);
    /****NEWYX2 BEGIN:增加原包类型****/
    GETTAG_SIZE(plv_pack,"02D",cTemp+24,3); 
    /****NEWYX2 END:增加原包类型****/
    
    set_zd_data("0290", cRespCode);  

    vtcp_log("[%s][%d] 1111 cTemp=[%s]\n",__FILE__,__LINE__,cTemp);              
    set_zd_data("0270", cTemp);  
    
    memset(cTemp,0,sizeof(cTemp));
    get_fd_data("0270", cTemp);
    vtcp_log("[%s][%d] 2222 cTemp=[%s]\n",__FILE__,__LINE__,cTemp);    
    
    vtcp_log(" --- 正常走帐 --- =[%s][%d] ",__FILE__,__LINE__);           
    strcpy( g_pub_tx.reply, "0000" );  /* 转到 Jtst设置8583域走帐  */
    
    set_zd_data("0210","01");
    set_zd_data("0680","1"); 
    
    return 0;
}



/***************************************************************************/
/* 函数名称: pubf_lv_read_pack()                                           */
/* 参数:                                                                   */
/* 功能描述: 读来帐报文                                                    */
/*                                                                         */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_lv_read_pack(TLVPACKAGE* plv_pack)
{
  FILE *fpkg;
  char fullfilename[100];        
  char filename[50];
  char* cTmpBuf;  
  int  iPackLen = 0;
  int  iTotalLen = 0;
  int  iPackId = 0;
  char cPack[MTU_LEN+1];   
  char *pTmp,cTmp;

  memset( fullfilename,'\0', sizeof(fullfilename) );    
  memset( filename,'\0', sizeof(filename) );
  memset( cPack, '\0', sizeof(cPack) );

  get_zd_data("0250",filename);
  vtcp_log("filename:[%s]",filename);  
  pub_base_strpack ( filename );
  if ( strlen(filename) < 1 )
  {
    sprintf(acErrMsg,"来报文件名错");
    WRITEMSG
    return -1;
  }
  sprintf( fullfilename,"%s/",getenv("FILDIR") );
  memcpy(fullfilename+strlen(fullfilename),filename,sizeof(filename));
  vtcp_log("来报报文文件名:[%s]",fullfilename);  

  fpkg = fopen(fullfilename, "r");
  if (fpkg==NULL)
  {
    sprintf(acErrMsg,"读来报文件[%s]打开文件出错",fullfilename);
    WRITEMSG
    return -1;
  }
  cTmpBuf = (char *)malloc(HV_BODY_LENGTH);   /* 报文体数据   报文头＋业务头+报文体   60k应该够了吧 */
  if ( NULL == cTmpBuf )
  {
    fclose(fpkg);      
    sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
    WRITEMSG
    return -1;        
  }   
  memset(cTmpBuf,0,HV_BODY_LENGTH); 
  g_reply_int = fread(cTmpBuf,sizeof(char),HV_BODY_LENGTH,fpkg);
  vtcp_log("g_reply_int=[%d]",g_reply_int);   
  if (!g_reply_int)
  {
    free(cTmpBuf);   
    cTmpBuf = NULL;   
    fclose(fpkg);      
    sprintf(acErrMsg,"读来报文件[%s]读文件出错",fullfilename);
    WRITEMSG
    return -1;
  }
  fclose(fpkg);

/*  pub_base_strpack(cTmpBuf);*/

  cTmpBuf[HV_BODY_LENGTH-1] = '\0';
  iTotalLen = strlen(cTmpBuf);
  vtcp_log("[%s][%d] 来报报文长 [%d]",__FILE__,__LINE__, iTotalLen);
  vtcp_log("\n%s",cTmpBuf);  

  while (1)
  {
    if ( (iPackId+1)*MTU_LEN > iTotalLen )
    {
      /*iPackLen = (iPackId+1)*MTU_LEN - iTotalLen;    ***剩余不足整包****/
      iPackLen = iTotalLen - iPackId*MTU_LEN ;  
    }
    else
    {
      iPackLen = MTU_LEN;
    }
    vtcp_log("MTU_LEN=[%d] iPackId=[%d]  iPackLen=[%d] ",MTU_LEN, iPackId, iPackLen);         
    memcpy(cPack, cTmpBuf+iPackId*MTU_LEN, MTU_LEN );
    vtcp_log("cPack=[%.512s] ",cPack);  

    dm_dmemcpy( &(plv_pack->dm), iPackId*MTU_LEN+1, cPack, iPackLen);
    iPackId ++;
    if ( iPackId * MTU_LEN >= iTotalLen )
    {
      break;          /****完毕****/
    }
  }/*end while*/ 
  free(cTmpBuf); 
  cTmpBuf = NULL;       
  vtcp_log("pubf_lv_read_pack 结束!");        
  return 0;
}

/***************************************************************************/
/* 函数名称: pubf_lv_pkg_hang_chk()                                        */
/* 参数:                                                                   */
/* 功能描述:    小额来帐 PKG 挂帐检查                                          */
/*                                                                         */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_lv_pkg_hang_chk(PAY_IN_AREA* payin, char* cmtno)
{
  char  cStat[1];            /** 来账业务状态 **/
  char  tx_br_no [BRNO_LEN+1] ;       /*大额来帐记账机构号*/  
  char  tx_tel [6] ;         /* 柜员号*/
  char  acc_v[33];
  char  name_v[61];
  int   ac_seqn_v = 1;
  int   ret;
  char  cTxamt[17];
  double dTxamt=0;
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;
  char cTempstr[1024];
 
  /* being NEWTCTD1 */  
  /* begin add by LiuHuafeng 2007-8-23 17:58:45 */
  char cRcpstat[4];
  memset(cRcpstat, 0 , sizeof(cRcpstat));
  memcpy(cRcpstat,"000",sizeof(cRcpstat)-1);
  /* end by LiuHuafeng 2007-8-23 17:58:54 */
  /* end NEWTCTD1 */ 
      
  memset(cTxamt , 0 , sizeof(cTxamt));
  memset(cStat, 0 , sizeof(cStat));
  memset(tx_br_no,0x00,sizeof(tx_br_no));
  memset(tx_tel,0x00,sizeof(tx_tel));
  memset(acc_v,0x00,sizeof(acc_v));
  memset(name_v,0x00,sizeof(name_v)); 
  dTxamt=0;
  
  memcpy(cTxamt,payin->F_txamt,sizeof(payin->F_txamt));   
  zip_space(cTxamt);
  str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
  vtcp_log("%s,%d txamt===[%f]",__FL__,dTxamt);
  vtcp_log("%s,%d txamt===[%s]",__FL__,payin->F_txamt);
  vtcp_log("%s,%d txamt===[%s]",__FL__,payin->T_txamt);
  dTxamt=dTxamt/100.0;
  set_zd_double("0390",dTxamt);
  set_zd_double("0400",dTxamt);
  cStat[0]=STAT_LZRECV1;/* 设置默认状态为 临时接收,不记帐 */
  
  ret=iFind_qsbrno(tx_br_no);
  if(ret)
  {
    vtcp_log("%s,%d 极端情况清算中心也不存在[%d]",__FILE__,__LINE__,ret);

    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:05:28 */
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:05:37 */
    /* end NEWTCTD1 */ 
    
    return ret;
  }
  set_zd_data(DC_TX_BR_NO,tx_br_no);   /* 设置默认mbfe来帐记账机构号为清算中心*/
  vtcp_log("%s,%d 设置默认接收机构为清算中心[%s]",__FILE__,__LINE__,tx_br_no);
  /* 检查来帐接收机构是否存在，来帐帐号是否存在，来帐开户机构是否存在 */
  ret =iSetLvTxBrno(payin);
  /* begin add by LiuHuafeng 2009-5-13 17:08:23 for 村镇银行 */
	if(ret == 9999)
	{
		cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，支行挂帐*/
		payin->T_stat[0]=STAT_LZGZ_BR;
		set_zd_data("0980",cStat);      /** 来账业务状态 **/
		memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
		set_zd_data("0890",cRcpstat);
		sprintf(acErrMsg,"得到柜员号错误,挂账!");
		/* 设置机构为清算中心 */
		vLvSetBrnoAndSts_Fd(tx_br_no,cStat,__FILE__,__LINE__);
		WRITEMSG
		/* 直接跳转的到J099 */
		return 999;
	}
	/* end by LiuHuafeng 2009-5-13 17:08:33 */
	/*beg add by ChengGX 2010-1-4 15:32 */
	if(ret)
	{
		cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，支行挂帐*/
		payin->T_stat[0]=STAT_LZGZ_BR;
		set_zd_data("0980",cStat);      /** 来账业务状态 **/
		memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
		set_zd_data("0890",cRcpstat);
		sprintf(acErrMsg,"得到柜员号错误,挂账!");
		/* 设置机构为来帐接受支付系统行号对应的机构 */
		iLvGetReceKinbr(payin->F_acbrno,tx_br_no);/*上边已经判断肯定不是返回9999，否则就设置成了清算中心*/
		vLvSetBrnoAndSts_Fd(tx_br_no,cStat,__FILE__,__LINE__);
		WRITEMSG
		/* 直接跳转的到J099 */
		return 100;
	}
	/*end add by ChengGX 2010-1-4 15:32 */
  vtcp_log("[%s][%d] 开始设置来帐接收机构",__FILE__,__LINE__);
  get_zd_data(DC_TX_BR_NO,tx_br_no);   /* 由上一步得到的大额来帐记账机构号*/
  memcpy(payin->T_brno,tx_br_no,BRNO_LEN);
  memcpy(payin->A_brno,tx_br_no,BRNO_LEN);
  ret = iGetReceTel(tx_br_no);    /*  柜员号 ==> 0070 */
  if ( ret )
  {
    cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，支行挂帐*/
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:05:28 */
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end add by LiuHuafeng 2007-8-23 18:06:04 */
    /* end NEWTCTD1 */    
    
    sprintf(acErrMsg,"得到柜员号错误,挂账!");
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return 100;
  }
  get_zd_data("0070",tx_tel); /* 由上一步得到的柜员号*/
  vtcp_log("%s,%d 注意这里操作员号%s",__FILE__,__LINE__,tx_tel);
  if(ret==1)
  {
    /* 交易机构找到了，但行号的开户行没有找到，支行挂账 */
    if(cStat[0]==STAT_LZNMAC_ERR ||cStat[0]==STAT_LZLMAC_ERR)
    {
    }
    else
    {
      cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，清算中心挂帐*/
    }
    payin->T_stat[0]=cStat[0];
    set_zd_data("0980",cStat);      /** 来账业务状态 **/    
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:08:46 */
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:07:54 */
    /* end NEWTCTD1 */
        
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    sprintf(acErrMsg,"得到开户机构号错误,支行挂账!");
    WRITEMSG
    return 100;
  }
  else if ( ret )
  {
    if(cStat[0]==STAT_LZNMAC_ERR ||cStat[0]==STAT_LZLMAC_ERR)
    {
    }
    else
    {
      cStat[0] = STAT_LZGZ_QS;     /* 'M'  信息不符，清算中心挂帐*/
    }
    payin->T_stat[0]=cStat[0];
    set_zd_data("0980",cStat);      /** 来账业务状态 **/   
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:09:09 */
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:09:11 */
    /* end NEWTCTD1 */    
     
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    sprintf(acErrMsg,"得到交易机构号错误,挂账!");
    WRITEMSG
    return 100;
  }
  /** 判断来账状态  **/
	if (  !memcmp(payin->F_pkgno,"001",3)||!memcmp(payin->F_pkgno,"003",3)
	    ||!memcmp(payin->F_pkgno,"005",3)||!memcmp(payin->F_pkgno,"007",3)
	    ||!memcmp(payin->F_pkgno,"008",3)||!memcmp(payin->F_pkgno,"011",3)
	    ||!memcmp(payin->F_pkgno,"010",3)
	   )
  {
    cStat[0] = STAT_LZRECV2 ;
  } 
  
  /* begin NEWTCTD1 */
  /* begin add by LiuHuafeng 2007-8-22 11:35:21 对回执类交易应答为拒绝的进行帐务处理 */
  else if(!memcmp(payin->F_pkgno,"009",3) && memcmp(payin->F_rcpstat,"00",2)!=0)
  {
    cStat[0] = STAT_LZRECV2;
    vtcp_log("[%s][%d]PKG009的状态[%c]\n",__FILE__,__LINE__,cStat[0]);
  }
    else if(!memcmp(payin->F_pkgno,"004",3))
  {
    /* 实时借记来帐虽然不进行帐务处理，但同样要查一边是否账户正常
       这样为其回执取得一个放到fd89中的值,需要注意004实时借记报文
       要检查付款人帐号，与正常的检查收款人账户不同
    */
    double dTmpTxamt=0.0;
    char * endptr;
    /*  大额来帐记账机构号及柜员号赋值  */       
    iLvGetRece_AcValue(payin);
    /**modified NEWTCTD 20070912 wangyongwei**/
    char tmp_amt[16];
    memset(tmp_amt,0,sizeof(tmp_amt));
    memcpy(tmp_amt,payin->F_txamt,sizeof(payin->F_txamt));
    dTmpTxamt=strtod(tmp_amt,&endptr)/100.0;
    /**modified NEWTCTD 20070912 wangyongwei**/
    vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
    vtcp_log("[%s][%d]txamt=[%f]",__FILE__,__LINE__,dTmpTxamt);
    vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
    vtcp_log("[%s][%d] tx_br_no=[%.5s]",__FILE__,__LINE__,tx_br_no);
    ret = iGetReceTel(tx_br_no);    /*  柜员号 ==> 0070 */
    if ( ret )
    {
      cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      sprintf(acErrMsg,"得到柜员号错误,挂账!");
      memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    get_zd_data("0070",tx_tel); /* 由上一步得到的柜员号*/
    vtcp_log("[%s][%d] tx_tel=[%.5s]",__FILE__,__LINE__,tx_tel);
    
        /****实时借记来帐检查,判断是否已经收到了325冲正应答，但我们刚收到实时借记业务的情况add by xyy YHBP****/
    struct hv_fzf_c  hv_fzf;
    memset(&hv_fzf , 0 , sizeof(hv_fzf));
    vtcp_log("[%s][%d]实时借记来帐检查记录信息，是否需要挂账\n",__FILE__,__LINE__);
    ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"otype='2' and o_pack_date ='%ld'  and o_packid='%.8s' and cmtno='325' and o_cmtno='004' " \
        ,apatoi(payin->F_packday,8),payin->F_packid);
    vtcp_log("[%s][%d]首先判断是否已经被冲正啦[%d]\n",__FILE__,__LINE__,ret);
    if(ret!=0 && ret!=100)
    {
        vtcp_log("[%s][%d]GAME OVER [%d]!!!!\n",__FILE__,__LINE__,ret);
        payin->T_stat[0]=STAT_LZGZ_BR;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"数据库操作失败[%d]",ret);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
    }
    ret = Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
    vtcp_log("[%s][%d]首先判断是否已经被冲正啦[%d]\n",__FILE__,__LINE__,ret);
    if(ret!=0 && ret!=100)
    {
        vtcp_log("[%s][%d]GAME OVER [%d]!!!!\n",__FILE__,__LINE__,ret);
        payin->T_stat[0]=STAT_LZGZ_BR;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"数据库操作失败[%d]",ret);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
    }
    else if(!ret)
    {
        vtcp_log("[%s][%d]GAME OVER [%d]!!!!\n",__FILE__,__LINE__,ret);
        cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
        payin->T_stat[0]=STAT_LZGZ_BR;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"020",sizeof(cRcpstat)-1); /**业务已冲正**/
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"业务已冲正[%d]!",ret);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
    }
    else
    {
      vtcp_log("[%s][%d]在冲正支付登记簿中没有找到相关记录\n",__FILE__,__LINE__);
    }
    Hv_fzf_Clo_Sel();
    if(memcmp(payin->F_txnum,TXNUM_TYJL,5)==0)
    {
        vtcp_log("%s,%d,实时业务中的银行本票不需要检查付款人的密码",__FILE__,__LINE__);
        vtcp_log("%s,%d T_addid=[%s]\n",__FILE__,__LINE__,payin->T_addid);
        vtcp_log("%s,%d payin->F_wtday=[%s]\n",__FILE__,__LINE__,payin->F_wtday);
        
        double dCpamt=0;
        char   cTempBuff[61]; /***add by NEWBP12 ******/
        struct cashier_c wd_cashier;
        
        int  ret=0;
        struct  com_parm_c     com_parm;
        struct  mdm_ac_rel_c   mdm_ac_rel;
        struct  in_mst_c       in_mst;

        char cBuff[2001];
        
        TYJL1_ADD_AREA lv_tyjl1;
        ZPJL2_ADD_AREA lv_jpjl2;
        TYJL4_ADD_AREA lv_tyjl4;
        
        memset(cTempBuff  , 0 , sizeof(cTempBuff));/**add by NEWBP12**/
        memset(cBuff      , 0 , sizeof(cBuff));
        memset(&lv_tyjl1  , 0 , sizeof(lv_tyjl1));
        memset(&lv_jpjl2  , 0 , sizeof(lv_jpjl2));
        memset(&lv_tyjl4  , 0 , sizeof(lv_tyjl4));
        memset(&wd_cashier, 0 , sizeof(wd_cashier));
        memset(&com_parm    , 0 , sizeof(com_parm));
        memset(&in_mst      , 0 , sizeof(in_mst));
        memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));/**end by  YHBP**/

        

        vtcp_log("[%s][%d] payin->T_addid =[%s]\n",__FILE__,__LINE__,payin->T_addid);
        vtcp_log("[%s][%d] payin->F_wtday =[%s]\n",__FILE__,__LINE__,payin->F_wtday);
        
        iLvGetAddit(payin,"001",cBuff,0);
        vtcp_log("[%s][%d] 得到的附加域1=[%s]",__FILE__,__LINE__,cBuff);
        memcpy((char *)&lv_tyjl1,cBuff, sizeof(lv_tyjl1));
        vtcp_log("[%s][%d] 写入附加域文件1=[%s]",__FL__,(char *)&lv_tyjl1);
        /**附加域1处理完毕**/

        memset(cBuff    , 0 , sizeof(cBuff));
        iLvGetAddit(payin,"002",cBuff,0);
        vtcp_log("[%s][%d] 得到的附加域2=[%s]",__FILE__,__LINE__,cBuff);
        memcpy((char *)&lv_jpjl2,cBuff, sizeof(lv_jpjl2));
        vtcp_log("[%s][%d] 写入附加域文件4=[%s]",__FL__,(char *)&lv_jpjl2);
        /**附加域2处理完毕**/


        memset(cBuff    , 0 , sizeof(cBuff));
        iLvGetAddit(payin,"004",cBuff,0);
        vtcp_log("[%s][%d] 得到的附加域4=[%s]",__FILE__,__LINE__,cBuff);
        memcpy((char *)&lv_tyjl4,cBuff, sizeof(lv_tyjl4));
        vtcp_log("[%s][%d] 写入附加域文件4=[%s]",__FL__,(char *)&lv_tyjl4);
        /**附加域4处理完毕**/
        
        vtcp_log("%s,%d 附加域信息处理完成拉\n",__FILE__,__LINE__);
        memcpy(wd_cashier.pono   ,lv_tyjl1.cpno ,sizeof(wd_cashier.pono)-1);
        wd_cashier.signday = apatoi(lv_tyjl1.cpday,8);
        vtcp_log("%s,%d pono=[%s]\n",__FILE__,__LINE__,wd_cashier.pono);
        vtcp_log("%s,%d signday=[%d]\n",__FILE__,__LINE__,wd_cashier.signday);
        /*** 20110422 银行本票号码位数变更修改 12位->16位
        ret = Cashier_Dec_Sel(g_pub_tx.reply,"pono='%s' and signday='%ld'",\
                              wd_cashier.pono,wd_cashier.signday);
        ***/
        ret = Cashier_Dec_Sel(g_pub_tx.reply,"(pono='%s' or pono='%s') and signday='%ld'",\
                              wd_cashier.pono,wd_cashier.pono+8,wd_cashier.signday);
        if (ret)
        {
            vtcp_log("[%s][%d]GAME OVER [%d]\n",__FILE__,__LINE__,ret);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"030",sizeof(cRcpstat)-1); /**本票号码不符**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"票据信息不符[%s][%d]!",wd_cashier.pono,wd_cashier.signday);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        vtcp_log("[%s][%d]准备FETCH 数据啦[%d]\n",__FILE__,__LINE__,ret);
        ret = Cashier_Fet_Sel(&wd_cashier ,g_pub_tx.reply);
        if (ret)
        {
            vtcp_log("[%s][%d]GAME OVER [%d]\n",__FILE__,__LINE__,ret);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"030",sizeof(cRcpstat)-1); /** 非本行票据**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"票据信息不符[%s][%d]!",wd_cashier.pono,wd_cashier.signday);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        vtcp_log("[%s][%d]票据状态为[%c]\n",__FILE__,__LINE__,wd_cashier.stat[0]);
        /**检查票据状态**/
        if(wd_cashier.stat[0]!='1')
        {
            vtcp_log("[%s][%d]GAME OVER 票据状态[%c]\n",__FILE__,__LINE__,wd_cashier.stat[0]);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/ 
            /****modify by NEWBP1 *****/
            if(wd_cashier.stat[0]=='2' || wd_cashier.stat[0]=='3')
            {
                memcpy(cRcpstat,"029",sizeof(cRcpstat)-1); /**NEWBP1 已经兑付或解付**/
            }
            else
            {
                memcpy(cRcpstat,"048",sizeof(cRcpstat)-1); /**NEWBP1 不是签发状态**/
            }
            /**end by NEWBP1**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]不是签发状态[%s],不允许兑付",__FILE__,__LINE__,wd_cashier.stat);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /***检查交易金额***/
        str2dbl(payin->T_txamt,sizeof(payin->T_txamt),0,&dCpamt);
        dCpamt = dCpamt/100;
        vtcp_log("[%s][%d]交易金额为[%f]\n",__FILE__,__LINE__,dCpamt);
        if(pub_base_CompDblVal(wd_cashier.avbal,dCpamt)!=0)
        {
            vtcp_log("[%s][%d]GAME OVER 交易金额[%f]\n",__FILE__,__LINE__,wd_cashier.avbal);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"044",sizeof(cRcpstat)-1); /**NEWBP出票金额不符**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]出票金额[%f]非[%f]",__FILE__,__LINE__,wd_cashier.avbal,dCpamt);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /**检查出票金额**/
        dCpamt = 0.00;
        str2dbl(lv_tyjl1.cpamt,sizeof(lv_tyjl1.cpamt),0,&dCpamt);
        dCpamt = dCpamt/100;
        vtcp_log("[%s][%d]附加域里的出票金额为[%f]\n",__FILE__,__LINE__,dCpamt);
        if(pub_base_CompDblVal(wd_cashier.avbal,dCpamt)!=0)
        {
            vtcp_log("[%s][%d]GAME OVER 出票金额[%f]\n",__FILE__,__LINE__,wd_cashier.avbal);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"044",sizeof(cRcpstat)-1); /**NEWBP 出票金额不符**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]出票金额[%f]非[%f]",__FILE__,__LINE__,wd_cashier.avbal,dCpamt);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /**** add by YH_BP ****/
        vtcp_log("[%s][%d]传入 密押[%s]\n",__FILE__,__LINE__,lv_tyjl4.mac);
        /**这里要判断密押**/
        /**add by NEWBP12**/
        memset(cTempBuff  , 0 , sizeof(cTempBuff));
        memcpy(cTempBuff,lv_tyjl4.mac,sizeof(lv_tyjl4.mac));
        zip_space(cTempBuff);
        /**end by NEWBP12**/
        if(memcmp(cTempBuff,wd_cashier.passwd,sizeof(wd_cashier.passwd)-1)!=0  )
        {
            vtcp_log("[%s][%d]GAME OVER 密押错误[%.20s]\n",__FILE__,__LINE__,lv_tyjl4.mac);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"043",sizeof(cRcpstat)-1); /**NEWBP 密押错**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]密押错误[%.20s]",__FILE__,__LINE__,lv_tyjl4.mac);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /**** end by YH_BP ****/
        vtcp_log("[%s][%d]lv_tyjl4.ptype=[%.2s]\n",__FILE__,__LINE__,lv_tyjl4.ptype);
        /**检查票据类型**/
        if(memcmp(lv_tyjl4.ptype,"05",sizeof(lv_tyjl4.ptype))!=0)
        {
            vtcp_log("[%s][%d]GAME OVER 票据类型[%s]\n",__FILE__,__LINE__,lv_tyjl4.ptype);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"022",sizeof(cRcpstat)-1); /**NEWBP1 票据类型不符**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]票据类型错[%s]",__FILE__,__LINE__,lv_tyjl4.ptype);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /**检查付款人帐号** delete by NEWBP        人行不要求检查原付款人信息
        vtcp_log("[%s][%d]付款帐号[%s]\n",__FILE__,__LINE__,payin->F_payactno);
        if(memcmp(payin->F_payactno,wd_cashier.payactno,strlen(wd_cashier.payactno))!=0)
        {
            vtcp_log("[%s][%d]GAME OVER 付款帐号[%s]\n",__FILE__,__LINE__,wd_cashier.payactno);
            cStat[0] = STAT_LZGZ_BR;   * N' 信息不符,支行挂帐*
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      ** 来账业务状态 **    
            memcpy(cRcpstat,"001",sizeof(cRcpstat)-1); **帐号不符**
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]付款帐号错[%s]",__FILE__,__LINE__,payin->F_payactno);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        **检查付款人户名**
        vtcp_log("[%s][%d]付款人[%s]\n",__FILE__,__LINE__,payin->F_payname);
        if(memcmp(payin->F_payname,wd_cashier.payname,strlen(wd_cashier.payname))!=0)
        {
            vtcp_log("[%s][%d]GAME OVER 付款人[%s]\n",__FILE__,__LINE__,wd_cashier.payname);
            cStat[0] = STAT_LZGZ_BR;   * N' 信息不符,支行挂帐*
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      ** 来账业务状态 ** 
            memcpy(cRcpstat,"002",sizeof(cRcpstat)-1); **户名不符**
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]付款人名错[%s]",__FILE__,__LINE__,payin->F_payname);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        end by NEWBP**********************************/
        /**检查提示付款日期**/
        vtcp_log("[%s][%d]付款日期[%.8s]\n",__FILE__,__LINE__,lv_tyjl4.tsday);
        if(apatoi(lv_tyjl4.tsday,8) < g_pub_tx.tx_date)/**modify by NEWBP1**/
        {
            vtcp_log("[%s][%d]GAME OVER 付款日期[%ld]\n",__FILE__,__LINE__,lv_tyjl4.tsday);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"045",sizeof(cRcpstat)-1); /**NEWBP 提示付款日期大于本票到期日**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]到期日[%d]提示日[%s]",__FILE__,__LINE__,wd_cashier.endday,lv_tyjl4.tsday);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /**add by NEWBP9 2008-5-7 11:04**/
        long lEnday=0;
        lEnday = wd_cashier.endday;
        vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEnday);
        pub_base_iJumpHoliday(&lEnday);
        vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEnday);
        if(lEnday < g_pub_tx.tx_date)/**注意:wd_cashier.endday  ---改成---> lEnday  end by NEWBP9**/
        {
            vtcp_log("[%s][%d][%d]本票已经到期不允许兑付\n",__FILE__,__LINE__,wd_cashier.endday);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/    
            memcpy(cRcpstat,"045",sizeof(cRcpstat)-1); /**modify by NEWBP3 本票已期**/
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"[%s][%d]到期日[%d]",__FILE__,__LINE__,wd_cashier.endday);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        if(wd_cashier.bptype[0]=='1') /**现金本票要检查兑付行**/
        {
            /**现金本票的话,检查兑付行是否与签发行同一个清算中心**/
            struct hv_uniontab_c Hv_uniontab;
            int iRet=0;
            memset(&Hv_uniontab,0,sizeof(Hv_uniontab));
            vtcp_log("[%s][%d]代理付款行为:[%s]\n",__FILE__,__LINE__,payin->F_acbrno);
            iRet=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab,"or_br_no='%.12s' ",payin->F_acbrno);
            /**得到兑付行的清算行号**/
            if(memcmp(Hv_uniontab.qs_no,HV_QS_BR_NO,sizeof(Hv_uniontab.qs_no)-1)!=0 || iRet!=0)
            {
                vtcp_log("%s,%d查询代理付款行错误[%s]",__FILE__,__LINE__,lv_tyjl1.dlpayno);
                cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
                payin->T_stat[0]=STAT_LZGZ_BR;
                set_zd_data("0980",cStat);      /** 来账业务状态 **/    
                memcpy(cRcpstat,"030",sizeof(cRcpstat)-1); /**modify by NEWBP3 他行不能兑付我行现金本票**/
                set_zd_data("0890",cRcpstat);
                sprintf(acErrMsg,"[%s][%d]查询代理付款行错误",__FILE__,__LINE__);
                vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                WRITEMSG
                return 999;
            }
        }
        /***add by NEWBP   ***/
        vtcp_log("[%s][%d]收款人[%s]\n",__FILE__,__LINE__,payin->F_cashname);
        /**首先判断收款人是否为原收款人**/
        /**add by NEWBP12**/
        memset(cTempBuff  , 0 , sizeof(cTempBuff));
        memcpy(cTempBuff,payin->F_cashname,sizeof(payin->F_cashname));
        zip_space(cTempBuff);
        /**end by NEWBP12**/
        if(memcmp(cTempBuff,wd_cashier.cashname,sizeof(wd_cashier.cashname)-1)!=0)/**modify by NEWBP6 注意把这个整个if语句可以直接替换**/
        {
             /**首先判断收款人不是为原收款人，那我们检查原收款人**/
             vtcp_log("[%s][%d]cashname=[%s]\n",__FILE__,__LINE__,wd_cashier.cashname);
             /***add by NEWBP8 2008-5-6 12:45***/
            if(wd_cashier.bptype[0]=='1')/**现金本票不可转让,即收款人必须为原收款人**/
            {
                 vtcp_log("[%s][%d]现金本票来帐中收款人[%s]\n",__FILE__,__LINE__,cTempBuff);
                 vtcp_log("[%s][%d]现金本票表中收款人[%s]\n",__FILE__,__LINE__,wd_cashier.cashname);
                 cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
                 payin->T_stat[0]=STAT_LZGZ_BR;
                 set_zd_data("0980",cStat);      /** 来账业务状态 **/
                 memcpy(cRcpstat,"041",sizeof(cRcpstat)-1); /**户名不符**/
                 set_zd_data("0890",cRcpstat);
                 sprintf(acErrMsg,"[%s][%d]付款人名错[%s]",__FILE__,__LINE__,payin->F_payname);
                 vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                 WRITEMSG
                 return 999;
            }
            /***end by NEWBP8 2008-5-6 12:46***/

             memset(cTempBuff  , 0 , sizeof(cTempBuff));
             memcpy(cTempBuff,lv_tyjl4.content,sizeof(lv_tyjl4.content));
             zip_space(cTempBuff);
             
             if(memcmp(cTempBuff,wd_cashier.cashname,sizeof(wd_cashier.cashname)-1)!=0)
             {
                  vtcp_log("[%s][%d]来帐中收款人[%s]\n",__FILE__,__LINE__,lv_tyjl4.content);
                  vtcp_log("[%s][%d]表中收款人[%s]\n",__FILE__,__LINE__,wd_cashier.cashname);
                  cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
                  payin->T_stat[0]=STAT_LZGZ_BR;
                  set_zd_data("0980",cStat);      /** 来账业务状态 **/
                  memcpy(cRcpstat,"041",sizeof(cRcpstat)-1); /**户名不符**/
                  set_zd_data("0890",cRcpstat);
                  sprintf(acErrMsg,"[%s][%d]付款人名错[%s]",__FILE__,__LINE__,payin->F_payname);
                  vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                  WRITEMSG
                  return 999;
             }
        }
        /**end by NEWBP6 **/
        set_zd_data("0910",wd_cashier.brno);
        vtcp_log("[%s][%d]签发行为:[%s]\n",__FILE__,__LINE__,wd_cashier.brno);
        /**暂时检查这些,下面更新表里的一些字段**/
        Cashier_Clo_Sel();
        
        ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",wd_cashier.brno);
        if(ret)
        {
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/
            memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        /** 检查本票签发户是否存在 **/
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
        if(ret)
        {
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/
            memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
        if(ret)
        {
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/
            memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        vtcp_log("[%s][%d]xyy付款帐户状态为[%c]\n",__FILE__,__LINE__,in_mst.sts[0]);
        if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
        {
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/
            memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
        vtcp_log("[%s][%d]xyy付款帐户为[%s]\n",__FILE__,__LINE__, com_parm.val);
        memcpy(acc_v,com_parm.val,sizeof(acc_v)-1);
        pub_base_old_acno(acc_v);
        pub_base_strpack_all(acc_v);
        memcpy(name_v,in_mst.name,sizeof(name_v)-1);
        pub_base_strpack_all(name_v);
    }
    else/**end by xyy YHBP**/
    {
        /*  付款人 帐号 户名 帐号状态的检查  */
        memcpy(acc_v,payin->F_payactno,sizeof(acc_v)-1);
        pub_base_old_acno(acc_v);
        pub_base_strpack_all(acc_v);
        memcpy(name_v,payin->F_payname,sizeof(name_v)-1);
        pub_base_strpack_all(name_v);
    }
    vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);
    vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);
    
    /* 付款人账号检查开始 */
    memset( &mdm_ac_rel_v, 0x00, sizeof(struct mdm_ac_rel_c));
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    else if (g_reply_int)
    {
      payin->T_stat[0]=STAT_LZGZ_QS;
      sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    /**add by wangyongwei 20070912**/
    #ifdef CZ_XQ
    char Ac_Br_No[13];
    char Js_brno[6];
    memset(Ac_Br_No,0,sizeof(Ac_Br_No));
    memset(Js_brno,0,sizeof(Js_brno));
    memcpy(Ac_Br_No,payin->F_acbrno,12);
    g_reply_int= GetBr_noByOr_br_no(Ac_Br_No,Js_brno);
    if(memcmp(mdm_ac_rel_v.opn_br_no,Js_brno,5)!=0){
        vtcp_log("%s,%d接收行非账号的开户行长治不允许这样!\n",__FILE__,__LINE__);
              cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
              payin->T_stat[0]=STAT_LZGZ_BR;
              set_zd_data("0980",cStat);      /** 来账业务状态 **/
              memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
              set_zd_data("0890",cRcpstat);
              sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
              vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
              WRITEMSG
              return 999;
    }
    #endif
    /*NEWTCTD5 2007112 帐号开通检查*/
    /**modify by YHBP**/
    vtcp_log("[%s][%d]看看是不是银行本票payin->F_txnum=[%.5s]\n",__FILE__,__LINE__,payin->F_txnum);
    if(memcmp(payin->F_txnum,TXNUM_TC,5)==0 || memcmp(payin->F_txnum,TXNUM_TD,5)==0)
    {
          double td_amt_total=0.0;
          struct tctd_ktzh_c cTctd_ktzh;
          memset(&cTctd_ktzh,0,sizeof(struct tctd_ktzh_c));
          g_reply_int = Tctd_ktzh_Sel(g_pub_tx.reply,&cTctd_ktzh,"tctd_acno='%s' and td_kt_flag='1'",mdm_ac_rel_v.ac_no);        
          if(g_reply_int){
                  vtcp_log("%s,%d付款帐号未开通通兑业务!\n",__FILE__,__LINE__);
                    cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
                    payin->T_stat[0]=STAT_LZGZ_BR;
                    set_zd_data("0980",cStat);      /** 来账业务状态 **/
                    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
                    set_zd_data("0890",cRcpstat);
                    sprintf(acErrMsg,"付款人帐号[%s],未开通通兑业务挂账!",acc_v);
                    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                    WRITEMSG
                    return 999;
          }
          g_reply_int = sql_sum_double("trace_log","amt",&td_amt_total,"tx_code='5840' and add_ind='0' and ac_no='%s' and tx_date='%d' and sts='0'",mdm_ac_rel_v.ac_no,g_pub_tx.tx_date);
          td_amt_total += dTmpTxamt;
          if(td_amt_total > cTctd_ktzh.td_amt_lmt){
                  vtcp_log("%s,%d交易额超过累计限额!\n",__FILE__,__LINE__);
                    cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
                    payin->T_stat[0]=STAT_LZGZ_BR;
                    set_zd_data("0980",cStat);      /** 来账业务状态 **/
                    memcpy(cRcpstat,"004",sizeof(cRcpstat)-1);
                    set_zd_data("0890",cRcpstat);
                    sprintf(acErrMsg,"交易额[%f]超过累计限额[%f]!",dTmpTxamt,cTctd_ktzh.td_amt_lmt);
                    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                    WRITEMSG
                    return 999;
          }
      /**add by wangyongwei 20070912**/
    }/**end YHBP**/
    if(mdm_ac_rel_v.draw_pwd_yn[0]!='Y')
    {
          if(memcmp(payin->F_txnum,TXNUM_TYJL,5)==0)
          {
                vtcp_log("[%s][%d]银行本票不检查有密无密=[%.5s]\n",__FILE__,__LINE__,payin->F_txnum);
          }
          else
          {
            vtcp_log("%s,%d,无密户不允许通兑",__FILE__,__LINE__);
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            memcpy(cRcpstat,"010",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 999;
        }
    }
    else
    {
        vtcp_log("[%s][%d]payin->F_txnum=[%s]\n",__FILE__,__LINE__,payin->F_txnum);
      if(memcmp(payin->F_txnum,TXNUM_TYJL,5)!=0)/**modify by YHBP**/
      {
          int iRc=0;
          TD_ADD_AREA wd_td;
          memset(&wd_td,'\0',sizeof(wd_td));
          iRc=iLvGetAddit(payin,"000",(char*)&wd_td,sizeof(wd_td));
          iRc=nBepsDescrypt(payin,(char*)&wd_td);
          CBshuff(wd_td.pswdcode);
          if(memcmp(mdm_ac_rel_v.draw_pwd,wd_td.pswdcode,6)!=0){
            vtcp_log("%s,%d,付款帐号密码不正确",__FILE__,__LINE__);
            /**帐号密码错误****/
            memcpy(cRcpstat,"010",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            return 999;
          }
          vtcp_log("%s,%d,付款帐号密码正确",__FILE__,__LINE__);
       }
       /**end YHBP**/
    }
    /** begin 检查介质的状态 by LiuHuafeng 2006-10-5 17:51 */
    if(mdm_ac_rel_v.note_sts[0]!='0' )
    {
      /* 介质状态不正常挂帐处理 */
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      sprintf(acErrMsg,"大额来账收款人账号[%s]介质状态错误[%s]",acc_v,mdm_ac_rel_v.note_sts[0]);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    /* end by LiuHuafeng 2006-10-5 17:52 */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    if(mdm_ac_rel_v.ac_seqn==9999){/* 多帐号介质默认主帐户 1 */
      g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and ac_id_type='1'",mdm_ac_rel_v.ac_id );
    } else {
      g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld'",mdm_ac_rel_v.ac_id);
    }
    if (g_reply_int==100)
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/    
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      sprintf(acErrMsg,"大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    else if (g_reply_int)
    {
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/    
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 999;
    }
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
      vtcp_log("%s,%d",__FILE__,__LINE__);
      ac_seqn_v  =  1;    
      g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
      if (g_reply_int)
      {
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      /* 判断账户余额 */
      if(dTmpTxamt>dd_mst_v.bal -1 )
      {
          vtcp_log("%s,%d dTmpTxamt[%.2lf],dd_mst_v.bal[%.2lf]\n",__FILE__,__LINE__,dTmpTxamt,dd_mst_v.bal);
        /* 账户留存余额小于1元 */
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"003",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      vtcp_log("%s,%d",__FILE__,__LINE__);
     if (dd_mst_v.ac_sts[0] != '1' || dd_mst_v.hold_sts[0] != '0' && dd_mst_v.hold_sts[0]!='3' )/**modify by YHBP**/
      {
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/    
        memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      vtcp_log("%s,%d",__FILE__,__LINE__);
      /*begin 判断是否别冻结 add by LiuHuafeng 2006-10-5 10:57 */
      if(dd_mst_v.hold_sts[0]=='1' && dd_mst_v.hold_sts[0]=='2' )
      {
        /* 帐号id被完全冻结或只进不出，所以要挂帐 */
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/
        memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      if(dd_mst_v.hold_sts[0]=='3' && dTmpTxamt > dd_mst_v.bal - dd_mst_v.hold_amt -1)
      {
        /* 帐号id被部分冻结所以要判断余额，不足要挂帐 */
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/
        memcpy(cRcpstat,"003",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      vtcp_log("%s,%d",__FILE__,__LINE__);
      /* end by LiuHuafeng 2006-10-5 11:23 */
      /* begin 判断是否是结算户 by LiuHuafeng 2006-10-5 11:24 */
  /****NEWTCTD9:通存通兑允许非结算户交易****/
      if(dd_mst_v.sttl_type[0]=='S' &&
    memcmp(payin->T_pkgno,PKGNO_SSDJ,3)!=0 && 
    memcmp(payin->T_pkgno,PKGNO_SSJJ,3)!=0)
      {
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/
        memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      vtcp_log("%s,%d",__FILE__,__LINE__);
      /* end by LiuHuafeng 2006-10-5 11:24 */
      pub_base_strpack(dd_mst_v.name);
      pub_base_strpack(name_v);
      vtcp_log("%s,%d",__FILE__,__LINE__);
      if (strcmp(dd_mst_v.name,name_v))
      {
        cStat[0] = STAT_LZRECV1;   /* 'H' 临时接收不记帐*/
        payin->T_stat[0]=STAT_LZRECV1;
        set_zd_data("0980",cStat);      /** 来账业务状态 **/
        memcpy(cRcpstat,"002",sizeof(cRcpstat)-1);
        set_zd_data("0890",cRcpstat);
        sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,name_v);
        vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
        WRITEMSG
        return 999;
      }
      vtcp_log("%s,%d",__FILE__,__LINE__);
      set_zd_data("0680","1");
    }/* begin add by LiuHuafeng 2006-10-5 17:53 检查内部帐户 */
    else
    {
        if(memcmp(payin->F_txnum,TXNUM_TYJL,5)!=0)/**modify by YHBP**/
          {
            /* 实时借记业务不支持内部账户 */
            memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            return 999;
        }
        else
        {
            struct in_mst_c sIn_mst;
            vtcp_log("%s,%d",__FILE__,__LINE__);
            g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
            if (g_reply_int)
            {
                cStat[0] = STAT_LZGZ_QS;     /* 'M'  信息不符,清算中心挂帐*/
                payin->T_stat[0]=STAT_LZGZ_BR;
                set_zd_data("0980",cStat);      /** 来账业务状态 **/
                memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
                set_zd_data("0890",cRcpstat);
                sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
                vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                WRITEMSG
                return 999;
            }
            vtcp_log("%s,%d",__FILE__,__LINE__);
            if (sIn_mst.sts[0] != '1' )
            {
                cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符,支行挂帐*/
                payin->T_stat[0]=STAT_LZGZ_BR;
                set_zd_data("0980",cStat);      /** 来账业务状态 **/
                memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
                set_zd_data("0890",cRcpstat);
                sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
                vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
                WRITEMSG
                return 999;
            }
            vtcp_log("%s,%d",__FILE__,__LINE__);
            set_zd_data("0680","6");/* 设置帐户类型 */
        }/**end by YHBP**/
    }
    vtcp_log("%s,%d",__FILE__,__LINE__);
    /* 正常则置8583域 */
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
    set_zd_data("0210", "01");     /* 币种 */      
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-24 17:16:57 */
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-24 17:17:03 */
    /* end NEWTCTD1 */
    payin->T_stat[0]=cStat[0];
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    vtcp_log("%s,%d 实时借记来帐付款账户判断正常结束，返回",__FILE__,__LINE__);
    return 0;    
  }
  /* end by LiuHuafeng 2007-8-22 11:37:26 */
  /* end NEWTCTD1 */

  else 
  {
    cStat[0] = STAT_LZRECV1 ;
    
    memcpy(acc_v,payin->F_cashactno,sizeof(acc_v)-1);
    pub_base_strpack_all(acc_v);
    memcpy(name_v,payin->F_cashname,sizeof(name_v)-1);
    pub_base_strpack_all(name_v);
    vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);
        
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);
    memset(cTempstr,0,sizeof(cTempstr));
    memcpy(cTempstr, name_v, sizeof(name_v)-1);
    dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
    memcpy(name_v, cTempstr, sizeof(name_v)-1);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);   
        
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
    set_zd_data("0210", "01");     /* 币种 */
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
    vtcp_log("[%s][%d]cStat=[%s] ",__FILE__,__LINE__,cStat);      
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:09:34 */      
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:09:36 */
    /* end NEWTCTD1 */
    
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
   /* 不用走帐了 转到J099登记就行 ChenMing  2006-10-20 19:05*/
    strcpy(g_pub_tx.reply, "0010"  );   
    return 999;
  }
  vtcp_log("[%s][%d] 来账默认状态=[%c]",__FILE__,__LINE__,cStat[0]);  
  /*  大额来帐记账机构号及柜员号赋值  */       
  iLvGetRece_AcValue(payin);
  
  vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
  vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
  vtcp_log("[%s][%d]acc_v=[%s] name_v=[%s]",__FILE__,__LINE__,acc_v,name_v);
    
  vtcp_log("[%s][%d] tx_br_no=[%.5s]",__FILE__,__LINE__,tx_br_no);

  ret = iGetReceTel(tx_br_no);    /*  柜员号 ==> 0070 */
  if ( ret )
  {
    cStat[0] = STAT_LZGZ_BR;     /* 'N'  信息不符，支行挂帐*/
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:09:43 */
    memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:09:45 */
    /* end NEWTCTD1 */    
    
    sprintf(acErrMsg,"得到柜员号错误,挂账!");
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return 100;
  }
  get_zd_data("0070",tx_tel); /* 由上一步得到的柜员号*/
  vtcp_log("[%s][%d] tx_tel=[%.5s]",__FILE__,__LINE__,tx_tel);

  /*  收款人 帐号 户名 帐号状态的检查  */
  /* iHvGetRece_AcValue(hv_pack,acc_v,name_v);add by LiuHuafeng 2006-9-1 20:22 */
  memcpy(acc_v,payin->F_cashactno,sizeof(acc_v)-1);
  pub_base_old_acno(acc_v);
  pub_base_strpack_all(acc_v);
  memcpy(name_v,payin->F_cashname,sizeof(name_v)-1);
  pub_base_strpack_all(name_v);
  vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
  vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);
  vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
  vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);

  /* 收款人账号检查开始 */
  memset( &mdm_ac_rel_v, 0x00, sizeof(struct mdm_ac_rel_c));
  g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
  if (g_reply_int==100)
  {
    cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:09:49 */
    memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:09:51 */
    /* end NEWTCTD1 */    
    
    sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return 100;
  }
  else if (g_reply_int)
  {
    payin->T_stat[0]=STAT_LZGZ_QS;
    sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:09:55 */
    memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:09:57 */
    /* end NEWTCTD1 */
    
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return -1;
  }
  /**add by wangyongwei 20070912**/
    #ifdef CZ_XQ
  if(memcmp(payin->F_pkgno,PKGNO_SSDJ,3)==0){
    char Ac_Br_No[13];
    char Js_brno[6];
    struct tctd_ktzh_c cTctd_ktzh;
    memset(&cTctd_ktzh,0,sizeof(struct tctd_ktzh_c));
    memset(Ac_Br_No,0,sizeof(Ac_Br_No));
    memset(Js_brno,0,sizeof(Js_brno));
    memcpy(Ac_Br_No,payin->F_acbrno,12);
    g_reply_int= GetBr_noByOr_br_no(Ac_Br_No,Js_brno);
    if(memcmp(mdm_ac_rel_v.opn_br_no,Js_brno,5)!=0){
        vtcp_log("%s,%d接收行非账号的开户行长治不允许这样!\n",__FILE__,__LINE__);
              cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
              payin->T_stat[0]=STAT_LZGZ_BR;
              set_zd_data("0980",cStat);      /** 来账业务状态 **/
              memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
              set_zd_data("0890",cRcpstat);
              sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
              vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
              WRITEMSG
              return 999;
    }
    /* add 20071112 NEWTCTD5 wangyongwei检查收款帐号开通通存业务*/
        g_reply_int=Tctd_ktzh_Sel(g_pub_tx.reply,&cTctd_ktzh,"tctd_acno='%s' and tc_kt_flag='1'",mdm_ac_rel_v.ac_no);
        if(g_reply_int){
            cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
            payin->T_stat[0]=STAT_LZGZ_BR;
            set_zd_data("0980",cStat);      /** 来账业务状态 **/
            memcpy(cRcpstat,"090",sizeof(cRcpstat)-1);
            set_zd_data("0890",cRcpstat);
            sprintf(acErrMsg,"收款人帐号未开通通存业务[%s]!",acc_v);
            vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
            WRITEMSG
            return 100;
        }
   }
    #endif
    /**add by wangyongwei 20070912**/
  /** begin 检查介质的状态 by LiuHuafeng 2006-10-5 17:51 */
  if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
  {
    /* 介质状态不正常挂帐处理 */
    cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:10:03 */
    memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:10:04 */
    /* end NEWTCTD1 */    
    
    sprintf(acErrMsg,"大额来账收款人账号[%s]介质状态错误[%s]",acc_v,mdm_ac_rel_v.note_sts[0]);
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return 100;
  }
  /* end by LiuHuafeng 2006-10-5 17:52 */
  memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
  if(mdm_ac_rel_v.ac_seqn==9999){/* 多帐号介质默认主帐户 1 */
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and ac_id_type='1'",mdm_ac_rel_v.ac_id );
  } else {
    /* rem by LiuHuafeng 2007-1-16 10:44
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and ac_id_type=%d",mdm_ac_rel_v.ac_id,mdm_ac_rel_v.ac_seqn);
    ****** end by LiuHuafeng 2007-1-16 10:44  replace by next line */
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld'",mdm_ac_rel_v.ac_id);
  }
  if (g_reply_int==100)
  {
    cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/    
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
    memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:10:21 */
    /* end NEWTCTD1 */    
    
    sprintf(acErrMsg,"大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return 100;
  }
  else if (g_reply_int)
  {
    payin->T_stat[0]=STAT_LZGZ_BR;
    set_zd_data("0980",cStat);      /** 来账业务状态 **/  
    
    /* begin NEWTCTD1 */
    /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
    memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
    set_zd_data("0890",cRcpstat);
    /* end by LiuHuafeng 2007-8-23 18:10:21 */
    /* end NEWTCTD1 */
      
    sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
    vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
    WRITEMSG
    return -1;
  }
  /* 根据币种生成账户序号  
  g_reply_int = pub_base_CurToSeqn( "01", &ac_seqn_v );
  if ( g_reply_int )
  {
  sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
  WRITEMSG
  return -1;
  } */
  if(prdt_ac_id_v.ac_id_type[0]=='1')
  {
    ac_seqn_v  =  1;    
    g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
    if (g_reply_int)
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/    
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */      
      
      sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return -1;
    }
   /* if (dd_mst_v.ac_sts[0] != '1' || dd_mst_v.hold_sts[0] != '0')  冻结只挂账完全冻结 20140311 wudawei */
    if (dd_mst_v.ac_sts[0] != '1' || dd_mst_v.hold_sts[0] == '1')
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/    
      
     /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */
            
      sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;
    }
    /*begin 判断是否别冻结 add by LiuHuafeng 2006-10-5 10:57 */
    if(dd_mst_v.hold_sts[0]=='1' )
    {
      /* 帐号id被完全冻结所以要挂帐 */
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"011",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */
      
      sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;
    }
    /* end by LiuHuafeng 2006-10-5 11:23 */
    /* begin 判断是否是结算户 by LiuHuafeng 2006-10-5 11:24 */
    if(dd_mst_v.sttl_type[0]=='S')
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */      
      
      sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;
    }
    /* end by LiuHuafeng 2006-10-5 11:24 */
    pub_base_strpack(dd_mst_v.name);
    pub_base_strpack(name_v);
    if (strcmp(dd_mst_v.name,name_v))
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"002",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */
      
      sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,name_v);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;
    }
    set_zd_data("0680","1");
  }/* begin add by LiuHuafeng 2006-10-5 17:53 检查内部帐户 */
  else
  {
    struct in_mst_c sIn_mst;
    g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
    if (g_reply_int)
    {
      cStat[0] = STAT_LZGZ_QS;     /* 'M'  信息不符，清算中心挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */
      
      sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;/** add by martin 20071206 原为-1 **/
    }
    if (sIn_mst.sts[0] != '1' )
    {
      cStat[0] = STAT_LZGZ_BR;   /* N' 信息不符，支行挂帐*/
      payin->T_stat[0]=STAT_LZGZ_BR;
      set_zd_data("0980",cStat);      /** 来账业务状态 **/
      
      /* begin NEWTCTD1 */
      /* begin add by LiuHuafeng 2007-8-23 18:10:19 */
      memcpy(cRcpstat,"001",sizeof(cRcpstat)-1);
      set_zd_data("0890",cRcpstat);
      /* end by LiuHuafeng 2007-8-23 18:10:21 */
      /* end NEWTCTD1 */
      
      sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
      vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
      WRITEMSG
      return 100;
    }
    set_zd_data("0680","6");/* 设置帐户类型 */
  }
  /**add by xyy YHBP
  vtcp_log("[%s][%d]请根我到新疆来F_txnum=[%.5s]\n",__FILE__,__LINE__,payin->F_txnum);
  if(!memcmp(payin->F_pkgno,"010",3) && !memcmp(payin->F_txnum,"30103",5))
  {
        set_zd_data("1011",acc_v);
      set_zd_data("1012","0");
      set_zd_data("101A","01");
      set_zd_data("101B","2");
      set_zd_double("1013",dTxamt);
      set_zd_data("1016","本票来帐");
  }
  else
  {
  **modify by xyy YHBP**/
  /* 正常则置8583域 */
  set_zd_data("0300", acc_v );  /* 收款人账号 */ 
  set_zd_data("0210", "01");     /* 币种 */      
  set_zd_data("0980",cStat);      /** 来账业务状态 **/
  
  /* begin NEWTCTD1 */
  /* begin add by LiuHuafeng 2007-8-24 17:16:57 */
  set_zd_data("0890",cRcpstat);
  /* end by LiuHuafeng 2007-8-24 17:17:03 */
    /* end NEWTCTD1 */
  
  payin->T_stat[0]=cStat[0];
  vLvSetBrnoAndSts_Fd(payin->T_brno,cStat,__FILE__,__LINE__);
  return 0;
}


/***************************************************************************/
/* 函数名称: pubf_lv_set_8583()                                            */
/* 参数:                                                                   */
/* 功能描述:    小额来帐 设置8583域                                        */
/*                                                                         */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_lv_set_8583(TLVPACKAGE* plv_pack, char* cmtno, int hangflg)
{
  double amount_v = 0.0;
  char    cTmpBuf[19];
  char    cAmount[16]; 

  memset(cAmount,0x00,sizeof(cAmount));     
  memset(cTmpBuf,0x00,sizeof(cTmpBuf));    
  vtcp_log(" --- pubf_lv_set_8583() Start !! ");    

  if ( hangflg == 1 )  /* 挂帐 */
  {
    /* set_zd_data("0680","0");    ??? */

    strcpy( g_pub_tx.reply, "0000" );  /* 转到 Jtst设置8583域走帐  */
    vtcp_log(" --- 挂帐 --- =[%s][%d] ",__FILE__,__LINE__);        
  }
  else  /* 不挂帐 */
  {
    strcpy( g_pub_tx.reply, "0000" );  /* 转到 Jtst设置8583域走帐  */
    set_zd_data("0680","1"); 
    vtcp_log(" --- 正常走帐 --- =[%s][%d] ",__FILE__,__LINE__);           
  }
  set_zd_data("0510",cmtno);         
  set_zd_data("0210","01");
  set_zd_data("0680","1");
  return 0;     
}

/***************************************************************************/
/* 函数名称: pubf_lv_handle_fzf_cmt()                                      */
/* 参数:                                                                   */
/* 功能描述:                                                               */
/*                                                                         */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_lv_handle_fzf_cmt(char* cmtno)
{
  strcpy( g_pub_tx.reply, "0010" );  
  set_zd_data("0510",cmtno);    
  return 0;      
}

/***************************************************************************/
/* 函数名称: pub_lv_recv_reg_main()                                        */
/* 功能描述: spJ099 小额来帐－登记来帐  主函数                             */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.09.12.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_lv_recv_reg_main(char* chkcmtno)
{
  int ret = 0;
  char Stat = '';
  char cmtno[4 ];      /** 报文编号**/   
  TLVPACKAGE lv_pack; 
  char cFinishflg='';       /* PKG 批量包包结束标志 '0'=PKG包还未处理完毕   '1'=PKG包已处理完毕*/
  int  iSubpackIndex = 0;   /* PKG包中子报文序号 */
  int  iStep=0;        /* CMT包中子报文序号 */

  memset(cmtno,'\0',sizeof(cmtno)); 
  get_zd_data("0510",cmtno);  
  vtcp_log("========  cmtno==[%s]   ========",cmtno);    

  get_zd_data("0500",&cFinishflg);
  get_zd_long("0520",&iSubpackIndex);
  get_zd_long("0530",&iStep);  
  
  vtcp_log("========  cFinishflg=[%c]    iSubpackIndex=[%d] ========",cFinishflg,iSubpackIndex);  
  vtcp_log("========  iStep=[%d]   ========",iStep);  
  
  vLvBrnoDebug(__FILE__,__LINE__);
  
  /*   非支付类循环走帐  */
  if( ( memcmp(cmtno,"322",3) ==0 ) ||   ( memcmp(cmtno,"324",3) ==0 ) ||
      ( memcmp(cmtno,"325",3) ==0 ) ||   ( memcmp(cmtno,"552",3) ==0 ) ||
      ( memcmp(cmtno,"554",3) ==0 ) ||   ( memcmp(cmtno,"920",3) ==0 ) )
  {
      if ( iStep != -1 )
      {                  
            vtcp_log("======== 来报尚未完毕，转到J098继续走帐处理 !!!! ========");  
            memcpy(g_pub_tx.reply,"0010",4); /* 不再循环了就置 0000， 否则置0010  */
            return 0;         
       }
  }
  
  if ( (cFinishflg != '0') && (cFinishflg != '1') )
  {    
    set_zd_data("0500","1");       /* 出口：置为结束 */
    sprintf(acErrMsg,"cFinishflg为错误的结束标志 ! ");
    WRITEMSG  
    return -1;
  }   
  if (cFinishflg == '1')
  {
    vtcp_log("======== 来报循环处理完毕，这里是程序结束的出口 !!!! ========");  
    memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了就置 0000， 否则置0010  */
    set_zd_data("0500","1");       /* 报文处理完毕 */ 
    return 0;
  }
  memset(&lv_pack, 0 , sizeof(lv_pack));
  lv_create(&lv_pack);
  vLvBrnoDebug(__FILE__,__LINE__);
  if ( pubf_lv_read_pack(&lv_pack) )   /* 读来帐报文  */
  {
    sprintf(acErrMsg,"读来帐报文出错 ! ");
    WRITEMSG  
    lv_destroy(&lv_pack);          
    return -1;
  }
  vLvBrnoDebug(__FILE__,__LINE__);
  /* pubf_lv_print_dm(&lv_pack.dm); */
  ret = lv_analyze_pack(&lv_pack);
  if ( ret )  /** 检查报文格式是否合法 **/ 
  {
    sprintf(acErrMsg,"接收到的报文格式错误 [%d] ", ret);
    WRITEMSG  
    lv_destroy(&lv_pack);          
    return -1;
  }
  vLvBrnoDebug(__FILE__,__LINE__);
  vtcp_log(" ============= 接收到的报文格式正确 ============= ");    
  vtcp_log("[%s][%d]=====Recievee length==[%d]==[%d]",__FILE__,__LINE__,lv_pack.length, lv_pack.dm.length);
  vtcp_log("[%s][%d]=====此笔传入的报文是==[%.3s]  =报文类型==%d", __FILE__,__LINE__,lv_pack.CMTCode,lv_pack.pkgType);

  /*  大额来帐记账机构号及柜员号赋值  */ 
  /* For Test !!!  */
  /*set_zd_data("0040", "000111");       流水号 */  
  vtcp_log("========  iSubpackIndex=[%d] ========",cFinishflg,iSubpackIndex);  
  vtcp_log("========  iStep=[%d]   ========",iStep);  

  if ( iSubpackIndex == 0 || iStep == -1)
  {      
    ret = lv_insRecvList(&lv_pack);
    if (ret)
    {
      sprintf(acErrMsg,"来帐插入登记簿失败 [%d] ", ret);
      WRITEMSG 
      lv_destroy(&lv_pack);
      /* begin add by LiuHuafeng for 20090515 村镇银行 *
      * 如果是920 并且插入lv_recvlist重复 那么认为已经处理过，是成功的
      * 设置 finish=1,0500,step = -1 0530 返回 0
      * 20090515
      */
      vtcp_log("%s,%d cmtno=== %s, ret = %d",__FILE__,__LINE__,cmtno,ret);
      if(memcmp(cmtno,"920",3) ==0 && ret == -239)
      {
         vtcp_log("%s,%d 是920 重复插入报文，认为正确直接返回0 ",__LINE__,__FILE__);
         /* 将处理数据回滚 */
         sql_rollback();
         strncpy(g_pub_tx.reply,"0000",4);
         set_zd_long("0500",1);
         set_zd_long("0530",-1);
         return 0;
      }
      /* end by LiuHuafeng 20090515 */
      return -1;
    }
    vtcp_log("%s,%d, 来帐插入登记簿成功，开始进入分别处理....\n",__FILE__,__LINE__);
  }
  if (lv_pack.pkgType)  /* 是PKG包 */
  {
    vtcp_log("[%s][%d] 是PKG包 ",__FILE__,__LINE__);   
    if ( pubf_lv_handle_pkg(&lv_pack) )   /*  小额来帐 PKG 包的处理   */
    {
      vtcp_log("[%s][%d] pubf_lv_handle_pkg() 处理小额来帐PKG包错误 pkgno=[%s]",__FILE__,__LINE__,lv_pack.CMTCode);
      sprintf(acErrMsg," 处理小额来帐 PKG 包错误 !  ");
      WRITEMSG
      lv_destroy(&lv_pack);            
      return -1;                    
    }
    memcpy(g_pub_tx.reply,"0010",4); /*  置0010，跳转到J098处理下一条明细 */
  }
  else  /* 是CMT包 */
  {
    vtcp_log("[%s][%d] 是CMT包 ",__FILE__,__LINE__);
    if ( pubf_lv_handle_cmt(&lv_pack) )   /*  小额来帐 CMT 包的处理   */
    {
      vtcp_log("[%s][%d] pubf_lv_handle_cmt() 处理小额来帐CMT包错误 lv_pack.CMTCode=[%s]",__FILE__,__LINE__,lv_pack.CMTCode);
      sprintf(acErrMsg," 处理小额来帐 CMT 包错误 !  ");
      WRITEMSG
      lv_destroy(&lv_pack);            
      return -1;                    
    }
    memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了就置 0000，CMT不用循环  */
    set_zd_data("0500","1");         /* 报文处理完毕 */ 
  }  
  lv_destroy(&lv_pack);  
  return 0;
}

/*  小额来帐 PKG 包的处理  */
int pubf_lv_handle_pkg(TLVPACKAGE* plv_pack)
{
  int ret;
  char pcPayqsactno[13];
  char pcBfFlag[2];
  int  iSubpackIndex = 0;   /* 包中子报文序号 */
  struct lv_wbctl_c wd_lv_wbctl;
  vtcp_log("[%s][%d] 是PKG包 ",__FILE__,__LINE__);      

  /****根据发起清算行判断是来帐还是往帐, 因为可能有对帐时补发的往帐****/
  memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));  
  memset(pcPayqsactno,'\0',sizeof(pcPayqsactno));
  memset(pcBfFlag,'\0',sizeof(pcBfFlag));          
  lv_pkg_getHeaderTag(plv_pack,"011",pcPayqsactno);
  lv_pkg_getHeaderTag(plv_pack,"BS4",pcBfFlag);
  vtcp_log("[%s][%d] pcPayqsactno=[%s]  pcBfFlag=[%s]  ",__FILE__,__LINE__,pcPayqsactno,pcBfFlag);
  vtcp_log("%s,%d 注意 iSubpackIndex=变化===[%d]",__FL__,iSubpackIndex);

  if(pcBfFlag[0]=='1')    /****是补发的包****/
  {
    vtcp_log("[%s][%d] 是补发的包 ",__FILE__,__LINE__);   
    /****是补发的往帐,往帐多帐肯定出错了,挂帐处理即可****/
    if (memcmp(pcPayqsactno,HV_QS_BR_NO,sizeof(pcPayqsactno)-1)==0)
    {
      /*begin add by LiuHuafeng NEWYX 20070412 */
      int iLwCheck =0;
      iLwCheck =0;
      /*enn add by LiuHuafeng NEWYX 20070412 */
      vtcp_log("[%s][%d] 是补发的往帐 \n",__FILE__,__LINE__);
      if(iSubpackIndex == 0 )
      {
          ret = lv_setwbctl_from_pack(plv_pack,&wd_lv_wbctl);
          if (ret)
          {
            sprintf(acErrMsg,"产生往包控制表记录错 [%d] ", ret);
            WRITEMSG  
            return -1;
          }
          ret = Lv_wbctl_Ins(wd_lv_wbctl,g_pub_tx.reply);
          /*begin add by LiuHuafeng NEWYX 20070412 */
          if(ret == -239) 
          {
          	/*************delete by xyy 2010-8-13 23:02:14 FORCZ
            vtcp_log("%s,%d 按照来帐处理，往帐已经逐渐冲突 ",__FILE__,__LINE__);
            iLwCheck =1;
            end by xyy 2010-8-13 23:02:26 FORCZ**************/
             /*  往帐少帐一般不会发生，如果发生了，只插入往包控制表 
            ----------可能是村镇银行的往帐 add by xyy2010-8-13 23:01:43 FORCZ */
            set_zd_long("0520", iSubpackIndex+1);   /* 登记完了 iSubpackIndex + 1, 取下一个明细 */
            memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了!!!!   置 0000， 否则置0010  */
            set_zd_data("0500","1");         /* 报文处理完毕 */ 
            return 0;
            /************end by xyy 2010-8-13 23:01:55  FORCZ************/
          }
          /* end add by LiuHuafeng NEWYX 20070412 */
          else if (ret)
          {
            sprintf(acErrMsg,"插入往包控制表错!ret==[%d]\n",ret);
            WRITEMSG
              strcpy(g_pub_tx.reply,"HV01");
            return (-1);
          }   
      }
      if( iLwCheck == 0)
      {
          /*  往帐少帐一般不会发生，如果发生了，只插入往包控制表 */
          set_zd_long("0520", iSubpackIndex+1);   /* 登记完了 iSubpackIndex + 1, 取下一个明细 */
          memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了!!!!   置 0000， 否则置0010  */
          set_zd_data("0500","1");         /* 报文处理完毕 */ 
          return 0;
      }
    }
    else  /****  ??  补发的来帐和正常的来包一样处理  ?? ****/
    {
      /* ??  */
      vtcp_log("[%s][%d] 是补发的来帐 \n",__FILE__,__LINE__);      
      vtcp_log("[%s][%d] pcPayqsactno=[%s]  pcBfFlag=[%s]  ",__FILE__,__LINE__,pcPayqsactno,pcBfFlag);
    }
  
  }
  /**** else 是正常的来包 ****/

  {
    vtcp_log("[%s][%d] 是正常的来帐或补发来帐 PKGNO=[%.3s] ",__FILE__,__LINE__,plv_pack->CMTCode);     
    /* rem by LiuHuafeng 20070406
    if (memcmp(pcPayqsactno, HV_QS_BR_NO, sizeof(pcPayqsactno)-1) != 0)
    {
    *************************************************************/
      /****是正常的来帐或补发来帐****/
      if (pcBfFlag[0]=='1')  vtcp_log("[%s][%d] 是补发的来帐 \n",__FILE__,__LINE__);
      else                  vtcp_log("[%s][%d] 是正常的来帐 \n",__FILE__,__LINE__);

      if (memcmp(plv_pack->CMTCode,PKGNO_PTDJ,3)==0)
      {
        ret=lv_deal_ptdjlz(plv_pack,0);   /* chkflg=0 登记来帐报文 */
        get_zd_long("0520",&iSubpackIndex);
        vtcp_log("%s,%d out ptdjlz  注意 iSubpackIndex=变化===[%d]",__FL__,iSubpackIndex);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_DQDJ,3)==0)
      {
        ret=lv_deal_dqdjlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_SSDJ,3)==0)
      {
        ret=lv_deal_ssdjlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_DJTH,3)==0)
      {
        ret=lv_deal_djthlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_PTJJ,3)==0)
      {
        ret=lv_deal_ptjjlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_DQJJ,3)==0)
      {
        ret=lv_deal_dqjjlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_SSJJ,3)==0)
      {
        ret=lv_deal_ssjjlz(plv_pack,0);
      }      
      else if(memcmp(plv_pack->CMTCode,PKGNO_PTJJHZ,3)==0)
      {
        ret=lv_deal_ptjjhzlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_DQJJHZ,3)==0)
      {
        ret=lv_deal_dqjjhzlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_SSDJHZ,3)==0)
      {
        ret=lv_deal_ssdjhzlz(plv_pack,0);
      }      
      else if(memcmp(plv_pack->CMTCode,PKGNO_SSJJHZ,3)==0)
      {
        ret=lv_deal_ssjjhzlz(plv_pack,0);
      }      
      else if(memcmp(plv_pack->CMTCode,PKGNO_SSXX,3)==0)
      {
        ret=lv_deal_ssxxlz(plv_pack,0);
      }
      else if(memcmp(plv_pack->CMTCode,PKGNO_TYXX,3)==0)
      {
        ret=lv_deal_tyxxlz(plv_pack,0);
      } 
      else
      {
        vtcp_log("%s,%d,不能处理的 PKGNO:[%.3s]\n",__FILE__,__LINE__, plv_pack->CMTCode);
        sprintf(acErrMsg," 不能处理的 PKGNO:[%.3s] ",plv_pack->CMTCode);
        WRITEMSG  
        return -1;
      }
      vtcp_log("%s,%d out ptdjlz  注意 iSubpackIndex=变化===[%d]",__FL__,iSubpackIndex);
    /** rem by LiuHuafeng 20070406
    }
    else
    {
      vtcp_log("%s,%d,怎么处理   ？？？ PKGNO:[%.3s]\n",__FILE__,__LINE__,plv_pack->CMTCode);
    }
    ******************************************************/
    vtcp_log("%s,%d out ptdjlz  注意 iSubpackIndex=变化===[%d]",__FL__,iSubpackIndex);
    if (ret)
    {
      vtcp_log("%s,%d,CMTNO[%.3s]来帐处理失败\n",__FILE__,__LINE__,plv_pack->CMTCode);
      sprintf(acErrMsg," 来帐CMTNO=[%.3s]处理失败 !  ",plv_pack->CMTCode);
      WRITEMSG
        return -1;
    }
    vtcp_log("%s,%d out ptdjlz  注意 iSubpackIndex=变化===[%d]",__FL__,iSubpackIndex);
  }
  vtcp_log("%s,%d out ptdjlz  注意 iSubpackIndex==应该变化===[%d]",__FL__,iSubpackIndex);
  return 0;      
}

/*  小额来帐 CMT 包的处理  登记到  hv_fzf  记帐完了，最后才跳到这一步 */
int pubf_lv_handle_cmt(TLVPACKAGE* plv_pack)
{
  int ret;
  vtcp_log("[%s][%d] cmtno=[%.3s] ",__FILE__,__LINE__,plv_pack->CMTCode);
  vtcp_log("[%s][%d]==lv->packDate=[%.8s]",  __FILE__,__LINE__,plv_pack->packDate);     
  /* add by LiuHuafeng 2006-10-14 8:24 设置cmt号 */
  
  if (memcmp(plv_pack->CMTCode,"301",3)==0)   /****查询****/
  {
    ret=lv_deal_cmt301(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"302",3)==0)  /****查复****/  
  {
    ret=lv_deal_cmt302(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"303",3)==0)   /****自由格式****/  
  {
    ret=lv_deal_cmt303(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"319",3)==0)  /****小额业务申请退回****/
  {
    ret=lv_deal_cmt319(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"320",3)==0)   /****小额业务退回应答****/  
  {
    ret=lv_deal_cmt320(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"322",3)==0)  /****小额业务撤销应答****/  
  {
    ret=lv_deal_cmt322(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"324",3)==0)  /****实时业务冲正应答****/  
  {
    ret=lv_deal_cmt324(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"325",3)==0)  /****实时业务冲正通知****/  
  {
    ret=lv_deal_cmt325(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"327",3)==0)  /****止付申请****/  
  {
    ret=lv_deal_cmt327(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"328",3)==0)  /****支付应答****/  
  {
    ret=lv_deal_cmt328(plv_pack); 
  }
  else if(memcmp(plv_pack->CMTCode,"254",3)==0)  /****清算通知****/  
  { 
    ret=lv_deal_cmt254(plv_pack); 
  }
  else if(memcmp(plv_pack->CMTCode,"920",3)==0)  /**批量包处理确认通知**/  
  {
    ret=lv_deal_cmt920(plv_pack);   
  }
  else if(memcmp(plv_pack->CMTCode,"911",3)==0)  /****批量包处理确认通知****/  
  {
    ret=lv_deal_cmt911(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"316",3)==0)  /****批量包处理状态通知****/
  {
    ret=lv_deal_cmt316(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"552",3)==0)  /****收费通知****/  
  {
    ret=lv_deal_cmt552(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"554",3)==0)  /****计费对外返还通知****/  
  {
    ret=lv_deal_cmt554(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"500",3)==0)  /****系统状态变更****/  
  {
    ret=lv_deal_cmt500(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"502",3)==0)  /****公共数据变更****/  
  {
    ret=lv_deal_cmt502(plv_pack);
  }
    /******* Begin Added by ChenMing 2007.04.14. NEWYX *****/
  else if(memcmp(plv_pack->CMTCode,"503",3)==0)  /***业务权限控制通知报文***/  
  {
    ret=lv_deal_cmt503(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"504",3)==0)  /***下级节点变更通知报文***/  
  {
    ret=lv_deal_cmt504(plv_pack);
  }    
    /******* End  Added by ChenMing 2007.04.14. NEWYX *****/

  else if(memcmp(plv_pack->CMTCode,"507",3)==0)  /****贷记业务金额上限变更****/
  {
    ret=lv_deal_cmt507(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"516",3)==0)  /****净借记控制可用额度预警通知****/  
  {
    ret=lv_deal_cmt516(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"842",3)==0)  /****小额业务包汇总核对****/
  {
    ret=lv_deal_cmt842(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"508",3)==0)  /****票交机构代理行变更报文****/
  {
    ret=lv_deal_cmt508(plv_pack);
  }/**add by xyy 2007-4-15 22:24 NEWYX**/
  else if(memcmp(plv_pack->CMTCode,"509",3)==0)  /****票交机构代理行变更报文****/
  {
    ret=lv_deal_cmt509(plv_pack);
  }
  else if(memcmp(plv_pack->CMTCode,"851",3)==0)  /****小额业务包明细核对应答****/
  {
    char cDzday[9];
    memset(cDzday, 0 , sizeof(cDzday));
    get_zd_data(DC_DATE,cDzday);
    ret=lv_deal_cmt851(plv_pack,cDzday);
  }
  else
  {
    /****TODO:为人行验收准备垃圾报文处理****
    vtcp_log("%s,%d,不能处理的CMTNO:[%.3s]\n",__FILE__,__LINE__,plv_pack->CMTCode);
    sprintf(acErrMsg," 不能处理的CMTNO:[%.3s] ",plv_pack->CMTCode);
    WRITEMSG        
    return -1;
    ****TODO END****/
    ret=lv_deal_oths(plv_pack);
  }
  if (ret)
  {
    vtcp_log("%s,%d,CMTNO[%.3s]来帐处理失败\n",__FILE__,__LINE__,plv_pack->CMTCode);
    sprintf(acErrMsg," 来帐CMTNO=[%.3s]处理失败 !  ",plv_pack->CMTCode);
    WRITEMSG
    return -1;
  }  

    memcpy(g_pub_tx.reply,"0000",4); /* 不再循环了就置 0000， 否则置0010  */  
    return 0;
}

int pubf_lv_print_dm(TDMEM * dm)
{
  TDBLOCK* pblock;
  vtcp_log("=====================  dm 中的内容  =================");  
  vtcp_log("dm->blockNum==[%d]", dm->blockNum);
  vtcp_log("dm->length==[%d]", dm->length);     
  vtcp_log("dm->curNum==[%d]", dm->curNum);  
  vtcp_log("dm->curBlock->blockNo==[%d]", dm->curBlock->blockNo);  
  vtcp_log("============= 以下为 dm->block 中的内容 =============\n");  
  pblock = &dm->block;
  while (pblock)
  {
    vtcp_log("======dm->block.blockNo==[%d]==============", pblock->blockNo);
    vtcp_log("dm->block.length==[%d]", pblock->length);     
    vtcp_log("dm->block.mem==[%.1024s]\n", pblock->mem);            
    pblock = pblock->next;
  }   
}

int pubf_lv_print_NOTPAY_IN_AREA(NOTPAY_IN_AREA * pNotpay_in)
{
  vtcp_log("pNotpay_in->optype =[%.2s]",    pNotpay_in->optype);
  vtcp_log("pNotpay_in->pkgno  =[%.3s]",    pNotpay_in->pkgno);
  vtcp_log("pNotpay_in->cmtno  =[%.3s]",    pNotpay_in->cmtno);     
  vtcp_log("pNotpay_in->orderno=[%.8s]",    pNotpay_in->orderno);
  vtcp_log("pNotpay_in->txnum  =[%.5s]",    pNotpay_in->txnum);
  vtcp_log("pNotpay_in->acbrno =[%.12s]",   pNotpay_in->acbrno);

  vtcp_log("pNotpay_in->rettype =[%.c]",    pNotpay_in->rettype);
  vtcp_log("pNotpay_in->respcode=[%c]",     pNotpay_in->respcode);
  vtcp_log("pNotpay_in->opackday=[%.8s]",   pNotpay_in->opackday);
  vtcp_log("pNotpay_in->opkgno  =[%.3s]",   pNotpay_in->opkgno);
  vtcp_log("pNotpay_in->opackid =[%.8s]",   pNotpay_in->opackid);
  vtcp_log("pNotpay_in->otxnum  =[%.5s]",   pNotpay_in->otxnum);

  vtcp_log("pNotpay_in->owtday  =[%.8s]",   pNotpay_in->owtday);
  vtcp_log("pNotpay_in->oorderno=[%.8s]",   pNotpay_in->oorderno);
  vtcp_log("pNotpay_in->oorbrno =[%.12s]",  pNotpay_in->oorbrno );
  vtcp_log("pNotpay_in->oacbrno =[%.12s]",  pNotpay_in->oacbrno );
  vtcp_log("pNotpay_in->otxtype =[%.c]",    pNotpay_in->otxtype );
  vtcp_log("pNotpay_in->cour    =[%.3s]",   pNotpay_in->cour    );
  vtcp_log("pNotpay_in->otxamt  =[%.15s]",  pNotpay_in->otxamt  );
  vtcp_log("pNotpay_in->content =[%.255s]", pNotpay_in->content );
  vtcp_log("pNotpay_in->refno   =[%.20s]",  pNotpay_in->refno   );

  vtcp_log("pNotpay_in->unit      =[%.8s]",  pNotpay_in->unit      );
  vtcp_log("pNotpay_in->belongcyc =[%.8s]",  pNotpay_in->belongcyc );
  vtcp_log("pNotpay_in->sftype    =[%.12s]", pNotpay_in->sftype    );
  vtcp_log("pNotpay_in->pdate     =[%.12s]", pNotpay_in->pdate     );
  vtcp_log("pNotpay_in->pnum      =[%.20s]",   pNotpay_in->pnum      );
  vtcp_log("pNotpay_in->paybrname =[%.3s]",  pNotpay_in->paybrname );
  vtcp_log("pNotpay_in->actno     =[%.15s]", pNotpay_in->actno     );
  vtcp_log("pNotpay_in->name      =[%.255s]",pNotpay_in->name      );
  vtcp_log("pNotpay_in->reason    =[%.20s]", pNotpay_in->reason    );

  vtcp_log("pNotpay_in->bsnum    =[%.2s]",   pNotpay_in->bsnum   );
  vtcp_log("pNotpay_in->passwd   =[%.60s]",  pNotpay_in->passwd  );
  vtcp_log("pNotpay_in->datalen  =[%.8s]",   pNotpay_in->datalen );
  vtcp_log("pNotpay_in->acttype  =[%.c]",    pNotpay_in->acttype );
  vtcp_log("pNotpay_in->pswdcd   =[%.c]",    pNotpay_in->pswdcd  );
  vtcp_log("pNotpay_in->pascode  =[%.128s]", pNotpay_in->pascode );
  vtcp_log("pNotpay_in->qtype    =[%.c]",    pNotpay_in->qtype   );

  vtcp_log("pNotpay_in->orbrno      =[%.12s]",  pNotpay_in->orbrno     );
  vtcp_log("pNotpay_in->payqsactno  =[%.12s]",  pNotpay_in->payqsactno );
  vtcp_log("pNotpay_in->sendco      =[%.4s]",   pNotpay_in->sendco     );
  vtcp_log("pNotpay_in->cashqsactno =[%.12s]",  pNotpay_in->cashqsactno);
  vtcp_log("pNotpay_in->receco      =[%.4s]",   pNotpay_in->receco     );
  vtcp_log("pNotpay_in->wssrno      =[%.6s]",   pNotpay_in->wssrno     );
  vtcp_log("pNotpay_in->txday       =[%.8s]",   pNotpay_in->txday      );
  vtcp_log("pNotpay_in->stat        =[%.c]",    pNotpay_in->stat       );
  vtcp_log("pNotpay_in->datatype    =[%.c]",    pNotpay_in->datatype   );

  vtcp_log("pNotpay_in->addid     =[%.8s]",  pNotpay_in->addid    );
  vtcp_log("pNotpay_in->qorderno  =[%.8s]",  pNotpay_in->qorderno );
  vtcp_log("pNotpay_in->qwtday    =[%.8s]",   pNotpay_in->qwtday   );
  vtcp_log("pNotpay_in->qorbrno   =[%.12s]",  pNotpay_in->qorbrno  );
  vtcp_log("pNotpay_in->racbrno   =[%.12s]",   pNotpay_in->racbrno  );
  vtcp_log("pNotpay_in->rwtday    =[%.12s]",   pNotpay_in->rwtday   );
  vtcp_log("pNotpay_in->rorderno  =[%.12s]",   pNotpay_in->rorderno );
  vtcp_log("pNotpay_in->totamt    =[%.15s]",    pNotpay_in->totamt   );
  vtcp_log("pNotpay_in->totcnt    =[%.8s]",    pNotpay_in->totcnt   );
  return 0;
}

void vLvSetBrnoAndSts_Fd(char * brno,char * stat,char * file,int line)
{
  char cKinbr[BRNO_LEN+1];
  char cStat[2];
  memset(cKinbr, 0 , sizeof(cKinbr));
  memset(cStat, 0 , sizeof(cStat));
  memcpy(cKinbr,brno,sizeof(cKinbr)-1);
  memcpy(cStat,stat,sizeof(cStat)-1);
  set_zd_data(DC_TX_BR_NO,cKinbr);
  set_zd_data("0980",cStat);
  vtcp_log("%s,%d 设置kinbr=[%s] 状态 ==[%c] ",file,line,cKinbr,cStat[0]);
  iGetReceTel(cKinbr);    /*  柜员号 ==> 0070 */
}

/*******************************************************************************
* 函数名称: iLvHang_Check_Acno                                                 *
* 描    述: 挂帐检查，检查帐号及状态，以及户名 
            设置好一些8583域，为走帐准备
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
* 补充说明: NEWTCTD9,拷贝自大额，只是改了挂帐科目
*******************************************************************************/
int iLvHang_Check_Acno(char* ac_no, char* ac_name, char* cKinbr)
{
  char stat;
  char cGZflag[2];
  int ret;
  struct com_branch_c sCom_branch;
    char cQs_br_no[BRNO_LEN+1];
  char  acc_v[33];
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;  
  int   ac_seqn_v; 
    char    cTempstr    [1024];
    
    char cOldStat[2];
  memset(cOldStat, 0, sizeof(cOldStat));        
    get_zd_data("0980",cOldStat);     /* 记录原状态 */
  vtcp_log("[%s][%d] cOldStat===[%s] ",__FILE__,__LINE__,cOldStat);
      
    memset(&sCom_branch, 0 , sizeof(sCom_branch));
    memset(cQs_br_no, 0 ,sizeof(cQs_br_no));  
  memset(acc_v,0x00,sizeof(acc_v));  

  vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name); 
  memset(cTempstr,0,sizeof(cTempstr));
  memcpy(cTempstr, ac_name, 60);
  dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
  memcpy(ac_name, cTempstr, 60);
  vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
        
  /* ========= 收款人账号检查开始  ==========*/
    vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
  memcpy(acc_v, ac_no ,sizeof(acc_v)-1);
  
  /*旧账号转新账号*/
  if(pub_base_old_acno(acc_v))
  {
       sprintf(acErrMsg,"pub_base_old_acno() 出错!! [%d]");
         WRITEMSG
        return -1;
    }
    vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);

    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)   /* 1. 帐号不存时 */
    {
          vtcp_log("%s,%d  账号[%s]不存在,支行挂账  ",__FILE__,__LINE__,acc_v);        
              
              set_zd_data("0680","6"); /* 帐户类型 */        
        /* 帐号不存时，设置挂帐机构 */
        ret = iHvHang_Check_Brno(0, cKinbr);
        if(ret)
        {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
        }
        get_zd_data(DC_TX_BR_NO,cKinbr);             
        vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);         
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                set_zd_data("0980",&stat);             
        } 

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
        if(ret)
        {
                  vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                  return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    vtcp_log("[%s][%d] cKinbr===[%s]  QS_BR_NO=[%s]",__FILE__,__LINE__,cKinbr,QS_BR_NO); 
    vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);    


    /* 2. 帐号存时，查找帐号的开户机构  机构号以帐号的开户机构为准    */
    ret = iHvHang_Check_Brno(mdm_ac_rel_v.opn_br_no, cKinbr);
    if(ret)
    {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
    }
    get_zd_data(DC_TX_BR_NO,cKinbr);             
    vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);

   /*  在这里如果机构号被重置，表示没有找到机构号，所以清算挂帐   
    if( memcmp(cKinbr,mdm_ac_rel_v.opn_br_no, BRNO_LEN) )  
    {
        stat = RECEIVE_CUSTNAME_ERR;
          vtcp_log("%s,%d  机构号被重置: opn_br_no=[%s]     cKinbr=[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no,cKinbr);        
               set_zd_data("0680","6");        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
        if(ret)
        {
                  vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                  return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }   */  
    
    /* ============= 以下机构号已确定 ============= */

    /*  判断介质状态 */
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' )
    {
        /* 介质状态不正常挂帐处理 */

          vtcp_log("%s,%d  账号[%s]介质状态错误  ",__FILE__,__LINE__,acc_v);        
               set_zd_data("0680","6");/* 帐户类型 */        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
        if(ret)
        {
                  vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                  return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }

    /*  是否活期存款户 */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    /*** 按财务要求 9开头内部户挂账 add by wudawei 20130823
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
     add end 20130823 ***/
     g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and ac_id_type='1'", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)  /* 非活期存款户 */
    {
              set_zd_data("0680","6");/* 帐户类型 */           
        vtcp_log("大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 

        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
        if(ret)
        {
                  vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                  return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    /* 内部帐户不检查户名 */
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
        ac_seqn_v  =  1; 
        set_zd_data("0680","1");
                   
        g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
        if (g_reply_int )
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
            WRITEMSG
            return -1;
        }
        
        if (dd_mst_v.ac_sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                    set_zd_data("0980",&stat);             
            }  

            set_zd_data("0680","6");/* 帐户类型 */
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
            if(ret)
            {
                      vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                      return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
        /*  判断是否被冻结  */
        if(dd_mst_v.hold_sts[0]=='1' )
        {
            /* 帐号id被完全冻结所以要挂帐 */
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                    set_zd_data("0980",&stat);             
            }  
                
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
            if(ret)
            {
                      vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                      return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
                char tmp_cmtno[4];
                memset(tmp_cmtno,0,sizeof(tmp_cmtno));
                get_zd_data("0510",tmp_cmtno);
        /*  判断是否是结算户 */
        if(dd_mst_v.sttl_type[0]=='S' && (memcmp(tmp_cmtno,"003",3)!=0 || memcmp(tmp_cmtno,"004",3)!=0))
        {
            sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
            if(ret)
            {
                      vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                      return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }

        pub_base_strpack(dd_mst_v.name);
        pub_base_strpack(ac_name);
        
        vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, dd_mst_v.name, sizeof(dd_mst_v.name)-1);
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy(dd_mst_v.name, cTempstr, sizeof(dd_mst_v.name)-1);
        vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name);

        vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, ac_name, 60);
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy(ac_name, cTempstr, 60);
        vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
          
        
        if (strcmp(dd_mst_v.name,ac_name))
        {
            sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,ac_name);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                    set_zd_data("0980",&stat);             
            }  

            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
            if(ret)
            {
                      vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                      return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }
    else /* 检查内部帐户 */
    {
        struct in_mst_c sIn_mst;
        memset(&sIn_mst,0,sizeof(sIn_mst));
        set_zd_data("0680","6");/* 设置帐户类型 */
                
        g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
        if (g_reply_int)
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
            WRITEMSG
            return -1;
        }
        
        if (sIn_mst.sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常 挂帐! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 设置帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
                    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_LVGZ,acc_v);
            if(ret)
            {
                      vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
                      return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }

    memset(cOldStat, 0, sizeof(cOldStat));        
    get_zd_data("0980",cOldStat);     /* 记录原状态 */
    vtcp_log("[%s][%d] 没有挂帐，状态应该不变=='9' ?  cOldStat===[%s] ",__FILE__,__LINE__,cOldStat);     

    vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
    return 0;    
}
