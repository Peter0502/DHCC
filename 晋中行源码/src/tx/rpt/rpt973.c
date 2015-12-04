/*************************************************************
* 文 件 名: rpt973.c
* 功能描述：对公结息客户回单
*
* 作    者: rob
* 完成日期: 2003年09月16日
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
#include "intst_hst_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct intst_hst_c	s_intst_hst;
struct intst_hst_c	o_intst_hst;      /* add begin by Yanqq 20110620  分段计息报表打印*/
char cr_acc_hrt[8];
char dr_acc_hrt[8];

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt973()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
        /* add begin by Yanqq 20110620  分段计息报表打印*/
        int dMon=0;
        int dYear=0;
        int dDay=0;
        long dBeg_date=0;
        /*add end by Yanqq 20110620 */
	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];
	char tmp_str[60];
	char old_br_no[6];
	long intst_date=0;
	
	memset( wherelist,0,sizeof(wherelist) );
        memset(&o_intst_hst,0,sizeof(o_intst_hst));   /** add begin by Yanqq 20110620  分段计息报表打印*/
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
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
	strcpy( filename , "BJdgjxhd" );	
	strcpy(vpfm, "dgjxhd" );
*/	
	strcpy( filename , "RPT973");
	strcpy(vpfm, "RPT973");
        /* add begin by Yanqq 20110620  分段计息报表打印*/
        dMon=g_pub_tx.tx_date/100%100;
        dYear=g_pub_tx.tx_date/10000;
        dDay=g_pub_tx.tx_date%100;
        if(dMon%3>0)
        {
            vtcp_log("%s,%d,当前月份非结息月份不出结息清单!",__FILE__,__LINE__);
            goto OkExit;
        }
        if(dMon/3==1)
        {
            dBeg_date=(dYear-1)*10000+12*100+21;
        }else if(dMon/3==2)
        {
            dBeg_date=dYear*10000+3*100+21;
        }else if(dMon/3==3)
        {
            dBeg_date=dYear*10000+6*100+21;
        }else{
            dBeg_date=dYear*10000+9*100+21;
        }
        /** 对与分段计息的出不来 20110620
	ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='0' and pt_ind='1' and intst_date=%ld order by br_no,prdt_no,ac_id",g_pub_tx.tx_date ); **/
        ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='0' and pt_ind='1' and intst_date>%ld  and intst_date<=%ld order by br_no,prdt_no,ac_id,intst_date",dBeg_date,g_pub_tx.tx_date );
        /*add end by Yanqq 20110620 */
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
            memset(&s_intst_hst,0,sizeof(s_intst_hst));
		ret = Intst_hst_Fet_Sel( &s_intst_hst , g_pub_tx.reply );
		if( ret==100 ) 
		{
			fclose(fp);
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取结息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack(s_intst_hst.br_no );
		if( strcmp( s_intst_hst.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret = print_tail();
				fclose(fp);
			}
			strcpy( old_br_no , s_intst_hst.br_no );
			page=1;
			line_file=0;
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_intst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.br_no , filename );
			ERR_DEAL
		}
		/****获取贷方科目字***/
		memset(cr_acc_hrt,0x00,sizeof(cr_acc_hrt));
		vtcp_log("[%s][%d]开始获取贷方科目字!\n",__FILE__,__LINE__);
		cGet_cr_acc_hrt(s_intst_hst.prdt_no,"0152",cr_acc_hrt);
		vtcp_log("[%s][%d]获取贷方科目字=====[%s]!\n",__FILE__,__LINE__,cr_acc_hrt);
		/****获取借方科目字***/
		memset(dr_acc_hrt,0x00,sizeof(dr_acc_hrt));
		vtcp_log("[%s][%d]开始获取借方科目字!\n",__FILE__,__LINE__);
		cGet_cr_acc_hrt(s_intst_hst.prdt_no,"0153",dr_acc_hrt);
		vtcp_log("[%s][%d]获取借方科目字=====[%s]!\n",__FILE__,__LINE__,dr_acc_hrt);
    	/* 打印信息 */
                /* add begin by Yanqq 20110620 若是ac_id没有变化的借用上次的intst_date **/
                if(o_intst_hst.ac_id == s_intst_hst.ac_id)
                {
                    s_intst_hst.ic_date=o_intst_hst.intst_date;
                }
                memcpy(&o_intst_hst,&s_intst_hst,sizeof(o_intst_hst));
                memset(&g_dd_mst,0,sizeof(g_dd_mst));
                ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
                if( ret )
                {
                    sprintf(acErrMsg, "取户名错误! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
                    WRITEMSG
                    goto ErrExit;
                }
                /*add end by Yanqq 20110620 */
                /* add begin by Yanqq 20110620 判断若是销户计息则不出利息清单 **/
                if(g_dd_mst.ac_sts[0]=='*')
                {
                    continue;
                }
                /*add end by Yanqq 20110620 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>=2 )	/* 大于换页 */
		{
			fprintf(fp,"\f");
			page++;
			line_file = 0;
			
		}
		ttlnum++;
	}		
	Intst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成对公计息客户回单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成对公计息客户回单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
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

	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_cxxhqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int  season;/*季度**/
	int i=0; 
	long tmpdate=0;
	char tmpname[51];
	char tmpac_no[51];
	struct com_branch_c sCom_branch;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d年%2d月%2d日" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",s_intst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_intst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'E': /* 账     号 */
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=1 ",s_intst_hst.ac_id);
			if( ret==100 )
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=9999 ",s_intst_hst.ac_id);
			}
			if( ret )
			{
				sprintf(acErrMsg, "取账户错误! [%d][%d]", s_intst_hst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mdm_ac_rel.ac_no); 
			break; 
		case 'L': /*利率*/
			sprintf( amt_tmp, "%.5lf" , s_intst_hst.rate ); 	
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'M': /*户名*/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "取户名错误! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , g_dd_mst.name); 
			break; 
		case 'N': /*利息积数*/
			
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
				
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'J': /* 利息 */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /*第几季度*/
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			season = (int )tmpdate%10000/100 /4 +1;
			vtcp_log("[%s][%d]这是第%d季度!\n",__FILE__,__LINE__,season);
			sprintf(str,"%1d",season);
			break;
		case 'Q': /*对应的贷方会计科目号**/
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , cr_acc_hrt ); 
			break; 
		case 'R': /*对应的借方会计科目号 martin 2008/12/26 14:48:28**/
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , dr_acc_hrt ); 
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

