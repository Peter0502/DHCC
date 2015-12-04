/*************************************************
* 文 件 名: spF610.c
* 功能描述：批量开户交易
*
* 作    者: jack
* 完成日期: 2005年02月24日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*--- 证件类型默认为1 -现改为可以任何证件类型fldstr[5]-*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_addr_inf_c.h"    /****加入客户地址表****/
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "note_min_no_c.h"
#include "com_branch_c.h"

spF610()
{ 	
	
	int ret=0;
	int dRet=0;
	char execstr[256];

	char cPrdt_code[4];
	char cBeg_card_no[20];
	char cEnd_card_no[20];
	char cCard_no[20];
	char cNote_no[20];
	char cz_acno[20];
	char cUnit_no[5];
	char cNote_type[4];
	
	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;	
	
	double dTram = 0.00;

	char cPass[7];
	long lLen = 0;

	char cErrname[51];  /****当检查户名时，将错误户名和正确户名放在此变量中，返回给前台 20070627****/

	char filename[100];
	char filename1[100];
	char fldstr[10][100];
	char tmpstr[1024];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	int i=0;

	char filename2[100];
	FILE *fp2 = NULL;
	char filename3[100];
	FILE *fp3 = NULL;	
	char filename4[100];	/*将正确开户结果备份*/
	
	long lSnum = 0;
	double dSamt = 0.00;
	
	int iErr_flag = 0;	/*---标志该批记录中是否有错误出现---*/
	char cReturn[64];

	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	long lBeg_note_no = 0;
	char cNote_tmp[16];
	char cNote_tmp1[9];
	int iLen = 0;

	long lCount = 0;
	double dBd_amt=0.00;
	
	char cHand_flag[2];	/*---信息录入方式---*/
	double dT_amt = 0.00;	/*---前台输入总金额---*/
	long lCount_i = 0;	/*---前台输入总笔数---*/
	long lRnum = 0;

	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct cif_addr_inf_c cif_addr_inf;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct note_min_no_c sNote_min_no;
	struct com_branch_c sCom_branch;
	struct cif_basic_inf_c t_cif_basic_inf;
	memset(&cif_addr_inf,0x0,sizeof(cif_addr_inf));

	memset(cPrdt_code,0x0,sizeof(cPrdt_code));
	memset(cBeg_card_no,0x0,sizeof(cBeg_card_no));
	memset(cEnd_card_no,0x0,sizeof(cEnd_card_no));
	memset(cCard_no,0x0,sizeof(cCard_no));
	memset(cNote_no,0x0,sizeof(cNote_no));
	memset(cz_acno,0x0,sizeof(cz_acno));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cNote_type,0x0,sizeof(cNote_type));
	memset( cReturn, 0x0, sizeof(cReturn) );
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );

	memset( cPass, 0x0, sizeof(cPass) );

	get_zd_data("0230",cPrdt_code);
	get_zd_data("0300",cz_acno);
	pub_base_old_acno( cz_acno );

	get_zd_data("0920",cUnit_no);
	get_zd_data("0900",cNote_type);
	
	get_zd_data( "0700", cHand_flag );

	get_zd_data( "0790", cPass );
	lLen = strlen( cPass );
	if ( lLen == 0 )
	{
		sprintf( acErrMsg, "密码不能为空!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( "0130", "密码不能为空!!!" );
		goto ErrExit;
	}

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "010") ) /* 存折 */
	{
		strcpy( cRctp, "存折" );
	}
	if ( !strcmp(cNote_type, "011") ) /* 储蓄零整存折 */
	{
		strcpy( cRctp, "零整存折" );
	}
	if ( !strcmp(cNote_type, "012") ) /* 贺卡定期 */
	{
		strcpy( cRctp, "贺卡存单" );
	}
	if ( !strcmp(cNote_type, "013") ) /* 定期存单 */
	{
		strcpy( cRctp, "定期存单" );
	}
	if ( !strcmp(cNote_type, "015") ) /* 定活存单 */
	{
		strcpy( cRctp, "定活存单" );
	}
	if ( !strcmp(cNote_type, "016") ) /* 定期一本通 */
	{
		strcpy( cRctp, "一本通" );
	}
	if ( !strcmp(cNote_type, "020") ) /* 卡 */
	{
		strcpy( cRctp, "卡" );
	}

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该单位编号对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	pub_base_strpack( sAg_unitinfo.unit_name );
	
	memset( filename1, 0x0, sizeof(filename1) );
	/**sprintf( filename1,"%s/0%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/ /**Del by hxc 100529 文件名添加6位流水号**/
    	sprintf( filename1,"%s/0%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
    	fp1 = fopen( filename1,"w" );   /*该文件用来记录开户正确处理结果*/
    	if( fp1==NULL )
    	{
        	sprintf(acErrMsg," open file [%s] error ",filename1 );
        	WRITEMSG
        	strcpy( g_pub_tx.reply,"S047" );
        	goto ErrExit;
    	}    	
    	
    	fprintf( fp1, "                         批量开户成功结果信息表[%s-%s]-[%s-%ld]\n", \
    		sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
    	fprintf( fp1, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━┯━━━━━━━━┑\n" );
    	fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│%16s│\n", "代理编号", "客户姓名", "开户金额", "证件号码", "账号", "凭证种类", "凭证号" );
    	fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━┿━━━━━━━━┥\n" );
    	
    	memset( filename2, 0x0, sizeof(filename2) );
    	/**sprintf( filename2,"%s/1%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/ /**Del by hxc 100529 文件名添加6位流水号**/
    	sprintf( filename2,"%s/1%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
    	fp2 = fopen( filename2,"w" );   /*该文件用来记录开户错误处理结果*/
    	if( fp2==NULL )
    	{
        	sprintf( acErrMsg, " open file [%s] error ", filename2 );
        	WRITEMSG
        	strcpy( g_pub_tx.reply,"S047" );
        	goto ErrExit;
    	}
    	
	fprintf( fp2, "                         批量开户错误结果信息表[%s-%s]-[%s-%ld]\n", \
    		sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
    	fprintf( fp2, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━┑\n" );
    	fprintf( fp2, "│%20s│%20s│%16s│%20s│%44s│\n", "代理编号", "客户姓名", "开户金额", "证件号码", "错误信息" );
    	fprintf( fp2, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━┥\n" );


	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );
	/*strcat(filename,"_610");*/

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
			sprintf( acErrMsg, "代理单位处理[%s]信息不符", fldstr[3] );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "H035" );
        		goto ErrExit;
		}
		
		dT_amt = atof( fldstr[7] );
		lCount_i = atoi(fldstr[6] );
	
	}
	
	/*------ 判断柜员是否拥有满足条件的凭证 -----*/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}
		
		memset(fldstr,0x0,sizeof(fldstr));
		/****现在是7项，此处应该改为7 ****/
		for( i=0;i<7;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		
		/*-	fldstr[0] 代理编号 ------------------------
			fldstr[1] 客户姓名
			fldstr[2] 开户金额
			fldstr[3] 身份证号码
		--------------------------------------------------*/
		vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[5]);
		if ( strlen(fldstr[1]) == 0 || strlen(fldstr[3]) == 0 )
		{
			continue;	/*其中任何一项为空认为该记录无效*/
		}
		
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[2] );
		dTmp_amt = dTmp_amt + dTemp_amt;

		lCount = lCount + 1;
	}
	fclose ( fp );
	
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"总金额与分户交易金额合计不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"明细余额为[%.2lf]", dTmp_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"总笔数与分户明细合计笔数不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"明细笔数为[%d]", lCount );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}

	if ( strcmp( cPrdt_code, "102" ) != 0 )		/*102产品为无介质户*/
	{
		ret = sql_sum_long ( "note_mst", "cnt", &lRnum, "tel = '%s' \
					and note_type = '%s' and sts = '0'", \
					g_pub_tx.tel, cNote_type );
		if ( lRnum < lCount )
		{
			sprintf(acErrMsg, "开户户数[%d]凭证数[%d]", lCount, lRnum );
			WRITEMSG
        	
			sprintf(acErrMsg, "柜员可用凭证不足这次批量开户!!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H018" );
			goto ErrExit;
		}
        	
		sprintf(acErrMsg, "开户户数[%d]凭证数[%d]", lCount, lRnum );
		WRITEMSG
	}

	if ( lCount == 0 )
	{
		sprintf(acErrMsg,"没有需要批量开户的记录!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H017" );
		goto ErrExit;
	}

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	if ( cHand_flag[0] != '0' )	/*---非手工去掉第一行---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}

	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<7;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );

				vtcp_log("[%s][%d]循环i[%s]\n",__FILE__,__LINE__,fldstr[i]);
		}
		/*-	fldstr[0] 代理编号 ----------------------------
			fldstr[1] 客户姓名
			fldstr[2] 开户金额
			fldstr[3] 身份证号码
			fldstr[4] 单位编号
			fldstr[5] 证件类型
		--------------------------------------------------*/
				vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[5]);
		pub_base_ltrim_zero( fldstr[0] );	/*---去掉个人编号左边的零---*/
		if ( strlen(fldstr[0]) == 0 )
		{
			strcpy( fldstr[0], "0" );
		}
		
		/*--------当代理编号、客户姓名、身份证号码为空时继续循环--------*/
		if ( strlen(fldstr[1]) == 0 || strlen(fldstr[3]) == 0 )
		{
			continue;	/*其中任何一项为空认为该记录无效*/
		}
				
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		dTx_amt = 0.00;
		dTx_amt = atof(fldstr[2]);

		pub_base_strpack_all( fldstr[1] );
    		vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[5]);

		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			/*----同一编号对应下的身份证号码已经存在则不允许开户----*/
			pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "已对应账号[%s]不能重新开户", sAg_peritemmap.ac_no );
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "已对应账号[%s]不能重新开户", sAg_peritemmap.ac_no );
				WRITEMSG
				goto TrLog;
			}
			/*---------------------------------------------*/
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"查询ag_peritemmap出错[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		/*-------------暂时封-----------------------------
		if ( sAg_unitinfo.opr_attri[0] == '0' )
		{
			ret = pub_base_CompDblVal( dTx_amt, 0.00 );
			if ( ret != 0 )
			{
				sprintf( acErrMsg,"代收账户必须零金额开户!!!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"H016");
				goto ErrExit;
			}
		}
		-----------------------------------------------*/

		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
			memset(&t_cif_basic_inf, 0x0, sizeof(t_cif_basic_inf));
     /**modified by ligl 2006-10-13 10:42**/
		strcpy(cif_id_code_rel.id_type, fldstr[5]);/* 证件类型 */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* 证件号码 */
    		vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[5]);

		strcpy(cif_basic_inf.type,"1");		      /* 客户类型 */
		strcpy(cif_basic_inf.name,fldstr[1]);	  /* 客户名称 */
		strcpy(cif_basic_inf.local,"1");	/* 本地标志 */
		strcpy(cif_basic_inf.poten,"1");	/* 潜在标志1-本行2-潜在 */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*是否本行股东2-是 1-否 */
		strcpy(cif_basic_inf.lvl,"4"); /*客户级别*/

		/*--------开始建立客户信息----------*/
		cif_no = 0;

		/* 检查客户证件类型＋证件号码是否已经存在 */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , cif_id_code_rel.id_type ,
					  cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"该客户已经拥有客户ID![%s][%s]",fldstr[5],fldstr[3]);
			WRITEMSG
			/****增加对户名的判断****/
			dRet = Cif_basic_inf_Sel(g_pub_tx.reply, &t_cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(dRet == 0)
			{
				pub_base_strpack(fldstr[1]);
				pub_base_strpack(t_cif_basic_inf.name);				
				if(strcmp(t_cif_basic_inf.name,fldstr[1]) != 0)
				{
					/****此处用cErrname变量，将下面的错误信息赋予13域返回给前台 20070627****/
					sprintf(cErrname,"数据库中为 [%s] 文件中为 [%s]\n", t_cif_basic_inf.name,fldstr[1]);
					WRITEMSG
					set_zd_data(DC_GET_MSG,cErrname);
					strcpy(g_pub_tx.reply,"C086");
					goto ErrExit;
				}
				else
				{
					goto OK_cif_no;	/*直接开户*/
				}
			}
			else if( dRet )
			{
				sprintf(acErrMsg,"取客户信息异常![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*对已存在的客户号赋值*/
			/****/
			/*g_pub_tx.cif_no=cif_no;*/
			/*******/
			goto OK_cif_no;	/*直接开户*/
		}
		else if( ret!=100 )
		{
			sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
			goto ErrExit;
		}
	
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"生成客户id失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
	
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	

		/* 登记客户证件与客户号对照表 */
		ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"登记客户证件与客户号对照表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C004");
			goto ErrExit;
		}
	
		/* 登记客户基本信息表 */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* 建立日期 */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* 建立时间 */
		strcpy( cif_basic_inf.sts , "1" ) ;         /* 客户状态：1正常 2注销 */
		cif_basic_inf.cif_seqn = 1 ;                /* 客户序号 */

		ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		/****/
		/*g_pub_tx.cif_no=cif_no;*/
		/*******/
				vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[5]);

		sprintf(acErrMsg,"建立客户信息成功,客户号[%ld]",cif_no);/*生成客户号*/
		WRITEMSG

		/****导盘开户时,增加插入客户表的信息,地址字段插入地址或劳保号  add by wanglei 20070618****/

		vtcp_log("[%s][%d]客户地址或劳保号 [%s]\n",__FILE__,__LINE__,fldstr[6]);
		if ( cHand_flag[0] != '0' )	/*---导盘---*/
		{
			vtcp_log("[%s][%d]客户地址或劳保号 [%s]\n",__FILE__,__LINE__,fldstr[6]);
		
			cif_addr_inf.cif_no       = cif_no; /* 客户号 */
			cif_addr_inf.addr_seqn    = 1;      /* 地址序号 */
			cif_addr_inf.addr_type[0] = '2';    /* 地址类型 1-单位 2-个人 3-其他 */
			strcpy(cif_addr_inf.country, "01");   /* 国别 中国 */
			strcpy(cif_addr_inf.addr,fldstr[6] );    /* 地址或劳保号 */
			vtcp_log("[%s][%d]客户地址或劳保号 [%s] addr = [%s]\n",__FILE__,__LINE__,fldstr[6],cif_addr_inf.addr);

			ret = Cif_addr_inf_Ins( cif_addr_inf,g_pub_tx.reply );
			if( ret )
			{
				sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]客户地址或劳保号 [%s] addr = [%s]\n",__FILE__,__LINE__,fldstr[6],cif_addr_inf.addr);
		}
		/****ended  20070618****/

