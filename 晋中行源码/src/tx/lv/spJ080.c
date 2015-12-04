/************************************************************
* �� �� ��:   spJ080.c
* ����������  С��ҵ��������ʴ���
*                ���ʱ�ΪSTAT_LZRECV2 'I' ״̬,�޸Ĳ���״̬����Ϊ���ܲ��޸�ֱ������
* ��    ��:   xyy
* ������ڣ�  2006-10-18 14:45
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"

static PAY_IN_AREA payin,payin_modif;
extern void lv_fullspace_len(char * con,int len);
extern char *pcLvGetDay();
static char *Getywname(char *);
static void fullspace_len(char * con,int len);
static char cQsbrno[BRNO_LEN+1];
static char cInBrno[BRNO_LEN+1];
int spJ080()
{

    int ret;
    char cTxamt[17];/* add by LiuHuafeng 2006-10-20 19:28 */
    char cCash_ac_no[33];
    memset(cCash_ac_no , 0 , sizeof(cCash_ac_no));
    memset(cInBrno     , 0 , sizeof(cInBrno));
    memset(cQsbrno     , 0 , sizeof(cQsbrno));
    memset(&payin      , 0 , sizeof(payin));
    memset(&payin_modif, 0 , sizeof(payin_modif));
    vtcp_log("[%s][%d]spJ080С��ҵ����ʴ�����BEGIN",__FILE__,__LINE__);
    get_fd_data( "0030", cInBrno);           /** ȡ�ý��׻��� **/

    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();
    if(iLvchkstat())
    {
        strncpy(g_pub_tx.reply,"P046",4);
        WRITEMSG
        goto ErrExit;
    }

    /****���payin*********/
    g_reply_int = iLvTisToPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"����iLvTisToPayin����,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] F_opcd=[%s]",__FL__,payin.F_opcd);


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
    /* add by LiuHuafeng 2006-10-20 19:30 ׼����ˮ�еĽ��׽��*/
    memcpy(cTxamt,payin.F_txamt,sizeof(payin.F_txamt));
    zip_space(cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&g_pub_tx.tx_amt1);
    g_pub_tx.tx_amt1/=100;
    /** end by LiuHuafeng 2006-10-20 19:30 */

    /**����payin��Ϣ**/
    memcpy((char*)&payin_modif, (char*)&payin,sizeof(payin_modif));

    /**����payin�ṹ����Ϣ����**/
    iLvResetPayin(&payin);
    g_reply_int = iLvPayinCheck0(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }

    /*** ȡ���������Ļ����� ***/
    memcpy(cQsbrno,QS_BR_NO,BRNO_LEN);

    if(memcmp(cInBrno,cQsbrno,BRNO_LEN)!=0)/**���������������**/
    {
        if(memcmp(cInBrno,payin.T_brno,BRNO_LEN)!=0)
        {
            sprintf( acErrMsg,"���ܴ����������д���,[%s][%d]cInBrno,T_brno", __FILE__,__LINE__,cInBrno,payin.T_brno);
            strcpy( g_pub_tx.reply, "S043" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
    }

    /* ���ñ��Ƿ���Ҫ���д��ʴ��� */
    if(payin.T_stat[0]!=payin.T_begstat[0])
    {
        vtcp_log("[%s][%d]�ñ�ҵ���Ѿ����й���������stat=[%c],begstat=[%c]!\n",__FILE__,__LINE__,payin.T_stat[0],payin.T_begstat[0]);
        sprintf( acErrMsg,"�ñ�ҵ���Ѿ����й���������,�����Դ��ʴ���!,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P077" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    if(payin.T_stat[0]!=STAT_LZGZ_QS)
    {
    /*** ��������Ҳ��һ��������Ҳ����֧�й��˵����
        if(memcmp(cInBrno,QS_BR_NO,BRNO_LEN)==0)
        {
            sprintf( acErrMsg,"֧�й���,�������Ĳ��ܴ���[%s][%d]cInBrno,T_brno", __FILE__,__LINE__,cInBrno);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
        }
    ********/
    }
    /********************add by xyy 2009-10-12 9:57:43 *************************/
    if(payin.T_stat[0]!=STAT_LZNMAC_ERR && payin.T_stat[0]!=STAT_LZLMAC_ERR && payin.T_stat[0]!=STAT_LZAMAC_ERR\
       && payin.T_stat[0]!=STAT_LZGZ_QS && payin.T_stat[0]!=STAT_LZGZ_BR )
    {
        sprintf( acErrMsg,"�ǹ���״̬���ܴ���!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy( g_pub_tx.reply, "S043" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] T_rcpstat=[%s] T_pkgno=[%.3s]",__FILE__,__LINE__,payin.T_rcpstat,payin.T_pkgno);
    if(memcmp(payin.T_pkgno,"008", 3)==0 || memcmp(payin.T_pkgno,"010",3)==0 || \
        memcmp(payin.T_pkgno,"011",3)==0)
    {
        if(memcmp(payin.T_rcpstat,"00",2)!=0)
        {
            sprintf( acErrMsg,"�Է�û�л�ִ�ɹ���ȥ�����˴���???");
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] T_rcpstat=[%s]",__FILE__,__LINE__,payin.T_rcpstat);
    }
    if(memcmp(payin.T_pkgno,"002", 3)==0 || memcmp(payin.T_pkgno,"004",3)==0 || \
        memcmp(payin.T_pkgno,"006",3)==0 || memcmp(payin.T_pkgno,"009",3)==0 )
    {
        sprintf( acErrMsg,"���ܴ�����౨��!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy( g_pub_tx.reply, "S043" );
        WRITEMSG
        goto ErrExit;
    }
    /******************end by xyy 2009-10-12 9:58:00**********************/
    vtcp_log("[%s][%d] opcd==[%.2s]",__FILE__,__LINE__,payin.F_opcd);
    vtcp_log("[%s][%d] pkgno===[%s]",__FILE__,__LINE__,payin.F_pkgno);
    if(memcmp(payin.F_opcd,OPCD_INACT_MODIFY,sizeof(payin.F_opcd))==0)
    {
        memcpy(payin.F_cashactno,payin_modif.F_cashactno,sizeof(payin.F_cashactno));
        memcpy(payin.F_cashname ,payin_modif.F_cashname,sizeof(payin.F_cashname));
        memcpy(payin.T_cashactno,payin_modif.F_cashactno,sizeof(payin.F_cashactno));
        memcpy(payin.T_cashname ,payin_modif.F_cashname,sizeof(payin.F_cashname));
        memcpy(payin.T_filler   ,cInBrno,BRNO_LEN);/**���ʻ���**/
        memcpy(payin.T_filler+BRNO_LEN,pcLvGetDay(),8);/**����ʱ��**/
        get_zd_data("0040",payin.T_inwssrno);/* ��ˮ��*/
        get_zd_data("0070",payin.T_inputno);/* ����Ա*/
        vtcp_log("[%s][%d] T_inwssrno==[%.6s]",__FILE__,__LINE__,payin.T_inwssrno);
        vtcp_log("[%s][%d] T_inputno===[%.6s]",__FILE__,__LINE__,payin.T_inputno);
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
        goto OkExit;
    }
    else if(memcmp(payin.F_opcd,OPCD_INACT,sizeof(payin.F_opcd))==0)
    {
        memcpy(cCash_ac_no,payin.T_cashactno,sizeof(payin.T_cashactno));
        vtcp_log("[%s][%d],cCash_ac_no=[%s]",__FILE__,__LINE__,cCash_ac_no);
        zip_space(cCash_ac_no);
        vtcp_log("[%s][%d],cCash_ac_no=[%s]",__FILE__,__LINE__,cCash_ac_no);
        if(memcmp(cCash_ac_no,"10101",strlen(cCash_ac_no))==0)
        {
              vtcp_log("[%s][%d],�տ��ʺ�Ϊ�ֽ��Ŀ,��ָ��һ�ʺŲ�������",__FILE__,__LINE__);
              sprintf( acErrMsg,"�տ��ʺ�Ϊ�ֽ��Ŀ,��ָ��һ�ʺŲ�������", __FILE__,__LINE__);
              strcpy( g_pub_tx.reply, "HV01" );
              set_zd_data(DC_GET_MSG,acErrMsg);
              WRITEMSG
              goto ErrExit;
        }
        payin.T_stat[0]=STAT_LZRECV2;
        get_zd_data("0040",payin.T_dealwssrno);/* ��ˮ��*/
        get_zd_data("0070",payin.T_dealno);/* ����Ա*/
        vtcp_log("[%s][%d] T_inwssrno==[%.6s]",__FILE__,__LINE__,payin.T_inwssrno);
        vtcp_log("[%s][%d] T_inputno===[%.6s]",__FILE__,__LINE__,payin.T_inputno);
        g_reply_int = iLvUpdRecFromPayin(&payin,0);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],����iLvUpdRecFromPayinʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"����iLvUpdRecFromPayinʧ��,[%s][%d]", __FILE__,__LINE__);
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
            strncpy(g_pub_tx.reply,"S023",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* ���ظ�ǰ̨ */
        g_reply_int = ilv10_PreTos();
        if(g_reply_int)
        {
            sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
            strncpy(g_pub_tx.reply,"P079",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        goto OkExit1;
    }
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit1:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"׼������ return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

char *Getywname(char * pkgno)
{
     char ywname[21];
     char cPkgno[4];
     int  iPkgno=0;

    memset(ywname,'\0',sizeof(ywname));
    memset(cPkgno, 0, sizeof(cPkgno));

    memcpy(cPkgno, payin.F_pkgno, sizeof(cPkgno)-1);
    iPkgno = apatoi(cPkgno,3);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__,iPkgno);
    switch(iPkgno)
    {
        case 1:
        memcpy(ywname,"С���մ�����",sizeof(ywname)-1);
        break;
        case 5:
        memcpy(ywname,"С�������",sizeof(ywname)-1);
        break;
        case 7:
        memcpy(ywname,"С���������",sizeof(ywname)-1);
        break;
        case  8:
        memcpy(ywname,"С���ջ�����",sizeof(ywname)-1);
        break;
        case 11:
        memcpy(ywname,"С�������",sizeof(ywname)-1);
        break;
        default:
                vtcp_log("[%s][%d]�����ҵ�����ͣ�",__FILE__,__LINE__);
            sprintf( acErrMsg,"�����ҵ������,[%s][%d]", __FILE__,__LINE__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            break;
    }
    vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,ywname);
    return ywname;
}
static void lv_fullspace_len(char * con,int len)
{
    char cTmpbuf[240];
    int i=0,iLen=0;
    memset(cTmpbuf, 0 , sizeof(cTmpbuf));
    memcpy(cTmpbuf,con,sizeof(cTmpbuf)-1);
    if(len<sizeof(cTmpbuf)-1) iLen=len;
    else iLen=sizeof(cTmpbuf)-1;
    for(i=0;i<iLen;i++)
    {
        if(cTmpbuf[i]=='\0') cTmpbuf[i]=' ';
    }
    memcpy(con,cTmpbuf, len);
}
ilv10_PreTos()
{
    int iRc;
    /****�����������payout2****/
    iLvSetPayout1(&payin);
    return(0);
}