/***根据产品编号 从dc_acc_rel 中获取贷方科目字**/
/***传入 产品编号 prdt_no,数据编码data_code,返回科目字acc_hrt***/
int cGet_cr_acc_hrt(char *c_prdt_no,char *data_code,char *acc_hrt)
{
	struct dd_parm_c 	s_dd_parm;
	struct dc_acc_rel_c 	s_dc_acc_rel;
	int ret = 0;
	vtcp_log("[%s][%d]产品编号=[%s],数据编码=[%s]\n",__FILE__,__LINE__,c_prdt_no,data_code);
	
	ret = Dd_parm_Sel(g_pub_tx.reply,&s_dd_parm,"prdt_no='%.4s' ",c_prdt_no);
	if(ret && ret!=100)
	{
		vtcp_log("[%s][%d]查找存款参数表数据库错误!ret=[%d]\n",__FILE__,__LINE__,ret);
		return (-1);
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]没有这个产品号!\n",__FILE__,__LINE__);
		return 1;
	}
	ret = Dc_acc_rel_Sel(g_pub_tx.reply,&s_dc_acc_rel,"dc_code='%.4s' and data_code='%.4s' ",s_dd_parm.dc_code,data_code);
		if(ret && ret!=100)
	{
		vtcp_log("[%s][%d]查找会计代码对应科目表数据库错误!ret=[%d]\n",__FILE__,__LINE__,ret);
		return (-1);
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]找不到对应的科目字!\n",__FILE__,__LINE__);
		return 1;
	}
	
	memcpy(acc_hrt,s_dc_acc_rel.acc_hrt,sizeof(s_dc_acc_rel.acc_hrt)-1);
	vtcp_log("[%s][%d]获取的科目字为[%s]\n",__FILE__,__LINE__,acc_hrt);
	
	return 0;
	
}
