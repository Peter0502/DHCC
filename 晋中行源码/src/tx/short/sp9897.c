/*************************************************
* �� �� ��:  sp9897.c
* ���������� ���˿ͻ���Ϣά����ݲ�ѯ
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
#include "cif_per_inf_c.h"

int sp9897()
{
	int ret = 0;
	long cif_no;
	char sts[2];
	char cif_typ[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_per_inf_c g_cif_per_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_per_inf,0x00,sizeof(struct cif_per_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0670",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/

	/*����*/
	/* �ͻ��ż�� */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* ����Ƿ���˿ͻ� */
	if ( cif_typ[0]!='1' && cif_typ[0]!='5' )
	{
		sprintf(acErrMsg,"�Ƕ�˽�ͻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C011");
		goto ErrExit;
	}	

	/*��ѯ���˿ͻ���Ϣ��*/
	ret=Cif_per_inf_Sel(g_pub_tx.reply,&g_cif_per_inf,"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˿ͻ���δ�ǼǸ��˿ͻ���Ϣ!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C110");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ���˿ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0270",g_cif_per_inf.name);            	/*�ͻ�����*/
		sprintf( acErrMsg, "AAAAAAAAAAAAA=[%s]", g_cif_per_inf.sex );
		WRITEMSG
	set_zd_data("0670",g_cif_per_inf.sex);             	/*�ͻ��Ա�*/
	set_zd_long("0450",g_cif_per_inf.n_bir);			/*ũ������*/	
	set_zd_long("0440",g_cif_per_inf.g_bir);			/*��������*/	
	set_zd_data("0810",g_cif_per_inf.birth_adr);       	/*������*/
	set_zd_data("0230",g_cif_per_inf.nationality);     	/*��������*/
	set_zd_data("0210",g_cif_per_inf.folk);     		/*�������*/
	set_zd_data("0220",g_cif_per_inf.language);   		/*���Դ���*/
	set_zd_data("0710",g_cif_per_inf.edu);   			/*���ѧ��*/
	set_zd_data("0890",g_cif_per_inf.headship);        	/*ְ��*/
	set_zd_data("0900",g_cif_per_inf.technical_post);  	/*ְ��*/
	set_zd_data("0650",g_cif_per_inf.clerk_no);  		/*ְԱ���*/
	set_zd_double("0400",g_cif_per_inf.mon_income);  	/*������*/
	set_zd_double("0410",g_cif_per_inf.h_mon_income);	/*��ͥ������*/
	set_zd_long("0500",g_cif_per_inf.home_cnt);  		/*������ͥ��Ա��Ŀ*/
	set_zd_long("0290",g_cif_per_inf.mar_date);			/*�������*/	
	set_zd_data("0680",g_cif_per_inf.marriage);        	/*����״��*/
	set_zd_data("0690",g_cif_per_inf.health);        	/*����״��*/
	set_zd_data("0700",g_cif_per_inf.religion);      	/*�ڽ�����*/
	set_zd_data("0240",g_cif_per_inf.country);      	/*��ס����*/

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
