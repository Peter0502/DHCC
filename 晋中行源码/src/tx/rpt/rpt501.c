/*************************************************************
* 文 件 名: rpt501.c
* 功能描述：打印大额往帐清单
*
* 作    者: dadary
* 完成日期: 2006年10月16日
*
* 注：格式文件为rpt501.tbl
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
#include "hv_zf_c.h"

static int page,total;
static struct hv_zf_c	hv_zf;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;

void cDeal_str(char *str,int lenth);
int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rpt501()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


vtcp_log("[%s][%d]开始打印大额往帐清单!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT501" );
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
	

	strcpy(vpfm, "rpt501");

	g_reply_int = Hv_zf_Dec_Sel(g_pub_tx.reply, \
		"wt_date=%ld  and  lw_ind = '1' ORDER BY br_no, cmtno,orderno,hv_sts  ",sComSysParm.lst_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_zf  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Hv_zf_Fet_Sel( &hv_zf, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_zf  CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100  )
		{
			vtcp_log("[%s][%d] fetch record finished!ret=[%d]\n",__FILE__,__LINE__,g_reply_int);
			if(iCycNum!=0)   /**只有在有记录的时候才会在下面的代码中打开文件的****/
			{
					g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_mbfesd );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					fclose( fp );
			}
			else if(iCycNum==0)
			{
				vtcp_log("[%s][%d]没有找到记录!\n",__FILE__,__LINE__);
				goto OkExit;
			}
			break;
		}

		iCycNum ++;
		/*机构号变,下一个文件*/
		if( strcmp( hv_zf.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, hv_zf.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
                         
                         
			g_reply_int = sql_count( "hv_zf", \
				"br_no='%s' and wt_date=%ld and lw_ind='1' ", hv_zf.br_no,hv_zf.wt_date );

			/*** total = g_reply_int / 25 + 1;del by martin 070517****/
			total = g_reply_int / 21 + 1;
			hy_line = 0;
			page = 1;

			iRecordNum = g_reply_int;

			g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, hv_zf.br_no );
		}

		
		vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "DD", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		hy_line++;
			
		/***if( hy_line % 25 == 0  &&hy_line!=iRecordNum )del by martin 070517***/
		if( hy_line % 21 == 0  &&hy_line!=iRecordNum )
		{
			vtcp_log("[%s][%d]liuyue==[%d]iRecordNum=[%d]\n",__FILE__,__LINE__,hy_line,iRecordNum);
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			fprintf(fp,"\f");
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
	} 
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	
	Hv_zf_Clo_Sel();

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
		case 'C':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_zf.br_no );
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
		case 'L': 
			vtcp_log("[%s][%d]开始打印--日期!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,hv_zf.wt_date);
			/*strcpy( str, cDate ); */
			break;
		case 'U':
			vtcp_log("[%s][%d]开始打印--交易类型!\n",__FILE__,__LINE__);
		         cGetHvCmnoDes(hv_zf.cmtno,cTx_Desc);
		         cDeal_str(cTx_Desc, bll);
			 memcpy(str,cTx_Desc,bll);
			 break;
		case 'R':
			vtcp_log("[%s][%d]开始打印--交易序号!\n",__FILE__,__LINE__);
			memcpy(str,hv_zf.orderno,bll);
			 break;
		case 'S':
			vtcp_log("[%s][%d]开始打印--付款帐号!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_zf.pay_ac_no,bll);
			
			break;
		case 'T':
			vtcp_log("[%s][%d]开始打印--付款名称!\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]mygsm===[%s],bli=[%c],bll=[%d]\n",__FILE__,__LINE__,mygsm,(char)bli,bll);
			/***
			deal_half_charcter_xier(hv_zf.pay_name,vstr,sizeof(hv_zf.pay_name)-1);**/
			memcpy(vstr,hv_zf.pay_name,sizeof(hv_zf.pay_name)-1);
			vtcp_log("[%d][%d][%d][%d]\n",bll,bll,bll,bll);
			vtcp_log("[%s][%d]name===[%s]\n",__FILE__,__LINE__,hv_zf.pay_name);
			vtcp_log("[%s][%d]vstr===[%s] bll =[%d]\n",__FILE__,__LINE__,vstr,bll);
			deal_half_charcter(vstr);
			cDeal_str(vstr, (sizeof(hv_zf.pay_name)-1));
			if(!strncmp(mygsm,"DD",2)) /**注意了这里对于是奇数的长度处理不好***/
			{
				
				memcpy(str,vstr+bll,bll);
				vtcp_log("[%s][%d]vstr+bll==[%s]\n",__FILE__,__LINE__,vstr+bll);
				vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,str);
				
			}
			else
			{
				memcpy(str,vstr,bll);
				vtcp_log("[%s][%d]vstr+bll==[%s]\n",__FILE__,__LINE__,vstr);
			}
			vtcp_log("[%s][%d] str===[%s]\n",__FILE__,__LINE__,str);
			break;
		case 'Q': 
			vtcp_log("[%s][%d]开始打印--交易金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_zf.tx_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'V':
			vtcp_log("[%s][%d]开始打印--接受行号!\n",__FILE__,__LINE__);
			memcpy(str,hv_zf.ac_br_no,bll);
			break;
		case 'X':
			
			vtcp_log("[%s][%d]开始打印--收款帐号!\n",__FILE__,__LINE__);
			memcpy(str,hv_zf.cash_ac_no,bll);
			cDeal_str(str,bll);
			break;
		case 'W':
			vtcp_log("[%s][%d]cash_name=[%s]\n",__FILE__,__LINE__,hv_zf.cash_name);
			
			vtcp_log("[%s][%d]开始打印--收款人名字!\n",__FILE__,__LINE__);
			/*deal_half_charcter_xier(hv_zf.cash_name,vstr,sizeof(hv_zf.cash_name)-1);*/
			memcpy(vstr,hv_zf.cash_name,sizeof(hv_zf.cash_name)-1);
			deal_half_charcter(vstr);
			cDeal_str(vstr, (sizeof(hv_zf.cash_name)-1));
			if(!strncmp(mygsm,"DD",2)) /**注意了这里对于是奇数的长度处理不好***/
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			break;
		case 'M':
			
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetHvzfStat(hv_zf.hv_sts[0],cTx_Desc);
			cDeal_str(cTx_Desc, bll);
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
		case 'Y': 
			vtcp_log("[%s][%d]开始打印--接受行名字!\n",__FILE__,__LINE__);
			cGet_br_name(hv_zf.ac_br_no,vstr);
			deal_half_charcter(vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			/*
			deal_half_charcter_xier(vstr,vstr,sizeof(vstr));*/
			cDeal_str(vstr, bll*2);
			if(!strncmp(mygsm,"DD",2)) /**注意了这里对于是奇数的长度处理不好***/
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			break; 
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]501501501-------[%s][%d],mygsm===[%s]\n",__FILE__,__LINE__,str,bll,mygsm);
	
	
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



