#include "find_debug.h" 
/*************************************************
* �� �� ��:  pubf_base_mdm_draw.c
* ���������� ����������֧ȡ��ʽ�Ƿ�Ϸ�
* ��    ��:  
* ������ڣ� 2003/01/06
* �޸ļ�¼��  
* 	 ��   ��:
*    �� �� ��:
*    �޸�����:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

/*************************************************
* �� �� ����  pub_base_mdm_draw
* ����������  ����������֧ȡ��ʽ�Ƿ�Ϸ�
* ��    �ߣ�  xxx
* ���ʱ�䣺  2003��01��06��
*
* ��    ����
*     ��  �룺
*     ��  ��:  g_pub_tx�ṹ
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_mdm_draw(struct mdm_attr_c mdm_attr_c,struct mdm_ac_rel_c mdm_ac_rel_c)
{
	/**��ѯ����**/
   if( mdm_attr_c.qry_pwd_yn[0]=='N' && g_pub_tx.qry_pwd_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"�ý��ʲ������ѯ����!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P213");
	   return 1;
   }
   if( g_pub_tx.qry_pwd_yn[0]=='Y' )
   {
		pub_base_EnDes( g_pub_tx.tx_date,mdm_ac_rel_c.ac_no,
			mdm_ac_rel_c.qry_pwd );
   }

	/**����֧ȡ**/
   if( mdm_attr_c.draw_pwd_yn[0]=='N' && mdm_ac_rel_c.draw_pwd_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"�ý��ʲ���������֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P137");
	   return 1;
   }
   if( mdm_attr_c.draw_pwd_yn[0]=='Y' && mdm_ac_rel_c.draw_pwd_yn[0]=='N' )
   {
       sprintf(acErrMsg,"�ý��ʱ�������֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P242");
	   return 1;
   }
   if( mdm_ac_rel_c.draw_pwd_yn[0]=='Y' )
   {
		pub_base_EnDes( g_pub_tx.tx_date,mdm_ac_rel_c.ac_no,
			mdm_ac_rel_c.draw_pwd );
   }

	/***֤��֧ȡ***/
   if( mdm_attr_c.draw_id_yn[0]=='N' && mdm_ac_rel_c.draw_id_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"�ý��ʲ�����֤��֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P137");
	   return 1;
   }
   if( mdm_attr_c.draw_id_yn[0]=='Y' && mdm_ac_rel_c.draw_id_yn[0]=='N' )
   {
       sprintf(acErrMsg,"�ý��ʱ���֤��֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P243");
	   return 1;
   }

	/***ӡ��֧ȡ***/
   if( mdm_attr_c.draw_seal_yn[0]=='N' && mdm_ac_rel_c.draw_seal_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"�ý��ʲ�����ӡ��֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P138");
	   return 1;
   }

	/**
   if( mdm_attr_c.draw_uncon_yn[0]=='N' && mdm_ac_rel_c.draw_uncon_yn[0]=='Y')
   {
       sprintf(acErrMsg,"�ý��ʲ���������֧ȡ!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P136");
	   return 1;
   }
	**/

	if( mdm_ac_rel_c.draw_seal_yn[0]=='N' && 
		mdm_ac_rel_c.draw_id_yn[0]=='N' && 
		mdm_ac_rel_c.draw_pwd_yn[0]=='N')	
   	{
		mdm_ac_rel_c.draw_uncon_yn[0]=='Y';
		/**
       sprintf(acErrMsg,"֧ȡ��ʽ����Ϊ��!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P141");
	   return 1;
	   **/
   	}	

	return 0;
}

