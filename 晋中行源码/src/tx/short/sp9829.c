/*************************************************
* �� �� ��:    sp9829.c
* ����������   
*              ���ݴ�����ʾ�˺ź��˺����ȡ�������ļ�
*
* ��    ��:    lance
* ������ڣ�   2003��01��24��
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
	
sp9829()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	char flag[2];
	double rate;
	
	/** ���ݳ�ʼ�� **/
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0370" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0370",ac_no);	/* ������ʺ� */	
	
	get_zd_int( "0340" , &ac_seqn );	/* �����˻���� */
	get_zd_data( "0710" , flag );	/* �������� */
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
    }

	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]seqn[%d]",
		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	WRITEMSG	
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		Ln_mst_Clo_Sel ();
	}

	if( flag[0] =='1' )
	{
	 	rate=g_ln_mst.rate;	/* ���� */
	}
	 				
	if( flag[0] =='2' )
	{
	 	rate=g_ln_mst.exp_rate;	/* չ�� */
	}
	 
	if( flag[0] =='3')
	{
	 	rate=g_ln_mst.over_rate;	/* ���� */
	}	
	 
	if( flag[0] =='4')
	{
	 	rate=g_ln_mst.fine_rate;	/* ��Ϣ */
	}	 
	
	/* ������� */
	sprintf(acErrMsg,"�����������[%s]",g_ln_mst.name);
	WRITEMSG	
	set_zd_double( "0840", rate );			/* ���� */			
			
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

