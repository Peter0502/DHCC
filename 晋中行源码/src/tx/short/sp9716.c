/*************************************************
* �� �� ��: sp9716.c
* ���������������ʺŻ��������Ϣ[͸֧Э��Ǽ�]
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
#include "cif_basic_inf_c.h"

sp9716()  
{ 		
	int ret=0;
	char tmp_ratio[21];

	struct cif_basic_inf_c	g_cif_basic_inf;
	struct prdt_ac_id_c	g_prdt_ac_id;

	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",g_pub_tx.ac_no);						/*�ʺ�*/
	pub_base_old_acno( g_pub_tx.ac_no );

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
	   					"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ʻ�������");
	 	WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
					 "ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
	 	sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
	 			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,
						 "cif_no=%ld",g_prdt_ac_id.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸÿͻ���!!cif_no=[%ld]",g_prdt_ac_id.cif_no);
		WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	set_zd_data("0250",g_cif_basic_inf.name);

 	ret=Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d \
			and ac_sts<>'*'", g_prdt_ac_id,g_prdt_ac_id.ac_seqn);
		sprintf(acErrMsg,"ret=[%d][%ld]",ret, g_prdt_ac_id );
		WRITEMSG
	if(!ret)
	{
 		ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
						g_od_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"��Ʒ������g_od_mst.prdt_no=[%s]",g_od_mst.prdt_no);
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld", \
							g_od_mst.pay_ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ʺŲ�����g_od_mst.pay_ac_id=[%ld]", \
					g_od_mst.prdt_no);
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		sprintf(acErrMsg,"ret=[%d]",ret);
		WRITEMSG
		set_zd_data("0230",g_od_mst.prdt_no);				/*͸֧��Ʒ���*/
		set_zd_data("0240",g_od_mst.prdt_no);				
		set_zd_data("0260",g_od_parm.title);				/*͸֧��Ʒ����*/
		set_zd_double("0390",g_od_mst.max_bal);				/*���͸֧�޶�*/
		set_zd_double("0400",g_od_mst.cash_max_bal);		/*�ֽ�͸֧�޶�*/
		set_zd_double("0410",g_od_mst.min_pay_amt);			/*��ͻ�����*/
		g_od_mst.min_pay_ratio=g_od_mst.min_pay_ratio*100;
		set_zd_double("0420",g_od_mst.min_pay_ratio);		/*��ͻ������*/
		set_zd_data("0310",g_mdm_ac_rel.ac_no);				/*�����ʺ�*/
	}

	set_zd_data("0300",g_pub_tx.ac_no);
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
   
