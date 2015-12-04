/***************************************************************************/
/* �ļ�����: lv_dsf_deal.c                                                 */
/* ��������: ������ո�ҵ�񹫹�����                                        */
/* ��������: 2005.11.20                                                    */
/* ��    ��: WeiZ                                                          */
/* ˵    ��:                                                               */
/* ��    ��: xyy                                                           */
/* �޸�����: 2006-12-10 14:06                                              */
/* �޸�����: ����ϵͳ�Ĵ��ո��Ĺ��������������Ӧ����ͳ�ĺ���,             */
/*            (#####ע�����ڵ����ͣ�)                                      */
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
/*#define LV_DSFZX "102161000017"            ���ո������к�,�����ȷ�һ��,�Ժ�Ҫ��*/
#define LV_DSFZX "906161000014"              /**ɽ��ʡ���ո������к� 906161000014  ����ǰ��Ҫ�滻**/

#define DSF_RECV    "0A"    /*���˽���*/
#define DSF_REJE    "1B"    /*���˾ܾ�*/
#define DSF_CONC    "0C"    /*���˳���*/
#define DSF_SUCC    "0D"    /*�ɹ���ִ40506*/
#define DSF_FAIL    "1E"    /*ʧ�ܻ�ִ40506*/
#define DSF_QSSU    "0F"    /*�ɹ���ִ40503/4*/
#define DSF_QSFA    "0G"    /*ʧ�ܻ�ִ40503/4*/
#define DSF_SEND    "0H"    /*���˷���*/
#define DSF_SREJ    "1I"    /*���˾ܾ�*/
#define DSF_QSSS    "0J"    /*��������*/
#define DSF_QSSF    "1K"    /*����ʧ��*/

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
    /******* ȡ�����ݽṹ���� ********/
    FILE    *fp;
    int     ret=0,offset=0,i=0;
    char    yw_type[6];
    char    yanamt[16];
    char    outFile[257];
    
    char cDsf_date[9];   /*���ո����Ĺ�����*/
    char cRe_lim_date[3];/*��ִ����(����)*/
    char cTotamt[16];    /*�����տ��ܽ��*/
    char cPay_num[9];    /*��������Ŀ*/
    char cCash_num[9];   /*�տ�����Ŀ*/
    char cTx_amt[16];    /*���׽��*/
    char cRe_dsf_date[9];/*���ո����Ĺ�����*/
    char cO_wt_date[9];  /*ԭί������*/
    char cWt_date[9];
    char cTmpXyno[121];
    
    vtcp_log("[%s][%d] ���봦����ո�ҵ������˵���!!!!\n",__FILE__,__LINE__);
    
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
        vtcp_log("[%s][%d],���ķ�������\n",__FILE__,__LINE__);
        return -1;
    }
    GETTAG(lv,"0BG",yw_type);
    vtcp_log("[%s][%d]���е�����\n",__FILE__,__LINE__);
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
        /*ע������õ��Ľ�����ȷ��*/

        vtcp_log("[%s][%d] pldfr.totamt=[%f]",__FILE__,__LINE__,dsf.pldfr.totamt);
        GETOFFSET2(dsf.pldfr.cash_qs_no);
        vtcp_log("[%s][%d] pldfr.cash_qs_no=[%s]",__FILE__,__LINE__,dsf.pldfr.cash_qs_no);
        GETOFFSET2(cCash_num);
        vtcp_log("[%s][%d] ******cCash_num******=[%s]",__FILE__,__LINE__,cCash_num);
        dsf.pldfr.cash_num = apatoi(cCash_num,8);
        vtcp_log("[%s][%d] pldfr.cash_num=[%ld]",__FILE__,__LINE__,dsf.pldfr.cash_num);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);

        /******* �����Ŀ�š�************/

        memcpy(dsf.pldfr.sts,DSF_RECV,2);        
        memcpy(dsf.pldfr.repstat2,"00",2);
        memcpy(dsf.pldfr.re_content2,"��Ӧ�ɹ�",8);
        vtcp_log("[%s][%d] pldfr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldfr.repstat2);
        vtcp_log("[%s][%d] pldfr.re_content2=[%s]",__FILE__,__LINE__,dsf.pldfr.re_content2);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);

        if (memcmp(dsf.pldfr.xyno,XYNO,60))
        {
            vtcp_log("[%s][%d] dsf.pldfr.xyno [%s]",__FILE__,__LINE__,dsf.pldfr.xyno);
            vtcp_log("[%s][%d] XYNO [%s]",__FILE__,__LINE__,XYNO);
            vtcp_log("[%s][%d] ��ͬ�ŷ�ȫ��",__FILE__,__LINE__);
            ret = lv_check_xmh(&dsf.pldfr);    /**���û����֤��,��֤,��ͨ��ȫ�����**/
            if (ret)
            {
                vtcp_log("[%s][%d],����Ŀ�ų��� ret=[%d]",__FILE__,__LINE__,ret);
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
        {   /*��ȫ0�ʹ���Ŀ�������ҵ���Ե��кŶ�Ӧ����Ŀ,û�оͽ���һ��*/
            vtcp_log("[%s][%d] ��ͬ��ȫ��",__FILE__,__LINE__);
            ret = lv_get_xmh(&dsf.pldfr);
            if (ret)
            {
                vtcp_log("[%s][%d],����Ŀ�ų��� ret=[%d]",__FILE__,__LINE__,ret);
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
        
        vtcp_log("[%s][%d]���е�����cash_num==[%d],\n",__FILE__,__LINE__,i);
        
        memset(outFile,0,sizeof(outFile));
        sprintf(outFile,"%s/tmp/df%ld_%s",getenv("WORKDIR"),dsf.pldfr.dsf_date,dsf.pldfr.orderno);
        fp = fopen(outFile,"w+");
        if (!fp)
        {
            vtcp_log("[%s][%d],���ܴ��ļ�%s\n",__FILE__,__LINE__,outFile);
            return -1;
        }
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        memcpy(dsf.pldfr.filename,outFile,256);
        fprintf(fp,"%s\n",cTmpXyno);
        vtcp_log("[%s][%d]dsf.pldfr.xyno==[%s]",__FILE__,__LINE__,dsf.pldfr.xyno);
        vtcp_log("[%s][%d] pldfr.yw_type=[%s]",__FILE__,__LINE__,dsf.pldfr.yw_type);
        vtcp_log("[%s][%d] pldfr.dsf_date=[%ld]",__FILE__,__LINE__,dsf.pldfr.dsf_date);
        /*����дһ��ȫ���׵ļ�¼,��ʱ��tx_mx_noΪ00000000*/
        memset(dsf.pldfr.tx_mx_no,'0',8);
        /*����Ǽǲ�*/

        vtcp_log("[%s][%d]׼�������¼lv_pldfr",__FILE__,__LINE__);
         
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
            vtcp_log("[%s][%d],������ո��Ǽǲ���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"[%s][%d]����Lv_pldsr�����",__FL__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(-1);
        }

        vtcp_log("[%s][%d]����׼��ѭ����",__FILE__,__LINE__);
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
            str2dbl(cTx_amt,sizeof(cTx_amt)-1,0,&dsf.pldfr.tx_amt);/*ע������õ��Ľ�����ȷ��*/
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
            /*����Ǽǲ�*/

            g_reply_int  = Lv_pldfr_Ins(dsf.pldfr ,g_pub_tx.reply );
            if(g_reply_int)
            {
                 Lv_pldfr_Debug(&dsf.pldfr);
                 vtcp_log("[%s][%d],������ո��Ǽǲ���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
                 sprintf( acErrMsg,"[%s][%d]����Lv_pldsr�����code=[%d]",__FL__,g_reply_int);
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
        str2dbl(cTotamt,sizeof(cTotamt)-1,0,&dsf.pldsr.totamt);/*ע������õ��Ľ�����ȷ��*/
        vtcp_log("[%s][%d] dsf.pldsr.totamt=[%f]",__FILE__,__LINE__,dsf.pldsr.totamt);
        dsf.pldsr.totamt = dsf.pldsr.totamt/100;
        vtcp_log("[%s][%d] dsf.pldsr.totamt=[%f]",__FILE__,__LINE__,dsf.pldsr.totamt);
        GETOFFSET2(dsf.pldsr.pay_qs_no);
        vtcp_log("[%s][%d] pldfr.pay_qs_no=[%s]",__FILE__,__LINE__,dsf.pldsr.pay_qs_no);
        GETOFFSET2(cPay_num);
        vtcp_log("[%s][%d] cPay_num=[%s]",__FILE__,__LINE__,cPay_num);
        dsf.pldsr.pay_num = apatoi(cPay_num,8);
        vtcp_log("[%s][%d] pldfr.pay_num=[%ld]",__FILE__,__LINE__,dsf.pldsr.pay_num);
        
  
        /******* �����Ŀ�š�************/
        memcpy(dsf.pldsr.sts,DSF_RECV,2);        /*Ĭ�ϵ�״̬Ϊ�ɹ�*/
        memcpy(dsf.pldsr.repstat2,"00",2);
        memcpy(dsf.pldsr.re_content2,"��Ӧ�ɹ�",8);
        vtcp_log("[%s][%d] pldfr.re_content2=[%s]",__FILE__,__LINE__,dsf.pldsr.re_content2);
        
        /*��ȫ0�ʹ���Ŀ�������ҵ���Ե��кŶ�Ӧ����Ŀ,û�оͽ���һ��*/
        ret = lv_get_xmno(&dsf.pldsr);
        if (ret)
        {
            vtcp_log("[%s][%d],ȡ��Ŀ�ų���ret[%d]\n",__FILE__,__LINE__,ret);
            memcpy(cTmpXyno , dsf.pldsr.re_content2 ,strlen(dsf.pldsr.re_content2));
            strcat(cTmpXyno , dsf.pldsr.xyno);
            /**������ò�Ҫreturn ��,Ӧ���Ȱ���ʼ�¼������
            return(-1);
            **/
        }
        else
        {
            memcpy(cTmpXyno , dsf.pldsr.xyno       ,sizeof(dsf.pldsr.xyno)-1);
        }
        zip_space(cTmpXyno);
        vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
        vtcp_log("[%s][%d],ȡ�õ���Ŀ��Ϊ=[%s]\n",__FILE__,__LINE__,cTmpXyno);
        
        i = dsf.pldsr.pay_num;
        vtcp_log("[%s][%d]pay_num==[%d]",__FILE__,__LINE__,i);
        memset(outFile,0,sizeof(outFile));
        sprintf(outFile,"%s/tmp/ds%ld_%s",getenv("WORKDIR"),dsf.pldsr.dsf_date,dsf.pldsr.orderno);
        fp = fopen(outFile,"w+");
        if (!fp)
        {
            vtcp_log("[%s][%d],���ܴ��ļ�%s\n",__FILE__,__LINE__,outFile);
            return -1;
        }
        
        vtcp_log("[%s][%d],outputFilename [%s]\n",__FILE__,__LINE__,outFile);
        fprintf(fp,"%s\n",cTmpXyno);
        memcpy(dsf.pldsr.filename,outFile,256);
        
        /*����дһ��ȫ���׵ļ�¼,��ʱ��tx_mx_noΪ00000000*/
        memset(dsf.pldsr.tx_mx_no,'0',8);
        /*����Ǽǲ�*/
        vtcp_log("[%s][%d] stat[%s]repstat2[%s]re_content2[%s]\n",__FILE__,__LINE__,
                dsf.pldsr.sts,dsf.pldsr.repstat2,dsf.pldsr.re_content2);

        g_reply_int = Lv_pldsr_Ins(dsf.pldsr,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]����Lv_pldsr�����code=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return(-1);
        }
        vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
        memset(dsf.pldsr.tx_mx_no,0,sizeof(dsf.pldsr.tx_mx_no));
        while(i--)
        {
            vtcp_log("[%s][%d]ѭ�����������¼\n",__FILE__,__LINE__);
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
            str2dbl(cTx_amt,sizeof(cTx_amt)-1,0,&dsf.pldsr.tx_amt);/*ע������õ��Ľ�����ȷ��*/
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldsr.tx_amt);
            dsf.pldsr.tx_amt = dsf.pldsr.tx_amt/100;
            vtcp_log("[%s][%d] pldfr.tx_amt=[%f]",__FILE__,__LINE__,dsf.pldsr.tx_amt);
            GETOFFSET2(dsf.pldsr.xyno);
            zip_space(dsf.pldsr.xyno);
            
            GETOFFSET2(dsf.pldsr.content);

            vtcp_log("[%s][%d],tx_mx_no=[%s]\n",__FILE__,__LINE__,dsf.pldsr.tx_mx_no);

            /*����Ǽǲ�*/
            
            g_reply_int = Lv_pldsr_Ins(dsf.pldsr,g_pub_tx.reply );
            if(g_reply_int)
            {
                sprintf( acErrMsg,"[%s][%d]����Lv_pldsr�����code=[%d]",__FL__,g_reply_int);
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                return(-1);
            }
            vtcp_log("[%s][%d] pldsr.repstat2=[%s]",__FILE__,__LINE__,dsf.pldsr.repstat2);
            vtcp_log("[%s][%d],�鿴tx_mx_no[%s]\n",__FILE__,__LINE__,dsf.pldsr.tx_mx_no);
            
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

        /**���볷����**/
        g_reply_int = Lv_pldscx_Ins(dsf.pldscx,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]����Lv_pldscx�����code=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return(-1);
        }
        if ( memcmp(dsf.pldscx.sts,DSF_FAIL,2)==0 ||
             memcmp(dsf.pldscx.sts,DSF_QSSU,2)==0 ||
             memcmp(dsf.pldscx.sts,DSF_QSFA,2)==0  ) 
        {
            /*ֱ�ӻ�Ӧ����ʧ��*/
            lv_return_40506(lv,&dsf.pldscx,"40505",iCount,allCount,return40506Pack);
        }
        vtcp_log("[%s][%d] pldscx.repstat2=[%s]",__FILE__,__LINE__,dsf.pldscx.repstat2);
    }
    else if (!memcmp(yw_type,"40506",5))
    {
        /*ֻ���ں�ͬ�ű����ʱ��Ҫ�����40506*/
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
             vtcp_log("[%s][%d],����ԭ���׳���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
             sprintf( acErrMsg,"[%s][%d]����ԭ���׳���[%d]",__FL__,g_reply_int);
             set_zd_data(DC_GET_MSG,acErrMsg);
             return(-1);
        }
        g_reply_int = Lv_pldsfqugx_Fet_Upd( &dsf.pldsfqugx,g_pub_tx.reply );
        if (g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]����ԭ���׳���[%d]",__FL__,g_reply_int);
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
            vtcp_log("%s,%Upd lv_pldsfqugx�����[%d]",__FILE__,__LINE__,g_reply_int);
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
        /*�Ǵ�������ҵ��*/
        vtcp_log("[%s][%d],���Ǵ��ո�ҵ��[%s],�ɹ�����\n",__FILE__,__LINE__,yw_type);
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
    
    /*���ݳ�ʼ��*/
    
    memset(cWt_date,0, sizeof(cWt_date));
    memset(actno   ,0, sizeof(actno));
    memset(cOrbrno ,0, sizeof(cOrbrno));
    memset(cQsbrno ,0, sizeof(cQsbrno));
    memset(cTmpBuf ,0, sizeof(cTmpBuf));
    memset(&lv_xmh ,0, sizeof(lv_xmh));
    memset(&hv_uniontab,0,sizeof(hv_uniontab));
    
    /********����Ҫ���⴦��һ��********/
    memcpy(df->pay_name,(char*)DBC2SBC(df->pay_name,sizeof(df->pay_name)-1),sizeof(df->pay_name)-1);/*������ȫ���ת�� add by ChengGX 20100603*/
    iRc = get_name(df->pay_ac_no,df->pay_name,s_avbal);
    avbal = atof(s_avbal);
    vtcp_log("[%s][%d][%d]�˺ŵ������[%s]\n",__FILE__,__LINE__,iRc,s_avbal);
    if(iRc ==-1 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"01",2);
        sprintf(df->re_content2,"�˺Ų�����");
        vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
        return 1;
    }
    else if(iRc ==-2 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"02",2);
        memcpy(df->re_content2,"�˺š���������",14);
        return 2;
    }
    else if(iRc ==-3 )
    {
        memcpy(df->sts,DSF_FAIL,2);
        memcpy(df->repstat2,"11",2);
        memcpy(df->re_content2,"�˺�״̬����",12);
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
            vtcp_log("[%s][%d]�˺ŵ������[%f]���׽����[%f]\n",__FILE__,__LINE__,avbal,totamt);
            sprintf(df->re_content2,"����\n");
            return 3;
        }
    }
    memcpy(df->sts,DSF_SUCC,2);
    memcpy(df->repstat2,"00",2);
    memcpy(df->re_content2,"��Ӧ�ɹ�",8);
    vtcp_log("[%s][%d] df->re_content2=[%s]",__FILE__,__LINE__,df->re_content2);
    
    /*���ݼ��,�����Ŀ���Ƿ����*/
    if (memcmp(df->xyno,XYNO,60)!=0)
    {
        vtcp_log("[%s][%d]Э��Ų���ȫ0,�벻Ҫ�ñ�����ȡЭ���",__FILE__,__LINE__);
        return -1;
    }
    
    /*----�����˺���ͬ��,ȡ���Э���---*/
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
            memcpy(df->re_content2,"��������Э�黧������",20);
            return 2;
        }
        
        /****���ﲻ�ǾͱȽ�һ���������˻���,TODO****/
        
        
        
        return 0;
    }
    Lv_xmh_Clo_Sel();
    vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
    /*���û��,����һ��,Э��Ű�����+�˺�������*/
    if (lv_xmh.xmno[0]=='\0' || lv_xmh.xmno[0]==' ')    /*Ϊ�ջ��ǿո���û��ȡ��*/
    {
        memset(&lv_xmh,0,sizeof(&lv_xmh));
        apitoa(df->wt_date,sizeof(cWt_date)-1,cWt_date);
        memcpy(cTmpBuf,cWt_date,sizeof(cWt_date)-1);
        strcat(cTmpBuf,df->pay_ac_no);
        strcat(cTmpBuf,df->cash_qs_no);/*mod by ChengGX 20100607 ��ˮ  ͬһ�����а��������ϸ*/
        strcat(cTmpBuf,df->packid);/**Ϊ�˷�ֹһ�����˼���ͬһ�ʻ���ͬ�����е�����**/
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
        /**���������xmno�е�o_qs_no**/
        memcpy(cOrbrno,df->cash_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);
        
        memcpy(lv_xmh.tel , g_pub_tx.tel, sizeof(lv_xmh.tel)-1);
        vtcp_log("[%s][%d] ����ԱΪ=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);

        memcpy(lv_xmh.brf , "�������Ӵ���Э���" , sizeof(lv_xmh.brf)-1);
        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no==[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
       
        g_reply_int = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );
        if(g_reply_int)
        {
            sprintf( acErrMsg,"[%s][%d]����LV_XMH�����",__FL__);
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
    
    /***�����ȼ���ʺź����,����ʺŲ����ھ�ֱ��return;
        �����������,ֱ��return;�������,ֱ��return;
        ����,��ʱ��ִ�ɹ�,���ж϶෽Э��õ���ȷ��***/
    memcpy(pldfr->pay_name,(char*)DBC2SBC(pldfr->pay_name,sizeof(pldfr->pay_name)-1),sizeof(pldfr->pay_name)-1);/*add by ChengGX 20100603ȫ���ת��*/
    ret = get_name(pldfr->pay_ac_no,pldfr->pay_name,s_avbal);
    vtcp_log("[%s][%d] ret[%d] avbal=[%f]\n",__FILE__,__LINE__,ret,s_avbal);
    if(ret ==-1 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"01",2);
        sprintf(pldfr->re_content2,"�˺Ų�����");
        vtcp_log("[%s][%d]�Ѿ���������ret=[%d]",__FILE__,__LINE__,ret);
        return 1;
    }
    else if(ret ==-2 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"02",2);
        memcpy(pldfr->re_content2,"�˺š���������",14);
        vtcp_log("[%s][%d]�Ѿ���������ret=[%d]",__FILE__,__LINE__,ret);
        return 2;
    }
    else if(ret ==-3 )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"11",2);
        memcpy(pldfr->re_content2,"�˺�״̬����",12);
        vtcp_log("[%s][%d]�Ѿ���������ret=[%d]",__FILE__,__LINE__,ret);
        return 3;
    }
    vtcp_log("[%s][%d]�˺ŵ����Ϊ[%s]\n",__FILE__,__LINE__,s_avbal);
    avbal = atof(s_avbal);
    totamt = pldfr->totamt;
    vtcp_log("[%s][%d]����totamt==��%f��",__FILE__,__LINE__,totamt);
    vtcp_log("[%s][%d]���avbal ==��%f��",__FILE__,__LINE__,avbal);
    if ( avbal<totamt )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"03",2);
        sprintf(pldfr->re_content2,"����\n");
        vtcp_log("[%s][%d]�Ѿ���������ret=[%d]",__FILE__,__LINE__,ret);
        return 3;
    }
    memcpy(pldfr->sts,DSF_SUCC,2);
    memcpy(pldfr->repstat2,"00",2);
    memcpy(pldfr->re_content2,"��Ӧ�ɹ�",10);
    vtcp_log("[%s][%d] pldfr->re_content2,=[%s]",__FILE__,__LINE__,pldfr->re_content2);
    
    memcpy(lv_xmh.xmno,pldfr->xyno,60);
    memcpy(lv_xmh.o_qs_no,pldfr->cash_qs_no,12);
    vtcp_log("[%s][%d]lv_xmh.o_qs_no=[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
    vtcp_log("[%s][%d],���ں˲��ͬ��[%s]\n",__FILE__,__LINE__,pldfr->xyno);

    ret = Lv_xmh_Sel(g_pub_tx.reply,&lv_xmh,"xmno='%s' and xm_ind='0' and o_qs_no='%s' ",lv_xmh.xmno,lv_xmh.o_qs_no);
    if(ret == 100 && getenv("FHTEST")!=NULL && strcmp(getenv("FHTEST"),"LVTEST")==0)
    {
        memcpy(lv_xmh.ac_no,pldfr->pay_ac_no,18);
        zip_space(lv_xmh.ac_no);
        memcpy(lv_xmh.name,pldfr->pay_name,sizeof(pldfr->pay_name));
        memcpy(lv_xmh.yw_ind,pldfr->yw_type,sizeof(pldfr->yw_type));
        lv_xmh.flg[0]='0';
        lv_xmh.xm_ind[0]='0';/**��**/
        memcpy(lv_xmh.br_no, QS_BR_NO ,sizeof(lv_xmh.br_no)-1);
        /**���������xmno�е�o_qs_no**/
        memcpy(cOrbrno,pldfr->cash_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);

        memcpy(lv_xmh.tel , g_pub_tx.tel, sizeof(lv_xmh.tel)-1);
        vtcp_log("[%s][%d] ����ԱΪ=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);
        
        memcpy(lv_xmh.brf , "�������Ӳ���Э���" , sizeof(lv_xmh.brf)-1);

        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no==[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
        vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
        ret = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );

    }
    if(ret)
    {
        vtcp_log("[%s][%d],��ѯ��Ŀ���sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"90",2);
        memcpy(pldfr->re_content2,"�޴˺�ͬ��",10);
        vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
        return(99); /*����1��ʾ�˺Ų�����,��Ϊ���ڵĻ�ִ��û�к�ͬ�Ŵ�����,�����Ժ��ٸİ�*/
    }

    /*����˺�*/
    trim(lv_xmh.ac_no);
    trim(pldfr->pay_ac_no);
    if ( memcmp(lv_xmh.ac_no,pldfr->pay_ac_no,strlen(lv_xmh.ac_no)) )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"01",2);
        sprintf(pldfr->re_content2,"�˺Ų�����");
        vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
        return 1;
    }
    /*�������*/
    trim(lv_xmh.name);
    trim(pldfr->pay_name);
    memcpy(pldfr->pay_name,(char*)DBC2SBC(pldfr->pay_name,sizeof(pldfr->pay_name)-1),sizeof(pldfr->pay_name)-1);/*ȫ���ת�� add by ChengGX 20100603*/
    if ( memcmp(lv_xmh.name,pldfr->pay_name,strlen(lv_xmh.name)) )
    {
        memcpy(pldfr->sts,DSF_FAIL,2);
        memcpy(pldfr->repstat2,"02",2);
        sprintf(pldfr->re_content2,"�˺š���������");
        return 2;
    }
    return 0; 
}

