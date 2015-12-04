/*************************************************
* 文 件 名: spR002.c
* 功能描述：瑞阳公司导入批量代扣入帐
*
*  作    者: 吴海帆
* 完成日期: 2015年07月31日
* 修改日期:
* 修 改 人:
* 修改内容: 
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#define cIdtp "1"
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ag_paytlcs_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "com_branch_c.h"
#include "rsp_code_c.h"
#include "dd_mst_c.h"
#define PAGE_CNT 50

gD805()
{
	int ret = 0,tmp_trace_cnt=0;
	int ret1 = 0;
	char cReturn[51];
	char execstr[248];
	char str_temp[2048];
	int iProc_flag = 0;	/*	1－增加对应关系;2－修改对应关系		*/
	int iErr_flag = 0;	/*	0－处理正确；1－处理失败	*/
	
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	
	long lSnum = 0;		/*成功笔数*/
	long lFnum = 0;		/*失败笔数*/
	double dSamt = 0.00;	/*成功金额*/
	double dFamt = 0.00;	/*失败金额*/
	
	int iK_flag = 0;	/*新开户标志*/
	long lK_num = 0;	/*新开户数*/
	double dK_amt = 0.00;	/*新开金额*/
	
	double ys_amt = 0.00;/* 燃气代收 应收金额*/

	char filename0[248];	/*新增燃气公司项目*/
	char filename[248];
	char filename1[248];
	char filename3[248];
	FILE *fp0 = NULL;
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	FILE *fp3 = NULL;
	
	char filename00[248];	/*新增燃气公司报表*/
	char filename01[248];
	char filename11[248];
	char filename31[248];
	FILE *fp00 = NULL;
	FILE *fp01 = NULL;
	FILE *fp11 = NULL;
	FILE *fp31 = NULL;
	double sumtram=0.0;
	int i=0,tot_cnt=0,err_tot_cnt=0;
		int lSsum1=0;
	long lTrace_no = 0;
	char RTrace_no[20];
	
	char tmpstr[512];

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_paytlcs_c sAg_paytlcs;
	struct dd_mst_c sDd_mst;
	struct in_mst_c sIn_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct com_branch_c sCom_branch;
	struct rsp_code_c sRsp_code;

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

	sql_begin();
	
	ret = Ag_unitinfo_Dec_Sel( g_pub_tx.reply , "unit_stat = '0' \
		and  opr_attri = '0' and beg_date <= %ld and end_date >= %ld", \
		g_pub_tx.tx_date, g_pub_tx.tx_date, g_pub_tx.tx_date );
	if ( ret )
	{
		sprintf(acErrMsg,"执行Ag_unitinfo_Dec_Sel出错[%d]", ret );
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
    		goto ErrExit;
	}
		
	while(1)
	{
		memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
		lSnum = 0;	/*成功笔数*/
		lFnum = 0;	/*失败笔数*/
		dSamt = 0.00;	/*成功金额*/
		dFamt = 0.00;	/*失败金额*/
    		sumtram=0.00;/**/
		ret = Ag_unitinfo_Fet_Sel( &sAg_unitinfo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			/*
			sql_begin();
			*/
			break;
		}
		vtcp_log("aaaaaaaaaaaaa[%s]\n",sAg_unitinfo.bank_acct_no);
		if ( ret )
		{
			sprintf(acErrMsg,"执行Ag_unitinfo_Fet_Sel出错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sAg_unitinfo.pay_type );
	
		if ( strcmp( sAg_unitinfo.pay_type, "35" ) == 0 )
		{
			continue;	/*------不处理现金代发户------*/
		}
		
		pub_base_strpack( sAg_unitinfo.unit_no );
		pub_base_strpack( sAg_unitinfo.unit_name );
		pub_base_strpack( sAg_unitinfo.opn_br_no );
		pub_base_strpack( sAg_unitinfo.bank_acct_no );	
		
		/*--------------  检查单位账号状态  ---------------*/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sAg_unitinfo.bank_acct_no );
		if ( ret )
		{
			sprintf(acErrMsg,"查询介质账号对照表失败");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			/*sql_rollback();*/
			continue;
		}		
		if(sAg_unitinfo.bank_acct_no[0]=='9')
		{
			memset(&sIn_mst,0x00,sizeof(sIn_mst));
	  		ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn= 0",sMdm_ac_rel.ac_id);
	  		if(ret)
	  		{
	  			sprintf(acErrMsg,"查询内部主文件失败!");
					WRITEMSG
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	  		}
	  		if(sMdm_ac_rel.note_sts[0]!='0' || sIn_mst.sts[0]!='1')
	  		{
	  			sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	  		}
	  		if(sAg_unitinfo.opr_attri[0]=='1')
	  		{
	  			ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);
	 			if(sIn_mst.bal<sumtram)
	 			{
	 				sprintf(acErrMsg,"账户余额不正常[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	 			}
	 		}
	  	}
	  	else
	  	{
			memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
			ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
				and ac_seqn = 1", sMdm_ac_rel.ac_id );		
			if ( ret )
			{
				sprintf( acErrMsg,"查询活期主文件失败[%d][%ld][%s]", ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
				WRITEMSG
				set_zd_data(DC_REPLY,g_pub_tx.reply);
				/*sql_rollback();*/
				continue;
			}
			
			if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
				|| sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0]=='2' )	/*销户或全冻*/
			{
				sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
				WRITEMSG
				/*sql_rollback();*/
				continue;	/*跳过开始下一个单位*/
			}
			if(sAg_unitinfo.opr_attri[0]=='1')
	  		{	
				ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);
	 			if(sDd_mst.bal-sDd_mst.hold_amt<sumtram)
	 			{
	 				sprintf(acErrMsg,"账户余额不正常[%s]", sAg_unitinfo.bank_acct_no);
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	 			}
	 		 }			
		 }
		/*------------ 打开文件完成处理结果文件 begin -----------*/
			
		memset(filename0,0x0,sizeof(filename0));
		sprintf(filename0,"%s/txt/REC01.%ld", getenv("HOME"), g_pub_tx.tx_date );
		
		fp0 = fopen( filename0,"w" );	
		if( fp0==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename0 );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 1;
		} 
		 	
		/*****     end     *****/
		
		/*该文件用来记录明细处理正确结果*/
		memset(filename,0x0,sizeof(filename));
		sprintf(filename,"%s/report/%s/BR0%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		fp = fopen( filename,"w" );	
		if( fp==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}
		/*******add by ligl 2006-11-23 17:11********/
		if(sAg_unitinfo.opr_attri[0]=='0')
	  	{	
	  		fprintf( fp, "                                 [%s][%s]批量代理业务成功记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━┯━━━━┑\n" );
    			fprintf( fp, "│%20s│%20s│%20s│%16s│%16s│%8s│\n", "代理编号","客户帐号", "客户姓名", "金额", "余额", "处理结果" );
    			fprintf( fp, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━┿━━━━┥\n" );
	  	
		}
	  	else
	  	{	
			fprintf( fp, "                                 [%s][%s]批量代理业务成功记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━┑\n" );
    			fprintf( fp, "│%20s│%20s│%16s│%20s│%20s│%8s│\n", "代理编号", "客户姓名", "金额", "证件号码", "客户账号", "处理结果" );
    			fprintf( fp, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━┥\n" );
    		} 
    		
    		/*该文件用来记录明细错误处理结果*/   	
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/report/%s/BR1%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		fp1 = fopen( filename1, "w" );
		if( fp1==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename1 );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			fclose( fp );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}
		/*******add by ligl 2006-11-23 17:11********/
 		if(sAg_unitinfo.opr_attri[0]=='0')
	  	{	
			fprintf( fp1, "                                [%s][%s]批量代理业务失败记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp1, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━┯━━━━┑\n" );
    			fprintf( fp1, "│%20s│%20s│%20s│%16s│%16s│%8s│\n", "代理编号","客户帐号", "客户姓名", "金    额", "余    额", "处理结果" );
    			fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━┿━━━━┥\n" );
		}
	  	else
	  	{       	
			fprintf( fp1, "                                [%s][%s]批量代理业务失败记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp1, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━━━━━━┑\n" );
    			fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%30s│\n", "代理编号", "客户姓名",   "金  额", "证件号码", "客户帐号", "处理结果" );
    			fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━━━━━━┥\n" );
    		}    	
        	lTrace_no = 0;
        	pub_base_PubQlsh_Trans( &lTrace_no );
    		
		ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
			and sata = '1' and result = '1' and tx_date=%ld", sAg_unitinfo.unit_no,g_pub_tx.tx_date );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Pay_agent_Dec_Sel出错[%d]", ret );
			WRITEMSG
			fclose( fp );
			fclose( fp1 );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}		

		tot_cnt=0; err_tot_cnt=0;
		while(1)
		{
			iErr_flag = 0;
			iProc_flag = 0;			
			
			memset( &sPay_agent, 0x0, sizeof(sPay_agent) );
			ret = Pay_agent_Fet_Sel( &sPay_agent , g_pub_tx.reply );
			if ( ret == 100 )
			{
				if ( sAg_unitinfo.opr_attri[0] == '0' && sAg_unitinfo.end_date == g_pub_tx.tx_date)	/*代收业务*/
				{
					ret = sql_execute( "insert into pay_agent_bk select * from \
						pay_agent where unit_no = '%s'", \
						sAg_unitinfo.unit_no );
					lSsum1= sql_count("(select distinct unit_no,item_no,ac_no from  pay_agent)"," unit_no='%s'",sAg_unitinfo.unit_no);		
					vtcp_log("[%s][%d]====人数====[%ld]\n",__FILE__,__LINE__,lSsum1);
	
					ret = sql_execute( "delete from pay_agent where unit_no = '%s' and result='0'", \
						sAg_unitinfo.unit_no );
				}
				else
				{
					sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
					WRITEMSG
					goto ErrExit;	
				}
				
				break;
			}
			if ( ret )
			{
				sprintf(acErrMsg,"执行Pay_agent_Fet_Sel出错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/**----------- 每次循环都重新初始化全局变量 -------------**/
			memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
			pub_base_sysinit();	
			
			g_pub_tx.trace_no = lTrace_no;	/*重新赋值为生成的流水号*/
			set_zd_long( "0040", lTrace_no );
      			/*g_pub_tx.trace_cnt=atoi(sPay_agent.cnt); */
			tmp_trace_cnt++;
      			g_pub_tx.trace_cnt=tmp_trace_cnt;
			memset( cReturn, 0x0, sizeof(cReturn) );
			memset( xs_acno,0x0,sizeof(xs_acno) );
			
			/*---交易机构为代理单位建立机构---*/
			strcpy( g_pub_tx.tx_br_no, sAg_unitinfo.opn_br_no );
			set_zd_data( DC_TX_BR_NO, sAg_unitinfo.opn_br_no );
			
			memset( tmpstr, 0x0, sizeof(tmpstr) );
			get_zd_data( "0030", tmpstr );
			sprintf(acErrMsg,"批量处理交易机构为[%s][%s]", g_pub_tx.tx_br_no, tmpstr );
			WRITEMSG
  			if(sAg_unitinfo.opr_attri[0] == '0')
  			{
				ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and ac_no = '%s'",sPay_agent.unit_no, sPay_agent.ac_no);
				if ( ret == 0 )
				{
					pub_base_strpack_all( sAg_peritemmap.name );
					pub_base_strpack_all( sPay_agent.name );
					if ( strcmp( sAg_peritemmap.name, sPay_agent.name) )
					{
						sprintf( cReturn, "户名不符,原户名为[%s]", sAg_peritemmap.name );
						sprintf( acErrMsg, "户名不符,原户名为[%s]", sAg_peritemmap.name );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					/*----------------------------------------------------*/
					vtcp_log("aaaaaaaaaaaaa[%s]\n",sAg_unitinfo.opr_attri);
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s' and note_sts <> '2'", sPay_agent.ac_no );
                        	        /*
                        	          2009/2/13, added by lianghaoran, "and note_sts <> '2'"
                        	        */
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]信息错", sPay_agent.ac_no );
						sprintf( acErrMsg, "查找账号[%s]信息错", sPay_agent.ac_no );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
						
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					
					pub_base_strpack( sDd_mst.ac_sts );
					if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1'||sMdm_ac_rel.note_sts[0]!='0' )	/*销户和完全冻结*/
					{
						strcpy( cReturn, "销户,完全冻结和挂失" );
						sprintf(acErrMsg,"销户和完全冻结[%s][%c][%c][%c]", sAg_peritemmap.item_no,sDd_mst.ac_sts[0] ,sDd_mst.hold_sts[0] ,sMdm_ac_rel.note_sts[0]);
						WRITEMSG
						iK_flag = 1;
						iErr_flag = 1;
						goto TrLog;
					}					
					sumtram=0.00;
					ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and ac_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no,sAg_peritemmap.ac_no);
	 				if(sDd_mst.bal<sumtram)
	 				{
						strcpy( cReturn, "余额不够扣" );
	 					sprintf(acErrMsg,"账户余额不正常[%s]", sAg_peritemmap.ac_no);
						WRITEMSG
						strcpy( g_pub_tx.reply, "H059" );
						set_zd_data(DC_REPLY,g_pub_tx.reply);
						iErr_flag = 1;
						goto TrLog;
	 				}	
				}
				else if ( ret == 100 )
				{
					strcpy( cReturn, "代理户不存在" );
					sprintf(acErrMsg, "查询代发记录出错[%s][%s]", sPay_agent.unit_no, sPay_agent.ac_no);
					WRITEMSG
					iErr_flag = 1;
					goto TrLog;
				}
				else if(ret)
				{
					strcpy( cReturn, "代理户有问题" );
					sprintf( acErrMsg,"查询代扣记录出错[%s][%s]", sPay_agent.unit_no, sPay_agent.ac_no );
					WRITEMSG
					goto ErrExit;
				}
			}
 			else
 			{	
				sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
				WRITEMSG
				goto ErrExit;	
			}		
			sprintf(acErrMsg,"---------------------开始活期存取款了-------------------");
			WRITEMSG

			strcpy( g_pub_tx.ac_no, sAg_peritemmap.ac_no );
			g_pub_tx.ac_seqn = 1;
		
			g_pub_tx.tx_amt1 = sPay_agent.tram;
		
			if ( sAg_unitinfo.opr_attri[0] == '1' )
			{
				sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
				WRITEMSG
				goto ErrExit;	
			}
			else
			{
				strcpy(g_pub_tx.add_ind,"0");	/*-- 代缴 --*/
				
				memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
				strcpy( g_pub_tx.sub_tx_code, "D003" );
				set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
				
				strcpy( g_pub_tx.prt_ind, "0" );
				set_zd_data( "102O", "0" );
				
				strcpy( g_pub_tx.prt_ind, "0" );
				set_zd_data( "102O", "0" );
				
				set_zd_data( "102J", "01" );
				set_zd_data( "102K", "2" );
				set_zd_double( "102F", g_pub_tx.tx_amt1 );
				set_zd_double( "102P", 0.00 );
				set_zd_double( "102Q", 0.00 );
				strcpy(g_pub_tx.brf, "代收");
			}
			/** strcpy(g_pub_tx.brf, sPay_agent.remark); **/
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.ac_wrk_ind,"001001001");
			strcpy(g_pub_tx.hst_ind,"1");
			g_pub_tx.svc_ind=1001;
			strcpy( g_pub_tx.tx_code, "G008" );
			sprintf(acErrMsg,"BEG D099[%s][%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
			WRITEMSG
			
			if ( sAg_unitinfo.opr_attri[0] == '0' )	/*代收业务建立*/
			{
				/*
				sql_commit();
				sql_begin();
				*/
			}

			ret = pub_acct_trance();
			if( ret != 0 )
			{
				sprintf( cReturn, "%s", g_pub_tx.reply );
				sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
			if ( ret )
			{
				sprintf( cReturn, "%s", g_pub_tx.reply );
				sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			strcpy( xs_acno, sAg_peritemmap.ac_no );
			
			goto NOKAIHU;
        	
NOKAIHU:		
			/*--- 维护代收付号码与单位对照表ag_peritemmap ---*/
			if ( iProc_flag == 1 )		/*--- 增加对应关系 ---*/
			{
				memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
			
				strcpy( sAg_peritemmap.bat_flag, "1" );
				strcpy( sAg_peritemmap.remark, sAg_unitinfo.pay_type );
			
				strcpy( sAg_peritemmap.unit_no, sPay_agent.unit_no );
				strcpy( sAg_peritemmap.item_no, sPay_agent.item_no );
				strcpy( sAg_peritemmap.name, sPay_agent.name );
				strcpy( sAg_peritemmap.id_type, cIdtp );
				strcpy( sAg_peritemmap.id_no, sPay_agent.id_no );
				strcpy( sAg_peritemmap.ac_no, xs_acno );
				strcpy( sPay_agent.ac_no, xs_acno );
				sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
				strcpy( sAg_peritemmap.stat, "0" );
				sAg_peritemmap.opn_date = g_pub_tx.tx_date;
				strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
				sAg_peritemmap.cls_date = 99999999;
				strcpy( sAg_peritemmap.cls_tel, "0000" );
		
				ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
				if ( ret )
				{
					sprintf(acErrMsg,"写记录入代收号码与单位对应表失败");
					WRITEMSG
					strcpy( cReturn, "写代收号码与单位对应表失败");
					iErr_flag = 1;
					goto TrLog;
				}
			}
			if ( iProc_flag == 2 )		/*--- 修改对应关系 ---*/
			{
				strcpy( sPay_agent.ac_no, xs_acno );
				
				ret = sql_execute( "update ag_peritemmap set name = '%s', \
					id_no = '%s' , ac_no = '%s', opn_date = %ld, opn_tel= '%s'\
					where unit_no = '%s' and item_no = '%s'", \
					sPay_agent.name, sPay_agent.id_no, xs_acno, \
					g_pub_tx.tx_date,g_pub_tx.tel, \
					sPay_agent.unit_no, sPay_agent.item_no );
				if ( ret )
				{
					sprintf(acErrMsg,"修改ag_peritemmap失败[%s][%d]!!!",sPay_agent.item_no,ret );
					WRITEMSG
					strcpy( cReturn, "维护代收号码与单位对应表失败");
					iErr_flag = 1;
					goto TrLog;
				}
			}
        	
			/*---------------联机批处理写日志---------------*/
			memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );
		
			strcpy( sAg_paytlcs.unit_no, sAg_unitinfo.unit_no );
			sprintf( sAg_paytlcs.bank_tlcs, "%ld", g_pub_tx.trace_no );
			
			strcpy( sAg_paytlcs.batch_no, "" );		/*对帐批次*/
			strcpy( sAg_paytlcs.unit_tlcs, "" );	/*代理流水*/
			strcpy( sAg_paytlcs.pre_no, "" );		/*电子账号*/
			strcpy( sAg_paytlcs.bank_acct_no, sAg_peritemmap.ac_no ); /*个人账号 */
			sprintf( sAg_paytlcs.trn_date, "%ld", g_pub_tx.tx_date );
			sprintf( sAg_paytlcs.trn_time, "%ld", g_pub_tx.tx_time );
			strcpy( sAg_paytlcs.br_no, g_pub_tx.tx_br_no );
			strcpy( sAg_paytlcs.tel_no, g_pub_tx.tel );
			sAg_paytlcs.trn_amt = sPay_agent.tram;
			strcpy( sAg_paytlcs.item_no, sPay_agent.item_no );
			strncpy( sAg_paytlcs.pay_month, sAg_paytlcs.trn_date, 6 );
			strcpy( sAg_paytlcs.pay_way, "1" );		/*- 日终批量 -*/
			strcpy( sAg_paytlcs.bill_flag, "0" );
			strcpy( sAg_paytlcs.stat, "0" );
		
			ret = Ag_paytlcs_Ins( sAg_paytlcs, g_pub_tx.reply );
			if (ret )
			{
				strcpy( cReturn, "写收付费历史表出错");
				sprintf(acErrMsg,"插入ag_paytlcs出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		
			ret = sql_execute( "update pay_agent set sata = '0', \
				result = '0', result_name = '%s' \
				where unit_no = '%s' and item_no = '%s'" ,\
				cReturn, sPay_agent.unit_no,sPay_agent.item_no );
			if ( ret )
			{
				strcpy( cReturn, "更改未处理记录状态错" );
				sprintf(acErrMsg,"更改pay_agent出错[%d][%s][%s]", \
					sPay_agent.unit_no, sPay_agent.item_no, ret);
				WRITEMSG
				goto ErrExit;
			}

TrLog:
			pub_base_strpack( sPay_agent.name );
			if ( iErr_flag == 0)
			{
				lSnum = lSnum + 1;
				dSamt = dSamt + sPay_agent.tram;
				
				if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"45",2)==0)
	  			{	
	  				/*生成流水*/
	  				memset(RTrace_no,0x00,sizeof(RTrace_no));
	  				sprintf(RTrace_no,"%08ld%012ld",sPay_agent.tx_date,sPay_agent.trace_no);
	  				/*银行流水|业务日期|单位编号|操作员号|用户号|业务年度|实应收金额*/					
					fprintf( fp0,"%20s|%08ld|%4s|%20s|%20s|%4ld|%.02f\r\n",\
					 RTrace_no,sPay_agent.tx_date,sPay_agent.unit_no,g_pub_tx.tel,sPay_agent.remark,sPay_agent.end_date ,sPay_agent.tram*100);	  										
				
				}
				if(sAg_unitinfo.opr_attri[0]=='0')
	  			{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
						WRITEMSG
					}
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						WRITEMSG
					}				
					fprintf( fp,"│%20s│%20s│%20s│%16.2lf│%16.2lf│%8s│\n", sPay_agent.item_no,sPay_agent.ac_no,sPay_agent.name,sPay_agent.tram,sDd_mst.bal,"成    功");	  					
					
				    	if(memcmp(sAg_unitinfo.pay_type,"45",2)==0)
					{
						ys_amt = sPay_agent.tram;	/*瑞阳供热实应收金额*/
				    	}
				    	else
				    	{
				  		ys_amt = 0.00;
				   	 }
				   	 
					pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "G008",  sPay_agent.ac_no, sAg_unitinfo.opr_attri, sPay_agent.tram,sDd_mst.bal, ys_amt);
					/**end wudawei 20140610**/
					if(tot_cnt==PAGE_CNT){
						fprintf( fp,"\f");
						tot_cnt=0;
					}
		  		}	/* 代收费完成 */
				/*******add by ligl 2006-11-23 17:11********/
				
				if ( iK_flag == 1 )
				{
					lK_num = lK_num + 1;	/*新开户数*/
					dK_amt = dK_amt + sPay_agent.tram;	/*新开金额*/
				}
				
				if ( sAg_unitinfo.opr_attri[0] == '0' )
				{
					/*
					sql_commit();
					*/
				}
				tot_cnt++;
			}
			else
			{
				memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
				ret1 = Rsp_code_Sel( g_pub_tx.reply , &sRsp_code, "code = '%s'", cReturn );
				if ( ret1 == 0 )
				{
					memset( cReturn, 0x0, sizeof(cReturn) );
					pub_base_strpack( sRsp_code.memo );
					strcpy( cReturn, sRsp_code.memo );
				}
				
				lFnum = lFnum + 1;
				dFamt = dFamt + sPay_agent.tram;
				/*******add by ligl 2006-11-23 17:11********/
				if(sAg_unitinfo.opr_attri[0]=='0')
	  			{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
						WRITEMSG
					}
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
						WRITEMSG
					}				
					fprintf( fp1,"│%20s│%20s│%20s│%16.2lf│%16.2lf│%8s│\n", sPay_agent.item_no,sPay_agent.ac_no,sPay_agent.name,sPay_agent.tram,sDd_mst.bal,"失    败");	  			
					if(err_tot_cnt==PAGE_CNT){
						fprintf( fp1,"\f");
						err_tot_cnt=0;
					}
	  			}	/* 代收完成 */
	  			/********/
	  			else
	  			{				
					sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
					WRITEMSG
					goto ErrExit;
				}
				err_tot_cnt++;
			}
		}
		Pay_agent_Clo_Sel();
		/*******add by ligl 2006-11-23 17:11********/		
		if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"44",2)==0)
	  	{	
	  		fclose(fp0);
	  	}
	 	 else if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"43",2)==0)
	  	{	
	  		fclose(fp0);
	  	}
	  	 else if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"45",2)==0)
	  	{	
	  		fclose(fp0);
	  	}
    		if(sAg_unitinfo.opr_attri[0]=='0')
    		{
   			fprintf( fp, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━┷━━━━┙\n" );
   			fprintf( fp, "                                                                                                报表日期[%ld]\n" ,g_pub_tx.tx_date); /**增加报表日期行 zgf 20131125**/
			fprintf( fp1,"┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━┷━━━━┙\n" );
    		}
    		else
    		{ 
   			/*******end by ligl 2006-11-23 17:11********/
			fprintf( fp, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━┙\n" );
			fprintf( fp, "                                                                                                报表日期[%ld]\n" ,g_pub_tx.tx_date); /**增加报表日期行 zgf 20131125**/
			fprintf( fp1,"┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━┙\n" );
    		}
		fclose(fp);
		fclose(fp1);
		
		if ( lFnum > 0 )	/*有失败记录*/
		{
			memset( filename11, 0x0, sizeof(filename11) );
			sprintf( filename11, "%s/report/%s/BR1%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
			fp11 = fopen( filename11, "w" );/*该文件用来记录明细错误处理结果*/
			if( fp11==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename11 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			
			fprintf( fp11, "BR1%s.txt 代理业务失败明细清单\n", sAg_unitinfo.unit_no );
			
			fclose( fp11 );
			
			if ( sAg_unitinfo.opr_attri[0] == '1' )
			{
				/*continue;	*代发出错就不需要继续记帐*/
			}
		}

		if ( lSnum > 0 )	/*有成功记录*/
		{
			sprintf(acErrMsg,"-------------开始单位账号存取款了-----------");
			WRITEMSG

			g_pub_tx.ac_id = 0;	 /*test clean */
			g_pub_tx.ac_seqn = 0;
			strcpy(g_pub_tx.beg_note_no,"");
        		
			strcpy( g_pub_tx.ac_no, sAg_unitinfo.bank_acct_no );
			g_pub_tx.tx_amt1 = dSamt;
			
			if ( sAg_unitinfo.bank_acct_no[0] == '9' )	/*-- 内部账 --*/
			{
				strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
				strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
				g_pub_tx.svc_ind=9001;
				
				if ( sAg_unitinfo.opr_attri[0] == '1' )
				{
					sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
					WRITEMSG
					goto ErrExit;
				}
				else
				{
					strcpy(g_pub_tx.add_ind,"1");	/*-- 代缴 --*/
					
					memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
					strcpy( g_pub_tx.sub_tx_code, "A017" );
					set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
				
					set_zd_data( "1214", "01" );
					set_zd_data( "1215", "2" );
					set_zd_double( "1218", g_pub_tx.tx_amt1 );
					
					strcpy(g_pub_tx.brf, "代收");
				}				
			}
			else
			{
				strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为活期*/
				strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
				g_pub_tx.svc_ind=1001;
								
				if ( sAg_unitinfo.opr_attri[0] == '1' )
				{
					sprintf( acErrMsg,"本业务为代收业务，非代付业务[%s]", sAg_unitinfo.opr_attri[0]);
					WRITEMSG
					goto ErrExit;
				}
				else
				{
					strcpy(g_pub_tx.add_ind,"1");	/*-- 代缴 --*/
					
					memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
					strcpy( g_pub_tx.sub_tx_code, "D099" );
					set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
					
					strcpy( g_pub_tx.prt_ind, "0" );
					set_zd_data( "1017", "0" );
        		
					set_zd_data( "101A", "01" );
					set_zd_data( "101B", "2" );
					set_zd_double( "1013", g_pub_tx.tx_amt1 );					
					
					strcpy(g_pub_tx.brf, "代收");
				}
			}		
			
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.ac_wrk_ind,"001001001");
			strcpy(g_pub_tx.hst_ind,"1");
			
			ret=pub_acct_trance();
			if( ret )
			{
				sprintf( cReturn, "记代理单位账出错[%s]", g_pub_tx.reply );
				sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
			if ( ret )
			{
				sprintf( cReturn, "记代理单位账出错[%s]", g_pub_tx.reply );
				sprintf( acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}	
                	
			g_pub_tx.ac_id = 0;
			g_pub_tx.ac_seqn = 0;
			g_pub_tx.tx_amt1 = 0.00;        /*发生额*/
			
			strcpy( g_pub_tx.ac_id_type,"0" ); /*账户类型设为活期*/			
			
			memset( filename01, 0x0, sizeof(filename01) );
			sprintf( filename01, "%s/report/%s/BR0%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
			fp01 = fopen( filename01, "w" );
			if( fp01==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename01 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			else
			{
				fprintf( fp01, "BR0%s.txt 代理业务成功明细清单\n", sAg_unitinfo.unit_no );
			}			

			fclose( fp01 );
			
			memset( filename3, 0x0, sizeof(filename3) );
			sprintf( filename3, "%s/report/%s/BRDWHZ%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
			
			fp3 = fopen( filename3, "w" ); /*------正确处理结果汇总-------*/
			if( fp1==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename3 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
			}
			else
			{
				memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
				ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
				
				pub_base_strpack( sCom_branch.br_name );
				pub_base_strpack( sAg_unitinfo.bank_acct_no );
				fprintf( fp3, "				[%s]批量处理单位传票	\n", sAg_unitinfo.unit_name );
				fprintf( fp3, "机构码: %s    			机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
				fprintf( fp3, "单位代码: %s   			单位名称: %s  \n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
				fprintf( fp3, "日期 : %ld 			单位账号：%s \n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
				
				fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
				fprintf( fp3, "│单位编号│           单   位   名   称            │  笔数  │    金   额     │\n" );
				fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
				fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
				fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
				fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
				fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n\n\n\n\n\n\n" );
				
				fprintf( fp3, "				[%s]批量处理个人汇总传票	\n", sAg_unitinfo.unit_name );
				fprintf( fp3, "机构码: %s    			机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
				fprintf( fp3, "日期 : %ld 			汇总科目 ： \n", g_pub_tx.tx_date );
				
				fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
				fprintf( fp3, "│单位编号│           单   位   名   称            │  笔数  │    金   额     │\n" );
				fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
				fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
				fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
				fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
				fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n" );
			}			
			
			fclose( fp3 );	
			
			memset( filename31, 0x0, sizeof(filename31) );
			sprintf( filename31, "%s/report/%s/BRDWHZ%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
			
			fp31 = fopen( filename31, "w" );
			if( fp31==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename31 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			else
			{
				fprintf( fp31, "BRDWHZ%s.txt 代理业务成功汇总清单\n", sAg_unitinfo.unit_no );
			}
			
			fclose( fp31 );
		}

		lSnum = 0;	/*成功笔数*/
		lFnum = 0;	/*失败笔数*/
		dSamt = 0.00;	/*成功金额*/
		dFamt = 0.00;	/*失败金额*/
	}
	Ag_unitinfo_Clo_Sel();	
	ret = sql_execute( "insert into pay_agent_bk select * from pay_agent where tx_date = %ld",g_pub_tx.tx_date );
	ret = sql_execute( "delete from pay_agent where tx_date = %ld",g_pub_tx.tx_date );
OkExit:
	set_zd_data( "DC_TX_BR_NO", "99999" );
	sql_commit();
	if(fp != NULL)
	{
		fclose(fp);
	}
	if(fp0 != NULL)
	{
		fclose(fp0);
	}
	if(fp01 != NULL)
	{
		fclose(fp01);
	}
	if(fp1 != NULL)
	{
		fclose(fp1);
	}
	if(fp11 != NULL)
	{
		fclose(fp11);
	}
	if(fp3 != NULL)
	{
		fclose(fp3);
	}
	if(fp31 != NULL)
	{
		fclose(fp31);
	}
    	strcpy( g_pub_tx.reply, "0000" );
    	sprintf( acErrMsg, "日终批量代理业务处理成功完毕！" );
    	WRITEMSG
    	set_zd_data( DC_REPLY, g_pub_tx.reply );
    	return 0;
ErrExit:
	set_zd_data( "DC_TX_BR_NO", "99999" );
	Pay_agent_Clo_Sel();
	sql_rollback();
	if(fp != NULL)
	{
		fclose(fp);
	}
	if(fp0 != NULL)
	{
		fclose(fp0);
	}
	if(fp01 != NULL)
	{
		fclose(fp01);
	}
	if(fp1 != NULL)
	{
		fclose(fp1);
	}
	if(fp11 != NULL)
	{
		fclose(fp11);
	}
	if(fp3 != NULL)
	{
		fclose(fp3);
	}
	if(fp31 != NULL)
	{
		fclose(fp31);
	}
    	sprintf(acErrMsg,"日终批量代理业务处理失败！[%s]", g_pub_tx.reply );
    	WRITEMSG
    	set_zd_data(DC_REPLY,g_pub_tx.reply);
    	return 1;
}
