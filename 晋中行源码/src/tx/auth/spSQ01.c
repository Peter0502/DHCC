/*************************************************
* 文 件 名:  spSQ01.c
* 功能描述： 交易前授权子交易。
*************************************************/
#define MYRETERR if(ret){ \
	sprintf(acErrMsg,"[%d]return error[%d]",__LINE__,ret); \
	WRITEMSG \
	goto ErrExit; \
	}
#define SHOW_MSG \
	memset(fd13,0,sizeof(fd13)); \
	get_zd_data(DC_GET_MSG,fd13); \
	vtcp_log("%s,%d,验证授权级别WYW[%s]",__FILE__,__LINE__,fd13); \

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "tx_log_c.h"
#include "auth_def_c.h"
#include "auth_code_c.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_chk_def_c.h"
#include "auth_inf_c.h"
#include "auth_tel_auth_c.h"
#include "auth_auth_lmt_c.h"
#include "auth_ac_rec_c.h"

#include "auth_cond_c.h"
#include "auth_item_c.h"

struct auth_item_c g_auth_item;
struct auth_cond_c g_auth_cond;
extern struct tx_def_c g_tx_def_c;
struct auth_code_c g_auth_code;
int over_ind=0;  /*条件解释结束标志*/
char g_lvl[21]; /**需要的授权人级别列表**/
char g_tx[2]; /*9提交后提示授权，输入内容后同一个画面再提交*/
	int if_val=1; /**条件成立标志1成立0否**/
	int fh_ret=0; /**复核检查结果**/
	int sq_ret=0; /**授权检查结果**/
	int ts_ret=0; /**tisi检查结果**/


