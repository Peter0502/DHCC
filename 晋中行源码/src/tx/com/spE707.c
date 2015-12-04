/*************************************************
* 文 件 名:  spE707.c
* 功能描述： 预先授权交易
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "menu_c.h"
#include "com_tel_c.h"
#include "auth_inf_c.h"
#include "auth_seqn_c.h"
#include "auth_tel_auth_c.h"
#include "auth_auth_lmt_c.h"

char brno[6],tel[5],code[5];
long mtr_date;
double amt;

int spE707()
{
	int	ret;
	struct com_tel_c vcom_tel;
	char passd[21];
	char vttl[21];
	char ct[3];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_data( "0910", brno );
	get_zd_data( "0920", tel );
	get_zd_data( "0640", code );
	get_zd_data( "0670", ct );
	if( ct[0]!='1' && ct[0]!='2' && ct[0]!='3' && ct[0]!='4' )
		ct[0]='2';
	get_zd_long( "0450", &mtr_date );
	get_zd_double( "0400", &amt );
	get_zd_data( "0300", g_pub_tx.ac_no );

	/**授权人身份和对**/
	get_zd_data("0880",vttl);
	get_zd_data("0790",passd);

	if( strcmp(vttl,g_pub_tx.tel) )
	{
		ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel,"tel='%s'",vttl);
		if( ret )
		{
			strcpy(g_pub_tx.reply,"S077");
			goto ErrExit;
		}
		if( pub_base_DesChk(g_pub_tx.tx_date," ",passd,vcom_tel.pwd) )
		{
			strcpy(g_pub_tx.reply,"S077");
			goto ErrExit;
		}
	}

	vtcp_log("AUTH DO COND[%s][%s][%s][%d][%s]",
				brno,tel,code,mtr_date,g_pub_tx.ac_no );
	
	ret=sub_chk_tel( brno,tel );
	if( ret ) goto ErrExit;
	
	ret=sub_chk_menu( code );
	if( ret ) goto ErrExit;
	
	ret=sub_chk_acno( g_pub_tx.ac_no );
	if( ret ) goto ErrExit;

	ret=sub_com_tel_amt_auth_chk( vttl, amt,ct ) ;
	if( ret )
	{
		vtcp_log("CHK AMT [%s][%.2lf]",g_pub_tx.tel,amt );
		strcpy( g_pub_tx.reply,"S074" );
		goto ErrExit;
	}
	ret=sub_mk_auth_code( vttl );
	if( ret ) goto ErrExit;

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
sub_chk_tel( char * brno,char * tel )
{
	struct com_tel_c com_tel_v;

	if( !strlen(tel) ) goto OkExit;

	g_reply_int = Com_tel_Sel( g_pub_tx.reply, &com_tel_v, 
		"tel='%s'", tel );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S019" );
		goto ErrExit;
	}
	if( com_tel_v.csts[0]=='3' )
	{
		WRITEMSG
		strcpy( g_pub_tx.reply,"S012" );
		goto ErrExit;
	}
	if( com_tel_v.csts[0]=='4' )
	{
		WRITEMSG
		strcpy( g_pub_tx.reply,"S011" );
		goto ErrExit;
	}
	if( strncmp(com_tel_v.br_no,brno,5) )
	{
		sprintf( acErrMsg, "查询录入机构出错![%s][%s]",com_tel_v.br_no,brno );
		WRITEMSG
		strcpy( g_pub_tx.reply,"B273" );
		goto ErrExit;
	}
	
OkExit:
	return 0;
