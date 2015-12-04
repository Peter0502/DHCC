/*************************************************
* �� �� ��:  spF034.c
* ���������� POS ���ѳ���
*
* ��    ��:  rob
* ������ڣ� 2003��2��20��
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

int spF034()
{

    int ret;
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
    get_zd_long("0520",&g_pub_tx.trace_no); /* �ն���ˮ�� */
    get_zd_long("0780",&g_pub_tx.old_trace_no); /* ԭ�ն���ˮ�� */


    if (strncmp(g_pub_tx.crd_no,CRD_HEAD,9) != 0)
    {
        sprintf(acErrMsg,"�ÿ����Ǳ��еĿ�!!!");
        WRITEMSG
        goto NOUSE;
    }

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

NOUSE:
    /* ��ѯ�����豸�����ռ�(ԭ����) */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra='%ld'",
          g_pub_tx.old_trace_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����豸�����ռǱ����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ���ñ���ˮ�Ƿ����� ������״̬(�ѱ�ȡ�� �����������˻�) */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"�ñ���ˮ������������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        goto ErrExit;
    }

    /* ���ó�������ǰ�Ĵ��� */

    /* ���ú�̨���쳷������ */   
    
    /* ����ԭ�����豸�����ռǵ�״̬Ϊ������״̬ */

    /* �������豸�����ռǽṹ��ֵ */
    
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

