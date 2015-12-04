/*************************************************
* �� �� ��:  sp9896.c
* ���������� ��˾�ͻ���Ϣά����ݲ�ѯ
*
* ��    ��:  rob
* ������ڣ� 2005��2��3��
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
#include "cif_cop_inf_c.h"

int sp9896()
{
	int ret = 0;
	long cif_no;
	char sts[2];
	char cif_typ[2];
	
	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_cop_inf_c g_cif_cop_inf;			

	/*���*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));

	/*��ʼ��*/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0670",sts);				/*��������*/
	get_zd_long("0280",&cif_no);			/*�ͻ���*/

	/*����*/
	/* �ͻ��ż�� */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* ����Ƿ���˿ͻ� */
	if ( cif_typ[0]!='2' )
	{
		sprintf(acErrMsg,"�ǹ�˾�ͻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C124");
		goto ErrExit;
	}	

	/*��ѯ��˾�ͻ���Ϣ��*/
	ret=Cif_cop_inf_Sel(g_pub_tx.reply,&g_cif_cop_inf,"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˿ͻ���δ�Ǽǹ�˾�ͻ���Ϣ!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C109");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ��˾�ͻ���Ϣ���쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0620",g_cif_cop_inf.crd_no);          /*����֤��*/
	set_zd_data( "0630",g_cif_cop_inf.license);         /*Ӫҵִ��*/
	set_zd_data( "0580",g_cif_cop_inf.credit_no);       /*�����*/
	set_zd_data( "0300",g_cif_cop_inf.units_license);   /*�Ǽ�֤��*/
	set_zd_data( "0310",g_cif_cop_inf.g_tax_no);   		/*��˰��*/
	set_zd_data( "0320",g_cif_cop_inf.d_tax_no);   		/*��˰��*/
	set_zd_data( "0660",g_cif_cop_inf.eco_attri);  		/*��������*/
	set_zd_data( "0670",g_cif_cop_inf.ent_attri);  		/*��ҵ����*/
	set_zd_data( "0220",g_cif_cop_inf.cif_scopes); 		/*��������*/
	set_zd_data( "0680",g_cif_cop_inf.belong_bank); 	/*������ҵ(����)*/
	set_zd_data( "0690",g_cif_cop_inf.belong_stat); 	/*������ҵ(����)*/
	set_zd_data( "0700",g_cif_cop_inf.region); 			/*���򼶱�*/
	set_zd_data( "0710",g_cif_cop_inf.mana_orga_moda); 	/*��Ӫ��ʽ*/
	set_zd_data( "0720",g_cif_cop_inf.bd_corp_orga_moda);	/*��ҵ��ʽ*/
	set_zd_data( "0230",g_cif_cop_inf.corp_act_mode);	/*������ʽ*/
	set_zd_long( "0440",g_cif_cop_inf.reg_date);		/*ע������*/
	set_zd_data( "0890",g_cif_cop_inf.reg_country);		/*ע�����*/
	set_zd_data( "0210",g_cif_cop_inf.cur_no);			/*ע�����*/
	set_zd_double( "0400",g_cif_cop_inf.reg_fund);		/*ע���ʽ�*/
	set_zd_double( "0410",g_cif_cop_inf.paicl_up_capital);/*ʵ���ʱ�*/
	set_zd_data( "0330",g_cif_cop_inf.ap_qualife);		/*�����ʸ�*/
	set_zd_data( "0810",g_cif_cop_inf.artificial_person);	/*��������*/
	set_zd_data( "0820",g_cif_cop_inf.ap_id);			/*�������֤��*/
	set_zd_data( "0900",g_cif_cop_inf.corp_size);		/*��ҵ��ģ*/
	set_zd_data( "0240",g_cif_cop_inf.corp_mana_sts);	/*��Ӫ״̬*/
	set_zd_data( "0640",g_cif_cop_inf.credit);			/*������*/
	set_zd_data( "0250",g_cif_cop_inf.supervior);		/*�ϼ���λ*/
	set_zd_data( "0260",g_cif_cop_inf.director);		/*���ܵ�λ*/

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
