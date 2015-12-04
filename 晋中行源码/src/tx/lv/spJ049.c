/********************************************************************************
* 定期贷记信息导入程序                                                          *
* 描      述: 把/home/mpt/tmp/name文件中的内容导入到lv_pkgreg表中               *  
* 文件的格式: (中间增加|分隔)                                                   *
*             收款人账号|收款人姓名|交易金额 |接收行行号|收款人开户行           *
*             cashactno |cashname  |txamt(分)|acbrno         | cashno           *
* 涉  及  表: lv_pkgreg lv_wbctl                                                *
* 编  写  者: xyy 2006-12-7 11:05                                               *
* 输入  参数: xmno ,pay_ac_no,name,feeflag,ywtype,content                       *
* 含      义: 文件里的基本信息导入到表中                                        * 
* Modified by SSH,2006.1.16,修改一些错误                                        *
********************************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "lv_wbctl_c.h"
#include "lv_xmh_c.h"
#include "dd_mst_c.h"  
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h" 
#include "hv_uniontab_c.h"

extern char* pcLvGetDay();
extern char *zip_space(char *);
extern char *strtokk (char *,char *);
int spJ049()
{
        int    iRc = 0;
        int    iOrderno;
        int    nLineId;
        FILE   *fpr;
        double dTotamt=0;   /* 总的交易金额*/
        double dTxamt=0;    /* 单条记录的交易金额*/
        double dAvbal =0.0; /* 帐户余额*/
        double cAmt =0.0;
        char  cTmpBuff    [61 ];
        char  cTimes      [5  ];  /* 总的记录数*/
        char  cKinbr      [BRNO_LEN+1];
        char  cTlrno      [5  ];  
        char  pcPackday   [9  ];  
        char  pcPackid    [9  ];
        char  pcFilename  [500];
        char  cXmno       [61 ];  /* 项目编号  */
        char  cPay_ac_no  [25 ];  /* 付款人帐号*/ 
        char  cName       [21 ];  /* 文件名称  */
        char  cFeeflg     [2  ];  /* 手续费用  */   /*手续费用是怎么处理的�**/
        char  cYw_type    [13 ];  /* 业务类型  */
        char  cContent    [61 ];  /* 附言      */
        char  cAcbrno     [13];   /* 接收行行号*/
        char  cCashqsno   [13];   /* 接收清算行*/
        char  cTotamt     [17];   /* 交易总金额*/
        char  cAvbal      [17];   /* 帐户余额*/
        char  cTmpDsf     [31];
        char  cStr        [120];   /*add by ChengGX 衡水 20100605*/
        memset(cStr,0, sizeof(cStr));
        
        struct _t_readline
        {
              char  xmno        [60]; /* 项目编号    */
              char  cashactno   [32]; /* 收款人账号  */        
              char  cashname    [60]; /* 收款人姓名  */
              char  txamt       [17]; /* 交易金额    */        
              char  cashno      [12]; /* 收款人开户行*/
              char  acbrno      [12]; /* 接收行行号  */
        };
        static struct _t_readline fdtl;
        static struct lv_pkgreg_c lv_pkgreg;
        static struct lv_xmh_c    lv_xmh;
        static struct lv_wbctl_c  lv_wbctl;
        static struct hv_uniontab_c hv_uniontab;
        static        PAY_IN_AREA payin;
  
        memset(&payin       ,0, sizeof(payin      ));
        memset(&lv_pkgreg   ,0, sizeof(lv_pkgreg  ));
        memset(&lv_wbctl    ,0, sizeof(lv_wbctl   ));
        memset(&fdtl        ,0, sizeof(fdtl       ));
        memset(&hv_uniontab ,0, sizeof(hv_uniontab));
        
        memset(cTmpBuff     ,0, sizeof(cTmpBuff   ));
        memset(cTimes       ,0, sizeof(cTimes     ));
        memset(cKinbr       ,0, sizeof(cKinbr     ));
        memset(cTlrno       ,0, sizeof(cTlrno     ));
        memset(pcPackday    ,0, sizeof(pcPackday  ));
        memset(pcPackid     ,0, sizeof(pcPackid   ));
        memset(pcFilename   ,0, sizeof(pcFilename ));
        memset(cXmno        ,0, sizeof(cXmno      ));
        memset(cPay_ac_no   ,0, sizeof(cPay_ac_no )); 
        memset(cName        ,0, sizeof(cName      ));
        memset(cFeeflg      ,0, sizeof(cFeeflg    ));
        memset(cYw_type     ,0, sizeof(cYw_type   ));
        memset(cAcbrno      ,0, sizeof(cAcbrno    ));
        memset(cCashqsno    ,0, sizeof(cCashqsno  ));
        memset(cTotamt      ,0, sizeof(cTotamt    ));
        memset(cAvbal       ,0, sizeof(cAvbal     ));
        memset(cTmpDsf      ,0, sizeof(cTmpDsf    ));
        
        pub_base_sysinit();
        /* 得到前台输入的内容 */
        get_fd_data("0030", cKinbr);    /**交易机构**/
        get_fd_data("0070", cTlrno);    /**操作员  **/
        get_fd_data("0961", cXmno);     /**项目号  **/
        get_fd_data("0330", cPay_ac_no);/**付款帐号**/
        get_fd_data("0380", cName);     /**文件名称**/
        get_fd_data("0962", cTmpBuff);  /**12位的,再重新选择一个域**/
        get_fd_data("0963", cContent);  /**付言    **/
        get_fd_data("0965", cFeeflg);   /**手续费标志**/
        get_fd_data("0390", cTotamt);   /**交易总金额**/
        
        memcpy(cYw_type,cTmpBuff ,sizeof(cYw_type)-1);
        vtcp_log("[%s][%d]业务类型==[%s]",__FL__,cYw_type);
        vtcp_log("[%s][%d]交易总金额==[%s]",__FL__,cTotamt);
        zipn_space(cXmno    ,sizeof(cXmno     )-1);
        zipn_space(cPay_ac_no,sizeof(cPay_ac_no)-1);
        zipn_space(cYw_type  ,sizeof(cYw_type  )-1);
        zipn_space(cContent  ,sizeof(cContent  )-1);

        vtcp_log("[%s][%d]您输入的项目编号是[%.60s]\n",__FL__,cXmno);
        vtcp_log("[%s][%d]您输入的付款帐号是[%.19s]\n",__FL__,cPay_ac_no);        
        vtcp_log("[%s][%d]您输入的文件名称是[%.20s]\n",__FL__,cName);
        vtcp_log("[%s][%d] spJ049开始喽\n",__FL__);
        
        if(iGetActnoSts(cPay_ac_no,&dAvbal))  /**首先检查帐户状态**/
        {
              sprintf( acErrMsg,"[%s][%d]检查付款帐号状态失败",__FL__);
            strncpy(g_pub_tx.reply,"P109",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] 开始检查营业状态,该帐户余额为[%f]",__FL__,dAvbal);
        if(iLvchkstat())   /**检查营业状态**/
        {
              sprintf( acErrMsg,"[%s][%d]检查营业状态错误",__FL__);
            strncpy(g_pub_tx.reply,"P046",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }

        sprintf(pcFilename,"%s/tmp/%.*s",getenv("WORKDIR"),sizeof(cName),cName);
        zip_space(pcFilename);
        vtcp_log("[%s][%d] pcFilename==[%s] \n",__FL__,pcFilename);        
        /* 根据前台输入,从数据库里得到此协议号的信息 */        
        g_reply_int = Lv_xmh_Sel(g_pub_tx.reply,&lv_xmh,"xmno='%s' ",cXmno);
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]得到此协议号的信息错误",__FL__);
            strncpy(g_pub_tx.reply,"P089",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        memcpy(cCashqsno , lv_xmh.o_qs_no,sizeof(cCashqsno)-1);
        vtcp_log("[%s][%d]  xmno    =[%s] \n",__FL__,lv_xmh.xmno);
        vtcp_log("[%s][%d]  xm_ind  =[%c] \n",__FL__,lv_xmh.xm_ind[0]);
        vtcp_log("[%s][%d]  ac_no   =[%s] \n",__FL__,lv_xmh.ac_no);
        vtcp_log("[%s][%d]  name    =[%s] \n",__FL__,lv_xmh.name);
        vtcp_log("[%s][%d]  yw_type =[%s] \n",__FL__,lv_xmh.yw_ind);
        vtcp_log("[%s][%d] cCashqsno=[%s] \n",__FL__,cCashqsno);
        /****检查项目是否有效****/
        if(lv_xmh.flg[0]!='0')
        {
            sprintf( acErrMsg,"[%s][%d]项目不合法",__FL__);
            strncpy(g_pub_tx.reply,"P089",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        zip_space(lv_xmh.ac_no);
        if(memcmp(lv_xmh.ac_no,cPay_ac_no,strlen(lv_xmh.ac_no))) /*mod by CGX 衡水 **/
        {
        		vtcp_log("[%s][%d]  lv_xmh.ac_no=[%s] cPay_ac_no[%s]\n",__FL__,lv_xmh.ac_no,cPay_ac_no);
            sprintf( acErrMsg,"[%s][%d]项目编号和付款人帐号不匹配",__FL__);
            strncpy(g_pub_tx.reply,"S039",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* 检查文件能否正确打开 */
        if((fpr=fopen(pcFilename,"rb"))==NULL) 
        {
            sprintf( acErrMsg,"[%s][%d]没有找到相对应的文件",__FL__);
            vtcp_log("[%s][%d] pcFilename==[%s] \n",__FL__,pcFilename);
	    strncpy(g_pub_tx.reply,"P090",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        } 
        /***判断是贷记还是借记业务**/
        if (memcmp(lv_xmh.xm_ind,"0",1)!=0)        
        {
            sprintf( acErrMsg,"[%s][%d]项目[%c]不是定期贷记",__FL__,lv_xmh.xm_ind[0]);
            strncpy(g_pub_tx.reply,"P090",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        
        dTotamt=0;
        nLineId=0;
        vtcp_log("[%s][%d]  \n",__FL__);
        while (!feof (fpr)) 
        {
            char pcLineBuf[256];
            vtcp_log("[%s][%d]  \n",__FL__);
            memset(pcLineBuf,0,sizeof(pcLineBuf));
            fgets(pcLineBuf,sizeof(pcLineBuf),fpr);
            zip_space(pcLineBuf);
            if(strlen(pcLineBuf)<3)
            {
                continue;
            }
            vtcp_log("[%s][%d]** nLineId=[%d] \n",__FL__,nLineId);
            if(nLineId==0)
            {   /***从文件中读取的记录是第一条的时候***/
                char *ptr;
                memset(&fdtl,'\0',sizeof(fdtl));
                ptr = strtokk(pcLineBuf,"|");
                if(ptr)
                {
                    zip_space(ptr);
                    strncpy(fdtl.xmno,ptr,sizeof(fdtl.xmno));        /*项目号*/
                    zip_space(fdtl.xmno);
                }else
                {
                    vtcp_log("[%s][%d],文件中无项目编号[%s]\n",__FL__,cXmno);
                    sprintf(acErrMsg, "[%s][%d]文件中无项目编号!",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                if(memcmp(fdtl.xmno,cXmno,sizeof(cXmno)-1)!=0)
                {  
                    vtcp_log("[%s][%d],[%s]项目号[%s]不一致\n",__FL__,fdtl.xmno,cXmno);
                    sprintf(acErrMsg,"[%s][%d] 文件中与您输入的项目号不一致",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                nLineId++;
            }
            else
            {    /***从文件中读取的不是第一条记录****/                
                char *ptr;
                memset(&lv_wbctl,'\0',sizeof(lv_wbctl));
                ptr = strtokk(pcLineBuf, "|");
                if(ptr)
                { 
                    strncpy(fdtl.cashactno,ptr,sizeof(fdtl.cashactno));
                    zipn_space(fdtl.cashactno,sizeof(fdtl.cashactno));
                }
                else
                {
                    vtcp_log("[%s][%d],文件记录中收款帐号不存在!\n",__FL__);
                    sprintf(acErrMsg,"[%s][%d] 文件记录中收款帐号不存在",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                ptr = strtokk(NULL,"|");
                if(ptr)
                {
                    strncpy(fdtl.cashname,ptr,sizeof(fdtl.cashname));
                    zipn_space(fdtl.cashname,sizeof(fdtl.cashname));
                }
                else
                {
                    vtcp_log("[%s][%d],文件记录中收款户名不存在!\n",__FL__);
                    sprintf(acErrMsg,"[%s][%d] 文件记录中收款户名不存在",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                ptr = strtokk(NULL,"|");
                if(ptr)
                {
                    strncpy(fdtl.txamt,ptr,sizeof(fdtl.txamt));
                    zipn_space(fdtl.txamt,sizeof(fdtl.txamt));
                }
                else
                {
                    vtcp_log("[%s][%d],文件记录中交易金额不存在!\n",__FL__);
                    sprintf(acErrMsg,"[%s][%d] 文件记录中交易金额不存在",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                ptr = strtokk(NULL,"|");
                if(ptr)
                {
                    strncpy(fdtl.acbrno,ptr,sizeof(fdtl.acbrno));
                    zipn_space(fdtl.acbrno,sizeof(fdtl.acbrno));
                }
                else
                {
                    vtcp_log("[%s][%d],文件记录中接收行号不存在!\n",__FL__);
                    sprintf(acErrMsg,"[%s][%d] 文件记录中接收行号不存在",__FL__);
                    strcpy( g_pub_tx.reply, "P090");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                memcpy(cAcbrno,fdtl.acbrno,sizeof(cAcbrno)-1);
                g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab," or_br_no='%s' ",cAcbrno);
                if(g_reply_int)
                {   
                      vtcp_log("[%s][%d]查询人行行名行号表错误 err=[%d]", __FL__,g_reply_int);
                    sprintf(acErrMsg,"[%s][%d]查询行名行号表错误[%s]",__FL__,cAcbrno);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                if(hv_uniontab.br_sts[0]!='1')
                {
                      vtcp_log("[%s][%d]该行名[%s]状态[%c]错误", __FL__,cAcbrno,hv_uniontab.br_sts[0]);
                    sprintf(acErrMsg,"%s,%该行名状态错误[%s]",__FL__,cAcbrno);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                if(memcmp(hv_uniontab.qs_no,cCashqsno,sizeof(cCashqsno)-1)!=0)
                {
                      vtcp_log("[%s][%d]该行名[%s]不属该项目[%s]清算行", __FL__,cAcbrno,cCashqsno);
                    sprintf(acErrMsg,"该行名[%s]不属该项目[%s]清算行",cAcbrno,cCashqsno);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                ptr = strtokk(NULL,"|");
                if (ptr)
                {
                    strncpy(fdtl.cashno,ptr,sizeof(fdtl.cashno));
                    zipn_space(fdtl.cashno,sizeof(fdtl.cashno));
                }
                else
                {
                    memcpy(fdtl.cashno,fdtl.acbrno,sizeof(fdtl.cashno));
                }
                
                
                /*add by weiz 为代收付添加*/
                ptr = strtokk(NULL,"|");
                memset(cTmpDsf,0,sizeof(cTmpDsf));
                if (ptr){
                    strncpy(cTmpDsf,ptr,strlen(ptr));
                    zip_space(cTmpDsf);
                }else{
                    memcpy(cTmpDsf,"\0",1);/*没有了就不加了*/
                }
                
                /*******把文件中取出的记录给payin结构体*******/
                memset(&payin,'\0',sizeof(payin));
                payin.F_crdb[0]=CRDB_CR;
                memcpy(payin.F_opcd ,OPCD_LR   ,sizeof(payin.F_opcd));
                memcpy(payin.F_pkgno,PKGNO_DQDJ,sizeof(payin.F_pkgno));
                iOrderno=iLvGetOrderno(); /*产生一个新的交易序号*/                        
                if (iOrderno<=0)
                {
                    vtcp_log("[%s][%d],得到支付交易序号错误!\n",__FL__);
                    sprintf(acErrMsg,"[%s][%d] 得到支付交易序号错误",__FL__);
                    strcpy( g_pub_tx.reply, "HV01");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
                vtcp_log("[%s][%d]1111111111111111",__FILE__,__LINE__);
                memcpy(payin.F_txnum,TXNUM_DQDJ,sizeof(payin.F_txnum));
                memcpy(payin.F_acbrno,fdtl.acbrno,sizeof(payin.F_acbrno));
                memcpy(payin.F_cashno,fdtl.cashno,sizeof(payin.F_cashno));
                str2dbl(fdtl.txamt,strlen(fdtl.txamt),0,&dTxamt);
                dTotamt+=dTxamt;
                dbl2str(&dTxamt,0,sizeof(payin.F_txamt),0,payin.F_txamt);
                vtcp_log("[%s][%d],字符金额:[%s],数字金额:[%f]\n",__FL__,fdtl.txamt,dTxamt);
                memcpy(payin.F_payactno,lv_xmh.ac_no,sizeof(payin.F_payactno));
                memcpy(payin.F_payname ,lv_xmh.name,sizeof(payin.F_payname));
                memcpy(payin.F_cashactno,fdtl.cashactno,sizeof(fdtl.cashactno));
                memcpy(payin.F_cashname ,fdtl.cashname ,sizeof(fdtl.cashname));

                memcpy(payin.F_ywtype   ,cYw_type      ,sizeof(payin.F_ywtype));
                memcpy(payin.F_content  ,cContent      ,sizeof(payin.F_content));
                payin.F_operlevel[0]='0';
                payin.F_feeflag[0]=cFeeflg[0];
                payin.F_lw_ind[0]='1';
                payin.F_packend[0]='0';
                vtcp_log("[%s][%d]22222222222222222222222",__FILE__,__LINE__);
                iLvPayinAutoInfo(&payin);/**得到日期和流水号**/
                memcpy(payin.A_brno,cKinbr,sizeof(payin.A_brno));
                iLvSetAddit(&payin,"dsf",cTmpDsf,24);
                iLvZipAll(&payin);/**压缩所有空格***/
                /****下面录入****/
                /****基本检查****/
                vtcp_log("[%s][%d]333333333333333333333",__FILE__,__LINE__);
                g_reply_int=iLvPayinCheck0(&payin);
                if(g_reply_int)
                {
                     vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
                     sprintf(acErrMsg,"[%s][%d] iLvPayinCheck0失败",__FL__);
                     strncpy(g_pub_tx.reply,"O206",4);
                     set_zd_data("0130",acErrMsg);
                     WRITEMSG
                     goto ErrExit;
                }
                /****要素检查****/
                vtcp_log("[%s][%d]444444444444444444444444",__FILE__,__LINE__);
                g_reply_int=iLvPayinCheck4(&payin);
                if(g_reply_int)
                {
                    vtcp_log("[%s][%d] err in check4 err=[%d]\n", __FL__,g_reply_int);
                    sprintf(acErrMsg,"[%s][%d] iLvPayinCheck4失败",__FL__);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                /****行号检查****/
                vtcp_log("[%s][%d]55555555555555555555555555",__FILE__,__LINE__);
                g_reply_int=iLvPayinCheck1(&payin);
                if(g_reply_int)
                {
                    vtcp_log("[%s][%d] err in check1 err=[%d]\n", __FL__,g_reply_int);
                    sprintf(acErrMsg,"[%s][%d] iLvPayinCheck1失败",__FL__);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                /***根据输入内容生成一条新的纪录****/
                vtcp_log("[%s][%d]6666666666666666666",__FILE__,__LINE__);
                g_reply_int=iLvNewRecFromPayin(&payin);
                if(g_reply_int)
                {
                    vtcp_log("[%s][%d]iLvNewRecFromPayin err=[%d]", __FL__,g_reply_int);
                    sprintf(acErrMsg,"[%s][%d] iLvNewRecFromPayin报错",__FL__);
                    strncpy(g_pub_tx.reply,"O206",4);
                    set_zd_data("0130",acErrMsg);
                    WRITEMSG
                    goto ErrExit;
                }
                vtcp_log("[%s][%d]777777777777777777777777",__FILE__,__LINE__);
                /****下面记帐****/
                memcpy(payin.T_checkno,cTlrno,sizeof(cTlrno)-1);
                memcpy(payin.T_checkwssrno,payin.A_wssrno,sizeof(payin.T_checkwssrno));
                memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));
                memcpy(payin.F_opcd,OPCD_CHECK,sizeof(payin.F_opcd));
                
                /******************更新帐务部分**********************
                iRc=iLv_updAcmrAndAct(&payin);
                payin.T_stat[0]=STAT_WZCHECK2;
                if(iRc)
                {
                    vtcp_log("[%s][%d],更新帐务失败,sqlcode=%d!\n",__FL__,sqlca.sqlcode);
                    it_txcom.txrsut=TX_REJECT;
                    gs100_error();
                    return(-1);
                }
                ********************************************************/
                
                payin.T_stat[0]=STAT_WZCHECK2;
                /****下面拼包****/
                if(nLineId>1)
                {
                    memcpy(payin.F_packid ,pcPackid ,sizeof(payin.F_packid));
                    memcpy(payin.F_packday,pcPackday,sizeof(payin.F_packday));
                    memcpy(payin.T_packid ,pcPackid ,sizeof(payin.T_packid));
                    memcpy(payin.T_packday,pcPackday,sizeof(payin.T_packday));
                }
                vtcp_log("[%s][%d]888888888888888888888",__FILE__,__LINE__);
                iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
                if(iRc!=PCHK_NEW && nLineId==1)
                {
                    vtcp_log("[%s][%d],生成新包错[%d]",__FL__,iRc);
                    sprintf( acErrMsg,"[%s][%d]生成新包错[%d]",__FL__,g_reply_int);
                    strcpy( g_pub_tx.reply, "P040");
                    set_zd_data(DC_GET_MSG,"生成新包错");
                    WRITEMSG
                    goto ErrExit;
                }
                vtcp_log("[%s][%d]999999999999999999999999",__FILE__,__LINE__);
                if(iRc!=PCHK_UPDATE && nLineId>1)
                {
                    vtcp_log("[%s][%d],添加到原有包错[%d]",__FL__,iRc);
                    sprintf( acErrMsg,"[%s][%d]添加到原有包错[%d]",__FL__,g_reply_int);
                    strcpy( g_pub_tx.reply, "P040");
                    set_zd_data(DC_GET_MSG,"添加到原有包错");
                    WRITEMSG
                    goto ErrExit;
                }
                vtcp_log("[%s][%d]000000000000000000",__FILE__,__LINE__);
                if(nLineId==1)
                {
                    memcpy(pcPackid ,payin.F_packid ,sizeof(payin.F_packid));
                    memcpy(pcPackday,payin.F_packday,sizeof(payin.F_packday));
                }
                /***插入新记录****/
                vtcp_log("[%s][%d]1111111111111111",__FILE__,__LINE__);
                g_reply_int=iLvUpdRecFromPayin(&payin,1);
                if(g_reply_int)
                {
                    vtcp_log("[%s][%d],调iLvUpdRecFromPayin错[%d]",__FL__, g_reply_int);
                    sprintf( acErrMsg,"[%s][%d]调iLvUpdRecFromPayin错[%d]",__FL__,g_reply_int);
                    strcpy( g_pub_tx.reply, "O206");
                    set_zd_data(DC_GET_MSG,"调用iLvUpdRecFromPayin错误");
                    WRITEMSG
                    goto ErrExit;
                }
                vtcp_log("[%s][%d]22222222222222",__FILE__,__LINE__);
                nLineId++;
            }
        }
        if(nLineId>1)
        {
            g_reply_int = sql_execute("update lv_wbctl set stat='2' where packid='%s' and pack_date='%s'",pcPackid,pcPackday);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],更新往包控制表失败[%d]",__FL__, g_reply_int);
                sprintf(acErrMsg,"[%s][%d]更新往包控制表失败",__FL__);
                strcpy( g_pub_tx.reply, "O206");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
        }
        
        /**给前台返回数据**/
        apitoa(nLineId-1,4,cTimes);
        zip_space(cTotamt);
        str2dbl(cTotamt,strlen(cTotamt),0,&cAmt);
        dTotamt /= 100;
        if(dTotamt != cAmt)
        {
               vtcp_log("[%s][%d],输入余额[%f]<>[%f]",__FL__,cAmt,dTotamt);
             sprintf(acErrMsg,"[%s][%d]输入余额跟文件中的不一致",__FL__);
             strcpy( g_pub_tx.reply, "O206");
             set_zd_data(DC_GET_MSG,acErrMsg);
             WRITEMSG
             goto ErrExit;
        }
        vtcp_log("[%s][%d]前后台的金额一致!",__FILE__,__LINE__);
        if(dTotamt - dAvbal >1)    /*如果余额不足1元,那么就拒绝做此业务*/
        {
               vtcp_log("[%s][%d],交易金额大于余额[%f]>[%f]",__FL__,dTotamt,dAvbal);
             sprintf(acErrMsg,"[%s][%d]交易金额大于余额",__FL__);
             strcpy( g_pub_tx.reply, "O206");
             set_zd_data(DC_GET_MSG,acErrMsg);
             WRITEMSG
             goto ErrExit;
        }
        set_zd_data("0360",cTimes);     /**得到总的记录数**/
        set_zd_double("0430",dTotamt);  /**得到总的金额数**/
        set_zd_data("0470",pcPackday);  /**得到包日期    **/
        set_zd_data("1280",pcPackid);   /**得到包序号    **/
        set_zd_data("0460",pcPackid);   /**得到包序号 mod by chenggx 20110610**/
        vtcp_log("[%s][%d]pcPackday=[%s]\n",__FILE__,__LINE__,pcPackday);
        vtcp_log("[%s][%d]pcPackid=[%s]\n",__FILE__,__LINE__,pcPackid);
        fclose(fpr);
        /*beg add by ChengGX 20100605 衡水 解决 定期贷记导盘程序可以重复做的问题*/
        memset(cStr,0, sizeof(cStr));
        sprintf(cStr,"mv %s %s/tmp/dsf/",pcFilename,getenv("WORKDIR"));
        vtcp_log("[%s][%d] cStr==[%s] pcFilename[%s]\n",__FILE__,__LINE__,cStr,pcFilename);
    	system(cStr);
        /*end add by ChengGX 20100605 衡水 解决 定期贷记导盘程序可以重复做的问题*/
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
/*根据帐号,我们来判断该帐户的状态*/
int iGetActnoSts(char *ac_no,double * avbal)
{
    int ret=0;

    struct dd_mst_c sDd_mst;
    struct td_mst_c sTd_mst;
    struct ln_mst_c sLn_mst;
    struct in_mst_c sIn_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;

    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sTd_mst,0x00,sizeof(struct td_mst_c));
    memset(&sLn_mst,0x00,sizeof(struct ln_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",ac_no);
    if(ret)
    {
          vtcp_log("[%s][%d]查询mdm_ac_rel出错[%d]!",__FILE__,__LINE__,ret);
          sprintf(acErrMsg,"查询mdm_ac_rel出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          return -1;
    }
    if(sMdm_ac_rel.note_sts[0]!='0') /**介质状态**/
    {
         vtcp_log("[%s][%d]mdm_ac_rel.note_sts==[%c]不正常!",__FILE__,__LINE__,sMdm_ac_rel.note_sts[0]);
         sprintf(acErrMsg,"mdm_ac_rel.note_sts==[%c]不正常!",sMdm_ac_rel.note_sts[0]);
         set_zd_data(DC_GET_MSG,acErrMsg);
         return -1;
    }
    ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
    if(ret)
    {
          vtcp_log("[%s][%d]查询prdt_ac_id出错[%d]!",__FILE__,__LINE__,ret);
          sprintf(acErrMsg,"查询prdt_ac_id出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          return -1;
    }
    switch(sPrdt_ac_id.ac_id_type[0])
    {
        case '1':/*活期*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);  
            if(ret)
            {
                  vtcp_log("[%s][%d]查询dd_mst出错[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"查询dd_mst出错!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sDd_mst.ac_sts[0]!='1')/*活期的帐户状态*/
            {
                  vtcp_log("[%s][%d]活期的帐户状态ac_sts[%c]不正常!",__FILE__,__LINE__,sDd_mst.ac_sts[0]);
                  sprintf(acErrMsg,"sDd_mst.ac_sts==[%c]不正常!",sDd_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            if(sDd_mst.hold_sts[0]!='0')/*活期的冻结状态*/
            {
                  vtcp_log("[%s][%d]活期的冻结状态hold_sts[%c]不正常!",__FILE__,__LINE__,sDd_mst.hold_sts[0]);
                  sprintf(acErrMsg,"sDd_mst.hold_sts==[%c]不正常!",sDd_mst.hold_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sDd_mst.bal;
            break;
        case '2':/*定期存款*/
            ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  vtcp_log("[%s][%d]查询td_mst出错[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"查询td_mst出错!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sTd_mst.ac_sts[0]!='1')/*定期的帐户状态*/
            {
                  vtcp_log("[%s][%d]定期的帐户状态ac_sts[%c]不正常!",__FILE__,__LINE__,sTd_mst.ac_sts[0]);
                  sprintf(acErrMsg,"sTd_mst.ac_sts==[%c]不正常!",sTd_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            if(sTd_mst.hold_sts[0]!='0')/*定期的冻结状态*/
            {
                  vtcp_log("[%s][%d]定期的冻结状态hold_sts[%c]不正常!",__FILE__,__LINE__,sTd_mst.hold_sts[0]);
                  sprintf(acErrMsg,"sTd_mst.hold_sts==[%c]不正常!",sTd_mst.hold_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sTd_mst.bal;
            break;
        case '3':/*贷款*/
            ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  sprintf(acErrMsg,"查询ln_mst出错!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sLn_mst.ac_sts[0]!='1')/*贷款的帐户状态*/
            {
                  sprintf(acErrMsg,"sLn_mst.ac_sts==[%c]不正常!",sLn_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sLn_mst.bal;
            break;
        case '9':/*内部帐*/
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  vtcp_log("[%s][%d]查询td_mst出错[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"查询in_mst出错!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sIn_mst.sts[0]!='1')/*内部帐的帐户状态*/
            {
                  vtcp_log("[%s][%d]内部帐的帐户状态[%c]不正常!",__FILE__,__LINE__,sIn_mst.sts[0]);
                  sprintf(acErrMsg,"sIn_mst.sts==[%c]不正常!",sIn_mst.sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sIn_mst.bal;
            break;
        default:
            vtcp_log("[%s][%d]无效的帐户类型[%c]!",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
            sprintf(acErrMsg,"无效的帐户类型");
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return -1;
            break;
    }
    vtcp_log("[%s][%d] 该帐户的余额为avbal ==[%f]",__FILE__,__LINE__,*avbal);
return 0;
}
