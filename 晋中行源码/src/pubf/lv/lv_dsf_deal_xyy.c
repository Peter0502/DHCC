/***************************************************************************/
/* 文件名称: lv_dsf_deal.c                                                 */
/* 功能描述: 处理代收付业务公共函数                                        */
/* 创建日期: 2005.11.20                                                    */
/* 作    者: WeiZ                                                          */
/* 说    明:                                                               */
/* 修    改: xyy                                                           */
/* 修改日期: 2006-12-10 14:06                                              */
/* 修改内容: 把老系统的代收付的公共函数改造成适应新体统的函数,             */
/*            (#####注意日期的类型！)                                      */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include <unistd.h>
#include "public.h"
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
#include "lv_pldsr_c.h"    
#include "lv_pldfr_c.h"    
#include "lv_pldscx_c.h"   
#include "lv_pldsfqugx_c.h"
#include "lv_xmh_c.h"
#include "lv_dsfsendlist_c.h"
#include "lv_addit_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"

#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
extern int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
extern int iGetHvFzfOrderno(char * orderno);
extern int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);
char * trim( char * str );
char * pcLvGetDay_work();
static double getavbal(const char *isactno,const char *iname,char *s_avbal);


#define GETOFFSET1(a)     lv_get_tag_multi(lv,"72C",a,offset,sizeof(a)-1); \
                        offset+=sizeof(a)-1; 
#define GETOFFSET2(b)   dm_dmemcpy2((b),&(lv->dm),offset,sizeof(b)-1); \
                        offset += sizeof(b)-1;
                        
#define DSFSEND(lv)        if (iRc = dsf_send(lv)){ \
                            return iRc; \
                        } 
#define XYNO    "000000000000000000000000000000000000000000000000000000000000"
#define DEBUG
#ifdef  DEBUG
#define Debug(x)      vtcp_log("[%s][%d]:",__FILE__,__LINE__);printf(x)
#else
#define Debug(x)
#endif

#define SET0(x) memset(x,0,sizeof(x))
/*#define LV_DSFZX "102161000017"            代收付中心行号,这里先放一个,以后要改*/
#define LV_DSFZX "906161000014"              /**山西省代收付中心行号 906161000014  上线前需要替换**/

#define DSF_RECV    "0A"    /*来账接收*/
#define DSF_REJE    "1B"    /*来账拒绝*/
#define DSF_CONC    "0C"    /*来账撤销*/
#define DSF_SUCC    "0D"    /*成功回执40506*/
#define DSF_FAIL    "1E"    /*失败回执40506*/
#define DSF_QSSU    "0F"    /*成功回执40503/4*/
#define DSF_QSFA    "0G"    /*失败回执40503/4*/
#define DSF_SEND    "0H"    /*往账发送*/
#define DSF_SREJ    "1I"    /*往账拒绝*/
#define DSF_QSSS    "0J"    /*往账清算*/
#define DSF_QSSF    "1K"    /*往账失败*/

static struct lv_xmh_c lv_xmh;
union dsfStruct 
{ 
    struct lv_pldsr_c pldsr; 
    struct lv_pldfr_c pldfr;
    struct lv_pldscx_c pldscx;
    struct lv_pldsfqugx_c pldsfqugx;
} dsf;

static int lv_40506_df(struct lv_pldfr_c *pldfr,int ret);
static int lv_40506_ds(struct lv_pldsr_c *pldsr,int ret);

static char* GETTAG(TLVPACKAGE *pack,char *name,char *value) 
{
    lv_get_tag(pack,name,value); 
    return(value);
}
static char* GETHEADTAG(TLVPACKAGE *pack,char *name,char *value) 
{
    lv_pkg_getHeaderTag(pack,name,value); 
    return(value);
}
static char* GETTAG_AMT(TLVPACKAGE *pack,char *name,char *value,int length) 
{
    char buf[32]; 
    double dblTmp=0;
    memset(buf,'\0',sizeof(buf));
    lv_get_tag(pack,name,buf); 
    dblTmp=atof(buf);
    sprintf(buf,"%*.0f",length,dblTmp);
    memcpy(value,buf,length);
    return(value);
}

