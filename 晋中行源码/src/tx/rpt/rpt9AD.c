/*************************************************************
* 文 件 名: rpt9AD.c
* 功能描述：导入活期明细表，内部帐明细表
* 作    者: dadary
* 完成日期: 2007年1月24日
* 说    明: 为了达到更好的效果，导入数据每个月月底执行，放在
*           	     
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
#include "acno_hst_log_c.h"
#include "dd_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "ln_mst_hst_c.h"

long   beg_tx_date		=	0;	/** > 导入的起始日期，表中已经存在的最大的日期**/
long   end_tx_date		=	0;	/** <= 导入的结束日期，表中已经存在的最大的日期**/
int ret ;

rpt9AD()
{
	printf("[%s][%d]开始导入活期明细表以及内部户明细表!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]开始导入活期明细表以及内部户明细表!\n",__FILE__,__LINE__);
	struct com_sys_parm_c		 s_com_sys_parm;
	
	float  d_bal			=	0.0;
	
	
	/**开始事务**/
	ret=sql_begin(); 
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 初始化全局变量 */
	pub_base_sysinit( );
    
    	
	/**获取起始日期***/
	ret = sql_max_long_xier ("acno_hst_log","tx_date",&beg_tx_date,"1=1");
	ERR_DEAL
	
	/***如果需要强制导入某个区间里面的数据再下面写死就可以了 ，注意区间是 前开后闭的  记得导入完成后删除您的改动***/
	beg_tx_date=20070430;
	/******就加一行就可以了****/
	
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL
	
	end_tx_date = s_com_sys_parm.lst_date;
	end_tx_date=20070630;
	
	printf("[%s][%d]下面程序导入从 大于[%ld]-- 小于或者等于[%ld]的报表数据!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	vtcp_log("[%s][%d]下面程序导入从 大于[%ld]-- 小于或者等于[%ld]的报表数据!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	
	
	/***导数据程序*****/
	
	/*if(insert_dd_hst())	goto ErrExit;
	
	if(insert_in_hst())	goto ErrExit;	*/
		
	if(insert_ln_hst()) goto ErrExit;
	
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"导入活期/内部明细成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"导入活期/内部明细失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
} 

int insert_dd_hst()
{
	vtcp_log("[%s][%d]开始导入活期历史明细!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct dd_mst_hst_c	s_dd_mst_hst;
	struct dd_mst_c		s_dd_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**提取acno_hst_log表中当前被打印的最大的序号**/
	long	t_hst_cnt = 0;	/**确保明细笔数一笔一笔增加避免同序号情况**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*记录一下插入的记录的条数**/
	
	ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_dd_mst_hst,0x00,sizeof(s_dd_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = Dd_mst_hst_Fet_Sel(&s_dd_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]操作活期明细表错误!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		/***如果机构号变化,查找dd_mst表获取开户机构,因为明细表中又的记录开户机构写的不对***/
		if(pre_ac_id!=s_dd_mst_hst.ac_id)
		{
			memset(&s_dd_mst,0x00,sizeof(s_dd_mst));
			
			ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_id=%ld",s_dd_mst_hst.ac_id);
			ERR_DEAL
			
			/**从acno_hst_log表中获取到帐号的明细序号***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_dd_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]活期帐户[%ld]当前打印到了[%ld]\n",__FILE__,__LINE__,s_dd_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_dd_mst_hst.ac_id;
			
			
		}
		
		/***明细序号递增***/
		t_hst_cnt ++;
		
		
		/***开始准备待插入数据***/
		memcpy(s_acno_hst_log.ac_type,"1",sizeof(s_acno_hst_log.ac_type)-1);	/*帐户类型 1活期  2 贷款  3 定期***/
		s_acno_hst_log.ac_id	=	s_dd_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_dd_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_dd_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_dd_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_dd_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_dd_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_dd_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_dd_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**处理金额**/
		s_acno_hst_log.pagecnt= 0 ;	/*当前记录属于打印中的第几页**/
		s_acno_hst_log.tx_amt = s_dd_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_dd_mst_hst.bal;
		
		if(s_dd_mst_hst.add_ind[0]=='0')	/*0 表示借方**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_dd_mst_hst.add_ind[0]=='1')	/*1 表示贷方**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]插入记录失败了![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]插入活期[%ld] 明细号[%ld]这是底[%d]条记录!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
	}
	
OkExit:
	vtcp_log("[%s][%d]活期明细导入成功!导入条数[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]活期明细导入失败!\n",__FILE__,__LINE__);
	return 1;
	
}


int insert_in_hst()
{
	vtcp_log("[%s][%d]开始导入内部历史明细!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct in_mst_hst_c	s_in_mst_hst;
	struct in_mst_c		s_in_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**提取acno_hst_log表中当前被打印的最大的序号**/
	long	t_hst_cnt = 0;	/**确保明细笔数一笔一笔增加避免同序号情况**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*记录一下插入的记录的条数**/
	
	ret = In_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_in_mst_hst,0x00,sizeof(s_in_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = In_mst_hst_Fet_Sel(&s_in_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]操作内部明细表错误!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		
		printf("[%s][%d]到了帐号[%ld] 序号＝[%d]\n",__FILE__,__LINE__,s_in_mst_hst.ac_id,s_in_mst_hst.hst_cnt);
		/***如果机构号变化,查找in_mst表获取开户机构,因为明细表中又的记录开户机构写的不对***/
		if(pre_ac_id!=s_in_mst_hst.ac_id)
		{
			memset(&s_in_mst,0x00,sizeof(s_in_mst));
			
			ret = In_mst_Sel(g_pub_tx.reply,&s_in_mst,"ac_id=%ld",s_in_mst_hst.ac_id);
			ERR_DEAL
			
			/**从acno_hst_log表中获取到帐号的明细序号***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_in_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]内部帐户[%ld]当前打印到了[%ld]\n",__FILE__,__LINE__,s_in_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_in_mst_hst.ac_id;
			
			
		}
		
		/***明细序号递增***/
		t_hst_cnt ++;
		
		
		/***开始准备待插入数据***/
		memcpy(s_acno_hst_log.ac_type,"3",sizeof(s_acno_hst_log.ac_type)-1);	/*帐户类型 1内部  2 贷款  3 定期***/
		s_acno_hst_log.ac_id	=	s_in_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_in_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_in_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_in_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_in_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_in_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_in_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_in_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**处理金额**/
		s_acno_hst_log.pagecnt= 0 ;	/*当前记录属于打印中的第几页**/
		s_acno_hst_log.tx_amt = s_in_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_in_mst_hst.bal;
		
		if(s_in_mst_hst.add_ind[0]=='0')	/*0 表示借方**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_in_mst_hst.add_ind[0]=='1')	/*1 表示贷方**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]插入记录失败了![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]插入内部[%ld] 明细号[%ld]这是第[%d]条记录!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
	}
	
OkExit:
	vtcp_log("[%s][%d]内部明细导入成功!导入条数[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]内部明细导入失败!\n",__FILE__,__LINE__);
	return 1;
	
}

int insert_ln_hst()
{
	vtcp_log("[%s][%d]开始导入贷款历史明细!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct ln_mst_hst_c	s_ln_mst_hst;
	struct ln_mst_c		s_ln_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**提取acno_hst_log表中当前被打印的最大的序号**/
	long	t_hst_cnt = 0;	/**确保明细笔数一笔一笔增加避免同序号情况**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*记录一下插入的记录的条数**/
	
	ret = Ln_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_ln_mst_hst,0x00,sizeof(s_ln_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = Ln_mst_hst_Fet_Sel(&s_ln_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]操作贷款明细表错误!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		printf("[%s][%d]到了帐号[%ld] 序号＝[%d]\n",__FILE__,__LINE__,s_ln_mst_hst.ac_id,s_ln_mst_hst.hst_cnt);
		/***如果机构号变化,查找ln_mst表获取开户机构,因为明细表中又的记录开户机构写的不对***/
		if(pre_ac_id!=s_ln_mst_hst.ac_id)
		{
			memset(&s_ln_mst,0x00,sizeof(s_ln_mst));
			
			ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_ln_mst_hst.ac_id);
			ERR_DEAL
			
			/**从acno_hst_log表中获取到帐号的明细序号***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_ln_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]贷款帐户[%ld]当前打印到了[%ld]\n",__FILE__,__LINE__,s_ln_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_ln_mst_hst.ac_id;
			
			
		}
		
		/***明细序号递增***/
		t_hst_cnt ++;
		
		
		/***开始准备待插入数据***/
		memcpy(s_acno_hst_log.ac_type,"2",sizeof(s_acno_hst_log.ac_type)-1);	/*帐户类型 1内部  2 贷款  3 定期***/
		s_acno_hst_log.ac_id	=	s_ln_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_ln_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_ln_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_ln_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_ln_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_ln_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_ln_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_ln_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**处理金额**/
		s_acno_hst_log.pagecnt= 0 ;	/*当前记录属于打印中的第几页**/
		s_acno_hst_log.tx_amt = s_ln_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_ln_mst_hst.bal;
		
		if(s_ln_mst_hst.add_ind[0]=='0')	/*0 表示借方**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_ln_mst_hst.add_ind[0]=='1')	/*1 表示贷方**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]插入记录失败了![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]插入贷款[%ld] 明细号[%ld]这是第[%d]条记录!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
		
		}
	
OkExit:
	vtcp_log("[%s][%d]贷款明细导入成功!导入条数[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]贷款明细导入失败!\n",__FILE__,__LINE__);
	return 1;
	
}