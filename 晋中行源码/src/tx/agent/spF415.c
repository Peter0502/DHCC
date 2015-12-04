/*************************************************
* �� �� ��: spF415.c
* ������������ؽɷ�ʱ�ļ��(ͨ��ͬ��)
*
* ��    ��: mike
* �������: 2004��06��30��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "mbfeghvrecvlist_c.h"
#include "mo_merge_ac_c.h"

spF415()  
{ 	
	int ret=0;
	char flag[2];

	struct mo_city_c sMo_city_c;
	struct mbfeghvrecvlist_c sMbfeghvrecvlist;
	struct mo_merge_ac_c sMo_merge_ac;

	memset(&sMo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(flag, 0x0, sizeof(flag));
	memset(&sMbfeghvrecvlist, 0x0, sizeof(struct mbfeghvrecvlist_c));
	memset(&sMo_merge_ac, 0x0, sizeof(struct mo_merge_ac_c));

	pub_base_sysinit();

	get_zd_data("0710", flag); /* �������� 1 �ֽ� 2 ת�� 3 ͬ�� 4 ��� */

	if (flag[0] == '3') /* ͬ�� */
	{
		sprintf(acErrMsg, "ͬ��");
		WRITEMSG
		get_zd_data("0300", sMo_city_c.payer_ac_no);
		get_zd_data("0310", sMo_city_c.payee_ac_no);
		get_zd_double("0420", &sMo_city_c.bill_amt);
		sprintf(acErrMsg, "payer [%s],payee [%s], [%lf]", 
			sMo_city_c.payer_ac_no, sMo_city_c.payee_ac_no, 
			sMo_city_c.bill_amt);
		WRITEMSG
		ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city_c, "payer_ac_no='%s' and \
			payee_ac_no='%s' and bill_amt=%lf and in_out_ind='2'", 
			sMo_city_c.payer_ac_no,
			sMo_city_c.payee_ac_no, sMo_city_c.bill_amt);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "û������������ͬ��ҵ��!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H028");
			goto ErrExit;
		}

		if (sMo_city_c.sts[0] != 'O' && sMo_city_c.sts[0] != 'P')
		{
			sprintf(acErrMsg, "�ñ�ҵ��û�м���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "H029");
			goto ErrExit;
		}
	}
	else if (flag[0] == '4') /* ���  */
	{
		sprintf(acErrMsg, "���");
		WRITEMSG
		get_zd_data("0300", sMbfeghvrecvlist.payeracc);
		get_zd_data("0310", sMbfeghvrecvlist.recipientacc);
		get_zd_double("0420", &sMbfeghvrecvlist.amount);
		sprintf(acErrMsg, " payer=[%s], payee=[%s], amt=[%lf]",
			sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
			sMbfeghvrecvlist.amount);
		WRITEMSG
		ret= Mbfeghvrecvlist_Sel(g_pub_tx.reply, &sMbfeghvrecvlist,
			"payeracc='%s' and recipientacc='%s' and amount=%lf",
			sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
			sMbfeghvrecvlist.amount);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "��ѯ������˵Ǽǲ��쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			ret = Mo_merge_ac_Sel(g_pub_tx.reply, &sMo_merge_ac, "ac_no='%s'", 
				sMbfeghvrecvlist.recipientacc);
			if ( ret )
			{
				sprintf(acErrMsg, "��ѯ�����˺Ŷ��ձ����!![%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack(sMo_merge_ac.old_ac_no);
			strcpy(sMbfeghvrecvlist.recipientacc, sMo_merge_ac.old_ac_no);
			ret= Mbfeghvrecvlist_Sel(g_pub_tx.reply, &sMbfeghvrecvlist,
				"payeracc='%s' and recipientacc='%s' and amount=%lf",
				sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
				sMbfeghvrecvlist.amount);
			if ( ret == 100)
			{
				sprintf(acErrMsg, "������˵Ǽǲ���û�иñ�ҵ��!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply, "H038");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "��ѯ������˵Ǽǲ�����!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
		}

		if (sMbfeghvrecvlist.procstate[0] != '0' && \
			sMbfeghvrecvlist.procstate[0] != '7')
		{
			sprintf(acErrMsg, "�ñ����˻�û�д���!! [%s]",
				sMbfeghvrecvlist.procstate);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H039");
			goto ErrExit;
		}
	}
	else 
	{
		sprintf(acErrMsg, "�������ʹ���!! [%s]", flag);
		WRITEMSG
		strcpy(g_pub_tx.reply, "H037");
		goto ErrExit;
	}

	strcpy(g_pub_tx.brf, "������˰����");
	if (pub_ins_trace_log()) 
	{ 
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��ؽɷѼ�鴦��ɹ�OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"��ؽɷѼ�鴦��ʧ��!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
