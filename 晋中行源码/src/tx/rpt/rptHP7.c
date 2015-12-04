/*************************************************************
* 文 件 名: rptHP7.c
* 功能描述：银行汇票申请兑付，且未兑付清算
* 作    者: gujy
* 完成日期: 2007年08月18日
*
* 注：格式文件为rptHP7.tbl
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

#define	 LINE_CNT 20
static int page = 1,total;
static struct hv_poinfo_c	hv_poinfo;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;
static	int	rownum = 0;
static	double	sum_amt = 0.0;

void cDeal_str(char *str,int lenth);
int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rptHP7()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/
	int    zz_ind     = 1;/***控制重复打印结尾次数**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "rptHP7" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除大额往帐业务清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
	

	strcpy(vpfm, "rptHP7");

	g_reply_int = Hv_poinfo_Dec_Sel(g_pub_tx.reply, \
		"l_tx_date=%ld  and po_sts='3' ORDER BY br_no  ",sComSysParm.lst_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE hv_poinfo  CUR error![%d]", g_reply_int ); 
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
			vtcp_log("[%s][%d] fetch record finished!ret=[%d]\n",__FILE__,__LINE__,g_reply_int);
			if(iCycNum!=0)   /**只有在有记录的时候才会在下面的代码中打开文件的****/
			{
					g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_mbfesd );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
			}
			else if(iCycNum==0)
			{
				vtcp_log("[%s][%d]没有找到记录rownum!\n",__FILE__,__LINE__,rownum);
				if(rownum)fclose(fp);
				goto OkExit;
			}
			fclose(fp);
			break;
		}
		iCycNum++;

		/*机构号变,下一个文件*/
		if( strcmp( hv_poinfo.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				if(zz_ind!=0)
					{
					  g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_mbfesd );
					  if( g_reply_int )
					  {
					  	sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					  	WRITEMSG
					  	goto ErrExit;
					  }
					}
				fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, hv_poinfo.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			 fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
                         
                         
			rownum = 0;
			sum_amt=0.0;
			page = 1;
			zz_ind++;
			if(rownum) fprintf(fp, "\f");
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		iCycNum	= 1;/*原值为0，会出现第一条记录没有结尾  martin 20081021*/

			strcpy( pre_br_no, hv_poinfo.br_no );
		}else{
			if(0 == rownum %LINE_CNT ){
							if(rownum) fprintf(fp, "\f");
							zz_ind++;
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
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/*******************delete by xyy 2007-7-28 12:16*******************/
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "DD", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		hy_line++;
			
						rownum++;
						sum_amt+=hv_poinfo.po_bal;

		if(  (rownum+1) % LINE_CNT == 0 )
		{
			vtcp_log("[%s][%d]liuyue==[%d]iRecordNum=[%d]\n",__FILE__,__LINE__,hy_line,iRecordNum);
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","ZZ",&opncnt,get_ratio_mbfesd);
			/*fprintf(fp,"\f");*/zz_ind=0;
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			/*g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}*/
			iCycNum	= 0;
		}
	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	
	Hv_zf_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"%d@%s  生成大额往账清单成功![%s]",__LINE__, __FILE__, g_pub_tx.reply);
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
		case 'A':
			sprintf(vstr, "%s",  hv_poinfo.br_no);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
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
			sprintf(vstr, "%s（%c%s）", sComBranch.br_name, ' ',sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'L': 
			vtcp_log("[%s][%d]开始打印--日期!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,hv_poinfo.l_tx_date);
			/*strcpy( str, cDate ); */
			break;
		case 'U':
			vtcp_log("[%s][%d]开始打印--交易类型!\n",__FILE__,__LINE__);
			 break;
		case 'R':
			vtcp_log("[%s][%d]开始打印--交易序号!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str, fmt, rownum+1);
			 break;
		case 'I':
			sprintf( fmt, "%%%dd", bll );
			sprintf(str, fmt, rownum);
			 break;
			break;
		case 'S':
			vtcp_log("[%s][%d]开始打印--付款帐号!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_ac_no,bll);
			
			break;
		case 'T':
			strncpy(str,hv_poinfo.op_tel,bll);
			vtcp_log("[%s][%d] str===[%s]\n",__FILE__,__LINE__,str);
			break;
		case 'Q': 
			vtcp_log("[%s][%d]开始打印--交易金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.po_bal );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'H': 
			vtcp_log("[%s][%d]开始打印--交易金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, sum_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'V':
			vtcp_log("[%s][%d]开始汇票种类!\n",__FILE__,__LINE__);
			/*memcpy(str,hv_poinfo.ac_br_no,bll);*/
			                                                          
			if( '0' == hv_poinfo.po_ind[0] ){ strncpy(str, "可转让汇票", bll);break;}
			if( '1' == hv_poinfo.po_ind[0] ){ strncpy(str, "不可转让汇票", bll);break;}
			if( '2' == hv_poinfo.po_ind[0] ){ strncpy(str, "现金汇票", bll);break;}
			break;
		case 'M':
		/*	po_sts        varchar2(1)   汇票当前状态标志 0-登记 1-签发 9-签发取消 2-汇票移存                                                    
    3-申请兑付 4-汇票兑付 5-全额结清 6-部分退回 7-未用退回                                                                          
    8-汇票解付 A-解付取消 B-发送 C-申请兑付取消 D-汇票登记取消                                                                      
    E-挂失 F-解挂失 G-逾期未用退回 H-未用解 I-未用冲帐冲帐             	*/
			if( '0' == hv_poinfo.po_sts[0] ){ strncpy(str, "登记", bll);break;}
			if( '1' == hv_poinfo.po_sts[0] ){ strncpy(str, "签发", bll);break;}
			if( '9' == hv_poinfo.po_sts[0] ){ strncpy(str, "签发取消", bll);break;}
			if( '2' == hv_poinfo.po_sts[0] ){ strncpy(str, "汇票移存", bll);break;}
			if( '3' == hv_poinfo.po_sts[0] ){ strncpy(str, "申请兑付", bll);break;}
			if( '4' == hv_poinfo.po_sts[0] ){ strncpy(str, "汇票兑付", bll);break;}
			if( '5' == hv_poinfo.po_sts[0] ){ strncpy(str, "全额清算", bll);break;}
			if( '6' == hv_poinfo.po_sts[0] ){ strncpy(str, "部分退回", bll);break;}
			if( '7' == hv_poinfo.po_sts[0] ){ strncpy(str, "未用退回", bll);break;}
			if( '8' == hv_poinfo.po_sts[0] ){ strncpy(str, "汇票解付", bll);break;}
			if( 'A' == hv_poinfo.po_sts[0] ){ strncpy(str, "解付取消", bll);break;}
			if( 'B' == hv_poinfo.po_sts[0] ){ strncpy(str, "发送", bll);break;}
			if( 'C' == hv_poinfo.po_sts[0] ){ strncpy(str, "申兑取消", bll);break;}
			if( 'D' == hv_poinfo.po_sts[0] ){ strncpy(str, "登记取消", bll);break;}
			if( 'E' == hv_poinfo.po_sts[0] ){ strncpy(str, "挂失", bll);break;}
			if( 'F' == hv_poinfo.po_sts[0] ){ strncpy(str, "解挂失", bll);break;}
			if( 'G' == hv_poinfo.po_sts[0] ){ strncpy(str, "逾期未用退回", bll);break;}
			if( 'H' == hv_poinfo.po_sts[0] ){ strncpy(str, "未用解付", bll);break;}
			if( 'I' == hv_poinfo.po_sts[0] ){ strncpy(str, "未用冲账", bll);break;}
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			/*memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetHvzfStat(hv_poinfo.hv_sts[0],cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);*/
			/*memcpy(str,hv_poinfo.cash_ac_no,bll);
			cDeal_str(str,bll);*/
			break;
		case 'W':
			vtcp_log("[%s][%d]cash_name=[%s]\n",__FILE__,__LINE__,hv_poinfo.cash_name);
			
			vtcp_log("[%s][%d]开始打印--收款人名字!\n",__FILE__,__LINE__);
			/*deal_half_charcter_xier(hv_poinfo.cash_name,vstr,sizeof(hv_poinfo.cash_name)-1);*/
			memcpy(vstr,hv_poinfo.cash_name,sizeof(hv_poinfo.cash_name)-1);
			deal_half_charcter(vstr);
			cDeal_str(vstr, (sizeof(hv_poinfo.cash_name)-1));
			/**注意了这里对于是奇数的长度处理不好***delete by xyy 2007-7-28 12:14
			if(!strncmp(mygsm,"DD",2)) 
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			**************/
			memcpy(str,vstr,bll);
			break;
		case 'X':
			strncpy(str, hv_poinfo.po_no, bll);	
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
			sprintf( str, "%04ld", page ); 
			break; 
		case 'Z': 
			vtcp_log("[%s][%d]开始打印--总计页数!\n",__FILE__,__LINE__);
			sprintf( str, "%04ld", page ); 
			break; 
			/**注意了这里对于是奇数的长度处理不好***delete by xyy 2007-7-28 12:15
		case 'Y': 
			vtcp_log("[%s][%d]开始打印--接受行名字!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.ac_br_no,vstr);
			deal_half_charcter(vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			*
			deal_half_charcter_xier(vstr,vstr,sizeof(vstr));*
			cDeal_str(vstr, bll*2);
			
			if(!strncmp(mygsm,"DD",2)) 
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			
			memcpy(str,vstr,bll);
			break; 
			********************/
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]501501501-------[%s][%d],mygsm===[%s]\n",__FILE__,__LINE__,str,bll,mygsm);
	
	
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



