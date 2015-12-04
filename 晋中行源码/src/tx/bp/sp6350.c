/***************************************************************************/
/**�ļ�����: sp6350.c                                                      */
/**��������: ���б�Ʊҵ��ͳ��                                              */
/**��    ע: �˽���û��ȡ������                                            */
/***************************************************************************/
#define ERR_DEAL if(ret){\
        sprintf(acErrMsg,"sql error");\
        WRITEMSG\
        goto ErrExit;\
}
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
sp6350()
{
    FILE    *fp;
    int     ret=0;
    char    lrbal[16];
    char    dubal[16];
    char    cOrbrno  [13];     /***�к�***/
    int     sdate;             /*��ʼ����*/
    int     pdate;             /*��������*/
    char    brno[6];           /*�����к�*/
    char    filename[100];     /*�ļ�����*/
    struct  cashier_c sCashier;/*��Ʊ������Ϣ��*/
    /****��ʼ������****/
    memset(&g_pub_tx,0,sizeof(g_pub_tx));
    memset(brno,     0,sizeof(brno));
    memset(lrbal,     0,sizeof(lrbal));
    memset(dubal,     0,sizeof(dubal));
    memset(cOrbrno     , 0 , sizeof(cOrbrno));
    memset(&sCashier,0x00,sizeof(sCashier));
    pub_base_sysinit();
    /*��ǰ̨��Ļȡֵ*/
    vtcp_log("[%s][%d]��ʼ����ͳ�ƹ���\n",__FILE__,__LINE__);
    get_zd_int("0440",&sdate);   /*��ʼ����*/
    get_zd_int("0450",&pdate);   /*��������*/
    get_fd_data("0020",brno);    /*ǩ���б�*/
    vtcp_log("[%s][%d]sdate=[%d]\n",__FILE__,__LINE__,sdate);
    vtcp_log("[%s][%d]pdate=[%d]\n",__FILE__,__LINE__,pdate);
    vtcp_log("[%s][%d]brno=[%s]\n",__FILE__,__LINE__,brno);
    /***��ѯ�к�***/
    pub_base_strpack(brno);
    if(GetOr_br_noByBr_no(brno,cOrbrno)!=0)
    {
        vtcp_log("[%s][%d]���ݻ����Ų��Ҳ������кŴ���[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"���ݻ����Ų��Ҳ������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***��֯�´��ļ�***/
    /*��ʼȡͳ������*/
    /*����ǩ��*/
    int signcnt=0;
    signcnt=sql_count("cashier","signday>=%d and signday<=%d and stat='1' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal,"signday>=%d and signday<=%d and stat='1' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    /**����ǩ���ֽ�Ʊ**/
    int signcnt1=0;
    signcnt1=sql_count("cashier","signday>=%d and signday<=%d and bptype='1' and stat not in('0','4','7') and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt1<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt1);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal1=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal1,"signday>=%d and signday<=%d and stat not in('0','4','7') and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal1);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    /**����ǩ��ת�ʱ�Ʊ**/
    int signcnt2=0;
    signcnt2=sql_count("cashier","signday>=%d and signday<=%d and bptype='0' and stat not in('0','4','7') and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt2<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt2);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal2=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal2,"signday>=%d and signday<=%d and stat not in('0','4','7') and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal2);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
     /**���й�ʧ��Ʊ**/
    int signcnt3=0;
    signcnt3=sql_count("cashier","signday>=%d and signday<=%d and stat='4' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt3<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt3);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal3=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal3,"signday>=%d and signday<=%d and stat='4' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal3);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    /**���б��Ҹ���Ʊ**/
    int signcnt4=0;
    signcnt4=sql_count("cashier","signday>=%d and signday<=%d and stat='2' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt4<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal4=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal4,"signday>=%d and signday<=%d and stat='2' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
     /**������Ʊ**/
    int signcnt5=0;
    signcnt5=sql_count("cashier","signday>=%d and signday<=%d  and stat='6' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt5<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt5);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal5=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal5,"signday>=%d and signday<=%d and stat='6' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal5);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
     /**�����ֽ�Ʊ���⸶**/
    int signcnt6=0;
    signcnt6=sql_count("cashier","signday>=%d and signday<=%d and stat='3' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt6<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt6);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal6=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal6,"signday>=%d and signday<=%d and stat='3' and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal6);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
     /**��Ʊ¼��**/
    int signcnt7=0;
    signcnt7=sql_count("cashier","signday>=%d and signday<=%d and stat='0'  and trim(brno) like '%s' ",sdate,pdate,brno);
    if(signcnt7<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt7);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal7=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal7,"signday>=%d and signday<=%d and stat='0'  and trim(brno) like '%s' ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal7);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    sprintf(lrbal,"%16.2f",signbal7);
    /**���жҸ����б�Ʊ**/
    int signcnt8=0;
    signcnt8=sql_count("cashier","signday>=%d and signday<=%d  and stat='2' and holdbrno like '%s' ",sdate,pdate,brno);
    if(signcnt8<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ����������[%d]",signcnt8);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    double signbal8=0.0;
    ret=sql_sum_double("cashier","avbal",&signbal8,"signday>=%d and signday<=%d and stat='2' and holdbrno like '%s'  ",sdate,pdate,brno);
    if(ret<0){
        sprintf(acErrMsg,"ͳ�Ʊ���ǩ��������[%f]",signbal8);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
    }
    sprintf(dubal,"%16.2f",signbal8);
    vtcp_log("[%s][%d]signcnt=[%d]\n",__FILE__,__LINE__,signcnt);
    vtcp_log("[%s][%d]signbal=[%f]\n",__FILE__,__LINE__,signbal);
    vtcp_log("[%s][%d]signcnt3=[%d]\n",__FILE__,__LINE__,signcnt3);
    vtcp_log("[%s][%d]signbal3=[%f]\n",__FILE__,__LINE__,signbal3);
    vtcp_log("[%s][%d]signcnt4=[%d]\n",__FILE__,__LINE__,signcnt4);
    vtcp_log("[%s][%d]signbal4=[%f]\n",__FILE__,__LINE__,signbal4);
    vtcp_log("[%s][%d]signcnt5=[%f]\n",__FILE__,__LINE__,signcnt5);
    vtcp_log("[%s][%d]signbal5=[%d]\n",__FILE__,__LINE__,signbal5);
    vtcp_log("[%s][%d]signcnt6=[%d]\n",__FILE__,__LINE__,signcnt6);
    vtcp_log("[%s][%d]signbal6=[%f]\n",__FILE__,__LINE__,signbal6);
    vtcp_log("[%s][%d]signcnt7=[%d]\n",__FILE__,__LINE__,signcnt7);
    vtcp_log("[%s][%d]signbal7=[%f]\n",__FILE__,__LINE__,signbal7);
    vtcp_log("[%s][%d]signcnt8=[%d]\n",__FILE__,__LINE__,signcnt8);
    vtcp_log("[%s][%d]signbal8=[%f]\n",__FILE__,__LINE__,signbal8);
    /***���***/
    set_zd_double("0390",signbal);
    set_zd_double("0400",signbal3); 
    set_zd_double("0410",signbal4);
    set_zd_double("0420",signbal5);
    set_zd_double("0430",signbal6);
    set_zd_data("0580",lrbal);
    set_zd_data("0590",dubal);
    vtcp_log("[%s][%d]��ɽ��\n",__FILE__,__LINE__);
    /***����***/
    set_zd_long("0440",signcnt);
    set_zd_long("0450",signcnt3);
    set_zd_long("0460",signcnt4);
    set_zd_long("0470",signcnt5);
    set_zd_long("0520",signcnt6);
    set_zd_long("0530",signcnt7);
    set_zd_long("0540",signcnt8);
    vtcp_log("[%s][%d]��ɱ���\n",__FILE__,__LINE__);
OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    	vtcp_log("[%s][%d]�ɹ����\n",__FILE__,__LINE__);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
