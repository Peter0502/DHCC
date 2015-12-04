/*************************************************
* �� �� ��: pubf_od_check.c
* ����������   
*              pub_od_check     ͸֧��Ʒ���Լ��
*
* ��    ��:    power
* ������ڣ�   2003��12��30��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"

/**********************************************************************
* �� �� ����  pub_od_check
* �������ܣ�  ͸֧��Ʒ���Լ��,���̨�������Ƿ�Ϻ���Ʒ���������Ҫ��
* ��    �ߣ�  power
* ���ʱ�䣺  2003��12��30��
*
* ��    ���� 
*     ��  �룺tx_type   char(1)   ��������(1-���� 2-�ֽ�͸֧ 3-͸֧ 4-����)    
*      
*     ��  ��:           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_od_check(char *tx_type)
{
	struct mo_infrm_c mo_infrm_c;

	int ret = 0;
	long min_day , max_day ;
	double min_rate , max_rate ;
	long infrm_date;

	memset( &g_od_parm,0x00,sizeof(struct od_parm_c) );

    /* ���ݲ�Ʒ�����͸֧��Ʒ�������鿴�Ƿ��и��ֲ�Ʒ */
	ret = Od_parm_Sel( g_pub_tx.reply , &g_od_parm, "prdt_no='%s'", 
					   g_od_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"͸֧��Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T001" );
		return 1;
	} 
	else if( ret !=0 )
	{
    	sprintf( acErrMsg,"ȡ͸֧��Ʒ�������쳣 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }

	switch( atoi(tx_type) )
	{
	  case 1:	/* ����ʱ��� */

		/* ��鿪�������Ƿ����������ں�ֹ������֮�� */	
		if ( (g_od_parm.beg_date > g_pub_tx.tx_date) || \
			(g_pub_tx.tx_date > g_od_parm.end_date))
		{	
			strcpy(acErrMsg,"�������ڲ������������ں�ֹ������֮��!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}
		
		/* ���ò�Ʒ��͸֧�޶��Ƿ񳬹�����޶������޶� */
		if ( g_od_mst.max_bal>g_od_parm.max_amt)
		{
			strcpy(acErrMsg,"�����͸֧��������޶");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T004" );
			goto ErrExit;
		}
		
		/* ��鿪�������������� */
			if ( g_td_parm.opn_lmt == 000 )
			{
				goto ErrExit;
			}
			break;
		
	  case 2:          /* ͸֧ʱ��� */
		
		/* ��齻�׽���Ƿ񳬹��ò�Ʒ�Ŀ���͸֧��� */
		if ( g_pub_tx.tx_amt1> ( g_od_mst.max_bal - g_od_mst.bal ) )
		{
			strcpy(acErrMsg,"���׽�");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T003" );
			goto ErrExit;
		}
		else if ( g_pub_tx.tx_amt1>g_od_parm.max_amt)
		{
			strcpy(acErrMsg,"͸֧��������޶");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T004" );
			goto ErrExit;
		}
		break;
		
	case 3:          /* �ֽ�͸֧ʱ��� */	
		/* ����Ƿ񳬹��ֽ�͸֧��� */
		if ( g_pub_tx.tx_amt1>g_od_parm.cash_amt )
		{
			strcpy(acErrMsg,"�ֽ�͸֧��������޶");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T005" );
			goto ErrExit;
		}
		break;
		
		
	case 4:          /* ����ʱ��� */	
		/* �����ͻ������ͻ������ */
		if ( g_pub_tx.tx_amt1<g_od_parm.min_pay_amt )
		{
			strcpy(acErrMsg,"͸֧������͹���ͻ����");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T006" );
			goto ErrExit;
		}else if( g_pub_tx.tx_amt1 > g_od_parm.min_pay_ratio * g_od_mst.bal )
		  {
		  	strcpy(acErrMsg,"͸֧������͹���ͻ����");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T006" );
			goto ErrExit;
		  }
		break;
		
	case 5:          /* ����ʱ��� */
		/* ������������������� (���ú���) */
		if ( g_dd_parm.cls_lmt == 000 )
		{
			goto ErrExit;
		}		
		break;
		
	default: /* ����������ڲ�������  */
	   
		strcpy( acErrMsg,"����������ڲ�������" );
		WRITEMSG
		goto ErrExit;

	}
 return 0;
 
/* ������� */
ErrExit:

		return (-1);

}
