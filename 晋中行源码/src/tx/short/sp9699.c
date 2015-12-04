/*************************************************
* �� �� ��:  sp9699.c
* ���������� ��Ȩ�˷��� for SQ01
*
* ��    ��:  rob
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_code_c.h"

int sp9699()
{
	struct auth_code_c vauth_code;
	char vtxcode[5];
	int ret=0;

	get_zd_long( DC_AUTH_CODE , &g_pub_tx.auth_code );
	get_zd_data( DC_TEL , g_pub_tx.tel );
	get_zd_data( DC_AUTH , g_pub_tx.auth );
	get_zd_data( "0920" , vtxcode );

	vtcp_log("AUTH_CODE[%d]",g_pub_tx.auth_code );
	vtcp_log("TEL[%s]",g_pub_tx.tel );
	vtcp_log("AUTH[%s]",g_pub_tx.auth );
	vtcp_log("TXCODE[%s]",vtxcode );

	ret = Auth_code_Sel( g_pub_tx.reply,&vauth_code,
		" auth_no=%d ",g_pub_tx.auth_code );
	if( ret )
	{
		sprintf( acErrMsg,"��Ȩ�벻����[%d]",g_pub_tx.auth_code );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}

	if( vauth_code.sts[0]=='9' )
	{
		sprintf( acErrMsg,"����ˮ�Ŷ�Ӧ�Ľ����Ѿ����" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z104" );
		goto ErrExit;
	}
	else if( vauth_code.sts[0]=='4' )
	{
		sprintf( acErrMsg,"�˽�����Ҫ���ˣ��밴F11���и���" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10G" );
		goto ErrExit;
	}
	else if( vauth_code.sts[0]=='3' )
	{
		if( strcmp(g_pub_tx.tel,vauth_code.tel) )
		{
			sprintf( acErrMsg,"�˽����Ѿ���Ȩ��[%s] [%s]",
				g_pub_tx.tel,vauth_code.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"Z10J" );
			goto ErrExit;
		}
	}
	if( strcmp(vtxcode,vauth_code.tx_code) )
	{
		sprintf( acErrMsg,"��ˮ�Ų����������[%s] [%s]",
			vtxcode,vauth_code.tx_code);
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10C" );
		goto ErrExit;
	}
		sprintf( acErrMsg,"��Ȩ����[%s]",vauth_code.auth );
		WRITEMSG
	set_zd_data( DC_AUTH,vauth_code.auth );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
