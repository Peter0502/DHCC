/*************************************************************
* 文 件 名: bc9AA.c
* 功能描述：导没有分户的科目的交易明细
*
* 作    者: dadary
* 完成日期: 2007年1月16日
*
*注意:此程序每天运行并且要求在翻盘后执行，而且已经完成了汇总帐
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define extern
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \ 
		}
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "dc_log_rz_c.h"
#include "acc_hrt_log_c.h"
#include "com_item_c.h"
#include "gl_sub_c.h"


bc9AA()
{
	int ret ;
	struct acc_hrt_log_c 		 s_acc_hrt_log;
	struct dc_log_rz_c		 s_dc_log_bk;
	struct com_item_c		 s_com_item;
	struct gl_sub_c			 s_gl_sub;
	struct com_sys_parm_c		 s_com_sys_parm;
	
	double d_bal			=	0.0;
	
	long   beg_tx_date		=	0;	/** > 导入的起始日期，表中已经存在的最大的日期**/
	long   end_tx_date		=	0;	/** <= 导入的结束日期，表中已经存在的最大的日期**/
	
	char pre_br_no[6];
	char pre_acc_hrt[6];


	memset(pre_br_no,0x00,sizeof(pre_br_no));
	memset(pre_acc_hrt,0x00,sizeof(pre_acc_hrt));
	
	memcpy(pre_br_no,"-----",5);
	memcpy(pre_acc_hrt,"-----",5);
	vtcp_log("[%s][%d]开始进行导入没有分户帐的科目的明细!\n",__FILE__,__LINE__);
	
	
	ret = sql_begin(); /*打开事务*/
    	if( ret != 0 )
    	{
    	    sprintf( acErrMsg, "BEGIN WORK error!!!" );
    	    WRITEMSG
    	    goto ErrExit;
    	}
    	
    	/**------- 初始化全局变量 --------**/
    	pub_base_sysinit();
	
	
		/**获取起始日期***/
	ret = sql_max_long_xier ("acc_hrt_log","tx_date",&beg_tx_date,"1=1");
	ERR_DEAL
	beg_tx_date=20070700;
	/****先写死了***/
	
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL
	
	end_tx_date = 20070799;
	
	printf("[%s][%d]下面程序导入从 大于[%ld]-- 小于或者等于[%ld]的报表数据!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	vtcp_log("[%s][%d]下面程序导入从 大于[%ld]-- 小于或者等于[%ld]的报表数据!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	ret = Dc_log_rz_Dec_Sel(g_pub_tx.reply,"acc_hrt in ( select acc_hrt from com_item where acc_knd='3')  and sts='0' and tx_date>%ld and tx_date<=%ld order by tx_date,tx_opn_br_no,acc_hrt,trace_no,trace_cnt",beg_tx_date,end_tx_date);
	while(1)
	{
		
		memset(&s_dc_log_bk,0x00,sizeof(s_dc_log_bk));
		memset(&s_com_item, 0x00,sizeof(s_com_item));
		memset(&s_acc_hrt_log,0x00,sizeof(s_acc_hrt_log));
		

		
		ret = Dc_log_rz_Fet_Sel(&s_dc_log_bk,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P001");
			goto ErrExit;
		}
		else if(ret==100)
			break;
		
		ret = Com_item_Sel(g_pub_tx.reply,&s_com_item,"acc_hrt='%s'",s_dc_log_bk.acc_hrt);
		if(ret)
		{
			vtcp_log("[%s][%d]宝贝对不起了，表中找不到你这个科目啊!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P001");
			goto ErrExit;
		}
		
		if(s_com_item.acc_knd[0]!='3') /***如果不是 不开分户的科目 直接跳过***/
				continue;
		
		if(memcmp(pre_br_no,s_dc_log_bk.tx_opn_br_no,5))
		{
		}
		
		/**获取科目当前科目的余额***/
		/**为了保险起见，当机构号变化的时候也要执行下面语句，避免一种情况，相邻两个机构，两机构就一个相同的科目有发生**/
		if(memcmp(pre_acc_hrt,s_dc_log_bk.acc_hrt,5) ||memcmp(pre_br_no,s_dc_log_bk.tx_opn_br_no,5) )
		{
			ret = Gl_mst_hst_Sel(g_pub_tx.reply,&s_gl_sub,"acc_hrt='%s' and br_no='%s' and \"date\"=%ld",
				s_dc_log_bk.acc_hrt,s_dc_log_bk.tx_opn_br_no,s_dc_log_bk.tx_date);
			if(ret && ret != 100)
			{
				vtcp_log("[%s][%d]查找支行总帐文件表错误!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			d_bal = 0.0;
			
			
			if(s_com_item.dc_ind[0]=='0' || s_com_item.dc_ind[0]=='1')
				d_bal = s_gl_sub.ldd_bal-s_gl_sub.lcd_bal;
			else if(s_com_item.dc_ind[0]=='2')
				d_bal = s_gl_sub.lcd_bal- s_gl_sub.ldd_bal;
				
			memcpy(pre_acc_hrt,s_dc_log_bk.acc_hrt,5);
			memcpy(pre_br_no,s_dc_log_bk.tx_opn_br_no,5);
		}	
		/***开始准备插入数据****/
		s_acc_hrt_log.tx_date		=		s_dc_log_bk.tx_date    ;
		s_acc_hrt_log.trace_no          =		s_dc_log_bk.trace_no   ;      
		s_acc_hrt_log.trace_cnt         =		s_dc_log_bk.trace_cnt  ;      
		memcpy(s_acc_hrt_log.entry_code,s_dc_log_bk.entry_code,sizeof(s_acc_hrt_log.entry_code)-1);       
		s_acc_hrt_log.dc_seqn           =		s_dc_log_bk.dc_seqn  ;        
		memcpy(s_acc_hrt_log.opn_br_no    ,s_dc_log_bk.opn_br_no    ,sizeof(s_acc_hrt_log.opn_br_no )-1);     
		memcpy(s_acc_hrt_log.tx_br_no     ,s_dc_log_bk.tx_br_no     ,sizeof(s_acc_hrt_log.tx_br_no )-1);    
		memcpy(s_acc_hrt_log.tx_opn_br_no ,s_dc_log_bk.tx_opn_br_no ,sizeof(s_acc_hrt_log.tx_opn_br_no )-1);    
		memcpy(s_acc_hrt_log.tx_tx_br_no  ,s_dc_log_bk.tx_tx_br_no  ,sizeof(s_acc_hrt_log.tx_tx_br_no )-1);    
		memcpy(s_acc_hrt_log.cur_no       ,s_dc_log_bk.cur_no       ,sizeof(s_acc_hrt_log.cur_no )-1);    
		memcpy(s_acc_hrt_log.acc_hrt      ,s_dc_log_bk.acc_hrt      ,sizeof(s_acc_hrt_log.acc_hrt )-1);    
		memcpy(s_acc_hrt_log.dc_ind       ,s_dc_log_bk.dc_ind       ,sizeof(s_acc_hrt_log.dc_ind )-1);    
		memcpy(s_acc_hrt_log.ct_ind       ,s_dc_log_bk.dc_ind       ,sizeof(s_acc_hrt_log.dc_ind )-1);    
		memcpy(s_acc_hrt_log.note_type    ,s_dc_log_bk.note_type    ,sizeof(s_acc_hrt_log.note_type )-1);    
		memcpy(s_acc_hrt_log.note_no      ,s_dc_log_bk.note_no      ,sizeof(s_acc_hrt_log.note_no )-1);          
		s_acc_hrt_log.amt               =		s_dc_log_bk.amt     ;         
		            
		memcpy( s_acc_hrt_log.tel         ,s_dc_log_bk.tel           ,sizeof(s_acc_hrt_log.tel )-1);   
		memcpy( s_acc_hrt_log.sts         ,s_dc_log_bk.sts           ,sizeof(s_acc_hrt_log.sts )-1); 
		memcpy( s_acc_hrt_log.print_flag  ,"0"           	     ,sizeof(s_acc_hrt_log.print_flag )-1);   
		s_acc_hrt_log.pagecnt		=		0;
		


		printf("计算开始------机构[%s]科目[%s],com_item.dc_ind=[%c],dc_log.dc_ind=[%c],dbal=[%.2f],amt=[%.2f]\n",
			s_dc_log_bk.tx_opn_br_no,s_dc_log_bk.acc_hrt,s_com_item.dc_ind[0],s_dc_log_bk.dc_ind[0],d_bal,s_dc_log_bk.amt);
		if(s_com_item.dc_ind[0]=='0')/***如果是两性的科目***/
		{
			printf("[%s][%d]两性科目开始更改余额!\n",__FILE__,__LINE__);
			if(s_dc_log_bk.dc_ind[0]=='1')/*两性，借+**/
				d_bal += s_dc_log_bk.amt;
			else if(s_dc_log_bk.dc_ind[0]=='2')/*两性，贷-***/
				d_bal -= s_dc_log_bk.amt;
		}
		else/***如果不是两性类科目，同边相加，异边相减**/
		{
			printf("[%s][%d]单性科目开始更改余额!\n",__FILE__,__LINE__);
			if(s_dc_log_bk.dc_ind[0]!=s_com_item.dc_ind[0])
				d_bal -= s_dc_log_bk.amt;
			else
				d_bal += s_dc_log_bk.amt;
		}
		
		printf("计算结束------机构[%s]科目[%s],com_item.dc_ind=[%c],dc_log.dc_ind=[%c],dbal=[%.2f],amt=[%.2f]\n",
			s_dc_log_bk.tx_opn_br_no,s_dc_log_bk.acc_hrt,s_com_item.dc_ind[0],s_dc_log_bk.dc_ind[0],d_bal,s_dc_log_bk.amt);
		/*********************金额，写金额，写acc_hrt_log的金额*********************/
		s_acc_hrt_log.bal               =		d_bal;        
		
		
		
		ret = Acc_hrt_log_Ins(s_acc_hrt_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]插入记录失败了!ret==[%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acc_hrt_log_Debug(&s_acc_hrt_log);
			goto ErrExit;
		}
	}
	Dc_log_rz_Clo_Sel();
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"导入日志表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"导入日志表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
} 
