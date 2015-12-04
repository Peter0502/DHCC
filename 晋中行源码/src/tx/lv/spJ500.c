/*****************************************************
* �� �� ��:  spJ500.c
* ���������� ���б�Ʊ�Ҹ�����¼��
* ��    ��:  xyy
* ������ڣ� 2008-1-9 16:47 
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

int spJ500()
{
    int iRc = 0;
    struct lv_wbctl_c lv_wbctl;
    struct lv_pkgreg_c lv_pkgreg;
    memset(&payin,0,sizeof(payin));
    memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
    memset(&lv_wbctl,0,sizeof(lv_wbctl));
    memset(&lvca, 0 , sizeof(lvca));
    vtcp_log("[%s][%d] spJ500��ʼ�\n",__FILE__,__LINE__);

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
    /* �����Ѽ�� */
    vtcp_log("[%s][%d] �����Ѽ��\n",__FILE__,__LINE__);
    iLvCountFeeamt(&payin);
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
    vtcp_log("[%s][%d] payactno[%s]\n",__FILE__,__LINE__,payin.F_payactno);
    vtcp_log("[%s][%d] cashactno[%s]\n",__FILE__,__LINE__,payin.F_cashactno);
    int iOrderno=0;
    vtcp_log("[%s][%d] ׼������֧���������\n",__FILE__,__LINE__);
    iOrderno=iLvGetOrderno();
    if(iOrderno<=0)
    {
        sprintf( acErrMsg,"�õ�֧��������ų���,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
    /***����������������һ���µļ�¼****/
    vtcp_log("[%s][%d] payin�ṹ��F to T\n",__FILE__,__LINE__);
    g_reply_int = iLvNewRecFromPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"����һ���µļ�¼����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /***�����¼�¼****/
    vtcp_log("[%s][%d] ����ǼǱ�����ϸ��¼\n",__FILE__,__LINE__);
    g_reply_int = iLvUpdRecFromPayin(&payin,1);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
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
        sprintf( acErrMsg,"iLvUpdRecFromPayin��,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]����payin�ṹ�����\n",__FILE__,__LINE__);
    /* ����ˮ��־ */
    g_reply_int = pub_ins_trace_log();
    if ( g_reply_int )
    {
        sprintf( acErrMsg, "����ˮ��־��" );
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
    /*****д������ϸ�� ******/
    iRc=ilv10_PreTos();
    if(iRc)
    {
        sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
        strncpy(g_pub_tx.reply,"P079",4);
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
