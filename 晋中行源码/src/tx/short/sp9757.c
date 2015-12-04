/*************************************************
* �� �� ��:  sp9757.c
* ���������� 1111���׻�����Ϣ�Ŀ��
* ��    ��:  lance
* ������ڣ� 2003��05��30��
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
#include "cif_basic_inf_c.h"
#include "cif_magn_inf_c.h"

int sp9757()
{
	int	ret;
	long cif_no;
	char cif_no_str[9];
	char cif_typ[2];
	
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*�ͻ�֤�������*/
	struct  cif_basic_inf_c cif_basic_inf;
	struct  cif_magn_inf_c cif_magn_inf;
	
	/*���*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_magn_inf,0x00,sizeof(struct cif_magn_inf_c));
		
	/*ǰ̨ȡֵ*/
	get_zd_long("0280",&cif_no);       				/*�ͻ���*/


	/* ���ͻ����Ƿ��Ѿ����� */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;;
	}
	sprintf(acErrMsg,"���ͻ����Ƿ��Ѿ����� PASS!");
	WRITEMSG
		
	/* �ͻ��ż�� */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* ����Ƿ�Թ��ͻ� */
	if ( cif_typ[0]!='3' )
	{
		sprintf(acErrMsg,"�ǻ����ͻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C101");
		goto ErrExit;
	}
	
	/* ��ѯ�����ͻ���Ϣ�����Ƿ�Ǽǹ� */
   	ret=Cif_magn_inf_Sel(g_pub_tx.reply,&cif_magn_inf, "cif_no=%ld",cif_no);
   	if( ret==0 )
   	{
       		sprintf( acErrMsg,"�˿ͻ���Ϣ�ѵǼ�!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C102");
   		goto ErrExit;
   	}else if( ret&&ret!=100 ){
         	sprintf( acErrMsg,"��ȡ�����ͻ���Ϣ��ʧ��!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}


   	ret=Cif_id_code_rel_Dec_Sel(g_pub_tx.reply, "cif_no=%ld",cif_no);
   	if( ret )
   	{
        sprintf( acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ��쳣!" );
        WRITEMSG
        goto ErrExit;
	}

	while(1)
	{
		ret=Cif_id_code_rel_Fet_Sel(&g_cif_id_code_rel,g_pub_tx.reply);
		if(ret==100)break;
   		else if( !ret )
   		{
			if(g_cif_id_code_rel.id_type[0]=='8')
				set_zd_data( "0620", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='A')
				set_zd_data( "0630", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='9')
				set_zd_data( "0300", g_cif_id_code_rel.id_no );	
			if(g_cif_id_code_rel.id_type[0]=='B')
				set_zd_data( "0330", g_cif_id_code_rel.id_no );								
   		}
   		else if( ret!=100 )
   		{
        	sprintf( acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ��쳣!" );
     	  	WRITEMSG
       		goto ErrExit;
   		}
	}

	Cif_id_code_rel_Clo_Sel();

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

