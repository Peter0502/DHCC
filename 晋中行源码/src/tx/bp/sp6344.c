/***************************************************************************/
/*�ļ�����: sp6344.c                                                       */
/*��������: ʵ�ָ���֧�жҸ���Ʊ                                           */
/*��    ��: xyy    2008-2-25                                               */    
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "com_item_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
#include "hv_define.h"
int sp6344()
{
    int     iRc=0;
    int     ret=0;
    long    lSignday=0;
    long    lEndday =0;
    char    cKinbr[6];
    char    cPayactno[33];
    char    cPayname[61];
    char    cCashactno[33];
    char    cCashname[61];
    char    cOcashname[61];
    char    cTel[5];
    char    cPono[21];
    char    cMiya[21];
    char    cBrno[6];
    char    cOrbrno[13];
    char    cTemp[17];  /* 20110422 [13->17] */
    char    cTxamt[17];
    double  dTxamt=0.0;
    struct  com_parm_c     com_parm;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    struct  in_mst_c       in_mst;
    struct  cashier_c      Cashier;      /***��Ʊ������Ϣ��***/
    struct  com_branch_c   Com_branch;   /***������***/
    struct  hv_uniontab_c  Hv_uniontab;
    struct TIS_GROUP{
        char bptype    [1 ];    /* ��Ʊ��� 0-��ת�ñ�Ʊ1-ת�ʱ�Ʊ2-�ֽ�Ʊ */
        char avbaltype [1 ];    /* �����־ 0-���Ʊ1-�����Ʊ */
        char pono      [16];    /* ��Ʊ���� */  /* 20110422 [12->16] */
        char signtype  [1 ];    /* ǩ����ʽ 0-�ֽ�,1-�Թ��ʺ�,2-ϵͳ��ת */
        char reqno     [10];    /* ��������� */
        char payactno  [32];    /* �������ʺ� */
        char payname   [60];    /* ���������� */
        char cashactno [32];    /* �տ����ʺ� */
        char cashname  [60];    /* �տ������� */
        char reason    [60];    /* ժҪ */
        char mac       [20];
        char temp      [656];
    }tis;
    /****��ʼ������****/
    memset(&com_parm    , 0 , sizeof(com_parm));
    memset(&in_mst      , 0 , sizeof(in_mst));
    memset(&tis         , 0 , sizeof(tis));
    memset(&Hv_uniontab , 0 , sizeof(Hv_uniontab));
    memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));
    memset(&Cashier     , 0 , sizeof(Cashier));
    memset(&Com_branch  , 0 , sizeof(Com_branch));
    memset(cTemp        , 0 , sizeof(cTemp));
    memset(cPayactno    , 0 , sizeof(cPayactno));
    memset(cPayname     , 0 , sizeof(cPayname));
    memset(cCashactno   , 0 , sizeof(cCashactno));
    memset(cCashname    , 0 , sizeof(cCashname));
    memset(cOcashname   , 0 , sizeof(cOcashname));
    memset(cOrbrno      , 0 , sizeof(cOrbrno));
    memset(cKinbr       , 0 , sizeof(cKinbr));
    memset(cTxamt       , 0 , sizeof(cTxamt));
    memset(cTel         , 0 , sizeof(cTel));
    memset(cBrno        , 0 , sizeof(cBrno));
    memset(cMiya        , 0 , sizeof(cMiya));
    memset(cPono        , 0 , sizeof(cPono));
    pub_base_sysinit();
    vtcp_log("[%s][%d]ͬ־��sp6344��ʼ��\n",__FILE__,__LINE__);
     /*��ǰ̨��Ļȡֵ*/
     /*��ǰ̨��Ļȡֵ*/
    get_fd_data("1230",(char *)&tis);
    vtcp_log("[%s][%d] tis.bptype    =[%.1s] \n",__FILE__,__LINE__,tis.bptype     );
    vtcp_log("[%s][%d] tis.avbaltype =[%.1s] \n",__FILE__,__LINE__,tis.avbaltype  );
    vtcp_log("[%s][%d] tis.pono      =[%.16s]\n",__FILE__,__LINE__,tis.pono       );
    vtcp_log("[%s][%d] tis.signtype  =[%.1s] \n",__FILE__,__LINE__,tis.signtype   );
    vtcp_log("[%s][%d] tis.reqno     =[%.10s]\n",__FILE__,__LINE__,tis.reqno      );
    vtcp_log("[%s][%d] tis.payactno  =[%.32s]\n",__FILE__,__LINE__,tis.payactno   );
    vtcp_log("[%s][%d] tis.payname   =[%.60s]\n",__FILE__,__LINE__,tis.payname    );
    vtcp_log("[%s][%d] tis.cashactno =[%.32s]\n",__FILE__,__LINE__,tis.cashactno  );
    vtcp_log("[%s][%d] tis.cashname  =[%.60s]\n",__FILE__,__LINE__,tis.cashname   );
    vtcp_log("[%s][%d] tis.reason    =[%.60s]\n",__FILE__,__LINE__,tis.reason     );
    vtcp_log("[%s][%d] tis.mac       =[%.20s]\n",__FILE__,__LINE__,tis.mac);
    
    memcpy(cTemp,tis.pono,sizeof(tis.pono));
    memcpy(cMiya,tis.mac,sizeof(tis.mac));
    get_zd_data("0030",cKinbr);        /*�����к�*/
    get_zd_long("0440",&lSignday);     /*ǩ������*/
    get_zd_data("0270",cOcashname);
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    get_zd_data("0070",cTel);          /*ǩ������*/
    vtcp_log("[%s][%d]cTel=[%s]\n",__FILE__,__LINE__,cTel);
    get_zd_data("0390",cTxamt);     /*��Ʊ���*/
    vtcp_log("[%s][%d]cTxamt=[%s]\n",__FILE__,__LINE__,cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    vtcp_log("[%s][%d]cOcashname=[%s]\n",__FILE__,__LINE__,cOcashname);
    
    /***�ж��Ƿ�����������***/
    pub_base_strpack(cKinbr);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("%s,%d �������Ĳ����ԶҸ���Ʊ",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"�������Ĳ����ԶҸ���Ʊ",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    /***�жϽ������Ƿ����***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&Com_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("%s,%d �޴˽��׻���[%d]",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"[%s][%d]�޴˽��׻���[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/*B357����*/
        goto ErrExit;
    }
    
    /**���б�Ʊǰ��8����**/
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);          /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    /**��鱾Ʊ��Ϣ**/
    iRc=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",cPono,lSignday);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd����==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    iRc=Cashier_Fet_Upd(&Cashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    
    lEndday = Cashier.endday;
    vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEndday);
    g_reply_int=pub_base_iJumpHoliday(&lEndday);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]�õ���Ʊ�����մ���[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"�õ���Ʊ�����մ���");
        Cashier_Debug(&Cashier);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEndday);
    
    if(lEndday < g_pub_tx.tx_date)
    {
        vtcp_log("[%s][%d][%d]�ñ�Ʊ�Ѿ����ڣ�������Ҹ�",__FILE__,__LINE__,Cashier.endday);
        sprintf(acErrMsg,"�ñ�Ʊ�Ѿ����ڣ�������Ҹ�",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    if(pub_base_CompDblVal(Cashier.avbal,dTxamt)!=0)
    {
        vtcp_log("[%s][%d]������Ľ��׽�����==[%f]",__FILE__,__LINE__,Cashier.avbal);
        sprintf(acErrMsg,"������Ľ��׽�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    if(memcmp(Cashier.bptype ,tis.bptype ,sizeof(tis.bptype    ))!=0)
    {
        vtcp_log("[%s][%d]������ı�Ʊ���ʹ���==[%c]",__FILE__,__LINE__,Cashier.bptype[0]);
        sprintf(acErrMsg,"������ı�Ʊ���ʹ���\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    zip_space(cMiya);
    if(memcmp(Cashier.passwd ,cMiya ,sizeof(Cashier.passwd  )-1)!=0)
    {
        vtcp_log("[%s][%d]���������Ѻ����==[%s][%s]",__FILE__,__LINE__,Cashier.passwd,cMiya);
        sprintf(acErrMsg,"���������Ѻ����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    /****��ʱ����
    if(memcmp(Cashier.ovocnum    ,tis.reqno     ,sizeof(tis.reqno     ))!=0)
    {
        vtcp_log("[%s][%d]�������������������==[%s]",__FILE__,__LINE__,Cashier.ovocnum);
        sprintf(acErrMsg,"�������������������\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");
        goto ErrExit;
    }
    ****/
    memcpy(cPayactno,tis.payactno,sizeof(tis.payactno ));
    pub_base_strpack(cPayactno);
    pub_base_strpack(Cashier.payactno);
    if(memcmp(Cashier.payactno ,cPayactno ,strlen(Cashier.payactno))!=0 || (strlen(Cashier.payactno)!=strlen(cPayactno)))
    {
        vtcp_log("[%s][%d]�����ʻ�����==[%s][%s]",__FILE__,__LINE__,Cashier.payactno,cPayactno);
        sprintf(acErrMsg,"������ĸ����ʻ�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/**B357����**/
        goto ErrExit;
    }
    memcpy(cPayname,tis.payname,sizeof(tis.payname  ));
    pub_base_strpack(cPayname);
    pub_base_strpack(Cashier.payname);
    if(memcmp(Cashier.payname ,cPayname ,strlen(cPayname))!=0 || strlen(cPayname)!=strlen(Cashier.payname))
    {
        vtcp_log("[%s][%d]������ĸ��������==[%s]  ",__FILE__,__LINE__,Cashier.payname);
        sprintf(acErrMsg,"������ĸ��������\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    if(Cashier.stat[0]!='1')
    {
        vtcp_log("[%s][%d]����ǩ��״̬,������Ҹ�=[%c]  ",__FILE__,__LINE__,Cashier.stat[0]);
        sprintf(acErrMsg,"[%s][%d]����ǩ��״̬,������Ҹ�",__FILE__,__LINE__);
        WRITEMSG
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    memcpy(cBrno,Cashier.brno,sizeof(cBrno)-1);/**ǩ������**/
    /**�ж϶Ҹ����Ƿ�������**/
    if(GetOr_br_noByBr_no(cKinbr,cOrbrno)!=0)
    {
        vtcp_log("[%s][%d] ���ݻ����Ų��Ҳ������кŴ���[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"���ݻ����Ų��Ҳ������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/**B357����**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]���Ҹ���Ϊ:[%s]\n",__FILE__,__LINE__,cOrbrno);
    pub_base_strpack(cOrbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d ��ѯ�����в������кŴ���[%s]",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"[%s][%d]��ѯ�����в������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    if(tis.bptype[0] =='1')/**������ֽ�Ʊ**/
    {
          if(memcmp(Hv_uniontab.qs_no,HV_QS_BR_NO,sizeof(Hv_uniontab.qs_no)-1)!=0)
          {
              vtcp_log("%s,%d �ֽ�Ʊ�����Ǳ��жҸ�[%s]",__FILE__,__LINE__,cOrbrno);
              sprintf(acErrMsg,"[%s][%d]�ֽ�Ʊ�����Ǳ��жҸ�",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357����**/
              goto ErrExit;
          }
          
          memcpy(cCashactno,tis.cashactno,sizeof(tis.cashactno ));
          pub_base_strpack(cCashactno);
          pub_base_strpack(Cashier.cashactno);
          if(memcmp(Cashier.cashactno ,cCashactno ,strlen(Cashier.cashactno))!=0 || (strlen(Cashier.cashactno)!=strlen(cCashactno)))
          {
              vtcp_log("[%s][%d]�տ��ʻ�����==[%s][%s]",__FILE__,__LINE__,Cashier.cashactno,cCashactno);
              sprintf(acErrMsg,"��������տ��ʻ�����\n",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357����**/
              goto ErrExit;
          }
          memcpy(cCashname,tis.cashname,sizeof(tis.cashname  ));
          pub_base_strpack(cCashname);
          pub_base_strpack(Cashier.cashname);
          if(memcmp(Cashier.cashname ,cCashname ,strlen(cCashname))!=0 || strlen(cCashname)!=strlen(Cashier.cashname))
          {
              vtcp_log("[%s][%d]��������տ������==[%s]  ",__FILE__,__LINE__,Cashier.cashname);
              sprintf(acErrMsg,"��������տ������\n",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357����**/
              goto ErrExit;
          }
    }
    else/**������ת�˱�Ʊ,����տ��˲�����ô���ж��Ƿ�ԭ�տ���������ȷ�أ�**/
    {
    	    vtcp_log("[%s][%d]cCashname=[%s]\n",__FILE__,__LINE__,cCashname);
          memcpy(cCashname,tis.cashname,sizeof(tis.cashname  ));
          pub_base_strpack(cCashname);
          pub_base_strpack(Cashier.cashname);
          if(memcmp(Cashier.cashname ,cCashname ,strlen(cCashname))!=0 || strlen(cCashname)!=strlen(Cashier.cashname))
          {
              if(memcmp(Cashier.cashname , cOcashname,strlen(Cashier.cashname))!=0 || (strlen(Cashier.cashname)!=strlen(cOcashname))) /***NEWBP3****/
          	  {
          	  	  vtcp_log("[%s][%d]�տ��ʻ�����==[%d][%d]",__FILE__,__LINE__,strlen(Cashier.cashname),strlen(cOcashname));
                  vtcp_log("[%s][%d]�տ��ʻ�����==[%s][%s]",__FILE__,__LINE__,Cashier.cashname,cOcashname);
                  sprintf(acErrMsg,"�������ԭ�տ��ʻ�����\n",__FILE__,__LINE__);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  strcpy(g_pub_tx.reply,"8888");/**B357����**/
                  goto ErrExit;
              }
              vtcp_log("[%s][%d]����ɹ�!!!\n",__FILE__,__LINE__);
          }
    }
    vtcp_log("[%s][%d]׼����ʼ���±��¼\n",__FILE__,__LINE__);
    Cashier.lactcls[0] = Cashier.stat[0];
    Cashier.stat[0] = '2';             /**ȫ��Ҹ�**/
    Cashier.cashday = g_pub_tx.tx_date;/**�Ҹ�����**/
    memcpy(Cashier.cashtlrno,cTel,sizeof(Cashier.cashtlrno)-1);
    memcpy(Cashier.jqtlrno,cTel,sizeof(Cashier.jqtlrno)-1);
    /**����Ʊ��**/
    memcpy(Cashier.holdactno  ,tis.cashactno ,sizeof(tis.cashactno ));
    memcpy(Cashier.holdname  ,tis.cashname  ,sizeof(tis.cashname  ));
    memcpy(Cashier.holdbrno   ,cOrbrno       ,sizeof(Cashier.holdbrno)-1);
    memcpy(Cashier.holdbrname   ,Hv_uniontab.br_name_f,sizeof(Cashier.holdname)-1);
    if(memcmp(Cashier.brno,cKinbr,sizeof(cKinbr)-1)==0)
    {
        Cashier.cashstat[0]='0'; /**0.ϵͳ�ڱ��жҸ�����ǩ��**/
    }
    else
    {
        Cashier.cashstat[0]='1'; /**1.ϵͳ�����жҸ�����ǩ��**/
    }
    memcpy(Cashier.filler ,tis.reason ,sizeof(Cashier.filler)-1);
    /***��ʼ���ɱ�Ʊ��¼***/
    vtcp_log("[%s][%d]׼�����±���\n",__FILE__,__LINE__);
    iRc=Cashier_Upd_Upd(Cashier,g_pub_tx.reply );
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]�޸ı�Ʊ��¼����[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�޸ı�Ʊ��¼����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]���±�Ʊ��¼���\n",__FILE__,__LINE__);
    /**�Ǽǽ�����ˮ**/
    memcpy(g_pub_tx.beg_note_no,tis.pono,16); /***NEWBP3**/   /* 20110422 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    /**���ǩ���еĶҸ��ʺ�**/
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",cBrno);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** ��鱾Ʊǩ�����Ƿ���� **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��鱾Ʊǩ����ʧ��!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��Ʊǩ��������!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data( "0300", com_parm.val);
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
