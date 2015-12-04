/***************************************************************************/
/* �ļ�����: spJ078.pc                                                   */
/* ��������: С����ܶ��ʽ���                                              */
/* ��������: 2006.01.11                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_sysctl_c.h"
#include "lv_chk842_m_c.h"
#include "lv_chk842_d_c.h"
#include "lv_define.h"
#include "syyh_czyh_qs_reg_c.h"
#include "hv_orno_brno_c.h"
#include "com_sys_parm_c.h"
#include "lv_chk851_d_c.h"

struct lv_chk842_m_c sLv_chk842_m;
struct lv_chk842_d_c sLv_chk842_d;
static struct lv_sysctl_c g_lv_sysctl;

#define MAX_CNT 5
static     int  iErrcnt=0;
int i6594_Initial();
int i6594_Verify();
int i6594_Process();
int nUpdLbctlQsdate();
int spJ078(){
    int ret=0;
    struct hv_orno_brno_c sHv_orno_brno;
    struct com_sys_parm_c com_sys_parm;
    memset(&sHv_orno_brno , 0 , sizeof(sHv_orno_brno));
    memset(&com_sys_parm, 0 , sizeof(com_sys_parm));
   
 
    vtcp_log("[%s][%d] С������˿�ʼ��!!",__FILE__,__LINE__);
    
    g_reply_int = i6594_Initial();
    if(g_reply_int){
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
            memcpy(g_pub_tx.reply,"P083",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    /****delete at 20120201
    if(iLvchkstat())
    {
        strncpy(g_pub_tx.reply,"P046",4);
        WRITEMSG
        goto ErrExit;
    }
    ****/

    /****�޸�Ϊֻ���com_sys_parm��״̬��lv_sysctl���κ�״̬�����Զ���  begin at 20120201****/
    ret=Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
    if(ret!=0){
      vtcp_log("%s,%d,ȡ�ù���ϵͳ���������,sqlcode=%d\n",__FILE__,__LINE__,ret);
      strncpy(acErrMsg,"ȡ�ù���ϵͳ���������",40);
      return(-1);
    }
    if(com_sys_parm.sys_sts!=0)
    {
      vtcp_log("%s,%d,��ǰ״̬���ɷ���С��ҵ��,sqlcode=%d\n",__FILE__,__LINE__,ret);
      strncpy(acErrMsg,"��ǰ״̬���ɷ���С��ҵ��",40);
      set_zd_data("0130",acErrMsg);
      return(-1);
    }
    /****end at 20120201****/


    g_reply_int = i6594_Verify();
    if(g_reply_int){
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
            memcpy(g_pub_tx.reply,"P083",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    /* TODO ����������е��������� */
    /* begin add by LiuHuafeng 2009-5-15 9:54:00 for �������� */
    if((char *)getenv("CZYH")!=NULL && strncmp((char *)getenv("CZYH"),"CZYH_YES",8)==0)
    {
        vtcp_log("%s,%d �Ǵ�������",__FILE__,__LINE__);
    }
    else
    {
        vtcp_log("[%s][%d] ׼��ѭ������������е�����!!!",__FILE__,__LINE__);
        g_reply_int=Hv_orno_brno_Dec_Sel( g_pub_tx.reply, " br_no like '9999_' ");
        while(1)
        {
             memset(&sHv_orno_brno , 0 , sizeof(sHv_orno_brno));
             g_reply_int=Hv_orno_brno_Fet_Sel( &sHv_orno_brno,g_pub_tx.reply );
             if( g_reply_int==100 ) break;
             if( g_reply_int )
             {
                  sprintf(acErrMsg," ���Ҵ������л��� ���� !! ");
                  WRITEMSG 
                  goto ErrExit;  
             }
             /* ��ҵ����Ҫ����������з������������� modify by xyy 2010-6-20 11:01:07*/
             vtcp_log("%s,%d ��Ȫ���У���Ҫ�������[%s]���е�����!",__FILE__,__LINE__,sHv_orno_brno.br_no);
             g_reply_int = pubf_com_lv_read_dz(sHv_orno_brno.br_no);
             if(g_reply_int)
             {
                 vtcp_log("%s,%d ����������ж����ļ�ʧ��",__FILE__,__LINE__);
                 memcpy(g_pub_tx.reply,"P083",4);
                 WRITEMSG
                 goto ErrExit;
             }
             vtcp_log("[%s][%d] [%s]�������е������!!",__FILE__,__LINE__,sHv_orno_brno.br_no);
        }
        Hv_orno_brno_Clo_Sel();
    }
    /* end by LiuHuafeng 2009-5-15 9:54:14 */
    g_reply_int = i6594_Process();
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    if(g_reply_int){
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
            memcpy(g_pub_tx.reply,"P083",4);
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    /* ����ˮ��־ */
    g_reply_int = pub_ins_trace_log();
    if ( g_reply_int )
    {
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
            memcpy(g_pub_tx.reply,"P083",4);
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        sprintf( acErrMsg, "����ˮ��־��" );
        WRITEMSG
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P031");
        goto ErrExit;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    GoodExit:
        strcpy( g_pub_tx.reply, "0000" );
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
    ErrExit:
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}

int i6594_Initial()
{
    memset(&sLv_chk842_m,0 ,sizeof(sLv_chk842_m));
    memset(&sLv_chk842_d,0 ,sizeof(sLv_chk842_d));
    memset(&g_lv_sysctl, 0 , sizeof(g_lv_sysctl));
    return 0;
}
int i6594_Verify()
{
    char cQsbrno[BRNO_LEN+1];
    char cZcday[9];
    int  iCount=0;
    char cKinbr[BRNO_LEN+1];
    char cTxday[9];
    memset(cKinbr, 0 , sizeof(cKinbr));
    memset(cZcday, 0 , sizeof(cZcday));
    memset(cQsbrno,'\0',sizeof(cQsbrno));
    memset(cTxday, 0 , sizeof(cTxday));
    iFind_qsbrno(cQsbrno);
    get_zd_data(DC_TX_BR_NO,cKinbr);
    pub_base_sysinit();

    if(memcmp(cKinbr,cQsbrno,BRNO_LEN))
    {
        WRITEMSG
        vtcp_log("[%s][%d] ���������Ĳ��������˽���\n",__FILE__,__LINE__);
        WRITEMSG
        strncpy(acErrMsg, "���������Ĳ��������˽���!", 40);
        WRITEMSG
        return -1;
    }
    g_reply_int =  Lv_sysctl_Sel(g_pub_tx.reply,&g_lv_sysctl,"1=1");
    if(g_reply_int)
    {
        vtcp_log("%s,%d С��ϵͳ���ڱ���� g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P082");
        return g_reply_int;
    }
    memcpy(cZcday,g_lv_sysctl.filler,sizeof(cZcday)-1);
    
    get_zd_data(DC_DATE,cTxday);
    if(!memcmp((char *)pcLvGetDay_work(),cTxday,8))
    {
        WRITEMSG
        vtcp_log("[%s][%d] С��֧��ϵͳ�������ۺ�ҵ������һֱ�������Զ��� \n",__FILE__,__LINE__);
        WRITEMSG
        strncpy(acErrMsg, "δ�������в����Զ���!", 40);
        set_zd_data("0130",acErrMsg);
        WRITEMSG
        return -1;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    iCount = sql_count("lv_chk842_m","dz_date=%s",cTxday);
    if(iCount==0){
        sprintf(acErrMsg,"�������û�������С��ҵ��ɲ��ö�����");
        set_zd_data("0130",acErrMsg);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P085");
        return -1;
    }
    iCount = sql_count("lv_chk842_d","zc_date=%s",cTxday);
    if(iCount==-1){
        sprintf(acErrMsg,"sql_count err");
        set_zd_data("0130",acErrMsg);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P026");
        return -1;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    if(iCount==0){
        sprintf(acErrMsg,"û���յ�С��֧����ϸ�����Զ���");
        set_zd_data("0130",acErrMsg);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P049");
        return -1;
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    return 0;
}
int i6594_Process()
{
    char cTxday[9];
    double dAmt=0,dCnt=0;    
    char cPkgno[4];
    int  iCnt=0;
    int  ttlnum=0,iCheckOK=0;
    char cFilename[512];
    FILE *fp;
    ttlnum=0;
    
    struct hv_orno_brno_c sHv_Or_Brno;
    memset(cTxday, 0 , sizeof(cTxday));
    get_zd_data(DC_DATE,cTxday);
    memset(cPkgno, 0 , sizeof(cPkgno));
    
    /* ��������ζ��ܽ��,����ccpc�����ݵ����ǵ���������ȥ�ң����������һ���������
        ���������Ѿ������˵���ccpc��Ȼû�����㣬�Ҳ����ڣ����ʾ�û���Գ�����������Ϊ
        �������������
     */
vtcp_log("%s,%d ",__FILE__,__LINE__);
    /* begin add by wangjian 20110701
    * �ȸ���һ�����˵��������� ������������״̬����������״̬������������Ϊ0��
    */
    g_reply_int = nUpdLbctlQsdate();
    if(g_reply_int)
    {
        sprintf(acErrMsg,"����851��������״̬����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P049");
        return g_reply_int;
    }
    /* end add by wangjian 20110701 */
    g_reply_int =  Lv_chk842_d_Dec_Upd(g_pub_tx.reply," 1=1 order by zc_date,pkgno,pack_sts");
    if(g_reply_int)
    {
        sprintf(acErrMsg,"��ȡ842���ʱ����");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P049");
        return g_reply_int;
    }
    
vtcp_log("%s,%d ",__FILE__,__LINE__);    

        memset(cFilename, 0 , sizeof(cFilename));
        pub_base_AllDwnFilName(cFilename);
        fp = fopen(cFilename,"w");
        if( fp==NULL )
        {
            sprintf(acErrMsg," open file [%s] error ",cFilename);
            WRITEMSG
            strcpy( g_pub_tx.reply,"S047" );
            return -1;
        }

    while(1)
    {
vtcp_log("%s,%d ",__FILE__,__LINE__);
        g_reply_int =  Lv_chk842_d_Fet_Upd(&sLv_chk842_d,g_pub_tx.reply);
        if(g_reply_int==100) break;
        if(g_reply_int)
        {
            sprintf(acErrMsg,"��ȡ842���ʱ��α����");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P049");
            return g_reply_int;
        }
vtcp_log("%s,%d ",__FILE__,__LINE__);
        /* �ȼ������ */
        dAmt=0,dCnt=0;
        vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
        g_reply_int=iUpdateChk842LWValue(&sLv_chk842_d);
        if(g_reply_int!=100 && g_reply_int!=0)
        {
            WRITEMSG
            vtcp_log("[%s][%d] ����842���ʶԱ���� sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            return g_reply_int;
        }
        g_reply_int =  Lv_chk842_d_Upd_Upd(sLv_chk842_d,g_pub_tx.reply);
        if(g_reply_int!=100 && g_reply_int!=0)
        {
            WRITEMSG
            vtcp_log("[%s][%d] ����842���ʶԱ���� sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            return g_reply_int;
        }
    }
vtcp_log("%s,%d ",__FILE__,__LINE__);
    Lv_chk842_d_Clo_Upd();
    /* ���842���ж��Ƿ���ʶ�ƽ */
    iErrcnt=0;
vtcp_log("%s,%d ",__FILE__,__LINE__);
    iErrcnt=sql_count("lv_chk842_d","dz_wb_cnt<>wb_cnt or dz_wb_tx_amt<>wb_tx_amt or dz_lb_cnt<>lb_cnt or dz_lb_tx_amt<>lb_tx_amt");
    if(iErrcnt<0)
    {
            WRITEMSG
            vtcp_log("[%s][%d] ����842���ʶԱ���� sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            return g_reply_int;
    }
vtcp_log("%s,%d ",__FILE__,__LINE__);
    iCnt=0;
    if(iErrcnt!=0)
    {
        int iCntbank=0.0;
        int iCntccpc=0.0;
vtcp_log("%s,%d ",__FILE__,__LINE__);
        /* ��ʼ���254���Ľ������ */
        iCntccpc=sql_count("lv_c254r","qs_date=%d and zc_amt<>0",atoi(cTxday));
        if(iCntccpc<0)
        {
            vtcp_log("[%s][%d] ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntccpc);
            WRITEMSG
            strncpy(acErrMsg, "��254���㱨�ı����!", 40);
            WRITEMSG
            return -1;
        }
vtcp_log("%s,%d ",__FILE__,__LINE__);
        iCntbank=sql_count("(select count(*)  from lv_chk842_d  group by zc_jd_lx,zc_date,zc_cc having sum(decode(pkgno,'009',-lb_tx_amt,lb_tx_amt)) <>sum(decode(pkgno,'009',-wb_tx_amt,wb_tx_amt)))","1=1");
        if(iCntbank<0)
        {
            vtcp_log("[%s][%d] ����842ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntbank);
            WRITEMSG
            strncpy(acErrMsg, "��842���ı�ͳ��254��������!", 40);
            WRITEMSG
            return -1;
        }
vtcp_log("%s,%d ",__FILE__,__LINE__);
        /* �������㲻ƽ��� */
        g_reply_int =  Lv_chk842_d_Dec_Sel(g_pub_tx.reply,"dz_wb_cnt<>wb_cnt or dz_wb_tx_amt<>wb_tx_amt or dz_lb_cnt<>lb_cnt or dz_lb_tx_amt<>lb_tx_amt");
        if(g_reply_int)
        {
            vtcp_log("[%s][%d] ����842ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
            WRITEMSG
            strncpy(acErrMsg, "��842���ı�ͳ�Ʋ�ƽ���α����!", 40);
            WRITEMSG
            return g_reply_int;
        }
        ttlnum=0;
        while(1)
        {
            g_reply_int =  Lv_chk842_d_Fet_Sel(&sLv_chk842_d,g_pub_tx.reply);
            if( g_reply_int==100 )break;
            if( g_reply_int!=0 )
            {
                sprintf(acErrMsg,"���Ҽ�¼����[%d]",g_reply_int);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                strcpy(g_pub_tx.reply,"D103");
                return g_reply_int;
            }
            if( !ttlnum )
            {
                fprintf( fp,"~������|�����|��������|����ڵ�����|��������|��������|������������|������������|��������ܼ�|��������ܼ�|������������ܼ�|������������ܼ�|\n" );
                if(iCntbank!=iCntccpc)
                {
                    fprintf( fp,"254|δ��ȫ  |%s|0|�յ�|%d��|Ӧ��|%d��|����Ƽ���ϵ|  |  |  |\n",cTxday,iCntccpc,iCntbank);
                    ttlnum++;
                }
            }
            fprintf( fp,"%s|%s|%ld|%s|%ld|%ld|%ld|%ld|%.2f|%.2f|%.2f|%.2f|\n",sLv_chk842_d.pkgno,sLv_chk842_d.zc_cc,sLv_chk842_d.zc_date,sLv_chk842_d.zc_jd_lx,sLv_chk842_d.lb_cnt,sLv_chk842_d.wb_cnt,sLv_chk842_d.dz_lb_cnt,sLv_chk842_d.dz_wb_cnt,sLv_chk842_d.lb_tx_amt/100,sLv_chk842_d.wb_tx_amt/100,sLv_chk842_d.dz_lb_tx_amt/100,sLv_chk842_d.dz_wb_tx_amt/100    );
            ttlnum++;
        }
        Lv_chk842_d_Clo_Sel();
    }
    else
    {
        g_reply_int =  Lv_sysctl_Dec_Upd(g_pub_tx.reply,"1=1");
        if(g_reply_int)
        {
            vtcp_log("%s,%d С��ϵͳ���ڱ���� g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P082");
            return g_reply_int;
        }
        g_reply_int =  Lv_sysctl_Fet_Upd(&g_lv_sysctl,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("%s,%d С��ϵͳ���ڱ���� g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P082");
            return g_reply_int;
        }
        g_lv_sysctl.checkstat[0]='0';
        g_reply_int =  Lv_sysctl_Upd_Upd(g_lv_sysctl,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("%s,%d С��ϵͳ���ڱ���� g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P082");
            return g_reply_int;
        }
        ttlnum=2;
        vtcp_log("%s %d ���ʳɹ�",__FILE__,__LINE__);
    }
    vtcp_log("%s %d ����over",__FILE__,__LINE__);
    if( !ttlnum )
    {
        vtcp_log("%s %d ����over",__FILE__,__LINE__);
        strcpy( g_pub_tx.reply,"S049" );
        fclose(fp);
        return -1;
    }
    else
    {
        int iCntbank=0.0;
        int iCntccpc=0.0;
        vtcp_log("%s %d ����over",__FILE__,__LINE__);
        vtcp_log("%s %d ����over",__FILE__,__LINE__);
vtcp_log("%s,%d ",__FILE__,__LINE__);
        /* ��ʼ���254���Ľ������ */
        iCntccpc=sql_count("lv_c254r","qs_date=%d and zc_amt<>0",atoi(cTxday));
        if(iCntccpc<0)
        {
            vtcp_log("[%s][%d] ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntccpc);
            WRITEMSG
            strncpy(acErrMsg, "��254���㱨�ı����!", 40);
            WRITEMSG
            return -1;
        }
vtcp_log("%s,%d ",__FILE__,__LINE__);
        iCntbank=sql_count("(select count(*)  from lv_chk842_d  group by zc_jd_lx,zc_date,zc_cc having sum(decode(pkgno,'009',-lb_tx_amt,lb_tx_amt)) <>sum(decode(pkgno,'009',-wb_tx_amt,wb_tx_amt)))","1=1");
        if(iCntbank<0)
        {
            vtcp_log("[%s][%d] ����842ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntbank);
            WRITEMSG
            strncpy(acErrMsg, "��842���ı�ͳ��254��������!", 40);
            WRITEMSG
            return -1;
        }
        fprintf( fp,"~������|�����|��������|����ڵ�����|��������|��������|������������|������������|��������ܼ�|��������ܼ�|������������ܼ�|������������ܼ�|\n" );
        if(iCntbank!=iCntccpc)
        {
            fprintf( fp,"254|δ��ȫ  |%s|0|�յ�|%d��|Ӧ��|%d��|����Ƽ���ϵ|  |  |  |\n",cTxday,iCntccpc,iCntbank);
            ttlnum++;
        }
        else
        {
            if(iErrcnt!=0){
            }
            else
            {
                iCheckOK=1;
                fprintf( fp,"  ��ϲ|������ƽ,�����������������||||||%||  |  |  |\n");
            }
        }
        fclose(fp);
        set_zd_data( DC_FILE_SND,"1");
        if(iCheckOK == 1)
        {
            struct lv_wbctl_c sWbctl;
            struct lv_lbctl_c sLbctl;
            char cBuf[1024];
            char cBasename[100];
            char cFullname[100];
            char cCmd[300];
            FILE * fp;
            int ret=0,iCnt=0;
            double dLbamt=0.0;
            double dWb_totamt=0.0;
            double dLb_totamt=0.0;
            struct syyh_czyh_qs_reg_c sQsreg;
            /* begin add by LiuHuafeng 2010-9-27 10:23:06 */
            char cOldbrno[BRNO_LEN+1];
            /* end by LiuHuafeng 2010-9-27 10:23:13 */
            
            memset(cOldbrno, 0 , sizeof(cOldbrno));
            vtcp_log("[%s][%d] ׼��ѭ������������е�����!!!",__FILE__,__LINE__);
            /* rem by LiuHuafeng 2010-9-27 10:22:29
             *ret=Hv_orno_brno_Dec_Sel( g_pub_tx.reply, " br_no like '9999_' ");
             * replace by next line **/
            ret=Hv_orno_brno_Dec_Sel( g_pub_tx.reply, " br_no like '9999_' order by br_no");
            while(1)
            {
                memset(&sQsreg , 0 , sizeof(sQsreg));
                memset(cFullname, 0 , sizeof(cFullname));
                memset(cBasename, 0 , sizeof(cBasename));
                memset(cBuf, 0 , sizeof(cBuf));
                memset(cCmd , 0 , sizeof(cCmd));
                memset(&sWbctl , 0 , sizeof(sWbctl));
                memset(&sLbctl , 0 , sizeof(sLbctl));
                
                memset(&sHv_Or_Brno , 0 , sizeof(sHv_Or_Brno));

                ret=Hv_orno_brno_Fet_Sel( &sHv_Or_Brno,g_pub_tx.reply );
                if( ret==100 ) break;
                if( ret )
                {
                     sprintf(acErrMsg," ���Ҵ������л��� ���� !! ");
                     WRITEMSG 
                     return -1;
                }


                vtcp_log("[%s][%d] [%s]�������п�ʼ�ǼǴ������Ǽǲ�!!",__FILE__,__LINE__,sHv_Or_Brno.br_no);
                
                /* ׼���´��Ķӳ��ļ� */
                sprintf(cBasename,"lv_xc_dz_%s_%08ld.txt",sHv_Or_Brno.br_no,g_pub_tx.tx_date);
                sprintf(cFullname,"%s/lv_xc_dz_%s_%08ld.txt",(char*)getenv("YQSH_PATH"),sHv_Or_Brno.br_no,g_pub_tx.tx_date);
                sprintf(cCmd,"ftpsend %s %s",(char*)getenv("YQSH_PATH"),cBasename);

                vtcp_log("[%s][%d] fullname=[%s]",__FILE__,__LINE__,cFullname);
                vtcp_log("[%s][%d] cCmd=[%s]",__FILE__,__LINE__,cCmd);
                /* begin add by LiuHuafeng 2009-5-14 20:34:09 for �������� */
                /* 1 ���Ѿ����������������йص������Ŵ���ȥ�����ô�����������״̬Ϊ����
                 * 2 ���Ѿ����������������йص������Ŵ���ȥ���û��������������˶Ժ������·�
                 */
                if(strlen(cOldbrno)==0)
                {
                  memcpy(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN);
                  vtcp_log("%s,%d ��һ�δ��ļ�  %s",__FILE__,__LINE__,cFullname);
                  fp = fopen(cFullname,"w");
                }
                else if(memcmp(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN)!=0)
                {
                  fclose(fp);
                  fp = fopen(cFullname,"w");
                  vtcp_log("%s,%d ԭ�ļ��رգ��´��ļ�  %s",__FILE__,__LINE__,cFullname);
                  memcpy(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN);
                  dWb_totamt=0.0;
                  dLb_totamt=0.0;
                }
                
                if(fp==NULL)
                {
                    vtcp_log("[%s][%d]���ļ�ʧ��!\n",__FILE__,__LINE__);
                    strncpy(g_pub_tx.reply,"P157",4);
                    return -1;
                }
                 
                ret = Lv_wbctl_Dec_Sel( g_pub_tx.reply , "qs_date=%ld and br_no='%s' and packstat='05' " ,g_pub_tx.tx_date, sHv_Or_Brno.br_no );
                if (ret){
                    vtcp_log("%s,%d ���α����",__FILE__,__LINE__);
                    WRITEMSG
                    return ret;
                }
                while(1)
                {
                    ret = Lv_wbctl_Fet_Sel( &sWbctl , g_pub_tx.reply );
                    if(ret == 100)
                        break;
                    if (ret){
                        vtcp_log("%s,%d ���Ҵ��������������� ret =%d",__FILE__,__LINE__,ret);
                        WRITEMSG
                        fclose(fp);
                        return ret;
                    }
                    sprintf(cBuf,"1|%08ld|%.08s|%s|%ld|%.2f|%.012s|",sWbctl.pack_date,sWbctl.packid,sWbctl.pkgno,sWbctl.dtlcnt,sWbctl.totamt,sWbctl.pay_qs_no);
                    dWb_totamt = dWb_totamt + sWbctl.totamt;
                    fprintf(fp,"%s\n",cBuf);
                }
                Lv_wbctl_Clo_Sel();
                /* �������� */
                ret = Lv_lbctl_Dec_Sel( g_pub_tx.reply , "qs_date=%ld and packstat='05' " ,g_pub_tx.tx_date );
                if (ret){
                    WRITEMSG
                    fclose(fp);
                    return ret;
                }
                while(1)
                {
                    dLbamt = 0.0;
                    iCnt = 0;
                    memset(&sLbctl, 0 , sizeof(sLbctl));
                    ret = Lv_lbctl_Fet_Sel( &sLbctl , g_pub_tx.reply );
                    if(ret == 100)
                        break;
                    if (ret){
                        vtcp_log("%s,%d ���Ҵ��������������� ret =%d",__FILE__,__LINE__,ret);
                        WRITEMSG
                        fclose(fp);
                        return ret;
                    }
                    /* ������������������Բ�ͬ���У�������ǡ��һ�����ͻ���ֲ��ظ������2010-12-30 11:14:59 �����Ҫ���Ӱ�������������Ϊ�����Ĳ�ѯ���޸�����
                     *iCnt= sql_count_sum_double("lv_pkgreg","tx_amt",&dLbamt,"packid = '%s' and pack_date=%ld and br_no='%s'",sLbctl.packid,sLbctl.pack_date,sHv_Or_Brno.br_no);
                     * replace by next line ***/
                     /****��ѯ����������������־Ϊ"2"(����)  modify at 20130409****/
                    iCnt= sql_count_sum_double("lv_pkgreg","tx_amt",&dLbamt,"packid = '%s' and pack_date=%ld and br_no='%s' and pay_qs_no='%s' and lw_ind ='2' ",sLbctl.packid,sLbctl.pack_date,sHv_Or_Brno.br_no,sLbctl.pay_qs_no);
                    if(iCnt>0)
                    {
                        sprintf(cBuf,"2|%08ld|%08s|%s|%ld|%.2f|%.012s|",sLbctl.pack_date,sLbctl.packid,sLbctl.pkgno,iCnt,dLbamt,sLbctl.pay_qs_no);
                        fprintf(fp,"%s\n",cBuf);
                    }
                    else
                        dLbamt = 0.0;
                    dLb_totamt = dLb_totamt + dLbamt;
                }
                Lv_lbctl_Clo_Sel();
                fprintf(fp,"\n");
                fclose(fp);
		/**** 20110513 begin
                ret = system(cCmd);
                if(ret)
                {
                    vtcp_log("%s,%d �´������ļ�����������ʧ��",__FILE__,__LINE__);
                    strncpy(g_pub_tx.reply,"P157",4);
                    return ret;
                }
		end 20110513****/
                /* ���������������������� */
                memset(&sQsreg , 0 , sizeof(sQsreg));
                sQsreg.tx_date=g_pub_tx.tx_date;
                vtcp_log("[%s][%d] tx_date =[%ld]",__FILE__,__LINE__,sQsreg.tx_date);
                sQsreg.flag[0]='2'; /* С�� */
                sQsreg.trace_no=g_pub_tx.trace_no;
                sQsreg.wz_amt = dWb_totamt;
                sQsreg.lz_amt = dLb_totamt;
                sQsreg.zc_amt = dWb_totamt - dLb_totamt;
                sQsreg.qs_flag[0]='1';/* δ���� */
                
                memcpy(sQsreg.br_no , sHv_Or_Brno.br_no , BRNO_LEN);
                
                vtcp_log("[%s][%d] zc_amt =[%f]",__FILE__,__LINE__,sQsreg.zc_amt);
                if(sQsreg.tx_date!=0)
                  ret = Syyh_czyh_qs_reg_Ins( sQsreg , g_pub_tx.reply );
                if(ret)
                {
                    Syyh_czyh_qs_reg_Debug(&sQsreg );
                    sprintf(acErrMsg," д����������������ʧ��![%d] ",ret);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P083" );
                    set_zd_data("0130","д�����ʧ��");
                    return ret;
                }
                vtcp_log("[%s][%d] [%s]��������С������Ǽǲ�д��!!",__FILE__,__LINE__,sQsreg.br_no);
            }
            Hv_orno_brno_Clo_Sel();
        }
        vtcp_log("%s %d ����over",__FILE__,__LINE__);
    }
    vtcp_log("%s %d ����over",__FILE__,__LINE__);
    set_zd_data("0670","0"); /** ����һ�����ʱ�־ **/
    vtcp_log("%s %d ����over",__FILE__,__LINE__);
    return  0;
}


int iUpdateChk842LWValue(struct lv_chk842_d_c *wp_lv_chk842_d)
{
    double dLbAmt=0;
    double dLbCnt=0;
    double dWbAmt=0;
    double dWbCnt=0;
    char cZcday   [9];
    char cZcjdlx  [2];
    char cZccc    [3];
    char cPkgno   [4];
    char cPackstat[3];

    dWbCnt=0.0;
    dLbCnt=0.0;
    dWbAmt=0.0;
    dLbAmt=0.0;
    
    memset(cZcday   , 0 , sizeof(cZcday   ));
    memset(cZcjdlx  , 0 , sizeof(cZcjdlx  ));
    memset(cZccc    , 0 , sizeof(cZccc    ));
    memset(cPkgno   , 0 , sizeof(cPkgno   ));
    memset(cPackstat, 0 , sizeof(cPackstat));
    
    memcpy(cZcjdlx  ,wp_lv_chk842_d->zc_jd_lx,sizeof(cZcjdlx  )-1);
    memcpy(cZccc    ,wp_lv_chk842_d->zc_cc,sizeof(cZccc    )-1);
    memcpy(cPkgno   ,wp_lv_chk842_d->pkgno,sizeof(cPkgno   )-1);
    memcpy(cPackstat,wp_lv_chk842_d->pack_sts,sizeof(cPackstat)-1);
    vtcp_log("[%s][%d] zcday=[%s],zcjdlx=[%s],zccc=[%s],packstat=[%s],packno=[%s]\n",
        __FILE__,__LINE__,cZcday,cZcjdlx,cZccc,cPackstat,cPkgno);
    
    if(!memcmp(cPkgno,"008",3)||!memcmp(cPkgno,"009",3)||!memcmp(cPkgno,"010",3)||!memcmp(cPkgno,"011",3))
    {
        /* ������� */
        if(!memcmp(cPackstat,"05",2))
        {
            g_reply_int=sql_sum_double("lv_wbctl","dtlsucamt",&dWbAmt,"zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ���������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dWbCnt=sql_count("lv_wbctl","zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(dWbCnt<0)
            {
                strncpy(acErrMsg,"ͳ������������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            /**
            EXEC SQL select sum(dtlsucamt),count(dtlsuccnt) into :dWbAmt,:dWbCnt from lv_wbctl 
                where zcday=:cZcday and zcnode=:cZcjdlx
                    and zcid=:cZccc and packstat =:cPackstat
                    and pkgno=:cPkgno;
            **/
            vtcp_log("[%s][%d] wbamt=[%f],wbcnt=[%.0f]\n",__FILE__,__LINE__,dWbAmt,dWbCnt);
        }
        else
        {
            g_reply_int=sql_sum_double("lv_wbctl","dtlsucamt",&dWbAmt,"packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ��������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dWbCnt=sql_count("lv_wbctl","packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(dWbCnt<0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ�����������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            /**
            EXEC SQL select sum(dtlsucamt),count(dtlsuccnt) into :dWbAmt,:dWbCnt from lv_wbctl 
                where packstat =:cPackstat and pkgno=:cPkgno;
            **/
            vtcp_log("[%s][%d] wbamt=[%f],wbcnt=[%.0f]\n",__FILE__,__LINE__,dWbAmt,dWbCnt);
        }
        /* ������� */
        if(!memcmp(cPackstat,"05",2))
        {
            /*
            EXEC SQL select sum(dtlsucamt),count(dtlsuccnt) into :dLbAmt,:dLbCnt from lv_lbctl 
                where zcday=:cZcday and zcnode=:cZcjdlx
                    and zcid=:cZccc and packstat =:cPackstat
                    and pkgno=:cPkgno;
            */
            g_reply_int=sql_sum_double("lv_lbctl","dtlsucamt",&dLbAmt,"zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ���������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dLbCnt=sql_count("lv_lbctl","zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(dLbCnt<0)
            {
                strncpy(acErrMsg,"ͳ������������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            vtcp_log("[%s][%d] lbamt=[%.0f],lbcnt=[%.0f]\n",__FILE__,__LINE__,dLbAmt,dLbCnt);
        }
        else
        {
            g_reply_int=sql_sum_double("lv_lbctl","dtlsucamt",&dLbAmt,"packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ�����������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dLbCnt=sql_count("lv_lbctl","packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(dLbCnt<0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ�����������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            /*
            EXEC SQL select sum(dtlsucamt),count(dtlsuccnt) into :dLbAmt,:dLbCnt from lv_wbctl 
                where packstat =:cPackstat and pkgno=:cPkgno;
            */
        }
    }
    else
    {
        /** ���ʴ��� **/
        if(!memcmp(cPackstat,"05",2))
        {
            /**
            EXEC SQL select sum(totamt),count(*) into :dWbAmt,:dWbCnt from lv_wbctl 
                where zcday=:cZcday and zcnode=:cZcjdlx
                    and zcid=:cZccc and packstat =:cPackstat
                    and pkgno=:cPkgno;
            **/
            g_reply_int=sql_sum_double("lv_wbctl","totamt",&dWbAmt,"zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ���������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dWbCnt=sql_count("lv_wbctl","zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(dWbCnt<0)
            {
                strncpy(acErrMsg,"ͳ������������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            vtcp_log("[%s][%d] wbamt=[%f],wbcnt=[%.0f]\n",__FILE__,__LINE__,dWbAmt,dWbCnt);
        }
        else
        {
            /**
            EXEC SQL select sum(totamt),count(*) into :dWbAmt,:dWbCnt from lv_wbctl 
                where packstat =:cPackstat and pkgno=:cPkgno;
            **/
            g_reply_int=sql_sum_double("lv_wbctl","totamt",&dWbAmt,"packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ��������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dWbCnt=sql_count("lv_wbctl","packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(dWbCnt<0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ�����������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            vtcp_log("[%s][%d] wbamt=[%f],wbcnt=[%.0f]\n",__FILE__,__LINE__,dWbAmt,dWbCnt);
        }
        /** ���ʴ��� **/
        if(!memcmp(cPackstat,"05",2))
        {
            /**
            EXEC SQL select sum(totamt),count(*) into :dLbAmt,:dLbCnt from lv_lbctl 
                where zcday=:cZcday and zcnode=:cZcjdlx
                    and zcid=:cZccc and packstat =:cPackstat
                    and pkgno=:cPkgno;
            ****/
            g_reply_int=sql_sum_double("lv_lbctl","totamt",&dLbAmt,"zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ���������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dLbCnt=sql_count("lv_lbctl","zc_date=%d and zcnode='%s' and zcid='%s' and packstat ='%s' and pkgno='%s'",wp_lv_chk842_d->zc_date,cZcjdlx,cZccc,cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ������������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            vtcp_log("[%s][%d] lbamt=[%.0f],lbcnt=[%.0f]\n",__FILE__,__LINE__,dLbAmt,dLbCnt);
        }
        else
        {
            /**
            EXEC SQL select sum(totamt),count(*) into :dLbAmt,:dLbCnt from lv_wbctl 
                where packstat =:cPackstat and pkgno=:cPkgno;
            **/
            g_reply_int=sql_sum_double("lv_lbctl","totamt",&dLbAmt,"packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(g_reply_int!=0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ��������ʽ�����",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
            dLbCnt=sql_count("lv_lbctl","packstat ='%s' and pkgno='%s'",cPackstat,cPkgno);
            if(dLbCnt<0)
            {
                strncpy(acErrMsg,"ͳ�Ʒ�����������������",50);
                vtcp_log("%s,%d %s",__FILE__,__LINE__,acErrMsg);
                set_zd_data("0130",acErrMsg);
                return g_reply_int;
            }
        }
    }
    wp_lv_chk842_d->dz_wb_cnt=dWbCnt;
    wp_lv_chk842_d->dz_lb_cnt=dLbCnt;
    wp_lv_chk842_d->dz_wb_tx_amt=dWbAmt*100;
    wp_lv_chk842_d->dz_lb_tx_amt=dLbAmt*100;
    vtcp_log("%s,%d d_wbcnt=%ld,d_lbcnt=%ld,d_wbamt=%f,d_lbamt=%f",__FILE__,__LINE__,
        wp_lv_chk842_d->dz_wb_cnt,    wp_lv_chk842_d->dz_lb_cnt,    wp_lv_chk842_d->dz_wb_tx_amt,    wp_lv_chk842_d->dz_lb_tx_amt);
    return 0;
}
                                                                             
    
int nUpdLbctlQsdate()
{
	int ret;
	int flg;
	struct lv_lbctl_c g_lv_lbctl;
	struct lv_chk851_d_c g_lv_chk851_d;
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_lbctl_c));
	memset(&g_lv_chk851_d,0x00, sizeof(g_lv_chk851_d));

	ret = Lv_chk851_d_Dec_Upd(g_pub_tx.reply,"1=1 order by pack_sts");
	if( ret ) {
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"SQLERR [%d]",ret);
		WRITEMSG
		return 1;
	}
	
	flg = 0;
	
	while(1)
	{
		ret = Lv_chk851_d_Fet_Upd(&g_lv_chk851_d,g_pub_tx.reply);
		if( ret==100 )
		{
			if( flg == 0) /* 851N4JU5=  */
			{
    			ret = sql_count("lv_chk842_d","1=1");
				if(ret)
				{
						vtcp_log("[%s][%d] 851�������ݵ�842���� ret===[%d]\n",__FILE__,__LINE__,ret);
						WRITEMSG
						strncpy(acErrMsg, "�����յ��Ķ��˱�����������ϵ�Ƽ�!", 40);
						set_zd_data("0130",acErrMsg);
						memcpy(g_pub_tx.reply,"P083",4);
						return 1;
				}
			}
			break;
		}
		if( ret!=0 )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			memcpy(g_pub_tx.reply,"P083",4);
			return 1;
		}

		flg ++;

		vtcp_log("%s,%d ",__FILE__,__LINE__);
		
		/* ���˲����� */
		if(g_lv_chk851_d.lw_flg[0]=='1')/* send */
			continue;
		
		/* ֻ������� */
		if(g_lv_chk851_d.lw_flg[0]=='2')/* receive */
		{
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_lbctl_Dec_Upd(g_pub_tx.reply,
				"pack_date=%d and packid='%s' and pay_qs_no='%s'",g_lv_chk851_d.pack_date,g_lv_chk851_d.packid,g_lv_chk851_d.pay_qs_no);
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				Lv_chk851_d_Clo_Upd();
				return 1;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				Lv_chk851_d_Clo_Upd();
				sprintf(acErrMsg,"����%sû�������յ�����Ƽ���ϵ",g_lv_chk851_d.packid);
				set_zd_data("0130",acErrMsg);
				return 1;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			memcpy(g_lv_lbctl.packstat,g_lv_chk851_d.pack_sts,sizeof(g_lv_lbctl.packstat)-1);
			/*g_lv_lbctl.dtlsucamt=g_lv_chk851_d.tot_amt;
			g_lv_lbctl.dtlsuccnt=g_lv_chk851_d.dtlcnt;*/
			memcpy(g_lv_lbctl.zcid,g_lv_chk851_d.zc_cc,sizeof(g_lv_lbctl.zcid)-1);
			g_lv_lbctl.zcnode[0]=g_lv_chk851_d.zc_jd_lx[0];
			g_lv_lbctl.zc_date=g_lv_chk851_d.zc_date;
			g_lv_lbctl.qs_date=g_lv_chk851_d.qs_date;

			ret = Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
			if(ret)
			{
				Lv_lbctl_Debug(&g_lv_lbctl);
				Lv_chk851_d_Clo_Upd();
				vtcp_log("%s,%d ������������� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				return 1;
			}

			Lv_lbctl_Clo_Upd();
		}
		
		memcpy(g_lv_chk851_d.dhcc_sts,g_lv_chk851_d.pack_sts,sizeof(g_lv_chk851_d.dhcc_sts));
		ret = Lv_chk851_d_Upd_Upd(g_lv_chk851_d,g_pub_tx.reply);
		if(ret)
		{
			Lv_wbctl_Debug(&g_lv_lbctl);
			Lv_chk851_d_Clo_Upd();
			vtcp_log("%s,%d ����851�α���� ret=%d",__FILE__,__LINE__,ret);
			WRITEMSG
			return 1;
		}
	}
	Lv_chk851_d_Clo_Sel();
	return 0;
}