int lv_dsf_deal(TLVPACKAGE *lv,char *packid,char *orderno,char *wt_date,
                TLVPACKAGE *return40506Pack,int iCount,int allCount)
{
    /******* 取到数据结构当中 ********/
    FILE    *fp;
    int     ret=0,offset=0,i=0;
    char    yw_type[6];
    char    yanamt[16];
    char    outFile[257];
    
    char cDsf_date[9];   /*代收付中心工作日*/
    char cRe_lim_date[3];/*回执期限(天数)*/
    char cTotamt[16];    /*本条收款总金额*/
    char cPay_num[9];    /*付款人数目*/
    char cCash_num[9];   /*收款人数目*/
    char cTx_amt[16];    /*交易金额*/
    char cRe_dsf_date[9];/*代收付中心工作日*/
    char cO_wt_date[9];  /*原委托日期*/
    char cWt_date[9];
    char cTmpXyno[121];
    
    vtcp_log("[%s][%d] 进入处理代收付业务的主菜单啦!!!!\n",__FILE__,__LINE__);
    
    memset(cDsf_date   ,0,sizeof(cDsf_date));
    memset(cRe_lim_date,0,sizeof(cRe_lim_date));
    memset(cTotamt     ,0,sizeof(cTotamt));
    memset(cPay_num    ,0,sizeof(cPay_num));
    memset(cCash_num   ,0,sizeof(cCash_num));
    memset(cTx_amt     ,0,sizeof(cTx_amt));
    memset(cRe_dsf_date,0,sizeof(cRe_dsf_date));
    memset(cO_wt_date  ,0,sizeof(cO_wt_date));
    memset(cWt_date    ,0,sizeof(cWt_date));
    memset(yw_type     ,0,sizeof(yw_type));
    memset(cTmpXyno    ,0,sizeof(cTmpXyno));
    
    memset(&dsf,0,sizeof(dsf));
    
    ret = lv_analyze_pack(lv);
    vtcp_log("[%s][%d]ret==[%d]",__FILE__,__LINE__,ret);
    if ( ret!=0 )
    {
        vtcp_log("[%s][%d],报文分析错误\n",__FILE__,__LINE__);
        return -1;
    }
    GETTAG(lv,"0BG",yw_type);
    vtcp_log("[%s][%d]运行到这里\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]ywtype==[%s]\n",__FILE__,__LINE__,yw_type);
    if ( !memcmp(yw_type,"40501",5))
    {
        memset(cDsf_date   ,0,sizeof(cDsf_date));
        memset(cTotamt     ,0,sizeof(cTotamt));
        memset(cCash_num   ,0,sizeof(cCash_num));
        memset(cTx_amt     ,0,sizeof(cTx_amt));
        memset(cWt_date    ,0,sizeof(cWt_date));
        memset(cTmpXyno    ,0,sizeof(cTmpXyno));
        
        memcpy(dsf.pldfr.packid,packid,sizeof(dsf.pldfr.packid)-1);
        memcpy(dsf.pldfr.orderno,orderno,sizeof(dsf.pldfr.orderno)-1);

        dsf.pldfr.wt_date = apatoi(wt_date,8);
        lv_get_tag(lv,"58A",dsf.pldfr.br_no);
        lv_get_tag(lv,"52A",dsf.pldfr.or_br_no);
        
        offset=0;
        offset=lv_get_tag_p(lv,"72C");
        offset+=5;

        GETOFFSET2(cDsf_date);
        dsf.pldfr.dsf_date = apatoi(cDsf_date,8);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        
        GETOFFSET2(dsf.pldfr.xyno);
        zip_space(dsf.pldfr.xyno);
        vtcp_log("[%s][%d] pldfr.xyno=[%s]",__FILE__,__LINE__,dsf.pldfr.xyno);

        GETOFFSET2(dsf.pldfr.pay_br_no);
        vtcp_log("[%s][%d] pldfr.pay_br_no=[%s]",__FILE__,__LINE__,dsf.pldfr.pay_br_no);
        GETOFFSET2(dsf.pldfr.pay_opn_br_no);
        vtcp_log("[%s][%d] pldfr.pay_opn_br_no=[%s]",__FILE__,__LINE__,dsf.pldfr.pay_opn_br_no);
        GETOFFSET2(dsf.pldfr.pay_ac_no);
        zip_space(dsf.pldfr.pay_ac_no);
        vtcp_log("[%s][%d] pldfr.pay_ac_no=[%s]",__FILE__,__LINE__,dsf.pldfr.pay_ac_no);
        GETOFFSET2(dsf.pldfr.pay_name);
        zip_space(dsf.pldfr.pay_name);
        vtcp_log("[%s][%d] pldfr.pay_name=[%s]",__FILE__,__LINE__,dsf.pldfr.pay_name);
        GETOFFSET2(dsf.pldfr.pay_addr);
        vtcp_log("[%s][%d] pldfr.pay_addr=[%s]",__FILE__,__LINE__,dsf.pldfr.pay_addr);
        GETOFFSET2(dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        GETOFFSET2(cTotamt);
        vtcp_log("[%s][%d] cTotamt=[%s]",__FILE__,__LINE__,cTotamt);
        str2dbl(cTotamt,sizeof(cTotamt)-1,0,&dsf.pldfr.totamt);
        vtcp_log("[%s][%d] pldfr.totamt=[%f]",__FILE__,__LINE__,dsf.pldfr.totamt);
        dsf.pldfr.totamt = dsf.pldfr.totamt/100;
        vtcp_log("[%s][%d] pldfr.totamt=[%f]",__FILE__,__LINE__,dsf.pldfr.totamt);
        /*注意这里得到的金额的正确性*/

        vtcp_log("[%s][%d] pldfr.totamt=[%f]",__FILE__,__LINE__,dsf.pldfr.totamt);
        GETOFFSET2(dsf.pldfr.cash_qs_no);
        vtcp_log("[%s][%d] pldfr.cash_qs_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_qs_no);
        GETOFFSET2(cCash_num);
        vtcp_log("[%s][%d] ******cCash_num******=[%s]",__FILE__,__LINE__,cCash_num);
        dsf.pldfr.cash_num = apatoi(cCash_num,8);
        vtcp_log("[%s][%d] pldfr.cash_num=[%ld]",__FILE__,__LINE__,dsf.pldfr.cash_num);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);

        /******* 检查项目号　************/

        memcpy(dsf.pldfr.sts,DSF_RECV,2);        
        memcpy(dsf.pldfr.repstat2,"00",2);
        memcpy(dsf.pldfr.re_content2,"回应成功",8);
        vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
        vtcp_log("[%s][%d] pldfr.re_content2=[%s]",__FILE__,__LINE__,dsf.pldfr.re_content2);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);

        if (memcmp(dsf.pldfr.xyno,XYNO,60))
        {
            vtcp_log("[%s][%d] dsf.pldfr.xyno [%s]",__FILE__,__LINE__,dsf.pldfr.xyno);
            vtcp_log("[%s][%d] XYNO [%s]",__FILE__,__LINE__,XYNO);
            vtcp_log("[%s][%d] 合同号非全零",__FILE__,__LINE__);
            ret = lv_check_xmh(&dsf.pldfr);    /**如果没有验证过,验证,不通过全部打回**/
            if (ret)
            {
                vtcp_log("[%s][%d],对项目号出错 ret=[%d]",__FILE__,__LINE__,ret);
                memcpy(cTmpXyno,dsf.pldfr.re_content2,strlen(dsf.pldfr.re_content2));
                strcat(cTmpXyno,dsf.pldfr.xyno);
            }
            else
            {
                memcpy(cTmpXyno,dsf.pldfr.xyno,sizeof(dsf.pldfr.xyno)-1);
            }
            vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
        }
        else
        {   /*是全0就从项目表里面找到相对的行号对应的项目,没有就建立一个*/
            vtcp_log("[%s][%d] 合同号全零",__FILE__,__LINE__);
            ret = lv_get_xmh(&dsf.pldfr);
            if (ret)
            {
                vtcp_log("[%s][%d],对项目号出错 ret=[%d]",__FILE__,__LINE__,ret);
                memcpy(cTmpXyno,dsf.pldfr.re_content2,strlen(dsf.pldfr.re_content2));
                strcat(cTmpXyno,dsf.pldfr.xyno);
            }
            else
            {
                memcpy(cTmpXyno,dsf.pldfr.xyno,sizeof(dsf.pldfr.xyno)-1);
            }
            vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
        }
        zip_space(cTmpXyno);
        vtcp_log("[%s][%d] cTmpXyno =[%s]\n",__FILE__,__LINE__,cTmpXyno);
        
        
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);

        i = dsf.pldfr.cash_num;
        
        vtcp_log("[%s][%d]运行到这里cash_num==[%d],\n",__FILE__,__LINE__,i);
        
        memset(outFile,0,sizeof(outFile));
        sprintf(outFile,"%s/tmp/df%ld_%s",getenv("WORKDIR"),dsf.pldfr.dsf_date,dsf.pldfr.orderno);
        fp = fopen(outFile,"w+");
        if (!fp)
        {
            vtcp_log("[%s][%d],不能打开文件%s\n",__FILE__,__LINE__,outFile);
            return -1;
        }
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        memcpy(dsf.pldfr.filename,outFile,256);
        fprintf(fp,"%s\n",cTmpXyno);
        vtcp_log("[%s][%d]dsf.pldfr.xyno==[%s]",__FILE__,__LINE__,dsf.pldfr.xyno);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        /*这里写一条全交易的记录,这时记tx_mx_no为00000000*/
        memset(dsf.pldfr.tx_mx_no,'0',8);
        /*插入登记簿*/

        vtcp_log("[%s][%d]准备插入记录lv_pldfr",__FILE__,__LINE__);
         
        vtcp_log("[%s][%d]lv_pldfr_c.packid=[%s]        ",__FILE__,__LINE__,dsf.pldfr.packid);
        vtcp_log("[%s][%d]lv_pldfr_c.orderno=[%s]       ",__FILE__,__LINE__,dsf.pldfr.orderno);
        vtcp_log("[%s][%d]lv_pldfr_c.wt_date=[%ld]      ",__FILE__,__LINE__,dsf.pldfr.wt_date);
        vtcp_log("[%s][%d]lv_pldfr_c.br_no=[%s]         ",__FILE__,__LINE__,dsf.pldfr.br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.or_br_no=[%s]      ",__FILE__,__LINE__,dsf.pldfr.or_br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.dsf_date=[%ld]     ",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        vtcp_log("[%s][%d]lv_pldfr_c.xyno=[%s]          ",__FILE__,__LINE__,dsf.pldfr.xyno);
        vtcp_log("[%s][%d]lv_pldfr_c.pay_br_no=[%s]     ",__FILE__,__LINE__,dsf.pldfr.pay_br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.pay_opn_br_no=[%s] ",__FILE__,__LINE__,dsf.pldfr.pay_opn_br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.pay_ac_no=[%s]     ",__FILE__,__LINE__,dsf.pldfr.pay_ac_no);
        vtcp_log("[%s][%d]lv_pldfr_c.pay_name=[%s]      ",__FILE__,__LINE__,dsf.pldfr.pay_name);
        vtcp_log("[%s][%d]lv_pldfr_c.pay_addr=[%s]      ",__FILE__,__LINE__,dsf.pldfr.pay_addr);
        vtcp_log("[%s][%d]lv_pldfr_c.yw_type=[%s]       ",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d]lv_pldfr_c.totamt=[%f]        ",__FILE__,__LINE__,dsf.pldfr.totamt);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_qs_no=[%s]    ",__FILE__,__LINE__,dsf.pldfr.cash_qs_no);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_num=[%ld]     ",__FILE__,__LINE__,dsf.pldfr.cash_num);
        vtcp_log("[%s][%d]lv_pldfr_c.tx_mx_no=[%s]      ",__FILE__,__LINE__,dsf.pldfr.tx_mx_no);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_br_no=[%s]    ",__FILE__,__LINE__,dsf.pldfr.cash_br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_opn_br_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_opn_br_no);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_ac_no=[%s]    ",__FILE__,__LINE__,dsf.pldfr.cash_ac_no);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_name=[%s]     ",__FILE__,__LINE__,dsf.pldfr.cash_name);
        vtcp_log("[%s][%d]lv_pldfr_c.cash_addr=[%s]     ",__FILE__,__LINE__,dsf.pldfr.cash_addr);
        vtcp_log("[%s][%d]lv_pldfr_c.tx_amt=[%f]        ",__FILE__,__LINE__,dsf.pldfr.tx_amt);
        vtcp_log("[%s][%d]lv_pldfr_c.content=[%s]       ",__FILE__,__LINE__,dsf.pldfr.content);
        vtcp_log("[%s][%d]lv_pldfr_c.sts=[%s]           ",__FILE__,__LINE__,dsf.pldfr.sts);
        vtcp_log("[%s][%d]lv_pldfr_c.beg_sts=[%s]       ",__FILE__,__LINE__,dsf.pldfr.beg_sts);
        vtcp_log("[%s][%d]lv_pldfr_c.operlevel=[%s]     ",__FILE__,__LINE__,dsf.pldfr.operlevel);
        vtcp_log("[%s][%d]lv_pldfr_c.filename=[%s]      ",__FILE__,__LINE__,dsf.pldfr.filename);
        vtcp_log("[%s][%d]lv_pldfr_c.repstat=[%s]       ",__FILE__,__LINE__,dsf.pldfr.repstat);
        vtcp_log("[%s][%d]lv_pldfr_c.succamt=[%f]       ",__FILE__,__LINE__,dsf.pldfr.succamt);
        vtcp_log("[%s][%d]lv_pldfr_c.succnum=[%ld]      ",__FILE__,__LINE__,dsf.pldfr.succnum);
        vtcp_log("[%s][%d]lv_pldfr_c.re_content=[%s]    ",__FILE__,__LINE__,dsf.pldfr.re_content);       
        vtcp_log("[%s][%d]lv_pldfr_c.re_dsf_date=[%ld]  ",__FILE__,__LINE__,dsf.pldfr.re_dsf_date);
        vtcp_log("[%s][%d]lv_pldfr_c.repstat2=[%s]      ",__FILE__,__LINE__,dsf.pldfr.repstat2);       
        vtcp_log("[%s][%d]lv_pldfr_c.re_content2=[%s]   ",__FILE__,__LINE__,dsf.pldfr.re_content2);

        g_reply_int  = Lv_pldfr_Ins(dsf.pldfr ,g_pub_tx.reply );
        if(g_reply_int)
        {
            Lv_pldfr_Debug(&dsf.pldfr);
            vtcp_log("[%s][%d],插入代收付登记簿错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"[%s][%d]插入Lv_pldsr表出错",__FL__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(-1);
        }

        vtcp_log("[%s][%d]这里准备循环了",__FILE__,__LINE__);
        while(i--)
        {
            memset(cTx_amt , 0 , sizeof(cTx_amt));
            memset(dsf.pldfr.cash_br_no    ,0,sizeof(dsf.pldfr.cash_br_no    ));
            memset(dsf.pldfr.cash_opn_br_no,0,sizeof(dsf.pldfr.cash_opn_br_no));
            memset(dsf.pldfr.cash_ac_no    ,0,sizeof(dsf.pldfr.cash_ac_no    ));
            memset(dsf.pldfr.cash_name     ,0,sizeof(dsf.pldfr.cash_name     ));
            memset(dsf.pldfr.cash_addr     ,0,sizeof(dsf.pldfr.cash_addr     ));
            memset(dsf.pldfr.tx_mx_no      ,0,sizeof(dsf.pldfr.tx_mx_no      ));
            memset(dsf.pldfr.content       ,0,sizeof(dsf.pldfr.content));
            
            vtcp_log("[%s][%d] pldfr.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.pldfr.tx_mx_no);
            vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
            GETOFFSET2(dsf.pldfr.tx_mx_no);
            vtcp_log("[%s][%d] pldfr.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.pldfr.tx_mx_no);
            GETOFFSET2(dsf.pldfr.cash_br_no    );
            GETOFFSET2(dsf.pldfr.cash_opn_br_no);
            GETOFFSET2(dsf.pldfr.cash_ac_no    );
            GETOFFSET2(dsf.pldfr.cash_name     );
            GETOFFSET2(dsf.pldfr.cash_addr     );
            
            zip_space(dsf.pldfr.cash_ac_no);
            zip_space(dsf.pldfr.cash_name);
            
            GETOFFSET2(cTx_amt);
            str2dbl(cTx_amt,sizeof(cTx_amt)-1,0,&dsf.pldfr.tx_amt);/*注意这里得到的金额的正确性*/
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldfr.tx_amt);
            dsf.pldfr.tx_amt = dsf.pldfr.tx_amt/100;
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldfr.tx_amt);
            GETOFFSET2(dsf.pldfr.content);
            vtcp_log("[%s][%d] pldfr.packid=[%s]",__FILE__,__LINE__,dsf.pldfr.packid);
            vtcp_log("[%s][%d] pldfr.orderno=[%s]",__FILE__,__LINE__,dsf.pldfr.orderno);
            vtcp_log("[%s][%d] pldfr.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.pldfr.tx_mx_no);
            vtcp_log("[%s][%d] pldfr.cash_br_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_br_no);
            vtcp_log("[%s][%d] pldfr.cash_opn_br_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_opn_br_no);
            vtcp_log("[%s][%d] pldfr.cash_ac_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_ac_no);
            vtcp_log("[%s][%d] pldfr.cash_name=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_name);
            vtcp_log("[%s][%d] pldfr.cash_addr=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_addr);
            /*插入登记簿*/

            g_reply_int  = Lv_pldfr_Ins(dsf.pldfr ,g_pub_tx.reply );
            if(g_reply_int)
            {
                 Lv_pldfr_Debug(&dsf.pldfr);
                 vtcp_log("[%s][%d],插入代收付登记簿错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
                 sprintf( acErrMsg,"[%s][%d]插入Lv_pldsr表出错code=[%d]",__FL__,g_reply_int);
                 set_zd_data(DC_GET_MSG,acErrMsg);
                 return(-1);
            }
            vtcp_log("[%s][%d]cTx_amt=[%s]",__FILE__,__LINE__,cTx_amt);
            
            fen2yan(cTx_amt,yanamt);
            vtcp_log("[%s][%d]yanamt=[%s]",__FILE__,__LINE__,yanamt);
            
            apitoa(dsf.pldfr.wt_date,sizeof(cWt_date)-1,cWt_date);
            vtcp_log("[%s][%d]yanamt=[%s]",__FILE__,__LINE__,yanamt);
            fprintf(fp,"%s|%s|%s|%s|%s|%s%s%s\n",dsf.pldfr.cash_ac_no,dsf.pldfr.cash_name,
                    yanamt,dsf.pldfr.cash_br_no,dsf.pldfr.cash_opn_br_no,
                    cWt_date,dsf.pldfr.orderno,dsf.pldfr.tx_mx_no);
            vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
        }
        
        fclose(fp);
        lv_return_40506(lv,&dsf.pldfr,"40501",iCount,allCount,return40506Pack);    
    }
    else if (!memcmp(yw_type,"40502",5))
    {
        memset(cDsf_date   ,0,sizeof(cDsf_date));
        memset(cRe_lim_date,0,sizeof(cRe_lim_date));
        memset(cTotamt     ,0,sizeof(cTotamt));
        memset(cPay_num    ,0,sizeof(cPay_num));
        memset(cTx_amt     ,0,sizeof(cTx_amt));
        memset(cTmpXyno    ,0,sizeof(cTmpXyno));
        
        memcpy(dsf.pldsr.packid,packid,sizeof(dsf.pldsr.packid)-1);
        memcpy(dsf.pldsr.orderno,orderno,sizeof(dsf.pldsr.orderno)-1);

        dsf.pldsr.wt_date  = apatoi (wt_date,8);
        lv_get_tag(lv,"58A",dsf.pldsr.br_no);
        lv_get_tag(lv,"52A",dsf.pldsr.or_br_no);
        
        offset=0;
        offset=lv_get_tag_p(lv,"72C");
        offset+=5;
        
        GETOFFSET2(cDsf_date);
        dsf.pldsr.dsf_date = apatoi(cDsf_date,8);
        vtcp_log("[%s][%d]pldsr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldsr.dsf_date);
        GETOFFSET2(cRe_lim_date);
        dsf.pldsr.re_lim_date = apatoi(cRe_lim_date,8);
        vtcp_log("[%s][%d]pldsr.re_lim_date=[%ld]",__FILE__,__LINE__,dsf.pldsr.re_lim_date);
        
        GETOFFSET2(dsf.pldsr.cash_br_no);
        vtcp_log("[%s][%d]pldsr.cash_br_no=[%s]",__FILE__,__LINE__,dsf.pldsr.cash_br_no);
        GETOFFSET2(dsf.pldsr.cash_opn_br_no);
        vtcp_log("[%s][%d]pldsr.cash_opn_br_no=[%s]",__FILE__,__LINE__,dsf.pldsr.cash_opn_br_no);
        GETOFFSET2(dsf.pldsr.cash_ac_no);
        vtcp_log("[%s][%d]pldsr.cash_ac_no=[%s]",__FILE__,__LINE__,dsf.pldsr.cash_ac_no);
        GETOFFSET2(dsf.pldsr.cash_name);
        vtcp_log("[%s][%d]pldsr.cash_name=[%s]",__FILE__,__LINE__,dsf.pldsr.cash_name);
        GETOFFSET2(dsf.pldsr.cash_addr);
        vtcp_log("[%s][%d]pldsr.cash_addr=[%s]",__FILE__,__LINE__,dsf.pldsr.cash_addr);
        GETOFFSET2(dsf.pldsr.yw_type);
        vtcp_log("[%s][%d]pldsr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldsr.yw_type);
        
        zip_space(dsf.pldsr.cash_ac_no);
        zip_space(dsf.pldsr.cash_name);
        
        GETOFFSET2(cTotamt);
        vtcp_log("[%s][%d] cTotamt=[%s]",__FILE__,__LINE__,cTotamt);
        str2dbl(cTotamt,sizeof(cTotamt)-1,0,&dsf.pldsr.totamt);/*注意这里得到的金额的正确性*/
        vtcp_log("[%s][%d] dsf.pldsr.totamt=[%f]",__FILE__,__LINE__,dsf.pldsr.totamt);
        dsf.pldsr.totamt = dsf.pldsr.totamt/100;
        vtcp_log("[%s][%d] dsf.pldsr.totamt=[%f]",__FILE__,__LINE__,dsf.pldsr.totamt);
        GETOFFSET2(dsf.pldsr.pay_qs_no);
        vtcp_log("[%s][%d] pldfr.pay_qs_no=[%s]",__FILE__,__LINE__,dsf.pldsr.pay_qs_no);
        GETOFFSET2(cPay_num);
        vtcp_log("[%s][%d] cPay_num=[%s]",__FILE__,__LINE__,cPay_num);
        dsf.pldsr.pay_num = apatoi(cPay_num,8);
        vtcp_log("[%s][%d] pldfr.pay_num=[%ld]",__FILE__,__LINE__,dsf.pldsr.pay_num);
        
  
        /******* 检查项目号　************/
        memcpy(dsf.pldsr.sts,DSF_RECV,2);        /*默认的状态为成功*/
        memcpy(dsf.pldsr.repstat2,"00",2);
        memcpy(dsf.pldsr.re_content2,"回应成功",8);
        vtcp_log("[%s][%d] pldfr.re_content2=[%s]",__FILE__,__LINE__,dsf.pldsr.re_content2);
        
        /*是全0就从项目表里面找到相对的行号对应的项目,没有就建立一个*/
        ret = lv_get_xmno(&dsf.pldsr);
        if (ret)
        {
            vtcp_log("[%s][%d],取项目号出错ret[%d]\n",__FILE__,__LINE__,ret);
            memcpy(cTmpXyno , dsf.pldsr.re_content2 ,strlen(dsf.pldsr.re_content2));
            strcat(cTmpXyno , dsf.pldsr.xyno);
            /**这里最好不要return 吧,应该先把这笔记录收下来
            return(-1);
            **/
        }
        else
        {
            memcpy(cTmpXyno , dsf.pldsr.xyno       ,sizeof(dsf.pldsr.xyno)-1);
        }
        zip_space(cTmpXyno);
        vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
        vtcp_log("[%s][%d],取得的项目号为=[%s]\n",__FILE__,__LINE__,cTmpXyno);
        
        i = dsf.pldsr.pay_num;
        vtcp_log("[%s][%d]pay_num==[%d]",__FILE__,__LINE__,i);
        memset(outFile,0,sizeof(outFile));
        sprintf(outFile,"%s/tmp/ds%ld_%s",getenv("WORKDIR"),dsf.pldsr.dsf_date,dsf.pldsr.orderno);
        fp = fopen(outFile,"w+");
        if (!fp)
        {
            vtcp_log("[%s][%d],不能打开文件%s\n",__FILE__,__LINE__,outFile);
            return -1;
        }
        
        vtcp_log("[%s][%d],outputFilename [%s]\n",__FILE__,__LINE__,outFile);
        fprintf(fp,"%s\n",cTmpXyno);
        memcpy(dsf.pldsr.filename,outFile,256);
        
        /*这里写一条全交易的记录,这时记tx_mx_no为00000000*/
        memset(dsf.pldsr.tx_mx_no,'0',8);
        /*插入登记簿*/
        vtcp_log("[%s][%d] stat[%s]repstat2[%s]re_content2[%s]\n",__FILE__,__LINE__,
                dsf.pldsr.sts,dsf.pldsr.repstat2,dsf.pldsr.re_content2);

        g_reply_int = Lv_pldsr_Ins(dsf.pldsr,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]插入Lv_pldsr表出错code=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return(-1);
        }
        vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
        memset(dsf.pldsr.tx_mx_no,0,sizeof(dsf.pldsr.tx_mx_no));
        while(i--)
        {
            vtcp_log("[%s][%d]循环插入各条记录\n",__FILE__,__LINE__);
            memset(dsf.pldsr.tx_mx_no     , 0 , sizeof(dsf.pldsr.tx_mx_no     ));
            memset(dsf.pldsr.pay_br_no    , 0 , sizeof(dsf.pldsr.pay_br_no    ));
            memset(dsf.pldsr.pay_opn_br_no, 0 , sizeof(dsf.pldsr.pay_opn_br_no));
            memset(dsf.pldsr.pay_ac_no    , 0 , sizeof(dsf.pldsr.pay_ac_no    ));
            memset(dsf.pldsr.pay_name     , 0 , sizeof(dsf.pldsr.pay_name     ));
            memset(dsf.pldsr.pay_addr     , 0 , sizeof(dsf.pldsr.pay_addr     ));
            memset(cTx_amt                , 0 , sizeof(cTx_amt                ));
            
            GETOFFSET2(dsf.pldsr.tx_mx_no     );
            GETOFFSET2(dsf.pldsr.pay_br_no    );
            GETOFFSET2(dsf.pldsr.pay_opn_br_no);
            GETOFFSET2(dsf.pldsr.pay_ac_no    );
            GETOFFSET2(dsf.pldsr.pay_name     );
            GETOFFSET2(dsf.pldsr.pay_addr     );
            
            zip_space(dsf.pldsr.pay_ac_no);
            zip_space(dsf.pldsr.pay_name);

            GETOFFSET2(cTx_amt);
            str2dbl(cTx_amt,sizeof(cTx_amt)-1,0,&dsf.pldsr.tx_amt);/*注意这里得到的金额的正确性*/
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldsr.tx_amt);
            dsf.pldsr.tx_amt = dsf.pldsr.tx_amt/100;
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldsr.tx_amt);
            GETOFFSET2(dsf.pldsr.xyno);
            zip_space(dsf.pldsr.xyno);
            
            GETOFFSET2(dsf.pldsr.content);

            vtcp_log("[%s][%d],tx_mx_no=[%s]\n",__FILE__,__LINE__,dsf.pldsr.tx_mx_no);

            /*插入登记簿*/
            
            g_reply_int = Lv_pldsr_Ins(dsf.pldsr,g_pub_tx.reply );
            if(g_reply_int)
            {
                sprintf( acErrMsg,"[%s][%d]插入Lv_pldsr表出错code=[%d]",__FL__,g_reply_int);
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                return(-1);
            }
            vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
            vtcp_log("[%s][%d],查看tx_mx_no[%s]\n",__FILE__,__LINE__,dsf.pldsr.tx_mx_no);
            
            vtcp_log("[%s][%d]cTx_amt=[%s]",__FILE__,__LINE__,cTx_amt);
            fen2yan(cTx_amt,yanamt);
            vtcp_log("[%s][%d]yanamt=[%s]",__FILE__,__LINE__,yanamt);
            
            fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld%s%s\n",dsf.pldsr.pay_ac_no,dsf.pldsr.pay_name,
                    yanamt,dsf.pldsr.xyno,dsf.pldsr.pay_br_no,dsf.pldsr.pay_opn_br_no,
                    dsf.pldsr.wt_date,dsf.pldsr.orderno,dsf.pldsr.tx_mx_no);
            vtcp_log("[%s][%d] pldsr.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.pldsr.tx_mx_no);
        }
        
        fclose(fp);
        vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
        lv_return_40506(lv,&dsf.pldsr,"40502",iCount,allCount,return40506Pack);
        
        
    }
    else if (!memcmp(yw_type,"40503",5))
    {
        return 0;
    }
    else if (!memcmp(yw_type,"40504",5))
    {
        return 0;
    }
    else if (!memcmp(yw_type,"40505",5))
    {
        memset(cDsf_date   ,0,sizeof(cDsf_date));
        memset(cO_wt_date  ,0,sizeof(cO_wt_date));
        
        memcpy(dsf.pldscx.packid,packid,sizeof(dsf.pldscx.packid)-1);
        memcpy(dsf.pldscx.orderno,orderno,sizeof(dsf.pldscx.orderno)-1);

        dsf.pldscx.wt_date = apatoi(wt_date,8);
        lv_get_tag(lv,"58A",dsf.pldscx.br_no);
        lv_get_tag(lv,"52A",dsf.pldscx.or_br_no);
        vtcp_log("[%s][%d]dsf.pldscx.wt_date=[%ld]",__FILE__,__LINE__,dsf.pldscx.wt_date);
        offset=0;
        offset=lv_get_tag_p(lv,"72C");
        offset+=5;
        
        GETOFFSET2(cO_wt_date);
        dsf.pldscx.o_wt_date = apatoi(cO_wt_date,8);
        vtcp_log("[%s][%d]pldscx.o_wt_date=[%ld]",__FILE__,__LINE__,dsf.pldscx.o_wt_date);
        
        GETOFFSET2(dsf.pldscx.o_orderno);
        GETOFFSET2(dsf.pldscx.o_yw_type);
        
        GETOFFSET2(cDsf_date);
        dsf.pldscx.dsf_date = apatoi(cDsf_date,8);
        vtcp_log("[%s][%d]pldscx.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldscx.dsf_date);
        
        GETOFFSET2(dsf.pldscx.content);
        
        lv_is40505fail(&dsf.pldscx);

        vtcp_log("[%s][%d]lv_pldscx_c.rowid=[%s]       \n",__FILE__,__LINE__,dsf.pldscx.rowid);
        vtcp_log("[%s][%d]lv_pldscx_c.packid=[%s]      \n",__FILE__,__LINE__,dsf.pldscx.packid);
        vtcp_log("[%s][%d]lv_pldscx_c.orderno=[%s]     \n",__FILE__,__LINE__,dsf.pldscx.orderno);
        vtcp_log("[%s][%d]lv_pldscx_c.wt_date=[%ld]    \n",__FILE__,__LINE__,dsf.pldscx.wt_date);
        vtcp_log("[%s][%d]lv_pldscx_c.br_no=[%s]       \n",__FILE__,__LINE__,dsf.pldscx.br_no);
        vtcp_log("[%s][%d]lv_pldscx_c.or_br_no=[%s]    \n",__FILE__,__LINE__,dsf.pldscx.or_br_no);
        vtcp_log("[%s][%d]lv_pldscx_c.o_wt_date=[%ld]  \n",__FILE__,__LINE__,dsf.pldscx.o_wt_date);
        vtcp_log("[%s][%d]lv_pldscx_c.o_orderno=[%s]   \n",__FILE__,__LINE__,dsf.pldscx.o_orderno);
        vtcp_log("[%s][%d]lv_pldscx_c.o_yw_type=[%s]   \n",__FILE__,__LINE__,dsf.pldscx.o_yw_type);
        vtcp_log("[%s][%d]lv_pldscx_c.dsf_date=[%ld]   \n",__FILE__,__LINE__,dsf.pldscx.dsf_date);
        vtcp_log("[%s][%d]lv_pldscx_c.content=[%s]     \n",__FILE__,__LINE__,dsf.pldscx.content);
        vtcp_log("[%s][%d]lv_pldscx_c.sts=[%s]         \n",__FILE__,__LINE__,dsf.pldscx.sts);
        vtcp_log("[%s][%d]lv_pldscx_c.beg_sts=[%s]     \n",__FILE__,__LINE__,dsf.pldscx.beg_sts);
        vtcp_log("[%s][%d]lv_pldscx_c.operlevel=[%s]   \n",__FILE__,__LINE__,dsf.pldscx.operlevel);
        vtcp_log("[%s][%d]lv_pldscx_c.re_dsf_date=[%ld]\n",__FILE__,__LINE__,dsf.pldscx.re_dsf_date);
        vtcp_log("[%s][%d]lv_pldscx_c.repstat2=[%s]    \n",__FILE__,__LINE__,dsf.pldscx.repstat2);
        vtcp_log("[%s][%d]lv_pldscx_c.re_content2=[%s] \n",__FILE__,__LINE__,dsf.pldscx.re_content2);

        /**插入撤销表**/
        g_reply_int = Lv_pldscx_Ins(dsf.pldscx,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]插入Lv_pldscx表出错code=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return(-1);
        }
        if ( memcmp(dsf.pldscx.sts,DSF_FAIL,2)==0 ||
             memcmp(dsf.pldscx.sts,DSF_QSSU,2)==0 ||
             memcmp(dsf.pldscx.sts,DSF_QSFA,2)==0  ) 
        {
            /*直接回应撤销失败*/
            lv_return_40506(lv,&dsf.pldscx,"40505",iCount,allCount,return40506Pack);
        }
        vtcp_log("[%s][%d] pldscx.repstat2=[%s]",__FILE__,__LINE__,dsf.pldscx.repstat2);
    }
    else if (!memcmp(yw_type,"40506",5))
    {
        /*只有在合同号变更的时候要求接收40506*/
        char dsfdate[9];
        char txtype[6];
        int iRc=0;
        memset(cWt_date    ,0,sizeof(cWt_date));
        memset(cRe_dsf_date,0,sizeof(cRe_dsf_date));
        iRc = dsf_recv40506(lv,packid);
        if (iRc != 1)
        {
            return iRc;
        }
        lv_get_tag(lv,"58A",dsf.pldsfqugx.or_br_no);
        offset=0;
        offset=lv_get_tag_p(lv,"72C");
        offset+=5;
        
        GETOFFSET2(cWt_date);
        dsf.pldsfqugx.wt_date = apatoi(cWt_date,8);
        vtcp_log("[%s][%d]pldsfqugx.wt_date=[%ld]",__FILE__,__LINE__,dsf.pldsfqugx.wt_date);
        
        GETOFFSET2(dsf.pldsfqugx.orderno);

        
        g_reply_int = Lv_pldsfqugx_Dec_Upd(g_pub_tx.reply,"orderno='%s' and wt_date='%ld' and or_br_no='%s' ",\
                      dsf.pldsfqugx.orderno,dsf.pldsfqugx.wt_date,dsf.pldsfqugx.or_br_no);   
        if(g_reply_int)
        {
             vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
             sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
             set_zd_data(DC_GET_MSG,acErrMsg);
             return(-1);
        }
        g_reply_int = Lv_pldsfqugx_Fet_Upd( &dsf.pldsfqugx,g_pub_tx.reply );
        if (g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(-1);
        }
        else
        {
            GETOFFSET2(txtype);

            GETOFFSET2(cRe_dsf_date);
            dsf.pldsfqugx.re_dsf_date = apatoi(cRe_dsf_date ,8);
            vtcp_log("[%s][%d]pldsfqugx.re_dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldsfqugx.re_dsf_date);
            
            GETOFFSET2(dsf.pldsfqugx.repstat);
            GETOFFSET2(dsf.pldsfqugx.re_content2);
            if (memcmp(dsf.pldsfqugx.repstat,"00",2)==0)
            {
                memcpy(dsf.pldsfqugx.sts,DSF_QSSS,2);
            }
            else
            {
                memcpy(dsf.pldsfqugx.sts,DSF_QSSF,2);
            }
        }

        g_reply_int = Lv_pldsfqugx_Upd_Upd(dsf.pldsfqugx, g_pub_tx.reply);
        if (g_reply_int)
        {
            vtcp_log("%s,%Upd lv_pldsfqugx表出错[%d]",__FILE__,__LINE__,g_reply_int);
            return (g_reply_int);
        }
        Lv_pldsfqugx_Clo_Upd();
    }
    else if (!memcmp(yw_type,"40507",5))
    {
        return 0;
    }
    else 
    { 
        /*非代代付类业务*/
        vtcp_log("[%s][%d],不是代收付业务[%s],成功处理\n",__FILE__,__LINE__,yw_type);
        return 0;
    }
}

static int lv_get_xmh(struct lv_pldfr_c *df)
{
    struct lv_xmh_c lv_xmh;
    char actno[34];
    int  iRc=0;
    double avbal=0.0;
    char s_avbal[100];
    char cWt_date[9];
    char cOrbrno[13];
    char cQsbrno[13];
    char cTmpBuf[61];
    struct hv_uniontab_c hv_uniontab;
    
    /*数据初始化*/
    
    memset(cWt_date,0, sizeof(cWt_date));
    memset(actno   ,0, sizeof(actno));
    memset(cOrbrno ,0, sizeof(cOrbrno));
    memset(cQsbrno ,0, sizeof(cQsbrno));
    memset(cTmpBuf ,0, sizeof(cTmpBuf));
    memset(&lv_xmh ,0, sizeof(lv_xmh));
    memset(&hv_uniontab,0,sizeof(hv_uniontab));
    
    /********这里要特殊处理一下********/
    memcpy(df->pay_name,(char*)DBC2SBC(df->pay_name,sizeof(df->pay_name)-1),sizeof(df->pay_name)-1);/*户名做全半角转换 add by ChengGX 20100603*/
    iRc = get_name(df->pay_ac_no,df->pay_name,s_avbal);
    avbal = atof(s_avbal);
    vtcp_log("[%s][%d][%d]账号的余额是[%s]\n",__FILE__,__LINE__,iRc,s_avbal);
    if(iRc ==-1 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"01",2);
        sprintf(df->re_content2,"账号不存在");
        vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
        return 1;
    }
    else if(iRc ==-2 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"02",2);
        memcpy(df->re_content2,"账号、户名不符",14);
        return 2;
    }
    else if(iRc ==-3 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"11",2);
        memcpy(df->re_content2,"账号状态错误",12);
        return 3;
    }
    else
    {
        double totamt;
        totamt = df->totamt;
        if ( avbal<totamt )
        {
            memcpy(df->sts,DSF_FAIL,2);
            memcpy(df->repstat2,"03",2);
            vtcp_log("[%s][%d]账号的余额是[%f]交易金额是[%f]\n",__FILE__,__LINE__,avbal,totamt);
            sprintf(df->re_content2,"余额不足\n");
            return 3;
        }
    }
    memcpy(df->sts,DSF_SUCC,2);
    memcpy(df->repstat2,"00",2);
    memcpy(df->re_content2,"回应成功",8);
    vtcp_log("[%s][%d] df->re_content2=[%s]",__FILE__,__LINE__,df->re_content2);
    
    /*数据检查,检查项目号是否存在*/
    if (memcmp(df->xyno,XYNO,60)!=0)
    {
        vtcp_log("[%s][%d]协议号不是全0,请不要用本函数取协议号",__FILE__,__LINE__);
        return -1;
    }
    
    /*----查找账号相同的,取这个协议号---*/
    memcpy(actno,df->pay_ac_no,sizeof(lv_xmh.ac_no)-1);
    memcpy(cQsbrno,df->cash_qs_no,sizeof(cQsbrno)-1);
    vtcp_log("[%s][%d]cQsbrno=[%s]",__FILE__,__LINE__,cQsbrno);
    trim(actno);
    strcat(actno,"%");

    Lv_xmh_Dec_Sel(g_pub_tx.reply,"ac_no like '%s' and xm_ind='0'  and o_qs_no='%s' ",actno,cQsbrno);
    while(1)
    {
        memset(&lv_xmh,0,sizeof(lv_xmh));

        g_reply_int = Lv_xmh_Fet_Sel( &lv_xmh,g_pub_tx.reply );
        if (g_reply_int)
        {
            break;
        }

        vtcp_log("[%s][%d],lv_xmh.xmno=[%s]\n",__FILE__,__LINE__,lv_xmh.xmno);
        memcpy(df->xyno,lv_xmh.xmno,sizeof(lv_xmh.xmno)-1);

        if ( dsfnamecmp(df->pay_name,lv_xmh.name)!=0 )
        {
            memcpy(df->sts,DSF_FAIL,2);
            memcpy(df->repstat2,"02",2);
            memcpy(df->re_content2,"付款人与协议户名不符",20);
            return 2;
        }
        
        /****这里不是就比较一个付款人账户吧,TODO****/
        
        
        
        return 0;
    }
    Lv_xmh_Clo_Sel();
    vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
    /*如果没有,建立一个,协议号按日期+账号来决定*/
    if (lv_xmh.xmno[0]=='\0' || lv_xmh.xmno[0]==' ')    /*为空或是空格都是没有取到*/
    {
        memset(&lv_xmh,0,sizeof(&lv_xmh));
        apitoa(df->wt_date,sizeof(cWt_date)-1,cWt_date);
        memcpy(cTmpBuf,cWt_date,sizeof(cWt_date)-1);
        strcat(cTmpBuf,df->pay_ac_no);
        strcat(cTmpBuf,df->cash_qs_no);/*mod by ChengGX 20100607 衡水  同一个包中包含多个明细*/
        strcat(cTmpBuf,df->packid);/**为了防止一天来了几个同一帐户不同清算行的来帐**/
        strcat(cTmpBuf,"df");
        zip_space(cTmpBuf);
        vtcp_log("[%s][%d]cTmpBuf=[%s]",__FILE__,__LINE__,cTmpBuf);
        memcpy(lv_xmh.xmno,cTmpBuf,strlen(cTmpBuf));
        trim(lv_xmh.xmno);
        vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
        memcpy(lv_xmh.br_no, QS_BR_NO ,sizeof(lv_xmh.br_no)-1);
        lv_xmh.xm_ind[0]='0';
        lv_xmh.flg[0]='0';
        memcpy(lv_xmh.ac_no,df->pay_ac_no,sizeof(lv_xmh.ac_no)-1);
        memcpy(lv_xmh.name,df->pay_name,sizeof(lv_xmh.name)-1);
        memcpy(lv_xmh.yw_ind,df->yw_type,sizeof(df->yw_type));
        /**这里添加上xmno中的o_qs_no**/
        memcpy(cOrbrno,df->cash_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);
        
        memcpy(lv_xmh.tel , g_pub_tx.tel, sizeof(lv_xmh.tel)-1);
        vtcp_log("[%s][%d] 操作员为=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);

        memcpy(lv_xmh.brf , "批量增加代付协议号" , sizeof(lv_xmh.brf)-1);
        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]查询对方清算中心号码错误\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no==[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
       
        g_reply_int = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]插入LV_XMH表出错",__FL__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return -1;
        }
        memcpy(df->xyno,lv_xmh.xmno,sizeof(lv_xmh.xmno)-1);
        return 0;
    }
    return 0;
}

int lv_check_xmh(struct lv_pldfr_c *pldfr)
{
    int ret=0;
    double avbal,totamt,dbavbal=0.0;
    char s_avbal[100];
    char cOrbrno[13];
    struct lv_xmh_c lv_xmh;
    struct hv_uniontab_c hv_uniontab;
    
    memset(cOrbrno,0,sizeof(cOrbrno));
    memset(&hv_uniontab,0,sizeof(hv_uniontab));
    memset(&lv_xmh     ,0,sizeof(lv_xmh));
    
    /***这里先检查帐号和余额,如果帐号不存在就直接return;
        如果户名不符,直接return;如果金额不足,直接return;
        否则,暂时回执成功,再判断多方协议好的正确性***/
    memcpy(pldfr->pay_name,(char*)DBC2SBC(pldfr->pay_name,sizeof(pldfr->pay_name)-1),sizeof(pldfr->pay_name)-1);/*add by ChengGX 20100603全半角转换*/
    ret = get_name(pldfr->pay_ac_no,pldfr->pay_name,s_avbal);
    vtcp_log("[%s][%d] ret[%d] avbal=[%f]\n",__FILE__,__LINE__,ret,s_avbal);
    if(ret ==-1 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"01",2);
        sprintf(pldfr->re_content2,"账号不存在");
        vtcp_log("[%s][%d]已经到这里了ret=[%d]",__FILE__,__LINE__,ret);
        return 1;
    }
    else if(ret ==-2 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"02",2);
        memcpy(pldfr->re_content2,"账号、户名不符",14);
        vtcp_log("[%s][%d]已经到这里了ret=[%d]",__FILE__,__LINE__,ret);
        return 2;
    }
    else if(ret ==-3 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"11",2);
        memcpy(pldfr->re_content2,"账号状态错误",12);
        vtcp_log("[%s][%d]已经到这里了ret=[%d]",__FILE__,__LINE__,ret);
        return 3;
    }
    vtcp_log("[%s][%d]账号的余额为[%s]\n",__FILE__,__LINE__,s_avbal);
    avbal = atof(s_avbal);
    totamt = pldfr->totamt;
    vtcp_log("[%s][%d]交易totamt==【%f】",__FILE__,__LINE__,totamt);
    vtcp_log("[%s][%d]余额avbal ==【%f】",__FILE__,__LINE__,avbal);
    if ( avbal<totamt )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"03",2);
        sprintf(pldfr->re_content2,"余额不足\n");
        vtcp_log("[%s][%d]已经到这里了ret=[%d]",__FILE__,__LINE__,ret);
        return 3;
    }
    memcpy(pldfr->sts,DSF_SUCC,2);
    memcpy(pldfr->repstat2,"00",2);
    memcpy(pldfr->re_content2,"回应成功",10);
    vtcp_log("[%s][%d] pldfr->re_content2,=[%s]",__FILE__,__LINE__,pldfr->re_content2);
    
    memcpy(lv_xmh.xmno,pldfr->xyno,60);
    memcpy(lv_xmh.o_qs_no,pldfr->cash_qs_no,12);
    vtcp_log("[%s][%d]lv_xmh.o_qs_no=[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
    vtcp_log("[%s][%d],正在核查合同号[%s]\n",__FILE__,__LINE__,pldfr->xyno);

    ret = Lv_xmh_Sel(g_pub_tx.reply,&lv_xmh,"xmno='%s' and xm_ind='0' and o_qs_no='%s' ",lv_xmh.xmno,lv_xmh.o_qs_no);
    if(ret == 100 && getenv("FHTEST")!=NULL && strcmp(getenv("FHTEST"),"LVTEST")==0)
    {
        memcpy(lv_xmh.ac_no,pldfr->pay_ac_no,18);
        zip_space(lv_xmh.ac_no);
        memcpy(lv_xmh.name,pldfr->pay_name,sizeof(pldfr->pay_name));
        memcpy(lv_xmh.yw_ind,pldfr->yw_type,sizeof(pldfr->yw_type));
        lv_xmh.flg[0]='0';
        lv_xmh.xm_ind[0]='0';/**贷**/
        memcpy(lv_xmh.br_no, QS_BR_NO ,sizeof(lv_xmh.br_no)-1);
        /**这里添加上xmno中的o_qs_no**/
        memcpy(cOrbrno,pldfr->cash_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);

        memcpy(lv_xmh.tel , g_pub_tx.tel, sizeof(lv_xmh.tel)-1);
        vtcp_log("[%s][%d] 操作员为=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);
        
        memcpy(lv_xmh.brf , "批量增加测试协议号" , sizeof(lv_xmh.brf)-1);

        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]查询对方清算中心号码错误\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no==[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
        vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
        ret = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );

    }
    if(ret)
    {
        vtcp_log("[%s][%d],查询项目表错sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"90",2);
        memcpy(pldfr->re_content2,"无此合同号",10);
        vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
        return(99); /*返回1表示账号不存在,因为现在的回执并没有合同号错的情况,不对以后再改吧*/
    }

    /*检查账号*/
    trim(lv_xmh.ac_no);
    trim(pldfr->pay_ac_no);
    if ( memcmp(lv_xmh.ac_no,pldfr->pay_ac_no,strlen(lv_xmh.ac_no)) )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"01",2);
        sprintf(pldfr->re_content2,"账号不存在");
        vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
        return 1;
    }
    /*检查名称*/
    trim(lv_xmh.name);
    trim(pldfr->pay_name);
    memcpy(pldfr->pay_name,(char*)DBC2SBC(pldfr->pay_name,sizeof(pldfr->pay_name)-1),sizeof(pldfr->pay_name)-1);/*全半角转换 add by ChengGX 20100603*/
    if ( memcmp(lv_xmh.name,pldfr->pay_name,strlen(lv_xmh.name)) )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"02",2);
        sprintf(pldfr->re_content2,"账号、户名不符");
        return 2;
    }
    return 0; 
}

