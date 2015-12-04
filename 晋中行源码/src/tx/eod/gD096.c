/*************************************************************
* �� �� ��: gD096.c
* �������������գ������������
*
* ��    ��: lance
* �������: 2003��10��02��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if(ret) {\
		sprintf(acErrMsg,"SQLERROR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "cif_basic_inf_c.h"

gD096()
{
	int  ret=0;
	long lst_free_date=0;
	double sect_acm=0.00;
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
   	/* ��ʼ��ȫ�ֱ��� */
   	pub_base_sysinit();
   		
	ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"1=1");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		/* �˻�״̬�����İ��Ҵ������ */
		if( (g_ln_mst.repay_type[0]=='3' || g_ln_mst.repay_type[0]=='4') &&g_ln_mst.ac_sts[0]=='1')
		{
			sprintf(acErrMsg,"�����İ��Ҵ������[%ld]",g_ln_mst.ac_id);
			WRITEMSG			
			continue;	
		}
		/* ���汾�塢���ռ�Ϣ������Ϣ ���迼�� */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_mst.intst_type[0]=='1' || g_ln_mst.intst_type[0]=='2' )
		{
			sprintf(acErrMsg,"��Ϣ���Ͳ����ϲ�����[%s]",g_ln_mst.intst_type);
			WRITEMSG
			continue;
		}
		
		/* ȷ������������ */
		ret=get_lst_ic_date( g_ln_mst.ic_date,g_ln_mst.intst_type,&lst_free_date);
		if(ret)	goto ErrExit;
		
		/* ��������ǰ������ */
		if( g_pub_tx.tx_date == lst_free_date )
		{
			sprintf(acErrMsg,"���������ڣ�������!��ǰ����[%ld]��������[%ld]",g_pub_tx.tx_date,lst_free_date);
			WRITEMSG
			continue;		
		}else{
			/* ȡ������� */
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
			if(ret)	 goto ErrExit;
			
			/* ������� */
			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , g_ln_parm.intst_days_type ,
				"2",g_ln_mst.bal,g_pub_tx.tx_amt1 , 
				&g_ln_mst.intst_acm ,"0",0,0 );
			if(ret) goto ErrExit;
			
			 /* �������� */
			 /* ���ڻ��� */
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.in_lo_intst ,g_pub_tx.tx_amt2, &g_ln_mst.in_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;		
			/* ������� */
			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.out_lo_intst ,g_pub_tx.tx_amt3, &g_ln_mst.out_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;
			/* �������� */
			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.cmpd_lo_intst ,g_pub_tx.tx_amt4, &g_ln_mst.cmpd_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;
			/* ����ֶλ����л��� */
			sect_acm=0.00;
			/***
			ret=sql_sum_double("com_sect_acm","Intst_acm",&scct_acm,"ac_id=%ld and ac_seqn=%d and sts='0' and intst_type='2'",g_ln_mst.ac_id ,g_ln_mst.ac_seqn);
			if(ret)	goto ErrExit;
			****/
			/*��ֶλ���
			ret=sql_execute("update com_sect_acm set sts='2' where ac_id=%ld and ac_seqn=%d and sts='0' and intst_type!='1'",g_ln_mst.ac_id ,g_ln_mst.ac_seqn);
			if(ret)	goto ErrExit;*/
			/* �����ļ����� ����㸴����������0����ע��*/
			g_ln_mst.in_lo_acm=0;
			g_ln_mst.out_lo_acm=0;
			g_ln_mst.cmpd_lo_acm=0;
			g_ln_mst.lst_date=g_pub_tx.tx_date;
			
			ret = Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
			if (ret)
			{
				 sprintf(acErrMsg,"���´������ļ�����!!");
				 WRITEMSG
				 goto ErrExit;
			}
		}       
   	}
	Ln_mst_Clo_Upd();
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��Ϣ���ڴ�������������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"AT12");/* ���޸� */
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��Ϣ���ڴ�������������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/**********************************************************************
* �� �� ����  get_lst_ic_date
* �������ܣ�  ȡ����������
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��11��03��
*
* ��    ����
*     ��  �룺   now_ic_date           	�������ļ��н�Ϣ����   
*                intst_type		�������ļ��н�Ϣ����  
*                lst_ic_date            �ϱʽ�Ϣ����
*          
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int get_lst_ic_date( long now_ic_date , char *intst_type , long *lst_free_date)
{
	int  yy=0;
	int  mm=0;
	int  dd=0;
	int  ret_tmp=0;
	long jx_date=0;
	char jx_limt_type[2];
	long jx_limt_val=0;
	char jx_limt_tmp[11];
	long lst_jx_day=0;
	long ic_date=0;
	long lst_ic_date=0;
		
	yy=now_ic_date/10000;
	mm=now_ic_date%10000/100;
	dd=now_ic_date%100;
			
	/* ���ڼ�� */
	/* ȡϵͳ��Ϣ���� */
	ret_tmp= pub_base_GetParm_int( "L0003" , 1 , &jx_date );
	if(ret_tmp) return 1;

	/* ȡ��Ϣ�������� */
	ret_tmp= pub_base_GetParm_Str( "L0004" , 1 , jx_limt_type );
	if(ret_tmp) return 1;	
	
	/* ȡ��Ϣ���� */
	ret_tmp= pub_base_GetParm_int( "L0005" , 1 , &jx_limt_val );
	if(ret_tmp) return 1;	
	
	/* ����� */  
	if( intst_type[0]=='5' )
	{
		if( mm==1 && dd<jx_date )/*�ϴν�Ϣ��ȥ��*/ 
		{
			lst_ic_date=(now_ic_date/10000-1)*10000+100+jx_date;
		}else{
			lst_ic_date=now_ic_date/10000*10000+100+jx_date;
		}
		sprintf(acErrMsg,"���Ľ�Ϣ����Ϊ  [%ld]",lst_ic_date);
		WRITEMSG						
	}else if( intst_type[0]=='4' ){	/*����*/
		if( now_ic_date%10000<300+jx_date)
		{
			lst_ic_date=(now_ic_date/10000-1)*10000+1200+jx_date;
		}else if( now_ic_date%10000<600+jx_date){
			lst_ic_date=now_ic_date/10000*10000+300+jx_date;
		}else if( now_ic_date%10000<900+jx_date){
			lst_ic_date=now_ic_date/10000*10000+600+jx_date;
		}else if( now_ic_date%10000<1200+jx_date){
			lst_ic_date=now_ic_date/10000*10000+900+jx_date;
		}else if( (now_ic_date%10000>=1200+jx_date) && (now_ic_date%10000<=1299)){
			lst_ic_date=now_ic_date/10000*10000+1200+jx_date;
		}else{
			sprintf(acErrMsg,"����Ľ�Ϣ���ڳ���!���������[%ld]ԭ����[%ld]",lst_ic_date,now_ic_date);
			WRITEMSG
			return 1;
		}
		sprintf(acErrMsg,"����Ľ�Ϣ����Ϊ  [%ld]",lst_ic_date);
		WRITEMSG
	}else if ( intst_type[0]=='3' ){	/*�½�*/
		if( mm==12&&dd>=21 )
		{
			lst_ic_date=now_ic_date/10000*10000+1200+jx_date;
		}else if( mm==1&&dd<21 ){	
			lst_ic_date=(now_ic_date/10000-1)*10000+1200+jx_date;
		}else if( dd<21 ){
			lst_ic_date=now_ic_date/10000*10000+(now_ic_date%10000/100-1)*100+jx_date;
		}else if( dd>=21 ){
			lst_ic_date=now_ic_date/100*100+jx_date;
		}else{
			sprintf(acErrMsg,"�½�Ľ�Ϣ���ڳ���!���������[%ld]ԭ����[%ld]",lst_ic_date,now_ic_date);
			WRITEMSG
			return 1;
		}
		sprintf(acErrMsg,"�½�Ľ�Ϣ����Ϊ [%ld]",lst_ic_date);
		WRITEMSG		
	}else{	/*���汾�塢���ռ�Ϣ������Ϣ ���迼�� */
		sprintf(acErrMsg,"���汾�塢���ռ�Ϣ������Ϣ ���迼��");
		WRITEMSG
	}	
	
	/* �������Ϣ���� */
	if( jx_limt_type[0]=='D' )	/* ��Ϣ������"��"Ϊ��λ */
	{
		ret_tmp=pub_base_deadlineD( lst_ic_date, jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}
	if( jx_limt_type[0]=='M' )	/* ��Ϣ������"��"Ϊ��λ */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}
	if( jx_limt_type[0]=='J' )	/* ��Ϣ������"��"Ϊ��λ */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, 3*jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}	
	if( jx_limt_type[0]=='Y' )	/* ��Ϣ������"��"Ϊ��λ */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, 12*jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}		
	sprintf(acErrMsg,"���Ϣ����Ϊ  [%ld]",lst_jx_day);
	WRITEMSG	
	*lst_free_date=lst_jx_day;
	sprintf(acErrMsg,"���Ϣ����Ϊ  [%ld]",*lst_free_date);
	WRITEMSG
	
	return 0;		
}
