/*************************************************
* �� �� ��:  sp9797.c
* ���������� ����֤�����ͺͺ����ѯ�ͻ��Ƿ��ѵǼ�
*
* ��    ��:  rob 
* ������ڣ� 2003��2��21��
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

int sp9797()
{
	int	ret;
	char id_no[21],id_type[2],code_flag[5];
	memset(id_type,0x00,sizeof(id_type));
	memset(id_no,0x00,sizeof(id_no));
	memset(code_flag,0x00,sizeof(code_flag));
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*�ͻ�֤�������*/

	/*���*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

	/*ǰ̨ȡֵ*/
	get_zd_data("0680",id_type);       				/*֤������*/
	get_zd_data("0620",id_no);       				/*֤������*/
	get_zd_data("0490",code_flag);  
	
	vtcp_log("%s,%d id_type=[%c], code_flag=[%s]\n",__FILE__,__LINE__,id_type[0],code_flag);
	/*���ղ���Ҫ�� 2101���ͻ�id ��ȥ���֤������ �������֤���������� ����49��ֵ ������ wudawei 20131011*/
  if(memcmp(code_flag,"1101",4)==0)
  {
			ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
					" id_no='%s' ",id_no);
	}
	else
	{
			ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
					"id_type='%s' and id_no='%s'",id_type,id_no);
		
	}
   	if( !ret )
   	{
        sprintf( acErrMsg,"�ͻ��ѵǼǣ�����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "C086" );
        goto ErrExit;
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

