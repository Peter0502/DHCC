/*************************************************
* 文 件 名: spEB01.c
* 功能描述：网银批量代理业务处理
*
* 作    者: jack
* 完成日期: 2004年02月28日
* 修改日期: 20130527
* 修 改 人: 胡朝华
* 修改内容: 根据商行原有批量程序按照网银需求做局部修改
*************************************************/
#define ERR_DEAL  {WRITEMSG goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*默认证件类型为身份证*现改为可以任何证件类型fldstr[5]*/
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ag_paytlcs_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "ag_qs_gd_rel_c.h"

spEB01()
{
	int ret=0, ret1=0, ins_flag=0; /* 0成功  1失败 */
	char cReturn[32];

	char cAc_no[20];
	char cUnit_no[5];
	char cUnit_no_tmp[5];
	char cRemark[17];
	char cHand_flag[2];		/*---信息录入方式---*/
	char cFtpfile[200];
	double dT_amt = 0.00;	/*---前台输入总金额---*/
	long lCount_i = 0;		/*---前台输入总笔数---*/

	long lBeg_date = 0;
	long lEnd_date = 0;

	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;

	/*------返回结果信息------*/
	long lSnum = 0;		/*成功笔数*/
	long lFnum = 0;		/*失败笔数*/
	double dSamt = 0.00;	/*成功金额*/
	double dFamt = 0.00;	/*失败金额*/

	int iK_flag = 0;	/*新开户标志*/
	long lK_num = 0;	/*新开户数*/
	double dK_amt = 0.00;	/*新开金额*/

	char cDate[9];		/*费用发生日期*/

	long cif_no = 0;
	int iLen = 0;
	long lCount = 0;

	char filename[248];
	char pcfilename[248];
	char filename1[248];
	char filename2[248];
	char sw_trace[20];
	FILE *fp=NULL;
	FILE *fp1=NULL;

	char execstr[256];

	char fldstr[10][100];
	char tmpstr[1024];
	

	int iProc_flag = 0;		/*处理标志*/
	int iProc_flag1 = 0;		/*成功失败标志*/
	int iErr_flag = 0;		/*是否有失败记录标志*/

	int i=0;

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_paytlcs_c sAg_paytlcs;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;
	struct ag_qs_gd_rel_c sAg_qs_gd_rel;
	memset(cAc_no,0x0,sizeof(cAc_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cRemark,0x0,sizeof(cRemark));
	memset(cReturn,0x0,sizeof(cReturn));
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );
	memset( cFtpfile, 0x0, sizeof(cFtpfile) );
	/****/
	memset(cUnit_no_tmp,0x0,sizeof(cUnit_no_tmp));
	memset(sw_trace,0x0,sizeof(sw_trace));
	memset(pcfilename,0x0,sizeof(pcfilename));
	memset(filename2,0x0,sizeof(filename2));
	/*add by kevinchan 20120311 start*/
	struct ag_peritemmap_c sAg_peritemmap1;
	int daliCnt=0;
	char newAcno[25];
	/*add by kevinchan 20120311 end*/

	get_zd_data("0520",sw_trace);
	get_zd_data("0920",cUnit_no);
	sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG
	memcpy(cUnit_no_tmp,cUnit_no,sizeof(cUnit_no_tmp)-1);
		sprintf( acErrMsg, "单位编号[%s]",cUnit_no_tmp);
	WRITEMSG
	memcpy(cUnit_no,cUnit_no_tmp,sizeof(cUnit_no)-1);
	sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG
	get_zd_data("0300",cAc_no);
	pub_base_old_acno( cAc_no );

	get_zd_data("0610",cRemark);
	get_zd_data( "0700", cHand_flag );

	get_zd_long( "0460", &lBeg_date );
	get_zd_long( "0470", &lEnd_date );

	memset( cDate, 0x0, sizeof(cDate) );
	sprintf( cDate, "%ld", lBeg_date );
	sprintf( acErrMsg, "单位账号[%s]",cAc_no);
	WRITEMSG

	sprintf( acErrMsg, "取得的批处理日期为[%ld][%ld]", lBeg_date, lEnd_date );
	WRITEMSG

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	memcpy(cUnit_no,cUnit_no_tmp,sizeof(cUnit_no)-1);
	sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG

	memset(filename,0x0,sizeof(filename));
	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel ( g_pub_tx.reply, &sAg_unitinfo,
			"unit_no = '%s'" , cUnit_no );
	if( ret )
	{
		sprintf(acErrMsg," 查询单位资料出错[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack( sAg_unitinfo.pay_type );
	/**校验企业清算账户和过度账户关联关系**/
	memset( &sAg_qs_gd_rel, 0x00, sizeof(sAg_qs_gd_rel) );
	ret = Ag_qs_gd_rel_Sel( g_pub_tx.reply, &sAg_qs_gd_rel, \
		"qs_ac_no = '%s' and unit_no = '%s'", cAc_no,cUnit_no );
	if ( ret )
	{
		sprintf(acErrMsg,"查询批量代理业务结算账户和过度账户关联表失败");
		WRITEMSG
		strcpy( g_pub_tx.reply,"EB05" );
		goto ErrExit;
	}
	vtcp_log("%s,%d 检查批量代理业务结算账户和过度账户关联表通过!",__FILE__,__LINE__);
	memset(filename1,0x0,sizeof(filename1));
	memset(filename2,0x0,sizeof(filename2));
	sprintf(filename2,"nb_bat_res%ld%s.txt",g_pub_tx.tx_date,sw_trace);
	sprintf(filename1,"%s/nb_bat_res%ld%s.txt",getenv("FILDIR"),g_pub_tx.tx_date,sw_trace);
	fp1 = fopen( filename1,"w" );	/*该文件用来记录处理结果*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	fprintf( fp1, "%20s│%20s│%16s│%20s│%20s│%18s│%28s│\n", "员工编号", "员工账号", "员工姓名", "交易金额", "成功状态", "备注" ,"指令流水号");

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	get_zd_data("0260",filename);
	sprintf(pcfilename,"%s/tmp/%s",getenv("WORKDIR"),filename);
	sprintf(acErrMsg,"取得的文件名为[%s]",pcfilename);
	WRITEMSG
	
	sprintf(cFtpfile,"%s/bin/ftp_getfile_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftp文件[%s]到平台取文件出错[%d]",filename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftp文件[%s]到平台取文件成功",filename);
	WRITEMSG

	fp = fopen( pcfilename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	if ( cHand_flag[0] == '0' )	/*---手工---*/
	{
		get_zd_double("0400",&dT_amt);
		get_zd_long( "0550", &lCount_i );
	}
	else /**非手工方式文件第一行为汇总信息**/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<9;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		if ( strncmp( cUnit_no, fldstr[3], 4 ) != 0 )
		{
			sprintf( acErrMsg, "代理单位[%s]编号信息不符", fldstr[3] );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H035" );
			goto ErrExit;
		}

		memset( cDate, 0x0, sizeof(cDate) );
		strcpy( cDate, fldstr[2] );
		/**如果文件中的日期与当天日期不一致,不允许通过 add at 20070517**/
		if(atol(fldstr[2])!=g_pub_tx.tx_date)
		{
			sprintf( acErrMsg, "处理日期[%s]信息不符", fldstr[2] );
			WRITEMSG
			strcpy( g_pub_tx.reply, "P096" );
			goto ErrExit;
		}
		/**end 20070517**/
		dT_amt = atof( fldstr[7] );
		lCount_i = atoi(fldstr[6] );

	}

	/*------ 判断明细金额和总金额是否相等 -----*/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		/**	fldstr[0] 账户名称
				fldstr[1] 金额
				fldstr[2] 账户
				fldstr[3] 单位编号
				fldstr[4] 代理次数
				fldstr[5] 指令流水号 **/
		if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		{
			continue;
		}

		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[1] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		sprintf(acErrMsg,"该笔明细金额与分户交易金额合计分别为[%.2lf][%.2lf]", dTemp_amt, dTmp_amt );
		WRITEMSG 

		lCount = lCount + 1;
	}
	fclose ( fp );

	sprintf(acErrMsg,"总金额与分户交易金额合计分别为[%.2lf][%.2lf]", dT_amt, dTmp_amt );
	WRITEMSG

	ret = pub_base_CompDblVal( dT_amt, dTmp_amt );
	if ( ret )
	{
		sprintf(acErrMsg,"总金额与分户交易金额合计不相等[总%lf][明细%lf]", dT_amt, dTmp_amt );
		WRITEMSG
		sprintf(acErrMsg,"明细总金额为[%.2lf]", dTmp_amt );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}

	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"总笔数与分户明细合计笔数不相等[%d][%d]!!!",lCount,lCount_i);
		WRITEMSG
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	vtcp_log("aaaaaaaaa[%s]\n",sAg_unitinfo.bank_acct_no);
		/**检测公司帐户的状态**/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sAg_unitinfo.bank_acct_no );
		if ( ret )
		{
			sprintf(acErrMsg,"查询介质账号对照表失败");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
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
				goto ErrExit;
	   }
	   if(sMdm_ac_rel.note_sts[0]!='0' || sIn_mst.sts[0]!='1')
		 {
				sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "H059" );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
				goto ErrExit;
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
					goto ErrExit;
				}
				if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
					|| sDd_mst.hold_sts[0] == '1' )	
				{
					sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					goto ErrExit;	
				}
	}
	/**********添加对活期企业账户账户状态的检查和余额情况的检查 20130727 hzh start************/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s' and note_sts= '0'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"查询介质账号对照表失败");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
		and ac_seqn = 1", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg,"查询活期主文件失败[%d][%ld][%s]", ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
		if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
			|| sDd_mst.hold_sts[0] == '1' )	
		{
			sprintf(acErrMsg,"账户状态不正常[%s]", sMdm_ac_rel.ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H059" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;	
		}
		if(sDd_mst.bal<dT_amt)
		{
			sprintf(acErrMsg,"账户余额不正常[%s]", sMdm_ac_rel.ac_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "T066" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
	/**********添加对活期企业账户账户状态的检查和余额情况的检查 20130727 hzh end************/

	fp = fopen( pcfilename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open 代理 file [%s] error ",pcfilename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"打开文件名称[%s]", pcfilename );
	WRITEMSG
	if ( cHand_flag[0] != '0' )	/*---非手工去掉第一行---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}
		ins_flag=0;

	while(1)
	{
		iProc_flag = 0;
		iProc_flag1 = 0;
		iK_flag = 0;

		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		{
			sprintf(acErrMsg,"*******[%s],[%s],[%s]", pcfilename,fldstr[0],fldstr[1] );
			WRITEMSG
			continue;
		}
		
		/**增加新旧账号转换**/
		memset(newAcno,0x0,sizeof(newAcno));
		strncpy(newAcno,fldstr[2],strlen(fldstr[2]));
		pub_base_old_acno(newAcno);
		
		/**代发编号**/
		sprintf(acErrMsg,"3333333333333333[%s]", pcfilename );
		WRITEMSG
			memset( &sAg_peritemmap1, 0x0, sizeof(sAg_peritemmap1) );
			ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap1,"unit_no = '%s' and ac_no = '%s' and stat='0'",cUnit_no, newAcno);
			if(ret)
			{
						if (memcmp(newAcno,"621",3) == 0)
						{
							sprintf( cReturn, "单位编号错误或者卡号不存在");
							sprintf( acErrMsg, "单位编号错误或者卡号不存在[%s][%s]",cUnit_no,fldstr[2]);
						}
						else
						{
							sprintf( cReturn, "单位编号或账号信息错误！");
							sprintf( acErrMsg, "单位编号或账号信息错误！[%s][%s]",cUnit_no,fldstr[2]);
						}
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
			}
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		/*对代收处理 收水电费等add by ligl 2006-11-24 21:06*/
		if(sAg_unitinfo.opr_attri[0]=='0')
	{
			memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap));
			pub_base_old_acno(fldstr[2]);
			  ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and ac_no = '%s'",sAg_unitinfo.unit_no, fldstr[2]);
		if(ret==0)
		{
		  pub_base_strpack_all( sAg_peritemmap.name );
					pub_base_strpack_all( fldstr[0] );
					if ( strcmp( sAg_peritemmap.name, fldstr[0] ) )
					{
							sprintf( cReturn, "户名不符,原户名为[%s]", sAg_peritemmap.name );
							sprintf( acErrMsg, "户名不符,原户名为[%s]", sAg_peritemmap.name );
							WRITEMSG
							iProc_flag1 = 1;
							iErr_flag = 1;
							goto TrLog;
					}
				memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel));
				ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel,
					"ac_no = '%s'", sAg_peritemmap.ac_no );
				if ( ret1 )
				{
					sprintf( cReturn, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
				ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst,
					"ac_id = %ld", sMdm_ac_rel.ac_id );
				if ( ret1 )
				{
					sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				pub_base_strpack( sDd_mst.ac_sts );
				if ( sDd_mst.ac_sts[0] == '*'||sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "已销户,完全冻结" );
					sprintf( acErrMsg, "已销户,新开户[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iK_flag = 1;
					iProc_flag = 1;
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

		}
		else if(ret==100)
		{
			strcpy( cReturn, "代理户不存在" );
			sprintf(acErrMsg, "查询代收记录出错[%s][%s]", cUnit_no, fldstr[2]);
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		else
		{
			sprintf(acErrMsg, "查询代发记录出错[%s][%s][%d]", cUnit_no, fldstr[2], ret );
			WRITEMSG
			goto ErrExit;
		}
	/*---------------批量导入数据部分代扣-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sAg_peritemmap1.item_no );
		strcpy(sPay_agent.name, fldstr[0] );
		strcpy(sPay_agent.ac_no, fldstr[2]);
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[1]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	   /*-- 需要批量处理 --*/
		strcpy(sPay_agent.result,"1");	 /*-- 未处理 --*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		strcpy(sPay_agent.cnt,fldstr[4]);/*--代收付次数--*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
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
	}
	/**end by ligl 2006-11-24 21:06**/
	else
	{
		sprintf(acErrMsg,"4444444444444444[%s]", pcfilename );
		WRITEMSG
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,
			"unit_no = '%s' and item_no = '%s' and stat='0'",
			cUnit_no, sAg_peritemmap1.item_no);
		if ( ret == 0 )
		{
			pub_base_strpack( sAg_peritemmap.id_no );
			pub_base_strpack_all( sAg_peritemmap.name );

			pub_base_strpack_all( fldstr[0] );

			if ( strcmp( sAg_peritemmap.name, fldstr[0] ) )
			{
				sprintf( cReturn, "户名不符,原户名为[%s]", sAg_peritemmap.name );
				sprintf( acErrMsg, "户名不符,原户名为[%s]", sAg_peritemmap.name );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}

			if ( strcmp(sAg_peritemmap.id_no, fldstr[6]) &&
				strcmp( sAg_peritemmap.name, fldstr[0]) &&
				sAg_unitinfo.opr_attri[0] == '1' &&
				sAg_unitinfo.auto_flag[0] == '1') /*-证件号码和户名不同,代付-*/
			{
				iLen = 0; ins_flag=1;
				iLen = strlen( fldstr[6] );
				if ( iLen == 0 )
				{
					sprintf( cReturn, "新开户,证件号码不能为空" );
					sprintf( acErrMsg, "新开户,证件号码不能为空" );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}

				strcpy( cReturn, "新开户" );
				sprintf(acErrMsg,"新开户[%s]", sAg_peritemmap.item_no);
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
			}
			/*-----------------------------------------------------------*/
			pub_base_strpack( sAg_peritemmap.ac_no );
			pub_base_old_acno( sAg_peritemmap.ac_no );

			if ( sAg_unitinfo.opr_attri[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" )
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )
			{
				memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
				ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel,
					"ac_no = '%s'", sAg_peritemmap.ac_no );
				if ( ret1 )
				{
					sprintf( cReturn, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "查找账号[%s]信息错", sAg_peritemmap.ac_no );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				if(strlen(fldstr[2]) != strlen(sAg_peritemmap.ac_no) || memcmp(fldstr[2],sAg_peritemmap.ac_no,strlen(fldstr[2])) != 0)
				{
						if (memcmp(fldstr[2],"621",3) == 0)
						{
							sprintf( cReturn, "单位编号错误或者卡号不存在");
							sprintf( acErrMsg, "单位编号错误或者卡号不存在[%s][%s]",cUnit_no,fldstr[2]);
						}
						else
						{
							sprintf( cReturn, "单位编号或账号信息错误！");
							sprintf( acErrMsg, "单位编号或账号信息错误！[%s][%s]",cUnit_no,fldstr[2]);
						}
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}

				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
				ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst,
					"ac_id = %ld", sMdm_ac_rel.ac_id );
				if ( ret1 )
				{
					sprintf( cReturn, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "查找账号[%s]活期信息错", sAg_peritemmap.ac_no );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}

				pub_base_strpack( sDd_mst.ac_sts );
				
				if( sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "该账户完全冻结,不能转帐" );
					sprintf( acErrMsg, "该账户完全冻结,不能转帐[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				if( sMdm_ac_rel.note_sts[0]!='0' )
				{
					strcpy( cReturn, "该账户介质状态不正常,不能转帐" );
					sprintf( acErrMsg, "该账户介质状态不正常,不能转帐[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				if ( sDd_mst.ac_sts[0] == '*')
				{
					strcpy( cReturn, "该账户已销户" );
					sprintf( acErrMsg, "该账户已销户,需新开户[%s]", sAg_peritemmap.item_no );
					WRITEMSG

					iLen = 0;
					iLen = strlen( fldstr[6] );
					if ( iLen == 0 && sDd_mst.ac_sts[0] == '*' )
					{
						sprintf( cReturn, "该账户已销户,需新开户" );
						sprintf( acErrMsg, "销户新开,证件号码不能为空" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
					}

					iK_flag = 1;
					iProc_flag = 1;
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
				
				/**增加对客户代理次数检查**/
				daliCnt = sql_count("pay_agent","ac_no='%s' and unit_no='%s'",newAcno,cUnit_no);
				if( daliCnt<0 )
				{
					sprintf(acErrMsg,"sql_count error!! [%d]",daliCnt);
					WRITEMSG
					goto ErrExit;
				}
				if((daliCnt+1)!=atoi(fldstr[4]))
				{
						sprintf( cReturn, "代理次数错误！应该为[%d]",daliCnt+1);
						sprintf( acErrMsg, "代理次数错误！[%s][%s]",cUnit_no,fldstr[2]);
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
			}
		}
		else if ( ret == 100 )
		{
				strcpy( cReturn, "代理编号不存在" );
				sprintf(acErrMsg, "查询代发记录出错[%s][%s]", cUnit_no, sAg_peritemmap1.item_no);
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		else
		{
			sprintf(acErrMsg, "查询代发记录出错[%s][%s][%d]", cUnit_no, sAg_peritemmap1.item_no, ret );
			WRITEMSG
			goto ErrExit;
		}

		/*---------------批量导入数据部分-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));

		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sAg_peritemmap1.item_no );
		strcpy(sPay_agent.name, fldstr[0] );
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[1]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	/*-- 需要批量处理 --*/
		strcpy(sPay_agent.result,"1");	/*-- 未处理 --*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		strcpy(sPay_agent.cnt,fldstr[4]);/*--代收付次数--*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		if ( sAg_unitinfo.opr_attri[0] == '0' )	/*代扣处理20050902*/
		{
			ret = sql_count( "pay_agent", "unit_no = '%s' and item_no = '%s'and cnt='%s' \
				and end_date >= %ld", cUnit_no, sAg_peritemmap1.item_no,fldstr[4], g_pub_tx.tx_date );
			if ( ret > 0 )
			{
				strcpy( cReturn, "代扣记录已存在,不能再添加");
				sprintf(acErrMsg,"代扣记录已存在,不能再添加!");
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
		}

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
			Pay_agent_Debug(&sPay_agent);
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		if ( iProc_flag1 == 0 )
		{
			strcpy( cReturn, "成功" );
		}
		}
TrLog:
		if ( iProc_flag1 == 0 )
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + atof(fldstr[1]);
			fprintf( fp1, "%10s│%20s│%30s│%20s│%10s│%20s│%20s│\n",sAg_peritemmap1.item_no, sAg_peritemmap.ac_no ,fldstr[0],
			fldstr[1], "Y", cReturn ,fldstr[5]);

			if ( iK_flag == 1 )
			{
			lK_num = lK_num + 1;	/*新开户数*/
			dK_amt = dK_amt + atof(fldstr[1]);	/*新开金额*/
			}
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[1]);
			fprintf( fp1, "%20s│%20s│%16s│%20s│%20s│%8s│%20s│\n",sAg_peritemmap1.item_no, fldstr[2] ,fldstr[0],
			fldstr[1], "N", cReturn ,fldstr[5]);
		}
	}

 fprintf( fp1, "\n" );

	fclose(fp);
	fclose(fp1);
	
	sprintf(acErrMsg,"成功笔数[%ld]金额[%.2f];失败笔数[%ld]金额[%.2f]", lSnum, dSamt, lFnum, dFamt );
	
	ret = sql_execute( "update ag_unitinfo set beg_date = %ld, end_date = %ld \
		where unit_no = '%s'", lBeg_date, lEnd_date, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "更改单位批量处理日期错!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H020" );
		ins_flag=1;
		goto ErrExit;
	}

	if ( iErr_flag != 0 ){
		/*              是代付业务           非数据库问题        是导盘方式   */
		if(sAg_unitinfo.opr_attri[0]=='1' && ins_flag==0  && cHand_flag[0] != '0'){
			vtcp_log("[%s][%d]导盘方式导入部分数据",__FILE__,__LINE__);
			strcpy( cReturn, "请看 8608 交易查看导入结果");
			memset( cFtpfile, 0x0, sizeof(cFtpfile) );
			sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename2);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
			{
			sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",filename2,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P157");
			goto ErrExit;
			}
			sprintf(acErrMsg,"ftp文件[%s]到平台成功",filename2);
			WRITEMSG
			goto OkExit;
		}
		/* 其他方式都认为失败,数据全部回滚 */
		vtcp_log("[%s][%d]导入数据失败\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}
OkExit:
	/**网银代发需要在这里添加对成功金额的记账 20130726 hzh start**/
	/****************借方记账赋值********************/
	set_zd_data("0660","1");/**借贷标志**/
	set_zd_data("0670","2");/**现转标志**/
	set_zd_data("0680","1");/**交易类型**/
	set_zd_data("0880","代发转出");/**摘要**/
	set_zd_data("0210","01");/**币种**/
	/**89,58域看看后面有什么用途**/
	/****/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	/**只对成功登记到pay_agent表（代收付未处理记录表）的数据记账**/
	set_zd_double("0400",dSamt);
	/****************贷方记账赋值********************/
	set_zd_data("1011",sAg_unitinfo.bank_acct_no);/**转入内部账户**/
	set_zd_data("1012","0");/**账户序号**/
	set_zd_data("1016","代发转入");/**摘要**/
	set_zd_data("101A","01");/**币种**/
	set_zd_data("101B","01");/**现转标志**/
	set_zd_data("101C","031");/**自输摘要标志**/
	set_zd_double("1013",dSamt);
	/**1016,101C,1014,1015**/
	vtcp_log("%s,%d 成功登记入代收付未处理记录表pay_agent的总金为[%lf]\n",__FILE__,__LINE__,dSamt);
	vtcp_log("%s,%d 记账赋值结束！！！\n",__FILE__,__LINE__);
	/**网银代发需要在这里添加对成功金额的记账 20130726 hzh end**/
	/**正确时时也往平台发明细文件**/
	memset( cFtpfile, 0x0, sizeof(cFtpfile) );
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename2);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
	sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",filename2,ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P157");
	goto ErrExit;
	}
	sprintf(acErrMsg,"ftp文件[%s]到平台成功",filename2);
	WRITEMSG
	/**正确时也往平台发明细文件**/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"批量代理业务处理完毕！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	fclose(fp);
	fclose(fp1);
	sprintf(acErrMsg,"%s,%d,导入数据失败ins[%d],iErr[%d]",__FILE__,__LINE__,ins_flag,iErr_flag);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}