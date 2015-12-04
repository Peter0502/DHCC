/*************************************************************
* �� �� ��:  sp9960.c
* ���������� 2108�շѽ���_�ͻ��˺Ż��Ի���2
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

int sp9960()
{
	int ret=0;

	struct prdt_ac_id_c prdt_ac_id_c;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("122D",g_pub_tx.ac_no);
	sprintf(acErrMsg, "���˺�Ϊ:[%s]", g_pub_tx.ac_no);
	WRITEMSG
	pub_base_old_acno( g_pub_tx.ac_no );               /**�Ծ��ʺŵĴ���**/
	pub_base_strpack( g_pub_tx.ac_no );
	if( strlen(g_pub_tx.ac_no)==0 ) goto OkExit;

	sprintf(acErrMsg, "���˺�Ϊ:[%s]", g_pub_tx.ac_no);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "�˺Ų�����![%s]",g_pub_tx.ac_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* jane��2003��7��27����� begin */
	/*�����˻�idȡ���˻����*/
	memset( &prdt_ac_id_c, 0x00, sizeof(struct prdt_ac_id_c) );
	
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c,
			   "ac_id=%ld", g_mdm_ac_rel.ac_id );
	if ( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W012");
		return 1;
	}else if( ret )
	{
	    sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ��쳣[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W013");
		return 1;
	}

	/* ��ѯ�ͻ�����*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,prdt_ac_id_c.ac_seqn,
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
	/* jane��2003��7��27����� end */

	/*	jane��2003��7��27�շ�
	*** ������� ***
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, \
							"ac_id=%ld and ac_seqn=1", g_mdm_ac_rel.ac_id);
		if(ret)
		{
	 		sprintf(acErrMsg,"�ÿ��ڲ����ڻ��ڻ�,�������շ�!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O226");
			goto ErrExit;
		}
	}
	else
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld", \
							  g_mdm_ac_rel.ac_id);
		if(ret==100)
		{
	 		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
	}

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", \
						 prdt_ac_id_c.ac_id);
		if(ret==100)
		{
	 		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
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
		sprintf(acErrMsg,"���˺Ų������շѽ���!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"O192");
   		goto ErrExit;
	}*/

	set_zd_data("122E", g_pub_tx.name);
	set_zd_data("122D", g_pub_tx.ac_no);

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
