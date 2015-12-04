/*************************************************************
* 文 件 名: rpt009.c
* 功能描述：通存通兑交易流水清单--他所代本所业务
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
#include "trace_log_bk_c.h"
#include "com_branch_c.h"
#include "tx_def_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_c.h"

struct dc_log_c s_trace_log_bk,dc_log;struct trace_log_bk_c tmp_trace_log;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
double cr_sum=0.0; /*贷方合计*/
double dr_sum=0.0; /*借方合计*/
long dr_cnt=0;
long cr_cnt=0;

int get_ratio_tdtdb(int bli,int bll,char str[100]);

rpt009()
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
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
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
	
	strcpy( filename , "RPT009");
	strcpy(vpfm, "RPT009");
	
	/* 条件:交易机构!=开户机构 */
	strcpy( tmpstr," tx_tx_br_no != tx_opn_br_no and acc_hrt!='70200' and ");
	strcat( wherelist,tmpstr );	
	/**组成查询**/
	
	strcat(wherelist,"  sts='0' and amt!=0  ORDER BY tx_opn_br_no , tx_tx_br_no , sts , trace_no , trace_cnt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Dc_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Dc_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
	vtcp_log("打印页尾1:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				dr_sum=0.00;
				cr_sum=0.00;
				dr_cnt=0;
				cr_cnt=0;
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack( s_trace_log_bk.tx_opn_br_no );
		if( strcmp( s_trace_log_bk.tx_opn_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
	vtcp_log("打印页尾2:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				dr_sum=0.00;
				cr_sum=0.00;
				dr_cnt=0;
				cr_cnt=0;
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_opn_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_opn_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_opn_br_no , filename );
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
	Dc_log_bk_Clo_Sel();

	/*  fclose(fp);  */
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成通存通兑流水清单(他代本)成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成通存通兑流水清单(他代本)失败!!!![%s]",g_pub_tx.reply);
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

/*打印合计金额*/
int print_sum()
{
	char vrec[3];
	
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
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
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; char ss_add_ind[3];
	int i=0; 
	struct com_branch_c sCom_branch;
	struct tx_def_c sTx_def;
	struct cif_id_code_rel_c sCif_id_code_rel;
	struct cif_basic_inf_c  sCif_basic_inf;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct com_item_c sCom_item;
	struct com_sys_parm_c sCom_sys_parm;
	int len=0;

	memset(&sCom_item,0x00,sizeof(struct com_item_c));
	memset(&sIn_mst,0x0,sizeof(struct in_mst_c));
	memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
	memset(&sTd_mst,0x0,sizeof(struct td_mst_c));
	memset(&sCif_id_code_rel,0x0,sizeof(struct cif_id_code_rel_c));
	memset(&sCif_basic_inf,0x0,sizeof(struct cif_basic_inf_c));
	memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&sTx_def,0x0,sizeof(struct tx_def_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	
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
			sprintf( t_str , "%4d年%2d月%2d日" , sCom_sys_parm.lst_date/10000,sCom_sys_parm.lst_date%10000/100 , sCom_sys_parm.lst_date%100 );			
			sprintf(fmt,"%%-%ds",bll);
 			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_opn_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_trace_log_bk.tx_opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_trace_log_bk.tx_opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_trace_log_bk.trace_no); 
			break; 
		case 'F': /* 操作员 */ 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tel);
			break;
		case 'I': /* 账      号 */
			sprintf(fmt, "%%-%ds", bll);
			memset(&tmp_trace_log,'\0',sizeof(tmp_trace_log));
			ret=Trace_log_bk_Sel(g_pub_tx.reply,&tmp_trace_log,"trace_no=%d and trace_cnt=%d",s_trace_log_bk.trace_no,s_trace_log_bk.trace_cnt);
			if(ret==0)
				sprintf( str, fmt, tmp_trace_log.ac_no);
			else
				sprintf( str, fmt, s_trace_log_bk.acc_hrt);
			break;
		case 'L':  /*户      名 */
			len=strlen(tmp_trace_log.ac_no);
			if((len==3) || (len==5) || (len==7))
			{
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",tmp_trace_log.ac_no);
				if(ret)
				{
					sprintf(acErrMsg,"查询公共科目表竟然出错啦!![%d]",ret);
					WRITEMSG
					goto ErrExit;	
				}	
				pub_base_strpack(sCom_item.acc_name);
				sprintf(t_str,"%s",sCom_item.acc_name);
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt, t_str);
				break;
			}
			if(tmp_trace_log.ac_no[0]=='9')
			{
				ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%d",tmp_trace_log.ac_id);
				if(ret && ret != 100)
				{
					sprintf(acErrMsg,"查询内部台帐主文件出错拉haha!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}	
			pub_base_strpack(sIn_mst.name); 
			sprintf( t_str , "%s" , sIn_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
			else
			{	
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",tmp_trace_log.ac_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"查询介质与帐户关系表出错!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",sMdm_ac_rel.id_type,sMdm_ac_rel.id_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"查询客户证件与客户号对照表出错!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%d",sCif_id_code_rel.cif_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"查询客户基本信息表出错 !![%d]",ret);
					WRITEMSG	
					goto ErrExit;
				}
			pub_base_strpack(sCif_basic_inf.name); 
			sprintf( t_str , "%s" , sCif_basic_inf.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
		case 'K': /* 交  易  额 */
			pub_base_dic_show_str(ss_add_ind,"trace_add_ind",s_trace_log_bk.dc_ind);

			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/*strcat( tmp_inf , amt_tmp);*/
			sprintf(fmt,"%%%ds",bll-5);
     			sprintf( str, fmt, amt_tmp );
			strcpy( str+strlen(str), " (");			
			sprintf(str+strlen(str),"%s",ss_add_ind);
			sprintf(str+strlen(str),")");
		if(s_trace_log_bk.dc_ind[0]=='1'){
			dr_sum+=s_trace_log_bk.amt;
				/****试验 20101210****/
				dr_cnt=dr_cnt;
				dr_cnt++;
		}
		else{
			cr_sum+=s_trace_log_bk.amt;
				/****试验 20101210****/
				cr_cnt=cr_cnt;
				cr_cnt++;
		}
			break;
		case 'J': /*借方合计金额*/
			sprintf(amt_tmp,"%.2lf",dr_sum);
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'G': /*借方合计笔数*/
			sprintf(amt_tmp,"%ld",dr_cnt);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'H': /*贷方合计金额*/
			sprintf(amt_tmp,"%.2lf",cr_sum);
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'R': /*贷方合计笔数*/
			sprintf(amt_tmp,"%ld",cr_cnt);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'M': /* 现转 */
			pub_base_dic_show_str(tmp_inf,"ct_ind",s_trace_log_bk.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'Q': /* 交易行 */
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str, fmt, s_trace_log_bk.tx_tx_br_no);
			break;
		case 'O': /* 状态 */
			pub_base_dic_show_str(tmp_inf,"rpt_log_sts",s_trace_log_bk.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, tmp_inf);
			break;
		case 'P': /* 交易日期 */
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt, s_trace_log_bk.tx_date);
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
