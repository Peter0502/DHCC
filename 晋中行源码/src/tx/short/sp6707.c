/*************************************************
* �� �� ��:    sp6707.c
* ����������   
*              ���ݿͻ��ŷ��ؿͻ�����
*
* ��    ��:    wanglei
* ������ڣ�   2006��11��28��
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

	
sp6707()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_id_code_rel_c      cif_id_code_rel_tmp; 
	struct cif_basic_inf_c        cif_basic_inf;

	int  ret =0;	
	long Icif_no=0;

	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/** ȡֵ����ֵ **/	
	get_zd_long( "0550" , &Icif_no );  /* �ͻ��� */
	vtcp_log("[%s][%d] cif_no=[%ld]",__FILE__,__LINE__,Icif_no);
	
					  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , Icif_no);
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

	set_zd_data("0250",cif_basic_inf.name);/**�ͻ�����**/

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
