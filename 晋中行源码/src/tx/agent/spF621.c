/*************************************************
* 文 件 名: spF621.c
* 功能描述：批量代理业务数据录入处理(此交易由620复制而来,进行了修改，为了配合winqt的新批量代理交易)
*
* 作    者: jack
* 完成日期: 2004年02月28日
* 修改日期:
* 修 改 人:
* 修改内容: 磁盘方式发工资时成功的户导入表中,不成功的生成文件
insert into tx_Def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('6621', 'winqt批量业务信息录入', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('6621', 0, 'F621', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('F621', 'winqt批量代发工资', 'spF621', '0', '0');
-------
insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('6621', 1, '', '', '', '', '', '', '', '', '0000', 'winqt批量数据导入');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '6621', 1, 'winqt批量导数授权', 'sq(1237)');
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

spF621()
{
	int ret=0, ret1=0, ins_flag=0; /* 0成功  1失败 */
	char cReturn[32];

	char cAc_no[20];
	char cUnit_no[5];
	char cUnit_no_tmp[5];
	char cRemark[17];
	char wherelist[61];
	
	char cHand_flag[2];		/*---信息录入方式---*/
	double dT_amt = 0.00;	/*---前台输入总金额---*/
	long lCount_i = 0;		/*---前台输入总笔数---*/

	long lBeg_date = 0;
	long lEnd_date = 0;

	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;
	double ds_amt = 0.00;
	double dd_bal = 0.00;
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
	char filename1[248];
	char filename2[248];
	char filename3[248];
	FILE *fp=NULL;
	FILE *fp1=NULL;
	FILE *fp2=NULL;
	FILE *fp3=NULL;

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
	memset(cAc_no,0x0,sizeof(cAc_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cRemark,0x0,sizeof(cRemark));
	memset(cReturn,0x0,sizeof(cReturn));
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );
	/****/
	memset(cUnit_no_tmp,0x0,sizeof(cUnit_no_tmp));
	
	memset(wherelist,0x00,sizeof(wherelist));
	
	/*add by kevinchan 20120311 start*/
	struct ag_peritemmap_c sAg_peritemmap1;
	int daliCnt=0;
	char newAcno[25];
	/*add by kevinchan 20120311 end*/

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
	sprintf( acErrMsg, "单位编号[%s]",cAc_no);
	WRITEMSG

	sprintf( acErrMsg, "取得的批处理日期为[%ld][%ld]", lBeg_date, lEnd_date );
	WRITEMSG

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	memcpy(cUnit_no,cUnit_no_tmp,sizeof(cUnit_no)-1);
	sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG
	sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG
	memset(filename,0x0,sizeof(filename));
	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );
		sprintf( acErrMsg, "单位编号[%s]",cUnit_no);
	WRITEMSG
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
	fp2 = fopen( filename2,"w" );	/*该文件用来记录正确处理结果*/
	if( fp2==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	fprintf( fp2, "\r\n\r\n\r\n                         [%s-%s]批量导入失败记录[%s-%ld]\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date);
	fprintf( fp2, "┍━━━━┯━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━┯━━━━━━━━━━━━━━━┑\r\n" );
		/**modify by kevinchan 20120326 start**/
		/**改变打印文件内容，方便行里给单位数据**/
		fprintf( fp2, "│%8s│%10s│%20s│%16s│%6s│%30s│\r\n", "代理编号","客户姓名", "账号", "金额", "次数", "处理结果" );
		/*fprintf( fp2, "│%20s│%20s│%16s│%20s│%30s│\r\n", "代理编号", "客户姓名", "金额", "证件号码", "处理结果" );*/
		/****/
	fprintf( fp2, "┝━━━━┿━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━┯━━━━━━━━━━━━━━━┥\r\n" );

			memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
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
	else				/*---非手工方式文件第一行为汇总信息---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<8;i++ )
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
		/****如果文件中的日期与当天日期不一致，不允许通过   add at 20070517****/
		if(atol(fldstr[2])!=g_pub_tx.tx_date)
		{
			sprintf( acErrMsg, "处理时间[%s]信息不符", fldstr[2] );
			WRITEMSG
			strcpy( g_pub_tx.reply, "P096" );
			goto ErrExit;
		}
		/****end 20070517****/
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
		for( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		/*-	fldstr[0] 代理编号 ------------------------
			fldstr[1] 客户姓名
			fldstr[2] 开户金额
			fldstr[3] 帐号
			fldstr[4] 单位编号
			fldstr[5] 证件类型
			fldstr[6] 证件号码
			fldstr[7] 该账户当日批量代理业务处理的次数
		--------------------------------------------------*/
		/**modify by kevinchan 20120311 start**/
		/**excel代理编号不存在**/
		/**if ( strlen(fldstr[0]) == 0 || strlen(fldstr[1]) == 0 )**/
		if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		/**modify by kevinchan 20120311 start**/
		{
			continue;
		}

		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[2] );
		dTmp_amt = dTmp_amt + dTemp_amt;

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
vtcp_log("aaaaaaaaa[%s]\r\n",sAg_unitinfo.bank_acct_no);
  /**检测公司帐户的金额**/
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
	  if(sAg_unitinfo.opr_attri[0]=='1')
	  {
				if(sIn_mst.bal<dT_amt)
				{
					sprintf(acErrMsg,"账户余额不正常[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "T066" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					goto ErrExit;
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
					goto ErrExit;
				}
				if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
					|| sDd_mst.hold_sts[0] == '1' )	/*销户和完全冻结*/
				{
					sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					goto ErrExit;	/****/
				}
				if(sAg_unitinfo.opr_attri[0]=='1')
			{
				   if(sDd_mst.bal<dT_amt)
				  {
						sprintf(acErrMsg,"账户余额不正常[%s]", sAg_unitinfo.bank_acct_no);
						WRITEMSG
						strcpy( g_pub_tx.reply, "T066" );
						set_zd_data(DC_REPLY,g_pub_tx.reply);
						goto ErrExit;
					}
			 }
		}
  /**********************/
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open 代理 file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

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

		/**memset( tmpstr, 0x0, sizeof(tmpstr) ); **/
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		/*当户名和证件号码都为空*/
    /*modify by kevinchan 20120311*/
		/*if ( strlen(fldstr[0]) == 0 ||  strlen(fldstr[1]) == 0 )*/
	  if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		{
			continue;
		}
		
		/*----验证传过来的单位编码 和 excel的每条单位编码是否一致----*/
		/*if ( strncmp( cUnit_no, fldstr[4], 4 ) != 0 )
		{
				sprintf( cReturn, "excel单位编码[%s]，录入的单位编码[%s]",fldstr[4],cUnit_no);
				sprintf( acErrMsg, "excel单位编码[%s]，录入的单位编码[%s]",fldstr[4],cUnit_no);
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		*/
		/**add by kevinchan 20120328 start**/
		/**增加新旧账号转换**/
		memset(newAcno,0x0,sizeof(newAcno));
		strncpy(newAcno,fldstr[3],strlen(fldstr[3]));
		pub_base_old_acno(newAcno);
		/**add by kevinchan 20120328 end**/

		pub_base_ltrim_zero( fldstr[0] );
		if ( strlen(fldstr[0]) == 0 )
		{
			/*modify by kevinchan 20120311 start*/
			/*strcpy( fldstr[0], "0" );*/
			memset( &sAg_peritemmap1, 0x0, sizeof(sAg_peritemmap1) );
			/**update by kevinchan 20120328 start**/
			/**ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap1,"unit_no = '%s' and ac_no = '%s' and stat='0'",cUnit_no, fldstr[3]);*/
			ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap1,"unit_no = '%s' and ac_no = '%s' and stat='0'",cUnit_no, newAcno);
			/**update by kevinchan 20120328 start**/
			if(ret)
			{
						/**sprintf(acErrMsg, "查询Ag_peritemmap出错[%s][%s][%d]", cUnit_no, fldstr[3], ret );
						WRITEMSG
						goto ErrExit;**/
						if (memcmp(newAcno,"621",3) == 0)
						{
									sprintf( cReturn, "单位编号错误或者卡号不存在");
									sprintf( acErrMsg, "单位编号错误或者卡号不存在[%s][%s]",cUnit_no,fldstr[3]);
						}
						else
						{
						sprintf( cReturn, "单位编号或账号信息错误！");
						sprintf( acErrMsg, "单位编号或账号信息错误！[%s][%s]",cUnit_no,fldstr[3]);
						}
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
			}
			strcpy( fldstr[0],sAg_peritemmap1.item_no);
			/*modify by kevinchan 20120311 end*/
		}

		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		/*对代收处理 收水电费等add by ligl 2006-11-24 21:06*/
		if(sAg_unitinfo.opr_attri[0]=='0')
	{
			memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap));
			pub_base_old_acno(fldstr[3]);
			  ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and ac_no = '%s'",sAg_unitinfo.unit_no, fldstr[3]);
		if(ret==0)
		{
		  pub_base_strpack_all( sAg_peritemmap.name );
					pub_base_strpack_all( fldstr[1] );
					if ( strcmp( sAg_peritemmap.name, fldstr[1] ) )
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
				/* 添加代理户名和账户名称比对 wudawei 20140516  */
				if ( strcmp( sAg_peritemmap.name, sDd_mst.name ) )
				{
							sprintf( cReturn, "户名不符,账户名为[%s]", sDd_mst.name  );
							sprintf( acErrMsg, "户名不符,账户名为[%s]", sDd_mst.name  );
							WRITEMSG
							iProc_flag1 = 1;
							iErr_flag = 1;
							goto TrLog;
				}
				/*  add end wudawei 20140516 */
				pub_base_strpack( sDd_mst.ac_sts );
				if ( sDd_mst.ac_sts[0] == '*' )
				{
					strcpy( cReturn, "已销户" );
					sprintf( acErrMsg, "已销户[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					/*iK_flag = 1;*/
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				/* 代扣时候 加上状态和余额的判断 wudawei 20140226 */
				if ( sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "完全冻结" );
					sprintf( acErrMsg, "完全冻结[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					/*iK_flag = 1;*/
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if ( sDd_mst.hold_sts[0]=='2' )
				{
					strcpy( cReturn, "只进不出" );
					sprintf( acErrMsg, "只进不出[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					/*iK_flag = 1;*/
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
				ds_amt = atof(fldstr[2]);
				dd_bal = sDd_mst.bal;
				dd_bal = dd_bal - sDd_mst.ctl_amt ;
				if(sDd_mst.hold_sts[0]=='3')
				{
					dd_bal = dd_bal - sDd_mst.hold_amt;
				}
				if(ds_amt > dd_bal )
				{
					strcpy( cReturn, "账面可用余额不足" );
					sprintf( acErrMsg, "账面余额不足[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
        /*wudawei 20140226 end*/
		}
		else if(ret==100)
		{
			strcpy( cReturn, "代理户不存在" );
					sprintf(acErrMsg, "查询代收记录出错[%s][%s]", cUnit_no, fldstr[3]);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
		}
		else
		{
						sprintf(acErrMsg, "查询代发记录出错[%s][%s][%d]", cUnit_no, fldstr[3], ret );
						WRITEMSG
						goto ErrExit;
		}
	/*---------------批量导入数据部分代扣-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, fldstr[0] );
		strcpy(sPay_agent.name, fldstr[1] );
		strcpy(sPay_agent.id_type, fldstr[5] );
		strcpy(sPay_agent.id_no, fldstr[6] );
		strcpy(sPay_agent.ac_no, fldstr[3]);
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[2]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	/*-- 需要批量处理 --*/
		strcpy(sPay_agent.result,"1");	/*-- 未处理 --*/
		vtcp_log("[%s][%d],======fldstr[7]========[%s]\r\n",__FILE__,__LINE__,fldstr[7]);
		strcpy(sPay_agent.cnt,fldstr[7]);/*--代收付次数--*/
		vtcp_log("[%s][%d],======fldstr[7]========[%s]\r\n",__FILE__,__LINE__,fldstr[7]);
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
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,
			"unit_no = '%s' and item_no = '%s' and stat='0'",
			cUnit_no, fldstr[0]);
		if ( ret == 0 )
		{
			pub_base_strpack( sAg_peritemmap.id_no );
			pub_base_strpack_all( sAg_peritemmap.name );

			pub_base_strpack_all( fldstr[1] );

			if ( strcmp( sAg_peritemmap.name, fldstr[1] ) )
			{
				sprintf( cReturn, "户名不符,原户名为[%s]", sAg_peritemmap.name );
				sprintf( acErrMsg, "户名不符,原户名为[%s]", sAg_peritemmap.name );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			pub_base_strpack( sAg_peritemmap.ac_no );
			pub_base_old_acno( sAg_peritemmap.ac_no );
			vtcp_log("%s,%d sAg_peritemmap.ac_no =======[%s]",__FILE__,__LINE__,sAg_peritemmap.ac_no);
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
				/**update by kevinchan 20120328 start**/
				/**商行业务要求录入时把异常数据过滤出来**/
				pub_base_strpack( sMdm_ac_rel.note_sts );
				vtcp_log("%s,%d sMdm_ac_rel.note_sts =======[%s]",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
				if(sMdm_ac_rel.note_sts[0] !='0')
				{
						sprintf( cReturn, "介质状态异常！" );
						sprintf( acErrMsg, "介质状态异常！" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
			  /**update by kevinchan 20120328 end**/
			
			if ( strcmp(sAg_peritemmap.id_no, fldstr[6]) &&
				strcmp( sAg_peritemmap.name, fldstr[1]) &&
				sAg_unitinfo.opr_attri[0] == '1' &&
				sAg_unitinfo.auto_flag[0] == '1') /*-证件号码和户名不同,代付-*/
			{
				iLen = 0;
			  ins_flag=1;
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


			/** modify by wudawei 20130531
			if ( sAg_unitinfo.opr_attri[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" )
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )
			{**/
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
				/**update by kevinchan 20120328 start**/
				/**商行业务要求录入时把异常数据过滤出来**/
				pub_base_strpack( sMdm_ac_rel.note_sts );
				if(sMdm_ac_rel.note_sts[0] !='0')
				{
						sprintf( cReturn, "介质状态异常！" );
						sprintf( acErrMsg, "介质状态异常！" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
			  /**update by kevinchan 20120328 end**/

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
				/* 添加代理户名和账户名称比对 wudawei 20140616  */
				if ( strcmp( sAg_peritemmap.name, sDd_mst.name ) )
				{
							sprintf( cReturn, "户名不符,账户名为[%s]", sDd_mst.name  );
							sprintf( acErrMsg, "户名不符,账户名为[%s]", sDd_mst.name  );
							WRITEMSG
							iProc_flag1 = 1;
							iErr_flag = 1;
							goto TrLog;
				}
				/*  add end wudawei 20140616 */
				pub_base_strpack( sDd_mst.ac_sts );
				/**update by kevinchan 20120328 start**/
				/**商行业务要求录入时把异常数据过滤出来**/
				if(sDd_mst.ac_sts[0] !='1')
				{
						sprintf( cReturn, "账号状态异常！" );
						sprintf( acErrMsg, "账号状态异常！" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
				pub_base_strpack( sDd_mst.hold_sts);
				if(sDd_mst.hold_sts[0] == '1')
				{
						sprintf( cReturn, "冻结状态异常！" );
						sprintf( acErrMsg, "冻结状态异常！" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
				/*
				if ( sDd_mst.ac_sts[0] == '*'||sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "已销户,完全冻结" );
					sprintf( acErrMsg, "已销户,新开户[%s]", sAg_peritemmap.item_no );
					WRITEMSG

					iLen = 0;
					iLen = strlen( fldstr[6] );
					if ( iLen == 0 && sDd_mst.ac_sts[0] == '*' )
					{
						sprintf( cReturn, "销户新开,证件号码不能为空" );
						sprintf( acErrMsg, "销户新开,证件号码不能为空" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
					}

					iK_flag = 1;
					iProc_flag = 1;
				}*/
			  /**update by kevinchan 20120328 end**/
				if(sDd_mst.sttl_type[0]=='S')
				{
					strcpy( cReturn, "该户为非结算户,不能转帐" );
					sprintf( acErrMsg, "该户为非结算户,不能转帐[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				/**add by kevinchan 20120326 start**/
				/**增加对客户代理次数检查**/
			  /**update by kevinchan 20120328 start**/
			  /*daliCnt = sql_count("pay_agent","ac_no='%s' and unit_no='%s'",fldstr[3],cUnit_no);*/
				daliCnt = sql_count("pay_agent","ac_no='%s' and unit_no='%s'",newAcno,cUnit_no);
				/**update by kevinchan 20120328 end**/
				if( daliCnt<0 )
				{
					sprintf(acErrMsg,"sql_count error!! [%d]",daliCnt);
					WRITEMSG
					goto ErrExit;
				}

				if((daliCnt+1)!=atoi(fldstr[7]))
				{
						sprintf( cReturn, "代理次数错误！应该为[%d]",daliCnt+1);
						sprintf( acErrMsg, "代理次数错误！[%s][%s]",cUnit_no,fldstr[3]);
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
				/**add by kevinchan 20120326 end**/
				
			/**modify by wudawei 20130531
			}
			**/
		}
		else if ( ret == 100 )
		{
				strcpy( cReturn, "代理编号不存在" );
				sprintf(acErrMsg, "查询代发记录出错[%s][%s]", cUnit_no, fldstr[0]);
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			/*
			iLen = 0;
			iLen = strlen( fldstr[1] );
			if ( iLen == 0 )
			{
				sprintf( cReturn, "新开户,户名不能为空" );
				sprintf( acErrMsg, "新开户,户名不能为空" );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}

			iLen = 0;
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

			if ( sAg_unitinfo.opr_attri[0] == '1' && sAg_unitinfo.auto_flag[0] == '1'
				&& strcmp(sAg_unitinfo.pay_type, "37" ) && strcmp(sAg_unitinfo.pay_type, "38" )
				&& strcmp(sAg_unitinfo.pay_type, "39" ) )--- 代付 ---
			{
				strcpy( cReturn, "新开户" );
				sprintf(acErrMsg,"新开户[%s]", sAg_peritemmap.item_no);
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
			}
			else
			{
				strcpy( cReturn, "代理编号不存在" );
				sprintf(acErrMsg, "查询代发记录出错[%s][%s]", cUnit_no, fldstr[0]);
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			*/
		}
		else
		{
			sprintf(acErrMsg, "查询代发记录出错[%s][%s][%d]", cUnit_no, fldstr[0], ret );
			WRITEMSG
			goto ErrExit;
		}

		/*---------------批量导入数据部分-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));

		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, fldstr[0] );
		strcpy(sPay_agent.name, fldstr[1] );
		strcpy(sPay_agent.id_type, fldstr[5] );
		/**modify by kevinchan 20120320 start**/
		/**批量导入工资数据时，插入查询出来的证件号码**/
		/*strcpy(sPay_agent.id_no, fldstr[6] );*/
		strcpy(sPay_agent.id_no, sAg_peritemmap.id_no);
		/**modify by kevinchan 20120320 end**/
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[2]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	/*-- 需要批量处理 --*/
		strcpy(sPay_agent.result,"1");	/*-- 未处理 --*/
		vtcp_log("[%s][%d],======fldstr[7]========[%s]\r\n",__FILE__,__LINE__,fldstr[7]);
		strcpy(sPay_agent.cnt,fldstr[7]);/*--代收付次数--*/
		vtcp_log("[%s][%d],======fldstr[7]========[%s]\r\n",__FILE__,__LINE__,fldstr[7]);
		if ( sAg_unitinfo.opr_attri[0] == '0' )	/*代扣处理20050902*/
		{
			ret = sql_count( "pay_agent", "unit_no = '%s' and item_no = '%s'and cnt='%s' \
				and end_date >= %ld", cUnit_no, fldstr[0],fldstr[7], g_pub_tx.tx_date );
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
			dSamt = dSamt + atof(fldstr[2]);
			fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│\r\n",fldstr[0], fldstr[1],
					fldstr[2], fldstr[6], sAg_peritemmap.ac_no, cReturn );

			if ( iK_flag == 1 )
			{
				lK_num = lK_num + 1;	/*新开户数*/
				dK_amt = dK_amt + atof(fldstr[2]);	/*新开金额*/
			}
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[2]);
			/**modify by kevinchan 20120326 start**/
			/*fprintf( fp2, "│%20s│%20s│%16s│%20s│%30s│\r\n",fldstr[0], fldstr[1],
					fldstr[2], fldstr[6], cReturn );*/
			fprintf( fp2, "│%8s│%10s│%20s│%16s│%6s│%30s│\r\n",fldstr[0],fldstr[1], fldstr[3],
					fldstr[2], fldstr[7], cReturn );
			/**modify by kevinchan 20120326 start**/
		}
	}
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

	memset( filename3, 0x0, sizeof(filename3) );
	pub_base_AllDwnFilName( filename3 );			/*  下传文件名  */

	memset( execstr, 0x0, sizeof(execstr) );
	if ( iErr_flag == 0 )
	{
		sprintf( acErrMsg, "新开[%ld],日期[%s]", lK_num, cDate );
		sprintf( execstr, "cp %s %s", filename1, filename3 );
	}
	else
	{
		sprintf(acErrMsg,"编号[%s],户名[%.6s],金额[%.2f]",fldstr[0],fldstr[1],atof(fldstr[2]));
		sprintf( execstr, "cp %s %s", filename2, filename3 );
	}

	/**add by kevinchan 20120313 start**/
  /**给前台返回代发信息**/
  sprintf(wherelist,"总笔数[%ld]成功笔数[%ld]金额[%.2f];失败笔数[%ld]",lCount, lSnum,dSamt,lFnum);	
	set_zd_data("0270", wherelist);
	/**add by kevinchan 20120313 end**/
  set_zd_data( "0130", acErrMsg );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"更改下传文件名失败[%s][%s][%d][%s]", filename3, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		ins_flag=1;
		goto ErrExit;
	}

	set_zd_data(DC_FILE_SND,"1");

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
		/****不判断代付还是代收，所以有错误情况都走进来  modify at 20130403
		if(sAg_unitinfo.opr_attri[0]=='1' && ins_flag==0  && cHand_flag[0] != '0'){
		****/
		if( ins_flag==0  && cHand_flag[0] != '0'){
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
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"批量代理业务处理完毕！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	sprintf(acErrMsg,"%s,%d,导入数据失败ins[%d],iErr[%d]",__FILE__,__LINE__,ins_flag,iErr_flag);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
