/**************************************************************************************
*文件名: rptLC3.c
*作者：张杰
*时间：20141125
*功能：打印理财产品分配收益的明细报表
*****************************************************************************************/
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
#include "fn_reg_c.h"
#include "com_sys_parm_c.h"
#include "fn_parm_c.h"
#include "fn_flt_radio_parm_c.h"

static struct fn_reg_c sFn_reg;
static struct fn_parm_c sFn_parm;
static int page = 0;
static int line = 0;
static char vpfm[21];
static FILE *fp = NULL;
static int ret = 0;
static char t_str[41];
static int ttlnum = 0;
static double sum = 0.0;
static double sum2= 0.0;
static int line_file = 0;
static int g_count=0;
static int opncnt = 0;
static int i=0;

int get_ratio_lcmx(int bli,int bll,char str[200]);
int rptLC3()
{
	char filename[50];
	char wherelist[1024];
	char old_br_no[6];
	char old_prdt_code[15];
	struct com_sys_parm_c sCom_sys_parm;
	
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(old_prdt_code,0x00,sizeof(old_prdt_code));
	memset(old_br_no,0x00,sizeof(old_br_no));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(vpfm,0x00,sizeof(vpfm));
	memset(t_str,0x00,sizeof(t_str));
	strcpy( old_br_no , "*****" );
	strcpy( old_prdt_code , "*****" );
	
	ret = sql_begin();
	if(ret != 0 )
	{
		sprintf(acErrMsg,"打开事务失败！");
		WRITEMSG
		goto ErrExit;
	}
	
	pub_base_sysinit();
	strcpy(filename,"lc_sy_mx");
	strcpy(vpfm,"lc_sy");
	
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;
	/**组成查询**/
	sprintf(wherelist,"return_date=%ld and sts in ('2','6')  order by tx_br_no,prdt_code,pact_no,sts ",g_pub_tx.tx_date);
	
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	
	while(1)
	{
		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(ret == 100)
		{
			if(ttlnum)
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
				memset(&sum2,0x00,sizeof(double));
				vtcp_log("打印页尾1:[%s]--[%d]",__FILE__,__LINE__);
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);				
				
			}	
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的理财产品分配收益明细!" );
				WRITEMSG
			}	
			break;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"读取理财产品登记表出错!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		
			/* 机构变换时 */
		pub_base_strpack( sFn_reg.tx_br_no );
		if( strcmp( sFn_reg.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
				strcpy( old_prdt_code , "*****" );
			}
			
			strcpy( old_br_no , sFn_reg.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( sFn_reg.tx_br_no, filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , sFn_reg.tx_br_no, filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		/*换产品号时*/
		if(strcmp(sFn_reg.prdt_code,old_prdt_code))
		{
			/* 第一个产品号 */
			if( strcmp( old_prdt_code , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
			
				/*打印页首*/
				page++;
				ret = print_head();
				ERR_DEAL
			}
			
			strcpy( old_prdt_code, sFn_reg.prdt_code);
			
		}
		
    		/* 打印信息 */
    if(sFn_reg.sts[0] == '*')
	  {
		   sFn_reg.tx_amt = -sFn_reg.tx_amt;
	  }		    		
		sum+=sFn_reg.tx_amt;
		sum2+=sFn_reg.fct_intst;
		ret = print_body();
		ERR_DEAL
		line_file++;
		/*if( line_file>50 )	 大于50行换页 */
		if( line_file>42 )	/* 大于42行换页 */
		{
			line_file=0;
			page++;	
			/*memset(&sum,0x00,sizeof(double));	
			memset(&sum2,0x00,sizeof(double));*/
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
	Fn_reg_Clo_Sel();
	
	/*清算中心打印全行的*/
	g_count = 0;
	i=1;
	strcpy( old_br_no , "*****" );
	strcpy( old_prdt_code , "*****" );
	/* 删除原文件 */
	ret = pub_rpt_rmfile( "10001", filename , 0 );
	ERR_DEAL

 	ret = pub_rpt_openfile(&fp,"10001",filename);
	ERR_DEAL	
		
	sprintf(wherelist," sts in ('2','6') and return_date = %ld order by prdt_code,tx_br_no,tx_date,pact_no ",g_pub_tx.tx_date);
	
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	
	while(1)
	{
		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(ret == 100)
		{
			if(ttlnum)
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				vtcp_log("打印页尾1:[%s]--[%d]",__FILE__,__LINE__);
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
							
				
			}	
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的理财产品销售明细!" );
				WRITEMSG
			}	
			break;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"读取理财产品登记表出错!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		

		/*换产品号时*/
		if(strcmp(sFn_reg.prdt_code,old_prdt_code))
		{
			/* 第一个产品号 */
			if( strcmp( old_prdt_code , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
			

			}
							/*打印页首*/
			page = 1;
			line_file=1;
			ret = print_head();
			ERR_DEAL
	
			strcpy( old_prdt_code, sFn_reg.prdt_code);
			strcpy( old_br_no , sFn_reg.tx_br_no );
		}
		else
		{
					/* 机构变换时 */
		/*pub_base_strpack( sFn_reg.tx_br_no );
		if( strcmp( sFn_reg.tx_br_no , old_br_no ) )
		{
			/ 第一个机构 /
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* 打印页尾 /
				ret = print_tail();
				ERR_DEAL
			
				/*strcpy( old_prdt_code , "*****" );/
			}
			
			strcpy( old_br_no , sFn_reg.tx_br_no );
			
			page++;
			line_file=1;
			
		
			/* 打印页首 /
			ret = print_head();
			ERR_DEAL
		}*/
		}		
		sum+=sFn_reg.tx_amt;
		sum2+=sFn_reg.fct_intst;
		ret = print_body();
		ERR_DEAL
		
		line_file++;
		/*if( line_file>50 )	 大于50行换页 */
		if( line_file>42 )	/* 大于42行换页 */
		{
			line_file=0;
			page++;	
			/*memset(&sum,0x00,sizeof(double));	
			memset(&sum2,0x00,sizeof(double));	*/
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
	Fn_reg_Clo_Sel();


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成理财产品分配收益明细表成功!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
printf("\n共处理[%d]条记录.\n",g_count);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成理财分配收益明细表失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcmx);
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
	
/*	if( page != 1 )*/
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
int get_ratio_lcmx( bli,bll,str )
int bli,bll;
char str[200];
{
	char fmt[10]; 
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel1;
	struct dd_mst_c sDd_mst;
	struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	int ret1=0,ret2=0;
	char str_mon[30];
	char str_mon2[30];
	char cAc_id_list[1024];
	char cAc_id_list_tmp[1024];
	int cnt = 0;
	int iCount = 0;
	double rate = 0.00;
	  double n = 0.0;
  double rate1 = 0.0;
  double amt = 0.0;
  double dSum_amt1 = 0.0;
  double dSum_amt2 = 0.0;
  double dSum_amt3 = 0.0;
  double dSum_amt4 = 0.0;
  double dSum_amt5 = 0.0;
  double dSum_amt6 = 0.0;
	
	memset(cAc_id_list,0x00,sizeof(cAc_id_list));
	memset(cAc_id_list_tmp,0x00,sizeof(cAc_id_list_tmp));
  memset(str_mon,'\0',sizeof(str_mon));
  memset(str_mon2,'\0',sizeof(str_mon2));
  memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));

	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_ac_rel1, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst, 0x0, sizeof(struct dd_mst_c));

	
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
			sprintf( t_str , "%4d年%2d月%2d日" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000/100 , sCom_sys_parm.lst_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /*产品编号*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%5s    " , sFn_reg.prdt_code); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* 机构名称 */
	
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",sFn_reg.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			if(i==1)	
			strcpy(sCom_branch.br_name," 清算中心");
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s" , sCom_branch.br_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'E':/*募集开始日期*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%d" , sFn_parm.beg_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'F':/*募集结束日期*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%d" , sFn_parm.end_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;		
		case 'S':/*起息日期*/
    	sprintf( t_str , "%d" , sFn_reg.ic_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'T':/*到期日期*/
    	sprintf( t_str , "%d" , sFn_reg.end_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'Y':/*加权利率*/
			    /*add by lwb 20141204  增加一个算实时加权利率的部分*/
        ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code = '%s'",sFn_parm.prdt_code);
        if(ret &&ret != 100)
        {
        	vtcp_log("Fn_flt_radio_parm_Sel error! ret[%d] ",ret);
          strcpy( g_pub_tx.reply,"D324" );
          goto ErrExit;	
        }
        else if(ret ==0){
        
       	if(sFn_flt_radio_parm.buy_amt){
        sql_sum_double("fn_reg","tx_amt",&dSum_amt1,"prdt_code='%s' and tx_amt >=0 and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt);
        amt = sFn_flt_radio_parm.buy_amt;
        rate = sFn_flt_radio_parm.flt_radio2;
        }
        if(sFn_flt_radio_parm.buy_amt1){
        sql_sum_double("fn_reg","tx_amt",&dSum_amt2,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt,sFn_flt_radio_parm.buy_amt1);
         amt = sFn_flt_radio_parm.buy_amt1;
         rate = sFn_flt_radio_parm.flt_radio21;
        }
        if(sFn_flt_radio_parm.buy_amt2){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt3,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt1,sFn_flt_radio_parm.buy_amt2);
         amt = sFn_flt_radio_parm.buy_amt2;
         rate = sFn_flt_radio_parm.flt_radio22;
        }
         if(sFn_flt_radio_parm.buy_amt3){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt4,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt2,sFn_flt_radio_parm.buy_amt3);
         amt = sFn_flt_radio_parm.buy_amt3;
         rate = sFn_flt_radio_parm.flt_radio23;
         } 
         if(sFn_flt_radio_parm.buy_amt4){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt5,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt3,sFn_flt_radio_parm.buy_amt4);
         amt = sFn_flt_radio_parm.buy_amt4;
         rate = sFn_flt_radio_parm.flt_radio24;
         }
         sql_sum_double("fn_reg","tx_amt",&dSum_amt6,"prdt_code='%s' and tx_amt >=%lf  and sts!='*'",sFn_parm.prdt_code,amt);


         n =  ( sFn_flt_radio_parm.flt_radio2*dSum_amt2 +sFn_flt_radio_parm.flt_radio21*dSum_amt3 +sFn_flt_radio_parm.flt_radio22*dSum_amt4+sFn_flt_radio_parm.flt_radio23*dSum_amt5 +rate*dSum_amt6)/(dSum_amt1+dSum_amt2+dSum_amt3+dSum_amt4+dSum_amt5+dSum_amt6);
         rate1 = sFn_parm.fct_rate*(1+n);
         vtcp_log("n = %.6lf ",n);
         vtcp_log("rate1 = %.6lf ",rate1);
        }
        /*add end */
      sprintf( t_str , "%.2lf%%" , rate1); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
      break;
		case 'H':/*机构号*/
			sprintf( t_str , "%s" ,sFn_reg.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'J':/*序号*/
			sprintf( t_str , "%d" , line_file); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I':/*协议编号*/
			sprintf( t_str , "%s" , sFn_reg.pact_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'L':/*投资人姓名*/

			ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id='%d' ",sFn_reg.ac_id);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			vtcp_log(" [%s]--[%d]   name = %s",__FILE__,__LINE__,sDd_mst.name );
      pub_base_strpack( sDd_mst.name ); 
			sprintf( t_str , "%s" , sDd_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	
		case 'N':/*理财资金账号*/
			sprintf( t_str , "%s" , sFn_reg.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;	
		case 'U':/*购买金额*/
			sprintf( t_str , "%.2lf" , sFn_reg.tx_amt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;	
		case 'V':/*单位编码*/
			sprintf( t_str , "%s" , sFn_reg.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'P':/*购买次数*/
	
			cnt = sql_count("fn_reg","prdt_code = '%s' and tx_br_no='%s' and ac_no = '%s' and sts != '*'",sFn_reg.prdt_code,sFn_reg.tx_br_no,sFn_reg.ac_no);
			sprintf( t_str , "%d" , cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Q':/*收益日期*/
			sprintf( t_str , "%d" , sFn_reg.return_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Z':/*证件号码*/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id='%d' ",sFn_reg.ac_id);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%s" , sMdm_ac_rel.id_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'W':/*产品收益率*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			/*ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
				if(sFn_flt_radio_parm.flt_radio2)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio2);
			if(sFn_flt_radio_parm.flt_radio21)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio21);
			if(sFn_flt_radio_parm.flt_radio22)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio22);
			if(sFn_flt_radio_parm.flt_radio23)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio23);
			if(sFn_flt_radio_parm.flt_radio24)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio24);*/
			sprintf( t_str , "%.2lf%%" , sFn_parm.fct_rate); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'M': /*产品名称*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "查询理财产品参数表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			pub_base_strpack( sFn_parm.title ); 
			sprintf( t_str , "%s" , sFn_parm.title); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'O': /*每页小计*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'R': /*收益金额*/
			sprintf( t_str , "%.2lf" , sFn_reg.fct_intst); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'G': /*收益金额小计*/
			sprintf(t_str,"%.2lf",sum2);
			pub_rpt_flttomon(t_str,str_mon2);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon2);
			break;
		case 'K': /*认购渠道*/
			if(!strcmp(sFn_reg.chnl_type,"0100"))
				strcpy(t_str,"柜面");
			if(!strcmp(sFn_reg.chnl_type,"0400"))
				strcpy(t_str,"网银");
			if(!strcmp(sFn_reg.chnl_type,"0300"))
				strcpy(t_str,"手机银行");
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'X':/*状态*/	
			if(!strcmp(sFn_reg.sts,"2"))
				strcpy(t_str,"入账");
			if(!strcmp(sFn_reg.sts,"6"))
				strcpy(t_str,"未入账");
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