char fd13[51]; /* by Wang Yongwei For Test 20100412*/
int spSQ01()
{
	struct auth_code_c vauth_code;
	char s_auth_code[11];
	char tstr[11];
	int	ret;
	int need_sq=1;
  char cOritel[5];
	over_ind=0;
	if_val=1;
	fh_ret=0;
	sq_ret=0;
	/*********Print for Test********/
	char cTmpAcno[20];
	memset(cTmpAcno, 0, sizeof(cTmpAcno));
	memset(cOritel,0,sizeof(cOritel));
	get_zd_data("0370", cTmpAcno);
	get_zd_data("1241",cOritel); /**修改92为1241**/
	vtcp_log(" ChenMing cTmpAcno=====[%s]", cTmpAcno);
	/*********Print for Test********/

	/** begin 网银贷款还本跳过授权和复核 liuzengying 20141122 **/
	char netbank[5] = {0};
	get_zd_data("0100", netbank);
	if ( memcmp(netbank,"netb",4) == 0 )
	{
		vtcp_log("netbank=[%s],网银渠道不需授权和复核!",netbank);
		goto OkExit;
	}
	else if ( memcmp(netbank,"mobb",4) == 0 )
	{
		vtcp_log("netbank=[%s],手机银行渠道不需授权和复核!",netbank);
		goto OkExit;
	}
	else
	{
		vtcp_log("非网银渠道都需要检查授权和复核!");
	}		
	/** end 网银贷款还本跳过授权和复核 liuzengying 20141122 **/
	

	if( g_tx_def_c.tx_type[0]=='4' && strcmp(g_tx_def_c.tx_code,"9800") )
	{
		vtcp_log("快查不需要授权code[%s]type[%s]",g_tx_def_c.tx_code,g_tx_def_c.tx_type);
		goto OkExit;
	}

	/* 初始化结构 */
	pub_base_sysinit();

	memset( s_auth_code,0,sizeof(s_auth_code) ); /* Added by Chenming 2007-1-11 11:51 */
	
	memset( g_lvl,0,sizeof(g_lvl) );
	memset( &g_auth_item,0,sizeof(g_auth_item) );
	memset( &g_auth_cond,0,sizeof(g_auth_cond) );
	get_zd_data( DC_TX_CODE,g_pub_tx.tx_code );
	get_zd_data( DC_AUTH_CODE,s_auth_code );
  vtcp_log("JJJJ交易代码:[%s] 授权码:[%s] 长度:[%d]",g_pub_tx.tx_code,s_auth_code,strlen(s_auth_code));

	if( strlen(s_auth_code)!=8 && atol(s_auth_code) ) 
	{
		sprintf(acErrMsg,"不明授权码[%s]",s_auth_code );
		WRITEMSG
		strcpy( g_pub_tx.reply,"H063" );
		goto ErrExit;
	}
	else if( strlen(s_auth_code)==8 ) 
	{
		g_pub_tx.auth_code=atol(s_auth_code+1); 
		g_tx[0]=s_auth_code[0]; g_tx[1]='\0';
		/**g_tx 9交易后提交**/
		/**g_tx 8 F12 **/
		/**g_tx 7 F11 复核**/
		/**g_tx 6 5703联动--授权 **/
		set_zd_data( DC_AUTH_CODE,s_auth_code+1 );

		vtcp_log("G_TX[%s] [%d]",g_tx,g_pub_tx.auth_code );
	}
	else
	{
		/**g_Tx 0 开始交易**/
		g_pub_tx.auth_code=atol(s_auth_code); 
		g_tx[0]='0'; g_tx[1]='\0';
	}
	get_zd_long( DC_TRACE_NO,&g_pub_tx.trace_no );
	get_zd_data( DC_TEL,g_pub_tx.tel );
	get_zd_data( DC_CHK,g_pub_tx.chk );
	vtcp_log("%s,%d,复核交易转换前交易柜员[%s],授权柜员[%s]\n",__FILE__,__LINE__,g_pub_tx.tel,g_pub_tx.chk);	
	get_zd_data( DC_AUTH,g_pub_tx.auth );
	get_zd_data( DC_TX_BR_NO,g_pub_tx.tx_br_no );
vtcp_log("授权传进的值为:DC_AUTH:[%s],DC_TRACE_NO:[%ld],DC_TX_BR_NO:[%s],[%s]--[%d],g_tx[%s]",g_pub_tx.auth,g_pub_tx.trace_no,g_pub_tx.tx_br_no,__FILE__,__LINE__,g_tx);

	/* 调利率交易必须高级授权 * 实现不了 问石总
    if( memcmp(g_pub_tx.tx_code,"2611",4)==0 &&  strlen(g_pub_tx.auth)==0 && strncmp(g_pub_tx.auth,"@@@",3) && g_tx[0]!='0')
    {
        sprintf(acErrMsg,"不明授权码[%s]",s_auth_code );
        WRITEMSG
        strcpy( g_pub_tx.reply,"H063" );
        goto ErrExit;
    }
	*/
	/**授权检查：分析操作员想干什么**/
	switch( g_tx[0] )
	{
		/***提示后完成***/
		case '5':
			ret=read_auth_code(); /**读取过去记录**/
			MYRETERR
			g_auth_code.sts[0]='3';  /**授权完成**/
			break;
		/**g_tx 9交易后提交 本人**/
		/**g_tx 8 F12输入授权码 本人**/
		case '9':
		case '8':
			ret=read_auth_code(); /**读取过去记录**/
			MYRETERR

			if( g_auth_code.sts[0]=='3' || g_auth_code.sts[0]=='4' )
			{
				sprintf(acErrMsg,"已经授权");
				WRITEMSG
				break;
			}

			if( strcmp(g_pub_tx.tel,g_auth_code.tel) )
			{
				sprintf(acErrMsg,"非发起人[%s][%s]",
							g_pub_tx.tel,g_auth_code.tel);
				strcpy( g_pub_tx.reply,"Z108" );
				goto ErrExit;
			}
			/**输入了授权码**/
			if( strlen(g_pub_tx.auth) && !strncmp(g_pub_tx.auth,"@@@",3) )
			{
				vtcp_log("用授权码完成交易code[%s]type[%s]",
						g_pub_tx.tx_code,g_tx_def_c.tx_type);

				ret=pub_sq_done_code();
				if( ret ) goto ErrExit;

				if( !strcmp(g_pub_tx.tel,g_pub_tx.auth) )
				{
					TRACE
					strcpy( g_pub_tx.reply,"Z109" );
					goto ErrExit;
				}
			}
			/**直接输入了授权人**/
			else if( strlen(g_pub_tx.auth) )
			{
				if( !strcmp(g_pub_tx.tel,g_pub_tx.auth) )
				{
					TRACE
					strcpy( g_pub_tx.reply,"Z109" );
					goto ErrExit;
				}
				vtcp_log("用授权人完成交易code[%s]type[%s]",
						g_pub_tx.tx_code,g_tx_def_c.tx_type);
				ret=pub_sq_done_tel();
				if( ret ) 
				{
					TRACE
					goto ErrExit;
				}
			}
			else
			{
				/*
				sprintf(acErrMsg,"no auth in this");
				WRITEMSG
				strcpy( g_pub_tx.reply,"Z10F" );
				goto ErrExit;
				*/
					sprintf(acErrMsg,"流水号 %07d", g_pub_tx.auth_code);
					set_zd_data(DC_GET_MSG,acErrMsg);
					WRITEMSG
					set_zd_long(DC_AUTH_CODE,g_pub_tx.auth_code);
					strcpy( g_pub_tx.reply,"Z003" );
                   /*added by duhuanping 2010/11/09 start */
					set_zd_data(DC_GET_MSG,g_lvl);
                    /*added by duhuanping 2010/11/09 end */
					goto ErrExit;
			}
			g_auth_code.sts[0]='3';  /**授权完成**/

			break;
		/**g_tx 6 5703联动--授权 **/
		case '6':
			ret=read_auth_code(); /**读取过去记录**/
			MYRETERR
			if( !strcmp(g_pub_tx.tel,g_auth_code.tel) )
			{
				sprintf(acErrMsg,"不能为自己授权auth[%s]tel[%s]",
							g_pub_tx.tel,g_auth_code.tel);
				WRITEMSG
				strcpy( g_pub_tx.reply,"Z109" );
				goto ErrExit;
			}
			set_zd_data( DC_AUTH,g_pub_tx.tel );
			set_zd_data( DC_AUTH_PSW,"------" );

			if( g_auth_code.sts[0]=='3' || g_auth_code.sts[0]=='4' )
			{
				TRACE
				strcpy( g_pub_tx.reply,"Z009" );
				goto ErrExit;
			}

			ret=pub_sq_done_tel();
			if( ret ) 
			{
				TRACE
				goto ErrExit;
			}

			g_auth_code.sts[0]='3';  /**授权完成**/

			ret=Auth_code_Upd_Upd( g_auth_code,g_pub_tx.reply );
			MYRETERR
			/*
			Auth_code_Clo_Upd();
			strcpy( g_pub_tx.reply,"Z006" );
			goto OkExit;
			*/
				need_sq=1;
			break;
		/**g_tx 7 F11 复核**/
		case '7':
			ret=read_auth_code(); /**读取过去记录**/
			MYRETERR
			if( !strcmp(g_pub_tx.chk,g_auth_code.tel) )
			{
				sprintf(acErrMsg,"不能为自己复核auth[%s]tel[%s]",
							g_pub_tx.chk,g_auth_code.tel);
				WRITEMSG
				strcpy( g_pub_tx.reply,"Z102" );
				goto ErrExit;
			}
			vtcp_log("tx.tel is [%s],auth_code.tel [%s]",g_pub_tx.tel,g_auth_code.tel);
			if( strcmp(g_pub_tx.tel,g_auth_code.tel) )
			{
				TRACE
				strcpy( g_pub_tx.reply,"H063" );
				goto ErrExit;
			}
			if( g_auth_code.auth_ind[0]!='3' && g_auth_code.auth_ind[0]!='4' )
				/**通过授权检查 =3 **/
			{
				sprintf( acErrMsg,"请首先进行授权" );
				WRITEMSG
				strcpy( g_pub_tx.reply,"Z008" );
				goto ErrExit;
			}
			break;
		/**g_Tx 0 开始交易**/
		case '0':
			vtcp_log("进行授权检查code[%s]type[%s]",
				g_pub_tx.tx_code,g_tx_def_c.tx_type);
			ret=pub_sq_chk( 3,0 );
			if( ret>0 )
			{
				SHOW_MSG
				ret=read_auth_code(); /**读取过去记录**/
				MYRETERR

				/**输入了授权码**/
				if( strlen(g_pub_tx.auth) && !strncmp(g_pub_tx.auth,"@@@",3) )
				{
					SHOW_MSG
					vtcp_log("用授权码完成交易code[%s]type[%s]",
						g_pub_tx.tx_code,g_tx_def_c.tx_type);

					ret=pub_sq_done_code();
					if( ret ) goto ErrExit;

					if( !strcmp(g_pub_tx.tel,g_pub_tx.auth) )
					{
						strcpy( g_pub_tx.reply,"Z109" );
						goto ErrExit;
					}
				}
				/**直接输入了授权人**/
				else if( strlen(g_pub_tx.auth) )
				{
					SHOW_MSG
					if( !strcmp(g_pub_tx.tel,g_pub_tx.auth) )
					{
						strcpy( g_pub_tx.reply,"Z109" );
						goto ErrExit;
					}
				}
				else if( ts_ret )
				{
					SHOW_MSG
					pub_base_strpack(g_auth_cond.mo);
					sprintf( acErrMsg,"%s",g_auth_cond.mo );
					/* set_zd_data(DC_GET_MSG,acErrMsg); by Win liuyong 20100415 */
					set_zd_long(DC_AUTH_CODE,g_pub_tx.auth_code);
					strcpy( g_pub_tx.reply,"Z008" );
					goto ErrExit;
				}
				else
				{
					SHOW_MSG
					pub_base_strpack(g_auth_cond.mo);
					sprintf(acErrMsg,"流水号 %07d %s", g_pub_tx.auth_code,
						g_auth_cond.mo);
					/* set_zd_data(DC_GET_MSG,acErrMsg); * by Win liuyong 20100415*/
					set_zd_long(DC_AUTH_CODE,g_pub_tx.auth_code);
					strcpy( g_pub_tx.reply,"Z003" );
					goto ErrExit;
				}
			}
			else if( !ret )
			{
				sprintf( acErrMsg,"不需要授权" );
				WRITEMSG
				need_sq=0;
			}
			else 
				goto ErrExit;
			break;
		default :
			strcpy( g_pub_tx.reply,"H063" );
			goto ErrExit;
			break;
	}

	SHOW_MSG
	vtcp_log("开始复核的检查");
	/***开始复核的检查***/
	/*need_sq 1--有auth_code 0--无auth_code**/
	if( g_tx[0]=='7' ) /**复核员操作 F11**/
	{
TRACE
		if( need_sq==0 )
		{
			strcpy( g_pub_tx.reply,"H063" );
			goto ErrExit;
		}
TRACE
		g_auth_code.sts[0]='9';
		strcpy( g_auth_code.chk,g_pub_tx.chk );
		g_auth_code.done_trace_no=g_pub_tx.trace_no;
TRACE
		ret=Auth_code_Upd_Upd( g_auth_code,g_pub_tx.reply );
		MYRETERR
TRACE
		Auth_code_Clo_Upd();

		vtcp_log("TEL[%s] CHK[%s] AUTH[%s]",
		g_auth_code.tel,g_auth_code.chk,g_auth_code.auth );
		set_zd_data( DC_AUTH,g_auth_code.auth );
		set_zd_data( DC_CHK,g_auth_code.chk );
		/****Add by SSH,2006.10.12,重新设置交易柜员,以把帐记在复核员上**/
		set_zd_data( DC_TEL,g_pub_tx.chk );
		strcpy(g_pub_tx.tel,g_pub_tx.chk);	
		vtcp_log("%s,%d,复核交易转换后交易柜员[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);	
		/*********************************************************************/
		goto OkExit;
	}
	else
	{
		ret=pub_sq_chk( 2,need_sq );
		if( ret>0 )
		{
			sprintf(acErrMsg,"需要复核 %d",need_sq);
			WRITEMSG
			
			if( need_sq )
			{
				ret=Auth_code_Upd_Upd( g_auth_code,g_pub_tx.reply );
				MYRETERR
				Auth_code_Clo_Upd();
			}

			sprintf(acErrMsg,"流水号 %07d", g_pub_tx.auth_code);
			set_zd_data(DC_GET_MSG,acErrMsg);
			set_zd_long( DC_AUTH_CODE,g_pub_tx.auth_code );
			strcpy( g_pub_tx.reply,"Z002" );
			goto ErrExit;
		}
		else if( ret<0 ) 
		{
			TRACE
			goto ErrExit;
		}
		else
		{
			sprintf(acErrMsg,"不必复核");
			WRITEMSG
			if( need_sq==1 )
			{
				if( g_tx[0]!='6' ) 
				{
					g_auth_code.sts[0]=='9'; /*完成*/
					g_auth_code.done_trace_no=g_pub_tx.trace_no;
				}
				ret=Auth_code_Upd_Upd( g_auth_code,g_pub_tx.reply );
				MYRETERR
				Auth_code_Clo_Upd();
			}

			if( g_tx[0]=='6' )
			{
				strcpy( g_pub_tx.reply,"Z006" );
				goto ErrExit;
			}
		}
	}
	SHOW_MSG	
	vtcp_log("%s,%d,TEL[%s] CHK[%s] AUTH[%s]",__FILE__,__LINE__,
	g_auth_code.tel,g_auth_code.chk,g_auth_code.auth );
	set_zd_data( DC_AUTH,g_auth_code.auth );
	set_zd_data( DC_CHK,g_auth_code.chk );
	
OkExit:
	SHOW_MSG	
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_AUTH_CODE," ");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	SHOW_MSG	
	if( !strcmp(g_pub_tx.reply,"0000") )
		strcpy(g_pub_tx.reply,"Z204");
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	if( g_tx[0]=='9' ) set_zd_long( DC_AUTH_CODE,g_pub_tx.auth_code );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int read_auth_code()
{
	int ret=0;

	ret=Auth_code_Dec_Upd( g_pub_tx.reply,"auth_no=%d",g_pub_tx.auth_code );
	MYRETERR
	ret=Auth_code_Fet_Upd( &g_auth_code,g_pub_tx.reply );
	if( ret==100 )
	{
		vtcp_log("此流水号码不存在[%d]",g_pub_tx.auth_code);TRACE
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}
	MYRETERR
		
	if( g_auth_code.sts[0]=='9' )
	{
		vtcp_log("已经交易完成了"); TRACE
		strcpy( g_pub_tx.reply,"Z107" );
		goto ErrExit;
	}

	strcpy( g_lvl,g_auth_code.lvl );

	ret=Auth_item_Sel( g_pub_tx.reply,&g_auth_item,
		"tx_code='%s' and seqn=%d",g_auth_code.tx_code,g_auth_code.cond_seqn );
	MYRETERR

OkExit:
	return 0;
ErrExit:
    return -1;
}
/**授权检查*mode 3 检查授权 2检查复核
havcode 1有记录 0无记录**/
int pub_sq_chk( int mode,int havcode )
{
	struct auth_code_c vauth_code;
	struct auth_cond_c v_auth_cond;
	int needauth=0;
	int ret=0;
	char cond_code[2];

	if( mode==2 ) strcpy( cond_code,"F" );
	else strcpy( cond_code,"S" );

	vtcp_log( "CCCC tx_code='%s' order by tx_code,seqn",g_pub_tx.tx_code );

	ret=Auth_item_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by tx_code,seqn",
		g_pub_tx.tx_code );
	MYRETERR

	while(1)
	{
		ret = Auth_item_Fet_Sel( &g_auth_item ,g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR

vtcp_log("tx[%s-%s-%d]",cond_code,g_auth_item.tx_code,g_auth_item.seqn);

		ret=Auth_cond_Dec_Sel( g_pub_tx.reply,
			"type='%s' and tx_code='%s' and seqn=%d order by no",
			cond_code,g_auth_item.tx_code,g_auth_item.seqn );
		MYRETERR
		while(1)
		{
			ret = Auth_cond_Fet_Sel( &v_auth_cond ,g_pub_tx.reply );
			if( ret==100 ) break;
			MYRETERR

TRACE
			memcpy( &g_auth_cond,&v_auth_cond,sizeof(v_auth_cond) );

			ret=pub_sq_sub_chk( v_auth_cond.cond );
TRACE
vtcp_log("nedd[%d]type[%s]txcode[%s]seqn[%d]-[%d]",
	ret,cond_code,g_auth_item.tx_code,g_auth_item.seqn,mode);
			if( ret==0 ) needauth=0;
			else if( ret==mode ) 
			{
				needauth=ret;
				break;
			}
			else if( ret ) goto ErrExit;
		}
		Auth_cond_Clo_Sel();

		if( needauth ) break;
	}
	Auth_item_Clo_Sel();

vtcp_log("SQCHK needauth[%d] havcode[%d]",needauth,havcode );
	if( needauth && !havcode )
	{
			memset( &vauth_code,0,sizeof(vauth_code) );
			pub_comm_make_auth_code( '3',&vauth_code.auth_no );
			g_pub_tx.auth_code=vauth_code.auth_no;
vtcp_log("AUTH[%d] insert",g_pub_tx.auth_code);			
			/**写入授权登记簿**/
			strcpy( vauth_code.tx_code,g_pub_tx.tx_code );
			vauth_code.trace_no = g_pub_tx.trace_no;
			vauth_code.cond_seqn = g_auth_item.seqn;
			strcpy( vauth_code.br_no,g_pub_tx.tx_br_no );
			vauth_code.done_trace_no=0;
			vauth_code.auth_ind[0]='3';
			vauth_code.sts[0]='0';
			if( mode==2 ) vauth_code.sts[0]='4';
			strcpy( vauth_code.tel,g_pub_tx.tel );
			g_lvl[10]='\0';
			strcpy( vauth_code.lvl,g_lvl );

			vtcp_log(" LSH[%d]",vauth_code.auth_no );
                        
                        /*added by liuyong 2010-4-15 start */
                        vtcp_log("%s,%d,LIUYONG授权级别:[%s]",__FILE__,__LINE__,g_lvl);
                        set_zd_data(DC_GET_MSG,g_lvl);
                        /*added by liuyong 2010-4-15 end */

			memcpy( &g_auth_code,&vauth_code,sizeof(g_auth_code) );
			vtcp_log("<<<<<<<<这里向授权表里插入值[%d]",vauth_code.auth_no);
			if( Auth_code_Ins( vauth_code,g_pub_tx.reply ) )
				goto ErrExit;
	}
	else if( needauth )
	{
		g_auth_code.sts[0]='4';
	}
OkExit:
	return needauth; /**1需要授权0不需要授权**/
ErrExit:
    return -1;
}
int pub_sq_sub_chk( char * state )
{
	int i=0,l=0;
	int len=0;
	int flag=0;
	char cmd[21],ocmd[21];
	char parm[201];
	int cmd_i=0;
	int starti=0,deep=0;

	ldchar( state,strlen(state),state );
	memset( cmd,0,sizeof(cmd) ); cmd_i=0;
	memset( ocmd,0,sizeof(ocmd) );

vtcp_log("SQ_SUB_CHK[%s]",state);

	len=strlen(state);

	i=0;
	while( i<len && state[i]!='\0' && !over_ind )
	{
		switch( state[i] )
		{
			case '(':
				i++;
				starti=i;
				deep++;
				while( i<len ) 
				{
					if( state[i]=='(' )
						deep++;
					else if( state[i]==')' )
						deep--;
					if( deep==0 ) 
					{
						l=i-starti;
						memset( parm,0,sizeof(parm) );
						strncpy( parm,state+starti,l );
						/**cl done cmd(parm)***/
vtcp_log("OOOOOOOcmd[%s]parm[%s]",cmd,parm);
						if( !strcmp(cmd,"if") )
						{
							if_val=get_cond_val( parm );
vtcp_log("       if_val=[%d]",if_val);
						}
						else if( !strcmp(cmd,"fh") ) /**函数fh用于检查是否要复核**/
						{
							fh_ret=get_fh_val( parm );
							if( fh_ret ) over_ind=1;
						}
						else if( !strcmp(cmd,"ts") )
						{
							ts_ret=1;
							sq_ret=1;
							over_ind=1;
						}
						else if( !strcmp(cmd,"sq") )
						{
vtcp_log("sqOOOOOcmd[%s]parm[%s]",cmd,parm);
							sq_ret=get_sq_val( parm );
							if( sq_ret ) over_ind=1;
vtcp_log("sqOOOoOcmd[%d]",sq_ret);
						}
						else if( !strcmp(cmd,"no") ) /**函数no表示不需要授权(复核)**/
						{
							sq_ret=fh_ret=0;
							over_ind=1;
						}
						else
							;
						break;
					}
					i++;
				}
				memset( cmd,0,sizeof(cmd) ); cmd_i=0;
				break;
			case '{': /*}*/
				i++;
				starti=i;
				deep++;
				while( i<len ) 
				{
					if( state[i]=='{' )
						deep++;
					else if( state[i]=='}' )
						deep--;
					if( deep==0 ) 
					{
						l=i-starti;
						memset( parm,0,sizeof(parm) );
						strncpy( parm,state+starti,l );
						/**cl done cmd(parm)***/
vtcp_log("OOOOOOO[%s(%s)]",cmd,parm);
						if( !strcmp(cmd,"else") && !if_val ) 
							pub_sq_sub_chk( parm );
						else if( if_val ) 
							pub_sq_sub_chk( parm );
						break;
					}
					i++;
				}
				break;
			default :
				cmd[cmd_i]=state[i];
				cmd_i++;
				break;
		}
		i++;
	}

	TRACE
	vtcp_log("NEED SQ[%d] FH[%d] TS[%d]",sq_ret,fh_ret,ts_ret); 
	
	if( sq_ret ) return 3;
	if( fh_ret ) return 2;

	return 0;
}
	/**函数if()的解释程序：用于解释if后的条件**/
	int get_cond_val( char * state )
	{
		int rlt=0,val=0;
		int len=0;
		int starti=0,deep=0;
		int i=0;
		char ttlval[201]; /*计算结果*/
		char ttltype='@';
		char tmpstr[201];
		char str[201]; /*当前表达式*/
		char strtype='@';
		char parm[201];
		int str_i=0;
		char curopr=' ';
		char tmpchr;

		memset( str,0,sizeof(str) );
		memset( parm,0,sizeof(parm) );
		memset( tmpstr,0,sizeof(tmpstr) );
		memset( ttlval,0,sizeof(ttlval) );
		strcpy( ttlval,"0" );
		len=strlen(state);

vtcp_log(" GETCONDVAL-------begin");
vtcp_log("				%s",state);
		rlt=0;

		i=0;
		while( i<len && state[i]!='\0' )
		{
/*vtcp_log("III[%d][%c]",i,state[i]); */
			switch( state[i] )
			{
				case '(':
					i++;
					starti=i;
					deep++;
					while( i<len ) 
					{
						if( state[i]=='(' )
							deep++;
						else if( state[i]==')' )
							deep--;
						if( deep==0 ) 
						{
							memset( parm,0,sizeof(parm) );
							strncpy( parm,state+starti,i-starti );
							val=get_cond_val( parm );
							if( val ) strcpy( tmpstr,"1" );
							else strcpy( tmpstr,"0" );
		        			opr( curopr,tmpstr,strtype,ttlval,&ttltype );
vtcp_log("VAL [%s] rlt[%d]",parm,val);
							break;
						}
						i++;
					}
					break;
				case '>':
					if( state[i+1]=='=' ) /* >= */
					{
						curopr=']';
						i++;
					}
					else
						curopr='>';
					break;
				case '=':
					if( state[i+1]==state[i] ) i++; /**处理&&和||**/
					curopr='=';
					break;
				case '<':
					if( state[i+1]=='>' ) /* <> */
					{
						curopr='~';
						i++;
					}
					else if( state[i+1]=='=' ) /* >= */
					{
						curopr='[';
						i++;
					}
					else
						curopr='<';
					break;
				case '!':
					if( state[i+1]=='=' ) /* != */
					{
						curopr='~';
						i++;
					}
					else
						curopr='~';
					break;
				case '&':
				case '|':
					if( state[i+1]==state[i] ) i++; /**处理&&和||**/
					curopr=state[i];
vtcp_log("CCC [%c]",curopr );
					break;
				case '+':
				case '-':
				case '/':
				case '*':
					curopr=state[i];
					break;
				case '%': /**帐户累计数**/
				case '@': /**字符串**/
				case '#': /**整数**/
				case '$': /**金额**/
					memset( str,0,sizeof(str) );
					tmpchr=state[i];
					i++;
					starti=i;
					while( i<len ) 
					{
						if( !( (state[i+1]>='A' && state[i+1]<='Z') ||
							(state[i+1]>='0' && state[i+1]<='9') ||
							(state[i+1]>='a' && state[i+1]<='z') ) )
						{
							memset( tmpstr,0,sizeof(tmpstr) );
							strncpy( tmpstr,state+starti,i+1-starti );
							if( !strcmp(tmpstr,"ac") || !strcmp(tmpstr,"acno") ||
								!strcmp(tmpstr,"zh") )
								strcpy( tmpstr,g_auth_item.code_ac );
							else if( !strcmp(tmpstr,"amt") || !strcmp(tmpstr,"je") )
								strcpy( tmpstr,g_auth_item.code_amt );
							else if( !strcmp(tmpstr,"dc") || !strcmp(tmpstr,"jd") )
								strcpy( tmpstr,g_auth_item.code_dc );
							else if( !strcmp(tmpstr,"ct") || !strcmp(tmpstr,"xz") )
								strcpy( tmpstr,g_auth_item.code_ct );
							else if( !strcmp(tmpstr,"xx") )
								strcpy( tmpstr,g_auth_item.code_xx );
							else if( !strcmp(tmpstr,"yy") )
								strcpy( tmpstr,g_auth_item.code_yy );
							else if( !strcmp(tmpstr,"zz") )
								strcpy( tmpstr,g_auth_item.code_zz );
		        			opr( tmpchr,tmpstr,'0',str,&strtype );

							ttltype=strtype;
vtcp_log("		GET $%s [%s] ttl[%c]str[%c]",tmpstr,str,ttltype,strtype );
		        			opr( curopr,str,strtype,ttlval,&ttltype );
vtcp_log("         tt[%s] ",ttlval );
							break;
						}
						i++;
					}
					break;
				default :
					if( (state[i]>='A' && state[i]<='Z') ||
						(state[i]>='0' && state[i]<='9') ||
						(state[i]=='.') ||
						(state[i]=='-') ||
						(state[i]>='a' && state[i]<='z') )
					{
vtcp_log("OTHER char[%c]",state[i] );
						starti=i;
						i++;
						if( i==len )
						{
							memset( str,0,sizeof(str) );
							strncpy( str,state+starti,i-starti );
vtcp_log("CHANG LIANG[%s]",str);
							strtype='0'; /**未知类型**/
		        			opr( curopr,str,strtype,ttlval,&ttltype );
vtcp_log("sq chK[%s] rlt[%s][%d]",state,ttlval,rlt);
						}
						while( i<len ) 
						{
							if( !( (state[i+1]>='A' && state[i+1]<='Z') ||
								(state[i+1]>='0' && state[i+1]<='9') ||
								(state[i+1]>='a' && state[i+1]<='z') ) )
							{
								memset( str,0,sizeof(str) );
								strncpy( str,state+starti,i+1-starti );
vtcp_log("CHANG LIANG[%s]",str);
		vtcp_log("SQ curopr[%c] str[%s] strtype[%c] ttlval[%s] ttltype[%c]",
			curopr,str,strtype,ttlval,ttltype );
								strtype='0'; /**未知类型**/
		        				opr( curopr,str,strtype,ttlval,&ttltype );
		vtcp_log("sq chk[%s] rlt[%s][%d]",state,ttlval,rlt);
								break;
							}
							i++;
						}
					}
			}
			i++;
			
		}

		vtcp_log("SQ CHK[%s] rlt[%s][%d]",state,ttlval,rlt);
		if( atol(ttlval) ) rlt=1;
		else rlt=0;
vtcp_log(" GETCONDVAL-------end[%d]",rlt);

			return rlt;
		ErrExit:
			return -1;
	}
	/**运算符opr当前值curval运算结果ttlval结果类型ttltype**/
		        int opr( curopr,curval,curtype,ttlval,ttltype )
		        	char curopr;
					char *curval;
					char curtype;
					char *ttlval;
					char *ttltype;
				{
					double tmpdb=0.00,ttldb=0.00;
					long tmplg=0,ttllg=0;
					char tmpstr[201],ttlstr[201];

		      vtcp_log("begin call opr[%c][%s][%c][%s][%c]", curopr,curval,curtype,ttlval,*ttltype );
					memset( tmpstr,0,sizeof(tmpstr) );
					memset( ttlstr,0,sizeof(ttlstr) );

					switch( curopr )
					{
						case '%':
							get_ac_sum_val( curval,&ttldb );
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;
						case '@': /**字符串**/
							get_zd_data( curval,ttlstr );
							*ttltype='@';
							break;
						case '#': /**整数**/
							get_zd_long( curval,&ttllg );
							sprintf( ttlstr,"%d",ttllg );
							*ttltype='#';
							break;
						case '$': /**金额**/
							get_zd_double( curval,&ttldb );
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;

						case '+' :
							ttldb=atof(ttlval);
							tmpdb=atof(curval);
							ttldb+=tmpdb;
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;
						case '-' :
							ttldb=atof(ttlval);
							tmpdb=atof(curval);
							ttldb-=tmpdb;
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;
						case '*' :
							ttldb=atof(ttlval);
							tmpdb=atof(curval);
							ttldb*=tmpdb;
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;
						case '/' :
							ttldb=atof(ttlval);
							tmpdb=atof(curval);
							if( tmpdb==0.00 || tmpdb==-0.00 )
								ttldb=0.0;
							else
								ttldb=ttldb/tmpdb;
							sprintf( ttlstr,"%lf",ttldb );
							*ttltype='$';
							break;

						case '~' : /*不等于*/
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)!=atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)!=atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( strcmp(ttlval,curval) )
									strcpy( ttlstr,"1" );
							}
							*ttltype='@';
							break;
						case '=' :
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)==atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)==atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( !strcmp(ttlval,curval) )
									strcpy( ttlstr,"1" );
							}
							break;
						case '>' :
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)>atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)>atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( strcmp(ttlval,curval)>0 )
									strcpy( ttlstr,"1" );
							}
							break;
						case ']' : /*  >=  */
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)>=atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)>=atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( strcmp(ttlval,curval)>=0 )
									strcpy( ttlstr,"1" );
							}
							break;
						case '<' :
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)<atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)<atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( strcmp(ttlval,curval)<0 )
									strcpy( ttlstr,"1" );
							}
							break;
						case '[' : /*  <=  */
							strcpy( ttlstr,"0" );
							if( *ttltype=='$' )
							{
								if( atof(ttlval)<=atof(curval) )
									strcpy( ttlstr,"1" );
							}
							else if( *ttltype=='#' )
							{
								if( atol(ttlval)<=atol(curval) )
									strcpy( ttlstr,"1" );
							}
							else
							{
								if( strcmp(ttlval,curval)<=0 )
									strcpy( ttlstr,"1" );
							}
							break;

						case '!' :
							strcpy( ttlstr,"0" );
							if( atol(curval)==0 )
								strcpy( ttlstr,"1" );
							break;
						case '&' :
							strcpy( ttlstr,"0" );
							if( atol(curval) && atol(ttlval) )
								strcpy( ttlstr,"1" );
							break;
						case '|' :
							strcpy( ttlstr,"0" );
							if( atol(curval) || atol(ttlval) )
								strcpy( ttlstr,"1" );
							break;

					default :
						strcpy( ttlstr,curval );
						break;
						/****ERROR******/
					}

					strcpy( ttlval,ttlstr );
		      vtcp_log("end call OPR[%c][%s][%c][%s][%c]", curopr,curval,curtype,ttlval,*ttltype );
					return 0;
				}
	/***取得帐户累计值***/
	int	get_ac_sum_val( char * curval,double *ttldb )
	{
		char vacno[33];
		int vacseqn;
		char vdc[2];
		char vct[2];
		double vamt;
		struct mdm_ac_rel_c vmdmacrel;
		struct auth_ac_rec_c vauthacrec;
		int ret=0;

vtcp_log("CC-------[%s]",curval);
		*ttldb=0.00;
		pub_base_strpack(g_auth_item.code_ac);
		if( strlen(g_auth_item.code_ac)<4 )
			return 0;
		else
			get_zd_data( g_auth_item.code_ac,vacno );

vtcp_log("CC-----ac[%s]",vacno);
		pub_base_strpack(g_auth_item.code_seqn);
		if( strlen(g_auth_item.code_seqn)<4 ) 
			vacseqn=atol(g_auth_item.code_seqn);
		else
			get_zd_int( g_auth_item.code_seqn,&vacseqn );

vtcp_log("CC-----acseqn[%d]",vacseqn);
		pub_base_strpack(g_auth_item.code_dc);
		if( strlen(g_auth_item.code_dc)==1 )
			vdc[0]=g_auth_item.code_dc[0];
		else if( strlen(g_auth_item.code_dc)<4 )
			return 0;
		else
			get_zd_data( g_auth_item.code_dc,vdc );
vtcp_log("CC-----dc[%s]cur[%c]",vdc,curval[0]);
		if( (curval[0]=='c' && vdc[0]!='2') ||
			(curval[0]=='q' && vdc[0]!='1') ) return 0;
vtcp_log("CC-----dc[%s]",vdc);

		pub_base_strpack(g_auth_item.code_ct);
		if( strlen(g_auth_item.code_ct)==1 )
			vdc[0]=g_auth_item.code_ct[0];
		else if( strlen(g_auth_item.code_ct)<4 )
			return 0;
		else
		{
			get_zd_data( g_auth_item.code_ct,vct );
			vct[0]=vct[0]+g_auth_item.ind[3]-'0';
		}
		if( vct[0]!='1' ) vct[0]='2';
vtcp_log("CC-----ctcur[%s]ct[%s]",curval,vct);
		if( (curval[1]=='x' && vct[0]!='1') ||
			(curval[1]=='z' && vct[0]!='2') ||
			(curval[1]=='t' && vct[0]!='3') ||
			(curval[1]=='d' && vct[0]!='4') ||
			(curval[1]=='Z' && vct[0]!='4' && vct[0]!='3' && vct[0]!='2' ) 
			) return 0;
vtcp_log("CC-----ct[%s]",vct);

		pub_base_strpack(g_auth_item.code_amt);
		if( strlen(g_auth_item.code_amt)<4 )
			return 0;
		else
			get_zd_double( g_auth_item.code_amt,&vamt );

vtcp_log("CC-----amt[%.2lf]",vamt);
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&vmdmacrel,"ac_no='%s'",vacno);
		if( ret ) goto ErrExit;
		if( vacseqn==0 && vmdmacrel.ac_seqn!=9999 ) vacseqn=vmdmacrel.ac_seqn; 
