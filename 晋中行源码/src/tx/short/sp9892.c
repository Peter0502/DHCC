/*************************************************
* �� �� ��:  sp9892.c
* ���������� �ͻ�������ʷ��Ϣά����ݲ�ѯ
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
#include "cif_wrk_hst_inf_c.h"

int sp9892()
{
	int ret = 0;
	int wrk_no;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_wrk_hst_inf_c g_cif_wrk_hst_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0700",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/
	get_zd_int("0490",&wrk_no);				/*�������*/

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
	ret=Cif_wrk_hst_inf_Sel(g_pub_tx.reply,&g_cif_wrk_hst_inf,
						"cif_no=%ld and wrk_no=%d",cif_no,wrk_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������ʷ��Ϣ�����ڸü�¼!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C068");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ�������ʷ��Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C069");
		goto ErrExit;
	}

	pub_base_strpack(g_cif_wrk_hst_inf.industry_no);	

	set_zd_data("0670",g_cif_wrk_hst_inf.wrk_ind);		/*����״̬*/
	set_zd_data("0220",g_cif_wrk_hst_inf.industry_no);	/*��ҵ����*/
	set_zd_data("0680",g_cif_wrk_hst_inf.vocation_no);	/*ְҵ����*/
	set_zd_data("0690",g_cif_wrk_hst_inf.position_no);	/*ְλ����*/

	set_zd_data("0260",g_cif_wrk_hst_inf.unit_name);	/*��λ����*/
	set_zd_long("0440",g_cif_wrk_hst_inf.beg_date);		/*��ְ����*/
	set_zd_long("0450",g_cif_wrk_hst_inf.end_date);		/*��ְ����*/
	set_zd_double("0410",g_cif_wrk_hst_inf.income);		/*��н����*/

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
