#include "find_debug.h" 
/*************************************************
* �� �� ��:  pubf_base_sysinit.c
* ���������� ��ʼ�� pub_tx �ṹ
* ��    ��:  jack
* ������ڣ� 2003/01/05
* �޸ļ�¼��  
* 1. ��   ��:
*    �� �� ��:
*    �޸�����:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

/**********************************************************************
* �� �� ����  pub_base_sysinit
* �������ܣ�  ��ʼ�� pub_tx �ṹ
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��05��
*
* ��    ����
*     ��  �룺
*      
*     ��  ��:  g_pub_tx�ṹ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_sysinit()
{
	MEMSET_DEBUG ( &g_pub_tx , 0x00 , sizeof(struct pub_tx) );
	/**--- xxx --- 20030918 ---- ������������ֵ���Զ�����ˮ ---**/
	MEMSET_DEBUG ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
   /*���º������� �� ��: pubf_base_data.c*/
	get_zd_data( DC_OPN_BR_NO , g_pub_tx.opn_br_no ); /* ������������ */
	get_zd_data( DC_TX_BR_NO , g_pub_tx.tx_br_no );   /* ���׻��� */
	get_zd_long( DC_TRACE_NO , &g_pub_tx.trace_no);   /* ��ˮ�� */
	get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);   /* ��ˮ�ʴ� */
	get_zd_long( DC_DATE , &g_pub_tx.tx_date);        /* �������� */
	get_zd_long( DC_TIME , &g_pub_tx.tx_time);        /* ����ʱ�� */
	get_zd_data( DC_TEL , g_pub_tx.tel );             /* ����Ա */
	get_zd_data( DC_CHK , g_pub_tx.chk );             /* ����Ա */	
	get_zd_data( DC_AUTH , g_pub_tx.auth );           /* ��Ȩ�� */
	get_zd_data( DC_TTY , g_pub_tx.tty );             /* ǰ̨�ն˺� */	
	get_zd_data( DC_FILE_SND , g_pub_tx.file_snd );   /* �������ͱ�־ */	
	get_zd_data( DC_REPLY , g_pub_tx.reply );         /* ��Ӧ�� */				
	get_zd_long( DC_AUTH_CODE , &g_pub_tx.auth_code); /* ��Ȩ�� */		
	get_zd_data( DC_CODE , g_pub_tx.dc_code );        /* ��ƴ��� */
	get_zd_data( DC_TX_CODE , g_pub_tx.tx_code );     /* ���״��� */	
	get_zd_data( DC_SUB_TX_CODE , g_pub_tx.sub_tx_code );     /* ���״��� */	

	strcpy(g_pub_tx.reply,"0000");
	
	vtcp_log("[%s][%d]init_completed!",__FILE__,__LINE__ );	
	return 0;
}
