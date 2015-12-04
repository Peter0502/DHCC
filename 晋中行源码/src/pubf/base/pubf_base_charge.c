#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_charge.c
* ����������   
*              pub_base_GetCharge   �����շѱ��ȡ���ý��
*              pub_base_GetRate     ���ݷ��ʱ�Ż�÷���
*
* ��    ��:    lance
* ������ڣ�   2003��12��27��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_def_rate_c.h"
#include "mo_def_agio_c.h"
#include "com_chrg_rate_c.h"

struct com_chrg_rate_c Gl_com_chrg_rate_c;

/**********************************************************************
* �� �� ����  pub_base_GetCharge
* �������ܣ�  �����շѱ��ȡ���ý��
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��12��27��
*
* ��    ����
*     ��  �룺cif_lvl      �ͻ�����
*             chrg_code    ���ñ��
*             amt          ���
*      
*     ��  ��: charge       ���ý��
*
*     ����ֵ�� 0 �ɹ�
*              1 ������
*             -1 ����
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_GetCharge(char *cif_lvl,char *chrg_code,double amt,double *charge){

	struct mo_def_rate_c	mo_def_rate_c;
	struct com_chrg_rate_c  com_chrg_rate_c;
	struct mo_def_agio_c	mo_def_agio_c;
	struct pub_tx pub_tx; 

	int    ret;
	double cif_lvl_rate;
	char   chrg_rate_no[4];
	int    beg_date;
	int    end_date;
	char   type[2];
    double val;
    double min_amt;
	double max_amt;
    int    date;
	char   chrg_rate_code[4];
	
    /* ���ݷֶη��ʱ�ȡ�ֶη��ʽṹ */
    ret = Mo_def_rate_Sel(g_pub_tx.reply, &mo_def_rate_c , \
			  "chrg_code='%s' \
			  and min_amt<=%lf and max_amt>=%lf" , chrg_code , amt ) ;

    if( ret==100 )
	{
		sprintf( acErrMsg,"�ֶη��ʱ��޼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�ֶη��ʱ��쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    /* ���ݿͻ�����ͽ��ױ���ȡ�Ż����� */
    ret = Mo_def_agio_Sel(g_pub_tx.reply, &mo_def_agio_c , "cif_lvl='%s' \
			  and tx_code='%s'" , cif_lvl , g_pub_tx.tx_code );
    if( ret==100 )
	{
		sprintf( acErrMsg,"�Żݶ�����޼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�Żݶ�����¼�쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    	
    	    
    /* ��ȡ���ʱ�źͿͻ��Żݱ��� */
	cif_lvl_rate = mo_def_agio_c.cif_lvl_rate;
    strcpy( chrg_rate_no , mo_def_rate_c.chrg_rate_no );
    
	/* ���ݷ��ʱ�Ų�ѯ�������ʱ�, ȡ����������(�������)������ֵ�Լ��շ�����
	   �����շ����޽�� */

    ret = Com_chrg_rate_Sel(g_pub_tx.reply, &com_chrg_rate_c ,
			 " chrg_rate_code='%s' " , chrg_rate_code ) ;

    if( ret==100 )
	{
		sprintf( acErrMsg,"�������ʱ��޼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�������ʱ��쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    
	/* ��ȡ��������(�������)������ֵ�Լ��շ����޽����շ����޽������� */

        strcpy( type , com_chrg_rate_c.type );
        beg_date = com_chrg_rate_c.beg_date;
		end_date = com_chrg_rate_c.end_date;
		val = com_chrg_rate_c.val;
    	min_amt = com_chrg_rate_c.min_amt;
        max_amt = com_chrg_rate_c.max_amt;

    /* �봫���ֵ���бȽ� */

    /* ���ڼ�� */  
    /* ����ȡϵͳ���ں���ȡ��ϵͳ���� */
	if ( date>end_date )
	{ 
		sprintf( acErrMsg,"���ڳ���������������� end_date=[%d]",end_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W001");
		return -1;
    }else if ( date<beg_date )
		{
    		sprintf( acErrMsg,"���ڵ��ڶ������������ beg_date=[%d]",beg_date);
			WRITEMSG
			strcpy( pub_tx.reply ,"W002");
			return -1;
		}

    /* ���������ж� */

    if ( type[0]==1 )  /* ��� */
	{   
		*charge=val*cif_lvl_rate;
	}else
	  {
		*charge=amt*val*cif_lvl_rate;
	  }
    
	/* ���ý���飬����Ƿ񳬹����ý��������� */

    if ( *charge > max_amt )
	{
		*charge=max_amt;
	}else if ( *charge < min_amt )
	  {
		*charge=min_amt;
	  }

 	return 0; 
}

/**********************************************************************
* �� �� ����  pub_base_GetRate
* �������ܣ�  ���ݷ��ʱ�Ż�÷���
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��12��27��
*
* ��    ����
*     ��  ��: chrg_rate_code ���ʱ��
*
*     ��  ��: val            ����
*
*     ����ֵ��0 �ɹ�
*             1 ������
*            -1 ����
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_GetRate(char *chrg_rate_code,long tx_date,double *val){
	
	struct com_chrg_rate_c	com_chrg_rate_c;
	int    ret;	

	if( !strcmp(Gl_com_chrg_rate_c.chrg_rate_code,chrg_rate_code) &&
		Gl_com_chrg_rate_c.beg_date<=tx_date &&
		Gl_com_chrg_rate_c.end_date>tx_date )
	{
		MEMCPY_DEBUG(&com_chrg_rate_c,&Gl_com_chrg_rate_c,sizeof(Gl_com_chrg_rate_c));
		ret=0;
	}
	else
    ret = Com_chrg_rate_Sel(g_pub_tx.reply, &com_chrg_rate_c ,
			 " chrg_rate_code='%s' and beg_date <= %ld and end_date > %ld " ,
			  chrg_rate_code, tx_date, tx_date);

    if( ret==100 )
	{
		if(! strcmp(SLBH, chrg_rate_code) && tx_date < 19991101){  /*����19991101֮ǰ����Ϣ˰, gujy*/
			Gl_com_chrg_rate_c.val = 0;
			*val = 0;
			return 0;
		}
		sprintf( acErrMsg,"�������ʱ��޼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
	{
    	sprintf( acErrMsg,"ȡ�������ʱ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
		MEMCPY_DEBUG( &Gl_com_chrg_rate_c,&com_chrg_rate_c,sizeof(Gl_com_chrg_rate_c));
    
	/* ��ȡ��������(�������)������ֵ�Լ��շ����޽����շ����޽������� */


		*val = com_chrg_rate_c.val;

    /* ���ڼ�� *  
    * ����ȡϵͳ���ں���ȡ��ϵͳ���� *
	if ( g_pub_tx.tx_date>end_date )
	{ 
		sprintf( acErrMsg,"���ڳ���������������� end_date=[%d]",end_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W001");
		return -1;
    }
	else if ( g_pub_tx.tx_date<beg_date )
	{
    	sprintf( acErrMsg,"���ڵ��ڶ������������ beg_date=[%d]",beg_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W002");
		return -1;
	} */
    
	return 0;
}

