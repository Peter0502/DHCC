/**************************************************************************/
/*�ļ�����: sp6343.c                                                      */
/*��������: ǩ����Ʊ��Ϣ��ǩ��                                            */
/*��    ��: xyy    2008-02-25                                             */
/**************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
extern int nCountBpMac(char *pcPayNo,char *pcPayActNo,double dTxamt,char *pcVocNo,char *pcSignDay,char *pcMac);
int sp6343()
{
    int     iRc=0;
    int     ret=0;
    char    cTel     [5 ];
    char    cMiya    [21];
    char    cPono    [17];  /* 20110422 [13->17] */
    char    lPono    [17];  /***NEWBP3����ˮ׼����****/ /* 20110422 [13->17] */
    char    cKinbr   [6 ];
    char    cHyflag  [2 ];
    char    cTemp    [21];
    char    cOrbrno  [13];
    char    cPayactno[33];
    char    cTxamt   [17];
    char    cSignday [9 ];
    char    cVocNum  [17] ;
    char    cVtype   [4];
    char    cC_VocNum[17] ;
    
    long    lQfDay=0;
    double  dTx_amt=0.0;
    struct  cashier_c      cCashier;     /***��Ʊ������Ϣ��***/
    struct  hv_uniontab_c  cHv_uniontab; /***�����к���Ϣ��***/
    struct  com_branch_c   cCom_branch;  /***������***/
    struct  com_parm_c     com_parm;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    struct  in_mst_c       in_mst;
    /****��ʼ������****/
    memset(cTel        , 0 , sizeof(cTel));
    memset(cMiya       , 0 , sizeof(cMiya));
    memset(cPono       , 0 , sizeof(cPono));
    memset(lPono       , 0 , sizeof(lPono));   /***NEWBP3����ˮ׼����****/
    memset(cKinbr      , 0 , sizeof(cKinbr));
    memset(cTemp       , 0 , sizeof(cTemp));
    memset(cVocNum     , 0 , sizeof(cVocNum));
    memset(cHyflag     , 0 , sizeof(cHyflag));
    memset(cTxamt      , 0 , sizeof(cTxamt));
    
    memset(cVtype      , 0 , sizeof(cVtype));
    memset(cC_VocNum   , 0 , sizeof(cC_VocNum));
    
    memset(cOrbrno     , 0 , sizeof(cOrbrno));
    memset(cSignday    , 0 , sizeof(cSignday));
    memset(cPayactno   , 0 , sizeof(cPayactno));
    memset(&cCom_branch ,0 , sizeof(cCom_branch));
    memset(&cCashier    ,0 , sizeof(cCashier));
    memset(&com_parm    ,0 , sizeof(com_parm));
    memset(&in_mst      ,0 , sizeof(in_mst));
    memset(&mdm_ac_rel  ,0 , sizeof(mdm_ac_rel));
    memset(&cHv_uniontab,0 , sizeof(cHv_uniontab));
    pub_base_sysinit();
    vtcp_log("[%s][%d]ͬ־��sp6343��ʼ��\n",__FILE__,__LINE__);
    /*��ǰ̨��Ļȡֵ*/
    get_zd_data("0320",cPono  );    /*��Ʊ����*/
    get_zd_data("0320",lPono  );    /*��Ʊ����NEWBP3����ˮ׼����*/
    get_zd_long("0450",&lQfDay );    /*ǩ������*/
    get_zd_data("0720",cHyflag);    /*��Ʊ��ѹ��ʽ0����1�˹�*/
    get_zd_data("0630",cMiya  );    /*��Ʊ��Ѻ*/
    get_zd_data("0030",cKinbr );    /*�����к�*/
    get_zd_data("0070",cTel   );    /*ǩ������Ա*/
    get_zd_data("0390",cTxamt);     /*��Ʊ���*/
    get_zd_data("0600",cVocNum);    /*��Աƾ֤����*/
    get_zd_data("0610",cC_VocNum);  /*�ͻ�ƾ֤����*/
    get_zd_data("0900",cVtype);     /*�ͻ�ƾ֤����*/
    
    vtcp_log("[%s][%d]cVocNum===[%s]\n",__FILE__,__LINE__,cVocNum);
    vtcp_log("[%s][%d] cPono   =[%s]\n",__FILE__,__LINE__,cPono   );
    vtcp_log("[%s][%d] lQfDay  =[%ld]\n",__FILE__,__LINE__,lQfDay );
    vtcp_log("[%s][%d] cHyflag =[%s]\n",__FILE__,__LINE__,cHyflag );
    vtcp_log("[%s][%d] cMiya   =[%s]\n",__FILE__,__LINE__,cMiya   );
    vtcp_log("[%s][%d] cKinbr  =[%s]\n",__FILE__,__LINE__,cKinbr  );
    vtcp_log("[%s][%d] cTel    =[%s]\n",__FILE__,__LINE__,cTel    );
    vtcp_log("[%s][%d] cTxamt  =[%s]\n",__FILE__,__LINE__,cTxamt  );
    
    str2dbl(cTxamt,strlen(cTxamt),0,&dTx_amt);
    vtcp_log("[%s][%d] dTx_amt  =[%f]\n",__FILE__,__LINE__,dTx_amt);
    
    /**�жϹ�Աƾ֤�Ƿ�ͱ�Ʊ����һ��**/
    /* if(memcmp(cVocNum+8,cPono+4, 8)!=0) 20110422 */
    if(memcmp(cVocNum,cPono, 16)!=0)       /* 20110422 */
    {
        vtcp_log("[%s][%d]ƾ֤����[%s]�ͱ�Ʊ����[%s]����\n",__FILE__,__LINE__,cVocNum,cPono);
        sprintf(acErrMsg,"ƾ֤����ͱ�Ʊ���벻��!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    /***�ж��Ƿ�����������***/
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("[%s][%d]�������Ĳ������޸ı�Ʊ[%s]\n",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"�������Ĳ������޸ı�Ʊ",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***�жϽ������Ƿ����***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&cCom_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]�޴˽��׻���[%s]\n",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"�޴˽��׻���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***���ݽ����к�,�õ��������к�***/
    if(GetOr_br_noByBr_no(cKinbr,cOrbrno)!=0)
    {
        vtcp_log("[%s][%d]���ݻ����Ų��Ҳ������кŴ���[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"���ݻ����Ų��Ҳ������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*����hv_uniotab,ȷ���������кŴ���*/
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&cHv_uniontab,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d] ��ѯ�����в������кŴ���[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"��ѯ�����в������кŴ���",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]\n",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+4,cPono,sizeof(cPono)-1);      /* 20110422 [8 -> 4] */
    vtcp_log("[%s][%d]cTemp==[%s]\n",__FILE__,__LINE__,cTemp);
    iRc=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",cTemp,lQfDay);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd����==[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    iRc=Cashier_Fet_Upd(&cCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*��Ʊ������Ϊ��*/
    if(pub_base_CompDblVal(cCashier.avbal,0.00)==0)
    {
        vtcp_log("[%s][%d]��Ʊ����Ϊ��==[%f]\n",__FILE__,__LINE__,cCashier.avbal);
        sprintf(acErrMsg,"��Ʊ����Ϊ��!\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***��ʼ�޸ı�Ʊ��¼***/
    if(memcmp(cCashier.optlrno,cTel,sizeof(cCashier.optlrno)-1)==0)
    {
        vtcp_log("[%s][%d]ǩ����¼�벻����ͬһ����Ա=[%s]\n",__FILE__,__LINE__,cTel);
        sprintf(acErrMsg,"¼���ǩ������ͬһ������Ա!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strncpy(g_pub_tx.reply,"P080",4);
        goto ErrExit;
    }
    if(cCashier.stat[0]!='0')/**��Ʊ״̬���ǵǼ�״̬����ǩ��**/    
    {
        vtcp_log("[%s][%d][%c]��Ʊ���ǵǼ�״̬[0]\n",__FILE__,__LINE__,cCashier.stat[0]);
        sprintf(acErrMsg,"��Ʊ���ǵǼ�״̬",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(cCashier.endday<g_pub_tx.tx_date)
    {
        vtcp_log("[%s][%d][%c]ǩ�����ڴ��ڱ�Ʊ������[0]\n",__FILE__,__LINE__,cCashier.endday);
        sprintf(acErrMsg,"ǩ�����ڴ��ڱ�Ʊ������\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(cCashier.avbal,dTx_amt)!=0)
    {
    	  vtcp_log("[%s][%d][%f]���׽���[%f]\n",__FILE__,__LINE__,cCashier.avbal,dTx_amt);
        sprintf(acErrMsg,"���׽���\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B103");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]׼����ʼ���±��¼\n",__FILE__,__LINE__);
    cCashier.checkflag[0]='0';               /**���ʱ�־**/
    cCashier.lactcls[0]=cCashier.stat[0];    /**�ϴ��˻�״̬**/
    cCashier.stat[0]='1';                    /**ǩ��״̬**/
    cCashier.regday=g_pub_tx.tx_date;        /**ǩ������**/
    memcpy(cCashier.chktlrno,cTel,sizeof(cCashier.chktlrno)-1);
    
    
    /***���Ӽ��ҵ��ί����ƾ֤***/
    pub_base_strpack(cC_VocNum);
    vtcp_log("[%s][%d]��ʼƾ֤���� type[%s][%s]\n",__FILE__,__LINE__,cVtype,cC_VocNum);

    /*���ƾ֤�����Ƿ����,��Ʊ��������������ҵ��ί�����ƾ֤*/
    iRc=pub_com_ChkNoteUse(cVtype);
    if( iRc )
    {
        sprintf(acErrMsg,"ƾ֤����ͣ��״̬!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N085");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] ƾ֤������鿪ʼ\n",__FILE__,__LINE__);
    /**����ʻ����ȴ���7λ,��ƾ֤����Ϊҵ��ί����**/
    /**�õ������ʺ�**/
    memcpy(cPayactno,cCashier.payactno,sizeof(cCashier.payactno)-1);
    vtcp_log("[%s][%d]cPayactno=[%s]\n",__FILE__,__LINE__,cPayactno);
    pub_base_strpack(cPayactno);
    if(strlen(cPayactno)>7 && memcmp(cVtype,"005",sizeof(cVtype)-1)==0)
    {
        g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",cPayactno);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"�ʺŲ�ѯ����!");
            strcpy(g_pub_tx.reply,"B117");
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d]Ms_mdm_ac_rel.ac_id=[%ld],cVocnum=[%s]\n",__FILE__,__LINE__,mdm_ac_rel.ac_id,cC_VocNum);
        vtcp_log("��ʼƾ֤���� type[%s][%s][%s][%d]\n",cVtype,cC_VocNum,__FILE__,__LINE__);
        g_reply_int=pub_com_NoteUse(mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn,cVtype,cC_VocNum,cC_VocNum,g_pub_tx.tel);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"����ƾ֤����!");
            strcpy(g_pub_tx.reply,"N080");
            WRITEMSG
            goto ErrExit;
        }
    }
    /** ƾ֤������� **/
    vtcp_log("[%s][%d]�ͻ�ƾ֤�������\n",__FILE__,__LINE__);
    memcpy(cCashier.ovoctype,cVtype,sizeof(cCashier.ovoctype)-1); /**�ͻ�ƾ֤����**/
    memcpy(cCashier.ovocnum,cC_VocNum,sizeof(cCashier.ovocnum)-1);/**�ͻ�ƾ֤����**/
    
    /**************����Ա�����б�Ʊƾ֤********************/
    ret=pub_com_ChkNoteUse("186");/*���ƾ֤�����Ƿ����,��Ʊǩ��������186ƾ֤*/
    if( ret )
    {
        sprintf(acErrMsg,"ƾ֤����ͣ��״̬!");
        strcpy(g_pub_tx.reply,"N085");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
    vtcp_log("[%s][%d] cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
    ret=pub_com_NoteUse(0,0,"186",cVocNum,cVocNum,cTel);/*ƾ֤���Ŵ���(���׹�����ʹ��ƾ֤)*/
    if( ret )
    {
        sprintf(acErrMsg,"��Աʹ��ƾ֤����!");
        strcpy(g_pub_tx.reply,"N007");
        WRITEMSG
        goto ErrExit;
    }
    memcpy(cCashier.voctype,"186",sizeof(cCashier.voctype)-1);/**ƾ֤����**/
    memcpy(cCashier.vocnum,cVocNum,sizeof(cCashier.vocnum)-1);/**ƾ֤����**/
    apitoa(cCashier.signday,8,cSignday);
    /*������Ѻ*/
    if(cHyflag[0]=='1')/**�ֹ���Ѻ**/
    {
        cCashier.addpassflg[0] = '1';
        memcpy(cCashier.passwd,cMiya,sizeof(cCashier.passwd)-1);
    }
    else if(cHyflag[0]=='0')/**������Ѻ**/
    {
        cCashier.addpassflg[0] = '0';
        vtcp_log("[%s][%d]�Զ���Ѻ\n",__FILE__,__LINE__);
        nCountBpMac(cCashier.orbrno,cCashier.payactno,cCashier.avbal*100,cCashier.pono,cSignday,cCashier.passwd);
        vtcp_log("[%s][%d]cCashier.passwd=[%s]\n",__FILE__,__LINE__,cCashier.passwd);
        
    }
    iRc=Cashier_Upd_Upd(cCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]�޸ı�Ʊ��¼����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�޸ı�Ʊ��¼����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();

    /**�Ǽǽ�����ˮ**/
    vtcp_log("[%s][%d]��Ʊ����cPono=[%s]\n",__FILE__,__LINE__,cPono);
    
    memcpy(g_pub_tx.beg_note_no,lPono,16); /***NEWBP3����ˮ׼����****/ /* 20110422 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***��������ֵ(����)***/
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",g_pub_tx.tx_br_no);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!cnt=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /**��鱾Ʊǩ�����Ƿ����**/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��鱾Ʊǩ����ʧ��!sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d' and ac_seqn=0 ",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��Ʊǩ��������!sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if(in_mst.sts[0]!='1'&&in_mst.sts[0]!='2')
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]\n",in_mst.ac_id,in_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    set_zd_data("1211",com_parm.val);
    set_zd_data("1214","01");
    set_zd_double("1218",atoi(cTxamt));
    set_zd_data("121A","���б�Ʊǩ��");
    set_zd_data("0620",cCashier.passwd);
    vtcp_log("[%s][%d]������Ѻ[%s]!\n",__FILE__,__LINE__,cCashier.passwd);
    set_zd_data("0520",cOrbrno);
    vtcp_log("[%s][%d]����˳��������!\n",__FILE__,__LINE__);
OkExit:
    strcpy( g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
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
