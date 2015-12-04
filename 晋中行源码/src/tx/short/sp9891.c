/*************************************************
* �� �� ��:  sp9891.c
* ���������� �ͻ�������Ϣά����ݲ�ѯ
* ��    ��:  rob
* ������ڣ� 2003��2��3��
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_lifsty_inf_c.h"

int sp9891()
{
	int ret = 0;
	int lifsty_no;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_lifsty_inf_c g_cif_lifsty_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0680",sts);					/*��������*/
	get_zd_long("0280",&cif_no);				/*�ͻ���*/
	get_zd_int("0340",&lifsty_no);				/*�������*/

	/*����*/
	/*��ѯ�ͻ�������Ϣ��*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ���в����ڸÿͻ���!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C051");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*��ѯ�ͻ���ַ��Ϣ��*/
	ret=Cif_lifsty_inf_Sel(g_pub_tx.reply,&g_cif_lifsty_inf,
			"cif_no=%ld and lifsty_no=%d",cif_no,lifsty_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�����ڸü�¼!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C071");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ�������Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C070");
		goto ErrExit;
	}

	set_zd_data("0670",g_cif_lifsty_inf.life_type);		/*���ʽ����*/
	set_zd_data("0220",g_cif_lifsty_inf.life_detail);	/*������ϸ*/

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
