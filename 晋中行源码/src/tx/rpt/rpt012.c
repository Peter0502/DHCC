/*************************************************************
* 文 件 名: rpt012.c
* 功能描述：凭证使用流水清单
*
* 作    者: lance
* 完成日期: 2003年07月13日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>   
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "note_parm_c.h"

struct note_mst_hst_c	s_note_mst_hst;
struct com_sys_parm_c	s_com_sys_parm;
struct note_parm_c	s_note_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_pzsyls(int bli,int bll,char str[100]);

rpt012()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_note_mst_hst , 0x00 , sizeof(struct note_mst_hst_c));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_note_parm , 0x00 , sizeof(struct note_parm_c));
			
	strcpy( old_br_no , "*****" );
		
   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	/*	
	strcpy( filename , "BRpzsyls" );	
	strcpy(vpfm, "pzsyls" );
	*/	
	strcpy( filename , "RPT012");
	strcpy(vpfm, "RPT012");
	/* 条件: 交易日期为上一交易日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	sprintf( tmpstr," tx_date=%ld and " , s_com_sys_parm.lst_date );
	strcat( wherelist,tmpstr );

	/**组成查询**/	
	strcat(wherelist," 1=1 ORDER BY br_no,note_type,beg_note_no,end_note_no,tel");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Note_mst_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Note_mst_hst_Fet_Sel( &s_note_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;
		}else if( ret ){
			sprintf(acErrMsg,"读取 流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack(s_note_mst_hst.br_no );
		if( strcmp( s_note_mst_hst.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_note_mst_hst.br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_note_mst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_note_mst_hst.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 打印信息 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Note_mst_hst_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成凭证使用交易清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成凭证使用流水清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_pzsyls);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表首 */
int print_head( )
{
	char vrec[3];
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表体 */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_pzsyls( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_note_mst_hst.tx_date/10000 , s_note_mst_hst.tx_date%10000/100 , s_note_mst_hst.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_note_mst_hst.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_note_mst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_note_mst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 凭证类型 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_note_mst_hst.note_type); 
			break; 
		case 'F': /* 凭证起始号码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.beg_note_no);
			break;
		case 'G': /* 凭证终止号码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.end_note_no);
			break;
		case 'H': /* 数量 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_note_mst_hst.note_cnt);
			break;
		case 'I': /* 交易机构 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.br_no);
			break;
		case 'J': /* 交易柜员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.tel);
			break;
		case 'K': /* 对方机构 */			
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_note_mst_hst.tw_br_no ); 
			break;
		case 'L': /* 对方柜员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_note_mst_hst.tw_tel);
			break;
		case 'M': /* 交易类型 */
			pub_base_dic_show_str(tmp_inf,"note_act",s_note_mst_hst.sts_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'Q': /* 交易时间 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_note_mst_hst.tx_time);
			break;
		case 'R': /* 凭证名称 */
			ret = Note_parm_Sel(g_pub_tx.reply, &s_note_parm, "note_type='%s'",
		s_note_mst_hst.note_type);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询凭证参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, s_note_parm.name);
			break;		

		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
