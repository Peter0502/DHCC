/*************************************************
* �� �� ��: sp9756.c
* �������������ݿͻ��ź�֤��������ʾ֤������
* ��    ��: lance
* ������ڣ�2003��05��30��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
	
sp9756()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct dic_data_c        vdic_data; 
	struct cif_id_code_rel_c       cif_id_code_rel; 	
	int ret=0;
	char wherelist[40];
	char filename[100];
	char type[2],tmp[5];
	FILE *fp;
	int i=0;
	long cif_no;
	char id_type[2];
	char opt_typ[2];
	
	memset (&vdic_data, 0x00, sizeof(struct dic_data_c));
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));

	/** ȡֵ����ֵ **/
	get_zd_data( "0670" , opt_typ );	/* �������� */	
	get_zd_long( "0280" , &cif_no );	/* �ͻ��� */
	get_zd_data( "0680" , id_type ); 	/* ֤������ */
	
	if( opt_typ[0]=='0' )	/* ��� */
	{
		/* ���ݿͻ����жϿͻ����� */
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"cif_no=%ld and id_type='%s'" ,cif_no,id_type);
		if( ret==0 )
		{
			sprintf(acErrMsg,"�ÿͻ�����֤���ѵǼ�![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C099");
			ERR_DEAL;
		}else if( ret&&ret!=100 ){
			sprintf(acErrMsg,"��֤����֤�������ϵ���쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			ERR_DEAL;
		}
	}else{	/* �޸Ļ���ɾ�� */
	
		/* ���ݿͻ����жϿͻ����� */
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"cif_no=%ld and id_type='%s'" ,cif_no,id_type);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ÿͻ�����֤����δ�Ǽ�![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C098");
			ERR_DEAL;
		}else if( ret ){
			sprintf(acErrMsg,"��֤����֤�������ϵ���쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			ERR_DEAL;
		}	
		set_zd_data( "0620" , cif_id_code_rel.id_no );
	}
	
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
