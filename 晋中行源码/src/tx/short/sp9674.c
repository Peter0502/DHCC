/*************************************************
* �� �� ��:    sp9674.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó����������
*
* ��    ��:    lance
* ������ڣ�   2003��06��28��
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
	
sp9674()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	char cif_type_id[2];
	char name[51];
	
	int ac_seqn;
	long cif_no_id;
	long cif_no;
	long save_ac_id;	
	
	/** ���ݳ�ʼ�� **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/			
	get_zd_data( "0370" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}

	ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"ac_no='%s'",ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
	    goto OkExit;
	}
	set_zd_data("0640",g_mdm_ac_rel.mdm_code);
	if( g_mdm_ac_rel.ac_seqn==1 )
	{
		ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%d and ac_seqn=%d",
			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"ac_id=%d",g_mdm_ac_rel.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
	    	goto OkExit;
		}
		set_zd_data( "0250", g_dd_mst.name );		/* �����˻����� */	
		set_zd_long( "0340", g_mdm_ac_rel.ac_seqn );			/* �����˻���� */
		goto OkExit;
	}
	else if( g_mdm_ac_rel.ac_seqn==0 )
	{
		ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%d and ac_seqn=%d",
			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"ac_id=%d",g_mdm_ac_rel.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
	    	goto OkExit;
		}
		set_zd_data( "0250", g_td_mst.name );		/* �����˻����� */	
		set_zd_long( "0340", g_mdm_ac_rel.ac_seqn );			/* �����˻���� */
		goto OkExit;
	}
	else
	{
		ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%d and ac_seqn=1",
			g_mdm_ac_rel.ac_id);
		if( ret )
		{
			ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%d ",
				g_mdm_ac_rel.ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ac_id=%d",g_mdm_ac_rel.ac_id);
				WRITEMSG
				strcpy( g_pub_tx.reply,"M003" );
	    		goto OkExit;
			}
			set_zd_data( "0250", g_td_mst.name );		/* �����˻����� */	
		}
		else
		{
			set_zd_data( "0250", g_dd_mst.name );		/* �����˻����� */	
		}
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
   
