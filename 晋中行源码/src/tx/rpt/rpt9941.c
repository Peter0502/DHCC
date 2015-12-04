/*************************************************************
* 文 件 名:  rpt9941.c
* 功能描述： 日报表打印
* 作    者:  
* 完成日期:  2003年08月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "[%s][%d] sql error",__FILE__,__LINE__ ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  MAX_RECS    48    /** 一页打印多少笔记录 **/
#define  MAX_LINES   110   /** 一页最多打印多少行 **/
#define  AMT_LEN     16    /** 报表中金额的长度 **/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"

FILE *fp;            /** 报表文件指针 **/
int totpage=0;       /** 总共的页码 **/
int page=0;          /** 当前页码 **/
int ret = 0;         /** 错误标志: 0-正确  其他-错误 **/
int totcnt = 0;      /** 记录总笔数 **/
int lines = 0;       /** 当前打印到第几行 **/
int opncnt = 0;
char vpfm[21];         /** 报表格式名 **/
char   old_acno[8];    /** 保存上次循环的科目控制字,用于判断科目控制字是否变化 **/
char   old_brno[8];    /** 保存上次机构编码,用于控制报表结束和创建新的报表文件 **/
char   filename[100];  /** 报表文件名 **/

struct gl_mst_c	s_gl_mst;
struct com_sys_parm_c s_com_sys_parm;
struct {
	char ac_no[8];
	char ac_name[31];
	char act_nums[7];
	char lst_dr_bal[17];
	char lst_cr_bal[17];
	char dr_txamt[17];
	char cr_txamt[17];
	char td_act_nums[7];
	char td_dr_bal[17];
	char td_cr_bal[17];
	char td_tx_nums[7];
} prt_data;
int get_ratio_lsqd(int bli,int bll,char str[100]);
void rpt9441_init();
int rpt_9441_prepare_hj_data();
int rpt_9441_prepare_data();
int rpt_9441_end();
int rpt_9441_print_tail();
int rpt_9441_page_nums();
int rpt_9441_print_head();

