/*************************************************
* �� �� ��:    sp9222.c
* ����������   
*              ���ݿͻ��Ų�ѯ��Ϣ
*
* ��    ��:    
* ������ڣ�   2012��3��21��
* �޸ļ�¼:
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS, YW_CNT_IND, YW_PERCENT) values ('9222', '�ͻ�������Ϣ��ѯ', '10000000000000000000000000001000000000000000000000000000000000000000000000000000111001000000000000000000000000000000000000000000', '0', '4', '2', '3', null);
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('9222', 0, '9222', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('9222', '�ͻ��ŵ���Ϣ', 'sp9222', '0', '0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

int sp9222()
{
	int			ret = 0;
	long    cif_no = 0;
	
	struct cif_email_inf_c cif_email_inf;
	struct cif_addr_inf_c cif_addr_inf;		

	/* ��� */
	memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	memset(&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));
	
	/* ��ʼ�� */
	pub_base_sysinit();
	
	/* ȡֵ */
	get_zd_long("0280", &cif_no);		/* �ͻ��� */
	
	if (cif_no) 
	{
		ret = Cif_addr_inf_Sel(g_pub_tx.reply, &cif_addr_inf, \
					"cif_no=%ld and addr_type='2' order by addr_seqn desc", cif_no);
		if (ret == 100)
		{
			sprintf(acErrMsg, "�����ڸÿͻ���!cif_no=[%ld]", cif_no);
			WRITEMSG

		}
		else if(ret)
		{
			sprintf(acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else 
	{
		sprintf(acErrMsg, "�ͻ���δ����!!cif_no=[%ld]", cif_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "C075");
		goto ErrExit;
	}
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf, \
					"cif_no=%ld and addr_type = '1' order by addr_seqn desc", cif_no);
	if (ret == 100)
	{
		sprintf(acErrMsg, "�����ڸÿͻ���!cif_no=[%ld]", cif_no);
		WRITEMSG
	}
	else if(ret)
	{
		sprintf(acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0810", cif_email_inf.email);	/* �ͻ���ϵ�绰 */
	memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf, \
					"cif_no=%ld and addr_type = '5' order by addr_seqn desc", cif_no);
	if (ret == 100)
	{
		sprintf(acErrMsg, "�����ڸÿͻ���!cif_no=[%ld]", cif_no);
		WRITEMSG
	}
	else if(ret)
	{
		sprintf(acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0830", cif_email_inf.email);	/* �ͻ���ϵ�绰 */
	set_zd_data("0820", cif_addr_inf.addr);	/* �ͻ���ϵ��ַ */
	set_zd_data("0860", cif_addr_inf.post_code);	/* �ʱ� */
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	
}
