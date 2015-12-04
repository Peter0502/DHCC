/***************************************************************************/
/**�ļ�����: sp6346.c                                                      */
/**��������: ���б�Ʊ�˻�                                                  */
/**��������: ʵ�ָ���֧���˻ر�Ʊ.                       		           */
/**��    �ģ�xyy      ���Ӵ���������                                     */        
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
sp6346()
{
    int     iRc=0,ret=0;
    char    cTel[5];
    char    cKinbr[6];
    char    cBrno[6];
    char    cThflag[2];
    char    cPono[21];
    char    cTemp[17];    /* 20110422 [13->17] */
    char    cContent[61];
    double  dTxamt=0.0;
    long    lSignday=0;
    struct  com_branch_c   sCom_branch;  /****������***/
    struct  com_parm_c     com_parm;     /****����������*****/
    struct  in_mst_c       in_mst;       /****�ڲ�̨�����ļ�******/
    struct  cashier_c      sCashier;     /***��Ʊ������Ϣ��*******/
    struct  mdm_ac_rel_c   mdm_ac_rel;   /****�������˻���ϵ��****/
    /****��ʼ������****/
    memset(cTel        , 0 , sizeof(cTel));
    memset(cPono       , 0 , sizeof(cPono));
    memset(cTemp       , 0 , sizeof(cTemp));
    memset(cBrno       , 0 , sizeof(cBrno));
    memset(cKinbr      , 0 , sizeof(cKinbr));
    memset(cThflag     , 0 , sizeof(cThflag));
    memset(cContent    , 0 , sizeof(cContent));
    memset(&com_parm    ,0 , sizeof(com_parm));
    memset(&in_mst      ,0 , sizeof(in_mst));
    memset(&mdm_ac_rel  ,0 , sizeof(mdm_ac_rel));
    memset(&sCashier   , 0 , sizeof(sCashier));
    memset(&sCom_branch, 0 , sizeof(sCom_branch));
    pub_base_sysinit();
    /*��ǰ̨��Ļȡֵ*/
    get_zd_long("0450",&lSignday);  /*ǩ������*/
    get_zd_data("0070",cTel);      /*ǩ������*/
    get_fd_data("0320",cTemp);     /*��Ʊ����*/
    get_zd_data("0030",cKinbr);    /*�����к�*/
    get_zd_data("0670",cThflag);   /*�˻�����*/
    get_zd_data("0250",cContent);  /*ժ    Ҫ*/
    vtcp_log("[%s][%d] cTel=[%s]\n",__FILE__,__LINE__,cTel);
    vtcp_log("[%s][%d] cTemp=[%s]\n",__FILE__,__LINE__,cTemp);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    vtcp_log("[%s][%d]cContent=[%s]\n",__FILE__,__LINE__,cContent);
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);     /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    /*�������Ƿ����*/
    iRc=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]�޴˽��׻���\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"�޴˽��׻���");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*�޸ı�Ʊ״̬*/
    g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",cPono,lSignday);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]��Ʊ������\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]��Ʊ������[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    g_reply_int=Cashier_Fet_Upd((char*)&sCashier,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]�����˱�Ʊ�Ŵ���\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]�����˱�Ʊ�Ŵ���[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(memcmp(sCashier.brno,cKinbr,sizeof(cKinbr)-1)!=0)
    {
        vtcp_log("[%s][%d]�뵽ǩ��������δ���˻�\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]�뵽ǩ��������δ���˻�\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='1')
    {
        vtcp_log("[%s][%d]��Ʊ����ǩ��״̬\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]��Ʊ����ǩ��״̬\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.checkflag[0]='0';
    pub_base_strpack(cContent);
    sCashier.stat[0]='6';/*��Ʊ�˻�״̬*/
    sCashier.lactcls[0]=sCashier.stat[0];
    memcpy(sCashier.filler,cContent,strlen(cContent));
    memcpy(sCashier.jqtlrno,cTel,sizeof(sCashier.jqtlrno)-1);
    memcpy(cBrno,sCashier.brno,sizeof(cBrno)-1);/**ǩ������**/
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]���±�Ʊ����\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]���±�Ʊ����[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    /*Ҫ��spA016�ӽ��׸�ֵ*/
    vtcp_log("[%s][%d]���±�Ʊ��¼���\n",__FILE__,__LINE__);
    /**�Ǽǽ�����ˮ**/
    memcpy(g_pub_tx.beg_note_no,cTemp,16);/***NEWBP3***/  /* 20110425 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**���ǩ���еĶҸ��ʺ�**/
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",cBrno);
    if(ret)
    {
        vtcp_log("[%s][%d]�жҸ�����ʱ�˻�����[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!ret=[%d]\n",__FILE__,__LINE__,ret);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    /**��鱾Ʊǩ�����Ƿ����**/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        vtcp_log("[%s][%d]��鱾Ʊǩ����ʧ��[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]��鱾Ʊǩ����ʧ��!ret=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]��Ʊǩ��������[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]��Ʊǩ��������!ret=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    if(in_mst.sts[0]!='1'&&in_mst.sts[0]!='2')
    {
        vtcp_log("[%s][%d]�ʺ�״̬������\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    set_zd_double("0390",sCashier.avbal);
    set_zd_double("0400",sCashier.avbal);
    set_zd_data("0210","01");
    set_zd_data("0880","δ���˻�");
    set_zd_data("0300",com_parm.val);
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