OK_cif_no:
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		ret = Note_min_no_Sel( g_pub_tx.reply, &sNote_min_no, \
			"br_no = '%s' and tel = '%s' and note_type = '%s'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type );
		if( ret==100 )
		{
			ret = sql_min_string("note_mst","beg_note_no" ,
				sNote_min_no.note_min_no , 16, 
				"br_no='%s' and tel='%s' and note_type='%s' and sts='0'",
				g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type ) ;
			if( ret )
			{
				sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
			sprintf( acErrMsg,"凭证[%s]",sNote_min_no.note_min_no );
			WRITEMSG
			pub_base_strpack( sNote_min_no.note_min_no );
			if( strlen(sNote_min_no.note_min_no)==0 )
			{
				sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
			strcpy( sNote_min_no.br_no,g_pub_tx.tx_br_no );
			strcpy( sNote_min_no.tel,g_pub_tx.tel );
			strcpy( sNote_min_no.note_type,cNote_type );
			
			ret=Note_min_no_Ins( sNote_min_no,g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
		}
		else if( ret==0 )
		{
			pub_base_strpack( sNote_min_no.note_min_no );
			if( strlen(sNote_min_no.note_min_no)==0 )
			{
				pub_base_strpack( sNote_min_no.note_min_no );
				ret = sql_min_string("note_mst","beg_note_no" ,
					sNote_min_no.note_min_no , 16, 
					"br_no='%s' and tel='%s' and note_type='%s' and sts='0'",
					g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type ) ;
				if( ret )
				{
					sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
				sprintf( acErrMsg,"凭证[%s]",sNote_min_no.note_min_no );
				WRITEMSG
				pub_base_strpack( sNote_min_no.note_min_no );
				if( strlen(sNote_min_no.note_min_no)==0 )
				{
					sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
				strcpy( sNote_min_no.br_no,g_pub_tx.tx_br_no );
				strcpy( sNote_min_no.tel,g_pub_tx.tel );
				strcpy( sNote_min_no.note_type,cNote_type );

				ret=sql_execute( "update note_min_no set note_min_no='%s' where br_no='%s' and tel='%s' and note_type='%s'",
					sNote_min_no.note_min_no, g_pub_tx.tx_br_no, 
					g_pub_tx.tel, cNote_type );
				if( ret )
				{
					sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
			}
		}
		else if( ret )
		{
			sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if ( !strcmp(cNote_type, "010") ) /* 存折 */
		{
			strcpy(g_pub_tx.mdm_code,"0010");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "011") ) /* 储蓄零整存折 */
		{
			strcpy(g_pub_tx.mdm_code,"0011");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "012") ) /* 贺卡存单 */
		{
			strcpy(g_pub_tx.mdm_code,"0012");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "013") ) /* 储蓄整整定期存单 */
		{
			strcpy(g_pub_tx.mdm_code,"0013");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "015") ) /* 储蓄个人定活存单 */
		{
			strcpy(g_pub_tx.mdm_code,"0015");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "016") ) /* 定期一本通 */
		{
			strcpy(g_pub_tx.mdm_code,"0016");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "020") ) /* 卡 */
		{
			strcpy(g_pub_tx.mdm_code,"0020");	
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );

			memset( cCard_no, 0x0, sizeof(cCard_no) );
			memset( cNote_no, 0x0, sizeof(cNote_no) );
			memcpy( cNote_no, sNote_min_no.note_min_no+9, 7 );
			
			sprintf( acErrMsg,"-------->生成的卡号的凭证号为[%s]", cNote_no );
        		WRITEMSG

			strcpy( cCard_no, "94007173308" );
			strcat( cCard_no, cNote_no );
			strcat( cCard_no, "0" );
			
			if( pub_card_par_1212( 1, cCard_no, g_pub_tx.crd_no ) )
    			{
        			sprintf( acErrMsg,"生成卡号[%s]校验位错误", cCard_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply,"F005" );
        			goto ErrExit;
    			}
    			
    			sprintf( acErrMsg, "-------->生成的卡号为[%s]", g_pub_tx.crd_no );
        		WRITEMSG
		}
		
		if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102产品为无介质户*/
		{
			strcpy( g_pub_tx.mdm_code, "0000" );
			memset( g_pub_tx.beg_note_no, 0x0, sizeof(g_pub_tx.beg_note_no) );
			strcpy( g_pub_tx.ac_wrk_ind, "0" );	/*--不需要做凭证销号处理--*/
		}

		strcpy(g_pub_tx.id_type, fldstr[5]);	
		strcpy(g_pub_tx.id_no,fldstr[3]);
		strcpy(g_pub_tx.name,fldstr[1]);

		strcpy(g_pub_tx.draw_uncon_yn,"N");
		strcpy(g_pub_tx.draw_pwd_yn,"Y");
		strcpy(g_pub_tx.qry_pwd, cPass );
		strcpy(g_pub_tx.draw_pwd, cPass );
		strcpy(g_pub_tx.draw_id_yn,"N");
		strcpy(g_pub_tx.draw_seal_yn,"N");

		strcpy(g_pub_tx.brf,"开介质");

		pub_base_strpack(g_pub_tx.crd_no);
		pub_base_strpack(g_pub_tx.beg_note_no);
		
		memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D002" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

		ret = pub_acct_opn_cif_mdm();
		if( ret )
		{
			sprintf(acErrMsg,"开介质错!!![%d][%s][%s][%s]",ret, fldstr[5], fldstr[3],fldstr[1]);
			WRITEMSG
			goto ErrExit;
		}
		memset( xs_acno,0x0,sizeof(xs_acno) );
		strcpy(xs_acno,g_pub_tx.ac_no);	/*生成显示账号*/
		vtcp_log("显示账号为[%s]",g_pub_tx.ac_no);


/*---------------------开账号---------------------*/
    		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

		strcpy(g_pub_tx.ac_no,xs_acno);
		strcpy(g_pub_tx.prdt_code,cPrdt_code);
		strcpy(g_pub_tx.cur_no,"01");
		strcpy(g_pub_tx.ac_type,"D");
		strcpy(g_pub_tx.intst_type,"");
		g_pub_tx.tx_amt1 = dTx_amt;		/*发生额*/
		strcpy(g_pub_tx.name,fldstr[1]);
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.hst_ind,"1");	/*日间入明细*/
		strcpy(g_pub_tx.brf,"批量开户");
		strcpy( g_pub_tx.ac_wrk_ind, "001" );		
		
		if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102产品为无介质户*/
		{
			strcpy( g_pub_tx.mdm_code, "0000" );
		}
		
		memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D002" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

		ret =  pub_acct_opn_ac();
		if (ret != 0)
		{
			sprintf(acErrMsg,"开户错误[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data( "1033", "01" );
		set_zd_data( "103E", "2" );
		set_zd_double( "1037", dTx_amt );
		dBd_amt+=dTx_amt;
		strcpy( g_pub_tx.brf, "批量开户" );
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"记活期开户会计账出错[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}

		memset( jz_acno, 0x0, sizeof(jz_acno) );
		jz_ac_seq = 0;
		sprintf(jz_acno,"%d",g_pub_tx.ac_id);
		jz_ac_seq = g_pub_tx.ac_seqn;

		sprintf(acErrMsg,"生成的记帐账号和账号序号分别为[%s] [%d]",jz_acno,g_pub_tx.ac_seqn);
		WRITEMSG

		pub_base_strpack( sAg_unitinfo.pay_type );
		if ( strncmp( sAg_unitinfo.pay_type, "31", 2 ) == 0  \
			&& strcmp( fldstr[0], "财政工资新开户" ) == 0 )
		{
			strcpy( fldstr[0], xs_acno );
		}
		pub_base_strpack( fldstr[0] );

		/*--------------- 写代理对应记录 ---------------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

		if ( sAg_unitinfo.opr_attri[0] == '0' )	/* 代收 */
		{
			strcpy( sAg_peritemmap.bat_flag, "0" );		/*-- 是否默认批量代扣需要确认??? --*/
			strcpy( sAg_peritemmap.remark, "代理缴费" );
		}
		if ( sAg_unitinfo.opr_attri[0] == '1' )	/* 代付 */
		{
			strcpy( sAg_peritemmap.bat_flag, "1" );
			strcpy( sAg_peritemmap.remark, "代理付费" );
		}

		strcpy( sAg_peritemmap.unit_no, cUnit_no );
		strcpy( sAg_peritemmap.item_no, fldstr[0] );
		strcpy( sAg_peritemmap.name, fldstr[1] );
		strcpy( sAg_peritemmap.id_type, fldstr[5] );
		strcpy( sAg_peritemmap.id_no, fldstr[3] );
		strcpy( sAg_peritemmap.ac_no, xs_acno );
		sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.stat, "0" );
		sAg_peritemmap.opn_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
		sAg_peritemmap.cls_date = 99999999;
		strcpy( sAg_peritemmap.cls_tel, "0000" );

		ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
		if (ret == -268 )	/*--- 有重复记录 ---*/
		{
			ret = sql_execute( "update ag_peritemmap set name = '%s', \
				id_no = '%s', ac_no = '%s',opn_date = %ld, opn_tel= '%s'\
				where unit_no = '%s' and item_no = '%s'", \
				fldstr[1], fldstr[3], xs_acno,g_pub_tx.tx_date, \
				g_pub_tx.tel, cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf(acErrMsg,"修改ag_peritemmap失败[%s][%d]!!!",fldstr[0],ret );
				WRITEMSG
				strcpy( g_pub_tx.reply, "P090" );
				goto ErrExit;
			}
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"写记录入Ag_peritemmap失败!!!!");
			WRITEMSG
			goto ErrExit;
		}

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		g_pub_tx.tx_amt1 = 0.00;		/*发生额*/

