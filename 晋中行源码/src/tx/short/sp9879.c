/*************************************************
* �� �� ��: sp9879.c
* �������������ݲ�Ʒ�Ų�ѯ��Ʒ
*
* ��    ��: jack
* ������ڣ�2003��03��30��
*
* �޸ļ�¼�� 
* ��    ��: 1��20030923
* �� �� ��: 1��jack
* �޸�����: 1���ɿ����ڲ�Ʒ��Ϊ���ڶ��ڶ����Կ�
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9879()
{
	struct dd_parm_c sDd_parm;	/*���ڲ�Ʒ������*/
	struct td_parm_c sTd_parm;	/*���ڲ�Ʒ������*/
	int ret = 0;
	int ret1 = 0;
	char cPrdt_no[4];

	memset(cPrdt_no,0,sizeof(cPrdt_no));

	pub_base_sysinit();
	get_zd_data( "0230", cPrdt_no );

	memset( &sDd_parm, 0x0, sizeof(sDd_parm) );		/*���ڲ�Ʒ*/
	ret = Dd_parm_Sel( g_pub_tx.reply , &sDd_parm ,\
			 "prdt_no = '%s'",cPrdt_no);
	if( ret==100 )
	{
		memset( &sTd_parm, 0x0, sizeof(sTd_parm) );	/*���ڲ�Ʒ*/
		ret1 = Td_parm_Sel( g_pub_tx.reply , &sTd_parm ,\
				 "prdt_no = '%s'",cPrdt_no);
		if( ret1 == 100 )
		{
			sprintf( acErrMsg, "û�в�ѯ���ò�Ʒ��Ŷ�Ӧ��¼!\
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H014");
			goto	ErrExit;
		}
		else if( ret1 )
		{
			sprintf( acErrMsg, "ִ��Dd_parm����!ret=[%d]", ret );
			WRITEMSG
			goto	ErrExit;
		}
        	
		if ( sTd_parm.cif_type[0] != '1' )
		{
			sprintf( acErrMsg, "�Ǹ��˿ͻ���Ʒ�����ڴ��������� \
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H056");
			goto	ErrExit;
		}
        	
		set_zd_data( "0270",sTd_parm.title);
	}
	else if ( ret == 0 )
	{
		if ( sDd_parm.cif_type[0] != '1' )
		{
			sprintf( acErrMsg, "�Ǹ��˿ͻ���Ʒ�����ڴ��������� \
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H056");
			goto	ErrExit;
		}

		set_zd_data( "0270",sDd_parm.title);
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_parm����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	

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