int rpt9941()
{
	/** 打开事物,建立数据库连接 **/
	ret=sql_begin();
	if( ret )
	{
	  sprintf( acErrMsg, "[%s][%d] 打开事务失败!!!",__FILE__,__LINE__ );
		WRITEMSG
		goto ErrExit;
	}
	
	/** 初始化全局变量 **/
	rpt9441_init();
	
		
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	
	
	/** 报表格式及报表名称 **/
	strcpy( filename , "BHDRPT101" );	
	strcpy( vpfm, "BHDRPT101" );
	

	/** 查询总帐(人民币) **/
	ret = Gl_mst_Dec_Sel( g_pub_tx.reply, "cur_no='01' ORDER BY acc_hrt" );
	ERR_DEAL
	
	while(1)
	{
	   ret = Gl_mst_Fet_Sel( &s_gl_mst,g_pub_tx.reply );
	   
	   /** 判断查询是否出错或结束 **/
	   rpt_9441_end();
	   if( ret ) 
	   {
      sprintf(acErrMsg,"[%s][%d]读取流水信息表异常出错!",__FILE__,__LINE__);
      WRITEMSG	
      goto ErrExit;			
		 }
		
		 /** 机构改变时,结束上一个报表,开始创建下一个机构报表文件 **/
		 if( memcmp(old_brno,s_gl_mst.br_no,sizeof(s_gl_mst.br_no)) )	
		 {      
         lines = 0;
        	       
         /** 删除原文件 **/
	       ret = pub_rpt_rmfile( s_gl_mst.br_no , filename , 0 );
	       ERR_DEAL

	       /** 打开文件 **/	
	       ret = pub_rpt_openfile( &fp, s_gl_mst.br_no , filename );
	       ERR_DEAL
	       
	       /** 计算页码数 **/
	       ret = rpt_9441_page_nums();
	       ERR_DEAL
	       
	       /** 打印表头 **/
	       ret = rpt_9441_print_head();
	       ERR_DEAL
		 }
		 
		 /** 判断，满页后换页 **/
		 if ( lines >= MAX_LINES )
		 {
		 	   rpt_9441_print_tail();
		 	   ret = rpt_9441_print_head();
	       ERR_DEAL
		 }
		
		 /** 判断科目归属改变时打印合计 **/
		 if( s_gl_mst.acc_hrt[0] != old_acno[0] )
	 	 {
	 	 	 /** 如果表内科目结束，表外科目开始，换格式 **/
	 	 	 if(old_acno[0] == '5' && s_gl_mst.acc_hrt[0] == '6')
	 	 	 {
	 	 	    ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","CC",&opncnt,get_ratio_lsqd);
	 	 	 }
	 	 	 
	 	 	 /** 打印合计后再判断一遍，满页后换页 **/
		   if ( lines >= MAX_LINES )
		   {
		 	     rpt_9441_print_tail();
		 	     ret = rpt_9441_print_head();
	         ERR_DEAL
		   }
	 	 	 
		   ret = rpt_9441_prepare_hj_data();
		   if( ret ) {
         sprintf(acErrMsg,"[%s][%d]计算科目合计出错!",__FILE__,__LINE__);
         WRITEMSG			
         goto ErrExit;			
		   }
		   
		   /** 打印合计后再判断一遍，满页后换页 **/
		   if ( lines >= MAX_LINES )
		   {
		 	     rpt_9441_print_tail();
		 	     ret = rpt_9441_print_head();
	         ERR_DEAL
		   }
		 }
		 memcpy(old_acno,s_gl_mst.acc_hrt,sizeof(s_gl_mst.acc_hrt)-1);
		
		 /** 给打印结构赋值 **/
		 ret = rpt_9441_prepare_data();
		 if( ret ) 
		 {
         sprintf(acErrMsg,"[%s][%d]报表打印出错!",__FILE__,__LINE__);
         WRITEMSG			
         goto ErrExit;			
		 }
		
		 /** 记录总数增加 **/
     totcnt++;
		
	} /** while 循环结束 **/
	
	
	OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日报表生成处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日报表生成处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

void rpt9441_init()
{
	memset(vpfm,0,sizeof(vpfm));
	memset(filename,0,sizeof(filename));
	memset(old_acno,0,sizeof(old_acno));
	memset(old_brno,0,sizeof(old_brno));
	pub_base_sysinit();
}	

int rpt_9441_prepare_hj_data()
{
	  char   amt_tmp[31];
	  double tmp_amt;
	  memset(&prt_data,0,sizeof(prt_data));
	
	  /** 6 - 7 类科目都归到表外 **/
	  if(old_acno[0] == '6' && s_gl_mst.acc_hrt[0] =='7')
	  	return(0);
	
    /** 合计名称 **/
    switch(s_gl_mst.acc_hrt[0])
    {
    	case '1':
    		sprintf(prt_data.ac_name,"一、资产类小计");
    		break;
    	case '2':
    		sprintf(prt_data.ac_name,"二、负债类小计");
    		break;
    	case '3':
    		sprintf(prt_data.ac_name,"三、所有者权益类小计");
    		break;
    	case '4':
    		sprintf(prt_data.ac_name,"四、资产负债共同类小计");
    		break;
    	case '5':
    		sprintf(prt_data.ac_name,"五、损益类小计");
    		break;
    	case '6':
    		sprintf(prt_data.ac_name,"表外科目合计");
    		break;   		
    	default:
    		return(-1);
    }
    
    /** 昨日借方余额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","ldd_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.lst_dr_bal,amt_tmp,sizeof(prt_data.lst_dr_bal)-1);
	  
	  /** 昨日贷方余额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","lcd_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.lst_cr_bal,amt_tmp,sizeof(prt_data.lst_cr_bal)-1);
	  
	  /** 今日借方发生额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rdd_amt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.dr_txamt,amt_tmp,sizeof(prt_data.dr_txamt)-1);
	  
	  /** 今日贷方发生额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rcd_amt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.cr_txamt,amt_tmp,sizeof(prt_data.cr_txamt)-1);
	  
	  /** 今日借方余额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","dr_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.td_dr_bal,amt_tmp,sizeof(prt_data.td_dr_bal)-1);
	  
	  /** 今日贷方余额 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","cr_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.td_cr_bal,amt_tmp,sizeof(prt_data.td_cr_bal)-1);
	  
	  /** 今日发生数 **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rdd_cnt+rcdcnt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%6.0lf" , tmp_amt ); 
	  strncp(prt_data.td_tx_nums,amt_tmp,sizeof(prt_data.td_tx_nums)-1);
		
    if(s_gl_mst.acc_hrt[0] < '6')
	     ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","AA",&opncnt,get_ratio_lsqd);
	  else
	 	   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);	 
  
    return(ret);
}

int rpt_9441_prepare_data()
{
	 char   amt_tmp[31];
	 struct com_item_c s_com_item;
	
   memset(&prt_data,0,sizeof(prt_data));
  	
   /** 科目 **/
   pub_base_strpack(s_gl_mst.acc_hrt);
	 strcpy(prt_data.ac_no,s_gl_mst.acc_hrt);

   /** 科目名称 **/
   ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								prt_data.ac_no );
   if(ret)
      return(ret);
   pub_base_strpack(s_com_item.acc_name);
	 strcpy(prt_data.ac_name,s_com_item.acc_name);
	 
	 /** 昨日户数 **/
	 
	 /** 昨日借方余额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal );
	 strncp(prt_data.lst_dr_bal,amt_tmp,sizeof(prt_data.lst_dr_bal)-1);
	 
	 /** 昨日贷方余额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal );
	 strncp(prt_data.lst_cr_bal,amt_tmp,sizeof(prt_data.lst_cr_bal)-1);
	 
	 /** 今日借方发生额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt );
	 strncp(prt_data.dr_txamt,amt_tmp,sizeof(prt_data.dr_txamt)-1);
	 
	  /** 今日贷方发生额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt );
	 strncp(prt_data.cr_txamt,amt_tmp,sizeof(prt_data.cr_txamt)-1);
	 
	 /**今日户数 **/
	 
	 /** 今日借方余额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal );
	 strncp(prt_data.td_dr_bal,amt_tmp,sizeof(prt_data.td_dr_bal)-1);
	 
	 /** 今日贷方余额 **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal );
	 strncp(prt_data.td_cr_bal,amt_tmp,sizeof(prt_data.td_cr_bal)-1);
	 
	 /** 业务量 **/
	 sprintf(prt_data.td_tx_nums,"%06d",s_gl_mst.rdd_cnt+s_gl_mst.rcd_cnt);
	 
	 if(old_acno[0] < '6')
	     ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","AA",&opncnt,get_ratio_lsqd);
	 else
	 	   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);	 
  
   return(ret);
}

