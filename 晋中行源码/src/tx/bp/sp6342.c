/***************************************************************************/
/*�ļ�����: sp6342.c                                                       */
/*��������: ǩ����Ʊ��Ϣ���޸�                                             */
/*��    ע: �ó���û��ȡ������                                             */
/*��    ��: xyy    2008-02-25                                              */ 
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "com_item_c.h"
#include "hv_uniontab_c.h"
extern int iLvGetReceOpnbr(const char * tx_br_no,const char * cash_ac_no,char * opn_br_no);
int sp6342()
{
    int     iRc=0;
    
    long    lSignday=0;
    
    char    cTel      [ 5];
    char    cKinbr    [ 6];
    char    cPono     [21];
    char    cQsno     [13];
    char    cTxamt    [17];
    char    cInflag   [ 2];
    char    cOpbrno   [ 6];
    char    cOrbrno   [13];
    char    cPaybrno  [13];
    char    cPayactno [33];
    char    cPaybrname[61];
    char    Tel[5];
    
    double  dTxamt=0.0;
    struct  cashier_c      Cashier;      /***��Ʊ������Ϣ��***/
    struct  hv_uniontab_c  Hv_uniontab1; /***�����к���Ϣ��***/
    struct  hv_uniontab_c  Hv_uniontab2; /***�����к���Ϣ��***/
    struct  hv_uniontab_c  Hv_uniontab3; /***�����к���Ϣ��***/
    struct  com_branch_c   Com_branch;   /***������***/
    struct TIS_GROUP{
        char bptype    [1 ];    /* ��Ʊ��� 0-��ת�ñ�Ʊ1-ת�ʱ�Ʊ2-�ֽ�Ʊ */
        char avbaltype [1 ];    /* �����־ 0-���Ʊ1-�����Ʊ */
        char pono      [16];    /* ��Ʊ���� */      /* 20110425 [12->16] */
        char signtype  [1 ];    /* ǩ����ʽ 0-�ֽ�,1-�Թ��ʺ�,2-ϵͳ��ת */
        char payactno  [32];    /* �������ʺ� */
        char payname   [60];    /* ���������� */
        char cashactno [32];    /* �տ����ʺ� */
        char cashname  [60];    /* �տ������� */
        char cashbrno  [12];    /* �տ����к�(�Ҹ���) */
        char cashbrname[60];    /* �Ҹ������� */
        char reason    [60];    /* ժ      Ҫ */
        char temp      [614];
    }tis;
    /****��ʼ������****/
    memset(&tis         , 0 , sizeof(tis));
    memset(cTel         , 0 , sizeof(cTel));
    memset(cPono        , 0 , sizeof(cPono));
    memset(cQsno        , 0 , sizeof(cQsno));
    memset(cKinbr       , 0 , sizeof(cKinbr));
    memset(cTxamt       , 0 , sizeof(cTxamt));
    memset(cOpbrno      , 0 , sizeof(cOpbrno));
    memset(cInflag      , 0 , sizeof(cInflag));
    memset(cOrbrno      , 0 , sizeof(cOrbrno));
    memset(cPaybrno     , 0 , sizeof(cPaybrno));
    memset(cPayactno    , 0 , sizeof(cPayactno));
    memset(cPaybrname   , 0 , sizeof(cPaybrname));
    memset(Tel   , 0 , sizeof(Tel));
    memset(&Cashier     , 0 , sizeof(Cashier));
    memset(&Com_branch  , 0 , sizeof(Com_branch));
    memset(&Hv_uniontab1, 0 , sizeof(Hv_uniontab1));
    memset(&Hv_uniontab2, 0 , sizeof(Hv_uniontab2));
    memset(&Hv_uniontab3, 0 , sizeof(Hv_uniontab3));
    
    pub_base_sysinit();
    vtcp_log("[%s][%d]ͬ־��sp6342��ʼ��\n",__FILE__,__LINE__);
    /*��ǰ̨��Ļȡֵ*/
    get_fd_data("1230",(char *)&tis);
    vtcp_log("[%s][%d] tis.bptype    =[%.1s] \n",__FILE__,__LINE__,tis.bptype     );
    vtcp_log("[%s][%d] tis.avbaltype =[%.1s] \n",__FILE__,__LINE__,tis.avbaltype  );
    vtcp_log("[%s][%d] tis.pono      =[%.16s]\n",__FILE__,__LINE__,tis.pono       );
    vtcp_log("[%s][%d] tis.signtype  =[%.1s] \n",__FILE__,__LINE__,tis.signtype   );
    vtcp_log("[%s][%d] tis.payactno  =[%.32s]\n",__FILE__,__LINE__,tis.payactno   );
    vtcp_log("[%s][%d] tis.payname   =[%.60s]\n",__FILE__,__LINE__,tis.payname    );
    vtcp_log("[%s][%d] tis.cashactno =[%.32s]\n",__FILE__,__LINE__,tis.cashactno  );
    vtcp_log("[%s][%d] tis.cashname  =[%.60s]\n",__FILE__,__LINE__,tis.cashname   );
    vtcp_log("[%s][%d] tis.cashbrno  =[%.12s]\n",__FILE__,__LINE__,tis.cashbrno   );
    vtcp_log("[%s][%d] tis.cashbrname=[%.60s]\n",__FILE__,__LINE__,tis.cashbrname );
    vtcp_log("[%s][%d] tis.reason    =[%.60s]\n",__FILE__,__LINE__,tis.reason     );
    
    get_zd_data("0030",cKinbr);        /*�����к�*/
    get_zd_data("0070",Tel);           /*����Ա*/
    get_zd_long("0050",&lSignday);     /*ǩ������*/
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    get_zd_data("0070",cTel);          /*ǩ������*/
    vtcp_log("[%s][%d]cTel=[%s]\n",__FILE__,__LINE__,cTel);
    get_zd_data("0390",cTxamt);        /*��Ʊ���*/
    vtcp_log("[%s][%d]cTxamt=[%s]\n",__FILE__,__LINE__,cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    
    /***�ж��Ƿ�����������***/
    pub_base_strpack(cKinbr);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("%s,%d �������Ĳ�����ǩ����Ʊ",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"�������Ĳ�����ǩ����Ʊ",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***�жϽ������Ƿ����***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&Com_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("%s,%d �޴˽��׻���[%d]",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"[%s][%d]�޴˽��׻���[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***���ݽ����к�,�õ��������к�***/
    g_reply_int=GetOr_br_noByBr_no(cKinbr,cOrbrno);
    vtcp_log("[%s][%d]cOrbrno=[%s]\n",__FILE__,__LINE__,cOrbrno);
    if(g_reply_int)
    {
        sprintf(acErrMsg,"����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"P007",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        vtcp_log("%s,%d ����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
        goto ErrExit;
    }
    /**�Ҹ��е���Ϣ**/
    vtcp_log("[%s][%d]�Ҹ���Ϊ:[%.12s]\n",__FILE__,__LINE__,tis.cashbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab1,"or_br_no='%.12s' ",tis.cashbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d ��ѯ�����в������кŴ���[%s]",__FILE__,__LINE__,tis.cashbrno);
        sprintf(acErrMsg,"[%s][%d]��ѯ�����в������кŴ���",__FILE__,__LINE__);
	/*****
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
	****/
    }
    memcpy(cQsno , Hv_uniontab1.qs_no,sizeof(Hv_uniontab1.qs_no)-1);
    vtcp_log("[%s][%d]�Ҹ�������:[%s]\n",__FILE__,__LINE__,cQsno);
    /**ǩ���е���Ϣ**/
    pub_base_strpack(cOrbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab2,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d ��ѯ�����в������кŴ���[%s]",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"[%s][%d]��ѯ�����в������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**�Ҹ��к�ǩ������ͬһ�������ĵ�**/
    if(memcmp(cQsno,Hv_uniontab2.qs_no,sizeof(Hv_uniontab2.qs_no)-1)==0)
    {
        cInflag[0]='0';/*����*/
    }
    else
    {
        cInflag[0]='1';/*����*/
    }
    vtcp_log("[%s][%d]���ڱ�־[%c]\n",__FILE__,__LINE__,cInflag[0]);
    
    /**���б�Ʊǰ��8����**/
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    memcpy(cPono+4,tis.pono,sizeof(tis.pono));   /* 20110425 [8->4] */
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    /**��鱾Ʊ��Ϣ**/
    iRc=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",cPono,lSignday);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd����==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    iRc=Cashier_Fet_Upd(&Cashier,g_pub_tx.reply);
    if(iRc!=0)
    {
          vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**add by robbit**/
    if(memcmp(Tel,Cashier.optlrno,4)!=0)
    {
        vtcp_log("[%s][%d]�޸Ĳ���Ա������¼�����Աһ��[%s][%s]  ",__FILE__,__LINE__,Tel,Cashier.optlrno);
        sprintf(acErrMsg,"�޸Ĳ���Ա������¼�����Աһ��",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /** end by robbit**/
    if(Cashier.stat[0]!='0')
    {
          vtcp_log("[%s][%d][%c]��Ʊ���ǵǼ�״̬[0]  ",__FILE__,__LINE__,Cashier.stat[0]);
        sprintf(acErrMsg,"��Ʊ���ǵǼ�״̬",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]׼����ʼ���±��¼\n",__FILE__,__LINE__);
    if(dTxamt==0.0)
    {
          vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
        sprintf(acErrMsg,"[%s][%d]������Ϊ��\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**�õ������ʺŵĿ�������**/
    memcpy(cPayactno,tis.payactno,sizeof(cPayactno)-1);
    vtcp_log("[%s][%d]cPayactno=[%s]\n",__FILE__,__LINE__,cPayactno);
    pub_base_strpack(cPayactno);
    if( strlen(cPayactno) >=7)
    {
        iRc = iLvGetReceOpnbr(cKinbr,cPayactno,cOpbrno);
        if(iRc!=0)
        {
            vtcp_log("[%s][%d]�õ�������������\n",__FILE__,__LINE__);
            sprintf(acErrMsg,"[%s][%d]�õ�������������\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy(g_pub_tx.reply,"8888");
            goto ErrExit;
        }
        vtcp_log("[%s][%d]�����ʺſ�������[%s]\n",__FILE__,__LINE__,cOpbrno);
        g_reply_int=GetOr_br_noByBr_no(cOpbrno,cPaybrno);
        vtcp_log("[%s][%d]�������Ϊ[%s]\n",__FILE__,__LINE__,cPaybrno);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
            strncpy(g_pub_tx.reply,"P007",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            vtcp_log("%s,%d ����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
            goto ErrExit;
        }
        pub_base_strpack(cPaybrno);
        iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab3,"or_br_no='%s' ",cPaybrno);
        if(iRc!=0)
        {
            vtcp_log("%s,%d ��ѯ�����в������кŴ���[%s]",__FILE__,__LINE__,cOrbrno);
            sprintf(acErrMsg,"[%s][%d]��ѯ�����в������кŴ���",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy(g_pub_tx.reply,"8888");
            goto ErrExit;
        }
        memcpy(cPaybrname,Hv_uniontab3.br_name_f,sizeof(cPaybrname)-1);   /*����ȫ��*/
    }
    else
    {
        memcpy(cPaybrno,cOrbrno,sizeof(cPaybrno)-1);
        memcpy(cOpbrno,cKinbr,sizeof(cOpbrno)-1);
        memcpy(cPaybrname,Hv_uniontab2.br_name_f,sizeof(cPaybrname)-1);   /*����ȫ��*/
    }
    
    vtcp_log("[%s][%d]��ʼ���ɱ�Ʊ��¼[%s]\n",__FILE__,__LINE__,cKinbr);
    /***��ʼ���ɱ�Ʊ��¼***/
    memcpy(Cashier.pono,cPono,sizeof(cPono)-1);
    memcpy(Cashier.orbrno,cOrbrno,sizeof(Cashier.orbrno)-1);
    memcpy(Cashier.payqsactno,Hv_uniontab2.qs_no,sizeof(Cashier.payqsactno)-1);
    Cashier.avbaltype[0]=tis.avbaltype[0];
    Cashier.bptype[0]=tis.bptype[0]; 
    Cashier.ccflag[0]=tis.signtype[0];
    vtcp_log("[%s][%d]����¼��Ʊ�ݺ���:[%s]\n",__FILE__,__LINE__,Cashier.pono);
    memcpy(Cashier.brno,cKinbr,sizeof(cKinbr)-1);
    memcpy(Cashier.payactno,tis.payactno,sizeof(Cashier.payactno)-1);
    memcpy(Cashier.payname,tis.payname,sizeof(Cashier.payname)-1);
    vtcp_log("[%s][%d]�������ʻ�:[%s]\n",__FILE__,__LINE__,Cashier.payactno);
    memcpy(Cashier.cour,"001",sizeof(Cashier.cour)-1);        /*����*/
    Cashier.avbal=dTxamt;
    memcpy(Cashier.cashactno,tis.cashactno,sizeof(Cashier.cashactno)-1);
    memcpy(Cashier.cashname,tis.cashname,sizeof(Cashier.cashname)-1);
    memcpy(Cashier.cashbrno,tis.cashbrno,sizeof(Cashier.cashbrno)-1);
    memcpy(Cashier.cashbrname,tis.cashbrname,sizeof(Cashier.cashbrname)-1);
    vtcp_log("[%s][%d]�տ����ʻ�[%s]\n",__FILE__,__LINE__,Cashier.cashactno);
    memcpy(Cashier.paybrno,cPaybrno,sizeof(Cashier.paybrno)-1);
    memcpy(Cashier.paybrname,cPaybrname,sizeof(Cashier.paybrname)-1); /*����ȫ��*/
    memcpy(Cashier.reason,tis.reason,sizeof(Cashier.reason)-1);/*����*/
    Cashier.stat[0]='0';               /***�Ǽ�***/
    Cashier.inflag[0]=cInflag[0];      /***����***/
    vtcp_log("[%s][%d]�������ʾ:[%c]\n",__FILE__,__LINE__,Cashier.inflag[0]);
    Cashier.addpassflg[0]='0';         /***��Ѻδ����***/
    memcpy(Cashier.optlrno,cTel,sizeof(cTel)-1);
    Cashier.lostcnt = 0 ;             /***��ʧ����***/
    vtcp_log("[%s][%d]ǩ������:[%ld]\n",__FILE__,__LINE__,Cashier.signday);
    iRc=Cashier_Upd_Upd(Cashier,g_pub_tx.reply );
    if(iRc!=0)
    {
    	  Cashier_Debug(&Cashier);
        vtcp_log("[%s][%d]�޸ı�Ʊ��¼����[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�޸ı�Ʊ��¼����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();

    vtcp_log("[%s][%d]�޸ı�Ʊ��¼�ɹ�\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ��Ʊ����pono=[%s]\n",__FILE__,__LINE__,tis.pono);
    memcpy(g_pub_tx.beg_note_no,tis.pono,16);    /* 20110425 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]�Ǽǽ�����ˮ���",__FILE__,__LINE__);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        memcpy(g_pub_tx.reply,"T063",4);
    }
    sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
