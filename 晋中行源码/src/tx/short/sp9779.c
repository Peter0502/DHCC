/*************************************************
* �� �� ��:  sp9779.c
* ���������� ���˺ź���Ż��Ի���(0250)
*
* ��    ��:  lance
* ������ڣ� 2003��06��02�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9779()
{

	int	ret=0;

	get_zd_data( "0370", g_pub_tx.ac_no ); 			/* �˺� */
	get_zd_int( "0340", &g_pub_tx.ac_seqn );		/* �˺���� */
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , g_pub_tx.ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"���˺Ų�����,����! erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"C115" );
		goto	ErrExit;
	}else if( ret !=0 ){
    		sprintf( acErrMsg,"��ȡ�����˺Ŷ��ձ�ʧ��,�������Ļ�����ϵ!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"C114" );
    		goto	ErrExit;
    	}
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG
	
	/* ��ѯ�ͻ�����*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"��������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�����!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}

	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
		case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
		case '3':
				strcpy(g_pub_tx.name,g_ln_mst.name);
				break;		
		case '9':
				strcpy(g_pub_tx.name,g_in_mst.name);
				break;							
		default	:
				sprintf(acErrMsg,"�˺Ŷ�Ӧ��Ʒ���ʹ���,�������Ļ�����ϵ!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
	}	
	
	set_zd_data( "0250" , g_pub_tx.name );
 			

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

