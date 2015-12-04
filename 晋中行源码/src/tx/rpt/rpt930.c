/*************************************************************
* 文 件 名: rpt930.c
* 功能描述：打印总行贷款余额表
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "dc_acc_rel_c.h"								/*增加“科目号”，zhangpeng 20131126*/
#include "com_item_c.h"									/*增加“科目号”，zhangpeng 20131126*/

struct ln_mst_c gLn_mst;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long g_count=0;
static double sum;
static double sumval;
int get_ratio_tdtdb(int bli,int bll,char str[200]);

rpt930()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];
	char prdt_no[5];			/**产品编号**/
	char tmp_str[60];
	char old_br_no[6];

	memset(&sum,0x00,sizeof(sum));
	memset(&sumval,0x00,sizeof(sumval));
	memset(&gLn_mst,0x00,sizeof(struct ln_mst_c));
	memset( wherelist,0,sizeof(wherelist) );
	strcpy(prdt_no,"***");
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
	
	strcpy( filename , "RPT930");
	strcpy(vpfm, "RPT930");
	
	/**组成查询**/
	memset(wherelist,'\0',sizeof(wherelist));
	strcat(wherelist," bal != 0.0 or in_lo_intst!=0.00 or out_lo_intst!=0.00 or cmpd_lo_intst!=0.00 order by opn_br_no,prdt_no,ac_id ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret= Ln_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Ln_mst_Fet_Sel( &gLn_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(sum));
				memset(&sumval,0x00,sizeof(sumval));
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
		pub_base_strpack( gLn_mst.opn_br_no );
		if( strcmp( gLn_mst.opn_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(sum));
				memset(&sumval,0x00,sizeof(sumval));
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			strcpy(prdt_no,"***");/**初始化产品小计**/
			strcpy( old_br_no , gLn_mst.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( gLn_mst.opn_br_no, filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , gLn_mst.opn_br_no, filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 打印信息 */
		vtcp_log("%s,%d,当前产品号：%s,prdt_no->%s",__FILE__,__LINE__,gLn_mst.prdt_no,prdt_no);    			
    if(strcmp( gLn_mst.prdt_no , prdt_no ))
    {
    	if(strcmp(prdt_no,"***"))
    		{
    			vtcp_log("%s,%d,产品号为：%s,小计：%f",__FILE__,__LINE__,gLn_mst.prdt_no,sumval);
    			print_XX();/**打印产品小计**/
    			sumval = 0.0;
    		}else
    		{
    			sumval = 0.0;
    		}
    	strcpy(prdt_no,gLn_mst.prdt_no);
    }
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			print_XX();/**打印产品小计**/
    	sumval = 0.0;
			ret=print_ZZ();
			ERR_DEAL
			memset(&sum,0x00,sizeof(sum));
			memset(&sumval,0x00,sizeof(sumval));
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
	Ln_mst_Clo_Sel();

	
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

/*打印小计*/
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
/*打印产品小计*/
int print_XX()
{
	char vrec[3];
	strcpy(vrec,"XX");
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
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dc_acc_rel_c s_dc_acc_rel;				/*增加“科目号”，zhangpeng 20131126*/
	struct com_item_c s_com_item;						/*增加“科目号”，zhangpeng 20131126*/
	double lixi;
	struct ln_parm_c sLn_parm;
	char str_mon[30];

	memset(&lixi,0x00,sizeof(lixi));
	memset(str_mon,0x00,sizeof(str_mon));
	memset(&sLn_parm,0x00,sizeof(struct ln_parm_c));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_dc_acc_rel, 0x0, sizeof(struct dc_acc_rel_c));	/*增加“科目号”，zhangpeng 20131126*/
	memset(&s_com_item, 0x0, sizeof(struct com_item_c));			/*增加“科目号”，zhangpeng 20131126*/
	
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
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",gLn_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name ,gLn_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'M': /*帐号*/
		       /*ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d",gLn_mst.ac_id);*/
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d order by ac_no",gLn_mst.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"查询介质与帐户关系表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%s" , sMdm_ac_rel.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'N': /*余额*/
			sum+=gLn_mst.bal;
			sumval+=gLn_mst.bal;
			sprintf(t_str,"%.2lf",gLn_mst.bal);
			pub_rpt_flttomon(t_str,str_mon);
			/****
			sprintf(fmt,"%%-%ds",bll);
			****/
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		case 'L':  /*户      名 */
			pub_base_strpack(gLn_mst.name); 
			sprintf( t_str , "%s" , gLn_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'O': /*利率*/
			sprintf(t_str,"%.6lf",gLn_mst.rate);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'F': /*起息日*/
			sprintf(t_str,"%d",gLn_mst.opn_date);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
		case 'I': /*到期日*/
			sprintf(t_str,"%d",gLn_mst.mtr_date);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
			
		/**欠息金额注释掉。zhangpeng 20131126 start**
		case 'J': //欠息金额
			lixi=gLn_mst.in_lo_intst+gLn_mst.out_lo_intst+gLn_mst.cmpd_lo_intst;
			sprintf(t_str,"%.2lf",lixi);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		*欠息金额注释掉。zhangpeng 20131126 end***/
		
		/**将欠息金额区分为表内欠息和表外欠息。zhangpeng 20131126 start***/	
		case 'J': /*表内欠息*/
			sprintf(t_str,"%.2lf",gLn_mst.in_lo_intst);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'Q': /*表外欠息*/
			sprintf(t_str,"%.2lf",gLn_mst.out_lo_intst);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		/**将欠息金额区分为表内欠息和表外欠息。zhangpeng 20131126 start***/
			
		/**产品名称注释掉。zhangpeng 20131126 start**
		case 'K': //产品名称
			ret=Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no='%s'",gLn_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"查询贷款产品参数表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(t_str,"%s",sLn_parm.title);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
		*产品名称注释掉。zhangpeng 20131126 end***/
		
		/**产品名称改为科目号。zhangpeng 20131126 start***/	
		case 'K': /*科目号*/
			ret=Ln_parm_Sel( g_pub_tx.reply ,&sLn_parm,"prdt_no = '%s'",gLn_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"查询贷款产品参数表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			/* 修改科目号搜到是利息科目的情况 */
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code = '%s' and data_code='0152'",sLn_parm.dc_code);
			if( ret )
			{
				sprintf(acErrMsg,"查询会计代码对应科目表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt = '%s'",s_dc_acc_rel.acc_hrt);
			if( ret )
			{
				sprintf(acErrMsg,"查询公共科目表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, s_com_item.acc_no ); 
			memset( &sLn_parm, 0x00 , sizeof( struct ln_parm_c ));
			memset( &s_dc_acc_rel, 0x00 , sizeof( struct dc_acc_rel_c ));
			memset( &s_com_item, 0x00 , sizeof( struct com_item_c ));
			break;
		/**产品名称改为科目号。zhangpeng 20131126 end***/
		
		case 'P': /*每页小计*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt," %%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'X': /*产品小计*/
			sprintf(t_str,"%.2lf",sumval);
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
