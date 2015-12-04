/*************************************************************
* �� �� ��: gDCLL.c
* �������������һ���ƻ�����(�����˺�)
*
* ��    ��: jack
* �������: 2003��9��19��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"
#include "ln_mst_hst_c.h"
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}

gDCLL()
{      
	int ret=0;
	int cnt=0;
	long ln_mst_max_date=0;
	long ln_mst_ic_date=0;
	int month_cnt=0;
	int month_now=0;
	
	struct ln_pay_pln_c	S_ln_pay_pln;
	struct ln_mst_c		S_ln_mst;
	struct ln_mst_hst_c	S_ln_mst_hst;
	
	memset( &S_ln_pay_pln , 0x00 ,sizeof( struct ln_pay_pln_c ));
	memset( &S_ln_mst , 0x00 ,sizeof( struct ln_mst_c ));
	memset( &S_ln_mst_hst , 0x00 ,sizeof( struct ln_mst_hst_c ));
			
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
       
	/* ���������ļ� */    
	ret = Ln_mst_Dec_Sel(g_pub_tx.reply," opn_date=ic_date and repay_type in ('3','4') ");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret= Ln_mst_Fet_Sel(&S_ln_mst, g_pub_tx.reply);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;			
		}else if( ret==100 ){
			sprintf(acErrMsg,"��ִ�С�%d����",cnt);
			WRITEMSG			
			break;
		}		
		TRACE
		ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst,"ac_id=%ld and ac_seqn=%d and tx_code='2498' ",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret&&ret!=100)
		{
			sprintf(acErrMsg,"��ѯ������ϸʧ�� ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if(ret==100){
			TRACE
			continue;
		}
							
		month_now = pub_base_CalMouths( S_ln_mst.ic_date , 20050921 ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"������[%d]",month_now);
		WRITEMSG
		if( S_ln_mst.opn_date%100 >21 )
		{
			month_now=month_now-1;
		}				
		ret= sql_execute( " UPDATE ln_pay_pln SET curr_cnt=%d WHERE ac_id=%ld and ac_seqn=%d ",month_now,S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		cnt++;
		
		TRACE
	        ret= Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
	        if(ret)		goto ErrExit;
		sprintf(acErrMsg,"----------------------����������--------------------------------");
		WRITEMSG	         
		sprintf(acErrMsg,"�����������������������ڡ�%d������¼����������������������������",cnt);
		WRITEMSG
		sprintf(acErrMsg,"ac_no=��%s��������=��%d������=��%s��",g_mdm_ac_rel.ac_no,S_ln_mst.mtr_date,S_ln_mst.name);
		WRITEMSG
		sprintf(acErrMsg,"ac_id=��%ld����Ϣ��=��%d��������=��%d��",S_ln_mst.ac_id,S_ln_mst.ic_date,month_now);
		WRITEMSG
		sprintf(acErrMsg,"-----------------------------------------------------------------");
		WRITEMSG
	}
	Ln_mst_Clo_Sel( );    		                         
	sprintf(acErrMsg,"���һ���ƻ����� PASS");
	WRITEMSG	
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���һ���ƻ������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���һ���ƻ�����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
