/************************************************************************
* �� �� ��:   spJ083.c
* ����������  ��ͬ���
*
* ��    ��:   dongxy
* ������ڣ�  2006-12-13
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#define loc_t sys_loc_t
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "lv_pldsfqugx_c.h"


spJ083()
{
    int iRet,k;
    char  cOpflag[2];
    char  cErrcode[61];
    TLVPACKAGE lv;
    struct lv_pldsfqugx_c lv_pldsfqugx;

    memset(cOpflag, 0x00, sizeof(cOpflag));
    memset(cErrcode, 0x00, sizeof(cErrcode));
    memset(&lv_pldsfqugx, 0x00, sizeof(lv_pldsfqugx));

    vtcp_log("[%s][%d]==========��ʼ��ʼ����",__FILE__,__LINE__);
    pub_base_sysinit();
    
    get_zd_long("0440",&lv_pldsfqugx.wt_date);     /** ί������ **/
    get_zd_data("0670", lv_pldsfqugx.xy_ind);  /** ��ͬ��Э�飩���� **/
    get_zd_data("0680", lv_pldsfqugx.ch_ind);  /** �����ʽ **/
    get_zd_data("0300", lv_pldsfqugx.pay_br_no);  /** �������к� **/
    get_zd_data("0310", lv_pldsfqugx.pay_qs_no);  /** �����������к� **/
    get_zd_data("0320", lv_pldsfqugx.pay_opn_br_no);  /** �����˿������к� **/
    get_zd_data("0330", lv_pldsfqugx.pay_ac_no);  /** �������˺� **/
    get_zd_data("0961", lv_pldsfqugx.xyno);
    get_zd_data("0962", lv_pldsfqugx.pay_name);
    get_zd_data("0963", lv_pldsfqugx.pay_addr);
    get_zd_data("0964", lv_pldsfqugx.content);
    
    vtcp_log("[%s][%d]==========lv_pldsfqugx.wt_date=[%d]",__FILE__,__LINE__,lv_pldsfqugx.wt_date);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.xy_ind=[%c]",__FILE__,__LINE__,lv_pldsfqugx.xy_ind[0]);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.ch_ind=[%c]",__FILE__,__LINE__,lv_pldsfqugx.ch_ind[0]);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_br_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_br_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_qs_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_qs_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_opn_br_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_opn_br_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_ac_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_ac_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.xyno=[%s]",__FILE__,__LINE__,lv_pldsfqugx.xyno);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_name=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_name);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_addr=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_addr);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.content=[%s]",__FILE__,__LINE__,lv_pldsfqugx.content);

    lv_create(&lv);
    iRet=lv_dsf_deal40507(&lv_pldsfqugx);
    if(iRet)
    {
            vtcp_log("[%s][%d],���״���ʧ��![%d]\n",__FILE__,__LINE__,iRet);
            cOpflag[0] = '1';
            memcpy(cErrcode, "���״���ʧ��",12);
            set_zd_data("0690", cOpflag);
            set_zd_data("0740", cErrcode);
            sprintf(acErrMsg,"RET [%d]",iRet);
            WRITEMSG
            goto ErrExit;
    }
    else 
    {
            cOpflag[0] = '0';
    }

    lv_destroy(&lv);

    /** ��ʼ��֯���صı��� **/
    set_zd_data("0690", cOpflag);  /** �ɹ�ʧ�ܱ�־ **/
    set_zd_data("0280", lv_pldsfqugx.orderno);  /** ������� **/
    set_zd_long("0290", lv_pldsfqugx.wt_date);  /** ��ί������ **/
    set_zd_data("0440", lv_pldsfqugx.packid);  /** ��id **/
    
    set_zd_data("0961", lv_pldsfqugx.xyno);  /** ��ͬЭ���**/
    set_zd_data("0962", lv_pldsfqugx.pay_name);  /** ���������� **/
    set_zd_data("0963", lv_pldsfqugx.pay_addr);  /** �����˵�ַ **/
    set_zd_data("0964", lv_pldsfqugx.content);  /** ���� **/
    
    set_zd_data("0670", lv_pldsfqugx.xy_ind);  /** ��ͬ��Э�飩���� **/
    set_zd_data("0680", lv_pldsfqugx.ch_ind);  /** �����ʽ **/
    set_zd_data("0320", lv_pldsfqugx.pay_br_no);  /** �������к� **/
    set_zd_data("0330", lv_pldsfqugx.pay_qs_no);  /** �����������к� **/
    set_zd_data("0370", lv_pldsfqugx.pay_opn_br_no);/** �����˿������к� **/
    set_zd_data("0810", lv_pldsfqugx.pay_ac_no);  /** �������˺� **/

    /*beg add by Chenggx 2011-6-16  ���� �Ǽ���ˮ*/
    /* �Ǽǽ�����ˮ */
    if ( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
    /*beg add by Chenggx 2011-6-16  ����*/
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
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