/*这个函数把分为单位的金额改成带小数点的*/
int  fen2yan(char *famt,char *yamt)
{
    /*榆林的不加*/
    memset(yamt,0,17);
    memcpy(yamt,famt,15);
    return 0;
    memset(yamt,0,17);
    memcpy(yamt,famt,13);
    strcat(yamt,".");
    memcpy(yamt+14,famt+13,2);
    return 0;
}

int lv_get_xmno(struct lv_pldsr_c *ds)
{
    struct lv_xmh_c lv_xmh;
    char actno[34];
    int  iRc=0;
    double avbal=0.0;
    char s_avbal[100];
    char cAvbal[21];
    char cWt_date[9];
    char cOrbrno[13];
    char cQsbrno[13];
    char cTmpBuf[61];
    struct hv_uniontab_c hv_uniontab;
    
    /*数据初始化*/
    memset(cOrbrno  ,0, sizeof(cOrbrno));
    memset(cWt_date ,0, sizeof(cWt_date));
    memset(cAvbal   ,0, sizeof(cAvbal));
    memset(actno    ,0, sizeof(actno));
    memset(cQsbrno  ,0, sizeof(cQsbrno));
    memset(cTmpBuf  ,0, sizeof(cTmpBuf));
    memset(&hv_uniontab,0,sizeof(hv_uniontab));
    memcpy(ds->cash_name,(char*)DBC2SBC(ds->cash_name,sizeof(ds->cash_name)-1),sizeof(ds->cash_name)-1);/**全半角转换add by ChengGX 20100603**/
    iRc = get_name(ds->cash_ac_no,ds->cash_name,s_avbal);
    vtcp_log("[%s][%d] iRc=[%d] 余额为=[%f]",__FILE__,__LINE__,iRc,s_avbal);
    if(iRc == -1)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"01",2);
        sprintf(ds->re_content2,"账号不存在");
        vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
        return 1;
    }
    else if (iRc==-2)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"02",2);
        memcpy(ds->re_content2,"账号、户名不符",14);
        return 2;
    }
    else if (iRc==-3)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"11",2);
        memcpy(ds->re_content2,"账号状态错误",12);
        return 3;
    }
    vtcp_log("[%s][%d]检查完成\n",__FILE__,__LINE__);
    
    memcpy(ds->sts,DSF_SUCC,2);
    memcpy(ds->repstat2,"00",2);
    memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-5-20  衡水*/
    memcpy(ds->re_content2,"成功回执",8);
    /*查找账号相同的,取这个协议号*/
    
    memcpy(actno,ds->cash_ac_no,sizeof(ds->cash_ac_no)-1);
    memcpy(cQsbrno,ds->pay_qs_no,sizeof(cQsbrno)-1);
    vtcp_log("[%s][%d]cQsbrno=[%s]",__FILE__,__LINE__,cQsbrno);
    trim(actno);
    strcat(actno,"%");
    vtcp_log("[%s][%d]要查询的账号为[%s]\n",__FILE__,__LINE__,actno);

    Lv_xmh_Dec_Sel(g_pub_tx.reply,"ac_no like '%s' and xm_ind='1' and o_qs_no='%s'",actno,cQsbrno);
    while(1)
    {
        memset(&lv_xmh,0,sizeof(lv_xmh));

        g_reply_int = Lv_xmh_Fet_Sel( &lv_xmh,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]DEBUG:代收的时候没有就生成一个项目号\n",__FILE__,__LINE__);
            memcpy(ds->sts,DSF_REJE,2);
            memcpy(ds->repstat2,"01",2);
            sprintf(ds->re_content2,"账号不存在");
            break;
        }
        vtcp_log("[%s][%d],lv_xmh.xmno=[%s]\n",__FILE__,__LINE__,lv_xmh.xmno);
        memcpy(ds->xyno,lv_xmh.xmno,sizeof(lv_xmh.xmno)-1);

        memcpy(ds->sts,DSF_SUCC,2);
        memcpy(ds->repstat2,"00",2);
        memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-6-03  衡水*/
        memcpy(ds->re_content2,"成功回执",8);
        return 0;
    }
    Lv_xmh_Clo_Sel();
    vtcp_log("[%s][%d]xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
    /*如果没有,建立一个,协议号按日期+账号来决定*/
    if (lv_xmh.xmno[0]=='\0' || lv_xmh.xmno[0]==' ')    /*为空或是空格都是没有取到*/
    {
        memset(&lv_xmh,0,sizeof(&lv_xmh));
        apitoa(ds->wt_date,sizeof(cWt_date)-1,cWt_date);
        vtcp_log("[%s][%d]cWt_date=[%s]",__FILE__,__LINE__,cWt_date);
        memcpy(cTmpBuf,cWt_date,sizeof(cWt_date)-1);
        strcat(cTmpBuf,ds->cash_ac_no);
        strcat(cTmpBuf,ds->pay_qs_no);/*mod by ChengGX 20100607 衡水  同一个包中包含多个明细*/
        strcat(cTmpBuf,ds->packid);/**为了防止一天来了几个同一帐户不同清算行的来帐**/
        strcat(cTmpBuf,"ds");
        zip_space(cTmpBuf);
        vtcp_log("[%s][%d]cTmpBuf=[%s]",__FILE__,__LINE__,cTmpBuf);
        memcpy(lv_xmh.xmno,cTmpBuf,strlen(cTmpBuf));
        trim(lv_xmh.xmno);
        vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
        lv_xmh.xm_ind[0]='1';
        lv_xmh.flg[0]='0';
        memcpy(lv_xmh.yw_ind,ds->yw_type,sizeof(ds->yw_type));
        memcpy(lv_xmh.ac_no,ds->cash_ac_no,sizeof(lv_xmh.ac_no)-1);
        vtcp_log("[%s][%d]lv_xmh.ac_no=[%s]",__FILE__,__LINE__,lv_xmh.ac_no);
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        
        memcpy(lv_xmh.tel , g_pub_tx.tel, sizeof(lv_xmh.tel)-1);
        vtcp_log("[%s][%d] 操作员为=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);
        
        memcpy(lv_xmh.brf , "批量增加代收协议号" , sizeof(lv_xmh.brf)-1);
        
        /*********这里需要特殊处理一下,根据帐号取户名*************/
        g_reply_int = get_name(lv_xmh.ac_no,lv_xmh.name,cAvbal);
        if(g_reply_int )
        {
            memcpy(ds->sts,DSF_FAIL,2);
            memcpy(ds->repstat2,"01",2);
            sprintf(ds->re_content2,"账号不存在");
            vtcp_log("[%s][%d]已经到这里了",__FILE__,__LINE__);
            return 1;
        }
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        
        /**这里添加上xmno中的o_qs_no**/
        memcpy(cOrbrno,ds->pay_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);

        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]查询对方清算中心号码错误\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        strcpy(lv_xmh.br_no, QS_BR_NO ); /**这里赋值清算中心**/
        vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
        vtcp_log("[%s][%d]lv_xmh.ac_no=[%s]",__FILE__,__LINE__,lv_xmh.ac_no);
        vtcp_log("[%s][%d]lv_xmh.xm_ind=[%s]",__FILE__,__LINE__,lv_xmh.xm_ind);
        vtcp_log("[%s][%d]lv_xmh.flg=[%s]",__FILE__,__LINE__,lv_xmh.flg);
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no=[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
        g_reply_int = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]插入项目表出错code==[%d]\n",__FILE__,__LINE__,g_reply_int);
            return -1;
        }
        vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
        vtcp_log("[%s][%d]lv_xmh.ac_no=[%s]",__FILE__,__LINE__,lv_xmh.ac_no);
        vtcp_log("[%s][%d]lv_xmh.xm_ind=[%s]",__FILE__,__LINE__,lv_xmh.xm_ind);
        vtcp_log("[%s][%d]lv_xmh.flg=[%s]",__FILE__,__LINE__,lv_xmh.flg);
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        memcpy(ds->xyno,lv_xmh.xmno,sizeof(lv_xmh.xmno)-1);
        memcpy(ds->sts,DSF_SUCC,2);
        memcpy(ds->repstat2,"00",2);
        memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-6-03  衡水*/
        memcpy(ds->re_content2,"成功回执",8);
        return 0;
    }
    vtcp_log("[%s][%d]取协议号已经完成\n",__FILE__,__LINE__);
    return -1;
}

