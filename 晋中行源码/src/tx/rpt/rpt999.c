/*************************************************************
* 文 件 名: rpt999.c
* 功能描述：日终传票统计数据
*
* 作    者: lance
* 完成日期: 2003年07月16日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#define ERR_XYM { strcpy(acErrMsg,"SQL ERROR!");strcpy(g_pub_tx.reply,"0230");WRITEMSG goto ErrExit; }
#include "public.h"
#include "rpt_note_ctl_c.h"
#include "rpt_note_inf_c.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_c.h"
#include "trace_log_c.h"

static char sqlcmd[500];
static char kzbz[50];
int flag=0;
long	num=0;

 
static	long date_1=0;
static	long date_3=0;
static	struct	rpt_note_ctl_c	s_rpt_note_ctl;
static	struct	rpt_note_inf_c	RPT_NOTE;    
static	struct	com_sys_parm_c	s_com_sys_parm;    
static	struct	dc_log_c		dc_log;
static	struct	dc_log_c		lst_dc_log;
static	struct	trace_log_c		trace_log;
static  long v_no=0;         	 
static  long v_go=0;         	 

rpt999()
{
	int    ret=0;
 
	char ln_ac_no[20],dd_ac_no[20],parm_code[6];  
	char tmp_str[100], where_list[200], group_list[100] ;
	long day_tmp,be_date;
    
	memset(&s_rpt_note_ctl, 0x00, sizeof(struct rpt_note_ctl_c));
	memset(&RPT_NOTE, 		0x00, sizeof(struct rpt_note_inf_c)); 
	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
	memset(&dc_log,			0x00, sizeof(struct dc_log_c));
	memset(&lst_dc_log,		0x00, sizeof(struct dc_log_c));
         
	ret=sql_begin(); /*打开事务*/
	if( ret != 0 )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/* 初始化全局变量 */
	pub_base_sysinit();
	/* 清理信息表中数据 */
	/* 计算反盘前日期 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"sys_date=%ld",
						   g_pub_tx.tx_date);
	if( ret )	ERR_DEAL
	date_1 = s_com_sys_parm.lst_date;
	/* 计算三天前日期 */
	ret = pub_base_deadlineD( date_1 , 3 , &date_3 );
	if( ret )	ERR_DEAL
	/* 删除三天前的数据 */
	ret = RPT_NOTE_del_3days( date_3 );

	if( ret )	ERR_DEAL
	/* 删除当天前的数据 */    
	ret = RPT_NOTE_del_1days( date_1 );
	if( ret )	ERR_DEAL

	/* 读条件定义表 */    
	ret = Rpt_note_ctl_Dec_Sel(g_pub_tx.reply,"1=1 order by no");
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	num=1;    
	while ( 1 )
	{
		ret = Rpt_note_ctl_Fet_Sel( &s_rpt_note_ctl , g_pub_tx.reply) ;
		if( ret==100 ) break;
		else if( ret )
		{
			sprintf(acErrMsg,"定义游标错[%d]",ret);
			strcpy (g_pub_tx.reply, "0230");
			ERR_DEAL
		}
	
		ldchar(s_rpt_note_ctl.wherelist,strlen(s_rpt_note_ctl.wherelist),
			s_rpt_note_ctl.wherelist);

printf( "  [%s][%s]\n",s_rpt_note_ctl.txname,s_rpt_note_ctl.wherelist );

		sprintf(acErrMsg,"[%s][%s]",s_rpt_note_ctl.txname,s_rpt_note_ctl.wherelist);
		WRITEMSG
		if ( RPT_NOTE_init_proc() ) ERR_DEAL				/*预处理*/

		if( s_rpt_note_ctl.flag[0]=='1' )
			flag=1;
		else
			flag=0;

printf("kzbz[%s]",kzbz);	
		v_no=0;
		if ( sqlcommd_proc() ) ERR_DEAL					/*进行检索*/
	}

	Rpt_note_ctl_Clo_Sel();	
	/* 记录分组项目 */
	
	/* 根据数据来源表不同,统计数据 */
	/* 统计dc_log_bk */
	/* 统计dc_log_bk */


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终传票统计数据处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终传票统计数据处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
RPT_NOTE_init_proc()										/*初始化*/
{
	ldchar(s_rpt_note_ctl.txname,strlen(s_rpt_note_ctl.txname),s_rpt_note_ctl.txname);
	ldchar(s_rpt_note_ctl.wherelist,strlen(s_rpt_note_ctl.wherelist),s_rpt_note_ctl.wherelist);
	ldchar(s_rpt_note_ctl.grouplist,strlen(s_rpt_note_ctl.grouplist),s_rpt_note_ctl.grouplist);
	ldchar(s_rpt_note_ctl.summary,strlen(s_rpt_note_ctl.summary),s_rpt_note_ctl.summary);

	memset(kzbz,'0',sizeof(kzbz)-1);
	kzbz[sizeof(kzbz)-1]=NULL;

	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"tx_date") )		kzbz[ 1]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"trace_no") )	kzbz[ 2]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"trace_cnt") )	kzbz[ 3]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"entry_code") )	kzbz[ 4]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"dc_seqn") )		kzbz[ 5]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"opn_br_no") )	kzbz[ 6]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"tx_br_no") )	kzbz[ 7]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"tx_open_br_no") )	kzbz[ 8]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"tx_tx_br_no") )	kzbz[ 9]='1';
	/**
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"cur_no") )		kzbz[10]='1';
	**/
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"acc_hrt") )		kzbz[11]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"dc_ind") )		kzbz[12]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"ct_ind") )		kzbz[13]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"note_type") )	kzbz[14]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"note_no") )		kzbz[15]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"amt") )			kzbz[16]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"tel") )			kzbz[17]='1';
	if ( NULL!=strstr(s_rpt_note_ctl.grouplist,"sts") )			kzbz[18]='1';

	kzbz[10]='1';	/*币种*/

	return 0;
}

