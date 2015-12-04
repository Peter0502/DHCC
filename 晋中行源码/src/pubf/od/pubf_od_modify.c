/*************************************************
* �� �� ��: pubf_od_modify.c
* ����������   
*              pub_od_change      ͸֧��Ʒ���
*
* ��    ��:    pwoer
* ������ڣ�   2003��12��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

/**********************************************************************
* �� �� ����  pub_od_change
* �������ܣ�  ͸֧��Ʒ��������˻���Ӧ�ò�Ʒ��Ÿı�
* ��    �ߣ�  pwoer
* ���ʱ�䣺  2003��12��27��
*
* ��    ����
*     ��  �룺ac_id     integer   �˻�ID   
*             ac_seqn   smailint  �˻����
*             prdt_no   char(3)   �²�Ʒ���
*      
*     ��  ��:           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_od_change( int ac_id , int ac_seqn , char *prdt_no )
{

    struct prdt_ac_id_c prdt_ac_id;

	int ret;
    char tx_type[2];
    char prdt_no_tmp[4];

    memset( &prdt_ac_id , 0*00 , sizeof ( struct prdt_ac_id_c ) );
	memset( &g_od_mst, 0x00, sizeof ( struct od_mst_c ) );

	/* ȡ������Ϣ */
	ret = Od_mst_Sel(g_pub_tx.reply, &g_od_mst , "ac_id=%ld and ac_seqn=%d" ,
						 ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"͸֧���ļ������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T002" );
		return 1;
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ͸֧���ļ��쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    	}
    	
    strcpy( prdt_no_tmp , g_od_mst.prdt_no );
    	memset ( &g_od_mst.prdt_no , 0 , strlen( g_od_mst.prdt_no ) );
    	 
    /* ����͸֧������麯�� */
    tx_type[0]='1';
    strcpy( g_od_mst.prdt_no , prdt_no );
    pub_od_check(&tx_type[0]);
    
   	memset ( &g_od_mst.prdt_no , 0 , strlen( g_od_mst.prdt_no ) );
    strcpy( g_od_mst.prdt_no , prdt_no_tmp );
        
    /* ����od_mst���в�Ʒ���� */

    ret = Od_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d" ,
						 ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"͸֧���ļ������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T002" );
		return 1;
	} else if ( ret !=0 )
		{
			sprintf( acErrMsg,"ȡ͸֧���ļ��������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
		}

    ret = Od_mst_Fet_Upd ( &g_od_mst, g_pub_tx.reply);
	if ( ret == 100 ) 
	{
		sprintf( acErrMsg,"FETCH od_mst ���� erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
    } else if ( ret != 0 )
		{
			sprintf( acErrMsg,"ִ��od_mstʱ���� erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
    	}

   	memset ( &g_od_mst.prdt_no , 0 , strlen( g_od_mst.prdt_no ) );
	strcpy( g_od_mst.prdt_no , prdt_no );

    ret = Od_mst_Upd_Upd( g_od_mst, g_pub_tx.reply);
    if ( ret == 239 )
	{
		sprintf( acErrMsg,"ִ��Od_mst_Upd_Upd�� code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D106" );
		return -1;
	} else if ( ret != 0 )
		{
			sprintf( acErrMsg,"����Od_mstʱ���� erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D105" );
			return -1;
		}

	/* ����prdt_ac_id���в�Ʒ���� */
    ret = Prdt_ac_id_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d" ,
						   ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"��Ʒ�˺Ŷ��ձ����޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"W012" );
		return 1;
	} else if ( ret !=0 )
		{
			sprintf( acErrMsg,"ȡ��Ʒ�˺Ŷ��ձ������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
		}
	
    ret = Prdt_ac_id_Fet_Upd ( &prdt_ac_id, g_pub_tx.reply);
	if ( ret == 100 ) 
	{
		sprintf( acErrMsg,"FETCH prdt_ac_id ���� erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
    } else if ( ret != 0 )
		{
			sprintf( acErrMsg,"ִ��prdt_ac_idʱ���� erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
    	}
    	
    memset ( &prdt_ac_id.prdt_no , 0 , strlen( prdt_ac_id.prdt_no ) );
	strcpy( prdt_ac_id.prdt_no , prdt_no );
	
    ret = Prdt_ac_id_Upd_Upd( &prdt_ac_id, g_pub_tx.reply);
    if ( ret == 239 )
	{
		sprintf( acErrMsg,"ִ��Prdt_ac_id_Upd_Upd�� code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D106" );
		return -1;
	} else if ( ret != 0 )
		{
			sprintf( acErrMsg,"����prdt_ac_idʱ���� erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D105" );
			return -1;
		}

    return 0;
}
