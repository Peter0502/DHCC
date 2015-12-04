/*************************************************
* 文 件 名: spF687.c(spF610.c功能类似)
* 功能描述：批量开户交易--直接读给神码传送的文件
*
* 作    者: jack
* 完成日期: 2005年02月24日
* 修改日期: 
* 修 改 人: ZHGF
* 修改内容: 改造成直接读原来给神码传送的文件
insert into tx_def values ('6687','神码审批后批量开卡','10000000000010000000000001000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000','1','4','2');
insert into tx_flow_def values ('6687','0','6687','#$');
insert into tx_sub_def values ('6687','神码审批后批量开卡','spF687','0','0');
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*--- 证件类型默认为1 -现改为可以任何证件类型fldstr[5]-*/
#include "public.h"
#include "card.h"
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
#include "cif_email_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_mger_rel_c.h"
#include "mo_sxc_c.h"

int sxc_sign(char * ac_no);

spF687()
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

	char filename[100],cFilename[100];
	char filename1[100];
	char fldstr[30][100];
	char tmpstr[1024];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	int i=0;

	char filename2[100];
	FILE *fp2 = NULL;
	char filename3[100];
	FILE *fp3 = NULL;	
	char filename4[100];	/*将正确开户结果备份*/
	char filename5[100];
	
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
	
	char cflag[2];
	char cHand_flag[2];	/*---信息录入方式---*/
	double dT_amt = 0.00;	/*---前台输入总金额---*/
	long lCount_i = 0;	/*---前台输入总笔数---*/
	long lRnum = 0,indate=0;

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
	
	struct cif_addr_inf_c	z_cif_addr_inf;
	struct cif_per_inf_c	z_cif_per_inf;
	struct cif_email_inf_c z_cif_email_inf;
	struct cif_email_inf_c z_cif_email_inf1;
	
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
	memset( cflag, 0x0, sizeof(cflag) );
	memset( cFilename,0x0 ,sizeof(cFilename));
	memset( cPass, 0x0, sizeof(cPass) );
	
	
	get_zd_data( "0790", cPass );
	get_zd_data("0230",cPrdt_code);
	get_zd_data("0900",cNote_type);
	get_zd_data("0920",cUnit_no);
	get_zd_long("0440",&indate);
	get_zd_data( "0700", cHand_flag );
	get_zd_data( "0260", cFilename);/**前台传来的文件**/
	if ( strcmp(cNote_type, "020") && strcmp(cNote_type, "024")&& strcmp(cNote_type, "027")&& strcmp(cNote_type, "028")&& strcmp(cNote_type, "029")&& strcmp(cNote_type, "031")&& strcmp(cNote_type, "032")) /* 不是卡 */
	{
		vtcp_log("%s,%d 只能开卡，现在cNote_type=[%s]\n",__FILE__,__LINE__,cNote_type);
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	lLen = strlen( cPass );
	if ( lLen == 0 )
	{
		sprintf( acErrMsg, "密码不能为空!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P242" );
		set_zd_data( "0130", "密码不能为空!!!" );
		goto ErrExit;
	}
	memset(filename,0x0,sizeof(filename));
	#if 0
	sprintf(filename,"%s/%s%d%sSHENMAOK.txt",getenv("FILDIR"),g_pub_tx.tel,indate,cUnit_no);
	#endif 
	sprintf(filename,"%s/%s",getenv("FILDIR"),cFilename);
	vtcp_log("%s,%d 看看组的文件正确吗filename=[%s]?!\n",__FILE__,__LINE__,filename);
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
		strcpy( cRctp, "银行卡" );
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
	memcpy(cz_acno,sAg_unitinfo.bank_acct_no,strlen(sAg_unitinfo.bank_acct_no));/**从数据库中去账号**/
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack(sAg_unitinfo.bank_acct_no);
	vtcp_log("%s,%d sAg_unitinfo.bank_acct_no=[%s]\n",__FILE__,__LINE__,sAg_unitinfo.bank_acct_no);
	
	memset( filename1, 0x0, sizeof(filename1) );
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

	memset(filename5,0x0,sizeof(filename5));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename5 );
	/*strcat(filename,"_610");*/
	
	sprintf(acErrMsg,"取得的文件名filename5为[%s]",filename5);
	WRITEMSG
	
	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," 没有找到文件哦 错了 [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

		get_zd_double("0400",&dT_amt);
		get_zd_long( "0550", &lCount_i );
	
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
		for( i=0;i<23;i++ )/*按照神码的文件格式 添加了单位编号 个人代理编号 开户金额**/
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
	}
	fclose ( fp );
	
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"总金额与分户交易金额合计不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"汇总金额为[%.2lf]明细金额为[%.2lf]",dT_amt,dTmp_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"总笔数与分户明细合计笔数不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"总笔数为[%d]明细笔数为[%d]",lCount_i,lCount );
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
	#if 0
	if ( cHand_flag[0] != '0' )	/*---非手工去掉第一行---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}/**经过神码处理后就没有统计的第一行了**/
	#endif
	
	/****逐条开户了***/
	
	lCount=1;
	while(1)
	{
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<23;i++ )/**共有23个值*/
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			pub_base_strpack( fldstr[i] );
			vtcp_log("[%s][%d]循环i[%s]\n",__FILE__,__LINE__,fldstr[i]);
		}
		if ( strlen(fldstr[0]) == 0  || strlen(fldstr[2]) == 0
		   ||strlen(fldstr[3]) == 0  || strlen(fldstr[4]) == 0
		   ||strlen(fldstr[14]) == 0 || strlen(fldstr[20]) == 0
		   ||strlen(fldstr[21]) == 0 /**||strlen(fldstr[22]) == 0**/
		   ||strlen(fldstr[1]) == 0)
		{
			if(strlen(fldstr[0]) == 0){
				sprintf( cReturn, "第[%d]行，卡号不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，卡号不能为空",lCount);
			}
			if(strlen(fldstr[1]) == 0){
				sprintf( acErrMsg, "第[%d]行，给神码传过去后的文件中客户号绝对不可能为空，请检查哪里有错误!",lCount);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C051");
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
			}
			if(strlen(fldstr[2]) == 0){
				sprintf( cReturn, "第[%d]行，证件种类不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，证件种类不能为空",lCount);
			}
			if(strlen(fldstr[3]) == 0){
				sprintf( cReturn, "第[%d]行，证件号码不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，证件号码不能为空",lCount);
			}
			if(strlen(fldstr[4]) == 0){
				sprintf( cReturn, "第[%d]行，客户名不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，客户名不能为空",lCount);
			}
			if(strlen(fldstr[14]) == 0){
				sprintf( cReturn, "第[%d]行，手机号码不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，手机号码不能为空",lCount);
			}
			if(strlen(fldstr[20]) == 0){
				sprintf( cReturn, "第[%d]行，单位编号不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，单位编号不能为空",lCount);
			}
			if(strlen(fldstr[21]) == 0){
				sprintf( cReturn, "第[%d]行，个人代理编号不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，个人代理编号不能为空",lCount);
			}
			/**if(strlen(fldstr[22]) == 0){
				sprintf( cReturn, "第[%d]行，开户金额不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，开户金额不能为空",lCount);
			}**/
			/*其中任何一项为空认为提示报错*/
			iErr_flag = 1;
			ret = 1;
			strcpy( g_pub_tx.reply, "H033" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,acErrMsg);
			/**goto TrLog;**/
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		vtcp_log("%s,%d cUnit_no=[%s]\n",__FILE__,__LINE__,cUnit_no);
		vtcp_log("%s,%d fldstr[20]=[%s]\n",__FILE__,__LINE__,fldstr[20]);
		/**pub_base_strpack_all(fldstr[20]);**/
		pub_base_ltrim(fldstr[20]); /**去掉左边空格**/
		vtcp_log("%s,%d 去掉左边空格后fldstr[20]=[%s]\n",__FILE__,__LINE__,fldstr[20]);
		if(memcmp(cUnit_no,fldstr[20],4)){
			/**单位编号输入错误*提示报错**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，单位编号输入不正确",lCount);
			strcpy( g_pub_tx.reply, "H034" );
			sprintf( acErrMsg, "第[%d]行,单位编号输入不正确,前台编号为%s,文件中为%s",lCount,cUnit_no,fldstr[20]);
			set_zd_data(DC_GET_MSG,acErrMsg);
			/***goto TrLog;**/
			goto ErrExit;
		}
				
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset(&z_cif_email_inf,0x00,sizeof(z_cif_email_inf));
		memset(&z_cif_email_inf1,0x00,sizeof(z_cif_email_inf1));
		memset(&t_cif_basic_inf,0x00,sizeof(t_cif_basic_inf));
		memset( &sAg_peritemmap, 0x00,sizeof(sAg_peritemmap) );
		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&z_cif_addr_inf,0x00,sizeof(z_cif_addr_inf));
		memset(&z_cif_per_inf,0x00,sizeof(z_cif_per_inf));
		
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		dTx_amt = 0.00;
		dTx_amt = atof(fldstr[22]);/**单户金额**/
		/*-	fldstr[0]   卡号 ------------ok------------
			fldstr[1]   客户号           ok
			fldstr[2]   持卡人证件种类   ok
			fldstr[3]   持卡人证件号码   ok
		    fldstr[4]   持卡人客户名     ok
		    fldstr[5]   持卡人客户英文名 
		    fldstr[6]   持卡人性别       
		    fldstr[7]   持卡人家庭住址   ok
		    fldstr[8]   持卡人家庭邮编   ok
		    fldstr[9]   持卡人家庭电话
		    fldstr[10]  持卡人单位名称   ok
		    fldstr[11]  持卡人单位地址   ok
		    fldstr[12]  持卡人单位邮编   ok
		    fldstr[13]  持卡人单位电话
		    fldstr[14]  持卡人手机号码   ok
		    fldstr[15]  持卡人邮箱
		    fldstr[16]  持卡人信誉等级
		    fldstr[17]  月收入
		    fldstr[18]  出生日期
		    fldstr[19]  婚姻状况
		    fldstr[20]  单位编号depid    ok
		    fldstr[21]  个人代理编号itemno    ok
		    fldstr[22]  个人代理编号itemno    ok
		----------------------------------------------*/
		
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[21] );
		if ( ret == 0 )
		{
			/*----同一编号对应下的身份证号码已经存在则不允许开户----*/
			/*pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "个人代理编号%s已有对应账号不能重新开户", fldstr[21]);
				WRITEMSG
				set_zd_data(DC_GET_MSG,acErrMsg);*/
				/***goto TrLog;***/
				/*goto ErrExit;
			}*/
			/*---------------------------------------------*/
				/*----同一单位编号 不允许个人代理编号重复----*/

				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.item_no );
				sprintf( cReturn, " 编号[%s]重复", sAg_peritemmap.item_no );
				/** 
				strcpy( g_pub_tx.reply, "H033" ); 
				**/
				strcpy(g_pub_tx.reply,"H033");
				set_zd_data(DC_GET_MSG,acErrMsg);
				sprintf( acErrMsg, "编号[%s]重复", sAg_peritemmap.item_no );				
				WRITEMSG
				
				goto ErrExit;
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"查询ag_peritemmap出错[%s]",fldstr[21] );
			WRITEMSG
			strcpy(g_pub_tx.reply,"H010");
			set_zd_data(DC_GET_MSG,acErrMsg);
			goto ErrExit;
		}

		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
		memset(&t_cif_basic_inf, 0x0, sizeof(t_cif_basic_inf));
		/*-	fldstr[0]   卡号 ------------ok------------
			fldstr[1]   客户号           ok
			fldstr[2]   持卡人证件种类   ok
			fldstr[3]   持卡人证件号码   ok
		    fldstr[4]   持卡人客户名     ok
		    fldstr[5]   持卡人客户英文名 
		    fldstr[6]   持卡人性别       
		    fldstr[7]   持卡人家庭住址   ok
		    fldstr[8]   持卡人家庭邮编   ok
		    fldstr[9]   持卡人家庭电话
		    fldstr[10]  持卡人单位名称   ok
		    fldstr[11]  持卡人单位地址   ok
		    fldstr[12]  持卡人单位邮编   ok
		    fldstr[13]  持卡人单位电话
		    fldstr[14]  持卡人手机号码   ok
		    fldstr[15]  持卡人邮箱
		    fldstr[16]  持卡人信誉等级
		    fldstr[17]  月收入
		    fldstr[18]  出生日期
		    fldstr[19]  婚姻状况
		    fldstr[20]  单位编号depid    ok
		    fldstr[21]  个人代理编号itemno    ok
		    fldstr[22]  个人代理编号itemno    ok
		----------------------------------------------*/
		strcpy(cif_id_code_rel.id_type, fldstr[2]);/* 证件类型 */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* 证件号码 */
    	vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[2]);

		strcpy(cif_basic_inf.type,"1");		      /* 客户类型 */
		strcpy(cif_basic_inf.name,fldstr[4]);	  /* 客户名称 */
		strcpy(cif_basic_inf.local,"1");	/* 本地标志 */
		strcpy(cif_basic_inf.poten,"1");	/* 潜在标志1-本行2-潜在 */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*是否本行股东2-是 1-否 */
		strcpy(cif_basic_inf.lvl,"4"); /*客户级别*/

		/*--------开始建立客户信息----------*/
		cif_no = 0;

		/***这段开户交易应该不用了，因为在给神码传送文件之前已经将没有建立客户号的证件建立了新的客户号
		这个地方如果查不到也不能再建立了，否则神码那边不知道，所以，如果查不到则直接报错
		***/
		/* 检查客户证件类型＋证件号码是否已经存在 */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , cif_id_code_rel.id_type ,
					  cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"该客户已经拥有客户ID![%s][%s]",fldstr[2],fldstr[3]);
			WRITEMSG
			if(cif_id_code_rel.cif_no!=atol(fldstr[1])){
					sprintf( acErrMsg, "第[%d]行，客户号不符合，数据库中为【%ld】",lCount,cif_id_code_rel.cif_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C051");
					set_zd_data(DC_GET_MSG,acErrMsg);
					/**goto TrLog;**/
					goto ErrExit;
			}
			dRet = Cif_basic_inf_Sel(g_pub_tx.reply, &t_cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(dRet == 0)
			{
				pub_base_strpack(fldstr[4]);
				pub_base_strpack(t_cif_basic_inf.name);				
				if(strcmp(t_cif_basic_inf.name,fldstr[4]) != 0)
				{
					/**客户户名不符*提示报错**/
					iErr_flag = 1;
					ret = 1;
					sprintf( acErrMsg, "第[%d]行，户名不符合，数据库中为【%s】",lCount,t_cif_basic_inf.name);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P042");
					set_zd_data(DC_GET_MSG,acErrMsg);
					/**goto TrLog;**/
					goto ErrExit;
				}
			}
			else if( dRet )
			{
				sprintf(acErrMsg,"取客户信息异常![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
			}
		}else 
		{
			sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
			set_zd_data(DC_GET_MSG,acErrMsg);
			goto ErrExit;
		}
/**OK_cif_no:**/
		/**取最小凭证号**/
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
				set_zd_data(DC_GET_MSG,acErrMsg);
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
				set_zd_data(DC_GET_MSG,acErrMsg);
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
				set_zd_data(DC_GET_MSG,acErrMsg);
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
					set_zd_data(DC_GET_MSG,acErrMsg);
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
					set_zd_data(DC_GET_MSG,acErrMsg);
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
					set_zd_data(DC_GET_MSG,acErrMsg);
					goto ErrExit;
				}
			}
		}
		else if( ret )
		{
			sprintf( acErrMsg,"查询柜员最小可用凭证出错[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply,"N012" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			goto ErrExit;
		}
		
		/***卡号现在不用重新生成 直接取fldstr[0]的值**/
		memcpy(g_pub_tx.crd_no,fldstr[0],19);/**19位**/
		vtcp_log("%s,%d g_pub_tx.crd_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.crd_no);
		vtcp_log("%s,%d fldstr[0]=[%s]\n",__FILE__,__LINE__,fldstr[0]);
		/***开始比较卡号的凭证号与柜员的凭证号是否一致**/
		if(memcmp(fldstr[0]+6,sNote_min_no.note_min_no+4,12)){/*卡号7-18位为凭证号码**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，卡号与凭证号不符[%s]",lCount,sNote_min_no.note_min_no);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "第[%d]行，卡号与凭证号不符[%s]",lCount,sNote_min_no.note_min_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"N012" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			/**goto TrLog;**/
			goto ErrExit;
		}
		vtcp_log("%s,%d 看看结果吧！ret=[%d]\n",__FILE__,__LINE__,ret);
		if(strcmp(cNote_type,"020")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0020");		
		}
		if(strcmp(cNote_type,"024")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0024");		
		}
		if(strcmp(cNote_type,"027")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0027");		
		}
		if(strcmp(cNote_type,"028")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0028");		
		}
		if(strcmp(cNote_type,"029")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0029");		
		}
		if(strcmp(cNote_type,"031")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0031");		
		}
		if(strcmp(cNote_type,"032")== 0)
		{
		strcpy(g_pub_tx.mdm_code,"0032");		
		}
		strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102产品为无介质户*/
		{
			strcpy( g_pub_tx.mdm_code, "0000" );
			memset( g_pub_tx.beg_note_no, 0x0, sizeof(g_pub_tx.beg_note_no) );
			strcpy( g_pub_tx.ac_wrk_ind, "0" );	/*--不需要做凭证销号处理--*/
		}
		/*-	fldstr[0]   卡号 ------------ok------------
			fldstr[1]   客户号           ok
			fldstr[2]   持卡人证件种类   ok
			fldstr[3]   持卡人证件号码   ok
		    fldstr[4]   持卡人客户名     ok
		    fldstr[5]   持卡人客户英文名 
		    fldstr[6]   持卡人性别       
		    fldstr[7]   持卡人家庭住址   ok
		    fldstr[8]   持卡人家庭邮编   ok
		    fldstr[9]   持卡人家庭电话
		    fldstr[10]  持卡人单位名称   ok
		    fldstr[11]  持卡人单位地址   ok
		    fldstr[12]  持卡人单位邮编   ok
		    fldstr[13]  持卡人单位电话
		    fldstr[14]  持卡人手机号码   ok
		    fldstr[15]  持卡人邮箱
		    fldstr[16]  持卡人信誉等级
		    fldstr[17]  月收入
		    fldstr[18]  出生日期
		    fldstr[19]  婚姻状况
		    fldstr[20]  单位编号depid    ok
		    fldstr[21]  个人代理编号itemno    ok
		----------------------------------------------*/
		
		strcpy(g_pub_tx.id_type, fldstr[2]);	
		strcpy(g_pub_tx.id_no,fldstr[3]);
		strcpy(g_pub_tx.name,fldstr[4]);
		
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
			sprintf(acErrMsg,"开介质错!!!ret[%d]crd_no[%s]beg_note_no[%s]",ret, fldstr[0],g_pub_tx.beg_note_no);
			WRITEMSG
			set_zd_data(DC_GET_MSG,acErrMsg);
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
		strcpy(g_pub_tx.name,fldstr[4]);
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
			set_zd_data(DC_GET_MSG,acErrMsg);
			goto ErrExit;
		}

		set_zd_data( "1033", "01" );
		set_zd_data( "103E", "2" );
		set_zd_double( "1037", dTx_amt );
		strcpy( g_pub_tx.brf, "批量开户" );
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"记活期开户会计账出错[%d]!",ret);
			WRITEMSG
			set_zd_data(DC_GET_MSG,acErrMsg);
			goto ErrExit;
		}
		#if 0
		/************为扣工本费和年费做准备,还要具体修改***/
		{
			/**********取款工本费************/
			strcpy(g_pub_tx.ac_no, dd_ac_no);
			g_pub_tx.ac_id = g_dd_mst.ac_id;
			g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;
			strcpy(g_pub_tx.ac_get_ind, "001");
			strcpy(g_pub_tx.ac_id_type, "1");
			strcpy(g_pub_tx.add_ind, "0");  
			strcpy(g_pub_tx.ct_ind, "2");   
			strcpy(g_pub_tx.prt_ind, "0");
			strcpy(g_pub_tx.hst_ind, "1");
			strcpy(g_pub_tx.intst_type, "1");
			g_pub_tx.ac_wrk_ind[9] = '1';
			g_pub_tx.ac_wrk_ind[8] = '1';
			strcpy(g_pub_tx.note_type, "299");
			strcpy(g_pub_tx.beg_note_no, dd_ac_no);
            g_pub_tx.svc_ind = 1001;
            g_pub_tx.tx_amt1 = tx_amt2 + tx_amt3 + tx_amt4;
            strcpy(g_pub_tx.brf, "");

            if (pub_acct_trance()) {
                    sprintf(acErrMsg, "交易记账错误!");
                    WRITEMSG
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    goto ErrExit;
            }
            set_zd_data("102J", g_ln_parm.cur_no);
            set_zd_double("102F", g_pub_tx.tx_amt1);
            strcpy(g_pub_tx.sub_tx_code, "D003");
            strcpy(g_pub_tx.prdt_code, g_dd_mst.prdt_no);
            ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
			if (ret != 0) {
            	sprintf(acErrMsg, "会计记账不成功!!");
            	WRITEMSG
            	set_zd_data(DC_GET_MSG,acErrMsg);
                goto ErrExit;
            }
		}
		/************为扣工本费和年费做准备***/
		#endif 
		memset( jz_acno, 0x0, sizeof(jz_acno) );
		jz_ac_seq = 0;
		sprintf(jz_acno,"%d",g_pub_tx.ac_id);
		jz_ac_seq = g_pub_tx.ac_seqn;

		sprintf(acErrMsg,"生成的记帐账号和账号序号分别为(g_pub_tx.ac_id)[%s] [%d]",jz_acno,g_pub_tx.ac_seqn);
		WRITEMSG
		
		/*begin 金太阳卡 签约随心存 wudawei 20151127*/		
		if(strcmp(cNote_type,"027")== 0 || strcmp(cNote_type,"032")== 0)
		{
			int sxc_ret = sxc_sign(xs_acno);
			if(sxc_ret==1)
			{
				sprintf(acErrMsg,"签约随心存失败，账号=[%s] ret=[%d]!!!",xs_acno,sxc_ret );
				WRITEMSG
				strcpy( g_pub_tx.reply, "P090" );
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
			}
			sprintf(acErrMsg,"签约随心存成功！[%s]  ",xs_acno);
			WRITEMSG
		}
		/* end金太阳卡 签约随心存 wudawei 20151127*/
		
		pub_base_strpack( sAg_unitinfo.pay_type );
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
		/*-	fldstr[0]   卡号 ------------ok------------
			fldstr[1]   客户号           ok
			fldstr[2]   持卡人证件种类   ok
			fldstr[3]   持卡人证件号码   ok
		    fldstr[4]   持卡人客户名     ok
		    fldstr[5]   持卡人客户英文名 
		    fldstr[6]   持卡人性别       
		    fldstr[7]   持卡人家庭住址   ok
		    fldstr[8]   持卡人家庭邮编   ok
		    fldstr[9]   持卡人家庭电话
		    fldstr[10]  持卡人单位名称   ok
		    fldstr[11]  持卡人单位地址   ok
		    fldstr[12]  持卡人单位邮编   ok
		    fldstr[13]  持卡人单位电话
		    fldstr[14]  持卡人手机号码   ok
		    fldstr[15]  持卡人邮箱
		    fldstr[16]  持卡人信誉等级
		    fldstr[17]  月收入
		    fldstr[18]  出生日期
		    fldstr[19]  婚姻状况
		    fldstr[20]  单位编号depid    ok
		    fldstr[21]  个人代理编号itemno    ok
			fldstr[22]  开户金额 opnamt
		----------------------------------------------*/
			
		strcpy( sAg_peritemmap.unit_no, cUnit_no );
		strcpy( sAg_peritemmap.item_no, fldstr[21] );
		strcpy( sAg_peritemmap.name, fldstr[4] );
		strcpy( sAg_peritemmap.id_type, fldstr[2] );
		strcpy( sAg_peritemmap.id_no, fldstr[3] );
		strcpy( sAg_peritemmap.ac_no, xs_acno );
		sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.stat, "0" );
		sAg_peritemmap.opn_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
		sAg_peritemmap.cls_date = 99999999;
		strcpy( sAg_peritemmap.cls_tel, "0000" );

		ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
		if (ret == -239 )	/*--- 有重复记录 ---*/
		{
			ret = sql_execute( "update ag_peritemmap set name = '%s', \
				id_no = '%s', ac_no = '%s',opn_date = %ld, opn_tel= '%s'\
				where unit_no = '%s' and item_no = '%s'", \
				fldstr[4], fldstr[3], xs_acno,g_pub_tx.tx_date, \
				g_pub_tx.tel, cUnit_no, fldstr[21] );
			if ( ret )
			{
				sprintf(acErrMsg,"修改ag_peritemmap失败[%s][%d]!!!",fldstr[21],ret );
				WRITEMSG
				strcpy( g_pub_tx.reply, "P090" );
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
			}
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"写记录入Ag_peritemmap失败!!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,acErrMsg);
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
    
    
    
		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		g_pub_tx.tx_amt1 = 0.00;		/*发生额*/

/***TrLog:		****/
		dTram = atof( fldstr[22] );
		/**现在只有可能成功，不允许失败，任何一张失败都整个回滚***/
		if ( ret == 0 )
		{
			/*-	fldstr[0]   卡号 ------------ok------------
			fldstr[1]   客户号           ok
			fldstr[2]   持卡人证件种类   ok
			fldstr[3]   持卡人证件号码   ok
		    fldstr[4]   持卡人客户名     ok
		    fldstr[5]   持卡人客户英文名 
		    fldstr[6]   持卡人性别       
		    fldstr[7]   持卡人家庭住址   ok
		    fldstr[8]   持卡人家庭邮编   ok
		    fldstr[9]   持卡人家庭电话
		    fldstr[10]  持卡人单位名称   ok
		    fldstr[11]  持卡人单位地址   ok
		    fldstr[12]  持卡人单位邮编   ok
		    fldstr[13]  持卡人单位电话
		    fldstr[14]  持卡人手机号码   ok
		    fldstr[15]  持卡人邮箱
		    fldstr[16]  持卡人信誉等级
		    fldstr[17]  月收入
		    fldstr[18]  出生日期
		    fldstr[19]  婚姻状况
		    fldstr[20]  单位编号depid    ok
		    fldstr[21]  个人代理编号itemno    ok
			fldstr[22]  开户金额 opnamt
		----------------------------------------------*/
			/***fprintf( fp1, "│%20s│%20s│%16s│%20s│%20s│%8s│%16s│\n", "代理编号", "客户姓名", "开户金额", "证件号码", "账号", "凭证种类", "凭证号" );***/
			fprintf( fp1, "│%20s│%20s│%16.2f│%20s│%20s│%8s│%16s│\n", fldstr[21], fldstr[4], \
				dTram, fldstr[3], xs_acno, cRctp, sNote_min_no.note_min_no );
				
			lSnum = lSnum + 1;
			dSamt = dSamt + dTram;
		}
		else
		{
			    /***fprintf( fp2, "│%20s│%20s│%16s│%20s│%44s│\n", "代理编号", "客户姓名", "开户金额", "证件号码", "错误信息" );**/
			    fprintf( fp2, "│%20s│%20s│%16.2f│%20s│%44s│\n", fldstr[21],fldstr[4],dTram,fldstr[3],cReturn );
		}
		lCount = lCount + 1;
	}
	
	fprintf( fp1, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━┷━━━━━━━━┙\n" );
	fprintf( fp2, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━┙\n" );

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	
	if ( iErr_flag == 0 )	/*如果无错误记录则记单位账*/
	{ /***现在只有代发，即是扣代理户的帐**/
		sprintf(acErrMsg,"-----------------开始单位账户取款了-------------------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
        
		g_pub_tx.ac_id = 0;	 /*test clean */
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");
        
		strcpy(g_pub_tx.ac_no,cz_acno);
		g_pub_tx.tx_amt1 = dT_amt;		/*已文件中的总金额为准*/
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
	{/**开户全部成功**/
		memset( filename3, 0x0, sizeof(filename3) );
		/**sprintf( filename3, "%s/DWKHHZ%s%ld%s.txt", getenv("FILDIR"), cUnit_no, g_pub_tx.tx_date,g_pub_tx.tel);**/
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
		
		/***sprintf( execstr, "cp %s %s", filename3, filename );***/
		sprintf( execstr, "cp %s %s", filename3, filename5 );/**filename是前台传送过来的文件名，
																filename5是为了可以使前台可以查询显示**/
	}
	else
	{/**有失败的情况**/
		/***sprintf( execstr, "cp %s %s", filename2, filename );***/
		sprintf( execstr, "cp %s %s", filename2, filename5 );/**filename是前台传送过来的文件名，
																filename5是为了可以使前台可以查询显示**/
	}

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"更改下传文件名失败[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;		
	}

	set_zd_data( DC_FILE_SND, "1" );
	
	if ( iErr_flag == 1 )
	{
		strcpy( g_pub_tx.reply, "H086" );
		sprintf(acErrMsg,"批量开户文件有其他错误!");
		set_zd_data(DC_GET_MSG,acErrMsg);
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


/*金太阳卡 签约随心存 wudawei 20151127*/
int sxc_sign(char * ac_no)
{
     int iRet=0;
     
     char cFlag[2];
     char tmpStr[2];
     struct mo_sxc_c     sMo_sxc;
     struct mdm_ac_rel_c sMdm_ac_rel;
     struct dd_mst_c     sDd_mst;
     
     memset(cFlag,       0x00, sizeof(cFlag));
     memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
     memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
     memset(&sDd_mst,     0x00, sizeof(sDd_mst));

     pub_base_sysinit();
     
     strcpy(g_pub_tx.ac_no,  ac_no);
     pub_base_strpack(g_pub_tx.ac_no);
     g_pub_tx.ac_seqn = 1;
     cFlag[0] = '1' ;     /**1为开户签约专用，其他交易不可传值1**/
     vtcp_log("ac_no[%s], ac_seqn[%d]", g_pub_tx.ac_no, g_pub_tx.ac_seqn);
     
     
 
     /*根据ac_no从表mdm_ac_rel中获取ac_id*/
     iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
     if(iRet)
     {
     	vtcp_log("获取ac_id失败，iRet=[%d]", iRet);
     	return 1;	
     }
     
     /**随心存只针对金太阳卡和复合测试卡2开放，其他卡不能签约随心存 20151128**/
     if(strcmp(sMdm_ac_rel.mdm_code,"0027")!=0 && strcmp(sMdm_ac_rel.mdm_code,"0032")!=0)
     	return 1;	
     
     if(cFlag[0] == '1')       
     {
     	sMo_sxc.qy_type[0] = '1';  /**开户签约标志1**/
     }
     
     /*登记遂心存登记薄*/
     sMo_sxc.ac_id = sMdm_ac_rel.ac_id;
     sMo_sxc.ac_seqn = g_pub_tx.ac_seqn;
     if(sMo_sxc.ac_seqn != 1)
     {
     	/**友情提示：目前活期序号都为1，如果以后加上子卡后存在序号不为1的账户，请注意修改查询交易sp8314,sp8315中的账户序号1**/
        vtcp_log("ac_seqn[%d]序号不为1，不允许遂心存签约", sMo_sxc.ac_seqn);
        return 0;	
     }
     sMo_sxc.bal = 0.00;
     sMo_sxc.intst = 0.00;
     sMo_sxc.un_intst = 0.00;
     sMo_sxc.tx_date   = g_pub_tx.tx_date;
     sMo_sxc.trace_no  = g_pub_tx.trace_no;
     strcpy(sMo_sxc.tx_br_no, g_pub_tx.tx_br_no);
     strcpy(sMo_sxc.tx_tel, g_pub_tx.tel);
     strcpy(sMo_sxc.opn_br_no, sMdm_ac_rel.opn_br_no);
     sMo_sxc.sts[0]  = '1';   /**1-签约**/
     sMo_sxc.flag[0] = '1';   /**1-当日发生业务 日终执行批量**/
     
     iRet = Mo_sxc_Ins(sMo_sxc, g_pub_tx.reply);
     if(iRet)
     {
        vtcp_log("登记遂心存登记薄失败，iRet=[%d]", iRet);
        return 1;	
     }
    
     return 0;

}