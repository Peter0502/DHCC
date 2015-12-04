/*************************************************
* �� �� ��:  spF027.c
* ���������� POS ���ѳ���
*            (���̻��ʻ���ת�������ʻ����ӽ���)(���п�)
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

int spF027()
{

    int ret;
    char marchno[10];
    char tty[9];
    char note_no[21];
    char in_type[5];
    long oldTx_date;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(tty,0x00,sizeof(tty));
    memset(note_no,0x00,sizeof(note_no));
    memset(in_type,0x00,sizeof(in_type));
    memset(marchno,0x00,sizeof(marchno));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    set_zd_double("1013",g_pub_tx.tx_amt1);
    sprintf(acErrMsg,"������[%lf]",g_pub_tx.tx_amt1);
    WRITEMSG
/*
	memset(cTrace_no, 0, sizeof(cTrace_no));
    get_zd_data("0610", cTrace_no);  ԭ�ն���ˮ�� 
    vtcp_log("ԭ�ն���ˮ�� =====[%s] ", cTrace_no);
	
	g_pub_tx.old_trace_no = atol(cTrace_no);
    vtcp_log("ԭ�ն���ˮ�� =====[%d] ", g_pub_tx.old_trace_no);
*/

	/*New:  Changed by ChenMing 2007.07.27. */	
	get_zd_long("0440", &g_pub_tx.old_trace_no);  /* ���彻�׵�ԭƽ̨��ˮ */
	get_zd_long("0450", &oldTx_date);  /* ���彻�׵�ԭ�������� */
	
	vtcp_log("���彻�׵�ԭƽ̨��ˮ�� =====[%d] ", g_pub_tx.old_trace_no);
	vtcp_log(" ���彻�׵�ԭ�������� =====[%d] ", oldTx_date);

    	/* ��鿨�ĺϷ��� */
    	if (pub_base_CheckCrdNo())
    	{
       	 	sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        	WRITEMSG
        	goto ErrExit;
    	}

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{ 
		/* �����ƶ�֧�� */
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
	}

    /* ��ѯ�����豸�����ռ� */
/*    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"trace_no=%ld",
          g_pub_tx.old_trace_no);
*/
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld ", g_pub_tx.old_trace_no, oldTx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����豸�����ռǱ����!! [%d]",ret);
        WRITEMSG
        sprintf(acErrMsg,"trace_no=[%ld]",g_pub_tx.old_trace_no);
        WRITEMSG
        goto ErrExit;
    }

    /* ���ñ���ˮ�Ƿ����� */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"�ñ���ˮ������������!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* �������Ŀ��ź�ԭ������ˮ�е��Ƿ����� */
    pub_base_strpack(chnl_self_trade.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if (strcmp(chnl_self_trade.ac_no,g_pub_tx.crd_no) != 0)
    {
        sprintf(acErrMsg,"����Ŀ��ź�ԭ������ˮ�еĿ��Ų�һ��!!");
        WRITEMSG
        sprintf(acErrMsg,"chnl_self_trade: [%s], g_pub_tx: [%s]",
               chnl_self_trade.ac_no,g_pub_tx.crd_no);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

    /* ���ñ���ˮ�Ƿ���������� 
       ���ܶ��ѳ����Ľ��׽���ȡ�������ܶ��ѳ����Ľ����ٴγ�����*/
    pub_base_strpack(chnl_self_trade.brf_no);
    if (strcmp(chnl_self_trade.brf_no,"PSXF") != 0)
    {
       sprintf(acErrMsg,"�ñ���ˮ�������������� [%s]",chnl_self_trade.brf_no);
       WRITEMSG
	   strcpy(g_pub_tx.reply, "AT13");
       goto ErrExit;
    }
   
    /* ���ñ���ˮ�Ƿ��ǵ��ս��� */
    if (chnl_self_trade.tx_date != g_pub_tx.tx_date)
    {
        sprintf(acErrMsg,"���ղ�������������!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* ���ñ���ˮ�Ľ�� Added by ChenMing 2007.07.28. */
    if (chnl_self_trade.tx_amt < g_pub_tx.tx_amt1)
    {
        sprintf(acErrMsg,"�������ܳ���ԭ���׽��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* ����̻��Ƿ���ͬ *
    pub_base_strpack(chnl_self_trade.spcd);
    pub_base_strpack(marchno);
    if (strcmp(marchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"�̻��Ų�ͬ,��������������!!");
        WRITEMSG
        sprintf(acErrMsg," [%s] ?= [%s] ",marchno,chnl_self_trade.spcd);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }   
    
    * ����Ƿ���ͬһ̨POS *
    pub_base_strpack(g_pub_tx.tty);
    pub_base_strpack(chnl_self_trade.ttyn);
    if (strcmp(chnl_self_trade.ttyn,g_pub_tx.tty) != 0)
    {
        sprintf(acErrMsg,"POS�豸�Ų�ͬ,��������������!!");
        WRITEMSG
        goto ErrExit;
    }
	*/

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    /* ���ô��� 
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
    strcpy(g_pub_tx.ct_ind,"2"); * ת�� *
    g_pub_tx.hst_ind[0] = '1';  * �ռ�����ϸ *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos���ѳ���--�ʻ�������!!");
        WRITEMSG
        goto ErrExit;
    }*/

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

