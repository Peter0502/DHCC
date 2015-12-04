/*************************************************
* �� �� ��:  spF033.c
* ���������� POS �����˻�
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
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"

int spF033()
{

    int ret;
    long oldDate;

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

    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* �̻��� */
    /*get_zd_long("0780",&g_pub_tx.old_trace_no);  ԭ�ն���ˮ�� */
    get_zd_long("0440",&g_pub_tx.old_trace_no);  /* ԭ����ƽ̨��ˮ��  */
    get_zd_long("0450",&oldDate);  /* ԭ�������� */

    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ��鿨��֧ȡ��ʽ */
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    /* ���֧ȡ��ʽ�ĺϷ��� */
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����֧ȡ�������!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        goto ErrExit;
    }

    /* ����̻���״̬  Remmed by ChenMing For Test
    ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����̻���״̬����!!!");
        WRITEMSG
        goto ErrExit;
    }*/

    /* ��ѯ�����豸�����ռ�(ԭ����) */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld",
          g_pub_tx.old_trace_no, oldDate);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����豸�����ռǱ����!! [%d]",ret);
        WRITEMSG
        sprintf(acErrMsg,"trace_no= [%ld]",g_pub_tx.old_trace_no);
        WRITEMSG
        goto ErrExit;
    }

    /* ���ñ���ˮ�Ƿ����� ������״̬(�ѱ�ȡ�� �����������˻�) */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"�ñ���ˮ������������!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* ���ԭ�������ں�������Ƿ�һ��
    if (oldDate != chnl_self_trade.tx_date)
    {
        sprintf(acErrMsg,"ԭ�������ں�ԭ��ˮ�ŵĽ������ڲ�һ��!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
     }*/

    /* ��鿨�ź�ԭ��ˮ��Ŀ����Ƿ�һ�� */
    pub_base_strpack(g_pub_tx.crd_no);
    pub_base_strpack(chnl_self_trade.ac_no);
    if (strcmp(g_pub_tx.crd_no,chnl_self_trade.ac_no) != 0)
    {
        sprintf(acErrMsg,"����Ŀ��ź�ԭ���׿��Ų�һ��!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
   
    /* ���ԭ���������Ƿ���ȷ 
    if (oldDate != chnl_self_trade.tx_date)
    {
        sprintf(acErrMsg,"ԭ�������ڲ�һ��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }*/

    /* ����Ƿ񳬳�һ�������� */
    if (g_pub_tx.tx_date - oldDate > 30)
    {
        sprintf(acErrMsg,"�˻�������һ����!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* ��齻�׽���Ƿ񳬳�ԭ������ */
    if (g_pub_tx.tx_amt1 > chnl_self_trade.tx_amt)
    {
        sprintf(acErrMsg,"�˻�������ԭ������!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }
    
    /* ���POS�ն˺��Ƿ�һ�� */
    pub_base_strpack(g_pub_tx.tty);
    pub_base_strpack(chnl_self_trade.ttyn);
    if (strcmp(g_pub_tx.tty,chnl_self_trade.ttyn) != 0)
    {
        sprintf(acErrMsg,"�˻�ֻ����ͬһ̨POS�ն���!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* ����̻����Ƿ�һ�� Remmed by ChenMing for Test
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"�̻��Ų�һ�²����˻�!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }*/
    
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */
    g_pub_tx.hst_ind[0] = '1';   /* �ռ�����ϸ */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos�˻�--�ͻ��Ŀ�������!!");
        WRITEMSG
        goto ErrExit;
    }
    
    memset(g_pub_tx.crd_no,0x0,sizeof(g_pub_tx.crd_no));
    strcpy(g_pub_tx.ac_no,chnl_marchinfo.mrchacno);
    
    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */
    g_pub_tx.hst_ind[0] = '1';   /* �ռ�����ϸ */
    g_pub_tx.ac_id_type[0] = '9';

sprintf(acErrMsg,"ac_no=[%s],ac_id=[%ld],ac_seqn=[%ld]",
    g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
WRITEMSG

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos�˻�--�̻��ʻ�ȡ�����!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ����ԭ�����豸�����ռǵķ����� */
    ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld and dz_date=%ld",
           g_pub_tx.old_trace_no, oldDate);
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

    chnl_self_trade.tx_amt = chnl_self_trade.tx_amt - g_pub_tx.tx_amt1;
    strcpy(chnl_self_trade.stat,"1");
  get_zd_long("0520",&chnl_self_trade.strb);   /* */	

    ret = Chnl_self_trade_Upd_Upd(chnl_self_trade, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }
    Chnl_self_trade_Clo_Upd();


    /* �������豸�����ռǽṹ��ֵ */
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

    get_zd_data("0370",g_pub_tx.ac_no);
    strcpy(chnl_self_trade.ac_no,g_pub_tx.ac_no);

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

/*
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    get_zd_long("0770",&g_pub_tx.old_trace_no);
    chnl_self_trade.stra = g_pub_tx.old_trace_no;
*/
    get_zd_long("0520",&chnl_self_trade.stra);
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,"2");
    strcpy(chnl_self_trade.info_source,"0");
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"PSTH");
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