/*��������ѷ�Ϊ��λ�Ľ��ĳɴ�С�����*/
int  fen2yan(char *famt,char *yamt)
{
    /*���ֵĲ���*/
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
    
    /*���ݳ�ʼ��*/
    memset(cOrbrno  ,0, sizeof(cOrbrno));
    memset(cWt_date ,0, sizeof(cWt_date));
    memset(cAvbal   ,0, sizeof(cAvbal));
    memset(actno    ,0, sizeof(actno));
    memset(cQsbrno  ,0, sizeof(cQsbrno));
    memset(cTmpBuf  ,0, sizeof(cTmpBuf));
    memset(&hv_uniontab,0,sizeof(hv_uniontab));
    memcpy(ds->cash_name,(char*)DBC2SBC(ds->cash_name,sizeof(ds->cash_name)-1),sizeof(ds->cash_name)-1);/**ȫ���ת��add by ChengGX 20100603**/
    iRc = get_name(ds->cash_ac_no,ds->cash_name,s_avbal);
    vtcp_log("[%s][%d] iRc=[%d] ���Ϊ=[%f]",__FILE__,__LINE__,iRc,s_avbal);
    if(iRc == -1)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"01",2);
        sprintf(ds->re_content2,"�˺Ų�����");
        vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
        return 1;
    }
    else if (iRc==-2)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"02",2);
        memcpy(ds->re_content2,"�˺š���������",14);
        return 2;
    }
    else if (iRc==-3)
    {
        memcpy(ds->sts,DSF_FAIL,2);
        memcpy(ds->repstat2,"11",2);
        memcpy(ds->re_content2,"�˺�״̬����",12);
        return 3;
    }
    vtcp_log("[%s][%d]������\n",__FILE__,__LINE__);
    
    memcpy(ds->sts,DSF_SUCC,2);
    memcpy(ds->repstat2,"00",2);
    memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-5-20  ��ˮ*/
    memcpy(ds->re_content2,"�ɹ���ִ",8);
    /*�����˺���ͬ��,ȡ���Э���*/
    
    memcpy(actno,ds->cash_ac_no,sizeof(ds->cash_ac_no)-1);
    memcpy(cQsbrno,ds->pay_qs_no,sizeof(cQsbrno)-1);
    vtcp_log("[%s][%d]cQsbrno=[%s]",__FILE__,__LINE__,cQsbrno);
    trim(actno);
    strcat(actno,"%");
    vtcp_log("[%s][%d]Ҫ��ѯ���˺�Ϊ[%s]\n",__FILE__,__LINE__,actno);

    Lv_xmh_Dec_Sel(g_pub_tx.reply,"ac_no like '%s' and xm_ind='1' and o_qs_no='%s'",actno,cQsbrno);
    while(1)
    {
        memset(&lv_xmh,0,sizeof(lv_xmh));

        g_reply_int = Lv_xmh_Fet_Sel( &lv_xmh,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]DEBUG:���յ�ʱ��û�о�����һ����Ŀ��\n",__FILE__,__LINE__);
            memcpy(ds->sts,DSF_REJE,2);
            memcpy(ds->repstat2,"01",2);
            sprintf(ds->re_content2,"�˺Ų�����");
            break;
        }
        vtcp_log("[%s][%d],lv_xmh.xmno=[%s]\n",__FILE__,__LINE__,lv_xmh.xmno);
        memcpy(ds->xyno,lv_xmh.xmno,sizeof(lv_xmh.xmno)-1);

        memcpy(ds->sts,DSF_SUCC,2);
        memcpy(ds->repstat2,"00",2);
        memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-6-03  ��ˮ*/
        memcpy(ds->re_content2,"�ɹ���ִ",8);
        return 0;
    }
    Lv_xmh_Clo_Sel();
    vtcp_log("[%s][%d]xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
    /*���û��,����һ��,Э��Ű�����+�˺�������*/
    if (lv_xmh.xmno[0]=='\0' || lv_xmh.xmno[0]==' ')    /*Ϊ�ջ��ǿո���û��ȡ��*/
    {
        memset(&lv_xmh,0,sizeof(&lv_xmh));
        apitoa(ds->wt_date,sizeof(cWt_date)-1,cWt_date);
        vtcp_log("[%s][%d]cWt_date=[%s]",__FILE__,__LINE__,cWt_date);
        memcpy(cTmpBuf,cWt_date,sizeof(cWt_date)-1);
        strcat(cTmpBuf,ds->cash_ac_no);
        strcat(cTmpBuf,ds->pay_qs_no);/*mod by ChengGX 20100607 ��ˮ  ͬһ�����а��������ϸ*/
        strcat(cTmpBuf,ds->packid);/**Ϊ�˷�ֹһ�����˼���ͬһ�ʻ���ͬ�����е�����**/
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
        vtcp_log("[%s][%d] ����ԱΪ=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
        apitoa(g_pub_tx.tx_date,sizeof(lv_xmh.last_date)-1,lv_xmh.last_date);
        
        memcpy(lv_xmh.brf , "�������Ӵ���Э���" , sizeof(lv_xmh.brf)-1);
        
        /*********������Ҫ���⴦��һ��,�����ʺ�ȡ����*************/
        g_reply_int = get_name(lv_xmh.ac_no,lv_xmh.name,cAvbal);
        if(g_reply_int )
        {
            memcpy(ds->sts,DSF_FAIL,2);
            memcpy(ds->repstat2,"01",2);
            sprintf(ds->re_content2,"�˺Ų�����");
            vtcp_log("[%s][%d]�Ѿ���������",__FILE__,__LINE__);
            return 1;
        }
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        
        /**���������xmno�е�o_qs_no**/
        memcpy(cOrbrno,ds->pay_qs_no,sizeof(cOrbrno)-1);
        vtcp_log("[%s][%d]cOrbrno==[%s]",__FILE__,__LINE__,cOrbrno);

        g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
            return(99);
        }
        memcpy(lv_xmh.o_qs_no,hv_uniontab.qs_no,sizeof(lv_xmh.o_qs_no)-1);
        strcpy(lv_xmh.br_no, QS_BR_NO ); /**���︳ֵ��������**/
        vtcp_log("[%s][%d]lv_xmh.xmno=[%s]",__FILE__,__LINE__,lv_xmh.xmno);
        vtcp_log("[%s][%d]lv_xmh.ac_no=[%s]",__FILE__,__LINE__,lv_xmh.ac_no);
        vtcp_log("[%s][%d]lv_xmh.xm_ind=[%s]",__FILE__,__LINE__,lv_xmh.xm_ind);
        vtcp_log("[%s][%d]lv_xmh.flg=[%s]",__FILE__,__LINE__,lv_xmh.flg);
        vtcp_log("[%s][%d]lv_xmh.name=[%s]",__FILE__,__LINE__,lv_xmh.name);
        vtcp_log("[%s][%d]lv_xmh.o_qs_no=[%s]",__FILE__,__LINE__,lv_xmh.o_qs_no);
        g_reply_int = Lv_xmh_Ins(lv_xmh ,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]������Ŀ�����code==[%d]\n",__FILE__,__LINE__,g_reply_int);
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
        memset(&ds->re_content2,0x00,sizeof(ds->re_content2));/*add by ChengGX 2010-6-03  ��ˮ*/
        memcpy(ds->re_content2,"�ɹ���ִ",8);
        return 0;
    }
    vtcp_log("[%s][%d]ȡЭ����Ѿ����\n",__FILE__,__LINE__);
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
            vtcp_log("[%s][%d]ԭ���������Ѿ�ȡ��sqlcode[%d]",__FILE__,__LINE__,g_reply_int);
            break;
        }
        /*����Ѿ��ظ���40504,�ͻظ�ʧ��*/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,ds.repstat);
        if ( isdigit(ds.repstat[0]) || memcmp(ds.repstat,"AA",2)==0)
        {
            memcpy(cx->sts,DSF_FAIL,2);
            memcpy(cx->repstat2,"90",2);
            strcpy(cx->re_content2,"�������Ѿ��ظ�");
            return 90;
        }
        vtcp_log("[%s][%d] here !!",__FILE__,__LINE__);
        memcpy(cx->sts,DSF_CONC,2);
        memcpy(cx->repstat2,"90",2);
        strcpy(cx->re_content2,"�ȴ�����ԭ����");
        vtcp_log("[%s][%d] here !!",__FILE__,__LINE__);
        /*����ԭ����Ϊ�ѳ���*/
        memcpy(ds.sts,"0C",2);
        g_reply_int = Lv_pldsr_Upd_Upd(ds , g_pub_tx.reply);
        if (g_reply_int)
        {
            vtcp_log("%s,%Upd lv_pldsr�����!!",__FILE__,__LINE__);
            return -1;
        }
        vtcp_log("[%s][%d] cx->sts=[%s]!",__FILE__,__LINE__,cx->sts);
        return 0;
    }
    Lv_pldsr_Clo_Upd();
    vtcp_log("[%s][%d] cx->repstat2=[%s]!!",__FILE__,__LINE__,cx->repstat2);
    memcpy(cx->sts,DSF_FAIL,2);
    memcpy(cx->repstat2,"90",2);
    strcpy(cx->re_content2,"�Ҳ���ԭ����");
    return 90;
}
/*********************************************************
������������������40507��ִ֮��Ļ�ִ�Ļ�ִ
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

    vtcp_log("[%s][%d]׼���α���!!",__FILE__,__LINE__);
    g_reply_int = Lv_dsfsendlist_Dec_Upd(g_pub_tx.reply,"orderno='%s' and wt_date=%ld and tx_type='%s' \
                                        and or_br_no='%s' ",dsl.orderno,dsl.wt_date,dsl.tx_type,dsl.or_br_no);   
    if(g_reply_int)
    {
         vtcp_log("[%s][%d],����ԭ���׳���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]����ԭ���׳���[%d]",__FL__,g_reply_int);
         /****NEWYX BEGIN:�Ҳ���ԭ���ײ�����****/
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
         /****NEWYX END:�Ҳ���ԭ���ײ�����****/
    }
    g_reply_int = Lv_dsfsendlist_Fet_Upd( &dsl,g_pub_tx.reply );
    if (g_reply_int!=100 && g_reply_int!=0)
    {
        sprintf( acErrMsg,"[%s][%d]����ԭ���׳���[%d]",__FL__,g_reply_int);
        sprintf(acErrMsg,"����ԭ���׳���[%s][%d]",__FILE__,__LINE__);
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
            g_reply_int  = Lv_dsfsendlist_Ins(lv_dsfsendlist,g_pub_tx.reply ); /*��ֵ����!!*/
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],������ո����˲���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
                sprintf( acErrMsg,"[%s][%d]������ո����˲���sqlcode=[%d]",__FL__,g_reply_int);
                set_zd_data(DC_GET_MSG,acErrMsg);
                return(-1);
            }
            return (0);
        }
    }

    g_reply_int = Lv_dsfsendlist_Upd_Upd(dsl, g_pub_tx.reply);
    if (g_reply_int)
    {
        vtcp_log("%s,%Upd lv_dsfsendlist�����[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"lv_dsfsendlist�����[%s][%d]",__FILE__,__LINE__);
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
          vtcp_log("[%s][%d],����lv_pldfr��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
          sprintf(acErrMsg,"����lv_pldfr��ʧ��[%s][%d]",__FILE__,__LINE__);
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
          vtcp_log("[%s][%d],����lv_pldsr��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
          sprintf(acErrMsg,"����lv_pldsr��ʧ��[%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,acErrMsg);
          return(g_reply_int);
        }      
    }

    return 0;
}

static int lv_40506_df(struct lv_pldfr_c *pldfr,int ret)
{
    /*���ڲ���Ҫ���������*/
    return 0;
}

static int lv_40506_ds(struct lv_pldsr_c *pldsr,int ret)
{
    pldsr->re_dsf_date = pldsr->dsf_date;
    sprintf(pldsr->repstat2,"%02d",ret);
    if ( ret != 0 )
    {
        sprintf(pldsr->re_content2,"%s","����ͬ�ų���"); 
    }
    return 0;
}

/*������������40503��ִҵ��*/
int lv_dsf_deal40503(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc=0 ,iSocketID=0; 
    struct lv_pldfr_c df;
    struct lv_pldfr_c df2;
    char lvdate[9];
    TLVPACKAGE lv;    /*40503���ذ�*/
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
        vtcp_log("[%s][%d]��ִ����δ��,���ܻ�ִ40503\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]��ִ����δ��,���ܻ�ִ40503",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d]here here !!",__FILE__,__LINE__);
    if (memcmp(repstat,"AA",2)==0)
    {
        vtcp_log("[%s][%d],��ԭ���׺�Ϊ[%s]����Ϊ[%s]\n",__FILE__,__LINE__,orderno,wt_date);
        
        g_reply_int = dsf_dj2df(wt_date,orderno,s_sucnum,s_sucamt);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]�����Զ���ִʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]�����Զ���ִʧ��",__FILE__,__LINE__);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]���Զ���ִ�ɹ�����Ϊ[%s],�ɹ����Ϊ[%s]\n",__FILE__,__LINE__,s_sucnum,s_sucamt);
        
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
    
    memset(df.tx_mx_no,'0',8);    /*ȫ0Ϊȫ��¼*/
    vtcp_log("[%s][%d] packid=[%s][%ld][%s]\n",__FILE__,__LINE__,df.packid,df.wt_date,df.orderno);

    g_reply_int = Lv_pldfr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",df.packid,df.orderno,df.wt_date,df.tx_mx_no);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Dec_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldfr_Fet_Upd(&df,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Fet_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    else
    {   
        /*����Ѿ���Ӧ,ֱ�ӱ���*/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,df.repstat);
        if (isdigit(df.repstat[0]) || memcmp(df.repstat,"AA",2)==0)
        {
            vtcp_log("[%s][%d]�����׼�����Ӧrepstat=[%s]",__FILE__,__LINE__,df.repstat);
            sprintf(errcode,"�����׼�����Ӧ,��Ӧ���Ϊ[%s]",df.repstat);
            return -1;
        }
        /*������ǻ�ִ�ɹ�״̬,ֱ�ӱ���*/
        if (memcmp(df.sts,DSF_SUCC,2)!=0 && memcmp(df.sts,DSF_QSFA,2)!=0)
        {
            vtcp_log("[%s][%d]ҵ��ǰ״̬���ǳɹ���ִ״̬,���ܻظ�40503",__FILE__,__LINE__);
            sprintf(errcode,"ҵ��ǰ״̬���ǳɹ���ִ״̬,���ܻظ�40503[%s]",df.sts);
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
            vtcp_log("[%s[%d]ע������Ľ���Ƕ���[%f]\n",__FILE__,__LINE__,df.succamt);
            if(df.succnum>0)/**�������0˵���гɹ���**/
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
        vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Upd_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    Lv_pldfr_Clo_Upd();
    /*���ļ���*/
    vtcp_log("[%s][%d],Find PLDFR success!\n",__FILE__,__LINE__);
    memcpy(lv.CMTCode,"012",3);     /*ͨ����Ϣҵ���*/
    lv.opeType = '1';               /*������С���*/
    lv.pkgType = '1';               /*������PKG��*/
    memcpy(lv.CMTNo,"005",3);       /*ҵ��Ҫ�ؼ�*/
    lv_set_tag(&lv,"0BG","40503");  /*ҵ�����ͺ�*/
    lv_set_tag(&lv,"52A",df.br_no);    
    lv_set_tag(&lv,"58A",df.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
    lv_get_dsf40506_orderno(&lv,df.orderno);
    vtcp_log("[%s][%d]df.orderno==[%s]",__FILE__,__LINE__,df.orderno);
    sprintf(temp,"%08d",130+117*df.cash_num);
    vtcp_log("[%s][%d]���ǳ��Ȱ�temp==[%s]",__FILE__,__LINE__,temp);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    lv_set_tag(&lv,"B40",temp);    /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
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
    vtcp_log("[%s][%d]�����ܽ��=[%s]",__FILE__,__LINE__,cTotamt);
    sprintf(temp,"%15s",cTotamt);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]cTotamt=[%s]\n",__FILE__,__LINE__,cTotamt);
    
    sprintf(cSuccamt,"%.2f",df.succamt);
    del_dot(cSuccamt,cSuccamt,15);
    vtcp_log("[%s][%d]�ɹ��ܽ��=[%s]",__FILE__,__LINE__,cSuccamt);
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
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldfr_Dec_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    while(1)
    {
        memset(&df2,0,sizeof(df2));
        vtcp_log("[%s][%d]׼����ʼfet����df2��",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Fet_Upd(&df2,g_pub_tx.reply);
        if(g_reply_int == 100)
        {
            vtcp_log("[%s][%d]��ˮ�������Ѿ�ȡ��sqlcode[%d].\n",__FILE__,__LINE__,g_reply_int);
            sprintf(errcode,"[%s][%d]��ˮ�������Ѿ�ȡ��",__FILE__,__LINE__);
            break;
        }
        else if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldfr_Fet_Upd��ʧ��",__FILE__,__LINE__);
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
            vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldfr_Upd_Upd��ʧ��",__FILE__,__LINE__);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]���ｫҪ������ϸ!",__FILE__,__LINE__);
        /*����ϸ*/
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
        vtcp_log("[%s][%d]�ֵ�,������ɣ���",__FILE__,__LINE__);
    }
    Lv_pldfr_Clo_Upd();
    vtcp_log("[%s][%d]׼��ƴ����",__FILE__,__LINE__);
    /*ƴ��*/
    /*��ͷ��Ϣ*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");

    /* mod by weiz �ж�һ��ȡ�к��Ƿ���ȷ*/

    if (hvqsbrno(&lv,df.br_no)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]ȡ�кų���",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,df.or_br_no)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]ȡ�кų���",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    vtcp_log("[%s][%d],lvdate=[%s]",__FILE__,__LINE__,lvdate);
    lv_get_dsf40506_packid(&lv,df.packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40503");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40503��ִ");
    lv_pkg_batch(&lv,"72D",temp);
    vtcp_log("[%s][%d]------------------------------����40503��ִ----------------------------",__FILE__,__LINE__);
    
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    /*����*/
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],д�����Ǽǲ���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д�����Ǽǲ���",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],д���ո�������\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д���ո�������",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d],genpack success!\n",__FILE__,__LINE__);
    /*����*/
    DSFSEND(&lv);
    vtcp_log("[%s][%d],sendpack success!\n",__FILE__,__LINE__);
    lv_destroy(&lv);

    return 0;
}