static int lv_is40505fail(struct lv_pldscx_c *cx)
{
    struct  lv_pldsr_c ds;
    char    s_orderno[9],s_tx_mx_no[9];
    long    s_wt_date=0;
    char    cYw_type[6];
    
    memset(&ds,0,sizeof(ds));
    memset(s_orderno,0,sizeof(s_orderno));
    memset(cYw_type ,0,sizeof(cYw_type));
    memset(s_tx_mx_no,0,sizeof(s_tx_mx_no));
    
    s_wt_date = cx->o_wt_date;
    memcpy(s_orderno,cx->o_orderno,sizeof(s_orderno)-1);
    memcpy(cYw_type ,cx->o_yw_type,sizeof(cYw_type)-1);
    memset(s_tx_mx_no,'0',sizeof(s_tx_mx_no)-1);
    vtcp_log("[%s][%d]s_orderno==[%s]",__FILE__,__LINE__,s_orderno);
    vtcp_log("[%s][%d]s_wt_date==[%ld]",__FILE__,__LINE__,s_wt_date);
    vtcp_log("[%s][%d]s_tx_mx_no=[%s]",__FILE__,__LINE__,s_tx_mx_no);
    vtcp_log("[%s][%d]cYw_type==[%s]",__FILE__,__LINE__,cYw_type);
    Lv_pldsr_Dec_Upd(g_pub_tx.reply,"orderno='%s' and wt_date='%ld' and tx_mx_no='%s' and yw_type='%s' ",s_orderno,s_wt_date,s_tx_mx_no,cYw_type);
    
    vtcp_log("[%s][%d],open fail40505_cur success!\n",__FILE__,__LINE__);
    while(1)
    {
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Upd( &ds,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]原交易数据已经取完sqlcode[%d]",__FILE__,__LINE__,g_reply_int);
            break;
        }
        /*如果已经回复了40504,就回复失败*/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,ds.repstat);
        if ( isdigit(ds.repstat[0]) || memcmp(ds.repstat,"AA",2)==0)
        {
            memcpy(cx->sts,DSF_FAIL,2);
            memcpy(cx->repstat2,"90",2);
            strcpy(cx->re_content2,"本交易已经回复");
            return 90;
        }
        vtcp_log("[%s][%d] here !!",__FILE__,__LINE__);
        memcpy(cx->sts,DSF_CONC,2);
        memcpy(cx->repstat2,"90",2);
        strcpy(cx->re_content2,"等待撤销原交易");
        vtcp_log("[%s][%d] here !!",__FILE__,__LINE__);
        /*更改原交易为已撤销*/
        memcpy(ds.sts,"0C",2);
        g_reply_int = Lv_pldsr_Upd_Upd(ds , g_pub_tx.reply);
        if (g_reply_int)
        {
            vtcp_log("%s,%Upd lv_pldsr表出错!!",__FILE__,__LINE__);
            return -1;
        }
        vtcp_log("[%s][%d] cx->sts=[%s]!",__FILE__,__LINE__,cx->sts);
        return 0;
    }
    Lv_pldsr_Clo_Upd();
    vtcp_log("[%s][%d] cx->repstat2=[%s]!!",__FILE__,__LINE__,cx->repstat2);
    memcpy(cx->sts,DSF_FAIL,2);
    memcpy(cx->repstat2,"90",2);
    strcpy(cx->re_content2,"找不到原交易");
    return 90;
}
/*********************************************************
这个函数用来处理除了40507回执之外的回执的回执
*********************************************************/
int dsf_recv40506(TLVPACKAGE *lv,char *packid)
{
    struct lv_dsfsendlist_c dsl,lv_dsfsendlist;
    char repaddit[92];
    char cRewtday[9];
    int iRc=0;

    memset(repaddit,0,sizeof(repaddit));
    memset(&dsl,0,sizeof(dsl));
    memset(&lv_dsfsendlist,0,sizeof(lv_dsfsendlist));
    memset(cRewtday,0,sizeof(cRewtday));
    
    lv_get_tag(lv,"58A",dsl.or_br_no);
    lv_get_tag_multi(lv,"72C",repaddit,0,91);

    dsl.wt_date = apatoi(repaddit,8);
    vtcp_log("[%s][%d]wt_date=[%ld]",__FILE__,__LINE__,dsl.wt_date);
    memcpy(dsl.orderno,repaddit+8,8);
    vtcp_log("[%s][%d]dsl.orderno=[%s]",__FILE__,__LINE__,dsl.orderno);
    memcpy(dsl.tx_type,repaddit+16,5);
    vtcp_log("[%s][%d]dsl.tx_type=[%s]",__FILE__,__LINE__,dsl.tx_type);
    if (memcmp(dsl.tx_type,"40507",5)==0)
    {
        vtcp_log("[%s][%d] dsl.tx_type=[%s]",__FILE__,__LINE__,dsl.tx_type);
        return 1;
    }

    vtcp_log("[%s][%d]准备游标啦!!",__FILE__,__LINE__);
    g_reply_int = Lv_dsfsendlist_Dec_Upd(g_pub_tx.reply,"orderno='%s' and wt_date=%ld and tx_type='%s' \
                                        and or_br_no='%s' ",dsl.orderno,dsl.wt_date,dsl.tx_type,dsl.or_br_no);   
    if(g_reply_int)
    {
         vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
         /****NEWYX BEGIN:找不到原交易不报错****/
         if(g_reply_int==100)
         {
             vtcp_log("[%s][%d] dsl.tx_type=[%s]",__FILE__,__LINE__,dsl.tx_type);
             return(0);
         }
         else
         {
             vtcp_log("[%s][%d] dsl.tx_type=[%s]",__FILE__,__LINE__,dsl.tx_type);
             return(-1);
         }
         /****NEWYX END:找不到原交易不报错****/
    }
    g_reply_int = Lv_dsfsendlist_Fet_Upd( &dsl,g_pub_tx.reply );
    if (g_reply_int!=100 && g_reply_int!=0)
    {
        sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
        sprintf(acErrMsg,"查找原交易出错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }       
    else
    {                                      
        memcpy(dsl.re_packid,packid,8  );
        lv_get_tag(lv,"0BC",dsl.re_orderno );
        lv_get_tag(lv,"30A",cRewtday);
        dsl.re_wt_date   = apatoi(cRewtday,8);
        vtcp_log("[%s][%d]dsl.re_wt_date=[%ld]",__FILE__,__LINE__,dsl.re_wt_date);
        lv_get_tag(lv,"52A",dsl.re_or_br_no);
        vtcp_log("[%s][%d]dsl.re_or_br_no=[%s]",__FILE__,__LINE__,dsl.re_or_br_no);
        lv_get_tag(lv,"58A",dsl.re_ac_br_no);
        vtcp_log("[%s][%d]dsl.re_ac_br_no=[%s]",__FILE__,__LINE__,dsl.re_ac_br_no);
        lv_get_tag(lv,"B40",dsl.re_msg_len  );
        vtcp_log("[%s][%d]dsl.re_msg_len=[%s]",__FILE__,__LINE__,dsl.re_msg_len);
        memcpy(dsl.re_addit,repaddit,91);
        vtcp_log("[%s][%d]dsl.re_addit=[%s]",__FILE__,__LINE__,dsl.re_addit);
        memcpy(dsl.repstat,repaddit+29,2);
        memcpy(dsl.re_content,repaddit+31,60);
        vtcp_log("[%s][%d]dsl.re_content=[%s]",__FILE__,__LINE__,dsl.re_content);
            
        if (memcmp(dsl.repstat,"00",2)==0)
        {
            memcpy(dsl.sts,DSF_QSSS,2);
        }
        else
        {
            memcpy(dsl.sts,DSF_QSSF,2);
        }
        if(g_reply_int == 100)
        {
            memcpy(lv_dsfsendlist.packid   ,dsl.re_packid,sizeof(lv_dsfsendlist.packid)-1);
            memcpy(lv_dsfsendlist.o_orderno  ,dsl.orderno,sizeof(dsl.orderno)-1);
            memcpy(lv_dsfsendlist.o_packid  ,dsl.packid,sizeof(dsl.packid)-1);
            lv_dsfsendlist.o_wt_date     = dsl.wt_date;
            memcpy(lv_dsfsendlist.orderno  ,dsl.re_orderno,sizeof(dsl.re_orderno)-1);
            memcpy(lv_dsfsendlist.or_br_no ,dsl.re_or_br_no,sizeof(dsl.re_or_br_no)-1);
            lv_dsfsendlist.wt_date = dsl.re_wt_date;  
            
            memcpy(lv_dsfsendlist.tx_type ,"40506",5);
            memcpy(lv_dsfsendlist.o_tx_type,dsl.tx_type,5);
            memcpy(lv_dsfsendlist.sts     ,DSF_RECV,2);
            memcpy(lv_dsfsendlist.content ,dsl.re_content,sizeof(lv_dsfsendlist.content)-1);
            g_reply_int  = Lv_dsfsendlist_Ins(lv_dsfsendlist,g_pub_tx.reply ); /*是值传递!!*/
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],插入代收付往账簿错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
                sprintf( acErrMsg,"[%s][%d]插入代收付往账簿错sqlcode=[%d]",__FL__,g_reply_int);
                set_zd_data(DC_GET_MSG,acErrMsg);
                return(-1);
            }
            return (0);
        }
    }

    g_reply_int = Lv_dsfsendlist_Upd_Upd(dsl, g_pub_tx.reply);
    if (g_reply_int)
    {
        vtcp_log("%s,%Upd lv_dsfsendlist表出错[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"lv_dsfsendlist表出错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return -1;
    }
    Lv_dsfsendlist_Clo_Upd();
    vtcp_log("[%s][%d]o_tx_type=[%s]",__FILE__,__LINE__,dsl.o_tx_type);
    if ( memcmp(dsl.o_tx_type,"40501",5)==0)
    {
        g_reply_int = sql_execute("update lv_pldfr set repstat='%s',re_content='%s' where wt_date='%ld' and orderno ='%s' ",\
                                   dsl.repstat,dsl.re_content,dsl.o_wt_date,dsl.orderno);
        if(g_reply_int)
        {
          vtcp_log("[%s][%d],更新lv_pldfr表失败[%d]",__FILE__,__LINE__, g_reply_int);
          sprintf(acErrMsg,"更新lv_pldfr表失败[%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,acErrMsg);
          return(g_reply_int);
        }
    } 
    else if (memcmp(dsl.o_tx_type,"40502",5)==0)
    {
        g_reply_int = sql_execute("update lv_pldsr set repstat='%s',re_content='%s' where wt_date='%ld' and orderno ='%s' ",\
                                   dsl.repstat,dsl.re_content,dsl.o_wt_date,dsl.orderno);
        if(g_reply_int)
        {
          vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
          sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,acErrMsg);
          return(g_reply_int);
        }      
    }

    return 0;
}

static int lv_40506_df(struct lv_pldfr_c *pldfr,int ret)
{
    /*现在不需要这个函数了*/
    return 0;
}

static int lv_40506_ds(struct lv_pldsr_c *pldsr,int ret)
{
    pldsr->re_dsf_date = pldsr->dsf_date;
    sprintf(pldsr->repstat2,"%02d",ret);
    if ( ret != 0 )
    {
        sprintf(pldsr->re_content2,"%s","检查合同号出错"); 
    }
    return 0;
}

/*本函数处理发送40503回执业务*/
int lv_dsf_deal40503(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc=0 ,iSocketID=0; 
    struct lv_pldfr_c df;
    struct lv_pldfr_c df2;
    char lvdate[9];
    TLVPACKAGE lv;    /*40503返回包*/
    char sorderno[9],temp[100];
    char s_sucnum[9],s_sucamt[16];
    char cTotamt[16];
    char cTx_amt[16];
    char cSuccamt[16];
    
    memset(cSuccamt,0,sizeof(cSuccamt));
    memset(cTotamt ,0,sizeof(cTotamt));
    memset(cTx_amt ,0,sizeof(cTx_amt));
    memset(sorderno,0,sizeof(sorderno));
    memcpy(sorderno,orderno,8);
    memset(s_sucnum,0,sizeof(s_sucnum));
    memset(s_sucamt,0,sizeof(s_sucamt));
    
    if(dsf_candfret(wt_date)==1)
    {
        vtcp_log("[%s][%d]回执期限未到,不能回执40503\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]回执期限未到,不能回执40503",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d]here here !!",__FILE__,__LINE__);
    if (memcmp(repstat,"AA",2)==0)
    {
        vtcp_log("[%s][%d],找原交易号为[%s]日期为[%s]\n",__FILE__,__LINE__,orderno,wt_date);
        
        g_reply_int = dsf_dj2df(wt_date,orderno,s_sucnum,s_sucamt);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]代付自动回执失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]代付自动回执失败",__FILE__,__LINE__);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]　自动回执成功笔数为[%s],成功金额为[%s]\n",__FILE__,__LINE__,s_sucnum,s_sucamt);
        
    }
    lv_load_tagifno();    
    lv_create(&lv);
    memset(lvdate,0,sizeof(lvdate));
    memcpy(lvdate,pcLvGetDay_work(),8);
    vtcp_log("[%s][%d],lvdate=[%s]",__FILE__,__LINE__,lvdate);
    memset(&df,0,sizeof(df));
    memset(&df2,0,sizeof(df2));
    memcpy(df.packid,packid,sizeof(df.packid)-1);
    df.wt_date = apatoi(wt_date,8);
    memcpy(df.orderno,orderno,sizeof(df.orderno)-1);
    
    memset(df.tx_mx_no,'0',8);    /*全0为全记录*/
    vtcp_log("[%s][%d] packid=[%s][%ld][%s]\n",__FILE__,__LINE__,df.packid,df.wt_date,df.orderno);

    g_reply_int = Lv_pldfr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",df.packid,df.orderno,df.wt_date,df.tx_mx_no);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Dec_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldfr_Fet_Upd(&df,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Fet_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    else
    {   
        /*如果已经回应,直接报错*/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,df.repstat);
        if (isdigit(df.repstat[0]) || memcmp(df.repstat,"AA",2)==0)
        {
            vtcp_log("[%s][%d]本交易己被回应repstat=[%s]",__FILE__,__LINE__,df.repstat);
            sprintf(errcode,"本交易己被回应,回应结果为[%s]",df.repstat);
            return -1;
        }
        /*如果不是回执成功状态,直接报错*/
        if (memcmp(df.sts,DSF_SUCC,2)!=0 && memcmp(df.sts,DSF_QSFA,2)!=0)
        {
            vtcp_log("[%s][%d]业务当前状态不是成功回执状态,不能回复40503",__FILE__,__LINE__);
            sprintf(errcode,"业务当前状态不是成功回执状态,不能回复40503[%s]",df.sts);
            return -1;
        }
        statof405034(df.sts,repstat);
        memcpy(df.repstat,repstat,sizeof(df.repstat)-1);
        strncpy(df.re_content,re_content,sizeof(df.re_content)-1);
        if (memcmp(repstat,"AA",2)==0)
        {
            df.succnum = apatoi(s_sucnum,8);
            str2dbl(s_sucamt,strlen(s_sucamt),0,&df.succamt);
            df.succamt = df.succamt/100;
            vtcp_log("[%s[%d]注意这里的金额是多少[%f]\n",__FILE__,__LINE__,df.succamt);
            if(df.succnum>0)/**如果大于0说明有成功的**/
            {
                memcpy(df.sts,DSF_QSSU,2);
            }
            else
            {
                memcpy(df.sts,DSF_QSFA,2);
            }
        }
        else if (memcmp(repstat,"00",2)==0)
        {
            statof405034(df.sts,repstat);
            memcpy(df.repstat,repstat,sizeof(df.repstat)-1);
            strncpy(df.re_content,re_content,sizeof(df.re_content)-1);
            vtcp_log("repstat=[%s],[%s][%d]",repstat,__FILE__,__LINE__);
            df.succnum = df.cash_num;
            df.succamt = df.totamt;
            vtcp_log("df.succnum=[%ld],df.cash_num=[%ld]  df.succamt=[%lf],df.totamt=[%lf]",df.succnum,df.cash_num,df.succamt,df.totamt);
        }
        else
        {
            statof405034(df.sts,repstat);
            memcpy(df.repstat,repstat,sizeof(df.repstat)-1);
            strncpy(df.re_content,re_content,sizeof(df.re_content)-1);
            vtcp_log("[%s][%d]here here",__FILE__,__LINE__);
            df.succnum =0;
            df.succamt = 0.0;
        }
    }

    g_reply_int = Lv_pldfr_Upd_Upd(df,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Upd_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    Lv_pldfr_Clo_Upd();
    /*组文件包*/
    vtcp_log("[%s][%d],Find PLDFR success!\n",__FILE__,__LINE__);
    memcpy(lv.CMTCode,"012",3);     /*通用信息业务包*/
    lv.opeType = '1';               /*表明是小额报文*/
    lv.pkgType = '1';               /*表明是PKG包*/
    memcpy(lv.CMTNo,"005",3);       /*业务要素集*/
    lv_set_tag(&lv,"0BG","40503");  /*业务类型号*/
    lv_set_tag(&lv,"52A",df.br_no);    
    lv_set_tag(&lv,"58A",df.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
    lv_get_dsf40506_orderno(&lv,df.orderno);
    vtcp_log("[%s][%d]df.orderno==[%s]",__FILE__,__LINE__,df.orderno);
    sprintf(temp,"%08d",130+117*df.cash_num);
    vtcp_log("[%s][%d]这是长度吧temp==[%s]",__FILE__,__LINE__,temp);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    lv_set_tag(&lv,"B40",temp);    /*每次只只一个业务,就是91个字节的数据*/
    vtcp_log("[%s][%d]df.wt_date==[%ld]",__FILE__,__LINE__,df.wt_date);
    sprintf(temp,"%8ld",df.wt_date);
    lv_set_tag(&lv,"72C",temp);
    vtcp_log("[%s][%d]sorderno=[%s]",__FILE__,__LINE__,sorderno);
    sprintf(temp,"%8s",sorderno);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]df.pay_br_no=[%s]",__FILE__,__LINE__,df.pay_br_no);
    sprintf(temp,"%12s",df.pay_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d],df.pay_opn_br_no=[%s]",__FILE__,__LINE__,df.pay_opn_br_no);
    sprintf(temp,"%12s",df.pay_opn_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]pay_ac_no=[%s]",__FILE__,__LINE__,df.pay_ac_no);
    sprintf(temp,"%32s",df.pay_ac_no);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]cash_qs_no=[%s]",__FILE__,__LINE__,df.cash_qs_no);
    sprintf(temp,"%12s",df.cash_qs_no);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    
    
    sprintf(cTotamt,"%.2f",df.totamt);
    del_dot(cTotamt,cTotamt,15);
    vtcp_log("[%s][%d]交易总金额=[%s]",__FILE__,__LINE__,cTotamt);
    sprintf(temp,"%15s",cTotamt);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]cTotamt=[%s]\n",__FILE__,__LINE__,cTotamt);
    
    sprintf(cSuccamt,"%.2f",df.succamt);
    del_dot(cSuccamt,cSuccamt,15);
    vtcp_log("[%s][%d]成功总金额=[%s]",__FILE__,__LINE__,cSuccamt);
    sprintf(temp,"%15s",cSuccamt);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]cSuccamt=[%s]\n",__FILE__,__LINE__,cSuccamt);

    sprintf(temp,"%08ld",df.succnum);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]df.succnum=[%ld]\n",__FILE__,__LINE__,df.succnum);
    
    sprintf(temp,"%08ld",df.cash_num);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]df.cash_num=[%ld]\n",__FILE__,__LINE__,df.cash_num);
    
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d],select success!\n",__FILE__,__LINE__);

    g_reply_int = Lv_pldfr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' ",df.packid,sorderno,df.wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Dec_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    while(1)
    {
        memset(&df2,0,sizeof(df2));
        vtcp_log("[%s][%d]准备开始fet数据df2了",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Fet_Upd(&df2,g_pub_tx.reply);
        if(g_reply_int == 100)
        {
            vtcp_log("[%s][%d]流水中数据已经取完sqlcode[%d].\n",__FILE__,__LINE__,g_reply_int);
            sprintf(errcode,"[%s][%d]流水中数据已经取完",__FILE__,__LINE__);
            break;
        }
        else if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldfr_Fet_Upd表失败",__FILE__,__LINE__);
            break;
        }        
        if ( !memcmp(df2.tx_mx_no,"00000000",8) )
        {
            vtcp_log("[%s][%d] tx_mx_no=[%s]\n",__FILE__,__LINE__,df2.tx_mx_no);
            continue;
        }
        if (memcmp(repstat,"AA",2)!=0)
        {
            memcpy(df2.repstat,repstat,sizeof(df2.repstat)-1);
            strncpy(df2.re_content,re_content,sizeof(df2.re_content)-1);
        }
        g_reply_int = Lv_pldfr_Upd_Upd(df2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldfr_Upd_Upd表失败",__FILE__,__LINE__);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]这里将要设置明细!",__FILE__,__LINE__);
        /*组明细*/
        sprintf(temp,"%8s",df2.tx_mx_no);
        lv_set_tag_multi(&lv,"72C",temp);
        vtcp_log("[%s][%d]df2.tx_mx_no=[%s]\n",__FILE__,__LINE__,df2.tx_mx_no);
        
        sprintf(temp,"%32s",df2.cash_ac_no);
        lv_set_tag_multi(&lv,"72C",temp);
        vtcp_log("[%s][%d]df2.cash_ac_no=[%s]\n",__FILE__,__LINE__,df2.cash_ac_no);
        vtcp_log("[%s][%d]df2.tx_amt=[%f]\n",__FILE__,__LINE__,df2.tx_amt);
        
        sprintf(cTx_amt,"%.2f",df2.tx_amt);
        del_dot(cTx_amt,cTx_amt,15);
        sprintf(temp,"%15s",cTx_amt);
        lv_set_tag_multi(&lv,"72C",temp);
        vtcp_log("[%s][%d]cTx_amt==[%s]",__FILE__,__LINE__,cTx_amt);
        
        sprintf(temp,"%2s",df2.repstat);
        lv_set_tag_multi(&lv,"72C",temp);
        
        sprintf(temp,"%-60s",df2.re_content);
        lv_set_tag_multi(&lv,"72C",temp);
        vtcp_log("[%s][%d]兄弟,来这里吧！！",__FILE__,__LINE__);
    }
    Lv_pldfr_Clo_Upd();
    vtcp_log("[%s][%d]准备拼包了",__FILE__,__LINE__);
    /*拼包*/
    /*包头信息*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");

    /* mod by weiz 判断一下取行号是否正确*/

    if (hvqsbrno(&lv,df.br_no)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]取行号出错",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,df.or_br_no)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]取行号出错",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    vtcp_log("[%s][%d],lvdate=[%s]",__FILE__,__LINE__,lvdate);
    lv_get_dsf40506_packid(&lv,df.packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40503");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40503回执");
    lv_pkg_batch(&lv,"72D",temp);
    vtcp_log("[%s][%d]------------------------------发送40503回执----------------------------",__FILE__,__LINE__);
    
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    /*发送*/
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],写往包登记簿错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写往包登记簿错",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],写代收付往包错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写代收付往包错",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d],genpack success!\n",__FILE__,__LINE__);
    /*发送*/
    DSFSEND(&lv);
    vtcp_log("[%s][%d],sendpack success!\n",__FILE__,__LINE__);
    lv_destroy(&lv);

    return 0;
}

/* 这个函数用于从lv_pldfr中找出交易号到lv_dqdjs找到对应的交易来得出结果 */
int  dsf_dj2df(char *s_wt_date,char *s_orderno,char *s_sucnum,char *s_sucamt)
{
    struct lv_pkgreg_c tx; /*对tx,可以很快和下面的函数进行统一  */
    struct lv_pldfr_c dsf; /*同理*/
    struct lv_pldfr_c dsf2; /*同理*/
    int iCount=0,iRc=0;
    double dSucAmt=0.00;
    char   djstr[30];
    char   cPkgno[4];
    
    memset(&dsf   , 0 , sizeof(dsf));
    memset(&dsf2  , 0 , sizeof(dsf2));
    memset(&tx    , 0 , sizeof(tx));
    memset(cPkgno , 0 , sizeof(cPkgno));
    memset(djstr  , 0 , sizeof(djstr));
    
    memcpy(cPkgno , "005" ,sizeof(cPkgno)-1);
        
    vtcp_log("[%s][%d],找原交易号为[%s]日期为[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    g_reply_int = Lv_pldfr_Dec_Sel(g_pub_tx.reply,"wt_date=%s and orderno='%s' and tx_mx_no<>'00000000'",\
                                   s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Sel表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"Lv_pldfr_Dec_Sel表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]准备循环",__FILE__,__LINE__);
    while (1)
    {
        memset(&dsf,0,sizeof(dsf));
        g_reply_int = Lv_pldfr_Fet_Sel(&dsf,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Sel表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Fet_Sel表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            break;
        } 
        memset(djstr,0,sizeof(djstr));
        sprintf(djstr,"%ld%s%s",dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        vtcp_log("[%s][%d]djstr=[%s]",__FILE__,__LINE__,djstr);
        g_reply_int = iDsf_gettx(&tx,dsf.wt_date,dsf.orderno,dsf.tx_mx_no,cPkgno);
        if(g_reply_int)
        {
            vtcp_log("找不到对应的定期贷记业务[%ld][%s][%s][%s]",dsf.wt_date,dsf.orderno,dsf.tx_mx_no,cPkgno);
            Lv_pldfr_Clo_Sel();
            return -1;
        }
        vtcp_log("[%s][%d]准备update",__FILE__,__LINE__);
        vtcp_log("[%s][%d]dsf.packid=[%s]",__FILE__,__LINE__,dsf.packid);
        vtcp_log("[%s][%d]dsf.orderno=[%s]",__FILE__,__LINE__,dsf.orderno);
        vtcp_log("[%s][%d]dsf.wt_date=[%ld]",__FILE__,__LINE__,dsf.wt_date);
        vtcp_log("[%s][%d]dsf.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.tx_mx_no);
        g_reply_int = Lv_pldfr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",dsf.packid,dsf.orderno,dsf.wt_date,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Dec_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]FET一条记录",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Fet_Upd(&dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Fet_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        else
        {
            vtcp_log("[%s][%d]准备得到金额啦!!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]tx.lv_sts=[%s]!!",__FILE__,__LINE__,tx.lv_sts);
            vtcp_log("[%s][%d]dsf2.sts=[%s]!!",__FILE__,__LINE__,dsf2.sts);
            Lv_pldfr_Debug(&dsf2);
            if (dsf_dqdjsts2dfsucc(tx.lv_sts,dsf2.sts,dsf2.repstat,dsf2.re_content)==0)
            {
                iCount++;
                dSucAmt += dsf2.tx_amt;
                vtcp_log("[%s][%d]dsf2.tx_amt==[%f]",__FILE__,__LINE__,dsf2.tx_amt);
                vtcp_log("[%s][%d]dSucAmt==[%f]",__FILE__,__LINE__,dSucAmt);
            }
            vtcp_log("[%s][%d]here here ",__FILE__,__LINE__);
        }
        vtcp_log("[%s][%d]iCount==[%d]",__FILE__,__LINE__,iCount);
        vtcp_log("[%s][%d]dSucAmt==[%f]",__FILE__,__LINE__,dSucAmt);
        vtcp_log("[%s][%d]UPD一条记录",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Upd_Upd(dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Upd_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        Lv_pldfr_Clo_Upd();

    }

    vtcp_log("[%s][%d]iCount=[%d]",__FILE__,__LINE__,iCount);
    Lv_pldfr_Clo_Sel();
    sprintf(s_sucnum,"%08d",iCount);
    
    sprintf(s_sucamt,"%.2f",dSucAmt);
    del_dot(s_sucamt,s_sucamt,15);

    vtcp_log("[%s][%d]dSucAmt=[%f]",__FILE__,__LINE__,dSucAmt);
    vtcp_log("[%s][%d]s_sucamt=[%s]",__FILE__,__LINE__,s_sucamt);
    vtcp_log("[%s][%d]s_sucamt=[%s]",__FILE__,__LINE__,s_sucamt);
    vtcp_log("[%s][%d],找原交易号为[%s]日期为[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    vtcp_log("[%s][%d],找原交易号为[%s]日期为[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    g_reply_int = sql_execute("update lv_pldfr set succamt='%f',succnum='%ld' where \
                               wt_date=%s and orderno ='%s' and tx_mx_no='00000000'",\
                               dSucAmt,iCount,s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],更新lv_pldfr表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"更新lv_pldfr表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]SUCCESS",__FILE__,__LINE__);
    return 0;
}
/*取得对应的业务*/
int iDsf_gettx(struct lv_pkgreg_c *lv_pkgreg, long day,char *orderno,char *tx_mx_no ,char *pkgno)
{
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    char  djstr[30];
    char  cAddid[9];
    struct lv_pkgreg_c sLv_pkgreg2;
    struct lv_addit_c lv_addit;
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    memset(cAddid   ,0,sizeof(cAddid));
    memset(djstr    ,0,sizeof(djstr));
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    memset(&sLv_pkgreg2     ,0,sizeof(struct lv_pkgreg_c));
    memset(&lv_addit,0,sizeof(struct lv_addit_c));
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    sprintf(djstr,"%ld%s%s",day,orderno,tx_mx_no);
    vtcp_log("[%s][%d] day==[%ld]",__FILE__,__LINE__,day);
    vtcp_log("[%s][%d]  no==[%s]",__FILE__,__LINE__,orderno);
    vtcp_log("[%s][%d]tx_mx_no==[%s]",__FILE__,__LINE__,tx_mx_no);
    
    vtcp_log("[%s][%d]djstr==[%s]",__FILE__,__LINE__,djstr);

    g_reply_int = Lv_addit_Sel(g_pub_tx.reply,&lv_addit,"tagname='dsf' and tagdata='%s'",djstr);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],查询Lv_addit表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"查询Lv_addit表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    else
    {
        memcpy(cAddid,lv_addit.addid,sizeof(cAddid)-1);
    }
    vtcp_log("[%s][%d]cAddid==[%s]",__FILE__,__LINE__,cAddid);

    g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply," addid = '%s' and pkgno = '%s' order by orderno",cAddid,pkgno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],声明lv_pkgreg表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"声明lv_pkgreg表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    while (1)
    {
        memset(&sLv_pkgreg2,0,sizeof(struct lv_pkgreg_c));

        g_reply_int = Lv_pkgreg_Fet_Sel(&sLv_pkgreg2, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],取lv_pkgreg表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"取lv_pkgreg表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]成功取得发出的交易\n",__FILE__,__LINE__);
        memcpy(lv_pkgreg,&sLv_pkgreg2,sizeof(sLv_pkgreg2));
        break;
    }
    Lv_pkgreg_Clo_Sel();
    vtcp_log("[%s][%d]sLv_pkgreg2.pkgno=[%s]",__FILE__,__LINE__,sLv_pkgreg2.pkgno);
    return 0;
}

/*这个函数说明：
把定期贷记业务的每笔小额的状态准确应答,返回0表示交易成功,-1表示失败*/
int dsf_dqdjsts2dfsucc(char *txs,char *dsfs,char *reps,char *repc)
{
    switch(*txs){
        case  STAT_INVALID      : /*无效状态*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"无效状态",8);
            return -1;
        case  STAT_WZLR          : /*往账录入*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账录入",8);
            return -1;
        case  STAT_WZCHECK1      : /*往账复核,挂帐 --无效,20051130 by lhf*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账复核,挂帐",13);
            return -1;
        case  STAT_WZCHECK2      : /*往账复核,记帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账复核,记帐",13);
            return -1;
        case  STAT_WZSEND      : /*往账发送*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账发送",8);
            return -1;
        case  STAT_WZREJECT      : /*往账拒绝*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账拒绝",8);
            return -1;
        case  STAT_WZSUCCESS  : /*往账扎差*/
            memcpy(dsfs,DSF_QSSU,2);
            memcpy(reps,"00",2);
            strncpy(repc,"往账扎差",8);
            return 0;
        case  STAT_WZINQUEUE  : /*往账排队*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账排队",8);
            return -1;
        case  STAT_WZCANCEL_WAIT:    /*往账撤销等待*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账撤销等待",12);
            return -1;
        case  STAT_WZCANCEL        :        /*往账撤销*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"往账撤销",8);
            return -1;
        case  STAT_WZBACK_WAIT  :          /*往账退回、冲正、止付等待*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账退回,冲正,止付等待",22);
            return -1;
        case  STAT_WZBACK        :         /*往账退回*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"往账退回",8);
            return -1;
        case  STAT_WZCZ            :         /*往账已冲正*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"往账已冲正",10);
            return -1;
        case  STAT_WZZF            :         /*往账已止付*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"往账已止付",10);
            return -1;
        case  STAT_WZEXCEPT        :         /*往账异常挂帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"往账异常挂帐",12);
            return -1;
        case  STAT_LZRECV1        :         /*临时接收,不记帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"临时接收",8);
            return -1;
        case  STAT_LZRECV2        :         /*正式接收,正常记帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"正式接收,正常记帐",17);
            return -1;
        case  STAT_LZNMAC_ERR   :           /*全国押错*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"全国押错",8);
            return -1;
        case  STAT_LZLMAC_ERR   :           /*地方押错*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"地方押错",8);
            return -1;
        case  STAT_LZAMAC_ERR   :           /*全国押和地方押都错*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"全国押和地方押都错",18);
            return -1;
        case  STAT_LZGZ_QS        :         /*信息不符,清算中心挂帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"信息不符,清算中心挂帐",21);
            return -1;
        case  STAT_LZGZ_BR        :         /*信息不符,支行挂帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"信息不符,支行挂帐",17);
            return -1;
        case  STAT_LZBACK_WAIT  :       /*来账退回等待*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账退回等待",12);
            return -1;
        case  STAT_LZBACK       :       /*来账退回*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账退回",8);
            return -1;
        case  STAT_YES_NOTSEND  :       /*已做同意应答且未发送*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"已做同意应答且未发送",20);
            return -1;
        case  STAT_YES_SEND     :       /*已做同意应答且已发送*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"已做同意应答且已发送",20);
            return -1;
        case  STAT_NO_NOTSEND   :       /*已做不同意应答且未发送*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"已做不同意应答但未发送",22);
            return -1;
        case  STAT_NO_SEND      :       /*已做不同意应答且已发送*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"已做不同意应答且已发送",22);
            return -1;
        case  STAT_LZCZ         :       /*来账已冲正*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账已冲正",10);
            return -1;
        case  STAT_LZZF         :       /*来账已止付*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账已止付",10);
            return -1;
        case  STAT_LZFK         :       /*来账已付款*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账已付款",10);
            return -1;
        case  STAT_LZEXCEPT     :       /*来账异常挂帐*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"来账异常挂帐",12);
            return -1;
        default:
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"未知错误,请联系管理员",21);
            return -1;
    }
    return 0;
}

/*这个函数说明：
把定期借记回执业务的每笔小额的状态准确应答,返回0表示交易成功,-1表示失败*/
int dsf_dqjjhz2dssts(char *rcpstat,char *dsfs,char *reps,char *repc)
{   
    long iCnt = 0;
    char cTmpSts[3];
    memset(cTmpSts , 0 , sizeof(cTmpSts));
    
    memcpy(cTmpSts , rcpstat , sizeof(cTmpSts)-1);
    vtcp_log("[%s][%d] cTmpSts =[%s]\n",__FILE__,__LINE__,cTmpSts);
    
    iCnt = apatoi(cTmpSts,2);
    
    switch(iCnt){
        case  0      : 
            memcpy(dsfs,DSF_QSSU,2);
            memcpy(reps,"00",2);
            strncpy(repc,"回执成功",8);
            return 0;
        case  1          :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"01",2);
            strncpy(repc,"账号不存在",8);
            return -1;
        case  2      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"02",2);
            strncpy(repc,"账号,户名不符",13);
            return -1;
        case  3      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"03",2);
            strncpy(repc,"账户余额不足支付",16);
            return -1;
        case  10      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"10",2);
            strncpy(repc,"账户密码错误",12);
            return -1;
        case  11      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"11",2);
            strncpy(repc,"账户状态错误",12);
            return -1;
        case  20  :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"20",2);
            strncpy(repc,"业务已撤销",10);
            return -1;
        case  90  :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"其他错误",8);
            return -1;
        default:
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"未知错误,请联系管理员",21);
            return -1;
    }
    return -1;
}
/*取得对应定期借记回执的业务*/
int iDsf_getdqjjhz(struct lv_pkgreg_c *lv_pkgreg, long day,char *orderno,char *tx_mx_no)
{
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    char  djstr[30];
    char  cAddid[9];
    struct lv_pkgreg_c sLv_pkgreg2;
    struct lv_addit_c lv_addit;
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    memset(cAddid   ,0,sizeof(cAddid));
    memset(djstr    ,0,sizeof(djstr));
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    memset(&sLv_pkgreg2     ,0,sizeof(struct lv_pkgreg_c));
    memset(&lv_addit,0,sizeof(struct lv_addit_c));
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    sprintf(djstr,"%ld%s%s",day,orderno,tx_mx_no);
    vtcp_log("[%s][%d] day==[%ld]",__FILE__,__LINE__,day);
    vtcp_log("[%s][%d]  no==[%s]",__FILE__,__LINE__,orderno);
    vtcp_log("[%s][%d]tx_mx_no==[%s]",__FILE__,__LINE__,tx_mx_no);
    
    vtcp_log("[%s][%d]djstr==[%s]",__FILE__,__LINE__,djstr);

    g_reply_int = Lv_addit_Sel(g_pub_tx.reply,&lv_addit,"tagname='dsf' and tagdata='%s'",djstr);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],查询Lv_addit表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"查询Lv_addit表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    else
    {
        memcpy(cAddid,lv_addit.addid,sizeof(cAddid)-1);
    }
    vtcp_log("[%s][%d]cAddid==[%s]",__FILE__,__LINE__,cAddid);

    g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply," addid = '%s' and pkgno='011' and lw_ind='2' order by orderno",cAddid);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],声明lv_pkgreg表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"声明lv_pkgreg表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    while (1)
    {
        memset(&sLv_pkgreg2,0,sizeof(struct lv_pkgreg_c));

        g_reply_int = Lv_pkgreg_Fet_Sel(&sLv_pkgreg2, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],取lv_pkgreg表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"取lv_pkgreg表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]成功取得发出的交易\n",__FILE__,__LINE__);
        memcpy(lv_pkgreg,&sLv_pkgreg2,sizeof(sLv_pkgreg2));
        break;
    }
    Lv_pkgreg_Clo_Sel();
    vtcp_log("[%s][%d]sLv_pkgreg2.pkgno=[%s]",__FILE__,__LINE__,sLv_pkgreg2.pkgno);
    return 0;
}
int lv_dsf_wbctl(TLVPACKAGE *wp_lvpack)
{
    int iRc=0;
    int recnt,index;
    TLVPACKAGE subpack;
    TLVPACKAGE return40506Pack;    /*用来返回40506包*/
    struct lv_wbctl_c lv_wbctl;
    char pcPackday[9];
    char pcPackid[9];
    char cTmporderno[9];
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    /*********数据初始化******************/
    memset(&lv_wbctl,'\0',sizeof(lv_wbctl));

    memset(pcPackday,'\0',sizeof(pcPackday));
    memset(pcPackid,'\0',sizeof(pcPackid));
    memset(cTmporderno, 0 , sizeof(cTmporderno));
    
    lv_pkg_getHeaderTag(wp_lvpack,"30E",pcPackday);
    lv_pkg_getHeaderTag(wp_lvpack,"0BD",cTmporderno);
    lv_orderno_add0(cTmporderno, sizeof(pcPackid)-1, pcPackid);
    vtcp_log("[%s][%d],pcPackday=[%s]",__FILE__,__LINE__,pcPackday);
    /******插入来包控制表************/
    iRc=lv_setwbctl_from_pack(wp_lvpack,&lv_wbctl);
    if(iRc)
    {
        vtcp_log("[%s][%d],产生往包控制表记录错[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"产生往包控制表记录错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }

    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    g_reply_int  = Lv_wbctl_Ins(lv_wbctl,g_pub_tx.reply );
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],插入往包控制表错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"[%s][%d]插入往包控制表错code=[%d]",__FL__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d],插入往包表成功\n",__FILE__,__LINE__);
    return(0);
}

