/***************************************************************************************
文件名: rptLC9.c
*作者：李文博
*时间：20150120
*功能：期理财产品分配收益后各个支行的收益金额
*****************************************************************************************/
#define ERR_DEAL if( ret ){\
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
#include "com_branch_c.h"
#include "fn_br_profit_c.h"
#include "fn_flt_radio_parm_c.h"


static struct com_branch_c sCom_branch;
static struct com_branch_c sCom_branch1;
static struct fn_reg_c sFn_reg;
static struct fn_parm_c sFn_parm;
static struct fn_br_profit_c sFn_br_profit;
static int page = 1;
static int line = 0;
static char vpfm[21];
static FILE *fp = NULL;
static int ret = 0;
static char t_str[41];
static int ttlnum = 0;
static double sum = 0.0;
static int line_file = 0;
static int g_count=0;
static int opncnt = 0;

static int iCount = 0;     
static double dAmt1 = 0.00;
static double dAmt2 = 0.00;
static double dAmt3 = 0.00;
static double dAmt4 = 0.00;

static int iCount1 = 0;     
static double dAmt5 = 0.00; 
static double dAmt6 = 0.00; 
static double dAmt7 = 0.00; 
static double dAmt8 = 0.00; 
static double dAmt9 = 0.00;
static double dAmt10 = 0.00;

int get_ratio_lcsyrb(int bli,int bll,char str[200]);

