/*************************************************************
* 文 件 名: rpt090.c
* 功能描述：柜员业务量统计报表
*
* 作    者: dadary
* 完成日期: 2006年12月4日
*
* 注：格式文件为RPT090.tbl
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "trace_log_rz_c.h"
#include "com_tel_c.h"

static int page,total;
static struct com_sys_parm_c sComSysParm;
static struct trace_log_rz_c s_trace_log_rz;
static struct com_tel_c	     s_com_tel;


char    pre_tel[5];
long	pre_trace_no = 0;
long	pre_tx_date = 0;


int    i_tx_num = 0;/**某一个轨员交易的笔数**/


int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rpt090()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	
	
	char cWherelist[512];
	
	
	
	FILE *fp; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/
	
	int 	ret = 0;
	
	
	int	i_beg_day	=	0;/*月的第一天**/
	int	i_end_day	=	0;/*月的最后一天也就是综合业务的本天***/
	
	int	i_index_day	=	0;/*for循环的day****/
	
	
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	long   beg_date = 0;/**查询的开始日期**/
	long   end_date = 0;/**查询的结束日期**/



	memset(pre_tel,0x00,sizeof(pre_tel));

	memset(cWherelist,0x00,sizeof(cWherelist));


	


	vtcp_log("[%s][%d]开始打印柜员业务量!\n",__FILE__,__LINE__);
	
    	g_reply_int = sql_begin(); /*打开事务*/
    	if( g_reply_int != 0 )
    	{
    	    sprintf( acErrMsg, "BEGIN WORK error!!!" );
    	    WRITEMSG
    	    goto ErrExit;
    	}
    	
    	/**------- 初始化全局变量 --------**/
    	pub_base_sysinit();
    
	strcpy( fname, "RPT090" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除大额往帐业务清单错误!!");
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

	/*确定起始日期*****/
	beg_date = sComSysParm.lst_date/100 *100 +1;
	end_date = sComSysParm.lst_date;
	
	i_beg_day	= 1;
	i_end_day	=sComSysParm.lst_date%100;


	vtcp_log("[%s][%d]下面将打印出【%l]--[%l]时间段终轨员的业务量!\n",__FILE__,__LINE__,beg_date,end_date);



	strcpy(vpfm, "RPT090");
	
	
	/*ret = Com_tel_Dec_Sel(g_pub_tx.reply,"csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6') and wrk_sts!='*') order by br_no,tel"); lwb 20150525  增加分行  */
	ret = Com_tel_Dec_Sel(g_pub_tx.reply,"csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6','7') and wrk_sts!='*') order by br_no,tel");
	if(ret)
	{
		vtcp_log("[%s][%d]查询柜员号错误!\n",__FILE__,__LINE__);
		goto OkExit;
	}
	while(1)
	{
		ret = Com_tel_Fet_Sel(&s_com_tel,g_pub_tx.reply);
		
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]从com_tel表获取记录错误!ret====[%d]\n",__FILE__,__LINE__,ret);
			
			goto OkExit;
		}
		
		if(ret==100)/***如果找完了*****/
		{
			vtcp_log("[%s][%d]找完了所有的机构!\n",__FILE__,__LINE__);
			break;
		}
		
		
		i_tx_num = 0;	/**柜员变化，改变其柜员业务量数***/
		
		
		for(i_index_day=i_beg_day;i_index_day<=i_end_day;i_index_day++)
		{
			memset(cWherelist,0x00,sizeof(cWherelist));
			sprintf(cWherelist," ( select distinct(trace_no)  from trace_log_rz where tel='%s' and tx_date= %ld  )",s_com_tel.tel,sComSysParm.lst_date/100*100+i_index_day);
			i_tx_num = i_tx_num + sql_count(cWherelist,"1=1");
			
			vtcp_log("[%s][%d]柜员[%s] 【%d]日  业务量=[%d]笔\n",__FILE__,__LINE__,s_com_tel.tel,i_index_day,i_tx_num);
		}
		
		vtcp_log("[%s][%d]柜员[%s] 【%d]月  总业务量=[%d]笔\n",__FILE__,__LINE__,s_com_tel.tel,sComSysParm.lst_date%10000/100,i_tx_num);
	
		/*机构号变,下一个文件*/
		if( strcmp( s_com_tel.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, s_com_tel.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		
			hy_line = 0;
			page = 1;

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, s_com_tel.br_no );
		}
		
		
		
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "KK", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		i_tx_num = 0;
		
		/****要是柜员号变化了，插入一条新的记录***/
		/**
		if( strcmp(s_com_tel.tel,pre_tel) && strlen(pre_tel)!=0)
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "KK", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			i_tx_num = 0;
			
			memcpy(pre_tel,s_com_tel.tel,sizeof(s_com_tel.tel)-1);
		}
		****/
		
	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	
	Trace_log_rz_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成大额往账清单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成大额未汇出往账清单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str,rpt_code,mygsm )
int bli,bll;
char str[100];
int  rpt_code;
char mygsm[3];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];

	memset(cTx_Desc,0x00,sizeof(cTx_Desc));
	memset(vstr,	0x00,sizeof(vstr));
	memset(cDate,	0x00,sizeof(cDate));
	memset(fmt,	0x00,sizeof(fmt));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	
	
	vtcp_log("[%s][%d]mygsm===[%s],bli=[%c],bll=[%d]\n",__FILE__,__LINE__,mygsm,(char)bli,bll);
	
	switch( bli ) 
	{
		case 'B':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'",s_com_tel.br_no );
			if( g_reply_int )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s（%c%s）", sComBranch.br_name, ' ',sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'E': 
			vtcp_log("[%s][%d]开始打印--年!sComSysParm.lst_date=[%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]开始打印--月!sComSysParm.lst_date[%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]开始打印--日![%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break;
		case 'U':
			vtcp_log("[%s][%d]开始打印--柜员号!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str,fmt,s_com_tel.tel);
			 
			 break;
		case 'V':
			vtcp_log("[%s][%d]开始打印--柜员名字!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str,fmt,s_com_tel.name);
			 break;
		case 'W': 
			vtcp_log("[%s][%d]开始打印--业务量!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,i_tx_num);
			/*strcpy( str, cDate ); */
			break;
		case 'X': 
			vtcp_log("[%s][%d]开始打印--日期!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,sComSysParm.lst_date/100);
			/*strcpy( str, cDate ); */
			break;
		
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]501501501-------[%s][%d],mygsm===[%s]\n",__FILE__,__LINE__,str,bll,mygsm);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



 