sqlcommd_proc()										/*根据SQL检索dc_log_bk*/
{
	memset(&lst_dc_log,0x00,sizeof(struct dc_log_c));
	int		ret;

	if ( RPT_NOTE_makesql() ) ERR_DEAL						/*拼写SQL语句*/

	sprintf(acErrMsg,"[%s][%d][%s]\n",__FILE__,__LINE__,sqlcmd);
	WRITEMSG
	/** 如果需要在出翻盘后的数据flag=1 **/
	if ( flag == 1 )
		ret =  Dc_log_Dec_Sel(g_pub_tx.reply,"%s",sqlcmd);
	else
		ret =  Dc_log_bk_Dec_Sel(g_pub_tx.reply,"%s",sqlcmd);

	if ( ret )  ERR_XYM

	while( 1 )
	{
		if ( flag == 1 )
			ret = Dc_log_Fet_Sel( &dc_log,g_pub_tx.reply);
		else
			ret = Dc_log_bk_Fet_Sel( &dc_log,g_pub_tx.reply);
		if ( ret == 100 ) 
		{
			if ( RPT_NOTE_Insert() ) ERR_DEAL
			break;
		}
		else if ( ret )  ERR_XYM

		if ( RPT_NOTE_IsIns() )								/*是否结束分组*/
		{
			if ( RPT_NOTE_Insert() ) ERR_DEAL				/*插入RPT_NOTE*/
		}

		if ( s_rpt_note_ctl.txbalmd==1 ) dc_log.amt+=lst_dc_log.amt;/*累计txbal*/
		dc_log.dc_seqn=lst_dc_log.dc_seqn++;					/*用ent_seq累计记录数*/
		memcpy(&lst_dc_log,&dc_log,sizeof(struct dc_log_c));
	}

	if ( flag == 1 )
		Dc_log_Clo_Sel();
	else
		Dc_log_bk_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}

