/*************************************************
* 文 件 名:  sp8305.c
* 功能描述： 凭证汇总清单
*
* 作    者:  Jarod
* 完成日期： 2007-05-27
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "com_tel_c.h"
#include "note_mst_hst_c.h"

long ys_cnt=0;			/* 昨日库存 */
	long in_cnt=0;			/* 今日领入 */
	long out_cnt=0;			/* 今日交出 */
	long use_cnt=0;			/* 今日使用 */
	long buy_cnt=0;			/* 客户购买 */
	long dsy_cnt=0;			/* 今日作废 */
	long cls_cnt=0;			/* 今日销号 */
	long lose_cnt=0;		/* 今日挂失 */
	long unlose_cnt=0;		/* 今日解挂 */
	long dd_cnt=0;			/* 今日销号 */
	long now_cnt=0;			/* 今日结存 */
	long tmp_cnt=0;

	long ys_cnt_tol=0;		/* 昨日库存总计 */
	long in_cnt_tol=0;		/* 今日领入总计 */
	long out_cnt_tol=0;		/* 今日交出总计 */
	long use_cnt_tol=0;		/* 今日使用总计 */
	long buy_cnt_tol=0;		/* 客户购买总计 */
	long dsy_cnt_tol=0;		/* 今日作废总计 */
	long cls_cnt_tol=0;		/* 今日销号总计 */
	long lose_cnt_tol=0;	/* 今日挂失总计 */
	long unlose_cnt_tol=0;	/* 今日解挂总计 */
	long dd_cnt_tol=0;		/* 今日销号总计 */	
	long now_cnt_tol=0;		/* 今日结存总计 */
	
	struct note_parm_c s_note_parm;
	struct note_mst_c s_note_mst;
	struct com_branch_c s_com_branch;
	struct com_tel_c s_com_tel;
	struct com_sys_parm_c sCom_sys_parm;
	
	int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_tdtdb(int bli,int bll,char str[100]);