ErrExit:
	return 1;
}
sub_chk_menu( char *code )
{
	struct menu_c menu_v;

	if( !strlen(code) ) goto OkExit;

	g_reply_int = Menu_Sel( g_pub_tx.reply,&menu_v,"dspcode='%s'", code );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入交易名称出错![%d]", g_reply_int );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S060" );
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
sub_chk_acno( char * ac_no )
{
	if( !strlen(g_pub_tx.ac_no) ) goto OkExit;

	if( pub_base_acgetname(g_pub_tx.ac_no, &g_pub_tx.cif_no, g_pub_tx.name) )
	{
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
sub_mk_auth_code( char * vttl )
{
	int ret=0;
	struct auth_inf_c auth_inf_v;
	struct auth_seqn_c auth_seqn_v;

	/**生成授权码**/
	ret=Auth_seqn_Dec_Upd( g_pub_tx.reply,"1=1" );
	if( ret ) goto ErrExit;

	ret=Auth_seqn_Fet_Upd( &auth_seqn_v,g_pub_tx.reply );
	if( ret==100 )
	{
		memset( &auth_seqn_v,0,sizeof(auth_seqn_v) );
		auth_seqn_v.seqn=1000000;
		ret=Auth_seqn_Ins( auth_seqn_v,g_pub_tx.reply );
		if( ret ) goto ErrExit;
	}
	else if( ret ) goto ErrExit;
	else
	{
		auth_seqn_v.seqn++;
		ret=Auth_seqn_Upd_Upd( auth_seqn_v,g_pub_tx.reply  );
		if( ret ) goto ErrExit;
	}

	Auth_seqn_Clo_Upd( );

	/**-------------------------------------------**/
	memset( &auth_inf_v,0,sizeof(auth_inf_v) );
	auth_inf_v.auth_no=auth_seqn_v.seqn;
	strcpy( auth_inf_v.br_no,brno );
	strcpy( auth_inf_v.tel,tel );
	auth_inf_v.mtr_date=mtr_date;
	strcpy( auth_inf_v.tx_code,code );
	auth_inf_v.amt=amt;
	strcpy( auth_inf_v.ac_no,g_pub_tx.ac_no );

	strcpy( auth_inf_v.auth_tel,vttl );
	strcpy( auth_inf_v.auth_br_no,g_pub_tx.tx_br_no );
	auth_inf_v.auth_date=g_pub_tx.tx_date;
	strcpy( auth_inf_v.sts,"0" );

	ret=Auth_inf_Ins( auth_inf_v,g_pub_tx.reply );
	if( ret ) goto ErrExit;
	
	set_zd_long( "0440",auth_inf_v.auth_no );
OkExit:
	return 0;
ErrExit:
	return 1;
}
sub_com_tel_amt_auth_chk( char *tel,double amt,char *ct )
{
	struct auth_tel_auth_c auth_tel_auth_c;
	struct auth_auth_lmt_c auth_auth_lmt_c;
	int ret=0;
	char vau[4];
	char vmo[41];
	char tmpstr[31];

	memset( &auth_tel_auth_c,0,sizeof(auth_tel_auth_c) );

vtcp_log("金额权限检查[%s][%.2lf][%s]",tel,amt,ct);
	ret=Auth_tel_auth_Sel( g_pub_tx.reply,&auth_tel_auth_c,
		"tel='%s'",tel );
	if( ret==100 )
	{
		/**没有的默认为普通柜员**/
		strcpy( auth_tel_auth_c.tel,tel );
		strcpy( auth_tel_auth_c.au_al,"PT1" );
		strcpy( auth_tel_auth_c.au_xj,"PT1" );
		strcpy( auth_tel_auth_c.au_zz,"PT1" );
		strcpy( auth_tel_auth_c.au_tc,"PT1" );
		strcpy( auth_tel_auth_c.au_de,"PT1" );
		strcpy( auth_tel_auth_c.ind,"0000000000" );
	}
	else if( ret ) goto ErrExit;

	switch( ct[0] )
	{
		case '1':
			strcpy( vau,auth_tel_auth_c.au_xj );
			strcpy( vmo,"现金权限" );
			break;
		case '2':
			strcpy( vau,auth_tel_auth_c.au_zz );
			strcpy( vmo,"转帐权限" );
			break;
		case '3':
			strcpy( vau,auth_tel_auth_c.au_tc );
			strcpy( vmo,"同城权限" );
			break;
		case '4':
			strcpy( vau,auth_tel_auth_c.au_de );
			strcpy( vmo,"大额权限" );
			break;
		default:
			strcpy( vau,auth_tel_auth_c.au_al );
			strcpy( vmo,"柜员权限" );
			break;
	}
	pub_base_strpack( vau );
	if( !strlen(vau) ) strcpy( vau,auth_tel_auth_c.au_al );

vtcp_log("auth lvl [%s]",vau );
	ret=Auth_auth_lmt_Sel( g_pub_tx.reply,&auth_auth_lmt_c,
		"auth_code='%s'",vau );
	if( ret==100 )
	{
		sprintf(acErrMsg,"交易限额定义缺失[%s]",vau);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S071" );
		goto ErrExit;
	}
	else if( ret ) goto ErrExit;
	
vtcp_log("auth lvl [%s][%.2lf]",tel,auth_auth_lmt_c.max_amt );
	if( pub_base_CompDblVal(amt,auth_auth_lmt_c.max_amt)>0 )
	{
		sprintf( tmpstr,"%.2lf",auth_auth_lmt_c.max_amt );
		strcat( vmo,tmpstr );
		vtcp_log( vmo ); TRACE
		set_zd_data(DC_GET_MSG,vmo);
		strcpy( g_pub_tx.reply,"S073" );
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}
