/*************************************************
* �� �� ��:  sp9899.c
* ���������� ��ѯ�ͻ�ID״̬
*
* ��    ��:  rob
* ������ڣ� 2003��2��3��
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"


int sp9899()
{
	int ret = 0;
	long cif_no;

	struct	cif_basic_inf_c	g_cif_basic_inf;
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_long("0280",&cif_no);

	/*����*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸÿͻ���!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0700",g_cif_basic_inf.sts);

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