int rptLC9()
{
	char filename[50];
	char wherelist[1024];
	int ttlnum1 = 0;
	int i = 0;
	struct com_sys_parm_c sCom_sys_parm;
	
	memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sCom_branch1,0x00,sizeof(sCom_branch1));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(vpfm,0x00,sizeof(vpfm));
	memset(t_str,0x00,sizeof(t_str));

	ret = sql_begin();
	if(ret != 0 )
  {
    sprintf(acErrMsg,"打开事务失败！");
    WRITEMSG
    goto ErrExit;
  }

	pub_base_sysinit();
	strcpy(filename,"lc_sy_rb");
	strcpy(vpfm,"lc_sy_rb");
	
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	ERR_DEAL
	g_pub_tx.tx_date = sCom_sys_parm.lst_date;
	/**组成查询**/
	sprintf(wherelist," br_type='4' order by br_no ");
	vtcp_log( "WHERE [%s]",wherelist );

	/* 删除原文件 */
	 ret = pub_rpt_rmfile( "10001", filename , 0 );
	ERR_DEAL

 	ret = pub_rpt_openfile(&fp,"10001",filename);
	ERR_DEAL	
	ret = Fn_parm_Dec_Sel(g_pub_tx.reply,"stat ='8' and prdt_code in (select prdt_code from fn_br_profit where pay_date=%ld) ",g_pub_tx.tx_date);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"查询理财参数表错误!![%d]",ret);
    WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		iCount1 = 0;
		dAmt5 = 0.00;
		dAmt6 = 0.00;
		dAmt7 = 0.00;
		dAmt8 = 0.00;
		dAmt10 = 0.00;
		
		ret = Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
		if(ret== 100)
		{    
			if(!ttlnum1){
				sprintf(acErrMsg,"没有已分配收益的理财产品!![%d]",ret);
				WRITEMSG
				break;
			}
			else
			{	
			
				break;	
			}
		}		
		else if(ret)
		{
			sprintf(acErrMsg,"查询理财参数表错误!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*打印表首*/
		ret = print_head();
		ERR_DEAL
		
		
		line_file=0;
		page++;
		ttlnum1++;
		
		ret = Com_branch_Dec_Sel(g_pub_tx.reply,wherelist);
		ERR_DEAL
		
		while(1)
		{
			memset(&sCom_branch,0x00,sizeof(sCom_branch));
			ret = Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
			if(ret == 100)
			{
				ret = print_ZZ();
				ERR_DEAL
				ret = print_tail();
				ERR_DEAL
				break;		
			}
			else if(ret)
			{
				sprintf(acErrMsg,"读取公共机构表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}			
			
			ret = print_body();
			ERR_DEAL
			line_file++;
			if( line_file>42 )      /* 大于42行换页 */
			{
				line_file=0;
				page++;
			}	
		
			ttlnum++;
			g_count++;


		}	
		Com_branch_Clo_Sel();
	}
	Fn_parm_Clo_Sel();

OkExit:
        sql_commit();   /*--提交事务--*/
        fclose(fp);
	fp = NULL;
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"生成理财产品收益日报表成功!!!![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
	printf("\n共处理[%d]条记录.\n",g_count);

        return 0;
ErrExit:
        sql_rollback(); /*--事务回滚--*/
        if(fp != NULL)
    	{
        	fclose(fp);
        	fp=NULL;
   	}
        sprintf(acErrMsg,"生成理财产品收益日报表失败!!!![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;

}


/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{
        ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcsyrb);
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

       /* if( page != 1 )*/
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
int print_body()
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
int get_ratio_lcsyrb( bli,bll,str )
int bli,bll;
char str[200];
{
	 char fmt[10];
	 struct com_sys_parm_c sCom_sys_parm;
	 struct mdm_ac_rel_c sMdm_ac_rel;
	 struct dd_mst_c sDd_mst;
	 struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	 int ret1=0,ret2=0;
	 char str_mon[30];
	 double dTmp_amt1 = 0.00;
	 double dTmp_amt2 = 0.00;
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


 	 memset(str_mon,'\0',sizeof(str_mon));
	 memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	 memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	 memset(&sDd_mst, 0x0, sizeof(struct dd_mst_c));
   memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));

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
      sprintf( t_str , "%4d年%2d月%2d日" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000
/100 , sCom_sys_parm.lst_date%100 );
      sprintf(fmt,"%%-%ds",bll);
      sprintf( str , fmt , t_str );
      break;
			case 'B': /*产品编号*/
			    sprintf( t_str , "%5s    " , sFn_parm.prdt_code);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
			    break;
			case 'D': /* 页码 */
			    sprintf(fmt,"%%%dld",bll);
			    sprintf(str, fmt, page);
			    break;
			case 'M': /*产品名称*/
					sprintf( t_str , "%s" , sFn_parm.title);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
					 break;
			case 'E':/*募集开始日期*/
			    sprintf( t_str , "%d" , sFn_parm.beg_date);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
					 break;
			case 'F':/*募集结束日期*/
			    sprintf( t_str , "%d" , sFn_parm.end_date);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
			    break;
		case 'R':/*起息日期*/
    	sprintf( t_str , "%d" , sFn_parm.ic_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'T':/*到期日期*/
    	sprintf( t_str , "%d" , sFn_parm.mtr_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;

		case 'W':/*产品收益率*/
			/*	ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code='%s' ",sFn_parm.prdt_code);
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
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio24);
		  if(rate)
			sprintf( t_str , "%.2lf~%.2lf%%" , sFn_parm.rate,rate); 
			else*/
			sprintf( t_str , "%.2lf%%" , sFn_parm.fct_rate); 
			 sprintf(fmt, "%%-%ds", bll);
			 sprintf(str, fmt, t_str);
			break;
		case 'S':/*机构号*/
			 sprintf( t_str , "%s" , sCom_branch.br_no);
       sprintf(fmt, "%%-%ds", bll);
       sprintf(str, fmt, t_str);
       break;
		case 'Q':/*机构名称*/
       pub_base_strpack( sCom_branch.br_name );
			 sprintf( t_str , "%s" , sCom_branch.br_name);
       sprintf(fmt, "%%-%ds", bll);
       sprintf(str, fmt, t_str);
       break;
		case 'G':/*总笔数*/
			iCount = sql_count("fn_reg","tx_br_no = '%s' and prdt_code='%s' and sts='2'",sCom_branch.br_no,sFn_parm.prdt_code);
			iCount1 += iCount;
			sprintf( t_str , "%d" , iCount);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'H':/*募集金额*/
			ret = sql_sum_double("fn_br_profit","tx_amt",&dAmt1,"br_no = '%s' and prdt_code='%s' ",sCom_branch.br_no,sFn_parm.prdt_code);
			/*dAmt1 /= 10000;*/
			dAmt5 += dAmt1;
			sprintf( t_str , "%.2lf" , dAmt1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'J': /*返回本金金额*/ ret = sql_sum_double("fn_reg","return_capital",&dAmt2,"tx_br_no = '%s' and prdt_code='%s' and sts='2'",sCom_branch.br_no,sFn_parm.prdt_code);
		  /*dAmt2 /= 10000;*/
		  dAmt6 += dAmt2;
			sprintf( t_str , "%.2lf" , dAmt2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'K': /*返回收益金额*/
			ret = sql_sum_double("fn_reg","fct_intst",&dAmt3,"tx_br_no = '%s' and prdt_code='%s' and sts='2'",sCom_branch.br_no,sFn_parm.prdt_code);
			/*dAmt3 /= 10000;*/
			dAmt7 += dAmt3;
			sprintf( t_str , "%.2lf" , dAmt3);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'L': /*支行分润金额*/
			ret = sql_sum_double("fn_br_profit","profit_amt",&dAmt4,"br_no = '%s' and prdt_code='%s' ",sCom_branch.br_no,sFn_parm.prdt_code);	
			/*dAmt4 /= 10000;*/
			dAmt8 += dAmt4;
			sprintf( t_str , "%.2lf" , dAmt4);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'V' :/*汇总总笔数*/
			sprintf( t_str , "%d" , iCount1);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'X' : /*汇总募集金额*/
			sprintf( t_str , "%.2lf" , dAmt5);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Z': /*汇总返回本金总金额*/
			sprintf( t_str , "%.2lf" , dAmt6);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I': /*汇总收益总金额*/
			sprintf( t_str , "%.2lf" , dAmt7);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'O' : /*汇总支行收益总金额*/
			sprintf( t_str , "%.2lf" , dAmt8);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Y':/*兑付金额*/
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
		case 'C': /*汇总兑付金额*/
		 sprintf( t_str , "%.2lf" , dAmt10);
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
