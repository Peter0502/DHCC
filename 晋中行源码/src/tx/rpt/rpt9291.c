/*************************************************************
* 文 件 名: rpt929.c
* 功能描述：打印长治市商业银行总行存款余额表  (月  报)
*
* 作    者: Jarod
* 完成日期: 2006年11月08日
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
#include "mo_merge_ac_c.h"
#include "dd_mst_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"

struct dd_mst_c gDd_mst;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long g_count=0;
char old_ac_no[25];
static double sum=0.0;

int get_ratio_tdtdb(int bli,int bll,char str[200]);

rpt929()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];

	memset(&sum,0x00,sizeof(double));	
	memset( &gDd_mst,0x00,sizeof(struct dd_mst_c));	
	memset( wherelist,0,sizeof(wherelist) );
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
	
	strcpy( filename , "RPT929");
	strcpy(vpfm, "RPT929");
	
	/**组成查询**/
	memset(wherelist,'\0',sizeof(wherelist));
	strcat(wherelist," bal != 0.0 and  ac_type<>'5'  order by opn_br_no,prdt_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret= Dd_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Dd_mst_Fet_Sel( &gDd_mst, g_pub_tx.reply );
		memset(old_ac_no,'\0',sizeof(old_ac_no));		
		sum+=gDd_mst.bal;
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
	vtcp_log("打印页尾1:[%s]--[%d]",__FILE__,__LINE__);
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读活期存款主文件出错啦!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack( gDd_mst.opn_br_no );
		if( strcmp( gDd_mst.opn_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , gDd_mst.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( gDd_mst.opn_br_no, filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , gDd_mst.opn_br_no, filename );
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
			ret=print_ZZ(); /*打印每页的小计*/
			ERR_DEAL
			memset(&sum,0x00,sizeof(double));	
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
		g_count++;
	}
	Dd_mst_Clo_Sel();

	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成新旧帐号对照表成功!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
printf("\n共处理[%d]条记录.\n",g_count);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成新旧帐号对照表失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tdtdb);
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

int print_ZZ()
{
	char vrec[3];
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
	ERR_DEAL
OkExit:
	return 0;
ErrExit:
	return 1;
}

/* 赋值函数 */
int get_ratio_tdtdb( bli,bll,str )
int bli,bll;
char str[200];
{
	char fmt[10]; 
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct mo_merge_ac_c sMo_merge_ac;
	int ret1=0,ret2=0;
	struct mdm_ac_rel_c sMdm_ac_rel;
	char str_mon[30];

    memset(str_mon,'\0',sizeof(str_mon));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sMo_merge_ac,0x00,sizeof(struct mo_merge_ac_c));	
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
			if(ret)
			{
				sprintf(acErrMsg,"查询公共系统参数出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d年%2d月%2d日" , sCom_sys_parm.sys_date/10000 , sCom_sys_parm.sys_date%10000/100 , sCom_sys_parm.sys_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",gDd_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name ,gDd_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'M': /*新帐号*/
			ret1=Mo_merge_ac_Sel(g_pub_tx.reply,&sMo_merge_ac,"ac_id=%d",gDd_mst.ac_id);
			if(ret1 == 100)
			{
				ret2=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d",gDd_mst.ac_id);
				if(ret2)
				{
					sprintf(acErrMsg,"查询介质与帐户关系表出错!![%d]",ret2);
					WRITEMSG
					goto ErrExit;
				}
				sprintf( t_str , "%s" , sMdm_ac_rel.ac_no); 
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt, t_str);
				break;
			}
			else if(ret1)
			{
				sprintf(acErrMsg,"查询新旧帐号对照表出错!!!~~~[%d]",ret1);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(old_ac_no,sMo_merge_ac.old_ac_no);
			sprintf( t_str , "%s" , sMo_merge_ac.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'I': /*旧帐号*/
			sprintf( t_str , "%s" ,old_ac_no);
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'N': /*余额*/
			sprintf(t_str,"%.2lf",gDd_mst.bal);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		case 'L':  /*户      名 */
			pub_base_strpack(gDd_mst.name); 
			sprintf( t_str , "%s" , gDd_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'O': /*每页小计*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
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
