/*************************************************
* �� �� ��:  spF030.c
* ���������� POS ����
*            (���̻����ʻ���ת��)(�ʺ��Ǳ��е��̻�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"

int spF030()
{

    int ret;
    char flag;
    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",chnl_marchinfo.mrchno); /* �̻��� */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_long("0780",&g_pub_tx.old_trace_no); /* ԭ�ն���ˮ�� */


    /* �����̻��Ų�ѯ�̻��˺� */
    ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�̻��ʺŴ���!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(g_pub_tx.crd_no,chnl_marchinfo.mrchacno);

    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ����ȡ��� */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */
    g_pub_tx.hst_ind[0] = '1';   /* �ռ�����ϸ */

    
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos���ѳ���--�̻��ʻ�ȡ�����!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ����ԭ������ˮ��״̬ */
    ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld",
          g_pub_tx.old_trace_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯԭPOS���ѵ������豸�����ռǴ���!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Chnl_self_trade_Fet_Upd(&chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }

    strcpy(chnl_self_trade.stat,"1");

    ret = Chnl_self_trade_Upd_Upd(chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }
    Chnl_self_trade_Clo_Upd();

    /* �������豸�����ռǽṹ��ֵ */
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

    get_zd_data("0370",g_pub_tx.ac_no);
    get_zd_char("0670",&flag);
    if (flag == '0')
    {
        strcpy(chnl_self_trade.ac_no,g_pub_tx.ac_no);
    }
    else
    {
        strcpy(chnl_self_trade.ac_no1,g_pub_tx.ac_no);
    }

    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
    /* �����׶���������ˮ�� */

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    get_zd_long("0280",&g_pub_tx.old_trace_no);
    chnl_self_trade.stra = g_pub_tx.old_trace_no;

    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,"2");
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"PSCX");
    strcpy(chnl_self_trade.add_ind,"1");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");

    /* �Ǽ������豸�����ռ� */
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