/* ����������ڴ�lv_pldfr���ҳ����׺ŵ�lv_dqdjs�ҵ���Ӧ�Ľ������ó���� */
int  dsf_dj2df(char *s_wt_date,char *s_orderno,char *s_sucnum,char *s_sucamt)
{
    struct lv_pkgreg_c tx; /*��tx,���Ժܿ������ĺ�������ͳһ  */
    struct lv_pldfr_c dsf; /*ͬ��*/
    struct lv_pldfr_c dsf2; /*ͬ��*/
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
        
    vtcp_log("[%s][%d],��ԭ���׺�Ϊ[%s]����Ϊ[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    g_reply_int = Lv_pldfr_Dec_Sel(g_pub_tx.reply,"wt_date=%s and orderno='%s' and tx_mx_no<>'00000000'",\
                                   s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldfr_Dec_Sel��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"Lv_pldfr_Dec_Sel��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]׼��ѭ��",__FILE__,__LINE__);
    while (1)
    {
        memset(&dsf,0,sizeof(dsf));
        g_reply_int = Lv_pldfr_Fet_Sel(&dsf,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Sel��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Fet_Sel��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            break;
        } 
        memset(djstr,0,sizeof(djstr));
        sprintf(djstr,"%ld%s%s",dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        vtcp_log("[%s][%d]djstr=[%s]",__FILE__,__LINE__,djstr);
        g_reply_int = iDsf_gettx(&tx,dsf.wt_date,dsf.orderno,dsf.tx_mx_no,cPkgno);
        if(g_reply_int)
        {
            vtcp_log("�Ҳ�����Ӧ�Ķ��ڴ���ҵ��[%ld][%s][%s][%s]",dsf.wt_date,dsf.orderno,dsf.tx_mx_no,cPkgno);
            Lv_pldfr_Clo_Sel();
            return -1;
        }
        vtcp_log("[%s][%d]׼��update",__FILE__,__LINE__);
        vtcp_log("[%s][%d]dsf.packid=[%s]",__FILE__,__LINE__,dsf.packid);
        vtcp_log("[%s][%d]dsf.orderno=[%s]",__FILE__,__LINE__,dsf.orderno);
        vtcp_log("[%s][%d]dsf.wt_date=[%ld]",__FILE__,__LINE__,dsf.wt_date);
        vtcp_log("[%s][%d]dsf.tx_mx_no=[%s]",__FILE__,__LINE__,dsf.tx_mx_no);
        g_reply_int = Lv_pldfr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",dsf.packid,dsf.orderno,dsf.wt_date,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Dec_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]FETһ����¼",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Fet_Upd(&dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Fet_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        else
        {
            vtcp_log("[%s][%d]׼���õ������!!",__FILE__,__LINE__);
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
        vtcp_log("[%s][%d]UPDһ����¼",__FILE__,__LINE__);
        g_reply_int = Lv_pldfr_Upd_Upd(dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldfr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldfr_Upd_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
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
    vtcp_log("[%s][%d],��ԭ���׺�Ϊ[%s]����Ϊ[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    vtcp_log("[%s][%d],��ԭ���׺�Ϊ[%s]����Ϊ[%s]\n",__FILE__,__LINE__,s_orderno,s_wt_date);
    g_reply_int = sql_execute("update lv_pldfr set succamt='%f',succnum='%ld' where \
                               wt_date=%s and orderno ='%s' and tx_mx_no='00000000'",\
                               dSucAmt,iCount,s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],����lv_pldfr��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"����lv_pldfr��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]SUCCESS",__FILE__,__LINE__);
    return 0;
}
/*ȡ�ö�Ӧ��ҵ��*/
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
        vtcp_log("[%s][%d],��ѯLv_addit��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"��ѯLv_addit��ʧ��[%s][%d]",__FILE__,__LINE__);
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
        vtcp_log("[%s][%d],����lv_pkgreg��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"����lv_pkgreg��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    while (1)
    {
        memset(&sLv_pkgreg2,0,sizeof(struct lv_pkgreg_c));

        g_reply_int = Lv_pkgreg_Fet_Sel(&sLv_pkgreg2, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],ȡlv_pkgreg��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"ȡlv_pkgreg��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]�ɹ�ȡ�÷����Ľ���\n",__FILE__,__LINE__);
        memcpy(lv_pkgreg,&sLv_pkgreg2,sizeof(sLv_pkgreg2));
        break;
    }
    Lv_pkgreg_Clo_Sel();
    vtcp_log("[%s][%d]sLv_pkgreg2.pkgno=[%s]",__FILE__,__LINE__,sLv_pkgreg2.pkgno);
    return 0;
}

/*�������˵����
�Ѷ��ڴ���ҵ���ÿ��С���״̬׼ȷӦ��,����0��ʾ���׳ɹ�,-1��ʾʧ��*/
int dsf_dqdjsts2dfsucc(char *txs,char *dsfs,char *reps,char *repc)
{
    switch(*txs){
        case  STAT_INVALID      : /*��Ч״̬*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��Ч״̬",8);
            return -1;
        case  STAT_WZLR          : /*����¼��*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"����¼��",8);
            return -1;
        case  STAT_WZCHECK1      : /*���˸���,���� --��Ч,20051130 by lhf*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"���˸���,����",13);
            return -1;
        case  STAT_WZCHECK2      : /*���˸���,����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"���˸���,����",13);
            return -1;
        case  STAT_WZSEND      : /*���˷���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"���˷���",8);
            return -1;
        case  STAT_WZREJECT      : /*���˾ܾ�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"���˾ܾ�",8);
            return -1;
        case  STAT_WZSUCCESS  : /*��������*/
            memcpy(dsfs,DSF_QSSU,2);
            memcpy(reps,"00",2);
            strncpy(repc,"��������",8);
            return 0;
        case  STAT_WZINQUEUE  : /*�����Ŷ�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����Ŷ�",8);
            return -1;
        case  STAT_WZCANCEL_WAIT:    /*���˳����ȴ�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"���˳����ȴ�",12);
            return -1;
        case  STAT_WZCANCEL        :        /*���˳���*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"���˳���",8);
            return -1;
        case  STAT_WZBACK_WAIT  :          /*�����˻ء�������ֹ���ȴ�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����˻�,����,ֹ���ȴ�",22);
            return -1;
        case  STAT_WZBACK        :         /*�����˻�*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"�����˻�",8);
            return -1;
        case  STAT_WZCZ            :         /*�����ѳ���*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"�����ѳ���",10);
            return -1;
        case  STAT_WZZF            :         /*������ֹ��*/
            memcpy(dsfs,DSF_CONC,2);
            memcpy(reps,"20",2);
            strncpy(repc,"������ֹ��",10);
            return -1;
        case  STAT_WZEXCEPT        :         /*�����쳣����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����쳣����",12);
            return -1;
        case  STAT_LZRECV1        :         /*��ʱ����,������*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��ʱ����",8);
            return -1;
        case  STAT_LZRECV2        :         /*��ʽ����,��������*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��ʽ����,��������",17);
            return -1;
        case  STAT_LZNMAC_ERR   :           /*ȫ��Ѻ��*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"ȫ��Ѻ��",8);
            return -1;
        case  STAT_LZLMAC_ERR   :           /*�ط�Ѻ��*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�ط�Ѻ��",8);
            return -1;
        case  STAT_LZAMAC_ERR   :           /*ȫ��Ѻ�͵ط�Ѻ����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"ȫ��Ѻ�͵ط�Ѻ����",18);
            return -1;
        case  STAT_LZGZ_QS        :         /*��Ϣ����,�������Ĺ���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��Ϣ����,�������Ĺ���",21);
            return -1;
        case  STAT_LZGZ_BR        :         /*��Ϣ����,֧�й���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��Ϣ����,֧�й���",17);
            return -1;
        case  STAT_LZBACK_WAIT  :       /*�����˻صȴ�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����˻صȴ�",12);
            return -1;
        case  STAT_LZBACK       :       /*�����˻�*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����˻�",8);
            return -1;
        case  STAT_YES_NOTSEND  :       /*����ͬ��Ӧ����δ����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"����ͬ��Ӧ����δ����",20);
            return -1;
        case  STAT_YES_SEND     :       /*����ͬ��Ӧ�����ѷ���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"����ͬ��Ӧ�����ѷ���",20);
            return -1;
        case  STAT_NO_NOTSEND   :       /*������ͬ��Ӧ����δ����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"������ͬ��Ӧ��δ����",22);
            return -1;
        case  STAT_NO_SEND      :       /*������ͬ��Ӧ�����ѷ���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"������ͬ��Ӧ�����ѷ���",22);
            return -1;
        case  STAT_LZCZ         :       /*�����ѳ���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����ѳ���",10);
            return -1;
        case  STAT_LZZF         :       /*������ֹ��*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"������ֹ��",10);
            return -1;
        case  STAT_LZFK         :       /*�����Ѹ���*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����Ѹ���",10);
            return -1;
        case  STAT_LZEXCEPT     :       /*�����쳣����*/
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"�����쳣����",12);
            return -1;
        default:
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"δ֪����,����ϵ����Ա",21);
            return -1;
    }
    return 0;
}

/*�������˵����
�Ѷ��ڽ�ǻ�ִҵ���ÿ��С���״̬׼ȷӦ��,����0��ʾ���׳ɹ�,-1��ʾʧ��*/
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
            strncpy(repc,"��ִ�ɹ�",8);
            return 0;
        case  1          :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"01",2);
            strncpy(repc,"�˺Ų�����",8);
            return -1;
        case  2      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"02",2);
            strncpy(repc,"�˺�,��������",13);
            return -1;
        case  3      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"03",2);
            strncpy(repc,"�˻�����֧��",16);
            return -1;
        case  10      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"10",2);
            strncpy(repc,"�˻��������",12);
            return -1;
        case  11      :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"11",2);
            strncpy(repc,"�˻�״̬����",12);
            return -1;
        case  20  :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"20",2);
            strncpy(repc,"ҵ���ѳ���",10);
            return -1;
        case  90  :
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"��������",8);
            return -1;
        default:
            memcpy(dsfs,DSF_QSFA,2);
            memcpy(reps,"90",2);
            strncpy(repc,"δ֪����,����ϵ����Ա",21);
            return -1;
    }
    return -1;
}
/*ȡ�ö�Ӧ���ڽ�ǻ�ִ��ҵ��*/
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
        vtcp_log("[%s][%d],��ѯLv_addit��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"��ѯLv_addit��ʧ��[%s][%d]",__FILE__,__LINE__);
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
        vtcp_log("[%s][%d],����lv_pkgreg��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"����lv_pkgreg��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    while (1)
    {
        memset(&sLv_pkgreg2,0,sizeof(struct lv_pkgreg_c));

        g_reply_int = Lv_pkgreg_Fet_Sel(&sLv_pkgreg2, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],ȡlv_pkgreg��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"ȡlv_pkgreg��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        vtcp_log("[%s][%d]�ɹ�ȡ�÷����Ľ���\n",__FILE__,__LINE__);
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
    TLVPACKAGE return40506Pack;    /*��������40506��*/
    struct lv_wbctl_c lv_wbctl;
    char pcPackday[9];
    char pcPackid[9];
    char cTmporderno[9];
    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    /*********���ݳ�ʼ��******************/
    memset(&lv_wbctl,'\0',sizeof(lv_wbctl));

    memset(pcPackday,'\0',sizeof(pcPackday));
    memset(pcPackid,'\0',sizeof(pcPackid));
    memset(cTmporderno, 0 , sizeof(cTmporderno));
    
    lv_pkg_getHeaderTag(wp_lvpack,"30E",pcPackday);
    lv_pkg_getHeaderTag(wp_lvpack,"0BD",cTmporderno);
    lv_orderno_add0(cTmporderno, sizeof(pcPackid)-1, pcPackid);
    vtcp_log("[%s][%d],pcPackday=[%s]",__FILE__,__LINE__,pcPackday);
    /******�����������Ʊ�************/
    iRc=lv_setwbctl_from_pack(wp_lvpack,&lv_wbctl);
    if(iRc)
    {
        vtcp_log("[%s][%d],�����������Ʊ��¼��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"�����������Ʊ��¼��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }

    vtcp_log("[%s][%d]",__FILE__,__LINE__);
    g_reply_int  = Lv_wbctl_Ins(lv_wbctl,g_pub_tx.reply );
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],�����������Ʊ��sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"[%s][%d]�����������Ʊ��code=[%d]",__FL__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d],����������ɹ�\n",__FILE__,__LINE__);
    return(0);
}

static int lv_setwbctl_from_pack(TLVPACKAGE *wp_lvpack, struct lv_wbctl_c *lv_wbctl)
{
    char caValue[61];
    char cPackday[9];
    memset(cPackday,0,sizeof(cPackday));
    memset(caValue,0 , sizeof(caValue));
    memset(lv_wbctl,'\0',sizeof(struct lv_wbctl_c));    
    GETHEADTAG(wp_lvpack,"30E",cPackday);/**��ί������**/
    lv_wbctl->pack_date = apatoi(cPackday,8);
    memset(caValue,'\0',sizeof(caValue));
    GETHEADTAG(wp_lvpack,"0BD",caValue);/**�����**/
    lv_orderno_add0(caValue, sizeof(lv_wbctl->packid)-1, lv_wbctl->packid);
    vtcp_log("[%s][%d],pack_date=[%d]",__FILE__,__LINE__,lv_wbctl->pack_date);
    vtcp_log("[%s][%d],packid====[%s]",__FILE__,__LINE__,lv_wbctl->packid);
    GETHEADTAG(wp_lvpack,"02C",lv_wbctl->pkgno);/**�����ͺ�**/
    GETHEADTAG(wp_lvpack,"C36",lv_wbctl->pay_qs_no);/*����������*/
    GETHEADTAG(wp_lvpack,"C37",lv_wbctl->cash_qs_no);/*����������*/
    /****����ϵͳ����****/
    lv_wbctl->in_date = apatoi(pcLvGetDay(),8);
    /***ҵ���ܱ���****/ 
    memset(caValue,'\0',sizeof(caValue));
    GETHEADTAG(wp_lvpack,"B42",caValue);
    lv_wbctl->dtlcnt=atoi(caValue);
    
    /****ԭ�����ͺ�*****/
    GETHEADTAG(wp_lvpack,"02D",lv_wbctl->o_pkgno);
    /****����ڵ��,ͨ����Ϣ��****/
    GETHEADTAG(wp_lvpack,"C36",lv_wbctl->ornode);
    /****���սڵ��,ͨ����Ϣ��****/
    GETHEADTAG(wp_lvpack,"C37",lv_wbctl->acnode);
    /****Ͻ�ڷַ���־****/
    GETHEADTAG(wp_lvpack,"BS7",lv_wbctl->ffflag);
    /****ҵ�����ͺ�***/
    GETHEADTAG(wp_lvpack,"0BG",lv_wbctl->txnum);
    /****��״̬,�Զ���*****/
    lv_wbctl->stat[0]=PSTAT_SEND;
    /****��ӡ��־,����****/
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
��������ѷ����Ļ�ִ������һ������,�Դ����ִ�Ļ�ִ,��40503,40504,40506����ǰ���á�
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

    /*���,ȡ�ð��źͳ�����Ϣ*/
    lv_pkg_getHeaderTag(lv,"0BD",dsl.packid);
    vtcp_log("[%s][%d]����İ������[%s]",__FILE__,__LINE__,dsl.packid);
    lv->tradeStartP=0;
    recnt = lv_count_recnt(lv);
    for(;recnt>0;recnt--)
    {
        iRc = lv_fetchpkg(lv,&sublv);
        if (iRc!=0)
        {
            vtcp_log("[%s][%d],ȡ����[%d]\n",__FILE__,__LINE__,iRc);
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
            /*������������*/
            vtcp_log("[%s][%d],�����ͺ�Ϊ%s\n",__FILE__,__LINE__,dsl.tx_type);
            return -1;
        }
        
        memcpy(dsl.sts,DSF_SEND,2);
        strcpy(dsl.content,"��ִ�ѷ���");

        g_reply_int  = Lv_dsfsendlist_Ins(dsl,g_pub_tx.reply ); /*��ֵ����!!*/
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],������ո����˲���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"[%s][%d]������ո����˲���sqlcode=[%d]",__FL__,g_reply_int);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(-1);
        }
        lv_reset(&sublv);
    }

    lv_destroy(&sublv);
    return 0;
}