RPT_NOTE_makesql()
{
	char filter[250];

	memset(sqlcmd,0x00,sizeof(sqlcmd));
	memset(filter,0x00,sizeof(filter));

	switch( atol(s_rpt_note_ctl.wherelist) )
	{
		case -1 :						/*在此处可以添加特定的SQL语句*/
			break;
		default :
			if ( strlen(s_rpt_note_ctl.wherelist) ) 
			{
				strcat(filter,s_rpt_note_ctl.wherelist);
				strcat(filter," AND ");
			}
			strcat(filter,"sts<>'1'");
			strcat(sqlcmd,filter);
			break;
	}
	if ( strlen(s_rpt_note_ctl.chk_code))
	{
		memset(filter,0x00,sizeof(filter));
		if ( flag == 1 )
		{
			sprintf(filter," AND trace_no in (select distinct \
				trace_no from trace_log where tx_code %s )",s_rpt_note_ctl.chk_code);
		}
		else
		{
			sprintf(filter," AND trace_no in (select distinct \
				trace_no from trace_log_bk where tx_code %s )",s_rpt_note_ctl.chk_code);
		}
		strcat(sqlcmd,filter);
	
	}

	strcat( sqlcmd," ORDER BY ");

	if ( strlen(s_rpt_note_ctl.grouplist) )
	{
		strcat(sqlcmd,s_rpt_note_ctl.grouplist);
		strcat(sqlcmd,",cur_no");
	}
	else
	{
		strcat(sqlcmd,"cur_no");
	}

	vtcp_log(sqlcmd);

	return 0;
ErrExit:
	return 1;
}

RPT_NOTE_IsIns()
{
	int i;

	if ( s_rpt_note_ctl.txbalmd==0 ) 
		return 1;

	for ( i=1 ; i<strlen(kzbz) ; i++ )
	{
		if ( kzbz[i]!='1' ) continue;

		if ( i== 1 && dc_log.tx_date!=lst_dc_log.tx_date) return 1;
		if ( i== 2 && dc_log.trace_no!=lst_dc_log.trace_no ) return 1;
		if ( i== 3 && dc_log.trace_cnt!=lst_dc_log.trace_cnt ) return 1;
		if ( i== 4 && RPT_NOTE_strcmp(dc_log.entry_code,lst_dc_log.entry_code) ) return 1;
/**		if ( i== 5 && dc_log.dc_seqn!=lst_dc_log.dc_seqn ) return 1; ***/
		if ( i== 6 && RPT_NOTE_strcmp(dc_log.opn_br_no,lst_dc_log.opn_br_no) ) return 1;
		if ( i== 7 && RPT_NOTE_strcmp(dc_log.tx_br_no,lst_dc_log.tx_br_no) ) return 1;
		if ( i== 8 && RPT_NOTE_strcmp(dc_log.tx_opn_br_no,lst_dc_log.tx_opn_br_no) ) return 1;
		if ( i== 9 && RPT_NOTE_strcmp(dc_log.tx_tx_br_no,lst_dc_log.tx_tx_br_no) ) return 1;
		if ( i==10 && RPT_NOTE_strcmp(dc_log.cur_no,lst_dc_log.cur_no) ) return 1;
		if ( i==11 && RPT_NOTE_strcmp(dc_log.acc_hrt,lst_dc_log.acc_hrt) ) return 1;
		if ( i==12 && RPT_NOTE_strcmp(dc_log.dc_ind,lst_dc_log.dc_ind) ) return 1;
		if ( i==13 && RPT_NOTE_strcmp(dc_log.ct_ind,lst_dc_log.ct_ind) ) return 1;
		if ( i==14 && RPT_NOTE_strcmp(dc_log.note_type,lst_dc_log.note_type) ) return 1;
		if ( i==15 && RPT_NOTE_strcmp(dc_log.note_no,lst_dc_log.note_no) ) return 1;
		if ( i==16 && dc_log.amt!=lst_dc_log.amt ) return 1;
		if ( i==17 && RPT_NOTE_strcmp(dc_log.tel,lst_dc_log.tel) ) return 1;
		if ( i==18 && RPT_NOTE_strcmp(dc_log.sts,lst_dc_log.sts) ) return 1;

	}

	return 0;
}

RPT_NOTE_strcmp(char *str1,char *str2)
{
	ldchar(str1,strlen(str1),str1);
	ldchar(str2,strlen(str2),str2);

	return ( strcmp(str1,str2) );
}

