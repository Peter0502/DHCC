/*************************************************************
* 文 件 名:  rpt935.c
* 功能描述： 总 帐 帐 页
* 作    者:  
* 完成日期:  2006年11月09日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret && ret!=100) {\
		sprintf( acErrMsg, "sql error[%d]",ret ); \
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
#include "com_sys_parm_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
#include "gl_mst_hst_c.h"
#define PAGE_CNT 48

struct gl_sub_c	s_gl_sub;
struct gl_sub_c	sGl_sub;
struct gl_mst_hst_c s_gl_mst_hst;
int flag=0;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char acc_type[2];
int opncnt;
FILE *fp; 
int ret = 0;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt111( )
{
	int tag = 0;
	int    ttlnum;
	int    cnt=0;  /** aaa***/
	char   old_acc[4];
	char   filename[100];
	char   old_br_no[8];

	struct com_sys_parm_c s_com_sys_parm;
	struct com_item_c s_com_item;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

  memset(old_acc, 0 ,sizeof(old_acc));
	memset(old_br_no,0,sizeof(old_br_no));

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	strcpy( filename , "RPT935" );
	strcpy( vpfm, "RPT935");

	ret = Gl_mst_hst_Dec_Sel( g_pub_tx.reply, "cur_no='01' and substr(acc_hrt,1,1)>='1' and substr(acc_hrt,1,1)>'5'\
	and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' ) ORDER BY br_no,acc_hrt,date" );
	ERR_DEAL
	
	ttlnum=0;
	while(1)
	{
		ret = Gl_mst_hst_Fet_Sel( &s_gl_mst_hst,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail( );
			
				fclose(fp);   /** aaa **/
			}
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的流水!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		  /* 不打印空白科目 add by martin 20061116*/
		  	if(!pub_base_CompDblVal(s_gl_mst_hst.rdd_amt,0.00) &&
		  	   !pub_base_CompDblVal(s_gl_mst_hst.rcd_amt,0.00) &&
		  	   !pub_base_CompDblVal(s_gl_mst_hst.dr_bal,0.00) &&
		  	   !pub_base_CompDblVal(s_gl_mst_hst.cr_bal,0.00)  )
		  {
		  	 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt='%s' \
			       	and (ydr_bal<>0 or ycr_bal<>0 or ydr_amt<>0 or ycr_amt<>0 or mdr_bal<>0 or mcr_bal<>0 )" ,s_gl_mst_hst.br_no, s_gl_mst_hst.acc_hrt);
			       	 if( ret==100 ) 
			       	 {
			       	   continue;
			         }
		  }
		

    sprintf(acErrMsg,"++++br_no=[%s],acc_hrt=[%s]!",s_gl_mst_hst.br_no,s_gl_mst_hst.acc_hrt);
	  WRITEMSG
    if(memcmp(old_br_no,s_gl_mst_hst.br_no,sizeof(s_gl_mst_hst.br_no)-1) )
    {
    	if(strlen(old_br_no))
    	{
    	   memset(old_acc, 0 ,sizeof(old_acc));
	       ret = print_tail( );
			   ERR_DEAL
			   fclose(fp);
	    }
			page = 1;
			line_file = 0;
			cnt = sql_count( "gl_mst_hst" , "cur_no='01' and br_no='%s' and substr(acc_hrt,1,1)>='1' and substr(acc_hrt,1,1)<='5'\
	        and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' )", s_gl_mst_hst.br_no );
		  sprintf(acErrMsg,"机构[%s]有记录[%d]",s_gl_mst_hst.br_no,cnt);
	    WRITEMSG
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_gl_mst_hst.br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_gl_mst_hst.br_no , filename );
			ERR_DEAL
			
	    memset(old_br_no,0,sizeof(old_br_no));
   	  memcpy(old_br_no,s_gl_mst_hst.br_no,sizeof(s_gl_mst_hst.br_no)-1); 
   	  
   	}

		/* 科目类变换时 */
		if(memcmp(old_acc,s_gl_mst_hst.acc_hrt,sizeof(old_acc)-1) )
		{
			if(strlen(old_acc)) 
			{
				/* 打印页尾 */	  
				ret = print_tail( );
				ERR_DEAL
				line_file = 0;
			}
				
			ret=Gl_sub_Sel( g_pub_tx.reply , &s_gl_sub , "br_no='%s' and cur_no='01' and acc_hrt='%s' " ,s_gl_mst_hst.br_no, s_gl_mst_hst.acc_hrt );
			  
		  memset(old_acc,0,sizeof(old_acc));
   	  memcpy(old_acc,s_gl_mst_hst.acc_hrt,sizeof(old_acc)-1);
	 
	    ret = print_head( );     /* 打表头 */
	    ERR_DEAL 
	    page++;
	    
	 }

    /* 打印科目信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		
		if( s_gl_mst_hst.date%100 ==10 || s_gl_mst_hst.date%100 ==20 )			
		{
			if(s_gl_mst_hst.date%100 ==10)
			{
				flag=0;
			}else if(s_gl_mst_hst.date%100 ==20)
			{
				flag=1;
			}
			/* 打小计 */
			ret = print_XJ();
			ERR_DEAL
	  /*	line_file++; */
		}
		ttlnum++;
	}	
	Gl_mst_hst_Clo_Sel( );
	
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"支行日报表生成处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"支行日报表生成处理失败!!!!![%s]",g_pub_tx.reply);
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
	
	vtcp_log("AAAAAAAAAAA----------------------[%s]",s_gl_mst_hst.acc_hrt );
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	
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

