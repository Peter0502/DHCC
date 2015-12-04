/*****************************************************
* �� �� ��:  spJ501.c
* ���������� ���б�Ʊ�Ҹ������޸�
* ��    ��:  xyy
* ������ڣ� 2008-1-11 10:19 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "lv_wbctl_c.h"
static    PAY_IN_AREA payin;
static    PAY_IN_AREA payin_modif;
int spJ501()
{
    int iRc = 0;
    char cTxamt[17];
    struct lv_wbctl_c  lv_wbctl;
    struct lv_pkgreg_c lv_pkgreg;
    
    memset(cTxamt       , 0, sizeof(cTxamt));
    memset(&lvca        , 0, sizeof(lvca));
    memset(&payin       , 0, sizeof(payin));
    memset(&lv_wbctl    , 0, sizeof(lv_wbctl));
    memset(&lv_pkgreg   , 0, sizeof(lv_pkgreg));
    memset(&payin_modif , 0, sizeof(payin_modif));

    vtcp_log("[%s][%d] spJ501��ʼ�\n",__FILE__,__LINE__);

    pub_base_sysinit();
    vtcp_log("[%s][%d] ���ϵͳ״̬\n",__FILE__,__LINE__);
    if(iLvchkstat())
    {
        strncpy(g_pub_tx.reply,"P046",4);
        WRITEMSG
        goto ErrExit;
    }
    /****���payin*********/
    vtcp_log("[%s][%d] ���payin�ṹ��\n",__FILE__,__LINE__);
    g_reply_int = iLvTisToPayin(&payin);
    if( g_reply_int )
    {
        sprintf( acErrMsg,"[%s][%d]����iHvTistoNotpay����", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P015" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] payin.F_lw_ind = [%c]",__FL__,payin.F_lw_ind[0]);
    
    
     /* ��������������Ϣ�Ƿ���� */
    g_reply_int = iLvFetchRecToPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"����iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /****������С���״̬�ǲ���¼��****/
    if(payin.T_stat[0]!=STAT_WZLR)
    {
        vtcp_log("%s,%d,���ײ�Ϊ¼��״̬[%c],�����޸�\n",__FILE__,__LINE__,payin.T_stat[0]);
        sprintf( acErrMsg,"���ײ�Ϊ¼��״̬,�����޸�[%s][%d]\n", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;

    }
    memcpy(cTxamt,payin.F_txamt,sizeof(payin.F_txamt));
    zip_space(cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&g_pub_tx.tx_amt1);
    g_pub_tx.tx_amt1/=100;
    
    /**����payin��Ϣ**/
    memcpy((char*)&payin_modif, (char*)&payin,sizeof(payin_modif));
    
    /**����payin�ṹ����Ϣ����**/
    iLvResetPayin(&payin);
    
    /****�������****/
    g_reply_int = iLvPayinCheck0(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /****Ҫ�ؼ��****/
    vtcp_log("[%s][%d] Ҫ�ؼ��iLvPayinCheck4\n",__FILE__,__LINE__);
    g_reply_int = iLvPayinCheck4(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /****�кż��****/
    vtcp_log("[%s][%d] �кż��iLvPayinCheck1\n",__FILE__,__LINE__);
    g_reply_int = iLvPayinCheck1(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] �տ��ʺż��iLvPayinCheck3\n",__FILE__,__LINE__);
    /****�տ��ʺż��****/
    g_reply_int=iLvPayinCheck3(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"�տ��ʺż�����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /***�����������Ϣ������**/
    vtcp_log("[%s][%d] �տ�����Ϣ\n",__FILE__,__LINE__);
    memcpy(payin.F_cashactno   ,payin_modif.F_cashactno   ,sizeof(payin.F_cashactno));
    memcpy(payin.F_cashname    ,payin_modif.F_cashname    ,sizeof(payin.F_cashname));
    memcpy(payin.T_cashactno   ,payin_modif.F_cashactno   ,sizeof(payin.F_cashactno));
    memcpy(payin.T_cashname    ,payin_modif.F_cashname    ,sizeof(payin.F_cashname));
    memcpy(payin.F_cashaddress ,payin_modif.F_cashaddress ,sizeof(payin.F_cashaddress));
    memcpy(payin.T_cashaddress ,payin_modif.F_cashaddress ,sizeof(payin.F_cashaddress));
    vtcp_log("[%s][%d] ��������Ϣ\n",__FILE__,__LINE__);
    memcpy(payin.F_payactno   ,payin_modif.F_payactno   ,sizeof(payin.F_payactno));
    memcpy(payin.F_payname    ,payin_modif.F_payname    ,sizeof(payin.F_payname));
    memcpy(payin.T_payactno   ,payin_modif.F_payactno   ,sizeof(payin.F_payactno));
    memcpy(payin.T_payname    ,payin_modif.F_payname    ,sizeof(payin.F_payname));
    memcpy(payin.F_payaddress ,payin_modif.F_payaddress ,sizeof(payin.F_payaddress));
    memcpy(payin.T_payaddress ,payin_modif.F_payaddress ,sizeof(payin.F_payaddress));
    vtcp_log("[%s][%d] ��������Ϣ\n",__FILE__,__LINE__);
    memcpy(payin.F_acbrno     ,payin_modif.F_acbrno     ,sizeof(payin.F_acbrno));
    memcpy(payin.T_acbrno     ,payin_modif.F_acbrno     ,sizeof(payin.F_acbrno));
                                                        
    memcpy(payin.F_payno      ,payin_modif.F_payno      ,sizeof(payin.F_payno ));
    memcpy(payin.T_payno      ,payin_modif.F_payno      ,sizeof(payin.F_payno ));
    vtcp_log("[%s][%d] ��������Ϣ\n",__FILE__,__LINE__);
    memcpy(payin.F_cashno     ,payin_modif.F_cashno     ,sizeof(payin.F_cashno ));
    memcpy(payin.T_cashno     ,payin_modif.F_cashno     ,sizeof(payin.F_cashno ));
    memcpy(payin.F_txamt      ,payin_modif.F_txamt      ,sizeof(payin.F_txamt));
    memcpy(payin.T_txamt      ,payin_modif.F_txamt     ,sizeof(payin.F_txamt));
    
    memcpy(payin.F_content     ,payin_modif.F_content     ,sizeof(payin.F_content ));
    memcpy(payin.T_content     ,payin_modif.F_content     ,sizeof(payin.F_content ));
    
    get_zd_data("0040",payin.T_checkwssrno);/* ��ˮ��----�޸���ˮ*/
    get_zd_data("0070",payin.T_checkno);/* ����Ա--�޸Ĳ���Ա*/
    vtcp_log("[%s][%d] T_checkwssrno==[%.6s]",__FILE__,__LINE__,payin.T_checkwssrno);
    vtcp_log("[%s][%d] T_checkno===[%.6s]",__FILE__,__LINE__,payin.T_checkno);
    /*****�����****/
    vtcp_log("[%s][%d]F_packid=[%s]\n",__FILE__,__LINE__,payin.F_packid);
    payin.F_packend[0]='1';    /**ʵʱҵ���һ������ֱ�Ӿͷ��**/
    iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
    if(iRc!=PCHK_UPDATE && iRc!=PCHK_NEW)
    {
        vtcp_log("[%s][%d]======0�°�,1�Ѵ���,2�Ѹ���===iRc=[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"�޷�ƴ��ָ����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    memcpy(payin.T_packday,payin.A_tbsdy,sizeof(payin.T_packday));
    memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
    vtcp_log("[%s][%d] pkgid====[%s]\n",__FILE__,__LINE__,payin.F_packid);
    vtcp_log("[%s][%d] packday==[%s]\n",__FILE__,__LINE__,payin.T_packday);
    g_reply_int = iLvUpdRecFromPayin(&payin,0);
    if(g_reply_int)
    {
        vtcp_log("%s,%d,���µǼǲ�ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
        sprintf( acErrMsg,"���µǼǲ�ʧ��,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /* ����ˮ��־ */
    g_reply_int = pub_ins_trace_log();
    if ( g_reply_int )
    {
        sprintf( acErrMsg, "����ˮ��־��" );
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /* ���ظ�ǰ̨ */
    g_reply_int = ilv10_PreTos();
    if(g_reply_int)
    {
        sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]���������\n",__FILE__,__LINE__);
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
/******************************************************************************
*                                                                             *
*    FUNCTION:           lv10_PreTos()                                        *
*   DESCRIPTION:       Ϊ���ذ���ֵ                                           *
******************************************************************************/
ilv10_PreTos()
{
    int iRc;
    /****�����������payout2****/
    iLvSetPayout1(&payin);
    return(0);
}
