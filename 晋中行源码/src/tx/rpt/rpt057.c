/*************************************************************
* 文 件 名: rpt057.c
* 功能描述：柜员揽存明细表
*
* 作    者: jane
* 完成日期: 2003年8月17日
*
* 注：格式文件为gylcmxb.tbl
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
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "prdt_ac_id_c.h"
#include "tel_save_inf_c.h"
#include "mo_jl_bal_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "td_mst_hst_c.h"
#include "dd_mst_hst_c.h"

long month;
double qmye,ypjs,yjs;
static int page,total;
static struct com_tel_c sComTel;
static struct tel_save_inf_c sTelSaveInf;
static struct com_sys_parm_c sComSysParm;
static struct dd_mst_c sDdMst;
static struct td_mst_c sTdMst;
static struct td_mst_hst_c sTdMstHst;
static struct dd_mst_hst_c sDdMstHst;
static struct prdt_ac_id_c sPrdtAcRel;
int get_ratio_gylcmxb(int bli,int bll,char str[100]);
rpt057()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int month_days;
	
	struct mdm_ac_rel_c sMdmAcRel;
	struct prdt_ac_id_c sPrdtAcId;
	struct mo_jl_bal_c sMoJlBal;
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
 
	strcpy( fname, "BMgylcmxb" );
	strcpy( fname, "RPT057");
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除柜员揽存明细表错误!!");
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

	month = sComSysParm.sys_date / 100 -1;
	if( month % 100 == 0 )
		month = month - 100 + 12;
	month_days = pub_base_MonDays( month );
	month = month * 100 ;
	vtcp_log(" wjwjw month [%ld] month_days [%d]", month, month_days );
	/*month 月 格式为:20031200*/

	strcpy(vpfm, "gylcmxb");
	strcpy(vpfm, "RPT057");

	/*g_reply_int = Com_tel_Dec_Sel(g_pub_tx.reply, "csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6') and wrk_sts!='*') ORDER BY br_no,tel" );lwb 20150525  增加分行     */
	g_reply_int = Com_tel_Dec_Sel(g_pub_tx.reply, "csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6','7') and wrk_sts!='*') ORDER BY br_no,tel" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE com_tel CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Com_tel_Fet_Sel( &sComTel, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH com_tel CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_gylcmxb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fclose( fp );
			break;
		}

		/*机构号变,下一个文件*/
		if( strcmp( sComTel.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_gylcmxb);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}
			page = 1;/*add by martin 20070428*/

			g_reply_int = pub_rpt_openfile( &fp, sComTel.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			
			if( page != 1 )/*add by martin 070428*/
		{
			fprintf ( fp , "\f" );
		}
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_gylcmxb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, sComTel.br_no );
		}

		/*打印该柜员揽存明细*/
		g_reply_int = Tel_save_inf_Dec_Sel(g_pub_tx.reply, "tel='%s' ORDER BY ac_no,ac_seqn", sComTel.tel );
		if( g_reply_int ) 
		{ 
			sprintf(acErrMsg, "DECLARE tel_save_inf CUR error![%d]", g_reply_int ); 
			WRITEMSG 
			goto ErrExit; 
		} 

		while(1)
		{
			memset( &sTelSaveInf, 0x00, sizeof( struct tel_save_inf_c ) );
			g_reply_int = Tel_save_inf_Fet_Sel( &sTelSaveInf, g_pub_tx.reply );
			if( g_reply_int == 100 )
				break;
			else if( g_reply_int )
			{
				sprintf( acErrMsg,"FETCH tel_save_inf CUR error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			/*帐户信息*/
			memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
			g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", sTelSaveInf.ac_no );
			if( g_reply_int && g_reply_int != 100 )
			{
				sprintf( acErrMsg,"SELECT mdm_ac_rel error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			/*帐户信息(定期2活期1)*/
			memset( &sPrdtAcRel, 0x00, sizeof( struct prdt_ac_id_c ) );
			g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &sPrdtAcRel, "ac_id=%ld", sMdmAcRel.ac_id );
			if( g_reply_int && g_reply_int != 100 )
			{
				sprintf( acErrMsg,"SELECT prdt_ac_id error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			qmye = 0.00;
			yjs = 0.00;
			ypjs = 0.00;
			vtcp_log("账户类型1为活期2为定期[%s]",sPrdtAcRel.ac_id_type);
			if(strcmp(sPrdtAcRel.ac_id_type,"1") == 0)
			{	
					/**根据账号查找dd_mst表**/
					memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
					g_reply_int = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id='%ld' ", sMdmAcRel.ac_id );
					if( g_reply_int == 100 )
							continue;
					else if( g_reply_int )
					{
						sprintf( acErrMsg,"SELECT dd_mst error![%ld][%d][%d]", sMdmAcRel.ac_id,sTelSaveInf.ac_seqn, g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					qmye = sDdMst.bal;				
					/*月积数**/
					g_reply_int = Dd_mst_hst_Dec_Sel( g_pub_tx.reply, "ac_id = %ld  and tx_date<%ld and tx_date>%ld order by hst_cnt" ,\ 
						sMdmAcRel.ac_id,  month + 100,month );
					if( g_reply_int !=0)
					{
						sprintf( acErrMsg,"DEC SELECT mo_jl_bal CUR error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					while(1)
					{
						memset( &sDdMstHst, 0x00, sizeof( struct dd_mst_hst_c ) );
						g_reply_int = Dd_mst_hst_Fet_Sel( &sDdMstHst, g_pub_tx.reply ); 
						if (g_reply_int == 100)
						{
							 break;
						}
						else if( g_reply_int !=0 )
						{
							sprintf( acErrMsg,"Fet Sel mo_jl_bal CUR error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						if( sDdMstHst.tx_date < month )
							sDdMstHst.tx_date = month + 1;
						if( sDdMstHst.tx_date > ( month + 100 ) )
							sDdMstHst.tx_date = month + month_days;
						yjs += sDdMstHst.bal * ( sDdMstHst.tx_date - sDdMstHst.tx_date + 1 );
					}					
					ypjs = yjs / month_days;
					/*
					if( qmye == 0.00 && ypjs == 0.00 && yjs == 0.00 )
						continue;
					*/		
					g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_gylcmxb );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					hy_line++;					
					if( hy_line % 25 == 0 )
					{
						g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_gylcmxb);
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						hy_line = 0;
						page++;						
						if( page != 1 )
						{
							fprintf ( fp , "\f" );
						}
						g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_gylcmxb);
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
					}					
					continue;	
					Dd_mst_hst_Clo_Sel();				
				}
				else if(strcmp(sPrdtAcRel.ac_id_type,"2") == 0)
				{			vtcp_log("ni ma de...");						/*2-定期*/
						/**根据账号查找td_mst表**/
						vtcp_log("bbbbbbbbbbbbbbbbbb");
						memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
						g_reply_int = Td_mst_Sel( g_pub_tx.reply, &sTdMst,"ac_id='%ld' ", sMdmAcRel.ac_id);
						if( g_reply_int == 100 )
							continue;
						else if( g_reply_int && g_reply_int != 100 )
						{
							sprintf( acErrMsg,"SELECT mo_jl_bal error![%ld][%d][%d]", sMdmAcRel.ac_id,sTelSaveInf.ac_seqn, g_reply_int );
							WRITEMSG
							goto ErrExit;
						}						
						qmye = sTdMst.bal;						
						/*月积数*/
						g_reply_int = Td_mst_hst_Dec_Sel( g_pub_tx.reply, "ac_id='%ld'  and tx_date<%ld and tx_date>%ld order by hst_cnt ", \ 
							sMdmAcRel.ac_id,  month + 100,month );
						if( g_reply_int !=0  )
						{
							sprintf( acErrMsg,"DEC SELECT mo_jl_bal CUR error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							memset( &sTdMstHst, 0x00, sizeof( struct td_mst_hst_c ) );
							g_reply_int = Td_mst_hst_Fet_Sel( &sTdMstHst, g_pub_tx.reply ); 
							if (g_reply_int == 100) break;
							else if( g_reply_int !=0)
							{
								sprintf( acErrMsg,"Fet Sel Td_mst_hst CUR error![%d]", g_reply_int );
								WRITEMSG
								goto ErrExit;
							}
							if( sTdMstHst.tx_date < month )
								sTdMstHst.tx_date = month + 1;
							if( sTdMstHst.tx_date > ( month + 100 ) )
								sTdMstHst.tx_date = month + month_days;
							vtcp_log("ni nainai...");
							vtcp_log("ni nainai...bal[%f]",sTdMstHst.bal);
							yjs += sTdMstHst.tx_amt * ( sTdMstHst.tx_date - sTdMstHst.tx_date + 1 );
							vtcp_log("ni daye...bal[%f]",yjs);
						}
						ypjs = yjs / month_days;
						/*
						if( qmye == 0.00 && ypjs == 0.00 && yjs == 0.00 )
							continue;
						*/		
						g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_gylcmxb );
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						hy_line++;
						
						if( hy_line % 25 == 0 )
						{
						g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_gylcmxb);
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
		
						hy_line = 0;
						page++;
						
						if( page != 1 )
						{
							fprintf ( fp , "\f" );
						}
						g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_gylcmxb);
						if( g_reply_int )
						{
							sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
							WRITEMSG
							goto ErrExit;
						}
						}						
						continue;
						Td_mst_hst_Clo_Sel();						
				}
				else
				{
					sprintf( acErrMsg,"账户类型错误....", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
			}
			continue;
	}		
	Com_tel_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成柜员揽存明细表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成柜员揽存明细表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_gylcmxb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	struct com_branch_c sComBranch;
	char amt_tmp[21];
	
	switch( bli ) 
	{ 
		case 'X': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, month/10000 ); 
			break; 
		case 'W': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, month/100%100 ); 
			break; 
		case 'U': 
			sprintf( str, "%02ld", page ); 
			break; 
		case 'A': 
			sprintf( vstr, "%s", sComTel.tel );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'B': 
			vtcp_log("11111111111111==[%s]",sTelSaveInf.ac_no);
			sprintf( vstr, "%s", sTelSaveInf.ac_no );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'C': 
			sprintf( vstr, "%4d", sTelSaveInf.ac_seqn );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'D': 
			sprintf( amt_tmp, "%.2lf" , qmye ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'E': 
			sprintf( amt_tmp, "%.2lf" , ypjs ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'F': 
			sprintf( amt_tmp, "%.2lf" , yjs ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'T': 
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", sComTel.br_no );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}
