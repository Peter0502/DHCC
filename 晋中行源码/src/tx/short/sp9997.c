/*************************************************
* �� �� ��: sp9717.c
* �������������ݲ�Ʒ��Ż�����ز���[͸֧Э��Ǽ�]
*
* ��    ��: rob
* ������ڣ�2003��03��31��
* �޸ļ�¼�� 
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

sp9717()  
{ 		
	int ret=0;
	char tmp_ratio[21],tmp_prdt[4],tmp_title[41];

	struct prdt_ac_id_c	g_prdt_ac_id;

	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0230",g_pub_tx.prdt_code);						/*��Ʒ���*/
	get_zd_data("0240",tmp_prdt);						

	get_zd_data("0260",tmp_title);								/*��Ʒ����*/
	get_zd_double("0390",&g_od_mst.max_bal);					/*͸֧�޶�*/
	get_zd_double("0400",&g_od_mst.cash_max_bal);				/*�ֽ��޶�*/
	get_zd_double("0410",&g_od_mst.max_bal);					/*��ͻ�����*/
	get_zd_double("0420",&g_od_mst.cash_max_bal);				/*��ͻ������*/
	get_zd_data("0310",g_mdm_ac_rel.ac_no);             		/*�����ʺ�*/

	if(!strlen(tmp_prdt))
	{
 		ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret==100 )
		{
			sprintf(acErrMsg,"��Ʒ������");
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		set_zd_data("0260",g_od_parm.title);				/*͸֧��Ʒ����*/
		set_zd_double("0390",g_od_parm.max_amt);			/*���͸֧�޶�*/
		set_zd_double("0400",g_od_parm.cash_amt);			/*�ֽ�͸֧�޶�*/
		set_zd_double("0410",g_od_parm.min_pay_amt);		/*��ͻ�����*/
		g_od_parm.min_pay_ratio=g_od_parm.min_pay_ratio*100;
		set_zd_double("0420",g_od_parm.min_pay_ratio);		/*��ͻ������*/
	}
	else if(strlen(tmp_prdt)&&strcmp(g_pub_tx.prdt_code,tmp_prdt))
	{
		sprintf(acErrMsg,"��Ʒ���Ͳ��ɸ���,��ѡ�����в���");
	 	WRITEMSG
		strcpy(g_pub_tx.reply,"O158");
		goto ErrExit;
	}
	else if(!strcmp(g_pub_tx.prdt_code,tmp_prdt))
	{
		sprintf(acErrMsg,"tmp_title=[%s]",tmp_title);
	 	WRITEMSG
		set_zd_data("0260",g_od_parm.title);				/*��Ʒ����*/
		set_zd_double("0390",g_od_mst.max_bal);				/*͸֧�޶�*/
		set_zd_double("0400",g_od_mst.cash_max_bal);		/*�ֽ��޶�*/
		set_zd_double("0410",g_od_mst.max_bal);				/*��ͻ�����*/
		set_zd_double("0420",g_od_mst.cash_max_bal);		/*��ͻ������*/
		set_zd_data("0310",g_mdm_ac_rel.ac_no);            	/*�����ʺ�*/
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
   
