/*************************************************
* �� �� ��:  sp9893.c
* ���������� �ͻ���ַ��Ϣά����ݲ�ѯ
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
#include "cif_addr_inf_c.h"

int sp9893()
{
	int ret = 0;
	int addr_seqn;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_addr_inf_c g_cif_addr_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0690",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/
	get_zd_int("0490",&addr_seqn);			/*��ַ���*/

	/*����*/
	/*��ѯ�ͻ�������Ϣ��*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ���в����ڸÿͻ���!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C097");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*��ѯ�ͻ���ַ��Ϣ��*/
	ret=Cif_addr_inf_Sel(g_pub_tx.reply,&g_cif_addr_inf,
						"cif_no=%ld and addr_seqn=%d",cif_no,addr_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ���ַ��Ϣ�����ڸü�¼!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C066");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���ַ��Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C067");
		goto ErrExit;
	}

	if( sts[0]=='2' && addr_seqn==1 )	/* ɾ����һ����¼ */
	{
		TRACE
		sprintf( acErrMsg, "������¼����ɾ��!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C104");		
		goto ErrExit;
	}	

	set_zd_data( "0670",g_cif_addr_inf.addr_type);     	 /*��ַ����*/
	set_zd_data( "0230",g_cif_addr_inf.country);     	 /*���ڹ���*/
	set_zd_data( "0260",g_cif_addr_inf.addr);            /*�ͻ���ַ*/
	set_zd_data( "0640",g_cif_addr_inf.post_code);       /*��������*/

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
