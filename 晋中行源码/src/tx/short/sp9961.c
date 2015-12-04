/*************************************************
* 文 件 名:  sp9961.c
* 功能描述： 输入帐号，回显证件信息
*
* 作    者:  mike
* 完成日期： 2003年07月05日
*
* 修改记录： 
*   日   期:20130326
*   修 改 人:
*   修改内容:使卡号前六位和ICCARDHEADCODE或者CARDHEADCODE，但是67域传来为空时挂失状态不报错，
使卡号前六位和ICCARDHEADCODE或者CARDHEADCODE，但是67域传来不为空时挂失状态报错
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "card.h"

int sp9961()
{
	int ret=0;
	char name[51],flag[2];

	struct mdm_attr_c	g_mdm_attr;

	memset( &g_mdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &g_mdm_attr, 0x00, sizeof( struct mdm_attr_c ) );

	get_zd_data( "0300", g_pub_tx.ac_no );		/* 帐号 */
	pub_base_old_acno( g_pub_tx.ac_no );        /**对旧帐号的处理**/
	
	get_zd_data("0670",flag);
	vtcp_log("[%s][%d]  0670[%s]\n",__FILE__,__LINE__,flag);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/*** 按财务部要求挂失状态都不卡 20130710 武大为
	if(( memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0  && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 ) || strlen(flag))
	{
	if( g_mdm_ac_rel.note_sts[0]=='1' )
			{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
			}
	}
	else
	***/
  if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已被换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已部提换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已做销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** 卡类不允许增加证件支取 ***/
	/**modified by ligl2008 2006-12-18 17:42 卡也需要证件支取**/
	/*if( g_mdm_attr.mdm_type[0]=='3' || g_mdm_attr.mdm_type[0]=='4' || \ 
		g_mdm_attr.mdm_type[0]=='5' )
	{
		sprintf(acErrMsg,"卡类账户不允许增加证件支取!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D227");
		goto ErrExit;
	}*/

	ret = pub_base_getacname( g_mdm_ac_rel.ac_id, name );
	if( ret )
	{
		sprintf(acErrMsg,"取户名函数错!");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("sp9961 户名！[%s][%d][%s]ret=[%d]id_no=[%s]",__FILE__,__LINE__,name,ret,g_mdm_ac_rel.id_no);

	set_zd_data( "0300", g_pub_tx.ac_no );				/* 账号 */
	set_zd_data( "0250", name );						/* 户名 */
	set_zd_data( "0680", g_mdm_ac_rel.draw_id_yn );		/* 证件支取 */
	set_zd_data( "0670", g_mdm_ac_rel.id_type );		/* 证件类型 */
    set_zd_data( "0620", g_mdm_ac_rel.id_no );          /*证件号*/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
