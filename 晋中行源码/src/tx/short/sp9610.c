/*************************************************
* �� �� ��:  sp9610.c
* ���������� ATM������ѯ
*
* ��    ��:  robbit
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:   20080416
* �� �� ��:  
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"

int sp9610()
{
	int ret=0;
	struct com_tel_c vcom_tel,pcom_tel;
	struct cash_mst_c vcashmst;
	char tel128[10];
	
	memset(&vcom_tel,0x00,sizeof(struct com_tel_c));
	memset(&pcom_tel,0x00,sizeof(struct com_tel_c));
	memset(&vcashmst,0x00,sizeof(struct cash_mst_c));
	memset(tel128,0,sizeof(tel128));
	
	get_zd_data("1280",tel128);
	memcpy(pcom_tel.tel,tel128,sizeof(pcom_tel.tel)-1);
	
	
	ret = Com_tel_Sel( g_pub_tx.reply,&vcom_tel," tel='%s' ",pcom_tel.tel );
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",pcom_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='4' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]O[%s]",pcom_tel.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O062" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='3' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]i[%s]",pcom_tel.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O063" );
		goto ErrExit;
	}

	ret = Cash_mst_Sel( g_pub_tx.reply,&vcashmst,"tel='%s'",vcom_tel.tel);
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",vcom_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	set_zd_double("0400",vcashmst.bal);
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
