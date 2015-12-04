/***************************************************************
* 文 件 名:     spD010.c
* 功能描述：    对公活期开集团户转换账号类型为集团户账号类型ac_no-cnt
*
* 作    者:     hhb
* 完成日期：    2011年8月11日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD010()
{
	int fun_mainacno(char *acno),fun_sonacno(char *acno1,char *acno2);
	char og_acno,om_acno,main_acno[30];
	memset(&og_acno,0x00,sizeof(og_acno));
	memset(&om_acno,0x00,sizeof(om_acno));
	memset(main_acno,0x00,sizeof(main_acno));

	pub_base_sysinit();

	get_zd_data("0670",&og_acno);/**是否开集团户,1 是, 0 否**/
	get_zd_data("0680",&om_acno);/**是否开主帐户,1 是, 0 否**/
	get_zd_data("0300",main_acno);/**主帐户**/
	get_zd_data("1167",g_pub_tx.ac_no);

	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(main_acno);
	vtcp_log("[%s][%d] og_acno=[%c],om_acno=[%c],ac_no=[%s],main_acno=[%s]\n",__FILE__,__LINE__,og_acno,om_acno,g_pub_tx.ac_no,main_acno);

	if(og_acno=='1' && om_acno=='1')
		g_reply_int=fun_mainacno(g_pub_tx.ac_no);
	if(og_acno=='1' && om_acno=='0')
		g_reply_int=fun_sonacno(g_pub_tx.ac_no,main_acno);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"账号转换失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

	vtcp_log("[%s][%d] ac_no=[%s],main_acno=[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no,main_acno);
	set_zd_data("1167",g_pub_tx.ac_no);

OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"开集团户账号转换成功![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"开集团户账号转换失败![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
/**开主帐户子函数**/
int fun_mainacno(char *acno)
{
	g_reply_int=0;
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",acno);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"声明数据库游标失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库游标取值失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	strcat(g_mdm_ac_rel.ac_no,"-1");
	strcpy(acno,g_mdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库更新记录失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Clo_Upd();
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库关闭游标失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/**开子账户子函数**/
int fun_sonacno(char *acno1,char *acno2)
{
	int cnt=0;
	g_reply_int=0;
	char tail[20],head[30];
	memset(tail,0x00,sizeof(tail));
	memset(head,0x00,sizeof(head));
	if(!strlen(acno2))
	{
		strcpy(acErrMsg,"主账号错!");
		strcpy(g_pub_tx.reply,"C115");
		WRITEMSG
		goto ErrExit;
	}
	memcpy(head,acno2,strlen(acno2)-2);
	cnt=sql_count("mdm_ac_rel","substr(ac_no,0,%d)='%s'",strlen(head),head);
	if(!cnt)
	{
		strcpy(acErrMsg,"主账号错!");
                strcpy(g_pub_tx.reply,"C115");
                WRITEMSG
                goto ErrExit;
	}
	cnt++;
	sprintf(tail,"-%d",cnt);
	strcat(head,tail);
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",acno1);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"声明数据库游标失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库游标取值失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	strcpy(g_mdm_ac_rel.ac_no,head);
	strcpy(acno1,g_mdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库更新记录失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Clo_Upd();
	if(g_reply_int)
		{
			sprintf(acErrMsg,"数据库关闭游标失败![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
OkExit:
	return 0;
ErrExit:
	return 1;
}
