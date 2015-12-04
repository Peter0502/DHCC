/*************************************************
* �� �� ��:  sp9895.c
* ���������� �ͻ�������Ϣά����ݲ�ѯ
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
#include "cif_alias_inf_c.h"

int sp9895()
{
	int ret = 0;
	int alias_seqn;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_alias_inf_c g_cif_alias_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0670",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/
	get_zd_int("0360",&alias_seqn);			/*�������*/

	/*����*/
	/*��ѯ�ͻ�������Ϣ��*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ���в����ڸÿͻ���!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C105");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*��ѯ�ͻ�������Ϣ��*/
		sprintf(acErrMsg,"sts=%s,cif_no=%ld and alias_seqn=%d",sts,cif_no,alias_seqn);
		WRITEMSG
	ret=Cif_alias_inf_Sel(g_pub_tx.reply,&g_cif_alias_inf,
						"cif_no=%ld and alias_seqn=%d",cif_no,alias_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�����ڸü�¼!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C057");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�ͻ�������Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

		sprintf( acErrMsg, "�쳣!!g_cif_alias_inf.alias=[%s]",
							g_cif_alias_inf.alias );
		WRITEMSG
		
	if( sts[0]=='2' && alias_seqn==1 )	/* ɾ����һ����¼ */
	{
		TRACE
		sprintf( acErrMsg, "������¼����ɾ��!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C104");		
		goto ErrExit;
	}		
	if( sts[0]=='1' && alias_seqn==1 )	/* �޸ĵ�һ����¼ */
	{
		TRACE
		sprintf( acErrMsg, "������¼�����޸�!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C111");		
		goto ErrExit;
	}
		
	set_zd_data( "0270",g_cif_alias_inf.alias);          /*�ͻ�ԭ����*/

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
