/*************************************************
* 文 件 名:  sp9982.c
* 功能描述： 支付密码查讯 _ Inclusive for transaction 2206
*			 
*			1.新购买密码器的账户，从次日起支付密码启用。
*
*			2.购买密码器的账户， 出票日期在之前10天内则不需输入支付密码，
*  
*			  但必须输入支票日期，且只在开户行办理支取业务。 （后台控制）
*
*			3.前台提示柜员输入支票10天内的有效日期，其它凭证一天。
*			  ( 不需要支付密码的账户都不能通兑 )
*
* 作    者:  jack
* 完成日期： 2003年08月19日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "paymach_mana_inf_c.h"

int sp9982()
{
	int	ret;
	int tag=0;
	long tmp_date;
	char ac_no[19];
	char pwd_yn[2];
	struct	paymach_mana_inf_c	g_paymach_mana_inf;		/* 支付密码 */
	struct	mdm_ac_rel_c		s_mdm_ac_rel;		

	memset(ac_no,0,sizeof(ac_no));
	memset(&g_paymach_mana_inf,0x00,sizeof(struct paymach_mana_inf_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit( );

	get_zd_data( "1021", ac_no );				/* 账    号 */
	get_zd_data( "1023", g_pub_tx.beg_note_no );		/* 凭证种类 */
	get_zd_long( "102E", &tmp_date );					/* 出票日期 */
        vtcp_log("spCHNO开始！[%ld][%s][%d]",tmp_date,__FILE__,__LINE__);

	pub_base_old_acno( ac_no );

	pub_base_strpack(ac_no);

	strcpy(g_pub_tx.ac_no,ac_no);

	/*** 检查出票日期 ***/
	if( tmp_date && pub_base_checkdate( tmp_date ) )
	{
   		sprintf( acErrMsg, "日期格式输入不正确，请重新输入!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D243");
		goto ErrExit;
	}

	/*  取出开户机构  */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", 
							g_pub_tx.ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录![%s]",g_pub_tx.ac_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** 委托收款、银行划款不需检验支付密码 ***/
	if( !strncmp(g_pub_tx.beg_note_no,"133",3) || \
		!strncmp(g_pub_tx.beg_note_no,"138",3) )
	{
		if( strcmp(g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no) )
		{
			sprintf(acErrMsg,"该票据只能在开户行办理!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D248");
			goto ErrExit;
		}
		
	 	strcpy( pwd_yn,"N" );
	}
	else
	{
		/* 账户是否有密码器 */
		ret = Paymach_mana_inf_Sel( g_pub_tx.reply, &g_paymach_mana_inf, \
									"ac_no='%s' and flag='0'", g_pub_tx.ac_no );
		if( ret == 100 )
			strcpy( pwd_yn,"N" );
		else if( ret )
   		{
   			sprintf( acErrMsg, "查找错![%d]", ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		else 
		{
			/*** 帐户有密码器，则检查出票日期 ***/
			ret = pub_base_checkdate( tmp_date );
			if( ret )
			{
   				sprintf( acErrMsg, "日期格式输入不正确，请重新输入!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"D243");
				goto ErrExit;
			}

			if( tmp_date > g_pub_tx.tx_date )
			{
   				sprintf( acErrMsg, "出票日期不得大于系统日期，请重新输入!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"D244");
				goto ErrExit;
			}

			/*** 出票日期小于处理日期 ***/
			if( g_paymach_mana_inf.pr_date < tmp_date )
				strcpy( pwd_yn,"Y" );
			else if( g_paymach_mana_inf.pr_date >= tmp_date )
				strcpy( pwd_yn,"N" );
		}
	}

	pub_base_strpack( g_pub_tx.tx_br_no );
	pub_base_strpack( s_mdm_ac_rel.opn_br_no );

	if( pwd_yn[0]=='N' && strcmp(g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no) )
   	{
   		sprintf( acErrMsg, "该户无支付密码，需在开户行办理业务![%s][%s][%s]",pwd_yn,g_pub_tx.tx_br_no,s_mdm_ac_rel.opn_br_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D240");
		goto ErrExit;
	}

	set_zd_data( "102C", pwd_yn );					/* 支付密码 */

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
