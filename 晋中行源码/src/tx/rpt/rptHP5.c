/*************************************************************
* 文 件 名: rptHP5.c
* 功能描述：汇票汇总
* 作    者: gujy
* 完成日期: 2007年08月18日
*
* 注：格式文件为rptHP5.tbl
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

#define	 LINE_CNT 3
static int page = 1,total;
static struct hv_poinfo_c	hv_poinfo, sum_hv_poinfo, tmp_hv_poinfo;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;
static	int	rownum = 0;
int		m_c_cnt=0,m_t_cnt=0,y_c_cnt=0,y_t_cnt=0;
double		m_c_amt=0,m_t_amt=0,y_c_amt=0,y_t_amt=0;
int		s_m_c_cnt=0,s_m_t_cnt=0,s_y_c_cnt=0,s_y_t_cnt=0;
double		s_m_c_amt=0,s_m_t_amt=0,s_y_c_amt=0,s_y_t_amt=0;
char pre_br_no[6];

void cDeal_str(char *str,int lenth);
int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rptHP5()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	FILE *fp; 
	int    iRecordNum = 0;/***符合条件的记录的条数,解决当记录数正好是整也时候会在另外一页上打印标题的问题**/
	int    iCycNum    = 0;/***循环走的次数**/
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
    
	strcpy( fname, "rptHP5" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除大额往帐业务清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1");
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
	

	strcpy(vpfm, "rptHP5");

	g_reply_int = Hv_poinfo_Dec_Sel(g_pub_tx.reply, \
		"substr(to_char(sign_date),1,4)=%ld and po_sts='2' ORDER BY br_no  ",sComSysParm.lst_date/10000 );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_zf  CUR error![%d]", g_reply_int ); 
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
			printf("[%s][%d] (%s) or_br_no (%s)fetch record finished!ret=[%d]\n",__FILE__,__LINE__,hv_poinfo.br_no, hv_poinfo.or_br_no, g_reply_int);
			if(iCycNum!=0)   /**只有在有记录的时候才会在下面的代码中打开文件的****/
			{
						vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
						memcpy(&hv_poinfo, &tmp_hv_poinfo, sizeof(hv_poinfo));
			printf("[%s][%d]  (%s)or_br_no (%s)fetch record finished!ret=[%d]\n",__FILE__,__LINE__,hv_poinfo.br_no, hv_poinfo.or_br_no, g_reply_int);
						g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
					m_c_cnt=s_m_c_cnt;m_t_cnt=s_m_t_cnt;y_c_cnt=s_y_c_cnt;y_t_cnt=s_y_t_cnt;
					m_c_amt=s_m_c_amt;m_t_amt=s_m_t_amt;y_c_amt=s_y_c_amt;y_t_amt=s_y_t_amt;
					g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_mbfesd );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					if(rownum)fclose(fp);
			}
			else if(iCycNum==0)
			{
				vtcp_log("[%s][%d]没有找到记录rownum!\n",__FILE__,__LINE__,rownum);
				if(rownum)fclose(fp);
				goto OkExit;
			}
			break;
		}

		/*memcpy(&tmp_hv_poinfo, &hv_poinfo, sizeof(hv_poinfo));

			printf("[%s][%d]  (%s)or_br_no (%s)fetch record finished!ret=[%d]\n",__FILE__,__LINE__,hv_poinfo.br_no, hv_poinfo.or_br_no, g_reply_int);

				if(hv_poinfo.sign_date-hv_poinfo.sign_date%100 == sComSysParm.lst_date-sComSysParm.lst_date%100){
						if('0' == hv_poinfo.deal_mode[0]){
								m_c_cnt+=1;
								s_m_c_cnt+=1;
								m_c_amt+=hv_poinfo.po_bal;
								s_m_c_amt+=hv_poinfo.po_bal;
						} else{
								m_t_cnt+=1;
								s_m_t_cnt+=1;
								m_t_amt+=hv_poinfo.po_bal;
								s_m_t_amt+=hv_poinfo.po_bal;
						}
				}
		if('0' == hv_poinfo.deal_mode[0]){
				y_c_cnt+=1;
				s_y_c_cnt+=1;
				y_c_amt+=hv_poinfo.po_bal;
				s_y_c_amt+=hv_poinfo.po_bal;
		} else{
				y_t_cnt+=1;
				s_y_t_cnt+=1;
				y_t_amt+=hv_poinfo.po_bal;
				s_y_t_amt+=hv_poinfo.po_bal;
		}

		iCycNum++;

		rownum++;*/
		/*机构号变,下一个文件*/
		if( strcmp( hv_poinfo.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( ! strlen( pre_br_no ) )
			{
			g_reply_int = pub_rpt_openfile( &fp, "10000" , fname );
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
			page = 1;
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			}else{
						vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
						g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						m_c_cnt=0;m_t_cnt=0;y_c_cnt=0;y_t_cnt=0;
						m_c_amt=0;m_t_amt=0;y_c_amt=0;y_t_amt=0;
			}
			strcpy( pre_br_no, hv_poinfo.br_no );
		}
		memcpy(&tmp_hv_poinfo, &hv_poinfo, sizeof(hv_poinfo));

			printf("[%s][%d]  (%s)or_br_no (%s)fetch record finished!ret=[%d]\n",__FILE__,__LINE__,hv_poinfo.br_no, hv_poinfo.or_br_no, g_reply_int);

				if(hv_poinfo.sign_date-hv_poinfo.sign_date%100 == sComSysParm.lst_date-sComSysParm.lst_date%100){
						if('0' == hv_poinfo.deal_mode[0]){
								m_c_cnt+=1;
								s_m_c_cnt+=1;
								m_c_amt+=hv_poinfo.po_bal;
								s_m_c_amt+=hv_poinfo.po_bal;
						} else{
								m_t_cnt+=1;
								s_m_t_cnt+=1;
								m_t_amt+=hv_poinfo.po_bal;
								s_m_t_amt+=hv_poinfo.po_bal;
						}
				}
		if('0' == hv_poinfo.deal_mode[0]){
				y_c_cnt+=1;
				s_y_c_cnt+=1;
				y_c_amt+=hv_poinfo.po_bal;
				s_y_c_amt+=hv_poinfo.po_bal;
		} else{
				y_t_cnt+=1;
				s_y_t_cnt+=1;
				y_t_amt+=hv_poinfo.po_bal;
				s_y_t_amt+=hv_poinfo.po_bal;
		}

		iCycNum++;
		rownum++;
			
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
			sprintf(vstr, "%s",  pre_br_no);
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
		case 'U':
			vtcp_log("[%s][%d]开始打印--交易类型!\n",__FILE__,__LINE__);
			 break;
		case 'R':
			vtcp_log("[%s][%d]开始打印--交易序号!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str, fmt, rownum+1);
			 break;
		case 'S':
			vtcp_log("[%s][%d]开始打印--付款帐号!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_ac_no,bll);
			
			break;
		case 'T':
			strncpy(str,hv_poinfo.op_tel,bll);
			vtcp_log("[%s][%d] str===[%s]\n",__FILE__,__LINE__,str);
			break;
/*int		m_c_cnt=0;m_t_cnt=0;y_c_cnt=0;y_t_cnt=0;
double		m_c_amt=0;m_t_amt=0;y_c_amt=0;y_t_amt=0;*/
		case 'Q': 
			sprintf( fmt, "%%%d.2ld", bll ); 
			sprintf( vstr, fmt, m_c_cnt );
			strcpy( str, vstr ); 
			break; 
		case 'I': 
			sprintf( fmt, "%%%d.2ld", bll ); 
			sprintf( vstr, fmt, m_t_cnt );
			strcpy( str, vstr ); 
			break; 
		case 'K': 
			sprintf( fmt, "%%%d.2ld", bll ); 
			sprintf( vstr, fmt, y_c_cnt );
			strcpy( str, vstr ); 
			break; 
		case 'N': 
			sprintf( fmt, "%%%d.2ld", bll ); 
			sprintf( vstr, fmt, y_t_cnt );
			strcpy( str, vstr ); 
			break; 
		case 'H': 
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, m_c_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'J': 
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, m_t_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'L': 
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, y_c_amt );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'P': 
			vtcp_log("[%s][%d]开始打印--多余金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, y_t_amt );/*用带逗号的金额*/
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
			if( 'G' == hv_poinfo.po_sts[0] ){ strncpy(str, "逾未用退", bll);break;}
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
			/*sprintf( str, "%04ld", page ); */
			sprintf(str, "%ld", hv_poinfo.sign_date);
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



