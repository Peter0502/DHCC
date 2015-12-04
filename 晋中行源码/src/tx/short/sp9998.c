/*************************************************
* 文 件 名:  sp9998.c
* 功能描述： 根据柜员号得到机构和日期信息
*
* 作    者:  Seagle
* 完成日期： 
*
* 修改记录：                
* 日   期:   20060612
* 修 改 人:      
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_tx_list_c.h"
#include "com_sys_parm_c.h"
#include "com_parm_c.h"

int sp9998()
{
	char fiedlst[100];
	char filter[100];
	char filename[100];
	struct com_tel_c vcom_tel,pcom_tel;
	struct com_branch_c vcom_branch;
	struct com_sys_parm_c vcom_sys_parm;
	struct auth_tx_list_c vauth_tx_list;
	struct com_parm_c sCom_parm;
	FILE *fp;
	int ret=0, i=0, j=0;
	char wherelist[100], tmpstr[50];
	char old_menu[5], new_menu[5];
 	vtcp_log("9998开始！[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();
 	vtcp_log("9998init over[%s][%d]",__FILE__,__LINE__);

	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(old_menu, 0x0, sizeof(old_menu));
	memset(new_menu, 0x0, sizeof(new_menu));

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&vcom_sys_parm," 1=1" );
	if( ret ) goto ErrExit;

	WRITEMSG

TRACE
	ret = Com_tel_Sel( g_pub_tx.reply,&vcom_tel,
		" tel='%s' ",g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='4' )
	{
		sprintf( acErrMsg,"柜员状态错[%s]O[%s]",g_pub_tx.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O062" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='3' )
	{
		sprintf( acErrMsg,"柜员状态错[%s]i[%s]",g_pub_tx.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O063" );
		goto ErrExit;
	}
	/* 比较当前日期和上次改密码时间的间隔 */
	struct com_parm_c com_parm_v;
	int pwd_days=0;
	memset(&com_parm_v,'\0',sizeof(com_parm_v));
	pwd_days=pub_base_CalTrueDays(vcom_tel.pwd_date,vcom_sys_parm.sys_date);
	if(ret=Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='HMMTS' and parm_seqn='1'" )){
		sprintf(acErrMsg, "查换密码天数出错![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S088");
		goto ErrExit;
	}
	if(pwd_days>atoi(com_parm_v.val))
		set_zd_long("0310",vcom_tel.pwd_date);/* 上次改密码的时间 */

	set_zd_data("0020",vcom_tel.br_no );
	set_zd_data("0030",vcom_tel.br_no );
	set_zd_data("0130",vcom_tel.tx_type );
	set_zd_data("0680",vcom_tel.lvl );
	set_zd_long("0470",vcom_sys_parm.sys_date );

	if ( atol(vcom_tel.tx_type) == 0 )
	{
		sprintf(acErrMsg, "该柜员没有权限登陆系统,请和科技处联系");
		WRITEMSG
		strcpy(g_pub_tx.reply, "S081");
		goto ErrExit;
	}
	ret = Com_branch_Sel( g_pub_tx.reply,&vcom_branch,
		" br_no='%s' ",vcom_tel.br_no );
	if( ret ) goto ErrExit;

	set_zd_data("0250",vcom_branch.br_name ); /*网点名称*/


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