static int lv_setwbctl_from_pack(TLVPACKAGE *wp_lvpack, struct lv_wbctl_c *lv_wbctl)
{
    char caValue[61];
    char cPackday[9];
    memset(cPackday,0,sizeof(cPackday));
    memset(caValue,0 , sizeof(caValue));
    memset(lv_wbctl,'\0',sizeof(struct lv_wbctl_c));    
    GETHEADTAG(wp_lvpack,"30E",cPackday);/**包委托日期**/
    lv_wbctl->pack_date = apatoi(cPackday,8);
    memset(caValue,'\0',sizeof(caValue));
    GETHEADTAG(wp_lvpack,"0BD",caValue);/**包序号**/
    lv_orderno_add0(caValue, sizeof(lv_wbctl->packid)-1, lv_wbctl->packid);
    vtcp_log("[%s][%d],pack_date=[%d]",__FILE__,__LINE__,lv_wbctl->pack_date);
    vtcp_log("[%s][%d],packid====[%s]",__FILE__,__LINE__,lv_wbctl->packid);
    GETHEADTAG(wp_lvpack,"02C",lv_wbctl->pkgno);/**包类型号**/
    GETHEADTAG(wp_lvpack,"C36",lv_wbctl->pay_qs_no);/*发起清算行*/
    GETHEADTAG(wp_lvpack,"C37",lv_wbctl->cash_qs_no);/*接收清算行*/
    /****进入系统日期****/
    lv_wbctl->in_date = apatoi(pcLvGetDay(),8);
    /***业务总笔数****/ 
    memset(caValue,'\0',sizeof(caValue));
    GETHEADTAG(wp_lvpack,"B42",caValue);
    lv_wbctl->dtlcnt=atoi(caValue);
    
    /****原包类型号*****/
    GETHEADTAG(wp_lvpack,"02D",lv_wbctl->o_pkgno);
    /****发起节点号,通用信息用****/
    GETHEADTAG(wp_lvpack,"C36",lv_wbctl->ornode);
    /****接收节点号,通用信息用****/
    GETHEADTAG(wp_lvpack,"C37",lv_wbctl->acnode);
    /****辖内分发标志****/
    GETHEADTAG(wp_lvpack,"BS7",lv_wbctl->ffflag);
    /****业务类型号***/
    GETHEADTAG(wp_lvpack,"0BG",lv_wbctl->txnum);
    /****包状态,自定义*****/
    lv_wbctl->stat[0]=PSTAT_SEND;
    /****打印标志,保留****/
    memset(lv_wbctl->brprtflag,'0',sizeof(lv_wbctl->brprtflag)-1);
    memset(lv_wbctl->qsprtflag,'0',sizeof(lv_wbctl->qsprtflag)-1);

    get_fd_data("0030",lv_wbctl->br_no);
    vtcp_log("[%s][%d]lv_wbctl->br_no=[%s]",__FILE__,__LINE__,lv_wbctl->br_no);
    return(0);
}
static int statof405034(char *stat,const char *repstat)
{
    if (memcmp(repstat,"00",2)==0)
    {
        memcpy(stat,DSF_QSSU,2);
    }
    else
    {
        memcpy(stat,DSF_QSFA,2);
    }
    return 0;
}

/******************************************************************
这个函数把发出的回执包放在一个表里,以处理回执的回执,在40503,40504,40506发送前调用　
******************************************************************/
int dsf_setsendlist(TLVPACKAGE *lv)
{
    struct lv_dsfsendlist_c dsl;
    TLVPACKAGE sublv;
    int recnt,iRc=0;
    char pcPackid[9];
    char cWt_date[9];

    memset(&dsl,0,sizeof(dsl));
    memset(cWt_date,0,sizeof(cWt_date));
    lv_create(&sublv);

    /*解包,取得包号和长度信息*/
    lv_pkg_getHeaderTag(lv,"0BD",dsl.packid);
    vtcp_log("[%s][%d]这里的包序号是[%s]",__FILE__,__LINE__,dsl.packid);
    lv->tradeStartP=0;
    recnt = lv_count_recnt(lv);
    for(;recnt>0;recnt--)
    {
        iRc = lv_fetchpkg(lv,&sublv);
        if (iRc!=0)
        {
            vtcp_log("[%s][%d],取包错[%d]\n",__FILE__,__LINE__,iRc);
            return -1;
        }
        lv_get_tag(&sublv,"0BG",dsl.tx_type);
        lv_get_tag(&sublv,"52A",dsl.or_br_no);
        lv_get_tag(&sublv,"58A",dsl.ac_br_no);
        lv_get_tag(&sublv,"30A",cWt_date);
        dsl.wt_date = apatoi(cWt_date,8);
        lv_get_tag(&sublv,"0BC",dsl.orderno);
        lv_get_tag(&sublv,"B40",dsl.msg_len);
        lv_get_tag_multi(&sublv,"72C",dsl.add_tag,0,100);
        vtcp_log("[%s][%d]dsl.add_tag==[%s]",__FILE__,__LINE__,dsl.add_tag);
        dsl.o_wt_date = apatoi(dsl.add_tag,8);
        memcpy(dsl.o_orderno,dsl.add_tag+8,8);
        
        if (memcmp(dsl.tx_type,"40503",5)==0)
        {
            memcpy(dsl.o_tx_type,"40501",5);
        } 
        else if( memcmp(dsl.tx_type,"40504",5)==0)
        {
            memcpy(dsl.o_tx_type,"40502",5);
        } 
        else if (memcmp(dsl.tx_type,"40506",5)==0)
        {
            memcpy(dsl.o_tx_type,dsl.add_tag+16,5);
        } 
        else 
        {
            /*其它包不处理*/
            vtcp_log("[%s][%d],包类型号为%s\n",__FILE__,__LINE__,dsl.tx_type);
            return -1;
        }
        
        memcpy(dsl.sts,DSF_SEND,2);
        strcpy(dsl.content,"回执已发送");

        g_reply_int  = Lv_dsfsendlist_Ins(dsl,g_pub_tx.reply ); /*是值传递!!*/
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],插入代收付往账簿错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"[%s][%d]插入代收付往账簿错sqlcode=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(-1);
        }
        lv_reset(&sublv);
    }

    lv_destroy(&sublv);
    return 0;
}

/*这个函数取得对应的清算行*/
int hvqsorbrno(TLVPACKAGE *lv,char *ibrno)
{
    struct hv_uniontab_c hut;
    int iRc=0;
    char brno[14],tempbrno[13];
    
    memset(&hut,0,sizeof(hut));
    memcpy(hut.or_br_no,ibrno,12);
    
    vtcp_log("[%s][%d]hut.or_br_no==[%s]",__FILE__,__LINE__,hut.or_br_no);
    
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&hut,"or_br_no='%s' ",hut.or_br_no);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],查询行名行号错误,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"查询行名行号错误",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    lv_pkg_batch(lv,"C37",hut.qs_no);
    vtcp_log("[%s][%d],sendco=[%s]\n",__FILE__,__LINE__,hut.sendco);
    return 0;
}
/*这个函数取得对应的清算行*/
int hvqsbrno(TLVPACKAGE *lv,char *ibrno)
{
    struct hv_uniontab_c hut;
    int iRc=0;
    char brno[14],tempbrno[13];
    
    memset(&hut,0,sizeof(hut));
    memcpy(hut.or_br_no,ibrno,12);
    
    vtcp_log("[%s][%d]or_br_no==[%s]",__FILE__,__LINE__,hut.or_br_no);
    
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&hut,"or_br_no='%s' ",hut.or_br_no);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],查询行名行号错误,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"查询行名行号错误",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d]qs_no==[%s]",__FILE__,__LINE__,hut.qs_no);
    lv_pkg_batch(lv,"C36",hut.qs_no);
    return 0;
}
int lv_get_dsf40506_packid(TLVPACKAGE *lv,char *packid)
{
    /*现不知道在什么地方取得,所以就在原来交易中前面加个40506*/
    int ipackid=0;
    ipackid = iLvGetPackid();
    sprintf(packid,"%08d",ipackid);
    vtcp_log("[%s][%d]得到packid=[%s]\n",__FILE__,__LINE__,packid);
    /*在wd_lv_wbctl中插入一条记录来查询*/
    lv_pkg_batch(lv,"0BD",packid);
    vtcp_log("[%s][%d]这里的包序号是[%s]",__FILE__,__LINE__,packid);
    return 0;
}

int lv_get_dsf40506_orderno(TLVPACKAGE *lv,char *orderno)
{
    /*现不知道在什么地方取得,所以就在原来交易中前面加个40506*/
    int i_orderno=0;
    i_orderno = dsf_iLvGetOrderno();
    sprintf(orderno,"%08d",i_orderno);
    vtcp_log("[%s][%d]得到orderno=[%s]\n",__FILE__,__LINE__,orderno);
    /*在wd_lv_wbctl中插入一条记录来查询*/
    lv_set_tag(lv,"0BC",orderno);
    
vtcp_log("[%s][%d]",__FILE__,__LINE__);
  return 0;
}
/*代收付的交易序号都从清算中心取,发包也都从清算中心发*/
int dsf_iLvGetOrderno() 
{
    char cBrno[BRNO_LEN+1];

    memset(cBrno, 0x0, sizeof(cBrno));
    memcpy(cBrno,QS_BR_NO,sizeof(BRNO_LEN)+1);
    vtcp_log("[%s][%d]cBrno=[%s]",__FILE__,__LINE__,cBrno);
    return iLvGetSeqno(cBrno, LV_ACTYPE);/**跟小额一个序号**/
}
int lv_get_dsforderno(char *s_orderno)
{
    int i_orderno=0;
    i_orderno = dsf_iLvGetOrderno();
    sprintf(s_orderno,"%08d",i_orderno);
    return 0;
}


int dsf_getstaterrcode(const char *stat,char *errcode){
    if ( memcmp(stat,"0A",2)==0 ){
        strcpy(errcode,"来账接收");
        return 0;
    }else if ( memcmp(stat,"1B",2)==0 ){
        strcpy(errcode,"来账拒绝");
        return 0;
    }else if ( memcmp(stat,"0C",2)==0 ){
        strcpy(errcode,"来账撤销");
        return 0;
    }else if ( memcmp(stat,"0D",2)==0 ){
        strcpy(errcode,"成功回执40506");
        return 0;
    }else if ( memcmp(stat,"1E",2)==0 ){
        strcpy(errcode,"失败回执40506");
        return 0;
    }else if ( memcmp(stat,"0F",2)==0 ){
        strcpy(errcode,"成功回执40503/4");
        return 0;
    }else if ( memcmp(stat,"0G",2)==0 ){
        strcpy(errcode,"失败回执40503/4");
        return 0;
    }else if ( memcmp(stat,"0H",2)==0 ){
        strcpy(errcode,"往账发送");
        return 0;
    }else if ( memcmp(stat,"1I",2)==0 ){
        strcpy(errcode,"往账拒绝");
        return 0;
    }else if ( memcmp(stat,"0J",2)==0 ){
        strcpy(errcode,"往账清算");
        return 0;
    }else if ( memcmp(stat,"1K",2)==0 ){
        strcpy(errcode,"往账失败");
        return 0;
    }else{
        strcpy(errcode,"未知状态,请联系管理员");
        return 0;
    }   
    return 0;
}

/*本函数处理发送40504回执业务*/
int lv_dsf_deal40504(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc=0 ,iSocketID=0,i=0; 
    struct lv_pldsr_c ds;
    struct lv_pldsr_c ds2;
    char lvdate[9];
    TLVPACKAGE lv;    /*40504返回包*/
    char s_orderno[9],temp[100];
    char s_sucnum[9],s_sucamt[16];
    char cTotamt[16];
    char cSuccamt[16];
    char cTx_amt[16];
    char cRe_lim_date[3];
    
    lv_load_tagifno();    
    lv_create(&lv);
    memset(lvdate      ,0, sizeof(lvdate));
    memset(s_sucnum    ,0, sizeof(s_sucnum));
    memset(s_sucamt    ,0, sizeof(s_sucamt));
    memset(cTotamt     ,0, sizeof(cTotamt));
    memset(cSuccamt    ,0, sizeof(cSuccamt));
    memset(cTx_amt     ,0, sizeof(cTx_amt));
    memset(cRe_lim_date,0, sizeof(cRe_lim_date));
    vtcp_log("[%s][%d]packid==[%s]",__FILE__,__LINE__,packid);
    vtcp_log("[%s][%d]orderno==[%s]",__FILE__,__LINE__,orderno);
    vtcp_log("[%s][%d]wt_date==[%s]",__FILE__,__LINE__,wt_date);
    vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,repstat);
    if (memcmp(repstat,"AA",2)==0)
    {
        if (dsf_jj2ds(wt_date,orderno,s_sucnum,s_sucamt)!=0)
        {
            vtcp_log("[%s][%d],代收自动回执失败\n",__FILE__,__LINE__);
            sprintf(errcode,"代收自动回执失败!");
            return -1;
        }
        vtcp_log("[%s][%d]自动回执成功笔数为[%s],成功金额为[%s]\n",__FILE__,__LINE__,s_sucnum,s_sucamt);
    }
    memcpy(lvdate,pcLvGetDay_work(),8);
    memset(&ds,0,sizeof(ds));
    memcpy(ds.packid,packid,sizeof(ds.packid)-1);
    ds.wt_date = apatoi(wt_date,8);
    memcpy(ds.orderno,orderno,sizeof(ds.orderno)-1);
    memset(ds.tx_mx_no,'0',8);    /*全0为全记录*/

    g_reply_int = Lv_pldsr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",ds.packid,ds.orderno,ds.wt_date,ds.tx_mx_no);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]dec lv_pldfr表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldsr_Fet_Upd(&ds,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Fet lv_pldfr表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    else
    {
        memset(s_orderno,0,9);
        memcpy(s_orderno,ds.orderno,8);
        vtcp_log("[%s][%d]ds.re_lim_date==[%d]",__FILE__,__LINE__,ds.re_lim_date);
        apitoa(ds.re_lim_date,sizeof(cRe_lim_date)-1,cRe_lim_date);
        if (dsf_candsret(wt_date,orderno,cRe_lim_date)==1)
        {
                vtcp_log("[%s][%d],回执期限未到,不能回执40504\n",__FILE__,__LINE__);
                sprintf(errcode,"回执期限未到,不能回执40504",__FILE__);
                return -1;
        }
        /**如果该交易已经应答过了**/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,ds.repstat);
          if (isdigit(ds.repstat[0]) || memcmp(ds.repstat,"AA",2)==0)
        {
                vtcp_log("[%s][%d]本交易己被回应,回应结果为[%s]",__FILE__,__LINE__,ds.repstat);
                sprintf(errcode,"本交易己被回应,回应结果为[%s]",ds.repstat);
                return -1;
        }
        /*如果不是回执成功状态,直接报错*/
        if (memcmp(ds.sts,DSF_SUCC,2)!=0 && memcmp(ds.sts,DSF_CONC,2)!=0 &&memcmp(ds.sts,DSF_QSFA,2)!=0)
        {
            vtcp_log("[%s][%d],业务当前状态不是成功回执状态,不能回复40504\n",__FILE__,__LINE__);
            sprintf(errcode,"[%s][%d]业务当前状态不是成功回执状态,不能回复40504[%s]",__FILE__,__LINE__,ds.sts);
            return -1;
        }
        statof405034(ds.sts,repstat);
        memcpy(ds.repstat,repstat,sizeof(ds.repstat)-1);
        strncpy(ds.re_content,re_content,sizeof(ds.re_content)-1);
        if (memcmp(repstat,"AA",2)==0)
        {
            ds.succnum = apatoi(s_sucnum,8);
            str2dbl(s_sucamt,strlen(s_sucamt),0,&ds.succamt);
            ds.succamt = ds.succamt/100;/*add by ChengGX 20100618 衡水*/
            vtcp_log("[%s[%d]注意这里的金额是多少[%f]\n",__FILE__,__LINE__,ds.succamt);/*add by ChengGX 20100618 衡水*/
            if(ds.succnum>0)/**如果大于0说明有成功的**/
            {
                memcpy(ds.sts,DSF_QSSU,2);
            }
            else
            {
                memcpy(ds.sts,DSF_QSFA,2);
            }
        }
        else if (memcmp(repstat,"00",2)==0)
        {
            statof405034(ds.sts,repstat);
            memcpy(ds.repstat,repstat,sizeof(ds.repstat)-1);
            strncpy(ds.re_content,re_content,sizeof(ds.re_content)-1);
            ds.succnum = ds.pay_num;
            ds.succamt = ds.totamt;
        }
        else
        {
            statof405034(ds.sts,repstat);
            memcpy(ds.repstat,repstat,sizeof(ds.repstat)-1);
            strncpy(ds.re_content,re_content,sizeof(ds.re_content)-1);
            ds.succnum = 0;
            ds.succamt =0.0;
        }
    }
    g_reply_int = Lv_pldsr_Upd_Upd(ds,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldsr_Upd_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    Lv_pldsr_Clo_Upd();
    /*组文件包*/
    memcpy(lv.CMTCode,"012",3);     /*通用信息业务包*/
    lv.opeType = '1';               /*表明是小额报文*/
    lv.pkgType = '1';               /*表明是PKG包*/
    memcpy(lv.CMTNo,"005",3);       /*业务要素集*/
    lv_set_tag(&lv,"0BG","40504");  /*业务类型号*/
    lv_set_tag(&lv,"52A",ds.br_no);    
    lv_set_tag(&lv,"58A",ds.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
    lv_get_dsf40506_orderno(&lv,ds.orderno);
    sprintf(temp,"%08d",130+117*ds.pay_num);
    lv_set_tag(&lv,"B40",temp);      /*每次只只一个业务,就是91个字节的数据*/
    sprintf(temp,"%8ld",ds.wt_date);
    lv_set_tag(&lv,"72C",temp);
    sprintf(temp,"%8s",s_orderno);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",ds.cash_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",ds.cash_opn_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%32s",ds.cash_ac_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",ds.pay_qs_no);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]",__FILE__,__LINE__);

    sprintf(cTotamt,"%.2f",ds.totamt);
    del_dot(cTotamt,cTotamt,15);
    sprintf(temp,"%15s",cTotamt);
    lv_set_tag_multi(&lv,"72C",temp);
    
    vtcp_log("[%s][%d]cTotamt==[%s]",__FILE__,__LINE__,cTotamt);
    
    sprintf(cSuccamt,"%.2f",ds.succamt);
    del_dot(cSuccamt,cSuccamt,15);
    sprintf(temp,"%15s",cSuccamt);
    lv_set_tag_multi(&lv,"72C",temp);
    
    vtcp_log("[%s][%d]cSuccamt=[%s]",__FILE__,__LINE__,cSuccamt);
    
    sprintf(temp,"%08ld",ds.succnum);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%08ld",ds.pay_num);
    lv_set_tag_multi(&lv,"72C",temp);

    g_reply_int = Lv_pldsr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' ",ds.packid,s_orderno,ds.wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"Lv_pldsr_Dec_Upd表失败[%s][%d]",__FILE__,__LINE__);
        return(g_reply_int);
    }
    while(1)
    {
        vtcp_log("[%s][%d],这里第%d笔交易\n",__FILE__,__LINE__,++i);
        memset(&ds2,0,sizeof(ds2));

        g_reply_int = Lv_pldsr_Fet_Upd(&ds2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            break;
        }
        if ( !memcmp(ds2.tx_mx_no,"00000000",8) )
        {
            continue;
        }
        if (memcmp(repstat,"AA",2)!=0)
        {
            memcpy(ds2.repstat,repstat,sizeof(ds2.repstat)-1);
            strncpy(ds2.re_content,re_content,sizeof(ds2.re_content)-1);
        }

        g_reply_int = Lv_pldsr_Upd_Upd(ds2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldsr_Upd_Upd表失败",__FILE__,__LINE__);
            return(g_reply_int);
        }
        
        /*组明细*/
        sprintf(temp,"%8s",ds2.tx_mx_no);
        lv_set_tag_multi(&lv,"72C",temp);
        sprintf(temp,"%32s",ds2.pay_ac_no);
        lv_set_tag_multi(&lv,"72C",temp);
        
        sprintf(cTx_amt,"%.2f",ds2.tx_amt);
        del_dot(cTx_amt,cTx_amt,15);
        sprintf(temp,"%15s",cTx_amt);
        lv_set_tag_multi(&lv,"72C",temp);
        vtcp_log("[%s][%d]cTx_amt==[%s]",__FILE__,__LINE__,cTx_amt);
        
        sprintf(temp,"%2s",ds2.repstat);
        lv_set_tag_multi(&lv,"72C",temp);
        sprintf(temp,"%-60s",ds2.re_content);
        lv_set_tag_multi(&lv,"72C",temp);

    }
    Lv_pldsr_Clo_Upd();
    vtcp_log("[%s][%d],发送40506.\n",__FILE__,__LINE__);
    /*拼包*/
    /*包头信息*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");
    if (hvqsbrno(&lv,ds.br_no)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,ds.or_br_no)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]取行号出错",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    lv_get_dsf40506_packid(&lv,ds.packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40504");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40504的回执");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    
    /*发送*/
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],写往包登记簿错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写往包登记簿错",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],写代收付往包错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写代收付往包错",__FILE__,__LINE__);
        return -1;
    }
    DSFSEND(&lv);
    lv_destroy(&lv);
    /*this only way to asolute this problem*/
    vtcp_log("[%s][%d]主函数成功返回",__FILE__,__LINE__); 
    return 0;
}

