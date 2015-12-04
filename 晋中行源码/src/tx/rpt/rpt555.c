/*************************************************************
* 文 件 名: rpt555.c
* 功能描述：支行资金头寸表
* 作    者: 
* 完成日期: 2006年12月05日
* 注：格式文件为rpt555.tbl
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define  EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "gl_sub_c.h"
#include "zjgl_mst_c.h"

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sCom_branch;
static struct tx_def_c sTxDef;

int    get_ratio_zhzjtc(int bli,int bll,char str[100]);
void   cDeal_str(char *str,int lenth);
double tmp_amt1=0.00,tmp_amt2=0.00,tmp_amt3=0.00,tmp_amt4=0.00;
double tmp_amt5=0.00,tmp_amt6=0.00,tmp_amt7=0.00,tmp_amt8=0.00;
double tmp_amt9=0.00,tmp_amt10=0.00,tmp_amt11=0.00,tmp_amt12=0.00;
double tmp_amt13=0.00,tmp_amt14=0.00; /**add zgf 20130926***/
double tmp_amt121=0.00,tmp_amt122=0.00,tmp_amt123=0.00;
char   t_str[41];
/** add by wudawei 20130927 **/
double d401_bal=0;
double d402_bal=0;
double d403_bal=0;
double d406_bal=0;
/** add end 20130927 **/
/***注意了:这里是打印一个个表单,每一页放两个表单*****/
rpt555()
{
	int  line, opncnt;
	int  hy_line;    /**记录一个机构的查询的表单的个数**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int  iCycNum    = 0;  /** 循环走的次数**/
	int ret=0,ret1=0,ret2=0,ret3=0,ret4=0,ret5=0,ret6=0;
  int ret7=0,ret8=0,ret9=0,ret10=0,ret11=0,ret12=0,ret121=0,ret122=0,ret123=0;
  int ret13=0,ret14=0;/**add zgf 20130926***/
	struct zjgl_mst_c tmp_zjgl;
        memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
          
    vtcp_log("[%s][%d]开始打印支行资金头寸!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }
   
    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT555" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除支行资金头寸表错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
	strcpy(vpfm, "rpt555");
		
        /*ret = Com_branch_Dec_Sel(g_pub_tx.reply,"1=1 and br_no not in('%s') and wrk_sts!='*'  order by br_no",KJ_BR_NO);mod by lwb 20150601*/
        ret = Com_branch_Dec_Sel(g_pub_tx.reply,"1=1 and br_no not in('%s') and wrk_sts!='*' and br_type!='7' order by br_no",KJ_BR_NO);
	if(ret)
	{
		vtcp_log("[%s][%d]查找com_branch错误!\n",__FILE__,__LINE__);
		sprintf(acErrMsg, "DECLARE COM_BRANCH ERROR !! [%d]", ret);
                WRITEMSG
                goto ErrExit;
	}
	while(1)
	{
		memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
                ret = Com_branch_Fet_Sel(&sCom_branch, g_pub_tx.reply);
                if (ret == 100)
                {
                	 fclose(fp);
                   break;
                }
                else if ( ret )
                {
                    sprintf(acErrMsg, "FETCH COM_BRANCH ERROR !! [%d]", ret);
                    WRITEMSG
                    goto ErrExit;
                }
          	pub_base_strpack(sCom_branch.br_no);
          /***zgf 20130926 原来注释掉**
					ret1 = sql_sum_double("gl_sub","cr_bal",&tmp_amt1,"acc_hrt in ('20100','20200','20500','21100','21500','21700','22100','22200','24300','25100') and br_no='%s' order by br_no",sCom_branch.br_no);
       		*/
       		/***mob zgf 20130926**/
        	ret1 = sql_sum_double("gl_sub","cr_bal",&tmp_amt1,"acc_hrt in ('20100','20200','20300','20500','21100','21500','21700','21800','22100','22200','24300','25100','22300','22500','22600','23400','23500','23600','25600') and br_no='%s' order by br_no",sCom_branch.br_no);
       		/***mob end zgf****/
       		if (ret1)
		{
			sprintf(acErrMsg, "统计查询存款错误!! [%d]", ret1);
			WRITEMSG
			goto ErrExit;
		}
		ret2=sql_sum_double("gl_sub","dr_bal",&tmp_amt2,"acc_hrt in ('12200','12300','12400','12500','12600','12700','12800','12900','13000','13600','14700','14800') and br_no='%s' order by br_no",sCom_branch.br_no);/*lpj20151124新增加136，147，148科目*/
       		if (ret2)
		{
			sprintf(acErrMsg, "统计查询贷款错误!! [%d]", ret2);
			WRITEMSG
			goto ErrExit;
		}
		 ret3 = sql_sum_double("gl_sub","dr_bal",&tmp_amt3,"acc_hrt in('11500','11400') and br_no='%s' order by br_no",sCom_branch.br_no);
       		if (ret3)
		{
			sprintf(acErrMsg, "统计查询存放同业错误!! [%d]", ret3);
			WRITEMSG
			goto ErrExit;
		}
		ret4 = sql_sum_double("zjgl_mst","dr_bal",&tmp_amt4,"acc_no in ('4070201') and opn_br_no='%s' order by opn_br_no",sCom_branch.br_no);
       		if (ret4)
		{
			sprintf(acErrMsg, "统计资金管理登记簿一级错误!! [%d]", ret4);
			WRITEMSG
			goto ErrExit;
		}
		ret5 = sql_sum_double("zjgl_mst","dr_bal",&tmp_amt5,"acc_no in ('4070202') and opn_br_no='%s' order by opn_br_no",sCom_branch.br_no);
       		if (ret5)
		{
			sprintf(acErrMsg, "统计资金管理登记簿二级错误!! [%d]", ret5);
			WRITEMSG
			goto ErrExit;
		}
		/*	4070203可能透支所以得用借贷方的差额
		ret6 = sql_sum_double("zjgl_mst","dr_bal",&tmp_amt6,"acc_no in ('4070203') and opn_br_no='%s' order by opn_br_no",sCom_branch.br_no);
		*/
		memset(&tmp_zjgl,'\0',sizeof(tmp_zjgl));
		ret6 = Zjgl_mst_Sel(g_pub_tx.reply ,&tmp_zjgl,"acc_no ='4070203' and opn_br_no='%s' and cur_no='01'",sCom_branch.br_no);
       		if (ret6)
		{
			sprintf(acErrMsg, "统计资金管理登记簿清算资金错误!! [%d]", ret6);
			WRITEMSG
			goto ErrExit;
		}
		tmp_amt6=tmp_zjgl.dr_bal-tmp_zjgl.cr_bal;

/**资金头寸上是从ass_mst上出的,只要是ASSCRZJ机构号对,sign='1'或ASSCCZJ机构号对,sign='2'就要选出来,故注掉原来从zjgl_mst上出表的这段
		ret7 = sql_sum_double("zjgl_mst","dr_bal",&tmp_amt7,"acc_no in ('4070204') and opn_br_no='%s' order by opn_br_no",sCom_branch.br_no);
       		if (ret7)
		{
			sprintf(acErrMsg, "统计资金管理登记簿拆借金额错误!! [%d]", ret7);
			WRITEMSG
			goto ErrExit;
		}
**/
		/**  拆解 和 上存 做判断 如果10000总行把机构号去掉  取全行汇总金额 wudawei 20150428  **/
		if(memcmp(sCom_branch.br_no,"10000",5)==0){
			ret7 = sql_sum_double("ass_mst","bal",&tmp_amt7,"sign='1' order by br_no" );
	  }else {
	  	ret7 = sql_sum_double("ass_mst","bal",&tmp_amt7,"sign='1' and br_no='%s' order by br_no",sCom_branch.br_no);
	  }
    if (ret7)
		{
			sprintf(acErrMsg, "统计资金管理登记簿拆借金额错误!! [%d]", ret7);
			WRITEMSG
			goto ErrExit;
		}
	/*
		ret8 = sql_sum_double("zjgl_mst","dr_bal",&tmp_amt8,"acc_no in ('4070206') and opn_br_no='%s' order by opn_br_no",sCom_branch.br_no);
	*/
		if(memcmp(sCom_branch.br_no,"10000",5)==0){
			ret8 = sql_sum_double("ass_mst","bal",&tmp_amt8,"sign='2' order by br_no" );
		}else {
			ret8 = sql_sum_double("ass_mst","bal",&tmp_amt8,"sign='2' and br_no='%s' order by br_no",sCom_branch.br_no);
		}
    if (ret8)
		{
			sprintf(acErrMsg, "统计资金管理登记簿上存资金错误!! [%d]", ret8);
			WRITEMSG
			goto ErrExit;
		}
				
		tmp_amt9=tmp_amt4+tmp_amt5+tmp_amt6+tmp_amt7+tmp_amt8;
		
		ret10 = sql_sum_double("gl_sub","cr_bal",&tmp_amt10,"acc_hrt in ('25100') and br_no='%s' order by br_no",sCom_branch.br_no);
       		if (ret10)
		{
			sprintf(acErrMsg, "统计资金管理登记簿上存资金错误!! [%d]", ret10);
			WRITEMSG
			goto ErrExit;
		}
		 ret11 = sql_sum_double("gl_sub","dr_bal",&tmp_amt11,"acc_hrt='10100' and br_no='%s' order by br_no",sCom_branch.br_no);
       		if (ret11)
		{
			sprintf(acErrMsg, "统计现金错误!! [%d]", ret11);
			WRITEMSG
			goto ErrExit;
		}
		
    /***add zgf 20130926 增加一般存款tmp_amt13  **/
  	ret13 = sql_sum_double("gl_sub","cr_bal",&tmp_amt13,"acc_hrt in ('20100','20200','20300','20500','21100','21500','21700','21800','22500','22600','23600','24300','25100','25600') and br_no='%s' order by br_no",sCom_branch.br_no);
 		if (ret13)
		{
			sprintf(acErrMsg, "统计查询存款错误!! [%d]", ret13);
			WRITEMSG
			goto ErrExit;
		}
		/** add 20130926 缴存准备金存款tmp_amt14**/
  	ret14 = sql_sum_double("gl_sub","cr_bal",&tmp_amt14,"acc_hrt in ('20100','20200','20300','20500','21100','21500','21700','21800','22500','22600','23600','25100','25600') and br_no='%s' order by br_no",sCom_branch.br_no);		
 		if (ret14)
		{
			sprintf(acErrMsg, "统计查询存款错误!! [%d]", ret14);
			WRITEMSG
			goto ErrExit;
		}
		
		/***add end zgf****/
		 /** 财务要求 401 403 406三科目要扎差金额 wudawei 20130927 **/
			/** 401 代理证券业务轧差后贷方余额 **/
	   ret = sql_sum_double( "gl_sub","cr_bal-dr_bal",&d401_bal,\
	   " br_no='%s' and acc_hrt='40100' ", sCom_branch.br_no);	   
			/** 402 代理证券业务轧差后贷方余额 **/
	   ret = sql_sum_double( "gl_sub","cr_bal-dr_bal",&d402_bal,\
	   " br_no='%s' and acc_hrt='40200' ", sCom_branch.br_no);	   
	      /** 403 其他代理业务轧差后贷方余额 **/
	   ret = sql_sum_double( "gl_sub","cr_bal-dr_bal",&d403_bal,\
	   " br_no='%s'and acc_hrt='40300' ", sCom_branch.br_no);	   
	      /** 406 委托业务轧差后贷方余额 **/
	   ret = sql_sum_double( "gl_sub","cr_bal-dr_bal",&d406_bal,\
	   " br_no='%s' and acc_hrt='40600' ", sCom_branch.br_no);   
	   tmp_amt14+=d401_bal+d402_bal+d403_bal+d406_bal;
	   /** wudawei 20130927 end **/
		   
		   	
    ret121=sql_sum_double("gl_sub","cr_bal",&tmp_amt121,"acc_hrt in('50100','50200','51100','51200','51300','51400','51500','52100','52200','52300','52700','52900','53000','53100','53500','53400','53500','53600','55000','56000') and br_no='%s' order by br_no",sCom_branch.br_no);
    if (ret121)
		{
			sprintf(acErrMsg, "统计查询簿错误!! [%d]", ret121);
			WRITEMSG
			goto ErrExit;
		}
		ret122=sql_sum_double("gl_sub","dr_bal",&tmp_amt122,"acc_hrt in('50100','50200','51100','51200','51300','51400','51500','52100','52200','52300','52700','52900','53000','53100','53500','53400','53500','53600','55000','56000') and br_no='%s' order by br_no",sCom_branch.br_no);
       		if (ret122)
		{
			sprintf(acErrMsg, "统计查询簿错误!! [%d]", ret122);
			WRITEMSG
			goto ErrExit;
		}
		ret123=sql_sum_double("gl_sub","dr_bal",&tmp_amt123,"acc_hrt='53300' and br_no='%s' order by br_no",sCom_branch.br_no);
       		if (ret123)
		{
			sprintf(acErrMsg, "统计查询簿错误!! [%d]", ret123);
			WRITEMSG
			goto ErrExit;
		}
		tmp_amt12=tmp_amt121-tmp_amt122-tmp_amt123;
	    		
		/*机构号变,下一个文件*/
		if( strcmp( sCom_branch.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**如果进入到这里来了,而且不是第一次进来,需要关闭前面的哪个文件**/
				fclose(fp);
				
			hy_line  = 0 ; /**如果机构号变化了,表单个数记录字段清0****/
			g_reply_int = pub_rpt_openfile( &fp, sCom_branch.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( pre_br_no, sCom_branch.br_no );
		}

		hy_line ++;	/**表单的个数需要增加**/
		iCycNum ++; /**记录while循环的次数**/
		
		/*开始打印查询的表单******/
		g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_zhzjtc);
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/****到这里就打印完了表单上的数据了****/
		
		/** if(hy_line>0 &&hy_line%3 ==0)已经打印出了两个表,换页del by martin 070519***/
		if(hy_line>0 &&hy_line%2 ==0)/**已经打印出了两个表,换页***/
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_zhzjtc);
			if( g_reply_int )
			{
			    sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
			    WRITEMSG
			    goto ErrExit;
			}
		}
	  }   
          Com_branch_Clo_Sel();
             

	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成支行资金头寸表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
        if (strcmp(g_pub_tx.reply,"0000") == 0)
           {
             strcpy(g_pub_tx.reply,"D157");
           }
	sprintf(acErrMsg,"生成支行资金头寸表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_zhzjtc( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char amt_tmp[41];	
	char br_name[71];
	int i=0; 
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(str,	0x00,	sizeof(str));
	
	switch( bli ) 
	{
		case 'A':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			pub_base_strpack( sCom_branch.br_name );
			sprintf(vstr, "%s（%s）", sCom_branch.br_name, sCom_branch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'B': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , sComSysParm.lst_date/10000 ,sComSysParm.lst_date%10000/100 , sComSysParm.lst_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str );
			break;
		case 'C': 
			vtcp_log("[%s][%d]开始打印--各项存款!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt1 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
/***add zgf 20130926 增加一般存款tmp_amt13  缴存准备金存款tmp_amt14**/	
		case 'X': 
			vtcp_log("[%s][%d]开始打印--一般存款!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt13 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'Y': 
			vtcp_log("[%s][%d]开始打印--缴存准备金存款!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt14 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
	
/***end zgf 20130926**/				
		case 'D': 
			vtcp_log("[%s][%d]开始打印--各项贷款!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt2 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		
		case 'E': 
			vtcp_log("[%s][%d]开始打印--存放同业!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt3 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'F': 
			vtcp_log("[%s][%d]开始打印--一级存款准备金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt4 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'G': 
			vtcp_log("[%s][%d]开始打印--二级存款准备金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt5 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'H': 
			vtcp_log("[%s][%d]开始打印--清算资金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt6 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'I': 
			vtcp_log("[%s][%d]开始打印--拆借资金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt7 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'J': 
			vtcp_log("[%s][%d]开始打印--上存资金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt8 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'K': 
			vtcp_log("[%s][%d]开始打印--存放总行合计!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt9 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;	
		case 'L': 
			vtcp_log("[%s][%d]开始打印--承兑保证金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt10 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'M': 
			vtcp_log("[%s][%d]开始打印--现金!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt11 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		case 'N': 
			vtcp_log("[%s][%d]开始打印--利润!\n",__FILE__,__LINE__);
			sprintf( amt_tmp, "%.2lf" , tmp_amt12 ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			memcpy(str,amt_tmp,bll);
			break;
		default : 
			memset( str,' ',bll ); 
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}

