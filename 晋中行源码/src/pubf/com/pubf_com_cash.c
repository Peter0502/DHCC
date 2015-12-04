#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_com_cash.c
* ����������    ��Ա�ֽ����������
*              1��
*            
* ��    ��:    jack
* ������ڣ�    2003��1��12��
*
* �޸ļ�¼��    
* 1. ��    ��: 	
*    �� �� ��: 	
*    �޸�����: 	
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

/**********************************************************************
* ��������    	pub_com_ChkTelCash
* �������ܣ�    	���ݻ����š���Ա�����ּ��ù�Ա��Ǯ�䣨�ֽ��ʺţ��Ƿ���ڣ�
*		״̬�Ƿ�����
* ����/ʱ�䣺    jack//2003��1��12��
* 
* ������
*     ���룺 
*	    	char 	*br_no	��������
*		char	*tel	��Ա����
*		char	*cur_no	����
*           
*     ����� 
*		g_pub_tx.reply	��Ӧ��
* �� �� ֵ: 0 �ɹ�,-1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/

int pub_com_ChkTelCash( char *br_no, char *tel, char *cur_no )
{    
	struct cash_mst_c sCashMst;
	
	int	ret;
 	
 	MEMSET_DEBUG( &sCashMst, 0x00, sizeof( struct cash_mst_c ) );
 	
 	ret = Cash_mst_Sel( g_pub_tx.reply, &sCashMst, "br_no = '%s' \
 			and tel = '%s' and cur_no = '%s'", br_no, tel, cur_no );
 	if ( ret == 100 )
 	{
 		sprintf( acErrMsg, "�����ڹ�ԱǮ�䣬�������ֽ���" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P154" );
 		return(-1);
 	}else if ( ret )
 	{
 		sprintf( acErrMsg, "���ҹ�ԱǮ���ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);
 	}
 	
 	if ( sCashMst.sts[0] != 'Y' )
 	{
 		sprintf( acErrMsg, "�ֽ��˺�δ���û�״̬������" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P150" );
 		return(-1);
 	}
 	
	return 0; 
}

/**********************************************************************
* ��������    	pub_com_RegCashMst
* �������ܣ�    	�Ǽ�/�޸Ĺ�ԱǮ�䣨�ֽ��ʺţ�,����޸ĺ��ʻ����С��0�����
*		��������룬����Ϸ���������������ȥ���������ж��ʻ�
*		����Ƿ�С��0��
* ����/ʱ�䣺    jack//2003��1��12��
* 
* ������
*     ���룺 
*	    	struct cash_mst_c *cash_mst
*			br_no	��������
*			tel	��Ա����
*			cur_no	����	
*		double	amt		 ������ 
*
*     ����� 
*		struct cash_mst_c *cash_mst
*		g_pub_tx.reply	��Ӧ��
* �� �� ֵ: 0 �ɹ�,-1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/

int pub_com_RegCashMst(
		struct cash_mst_c *cash_mst, 
		double amt )
{
	int	ret;
 	
 	ret = Cash_mst_Dec_Upd ( g_pub_tx.reply, "br_no = '%s' and tel = '%s' \
 				and cur_no = '%s'", cash_mst->br_no, \
 				cash_mst->tel, cash_mst->cur_no );
 	if ( ret )
 	{
 		sprintf( acErrMsg, "ִ��Cash_mst_Dec_Sel��ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);
 	}

	ret = Cash_mst_Fet_Upd( cash_mst );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�����ڹ�ԱǮ�䣬�������ֽ���" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P154" );
		return(-1);
	}else if ( ret )
	{
 		sprintf( acErrMsg, "ִ��Cash_mst_Fet_Sel��ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);		
	}

 	/* �ж����ջ��Ǹ��������ʻ���� */
 	if ( g_pub_tx.add_ind[0] == '1' )
 	{
 		cash_mst->bal += amt;
 	}else if ( g_pub_tx.add_ind[0] == '2' )
 	{
 		cash_mst->bal -= amt;
 	}
 	
 	/* �Ƚ�����Ƿ�С��0�����С������ʧ����������ʻ���� */
 	if ( pub_base_CompDblVal( cash_mst->bal , 0.00 ) < 0 )
 	{
 		sprintf( acErrMsg, "��Ա�ֽ��㣬��������ķ�����" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P152" );
 		return(-1);
 	}
	 	
 	/* �޸��ֽ�̨�� */
 	ret = Cash_mst_Upd_Upd( *cash_mst, g_pub_tx.reply );
 	if ( ret )
	{
		sprintf( acErrMsg, "Ǯ�����ļ��쳣���������Ļ�����ϵ" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P153" );
		return(-1);	
	}
	
	Cash_mst_Clo_Upd();
	
	return 0; 
}

/**********************************************************************
* ��������    	pub_com_RegCashHst
* �������ܣ�    	�Ǽǹ�Ա�ֽ�̨����ϸ
*
* ����/ʱ�䣺    jack//2003��1��12��
* 
* ������
*     ���룺 
*	    	struct cash_mst_c cash_mst
*			br_no	��������
*			tel	��Ա����
*			cur_no	����	
*
*     ����� 
*		g_pub_tx.reply	��Ӧ��
* �� �� ֵ: 0 �ɹ�,-1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/

int pub_com_RegCashHst(	struct cash_mst_c cash_mst )
{
	struct cash_mst_hst_c sCashHst;
	int	ret;
 	
 	MEMSET_DEBUG( &sCashHst, 0x00, sizeof( struct cash_mst_hst_c ) );
 	
 	sCashHst.trace_no = g_pub_tx.trace_no;	/*������ˮ��*/
	sCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*������ˮ���*/
	strcpy( sCashHst.br_no , cash_mst.br_no );	/*����*/
	strcpy( sCashHst.ac_no , cash_mst.ac_no );	/*�˺�*/
	strcpy( sCashHst.tel, cash_mst.tel );		/*��Ա*/
	strcpy( sCashHst.cur_no , cash_mst.cur_no );	/*����*/
	sprintf( sCashHst.add_ind , g_pub_tx.add_ind ); 	/*�ո���־*/
	sCashHst.amt =	g_pub_tx.tx_amt1;		/*������*/
	sCashHst.tx_date = g_pub_tx.tx_date;		/*��ˮ����*/
	sCashHst.tx_time = g_pub_tx.tx_time;		/*����ʱ��*/
	strcpy( sCashHst.brf ,	g_pub_tx.brf );		/*ժҪ*/
	strcpy( sCashHst.cash_code , g_pub_tx.cash_code );	/*�ֽ����*/

 	ret = Cash_mst_hst_Ins( sCashHst, g_pub_tx.reply );
 	if ( ret )
 	{
                sprintf( acErrMsg, "�Ǽ�Ǯ����ϸ����![%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "P155" );
                return(-1);
 	}

	return 0; 
}
