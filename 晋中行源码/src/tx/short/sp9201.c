/*************************************************
* 文 件 名:  sp9201.c
* 功能描述： 判断柜员号
*
* 作    者:  wyb
* 完成日期： 2006年12月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "kajiaojie_c.h"

int sp9201()
{
	int ret=0;
	char tel[5];
  char tels[5];
  char m[2],n[2];
  struct kajiaojie_c g_kajiaojie;
	struct com_tel_c g_com_tel;
  struct com_tel_c g_com_tels;
  memset( &g_kajiaojie, 0x00, sizeof( struct kajiaojie_c ) );
	memset( &g_com_tel, 0x00, sizeof( struct com_tel_c ) );
	memset( &g_com_tels, 0x00, sizeof( struct com_tel_c ) );
pub_base_sysinit();
	get_zd_data( "0920", tel );					/* 柜员号 */
  get_zd_data("0910",tels);
  get_zd_data("0690",m);
  get_zd_data("0680",n);
  vtcp_log("wyb----[%s][%s][%s][%s]",tel,tels,m,n);
	ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",tel);
	if( ret )
	{
		sprintf(acErrMsg,"不存在该柜员!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}
	if(strcmp(g_com_tel.br_no,g_pub_tx.tx_br_no))
		{
			sprintf(acErrMsg,"该柜员不是本机构柜员!!tel=[%s]",tel);
			WRITEMSG
		strcpy(g_pub_tx.reply,"S009");
		goto ErrExit;
			}
ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tels,"tel='%s'",tels);
	if( ret )
	{
		sprintf(acErrMsg,"不存在该柜员!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}
	if(strcmp(g_com_tel.br_no,g_pub_tx.tx_br_no))
		{
			sprintf(acErrMsg,"该柜员不是本机构柜员!!tel=[%s]",tel);
			WRITEMSG
		strcpy(g_pub_tx.reply,"S009");
		goto ErrExit;
			}
get_zd_data("0690",g_com_tel.csts);
get_zd_data("0680",g_com_tels.csts);
vtcp_log("aa[%s][%s]",g_com_tel.csts,g_com_tels.csts);
ret=Com_tel_Upd_Upd(g_com_tel,g_pub_tx.reply);
if( ret )
	{
		sprintf(acErrMsg,"修改错误!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
		sprintf(acErrMsg,"修改错误!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
ret=Com_tel_Upd_Upd(g_com_tels,g_pub_tx.reply);
if( ret )
	{
		sprintf(acErrMsg,"修改错误!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	strcpy(g_kajiaojie.tel,tel);
	strcpy(g_kajiaojie.tels,tels);
get_zd_data("0690",g_kajiaojie.csts);
get_zd_data("0680",g_kajiaojie.cst);
ret=Kajiaojie_Ins(g_kajiaojie,g_pub_tx.reply);
if(ret)
	{sprintf(acErrMsg,"插入错误!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D107");
		goto ErrExit;
		}
vtcp_log("wyb[%s][%s][%s][%s]",g_kajiaojie.tel,g_kajiaojie.tels,g_kajiaojie.csts,g_kajiaojie.cst);
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
