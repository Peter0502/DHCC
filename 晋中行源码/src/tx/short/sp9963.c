/*************************************************
*2101��֤����ݲ�ѯ
* �� �� ��: sp9963.c
* ��������������֤�����ͺ�֤������ ���ؿͻ�����
*			����
* ��    ��: mike
* ������ڣ�2003��07��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9963()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "0670" , cif_id_code_rel.id_type );	/* ֤������ */
	get_zd_data( "0620" , cif_id_code_rel.id_no );		/* ֤������ */
	vtcp_log("[%s].........[%s]",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	
	/* ����֤�����ͺ�֤������ȡ�ͻ��� */
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
		  "id_type='%s' and id_no='%s'",cif_id_code_rel.id_type, \
		  cif_id_code_rel.id_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ϣû�еǼ�,�뵽�ͻ����ĵǼ�[%s]", \
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
					  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_id_code_rel.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ӧ�Ŀͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}

	/* ������� */
	pub_base_strpack(cif_basic_inf.name);
	set_zd_data( "0250", cif_basic_inf.name );
	sprintf(g_pub_tx.ac_no,"%ld",cif_id_code_rel.cif_no);
	set_zd_data( "0300", g_pub_tx.ac_no );
	sprintf(g_pub_tx.ac_no,"%ld",cif_id_code_rel.cif_no);

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
