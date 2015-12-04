/*************************************************
* �� �� ��:    sp9708.c
* ����������   �����ʺź��˻���Ż��������Ϣ
* ��    ��:    rob
* ������ڣ�   2003��02��19��
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
#include "prdt_ac_id_c.h"

sp9708()  
{ 		
	int ret=0,ac_seqn;
	char title[40],cur_no[3];
	char ac_no[20],name[50];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);							/*�ʺ�*/
	get_zd_int("1022",&ac_seqn);						/*�ʺ����*/
	
	/* �����ʺź��˻����ȡ��Ϣ */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
			   			"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,ac_seqn);
   	if( ret )
	{
		sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		 case '1':                               /*1-����*/
				 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
					 "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret )
				 {
					 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret )
				 {
					 sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }
				 bal=g_dd_mst.bal;
				 strcpy(name,g_dd_mst.name);
				 strcpy(title,g_dd_parm.title);
				 strcpy(cur_no,g_dd_parm.cur_no);
				 break;
		 case '2':                               /*2-����*/
				 ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
					 "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret )
				 {
					 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret )
				 {
					 sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 bal=g_td_mst.bal;
				 strcpy(name,g_td_mst.name);
				 strcpy(title,g_td_parm.title);
				 strcpy(cur_no,g_td_parm.cur_no);
				 break;
		default:
				 break;
		}

	/* ������� */
	set_zd_data("1141",g_mdm_ac_rel.draw_uncon_yn);
	set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("1147",g_mdm_ac_rel.draw_seal_yn);
	set_zd_data("1148",g_mdm_ac_rel.id_type);
	set_zd_double("0420",bal);
	set_zd_data("0210",cur_no);
	set_zd_data("1166",name);

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
 
