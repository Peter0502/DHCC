/********************************************************************************
* ���ڽ����Ϣ�������                                                          *
* ����:��/home/mpt/tmp/name�ļ��е����ݵ��뵽lv_pkgreg����                      *
* ������ļ����ֶ�˳�����£�(�м�����|�ָ�)                                     *
* �������˺�|����������|���׽�� |�෽Э���|�������к�|������˿�����|���ո�   *
* payactno  |payname   |txamt(��)|  protno  |   acbrno | cashno     |dsf        *
*   ����:dsf ===wt_date+orderno+tx_mx_no                                        *
*                                                                               *
* �漰��:  lv_dqjjs                                                             *
*                                                                               *
*     name   -wd_fdtl                                                           *
* �� д ��: xyy           2007-1-11 10:21                                       *
* �������: xmno ,actno,name,feeflag,ywtype,content                             *
* ��  ��: �ļ���Ļ�����Ϣ���뵽����                                            *  
* Modified by SSH,2006.1.16,�޸�һЩ����                                        *
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
#include "lv_pldsr_c.h"

#define HZDATE  2   /** ��ͨ���ҵ���ִ����      **/

extern char* pcLvGetDay();
extern char *zip_space(char *);
extern char *strtokk (char *,char *);
extern char * pub_base_daynumLAT(char *date, int num);
extern int iGetActnoSts1(char *ac_no,double *avbal);
int spJ057()
{
    int    iRc = 0;
    int    iOrderno;
    int    nLineId;
    int    iHzdate=0;
    FILE   *fpr;
    double dTotamt=0;
    double dTxamt=0;
    double dAvbal = 0.0;
    double cAmt =0.0;
    char  cTmpBuff    [61 ];
    char  cTimes      [5  ];  /* �ܵļ�¼��*/
    char  cKinbr      [BRNO_LEN+1];
    char  cTlrno      [5  ];  
    char  pcPackday   [9  ];  
    char  pcPackid    [9  ];
    char  pcFilename  [500];
    char  cXmno       [61 ];  /* ��Ŀ���  */
    char  cCash_ac_no [25 ];  /* �տ����ʺ�*/ 
    char  cName       [21 ];  /* �ļ�����  */
    char  cFeeflg     [2  ];  /* �����ѱ�־*/
    char  cYw_type    [13 ];  /* ҵ������  */
    char  cYw_ind     [13 ];
    char  cContent    [61 ];  /* ����      */
    char  cAcbrno     [13];   /* �������к�*/
    char  cCashqsno   [13];   /* ����������*/
    char  cTxamt      [18];   /* ���׽��*/
    char  cTotamt     [18];   /* �����ܽ��*/
    char  cWtday      [9 ];
    char  cStr        [120];   /*add by ChengGX ��ˮ 20100605*/
    memset(cStr,0, sizeof(cStr));
        
    struct _t_readline
    {
          char  xmno        [60]; /* ��Ŀ���    */
          char  payactno    [32]; /* �������˺�  */        
          char  payname     [60]; /* ����������  */
          char  txamt       [17]; /* ���׽��    */
          char  protno      [60]; /* �෽Э���*/
          char  payno       [12]; /* �����˿�����*/
          char  acbrno      [12]; /* �������к�  */
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
    memset(cCash_ac_no  ,0, sizeof(cCash_ac_no )); 
    memset(cName        ,0, sizeof(cName      ));
    memset(cFeeflg      ,0, sizeof(cFeeflg    ));
    memset(cYw_type     ,0, sizeof(cYw_type   ));
    memset(cYw_ind      ,0, sizeof(cYw_ind    ));
    memset(cAcbrno      ,0, sizeof(cAcbrno    ));
    memset(cCashqsno    ,0, sizeof(cCashqsno  ));
    memset(cTxamt       ,0, sizeof(cTxamt     ));
    memset(cTotamt      ,0, sizeof(cTotamt    ));
    
    pub_base_sysinit();
    /* �õ�ǰ̨��������� */
    get_fd_data("0030", cKinbr);    /**���׻���**/
    get_fd_data("0070", cTlrno);    /**����Ա  **/
    get_fd_data("0961", cXmno);     /**��Ŀ��  **/
    get_fd_data("0330", cCash_ac_no);/**�����ʺ�**/
    get_fd_data("0380", cName);     /**�ļ�����**/
    get_fd_data("0962", cTmpBuff);  /**12λ��,������ѡ��һ����**/
    get_fd_data("0963", cContent);  /**����    **/
    get_fd_data("0965", cFeeflg);   /**�����ѱ�־**/
    get_fd_data("0390", cTotamt);   /**�����ܽ��**/
    
    iHzdate=HZDATE;
    
    zip_space(cTmpBuff);
    vtcp_log("[%s][%d]cTmpBuff==[%s]",__FL__,cTmpBuff);
    memcpy(cYw_type,cTmpBuff ,sizeof(cYw_type)-1);
    vtcp_log("[%s][%d]ҵ������==[%s]",__FL__,cYw_type);
    zipn_space(cXmno    ,sizeof(cXmno     )-1);
    zipn_space(cCash_ac_no,sizeof(cCash_ac_no)-1);
    zipn_space(cYw_type  ,sizeof(cYw_type  )-1);
    zipn_space(cContent  ,sizeof(cContent  )-1);

    vtcp_log("[%s][%d]���������Ŀ�����[%.60s]\n",__FL__,cXmno);
    vtcp_log("[%s][%d]������ĸ����ʺ���[%.19s]\n",__FL__,cCash_ac_no);        
    vtcp_log("[%s][%d]��������ļ�������[%.20s]\n",__FL__,cName);
    vtcp_log("[%s][%d] spJ057��ʼ�\n",__FL__);
    
    if(iGetActnoSts1(cCash_ac_no,&dAvbal))  /**���ȼ���ʻ�״̬**/
    {
          sprintf( acErrMsg,"[%s][%d]��鸶���ʺ�״̬ʧ��",__FL__);
        strncpy(g_pub_tx.reply,"P109",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] ��ʼ���Ӫҵ״̬,���ʻ����Ϊ[%f]",__FL__,dAvbal);
    if(iLvchkstat())   /**���Ӫҵ״̬**/
    {
        sprintf( acErrMsg,"[%s][%d]���Ӫҵ״̬����",__FL__);
        strncpy(g_pub_tx.reply,"P046",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }

    sprintf(pcFilename,"%s/tmp/%.*s",getenv("WORKDIR"),sizeof(cName)-1,cName);
    zip_space(pcFilename);
    vtcp_log("[%s][%d] pcFilename==[%s] \n",__FL__,pcFilename);        
    /* ����ǰ̨����,�����ݿ���õ���Э��ŵ���Ϣ */        
    g_reply_int = Lv_xmh_Sel(g_pub_tx.reply,&lv_xmh,"xmno='%s' ",cXmno);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"[%s][%d]�õ���Э��ŵ���Ϣ����",__FL__);
        strncpy(g_pub_tx.reply,"P089",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    memcpy(cCashqsno , lv_xmh.o_qs_no,sizeof(cCashqsno)-1);
    memcpy(cYw_ind   , lv_xmh.yw_ind ,sizeof(cYw_ind)-1);
    vtcp_log("[%s][%d]  xmno    =[%s] \n",__FL__,lv_xmh.xmno);
    vtcp_log("[%s][%d]  xm_ind  =[%c] \n",__FL__,lv_xmh.xm_ind[0]);
    vtcp_log("[%s][%d]  ac_no   =[%s] \n",__FL__,lv_xmh.ac_no);
    vtcp_log("[%s][%d]  name    =[%s] \n",__FL__,lv_xmh.name);
    vtcp_log("[%s][%d]  yw_type =[%s] \n",__FL__,lv_xmh.yw_ind);
    vtcp_log("[%s][%d] cCashqsno=[%s] \n",__FL__,cCashqsno);
    /****�����Ŀ�Ƿ���Ч****/
    if(lv_xmh.flg[0]!='0')
    {
        sprintf( acErrMsg,"[%s][%d]��Ŀ���Ϸ�",__FL__);
        strncpy(g_pub_tx.reply,"P089",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    zip_space(lv_xmh.ac_no);
    /**if(memcmp(lv_xmh.ac_no,cCash_ac_no,sizeof(cCash_ac_no)-1)) mod by ChengGX 2010-5-13 ��ˮ **/
    if(memcmp(lv_xmh.ac_no,cCash_ac_no,strlen(cCash_ac_no))) 
    {
        sprintf( acErrMsg,"[%s][%d]��Ŀ��ź͸������ʺŲ�ƥ��",__FL__);
        strncpy(g_pub_tx.reply,"S039",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    if(memcmp(cYw_type,cYw_ind,sizeof(cYw_ind)-1)) 
    {
        sprintf( acErrMsg,"[%s][%d]ҵ������[%s][%s]��һ��",__FL__,cYw_type,cYw_ind);
        strncpy(g_pub_tx.reply,"S039",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /* ����ļ��ܷ���ȷ�� */
    if((fpr=fopen(pcFilename,"rb"))==NULL) 
    {
        sprintf( acErrMsg,"[%s][%d]û���ҵ����Ӧ���ļ�",__FL__);
        strncpy(g_pub_tx.reply,"P090",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /***�ж��Ǵ��ǻ��ǽ��ҵ��**/
    if (memcmp(lv_xmh.xm_ind,"1",1)!=0)        
    {
        sprintf( acErrMsg,"[%s][%d]��Ŀ[%c]���Ƕ��ڽ��",__FL__,lv_xmh.xm_ind[0]);
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
            vtcp_log("[%s][%d] ",__FILE__,__LINE__);
            continue;
        }
        vtcp_log("[%s][%d]** nLineId=[%d] \n",__FL__,nLineId);
        if(nLineId==0)
        {   /***���ļ��ж�ȡ�ļ�¼�ǵ�һ����ʱ��***/
            char *ptr;
            memset(&fdtl,'\0',sizeof(fdtl));
            ptr = strtokk(pcLineBuf,"|");
            if(ptr)
            {
                zip_space(ptr);
                strncpy(fdtl.xmno,ptr,sizeof(fdtl.xmno));        /*��Ŀ��*/
                zip_space(fdtl.xmno);
            }
            else
            {
                vtcp_log("[%s][%d],�ļ�������Ŀ���[%s]\n",__FL__,cXmno);
                sprintf(acErrMsg, "[%s][%d]�ļ�������Ŀ���!",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            if(memcmp(fdtl.xmno,cXmno,sizeof(cXmno)-1)!=0)
            {
                vtcp_log("[%s][%d],[%s]��Ŀ��[%s]��һ��\n",__FL__,fdtl.xmno,cXmno);
                sprintf(acErrMsg,"[%s][%d] �ļ��������������Ŀ�Ų�һ��",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            nLineId++;
        }
        else
        {    /***���ļ��ж�ȡ�Ĳ��ǵ�һ����¼****/                
            char *ptr;
            char cDsfstr[30];
            memset(cDsfstr,0 ,sizeof(cDsfstr)-1);
            memset(&lv_wbctl,'\0',sizeof(lv_wbctl));
            ptr = strtokk(pcLineBuf, "|");
            if(ptr)
            { 
                strncpy(fdtl.payactno,ptr,sizeof(fdtl.payactno));
                zipn_space(fdtl.payactno,sizeof(fdtl.payactno));
            }
            else
            {
                vtcp_log("[%s][%d],�ļ���¼���տ��ʺŲ�����!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �ļ���¼���տ��ʺŲ�����",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            ptr = strtokk(NULL,"|");
            if(ptr)
            {
                strncpy(fdtl.payname,ptr,sizeof(fdtl.payname));
                zipn_space(fdtl.payname,sizeof(fdtl.payname));
            }
            else
            {
                vtcp_log("[%s][%d],�ļ���¼���տ��������!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �ļ���¼���տ��������",__FL__);
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
                vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            }
            else
            {
                vtcp_log("[%s][%d],�ļ���¼�н��׽�����!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �ļ���¼�н��׽�����",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            ptr = strtokk(NULL,"|");
            if(ptr)
            {
                strncpy(fdtl.protno,ptr,sizeof(fdtl.protno));
                zipn_space(fdtl.protno,sizeof(fdtl.protno));
            }
            else
            {
                vtcp_log("[%s][%d],�ļ���¼�ж෽Э��Ų�����!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �ļ���¼�ж෽Э��Ų�����",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            ptr = strtokk(NULL,"|");
            if(ptr)
            {
                strncpy(fdtl.acbrno,ptr,sizeof(fdtl.acbrno));
                zipn_space(fdtl.acbrno,sizeof(fdtl.acbrno));
            }
            else
            {
                vtcp_log("[%s][%d],�ļ���¼�н����кŲ�����!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �ļ���¼�н����кŲ�����",__FL__);
                strcpy( g_pub_tx.reply, "P090");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            memcpy(cAcbrno,fdtl.acbrno,sizeof(cAcbrno)-1);
            g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab," or_br_no='%s' ",cAcbrno);
            if(g_reply_int)
            {   
                vtcp_log("[%s][%d]��ѯ���������кű���� err=[%d]", __FL__,g_reply_int);
                sprintf(acErrMsg,"[%s][%d]��ѯ�����кű����[%s]",__FL__,cAcbrno);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            if(hv_uniontab.br_sts[0]!='1')
            {
                vtcp_log("[%s][%d]������[%s]״̬[%c]����", __FL__,cAcbrno,hv_uniontab.br_sts[0]);
                sprintf(acErrMsg,"[%s][%d]������״̬����[%s]",__FL__,cAcbrno);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            if(memcmp(hv_uniontab.qs_no,cCashqsno,sizeof(cCashqsno)-1)!=0)
            {
                vtcp_log("[%s][%d]������[%s]��������Ŀ[%s]������", __FL__,cAcbrno,cCashqsno);
                sprintf(acErrMsg,"������[%s]��������Ŀ[%s]������",cAcbrno,cCashqsno);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            ptr = strtokk(NULL,"|");
            if (ptr)
            {
                strncpy(fdtl.payno,ptr,sizeof(fdtl.payno));
                zipn_space(fdtl.payno,sizeof(fdtl.payno));
            }
            else
            {
                memcpy(fdtl.payno,fdtl.acbrno,sizeof(fdtl.payno));
            }
            /*add by xyy 2007-3-1 17:08 Ϊ���ո����*/
            ptr = strtokk(NULL,"|");
            memset(cDsfstr,0,sizeof(cDsfstr));
            if (ptr)
            {
                strncpy(cDsfstr,ptr,strlen(ptr));
                zipn_space(cDsfstr,30);
            }
            else
            {
                memcpy(cDsfstr,"\0",1);/*û���˾Ͳ�����*/
                vtcp_log("[%s][%d]�ļ���û��cDsfstr��ֵ!",__FILE__,__LINE__);
            }
            /*******���ļ���ȡ���ļ�¼��payin�ṹ��*******/
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            memset(&payin,'\0',sizeof(payin));
            payin.F_crdb[0]=CRDB_DR;
            memcpy(payin.F_opcd ,OPCD_LR   ,sizeof(payin.F_opcd));
            memcpy(payin.F_pkgno,PKGNO_DQJJ,sizeof(payin.F_pkgno));
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            iOrderno=iLvGetOrderno(); /*����һ���µĽ������*/                        
            if (iOrderno<=0)
            {
                vtcp_log("[%s][%d],�õ�֧��������Ŵ���!\n",__FL__);
                sprintf(acErrMsg,"[%s][%d] �õ�֧��������Ŵ���",__FL__);
                strcpy( g_pub_tx.reply, "HV01");
                set_zd_data(DC_GET_MSG,acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            memcpy(payin.F_txnum,TXNUM_DQJJ,sizeof(payin.F_txnum));
            memcpy(payin.F_acbrno,fdtl.acbrno,sizeof(payin.F_acbrno));
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            memcpy(payin.F_payno,fdtl.payno,sizeof(payin.F_payno));
            vtcp_log("[%s][%d],fdtl.txamt==[%s]",__FL__,fdtl.txamt);
            memcpy(cTxamt,fdtl.txamt,sizeof(fdtl.txamt));
            vtcp_log("[%s][%d],fdtl.txamt===[%s]",__FL__,fdtl.txamt);
            zip_space(cTxamt);
            str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
            vtcp_log("[%s][%d],cTxamt===[%s],���ֽ��:[%f]\n",__FL__,cTxamt,dTxamt);
            dTotamt+=dTxamt;
            dbl2str(&dTxamt,0,sizeof(payin.F_txamt),0,payin.F_txamt);
            vtcp_log("[%s][%d],�ַ����:[%s],�ܽ��:[%f]\n",__FL__,fdtl.txamt,dTotamt);
            memcpy(payin.F_cashactno,lv_xmh.ac_no,sizeof(payin.F_cashactno));
            memcpy(payin.F_cashname ,lv_xmh.name,sizeof(payin.F_cashname));
            memcpy(payin.F_payactno,fdtl.payactno,sizeof(fdtl.payactno));
            memcpy(payin.F_payname ,fdtl.payname ,sizeof(fdtl.payname));
            memcpy(payin.F_dfxyno  ,fdtl.protno ,sizeof(fdtl.protno));
            
            memcpy(payin.F_ywtype   ,cYw_type      ,sizeof(payin.F_ywtype));
            memcpy(payin.F_content  ,cContent      ,sizeof(payin.F_content));

            vtcp_log("[%s][%d]payin.F_cashactno=[%s]",__FILE__,__LINE__,payin.F_cashactno);
            vtcp_log("[%s][%d]payin.F_cashname= [%s]",__FILE__,__LINE__,payin.F_cashname);
            vtcp_log("[%s][%d]payin.F_payactno= [%s]",__FILE__,__LINE__,payin.F_payactno);
            vtcp_log("[%s][%d]payin.F_payname=  [%s]",__FILE__,__LINE__,payin.F_payname);
            vtcp_log("[%s][%d]payin.F_dfxyno=   [%s]",__FILE__,__LINE__,payin.F_dfxyno);
            vtcp_log("[%s][%d]payin.F_ywtype=   [%s]",__FILE__,__LINE__,payin.F_ywtype);
            vtcp_log("[%s][%d]payin.F_content=  [%s]",__FILE__,__LINE__,payin.F_content);
            vtcp_log("[%s][%d]payin.F_acbrno=   [%s]",__FILE__,__LINE__,payin.F_acbrno);
            vtcp_log("[%s][%d]payin.F_txamt=    [%s]",__FILE__,__LINE__,payin.F_txamt);
            vtcp_log("[%s][%d]payin.F_payno=    [%s]",__FILE__,__LINE__,payin.F_payno);
            
            payin.F_operlevel[0]='0';
            payin.F_feeflag[0]=cFeeflg[0];
            payin.F_lw_ind[0] ='1';
            payin.F_packend[0]='0';
            iLvPayinAutoInfo(&payin);/**�õ����ں���ˮ��**/
            memcpy(payin.A_brno,cKinbr,sizeof(payin.A_brno));
            
            iLvSetAddit(&payin,"dsf",cDsfstr,24); /**add by xyy 2007-3-1 17:04**/
            
            iLvZipAll(&payin);/**ѹ�����пո�***/
            /****����¼��****/
            /****�������****/
            g_reply_int=iLvPayinCheck0(&payin);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
                sprintf(acErrMsg,"[%s][%d] iLvPayinCheck0ʧ��",__FL__);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            /****Ҫ�ؼ��****/
            g_reply_int=iLvPayinCheck4(&payin);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d] err in check4 err=[%d]\n", __FL__,g_reply_int);
                sprintf(acErrMsg,"[%s][%d] iLvPayinCheck4ʧ��",__FL__);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            /****�кż��****/
            g_reply_int=iLvPayinCheck1(&payin);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d] err in check1 err=[%d]\n", __FL__,g_reply_int);
                sprintf(acErrMsg,"[%s][%d] iLvPayinCheck1ʧ��",__FL__);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            /***����������������һ���µļ�¼****/
            g_reply_int=iLvNewRecFromPayin(&payin);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d]iLvNewRecFromPayin err=[%d]", __FL__,g_reply_int);
                sprintf(acErrMsg,"[%s][%d] iLvNewRecFromPayin����",__FL__);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            /****�������****/
            memcpy(payin.T_checkno,cTlrno,sizeof(cTlrno)-1);
            memcpy(payin.T_checkwssrno,payin.A_wssrno,sizeof(payin.T_checkwssrno));
            memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));
            memcpy(payin.T_wtday,pcLvGetDay(),sizeof(payin.T_wtday));
            memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));/*opcd Ϊ¼��*/
            vtcp_log("[%s][%d]payin.T_wtday==[%s]",__FILE__,__LINE__,payin.T_wtday);
            
            memcpy(cWtday,payin.T_wtday,sizeof(cWtday)-1);
            vtcp_log("[%s][%d]cWtday==[%s]",__FILE__,__LINE__,cWtday);
            memcpy(payin.F_respday , pub_base_daynumLAT(cWtday,iHzdate),sizeof(payin.F_respday));
            vtcp_log("[%s][%d]��ִ����==[%s]",__FILE__,__LINE__,payin.F_respday);
            vtcp_log("[%s][%d]payin.T_cashqsactno==[%s]",__FILE__,__LINE__,payin.T_cashqsactno);
            
            g_reply_int=iCheckHzdate(payin.T_cashqsactno,iHzdate);
            if (g_reply_int)
            {
                vtcp_log("[%s][%d],����ִ���ڴ���[%d]",__FILE__,__LINE__,g_reply_int);
                sprintf(acErrMsg,"����ִ���ڴ���[%s][%d]",__FILE__,__LINE__);
                strncpy(g_pub_tx.reply,"O206",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
            }
            /****��Ȼ�Ļس�״̬Ϊ"¼��"  modify at 20130422
            *payin.T_stat[0]=STAT_WZLR;״̬Ϊ¼��*
            payin.T_stat[0]=STAT_WZCHECK2;*״̬Ϊ���� ���� mod by ChengGX 20100514 ��ˮ*
            ****/
            payin.T_stat[0]=STAT_WZLR;/****״̬Ϊ¼��****/
            /****����ƴ��****/
            if(nLineId>1)
            {
                memcpy(payin.F_packid ,pcPackid ,sizeof(payin.F_packid));
                memcpy(payin.F_packday,pcPackday,sizeof(payin.F_packday));
                memcpy(payin.T_packid ,pcPackid ,sizeof(payin.T_packid));
                memcpy(payin.T_packday,pcPackday,sizeof(payin.T_packday));
            }
            vtcp_log("[%s][%d] packid=[%s] packday==[%s]",__FILE__,__LINE__,pcPackid,pcPackday);
            iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
            if(iRc!=PCHK_NEW && nLineId==1)
            {
                vtcp_log("[%s][%d],�����°���[%d]",__FL__,iRc);
                sprintf( acErrMsg,"[%s][%d]�����°���[%d]",__FL__,g_reply_int);
                strcpy( g_pub_tx.reply, "P040");
                set_zd_data(DC_GET_MSG,"�����°���");
                WRITEMSG
                goto ErrExit;
            }
            if(iRc!=PCHK_UPDATE && nLineId>1)
            {
                vtcp_log("[%s][%d],��ӵ�ԭ�а���[%d]",__FL__,iRc);
                sprintf( acErrMsg,"[%s][%d]��ӵ�ԭ�а���[%d]",__FL__,g_reply_int);
                strcpy( g_pub_tx.reply, "P040");
                set_zd_data(DC_GET_MSG,"��ӵ�ԭ�а���");
                WRITEMSG
                goto ErrExit;
            }
            if(nLineId==1)
            {
                memcpy(pcPackid ,payin.F_packid ,sizeof(payin.F_packid));
                memcpy(pcPackday,payin.F_packday,sizeof(payin.F_packday));
                vtcp_log("[%s][%d] packid=[%s] packday==[%s]",__FILE__,__LINE__,pcPackid,pcPackday);
            }
            /***�����¼�¼****/
            g_reply_int=iLvUpdRecFromPayin(&payin,1);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d],��iLvUpdRecFromPayin��[%d]",__FL__, g_reply_int);
                sprintf( acErrMsg,"[%s][%d]��iLvUpdRecFromPayin��[%d]",__FL__,g_reply_int);
                strcpy( g_pub_tx.reply, "O206");
                set_zd_data(DC_GET_MSG,"����iLvUpdRecFromPayin����");
                WRITEMSG
                goto ErrExit;
            }
            nLineId++;
        }
    }
    if(nLineId>1)
    {
        /**�����hz_dateд����**/
        g_reply_int = sql_execute("update lv_wbctl set stat='2',hz_date='3' where packid='%s' and pack_date='%s'",pcPackid,pcPackday);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],�����������Ʊ�ʧ��[%d]",__FL__, g_reply_int);
            sprintf(acErrMsg,"[%s][%d]�����������Ʊ�ʧ��",__FL__);
            strcpy( g_pub_tx.reply, "O206");
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] packid=[%s] packday==[%s]",__FILE__,__LINE__,pcPackid,pcPackday);
    }
    
    /**��ǰ̨��������**/
    apitoa(nLineId-1,4,cTimes);
    zip_space(cTotamt);
    str2dbl(cTotamt,strlen(cTotamt),0,&cAmt);
    dTotamt /= 100;
    if(dTotamt != cAmt)
    {
         vtcp_log("[%s][%d],�������[%f]<>[%f]",__FL__,cAmt,dTotamt);
         sprintf(acErrMsg,"[%s][%d]���������ļ��еĲ�һ��",__FL__);
         strcpy( g_pub_tx.reply, "O206");
         set_zd_data(DC_GET_MSG,acErrMsg);
         WRITEMSG
         goto ErrExit;
    }
    vtcp_log("[%s][%d]ǰ��̨�Ľ��һ��!",__FILE__,__LINE__);
    set_zd_data("0360",cTimes);     /**�õ��ܵļ�¼��**/
    set_zd_double("0430",dTotamt);  /**�õ��ܵĽ����**/
    set_zd_data("0470",pcPackday);  /**�õ�������    **/
    set_zd_data("1280",pcPackid);   /**�õ������    **/
    set_zd_data("0460",pcPackid);   /**�õ������  mod by chenggx 20110610   **/
    fclose(fpr);
	/*beg add by Chenggx 2011-6-16  ���� ���̳ɹ����ɱ����ļ�*
	{
		FILE   *fp;
		int ret=0,cnt;
		char *pBuf;
		char  o_packday   [9];  
		char  o_orderno    [9];
		char  sLineBuf[256];
		char  sBuf[65];
		char  sFileName[256];
		
		struct lv_pldsr_c  lv_pldsr;
		
		memset(o_packday,0,sizeof(o_packday));
		memset(o_orderno,0,sizeof(o_orderno));
		memset(sLineBuf,0,sizeof(sLineBuf));
		memset(sBuf,0,sizeof(sBuf));
		memset(sFileName,0,sizeof(sFileName));
		memset(&lv_pldsr,0,sizeof(lv_pldsr));
		vtcp_log("[%s][%d] ����ǰ̨�������ļ� ���ļ��ڶ��ж��� ԭ����ί�����ڣ������pcFilename[%s]\n",
		__FILE__,__LINE__,pcFilename);
		vtcp_log("[%s][%d]��������[%s]!",__FILE__,__LINE__,cKinbr);
		vtcp_log("[%s][%d]������Ա[%s]!",__FILE__,__LINE__,cTlrno);
		if((fpr=fopen(pcFilename,"rb"))==NULL) 
		{
			vtcp_log("[%s][%d] ���̳ɹ�,������Ӧ����ʧ��\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"[%s][%d]���̳ɹ�,������Ӧ����ʧ��",__FL__);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto OkExit;
		}
		memset(sLineBuf,0,sizeof(sLineBuf));
		fgets(sLineBuf,sizeof(sLineBuf),fpr);
		vtcp_log("[%s][%d]��һ��[%s]!",__FILE__,__LINE__,sLineBuf);
		
		memset(sLineBuf,0,sizeof(sLineBuf));
		fgets(sLineBuf,sizeof(sLineBuf),fpr);
		fclose(fpr);
		pBuf = strtokk(sLineBuf, "|");
		pBuf = strtokk(NULL,"|");
		pBuf = strtokk(NULL,"|");
		pBuf = strtokk(NULL,"|");
		pBuf = strtokk(NULL,"|");
		pBuf = strtokk(NULL,"|");
		pBuf = strtokk(NULL,"|");
		memset(sBuf,0,sizeof(sBuf));
		memcpy(sBuf,pBuf,24);
		vtcp_log("[%s][%d]sBuf ԭ����������Ϣ[%s]!",__FILE__,__LINE__,sBuf);
		memcpy(o_packday,sBuf,8);
		memcpy(o_orderno,sBuf+8,8);
		ret=Lv_pldsr_Dec_Sel(g_pub_tx.reply," wt_date='%s' and orderno = '%s' and yw_type='%s' and tx_mx_no!='00000000' ",o_packday,o_orderno,cYw_type);
		if(ret)
		{
			vtcp_log("[%s][%d] ���̳ɹ�,������Ӧ����ʧ��\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"[%s][%d]���̳ɹ�,������Ӧ����ʧ��",__FL__);
			strncpy(g_pub_tx.reply,"P090",4);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
		}
		memset(sFileName,0,sizeof(sFileName));
		sprintf(sFileName,"%s/tmp/%s_%s_%s",getenv("WORKDIR"),cKinbr,o_packday,o_orderno);
		zip_space(sFileName);
		vtcp_log("[%s][%d] sFileName==[%s] \n",__FL__,sFileName);   
		if((fp=fopen(sFileName,"wb"))==NULL) 
		{
			vtcp_log("[%s][%d] ���̳ɹ�,������Ӧ����ʧ��\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"[%s][%d]���̳ɹ�,������Ӧ����ʧ��",__FL__);
			strncpy(g_pub_tx.reply,"P090",4);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			Lv_pldsr_Clo_Sel();
			goto ErrExit;
		}
		cnt=0;
		while(1)
		{
			
			memset(&lv_pldsr,0,sizeof(lv_pldsr));
			ret=Lv_pldsr_Fet_Sel(&lv_pldsr,g_pub_tx.reply);
			if(ret == 100){
				if(cnt==0){
					vtcp_log("[%s][%d] ���̳ɹ�,������Ӧ����ʧ��\n",__FILE__,__LINE__);
					sprintf( acErrMsg,"[%s][%d]���̳ɹ�,������Ӧ����ʧ��",__FL__);
					strncpy(g_pub_tx.reply,"P090",4);
					set_zd_data(DC_GET_MSG,acErrMsg);
					WRITEMSG
					fclose(fp);
					Lv_pldsr_Clo_Sel();
					goto ErrExit;
				}
				break;
			}
			else if(ret)
			{
				vtcp_log("[%s][%d] ���̳ɹ�,������Ӧ����ʧ��\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"[%s][%d]���̳ɹ�,������Ӧ����ʧ��",__FL__);
				strncpy(g_pub_tx.reply,"P090",4);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				fclose(fp);
				Lv_pldsr_Clo_Sel();
				goto ErrExit;
			}	
			vtcp_log(" [%s]--[%d] 123321",__FILE__,__LINE__);
			fprintf(fp,"%s|%s|%ld|%.2lf|%s\n",lv_pldsr.packid,lv_pldsr.orderno,lv_pldsr.wt_date,lv_pldsr.tx_amt,lv_pldsr.tx_mx_no); 
			cnt++;
		}
		Lv_pldsr_Clo_Sel();
		fclose(fp);

	}
	*beg add by Chenggx 2011-6-16  ����*/
	/*beg add by Chenggx 2011-6-16  ���� �Ǽ���ˮ*/
	/* �Ǽǽ�����ˮ */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
			goto ErrExit;
	}
    /*beg add by Chenggx 2011-6-16  ����*/
    /*beg add by ChengGX 20100605 ��ˮ ��� ���ڽ�ǵ��̳�������ظ���������*/
    memset(cStr,0, sizeof(cStr));
    sprintf(cStr,"mv %s %s/tmp/dsf/",pcFilename,getenv("WORKDIR"));
    vtcp_log("[%s][%d] cStr==[%s] pcFilename[%s]\n",__FILE__,__LINE__,cStr,pcFilename);
    system(cStr);
    /*end add by ChengGX 20100605 ��ˮ ��� ���ڽ�ǵ��̳�������ظ���������*/
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
/*�����ʺ�,�������жϸ��ʻ���״̬*/
int iGetActnoSts1(char *ac_no,double * avbal)
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
          vtcp_log("[%s][%d]��ѯmdm_ac_rel����[%d]!",__FILE__,__LINE__,ret);
          sprintf(acErrMsg,"��ѯmdm_ac_rel����!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          return -1;
    }
    if(sMdm_ac_rel.note_sts[0]!='0') /**����״̬**/
    {
         vtcp_log("[%s][%d]mdm_ac_rel.note_sts==[%c]������!",__FILE__,__LINE__,sMdm_ac_rel.note_sts[0]);
         sprintf(acErrMsg,"mdm_ac_rel.note_sts==[%c]������!",sMdm_ac_rel.note_sts[0]);
         set_zd_data(DC_GET_MSG,acErrMsg);
         return -1;
    }
    ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
    if(ret)
    {
          vtcp_log("[%s][%d]��ѯprdt_ac_id����[%d]!",__FILE__,__LINE__,ret);
          sprintf(acErrMsg,"��ѯprdt_ac_id����!![%d]",ret);
          set_zd_data(DC_GET_MSG,acErrMsg);
          WRITEMSG
          return -1;
    }
    switch(sPrdt_ac_id.ac_id_type[0])
    {
        case '1':/*����*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);  
            if(ret)
            {
                  vtcp_log("[%s][%d]��ѯdd_mst����[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"��ѯdd_mst����!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sDd_mst.ac_sts[0]!='1')/*���ڵ��ʻ�״̬*/
            {
                  vtcp_log("[%s][%d]���ڵ��ʻ�״̬ac_sts[%c]������!",__FILE__,__LINE__,sDd_mst.ac_sts[0]);
                  sprintf(acErrMsg,"sDd_mst.ac_sts==[%c]������!",sDd_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            if(sDd_mst.hold_sts[0]!='0')/*���ڵĶ���״̬*/
            {
                  vtcp_log("[%s][%d]���ڵĶ���״̬hold_sts[%c]������!",__FILE__,__LINE__,sDd_mst.hold_sts[0]);
                  sprintf(acErrMsg,"sDd_mst.hold_sts==[%c]������!",sDd_mst.hold_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sDd_mst.bal;
            break;
        case '2':/*���ڴ��*/
            ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  vtcp_log("[%s][%d]��ѯtd_mst����[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"��ѯtd_mst����!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sTd_mst.ac_sts[0]!='1')/*���ڵ��ʻ�״̬*/
            {
                  vtcp_log("[%s][%d]���ڵ��ʻ�״̬ac_sts[%c]������!",__FILE__,__LINE__,sTd_mst.ac_sts[0]);
                  sprintf(acErrMsg,"sTd_mst.ac_sts==[%c]������!",sTd_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            if(sTd_mst.hold_sts[0]!='0')/*���ڵĶ���״̬*/
            {
                  vtcp_log("[%s][%d]���ڵĶ���״̬hold_sts[%c]������!",__FILE__,__LINE__,sTd_mst.hold_sts[0]);
                  sprintf(acErrMsg,"sTd_mst.hold_sts==[%c]������!",sTd_mst.hold_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sTd_mst.bal;
            break;
        case '3':/*����*/
            ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  sprintf(acErrMsg,"��ѯln_mst����!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sLn_mst.ac_sts[0]!='1')/*������ʻ�״̬*/
            {
                  sprintf(acErrMsg,"sLn_mst.ac_sts==[%c]������!",sLn_mst.ac_sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sLn_mst.bal;
            break;
        case '9':/*�ڲ���*/
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst," ac_id=%ld",sPrdt_ac_id.ac_id);   
            if(ret)
            {
                  vtcp_log("[%s][%d]��ѯtd_mst����[%d]!",__FILE__,__LINE__,ret);
                  sprintf(acErrMsg,"��ѯin_mst����!![%d]",ret);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  WRITEMSG
                  return -1;
            }
            if(sIn_mst.sts[0]!='1')/*�ڲ��ʵ��ʻ�״̬*/
            {
                  vtcp_log("[%s][%d]�ڲ��ʵ��ʻ�״̬[%c]������!",__FILE__,__LINE__,sIn_mst.sts[0]);
                  sprintf(acErrMsg,"sIn_mst.sts==[%c]������!",sIn_mst.sts[0]);
              set_zd_data(DC_GET_MSG,acErrMsg);
              return -1;
            }
            *avbal=sIn_mst.bal;
            break;
        default:
            vtcp_log("[%s][%d]��Ч���ʻ�����[%c]!",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
            sprintf(acErrMsg,"��Ч���ʻ�����");
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            return -1;
            break;
    }
    vtcp_log("[%s][%d] ���ʻ������Ϊavbal ==[%f]",__FILE__,__LINE__,*avbal);
    return 0;
}
