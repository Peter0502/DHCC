/*************************************************
* �� �� ��: sp9715.c
* �������������ݸ����Ų�ѯ���޶�����_NO.2
*
* ��    ��: rob
* ������ڣ�2003��03��25��
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
#include "card_consum_lmt_c.h"

sp9715()  
{ 		
	int ret=0;

	struct  prdt_ac_id_c   		g_prdt_ac_id;
	struct  card_consum_lmt_c   g_card_consum_lmt;
	struct  card_consum_lmt_c   card_consum_lmt;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_card_consum_lmt,0x00,sizeof(struct card_consum_lmt_c));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",g_pub_tx.ac_no);					/*������*/
	pub_base_old_acno( g_pub_tx.ac_no );                /**�Ծ��ʺŵĴ���**/

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						g_pub_tx.ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",g_pub_tx.ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
	    sprintf(acErrMsg,"�����뿨��!ac_no=[%s]",g_pub_tx.ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"O191");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.main_ind[0]!='2' )
	{
	    sprintf(acErrMsg,"�����븱����!ac_no=[%s]",g_pub_tx.ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"P241");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, \
						"ac_id=%ld", g_mdm_ac_rel.ac_id);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

 	ret=Card_consum_lmt_Sel(g_pub_tx.reply,&g_card_consum_lmt, \
							"ac_no='%s'",g_pub_tx.ac_no);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"��ѯ���޶�ǼǱ��쳣[%s]");
		WRITEMSG
		strcpy( g_pub_tx.reply, "D119" );
		goto ErrExit;
	}

	set_zd_double("0390",g_card_consum_lmt.da_cash_lmt);
	set_zd_double("0400",g_card_consum_lmt.da_amt_lmt);
	set_zd_double("0410",g_card_consum_lmt.da_con_lmt);
	set_zd_double("0420",g_card_consum_lmt.od_cash_lmt);
	set_zd_double("0430",g_card_consum_lmt.od_amt_lmt);
	set_zd_double("1001",g_card_consum_lmt.mo_cash_lmt);
	set_zd_double("1002",g_card_consum_lmt.mo_amt_lmt);
	set_zd_double("1003",g_card_consum_lmt.mo_con_lmt);

	set_zd_data("0310",g_pub_tx.ac_no);

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
   
