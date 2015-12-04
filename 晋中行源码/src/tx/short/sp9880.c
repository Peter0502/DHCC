/***************************************************************
* �� �� ��: sp9880.c
* ����������������¼��ѯ
*
* ��    ��: jack
* ������ڣ�2003��03��31��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"

sp9880()
{
	int ret = 0;
	int num = 0;
	char cUnit_no[5];
	char cFlag[2];

	char cFileName[128];
	FILE *fp;

	struct pay_agent_c sPay_agent; /*  ����ҵ���¼��  */

	memset(&sPay_agent,0x0,sizeof(sPay_agent));
	memset(cUnit_no,0x0,sizeof(cUnit_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0700", cFlag );

	if ( cFlag[0] == '1' )
	{
		goto OkExit;
	}

	memset( cFileName, 0x00, sizeof( cFileName ) );

	/* ���ݹ�Ա�š��ն˺�������ʱ�ļ� */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "�����´�ȫ·���ļ���(����)��" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "��ʱ�ļ�����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
					order by item_no", cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Pay_agent_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sPay_agent, 0x00, sizeof(sPay_agent));
		
		ret = Pay_agent_Fet_Sel( &sPay_agent, g_pub_tx.reply );
		if ( ret == 100)
		{
			if (num == 0)
			{
				sprintf( acErrMsg, "������¼������!!!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "H013");
				goto ErrExit;
			}
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Pay_agent_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%f|\n", sPay_agent.unit_no, sPay_agent.item_no, sPay_agent.tram);

		num++;
	}
	Pay_agent_Clo_Upd( );

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������Ϣ��ѯ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"������Ϣ��ѯʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
