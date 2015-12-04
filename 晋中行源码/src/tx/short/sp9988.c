/*************************************************
* 文 件 名:  sp9988.c
* 功能描述： 柜员权限下传
*
* 作    者:  jack
* 完成日期： 
*
* 修改记录： 权限交集的定义
* 日   期:   20030823
* 修 改 人:  rob
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
#include "com_tel_change_c.h"
#include "com_tel_change_hst_c.h"
int sp9988()
{
	char fiedlst[100];
	char filter[100];
	char filename[100];
	char cTel[5];
	struct com_tel_c vcom_tel,pcom_tel,vcom_tel1;
	struct com_branch_c vcom_branch;
	struct com_sys_parm_c vcom_sys_parm;
	struct auth_tx_list_c vauth_tx_list;
	struct com_parm_c sCom_parm;
	FILE *fp;
	int ret=0, i=0, j=0;
	char wherelist[100], tmpstr[50];
	char old_menu[5], new_menu[5];
 	vtcp_log("9988开始！[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();
 	vtcp_log("9988init over[%s][%d]",__FILE__,__LINE__);

	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(cTel, 0x0, sizeof(cTel));
	memset(old_menu, 0x0, sizeof(old_menu));
	memset(new_menu, 0x0, sizeof(new_menu));
	memset(&vcom_tel1, 0x0, sizeof(vcom_tel1));

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&vcom_sys_parm," 1=1" );
	if( ret ) goto ErrExit;

	get_zd_data( "0790",pcom_tel.pwd );	/*口令*/
	get_zd_data( "0920",cTel);	/*原始柜员 */
	sprintf( acErrMsg, "口令!!" );
	WRITEMSG
	sprintf( acErrMsg, "口令!![%s][%s]",cTel,g_pub_tx.tel );
	WRITEMSG
  TRACE
	ret = Com_tel_Sel( g_pub_tx.reply,&vcom_tel," tel='%s' ",g_pub_tx.tel );
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
  ret = Com_tel_Sel( g_pub_tx.reply,&vcom_tel1," tel='%s' ",cTel );
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",cTel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	if( vcom_tel1.csts[0]=='4' )
	{
		sprintf( acErrMsg,"柜员状态错[%s]O[%s]",cTel,vcom_tel1.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O062" );
		goto ErrExit;
	}
	if( vcom_tel1.csts[0]=='3')
	{
		sprintf( acErrMsg,"柜员状态错[%s]i[%s]",cTel,vcom_tel1.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O063" );
		goto ErrExit;
	}
	if( pub_base_DesChk(g_pub_tx.tx_date," ",pcom_tel.pwd,vcom_tel1.pwd) )
	{
		sprintf( acErrMsg, "口令不正确!!" );
		WRITEMSG
		set_zd_data("0790","XXXXXX" );
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}
	set_zd_data("0580",vcom_tel.name );
	set_zd_data("0790",vcom_tel.pwd );
	set_zd_data("0920",vcom_tel.beg_menu );
	set_zd_long("0440",vcom_tel.pwd_date );
	set_zd_data("0690",vcom_tel.csts );
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
		" br_no='%s' ",g_pub_tx.tx_br_no );
	if( ret ) goto ErrExit;

	set_zd_data("0210",vcom_branch.wrk_sts ); /*网点工作状态*/
	pub_base_strpack(vcom_tel.tx_type);

	vtcp_log("RRRRR tel[%s] com[%d] br_no[%s]",vcom_tel.tx_type,
			vcom_branch.tx_type,g_pub_tx.tx_br_no);

	if( strcmp(vcom_tel.tx_type,"1111111111") == 0 && vcom_branch.tx_type==0 ) /**全部**/
	{
TRACE
		set_zd_data("0670","1");
	}
	else 
	{
		pub_base_AllDwnFilName( filename );
		fp=fopen(filename,"w");
		if( fp==NULL )
		{
			sprintf( acErrMsg,"open file error [%s]",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047");
			goto ErrExit;
		}

		if (atol(vcom_tel.tx_type) != 0)
		{
			strcpy(wherelist, "tx_type in (");
			for (i=0, j=0; i < 10; i++)
			{
				if (vcom_tel.tx_type[i] == '1')
				{
					j++;
					memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
					ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, 
						"parm_code='GYQX' and parm_seqn=%d", i);
					if (ret != 0)
					{
						sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
						WRITEMSG
						goto ErrExit;
					}
					memset(tmpstr, 0x0, sizeof(tmpstr));
					pub_base_strpack(sCom_parm.val);
					if (j == 1)
					{
						sprintf(tmpstr, "%s", sCom_parm.val);
					}
					else
					{
						sprintf(tmpstr, ",%s", sCom_parm.val);
					}
					strcat(wherelist, tmpstr);
				}
			}
			if( !strcmp(g_pub_tx.tx_br_no,"61099") )
			{
					sprintf(tmpstr, ",99");
					strcat(wherelist, tmpstr);
			}
			strcat(wherelist, ")");
			sprintf(acErrMsg, "CGH********柜员权限集合为: [%s]", wherelist);
			WRITEMSG
		}

		if( !atol(vcom_tel.tx_type) && vcom_branch.tx_type ) /**全部**/
		{
			ret=Auth_tx_list_Dec_Sel( g_pub_tx.reply,
				"tx_type=%d ", vcom_branch.tx_type);
			TRACE
		}
		else if( atol(vcom_tel.tx_type) && !vcom_branch.tx_type ) /**全部**/
		{
			ret=Auth_tx_list_Dec_Sel( g_pub_tx.reply, "%s order by \
				menu_code", wherelist);
			TRACE
		}
		else
		{
			ret=Auth_tx_list_Dec_Sel( g_pub_tx.reply,
				"%s and menu_code in(select menu_code from auth_tx_list \
				where tx_type=%d) order by menu_code", wherelist, 
				vcom_branch.tx_type);
			TRACE
		}
		if( ret )
		{
			TRACE
			goto ErrExit;
		}

		sprintf(acErrMsg, "tel_type=[%s], branch_type=[%d]", vcom_tel.tx_type,
			vcom_branch.tx_type);
		WRITEMSG

		while(1)
		{
			 ret=Auth_tx_list_Fet_Sel( &vauth_tx_list,g_pub_tx.reply );
			 if( ret==100 ) break;
			 if( ret )
			 {
				TRACE
				goto ErrExit;
			 }

			 strcpy(new_menu, vauth_tx_list.menu_code);
			 if (strcmp(new_menu, old_menu) == 0)
			 {
				continue;
			 }

			 fprintf(fp,"%s|%s|\n",g_pub_tx.tel,vauth_tx_list.menu_code);
			 strcpy(old_menu, new_menu);
		}
		Auth_tx_list_Clo_Sel();

		fclose(fp);

		set_zd_data("0670","0");
		set_zd_data(DC_FILE_SND,"1");
TRACE
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
