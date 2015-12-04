/*************************************************************
* 文 件 名: rpt504.c
* 功能描述：打印大额汇票
*
* 作    者: dadary
* 完成日期: 2006年10月18日
*
* 注：格式文件为rpt504.tbl
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
#include "hv_poinfo_c.h"

static int page,total;
static struct hv_poinfo_c	hv_poinfo;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
/***注意了:这里是打印一个个表单,每一页放两个表单*****/
rpt504()
{
	int line, opncnt;
	int hy_line;    /**记录一个机构的查询的表单的个数**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	vtcp_log("[%s][%d]开始打印大额查询!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT504" );
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

	strcpy(vpfm, "rpt504");

	g_reply_int = Hv_poinfo_Dec_Sel(g_pub_tx.reply, \
   			"l_tx_date=%ld   ORDER BY br_no, po_no,po_sts ",sComSysParm.lst_date );

	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_fzf  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Hv_poinfo_Fet_Sel( &hv_poinfo, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_poinfo  CUR error![%d]", g_reply_int );
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
		if( strcmp( hv_poinfo.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**如果进入到这里来了,而且不是第一次进来,需要关闭前面的哪个文件**/
				fclose(fp);
				
			hy_line  = 0 ; /**如果机构号变化了,表单个数记录字段清0****/
			g_reply_int = pub_rpt_openfile( &fp, hv_poinfo.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( pre_br_no, hv_poinfo.br_no );
		}

		hy_line ++;	/**表单的个数需要增加**/
		iCycNum ++; /**记录while循环的次数**/
		
		/*开始打印查询的表单******/
		g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/****到这里就打印完了表单上的数据了****/
		
	
		
		if(hy_line>0 &&hy_line%3 ==0)/**已经打印出了两个表,换页***/
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fprintf(fp,"\f");
		}

	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	Hv_poinfo_Clo_Sel();

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

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];

	memset(cTx_Desc,0x00,sizeof(cTx_Desc));

	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{
		case 'C':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_poinfo.br_no );
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
		case 'L':
			vtcp_log("[%s][%d]开始打印--签发行号!\n",__FILE__,__LINE__);
			sprintf(str,"%8d",hv_poinfo.sign_date);
			 break;
		case 'U':
			vtcp_log("[%s][%d]开始打印--签发行号!\n",__FILE__,__LINE__);
		         memcpy(str,hv_poinfo.pay_br_no,bll);
			 break;
		case 'R':
			vtcp_log("[%s][%d]开始打印--汇票号码!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.po_no,bll);
			break;
	        case 'I':
			vtcp_log("[%s][%d]开始打印--汇票类型!\n",__FILE__,__LINE__);
			if(hv_poinfo.po_ind[0]=='0')
				memcpy(str,"可转让汇票",10);
			if(hv_poinfo.po_ind[0]=='1')
				memcpy(str,"不可转让汇票",10);
			if(hv_poinfo.po_ind[0]=='1')
				memcpy(str,"现金汇票",8);
			break;
		case 'S':
			vtcp_log("[%s][%d]开始打印--付款帐号!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_ac_no,bll);
			
			break;
		case 'T':
			vtcp_log("[%s][%d]开始打印--付款名称!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_name,bll);
			cDeal_str(str, bll);
			break;
		case 'Q': 
			vtcp_log("[%s][%d]开始打印--票面金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.po_bal );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'P': 
			vtcp_log("[%s][%d]开始打印--实际结算金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.cash_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'O': 
			vtcp_log("[%s][%d]开始打印--多余金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.rem_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'V':
			vtcp_log("[%s][%d]开始打印--接受行号!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_br_no,bll);
			break;
		case 'X':
			vtcp_log("[%s][%d]开始打印--收款帐号!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_ac_no,bll);
			cDeal_str(str,bll);
			break;
		case 'W':
			vtcp_log("[%s][%d]开始打印--收款人名字!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_name,bll);
			cDeal_str(str,bll);
			break;
		case 'M':
			vtcp_log("[%s][%d]开始打印--汇票状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetHpStat(hv_poinfo.po_sts[0],cTx_Desc);
			
			memcpy(str,cTx_Desc,bll);
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
		case 'D': 
			vtcp_log("[%s][%d]开始打印--页数!\n",__FILE__,__LINE__);
			sprintf( str, "%02ld", page ); 
			break; 
		case 'Z': 
			vtcp_log("[%s][%d]开始打印--总计页数!\n",__FILE__,__LINE__);
			sprintf( str, "%02ld", total ); 
			break; 
		case 'K': 
			vtcp_log("[%s][%d]开始打印--兑付行名!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.cash_br_no,vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			cDeal_str(vstr, bll);
			memcpy(str,vstr,bll);
			break; 
		case 'Y': 
			vtcp_log("[%s][%d]开始打印--签发行名!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.pay_br_no,vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			cDeal_str(vstr, bll);
			memcpy(str,vstr,bll);
			break; 
		case 'J':
			vtcp_log("[%s][%d]开始打印--数据来源!\n",__FILE__,__LINE__);
			if(hv_poinfo.bank_type[0]=='0')
				memcpy(str,"本行汇票",bll);
			else if(hv_poinfo.bank_type[0]=='1')
				memcpy(str,"他行汇票",bll);
			break;
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}


