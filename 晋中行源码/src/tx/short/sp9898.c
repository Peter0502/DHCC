/*************************************************
* �� �� ��:  sp9898.c
* ���������� �ͻ�������Ϣά��
*
* ��    ��:  rob
* ������ڣ� 2003��2��3��
*
* �޸ļ�¼�� 
* ��   ��:   2003-05-31	
* �� �� ��:  lance
* �޸�����:  �����ϵ��ʽ����ϵ��ַ
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

int sp9898()
{
	int ret = 0;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_email_inf_c cif_email_inf;	
	struct cif_addr_inf_c cif_addr_inf;	
		
	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));
		
	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0670",sts);			/*��������*/
	get_zd_long("0280",&cif_no);		/*�ͻ���*/

	/*����*/
	/*��ѯ�ͻ�������Ϣ��*/
	if( cif_no )
	{
		ret = Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf , \
								 "cif_no=%ld",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ڸÿͻ���!cif_no=[%ld]",cif_no);
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
	}
	else
	{
		sprintf(acErrMsg,"�ͻ���δ����!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C075");
		goto ErrExit;
	}
	
	/*��ѯ�ͻ���ϵ��Ϣ��*/
	ret = sql_count( "cif_email_inf", "cif_no=%ld and addr_seqn=1", cif_no );
	if( ret<0 )
	{
		sprintf(acErrMsg,"TRANSFER SQL_COUNT ERROR!");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret>0 )
	{
		ret = Cif_email_inf_Sel( g_pub_tx.reply , &cif_email_inf ,
					 			"cif_no=%ld and addr_seqn=1",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ÿͻ���δ¼����ϵ�绰!",cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C106");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "��ѯ�ͻ���ϵ��ʽ��Ϣ���쳣!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else strcpy(cif_email_inf.email,"�޼�¼");

	/*��ѯ�ͻ���ַ��Ϣ��*/
	ret = sql_count( "cif_addr_inf", "cif_no=%ld and addr_seqn=1", cif_no );
	if( ret<0 )
	{
		sprintf(acErrMsg,"TRANSFER SQL_COUNT ERROR!");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret>0 )
	{
		ret = Cif_addr_inf_Sel( g_pub_tx.reply , &cif_addr_inf ,
					 			"cif_no=%ld and addr_seqn=1",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ÿͻ���δ¼����ϵ��ַ!",cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C107");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "��ѯ�ͻ���ַ��ʽ��Ϣ���쳣!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else strcpy( cif_addr_inf.addr , "����" );

	set_zd_data( "0670",g_cif_basic_inf.type);            	/*�ͻ�����*/
	set_zd_data( "0260",g_cif_basic_inf.name);            	/*�ͻ�����*/
	set_zd_data( "0680",g_cif_basic_inf.local);      	  	/*���ر�־*/
	set_zd_data( "0690",g_cif_basic_inf.poten);         	/*Ǳ�ڱ�־*/
	set_zd_data( "0700",g_cif_basic_inf.shareholder_ind);	/*���йɶ�*/
	set_zd_data( "0710",g_cif_basic_inf.lvl);				/*�ͻ�����*/
	set_zd_data( "0930",g_cif_basic_inf.sts);				/*�ͻ���ǰ״̬*/
	set_zd_data( "0810",cif_email_inf.email);				/*�ͻ���ϵ�绰*/
	set_zd_data( "0820",cif_addr_inf.addr);					/*�ͻ���ϵ��ַ*/	
	set_zd_data( "0830",cif_email_inf.addr_type);				/*�ͻ���ϵ��ʽ*/

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
