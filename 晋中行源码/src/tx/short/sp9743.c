/*************************************************
* �� �� ��:    sp9743.c
* ����������   
*              ���ݴ����˺�ȡ���˻����
*
* ��    ��:    lance
* ������ڣ�   2003��03��22��
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
	
sp9743()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	
	
	struct ln_mst_c		ln_mst_tmp;
	
	/** ���ݳ�ʼ�� **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0370" , ac_no );	/* �����˺� */

	
	/* ���ݴ����˺Ų�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* ���ݴ����˻�ID��ѯ�������ļ����� */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"���ݴ����˻�ID��ѯ�������ļ�����[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
 		set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ */   
 		sprintf(acErrMsg,"�������ļ������޴˼�¼[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L002");
 		goto ErrExit;
   	 }	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%s]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"ȡ�������ļ�PASS");
	WRITEMSG 

	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	redflag=1;
	set_zd_long( "0350", ln_mst_tmp.ac_seqn );	/* �����˻���� */
	
	set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/    	
    	goto OkExit;
    	}
    	
    	
  	redflag=9;
	set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ 9-������¼*/ 	 		
		
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
   