/*这个包用来发送数据包*/
static int dsf_send(TLVPACKAGE *lv)
{
    int iRc =0,iSocketID;
    vtcp_log("[%s][%d],发送代收付业务包.\n",__FILE__,__LINE__);
    /*发送*/
    iRc=lv_initPara(NULL);
    if (iRc)
    {
        vtcp_log("[%s][%d],初始化网络参数错误 return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"初始化网络参数错误[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    /* 2 处理其他状态的包 */
    iRc = lv_connect(LV_SERVER_BATCH);
    iSocketID = iRc;
    if (iSocketID<=0)
    {
        vtcp_log("[%s][%d],连接前置机错误 return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"连接前置机错误[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        lv_destroy(lv);
        lv_netClose(iSocketID);
        return(-1);
    }
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    /* 2 处理其他状态的包 */
    iRc = lv_send(iRc,lv);
    if (iRc)
    {
        vtcp_log("[%s][%d],发送数据包到前置机出错 return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"发送数据包到前置机出错[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        lv_destroy(lv);
        lv_netClose(iSocketID);
        return(-1);
    }
    lv_netClose(iSocketID);
    
    return 0;
}
static dsfnamecmp(const char *name1,const char *name2){
    char n1[61],n2[61];
    return 0;
    /*不要检查用户名,因为人行给的用户和本行维护的根本就不一样 */
    vtcp_log("[%s][%d],dsfnamecmp name1[%s],name2[%s]\n",__FILE__,__LINE__,name1,name2);
    memset(n1,0,sizeof(n1));
    memset(n2,0,sizeof(n2));
    
    memcpy(n1,name1,60);
    memcpy(n2,name2,60);
    trim(n1);
    trim(n2);
    zip_space(n1);
    zip_space(n2);
    vtcp_log("[%s][%d],dsfnamecmp n1[%s],n2[%s]\n",__FILE__,__LINE__,name1,name2);
    
    return memcmp(n1,n2,strlen(n1));
    
}

/*本函数返回40506数据包,每次只返回一个交易的回执包*/
int lv_return_40506(TLVPACKAGE *ilv,void *dsf,char *type,int iCount,int allCount,TLVPACKAGE  *lv)
{
    
    char lvdate[9];
    struct lv_pldfr_c *df;
    struct lv_pldsr_c *ds;
    struct lv_pldscx_c *dc;
    int iRc=0,iSocketID;
    char temp[100];
    char s_o_orderno[9];
    char cDs_date[9];
    char cWt_date[9];
    memset(s_o_orderno,0,9);
    memset(temp,0,sizeof(temp));
    memset(cDs_date,0,sizeof(cDs_date));
    memset(cWt_date,0,sizeof(cWt_date));
    
    lv_load_tagifno();    
    
    if (iCount==1)
    {
        /*就第一次做这个。*/
        lv_create(lv);
    }
    memset(lvdate,0,sizeof(lvdate));
    memcpy(lvdate,pcLvGetDay_work(),8);
    vtcp_log("[%s][%d],type=[%s]\n",__FILE__,__LINE__,type);
    /*组包*/
    if (!memcmp(type,"40501",5))
    {
        vtcp_log("[%s][%d],为40501交易回执40506\n",__FILE__,__LINE__);
        /*下面写入包信息*/
        df=dsf;
        memcpy(s_o_orderno,df->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*通用信息业务包*/
        lv->opeType = '1';           /*表明是小额报文*/
        lv->pkgType = '1';           /*表明是PKG包*/
        /*第一笔业务*/
        memcpy(lv->CMTNo,"005",3);   /*业务要素集*/
        /*正文*/
        lv_set_tag(lv,"0BG","40506");  /*业务类型号*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
        lv_get_dsf40506_orderno(lv,df->orderno);
        lv_set_tag(lv,"B40","00000091");    /*每次只只一个业务,就是91个字节的数据*/
        
        memset(cDs_date,0,sizeof(cDs_date));
        memset(cWt_date,0,sizeof(cWt_date));

        apitoa(df->wt_date,sizeof(cWt_date)-1,cWt_date);
        lv_set_tag(lv,"72C",cWt_date);
        lv_set_tag_multi(lv,"72C",s_o_orderno);
        lv_set_tag_multi(lv,"72C","40501");
        apitoa(df->dsf_date,sizeof(cDs_date)-1,cDs_date);
        lv_set_tag_multi(lv,"72C",cDs_date);
        lv_set_tag_multi(lv,"72C",df->repstat2);
        sprintf(temp,"%-60s",df->re_content2);
        lv_set_tag_multi(lv,"72C",temp);
        
        lv_batch_end(lv);
        
        /*包头信息*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,df->br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,df->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,df->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40501的回执");
            lv_pkg_batch(lv,"72D",temp);
            vtcp_log("[%s][%d],lvdate[%s]\n",__FILE__,__LINE__,lvdate);
            memcpy(lv->workDate,lvdate,8);
            lv_gen_pack(lv);
            lv_dsf_wbctl(lv);
        }
        vtcp_log("[%s][%d],lv->workDate= [%8s]\n",__FILE__,__LINE__,lv->workDate);    
    }
    else if (!memcmp(type,"40502",5))
    {
        vtcp_log("[%s][%d],为40502交易回执40506\n",__FILE__,__LINE__);
        /*下面写入包信息*/
        ds=dsf;
        memcpy(s_o_orderno,ds->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*通用信息业务包*/
        lv->opeType = '1';           /*表明是小额报文*/
        lv->pkgType = '1';           /*表明是PKG包*/
        /*第一笔业务*/
        memcpy(lv->CMTNo,"005",3);   /*业务要素集*/
        /*正文*/
        lv_set_tag(lv,"0BG","40506");  /*业务类型号*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
        lv_get_dsf40506_orderno(lv,ds->orderno);
        lv_set_tag(lv,"B40","00000091");    /*每次只只一个业务,就是91个字节的数据*/
        
        memset(cDs_date,0,sizeof(cDs_date));
        memset(cWt_date,0,sizeof(cWt_date));
        
        apitoa(ds->wt_date,sizeof(cWt_date)-1,cWt_date);
        lv_set_tag(lv,"72C",cWt_date);
        lv_set_tag_multi(lv,"72C",s_o_orderno);
        lv_set_tag_multi(lv,"72C","40502");
        apitoa(ds->dsf_date,sizeof(cDs_date)-1,cDs_date);
        lv_set_tag_multi(lv,"72C",cDs_date);
        
        lv_set_tag_multi(lv,"72C",ds->repstat2);
        memset(temp,0,sizeof(temp));
        sprintf(temp,"%-60s",ds->re_content2);
        vtcp_log("[%s][%d],tmplen=%d,%s\n",__FILE__,__LINE__,strlen(temp),temp);
        lv_set_tag_multi(lv,"72C",temp);
        vtcp_log("[%s][%d],tmplen=%d,%s\n",__FILE__,__LINE__,strlen(temp),temp);
        
        lv_batch_end(lv);
        
        /*包头信息*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,ds->br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,ds->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,ds->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40502的回执");
            lv_pkg_batch(lv,"72D",temp);
            /* .... */
            memcpy(lv->workDate,lvdate,8);
            lv_gen_pack(lv);
            lv_dsf_wbctl(lv);
        }
        vtcp_log("[%s][%d],lv->workDate= [%8s]\n",__FILE__,__LINE__,lv->workDate);
    }
    else if (!memcmp(type,"40505",5))
    {
        vtcp_log("[%s][%d],为40505交易回执40506\n",__FILE__,__LINE__);
        /*下面写入包信息*/
        dc=dsf;
        memcpy(s_o_orderno,dc->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*普通贷记业务包*/
        lv->opeType = '1';           /*表明是小额报文*/
        lv->pkgType = '1';           /*表明是PKG包*/
        /*第一笔业务*/
        memcpy(lv->CMTNo,"005",3);   /*业务要素集*/
        /*正文*/
        lv_set_tag(lv,"0BG","40506");  /*业务类型号*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
        lv_get_dsf40506_orderno(lv,dc->orderno);
        lv_set_tag(lv,"B40","00000091");    /*每次只只一个业务,就是91个字节的数据*/
        
        memset(cDs_date,0,sizeof(cDs_date));
        memset(cWt_date,0,sizeof(cWt_date));
        
        apitoa(dc->wt_date,sizeof(cWt_date)-1,cWt_date);
        lv_set_tag(lv,"72C",cWt_date);
        lv_set_tag_multi(lv,"72C",s_o_orderno);
        lv_set_tag_multi(lv,"72C","40505");
        apitoa(dc->dsf_date,sizeof(cDs_date)-1,cDs_date);
        lv_set_tag_multi(lv,"72C",cDs_date);
        lv_set_tag_multi(lv,"72C",dc->repstat2);
        sprintf(temp,"%-60s",dc->re_content2);
        lv_set_tag_multi(lv,"72C",temp);
        
        lv_batch_end(lv);
        
        /*包头信息*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,dc->br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,dc->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,dc->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40505的回执");
            lv_pkg_batch(lv,"72D",temp);
            /* .... */
            memcpy(lv->workDate,lvdate,8);
            lv_gen_pack(lv);
            lv_dsf_wbctl(lv);
        }
        vtcp_log("[%s][%d],lv->workDate= [%8s]\n",__FILE__,__LINE__,lv->workDate);
    }
    
    /*发送*/
    if (iCount==allCount)
    {
        vtcp_log("[%s][%d],发送40506.\n",__FILE__,__LINE__);
        
        dsf_setsendlist(lv);    
        dsf_send(lv);
        lv_destroy(lv);
    }
    return 0;
}

static int lv_movtag(TLVPACKAGE *ilv,char *tag1,TLVPACKAGE *lv,char *tag2)
{
    char buf[61]; /*只处理小数据*/ 
    memset(buf,0,sizeof(buf));
    lv_get_tag(ilv,tag1,buf);
    lv_set_tag(lv,tag2,buf);
    return 0;
}

static double getavbal(const char *isactno,const char *iname,char *s_avbal)
{
  double avbal=1.0;
  return avbal;
}


/* 这个函数用于从lv_pldsr中找出交易号到lv_dqjjs找到对应的交易来得出结果 */
int  dsf_jj2ds(char *s_wt_date,char *s_orderno,char *s_sucnum,char *s_sucamt)
{
    struct lv_pkgreg_c tx; /*对tx,可以很快和下面的函数进行统一  */
    struct lv_pldsr_c dsf; /*同理*/
    struct lv_pldsr_c dsf2; /*同理*/
    int iCount=0,iRc=0;
    double dSucAmt=0.00;
    char djstr[30];
    
    memset(&dsf,0,sizeof(dsf));
    memset(&dsf2,0,sizeof(dsf2));
    memset(&tx,0,sizeof(tx));
    vtcp_log("[%s][%d] jj2ds 函数开始了！！",__FILE__,__LINE__);
    g_reply_int = Lv_pldsr_Dec_Sel(g_pub_tx.reply,"wt_date=%s and orderno='%s' and tx_mx_no<>'00000000'",\
                                   s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Sel表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"Lv_pldsr_Dec_Sel表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    while (1)
    {
        memset(&dsf,0,sizeof(dsf));
        memset(&tx ,0,sizeof(tx));
        g_reply_int = Lv_pldsr_Fet_Sel(&dsf,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Sel表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Sel表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            break;
        }
        memset(djstr,0,sizeof(djstr));
        sprintf(djstr,"%ld%s%s",dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        g_reply_int = iDsf_getdqjjhz(&tx,dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]找不到对应的定期借记业务[%ld][%s][%s]",
                __FILE__,__LINE__,dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
            Lv_pldsr_Clo_Sel();
            return -1;
        }
        memset(&dsf2,0,sizeof(dsf2));
        vtcp_log("[%s][%d]准备Upd游标了！！",__FILE__,__LINE__);
        vtcp_log("[%s][%d]dsf.orderno==[%s]",__FILE__,__LINE__,dsf.orderno);
        vtcp_log("[%s][%d]dsf.tx_mx_no==[%s]",__FILE__,__LINE__,dsf.tx_mx_no);
        vtcp_log("[%s][%d]dsf.wt_date==[%ld]",__FILE__,__LINE__,dsf.wt_date);
        vtcp_log("[%s][%d]dsf.packid==[%s]",__FILE__,__LINE__,dsf.packid);
        g_reply_int = Lv_pldsr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",dsf.packid,dsf.orderno,dsf.wt_date,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Dec_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        g_reply_int = Lv_pldsr_Fet_Upd(&dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        else
        {
            vtcp_log("[%s][%d]检查代收业务每笔的状态!!!!!!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]tx.lv_sts==[%s],dsfstat[%s]\n",__FILE__,__LINE__,tx.lv_sts,dsf2.sts);
            if (dsf_dqjjhz2dssts(tx.rcpstat,dsf2.sts,dsf2.repstat,dsf2.re_content)==0)
            {
                vtcp_log("[%s][%d]成功的交易txstat[%s],dsfstat[%s]\n",__FILE__,__LINE__,tx.lv_sts,dsf2.sts);
                iCount++;
                dSucAmt += dsf2.tx_amt;
            }
        }
        g_reply_int = Lv_pldsr_Upd_Upd(dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Upd_Upd表失败[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        Lv_pldsr_Clo_Upd();
        vtcp_log("[%s][%d],成功笔数%d,成功金额%f\n",__FILE__,__LINE__,iCount,dSucAmt);
    }
    Lv_pldsr_Clo_Sel();
    sprintf(s_sucnum,"%08d",iCount);

    sprintf(s_sucamt,"%.2f",dSucAmt);
    del_dot(s_sucamt,s_sucamt,15);
    
    vtcp_log("[%s][%d],总成功笔数%d,总成功金额%f\n",__FILE__,__LINE__,iCount,dSucAmt);

    g_reply_int = sql_execute("update lv_pldsr set succamt='%f',succnum='%ld' where \
                               wt_date=%s and orderno ='%s' and tx_mx_no='00000000'",\
                               dSucAmt,iCount,s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d],成功笔数%d,成功金额%f\n",__FILE__,__LINE__,iCount,dSucAmt);
    return 0;
}

char * trim( char * str )
{
    char *p;

    for( p = str; isspace( *p ) && p < str + strlen( str ); p++ );
    strcpy( str, p );

    for( p = str + strlen( str ) - 1; isspace( *p ) && p > str ; p-- );
    *( p + 1 ) = '\0';

    return ( str );
}

static char* pcLvGetDay_work()
{
    struct lv_sysctl_c lv_sysctl;
    memset(&lv_sysctl,0,sizeof(lv_sysctl));
    
    char  cTxday[9];
    
    memset(cTxday,'\0',sizeof(cTxday));
    g_reply_int = Lv_sysctl_Sel(g_pub_tx.reply,&lv_sysctl,"1=1");
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],取得小额系统状态错误,sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
        strncpy(acErrMsg,"取得小额系统状态错误",40);
        set_zd_data("0130",acErrMsg);
        return(NULL);
    }
    sprintf(cTxday,"%8ld",lv_sysctl.tx_date);
    vtcp_log("[%s][%d],cTxday=[%s]",__FILE__,__LINE__,cTxday);
    return (cTxday);
}
int lv_return_40506_all(char *packid,char *wt_date,char *txtype)
{
    TLVPACKAGE lv;
    lv_create(&lv);
    
    /*找相应的库表*/
    lv_destroy(&lv);
    
    return 0;
}

static int lv_movPtag(TLVPACKAGE *ilv,char *tag1,TLVPACKAGE *lv,char *tag2)
{
    char buf[61]; /*只处理小数据*/ 
    memset(buf,0,sizeof(buf));
    lv_pkg_getHeaderTag(ilv,tag1,buf);
    vtcp_log("[%s][%d],buf=[%s]\n",__FILE__,__LINE__,buf);
    lv_pkg_batch(lv,tag2,buf);
    return 0;
}
int lv_get_dsf40507_packid(TLVPACKAGE *lv,char *packid)
{
    int ipackid=0;
    ipackid = iLvGetPackid();
    sprintf(packid,"%08d",ipackid);
    vtcp_log("[%s][%d]得到packid=[%s]\n",__FILE__,__LINE__,packid);
    /*在wd_lv_wbctl中插入一条记录来查询*/
    return 0;
}

/*本函数处理40507业务*/
int lv_dsf_deal40507(struct lv_pldsfqugx_c * lv_pldsfqugx)
{
    TLVPACKAGE  lv;
    char lvdate[9];
    int iRc=0;
    char temp[100];
    struct lv_dsfsendlist_c lv_dsfsendlist;
    
    /*先发送数据包*/
    lv_load_tagifno();    
    lv_create(&lv);
    memset(lvdate,0,sizeof(lvdate));
    memset(&lv_dsfsendlist,0,sizeof(lv_dsfsendlist));
    memcpy(lvdate,pcLvGetDay_work(),8);
    
    memcpy(lv_pldsfqugx->pkgno,"012" , sizeof(lv_pldsfqugx->pkgno)-1);
    lv_pldsfqugx->wt_date = apatoi(lvdate,8);

    lv_get_dsf40507_packid(&lv,lv_pldsfqugx->packid);
    vtcp_log("[%s][%d]lv_pldsfqugx->packid==[%s]",__FILE__,__LINE__,lv_pldsfqugx->packid);
    lv_get_dsforderno(lv_pldsfqugx->orderno);
    vtcp_log("[%s][%d]lv_pldsfqugx->orderno==[%s]",__FILE__,__LINE__,lv_pldsfqugx->orderno);

    get_fd_data("0030",lv_pldsfqugx->br_no);
    vtcp_log("[%s][%d]lv_pldsfqugx->br_no==[%s]",__FILE__,__LINE__,lv_pldsfqugx->br_no);
    hvbrno(lv_pldsfqugx->br_no);
    vtcp_log("[%s][%d]lv_pldsfqugx->br_no==[%s]",__FILE__,__LINE__,lv_pldsfqugx->br_no);
    vtcp_log("[%s][%d],这里是进来的数据\n",__FILE__,__LINE__);

    /*组文件包*/
    memcpy(lv.CMTCode,"012",3);     /*通用信息业务包*/
    lv.opeType = '1';               /*表明是小额报文*/
    lv.pkgType = '1';               /*表明是PKG包*/
    memcpy(lv.CMTNo,"005",3);       /*业务要素集*/
    lv_set_tag(&lv,"0BG","40507");  /*业务类型号*/
    
    
    lv_set_tag(&lv,"52A",lv_pldsfqugx->br_no);    
    memcpy(lv_pldsfqugx->or_br_no,lv_pldsfqugx->br_no,sizeof(lv_pldsfqugx->or_br_no)-1);
    lv_set_tag(&lv,"58A",LV_DSFZX);
    lv_set_tag(&lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
    lv_set_tag(&lv,"0BC",lv_pldsfqugx->orderno);
    sprintf(temp,"%08d",310);
    lv_set_tag(&lv,"B40",temp);    /*每次只只一个业务,就是318个字节的数据*/
    /*sprintf(temp,"%8s",lv_pldsfqugx->wt_date);
    lv_set_tag(&lv,"72C",temp); del by weiz ,应该没有这个东东*/
    vtcp_log("[%s][%d]开始添加附加数据域\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]lv_pldsfqugx->xyno=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->xyno);
    sprintf(temp,"%-60s",lv_pldsfqugx->xyno);
    lv_set_tag(&lv,"72C",temp);
    sprintf(temp,"%1s",lv_pldsfqugx->xy_ind);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]合同类型=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->xy_ind);
    sprintf(temp,"%1s",lv_pldsfqugx->ch_ind);
    vtcp_log("[%s][%d]变更方式=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->ch_ind);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_br_no);
    vtcp_log("[%s][%d]付款行号=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->pay_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_qs_no);
    vtcp_log("[%s][%d]付款清算行=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->pay_qs_no);
    lv_set_tag_multi(&lv,"72C",temp); 
    sprintf(temp,"%12s",lv_pldsfqugx->pay_opn_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%32s",lv_pldsfqugx->pay_ac_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->pay_name);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->pay_addr);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->content);
    lv_set_tag_multi(&lv,"72C",temp);
    
    vtcp_log("[%s][%d]---------------------发送40507-------------------------\n",__FILE__,__LINE__);
    /*拼包*/
    /*包头信息*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");
    
    vtcp_log("[%s][%d]lv_pldsfqugx->br_no==[%s]",__FILE__,__LINE__,lv_pldsfqugx->br_no);
    
    if (hvqsbrno(&lv,lv_pldsfqugx->br_no)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,LV_DSFZX)==-1)
    {
        vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    
    lv_pkg_batch(&lv,"0BD",lv_pldsfqugx->packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40507");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40507报文");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    /*在wd_lv_wbctl中插入一条记录来查询*/

    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],写往包登记簿错\n",__FILE__,__LINE__);
        return -1;
    }
    /*发送*/
    lv.packID=atoi(lv_pldsfqugx->packid);
    DSFSEND(&lv);
    lv_destroy(&lv);
    vtcp_log("[%s][%d],把以后数据插入数据库\n",__FILE__,__LINE__);
    memcpy(lv_pldsfqugx->sts,DSF_SEND,2);

    /*插入数据库*/
    vtcp_log("[%s][%d]准备插入协议变更登记薄",__FILE__,__LINE__);
    
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pkgno=[%s]        \n",__FILE__,__LINE__,lv_pldsfqugx->pkgno);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.orderno=[%s]      \n",__FILE__,__LINE__,lv_pldsfqugx->orderno);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.wt_date=[%ld]     \n",__FILE__,__LINE__,lv_pldsfqugx->wt_date);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.br_no=[%s]        \n",__FILE__,__LINE__,lv_pldsfqugx->br_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.or_br_no=[%s]     \n",__FILE__,__LINE__,lv_pldsfqugx->or_br_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.packid=[%s]       \n",__FILE__,__LINE__,lv_pldsfqugx->packid);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.xyno=[%s]         \n",__FILE__,__LINE__,lv_pldsfqugx->xyno);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.xy_ind=[%s]       \n",__FILE__,__LINE__,lv_pldsfqugx->xy_ind);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.ch_ind=[%s]       \n",__FILE__,__LINE__,lv_pldsfqugx->ch_ind);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_br_no=[%s]    \n",__FILE__,__LINE__,lv_pldsfqugx->pay_br_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_qs_no=[%s]    \n",__FILE__,__LINE__,lv_pldsfqugx->pay_qs_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_opn_br_no=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->pay_opn_br_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_ac_no=[%s]    \n",__FILE__,__LINE__,lv_pldsfqugx->pay_ac_no);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_name=[%s]     \n",__FILE__,__LINE__,lv_pldsfqugx->pay_name);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.pay_addr=[%s]     \n",__FILE__,__LINE__,lv_pldsfqugx->pay_addr);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.content=[%s]      \n",__FILE__,__LINE__,lv_pldsfqugx->content);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.sts=[%s]          \n",__FILE__,__LINE__,lv_pldsfqugx->sts);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.beg_sts=[%s]      \n",__FILE__,__LINE__,lv_pldsfqugx->beg_sts);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.operlevel=[%s]    \n",__FILE__,__LINE__,lv_pldsfqugx->operlevel);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.re_dsf_date=[%ld] \n",__FILE__,__LINE__,lv_pldsfqugx->re_dsf_date);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.repstat=[%s]      \n",__FILE__,__LINE__,lv_pldsfqugx->repstat);
    vtcp_log("[%s][%d]  lv_pldsfqugx_c.re_content2=[%s]  \n",__FILE__,__LINE__,lv_pldsfqugx->re_content2);

    g_reply_int  = Lv_pldsfqugx_Ins(*lv_pldsfqugx ,g_pub_tx.reply );
    if(g_reply_int)
    {
         vtcp_log("[%s][%d],插入签约关系变更通知登记簿错sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]插入签约关系变更通知登记簿错",__FL__);
         set_zd_data(DC_GET_MSG,acErrMsg);
         return(-1);
    }
    memcpy(lv_dsfsendlist.packid ,lv_pldsfqugx->packid,sizeof(lv_pldsfqugx->packid));
    memcpy(lv_dsfsendlist.orderno,lv_pldsfqugx->orderno,sizeof(lv_pldsfqugx->orderno));
    memcpy(lv_dsfsendlist.or_br_no ,lv_pldsfqugx->or_br_no,sizeof(lv_pldsfqugx->or_br_no));
    lv_dsfsendlist.wt_date = lv_pldsfqugx->wt_date;

    memcpy(lv_dsfsendlist.o_tx_type,"40507",5); 
    memcpy(lv_dsfsendlist.o_orderno,lv_pldsfqugx->orderno,sizeof(lv_pldsfqugx->orderno));
    lv_dsfsendlist.o_wt_date =  lv_pldsfqugx->wt_date;;    

    memcpy(lv_dsfsendlist.tx_type ,"40507",5);
    memcpy(lv_dsfsendlist.sts,DSF_SEND,2);
    memcpy(lv_dsfsendlist.content,lv_pldsfqugx->content,sizeof(lv_pldsfqugx->content));
    g_reply_int  = Lv_dsfsendlist_Ins(lv_dsfsendlist,g_pub_tx.reply ); /*是值传递!!*/
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],插入代收付往账簿错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"[%s][%d]插入代收付往账簿错sqlcode=[%d]",__FL__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    return 0;
}

/*本函数处理发送40505回执业务*/
int lv_dsf_deal40505_return(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc =0,iSocketID; 
    struct lv_pldscx_c cx;
    char lvdate[9];
    char cRe_date[9];
    TLVPACKAGE lv;    /*40503返回包*/
    char sorderno[9],temp[100];
    
    memset(sorderno,0,sizeof(sorderno));
    memcpy(sorderno,orderno,8);
    
    lv_load_tagifno();    
    lv_create(&lv);
    memset(lvdate,0,sizeof(lvdate));
    memset(cRe_date,0,sizeof(cRe_date));
    memcpy(lvdate,pcLvGetDay_work(),8);
    memset(&cx,0,sizeof(cx));
    memset(&cx,0,sizeof(cx));
    memcpy(cx.packid,packid,sizeof(cx.packid)-1);
    vtcp_log("[%s][%d] packid=[%s]\n",__FILE__,__LINE__,cx.packid);
    vtcp_log("[%s][%d] wt_date=[%s]\n",__FILE__,__LINE__,wt_date);
    cx.wt_date = apatoi(wt_date,8);
    vtcp_log("[%s][%d] packid=[%s][%ld]\n",__FILE__,__LINE__,cx.packid,cx.wt_date);
    memcpy(cx.orderno,orderno,sizeof(cx.orderno)-1);
    vtcp_log("[%s][%d] packid=[%s][%ld][%s]\n",__FILE__,__LINE__,cx.packid,cx.wt_date,cx.orderno);

    g_reply_int = Lv_pldscx_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' ",cx.packid,cx.orderno,cx.wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldscx_Dec_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Dec_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldscx_Fet_Upd(&cx,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldscx_Fet_Upd表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Fet_Upd表失败",__FILE__,__LINE__);
        return(g_reply_int);
    }
    else
    {
        /*如果已经回应,直接报错*/
        if (memcmp(cx.sts,DSF_REJE,2)==0 || memcmp(cx.sts,DSF_SUCC,2)==0 || memcmp(cx.sts,DSF_FAIL,2)==0)
        {
            vtcp_log("[%s][%d]本交易己被回应,回应结果为[%s]",__FILE__,__LINE__,cx.repstat2);
            sprintf(errcode,"[%s][%d]本交易己被回应,回应结果为[%s]",__FILE__,__LINE__,cx.repstat2);
            return -1;
        }
        if (memcmp(repstat,"00",2)==0)
        {
            memcpy(cx.sts,DSF_SUCC,2);
        }
        else
        {
            memcpy(cx.sts,DSF_FAIL,2);
        }
        memcpy(cx.repstat2,repstat,sizeof(cx.repstat2)-1);
        strncpy(cx.re_content2,re_content,sizeof(cx.re_content2)-1);
        vtcp_log("[%s][%d]here here !!",__FILE__,__LINE__);
    }
    
    g_reply_int = Lv_pldscx_Upd_Upd(cx, g_pub_tx.reply);
    if (g_reply_int)
    {
        vtcp_log("%s,%Upd Lv_pldscx_Upd_Upd表失败[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Upd_Upd表失败",__FILE__,__LINE__);
        return (g_reply_int);
    }
    Lv_pldscx_Clo_Upd();
    /*组文件包*/
    vtcp_log("[%s][%d],Find PLDSCX success!\n",__FILE__,__LINE__);
    memcpy(lv.CMTCode,"012",3);     /*普通贷记业务包*/
    lv.opeType = '1';               /*表明是小额报文*/
    lv.pkgType = '1';               /*表明是PKG包   */
    memcpy(lv.CMTNo,"005",3);       /*业务要素集    */
    lv_set_tag(&lv,"0BG","40506");  /*业务类型号    */
    lv_set_tag(&lv,"52A",cx.br_no);    
    lv_set_tag(&lv,"58A",cx.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
    lv_get_dsf40506_orderno(&lv,cx.orderno);
    sprintf(temp,"%08d",91);
    lv_set_tag(&lv,"B40",temp);    /*每次只只一个业务,就是91个字节的数据*/
    sprintf(temp,"%8d",cx.wt_date);
    lv_set_tag(&lv,"72C",temp);
    sprintf(temp,"%8s",sorderno);
    lv_set_tag_multi(&lv,"72C",temp);
    lv_set_tag_multi(&lv,"72C","40505");
    
    apitoa(cx.dsf_date,sizeof(cRe_date)-1,cRe_date);
    lv_set_tag_multi(&lv,"72C",cRe_date);
    
    lv_set_tag_multi(&lv,"72C",cx.repstat2);
    sprintf(temp,"%-60s",cx.re_content2);
    lv_set_tag_multi(&lv,"72C",temp);
    
    vtcp_log("[%s][%d]---------------------------发送40506-------------------------------\n",__FILE__,__LINE__);
    /*拼包*/
    /*包头信息*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");
    /*lv_pkg_batch(&lv,"C36",cx.br_no);*/
    hvqsbrno(&lv,cx.br_no);
    hvqsorbrno(&lv,cx.or_br_no);
    lv_pkg_batch(&lv,"30E",lvdate);
    lv_get_dsf40506_packid(&lv,cx.packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40506");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40505回执");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],写往包登记簿错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写往包登记簿错",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],写代收付往包错\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]写代收付往包错",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d],genpack success!\n",__FILE__,__LINE__);
    /*发送*/
    DSFSEND(&lv);
    vtcp_log("[%s][%d],sendpack success!\n",__FILE__,__LINE__);
    lv_destroy(&lv);
    
    return 0;
}