RPT_NOTE_Insert()											/*插入日终凭证表*/
{
	int		ret=0;
	if ( pub_base_CompDblVal(lst_dc_log.amt,0.00)==0 )				/*txbal=0不写RPT_NOTE*/
	{
		return 0;
	}

	memset(&RPT_NOTE,0x00,sizeof(struct rpt_note_inf_c));

	RPT_NOTE.tx_date=lst_dc_log.tx_date;

	RPT_NOTE.trace_no=num;
	num++;

	strcpy(RPT_NOTE.opn_br_no,lst_dc_log.opn_br_no);
	strcpy(RPT_NOTE.tx_br_no,lst_dc_log.tx_br_no);
	strcpy(RPT_NOTE.tx_opn_br_no,lst_dc_log.tx_opn_br_no);
	/*****
	if( strcmp(lst_dc_log.entry_code,"L3A100")==0 )
	{
		strcpy(RPT_NOTE.tx_tx_br_no,lst_dc_log.tx_opn_br_no);	
	}else{	
	****/
		strcpy(RPT_NOTE.tx_tx_br_no,lst_dc_log.tx_tx_br_no);
	/****
	}
	****/
	
	/*** 增加按照科目号取名称 不去trace_log取帐号ac_no dong 20030824**/
	if ( s_rpt_note_ctl.flag[1]=='0' )
	{
		ret = sql_unique_string("com_item","acc_no",trace_log.ac_no,7,"acc_hrt='%s'",lst_dc_log.acc_hrt);
		if ( ret )
		{
			sprintf(acErrMsg," Select com_item error trace_no=[%d] trace_cnt=[%d] [%d] [%s]",
			lst_dc_log.trace_no,lst_dc_log.trace_cnt,ret,lst_dc_log.acc_hrt);
			ERR_DEAL	
		}
		vtcp_log(" ac_no=[%s] [%s]",trace_log.ac_no,lst_dc_log.acc_hrt);
	}
	else if ( s_rpt_note_ctl.flag[1]=='1' )
	{
		ret = Trace_log_bk_Sel( g_pub_tx.reply , &trace_log , \
			"trace_no=%d AND trace_cnt=%d", lst_dc_log.trace_no,lst_dc_log.trace_cnt);
		if ( ret )
		{
			sprintf(acErrMsg," Select Trace_log error trace_no=[%d] trace_cnt=[%d] [%d]",
			lst_dc_log.trace_no,lst_dc_log.trace_cnt,ret);
			ERR_DEAL	
		}
	}
	else if ( s_rpt_note_ctl.flag[1]=='2' ) /** 到凭证号里取帐号 **/
	{
		strcpy(trace_log.ac_no,lst_dc_log.note_no);
	}

	ret = Pub_base_Getacname(trace_log.ac_no,RPT_NOTE.name,&g_pub_tx.ac_id, \
		&g_pub_tx.cif_no,&g_pub_tx.tx_amt1,g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg," Select name error trace_no=[%d] trace_cnt=[%d] [%d]",
		lst_dc_log.trace_no,lst_dc_log.trace_cnt,ret);
	}
/***
	g_pub_tx.ac_id=lst_dc_log.acid;
	if( kzbz[9]=='1' && kzbz[7]!='1' )
	{
		BshBz=-1;
	}
	else
	{
		if ( pub_base_acid_acno(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_no1) ) 
			ERR_DEAL	*取帐号*
	}
	ldchar(g_pub_tx.acno1,strlen(g_pub_tx.acno1),g_pub_tx.acno1);
	strcpy(RPT_NOTE.acno,g_pub_tx.acno1);
**/
	strcpy(RPT_NOTE.ac_no,trace_log.ac_no);
	strcpy(RPT_NOTE.dc_ind,lst_dc_log.dc_ind);
	if ( s_rpt_note_ctl.flag[2]=='1' )
	{
		strcpy(RPT_NOTE.acc_hrt,s_rpt_note_ctl.tw_acchrt);
		ldchar(RPT_NOTE.acc_hrt,strlen(RPT_NOTE.acc_hrt),RPT_NOTE.acc_hrt);
	}
	else
	{
		strcpy(RPT_NOTE.acc_hrt,lst_dc_log.acc_hrt);
		ldchar(RPT_NOTE.acc_hrt,strlen(RPT_NOTE.acc_hrt),RPT_NOTE.acc_hrt);
	}
	strcpy(RPT_NOTE.tw_acchrt,s_rpt_note_ctl.tw_acchrt);
	RPT_NOTE.amt=lst_dc_log.amt;
	strcpy(RPT_NOTE.cur_no,lst_dc_log.cur_no);

	RPT_NOTE_getdfkmkzz(RPT_NOTE.tw_acchrt,RPT_NOTE.acc_hrt);
	RPT_NOTE_getzy(RPT_NOTE.summary);