/*�������ȡ�ö�Ӧ��������*/
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
        vtcp_log("[%s][%d],��ѯ�����кŴ���,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"��ѯ�����кŴ���",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    lv_pkg_batch(lv,"C37",hut.qs_no);
    vtcp_log("[%s][%d],sendco=[%s]\n",__FILE__,__LINE__,hut.sendco);
    return 0;
}
/*�������ȡ�ö�Ӧ��������*/
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
        vtcp_log("[%s][%d],��ѯ�����кŴ���,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"��ѯ�����кŴ���",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d]qs_no==[%s]",__FILE__,__LINE__,hut.qs_no);
    lv_pkg_batch(lv,"C36",hut.qs_no);
    return 0;
}
int lv_get_dsf40506_packid(TLVPACKAGE *lv,char *packid)
{
    /*�ֲ�֪����ʲô�ط�ȡ��,���Ծ���ԭ��������ǰ��Ӹ�40506*/
    int ipackid=0;
    ipackid = iLvGetPackid();
    sprintf(packid,"%08d",ipackid);
    vtcp_log("[%s][%d]�õ�packid=[%s]\n",__FILE__,__LINE__,packid);
    /*��wd_lv_wbctl�в���һ����¼����ѯ*/
    lv_pkg_batch(lv,"0BD",packid);
    vtcp_log("[%s][%d]����İ������[%s]",__FILE__,__LINE__,packid);
    return 0;
}

