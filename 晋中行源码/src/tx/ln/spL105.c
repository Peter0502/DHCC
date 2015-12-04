/*************************************************
* �� �� ��: spL105.c
* ����������   
*              ��������
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
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"
			
spL105()  
{ 		
	int ret=0;
	char sts[2];
	char pay_ac_no[20];
	char save_ac_no[20];
	long cif_no;
	
	struct ln_auth_c	ln_auth;
	struct ln_auth_c	ln_auth_old;	
	
	memset( &ln_auth , 0x00 , sizeof( struct ln_auth_c ) );
	memset( &ln_auth_old , 0x00 , sizeof( struct ln_auth_c ) );
	pub_base_sysinit();
	get_zd_data("0660" , sts);		/* �������� */
	get_zd_data("0630" , ln_auth.pact_no);	/* �� �� �� */
	get_zd_data("0250" , ln_auth.name);	/* ����� */
	get_zd_data("0380" , ln_auth.pay_ac_no);/* �����˺� */
	get_zd_data("0320" , ln_auth.save_ac_no);/* ����˻� */
	get_zd_double("0400" , &ln_auth.amt);	/* ������ */
	get_zd_double("0840" , &ln_auth.rate);	/* ��ͬ���� */	
	get_zd_int("0350" , &ln_auth.time_lmt);	/* �������� */
  pub_base_old_acno(ln_auth.pay_ac_no);
  pub_base_old_acno(ln_auth.save_ac_no);
 	/**** 
	DBG_printf("~~Start %d@%s  in(%s)  --> trace_no[%ld] \n", __LINE__, __FILE__, __FUNCTION__, g_pub_tx.trace_no);
	****/
	
	/* ���ݻ����˺�ȡ�����˺�ID */
	pub_base_strpack( ln_auth.pay_ac_no );
	if( strlen(ln_auth.pay_ac_no) )
	{
		ret = pub_ln_AcCheck( ln_auth.pay_ac_no,1,&cif_no,&ln_auth.pay_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L184");
			}
			goto  ErrExit;
		}

	}	
	/* ���ݴ���˺�ȡ����˺�ID */	
	pub_base_strpack( ln_auth.save_ac_no );
	if( strlen( ln_auth.save_ac_no ) )
	{
		ret = pub_ln_AcCheck( ln_auth.save_ac_no,1,&cif_no,&ln_auth.save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}

	}
	/* ���ݴ���˺�ȡ����˺Ž������� */
	strcpy( ln_auth.save_mdm_code , g_mdm_ac_rel.mdm_code );
		 
	
	if( sts[0]=='0')	/* ¼�� */
	{				
		/** ��Ӵ���������Ϣ�� **/
		strcpy( ln_auth.sts ,"0" );		/* ���״̬----���� */
		ret = Ln_auth_Ins( ln_auth , g_pub_tx.reply );
		if (ret&&ret!=-239)
		{
			sprintf(acErrMsg,"�ǼǴ���������Ϣ���쳣����,�������Ļ�����ϵ![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L164");
			goto ErrExit;
		}else if(ret==-239){
			sprintf(acErrMsg,"����������Ϣ�����ظ���¼![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L165");
			goto ErrExit;
		}							
		sprintf(acErrMsg,"�ǼǴ���������Ϣ�� PASS!");
		WRITEMSG
		/* ��ѯ�����Ǽǲ����Ƿ����ظ��ĺ�ͬ�� */
		/* ��ѯ�жҵǼǲ����Ƿ����ظ��ĺ�ͬ�� */
						
	}else if( sts[0]=='1' ){	/* �޸� */
	
		ret = Ln_auth_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , ln_auth.pact_no );
		if( ret )
		{
			sprintf( acErrMsg,"Prepare Ln_auth_Dec_Upd �α����[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}
		ret = Ln_auth_Fet_Upd( &ln_auth_old , g_pub_tx.reply );
		if( ret==100 )
		{
			sprintf( acErrMsg,"�޴�������¼![%d]",ret);
			WRITEMSG
		 	strcpy( g_pub_tx.reply ,"L175" );
			ERR_DEAL ;
		}else if( ret ){
		 	sprintf( acErrMsg,"fetch Ln_auth_Fet_Upd error code = [%d]",ret);
		 	WRITEMSG
		 	strcpy( g_pub_tx.reply ,"D103" );
		 	ERR_DEAL ;
		}
		/*MIS ��������ʵxX*/
		if('x' == ln_auth_old.sts[0] || 'X' == ln_auth_old.sts[0]) 
			strcpy( ln_auth.sts ,"0" );	
		else		
			strcpy( ln_auth.sts ,ln_auth_old.sts );		/* ���״̬ */	
		strcpy(ln_auth.rowid, ln_auth_old.rowid); /*rowid   û�м���, gujy 20060824*/
		ln_auth.opn_date=ln_auth_old.opn_date;
		ln_auth.mtr_date=ln_auth_old.mtr_date;

		ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf( acErrMsg,"Ln_auth_Upd_Upd ����[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
		Ln_auth_Clo_Upd( );
		/****
		DBG_printf("%d@%s ====%d \n", __LINE__, __FILE__, ret);
		****/
		sprintf(acErrMsg,"�޸�������ϢPASS!");
		WRITEMSG			
	}else if( sts[0]=='2' ){	/* ɾ�� */
		ret=sql_execute("delete from ln_auth where pact_no='%s'",ln_auth.pact_no);
		if (ret)	ERR_DEAL;	
	
	}
	vtcp_log("~~Start[%d][%s]--> trace_no[%ld] \n", __LINE__, __FILE__,g_pub_tx.trace_no);
	 /*** �Ǽǽ�����ˮ ***/
	 	strcpy(g_pub_tx.brf,"��������");
    if( pub_ins_ln_trace_log(ln_auth.amt))
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
    WRITEMSG

			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
/****
DBG_printf("~~END ���������ɹ� %d@%s  in(%s)  --> trace_no[%ld] \n", __LINE__, __FILE__, __FUNCTION__, g_pub_tx.trace_no);
****/
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    

