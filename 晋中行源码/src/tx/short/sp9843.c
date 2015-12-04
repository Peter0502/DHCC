/*************************************************
* �� �� ��:  sp9843.c
* ���������� ��λ��ʱ�˺Ų�ѯ(��Ӧ����)
*
* ��    ��:  jack
* ������ڣ� 2003��1��31�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_unitacctmap_c.h"

int sp9843()
{
	struct ag_unitacctmap_c sUnitAcctMap;	/*��λ��ʱ�˺Ź����*/
	int ret = 0;

	char cUnit_no[5];
	char cBr_no[6];
	char proc_flag[2];

	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cBr_no,0,sizeof(cBr_no));
	memset(proc_flag,0x0,sizeof(proc_flag));

	pub_base_sysinit();
	get_zd_data("0360",cUnit_no);
	get_zd_data("0910",cBr_no);
	get_zd_data("0700",proc_flag);
		sprintf( acErrMsg, "�����־Ϊ [%s]",proc_flag);
		WRITEMSG

	ret=Ag_unitacctmap_Sel( g_pub_tx.reply , &sUnitAcctMap ,\
			 "br_no = '%s' and unit_no = '%s' and stat ='0'",cBr_no,cUnit_no);
	if( ret==100 )
	{
		if (proc_flag[0] == '0')
		{
			goto	OkExit;
		}
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��ʱ�˻���¼!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "H010" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitacctmap����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	if (proc_flag[0] == '0')
	{
		sprintf( acErrMsg, "�����Ѿ�����,����������");
		WRITEMSG
		strcpy( g_pub_tx.reply, "H001" );
		goto	ErrExit;
	}

	set_zd_data( "0670",sUnitAcctMap.acct_type);
	set_zd_data( "0300",sUnitAcctMap.bank_acct_no);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
