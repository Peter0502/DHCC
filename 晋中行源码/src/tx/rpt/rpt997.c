/*************************************************************
* 文 件 名: rpt997.c
* 功能描述：日终流水清单打印
*
* 作    者: 
* 完成日期: 2003年08月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "tx_def_c.h"
#include "com_sys_parm_c.h"
#include "trace_log_bk_c.h"
#include "dc_log_bk_c.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
/**
#define PAGE_CNT 48
**/
#define PAGE_CNT 42

struct trace_log_bk_c	s_trace_log_bk;
struct dc_log_bk_c	s_dc_log_bk;
struct tx_def_c s_tx_def;
struct dd_mst_c s_dd_mst;
struct ln_mst_c s_ln_mst;
struct td_mst_c s_td_mst;
struct in_mst_c s_in_mst;
struct com_item_c s_com_item;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt997( )
{
	int    ttlnum;
	char   old_br_no[6];
	char   filename[100];

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
	memset(&s_tx_def,0x00,sizeof(struct tx_def_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&s_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy( old_br_no , "*****" );

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	/*strcpy( filename , "BRlsqd" );	*/
	strcpy( filename , "RPT997" );
	strcpy( vpfm, "RPT997");
	/*strcpy( vpfm, "lsqd" );*/

	/*ret = Trace_log_bk_Dec_Sel( g_pub_tx.reply, "1=1 ORDER BY tx_br_no,tel,trace_no,trace_cnt,sts,opn_br_no" );*/
	/****
	ret = Trace_log_bk_Dec_Sel( g_pub_tx.reply, "sub_tx_code<>'Z004' and substr(tx_br_no,1,1) != '0' and 1=1 ORDER BY tx_br_no,tel,trace_no,trace_cnt,sts,opn_br_no" );
	****/
	/****村镇银行机构号都是'0'开头,在此屏蔽掉  20100714****/
	ret = Trace_log_bk_Dec_Sel( g_pub_tx.reply, "sub_tx_code<>'Z004'  and 1=1 ORDER BY tx_br_no,tel,trace_no,trace_cnt,sts,opn_br_no" );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Trace_log_bk_Fet_Sel( &s_trace_log_bk,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的流水!" );
				WRITEMSG
			}
			break;
		}
		else if( ret ){
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}

		if(s_trace_log_bk.tx_br_no[0] =='0' )/****目前村镇银行的机构号为 0 开头 20100722****/
		{
			memcpy(s_trace_log_bk.tx_br_no,"10001",sizeof(s_trace_log_bk.tx_br_no));
		}else if(s_trace_log_bk.tx_br_no[0] =='6' || s_trace_log_bk.tx_br_no[0] =='7')/***  新柜面通 如果是6,7开头的机构号 新生成一个文件 武大为 20150722 ***/
		{
			memcpy(s_trace_log_bk.tx_br_no,"70000",sizeof(s_trace_log_bk.tx_br_no));
		}

		/* 机构变换时 */
		pub_base_strpack( s_trace_log_bk.tx_br_no );
		if( strcmp( s_trace_log_bk.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}

    	/* 查询借贷分录和交易金额 */
    memset(&s_dc_log_bk,0x00,sizeof(struct dc_log_bk_c));
    ret = Dc_log_bk_Sel( g_pub_tx.reply, &s_dc_log_bk, "trace_no='%ld' and trace_cnt='%ld' ",s_trace_log_bk.trace_no,s_trace_log_bk.trace_cnt );
	  
    	/* 打印信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		sprintf( acErrMsg, "lanceye=第[%d]行!",line );
		WRITEMSG
		if( line_file>PAGE_CNT )
		{
			line_file=0;
			page++;
			sprintf( acErrMsg, "lanceye=第[%d]页!",page );
			WRITEMSG			
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			
			
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}

	Trace_log_bk_Clo_Sel( );
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终流水清单处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终流水清单处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
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
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
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
	
	/*fprintf (fp,"\f");*/
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
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	char br_name[71];
	int i=0; 
	memset(&s_tx_def,0x00,sizeof(struct tx_def_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&s_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&s_com_item,0x00,sizeof(struct com_item_c));
	

	vtcp_log("TRACE_LOG.BR_NO==============[%s]",s_trace_log_bk.tx_br_no);

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_trace_log_bk.tx_date/10000 , s_trace_log_bk.tx_date%10000/100 , s_trace_log_bk.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			if(s_trace_log_bk.tx_br_no[0] =='0' ||s_trace_log_bk.tx_br_no[0] =='6' ||  s_trace_log_bk.tx_br_no[0] =='7')/****目前村镇银行的机构号为 0 开头 20100722****/
			{
				memcpy(s_trace_log_bk.tx_br_no,"10001",sizeof(s_trace_log_bk.tx_br_no));
			}
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			if(s_trace_log_bk.tx_br_no[0] =='0' ||s_trace_log_bk.tx_br_no[0] =='6' ||  s_trace_log_bk.tx_br_no[0] =='7')/****目前村镇银行的机构号为 0 开头 20100722****/
			{
				memcpy(s_trace_log_bk.tx_br_no,"10001",sizeof(s_trace_log_bk.tx_br_no));
			}
			ret = pub_base_getbrname( s_trace_log_bk.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,br_name,s_trace_log_bk.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 add by martin 取消流水前加（*） */
		        if( strcmp( s_trace_log_bk.sts , "1" ) )
		        {
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_no); 
			break; 
			}else{
			sprintf(fmt,"*%%%dld",--bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_no); 
			break;
		        }
		case 'F': /* 记账员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tel);
			break;
		case 'G': /* 复核员,授权人 */
			if( strlen(s_trace_log_bk.chk) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_trace_log_bk.chk );
			}
			else if( strlen(s_trace_log_bk.aut) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_trace_log_bk.aut );
			}
			else 
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str,fmt,"    " );
			}
			break;
		case 'H': /* 交易码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tx_code);
			break;
		case 'I': /* 账      号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.ac_no);
			break;
		case 'J': /* 账户序号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_trace_log_bk.ac_seqn);
			break;
		case 'K': /* 交  易  额 */
			/*sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt );modify by martin 2008/12/23 10:43:44*/
			sprintf( amt_tmp, "%.2lf" , s_dc_log_bk.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.prdt_no);
			break;
		case 'M': /* 现转 */
			pub_base_dic_show_str(tmp_inf,"rpt_ct_ind",s_trace_log_bk.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'N': /* 代理行 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.opn_br_no);
			break;
		case 'O': /* 状态 */
			pub_base_dic_show_str(tmp_inf , "rpt_log_sts" , s_trace_log_bk.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt , tmp_inf);
			break;
		case 'P': /* 交易日期 */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_date);
			break;
		case 'Q': /* 交易时间 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_time);
			break;
		case 'R': /* 流水笔次 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_cnt);
			break;		
		case 'S': /* 摘要 */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, s_trace_log_bk.brf);
			break;
		case 'T': /* 借贷 */
			if( s_dc_log_bk.dc_ind[0]=='1' )
				    strcpy( tmp_inf,"借" );
		  else if( s_dc_log_bk.dc_ind[0]=='2' )
				    strcpy( tmp_inf,"贷" );
      else
            strcpy( tmp_inf,"--" ); 
			/*if( s_trace_log_bk.add_ind[0]=='0' )
				strcpy( tmp_inf,"借" );
			else if( s_trace_log_bk.add_ind[0]=='1' )
				strcpy( tmp_inf,"贷" );
			else 
				strcpy( tmp_inf,"--" );*/

			pub_base_strpack(tmp_inf);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
