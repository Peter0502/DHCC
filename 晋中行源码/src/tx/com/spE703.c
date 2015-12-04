/*************************************************
* �� �� ��:  spE703.c
* ���������� ��Ȩ��
*
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "auth_code_c.h"
#include "tx_log_c.h"

int spE703()
{
	int	ret;
	struct auth_code_c vauth_code;
	struct tx_log_c vtx_log;
	long vau_code;
	char tmpstr[21];

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	get_zd_data("0330",tmpstr);		
	vau_code=atol(tmpstr);

	sprintf(acErrMsg,"code[%d]",vau_code);

	ret=Auth_code_Sel( g_pub_tx.reply,&vauth_code,"auth_no=%d",vau_code);
	if( ret )
	{
		vtcp_log("����Ȩ�벻����");TRACE
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}
	if( vauth_code.sts[0]=='4' || vauth_code.sts[0]=='3' )
	{
		vtcp_log("����Ȩ���Ѿ���Ȩ");TRACE
		strcpy( g_pub_tx.reply,"Z10J" );
		goto ErrExit;
	}
	if( vauth_code.sts[0]=='9' )
	{
		vtcp_log("����Ȩ���Ѿ����");TRACE
		strcpy( g_pub_tx.reply,"Z104" );
		goto ErrExit;
	}
vtcp_log("tel[%s] code[%s]",vauth_code.tel,vauth_code.tx_code);
	set_zd_data("0920",vauth_code.tel);
	set_zd_data("0640",vauth_code.tx_code);
OkExit:
    strcpy( g_pub_tx.reply, "Z100" );
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