TrLog:		
		dTram = atof( fldstr[2] );

		if ( ret == 0 )
		{
			if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102产品为无介质户*/
			{				
				memset( sNote_min_no.note_min_no, 0x0, sizeof(sNote_min_no.note_min_no) );
				memset( cRctp, 0x0, sizeof(cRctp) );
				strcpy( cRctp, "无介质" );
			}
			fprintf( fp1, "│%20s│%20s│%16.2f│%20s│%20s│%8s│%16s│\n", fldstr[0], fldstr[1], \
				dTram, fldstr[3], xs_acno, cRctp, sNote_min_no.note_min_no );
				
			lSnum = lSnum + 1;
			dSamt = dSamt + dTram;
		}
		else
		{
			fprintf( fp2, "│%20s│%20s│%16.2f│%20s│%44s│\n", fldstr[0], fldstr[1], \
				dTram, fldstr[3], cReturn );
		}
	}
	
	fprintf( fp1, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━┷━━━━━━━━┙\n" );
	fprintf( fp2, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━┙\n" );

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	ret = pub_base_CompDblVal( dT_amt, dBd_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"总金额与分户交易金额合计不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"明细余额为[%.2lf]", dBd_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	if ( iErr_flag == 0 )	/*如果无错误记录则记单位账*/
	{ 
		sprintf(acErrMsg,"-----------------开始单位账户取款了-------------------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
        
		g_pub_tx.ac_id = 0;	 /*test clean */
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");
        
		strcpy(g_pub_tx.ac_no,cz_acno);
		g_pub_tx.tx_amt1 = dT_amt;		/**/
		strcpy(g_pub_tx.ct_ind,"2");
        
		if ( cz_acno[0] == '9' )	/*-- 内部账 --*/
		{
			strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
			g_pub_tx.svc_ind=9001;
        
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "A016" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			set_zd_data( "1204", "01" );
			set_zd_data( "1205", "2" );
			set_zd_data( "120A", "批量开户" );
			set_zd_double( "1208", dT_amt );
		}
		else
		{
			strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为内部*/
			g_pub_tx.svc_ind=1001;
        
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D003" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			set_zd_data( "102J", "01" );
			set_zd_data( "102K", "2" );
			set_zd_double( "102F", dT_amt );
			set_zd_double( "102P", 0.00 );
			set_zd_double( "102Q", 0.00 );
		}
        
		if ( sAg_unitinfo.opr_attri[0] == '1' )
		{
			strcpy(g_pub_tx.brf, "代付业务开户");
		}
		else
		{
			strcpy(g_pub_tx.brf, "代收业务开户");
		}
        
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号,零金额不计流水,明细*/
		strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
        
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}		
		
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"记活期开户会计账出错[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	
	memset( execstr, 0x0, sizeof(execstr) );
	if ( iErr_flag == 0 )
	{
		memset( filename3, 0x0, sizeof(filename3) );
		/**sprintf( filename3, "%s/DWKHHZ%s%ld%s.txt", getenv("FILDIR"), cUnit_no, g_pub_tx.tx_date,g_pub_tx.tel); **/
		sprintf( filename3, "%s/DWKHHZ%s%ld%s%06d.txt", getenv("FILDIR"), cUnit_no, g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no); /**Add by hxc 100529 文件名添加6位流水号**/
		
		fp3 = fopen( filename3, "w" ); /*------正确处理结果汇总-------*/
		if( fp1==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename3 );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
		
		memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
		ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
		
		pub_base_strpack( sCom_branch.br_name );
		pub_base_strpack( sAg_unitinfo.bank_acct_no );
		fprintf( fp3, "				[%s]批量处理单位传票	\n", sAg_unitinfo.unit_name );
		fprintf( fp3, "机构码: %s    			机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "单位代码: %s   			单位名称: %s  \n", cUnit_no, sAg_unitinfo.unit_name );
		fprintf( fp3, "日期 : %ld 			单位账号：%s \n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
		
		fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
		fprintf( fp3, "│单位编号│           单   位   名   称            │  人数  │    金   额     │\n" );
		fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
		fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
		fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
		fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
		fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n\n\n\n\n\n\n\n" );

		fprintf( fp3, "				[%s]批量处理个人汇总传票	\n", sAg_unitinfo.unit_name );
		fprintf( fp3, "机构码: %s    			机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "日期 : %ld 			汇总科目 ： \n", g_pub_tx.tx_date );
		
		fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
		fprintf( fp3, "│单位编号│           单   位   名   称            │  人数  │    金   额     │\n" );
		fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
		fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
		fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
		fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
		fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n" );
		
		fclose( fp3 );		
		
		memset( filename4, 0x0, sizeof(filename4) );
		/**sprintf( filename4,"%s/DLKHMX%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/
    sprintf( filename4,"%s/DLKHMX%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no); /**Add by hxc 100529 文件名添加6位流水号**/
			
		sprintf( execstr, "cp %s %s", filename1, filename4 );
		ret = system( execstr );
		
		sprintf( execstr, "cp %s %s", filename3, filename );
	}
	else
	{
		sprintf( execstr, "cp %s %s", filename2, filename );
	}

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"更改下传文件名失败[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;		
	}

	set_zd_data( DC_FILE_SND, "1" );
	
	if ( iErr_flag == 1 )
	{
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"批量开户成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose( fp );
    fclose( fp1 );
    fclose( fp2 );
    sprintf(acErrMsg,"批量开户失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
