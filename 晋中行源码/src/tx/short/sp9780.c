/*************************************************
* �� �� ��:  sp9780.c
* ���������� �ɿͻ�����ʾ������Ϣ
*
* ��    ��:  lance
* ������ڣ� 2003��06��03�� 
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

	
int sp9780()
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
   	if( ret==100 )
   	{
       		sprintf( acErrMsg,"�˻����ͻ���Ϣ��δ�Ǽ�!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C103");
   		goto ErrExit;
   	}else if( ret ){
         	sprintf( acErrMsg,"��ȡ�����ͻ���Ϣ��ʧ��!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}
		
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	set_zd_long("0280",cif_magn_inf.cif_no);		/* �ͻ��� */
	set_zd_data("0620",cif_magn_inf.units_license);		/* ����֤�� */		
	set_zd_data("0330",cif_magn_inf.ap_qualife);	/* ����֤�� */	
	set_zd_data("0810",cif_magn_inf.artificial_person);/* ��ҵ�������� */				
	set_zd_data("0820",cif_magn_inf.ap_id);	   	/* �������֤���� */
	set_zd_data("0580",cif_magn_inf.credit_no);		/* ����� */
	set_zd_data("0710",cif_magn_inf.own_attri);	/* ��������ʽ */
	set_zd_data("0670",cif_magn_inf.ent_attri);		/* ��ҵ���� */	
	set_zd_data("0640",cif_magn_inf.credit);				/* ������ */		
	set_zd_data("0250",cif_magn_inf.supervior);	/* �ϼ���λ */	
	set_zd_data("0260",cif_magn_inf.director);	/* ���ܵ�λ */	
	set_zd_data("0270",cif_magn_inf.financer);	/* �����ʽ������λ */	
	set_zd_data("0830",cif_magn_inf.opn_bank);	/* �����˺ſ����� */	
	set_zd_double("0410",cif_magn_inf.opn_amt);	/* �����ʽ� */	





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

