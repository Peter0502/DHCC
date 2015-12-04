/***************************************************************
* �� �� ��:     pubf_de_check_mst.c
* ����������    ������ļ��˻�״̬
*
* ��    ��:	jack
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

/***************************************************************
* �� �� ��:     pubf_de_chk_dd_mst.c
* ����������    ���������ļ��˻�״̬
*			flag[0]:��һλ�Ƿ���ȡ���ļ� 0-δȡ 1-��ȡ��
*			flag[1]:�ڶ�λ�Ƿ����ʻ���Ч���� 0-�� 1-�ǣ�
*			flag[2]:����λ�Ƿ����ʻ�״̬ 0-�� 1-�ǣ�
*			flag[3]:����λ�Ƿ����ʻ����� 0-�� 1-�ǣ�
*
* ��    ��:
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
int pub_de_chk_dd_mst(char flag[10])
{
	int ret=0;

	if( flag[0]=='0' )
	{
		ret=pub_base_currentdis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_dd_mst);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ���ڽ��㻧���ļ�����![%d][%s][%d]",ret,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
			WRITEMSG
			return 1;
		}
	}

	if( flag[1]=='1' && g_dd_mst.val_date!=0 
		&& g_dd_mst.val_date<g_pub_tx.tx_date )
	{
		sprintf(acErrMsg,"���˻��Ѿ�����Ч�ڣ�������![%d][%d]",g_dd_mst.val_date,g_pub_tx.tx_date);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P285");
		return 1;
	}

	if( flag[2]=='1' && g_dd_mst.ac_sts[0]!='1' )
	{
		sprintf("���˻��˻�״̬������![%s]",g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
		return 1;
	}

	if( flag[3]=='1' && g_dd_mst.hold_sts[0]!='0' )
	{
		sprintf("���˻��˻��Ѿ�������![%s]",g_dd_mst.hold_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}

/***************************************************************
* �� �� ��:     pubf_de_chk_td_mst.c
* ����������    ��鶨�����ļ��˻�״̬
*				flag[0]:��һλ�Ƿ���ȡ���ļ� 0-δȡ 1-��ȡ��
*				flag[1]:�ڶ�λ�Ƿ����ʻ�֧ȡ�����Ƿ񳬹��ܴ��� 0-�� 1-�ǣ�
*				flag[2]:����λ�Ƿ����ʻ�״̬ 0-�� 1-�ǣ�
*				flag[3]:����λ�Ƿ����ʻ����� 0-�� 1-�ǣ�
*
* ��    ��:
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
int pub_de_chk_td_mst(char flag[10])
{
	int ret=0;

	if( flag[0]=='0' )
	{
		ret=pub_base_termidis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_td_mst);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ���ڽ��㻧���ļ�����![%d][%s][%d]",ret,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
			WRITEMSG
			return 1;
		}
	}

	if( flag[1]=='1' && g_td_mst.ttl_cnt!=0 && g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
	{
		sprintf(acErrMsg,"���˻��Ѿ�����֧ȡ������������![%d][%d]",g_td_mst.ttl_cnt,g_td_mst.tx_cnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P286");
		return 1;
	}

	if( flag[2]=='1' && g_td_mst.ac_sts[0]!='1' )
	{
		sprintf("���˻��˻�״̬������![%s]",g_td_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
		return 1;
	}

	if( flag[3]=='1' && g_td_mst.hold_sts[0]!='0' )
	{
		sprintf("���˻��˻��Ѿ�������![%s]",g_td_mst.hold_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}