int lv_get_dsf40506_orderno(TLVPACKAGE *lv,char *orderno)
{
    /*�ֲ�֪����ʲô�ط�ȡ��,���Ծ���ԭ��������ǰ��Ӹ�40506*/
    int i_orderno=0;
    i_orderno = dsf_iLvGetOrderno();
    sprintf(orderno,"%08d",i_orderno);
    vtcp_log("[%s][%d]�õ�orderno=[%s]\n",__FILE__,__LINE__,orderno);
    /*��wd_lv_wbctl�в���һ����¼����ѯ*/
    lv_set_tag(lv,"0BC",orderno);
    
vtcp_log("[%s][%d]",__FILE__,__LINE__);
  return 0;
}
/*���ո��Ľ�����Ŷ�����������ȡ,����Ҳ�����������ķ�*/
int dsf_iLvGetOrderno() 
{
    char cBrno[BRNO_LEN+1];

    memset(cBrno, 0x0, sizeof(cBrno));
    memcpy(cBrno,QS_BR_NO,sizeof(BRNO_LEN)+1);
    vtcp_log("[%s][%d]cBrno=[%s]",__FILE__,__LINE__,cBrno);
    return iLvGetSeqno(cBrno, LV_ACTYPE);/**��С��һ�����**/
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
        strcpy(errcode,"���˽���");
        return 0;
    }else if ( memcmp(stat,"1B",2)==0 ){
        strcpy(errcode,"���˾ܾ�");
        return 0;
    }else if ( memcmp(stat,"0C",2)==0 ){
        strcpy(errcode,"���˳���");
        return 0;
    }else if ( memcmp(stat,"0D",2)==0 ){
        strcpy(errcode,"�ɹ���ִ40506");
        return 0;
    }else if ( memcmp(stat,"1E",2)==0 ){
        strcpy(errcode,"ʧ�ܻ�ִ40506");
        return 0;
    }else if ( memcmp(stat,"0F",2)==0 ){
        strcpy(errcode,"�ɹ���ִ40503/4");
        return 0;
    }else if ( memcmp(stat,"0G",2)==0 ){
        strcpy(errcode,"ʧ�ܻ�ִ40503/4");
        return 0;
    }else if ( memcmp(stat,"0H",2)==0 ){
        strcpy(errcode,"���˷���");
        return 0;
    }else if ( memcmp(stat,"1I",2)==0 ){
        strcpy(errcode,"���˾ܾ�");
        return 0;
    }else if ( memcmp(stat,"0J",2)==0 ){
        strcpy(errcode,"��������");
        return 0;
    }else if ( memcmp(stat,"1K",2)==0 ){
        strcpy(errcode,"����ʧ��");
        return 0;
    }else{
        strcpy(errcode,"δ֪״̬,����ϵ����Ա");
        return 0;
    }   
    return 0;
}

/*������������40504��ִҵ��*/
int lv_dsf_deal40504(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc=0 ,iSocketID=0,i=0; 
    struct lv_pldsr_c ds;
    struct lv_pldsr_c ds2;
    char lvdate[9];
    TLVPACKAGE lv;    /*40504���ذ�*/
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
            vtcp_log("[%s][%d],�����Զ���ִʧ��\n",__FILE__,__LINE__);
            sprintf(errcode,"�����Զ���ִʧ��!");
            return -1;
        }
        vtcp_log("[%s][%d]�Զ���ִ�ɹ�����Ϊ[%s],�ɹ����Ϊ[%s]\n",__FILE__,__LINE__,s_sucnum,s_sucamt);
    }
    memcpy(lvdate,pcLvGetDay_work(),8);
    memset(&ds,0,sizeof(ds));
    memcpy(ds.packid,packid,sizeof(ds.packid)-1);
    ds.wt_date = apatoi(wt_date,8);
    memcpy(ds.orderno,orderno,sizeof(ds.orderno)-1);
    memset(ds.tx_mx_no,'0',8);    /*ȫ0Ϊȫ��¼*/

    g_reply_int = Lv_pldsr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",ds.packid,ds.orderno,ds.wt_date,ds.tx_mx_no);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]dec lv_pldfr��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldsr_Fet_Upd(&ds,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Fet lv_pldfr��ʧ��",__FILE__,__LINE__);
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
                vtcp_log("[%s][%d],��ִ����δ��,���ܻ�ִ40504\n",__FILE__,__LINE__);
                sprintf(errcode,"��ִ����δ��,���ܻ�ִ40504",__FILE__);
                return -1;
        }
        /**����ý����Ѿ�Ӧ�����**/
        vtcp_log("[%s][%d]repstat==[%s]",__FILE__,__LINE__,ds.repstat);
          if (isdigit(ds.repstat[0]) || memcmp(ds.repstat,"AA",2)==0)
        {
                vtcp_log("[%s][%d]�����׼�����Ӧ,��Ӧ���Ϊ[%s]",__FILE__,__LINE__,ds.repstat);
                sprintf(errcode,"�����׼�����Ӧ,��Ӧ���Ϊ[%s]",ds.repstat);
                return -1;
        }
        /*������ǻ�ִ�ɹ�״̬,ֱ�ӱ���*/
        if (memcmp(ds.sts,DSF_SUCC,2)!=0 && memcmp(ds.sts,DSF_CONC,2)!=0 &&memcmp(ds.sts,DSF_QSFA,2)!=0)
        {
            vtcp_log("[%s][%d],ҵ��ǰ״̬���ǳɹ���ִ״̬,���ܻظ�40504\n",__FILE__,__LINE__);
            sprintf(errcode,"[%s][%d]ҵ��ǰ״̬���ǳɹ���ִ״̬,���ܻظ�40504[%s]",__FILE__,__LINE__,ds.sts);
            return -1;
        }
        statof405034(ds.sts,repstat);
        memcpy(ds.repstat,repstat,sizeof(ds.repstat)-1);
        strncpy(ds.re_content,re_content,sizeof(ds.re_content)-1);
        if (memcmp(repstat,"AA",2)==0)
        {
            ds.succnum = apatoi(s_sucnum,8);
            str2dbl(s_sucamt,strlen(s_sucamt),0,&ds.succamt);
            ds.succamt = ds.succamt/100;/*add by ChengGX 20100618 ��ˮ*/
            vtcp_log("[%s[%d]ע������Ľ���Ƕ���[%f]\n",__FILE__,__LINE__,ds.succamt);/*add by ChengGX 20100618 ��ˮ*/
            if(ds.succnum>0)/**�������0˵���гɹ���**/
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
        vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldsr_Upd_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    Lv_pldsr_Clo_Upd();
    /*���ļ���*/
    memcpy(lv.CMTCode,"012",3);     /*ͨ����Ϣҵ���*/
    lv.opeType = '1';               /*������С���*/
    lv.pkgType = '1';               /*������PKG��*/
    memcpy(lv.CMTNo,"005",3);       /*ҵ��Ҫ�ؼ�*/
    lv_set_tag(&lv,"0BG","40504");  /*ҵ�����ͺ�*/
    lv_set_tag(&lv,"52A",ds.br_no);    
    lv_set_tag(&lv,"58A",ds.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
    lv_get_dsf40506_orderno(&lv,ds.orderno);
    sprintf(temp,"%08d",130+117*ds.pay_num);
    lv_set_tag(&lv,"B40",temp);      /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
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
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"Lv_pldsr_Dec_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
        return(g_reply_int);
    }
    while(1)
    {
        vtcp_log("[%s][%d],�����%d�ʽ���\n",__FILE__,__LINE__,++i);
        memset(&ds2,0,sizeof(ds2));

        g_reply_int = Lv_pldsr_Fet_Upd(&ds2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
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
            vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(errcode,"[%s][%d]Lv_pldsr_Upd_Upd��ʧ��",__FILE__,__LINE__);
            return(g_reply_int);
        }
        
        /*����ϸ*/
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
    vtcp_log("[%s][%d],����40506.\n",__FILE__,__LINE__);
    /*ƴ��*/
    /*��ͷ��Ϣ*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");
    if (hvqsbrno(&lv,ds.br_no)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,ds.or_br_no)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]ȡ�кų���",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    lv_get_dsf40506_packid(&lv,ds.packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40504");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40504�Ļ�ִ");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    
    /*����*/
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],д�����Ǽǲ���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д�����Ǽǲ���",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],д���ո�������\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д���ո�������",__FILE__,__LINE__);
        return -1;
    }
    DSFSEND(&lv);
    lv_destroy(&lv);
    /*this only way to asolute this problem*/
    vtcp_log("[%s][%d]�������ɹ�����",__FILE__,__LINE__); 
    return 0;
}