/*
说明：
根据传入的机构号得到支付系统行号
*/
int hvbrno(char *ibrno)
{
    struct hv_orno_brno_c hv_orbr_no;
    int iRc=0;
    char brno[14],tempbrno[13];
    
    memset(brno,0,sizeof(brno));
    memcpy(brno,ibrno,5);
    trim(brno);
    
    vtcp_log("[%s][%d],brno=[%s]\n",__FILE__,__LINE__,brno);
    if (isdigit(brno[5]))/*判断第5位是否为数字*/
    {
        vtcp_log("[%s][%d],isdigit=[%d]%c\n",__FILE__,__LINE__,isdigit(brno[4]),brno[4]);
        return 0;
    }
    memcpy(hv_orbr_no.br_no,brno,5);

    iRc=Hv_orno_brno_Sel(g_pub_tx.reply,&hv_orbr_no,"br_no='%s' ",hv_orbr_no.br_no);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],查询行名行号错误,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"查询行名行号错误",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    memcpy(ibrno,hv_orbr_no.or_br_no,12);
    return 0;
}
/*检查是否可以回执对应的代付业务已经可做40503回执了
可以做返回0,不可以返回1*/
int dsf_candfret(char *iwt_date)
{
    int days=0;
    int ithdays=0;
    int todate=0;
    int iwtday=0;
    int iRc=0;
    struct lv_param_c lv_param;
    memset(&lv_param,0,sizeof(lv_param));
    
    
    /*取小额当前日期*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(iwt_date,8);
    vtcp_log("[%s][%d]iwtday=[%ld]todate=[%ld]",__FILE__,__LINE__,iwtday,todate);
    /*取小额当前日期和委托日期的差实天数*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*取退汇规定时间*/

    iRc = Lv_param_Sel(g_pub_tx.reply,&lv_param,"par_code='THDAYS' ");
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],请查询lv_param表的THDAYS值,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"请查询lv_param表的THDAYS值",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    zip_space(lv_param.par_value);
    ithdays = apatoi(lv_param.par_value,8);
    /*比较差实天数和退汇规定时间,返回*/
    if (days>ithdays)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*检查是否可以回执对应的代收业务已经可做40504回执了*/
int dsf_candsret(char * cWt_date,char *cOrderno,char *cReday)
{
    struct lv_pldsr_c ds;
    struct lv_pkgreg_c tx;
    int days=0;
    int ithdays=0;
    int todate=0;
    int iwtday=0;
    char cPkgno[4];
    
    memset(cPkgno , 0 , sizeof(cPkgno));
    memcpy(cPkgno , "006" ,sizeof(cPkgno)-1);
    
    return 0;/**TODO for TEST**/
    
    
    
    /*根据对应的小额借记业务状态来看是否可回(状态为扎差、拒绝、止付),不能继续*/
    memset(&ds,0,sizeof(ds));
    memset(&tx,0,sizeof(tx));

    Lv_pldsr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",cOrderno,cWt_date);
    while(1)
    {
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel( &ds,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]数据已经取完.\n",__FILE__,__LINE__);
            break;
        }
        memset(&tx,0,sizeof(tx));
        iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
        if (tx.lv_sts[0]==STAT_WZREJECT || 
            tx.lv_sts[0]==STAT_WZSUCCESS ||
            tx.lv_sts[0]==STAT_WZZF)
        {
            return 0;
        }
        else
        {
            break;
        }    
    }
    Lv_pldsr_Clo_Sel();    
    /*取小额当前日期*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(cWt_date,8);
    /*取小额当前日期和委托日期的差实天数*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*比较差实天数和回执规定时间,如果返回*/
    if (days>atoi(cReday))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*检查是否可以回执对应的代收业务已经超期了,-1为交易未完成,
但已经超期,0为交易已完成,1为交易未完成,未到期*/
int dsf_candsret2(char *cWt_date,char *cOrderno,char *cReday)
{
    struct lv_pldsr_c ds;
    struct lv_pkgreg_c tx;
    int days=0;
    int ithdays=0;
    int todate=0;
    int iwtday=0;
    char cPkgno[4];
    
    memset(cPkgno , 0 , sizeof(cPkgno));
    memcpy(cPkgno , "006" ,sizeof(cPkgno)-1);
    
    /*根据对应的小额借记业务状态来看是否可回(状态为扎差、拒绝、止付),不能继续*/
    memset(&ds,0,sizeof(ds));
    memset(&tx,0,sizeof(tx));

    Lv_pldsr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",cOrderno,cWt_date);
    while(1)
    {
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel( &ds,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]数据已经取完.\n",__FILE__,__LINE__);
            break;
        } 
        memset(&tx,0,sizeof(tx));
        iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
        if (tx.lv_sts[0]==STAT_WZREJECT || 
            tx.lv_sts[0]==STAT_WZSUCCESS ||
            tx.lv_sts[0]==STAT_WZZF)
        {
            return 0;
        }
        else
        {
            break;
        }    
    }
    Lv_pldsr_Clo_Sel();  
    /*取小额当前日期*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(cWt_date,8);
    /*取小额当前日期和委托日期的差实天数*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*比较差实天数和回执规定时间,如果返回*/
    if (days>atoi(cReday))
    {
        return -1;
    }    
    else
    {
        return 1;
    }    
}

/*解释交易状态*/
int dsf_txstat(char stat,char *str2)
{
    char str[30];
    vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
    switch(stat){
        case STAT_INVALID:/*无效状态*/
            strcpy(str,"无效状态");
            break;
        case STAT_WZLR:/*往账录入*/
            strcpy(str,"往账录入");
            break;
        case STAT_WZCHECK1     :/*往账复核,挂帐 --无效,20051130 by lhf*/
            strcpy(str,"往账复核,挂帐");
            break;
        case STAT_WZCHECK2     :  /*往账复核,记帐*/
            strcpy(str,"往账复核,记帐");
            break;
        case STAT_WZSEND:  /*往账发送*/
            strcpy(str,"往账发送");
            break;
        case STAT_WZREJECT     :  /*往账拒绝*/
            strcpy(str,"往账拒绝");
            break;
        case STAT_WZSUCCESS    :  /*往账扎差*/
            strcpy(str,"往账扎差");
            break;
        case STAT_WZINQUEUE    :  /*往账排队*/
            strcpy(str,"往账排队");
            break;
        case STAT_WZCANCEL_WAIT:  /*往账撤销等待*/
            strcpy(str,"往账撤销等待");
            break;
        case STAT_WZCANCEL     :  /*往账撤销*/
            strcpy(str,"往账撤销");
            break;
        case STAT_WZBACK_WAIT  :  /*往账退回止付等待*/
            strcpy(str,"往账退回止付等待");
            break;
        case STAT_WZBACK:  /*往账退回*/
            strcpy(str,"往账退回");
            break;
        case STAT_WZCZ         :  /*往账已冲正*/
            strcpy(str,"往账已冲正");
            break;
        case STAT_WZZF         :  /*往账已止付*/
            strcpy(str,"往账已止付");
            break;
        case STAT_WZEXCEPT     :  /*往账异常挂帐*/
            strcpy(str,"往账异常挂帐");
            break;
        case STAT_LZRECV1      :  /*临时接收,不记帐*/
            strcpy(str,"临时接收,不记帐");
            break;
        case STAT_LZRECV2      :  /*正式接收,正常记帐*/
            strcpy(str,"正式接收记帐");
            break;
        case STAT_LZBACK_WAIT  :  /*来账退回等待*/
            strcpy(str,"来账退回等待");
            break;
        case STAT_LZBACK       :  /*来账退回*/
            strcpy(str,"来账退回");
            break;
        case STAT_YES_NOTSEND  :  /*已做同意应答且未发送*/
            strcpy(str,"已做同意应答但未发送");
            break;
        case STAT_YES_SEND     :  /*已做同意应答且已发送*/
            strcpy(str,"已做同意应答且已发送");
            break;
        case STAT_NO_NOTSEND   :  /*已做不同意应答且未发送*/
            strcpy(str,"已做不同意应答但未发送");
            break;
        case STAT_NO_SEND      :  /*已做不同意应答且已发送*/
            strcpy(str,"已做不同意应答且已发送");
            break;
        case STAT_LZCZ         :  /*来账已冲正*/
            strcpy(str,"来账已冲正");
            break;
        case STAT_LZZF         :  /*来账已止付*/
            strcpy(str,"来账已止付");
            break;
        case STAT_LZFK         :  /*来账已付款*/
            strcpy(str,"来账已付款");
            break;
        case STAT_LZNMAC_ERR   :  /*全国押错*/
            strcpy(str,"全国押错");
            break;
        case STAT_LZLMAC_ERR   :  /*地方押错*/
            strcpy(str,"地方押错");
            break;
        case STAT_LZAMAC_ERR   :  /*全国押和地方押都错*/
            strcpy(str,"全国押和地方押都错");
            break;
        case STAT_LZGZ_QS      :  /*清算中心挂帐*/
            strcpy(str,"清算中心挂帐");
            break;
        case STAT_LZEXCEPT     :  /*来账异常挂帐*/
            strcpy(str,"来账异常挂帐");
            break;
        default:
            break;
    }
    strcat(str2,str);
    return 0;
}

/*是撤销交易把对应的定期借记业务序号写在文件的位置上*/
int dsf_getjjorderno(char *wt_day,char *pack_id,char *order_no,char *outarr)
{
    struct lv_pldscx_c sc;
    struct lv_pldsr_c ds;
    struct lv_pkgreg_c tx;
    char firstOrderno[9];
    char lastOrderno[9];
    char cPkgno[4];
    int iRc=0,i=0;
    
    memset(&sc,0,sizeof(sc));
    memset(cPkgno , 0 , sizeof(cPkgno));
    memset(firstOrderno,0,sizeof(firstOrderno));
    memset(lastOrderno,0,sizeof(lastOrderno));

    sc.wt_date = apatoi(wt_day,8);
    memcpy(cPkgno , "006" , sizeof(cPkgno));
    memcpy(sc.packid,pack_id,sizeof(sc.packid)-1);
    memcpy(sc.orderno,order_no,sizeof(sc.orderno)-1);

    g_reply_int = Lv_pldscx_Sel(g_pub_tx.reply,&sc,"packid='%s' and orderno='%s' and wt_date='%ld'",\
                                                    sc.packid,sc.orderno,sc.wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],查询Lv_pldscx表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"查询Lv_pldscx表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]准备查lv_pldsr记录啦！",__FILE__,__LINE__);
    vtcp_log("[%s][%d]orderno=[%s] wt_date=[%ld]",__FILE__,__LINE__,sc.o_orderno,sc.o_wt_date);
    g_reply_int = Lv_pldsr_Dec_Sel(g_pub_tx.reply," orderno= '%s' and wt_date='%ld' and tx_mx_no<>'00000000' ",\
                                                   sc.o_orderno,sc.o_wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],声明lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"声明lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    } 
    while(1)
    {
        vtcp_log("[%s][%d],第一条代收业务\n",__FILE__,__LINE__);
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel(&ds, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],数据已经取完[%d]",__FILE__,__LINE__, g_reply_int);
            break;
        } 
        memset(&tx,0,sizeof(tx));
        g_reply_int = iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],数据已经取完[%d]",__FILE__,__LINE__, g_reply_int);
            break;
        }
        if (i==0)
        { /*第一次*/
            memcpy(firstOrderno,tx.orderno,sizeof(firstOrderno)-1);
            memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            vtcp_log("[%s][%d]lastOrderno==[%s]",__FILE__,__LINE__,lastOrderno);
        } 
        else 
        {
            memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            vtcp_log("[%s][%d]lastOrderno==[%s]",__FILE__,__LINE__,lastOrderno);
        }
        i++;
    }
    Lv_pldsr_Clo_Sel();
    vtcp_log("[%s][%d]===strlen===[%d]===",__FILE__,__LINE__,strlen(firstOrderno));
    if (strlen(firstOrderno)>1)
    {
        char cTmpArr[23];
        char cTmpbuf[31];
        memset(cTmpbuf,0,sizeof(cTmpbuf));
        memset(cTmpArr,0,sizeof(cTmpArr));
        vtcp_log("[%s][%d] strlen(firstOrderno)=[%d]",__FILE__,__LINE__,strlen(firstOrderno));
        vtcp_log("[%s][%d] strlen(lastOrderno)=[%d]",__FILE__,__LINE__,strlen(lastOrderno));
        sprintf(cTmpbuf,"定期借记序号%s到%s",firstOrderno,lastOrderno);
        vtcp_log("[%s][%d] cTmpbuf==[%s]",__FILE__,__LINE__,cTmpbuf);
        vtcp_log("[%s][%d] here2=tx.lv_sts[0]==[%c]",__FILE__,__LINE__,tx.lv_sts[0]);
        dsf_txstat(tx.lv_sts[0],cTmpArr);
        zip_space(cTmpArr);
        vtcp_log("[%s][%d] here3= cTmpbuf=[%s]",__FILE__,__LINE__,cTmpbuf);
        vtcp_log("[%s][%d] here3= cTmpArr=[%s]",__FILE__,__LINE__,cTmpArr);
        cTmpArr[22]='\0';
        vtcp_log("[%s][%d] cTmpArr=[%-22s]",__FILE__,__LINE__,cTmpArr);
        vtcp_log("[%s][%d] sc.o_orderno=[%s]",__FILE__,__LINE__,sc.o_orderno);
        vtcp_log("[%s][%d] sc.o_wt_date=[%ld]",__FILE__,__LINE__,sc.o_wt_date);
        
        sprintf(outarr,"%s%s代收日期%ld,序号%s",cTmpbuf,cTmpArr,sc.o_wt_date,sc.o_orderno);
        vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
    }
    else
    {
        vtcp_log("[%s][%d] here4=",__FILE__,__LINE__);
        sprintf(outarr,"未做定期借记交易,原交易日期[%ld]序号[%s]",sc.o_wt_date,sc.o_orderno);
    } 
    vtcp_log("[%s][%d]===outarr===[%s]",__FILE__,__LINE__,outarr);
    return 0;
}

