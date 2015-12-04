/*************************************************
* 文 件 名: gD400.c
* 功能描述：为燃气公司导入数据
* 作    者: 武大为
* 完成日期: 2014年03月14日
* 修改日期:
* 修 改 人:
* 修改内容: 
* 导入文件格式： 煤气号|金额|日期
* insert into com_eod_exec (DO_SEQ, SUB_TX_CODE, PROG, PROG_NAME, BR_TYPE, EXEC_PD, PD_MONTH, EXEC_BEG, EXEC_END, DO_TYPE, USING, HOLIDAY, FINISH, EOY_NUM, EOY_FINISH)
values (123, 'Z110', 'gD400', '燃气代扣导数', '1111111111', '0', '0', '00', '00', '0', 'Y', 'Y', 'N', '3', 'Y');
*************************************************/
#define ERR_DEAL  {WRITEMSG goto ErrExit;}
#define EXTERN
#define cIdtp "1"		
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

gD400()
{
	char filename[248];
	char filename1[248];
	char filename2[248];
	char filename3[248];
	FILE *fp=NULL;
	FILE *fp1=NULL;
	FILE *fp2=NULL;
	FILE *fp3=NULL;
	
	char tmpstr[1024];
	char cUnit_no[5];
	char fldstr[5][100];
	char cReturn[32];
	char sItem_no[20];
  char tx_date[9];
  char tmp[3];
	
	int ret = 0, ins_flag=0, i=0;
	
	int iProc_flag = 0;		/*处理标志*/
	int iProc_flag1 = 0;	/*成功失败标志*/
	int iErr_flag = 0;		/*是否有失败记录标志*/
	
	double ys_amt = 0.00;
	double tx_amt = 0.00;
	double dd_bal = 0.00;
	char sTx_amt[18];
	
	long lSnum = 0;		/*成功笔数*/
	long lFnum = 0;		/*失败笔数*/

	double dSamt = 0.00;	/*成功金额*/
	double dFamt = 0.00;	/*失败金额*/
	
	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
  struct in_mst_c sIn_mst;
  
  sql_begin();
  
  memset(tx_date,0x00,sizeof(tx_date));
	sprintf(tx_date, "%ld", g_pub_tx.tx_date); 
	
	memset(tmp,0x00,sizeof(tmp));
	strcpy(tmp, tx_date+6);
	if(strncmp(tmp ,"22", 2)!=0)
	{
		vtcp_log("不是22号 不代扣煤气费[%s]",tx_date);
		goto OkExit;
  }

	memset(filename,0x0,sizeof(filename));
	sprintf(filename, "%s/k01%ld.txt", getenv("FILDIR"), g_pub_tx.tx_date); 

	pub_base_sysinit();
	/*---------------------------  查单位信息 燃气44 ----------------------------------*/	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel ( g_pub_tx.reply, &sAg_unitinfo,
			"pay_type = '%s'" , "44" );
	if( ret )
	{
		sprintf(acErrMsg," 查询单位资料出错[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	memset(cUnit_no,0x00,sizeof(cUnit_no));
	strcpy(cUnit_no, sAg_unitinfo.unit_no);
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack( sAg_unitinfo.pay_type );
	
	/*---------------------------------------把记录正确和错误信息的文件初始化------------------------------*/
	memset(filename1,0x0,sizeof(filename1));
	sprintf(filename1,"%s/0%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel);
	fp1 = fopen( filename1,"w" );	/*该文件用来记录正确处理结果*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	fprintf( fp1, "                                     [%s-%s]批量导入成功记录[%s-%ld]\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
	fprintf( fp1, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━┑\r\n" );
	fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│\r\n", "代理编号", "客户姓名", "交易金额", "证件号码", "账号", "处理结果" );
	fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━┥\r\n" );

	memset(filename2,0x0,sizeof(filename2));
	sprintf(filename2,"%s/1%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel);
	fp2 = fopen( filename2,"w" );	/*该文件用来记录错误处理结果*/
	if( fp2==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	fprintf( fp2, "\r\n\r\n\r\n                         [%s-%s]批量导入失败记录[%s-%ld]\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date);
	fprintf( fp2, "┍━━━━┯━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━┯━━━━━━━━━━━━━━━┑\r\n" );
	fprintf( fp2, "│%8s│%10s│%20s│%16s│%6s│%30s│\r\n", "","客户姓名", "账号", "金额", "用户号", "处理结果" );
	fprintf( fp2, "┝━━━━┿━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━┯━━━━━━━━━━━━━━━┥\r\n" );

	/*---------------------------------  打开要读的文件    -----------------------------------*/
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	
	while(1)
	{
		iProc_flag = 0;
		iProc_flag1 = 0;  
		
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0; i<3; i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		if ( strlen(fldstr[0]) == 0 || strlen(fldstr[1]) == 0 )
		{
			continue;
		}
		memset(cReturn,0x00,sizeof(cReturn));
		vtcp_log(" 日期 [%s] ",tx_date);
		if (strncmp(fldstr[2] ,tx_date,8) !=0 )
		{
				sprintf( cReturn, "交易日期不对[%s][%s]", fldstr[0], fldstr[2]);
				sprintf( acErrMsg, "交易日期不对[%s][%s]", fldstr[0], fldstr[2]);					
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		memset(sItem_no,0x00,sizeof(sItem_no));
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and remark = '%s' and stat='0'",cUnit_no, fldstr[0]);
		if(ret)
		{					
				sprintf( cReturn, "用户编号不存在！[%s]", fldstr[0]);
				sprintf( acErrMsg, "用户编号不存在！[%s][%s]",cUnit_no,fldstr[0]);					
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		strcpy( sItem_no,sAg_peritemmap.item_no);
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel));
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", sAg_peritemmap.ac_no );
		if ( ret )
		{
			sprintf( cReturn, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
			sprintf( acErrMsg, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		pub_base_strpack( sMdm_ac_rel.note_sts );
		vtcp_log("%s,%d sMdm_ac_rel.note_sts =======[%s]",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
		if(sMdm_ac_rel.note_sts[0] == '*')
		{
				sprintf( cReturn, "介质状态异常！" );
				sprintf( acErrMsg, "介质状态异常！" );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		
				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
				ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", sMdm_ac_rel.ac_id );
				if ( ret )
				{
					sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				pub_base_strpack( sDd_mst.ac_sts );
				if ( sDd_mst.ac_sts[0] == '*' )
				{
					strcpy( cReturn, "已销户" );
					sprintf( acErrMsg, "已销户[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if ( sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "完全冻结" );
					sprintf( acErrMsg, "完全冻结[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if ( sDd_mst.hold_sts[0]=='2' )
				{
					strcpy( cReturn, "只进不出" );
					sprintf( acErrMsg, "只进不出[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if(sDd_mst.sttl_type[0]=='S')
				{
					strcpy( cReturn, "该户为非结算户,不能转帐" );
					sprintf( acErrMsg, "该户为非结算户,不能转帐[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				ys_amt = atof(fldstr[1]);
				dd_bal = sDd_mst.bal - sDd_mst.ctl_amt ;
				if(sDd_mst.hold_sts[0]=='3')
				{
					dd_bal = dd_bal - sDd_mst.hold_amt;
				}
				if(ys_amt > dd_bal )
				{
					tx_amt = dd_bal;
				}else
				{
					tx_amt = ys_amt;
				}
				vtcp_log("[%s][%d] 应收[%lf],余额[%lf], 冻结金额[%lf], 控制金额[%lf], 发生额[%lf]",__FILE__,__LINE__,ys_amt,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,tx_amt);
				if(tx_amt==0)
				{
					strcpy( cReturn, "该账户余额为0" );
					sprintf( acErrMsg, "该账户余额为0[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					/* 写短信息 add by wudawei 20140610  您尾号#1#于#2#余额为0，无法自动代扣燃气费，请您尽快到晋中银行网点缴费**/
					pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "G008",  sAg_peritemmap.ac_no, sAg_unitinfo.opr_attri, 0.00,0.00, ys_amt); 
					/**end wudawei 20140610**/
					goto TrLog;
				}
				memset(sTx_amt,0x00,sizeof(sTx_amt));
				sprintf( sTx_amt, "%10.2lf", tx_amt);
				vtcp_log("[%s][%d] sAg_peritemmap.name=[%s] , sAg_peritemmap.ac_no=[%s]",__FILE__,__LINE__,sAg_peritemmap.name , sAg_peritemmap.ac_no);
				
		/*---------------批量导入数据部分代扣-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sItem_no );
		strcpy(sPay_agent.name, sAg_peritemmap.name );
		strcpy(sPay_agent.id_type, sAg_peritemmap.id_type );
		strcpy(sPay_agent.id_no, sAg_peritemmap.id_no );
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = g_pub_tx.tx_date;
		sPay_agent.tram = tx_amt;
		strcpy(sPay_agent.remark, sAg_peritemmap.remark );
		strcpy(sPay_agent.sata,"1");	/*-- 需要批量处理 --*/
		strcpy(sPay_agent.result,"1");	/*-- 未处理 --*/
		strcpy(sPay_agent.cnt,"1");/*--代收付次数--*/
		strcpy(sPay_agent.result_name,fldstr[1]);/*--保存下应收金额--*/
		ret = 0;
		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply  );
		if (ret != 0)
		{
			if ( ret ==-268)
			{
				strcpy( cReturn, "未处理记录重复");
				sprintf(acErrMsg,"未处理记录重复");
			}
			else
			{
				strcpy( cReturn, "写未处理记录表出错");
				sprintf(acErrMsg,"插入未处理记录表失败!");
			}
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		if ( iProc_flag1 == 0 )
		{
			strcpy( cReturn, "成功" );
		}
		
		
		/*-------------------    写日志  -----------------------------*/
	TrLog:
		if ( iProc_flag1 == 0 )
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + tx_amt;
			/* fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│\r\n", "代理编号", "客户姓名", "交易金额", "证件号码", "账号", "处理结果" ); */
			fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│\r\n",sItem_no, sAg_peritemmap.name,
					sTx_amt, sAg_peritemmap.id_no, sAg_peritemmap.ac_no, cReturn );
			vtcp_log("[%s][%d] 写成功记录 sAg_peritemmap.ac_no=[%s] , sAg_peritemmap.item_no=[%s]",__FILE__,__LINE__,sAg_peritemmap.ac_no,sItem_no);
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[1]);
			/* fprintf( fp2, "│%8s│%10s│%20s│%16s│%6s│%30s│\r\n", "代理编号","客户姓名", "账号", "金额", "用户号", "处理结果" );*/
			fprintf( fp2, "│%8s│%10s│%20s│%16s│%6s│%30s│\r\n",sItem_no,sAg_peritemmap.name, sAg_peritemmap.ac_no,
					fldstr[1], fldstr[0], cReturn );
			vtcp_log("[%s][%d] 写失败记录[%s] sAg_peritemmap.ac_no=[%s] , sAg_peritemmap.item_no=[%s]",__FILE__,__LINE__,cReturn,sAg_peritemmap.ac_no,sItem_no);
		}
	}
	
	
	
	/* 循环完毕 写文件底部 */
	
	fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━┥\r\n" );
	fprintf( fp1, "│%20s│%20d│%16.2f│%20s│%20s│%8s│\r\n","合计", lSnum, dSamt, "", "", "");
 	fprintf( fp1, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━┙\r\n" );
 	
  fprintf( fp2, "┝━━━━┿━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━┯━━━━━━━━━━━━━━━┥\r\n" );
  fprintf( fp2, "│%8s│%10d│%20s│%16.2f│%6s│%30s│\r\n","合计",lFnum, "", dFamt, "", "" );
 	fprintf( fp2, "┕━━━━┷━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━┷━━━━━━━━━━━━━━━┙\r\n" );

	fclose(fp);
	fclose(fp1);
	fclose(fp2);

	sprintf(acErrMsg,"成功笔数[%ld]金额[%.2f];失败笔数[%ld]金额[%.2f]", lSnum, dSamt, lFnum, dFamt );		
	WRITEMSG
	ret = sql_execute( "update ag_unitinfo set beg_date = %ld, end_date = %ld \
		where unit_no = '%s'", g_pub_tx.tx_date, g_pub_tx.tx_date, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "更改单位批量处理日期错!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H020" );
		ins_flag=1;
		goto ErrExit;
	}

	if ( iErr_flag != 0 ){
		if( ins_flag==0  ){
			vtcp_log("[%s][%d]导盘方式导入部分数据",__FILE__,__LINE__);
			strcpy( cReturn, "请看 8608 交易查看导入结果");
			goto OkExit;
		}
		/* 其他方式都认为失败,数据全部回滚 */
		vtcp_log("[%s][%d]导入数据失败\r\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}


OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"批量代理业务处理完毕！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	sql_rollback();
	sprintf(acErrMsg,"%s,%d,导入数据失败ins[%d],iErr[%d]",__FILE__,__LINE__,ins_flag,iErr_flag);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	
}