int rpt_9441_end()
{
	   if( ret==100 ) 
     {
       if( totcnt )
       {
         /** 结束,打页尾 **/
         rpt_9441_print_tail();
         return(ret);
         
       } else {
         sprintf( acErrMsg, "[%s][%d]本日没有要打印的流水!" ,__FILE__,__LINE__);
         WRITEMSG
         return(ret);
       }
     } else if( ret ) {
        sprintf(acErrMsg,"[%s][%d]读取流水信息表异常出错!",__FILE__,__LINE__);
        WRITEMSG	
        return(ret);				
		 }
}

int rpt_9441_print_tail()
{
	  if( old_acno[0]<6)
        ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","BB",&opncnt,get_ratio_lsqd);
    else
    	  ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);
    return(ret);
}

int rpt_9441_page_nums()
{
	  long lpage = 0;
	  
	  /** 统计当前机构科目记录数 **/
	  ret = sql_sum_long("gl_mst","acc_hrt",&lpage," where br_no = '%s' ",s_gl_mst.br_no);
	  if( ret ) 
	  {
      sprintf(acErrMsg,"[%s][%d]统计记录数出错!",__FILE__,__LINE__);
      WRITEMSG	
      return(ret);
    }
    
    /** 计算页码总数 **/
    totpage = (int) (lpage / MAX_RECS);
    
    totpage++;
    
}

int rpt_9441_print_head()
{
   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","HH",&opncnt,get_ratio_lsqd);
   return(ret);
}

/* 赋值函数 */
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char t_str[101]; 
	char fmt[20]; 

	switch( bli ) 
	{ 
		case 'A': /* 机构编号 */ 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_gl_mst.br_no ); 
			break;
		case 'B': /* 机构名称 */
			ret = pub_base_getbrname( s_gl_mst.br_no, t_str );
			ERR_DEAL
			pub_base_strpack( t_str ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'P': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
    case 'C': /* 页码合计 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, totpage); 
			break; 
		case 'D': /* 页码 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, page); 
			break;
    case 'E': /* 科目编号 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.ac_no); 
			break;
		case 'F': /* 科目名称 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.ac_name); 
			break;
		case 'G': /* 昨日户数 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.act_nums); 
			break;
		case 'H': /* 昨日借方余额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.lst_dr_bal); 
			break;
		case 'I': /* 昨日贷方余额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.lst_cr_bal); 
			break;
		case 'J': /* 本日借方发生额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.dr_txamt); 
			break;
		case 'K': /* 本日贷方发生额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.dr_txamt); 
			break;
		case 'L': /* 本日户数 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_act_nums); 
			break;
		case 'M': /* 本日借方余额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_dr_bal); 
			break;
		case 'N': /* 本日贷方余额 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_cr_bal); 
			break;
		case 'O': /* 今日业务量 */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_tx_nums); 
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
