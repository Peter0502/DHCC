#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_cur_no.c
* ����������    ���ּ���ļ�
*              1��
*            
* ��    ��:    Terry
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
#include "com_cur_no_code_c.h"

/**********************************************************************
* ��������    	pub_base_ChkCurno
* �������ܣ�    	���ݱ��ּ����Ƿ���ڣ��Ƿ�����
* ����/ʱ�䣺    Terry//2003��1��12��
* 
* ������
*     ���룺 
*	    	char 	*cur_no  	���ֺ���
*           
*     ����� 
*		g_pub_tx.reply		��Ӧ��
* �� �� ֵ: 0 �ɹ�,-1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/

int pub_base_ChkCurno( char * cur_no )
{    
	struct com_cur_no_code_c sComCur;
	
	int	ret;
 	
 	MEMSET_DEBUG( &sComCur, 0x00, sizeof( struct com_cur_no_code_c ) );
 	
 	ret = Com_cur_no_code_Sel( g_pub_tx.reply, &sComCur, \
 					"cur_no = '%s'", cur_no );
 	if ( ret == 100 )
 	{
 		sprintf( acErrMsg, "�����ڴ˱���cur_no=[%s]", cur_no );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "O002" );
 		return(-1);
 	}else if ( ret )
 	{
 		sprintf( acErrMsg, "���ұ��ֱ����ret=[%d]", ret );
 		WRITEMSG
 		return(-1); 		
 	}
 	
 	if ( sComCur.use_ind[0] == 'N' )
 	{
 		sprintf( acErrMsg, "�˱��ִ���ͣ��״̬[%s]", cur_no );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "O003" );
 		return(-1); 		
 	}
 	
	return 0; 
}
