/*************************************************
* 文 件 名:  spE505.c
* 功能描述： 修改口令交易
*
* 作    者:  andy
* 完成日期： 2004年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"

int spE505()
{
	int	ret;
	char ac_no[20];
  char draw_pwd[7];
  char qry_pwd[7];
	struct	com_tel_c		g_com_tel;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(ac_no,0,sizeof(ac_no));
  memset(draw_pwd,0,sizeof(draw_pwd));
  memset(qry_pwd,0,sizeof(qry_pwd));
	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_data("0790",draw_pwd);			/*原密码*/
	get_zd_data("0800",g_pub_tx.qry_pwd);			/*新密码*/
	
 /*vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,draw_pwd);
 vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,g_pub_tx.qry_pwd);*/
	if(!strcmp(draw_pwd,g_pub_tx.qry_pwd))
	{
		sprintf( acErrMsg, "新旧密码不能相同!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O159");
		goto ErrExit;
	}

	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "更改柜员口令出错!!]" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O052");
		goto ErrExit;
	}

	
pub_base_EnDes(g_pub_tx.tx_date,"",draw_pwd);
	 
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.qry_pwd);	
	/*if( pub_base_DesChk(g_pub_tx.tx_date," ",draw_pwd,g_com_tel.pwd) )
	{*/
	if( strcmp(draw_pwd,g_com_tel.pwd) )
	{
		sprintf( acErrMsg, "原口令输入不正确!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}
	/*密码加密*/

	/*修改柜员密码*/
	g_reply_int=Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'", g_pub_tx.tel);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Com_tel_Fet_Upd(&g_com_tel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(g_com_tel.pwd,g_pub_tx.qry_pwd);
	g_com_tel.pwd_date=g_pub_tx.tx_date;

	g_reply_int=Com_tel_Upd_Upd(g_com_tel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"修改记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Com_tel_Clo_Upd( );

	set_zd_data("1028",g_pub_tx.qry_pwd);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
