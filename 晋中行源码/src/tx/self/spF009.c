/*************************************************
* �� �� ��:  spF009.c
* ���������� ATM ת��(ת�뷽�ӽ���)
*
* ��    ��:  rob
* ������ڣ� 2003��2��17��
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

int spF009()
{

    int ret;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",chnl_self_trade.ac_no); /* ת�����ʺ� */
    get_zd_data("0310",g_pub_tx.crd_no);  /* ת�뿨�Ż��˺� */
    get_zd_double("0400",&g_pub_tx.tx_amt1); /* ���׽�� */
    
   sprintf(acErrMsg," ת�����ʺ�[%s] ת�뿨��[%s] ���׽��[%lf]", chnl_self_trade.ac_no, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
        WRITEMSG
    /* ��鿨�ĺϷ��� 
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }*/

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    /* ���ô��� 

    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); * ת�� *
    g_pub_tx.hst_ind[0] = '1';   * �ռ�����ϸ *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"atmת��--ת�뷽������!!!");
        WRITEMSG
        goto ErrExit;
    }*/

    /* �������豸�����ռǽṹ��ֵ */
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
    strcpy(chnl_self_trade.ac_no1,g_pub_tx.crd_no);
    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"1");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    /* �����׶���������ˮ�� */
    get_zd_long("0520", &chnl_self_trade.stra); 
    get_zd_long("0450", &chnl_self_trade.dz_date); 

   sprintf(acErrMsg," 0520 stra====[%ld] 0450 dz_date==[%ld]", chnl_self_trade.stra, chnl_self_trade.dz_date);
        WRITEMSG
        
    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind);
    strcpy(chnl_self_trade.info_source,"0");
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"ATMZ");
    strcpy(chnl_self_trade.add_ind,"0");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");


    /* �Ǽ������豸�����ռ� */
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"AT07");
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

