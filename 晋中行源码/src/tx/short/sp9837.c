/*************************************************
* �� �� ��:    sp9837.c
* ����������   
*              �����˺�ȡ���˻����ƺ��ʻ����
*
* ��    ��:    rob
* ������ڣ�   2003��01��25��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9837()  
{ 		
	int ret=0;
	char ac_no[20],ac_type[2],tmp_name[50];
	double tmp_bal;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data("0310",ac_no);						/* �ʺ� */
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����˺Ŷ��ձ��в����ڴ˼�¼!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}
	else if( ret )
	{
		sprintf( acErrMsg,"��ѯ�����ʺŶ��ձ��쳣!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}

	if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)g_mdm_ac_rel.ac_seqn=1;

	/* ������ʾ�ʺź��˺���Ų�ѯ���ļ� */
	ret=pub_base_disac_file(ac_no, g_mdm_ac_rel.ac_seqn, ac_type);
	if( ret )
	{
		sprintf( acErrMsg,"������ʾ�ʺź��˺���Ų�ѯ���ļ�erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}

	switch(ac_type[0])
	{
		case '1':	tmp_bal=g_dd_mst.bal;
					strcpy(tmp_name,g_dd_mst.name);
					break;
		case '2':   tmp_bal=g_td_mst.bal;
					strcpy(tmp_name,g_td_mst.name);
					break;
		 default:	
					break;
	}

	/* ������� */
	set_zd_data( "0250", tmp_name );				/* ���� */
	set_zd_double( "0410", tmp_bal);				/* ���	*/    	
		
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
   