/*����������������ݰ�*/
static int dsf_send(TLVPACKAGE *lv)
{
    int iRc =0,iSocketID;
    vtcp_log("[%s][%d],���ʹ��ո�ҵ���.\n",__FILE__,__LINE__);
    /*����*/
    iRc=lv_initPara(NULL);
    if (iRc)
    {
        vtcp_log("[%s][%d],��ʼ������������� return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"��ʼ�������������[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    /* 2 ��������״̬�İ� */
    iRc = lv_connect(LV_SERVER_BATCH);
    iSocketID = iRc;
    if (iSocketID<=0)
    {
        vtcp_log("[%s][%d],����ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"����ǰ�û�����[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        lv_destroy(lv);
        lv_netClose(iSocketID);
        return(-1);
    }
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    /* 2 ��������״̬�İ� */
    iRc = lv_send(iRc,lv);
    if (iRc)
    {
        vtcp_log("[%s][%d],�������ݰ���ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�������ݰ���ǰ�û�����[%s][%d]",__FILE__,__LINE__);
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
    /*��Ҫ����û���,��Ϊ���и����û��ͱ���ά���ĸ����Ͳ�һ�� */
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

/*����������40506���ݰ�,ÿ��ֻ����һ�����׵Ļ�ִ��*/
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
        /*�͵�һ���������*/
        lv_create(lv);
    }
    memset(lvdate,0,sizeof(lvdate));
    memcpy(lvdate,pcLvGetDay_work(),8);
    vtcp_log("[%s][%d],type=[%s]\n",__FILE__,__LINE__,type);
    /*���*/
    if (!memcmp(type,"40501",5))
    {
        vtcp_log("[%s][%d],Ϊ40501���׻�ִ40506\n",__FILE__,__LINE__);
        /*����д�����Ϣ*/
        df=dsf;
        memcpy(s_o_orderno,df->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*ͨ����Ϣҵ���*/
        lv->opeType = '1';           /*������С���*/
        lv->pkgType = '1';           /*������PKG��*/
        /*��һ��ҵ��*/
        memcpy(lv->CMTNo,"005",3);   /*ҵ��Ҫ�ؼ�*/
        /*����*/
        lv_set_tag(lv,"0BG","40506");  /*ҵ�����ͺ�*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
        lv_get_dsf40506_orderno(lv,df->orderno);
        lv_set_tag(lv,"B40","00000091");    /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
        
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
        
        /*��ͷ��Ϣ*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,df->br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,df->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,df->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40501�Ļ�ִ");
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
        vtcp_log("[%s][%d],Ϊ40502���׻�ִ40506\n",__FILE__,__LINE__);
        /*����д�����Ϣ*/
        ds=dsf;
        memcpy(s_o_orderno,ds->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*ͨ����Ϣҵ���*/
        lv->opeType = '1';           /*������С���*/
        lv->pkgType = '1';           /*������PKG��*/
        /*��һ��ҵ��*/
        memcpy(lv->CMTNo,"005",3);   /*ҵ��Ҫ�ؼ�*/
        /*����*/
        lv_set_tag(lv,"0BG","40506");  /*ҵ�����ͺ�*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
        lv_get_dsf40506_orderno(lv,ds->orderno);
        lv_set_tag(lv,"B40","00000091");    /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
        
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
        
        /*��ͷ��Ϣ*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,ds->br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,ds->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,ds->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40502�Ļ�ִ");
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
        vtcp_log("[%s][%d],Ϊ40505���׻�ִ40506\n",__FILE__,__LINE__);
        /*����д�����Ϣ*/
        dc=dsf;
        memcpy(s_o_orderno,dc->orderno,8);
        memcpy(lv->CMTCode,"012",3); /*��ͨ����ҵ���*/
        lv->opeType = '1';           /*������С���*/
        lv->pkgType = '1';           /*������PKG��*/
        /*��һ��ҵ��*/
        memcpy(lv->CMTNo,"005",3);   /*ҵ��Ҫ�ؼ�*/
        /*����*/
        lv_set_tag(lv,"0BG","40506");  /*ҵ�����ͺ�*/
        lv_movtag(ilv,"58A",lv,"52A");
        lv_movtag(ilv,"52A",lv,"58A");
        lv_set_tag(lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
        lv_get_dsf40506_orderno(lv,dc->orderno);
        lv_set_tag(lv,"B40","00000091");    /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
        
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
        
        /*��ͷ��Ϣ*/
        if (iCount==allCount)
        {
            lv_pkg_batch(lv,"02C","012");
            if (hvqsbrno(lv,dc->br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            if (hvqsorbrno(lv,dc->or_br_no)==-1)
            {
                vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
                return -1;
            }
            lv_pkg_batch(lv,"30E",lvdate);
            lv_get_dsf40506_packid(lv,dc->packid);
            lv_pkg_batch(lv,"BS7","0");
            lv_pkg_batch(lv,"0BG","40506");
            sprintf(temp,"%04d",allCount);
            lv_pkg_batch(lv,"B42",temp);
            sprintf(temp,"%-60s","40505�Ļ�ִ");
            lv_pkg_batch(lv,"72D",temp);
            /* .... */
            memcpy(lv->workDate,lvdate,8);
            lv_gen_pack(lv);
            lv_dsf_wbctl(lv);
        }
        vtcp_log("[%s][%d],lv->workDate= [%8s]\n",__FILE__,__LINE__,lv->workDate);
    }
    
    /*����*/
    if (iCount==allCount)
    {
        vtcp_log("[%s][%d],����40506.\n",__FILE__,__LINE__);
        
        dsf_setsendlist(lv);    
        dsf_send(lv);
        lv_destroy(lv);
    }
    return 0;
}

static int lv_movtag(TLVPACKAGE *ilv,char *tag1,TLVPACKAGE *lv,char *tag2)
{
    char buf[61]; /*ֻ����С����*/ 
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


/* ����������ڴ�lv_pldsr���ҳ����׺ŵ�lv_dqjjs�ҵ���Ӧ�Ľ������ó���� */
int  dsf_jj2ds(char *s_wt_date,char *s_orderno,char *s_sucnum,char *s_sucamt)
{
    struct lv_pkgreg_c tx; /*��tx,���Ժܿ������ĺ�������ͳһ  */
    struct lv_pldsr_c dsf; /*ͬ��*/
    struct lv_pldsr_c dsf2; /*ͬ��*/
    int iCount=0,iRc=0;
    double dSucAmt=0.00;
    char djstr[30];
    
    memset(&dsf,0,sizeof(dsf));
    memset(&dsf2,0,sizeof(dsf2));
    memset(&tx,0,sizeof(tx));
    vtcp_log("[%s][%d] jj2ds ������ʼ�ˣ���",__FILE__,__LINE__);
    g_reply_int = Lv_pldsr_Dec_Sel(g_pub_tx.reply,"wt_date=%s and orderno='%s' and tx_mx_no<>'00000000'",\
                                   s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldsr_Dec_Sel��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"Lv_pldsr_Dec_Sel��ʧ��[%s][%d]",__FILE__,__LINE__);
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
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Sel��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Sel��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            break;
        }
        memset(djstr,0,sizeof(djstr));
        sprintf(djstr,"%ld%s%s",dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        g_reply_int = iDsf_getdqjjhz(&tx,dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]�Ҳ�����Ӧ�Ķ��ڽ��ҵ��[%ld][%s][%s]",
                __FILE__,__LINE__,dsf.wt_date,dsf.orderno,dsf.tx_mx_no);
            Lv_pldsr_Clo_Sel();
            return -1;
        }
        memset(&dsf2,0,sizeof(dsf2));
        vtcp_log("[%s][%d]׼��Upd�α��ˣ���",__FILE__,__LINE__);
        vtcp_log("[%s][%d]dsf.orderno==[%s]",__FILE__,__LINE__,dsf.orderno);
        vtcp_log("[%s][%d]dsf.tx_mx_no==[%s]",__FILE__,__LINE__,dsf.tx_mx_no);
        vtcp_log("[%s][%d]dsf.wt_date==[%ld]",__FILE__,__LINE__,dsf.wt_date);
        vtcp_log("[%s][%d]dsf.packid==[%s]",__FILE__,__LINE__,dsf.packid);
        g_reply_int = Lv_pldsr_Dec_Upd(g_pub_tx.reply,"packid='%s' and  orderno='%s' and \
                      wt_date='%ld' and tx_mx_no='%s' ",dsf.packid,dsf.orderno,dsf.wt_date,dsf.tx_mx_no);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Dec_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        g_reply_int = Lv_pldsr_Fet_Upd(&dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Fet_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        else
        {
            vtcp_log("[%s][%d]������ҵ��ÿ�ʵ�״̬!!!!!!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]tx.lv_sts==[%s],dsfstat[%s]\n",__FILE__,__LINE__,tx.lv_sts,dsf2.sts);
            if (dsf_dqjjhz2dssts(tx.rcpstat,dsf2.sts,dsf2.repstat,dsf2.re_content)==0)
            {
                vtcp_log("[%s][%d]�ɹ��Ľ���txstat[%s],dsfstat[%s]\n",__FILE__,__LINE__,tx.lv_sts,dsf2.sts);
                iCount++;
                dSucAmt += dsf2.tx_amt;
            }
        }
        g_reply_int = Lv_pldsr_Upd_Upd(dsf2,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]Lv_pldsr_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
            sprintf(acErrMsg,"Lv_pldsr_Upd_Upd��ʧ��[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return(g_reply_int);
        }
        Lv_pldsr_Clo_Upd();
        vtcp_log("[%s][%d],�ɹ�����%d,�ɹ����%f\n",__FILE__,__LINE__,iCount,dSucAmt);
    }
    Lv_pldsr_Clo_Sel();
    sprintf(s_sucnum,"%08d",iCount);

    sprintf(s_sucamt,"%.2f",dSucAmt);
    del_dot(s_sucamt,s_sucamt,15);
    
    vtcp_log("[%s][%d],�ܳɹ�����%d,�ܳɹ����%f\n",__FILE__,__LINE__,iCount,dSucAmt);

    g_reply_int = sql_execute("update lv_pldsr set succamt='%f',succnum='%ld' where \
                               wt_date=%s and orderno ='%s' and tx_mx_no='00000000'",\
                               dSucAmt,iCount,s_wt_date,s_orderno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],����lv_pldsr��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"����lv_pldsr��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d],�ɹ�����%d,�ɹ����%f\n",__FILE__,__LINE__,iCount,dSucAmt);
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
        vtcp_log("[%s][%d],ȡ��С��ϵͳ״̬����,sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
        strncpy(acErrMsg,"ȡ��С��ϵͳ״̬����",40);
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
    
    /*����Ӧ�Ŀ��*/
    lv_destroy(&lv);
    
    return 0;
}

static int lv_movPtag(TLVPACKAGE *ilv,char *tag1,TLVPACKAGE *lv,char *tag2)
{
    char buf[61]; /*ֻ����С����*/ 
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
    vtcp_log("[%s][%d]�õ�packid=[%s]\n",__FILE__,__LINE__,packid);
    /*��wd_lv_wbctl�в���һ����¼����ѯ*/
    return 0;
}

/*����������40507ҵ��*/
int lv_dsf_deal40507(struct lv_pldsfqugx_c * lv_pldsfqugx)
{
    TLVPACKAGE  lv;
    char lvdate[9];
    int iRc=0;
    char temp[100];
    struct lv_dsfsendlist_c lv_dsfsendlist;
    
    /*�ȷ������ݰ�*/
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
    vtcp_log("[%s][%d],�����ǽ���������\n",__FILE__,__LINE__);

    /*���ļ���*/
    memcpy(lv.CMTCode,"012",3);     /*ͨ����Ϣҵ���*/
    lv.opeType = '1';               /*������С���*/
    lv.pkgType = '1';               /*������PKG��*/
    memcpy(lv.CMTNo,"005",3);       /*ҵ��Ҫ�ؼ�*/
    lv_set_tag(&lv,"0BG","40507");  /*ҵ�����ͺ�*/
    
    
    lv_set_tag(&lv,"52A",lv_pldsfqugx->br_no);    
    memcpy(lv_pldsfqugx->or_br_no,lv_pldsfqugx->br_no,sizeof(lv_pldsfqugx->or_br_no)-1);
    lv_set_tag(&lv,"58A",LV_DSFZX);
    lv_set_tag(&lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
    lv_set_tag(&lv,"0BC",lv_pldsfqugx->orderno);
    sprintf(temp,"%08d",310);
    lv_set_tag(&lv,"B40",temp);    /*ÿ��ֻֻһ��ҵ��,����318���ֽڵ�����*/
    /*sprintf(temp,"%8s",lv_pldsfqugx->wt_date);
    lv_set_tag(&lv,"72C",temp); del by weiz ,Ӧ��û���������*/
    vtcp_log("[%s][%d]��ʼ��Ӹ���������\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]lv_pldsfqugx->xyno=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->xyno);
    sprintf(temp,"%-60s",lv_pldsfqugx->xyno);
    lv_set_tag(&lv,"72C",temp);
    sprintf(temp,"%1s",lv_pldsfqugx->xy_ind);
    lv_set_tag_multi(&lv,"72C",temp);
    vtcp_log("[%s][%d]��ͬ����=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->xy_ind);
    sprintf(temp,"%1s",lv_pldsfqugx->ch_ind);
    vtcp_log("[%s][%d]�����ʽ=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->ch_ind);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_br_no);
    vtcp_log("[%s][%d]�����к�=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->pay_br_no);
    lv_set_tag_multi(&lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_qs_no);
    vtcp_log("[%s][%d]����������=[%s]\n",__FILE__,__LINE__,lv_pldsfqugx->pay_qs_no);
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
    
    vtcp_log("[%s][%d]---------------------����40507-------------------------\n",__FILE__,__LINE__);
    /*ƴ��*/
    /*��ͷ��Ϣ*/
    lv_batch_end(&lv);
    lv_pkg_batch(&lv,"02C","012");
    
    vtcp_log("[%s][%d]lv_pldsfqugx->br_no==[%s]",__FILE__,__LINE__,lv_pldsfqugx->br_no);
    
    if (hvqsbrno(&lv,lv_pldsfqugx->br_no)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        return -1;
    }
    if (hvqsorbrno(&lv,LV_DSFZX)==-1)
    {
        vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
        return -1;
    }
    lv_pkg_batch(&lv,"30E",lvdate);
    
    lv_pkg_batch(&lv,"0BD",lv_pldsfqugx->packid);
    lv_pkg_batch(&lv,"BS7","0");
    lv_pkg_batch(&lv,"0BG","40507");
    lv_pkg_batch(&lv,"B42","0001");
    sprintf(temp,"%-60s","40507����");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    /*��wd_lv_wbctl�в���һ����¼����ѯ*/

    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],д�����Ǽǲ���\n",__FILE__,__LINE__);
        return -1;
    }
    /*����*/
    lv.packID=atoi(lv_pldsfqugx->packid);
    DSFSEND(&lv);
    lv_destroy(&lv);
    vtcp_log("[%s][%d],���Ժ����ݲ������ݿ�\n",__FILE__,__LINE__);
    memcpy(lv_pldsfqugx->sts,DSF_SEND,2);

    /*�������ݿ�*/
    vtcp_log("[%s][%d]׼������Э�����ǼǱ�",__FILE__,__LINE__);
    
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
         vtcp_log("[%s][%d],����ǩԼ��ϵ���֪ͨ�Ǽǲ���sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]����ǩԼ��ϵ���֪ͨ�Ǽǲ���",__FL__);
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
    g_reply_int  = Lv_dsfsendlist_Ins(lv_dsfsendlist,g_pub_tx.reply ); /*��ֵ����!!*/
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],������ո����˲���sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"[%s][%d]������ո����˲���sqlcode=[%d]",__FL__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(-1);
    }
    return 0;
}

/*������������40505��ִҵ��*/
int lv_dsf_deal40505_return(char *packid,char * orderno ,char *wt_date, char *repstat,char *re_content,char *errcode)
{
    int iRc =0,iSocketID; 
    struct lv_pldscx_c cx;
    char lvdate[9];
    char cRe_date[9];
    TLVPACKAGE lv;    /*40503���ذ�*/
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
        vtcp_log("[%s][%d]Lv_pldscx_Dec_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Dec_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    g_reply_int = Lv_pldscx_Fet_Upd(&cx,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]Lv_pldscx_Fet_Upd��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Fet_Upd��ʧ��",__FILE__,__LINE__);
        return(g_reply_int);
    }
    else
    {
        /*����Ѿ���Ӧ,ֱ�ӱ���*/
        if (memcmp(cx.sts,DSF_REJE,2)==0 || memcmp(cx.sts,DSF_SUCC,2)==0 || memcmp(cx.sts,DSF_FAIL,2)==0)
        {
            vtcp_log("[%s][%d]�����׼�����Ӧ,��Ӧ���Ϊ[%s]",__FILE__,__LINE__,cx.repstat2);
            sprintf(errcode,"[%s][%d]�����׼�����Ӧ,��Ӧ���Ϊ[%s]",__FILE__,__LINE__,cx.repstat2);
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
        vtcp_log("%s,%Upd Lv_pldscx_Upd_Upd��ʧ��[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(errcode,"[%s][%d]Lv_pldscx_Upd_Upd��ʧ��",__FILE__,__LINE__);
        return (g_reply_int);
    }
    Lv_pldscx_Clo_Upd();
    /*���ļ���*/
    vtcp_log("[%s][%d],Find PLDSCX success!\n",__FILE__,__LINE__);
    memcpy(lv.CMTCode,"012",3);     /*��ͨ����ҵ���*/
    lv.opeType = '1';               /*������С���*/
    lv.pkgType = '1';               /*������PKG��   */
    memcpy(lv.CMTNo,"005",3);       /*ҵ��Ҫ�ؼ�    */
    lv_set_tag(&lv,"0BG","40506");  /*ҵ�����ͺ�    */
    lv_set_tag(&lv,"52A",cx.br_no);    
    lv_set_tag(&lv,"58A",cx.or_br_no);
    lv_set_tag(&lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
    lv_get_dsf40506_orderno(&lv,cx.orderno);
    sprintf(temp,"%08d",91);
    lv_set_tag(&lv,"B40",temp);    /*ÿ��ֻֻһ��ҵ��,����91���ֽڵ�����*/
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
    
    vtcp_log("[%s][%d]---------------------------����40506-------------------------------\n",__FILE__,__LINE__);
    /*ƴ��*/
    /*��ͷ��Ϣ*/
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
    sprintf(temp,"%-60s","40505��ִ");
    lv_pkg_batch(&lv,"72D",temp);
    /* .... */
    memcpy(lv.workDate,lvdate,8);
    lv_gen_pack(&lv);
    if (lv_dsf_wbctl(&lv)!=0)
    {
        vtcp_log("[%s][%d],д�����Ǽǲ���\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д�����Ǽǲ���",__FILE__,__LINE__);
        return -1;
    }
    if (dsf_setsendlist(&lv)!=0)
    {
        vtcp_log("[%s][%d],д���ո�������\n",__FILE__,__LINE__);
        sprintf(errcode,"[%s][%d]д���ո�������",__FILE__,__LINE__);
        return -1;
    }
    vtcp_log("[%s][%d],genpack success!\n",__FILE__,__LINE__);
    /*����*/
    DSFSEND(&lv);
    vtcp_log("[%s][%d],sendpack success!\n",__FILE__,__LINE__);
    lv_destroy(&lv);
    
    return 0;
}

/*
˵����
���ݴ���Ļ����ŵõ�֧��ϵͳ�к�
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
    if (isdigit(brno[5]))/*�жϵ�5λ�Ƿ�Ϊ����*/
    {
        vtcp_log("[%s][%d],isdigit=[%d]%c\n",__FILE__,__LINE__,isdigit(brno[4]),brno[4]);
        return 0;
    }
    memcpy(hv_orbr_no.br_no,brno,5);

    iRc=Hv_orno_brno_Sel(g_pub_tx.reply,&hv_orbr_no,"br_no='%s' ",hv_orbr_no.br_no);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],��ѯ�����кŴ���,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"��ѯ�����кŴ���",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    memcpy(ibrno,hv_orbr_no.or_br_no,12);
    return 0;
}
/*����Ƿ���Ի�ִ��Ӧ�Ĵ���ҵ���Ѿ�����40503��ִ��
����������0,�����Է���1*/
int dsf_candfret(char *iwt_date)
{
    int days=0;
    int ithdays=0;
    int todate=0;
    int iwtday=0;
    int iRc=0;
    struct lv_param_c lv_param;
    memset(&lv_param,0,sizeof(lv_param));
    
    
    /*ȡС�ǰ����*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(iwt_date,8);
    vtcp_log("[%s][%d]iwtday=[%ld]todate=[%ld]",__FILE__,__LINE__,iwtday,todate);
    /*ȡС�ǰ���ں�ί�����ڵĲ�ʵ����*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*ȡ�˻�涨ʱ��*/

    iRc = Lv_param_Sel(g_pub_tx.reply,&lv_param,"par_code='THDAYS' ");
    if(iRc!=0)
    {
        vtcp_log("[%s][%d],���ѯlv_param���THDAYSֵ,sqlcode=%d\n",__FILE__,__LINE__,iRc);
        strncpy(acErrMsg,"���ѯlv_param���THDAYSֵ",40);
        set_zd_data("0130",acErrMsg);
        return(-1);
    }
    zip_space(lv_param.par_value);
    ithdays = apatoi(lv_param.par_value,8);
    /*�Ƚϲ�ʵ�������˻�涨ʱ��,����*/
    if (days>ithdays)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*����Ƿ���Ի�ִ��Ӧ�Ĵ���ҵ���Ѿ�����40504��ִ��*/
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
    
    
    
    /*���ݶ�Ӧ��С����ҵ��״̬�����Ƿ�ɻ�(״̬Ϊ����ܾ���ֹ��),���ܼ���*/
    memset(&ds,0,sizeof(ds));
    memset(&tx,0,sizeof(tx));

    Lv_pldsr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",cOrderno,cWt_date);
    while(1)
    {
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel( &ds,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]�����Ѿ�ȡ��.\n",__FILE__,__LINE__);
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
    /*ȡС�ǰ����*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(cWt_date,8);
    /*ȡС�ǰ���ں�ί�����ڵĲ�ʵ����*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*�Ƚϲ�ʵ�����ͻ�ִ�涨ʱ��,�������*/
    if (days>atoi(cReday))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*����Ƿ���Ի�ִ��Ӧ�Ĵ���ҵ���Ѿ�������,-1Ϊ����δ���,
���Ѿ�����,0Ϊ���������,1Ϊ����δ���,δ����*/
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
    
    /*���ݶ�Ӧ��С����ҵ��״̬�����Ƿ�ɻ�(״̬Ϊ����ܾ���ֹ��),���ܼ���*/
    memset(&ds,0,sizeof(ds));
    memset(&tx,0,sizeof(tx));

    Lv_pldsr_Dec_Sel(g_pub_tx.reply,"orderno='%s' and wt_date=%s and tx_mx_no<>'00000000'  ",cOrderno,cWt_date);
    while(1)
    {
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel( &ds,g_pub_tx.reply );
        if (g_reply_int)
        {
            vtcp_log("[%s][%d]�����Ѿ�ȡ��.\n",__FILE__,__LINE__);
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
    /*ȡС�ǰ����*/
    todate = apatoi(pcLvGetDay_work(),8);
    iwtday = apatoi(cWt_date,8);
    /*ȡС�ǰ���ں�ί�����ڵĲ�ʵ����*/
    days = pub_base_CalTrueDays(iwtday,todate);
    /*�Ƚϲ�ʵ�����ͻ�ִ�涨ʱ��,�������*/
    if (days>atoi(cReday))
    {
        return -1;
    }    
    else
    {
        return 1;
    }    
}

/*���ͽ���״̬*/
int dsf_txstat(char stat,char *str2)
{
    char str[30];
    vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
    switch(stat){
        case STAT_INVALID:/*��Ч״̬*/
            strcpy(str,"��Ч״̬");
            break;
        case STAT_WZLR:/*����¼��*/
            strcpy(str,"����¼��");
            break;
        case STAT_WZCHECK1     :/*���˸���,���� --��Ч,20051130 by lhf*/
            strcpy(str,"���˸���,����");
            break;
        case STAT_WZCHECK2     :  /*���˸���,����*/
            strcpy(str,"���˸���,����");
            break;
        case STAT_WZSEND:  /*���˷���*/
            strcpy(str,"���˷���");
            break;
        case STAT_WZREJECT     :  /*���˾ܾ�*/
            strcpy(str,"���˾ܾ�");
            break;
        case STAT_WZSUCCESS    :  /*��������*/
            strcpy(str,"��������");
            break;
        case STAT_WZINQUEUE    :  /*�����Ŷ�*/
            strcpy(str,"�����Ŷ�");
            break;
        case STAT_WZCANCEL_WAIT:  /*���˳����ȴ�*/
            strcpy(str,"���˳����ȴ�");
            break;
        case STAT_WZCANCEL     :  /*���˳���*/
            strcpy(str,"���˳���");
            break;
        case STAT_WZBACK_WAIT  :  /*�����˻�ֹ���ȴ�*/
            strcpy(str,"�����˻�ֹ���ȴ�");
            break;
        case STAT_WZBACK:  /*�����˻�*/
            strcpy(str,"�����˻�");
            break;
        case STAT_WZCZ         :  /*�����ѳ���*/
            strcpy(str,"�����ѳ���");
            break;
        case STAT_WZZF         :  /*������ֹ��*/
            strcpy(str,"������ֹ��");
            break;
        case STAT_WZEXCEPT     :  /*�����쳣����*/
            strcpy(str,"�����쳣����");
            break;
        case STAT_LZRECV1      :  /*��ʱ����,������*/
            strcpy(str,"��ʱ����,������");
            break;
        case STAT_LZRECV2      :  /*��ʽ����,��������*/
            strcpy(str,"��ʽ���ռ���");
            break;
        case STAT_LZBACK_WAIT  :  /*�����˻صȴ�*/
            strcpy(str,"�����˻صȴ�");
            break;
        case STAT_LZBACK       :  /*�����˻�*/
            strcpy(str,"�����˻�");
            break;
        case STAT_YES_NOTSEND  :  /*����ͬ��Ӧ����δ����*/
            strcpy(str,"����ͬ��Ӧ��δ����");
            break;
        case STAT_YES_SEND     :  /*����ͬ��Ӧ�����ѷ���*/
            strcpy(str,"����ͬ��Ӧ�����ѷ���");
            break;
        case STAT_NO_NOTSEND   :  /*������ͬ��Ӧ����δ����*/
            strcpy(str,"������ͬ��Ӧ��δ����");
            break;
        case STAT_NO_SEND      :  /*������ͬ��Ӧ�����ѷ���*/
            strcpy(str,"������ͬ��Ӧ�����ѷ���");
            break;
        case STAT_LZCZ         :  /*�����ѳ���*/
            strcpy(str,"�����ѳ���");
            break;
        case STAT_LZZF         :  /*������ֹ��*/
            strcpy(str,"������ֹ��");
            break;
        case STAT_LZFK         :  /*�����Ѹ���*/
            strcpy(str,"�����Ѹ���");
            break;
        case STAT_LZNMAC_ERR   :  /*ȫ��Ѻ��*/
            strcpy(str,"ȫ��Ѻ��");
            break;
        case STAT_LZLMAC_ERR   :  /*�ط�Ѻ��*/
            strcpy(str,"�ط�Ѻ��");
            break;
        case STAT_LZAMAC_ERR   :  /*ȫ��Ѻ�͵ط�Ѻ����*/
            strcpy(str,"ȫ��Ѻ�͵ط�Ѻ����");
            break;
        case STAT_LZGZ_QS      :  /*�������Ĺ���*/
            strcpy(str,"�������Ĺ���");
            break;
        case STAT_LZEXCEPT     :  /*�����쳣����*/
            strcpy(str,"�����쳣����");
            break;
        default:
            break;
    }
    strcat(str2,str);
    return 0;
}

/*�ǳ������װѶ�Ӧ�Ķ��ڽ��ҵ�����д���ļ���λ����*/
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
        vtcp_log("[%s][%d],��ѯLv_pldscx��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"��ѯLv_pldscx��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    vtcp_log("[%s][%d]׼����lv_pldsr��¼����",__FILE__,__LINE__);
    vtcp_log("[%s][%d]orderno=[%s] wt_date=[%ld]",__FILE__,__LINE__,sc.o_orderno,sc.o_wt_date);
    g_reply_int = Lv_pldsr_Dec_Sel(g_pub_tx.reply," orderno= '%s' and wt_date='%ld' and tx_mx_no<>'00000000' ",\
                                                   sc.o_orderno,sc.o_wt_date);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],����lv_pldsr��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"����lv_pldsr��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    } 
    while(1)
    {
        vtcp_log("[%s][%d],��һ������ҵ��\n",__FILE__,__LINE__);
        memset(&ds,0,sizeof(ds));

        g_reply_int = Lv_pldsr_Fet_Sel(&ds, g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],�����Ѿ�ȡ��[%d]",__FILE__,__LINE__, g_reply_int);
            break;
        } 
        memset(&tx,0,sizeof(tx));
        g_reply_int = iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],�����Ѿ�ȡ��[%d]",__FILE__,__LINE__, g_reply_int);
            break;
        }
        if (i==0)
        { /*��һ��*/
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
        sprintf(cTmpbuf,"���ڽ�����%s��%s",firstOrderno,lastOrderno);
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
        
        sprintf(outarr,"%s%s��������%ld,���%s",cTmpbuf,cTmpArr,sc.o_wt_date,sc.o_orderno);
        vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
    }
    else
    {
        vtcp_log("[%s][%d] here4=",__FILE__,__LINE__);
        sprintf(outarr,"δ�����ڽ�ǽ���,ԭ��������[%ld]���[%s]",sc.o_wt_date,sc.o_orderno);
    } 
    vtcp_log("[%s][%d]===outarr===[%s]",__FILE__,__LINE__,outarr);
    return 0;
}

/*�ҳ����ն�Ӧ�Ľ�ǽ���*/
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
                vtcp_log("[%s][%d]�����Ѿ�ȡ��.\n",__FILE__,__LINE__);
                break;
            }
            memset(&tx,0,sizeof(tx));
            vtcp_log("[%s][%d]wt_date=[%ld]",__FILE__,__LINE__,ds.wt_date);
            g_reply_int = iDsf_gettx(&tx,ds.wt_date,ds.orderno,ds.tx_mx_no,cPkgno);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],�����Ѿ�ȡ��[%d]",__FILE__,__LINE__, g_reply_int);
                break;
            }
            if (i==0)
            { /*��һ��*/
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
            sprintf(outarr,"��Ӧ�����[%s]��[%s]",firstOrderno,lastOrderno);
            
            /**������Ҫ���Ӱ���״̬**/
            memset(&gLv_wbctl , 0 , sizeof(gLv_wbctl));
            iRc = Lv_wbctl_Sel( g_pub_tx.reply,&gLv_wbctl,"packid = %s and pack_date = %d and pkgno='006' ",tx.packid,tx.pack_date);
            if ( iRc )
            {
                sprintf(acErrMsg,"С���������Ʊ��в����ڸü�¼");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d] packstat=[%s]\n",__FILE__,__LINE__,gLv_wbctl.packstat);
            /*beg  mod by ChengGX 2010514 ��ˮ*/
            if(strlen(gLv_wbctl.packstat)>0)
                dsf_wbctl(gLv_wbctl.packstat,outarr);
            else
                strcpy(outarr+strlen(outarr),"  ƴ���ɹ� δ����");
            /*end mod by ChengGX 2010514 ��ˮ */
            vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
            
            if(strlen(tx.rcpstat)==2)
            {
                memcpy(cBuff,",���յ�Ӧ��",11);
            }
            else
            {
                memcpy(cBuff,",δ�յ�Ӧ��",13);
            }
            strcat(outarr,cBuff);
            vtcp_log("[%s][%d] xyy outarr=[%s]\n",__FILE__,__LINE__,outarr);
            vtcp_log("[%s][%d] txstat=[%s]\n",__FILE__,__LINE__,tx.lv_sts);
        }
        else
        {
            sprintf(outarr,"δ�����ڽ�ǽ���");
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
            sprintf(outarr,"��Ӧ���ڽ��[%s],",firstOrderno);
            if(strlen(tx.rcpstat)==2)/***�������Ϊ2�Ļ�,�Ǿ�˵���Ѿ��������ڽ�ǻ�ִ��**/
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
            sprintf(outarr,"δ����Ӧ���ڽ�ǽ���");
        }
    }
    return 0;
}

/*�ҳ�������Ӧ�Ĵ��ǽ���*/
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
                vtcp_log("[%s][%d]�����Ѿ�ȡ��.\n",__FILE__,__LINE__);
                break;
            }
            memset(&tx,0,sizeof(tx));
            g_reply_int = iDsf_gettx(&tx,df.wt_date,df.orderno,df.tx_mx_no,cPkgno);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],�����Ѿ�ȡ��[%d]",__FILE__,__LINE__, g_reply_int);
                break;
            }
            if (i==0)
            { 
                /*��һ��*/
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
            sprintf(outarr,"��Ӧ���ڴ������[%s]��[%s]",firstOrderno,lastOrderno);
            /**������Ҫ���Ӱ���״̬**/
            memset(&gLv_wbctl , 0 , sizeof(gLv_wbctl));
            iRc = Lv_wbctl_Sel( g_pub_tx.reply,&gLv_wbctl,"packid = %s and pack_date = %d and pkgno='005' ",tx.packid,tx.pack_date);
            if ( iRc )
            {
                sprintf(acErrMsg,"С���������Ʊ��в����ڸü�¼");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d] packstat=[%s]\n",__FILE__,__LINE__,gLv_wbctl.packstat);
            /*beg  mod by ChengGX 2010514 ��ˮ*/
            if(strlen(gLv_wbctl.packstat)>0)
                dsf_wbctl(gLv_wbctl.packstat,outarr);
            else
                strcpy(outarr+strlen(outarr),"  ƴ���ɹ�");
            /*end mod by ChengGX 2010514 ��ˮ */
            vtcp_log("[%s][%d] outarr=[%s]\n",__FILE__,__LINE__,outarr);
        }
        else
        {
            sprintf(outarr,"δ�����ڴ��ǽ���");
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
            sprintf(outarr,"��Ӧ���ڴ������[%s]",firstOrderno);
            dsf_txstat(tx.lv_sts[0],outarr);
            vtcp_log("[%s][%d] xyy outarr=[%s]\n",__FILE__,__LINE__,outarr);
            vtcp_log("[%s][%d] txstat=[%s]\n",__FILE__,__LINE__,tx.lv_sts);
        }
        else
        {
            sprintf(outarr,"δ�����ڴ��ǽ���");
        }
    }
    return 0;
}


