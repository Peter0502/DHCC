#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_GetTrad.c
* 功能描述：
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "tx_log_c.h"
#include "auth_def_c.h"
#include "auth_code_c.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_cond_def_c.h"
#include "svrpub_c.h"
#include "auth_chk_def_c.h"
#include "auth_inf_c.h"
#include "auth_auth_auth_c.h"
#include "auth_auth_lmt_c.h"
#include "public.h"
extern struct  tx_def_c g_tx_def_c;

/**授权主控**/
int pub_comm_auth_proc()
{
	long vau_code;
	long vtrace_no;
	int ret=0;
	int needauth=0;
	struct auth_code_c vauth_code;
	char tel[5],auth[5],chk[5];
	char vtx_code[5];
	char vbr_no[6];
	int do_over=0;
	char wact_flag[11]; /*流程标志*/
	char wlvl_flag[11]; /*柜员级别标志*/
	char vact_flag[2];
	char vmo[41];

	get_zd_data( DC_TX_CODE,vtx_code );
	get_zd_long( DC_AUTH_CODE,&vau_code );
	get_zd_long( DC_TRACE_NO,&vtrace_no );
	get_zd_data( DC_TEL,tel );
	get_zd_data( DC_CHK,chk );
	get_zd_data( DC_AUTH,auth );
	get_zd_data( DC_TX_BR_NO,vbr_no );

	MEMSET_DEBUG( wact_flag,0,sizeof(wact_flag) );
	MEMSET_DEBUG( wlvl_flag,0,sizeof(wlvl_flag) );

vtcp_log("TEL AUTH CHK [%s] [%s] [%s]",tel,auth,chk );

	if( g_tx_def_c.tx_type[0]=='4' && strcmp(vtx_code,"9800") )
	{
		vtcp_log("快捷查询不需要授权处理");
	}
	else if( strlen(auth) && !strncmp(auth,"@@@",3) )
	{
		ret = pub_comm_bef_chk_auth( vtx_code,wact_flag,wlvl_flag,vmo ) ;
		if( ret<0 ) 
		{
			strcpy( RtCode,g_pub_tx.reply );
			goto ErrExit;
		}
		get_zd_data(DC_TX_BR_NO,g_pub_tx.tx_br_no);
		get_zd_data(DC_TEL,g_pub_tx.tel);
		ret=pub_comm_chk_far( vtx_code,wact_flag,wlvl_flag );
		if( ret ) 
		{
			strcpy( RtCode,g_pub_tx.reply );
			goto ErrExit;
		}
	}
	else if( strlen(auth) )
	{
		ret = pub_comm_bef_chk_auth( vtx_code,wact_flag,wlvl_flag,vmo ) ;
		if( ret<0 ) 
		{
			strcpy( RtCode,g_pub_tx.reply );
			goto ErrExit;
		}
		get_zd_data(DC_TX_BR_NO,g_pub_tx.tx_br_no);
		get_zd_data(DC_TEL,g_pub_tx.tel);
		ret=pub_comm_chk_loa( vtx_code,wact_flag,wlvl_flag );
		if( ret ) 
		{
			strcpy( RtCode,g_pub_tx.reply );
			goto ErrExit;
		}
	}
	else if( vau_code ) /**有授权码：授权或复核处理**/
	{
		ret=Auth_code_Dec_Upd( RtCode,"auth_no=%d",vau_code);
		if( ret )
		{
			vtcp_log("此授权码不存在");TRACE
			strcpy( RtCode,"Z101" );
			goto ErrExit0;
		}
		ret=Auth_code_Fet_Upd( &vauth_code,RtCode );
		if( ret )
		{
			vtcp_log("此授权码不存在");TRACE
			strcpy( RtCode,"Z101" );
			goto ErrExit;
		}

			if( vauth_code.sts[0]=='9' ) 
			{
				vtcp_log("已经完成了"); TRACE
				strcpy( RtCode,"Z107" );
				goto ErrExit;
			}

		strcpy( vact_flag,vauth_code.auth_ind );

vtcp_log("vauth_code.auth_ind=[%s]",vauth_code.auth_ind);

		if( vact_flag[0]=='2' ) /**复核**/
		{
			if( !strcmp(vauth_code.tel,chk) )
			{
				vtcp_log("不能为自己复核");TRACE
				strcpy( RtCode,"Z102" );
				goto ErrExit;
			}
		}
		else if( vact_flag[0]=='3' ) /**授权**/
		{
			if( strlen(auth) )
			{
				if( !strcmp(auth,vauth_code.tel) ) /**未授权，进行授权**/
				{
					vtcp_log("此交易不能由本人授权[%s][%s]",tel,vauth_code.tel);
					TRACE
					strcpy( RtCode,"Z109" );
					goto ErrExit;
				}
			}
			else if( vauth_code.sts[0]=='3' ) /**已经授权，需要完成**/
			{
				if( strcmp(tel,vauth_code.tel) )
				{
					vtcp_log("此交易不是你发起的[%s][%s]",tel,vauth_code.tel);
					TRACE
					strcpy( RtCode,"Z108" );
					goto ErrExit;
				}
			}
			else
			{
				if( !strcmp(tel,vauth_code.tel) ) /**未授权，进行授权**/
				{
					vtcp_log("此交易不能由本人授权[%s][%s]",tel,vauth_code.tel);
					TRACE
					strcpy( RtCode,"Z109" );
					goto ErrExit;
				}
			}
		}
		else
		{
			vtcp_log("已经完成了"); TRACE
			strcpy( RtCode,"Z107" );
			goto ErrExit;
		}

		if( !strcmp(vtx_code,"9800") ) /**输入授权码回显交易**/
		{
			vtcp_log(" RUN 9800 TRAD to回显交易信息");
			if( vact_flag[0]=='2' ) /**复核**/
			{
				set_chk_fied( vauth_code.tx_code );
				/**返回原交易人**/
				set_zd_data(DC_CHK,vauth_code.tel );
				strcpy( RtCode,"Z007" );
				goto ErrExit;
			}
			else
			{
				ret=pub_comm_auth_respon(vauth_code);
				if( ret ) goto ErrExit;
			}
		}
		/****修改登记簿****/
		if( vact_flag[0]=='2' ) /**复核**/
		{
				strcpy( vauth_code.chk,chk );
				if( pub_comm_telauth_chk(vauth_code,chk) ) goto ErrExit;
				vauth_code.sts[0]='9';
		}
		if( vact_flag[0]=='3' ) /**授权**/
		{
			if( strlen(auth) )
			{
				strcpy( vauth_code.auth,auth );
				if( pub_comm_telauth_chk(vauth_code,auth) ) goto ErrExit;
				vauth_code.sts[0]='9';
			}
			else if( vauth_code.sts[0]=='3' ) /**已经授权，需要完成**/
			{
				vauth_code.sts[0]='9';
			}
			else
			{
				strcpy( vauth_code.auth,tel );
				if( pub_comm_telauth_chk(vauth_code,tel) ) goto ErrExit;
				vauth_code.sts[0]='3';
				needauth=4;
				strcpy( RtCode,"Z006" );
				set_zd_data(DC_REPLY,RtCode);
			}
		}

vtcp_log("vauth_code.auth_ind=[%s]",vauth_code.auth_ind);
		vauth_code.done_trace_no=vtrace_no;

		ret=Auth_code_Upd_Upd( vauth_code,RtCode );
		if( ret )
		{
			vtcp_log("此授权码不存在");TRACE
			strcpy( RtCode,"Z101" );
			Auth_code_Clo_Upd();
			goto ErrExit;
		}

		/**为记账的结构赋值**/
		set_zd_data(DC_CHK,vauth_code.chk);
		set_zd_data(DC_AUTH,vauth_code.auth);
		pub_base_strpack( vauth_code.auth );
		pub_base_strpack( vauth_code.aud );
		if( !strlen(vauth_code.auth) && strlen(vauth_code.aud) )
			set_zd_data(DC_AUTH,vauth_code.aud);

		Auth_code_Clo_Upd();
	}
	else /**无授权码**/
	{
		needauth=0;
	}

	return needauth; /**1需要授权0不需要或授权成功2授权失败4只授权成功,返回**/
ErrExit:
	strcpy( RtCode,RtCode );
	Auth_code_Clo_Upd();
	set_zd_data(DC_REPLY,RtCode);
    return -1;
ErrExit0:
	strcpy( RtCode,RtCode );
	set_zd_data(DC_REPLY,RtCode);
    return -1;
}
int pub_comm_chk_auth_cond( char *cond_code,char *tx_code )
{
	struct auth_cond_def_c vcond_def;
	int needauth=1;
	int ret=0;

	ret = Auth_cond_def_Dec_Sel( RtCode," cond_code='%s' and tx_code='%s' ",
		cond_code,tx_code );
	if( ret ) goto ErrExit;
vtcp_log("--Q----[%s][%s] ",cond_code,tx_code);

	while( 1 )
	{
		ret = Auth_cond_def_Fet_Sel( &vcond_def,RtCode );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
		
vtcp_log("-------[%s]",vcond_def.cond_mo);
		pub_base_strpack( vcond_def.cond_mo );
		/***解释cond_mo NEED先对付**/
		if( !strlen(vcond_def.cond_mo) )
		{
vtcp_log("no pub_comm_auth_condchk");TRACE
			needauth=0; /*条件符合*/
			break;
		}
		else
		{
vtcp_log("pub_comm_auth_condchk");TRACE
			if( pub_comm_auth_condchk(vcond_def.cond_mo)==0 )
			{
				needauth=0; /*条件符合*/
				break;
			}
		}
	}
	Auth_cond_def_Clo_Sel( );

	return needauth;
ErrExit:
	return -1;
}
/**生成授权码*mode 复核,授权,审批**/
pub_comm_make_auth_code( char mode,long *auth_no )
{
	char tmpstr[21];
	long vtraceno;
	/****Modified by SSH,2006.10.11,使用序列代替共享内存取授权码****/
	vtraceno=nGetAuthSeqn();
	sprintf ( tmpstr,"%c%06d",mode,vtraceno );
	tmpstr[7]='\0';
	vtcp_log("make AUTH_CODE [%d][%s]",atol(tmpstr),tmpstr);
	*auth_no=atol(tmpstr);
	return(0);
	/*get_zd_long(DC_TRACE_NO,&vtraceno );*/
	vtraceno=get_tlsrno ();
	sprintf ( tmpstr,"%c%06d",mode,vtraceno );
	tmpstr[7]='\0';
vtcp_log("make AUTH_CODE [%d][%s]",atol(tmpstr),tmpstr);
	/* set_zd_long( DC_AUTH_CODE,atol(tmpstr) ); */
	*auth_no=atol(tmpstr);

	return 0;
}
/**mode 0 比较 1付值 **/
int pub_auth_cmp_pack( struct auth_code_c vauth_code,int mode )
{
	struct tx_log_c vtx_log;
	int ret=0;

	ret = Tx_log_Sel( RtCode,&vtx_log,
		" trace_no=%d ",vauth_code.trace_no );
	if( ret )
	{
		sprintf( acErrMsg,"交易不存在[%d]",vauth_code.trace_no );
		WRITEMSG
		strcpy( RtCode,"Z106" );
		goto ErrExit;
	}

	ret = pub_comm_chan_oldpack( vtx_log.bit_map,vtx_log.tx_pack );
	if( ret )
	{
		sprintf( acErrMsg,"error" );
		WRITEMSG
		strcpy( RtCode,"Z106" );
		goto ErrExit;
	}

	if( mode==1 )
	{
		ret = pub_comm_cmpchk_pack_data( vtx_log.bit_map );
		if( ret )
		{
			sprintf( acErrMsg,"error" );
			WRITEMSG
			strcpy( RtCode,"Z103" );
			goto ErrExit;
		}
	}
	else
	{
		ret = pub_comm_cmp_pack_data( vtx_log.bit_map );
		if( ret )
		{
			sprintf( acErrMsg,"error" );
			WRITEMSG
			strcpy( RtCode,"Z103" );
			goto ErrExit;
		}
	}

	return 0;
ErrExit:
	return 1;
}
int pub_comm_auth_respon(struct auth_code_c vauth_code)
{
	struct tx_log_c vtx_log;
	int ret=0;
	char trcd[5];

	get_zd_data("0180",trcd);
	if( strcmp(trcd,vauth_code.tx_code) )
	{
		sprintf(acErrMsg,"trcd[%s] [%s]",trcd,vauth_code.tx_code);
		WRITEMSG
		strcpy( RtCode,"Z10C" );
		goto ErrExit;
	}

	ret = Tx_log_Sel( RtCode,&vtx_log,
		" trace_no=%d ",vauth_code.trace_no );
	if( ret )
	{
		sprintf( acErrMsg,"交易不存在[%d]",vauth_code.trace_no );
		WRITEMSG
		strcpy( RtCode,"Z106" );
		goto ErrExit;
	}

	ret = pub_comm_chan_oldpack( vtx_log.bit_map,vtx_log.tx_pack );
	if( ret )
	{
		sprintf( acErrMsg,"error" );
		WRITEMSG
		strcpy( RtCode,"Z106" );
		goto ErrExit;
	}

	ret = pub_comm_show_pack( vtx_log.bit_map );
	if( ret )
	{
		sprintf( acErrMsg,"error" );
		WRITEMSG
		strcpy( RtCode,"Z106" );
		goto ErrExit;
	}
	pub_set_trad_bit( vtx_log.bit_map );

	set_zd_data(DC_REPLY,"0000");
	strcpy( RtCode,"0000" );

	return 1;
ErrExit:
	return 1;
}
int pub_comm_telauth_chk(struct auth_code_c vauth_code,char *ttlr)
{
	int ret=0;
	char vact_flag[2];
	struct auth_def_c vauth_def;
	struct com_tel_c vcom_tel;
	char vbr_no[6];

	strcpy( vact_flag,vauth_code.auth_ind );

	if( vact_flag[0]=='0' ) /**再次**/
		return 0;

	ret = Auth_def_Sel( RtCode,&vauth_def,"tx_code='%s' and seqn=%d", 
		vauth_code.tx_code,vauth_code.cond_seqn );
	if( ret==100 ) 
	{
		strcpy( vauth_def.flow_code,"300" );
		strcpy( vauth_def.lvl,"" );
	}
	else if( ret ) 
		goto ErrExit;

	ret= Com_tel_Sel( RtCode,&vcom_tel,"tel='%s'",ttlr);
	if( ret ) 
	{
		TRACE
		strcpy( RtCode,"Z10A" );
		goto ErrExit;
	}
		get_zd_data( DC_TX_BR_NO,vbr_no );
		if( strcmp(vbr_no,vcom_tel.br_no) )
		{
			vtcp_log("授权人非本机构柜员[%s][%s]",vbr_no,vcom_tel.br_no);TRACE
			strcpy( RtCode,"Z10E" );
			goto ErrExit;
		}
	if( vcom_tel.csts[0]=='3' || vcom_tel.csts[0]=='4' )
	{
		TRACE
		strcpy( RtCode,"Z10A" );
		goto ErrExit;
	}
	if( vact_flag[0]=='1'  /**审批**/
	/*	|| vact_flag[0]=='2'  **复核**/
		|| vact_flag[0]=='3' ) /**授权**/
	{
		if( strstr(vauth_def.lvl,vcom_tel.lvl)==NULL && strlen(vauth_def.lvl) )
		{
			vtcp_log("auth_def[%s] tel.lvl[%s]",vauth_def.lvl,vcom_tel.lvl);
			TRACE
			strcpy( RtCode,"Z10B" );
			goto ErrExit;
		}
	}
	return 0;
ErrExit:
	return 1;
}
int pub_comm_auth_condchk( char *cond_mo )
{
	char vstr[2][128];
	double vdbl[3];
	long vlng[3];
	char vpstr[128],vpstr1[128];
	char tmpstr[128];
	char tmpstr1[128];
	char tmpstr2[128];
	char tmpstr3[128];
	int i=0,j=0,k=0;
	int len=0;
	int flag=1;
	int flag1=1;
	char c,cc,cmp1,cmp2,lx='0';
	char *p,*q;

vtcp_log("pub_comm_auth_condchk BEG[%s]",cond_mo);TRACE
	/***
	@字符串
	$金额
	#整数
	***/
	memset(tmpstr2,0,sizeof(tmpstr2));
	memset(tmpstr3,0,sizeof(tmpstr3));
	pub_base_cut_str( cond_mo,tmpstr,"<>=!",1 );
	j=strlen(tmpstr);
	p=strchr(cond_mo,'&');/*当有2个判断条件时，拆开判断条件20131224*/
	i=strlen(p);
	vtcp_log("bbbbbbbbb GET[%s][%ld] [%c][%c]",p,i,cmp1,cmp2);
	/** add beg by wjl 20131224 com_def_chrg.cond有2个判断，获取第一条判断*/
	if(i>1)
	{
	memcpy(tmpstr2,cond_mo,strlen(cond_mo)-i);
		cmp1=cond_mo[j];
	if( strchr("<>=!",tmpstr2[j+1]) )
	{
		j++;
		cmp2=tmpstr2[j];
	}
	strcpy( tmpstr1,tmpstr2+j+1);
vtcp_log("cccccccccccccc GET[%s][%s] [%c][%c]",tmpstr,tmpstr1,cmp1,cmp2);
	}
	/**end 20131224*/
	else
	{
	cmp1=cond_mo[j];
	if( strchr("<>=!",cond_mo[j+1]) )
	{
		j++;
		cmp2=cond_mo[j];
	}
	strcpy( tmpstr1,cond_mo+j+1);
vtcp_log("pub_comm_auth_condchk GET[%s][%s] [%c][%c]",tmpstr,tmpstr1,cmp1,cmp2);TRACE
	}
	if( tmpstr[0]=='$' || tmpstr1[0]=='$' )
	{
		if( tmpstr[0]=='$' ) get_zd_double(tmpstr+1,&vdbl[0]);
		else vdbl[0]=atof(tmpstr);
		if( tmpstr1[0]=='$' ) get_zd_double(tmpstr1+1,&vdbl[1]);
		else vdbl[1]=atof(tmpstr1);
		
		vdbl[2]=vdbl[0]-vdbl[1];
vtcp_log("pub_comm_auth_condchk [%lf][%c][%c][%lf] [%lf]",
vdbl[0],cmp1,cmp2,vdbl[1],vdbl[2]);TRACE
		if( (cmp1=='!' && cmp2=='=') || (cmp1=='=' && cmp2=='!') ||
			(cmp1=='<' && cmp2=='>') || (cmp1=='<' && cmp2=='>') ) 
		{
			if( vdbl[2]>0.005 || vdbl[2]<-0.005 ) flag=0;
		}
		else if( (cmp1=='>' && cmp2=='=') || (cmp1=='=' && cmp2=='>') ) 
		{
			if( vdbl[2]>=-0.005 ) flag=0;
		}
		else if( (cmp1=='<' && cmp2=='=') || (cmp1=='=' && cmp2=='<') ) 
		{
			if( vdbl[2]<=0.005 ) flag=0;
		}
		else if( cmp1=='>' ) 
		{
			if( vdbl[2]>0.005 ) flag=0;
		}
		else if( cmp1=='<' ) 
		{
			if( vdbl[2]<-0.005 ) flag=0;
		}
		else if( cmp1=='=' ) 
		{
			if( vdbl[2]<0.005 && vdbl[2]>-0.005 ) flag=0;
		}
	}
	else if( tmpstr[0]=='#' || tmpstr1[0]=='#' )
	{
		if( tmpstr[0]=='$' ) get_zd_long(tmpstr+1,&vlng[0]);
		else vlng[0]=atol(tmpstr);
		if( tmpstr1[0]=='$' ) get_zd_long(tmpstr1+1,&vlng[1]);
		else vlng[1]=atol(tmpstr1);
		
		vlng[2]=vlng[0]-vlng[1];
		if( (cmp1=='!' && cmp2=='=') || (cmp1=='=' && cmp2=='!') ||
			(cmp1=='<' && cmp2=='>') || (cmp1=='<' && cmp2=='>') ) 
		{
			if( vlng[2]!=0 ) flag=0;
		}
		else if( (cmp1=='>' && cmp2=='=') || (cmp1=='=' && cmp2=='>') ) 
		{
			if( vlng[2]>=0 ) flag=0;
		}
		else if( (cmp1=='<' && cmp2=='=') || (cmp1=='=' && cmp2=='<') ) 
		{
			if( vlng[2]<=0 ) flag=0;
		}
		else if( cmp1=='>' ) 
		{
			if( vlng[2]>0 ) flag=0;
		}
		else if( cmp1=='<' ) 
		{
			if( vlng[2]<0 ) flag=0;
		}
		else if( cmp1=='=' ) 
		{
			if( vlng[2]=0 ) flag=0;
		}
	}
	else if( tmpstr[0]=='@' || tmpstr1[0]=='@' )
	{
		if( tmpstr[0]=='@' ) get_zd_data(tmpstr+1,vpstr);
		else strcpy( vpstr,tmpstr);
		if( tmpstr1[0]=='@' ) get_zd_data(tmpstr1+1,vpstr1);
		else strcpy( vpstr1,tmpstr1);

vtcp_log("COMD [%s][%s] [%c][%c]", vpstr,vpstr1,cmp1,cmp2);
		if( cmp1!='=' || cmp2!='=' ) 
		{
			if( strcmp(vpstr,vpstr1) ) flag=0;
		}
		else 
		{
			if( !strcmp(vpstr,vpstr1) ) flag=0;
		}
	}
	else
	{
		if( cmp1!='=' || cmp2!='=' ) 
		{
TRACE
			if( strcmp(tmpstr,tmpstr1) ) flag=0;
		}
		else 
		{
TRACE
			if( !strcmp(tmpstr,tmpstr1) ) flag=0;
		}
	}
	if(i>1)/*add beg当收费条件cond（&&）有2个判断时**/
	{
	strcpy( tmpstr3,p+2);
	pub_base_cut_str(tmpstr3,tmpstr,"<>=!",1 );
	j=strlen(tmpstr);
	strcpy( tmpstr3,p+2);
	vtcp_log("ddddddddd[%s]",tmpstr3);
	cmp1=tmpstr3[j];
	if( strchr("<>=!",tmpstr3[j+1]) )
	{
		j++;
		cmp2=p[j];
	}
	strcpy( tmpstr1,tmpstr3+j+1);
vtcp_log("pub_comm_auth_condchk GET[%s][%s] [%c][%c]",tmpstr,tmpstr1,cmp1,cmp2);TRACE

	if( tmpstr[0]=='$' || tmpstr1[0]=='$' )
	{
		if( tmpstr[0]=='$' ) get_zd_double(tmpstr+1,&vdbl[0]);
		else vdbl[0]=atof(tmpstr);
		if( tmpstr1[0]=='$' ) get_zd_double(tmpstr1+1,&vdbl[1]);
		else vdbl[1]=atof(tmpstr1);
		
		vdbl[2]=vdbl[0]-vdbl[1];
vtcp_log("pub_comm_auth_condchk [%lf][%c][%c][%lf] [%lf]",
vdbl[0],cmp1,cmp2,vdbl[1],vdbl[2]);TRACE
		if( (cmp1=='!' && cmp2=='=') || (cmp1=='=' && cmp2=='!') ||
			(cmp1=='<' && cmp2=='>') || (cmp1=='<' && cmp2=='>') ) 
		{
			if( vdbl[2]>0.005 || vdbl[2]<-0.005 ) flag1=0;
		}
		else if( (cmp1=='>' && cmp2=='=') || (cmp1=='=' && cmp2=='>') ) 
		{
			if( vdbl[2]>=-0.005 ) flag1=0;
		}
		else if( (cmp1=='<' && cmp2=='=') || (cmp1=='=' && cmp2=='<') ) 
		{
			if( vdbl[2]<=0.005 ) flag1=0;
		}
		else if( cmp1=='>' ) 
		{
			if( vdbl[2]>0.005 ) flag1=0;
		}
		else if( cmp1=='<' ) 
		{
			if( vdbl[2]<-0.005 ) flag1=0;
		}
		else if( cmp1=='=' ) 
		{
			if( vdbl[2]<0.005 && vdbl[2]>-0.005 ) flag1=0;
		}
	}
	else if( tmpstr[0]=='#' || tmpstr1[0]=='#' )
	{
		if( tmpstr[0]=='$' ) get_zd_long(tmpstr+1,&vlng[0]);
		else vlng[0]=atol(tmpstr);
		if( tmpstr1[0]=='$' ) get_zd_long(tmpstr1+1,&vlng[1]);
		else vlng[1]=atol(tmpstr1);
		
		vlng[2]=vlng[0]-vlng[1];
		if( (cmp1=='!' && cmp2=='=') || (cmp1=='=' && cmp2=='!') ||
			(cmp1=='<' && cmp2=='>') || (cmp1=='<' && cmp2=='>') ) 
		{
			if( vlng[2]!=0 ) flag1=0;
		}
		else if( (cmp1=='>' && cmp2=='=') || (cmp1=='=' && cmp2=='>') ) 
		{
			if( vlng[2]>=0 ) flag1=0;
		}
		else if( (cmp1=='<' && cmp2=='=') || (cmp1=='=' && cmp2=='<') ) 
		{
			if( vlng[2]<=0 ) flag1=0;
		}
		else if( cmp1=='>' ) 
		{
			if( vlng[2]>0 ) flag1=0;
		}
		else if( cmp1=='<' ) 
		{
			if( vlng[2]<0 ) flag1=0;
		}
		else if( cmp1=='=' ) 
		{
			if( vlng[2]=0 ) flag1=0;
		}
	}
	else if( tmpstr[0]=='@' || tmpstr1[0]=='@' )
	{
		if( tmpstr[0]=='@' ) get_zd_data(tmpstr+1,vpstr);
		else strcpy( vpstr,tmpstr);
		if( tmpstr1[0]=='@' ) get_zd_data(tmpstr1+1,vpstr1);
		else strcpy( vpstr1,tmpstr1);

vtcp_log("fdsdsfdf [%s][%s] [%c][%c]", vpstr,vpstr1,cmp1,cmp2);
		if( cmp1!='=' || cmp2!='=' ) 
		{
			if( strcmp(vpstr,vpstr1) ) flag1=0;
		}
		else 
		{
			if( !strcmp(vpstr,vpstr1) ) flag1=0;
		}
	}
	else
	{
		if( cmp1!='=' || cmp2!='=' ) 
		{
TRACE
			if( strcmp(tmpstr,tmpstr1) ) flag1=0;
		}
		else 
		{
TRACE
			if( !strcmp(tmpstr,tmpstr1) ) flag1=0;
		}
	}
	vtcp_log("%s,%d gggggg fflag==[%d]flag1=[%d]",__FILE__,__LINE__,flag,flag1);
	if((flag1==1)||(flag==1))
	{
		flag=1;
	}
	else
		flag=0;
	
	}
	/**end by wjl 20131224**/
	
vtcp_log("%s,%d ffffff fflag====[%d]",__FILE__,__LINE__,flag);
TRACE
	return flag;
ErrExit:
	return -1;
}
/*检查该复核定义是否要求复核制网点**/
int pub_comm_chk_chk_ind( char *chk_ind,char *tel,char *br_code )
{
	struct com_branch_c vbr;
	struct com_tel_c vte;
	int ret=0;
	int	flag=0;
	int chk=0;

	sprintf(acErrMsg,"传进来的机构和柜员为[%s][%s]", br_code, tel );
	WRITEMSG

	ret=Com_branch_Sel( g_pub_tx.reply,&vbr,"br_no='%s'",br_code);
	if( ret )
	{
		sprintf(acErrMsg,"com_branch error[%s] [%d]",br_code,ret);
		WRITEMSG
		goto ErrExit;
	}
	ret=Com_tel_Sel( g_pub_tx.reply,&vte,"tel='%s'",tel);
	if( ret )
	{
		sprintf(acErrMsg,"com_tel error[%s] [%d]",tel,ret);
		WRITEMSG
		goto ErrExit;
	}

	if( vbr.ind[0]=='1' || vte.chk_ind[0]=='1' ) /**复核制柜员**/
		chk=1;
	else
		chk=0;
sprintf(acErrMsg,"YPYPYP ch[%d] chk_ind[%s]",chk,chk_ind);
WRITEMSG
	if( chk==1 && chk_ind[0]=='2' )
		flag=1;
	else if( chk==0 && chk_ind[0]=='1' )
		flag=1;
	else
		flag=0;

	return flag;
ErrExit:
	return -1;
}
/**交易前授权检查函数**/
int pub_comm_bef_chk_auth(char *txcode,char *vact_flag,char *vlvl_flag,char *mo)
{
	struct  auth_def_c vauth_def;
	int needauth=0;
	int ret;

vtcp_log("交易前授权检查函数");
	/**seqn<0表示暂停此记录检查**/
	ret = Auth_def_Dec_Sel( RtCode,
		" tx_code='%s' and seqn>=0 order by seqn ",txcode );
	if( ret ) goto ErrExit;

	while( 1 )
	{
		ret = Auth_def_Fet_Sel( &vauth_def,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
vtcp_log("%s===",vauth_def.flow_code );
		if( vauth_def.flow_code[0]=='2' )
		{
			get_zd_data(DC_TX_BR_NO,g_pub_tx.tx_br_no);
			get_zd_data(DC_TEL,g_pub_tx.tel);
			/*检查该复核定义是否要求复核制网点**/
			ret=pub_comm_chk_chk_ind(vauth_def.chk_ind,
					g_pub_tx.tel,g_pub_tx.tx_br_no);
			if( ret<0 ) goto ErrExit;
			if( ret ) 	continue; /**条件不成立**/
		}

vtcp_log("%s===",vauth_def.cond_code );
		ret = pub_comm_chk_auth_cond( vauth_def.cond_code,txcode );
		if( ret<0 ) 
		{
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}
		if( ret ) 	continue; /**条件不成立**/

vtcp_log("%s===",vauth_def.lvl );
		/*vauth_def.flow_code 流程代号 NEED*/
		vact_flag[0]=vauth_def.flow_code[0];
		/*vauth_def.lvl 柜员级别 */
		/*vlvl_flag[0]=vauth_def.lvl[0]; */
		strcpy( vlvl_flag,vauth_def.lvl );
		strcpy( mo,vauth_def.mo );

		needauth=1;

		break;
	}
	Auth_def_Clo_Sel( );

	if( needauth==0 )
	{
		vtcp_log( "不需授权" );
		return 0;
	}

	return needauth;
ErrExit:
    return -1;
}
pub_comm_chk_far( char *txcode,char *vact_flag,char *vlvl_flag )
{
	struct auth_inf_c auth_inf_c;
	struct auth_chk_def_c auth_chk_def_c;
	struct com_tel_c com_tel_c;
	char auth[5];
	char aupass[7];
	char sqh[21];
	int ret=0;
	char tmpstr[101];

	get_zd_data( DC_AUTH,auth );
	get_zd_data( DC_AUTH_PSW,aupass );

vtcp_log("auth[%s] aupass[%s]",auth,aupass);
	sqh[0]=auth[3];
	strcpy( sqh+1,aupass );

	ret=Auth_inf_Dec_Upd( g_pub_tx.reply,"auth_no=%d",atol(sqh) );
	if( ret ) goto ErrExit;
	ret=Auth_inf_Fet_Upd( &auth_inf_c,g_pub_tx.reply );
	if( ret==100 )
	{
		TRACE
		strcpy( g_pub_tx.reply,"C126");
		goto ErrExit;
	}
	else if( ret ) goto ErrExit;
TRACE
	ret=Auth_inf_Sel( g_pub_tx.reply,&auth_inf_c,"auth_no=%d",atol(sqh) );
	/**状态**/
	if( auth_inf_c.sts[0]=='1' )
	{
		TRACE
		strcpy( g_pub_tx.reply,"S067");
		goto ErrExit;
	}
	else if( auth_inf_c.sts[0]=='2' )
	{
		TRACE
		strcpy( g_pub_tx.reply,"S068");
		goto ErrExit;
	}
TRACE
	/**有效期**/
	if( auth_inf_c.mtr_date<g_pub_tx.tx_date )
	{
		TRACE
		strcpy( g_pub_tx.reply,"S069");
		goto ErrExit;
	}
TRACE
	/**被授权机构**/
	pub_base_strpack(auth_inf_c.br_no);
	if( strlen(auth_inf_c.br_no) && strcmp(auth_inf_c.br_no,g_pub_tx.tx_br_no))
	{
		TRACE
		strcpy( g_pub_tx.reply,"S070");
		goto ErrExit;
	}
	/**交易检查**/
	pub_base_strpack(auth_inf_c.tx_code);
	if( strlen(auth_inf_c.tx_code)==0 )
		;
	else if( strcmp(txcode,auth_inf_c.tx_code) )
	{
		vtcp_log("txcode[%s] inf_tx_code[%s]",txcode,auth_inf_c.tx_code);
		strcpy( g_pub_tx.reply,"S066" );
		goto ErrExit;
	}
TRACE
	/**被授权柜员**/
	pub_base_strpack(auth_inf_c.tel);
	if( strlen(auth_inf_c.tel) && strcmp(auth_inf_c.tel,g_pub_tx.tel))
	{
		TRACE
		strcpy( g_pub_tx.reply,"S071");
		goto ErrExit;
	}

TRACE
	ret=Com_tel_Sel( g_pub_tx.reply,&com_tel_c,"tel='%s'",auth_inf_c.auth_tel );
	if( ret ) goto ErrExit;

TRACE
	if( com_tel_c.csts[0]!='0' && com_tel_c.csts[0]!='1' 
		&& com_tel_c.csts[0]!='2' )
	{
		vtcp_log("TEL_STS[%s]",com_tel_c.csts);
		strcpy( g_pub_tx.reply,"S062");
		goto ErrExit;
	}

	if( strlen(vlvl_flag)==0 || strstr(vlvl_flag,com_tel_c.lvl) )
		;
	else
	{
		vtcp_log("S063 [%s][%s]",vlvl_flag,com_tel_c.lvl );
		strcpy( g_pub_tx.reply,"S063");
		goto ErrExit;
	}
TRACE

TRACE
	/**帐号检查**/
	ret=Auth_chk_def_Sel( g_pub_tx.reply,&auth_chk_def_c,
		"tx_code='%s' and seqn=10001",
		auth_inf_c.tx_code );
	if( ret && ret!=100 ) goto ErrExit;
	else if( !ret )
	{
		get_zd_data(auth_chk_def_c.data_code,tmpstr);
		pub_base_strpack( auth_inf_c.ac_no );
		if( strlen(auth_inf_c.ac_no) && strcmp(auth_inf_c.ac_no,tmpstr) )
		{
			strcpy( g_pub_tx.reply,"S064" );
			goto ErrExit;
		}
	}
TRACE
	/**金额检查**/
	ret=Auth_chk_def_Sel( g_pub_tx.reply,&auth_chk_def_c,
		"tx_code='%s' and seqn=10002",
		auth_inf_c.tx_code );
	if( ret && ret!=100 ) goto ErrExit;
	else if( !ret )
	{
		get_zd_data(auth_chk_def_c.data_code,tmpstr);
		if( pub_base_CompDblVal(atof(tmpstr),auth_inf_c.amt)>0 )
		{
			strcpy( g_pub_tx.reply,"S065" );
			goto ErrExit;
		}
		auth_inf_c.use_amt=atof(tmpstr);
	}
	auth_inf_c.sts[0]='1';
	get_zd_long(DC_DATE,&auth_inf_c.use_date);
	get_zd_long(DC_TRACE_NO,&auth_inf_c.use_trace_no );
	get_zd_data(DC_TEL,auth_inf_c.use_tel );
	ret=Auth_inf_Upd_Upd(auth_inf_c,g_pub_tx.reply);
	if( ret ) goto ErrExit;
	Auth_inf_Clo_Upd( );

TRACE
	vtcp_log("授权号检查成功[%s%s]",auth,aupass);
	return 0;
ErrExit:
TRACE
	return -1;
}
pub_comm_chk_loa( char *txcode,char *vact_flag,char *vlvl_flag )
{
	struct auth_inf_c auth_inf_c;
	struct auth_chk_def_c auth_chk_def_c;
	struct com_tel_c com_tel_c;
	char auth[5];
	char aupass[7];
	char sqh[21];
	int ret=0;
	char tmpstr[101];
	double amt=0;

	get_zd_data( DC_AUTH,auth );
	get_zd_data( DC_AUTH_PSW,aupass );

vtcp_log("auth[%s] aupass[%s]",auth,aupass);

	ret=Com_tel_Sel( g_pub_tx.reply,&com_tel_c,"tel='%s'",auth );
	if( ret ) 
	{
		strcpy( g_pub_tx.reply,"S076");
		goto ErrExit;
	}

	if( com_tel_c.csts[0]!='0' && com_tel_c.csts[0]!='1' 
		&& com_tel_c.csts[0]!='2' )
	{
		vtcp_log("TEL_STS[%s]",com_tel_c.csts);
		strcpy( g_pub_tx.reply,"S062");
		goto ErrExit;
	}
	pub_base_EnDes(g_pub_tx.tx_date,"",aupass);

vtcp_log("auth[%s] aupass[%s]",auth,aupass);
	if( strcmp(aupass,com_tel_c.pwd) )
	{
		strcpy( g_pub_tx.reply,"S075");
		goto ErrExit;
	}

vtcp_log("auth[%s] aupass[%s] lvl[%s]in[%s]",auth,aupass,vlvl_flag,com_tel_c.lvl);
	if( strlen(vlvl_flag)==0 || strstr(vlvl_flag,com_tel_c.lvl) )
		;
	else
	{
		vtcp_log("S063 [%s][%s]",vlvl_flag,com_tel_c.lvl );
		strcpy( g_pub_tx.reply,"S063");
		goto ErrExit;
	}
vtcp_log("Auth[%s] aupass[%s]",auth,aupass);
TRACE
	ret=pub_auth_get_amt( txcode,&amt );
	if( ret ) goto ErrExit;
vtcp_log("aUth[%s] aupass[%s] [%.2lf]",auth,aupass,amt);
TRACE
	ret=pub_com_tel_amt_auth_chk( auth,amt );
	if( ret ) 
	{
		strcpy( g_pub_tx.reply,"S078" );
		goto ErrExit;
	}

	vtcp_log("本地授权成功[%s%s]",auth,aupass);
	return 0;
ErrExit:
TRACE
	return -1;
}
pub_auth_get_amt( char *txcode,double *amt )
{
	struct auth_chk_def_c auth_chk_def_c;
	char tmpstr[81];
	int ret=0;

	/**金额检查**/
	ret=Auth_chk_def_Sel( g_pub_tx.reply,&auth_chk_def_c,
		"tx_code='%s' and seqn=10002",
		txcode );
	if( ret && ret!=100 ) goto ErrExit;
	else if( !ret )
	{
		pub_base_strpack(auth_chk_def_c.data_code);
		get_zd_data(auth_chk_def_c.data_code,tmpstr);
		*amt=atof(tmpstr);
	}
	else
	{
		*amt=0.0;
	}
	return 0;
ErrExit:
	return 1;
}
pub_com_tel_amt_auth_chk( char *tel,double amt )
{
	struct auth_auth_auth_c auth_auth_auth_c;
	struct auth_auth_lmt_c auth_auth_lmt_c;
	int ret=0;

	MEMSET_DEBUG( &auth_auth_auth_c,0,sizeof(auth_auth_auth_c) );

	ret=Auth_auth_auth_Sel( g_pub_tx.reply,&auth_auth_auth_c,
		"tel='%s'",tel );
	if( ret==100 )
	{
		/**没有的默认为普通柜员**/
		strcpy( auth_auth_auth_c.tel,tel );
		strcpy( auth_auth_auth_c.auth_code,"PT1" );
	}
	else if( ret ) goto ErrExit;
vtcp_log("auth lvl [%s]",auth_auth_auth_c.auth_code );
	ret=Auth_auth_lmt_Sel( g_pub_tx.reply,&auth_auth_lmt_c,
		"auth_code='%s'",auth_auth_auth_c.auth_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"交易限额定义缺失[%s]",auth_auth_auth_c.auth_code);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S071" );
		goto ErrExit;
	}
	else if( ret ) goto ErrExit;
	
vtcp_log("auth lvl [%s][%.2lf]",tel,auth_auth_lmt_c.max_amt );
	if( pub_base_CompDblVal(amt,auth_auth_lmt_c.max_amt)>0 )
	{
		strcpy( g_pub_tx.reply,"S073" );
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}
