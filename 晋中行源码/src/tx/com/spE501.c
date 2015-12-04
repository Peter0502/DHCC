/*************************************************
* 文 件 名:  spE501.c
* 功能描述： 网点开工交易
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
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "com_sys_parm_c.h"

int spE501()
{
	int	ret;
	FILE	*fp;
	char	filename[51]; 
	char	mailname[51]; 
	char	tmpstr[100];
	
	struct	com_sys_parm_c	g_com_sys_parm;
	struct	com_branch_c	g_com_branch;
	struct	com_tel_c		g_com_tel;

	memset(&g_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));

	/* 初始化结构 */
	pub_base_sysinit();
	
	/* 查询系统状态 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&g_com_sys_parm,"1=1");
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "查询公共系统参数状态错误!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O072");
		goto ErrExit;
	}
	else if( ret==100 )
	{
		sprintf( acErrMsg, "公共系统无参数!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O247");
		goto ErrExit;
	}

	if( g_com_sys_parm.sys_sts!=0 )
	{
		sprintf( acErrMsg, "系统非正常状态,不可以开工!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O071");
		goto ErrExit;
	}

	/* 修改网点状态 */
	ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",g_pub_tx.tx_br_no);
	if( ret )
	{
		sprintf( acErrMsg, "查询公共机构码表错误!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	ret = Com_branch_Fet_Upd(&g_com_branch,g_pub_tx.reply);
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "查询公共机构码表错误!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	if( g_com_branch.wrk_sts[0]=='1' )
	{
		sprintf( acErrMsg, "网点已签到!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O069");
		goto ErrExit;
	}
	
	if(g_com_branch.wrk_sts[0]=='*')
	{
		sprintf( acErrMsg, "网点处于关闭状态!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O068");
		goto ErrExit;
	}

	if(g_com_branch.wrk_sts[0]=='0')
	{
		sprintf( acErrMsg, "网点不是可签到状态!!" );
		WRITEMSG
			/***********
		strcpy(g_pub_tx.reply,"O060");
		goto ErrExit;
			***********************/
	}

	/*更改网点状态为已签到*/
	strcpy(g_com_branch.wrk_sts,"1");

	ret = Com_branch_Upd_Upd(g_com_branch,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg, "修改网点状态出错!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O067");
		goto ErrExit;
	}

	Com_branch_Clo_Upd( );

	/*更改所有柜员状态为正式签退*	 注掉了 为什么要更改成正式签退？本来就是正式签退的
	ret=Com_tel_Dec_Upd(g_pub_tx.reply,"br_no='%s' ",
			g_pub_tx.tx_br_no);
	if(ret)
	{
		sprintf( acErrMsg, "定义游标错误!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}

	while(1)
	{
		ret=Com_tel_Fet_Upd(&g_com_tel,g_pub_tx.reply);
		if(ret&&ret!=100)
		{
			sprintf( acErrMsg, "查询柜员表错误!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O070");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		if( g_com_tel.lvl[0]=='6' ) continue;

		strcpy(g_com_tel.csts,"2");

		ret=Com_tel_Upd_Upd(g_com_tel,g_pub_tx.reply);
		if(ret)
		{
			sprintf( acErrMsg, "修改所有柜员状态出错!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O067");
			goto ErrExit;
		}
	}
	Com_tel_Clo_Upd( );
	*/
	set_zd_long("0440",g_com_sys_parm.sys_date);
	
	/* 读取本日本网点提示文件 */

	sprintf(mailname, "%s/%ld/%s%ld" ,getenv("MAILDIR"), 
			g_pub_tx.tx_date , g_pub_tx.tx_br_no , g_pub_tx.tx_date ); 
	sprintf(acErrMsg,"文件名字[%s]",mailname);
	WRITEMSG
					      	
	fp = fopen( mailname,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",mailname );
		WRITEMSG
	}
	else
	{
		fclose(fp);
		pub_base_AllDwnFilName( filename );
		vtcp_log(filename);		
		
		sprintf(tmpstr,"cp %s %s 1>/dev/null 2>/dev/null",mailname, filename);
		system(tmpstr);
		vtcp_log(tmpstr);

		set_zd_data( DC_FILE_SND,"1" );	
	}


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
