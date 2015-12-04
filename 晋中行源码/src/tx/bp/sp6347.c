/***************************************************************************/
/**�ļ�����: sp6347.c                                                      */
/**��������: ���б�Ʊ��ʧ                                                  */
/**��    ע: �˽���û��ȡ������,Ҫ��ȡ������                               */
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "bplosthis_c.h"
sp6347()
{
    int     iRc=0;
    int     iSignday=0;
    int     iLostday=0;
    int     lLostcnt=0;
    char    sKinbr[6];
    char    sTel[5];
    char    sTemp[61];
    char    cPono[21];
    char    sAvbal[16];
    char    sTxamt[13];
    char    cFiller[61];
    double  dAvbal=0.0;
    double  dTxamt=0.0;
    struct  cashier_c      sCashier;     /***��Ʊ������Ϣ��***/
    struct  bplosthis_c    sBplosthis;   /***��Ʊ��ʧ��Ϣ��***/
    struct  TIS_GROUP
    {
        char inflag;          /*��Ʊ����*/
        char signday[8];      /*��Ʊ����*/
        char pono   [16];     /*��Ʊ����*/  /* 20110422 [12->16] */
        char paybrno[12];     /*ǩ�����к�*/
        char paybrname[60];   /*ǩ��������*/
        char cashbrno[12];    /*�Ҹ����к�*/
        char cashbrname[60];  /*�Ҹ�������*/
        char filler[60];      /*��      ע*/
        char temp[749];
    }tis;
    /*��ʼ������*/
    memset(&sCashier,0x00,sizeof(sCashier));
    memset(&sBplosthis,0x00,sizeof(sBplosthis));
    memset(&tis,0x00,sizeof(tis));
    
    memset(sTel    , 0 , sizeof(sTel));
    memset(sTemp   , 0 , sizeof(sTemp));
    memset(cPono   , 0 , sizeof(cPono));
    memset(cFiller , 0 , sizeof(cFiller));
    memset(sKinbr  , 0 , sizeof(sKinbr));
    
    pub_base_sysinit();
    /*��ǰ̨��Ļȡֵ*/
    get_fd_data("1230",(char *)&tis);
    get_zd_data("0030",sKinbr);   /*�����к�*/
    get_zd_int("0050", &iLostday);/*��ʧ����*/
    get_zd_data("0070",sTel);     /*����Ա*/
    get_zd_data("0390",sAvbal);   /*Ʊ����*/
    get_zd_data("0400",sTxamt);   /*������*/
    str2dbl(sAvbal,strlen(sAvbal),0,&dAvbal);
    str2dbl(sTxamt,strlen(sTxamt),0,&dTxamt);
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+(20-sizeof(tis.pono)),tis.pono,sizeof(tis.pono));
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    memcpy(sCashier.pono,cPono,sizeof(sCashier.pono)-1);
    vtcp_log("[%s][%d]sCashier.pono=[%s]\n",__FILE__,__LINE__,sCashier.pono);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    memcpy(sTemp,tis.signday,sizeof(tis.signday));
    pub_base_strpack_all(sTemp);
    vtcp_log("[%s][%d]sTemp=[%s]\n",__FILE__,__LINE__,sTemp);
    iSignday=atoi(sTemp);
    vtcp_log("[%s][%d]sSignday=[%d]\n",__FILE__,__LINE__,iSignday);
    /*****Ϊʲô��???
    g_reply_int=Bplosthis_Sel(g_pub_tx.reply,&sBplosthis,"pono='%s' and lostday='%d' ",sCashier.pono,iLostday);
    if(g_reply_int==0)
    {
        sprintf(acErrMsg,"[%s]][%d]���ղ������ظ���ʧ[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    ****/
    /***���е�,�����м�¼,�޸ļ���***/
    vtcp_log("[%s][%d]���б�Ʊ\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]sCashier.pono=[%s]\n",__FILE__,__LINE__,sCashier.pono);
    vtcp_log("[%s][%d]iSignday=[%d]\n",__FILE__,__LINE__,iSignday);
    g_reply_int=Cashier_Sel(g_pub_tx.reply,&sCashier,"pono='%s' and signday='%d' ",sCashier.pono,iSignday);
    if(g_reply_int!=0)
    {
        sprintf(acErrMsg,"[%s]][%d]�޴˱�Ʊ��Ϣ[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='1')    /*'1'ǩ��״̬*/
    {
        vtcp_log("[%s][%d]��Ʊ״̬����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"��Ʊ״̬����");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(fabs(sCashier.avbal-dAvbal)>0.0001)
    {
        vtcp_log("[%s][%d]dAvbal=[%f]\n",__FILE__,__LINE__,dAvbal);
        vtcp_log("[%s][%d]sCashier.avbal=[%f]\n",__FILE__,__LINE__,sCashier.avbal);
        vtcp_log("[%s][%d]��һ��\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"��һ��");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /****2008��04��29�գ����Ȼע����һ��
    if(memcmp(sCashier.brno,sKinbr,sizeof(sKinbr)-1)!=0)
    {
        vtcp_log("[%s][%d]�Ǳ���ǩ��,���ܹ�ʧ\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"�뵽ǩ���й�ʧ");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    ****/
    memset(sTemp,0x00,sizeof(sTemp));
    memcpy(sTemp,tis.paybrname,sizeof(sTemp)-1);
    pub_base_strpack_all(sTemp);
    if(memcmp(sCashier.paybrname,sTemp,strlen(sTemp)))
    {
        vtcp_log("[%s][%d]ǩ������������\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]sTemp[%s]\n",__FILE__,__LINE__,sTemp);
        vtcp_log("[%s][%d]sCashier.paybrname[%s]\n",__FILE__,__LINE__,sCashier.paybrname);
        set_zd_data(DC_GET_MSG,"ǩ������������");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(memcmp(sCashier.cashbrno,tis.cashbrno,sizeof(sCashier.cashbrno)-1))
    {
        vtcp_log("[%s][%d]�Ҹ����кŲ���\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]sCashier.cashbrno=[%s]\n",__FILE__,__LINE__,sCashier.cashbrno);
        vtcp_log("[%s][%d]tis.cashbrno=[%s]\n",__FILE__,__LINE__,tis.cashbrno);
        set_zd_data(DC_GET_MSG,"�Ҹ����кŲ���");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    memset(sTemp,0x00,sizeof(sTemp));
    memcpy(sTemp,tis.cashbrname,sizeof(sTemp)-1);
    pub_base_strpack_all(sTemp);
    if(memcmp(sCashier.cashbrname,sTemp,strlen(sTemp)))
    {
        vtcp_log("[%s][%d]�Ҹ�����������\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"�Ҹ�����������");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]Ҫ����һ����ʧ��¼\n",__FILE__,__LINE__);
    /*����һ����ʧ��Ϣ*/
    memcpy(sBplosthis.pono,sCashier.pono,sizeof(sBplosthis.pono)-1);
    memcpy(sBplosthis.paybrno,sCashier.paybrno,sizeof(sBplosthis.paybrno)-1);
    memcpy(sBplosthis.paybrname,sCashier.paybrname,sizeof(sBplosthis.paybrname)-1);
    memcpy(sBplosthis.cashbrno,sCashier.cashbrno,sizeof(sBplosthis.cashbrno)-1);
    memcpy(sBplosthis.cashbrname,sCashier.cashbrname,sizeof(sBplosthis.cashbrname)-1);
    memcpy(sBplosthis.lostbrno,sKinbr,sizeof(sKinbr)-1);
    memcpy(sBplosthis.bptype,sCashier.bptype,sizeof(sKinbr)-1);

    memcpy(cFiller,tis.filler,sizeof(cFiller)-1);
    pub_base_strpack(cFiller);
    memcpy(sBplosthis.filler,cFiller,sizeof(sBplosthis.filler)-1);
    
    sBplosthis.lostday=iLostday;        /*��ʧ����*/
    sBplosthis.signday=iSignday;        /*ǩ������*/
    sBplosthis.inflag[0]=tis.inflag;

    sBplosthis.avbal=dAvbal;
    sBplosthis.stat[0]='0';            /*0��ʧ1���ʧ*/
    
    lLostcnt=sql_count("bplosthis","pono='%s' and lostday='%d' ",sCashier.pono,iLostday);
    if(lLostcnt<0){
       vtcp_log("[%s][%d]δ�ҵ����������ļ�¼![%d]\n",__FILE__,__LINE__,lLostcnt);
       strcpy(g_pub_tx.reply,"S049");
       goto ErrExit;
    }
    sBplosthis.lostcnt=lLostcnt+1;              /*��ʧ����*/
    
    vtcp_log("[%s][%d]��ʧ����[%d]\n",__FILE__,__LINE__,sBplosthis.lostcnt);
    memcpy(sBplosthis.losttlrno,sTel,sizeof(sBplosthis.losttlrno)-1);
    g_reply_int=Bplosthis_Ins(sBplosthis,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]�����ʧ�Ǽǲ�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"�����ʧ�Ǽǲ�����");
        Bplosthis_Debug(&sBplosthis);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*���±�Ʊ��Ϣ*/
    vtcp_log("[%s][%d]Ҫ���±�Ʊ��Ϣ\n",__FILE__,__LINE__);
    if((g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",sCashier.pono,iSignday))!=0){
        vtcp_log("[%s][%d]��Ʊ������\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"��Ʊ������");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Cashier_Fet_Upd(&sCashier,g_pub_tx.reply))!=0)
    {
        vtcp_log("[%s][%d]�����˱�Ʊ�Ŵ���\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"�����˱�Ʊ�Ŵ���");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.lactcls[0]=sCashier.stat[0];
    sCashier.stat[0]='4';        /*��ʧ*/
    sCashier.lostcnt++;
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]���±�Ʊ����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"���±�Ʊ����");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]���±�Ʊ����\n",__FILE__,__LINE__);
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
