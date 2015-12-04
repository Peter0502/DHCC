/*************************************************
* 文 件 名:  sp7605.c
* 功能描述： 小区管理
*
* 作    者:	 houyi 
* 完成日期： 2010年10月29日
insert into tx_def values('7605','小区管理','10000000000000000000000011100100000000000000000000010000010000000000010000000000000000000000000000000000000000000000000000000000','0','6','2','3','');
insert into tx_flow_def values('7605','0','7605','#$');
insert into tx_sub_def values('7605','小区管理','sp7605','0','0');

*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "xqdz_c.h"
#include "sub_dd_mst_c.h"

sp7605()
{
	char   cOpt[2];
	long   xqbh=0;
	struct xqdz_c  sXqdz,sXqdz_temp;
	struct sub_dd_mst_c sSub_dd_mst;
	
	memset(cOpt,0x00,sizeof(cOpt));
	memset(&sXqdz,0x00,sizeof(sXqdz));
	
	pub_base_sysinit();
	
	get_zd_data("0300",sXqdz.xqdz100);/**小区主账户**/
	pub_base_old_acno(sXqdz.xqdz100);
	strcpy(g_pub_tx.ac_no,sXqdz.xqdz100);
	get_zd_long("0520",&xqbh);        /**小区编号**/
	get_zd_data("0250",sXqdz.xqdz15); /**小区名称**/
	get_zd_data("0260",sXqdz.xqdz16); /**小区地址**/
	/*-- modify begin reason:联系人只需要20位 新增的小区开发商需要60位 by wzs 20120214 --*/
	/* get_zd_data("0270",sXqdz.xqdz17); *//**联系人**/
	get_zd_data("0810",sXqdz.xqdz17); /**联系人**/
	get_zd_data("0270",sXqdz.xqdz31); /**小区开发商**/
	/*-- modify end by wzs 20120214 --*/
	get_zd_data("0580",sXqdz.xqdz18); /**联系电话**/
	get_zd_data("0700",cOpt);
	sprintf(sXqdz.xqdz101,"%ld",xqbh);
	
	if(cOpt[0]=='1')
	{
		sXqdz.xqdz30[0]='1';
		sXqdz.xqdz20=g_pub_tx.tx_date;
		sXqdz.xqdz21=g_pub_tx.tx_time;
		strcpy(sXqdz.xqdz22,g_pub_tx.tel);
		strcpy(sXqdz.xqdz110,g_pub_tx.opn_br_no);
		strcpy(sXqdz.xqdz111,g_pub_tx.opn_br_no);
		sXqdz.xqdz23=g_pub_tx.tx_date;
		sXqdz.xqdz24=g_pub_tx.tx_time;
		strcpy(sXqdz.xqdz25,g_pub_tx.tel);
		
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",g_pub_tx.ac_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Mdm_ac_rel_Dec_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Mdm_ac_rel_Fet_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		/* g_mdm_ac_rel.kt_flag[9]='1'; * 是否开通小区的标志位置 */
		g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Mdm_ac_rel_Upd_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		Mdm_ac_rel_Clo_Upd();
		g_reply_int=Xqdz_Ins(sXqdz,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Xqdz_Ins错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"NXC8");
			goto ErrExit;
		}
	}
	else
	{
		memset(&sXqdz_temp,0x00,sizeof(sXqdz_temp));
		g_reply_int=Xqdz_Dec_Upd(g_pub_tx.reply,"xqdz100 in('%s','%s') and xqdz101='%s' ",g_pub_tx.ac_no,sXqdz.xqdz100,sXqdz.xqdz101);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Xqdz_Dec_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		g_reply_int=Xqdz_Fet_Upd(&sXqdz_temp,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Xqdz_Fet_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		if(cOpt[0]=='3')
		{
			sXqdz_temp.xqdz30[0]='2';
		}
		else
		{
			strcpy(sXqdz_temp.xqdz15,sXqdz.xqdz15);
			strcpy(sXqdz_temp.xqdz16,sXqdz.xqdz16);
			strcpy(sXqdz_temp.xqdz17,sXqdz.xqdz17);
			strcpy(sXqdz_temp.xqdz18,sXqdz.xqdz18);
			/*-- add begin reason: 新增小区开发商 by wzs 20120214 --*/
			strcpy(sXqdz_temp.xqdz31,sXqdz.xqdz31);
			/*-- add end by wzs 20120214 --*/
			sXqdz_temp.xqdz23=g_pub_tx.tx_date;
			sXqdz_temp.xqdz24=g_pub_tx.tx_time;
			strcpy(sXqdz_temp.xqdz25,g_pub_tx.tel);
		}
		g_reply_int=Xqdz_Upd_Upd(sXqdz_temp,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Xqdz_Fet_Upd错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		Xqdz_Clo_Upd();
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	Mdm_ac_rel_Clo_Upd();
	Xqdz_Clo_Upd();
	Sub_dd_mst_Clo_Upd();
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

