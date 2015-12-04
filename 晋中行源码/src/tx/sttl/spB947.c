/********************************************************************
* �� �� ��:  spB947.c
* ���������� ���������׷��Ȩת����ת�����˹��ܣ�����ԭ(ת)���ֵ�״̬��
*
* ��    ��:  xyy
* ������ڣ� 2009-9-18 21:28:50
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*********************************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "com_parm_c.h"

int spB947()
{
    char tmp[10];
    char cKinbr[BRNO_LEN+1];
	  char	cTinAcNo[14];				/* ת�����ʺ� */
    struct mo_discnt_c sMoDiscnt, sMoDiscnt1;    /* ���ֵǼǱ� */
    double    dAmt1=0.00, dAmt2=0.00, dAmt3=0.00;
    int       ret=0;
		char cPn_knd[2];
    memset(&sMoDiscnt , 0x00, sizeof(struct mo_discnt_c));
    memset(&sMoDiscnt1, 0x00, sizeof(struct mo_discnt_c));
    memset(&g_pub_tx  , 0x00, sizeof(g_pub_tx));
    memset(cKinbr  ,   0 , sizeof(cKinbr));
    memset(cPn_knd  ,   0 , sizeof(cPn_knd));
    memset(cTinAcNo  ,   0 , sizeof(cTinAcNo));

    /* ���ݳ�ʼ�� */
    pub_base_sysinit();
    strcpy(g_pub_tx.mdm_code, NULL_MDM_CODE);    /* �ս��ʽ��ʴ��� */

    /* ��ǰ̨��Ļȡֵ */
    get_zd_data("0900", sMoDiscnt.note_type);   /* Ʊ������ */
    get_zd_data("0600", sMoDiscnt.pnote_no);    /* Ʊ�ݺ� */
	  get_zd_data( "0370", cTinAcNo );				/* ת�����ʺ� */
    get_zd_data("0670", sMoDiscnt.tpay_ac_type);/* �����˻����� */
    get_zd_data("0910", cKinbr);
    get_zd_data("0980",cPn_knd);
    if(cPn_knd[0]=='E')
    {
    	get_zd_data("1032",sMoDiscnt.pnote_no);
    }
    sprintf(acErrMsg, "pnote_no=[%s]", sMoDiscnt.pnote_no);
    WRITEMSG
    /*
     * ���Ʊ���Ƿ���ڣ�״̬�Ƿ�Ϊ¼�룬���������Ƿ�Ϊ���ת��,�޸����ֵ�
     * �ǲ� 
     */
    ret = Mo_discnt_Dec_Upd(g_pub_tx.reply, "note_type='%s' and pnote_no='%s' \
                and ((pn_type='4' and po_knd='P') or (pn_type='5' and po_knd='E')) and sts!='*' and br_no ='%s' ", \
          sMoDiscnt.note_type, sMoDiscnt.pnote_no,cKinbr);/**NEWDP**/
    if (ret) {
        sprintf(acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_discnt_Fet_Upd(&sMoDiscnt, g_pub_tx.reply);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "��Ʊ��δ�����ʽת����ת��!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B249");
        goto ErrExit;
    }
    else if (ret) 
    {
        sprintf(acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    if (sMoDiscnt.sts[0] == '3') 
    {
        sprintf(acErrMsg, "�˱������Ѽ���!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B148");
        goto ErrExit;
    }
    /* ����Ʊ�����ͺ�Ʊ�ݺŲ�ѯ���ֵǼǱ���Ʊ��¼��ʱ����Ϣ */
    ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' and pnote_no='%s' \
        and (pn_type='1' or pn_type='2' or pn_type='3') and sts='4' ", sMoDiscnt.note_type, sMoDiscnt.pnote_no);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "�޴�Ʊ����Ϣ!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B130");
        goto ErrExit;
    } 
    else if (ret) 
    {
        sprintf(acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /* �޸�ת���ֵǼǱ���״̬Ϊ���� */
    strcpy(sMoDiscnt.sts, "3");    /* ��־��3 ���� */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* ת�����ʺ� */
    sMoDiscnt.wrk_date = g_pub_tx.tx_date;    /* ��ˮ���� */
    sMoDiscnt.trace_no = g_pub_tx.trace_no;    /* ������ˮ�� */

    ret = Mo_discnt_Upd_Upd(sMoDiscnt, g_pub_tx.reply);
    if (ret) 
    {
        sprintf(acErrMsg, "�޸����ֵǼǱ���[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_discnt_Clo_Upd();
    
    get_zd_double("1001", &dAmt1);    /* ת������Ϣ */
    get_zd_double("1002", &dAmt2);    /* Ʊ����� */
    dAmt3 = dAmt2 - dAmt1;            /* ������ */
    set_zd_double("1003", dAmt3);
    vtcp_log("[%s][%d] [%f]-[%f]=[%f]",__FILE__,__LINE__,dAmt2,dAmt1,dAmt3);

    /* ����ˮ��־ */
    strcpy(g_pub_tx.cur_no, "01");
    strcpy(g_pub_tx.no_show, "0");
    ret = pub_ins_trace_log();
    if (ret) 
    {
        sprintf(acErrMsg, "����ˮ��־��");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] ��׷��Ȩת���ּ������!!",__FILE__,__LINE__);
OkExit:
    memset(tmp,0,sizeof(tmp));
    get_zd_data("0670",tmp);
    vtcp_log("[%s][%d],CESHI-0670[%s]",__FILE__,__LINE__,tmp);
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