/***20061018 by martin***/
/*户名*/
     case'U':/*户    名*/
     	if( s_trace_log_bk.ac_id ==0 )
     	{
     		vtcp_log("[%s][%d]11111111111111break==============!\n",__FILE__,__LINE__);
     		memset( str,' ',bll ); 
     		break;
     	}
     	else
     	{
     				vtcp_log("[%s][%d]TRACE_LOG.ac_id==============[%ld]!\n",__FILE__,__LINE__,s_trace_log_bk.ac_id);
     				/*in_mst*/
     				memset(&s_in_mst,0x00,sizeof(struct in_mst_c)); 
     				ret=In_mst_Sel( g_pub_tx.reply, &s_in_mst , "ac_id='%ld' and ac_seqn='%ld' ",s_trace_log_bk.ac_id,s_trace_log_bk.ac_seqn);
     				if(ret ==0)
     				{
     		  			pub_base_strpack( s_in_mst.name );
	        			sprintf(fmt, "%%-%ds", bll);
	        			sprintf(str, fmt, s_in_mst.name );
			    			break;
     	  		}
        		else
      {
      /*td_mst*/
      ret=Td_mst_Sel( g_pub_tx.reply, &s_td_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",s_trace_log_bk.ac_id,s_trace_log_bk.ac_seqn);
    	 if(ret==0)
     		{
     		  pub_base_strpack( s_td_mst.name );
	        sprintf(fmt, "%%-%ds", bll);
	        sprintf(str, fmt, s_td_mst.name );
			    break;
     	  }
        else 
      /*dd_mst*/
      ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",s_trace_log_bk.ac_id,s_trace_log_bk.ac_seqn);
     		if(ret==0)
     		{
     		  pub_base_strpack( s_dd_mst.name );
	        sprintf(fmt, "%%-%ds", bll);
	        sprintf(str, fmt, s_dd_mst.name );
			    break;
     	  }
        else
      /*ln_mst*/
      ret=Ln_mst_Sel( g_pub_tx.reply, &s_ln_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",s_trace_log_bk.ac_id,s_trace_log_bk.ac_seqn);
    		if(ret==0)
     		{
     		  pub_base_strpack( s_ln_mst.name );
	        sprintf(fmt, "%%-%ds", bll);
	        sprintf(str, fmt, s_ln_mst.name );
			    break;
     	  }
        else
        {
        memset( str,' ',bll ); 
       				break;
       	}
       }
      }
/******/
/***20061018 by martin***/
     case'V':/*交易名称*/
     	ret=Tx_def_Sel( g_pub_tx.reply, &s_tx_def , "1=1 and tx_code='%s' ",s_trace_log_bk.tx_code );
	    vtcp_log("[%s][%d]TRACE_LOG.ret==============[%d]!\n",__FILE__,__LINE__,ret);
	    if(ret!=0)
	    {
	    	memset( str,' ',bll );
	    	break;
	    }
	    pub_base_strpack( s_tx_def.tx_name );
	    sprintf(fmt, "%%-%ds", bll);
	    sprintf(str, fmt, s_tx_def.tx_name );
			break;
		case 'X': /* 科目号 */
			ret=Com_item_Sel( g_pub_tx.reply, &s_com_item , "1=1 and acc_hrt='%s' ",s_dc_log_bk.acc_hrt );
	    vtcp_log("[%s][%d]TRACE_LOG.ret==============[%d]!\n",__FILE__,__LINE__,ret);
	    if(ret!=0)
	    {
	    	memset( str,' ',bll );
	    	break;
	    }
	    pub_base_strpack( s_com_item.acc_no );
	    sprintf(fmt, "%%-%ds", bll);
	    sprintf(str, fmt, s_com_item.acc_no );
			break;
/******/
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
