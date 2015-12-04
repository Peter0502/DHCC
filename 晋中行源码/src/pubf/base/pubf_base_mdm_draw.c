#include "find_debug.h" 
/*************************************************
* 文 件 名:  pubf_base_mdm_draw.c
* 功能描述： 检查介质属性支取方式是否合法
* 作    者:  
* 完成日期： 2003/01/06
* 修改记录：  
* 	 日   期:
*    修 改 人:
*    修改内容:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

/*************************************************
* 函 数 名：  pub_base_mdm_draw
* 功能描述：  检查介质属性支取方式是否合法
* 作    者：  xxx
* 完成时间：  2003年01月06日
*
* 参    数：
*     输  入：
*     输  出:  g_pub_tx结构
*     返回值： 0 成功
*            非0 失败
* 修改历史：
*                   
********************************************************************/
int pub_base_mdm_draw(struct mdm_attr_c mdm_attr_c,struct mdm_ac_rel_c mdm_ac_rel_c)
{
	/**查询密码**/
   if( mdm_attr_c.qry_pwd_yn[0]=='N' && g_pub_tx.qry_pwd_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"该介质不允许查询密码!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P213");
	   return 1;
   }
   if( g_pub_tx.qry_pwd_yn[0]=='Y' )
   {
		pub_base_EnDes( g_pub_tx.tx_date,mdm_ac_rel_c.ac_no,
			mdm_ac_rel_c.qry_pwd );
   }

	/**密码支取**/
   if( mdm_attr_c.draw_pwd_yn[0]=='N' && mdm_ac_rel_c.draw_pwd_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"该介质不允许密码支取!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P137");
	   return 1;
   }
   if( mdm_attr_c.draw_pwd_yn[0]=='Y' && mdm_ac_rel_c.draw_pwd_yn[0]=='N' )
   {
       sprintf(acErrMsg,"该介质必须密码支取!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P242");
	   return 1;
   }
   if( mdm_ac_rel_c.draw_pwd_yn[0]=='Y' )
   {
		pub_base_EnDes( g_pub_tx.tx_date,mdm_ac_rel_c.ac_no,
			mdm_ac_rel_c.draw_pwd );
   }

	/***证件支取***/
   if( mdm_attr_c.draw_id_yn[0]=='N' && mdm_ac_rel_c.draw_id_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"该介质不允许证件支取!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P137");
	   return 1;
   }
   if( mdm_attr_c.draw_id_yn[0]=='Y' && mdm_ac_rel_c.draw_id_yn[0]=='N' )
   {
       sprintf(acErrMsg,"该介质必须证件支取!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P243");
	   return 1;
   }

	/***印鉴支取***/
   if( mdm_attr_c.draw_seal_yn[0]=='N' && mdm_ac_rel_c.draw_seal_yn[0]=='Y' )
   {
       sprintf(acErrMsg,"该介质不允许印鉴支取!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P138");
	   return 1;
   }

	/**
   if( mdm_attr_c.draw_uncon_yn[0]=='N' && mdm_ac_rel_c.draw_uncon_yn[0]=='Y')
   {
       sprintf(acErrMsg,"该介质不允许任意支取!");
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
       sprintf(acErrMsg,"支取方式不能为空!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P141");
	   return 1;
	   **/
   	}	

	return 0;
}

