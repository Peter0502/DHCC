/*************************************************************
* �� �� ��: gDCLN.c
* �������������һ���ƻ�����
*
* ��    ��: jack
* �������: 2003��3��30��
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

gDCLN()
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
	ret = Ln_mst_Dec_Upd(g_pub_tx.reply," opn_date!=ic_date and repay_type in ('3','4') ");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret= Ln_mst_Fet_Upd(&S_ln_mst, g_pub_tx.reply);
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
		
		cnt++;
		/*�Դ˻����⴦��*/
		if( S_ln_mst.ac_id == 100354593 )
		{
			ln_mst_max_date= 20040326;
		
			sprintf(acErrMsg,"������ϸ���������ǰ�����ա�%d��",ln_mst_max_date);
			WRITEMSG
			sprintf(acErrMsg,"�������ļ���ԭ�е���Ϣ���ڡ�%d��",S_ln_mst.ic_date);
			WRITEMSG
		}else{	
			ln_mst_max_date=0;
			ret= sql_max_long("ln_mst_hst","tx_date",&ln_mst_max_date,"ac_id=%ld and ac_seqn=%d and tx_code='2486'",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
			if( ret )	goto ErrExit;
			sprintf(acErrMsg,"������ϸ���������ǰ�����ա�%d��",ln_mst_max_date);
			WRITEMSG
			sprintf(acErrMsg,"�������ļ���ԭ�е���Ϣ���ڡ�%d��",S_ln_mst.ic_date);
			WRITEMSG	
		}
			
		if( ln_mst_max_date%100 < 21 )
		{
			sprintf(acErrMsg,"������С��21��");
			WRITEMSG	
			if( ln_mst_max_date%10000<=200 )/*����1�·�*/
			{
				sprintf(acErrMsg,"������Ϊһ�·�");
				WRITEMSG				
				ln_mst_ic_date= (ln_mst_max_date/10000-1)*10000+1222;
			}else{
				ln_mst_ic_date= (ln_mst_max_date/100-1)*100+22;	
			}			
		}else{
			sprintf(acErrMsg,"�����մ���21��");
			WRITEMSG
		 	ln_mst_ic_date= ln_mst_max_date/100*100+22;
		}
		S_ln_mst.ic_date= ln_mst_ic_date;
		sprintf(acErrMsg,"�޸ĺ����Ϣ���ڡ�%d��",S_ln_mst.ic_date);
		WRITEMSG							
		
		/*�޸İ��һ���ƻ�*
		ret= Ln_pay_pln_Sel( g_pub_tx.reply, &S_ln_pay_pln,"ac_id=%ld and ac_seqn=%d ",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if (ret)
		{
			sprintf(acErrMsg,"Ln_pay_pln_Sel ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}	
		****/
			
		/* ���������� */
		month_cnt = pub_base_CalMouths( S_ln_mst.ic_date , S_ln_mst.mtr_date ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"������[%d]",month_cnt);
		WRITEMSG
		if( S_ln_mst.mtr_date%100>21 )	
		{
			sprintf(acErrMsg,"�޸ĺ�������[%d]",month_cnt);
			WRITEMSG
			month_cnt=month_cnt+1;
		}
		sprintf(acErrMsg,"�޸ĺ�������[%d]",month_cnt);
		WRITEMSG		
		month_now = pub_base_CalMouths( S_ln_mst.ic_date , 20050921 ); 
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"������[%d]",month_now);
		WRITEMSG				
		ret= sql_execute( " UPDATE ln_pay_pln SET ttl_cnt=%d, curr_cnt=%d WHERE ac_id=%ld and ac_seqn=%d ",month_cnt,month_now,S_ln_mst.ac_id,S_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		TRACE
		ret = Ln_mst_Upd_Upd( S_ln_mst , g_pub_tx.reply  );
	        if (ret)
	        {
	               sprintf(acErrMsg,"���´������ļ�����!!");
	               WRITEMSG
	               goto ErrExit;
	        }
	        ret= Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d",S_ln_mst.ac_id,S_ln_mst.ac_seqn);
	        if(ret)		goto ErrExit;
		sprintf(acErrMsg,"----------------------����������--------------------------------");
		WRITEMSG	         
		sprintf(acErrMsg,"�����������������������ڡ�%d������¼����������������������������",cnt);
		WRITEMSG
		sprintf(acErrMsg,"ac_no=��%s��������=��%d������=��%s��",g_mdm_ac_rel.ac_no,S_ln_mst.mtr_date,S_ln_mst.name);
		WRITEMSG
		sprintf(acErrMsg,"ac_id=��%ld����Ϣ��=��%d��������=��%d��������=��%d��",S_ln_mst.ac_id,S_ln_mst.ic_date,month_cnt,month_now);
		WRITEMSG
		sprintf(acErrMsg,"-----------------------------------------------------------------");
		WRITEMSG
	}
	Ln_mst_Clo_Upd( );    		                         
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