/*�����ʺ�ȡ���������,���еĻ������Ը�����ϵͳ�ڵĻ�����һ��,����,����Ͳ��жϻ�����*/
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
      sprintf(acErrMsg,"��ѯmdm_ac_rel����!![%d]",ret);
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
  if(ret)
  {
      sprintf(acErrMsg,"��ѯprdt_ac_id����!![%d]",ret);
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  vtcp_log("[%s][%d]ac_id_type=[%c]",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
  switch(sPrdt_ac_id.ac_id_type[0])
  {
    case '1':/*����*/
      ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);  
      if(ret)
      {
          sprintf(acErrMsg,"��ѯdd_mst����!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
      if(sDd_mst.ac_sts[0]!='1' || sDd_mst.hold_sts[0]=='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sDd_mst.name);          
          sprintf(acErrMsg,"״̬״̬[%s]�򶳽�״̬[%s]������!",sDd_mst.ac_sts,sDd_mst.hold_sts);
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
          sprintf(acErrMsg,"��ѯdd_mst��������!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }
      dAvbal=sDd_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    case '2':/*���ڴ��*/
      ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"��ѯtd_mst����!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
      if(sTd_mst.ac_sts[0]!='1' || sTd_mst.hold_sts[0]=='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sTd_mst.name);          
          sprintf(acErrMsg,"״̬״̬[%s]�򶳽�״̬[%s]������!",sTd_mst.ac_sts,sTd_mst.hold_sts);
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
          sprintf(acErrMsg,"��ѯtd_mst��������!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }   
      dAvbal=sTd_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    /**********delete by xyy 2009-7-24 15:29:33
    case '3':/ *����* /
      ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"��ѯln_mst����!![%d]",ret);
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
          sprintf(acErrMsg,"��ѯln_mst��������!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }  
      dAvbal=sLn_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
      ********************/
    case '9':/*�ڲ���*/
      ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
      if(ret)
      {
          sprintf(acErrMsg,"��ѯin_mst����!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit;
      }
       if(sIn_mst.sts[0]!='1')
      {
          vtcp_log("[%s][%d]cName=[%s]sDd_mst.name=[%s]",__FILE__,__LINE__,cName,sIn_mst.name);          
          sprintf(acErrMsg,"״̬״̬[%s]������!",sIn_mst.sts);
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
          sprintf(acErrMsg,"��ѯin_mst��������!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          goto ErrExit2;
      }
      dAvbal=sIn_mst.bal;
      sprintf(avbal,"%f",dAvbal);
      break;
    default:
      sprintf(acErrMsg,"��Ч���ʻ�����");
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
    if(memcmp(stat,RET_SUCCESS,2)==0)  /*��ִ�ɹ�*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�ɹ�]");
    }
    else if(memcmp(stat,RET_NOACTNO,2)==0)/*�ʺŲ���*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�ʺŲ���]");
    }
    else if(memcmp(stat,RET_NONAME,2)==0)/*�ʺŻ�������*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�ʺŻ�������]");
    }
    else if(memcmp(stat,RET_NOMONEY,2)==0)  /*�˻�����֧��*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�˻�����]");
    }
    else if(memcmp(stat,RET_PSWDERR,2)==0) /*�˻��������*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�˻��������]");
    }
    else if(memcmp(stat,RET_ACTCLSERR,2)==0) /*�ʺ�״̬����*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[�ʺ�״̬����]");
    }
    else if(memcmp(stat,RET_HASCANCEL,2)==0) /*ҵ���ѳ���*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[ҵ���ѳ���]");
    }
    else if(memcmp(stat,RET_OTHER,2)==0) /*��������*/
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ��ѻ�ִ[��������]");
    }
    else
    {
        vtcp_log("[%s][%d] here1=stat===[%s]",__FILE__,__LINE__,stat);
        strcpy(str,",�ҶԷ�û������״̬�Ĵ���");
    }
    vtcp_log("[%s][%d]str==[%s]",__FILE__,__LINE__,str);
    strcat(str2,str);
    vtcp_log("[%s][%d]str2==[%s]",__FILE__,__LINE__,str2);
    return 0;
}
/**�����״̬**/
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
            strncpy(cTmpBuf,",���δͨ��",11);
            break;
        case  2      : 
            strncpy(cTmpBuf,",�����ѷ���",11);
            break;
        case  3      : 
            strncpy(cTmpBuf,",����������",11);
            break;
        case  4      : 
            strncpy(cTmpBuf,",�������Ŷ�",11);
            break;
        case  5      : 
            strncpy(cTmpBuf,",����������",11);
            break;
        case  6      : 
            strncpy(cTmpBuf,",�����ѳ���",11);
            break;
        case  10      : 
            strncpy(cTmpBuf,",�����ѻ�ִ",11);
            break;
        case  11      : 
            strncpy(cTmpBuf,",�����Ѿܸ�",11);
            break;
        case  12     : 
            strncpy(cTmpBuf,",�����ѳ���",11);
            break;
        case  13      : 
            strncpy(cTmpBuf,",������ֹ��",11);
            break;
        case  14      : 
            strncpy(cTmpBuf,",�����ѳ�ʱ",11);
             break;
        default:
            strncpy(cTmpBuf,",δ֪����",9);
            break;
    }
    strcat(str2 , cTmpBuf);
    return 0;
}
/*********************************һ�º�������û���õ�**********************************/

/*����Ƕ�lv_dsf_deal���ع�,�������������
��ʱ����,ֻ��ֹ������п�����Ҫ*/
int lv_dsf_deal2(TLVPACKAGE *ilv)
{
    int iRc=0,iCount=0,i;
    TLVPACKAGE sublv,olv;        /*С��,���صİ�*/
    static char txtype[6];
    /*��ʼ������*/
    lv_create(&sublv);
    lv_create(&olv);
    
    /*�ֽ�����*/
    iCount = lv_count_recnt(ilv);
    while(!iCount--)
    {
        iRc=lv_fetchpkg(ilv,&sublv);

        iRc = lv_analyze_pack(ilv);
        if ( iRc!=0 )
        {
            vtcp_log("%s,%Upd ���Ľ�������iRc=[%d]",__FILE__,__LINE__,g_reply_int);
            sprintf(acErrMsg,"���ո����Ľ�������[%s][%d]",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            return (-1);
        }

        memset(txtype,0,sizeof(txtype));
        GETTAG(ilv,"0BG",txtype);    /*ȡ���ո�ҵ������*/
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
            vtcp_log("[%s][%d],�޴�ҵ������yw_type[%s],����\n",txtype);
            continue;
        }
        
    }
    
    
    /*����40506��Ӧ��*/
    lv_destroy(&sublv);
    lv_destroy(&olv);
    return 0;
}
/*���ݻ�ִ�������ǼǱ����״̬�޸�ԭ���׵�״̬*/
int dsf_wb2tx(char *iwt_date,char *iorderno,char stat,char *otx_stat)
{
    return 0;
}
int dsf_dayfromto(char *from,char *to)
{
   return 0;/*�ú����ѱ����滻��pub_base_CalTrueDays*/
}
int lv_dsf_deal40507_batch(TLVPACKAGE  *lv,struct lv_pldsfqugx_c *lv_pldsfqugx,int total,int count,int totalnum)
{
    char lvdate[9];
    int iRc=0;
    char temp[100];
    
    /*�ȷ������ݰ�*/
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

    /*���ļ���*/
    memcpy(lv->CMTCode,"012",3);   /*ͨ����Ϣҵ���*/
    lv->opeType = '1';             /*������С���*/
    lv->pkgType = '1';             /*������PKG��*/
    memcpy(lv->CMTNo,"005",3);     /*ҵ��Ҫ�ؼ�*/
    lv_set_tag(lv,"0BG","40507");  /*ҵ�����ͺ�*/
    
    
    lv_set_tag(lv,"52A",lv_pldsfqugx->br_no);    
    memcpy(lv_pldsfqugx->or_br_no,lv_pldsfqugx->br_no,sizeof(lv_pldsfqugx->or_br_no)-1);
    lv_set_tag(lv,"58A",LV_DSFZX);
    lv_set_tag(lv,"30A",lvdate);    /*���Ҫȡ��ǰС��ϵͳ������*/
    lv_set_tag(lv,"0BC",lv_pldsfqugx->orderno);
    sprintf(temp,"%08d",310);
    lv_set_tag(lv,"B40",temp);    /*ÿ��ֻֻһ��ҵ��,����318���ֽڵ�����*/
    /*sprintf(temp,"%8s",lv_pldsfqugx->wt_date);
    lv_set_tag(lv,"72C",temp); del by weiz ,Ӧ��û���������*/
    vtcp_log("[%s][%d]��ʼ��Ӹ���������\n",__FILE__,__LINE__);
    sprintf(temp,"%-60s",lv_pldsfqugx->xyno);
    lv_set_tag(lv,"72C",temp);
    sprintf(temp,"%1s",lv_pldsfqugx->xy_ind);
    lv_set_tag_multi(lv,"72C",temp);
    vtcp_log("[%s][%d]�Ѿ����е�����\n",__FILE__,__LINE__);
    sprintf(temp,"%1s",lv_pldsfqugx->ch_ind);
    vtcp_log("[%s][%d]�Ѿ����е�����\n",__FILE__,__LINE__);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_br_no);
    lv_set_tag_multi(lv,"72C",temp);
    sprintf(temp,"%12s",lv_pldsfqugx->pay_qs_no);
    vtcp_log("[%s][%d]�Ѿ����е�����\n",__FILE__,__LINE__);
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
    
    vtcp_log("[%s][%d],����40507.\n",__FILE__,__LINE__);
    /*ƴ��*/
    /*��ͷ��Ϣ*/
    lv_batch_end(lv);
    if (total==count)
    {
        lv_pkg_batch(lv,"02C","012");
        if (hvqsbrno(lv,lv_pldsfqugx->br_no)==-1)
        {
            vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
            return -1;
        }
        if (hvqsorbrno(lv,LV_DSFZX)==-1)
        {
            vtcp_log("[%s][%d],ȡ�кų���\n",__FILE__,__LINE__);
            return -1;
        }
        lv_pkg_batch(lv,"30E",lvdate);
        
        lv_pkg_batch(lv,"0BD",lv_pldsfqugx->packid);
        lv_pkg_batch(lv,"BS7","0");
        lv_pkg_batch(lv,"0BG","40507");
        sprintf(temp,"%04d",totalnum);
        lv_pkg_batch(lv,"B42",temp);
        sprintf(temp,"%-60s","40507����");
        lv_pkg_batch(lv,"72D",temp);
        /* .... */
        memcpy(lv->workDate,lvdate,8);
        lv_gen_pack(lv);
        /*��wd_lv_wbctl�в���һ����¼����ѯ*/
        lv_dsf_wbctl(lv);
        if (lv_dsf_wbctl(lv)!=0)
        {
            vtcp_log("[%s][%d],д�����Ǽǲ���\n",__FILE__,__LINE__);
            return -1;
        }
        /*����*/
        lv->packID=atoi(lv_pldsfqugx->packid);
        DSFSEND(lv);
        vtcp_log("[%s][%d],���Ժ����ݲ������ݿ�\n",__FILE__,__LINE__);
        memcpy(lv_pldsfqugx->sts,DSF_SEND,2);
    }

    /*�������ݿ�*/

    g_reply_int  = Lv_pldsfqugx_Ins(*lv_pldsfqugx ,g_pub_tx.reply );
    if(g_reply_int)
    {
         vtcp_log("[%s][%d],����ǩԼ��ϵ���֪ͨ�Ǽǲ���sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
         sprintf( acErrMsg,"[%s][%d]����ǩԼ��ϵ���֪ͨ�Ǽǲ���",__FL__);
         set_zd_data(DC_GET_MSG,acErrMsg);
         return(-1);
    }
    return 0;
}
static int getactname(const char *isactno,const char *iname)
{
     return 0;
}
