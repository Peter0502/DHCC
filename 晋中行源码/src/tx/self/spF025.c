/*************************************************
* �� �� ��:  spF025.c
* ���������� POS ����(ת�˵��̻����ʻ���)(�ʺ������е��̻�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��19��
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

int spF025()
{

    int ret;
    struct chnl_self_trade_c chnl_self_trade;
    char flag;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0370",g_pub_tx.ac_no);
    get_zd_char("0670",&flag);
    get_zd_long("0280",&g_pub_tx.old_trace_no);
    strcpy(g_pub_tx.cur_no,"01");

    /* �������豸�����ռǽṹ��ֵ */
    
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
    strcpy(chnl_self_trade.ac_no,g_pub_tx.ac_no);
    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    /* �����׶���������ˮ�� */

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;
    chnl_self_trade.stra = g_pub_tx.old_trace_no;
    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,"2");
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"PSXF");
    strcpy(chnl_self_trade.add_ind,"0");
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