/*找出代收对应的借记交易*/
int dsf_getdsjj(char *wt_day,char *pack_id,char *order_no,char *tx_mx_no,char *outarr)
{
    struct lv_pldsr_c ds;
    struct lv_pkgreg_c tx;
    char firstOrderno[9],lastOrderno[9];
    char cTx_mx_no[9];
    char cPkgno[4];
    char cBuff[14];
    int iRc=0,i=0;
    struct lv_wbctl_c gLv_wbctl;
    
    memset(firstOrderno,0,sizeof(firstOrderno));
    memset(lastOrderno,0,sizeof(lastOrderno));
    memset(cTx_mx_no  ,0,sizeof(cTx_mx_no));
    memset(cBuff      ,0,sizeof(cBuff));
    memset(cPkgno     ,0,sizeof(cPkgno));
    
    memcpy(cPkgno , "006" , sizeof(cPkgno)-1);
    sprintf(cTx_mx_no,"%08s",tx_mx_no);
    vtcp_log("[%s][%d]tx_mx_no=[%s]",__FILE__,__LINE__,tx_mx_no);
    vtcp_log("[%s][%d]cTx_mx_no=[%s]",__FILE__,__LINE__,cTx_mx_no);
    if(memcmp(cTx_mx_no, "00000000", 8)==0)
    {
        Lv_pldsr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",order_no,wt_day);
        while(1)
        {
            memset(&ds,0,sizeof(ds));
            vtcp_log("[%s][%d]tx_mx_no=[%s]",__FILE__,__LINE__,tx_mx_no);
            g_reply_int = Lv_pldsr_Fet_Sel( &ds,g_pub_tx.reply );
            if (g_reply_int)
            {
                vtcp_log("[%s][%d]数据已经取完.\n",__FILE__,__LINE__);
                break;
            }
            memset(&tx,0,sizeof(tx));
            vtcp_log("[%s][%d]wt_date=[%ld]",__FILE__,__LINE__,ds.wt_date);
            g_reply_int = iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],数据已经取完[%d]",__FILE__,__LINE__, g_reply_int);
                break;
            }
            if (i==0)
            { /*第一次*/
                memcpy(firstOrderno,tx.orderno,sizeof(firstOrderno)-1);
                memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            }
            else
            {
                memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            }
            vtcp_log("[%s][%d]xyy rcpstat=[%s] strlen=[%d]",__FILE__,__LINE__,tx.rcpstat,strlen(tx.rcpstat));
            
            i++;
        }
        Lv_pldsr_Clo_Sel(); 
        vtcp_log("[%s][%d]firstOrderno=[%s]",__FILE__,__LINE__,firstOrderno);
        if (strlen(firstOrderno)>1)
        {
            sprintf(outarr,"对应定借从[%s]到[%s]",firstOrderno,lastOrderno);
            
            /**这里需要增加包的状态**/
            memset(&gLv_wbctl , 0 , sizeof(gLv_wbctl));
            iRc = Lv_wbctl_Sel( g_pub_tx.reply,&gLv_wbctl,"packid = %s and pack_date = %d and pkgno='006' ",tx.packid,tx.pack_date);
            if ( iRc )
            {
                sprintf(acErrMsg,"小额往包控制表中不存在该记录");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d] packstat=[%s]\n",__FILE__,__LINE__,gLv_wbctl.packstat);
            /*beg  mod by ChengGX 2010514 衡水*/
            if(strlen(gLv_wbctl.packstat)>0)
                dsf_wbctl(gLv_wbctl.packstat,outarr);
            else
                strcpy(outarr+strlen(outarr),"  拼包成功 未发送");
            /*end mod by ChengGX 2010514 衡水 */
            vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
            
            if(strlen(tx.rcpstat)==2)
            {
                memcpy(cBuff,",且收到应答",11);
            }
            else
            {
                memcpy(cBuff,",未收到应答",13);
            }
            strcat(outarr,cBuff);
            vtcp_log("[%s][%d] xyy outarr=[%s]\n",__FILE__,__LINE__,outarr);
            vtcp_log("[%s][%d] txstat=[%s]\n",__FILE__,__LINE__,tx.lv_sts);
        }
        else
        {
            sprintf(outarr,"未做定期借记交易");
        }
    }
    else
    {
        vtcp_log("[%s][%d]tx_mx_no=[%s]",__FILE__,__LINE__,tx_mx_no);
        Lv_pldsr_Sel(g_pub_tx.reply,&ds,"orderno='%s' and wt_date=%s and tx_mx_no='%08s' ",order_no,wt_day,tx_mx_no);
        memset(&tx,0,sizeof(tx));
        vtcp_log("[%s][%d]ds.wt_date=[%ld]",__FILE__,__LINE__,ds.wt_date);
        iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);

        memcpy(firstOrderno,tx.orderno,sizeof(firstOrderno)-1);
        if (strlen(firstOrderno)>1)
        {
            sprintf(outarr,"对应定期借记[%s],",firstOrderno);
            if(strlen(tx.rcpstat)==2)/***如果长度为2的话,那就说明已经做过定期借记回执啦**/
            {
                dsf_txstat(tx.lv_sts[0],outarr);
                dsf_dqjjhzstat(tx.rcpstat,outarr);
            }
            else
            {
                dsf_txstat(tx.lv_sts[0],outarr);
            }
            vtcp_log("[%s][%d] xyy outarr=[%s]\n",__FILE__,__LINE__,outarr);
            vtcp_log("[%s][%d] txstat=[%s]\n",__FILE__,__LINE__,tx.lv_sts);
        }
        else
        {
            sprintf(outarr,"未做对应定期借记交易");
        }
    }
    return 0;
}

/*找出代付对应的贷记交易*/
int dsf_getdfdj(char *wt_day,char *pack_id,char *order_no,char *tx_mx_no,char *outarr)
{
    struct lv_pldfr_c df;
    struct lv_pkgreg_c tx;
    char firstOrderno[9],lastOrderno[9];
    char cTx_mx_no[9];
    char cPkgno[4];
    int iRc=0,i=0;
    struct lv_wbctl_c gLv_wbctl;
    
    memset(firstOrderno,0,sizeof(firstOrderno));
    memset(lastOrderno,0,sizeof(lastOrderno));
    memset(cTx_mx_no  ,0,sizeof(cTx_mx_no));
    memset(cPkgno     ,0,sizeof(cPkgno));
    
    memcpy(cPkgno , "005" ,sizeof(cPkgno)-1);
    sprintf(cTx_mx_no,"%08s",tx_mx_no);
    vtcp_log("[%s][%d]tx_mx_no=[%s]",__FILE__,__LINE__,tx_mx_no);
    vtcp_log("[%s][%d]cTx_mx_no=[%s]",__FILE__,__LINE__,cTx_mx_no);
    if(memcmp(cTx_mx_no, "00000000", 8)==0)
    {
        Lv_pldfr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",order_no,wt_day);
        while(1)
        {
            memset(&df,0,sizeof(df));
            
            g_reply_int = Lv_pldfr_Fet_Sel( &df,g_pub_tx.reply );
            if (g_reply_int)
            {
                vtcp_log("[%s][%d]数据已经取完.\n",__FILE__,__LINE__);
                break;
            }
            memset(&tx,0,sizeof(tx));
            g_reply_int = iDsf_gettx(&tx,df.wt_date,df.orderno,df.tx_mx_no,cPkgno);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],数据已经取完[%d]",__FILE__,__LINE__, g_reply_int);
                break;
            }
            if (i==0)
            { 
                /*第一次*/
                memcpy(firstOrderno,tx.orderno,sizeof(firstOrderno)-1);
                memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            } 
            else 
            {
                memcpy(lastOrderno,tx.orderno,sizeof(lastOrderno)-1);
            }
            i++;
        }
        Lv_pldfr_Clo_Sel();
        if (strlen(firstOrderno)>1)
        {
            sprintf(outarr,"对应定期贷记序号[%s]到[%s]",firstOrderno,lastOrderno);
            /**这里需要增加包的状态**/
            memset(&gLv_wbctl , 0 , sizeof(gLv_wbctl));
            iRc = Lv_wbctl_Sel( g_pub_tx.reply,&gLv_wbctl,"packid = %s and pack_date = %d and pkgno='005' ",tx.packid,tx.pack_date);
            if ( iRc )
            {
                sprintf(acErrMsg,"小额往包控制表中不存在该记录");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d] packstat=[%s]\n",__FILE__,__LINE__,gLv_wbctl.packstat);
            /*beg  mod by ChengGX 2010514 衡水*/
            if(strlen(gLv_wbctl.packstat)>0)
                dsf_wbctl(gLv_wbctl.packstat,outarr);
            else
                strcpy(outarr+strlen(outarr),"  拼包成功");
            /*end mod by ChengGX 2010514 衡水 */
            vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
        }
        else
        {
            sprintf(outarr,"未做定期贷记交易");
        }
    }
    else
    {
        vtcp_log("[%s][%d]tx_mx_no=[%s]",__FILE__,__LINE__,tx_mx_no);
        Lv_pldfr_Sel(g_pub_tx.reply,&df,"orderno='%s' and wt_date=%s and tx_mx_no='%08s' ",order_no,wt_day,tx_mx_no);
        memset(&tx,0,sizeof(tx));
        vtcp_log("[%s][%d]ds.wt_date=[%ld]",__FILE__,__LINE__,df.wt_date);
        iDsf_gettx(&tx,df.wt_date,df.orderno,df.tx_mx_no,cPkgno);

        memcpy(firstOrderno,tx.orderno,sizeof(firstOrderno)-1);
        if (strlen(firstOrderno)>1)
        {
            sprintf(outarr,"对应定期贷记序号[%s]",firstOrderno);
            dsf_txstat(tx.lv_sts[0],outarr);
            vtcp_log("[%s][%d] xyy outarr=[%s]\n",__FILE__,__LINE__,outarr);
            vtcp_log("[%s][%d] txstat=[%s]\n",__FILE__,__LINE__,tx.lv_sts);
        }
        else
        {
            sprintf(outarr,"未做定期贷记交易");
        }
    }
    return 0;
}


/*根据帐号取户名和余额,人行的户名可以跟我行系统内的户名不一致,所以,这里就不判断户名了*/
int get_name(char *ac_no,char *name,char  *avbal)
{
  int ret=0;
  int len=0;
  char cName[51];
  char cAcno[33];
  double dAvbal=0.0;
  struct dd_mst_c sDd_mst;
  struct td_mst_c sTd_mst;
  struct ln_mst_c sLn_mst;
  struct in_mst_c sIn_mst;
  struct mdm_ac_rel_c sMdm_ac_rel;
  struct prdt_ac_id_c sPrdt_ac_id;

  memset(cAcno   ,  0 ,sizeof(cAcno));
  memset(cName   ,  0 ,sizeof(cName));
  memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&sTd_mst,0x00,sizeof(struct td_mst_c));
  memset(&sLn_mst,0x00,sizeof(struct ln_mst_c));
  memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
  memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

  memcpy(cName,name,sizeof(cName)-1);
  memcpy(cAcno,ac_no,sizeof(cAcno)-1);
  zip_space(cAcno);
  
  zip_space(cName);
  vtcp_log("[%s][%d]cName=[%s]",__FILE__,__LINE__,cName);
  len=strlen(cName);
  vtcp_log("[%s][%d]len=[%d]",__FILE__,__LINE__,len);
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",cAcno);
  if(ret)
  {
      sprintf(acErrMsg,"查询mdm_ac_rel出错!![%d]",ret);
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
  if(ret)
  {
      sprintf(acErrMsg,"查询prdt_ac_id出错!![%d]",ret);
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  vtcp_log("[%s][%d]ac_id_type=[%c]",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
  switch(sPrdt_ac_id.ac_id_type[0])
  {
    case '1':/*活期*/
      ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);  
      if(ret)
      {
          sprintf(acErrMsg,"查询dd_mst出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
      if(sDd_mst.ac_sts[0]!='1' || sDd_mst.hold_sts[0]=='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sDd_mst.name);          
          sprintf(acErrMsg,"状态状态[%s]或冻结状态[%s]不正常!",sDd_mst.ac_sts,sDd_mst.hold_sts);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit3;
      }
      if(!len)
      {
          memcpy(name,sDd_mst.name,sizeof(sDd_mst.name)-1);
          memcpy(cName,name,strlen(name));
      }
      if(memcmp(cName,sDd_mst.name,strlen(sDd_mst.name))!=0)
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sDd_mst.name);          
          sprintf(acErrMsg,"查询dd_mst户名出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }
      dAvbal=sDd_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    case '2':/*定期存款*/
      ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"查询td_mst出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
      if(sTd_mst.ac_sts[0]!='1' || sTd_mst.hold_sts[0]=='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sTd_mst.name);          
          sprintf(acErrMsg,"状态状态[%s]或冻结状态[%s]不正常!",sTd_mst.ac_sts,sTd_mst.hold_sts);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit3;
      }
      if(!len)
      {
          memcpy(name,sTd_mst.name,sizeof(sTd_mst.name)-1);
          memcpy(cName,name,strlen(name));
      }
      if(memcmp(cName,sTd_mst.name,strlen(sTd_mst.name))!=0)
      { 
          vtcp_log("[%s][%d]cName=[%s]sTd_mst.name=[%s]",__FILE__,__LINE__,cName,sTd_mst.name); 
          sprintf(acErrMsg,"查询td_mst户名出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }   
      dAvbal=sTd_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    /**********delete by xyy 2009-7-24 15:29:33
    case '3':/ *贷款* /
      ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"查询ln_mst出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
      if(!len)
      {
          memcpy(name,sLn_mst.name,sizeof(sLn_mst.name)-1);
          memcpy(cName,name,strlen(name));
      }
      if(memcmp(cName,sLn_mst.name,strlen(sLn_mst.name))!=0)
      {
          vtcp_log("[%s][%d]cName=[%s]sLn_mst.name=[%s]",__FILE__,__LINE__,cName,sLn_mst.name); 
          sprintf(acErrMsg,"查询ln_mst户名出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }  
      dAvbal=sLn_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
      ********************/
    case '9':/*内部帐*/
      ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"查询in_mst出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
       if(sIn_mst.sts[0]!='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sIn_mst.name);          
          sprintf(acErrMsg,"状态状态[%s]不正常!",sIn_mst.sts);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit3;
      }
      if(!len)
      {
          memcpy(name,sIn_mst.name,sizeof(sIn_mst.name)-1);
          memcpy(cName,name,strlen(name));
      }
      if(memcmp(cName,sIn_mst.name,strlen(sIn_mst.name))!=0)
      {
          vtcp_log("[%s][%d]cName=[%s]sIn_mst.name=[%s]",__FILE__,__LINE__,cName,sIn_mst.name);
          sprintf(acErrMsg,"查询in_mst户名出错!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }
      dAvbal=sIn_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    default:
      sprintf(acErrMsg,"无效的帐户类型");
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
      break;
  }
OkExit:
   return 0;
ErrExit:
   return -1;
ErrExit2:
   return -2;
ErrExit3:
   return -3;
}

int dsf_dqjjhzstat(char * stat,char *str2)
{
    char str[31];
    memset(str , 0 , sizeof(str));
    vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
    if(memcmp(stat,RET_SUCCESS,2)==0)  /*回执成功*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[成功]");
    }
    else if(memcmp(stat,RET_NOACTNO,2)==0)/*帐号不符*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[帐号不符]");
    }
    else if(memcmp(stat,RET_NONAME,2)==0)/*帐号户名不符*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[帐号户名不符]");
    }
    else if(memcmp(stat,RET_NOMONEY,2)==0)  /*账户余额不足支付*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[账户余额不足]");
    }
    else if(memcmp(stat,RET_PSWDERR,2)==0) /*账户密码错误*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[账户密码错误]");
    }
    else if(memcmp(stat,RET_ACTCLSERR,2)==0) /*帐号状态错误*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[帐号状态错误]");
    }
    else if(memcmp(stat,RET_HASCANCEL,2)==0) /*业务已撤销*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[业务已撤销]");
    }
    else if(memcmp(stat,RET_OTHER,2)==0) /*其他错误*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方已回执[其他错误]");
    }
    else
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",且对方没有这种状态的错误");
    }
    vtcp_log("[%s][%d]str==[%s]",__FILE__,__LINE__,str);
    strcat(str2,str);
    vtcp_log("[%s][%d]str2==[%s]",__FILE__,__LINE__,str2);
    return 0;
}
/**翻译包状态**/
int dsf_wbctl(char * packstat,char *str2)
{
    long iCnt = 0;
    char cTmpSts[3];
    char cTmpBuf[12];
    memset(cTmpBuf , 0 , sizeof(cTmpBuf));
    memset(cTmpSts , 0 , sizeof(cTmpSts));
    
    memcpy(cTmpSts , packstat , sizeof(cTmpSts)-1);
    vtcp_log("[%s][%d] cTmpSts =[%s]\n",__FILE__,__LINE__,cTmpSts);
    
    iCnt = apatoi(cTmpSts,2);
    
    switch(iCnt){
        case  1      : 
            strncpy(cTmpBuf,",检查未通过",11);
            break;
        case  2      : 
            strncpy(cTmpBuf,",往包已发送",11);
            break;
        case  3      : 
            strncpy(cTmpBuf,",往包已扎差",11);
            break;
        case  4      : 
            strncpy(cTmpBuf,",往包已排队",11);
            break;
        case  5      : 
            strncpy(cTmpBuf,",往包已清算",11);
            break;
        case  6      : 
            strncpy(cTmpBuf,",往包已撤销",11);
            break;
        case  10      : 
            strncpy(cTmpBuf,",往包已回执",11);
            break;
        case  11      : 
            strncpy(cTmpBuf,",往包已拒付",11);
            break;
        case  12     : 
            strncpy(cTmpBuf,",往包已冲正",11);
            break;
        case  13      : 
            strncpy(cTmpBuf,",往包已止付",11);
            break;
        case  14      : 
            strncpy(cTmpBuf,",往包已超时",11);
             break;
        default:
            strncpy(cTmpBuf,",未知错误",9);
            break;
    }
    strcat(str2 , cTmpBuf);
    return 0;
}
/*********************************一下函数可能没有用到**********************************/

/*这个是对lv_dsf_deal的重构,他处理的整个包
暂时不用,只防止检查里有可能需要*/
int lv_dsf_deal2(TLVPACKAGE *ilv)
{
    int iRc=0,iCount=0,i;
    TLVPACKAGE sublv,olv;        /*小包,返回的包*/
    static char txtype[6];
    /*初始化数据*/
    lv_create(&sublv);
    lv_create(&olv);
    
    /*分解来包*/
    iCount = lv_count_recnt(ilv);
    while(!iCount--)
    {
        iRc=lv_fetchpkg(ilv,&sublv);

        iRc = lv_analyze_pack(ilv);
        if ( iRc!=0 )
        {
            vtcp_log("%s,%Upd 报文解析错误iRc=[%d]",__FILE__,__LINE__,g_reply_int);
            sprintf(acErrMsg,"代收付报文解析错误[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return (-1);
        }

        memset(txtype,0,sizeof(txtype));
        GETTAG(ilv,"0BG",txtype);    /*取代收付业务类型*/
        vtcp_log("[%s][%d]txtype==[%s]",__FILE__,__LINE__,txtype);
        if ( !memcmp(txtype,"40501",5))
        {
        }
        else if ( !memcmp(txtype,"40502",5))
        {
        }
        else if ( !memcmp(txtype,"40505",5))
        {
        }
        else if ( !memcmp(txtype,"40506",5))
        {
        }
        else 
        {
            vtcp_log("[%s][%d],无此业务类型yw_type[%s],忽略\n",txtype);
            continue;
        }
        
    }
    
    
    /*返回40506回应包*/
    lv_destroy(&sublv);
    lv_destroy(&olv);
    return 0;
}
/*根据回执在往包登记表里的状态修改原交易的状态*/
int dsf_wb2tx(char *iwt_date,char *iorderno,char stat,char *otx_stat)
{
    return 0;
}
int dsf_dayfromto(char *from,char *to)
{
   return 0;/*该函数已被俺替换成pub_base_CalTrueDays*/
}
int lv_dsf_deal40507_batch(TLVPACKAGE  *lv,struct lv_pldsfqugx_c *lv_pldsfqugx,int total,int count,int totalnum)
{
    char lvdate[9];
    int iRc=0;
    char temp[100];
    
    /*先发送数据包*/
    lv_load_tagifno();    
    
    memset(lvdate,0,sizeof(lvdate));
    memcpy(lvdate,pcLvGetDay_work(),8);
    
    if (count==total)
    {
        lv_get_dsf40507_packid(lv,lv_pldsfqugx->packid);
    }    
    lv_get_dsforderno(lv_pldsfqugx->orderno);

    get_fd_data("0030",lv_pldsfqugx->br_no);
    hvbrno(lv_pldsfqugx->br_no);
    vtcp_log("[%s][%d],lv_pldsfqugx->br_no=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->br_no);

    /*组文件包*/
    memcpy(lv->CMTCode,"012",3);   /*通用信息业务包*/
    lv->opeType = '1';             /*表明是小额报文*/
    lv->pkgType = '1';             /*表明是PKG包*/
    memcpy(lv->CMTNo,"005",3);     /*业务要素集*/
    lv_set_tag(lv,"0BG","40507");  /*业务类型号*/
    
    
    lv_set_tag(lv,"52A",lv_pldsfqugx->br_no);    
    memcpy(lv_pldsfqugx->or_br_no,lv_pldsfqugx->br_no,sizeof(lv_pldsfqugx->or_br_no)-1);
    lv_set_tag(lv,"58A",LV_DSFZX);
    lv_set_tag(lv,"30A",lvdate);    /*这个要取当前小额系统的日期*/
    lv_set_tag(lv,"0BC",lv_pldsfqugx->orderno);
    sprintf(temp,"%08d",310);
    lv_set_tag(lv,"B40",temp);    /*每次只只一个业务,就是318个字节的数据*/
    /*sprintf(temp,"%8s",lv_pldsfqugx->wt_date);
    lv_set_tag(lv,"72C",temp); del by weiz ,应该没有这个东东*/
    vtcp_log("[%s][%d]开始添加附加数据域\n",__FILE__,__LINE__);
    sprintf(temp,"%-60s",lv_pldsfqugx->xyno);
    lv_set_tag(lv,"72C",temp);
    sprintf(temp,"%1s",lv_pldsfqugx->xy_ind);
    lv_set_tag_multi(lv,"72C",temp);
    vtcp_log("[%s][%d]已经运行到这里\n",__FILE__,__LINE__);
    sprintf(temp,"%1s",lv_pldsfqugx->ch_ind);
    vtcp_log("[%s][%d]已经运行到这里\n",__FILE__,__LINE__);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_br_no);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_qs_no);
    vtcp_log("[%s][%d]已经运行到这里\n",__FILE__,__LINE__);
    lv_set_tag_multi(lv,"72C",temp); 
    sprintf(temp,"%12s",lv_pldsfqugx->pay_opn_br_no);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%32s",lv_pldsfqugx->pay_ac_no);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->pay_name);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->pay_addr);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%-60s",lv_pldsfqugx->content);
    lv_set_tag_multi(lv,"72C",temp);
    
    vtcp_log("[%s][%d],发送40507.\n",__FILE__,__LINE__);
    /*拼包*/
    /*包头信息*/
    lv_batch_end(lv);
    if (total==count)
    {
        lv_pkg_batch(lv,"02C","012");
        if (hvqsbrno(lv,lv_pldsfqugx->br_no)==-1)
        {
            vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
            return -1;
        }
        if (hvqsorbrno(lv,LV_DSFZX)==-1)
        {
            vtcp_log("[%s][%d],取行号出错\n",__FILE__,__LINE__);
            return -1;
        }
        lv_pkg_batch(lv,"30E",lvdate);
        
        lv_pkg_batch(lv,"0BD",lv_pldsfqugx->packid);
        lv_pkg_batch(lv,"BS7","0");
        lv_pkg_batch(lv,"0BG","40507");
        sprintf(temp,"%04d",totalnum);
        lv_pkg_batch(lv,"B42",temp);
        sprintf(temp,"%-60s","40507报文");
        lv_pkg_batch(lv,"72D",temp);
        /* .... */
        memcpy(lv->workDate,lvdate,8);
        lv_gen_pack(lv);
        /*在wd_lv_wbctl中插入一条记录来查询*/
        lv_dsf_wbctl(lv);
        if (lv_dsf_wbctl(lv)!=0)
        {
            vtcp_log("[%s][%d],写往包登记簿错\n",__FILE__,__LINE__);
            return -1;
        }
        /*发送*/
        lv->packID=atoi(lv_pldsfqugx->packid);
        DSFSEND(lv);
        vtcp_log("[%s][%d],把以后数据插入数据库\n",__FILE__,__LINE__);
        memcpy(lv_pldsfqugx->sts,DSF_SEND,2);
    }

    /*插入数据库*/

    g_reply_int  = Lv_pldsfqugx_Ins(*lv_pldsfqugx ,g_pub_tx.reply );
    if(g_reply_int)
    {
         vtcp_log("[%s][%d],插入签约关系变更通知登记簿错sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]插入签约关系变更通知登记簿错",__FL__);
         set_zd_data(DC_GET_MSG,acErrMsg);
         return(-1);
    }
    return 0;
}
static int getactname(const char *isactno,const char *iname)
{
     return 0;
}