/* 打印小计 */
int print_XJ( )
{
	char vrec[3];
	
	vtcp_log("BBBBBBBBBBBB----------------------[%s]",s_gl_mst_hst.acc_hrt );
	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 打印表尾月计 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	fprintf(fp,"\f");
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 表内打印表体 */
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
	struct com_item_c s_com_item;

	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41];  
	char tmp_inf[41];
	char tmp_acc_name[41];
	char fmt[20]; 
	char br_name[71];
	double tmp_amt;
	double tmp_amt1;
	int i=0; 

  pub_base_strpack( s_gl_sub.br_no );
  
	switch( bli ) 
	{ 
/*		case 'A':  交易日期 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.date/10000 , g_pub_tx.date%10000/100 , g_pub_tx.date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
*/
			case 'A':  /* 日期 */
			sprintf( t_str , "%4d年  %2d月" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
			
		case 'B': /* 科目名称 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( s_gl_sub.br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%5s 行名:%s " , s_gl_sub.br_no, br_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E':/* 上年底借余额 */
			if( s_gl_sub.ydr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'F': /* 上年底贷余额 */
			if( s_gl_sub.ycr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;	
		case 'G': /* 止上月底借发生额 */
			memset( str,' ',bll );  
			break;
		case 'H': /* 止上月底贷发生额 */
			memset( str,' ',bll ); 
			break; 
		case 'J': /* 本年累计借发生额 */
			if( s_gl_sub.ydr_amt==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_amt ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;	
		case 'K': /* 本年累计贷发生额 */	
			if( s_gl_sub.ycr_amt==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_amt ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'M': /* 上月底借余额 */
			if( s_gl_sub.mdr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;	
		case 'N': /* 上月底贷余额 */	
			if( s_gl_sub.mcr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'L': /* 日期 */
			sprintf( t_str , "%2d" , s_gl_mst_hst.date%100 );
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
			
		case 'O': /* 本日借发生额 */
			if( s_gl_mst_hst.rdd_amt==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.rdd_amt ); 
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'P': /* 本日贷发生额 */
			if( s_gl_mst_hst.rcd_amt==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.rcd_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'Q': /* 本日借余额 */
			if( s_gl_mst_hst.dr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.dr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.dr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp );
		  } 
			break;
		case 'R': /* 本日贷余额 */
			if( s_gl_mst_hst.cr_bal==0 )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst_hst.cr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;			
		case 'I': /* 科目号 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'S': /* 借发生额小计 */
			vtcp_log( "s_gl_mst_hst.acc_hrt====================[%s]",s_gl_mst_hst.acc_hrt );
			if(flag==0)
			{
				ret = sql_sum_double( "gl_mst_hst","rdd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date>='%d01' and date<='%d10' ", s_gl_mst_hst.acc_hrt,s_gl_mst_hst.br_no, g_pub_tx.tx_date/100, g_pub_tx.tx_date/100);
      }
      if(flag==1)
      {
				ret = sql_sum_double( "gl_mst_hst","rdd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date>='%d11' and date<='%d20'", s_gl_mst_hst.acc_hrt,s_gl_mst_hst.br_no, g_pub_tx.tx_date/100, g_pub_tx.tx_date/100);
			}
			  ERR_DEAL
			if( tmp_amt==0  )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'T': /* 贷发生额小计 */
			if(flag==0)
			{
				ret = sql_sum_double( "gl_mst_hst","rcd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date>='%d01' and date<='%d10' ", s_gl_mst_hst.acc_hrt,s_gl_mst_hst.br_no , g_pub_tx.tx_date/100, g_pub_tx.tx_date/100);
			}
      if(flag==1)
      {
      	ret = sql_sum_double( "gl_mst_hst","rcd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date>='%d11' and date<='%d20' ", s_gl_mst_hst.acc_hrt,s_gl_mst_hst.br_no , g_pub_tx.tx_date/100, g_pub_tx.tx_date/100);
			}
			ERR_DEAL
			
			if( tmp_amt==0  )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
		  }
			break;
		case 'U': /* 借发生额月计 */
				ret = sql_sum_double( "gl_mst_hst","rdd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date like '%d%%' ", s_gl_sub.acc_hrt ,s_gl_sub.br_no, g_pub_tx.tx_date/100 );
		/*	  if( s_gl_mst_hst.acc_hrt[0]=='4' )
			  {
			  ret = sql_sum_double( "gl_mst_hst","dr_bal",&tmp_amt1,"cur_no='01' and acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  } */
			ERR_DEAL
			if( tmp_amt==0  )
			{
				memset( str,' ',bll );
			}
			else
			{
			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			}
			break;
		case 'V': /* 贷发生额月计 */
				ret = sql_sum_double( "gl_mst_hst","rcd_amt",&tmp_amt,"cur_no='01' and acc_hrt='%s' and br_no='%s' and date like '%d%%' ", s_gl_sub.acc_hrt ,s_gl_sub.br_no, g_pub_tx.tx_date/100);
	/*		  if(s_gl_mst_hst.acc_hrt[0]=='4')
			  {
			  ret = sql_sum_double( "gl_mst_hst","cr_bal",&tmp_amt1,"cur_no='01' and acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  } */
			ERR_DEAL

			if( tmp_amt==0  )
			{
				memset( str,' ',bll );
			}else
			{
			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
		  }
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