vtcp_log("cc---------------[%d]",vacseqn );
vtcp_log("CCCC ac_id=%ld and ac_seqn=%d and dc='%c' and ct='%c'",
			vmdmacrel.ac_id,vacseqn,vdc[0],vct[0] );

		ret=Auth_ac_rec_Sel( g_pub_tx.reply,&vauthacrec,
			"ac_id=%ld and ac_seqn=%d and dc='%c' and ct='%c'",
			vmdmacrel.ac_id,vacseqn,vdc[0],vct[0] );
		if( ret==100 ) vauthacrec.amt=0.00;
		else if( ret ) goto ErrExit;

vtcp_log("CC-----tt[%.2lf]---",vauthacrec.amt);
		*ttldb=vauthacrec.amt+vamt;
vtcp_log("CC-----ttdb[%.2lf]---",*ttldb);
		
		return 0;
	ErrExit:
		return 1;
	}
	/**函数fh()的解释程序：用于检查是否要复核 parm：1仅对复核制网点有效,其他全有效**/
	int	get_fh_val( char * parm )
	{
		struct com_branch_c jg;
		int ret=0;
		int rlt=0;

		if( parm[0]=='1' )
		{
			ret=Com_branch_Sel( g_pub_tx.reply,&jg,"br_no='%s'",g_pub_tx.tx_br_no);
			MYRETERR
			if( jg.ind[0]=='1' ) rlt=1;
		}
		else
		{
			rlt=1;
		}
vtcp_log("-------fh----run fh()-[%s]==>[%d]",parm,rlt );
			return rlt;
		ErrExit:
			return -1;
	}
	/**函数fh()的解释程序：用于检查是否要复核 parm：1仅对复核制网点有效,其他全有效**/
	int get_sq_val( char * parm )
	{
		int rlt=0;
		
		if( strlen(parm)<sizeof(g_lvl)-1 )
			strcpy( g_lvl,parm );
		else
			strcpy( g_lvl,"1234000" );
		rlt=1;
			return rlt;
		ErrExit:
			return -1;
	}

