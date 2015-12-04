/*************************************************
* �� �� ��:  spF043.c
* ���������� POS Ԥ��Ȩ��ɳ���(�������ʻ���ת���ӽ���)(���п�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��22��
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

int spF043()
{

    int ret;
    char flag;
    long old_date;
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
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* �̻��� */
   
    get_zd_long("0780",&g_pub_tx.old_trace_no);  /*ԭ�ն���ˮ�� */
    get_zd_long("0450",&old_date);   
    get_zd_long("0440",&chnl_self_trade.stra);  /*  ԭƽ̨��ˮ��*/
    get_zd_long("0440",&chnl_self_trade.dz_date);  /*  ƽ̨����*/
    get_zd_long("0870",&g_pub_tx.auth_code);
    get_zd_char("0670",&flag); /* ��Ϣ��Դ */
    if (flag != '1')
    {
        /* �����̻� ���̻��ļ��
        ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
        if (ret != 0)
        {
            sprintf(acErrMsg,"����̻���Ϣ����!!");
            WRITEMSG
			strcpy(g_pub_tx.reply, "AT08");
            goto ErrExit;
        }*/
    }

    /* ��ѯԤ��Ȩ��ɵĽ�����ˮ�ռ� */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld",
          g_pub_tx.old_trace_no, old_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����豸�����ռǴ���!!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"�ñ���ˮΪ������״̬,���ܳ���!!");
        WRITEMSG
        goto ErrExit;
    }
	vtcp_log("chnl_self_trade.tx_amt====[%lf]",chnl_self_trade.tx_amt);
	vtcp_log("g_pub_tx.tx_amt1====[%lf]",g_pub_tx.tx_amt1);

    /* ��齻�׽�� */
    if (pub_base_CompDblVal(chnl_self_trade.tx_amt,g_pub_tx.tx_amt1) != 0)
    {
        sprintf(acErrMsg,"�������׽���ԭ���׽�һ��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

    /* ��鿨���Ƿ��ԭ����һ�� */
    pub_base_strpack(chnl_self_trade.ac_no1);
    pub_base_strpack(g_pub_tx.crd_no);
    if (strcmp(chnl_self_trade.ac_no1,g_pub_tx.crd_no) != 0)
    {
        sprintf(acErrMsg,"���ź�ԭ���Ĳ�һ��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* ����̻��Ƿ�һ��
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"�̻��Ų�һ��!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    } */

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

