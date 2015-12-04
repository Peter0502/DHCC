/*************************************************
* �� �� ��:  spF048.c
* ���������� POS ���п�Ԥ��Ȩ��ɵĴ���(�������ʻ���ת���ӽ���)(���п�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��26��
*
* �޸ļ�¼�� ���п������̻�Ҫ������ӽ�����
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"

int spF048()
{

    int ret;
    char flag;
    struct chnl_self_trade_c chnl_self_trade;
    struct chnl_marchinfo_c chnl_marchinfo;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]���뵽���п�Ԥ��Ȩ����ӽ���\n",__FILE__,__LINE__);
    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* �̻��� */
    
    get_zd_long("0520",&g_pub_tx.old_trace_no); /* �ն���ˮ�� */
    get_zd_long("0470",&g_pub_tx.auth_code);       /* Ԥ��Ȩ�� */
    get_zd_char("0670",&flag); /* ��Ϣ��Դ */

    if (flag != '1')
    {
        /* �����̻� ���̻��ļ��*/
        ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
        if (ret != 0)
        {
            sprintf(acErrMsg,"����̻���Ϣ����!!");
            WRITEMSG
			strcpy(g_pub_tx.reply, "AT08");
            goto ErrExit;
        }
    }
    
    /* ����Ԥ��Ȩ�Ų�ѯ��Ԥ��Ȩ�Ľ����ռ� */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,
          "premise_no=%ld and brf_no='PSSQ'", g_pub_tx.auth_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯԤ��Ȩ�����ռǴ���!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
  
    sprintf(acErrMsg,"Ԥ��Ȩ��״̬stat===[%s]",chnl_self_trade.stat);
        WRITEMSG
    /* �ж�Ԥ��Ȩ��״̬�Ƿ����� */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"Ԥ��Ȩ��״̬������!!!stat===[%s]",chnl_self_trade.stat);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
	vtcp_log("chnl_self_trade.premise_amt=====[%lf]",chnl_self_trade.premise_amt);
    vtcp_log("g_pub_tx.tx_amt1=====[%lf]",g_pub_tx.tx_amt1);

    /* �жϽ���Ƿ񳬳�Ԥ��Ȩ��� */
    if (pub_base_CompDblVal(chnl_self_trade.premise_amt,g_pub_tx.tx_amt1) < 0)
    {
        sprintf(acErrMsg,"������Ԥ��Ȩ���!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* ��鿨���Ƿ�һ�� */
    pub_base_strpack(g_pub_tx.crd_no);
    pub_base_strpack(chnl_self_trade.ac_no);
    if (strcmp(g_pub_tx.crd_no,chnl_self_trade.ac_no) != 0)
    {
        sprintf(acErrMsg,"���Ų�һ�£�����");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* ����̻��Ƿ�һ�� */
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"�̻��Ų�һ��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
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

