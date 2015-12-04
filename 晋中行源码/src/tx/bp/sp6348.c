/***************************************************************************/
/**�ļ�����: sp6348.c                                                      */
/**��������: ���б�Ʊ���ʧ                                                */
/**��    ע: �˽���û��ȡ������                                            */
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "bplosthis_c.h"
sp6348()
{
    int     iRc=0;
    long    lLostday=0;
    
    char    sTel [ 5];
    char    cTemp[17];  /* 20110422 [13->17] */
    char    cPono[21];
    char    sKinbr[6];
    
    struct  cashier_c      sCashier;     /***��Ʊ������Ϣ��***/
    struct  bplosthis_c    sBplosthis;   /***��Ʊ��ʧ��Ϣ��***/

    /****��ʼ������****/
    memset(sTel  , 0 , sizeof(sTel));
    memset(cTemp , 0 , sizeof(cTemp));
    memset(cPono , 0 , sizeof(cPono));
    memset(sKinbr, 0 , sizeof(sKinbr));
    memset(&sCashier  , 0 , sizeof(sCashier));
    memset(&sBplosthis, 0 , sizeof(sBplosthis));
    
    pub_base_sysinit();
    
    /*��ǰ̨��Ļȡֵ*/
    get_zd_long("0440",&lLostday);  /*��ʧ����*/
    get_fd_data("0320",cTemp);      /*��Ʊ����*/
    get_zd_data("0030",sKinbr);     /*�����к�*/
    get_zd_data("0070",sTel);       /*�� �� Ա*/
    
    vtcp_log("[%s][%d]lLostday=[%ld]\n",__FILE__,__LINE__,lLostday);
    vtcp_log("[%s][%d]   cTemp=[%s]\n",__FILE__,__LINE__,cTemp);
    vtcp_log("[%s][%d]  sKinbr=[%s]\n",__FILE__,__LINE__,sKinbr);
    vtcp_log("[%s][%d]    sTel=[%s]\n",__FILE__,__LINE__,sTel);
    
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);   /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    /**�޸Ĺ�ʧ�Ǽǲ���Ϣ**/
    if((g_reply_int=Bplosthis_Dec_Upd(g_pub_tx.reply,"pono='%s' and lostday='%d' and stat='0' ",cPono,lLostday))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]��ʧ�Ǽǲ�������[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Bplosthis_Fet_Upd(&sBplosthis,g_pub_tx.reply))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]��ʧ�Ǽǲ�������[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sBplosthis.stat[0]!='0')
    {
        sprintf(acErrMsg,"[%s][%d]��Ʊ״̬���ǹ�ʧ,���ܽ��\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]sKinbr=[%s]\n",__FILE__,__LINE__,sKinbr);
    memcpy(sBplosthis.unlostbrno,sKinbr,sizeof(sKinbr)-1);
    sBplosthis.unlostday=g_pub_tx.tx_date;
    sBplosthis.stat[0]='1';                            /*���ʧ*/
    memcpy(sBplosthis.unlosttlrno,sTel,sizeof(sTel)-1);/*��Ҳ���Ա*/
    iRc=Bplosthis_Upd_Upd(sBplosthis,g_pub_tx.reply);
    if(iRc!=0)
    {
        sprintf(acErrMsg,"[%s][%d]���±�Ʊ��ʧ�Ǽǲ�����[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Bplosthis_Clo_Upd();
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    vtcp_log("[%s][%d]sBplosthis.inflag=[%c]\n",__FILE__,__LINE__,sBplosthis.inflag[0]);
    vtcp_log("[%s][%d]sBplosthis.signday=[%d]\n",__FILE__,__LINE__,sBplosthis.signday);
    /*���±�Ʊ��Ϣ*/
    if((g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",cPono,sBplosthis.signday))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]�޴˱�Ʊ��Ϣ[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Cashier_Fet_Upd(&sCashier,g_pub_tx.reply))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]��Ʊ��Ϣ������[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='4')
    {    /***'4'��Ʊ��ʧ***/
        sprintf(acErrMsg,"[%s][%d]ԭ��Ʊ״̬��[%d]\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.lactcls[0]=sCashier.stat[0];
    sCashier.stat[0]='1';  /***���ʧ***/
    sCashier.lostcnt--;    /***��ʧ����***/
    /***���б�Ʊ,��״̬***/
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        sprintf(acErrMsg,"[%s][%d]���±�Ʊ����[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]������!!\n",__FILE__,__LINE__);
    /**�Ǽǽ�����ˮ**/
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]���������\n",__FILE__,__LINE__);
OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