int rpt130()
{
	int ttlnum=0;
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];
	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( vpfm,0,sizeof(vpfm) );
	memset(filename,0x00,sizeof(filename));
	memset( &s_note_parm , 0x00 , sizeof(struct note_parm_c));
	memset( &s_note_mst , 0x00 , sizeof(struct note_mst_c));
  memset(&s_com_branch,0x00,sizeof(struct com_branch_c));	
  memset(&s_com_tel,0x00,sizeof(struct com_tel_c));	
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
	
	strcpy( filename , "RPT130");
	strcpy(vpfm, "RPT130");
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
	ret=Com_branch_Dec_Sel(g_pub_tx.reply,"br_no != '10000' and br_type!='0' order by br_no");
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
	ERR_DEAL
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
	
	while(1)
	{ /*begin 1*/
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
		ret=Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
		if(ret==100)
		{
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
			if( ttlnum )
			{
				/* 打页尾 */
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;
	vtcp_log("[%s][%d] wanglei ",__FILE__,__LINE__);
		}
		else ERR_DEAL
			
		/* 机构变换时 */
		if( strcmp(  s_com_branch.br_no, old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
	vtcp_log("打印页尾2:[%s]--[%d]",__FILE__,__LINE__);
				/* 打印页尾 */
				ret =print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_com_branch.br_no);
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_com_branch.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp ,s_com_branch.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		
		ret=Com_tel_Dec_Sel(g_pub_tx.reply,"br_no='%s' and tel not like '%%99%%' order by tel",s_com_branch.br_no);
		ERR_DEAL
		while(1)
		{/*begin 2*/
			memset(&s_com_tel,0x00,sizeof(struct com_tel_c));
			ret=Com_tel_Fet_Sel(&s_com_tel,g_pub_tx.reply);
			if(ret==100){
				break;	
			}
			else ERR_DEAL
			
			ret=Note_parm_Dec_Sel(g_pub_tx.reply,"dc_ind='Y' and ctl_no_ind='Y' order by note_type");
			ERR_DEAL
			while(1)
			{/*while 3*/
				memset(&s_note_parm,0x00,sizeof(struct note_parm_c));
				ret=Note_parm_Fet_Sel(&s_note_parm,g_pub_tx.reply);
				if(ret==100)break;
				else ERR_DEAL
					
				/*初始化变量*/	
				ys_cnt=0;			/* 昨日库存 */
	      in_cnt=0;			/* 今日领入 */
	      out_cnt=0;			/* 今日交出 */
	      use_cnt=0;			/* 今日使用 */
	      buy_cnt=0;			/* 客户购买 */
	      dsy_cnt=0;			/* 今日作废 */
	      cls_cnt=0;			/* 今日销号 */
	      lose_cnt=0;		/* 今日挂失 */
	      unlose_cnt=0;		/* 今日解挂 */
	      dd_cnt=0;			/* 今日销号 */
	      now_cnt=0;			/* 今日结存 */
	      tmp_cnt=0;
        
	      memset(wherelist,0x00,sizeof(wherelist));
	      sprintf(wherelist," br_no='%s' and tel='%s' and note_type='%s' ",s_com_branch.br_no,s_com_tel.tel,s_note_parm.note_type);
	           		/* 统计今日结存 */
				ret = sql_sum_long( "note_mst" , "cnt" , &tmp_cnt ," %s and sts in('0','1') ",wherelist);
				ERR_DEAL
     		now_cnt = tmp_cnt;
     		tmp_cnt=0;
 				
 				/* 今日领入 */
 				ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt," tx_date=%ld and %s and sts_ind in('1','3','5','9','A')",g_pub_tx.tx_date,wherelist );
			ERR_DEAL			
				in_cnt=tmp_cnt;  
				tmp_cnt=0;
				
						/* 今日交出 */	
				ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt,	"tx_date=%ld and %s and sts_ind in('2','4','7','8')" ,g_pub_tx.tx_date,wherelist );
				ERR_DEAL		
				out_cnt=tmp_cnt; 
				tmp_cnt=0; 	
				
						/* 今日使用 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt," tx_date=%ld and %s and sts_ind ='D'" ,g_pub_tx.tx_date,wherelist );
		ERR_DEAL		
		use_cnt=tmp_cnt;	
				tmp_cnt=0;
				
						/* 客户购买 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt ," tx_date=%ld and %s and sts_ind ='6'" ,g_pub_tx.tx_date,wherelist );
		ERR_DEAL		
			buy_cnt=tmp_cnt;	
			tmp_cnt=0;
			
					/* 今日作废 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt ," tx_date=%ld and %s and sts_ind ='E'" ,g_pub_tx.tx_date,wherelist );
		ERR_DEAL		
				dsy_cnt=tmp_cnt;
				tmp_cnt=0;
				
				
						/* 今日销号 */
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt ," tx_date=%ld and %s and sts_ind ='H'" ,g_pub_tx.tx_date,wherelist );
		ERR_DEAL		
				cls_cnt=tmp_cnt;
				tmp_cnt=0;
				
				
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
				
			}/*while 3*/
			Note_parm_Clo_Sel();
		}	/*end 2*/
		Com_tel_Clo_Sel();
	}/*end 1*/
	Com_branch_Clo_Sel();
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成凭证汇总清单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成凭证汇总清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tdtdb);
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

/* 赋值函数 */
int get_ratio_tdtdb( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char fmt[10]; 
	
	switch( bli ) 
	{
		case 'A': /* 交易日期 */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm," 1=1");			
			if(ret)
			{
				sprintf(acErrMsg,"取系统日期出错!![%d]",ret);			
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d年%2d月%2d日" , sCom_sys_parm.sys_date/10000,sCom_sys_parm.sys_date%10000/100 , sCom_sys_parm.sys_date%100 );			
			sprintf(fmt,"%%-%ds",bll);
 			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* 机构名称 */
			pub_base_strpack(s_com_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,s_com_branch.br_name, s_com_branch.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 凭证名称 */ 
			pub_base_strpack(s_note_parm.name ); 
			sprintf( t_str , "%s" ,s_note_parm.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'F': /*领入  */ 
			sprintf( t_str , "%ld" ,in_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'G': /* 缴出 */ 
			sprintf( t_str , "%ld" ,out_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'H': /* 使用 */
			sprintf( t_str , "%ld" ,use_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break; 
		case 'I': /* 出售 */
			sprintf( t_str , "%ld" ,buy_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break; 
		case 'J': /* 作废 */
			sprintf( t_str , "%ld" ,dsy_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break; 
		case 'K': /* 销号 */
			sprintf( t_str , "%ld" ,cls_cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'L': /* 结存 */
			sprintf( t_str , "%ld" ,now_cnt ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break; 
		case 'M': /*柜员号  */
			sprintf( t_str , "%s" ,s_com_tel.tel); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break; 
		case 'N': /* 姓名 */  
			sprintf( t_str , "%s" ,s_com_tel.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
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
