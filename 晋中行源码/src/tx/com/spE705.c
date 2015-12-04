/*************************************************
* �� �� ��:  spE705.c
* ���������� ��Ա��λ����
*
* ��    ��:  andy
* ������ڣ� 2004��3��25��
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
#include "com_tel_c.h"
#include "tx_log_c.h"

int spE705()
{
	int	ret;
	struct com_tel_c g_com_tel;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	get_zd_data("0790",g_pub_tx.draw_pwd);		
	get_zd_data("0920",g_pub_tx.tel);		

	ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ա���!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);

	if( strcmp(g_pub_tx.draw_pwd,g_com_tel.pwd) )
	{
		sprintf( acErrMsg,"��Ա�����ȷ!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"O123" );
		goto ErrExit;
	}

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
