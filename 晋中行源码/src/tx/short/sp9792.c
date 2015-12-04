/*************************************************************
* �� �� ��:  sp9792.c
* ���������� �շѽ���_�ͻ��˺Ż��Ի���
* ��    ��:  rob
* ������ڣ� 2003��6��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9792()
{
	int ret=0;

	struct prdt_ac_id_c prdt_ac_id_c;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("1221",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );               /**�Ծ��ʺŵĴ���**/
	set_zd_data("1221", g_pub_tx.ac_no);
		
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if(ret==100)
	{
		sprintf( acErrMsg, "�˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	/*** ������� ***/
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, \
							"ac_id=%ld and ac_seqn=1", g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"�ÿ��ڲ����ڻ��ڻ�,�������շ�!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O226");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"���Ҵ���! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}
	else
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld", \
							  g_mdm_ac_rel.ac_id);
		if(ret)
		{
	 		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"���Ҵ���! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=1", \
						 prdt_ac_id_c.ac_id);
		if( ret )
		{
	 		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		if(g_dd_mst.ac_sts[0]=='*')	
		{
			sprintf(acErrMsg,"���˺�������!! [%d]",ret);
	 		WRITEMSG
	  		strcpy(g_pub_tx.reply,"P245");
	   		goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"���˺Ų������շѽ���!! [%s]",prdt_ac_id_c.prdt_no);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"O192");
   		goto ErrExit;
	}
	vtcp_log("ssssssssssssssss=[%s]",g_dd_mst.name);
	set_zd_data("122C", g_dd_mst.name);
	set_zd_data("1228", g_mdm_ac_rel.draw_pwd_yn);
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