/**
vtcp_log("ac_no=[%s]",RPT_NOTE.ac_no);
vtcp_log("dc_ind=[%s]",RPT_NOTE.dc_ind);
vtcp_log("acc_hrt=[%s]",RPT_NOTE.acc_hrt);
vtcp_log("tw_acchrt=[%s]",RPT_NOTE.tw_acchrt);
vtcp_log("amt=[%lf]",RPT_NOTE.amt);
vtcp_log("summary=[%s]",RPT_NOTE.summary);
**/
	v_no++;
	RPT_NOTE.trace_no=s_rpt_note_ctl.no*10000+v_no;
vtcp_log("trace_no=[%d]",RPT_NOTE.trace_no);
	
	/*--- xxx --- 20030920 --- 个别特别多的传票单独出一个文件 ---*/
	if( s_rpt_note_ctl.flag[3]=='1' )
		strcpy(RPT_NOTE.txcode,"T");
	/*-----------------------------------------------------------*/
	ret = Rpt_note_inf_Ins(RPT_NOTE,g_pub_tx.reply) ;
	if ( ret )
	{
		sprintf(acErrMsg,"插入日终凭证表错");
		ERR_DEAL
	}
	if ( RPT_NOTE_strcmp(dc_log.tx_tx_br_no,lst_dc_log.tx_tx_br_no)) num=1;

	memset(&lst_dc_log,0x00,sizeof(struct dc_log_c));

	return 0;
ErrExit:
	return 1;
}

RPT_NOTE_getdfkmkzz( char * tw_acchrt, char * acchrt )
{
	char tmpkm[8];

	switch( atol(tw_acchrt) )
	{
		case 0  :
				break;
		case -1 : strcpy(tw_acchrt,""); 			/*在此处添加科目的控制程序*/
				break;
		default :
				/***** fwb 20020704 *****/
				strcpy(tmpkm,acchrt); 			/*把acchrt和tw_acchrt颠倒位置*/
				strcpy(acchrt,tw_acchrt);
				strcpy(tw_acchrt,tmpkm);
				/***** end 20020704 *****/
				break;
	}

	return 0;
}

RPT_NOTE_getzy( char * summary )
{
	switch( atol(summary) )
	{
		case -1 : strcpy(summary,"");				/*在此处添加摘要的控制程序*/
				break;
		case 0:
			strcpy( summary,s_rpt_note_ctl.txname );
				break;
		default :
				ldchar(summary,strlen(summary),summary);
				if ( strlen(summary)==0 ) strcpy(summary,s_rpt_note_ctl.summary);
				break;
	}

	return 0;
}

/* 删除3天前的数据 */
int RPT_NOTE_del_3days(long date3)	
{
	char str0[100];

	sprintf(str0,"tx_date<=%d",date3);
	if ( deldb("RPT_NOTE_INF",str0) )  ERR_DEAL
	
	return 0;
ErrExit:
	return -1;
}
/* 删除满足条件的数据 */
int RPT_NOTE_del_1days(long date1)			
{
	char str0[100];

	sprintf(str0,"tx_date=%d",date1);
	if ( deldb("RPT_NOTE_INF",str0) )  ERR_DEAL
									
	return 0;
ErrExit:
	return -1;
}
