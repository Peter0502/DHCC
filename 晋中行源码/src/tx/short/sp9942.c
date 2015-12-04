/*************************************************
* �� �� ��:  sp9942.c
* ���������� ͨ���ͻ��Ų�ѯ�ͻ�����
* ��    ��:  lance
* ������ڣ� 2003��07��01��
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_per_inf_c.h"

int sp9942()
{
	int	ret=0,i=0;
	long cif_no;
	long bir_date;
	char sex[3];
	char cif_typ[2];
	char sts[2];
	
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*�ͻ�֤�������*/
	struct 	cif_per_inf_c   cif_per_inf;	/*���˿ͻ���Ϣ��*/
	
	/*���*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));	
	
	/*ǰ̨ȡֵ*/
	get_zd_long("0280",&cif_no);       				/*�ͻ���*/

	/* �ͻ��ż�� */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* ����Ƿ���˿ͻ� */
	if ( cif_typ[0]=='1' || cif_typ[0]=='5' )
	{
		sprintf(acErrMsg,"��˽�ͻ�!");
		WRITEMSG
		strcpy(sts,"1");
		set_zd_data("0660",sts);
	}/***���ӷ��ػ����ͻ����� add by wanglei 20070405***/
	else if(cif_typ[0] == '3')
	{
		sprintf(acErrMsg,"�����ͻ�!");
		WRITEMSG
		strcpy(sts,"3");
		set_zd_data("0660",sts);
	}else{
		sprintf(acErrMsg,"�Թ��ͻ�!");
		WRITEMSG
		strcpy(sts,"2");		
		set_zd_data("0660",sts);
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
