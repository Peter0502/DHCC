/*************************************************
* �� �� ��:  sp9962.c
* ���������� �����Ա�ţ����Թ�Ա����----��Ա������
*
* ��    ��:  rob
* ������ڣ� 2003��07��15��
*
* �޸ļ�¼�� 
*  ��   ��: 20070429
* �� �� ��: wanglei
* �޸�����: ������״̬Ϊ����ʱ��У�鸶��������
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9962()
{
	int ret=0;
	char tel[5];
	char cTxtype[2]; /****�������� 1-���� 2-���� 3-�ջ�****/
	char cPasswd[7]; /****��Ա����****/

	struct com_tel_c g_com_tel;

	memset( &g_com_tel, 0x00, sizeof( struct com_tel_c ) );
	memset( cTxtype, 0x00, sizeof(cTxtype) );
	memset( cPasswd, 0x00, sizeof(cPasswd) );

	get_zd_data( "0920", tel );					/* ��Ա�� */
	get_zd_data( "0700", cTxtype );					/* �������� */

	ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",tel);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڸù�Ա!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(cTxtype[0] == '2')/****����ʱ����鸶����Ա������****/
	{
		get_zd_data( "0790", cPasswd );		/* ��Ա���� */
		pub_base_EnDes(g_pub_tx.tx_date,"",cPasswd);
		if(memcmp(g_com_tel.pwd,cPasswd,6) != 0)
		{
			sprintf(acErrMsg,"�Է���Ա�������!!tel=[%s]",tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O123");
			goto ErrExit;
		}
	}/****���ݽ���Ҫ�󣬸���ʱ���Է���Ա������ǩ��״̬  add by wanglei 20070501****/
	else if(cTxtype[0] == '1' && g_com_tel.csts[0] != '2')
	{
		sprintf(acErrMsg,"�Է���Ա����ǩ��״̬!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P089");
		goto ErrExit;
	}

	set_zd_data( "0250", g_com_tel.name );		/* ��Ա���� */

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