/**用授权码完成交易**/
int pub_sq_done_code()
{
	struct auth_code_c vauth_code;
	char auth[5];
	char aupass[7];
	char authno[10];
	int ret=0;

	get_zd_data( DC_AUTH,auth );
	get_zd_data( DC_AUTH_PSW,aupass );

	vtcp_log("[%s][%d]author[%s]",__FILE__,__LINE__,auth);
	sprintf( authno,"%c%s",auth[3],aupass );
	ret=pub_sub_chk_far( atol(authno) );
	MYRETERR

	return 0;
ErrExit:
	return 1;
}
	
	int pub_sub_chk_far( long authno )
	{
		struct auth_inf_c auth_inf_c;
		struct auth_chk_def_c auth_chk_def_c;
		struct com_tel_c com_tel_c;
		int ret=0;
		char tmpstr[101];
		double tmpdbl;
		int errflag_ac=0;
		int errflag_amt=0;
		char vvct[11];

		vtcp_log("[%s][%d]authno [%d] ",__FILE__,__LINE__,authno);

		ret=Auth_inf_Dec_Upd( g_pub_tx.reply,"auth_no=%d",authno );
		if( ret ) goto ErrExit;
		ret=Auth_inf_Fet_Upd( &auth_inf_c,g_pub_tx.reply );
		if( ret==100 )
		{
			TRACE
			strcpy( g_pub_tx.reply,"C126");
			goto ErrExit;
		}
		else if( ret ) goto ErrExit;
	
		/**状态**/
		if( auth_inf_c.sts[0]=='1' )
		{
			sprintf(acErrMsg,"授权码已经使用了" );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S067");
			goto ErrExit;
		}
		else if( auth_inf_c.sts[0]=='2' )
		{
			sprintf(acErrMsg,"授权码已经废弃了" );
			WRITEMSG
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
		if( strlen(auth_inf_c.tx_code) && strcmp(g_pub_tx.tx_code,auth_inf_c.tx_code) )
		{
			vtcp_log("txcode[%s] inf_tx_code[%s]",g_pub_tx.tx_code,auth_inf_c.tx_code);
			strcpy( g_pub_tx.reply,"S066" );
			goto ErrExit;
		}
		/**被授权柜员**/
		pub_base_strpack(auth_inf_c.tel);
		if( strlen(auth_inf_c.tel) && strcmp(auth_inf_c.tel,g_pub_tx.tel))
		{
			TRACE
			strcpy( g_pub_tx.reply,"S071");
			goto ErrExit;
		}

		TRACE
		/**授权人（授权号所有人）检查**/
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

		if( strlen(g_lvl)==0 || strstr(g_lvl,com_tel_c.lvl) )
			;
		else
		{
			vtcp_log("S063 [%s][%s]",g_lvl,com_tel_c.lvl );
			strcpy( g_pub_tx.reply,"S063");
			goto ErrExit;
		}

				get_zd_data(g_auth_item.code_ct,vvct);

				get_zd_data(g_auth_item.code_ac,tmpstr);
				pub_base_strpack( auth_inf_c.ac_no );
				if(strlen(auth_inf_c.ac_no) && strcmp(auth_inf_c.ac_no,tmpstr))
				{
					vtcp_log("[%s][%s]",auth_inf_c.ac_no,tmpstr );
					strcpy( g_pub_tx.reply,"S064" );
					goto ErrExit;
				}
				get_zd_data(g_auth_item.code_amt,tmpstr);
				if( pub_base_CompDblVal(atof(tmpstr),auth_inf_c.amt)>0 )
				{
					strcpy( g_pub_tx.reply,"S065" );
					goto ErrExit;
				}

			auth_inf_c.sts[0]='1';
			auth_inf_c.use_date=g_pub_tx.tx_date;
			auth_inf_c.use_trace_no=g_pub_tx.trace_no;
			strcpy( auth_inf_c.use_tel,g_pub_tx.tel );
			ret=Auth_inf_Upd_Upd(auth_inf_c,g_pub_tx.reply);
			if( ret ) goto ErrExit;
			Auth_inf_Clo_Upd( );

			ret=sub_com_tel_amt_auth_chk(auth_inf_c.auth_tel,atof(tmpstr),vvct);
			if( ret ) 
			{
				strcpy( g_pub_tx.reply,"S078" );
				goto ErrExit;
			}

			vtcp_log("授权号检查成功[%d]",authno);
			
		strcpy( g_pub_tx.auth,auth_inf_c.auth_tel );

		strcpy( g_auth_code.auth,g_pub_tx.auth );
		g_auth_code.sts[0]='9';

		set_zd_data( DC_AUTH,g_pub_tx.auth );
		return 0;
	ErrExit:
		return 1;
	}
/**用授权人完成交易**/
int pub_sq_done_tel()
{
	struct auth_inf_c auth_inf_c;
	struct auth_chk_def_c auth_chk_def_c;
	struct com_tel_c com_tel_c,com_tel_1;
	char auth[5];
	char aupass[7];
	char sqh[21];
	int ret=0;
	char tmpstr[101];
	double amt=0;
	char ct[2];
	char cOritel[5];
	get_zd_data( DC_AUTH,auth );
	get_zd_data("1241",cOritel); /**修改92为1241**/
	get_zd_data( DC_AUTH_PSW,aupass );
vtcp_log("auth[%s] aupass[%s]",auth,aupass);
  
	ret=Com_tel_Sel( g_pub_tx.reply,&com_tel_c,"tel='%s'",auth );
	if( ret ) 
	{
		strcpy( g_pub_tx.reply,"S076");
		goto ErrExit;
	}
/* updated by duhuanping 2010/11/09 start */
	/*ret=Chk_tel_br( com_tel_c );
	if( ret ) 
	{
		vtcp_log("TEL_STS[%s]",com_tel_c.csts);
		strcpy( g_pub_tx.reply,"S062");
		goto ErrExit;
	}*/
/* updated by duhuanping 2010/11/09 end */
	if( com_tel_c.csts[0]!='0' && com_tel_c.csts[0]!='1' 
		&& com_tel_c.csts[0]!='2' )
	{
		vtcp_log("TEL_STS[%s]",com_tel_c.csts);
		strcpy( g_pub_tx.reply,"S062");
		goto ErrExit;
	}

	if( strcmp(aupass,"------") )
	{
		pub_base_EnDes(g_pub_tx.tx_date,"",aupass);

		vtcp_log("auth[%s] cOritel[%s] aupass[%s]",auth,cOritel,aupass);
		ret=Com_tel_Sel(g_pub_tx.reply,&com_tel_1,"tel='%s'",cOritel);
		if( ret ) 
		{
			strcpy( g_pub_tx.reply,"S076");
			goto ErrExit;
		}		
		if( strcmp(aupass,com_tel_1.pwd) )
		{
			strcpy( g_pub_tx.reply,"S075");
			goto ErrExit;
		}
		vtcp_log("auth[%s] aupass[%s] lvl[%s]in[%s]",
			auth,aupass,g_lvl,com_tel_c.lvl);
	} 
	if( strlen(g_lvl)==0 || strstr(g_lvl,com_tel_c.lvl) )
		;
	else
	{
		vtcp_log("S063 [%s][%s]",g_lvl,com_tel_c.lvl );
		strcpy( g_pub_tx.reply,"Z10K");
		goto ErrExit;
	}
		pub_base_strpack(g_auth_item.code_amt);
		if( !strlen(g_auth_item.code_amt) )
			amt=0.00;
		else
		{
			get_zd_data(g_auth_item.code_amt,tmpstr);
			amt=atof(tmpstr);
		}

		pub_base_strpack(g_auth_item.code_ct);
		ct[1]='\0';
		if( strlen(g_auth_item.code_ct)==4 )
		{
			get_zd_data(g_auth_item.code_ct,tmpstr);
			ct[0]=tmpstr[0];
		}
		else if( strlen(g_auth_item.code_ct) )
		{
			ct[0]=g_auth_item.code_ct[0];
		}
		else
		{
			ct[0]='0';
		}
		ct[0]=ct[0]+g_auth_item.ind[3]-'0';  /**ind用于调整各个画面现转的不同
											 基准为1现2转3同4大*/

	ret=sub_com_tel_amt_auth_chk( auth,amt,ct );
	if( ret ) 
	{
		strcpy( g_pub_tx.reply,"S078" );
		goto ErrExit;
	}

	strcpy( g_auth_code.auth,auth );

	vtcp_log("本地授权成功[%s%s]",auth,aupass);
	return 0;
ErrExit:
TRACE
	return -1;
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
int Chk_tel_br( com_tel_c )
 struct com_tel_c com_tel_c;
{
	struct com_branch_c com_jg;
	int ret=0;

	if( !strcmp(com_tel_c.br_no,g_pub_tx.tx_br_no) )
		return 0;
	
	ret=Com_branch_Sel(g_pub_tx.reply,&com_jg,"br_no='%s'",g_pub_tx.tx_br_no );
	if( ret ) return 1;

	if( !strcmp(com_tel_c.br_no,com_jg.up_br_no) )
		return 0;
		
	if( !strcmp(com_tel_c.br_no,"61000") )
		return 0;

	if( !strcmp(com_tel_c.br_no,"61099") )
		return 0;
	
	return 1;
}
