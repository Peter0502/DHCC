/*************************************************
* �� �� ��:  sp9890.c
* ���������� �ͻ��������ڲ�Ʒ��Ϣά����ݲ�ѯ
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
#include "cif_oth_prdt_inf_c.h"

int sp9890()
{
	int ret = 0;
	int inf_no;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_oth_prdt_inf_c g_cif_oth_prdt_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0680",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/
	get_zd_int("0490",&inf_no);				/*��Ϣ���*/

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

	/*��ѯ�ͻ��������ڲ�Ʒ��Ϣ��*/
	ret=Cif_oth_prdt_inf_Sel(g_pub_tx.reply,&g_cif_oth_prdt_inf,
						"cif_no=%ld and inf_no=%d",cif_no,inf_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ��������ڲ�Ʒ��Ϣ�����ڸü�¼!!cif_no=[%ld]",
				cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C072");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ��������ڲ�Ʒ��Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C073");
		goto ErrExit;
	}

	set_zd_data("0670",g_cif_oth_prdt_inf.unit_type);	/*��������*/
	set_zd_data("0910",g_cif_oth_prdt_inf.unit_no);		/*��������*/
	set_zd_data("0660",g_cif_oth_prdt_inf.prdt_code);	/*��Ʒ����*/
	set_zd_data("0210",g_cif_oth_prdt_inf.pro_cur_no);	/*��Ʒ����*/
	set_zd_data("0300",g_cif_oth_prdt_inf.ac_no);		/*��Ʒ����*/
	set_zd_double("0390",g_cif_oth_prdt_inf.balance);		/*������*/
	set_zd_double("0400",g_cif_oth_prdt_inf.mon_pay);		/*�»����*/

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
