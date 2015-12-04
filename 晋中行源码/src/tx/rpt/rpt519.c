/*************************************************************
* 文 件 名: rpt519.c
* 功能描述：打印|小额自由格式
*
* 作    者: dadary
* 完成日期: 2006年10月18日
*
* 注：格式文件为rpt519.tbl
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
* 注意：要是没有写表的BR_NO字段会死得很难看的 
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
#include "hv_fzf_c.h"

static int page,total;
static struct hv_fzf_c	hv_fzf;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;
static int hy_line = 0;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
char *cGetHvCmnoDes(char *cmtno,char *desc);
char *cGetHvzfStat(char stat,char *desc);
void cDeal_str(char *str,int lenth);
char * cDeal_content(char *buf,char ch,char *str);
void  myfun(char *source,char *target);
/***注意了:这里是打印一个个表单,每一页放4个表单*****/
rpt519()
{
	
	int line, opncnt;
	 hy_line = 0;    /**记录一个机构的查询的表单的个数**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp ; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/

	total			  = 0;/***某以机构的自由格式总笔数**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	vtcp_log("[%s][%d]开始打印|小额自由格式!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT519" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除|小额往帐业务清单错误!!");
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

	strcpy(vpfm, "rpt519");

	g_reply_int = Hv_fzf_Dec_Sel(g_pub_tx.reply, \
		"wt_date=%ld   and  cmtno ='303'  and otype ='2' ORDER BY br_no, cmtno,orderno,hv_fzf_sts  ",sComSysParm.lst_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_fzf  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Hv_fzf_Fet_Sel( &hv_fzf, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_fzf  CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100  )
		{
			if(iCycNum>0)
			{
				fclose(fp);
				break;
			}
			else
			{
				vtcp_log("[%s][%d]没有找到记录!\n",__FILE__,__LINE__);
				goto OkExit;	
			}
		}

		
		/*机构号变,下一个文件*/
		vtcp_log("[%s][%d] [%s]=====[%s] ",__FILE__,__LINE__,hv_fzf.br_no, pre_br_no );
		/**********如果接受行为空的话就直接给个 清算呀 这个问题找了我好久好久阿****/
		/***liuyue 20061116****/
		if(hv_fzf.br_no[0]=='')
			memcpy(hv_fzf.br_no,QS_BR_NO,5);
		/*********没有办法************/
		
		if( strcmp( hv_fzf.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**如果进入到这里来了,而且不是第一次进来,需要关闭前面的哪个文件**/
			{
				fclose(fp);
				vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
			}
			vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);	
			hy_line  = 0 ; /**如果机构号变化了,表单个数记录字段清0****/
			g_reply_int = pub_rpt_openfile( &fp, hv_fzf.br_no , fname );
			vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
                         vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
			total = sql_count( "hv_fzf", \
				"br_no='%s' and wt_date=%ld and otype='1'  and cmtno='303' ", hv_fzf.br_no,hv_fzf.wt_date );
			vtcp_log("xier[%s][%d] total==[%d]\n",__FILE__,__LINE__,total);
			strcpy( pre_br_no, hv_fzf.br_no );
		}

		hy_line ++;	/**表单的个数需要增加**/
		iCycNum ++; /**记录while循环的次数**/
		
		/*开始打印查询的表单******/
		vtcp_log("[%s][%d] begin \n",__FILE__,__LINE__);  
		if(fp!=NULL)
			vtcp_log("[%s][%d]文件错误了吗\n",__FILE__,__LINE__);
		else 
			vtcp_log("[%s][%d]文件错误了吗\n",__FILE__,__LINE__);   
		g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		vtcp_log("[%s][%d] end \n",__FILE__,__LINE__);
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/****到这里就打印完了表单上的数据了****/
		
	
		
		/** if(hy_line>0 &&hy_line%6 ==0)已经打印出了两个表,换页 del by martin 070519***/
		if(hy_line>0 &&hy_line%4 ==0)/**已经打印出了两个表,换页***/
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}

	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	Hv_fzf_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成|小额往账清单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成|小额未汇出往账清单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
		vtcp_log("[%s][%d]hehere\n",__FILE__,__LINE__);
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];
	char cTel[7];
	
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(cDate,	0x00,	sizeof(cDate));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	memset(str,	0x00,	sizeof(str));
	
	vtcp_log("[%s][%d]hehere\n",__FILE__,__LINE__);
	switch( bli ) 
	{
		case 'C':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_fzf.br_no );
			if( g_reply_int )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s（%s）", sComBranch.br_name, sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'A': 
			vtcp_log("[%s][%d]开始打印--序号!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,hy_line);
			break;
		case 'B': 
			vtcp_log("[%s][%d]开始打印--查询行号!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.or_br_no,bll);
			break;
		case 'D':
			vtcp_log("[%s][%d]开始打印--接受行号!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.ac_br_no,bll);
			 break;
		case 'E': 
			vtcp_log("[%s][%d]开始打印--年!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]开始打印--月!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]开始打印--日!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'H':
			vtcp_log("[%s][%d]打印委托日期!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, hv_fzf.wt_date ); 
			break;
		case 'I':
			vtcp_log("[%s][%d]开始打印--总笔数!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, total  ); 
			 break;
		case 'J': 
			vtcp_log("[%s][%d]开始打印--交易序号!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.orderno,bll);
			break;
		case 'O':
			vtcp_log("[%s][%d]开始打印--查询内容1!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
			cDeal_content(hv_fzf.req_content,'1',vstr);
			vtcp_log("funk!vstr==[%s]\n",vstr);
			memcpy(str,vstr,bll);
			 break;
		case 'P':
			vtcp_log("[%s][%d]开始打印--查询内容2!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
		        cDeal_content(hv_fzf.req_content,'2',vstr);
			memcpy(str,vstr,bll);
			 break;
		case 'Q':
			vtcp_log("[%s][%d]开始打印--查询内容3!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
		        cDeal_content(hv_fzf.req_content,'3',vstr);
		        vtcp_log("[%s][%d]查询内容的第三部分vstr==[%s]\n",__FILE__,__LINE__,vstr);
			memcpy(str,vstr,bll);
			vtcp_log("[%s][%d]查询内容的第三部分==[%s]\n",__FILE__,__LINE__,str);
			 break;
		case 'U':
			vtcp_log("[%s][%d]开始打印--数据来源!\n",__FILE__,__LINE__);
			if(hv_fzf.lw_ind[0]=='1')
				memcpy(str,"本行发起",bll);
			else if(hv_fzf.lw_ind[0]=='2')
				memcpy(str,"它行发起",bll);
			break;
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]hehere\n",__FILE__,__LINE__);
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}


