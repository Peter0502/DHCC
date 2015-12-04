/*****************************************************
* 文 件 名:  sp9597.c
* 功能描述： 短信银行信息查询
* 完成日期： 2007-05-09 17:58
* 修改记录： 
* 日    期:
* 修 改 人: SSH
* 修改内容:
*****************************************************/
/***
insert into tx_def values('9597','通用表查询','11100000000000000000100010000000000000000001001000000000010000000011100000000010000000000001000000000000000000000000000000000000','0','4',0);
insert into tx_flow_def values('9597',0,'9597','#$');
insert into tx_sub_def values('9597','通用表查询','sp9597','0','0');
***/
#define EXTERN
#include <string.h>
#include "public.h"
#include "mob_acct_type_c.h"
#include "mob_server_type_c.h"
int sp9597()
{
	int ret=0;
	char acno[20];
	char cFilename[128];
	double dAmt = 0.0;
	int n = 0;
	long lst_fee_date = 0;
	char cServer_type[6];
	char cMob_sts[2];
	char cFiller[5];  /*add by lwb 20150325增加返回filler中的贷款签约状态*/
	struct mob_acct_type_c sMob ,sMob_acct_type;
	struct mob_server_type_c sMob_server_type;
	memset(acno, 0 , sizeof(acno));
	memset(cMob_sts, 0 , sizeof(cMob_sts));
	memset(cServer_type,0x00,sizeof(cServer_type));
	memset(cFilename, 0 , sizeof(cFilename));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sMob_server_type,0x00,sizeof(sMob_server_type));
	memset(cFiller,0x00,sizeof(cFiller));
	vtcp_log("sp9597 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("0300",acno);
	trim(acno);
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_no = '%s' ", \
		acno );
	if ( ret )
	{
		sprintf( acErrMsg,"执行Mob_acct_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			vtcp_log("查询mob_acct_type表中无签约手机账户信息\n");
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg,"执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		strcpy(cServer_type,sMob_acct_type.server_type);
		strcpy(cMob_sts,sMob_acct_type.mob_sts);
		lst_fee_date = sMob_acct_type.lst_fee_date;
		strcat(cFilename,sMob_acct_type.mobile);
		strcat(cFilename,"|");
                strcat(cFiller,sMob_acct_type.filler); 
	}
	trim(cFilename);
	Mob_acct_type_Clo_Sel();
	if(strlen(cServer_type) != 0){
	ret = Mob_server_type_Sel(g_pub_tx.reply,&sMob_server_type,"server_type = '%s'",cServer_type);
	if(ret )
	{
		sprintf( acErrMsg,"执行Mob_acct_type_Fet_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	n=pub_base_CalMouths(lst_fee_date,g_pub_tx.tx_date);
	if(n>3)
	n=3;
	dAmt = sMob_server_type.fee_standard*n;
	vtcp_log("cccccccccccccccccccccccccc=[%s]  dAmt=[%f]",cFilename,dAmt);
	set_zd_data("0250",cFilename);
	set_zd_data("0670",cMob_sts);
	set_zd_double("0400",dAmt);
	set_zd_data("0320",cFiller);
  }
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
