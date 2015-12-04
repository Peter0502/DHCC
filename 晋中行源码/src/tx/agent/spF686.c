/*************************************************
* 文 件 名: spF686.c--spF610.c修改
* 功能描述：批量开户交易-神码
*
* 作    者: jack
* 完成日期: 2005年02月24日
* 修改日期: 2010-03-22
* 修 改 人: ZHGF
* 修改内容: 改成支持神码要求的异步通信开卡模式
insert into tx_def values ('6686','神码批量开卡生成文件','10000000000000000000000001000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000','1','4','0','3','');
insert into tx_flow_def values ('6686','0','6686','#$');
insert into tx_sub_def values ('6686','神码批量开卡生成文件','spF686','0','0');

* 前台导磁盘的文件格式按照神码的文件要求进行，如下：
卡号|客户号|持卡人证件种类|持卡人证件号码|持卡人客户名|持卡人客户英文名|持卡人性别|持卡人家庭住址|持卡人家庭邮编|持卡人家庭电话|持卡人单位名称|持卡人单位地址|持卡人单位邮编|持卡人单位电话|持卡人手机号码|持卡人邮箱|持卡人信誉等级|月收入|出生日期|婚姻状况|单位编号|个人代理编号|开户金额|
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"/*--- 证件类型默认为1 -现改为可以任何证件类型fldstr[5]-*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "note_mst_c.h"
#include "cif_basic_inf_c.h"
#include "cif_mger_rel_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_wrk_hst_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_per_inf_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "note_min_no_c.h"
#include "com_branch_c.h"
#include "mdm_prdt_rel_c.h"
#include "cif_email_inf_c.h"
#include "card.h"
int transfer(char *buf,char *fdstr);
extern char *get_env_info(char *infoname);

spF686()
{ 	
	int ret=0;
	char execstr[256];

	char cPrdt_code[4];
	char cBeg_card_no[20];
	char cEnd_card_no[20];
	char cCard_no[20];
	char cNote_no[20];
	char cz_acno[20];
	char cUnit_no[5];
	char cNote_type[4];
	char cCpfilename[200];
	char cFtpfile[200];

	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;	

	double dTram = 0.00;

	char cPass[7];
	long lLen = 0;

	char filename[100],cFilename[100];
	char filename1[100];
	char fldstr[30][150];/**神码要求是20个内容暂时定义为30，最长的为140，所以暂时定为150***/
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

	char cHand_flag[2];	/*---信息录入方式---*/
	double dT_amt = 0.00;	/*---前台输入总金额---*/
	long lCount_i = 0;	/*---前台输入总笔数---*/
	long lRnum = 0,iOpcnt=0;

	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_mger_rel_c cif_mger_rel;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct note_min_no_c sNote_min_no;
	struct com_branch_c sCom_branch;
	struct mdm_prdt_rel_c sMdm_prdt_rel;
	struct cif_addr_inf_c	z_cif_addr_inf;
	struct cif_per_inf_c	z_cif_per_inf;
	struct note_mst_c	note_mst_z;
	struct cif_email_inf_c z_cif_email_inf;
	struct cif_email_inf_c z_cif_email_inf1;
	memset(&note_mst_z,0x00,sizeof(struct note_mst_c));

	memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
	memset(&cif_mger_rel,0x00,sizeof(cif_mger_rel));
	memset(&cif_id_code_rel,0x00,sizeof(cif_id_code_rel));
	memset(&sAg_unitinfo,0x00,sizeof(sAg_unitinfo));
	memset(&sAg_peritemmap,0x00,sizeof(sAg_peritemmap));
	memset(&sPay_agent,0x00,sizeof(sPay_agent));
	memset(&sNote_min_no,0x00,sizeof(sNote_min_no));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
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
	memset( filename,0x0,sizeof(filename));
	memset( cFilename,0x0,sizeof(cFilename));
	memset( filename1,0x0,sizeof(filename1));
	memset( filename2,0x0,sizeof(filename2));
	memset( filename3,0x0,sizeof(filename3));
	memset( filename4,0x0,sizeof(filename4));
	memset(cCpfilename,0x00,sizeof(cCpfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));

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
	/**
卡号|客户号|持卡人证件种类|持卡人证件号码|持卡人客户名|持卡人客户英文名|持卡人性别|持卡人家庭住址|持卡人家庭邮编|持卡人家庭电话|持卡人单位名称|持卡人单位地址|持卡人单位邮编|持卡人单位电话|持卡人手机号码|持卡人邮箱|持卡人信誉等级|月收入|出生日期|婚姻状况|代理单位编号|个人代理编号|开户金额|
	**/
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "020") || !strcmp(cNote_type, "024") ) /* 卡 $IC卡*/
	{
		strcpy( cRctp, "卡" );
	}else{/**注意**有几种卡呢？？？？如果还有其他卡需要进行添加修改此处**/
		sprintf( acErrMsg, "介质类型错误，不是卡!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O188" );
		set_zd_data( "0130", "介质类型错误!!!" );
		goto ErrExit;
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
	sprintf( filename1,"%s/0%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,正确文件名filename1[%s]\n",__FILE__,__LINE__,filename1);
	sprintf(cFilename,"0%s%ld%s%06d.txt",cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	fp1 = fopen( filename1,"w" );   /*按照神码要求生成文件*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	sprintf( filename2,"%s/1%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,有错误信息的记录写成filename2[%s]\n",__FILE__,__LINE__,filename2);
	fp2 = fopen(filename2,"w" );   /*生成前台柜员的错误提示文件*/
	if(fp2==NULL)
	{
		sprintf( acErrMsg, " open file [%s] error ", filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf(fp2,"@3v1\n");
  	
	fprintf( fp2, "                         批量开户错误结果信息表[%s-%s]			\n", \
    	sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
	fprintf( fp2, "    ┍━━━━━┯━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━┑\n" );
	fprintf( fp2, "    │%10s│%10s│%20s│%20s│%44s│\n", "代理编号", "客户姓名", "待开卡号", "证件号码", "错误信息" );
	fprintf( fp2, "    ┝━━━━━┿━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━┥\n" );

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG
	/**将前台传来的文件进行备份**/
	sprintf(cCpfilename,"cp %s %s/CARD%s%d%sOK.txt",filename,getenv("FILDIR"),g_pub_tx.tel,g_pub_tx.tx_date,cUnit_no);
	vtcp_log("%s,%d cCpfilename=[%s]\n",__FILE__,__LINE__,cCpfilename);
	ret=system(cCpfilename);
	if (ret)
	{
		sprintf(acErrMsg,"拷贝前台传来的文件出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}

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
	else				/*---非手工方式文件第一行为汇总信息---主要处理批量导盘的文件*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		/*-	批量文件的内容第一行汇总格式如下
			fldstr[0] 单位编号
		    fldstr[1] 汇总笔数
		    fldstr[2] 汇总金额
			
		----批量文件的其他每一行对应于一条记录，格式和手工录入一样，如下
			
		卡号|客户号|持卡人证件种类 |持卡人证件号码|持卡人客户名|持卡人客户英文名|持卡人性别|持卡人家庭住址|持卡人家庭邮编|持卡人家庭电话|持卡人单位名称|持卡人单位地址 |持卡人单位邮编|持卡人单位电话|持卡人手机号码|持卡人邮箱|持卡人信誉等级|月收入|出生日期|婚姻状况|单位编号|个人代理编号|开户金额|
			
		----------------------------------------------*/
		memset(fldstr,0x0,sizeof(fldstr));
		for(i=0;i<3;i++)/**第一行就两个内容 单位编号|总笔数|总金额|**/
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" fldstr[%d]=[%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		
		long    fd55=0;
		get_zd_long( "0550", &fd55);/**总笔数****/
		if(fd55 != atoi(fldstr[1]))
		{
			sprintf(acErrMsg,"总笔数与盘内笔数不相等!!!" );
			WRITEMSG
			sprintf(acErrMsg,"盘内汇总笔数为[%d]", atoi(fldstr[1]) );
			set_zd_data( "0130", acErrMsg );
			strcpy( g_pub_tx.reply, "H019" );
			goto ErrExit;
		}
		lCount_i = atoi(fldstr[1]);	
		dT_amt = atof(fldstr[2]);
		vtcp_log("%s,%d,文件中的汇总金额fldstr[2]=[%s]",__FILE__,__LINE__,fldstr[2]);
		vtcp_log("%s,%d,文件中的汇总金额为[%lf]",__FILE__,__LINE__,dT_amt);
	}
	vtcp_log("%s,%d,统计笔数[%d]前台传笔数[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	/*------ 判断柜员是否拥有满足条件的凭证 -----*/
	lCount=0;/**刚开始为1**/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			vtcp_log("%s,%d,处理最后一行!",__FILE__,__LINE__);
			memset(fldstr,0x0,sizeof(fldstr));
			if ( cHand_flag[0] != '0' )
				{
					for( i=0;i<23;i++ )/*按照神码的文件格式 添加了单位编号 个人代理编号 开户金额**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
				}else
				{/**手工开卡**/
					transfer(tmpstr,fldstr[0]);
				}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,最后一行为非法行!",__FILE__,__LINE__);
				vtcp_log("%s,%d,统计笔数lCount=[%d]",__FILE__,__LINE__,lCount);	
				break;
			}
			/**先将个人代理编号数字前面的0去掉**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]显示各个列信息\n",__FILE__,__LINE__);
		vtcp_log("fldstr0===[%s]\n",fldstr[0]);
		vtcp_log("fldstr1===[%s]\n",fldstr[1]);
		vtcp_log("fldstr2===[%s]\n",fldstr[2]);
		vtcp_log("fldstr3===[%s]\n",fldstr[3]);
		vtcp_log("fldstr4===[%s]\n",fldstr[4]);
		vtcp_log("fldstr5===[%s]\n",fldstr[5]);
		vtcp_log("fldstr6===[%s]\n",fldstr[6]);
		vtcp_log("fldstr7===[%s]\n",fldstr[7]);
		vtcp_log("fldstr8===[%s]\n",fldstr[8]);
		vtcp_log("fldstr9===[%s]\n",fldstr[9]);
		vtcp_log("fldstr10===[%s]\n",fldstr[10]);
		vtcp_log("fldstr11===[%s]\n",fldstr[11]);
		vtcp_log("fldstr12===[%s]\n",fldstr[12]);
		vtcp_log("fldstr13===[%s]\n",fldstr[13]);
		vtcp_log("fldstr14===[%s]\n",fldstr[14]);
		vtcp_log("fldstr15===[%s]\n",fldstr[15]);
		vtcp_log("fldstr16===[%s]\n",fldstr[16]);
		vtcp_log("fldstr17===[%s]\n",fldstr[17]);
		vtcp_log("fldstr18===[%s]\n",fldstr[18]);
		vtcp_log("fldstr19===[%s]\n",fldstr[19]);
		vtcp_log("fldstr20===[%s]\n",fldstr[20]);
		vtcp_log("fldstr21===[%s]\n",fldstr[21]);
		vtcp_log("fldstr22===[%s]\n",fldstr[22]);
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
		vtcp_log("%s,%d,统计笔数lCount=[%d]",__FILE__,__LINE__,lCount);	
		break;
		}
		memset(fldstr,0x0,sizeof(fldstr));
		if( cHand_flag[0] != '0' )
			{
				for( i=0;i<23;i++ )/*按照神码的文件格式 添加了单位编号 个人代理编号 开户金额**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
			}else
			{			
				vtcp_log("[%s][%d]显示各个列信息\n",__FILE__,__LINE__);
				transfer(tmpstr,fldstr[0]);
			}
			
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,此行为非法行!",__FILE__,__LINE__);
				continue;
			}
		/*
		卡号|客户号|持卡人证件种类|持卡人证件号码|持卡人客户名|持卡人客户英文名|持卡人性别|持卡人家庭住址|持卡人家庭邮编|持卡人家庭电话|持卡人单位名称|持卡人单位地址|持卡人单位邮编|持卡人单位电话|持卡人手机号码|持卡人邮箱|持卡人信誉等级|月收入|出生日期|婚姻状况|单位编号|个人代理编号|开户金额|
		*/
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
		    fldstr[22]  开户金额opnamt    ok
		----------------------------------------------*/
		{
			/**先将个人代理编号数字前面的0去掉**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]显示各个列信息\n",__FILE__,__LINE__);
		vtcp_log("fldstr0===[%s]\n",fldstr[0]);
		vtcp_log("fldstr1===[%s]\n",fldstr[1]);
		vtcp_log("fldstr2===[%s]\n",fldstr[2]);
		vtcp_log("fldstr3===[%s]\n",fldstr[3]);
		vtcp_log("fldstr4===[%s]\n",fldstr[4]);
		vtcp_log("fldstr5===[%s]\n",fldstr[5]);
		vtcp_log("fldstr6===[%s]\n",fldstr[6]);
		vtcp_log("fldstr7===[%s]\n",fldstr[7]);
		vtcp_log("fldstr8===[%s]\n",fldstr[8]);
		vtcp_log("fldstr9===[%s]\n",fldstr[9]);
		vtcp_log("fldstr10===[%s]\n",fldstr[10]);
		vtcp_log("fldstr11===[%s]\n",fldstr[11]);
		vtcp_log("fldstr12===[%s]\n",fldstr[12]);
		vtcp_log("fldstr13===[%s]\n",fldstr[13]);
		vtcp_log("fldstr14===[%s]\n",fldstr[14]);
		vtcp_log("fldstr15===[%s]\n",fldstr[15]);
		vtcp_log("fldstr16===[%s]\n",fldstr[16]);
		vtcp_log("fldstr17===[%s]\n",fldstr[17]);
		vtcp_log("fldstr18===[%s]\n",fldstr[18]);
		vtcp_log("fldstr19===[%s]\n",fldstr[19]);
		vtcp_log("fldstr20===[%s]\n",fldstr[20]);
		vtcp_log("fldstr21===[%s]\n",fldstr[21]);
		vtcp_log("fldstr22===[%s]\n",fldstr[22]);
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
		    fldstr[22]  开户金额opnamt    ok
		------------------------------------------*/
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
		vtcp_log("%s,%d,统计笔数lCount=[%d]",__FILE__,__LINE__,lCount);
	}
	/**去掉，不合理。wangwk-20110117
	if ( cHand_flag[0] == '1' )	---导盘---
		lCount=lCount-1;
		**/
	vtcp_log("%s,%d,统计笔数[%d]前台传笔数[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount == 0 )
	{
		sprintf(acErrMsg,"没有需要批量开户的记录!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H017" );
		goto ErrExit;
	}
	fclose ( fp );
	vtcp_log("%s,%d,统计笔数[%d]前台传笔数[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"总笔数与分户明细合计笔数不相等!!!" );
		WRITEMSG
		sprintf(acErrMsg,"明细笔数为[%d]", lCount );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"明细余额为[%.2lf]dT_amt=[%.2lf]", dTmp_amt,dT_amt);
	WRITEMSG
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"汇总金额[%.2lf]与明细总金额为[%.2lf]",dTmp_amt,dT_amt);
		WRITEMSG
		sprintf(acErrMsg,"汇总金额[%.2lf]与明细总金额为[%.2lf]",dTmp_amt,dT_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	memset(&sMdm_prdt_rel,0,sizeof(struct mdm_prdt_rel_c));
	ret = Mdm_prdt_rel_Sel(g_pub_tx.reply,&sMdm_prdt_rel,"prdt_no='%s' and mdm_code1 ='0'||'%s'",cPrdt_code,cNote_type);
	if(ret)
	{
		vtcp_log("%s,%d,产品与介质不存在关联关系!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"Z905");
		goto ErrExit;
	}
	if ( strcmp( cNote_type, "020" ) == 0 )		/**卡介质需要下凭证**/
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
	/****这种模式的话就要重新计算柜员可用凭证了，因为不是真正下柜员凭证**/
	{
		ret=Note_mst_Dec_Sel(g_pub_tx.reply,"tel='%s' and note_type='%s' and sts='0' and cnt>0\
			order by beg_note_no",g_pub_tx.tel,cNote_type);
		if(ret){
			sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		
		ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
		if(ret){
			sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
	}
	iOpcnt=0;
	lCount=0;
	/***逐条检查****/
	while(1)
	{
		memset(tmpstr,0,sizeof(tmpstr));
		if(feof(fp))
		{
			break;
		}
		fgets( tmpstr,1024,fp );
		/**if( feof(fp) )**/
		if(0) 
		{
			break;
			vtcp_log("%s,%d,处理最后一行!",__FILE__,__LINE__);
			memset(fldstr,0x0,sizeof(fldstr));
			if( cHand_flag[0] != '0')
				{
					for( i=0;i<23;i++ )/*改成23 神码要求加上单位编码,个人代理编号，开户金额后就是23个**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
				}
				else
				{/**手工开卡**/
						/**转化一下前台输入顺序-wangwenkui-20101226**/
						transfer(tmpstr,fldstr[0]);
				}
		/**转化一下前台输入顺序**/
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,最后一行为非法行!",__FILE__,__LINE__);
				vtcp_log("%s,%d,lCount=[%d]",__FILE__,__LINE__,lCount);	
				break;
			}
			pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if ( strlen(fldstr[0]) == 0  || strlen(fldstr[2]) == 0
		   ||strlen(fldstr[3]) == 0  || strlen(fldstr[4]) == 0
		   ||strlen(fldstr[14]) == 0 || strlen(fldstr[20]) == 0
		   ||strlen(fldstr[21]) == 0 )
		{
			if(strlen(fldstr[0]) == 0){
				sprintf( cReturn, "第[%d]行，卡号不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，卡号不能为空",lCount);
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
			
			/*其中任何一项为空认为提示报错*/
			iErr_flag = 1;
			ret = 1;
			strcpy( g_pub_tx.reply, "H033" );
			WRITEMSG
			goto TrLogEnd;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(memcmp(cUnit_no,fldstr[20],4)){
			/**单位编号输入错误*提示报错**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，单位编号输入不正确",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "第[%d]行，单位编号输入不正确",lCount);
			WRITEMSG
			goto TrLogEnd;
		}
		
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		memset(&z_cif_email_inf,0x00,sizeof(z_cif_email_inf));
		memset(&z_cif_email_inf1,0x00,sizeof(z_cif_email_inf1));
		memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
		memset( &sAg_peritemmap, 0x00,sizeof(sAg_peritemmap) );
		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_mger_rel, 0x0, sizeof(cif_mger_rel));
		memset(&z_cif_addr_inf,0x00,sizeof(z_cif_addr_inf));
		memset(&z_cif_per_inf,0x00,sizeof(z_cif_per_inf));
		
	    ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[21] );
			vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if ( ret == 0 )
		{
			/*----同一编号对应下的身份证号码已经存在则不允许开户--这个地方要韩希超和行里定一下？可以不一个人多开户吗？如果多开的话，原来的发工资账号如何处理？--*/
			pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "个人代理编号%s已有对应账号不能重新开户", fldstr[21]);
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "个人代理编号%s已有对应账号不能重新开户", fldstr[21]);
				WRITEMSG
				goto TrLogEnd;
			}
			/*---------------------------------------------*/
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"查询ag_peritemmap出错[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}
	    
		strcpy(cif_id_code_rel.id_type, fldstr[2]);/* 证件类型 */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* 证件号码 */
		pub_base_strpack(cif_id_code_rel.id_type);
		pub_base_strpack(cif_id_code_rel.id_no);
		vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[2]);
		vtcp_log("[%s][%d]证件号码[%s]\n",__FILE__,__LINE__,fldstr[3]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		strcpy(cif_basic_inf.type,"1");		      /* 客户类型 */
		strcpy(cif_basic_inf.name,fldstr[4]);	  /* 客户名称 */
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
			ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(ret == 0)
			{
				pub_base_strpack(fldstr[4]);
				pub_base_strpack(cif_basic_inf.name);				
				if(strcmp(cif_basic_inf.name,fldstr[4]) != 0)
				{
					/**客户户名不符*提示报错**/
					iErr_flag = 1;
					ret = 1;
					sprintf( cReturn, "第[%d]行，户名不符合，数据库中为【%s】",lCount,cif_basic_inf.name);
					strcpy( g_pub_tx.reply, "H033" );
					sprintf( acErrMsg, "第[%d]行，户名不符合，数据库中为【%s】",lCount,cif_basic_inf.name);
					WRITEMSG
					goto TrLogEnd;
				}
			}
			else if( ret )
			{
				sprintf(acErrMsg,"取客户信息异常![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*对已存在的客户号赋值*/
			/***将需要的信息从数据库中查询出来**/
			/* 登记客户基本信息表 */
			ret = Cif_addr_inf_Sel(g_pub_tx.reply,&z_cif_addr_inf,"cif_no=%d",cif_no);
			if( ret && ret!=100)
			{
				sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
				WRITEMSG
				/**
				strcpy(g_pub_tx.reply,"C067");
				goto ErrExit;**/
			}
			ret = Cif_per_inf_Sel(g_pub_tx.reply,&z_cif_per_inf,"cif_no=%d",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"查询个人客户信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf,"cif_no=%d and addr_type='5'",cif_no);
			if( ret )/**手机**/
			{
				sprintf(acErrMsg,"个人客户电子信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C062");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf1,"cif_no=%d and addr_type='6'",cif_no);
			if( ret )/**邮箱**/
			{
				sprintf(acErrMsg,"客户电子地址信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;**/
			}
			goto OK_cif_no_End;	/*直接开户*/
		}else if( ret!=100 )
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
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	

		ret = Cif_id_code_rel_Ins( cif_id_code_rel ,g_pub_tx.reply);
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
		ret = Cif_basic_inf_Ins( cif_basic_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/**登记客户地址信息表**/
		z_cif_addr_inf.cif_no=cif_no;
		z_cif_addr_inf.addr_seqn=1;
		z_cif_addr_inf.addr_type[0]='2';
		memcpy(z_cif_addr_inf.addr,fldstr[11],sizeof(z_cif_addr_inf.addr)-1);
		memcpy(z_cif_addr_inf.post_code,fldstr[8],sizeof(z_cif_addr_inf.post_code)-1);
		ret = Cif_addr_inf_Ins( z_cif_addr_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
		}
		
		/**登记个人客户信息表**/
		z_cif_per_inf.cif_no=cif_no;
		z_cif_per_inf.g_bir=atol(fldstr[18]);
		memcpy(z_cif_per_inf.sex,fldstr[6],1);
		memcpy(z_cif_per_inf.marriage,fldstr[19],1);/**需要和神码约定具体值 代表什么意思，如果不一致需要转换**/
		memcpy(z_cif_per_inf.name,fldstr[4],sizeof(z_cif_per_inf.name)-1);
		memcpy(z_cif_per_inf.nationality,"01",sizeof(z_cif_per_inf.nationality)-1);
		memcpy(z_cif_per_inf.country,"01",sizeof(z_cif_per_inf.country)-1);
		z_cif_per_inf.mon_income=atof(fldstr[17]);
		ret = Cif_per_inf_Ins( z_cif_per_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"个人客户信息表![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C008");
			goto ErrExit;
		}
		sprintf(acErrMsg,"建立客户信息成功,客户号[%ld]",cif_no);/*生成客户号*/
		WRITEMSG
		/**客户电子地址信息表**/
		z_cif_email_inf.cif_no=cif_no;
		ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
		z_cif_email_inf.addr_seqn=ret+1;
		z_cif_email_inf.addr_type[0]='5';/**手机**/
		memcpy(z_cif_email_inf.email,fldstr[14],sizeof(z_cif_email_inf.email)-1);
		ret = Cif_email_inf_Ins( z_cif_email_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"个人客户信息表![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(strlen(fldstr[15])){/**电子邮件**/
			z_cif_email_inf1.cif_no=cif_no;
			ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
			z_cif_email_inf1.addr_seqn=ret+1;
			z_cif_email_inf1.addr_type[0]='6';/**电子邮件**/
			memcpy(z_cif_email_inf1.email,fldstr[15],sizeof(z_cif_email_inf1.email)-1);
			ret = Cif_email_inf_Ins( z_cif_email_inf1 ,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"个人客户信息表![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;
			}
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
OK_cif_no_End:
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		/**按照开卡张数进行计算凭证号码**/
		vtcp_log("%s,%d iOpcnt=[%d],note_mst_z.cnt=[%d]\n",__FILE__,__LINE__,iOpcnt,note_mst_z.cnt);
		if(iOpcnt>note_mst_z.cnt){/***需要换另一本凭证了**/
			ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			if(ret==100){
				vtcp_log("%s,%d 凭证不够了，不应该啊！！！\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			iOpcnt=iOpcnt-note_mst_z.cnt;/***重新计算**/
		}
		vtcp_log("%s,%d sNote_min_no.note_min_no={%s},iOpcnt={%d}\n",__FILE__,__LINE__,sNote_min_no.note_min_no,iOpcnt);
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
		ret=pub_com_CalNoteNo("020",note_mst_z.beg_note_no,sNote_min_no.note_min_no,iOpcnt);
		if(ret){
			sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		vtcp_log("%s,%d 卡号[%s],凭证号[%s],ret=[%d]\n",__FILE__,__LINE__,fldstr[0],sNote_min_no.note_min_no+4,ret);
		/***开始比较卡号的凭证号与柜员的凭证号是否一致**/
		if(memcmp(fldstr[0]+6,sNote_min_no.note_min_no+4,12)){/*卡号7-18位为凭证号码**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，卡号[%s]与凭证号不符[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "第[%d]行，卡号[%s]与凭证号不符[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			WRITEMSG
			goto TrLogEnd;
		}
		vtcp_log("%s,%d 看看结果吧！ret=[%d]\n",__FILE__,__LINE__,ret);
TrLogEnd:	
		vtcp_log("%s,%d 看看结果吧！ret=[%d]\n",__FILE__,__LINE__,ret);
		if ( ret == 0 )
		{
			fprintf( fp1, "%19s|%12d|%1s|%20s|%30s|\
				%30s|%1s|%140s|%6s|\
				%15s|%60s|%50s|%6s|\
				%15s|%15s|%100s|%1s|%17.2f|%8s|%1s|\
				%4s|%20s|%17s|\n",
				fldstr[0],cif_no,fldstr[2],fldstr[3],fldstr[4],\
				"",z_cif_per_inf.sex,z_cif_addr_inf.addr,z_cif_addr_inf.post_code,\
				"",sAg_unitinfo.unit_name,"","",\
				"",fldstr[14],fldstr[15],fldstr[16],z_cif_per_inf.mon_income,fldstr[18],z_cif_per_inf.marriage,\
				cUnit_no,fldstr[21],fldstr[22]);
		}
		else
		{
			/**fprintf( fp2, "    │%10s│%10s│%20s│%20s│%44s│\n", "代理编号", "客户姓名", "待开卡号", "证件号码", "错误信息" );**/
			fprintf( fp2, "    │%10s│%10s│%20s│%20s│%44s│\n", fldstr[21], fldstr[4], \
			fldstr[0], fldstr[3], cReturn );
		}
		
		lCount = lCount + 1;
		iOpcnt++;
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		break;
		}
		
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset(fldstr,0x0,sizeof(fldstr));
		if( cHand_flag[0] != '0')
		{
			for( i=0;i<23;i++ )/*改成23 神码要求加上单位编码,个人代理编号，开户金额后就是23个**/
			{
				ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
				pub_base_strpack( fldstr[i] );
			}
		}else
		{
			/**手工开卡**/
						/**转化一下前台输入顺序-wangwenkui-20101226**/
						transfer(tmpstr,fldstr[0]);
		}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,此行为非法行!",__FILE__,__LINE__);
				continue;
			}
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
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
		    fldstr[22]  开户金额opnamt    ok
		----------------------------------------------*/
		if ( strlen(fldstr[0]) == 0  || strlen(fldstr[2]) == 0
		   ||strlen(fldstr[3]) == 0  || strlen(fldstr[4]) == 0
		   ||strlen(fldstr[14]) == 0 || strlen(fldstr[20]) == 0
		   ||strlen(fldstr[21]) == 0 )
		/**应行里要求,必须为零**/
		{
			if(strlen(fldstr[0]) == 0){
				sprintf( cReturn, "第[%d]行，卡号不能为空",lCount);
				sprintf( acErrMsg, "第[%d]行，卡号不能为空",lCount);
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
			if(strlen(fldstr[22]) != 0&& atof(fldstr[22])!= 0.0)
			{
				sprintf( cReturn, "第[%d]行，开户金额必须为空",lCount);
				sprintf( acErrMsg, "第[%d]行，开户金额必须为空",lCount);
			}
			/*其中任何一项为空认为提示报错*/
			iErr_flag = 1;
			ret = 1;
			strcpy( g_pub_tx.reply, "H033" );
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(memcmp(cUnit_no,fldstr[20],4)){
			/**单位编号输入错误*提示报错**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，单位编号输入不正确",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "第[%d]行，单位编号输入不正确",lCount);
			WRITEMSG
			goto TrLog;
		}
		
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		memset(&z_cif_email_inf,0x00,sizeof(z_cif_email_inf));
		memset(&z_cif_email_inf1,0x00,sizeof(z_cif_email_inf1));
		memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
		memset( &sAg_peritemmap, 0x00,sizeof(sAg_peritemmap) );
		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_mger_rel, 0x0, sizeof(cif_mger_rel));
		memset(&z_cif_addr_inf,0x00,sizeof(z_cif_addr_inf));
		memset(&z_cif_per_inf,0x00,sizeof(z_cif_per_inf));
		
	    ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[21] );
			vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if ( ret == 0 )
		{
			/*----同一编号对应下的身份证号码已经存在则不允许开户--这个地方要韩希超和行里定一下？可以不一个人多开户吗？如果多开的话，原来的发工资账号如何处理？--*/
			pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "个人代理编号%s已有对应账号不能重新开户", fldstr[21]);
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "个人代理编号%s已有对应账号不能重新开户", fldstr[21]);
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
		/**开始查询客户号是否存在，不存在则重新开立**/
		
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
		    fldstr[21]  开户金额    ok
		----------------------------------------------*/
		
	    
		strcpy(cif_id_code_rel.id_type, fldstr[2]);/* 证件类型 */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* 证件号码 */
		pub_base_strpack(cif_id_code_rel.id_type);
		pub_base_strpack(cif_id_code_rel.id_no);
		vtcp_log("[%s][%d]证件类型[%s]\n",__FILE__,__LINE__,fldstr[2]);
		vtcp_log("[%s][%d]证件号码[%s]\n",__FILE__,__LINE__,fldstr[3]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		strcpy(cif_basic_inf.type,"1");		      /* 客户类型 */
		strcpy(cif_basic_inf.name,fldstr[4]);	  /* 客户名称 */
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
			fldstr[22]  开户金额    ok
			----------------------------------------------*/
			/****增加对户名的判断****/
			ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(ret == 0)
			{
				pub_base_strpack(fldstr[4]);
				pub_base_strpack(cif_basic_inf.name);				
				if(strcmp(cif_basic_inf.name,fldstr[4]) != 0)
				{
					/**客户户名不符*提示报错**/
					iErr_flag = 1;
					ret = 1;
					sprintf( cReturn, "第[%d]行，户名不符合，数据库中为【%s】",lCount,cif_basic_inf.name);
					strcpy( g_pub_tx.reply, "H033" );
					sprintf( acErrMsg, "第[%d]行，户名不符合，数据库中为【%s】",lCount,cif_basic_inf.name);
					WRITEMSG
					goto TrLog;
				}
			}
			else if( ret )
			{
				sprintf(acErrMsg,"取客户信息异常![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*对已存在的客户号赋值*/
			/***将需要的信息从数据库中查询出来**/
			/* 登记客户基本信息表 */
			ret = Cif_addr_inf_Sel(g_pub_tx.reply,&z_cif_addr_inf,"cif_no=%d",cif_no);
			if( ret && ret!=100)
			{
				sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
				WRITEMSG
				/**
				strcpy(g_pub_tx.reply,"C067");
				goto ErrExit;**/
			}
			ret = Cif_per_inf_Sel(g_pub_tx.reply,&z_cif_per_inf,"cif_no=%d",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"查询个人客户信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf,"cif_no=%d and addr_type='5'",cif_no);
			if( ret )/**手机**/
			{
				sprintf(acErrMsg,"个人客户电子信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C062");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf1,"cif_no=%d and addr_type='6'",cif_no);
			if( ret )/**邮箱**/
			{
				sprintf(acErrMsg,"客户电子地址信息表![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;**/
			}
			goto OK_cif_no;	/*直接开户*/
		}else if( ret!=100 )
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
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	

		ret = Cif_id_code_rel_Ins( cif_id_code_rel ,g_pub_tx.reply);
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
		ret = Cif_basic_inf_Ins( cif_basic_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/**登记客户地址信息表**/
		z_cif_addr_inf.cif_no=cif_no;
		z_cif_addr_inf.addr_seqn=1;
		z_cif_addr_inf.addr_type[0]='2';
		memcpy(z_cif_addr_inf.addr,fldstr[11],sizeof(z_cif_addr_inf.addr)-1);
		memcpy(z_cif_addr_inf.post_code,fldstr[8],sizeof(z_cif_addr_inf.post_code)-1);
		ret = Cif_addr_inf_Ins( z_cif_addr_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
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
		/**登记个人客户信息表**/
		z_cif_per_inf.cif_no=cif_no;
		z_cif_per_inf.g_bir=atol(fldstr[18]);
		memcpy(z_cif_per_inf.sex,fldstr[6],1);
		memcpy(z_cif_per_inf.marriage,fldstr[19],1);/**需要和神码约定具体值 代表什么意思，如果不一致需要转换**/
		memcpy(z_cif_per_inf.name,fldstr[4],sizeof(z_cif_per_inf.name)-1);
		memcpy(z_cif_per_inf.nationality,"01",sizeof(z_cif_per_inf.nationality)-1);
		memcpy(z_cif_per_inf.country,"01",sizeof(z_cif_per_inf.country)-1);
		z_cif_per_inf.mon_income=atof(fldstr[17]);
		ret = Cif_per_inf_Ins( z_cif_per_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"个人客户信息表![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C008");
			goto ErrExit;
		}
		sprintf(acErrMsg,"建立客户信息成功,客户号[%ld]",cif_no);/*生成客户号*/
		WRITEMSG
		/**客户电子地址信息表**/
		z_cif_email_inf.cif_no=cif_no;
		ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
		z_cif_email_inf.addr_seqn=ret+1;
		z_cif_email_inf.addr_type[0]='5';/**手机**/
		memcpy(z_cif_email_inf.email,fldstr[14],sizeof(z_cif_email_inf.email)-1);
		ret = Cif_email_inf_Ins( z_cif_email_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"个人客户信息表![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(strlen(fldstr[15])){/**电子邮件**/
			z_cif_email_inf1.cif_no=cif_no;
			ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
			z_cif_email_inf1.addr_seqn=ret+1;
			z_cif_email_inf1.addr_type[0]='6';/**电子邮件**/
			memcpy(z_cif_email_inf1.email,fldstr[15],sizeof(z_cif_email_inf1.email)-1);
			ret = Cif_email_inf_Ins( z_cif_email_inf1 ,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"个人客户信息表![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;
			}
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
OK_cif_no:
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		/**按照开卡张数进行计算凭证号码**/
		vtcp_log("%s,%d iOpcnt=[%d],note_mst_z.cnt=[%d]\n",__FILE__,__LINE__,iOpcnt,note_mst_z.cnt);
		if(iOpcnt>note_mst_z.cnt){/***需要换另一本凭证了**/
			ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			if(ret==100){
				vtcp_log("%s,%d 凭证不够了，不应该啊！！！\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			iOpcnt=iOpcnt-note_mst_z.cnt;/***重新计算**/
		}
		vtcp_log("%s,%d sNote_min_no.note_min_no={%s},iOpcnt={%d}\n",__FILE__,__LINE__,sNote_min_no.note_min_no,iOpcnt);
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
		ret=pub_com_CalNoteNo("020",note_mst_z.beg_note_no,sNote_min_no.note_min_no,iOpcnt);
		if(ret){
			sprintf(acErrMsg,"查询柜员凭证台账表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		/***开始比较卡号的凭证号与柜员的凭证号是否一致**/
		vtcp_log("%s,%d 卡号【%s】与凭证号[%s]\n",__FILE__,__LINE__,fldstr[0],sNote_min_no.note_min_no+4);
		if(memcmp(fldstr[0]+6,sNote_min_no.note_min_no+4,12)){/*卡号7-18位为凭证号码**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "第[%d]行，卡号【%s】与凭证号不符[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "第[%d]行，卡号[%s]与凭证号不符[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d 看看结果吧！ret=[%d]\n",__FILE__,__LINE__,ret);
TrLog:	
		vtcp_log("%s,%d 看看结果吧！ret=[%d]\n",__FILE__,__LINE__,ret);
		if ( ret == 0 )
		{
			/***神码的要求生成文件fp1*
			卡号|客户号|持卡人证件种类|持卡人证件号码|持卡人客户名|
			持卡人客户英文名|持卡人性别|持卡人家庭住址|持卡人家庭邮编|
			持卡人家庭电话|持卡人单位名称|持卡人单位地址|持卡人单位邮编|
			持卡人单位电话|持卡人手机号码|持卡人邮箱|持卡人信誉等级|月收入|出生日期|婚姻状况***/
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
			fldstr[22]  开户金额opnamt    ok
		----------------------------------------------*/
			fprintf( fp1, "%19s|%12d|%1s|%20s|%30s|\
				%30s|%1s|%140s|%6s|\
				%15s|%60s|%50s|%6s|\
				%15s|%15s|%100s|%1s|%17.2f|%8s|%1s|\
				%4s|%20s|%17s|\n",
				fldstr[0],cif_no,fldstr[2],fldstr[3],fldstr[4],\
				"",z_cif_per_inf.sex,z_cif_addr_inf.addr,z_cif_addr_inf.post_code,\
				"",sAg_unitinfo.unit_name,"","",\
				"",fldstr[14],fldstr[15],fldstr[16],z_cif_per_inf.mon_income,fldstr[18],z_cif_per_inf.marriage,\
				cUnit_no,fldstr[21],fldstr[22]);
		}
		else
		{
			/**fprintf( fp2, "    │%10s│%10s│%20s│%20s│%44s│\n", "代理编号", "客户姓名", "待开卡号", "证件号码", "错误信息" );**/
			fprintf( fp2, "    │%10s│%10s│%20s│%20s│%44s│\n", fldstr[21], fldstr[4], \
			fldstr[0], fldstr[3], cReturn );
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		lCount = lCount + 1;
		iOpcnt++;
	}
	
	fprintf( fp2, "    ┕━━━━━┷━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━┙\n" );

	fprintf(fp2,"\n\n");
	fprintf(fp2,"批量开户导盘失败!!!请检查以上记录错误!!!!\n");

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	if ( iErr_flag == 1 )/**至少保证录入或导盘时所有的记录都能过去,否则和神码那边更麻烦**/
	{
		strcpy( g_pub_tx.reply, "H086" );
		goto ErrExit;
	}
	set_zd_data("0260",cFilename);/**用于给平台传送文件**/
	/**添加一个shell用于 核心机器传送到金融交换平台***/
	vtcp_log("%s,%d cFilename=[%s]\n",__FILE__,__LINE__,cFilename);
        /****修改为新的加密方式 20120604
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s",getenv("WORKDIR"),cFilename);
	****/
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s %s %s",getenv("WORKDIR"),get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",cFilename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftp文件[%s]到平台成功",cFilename);
	WRITEMSG

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
/**
**转化前台的文件域的顺序
**/
int transfer(char *buf,char *fdstr)
{
		int i;
		int ret;
		char cFldstr[30][150];/**神码要求是20个内容暂时定义为30，最长的为140，所以暂时定为150***/
		memset(cFldstr, 0, sizeof(cFldstr));
		vtcp_log("[%s][%d] 处理内容【%s】 ", __FILE__, __LINE__,buf);
		for( i=0;i<9;i++ )/*改成23 神码要求加上单位编码,个人代理编号，开户金额后就是23个**/
		{
			ret=pub_base_cut_str( buf,cFldstr[i],"|",i+1 );
			vtcp_log("[%s][%d] [%d] [%s] ", __FILE__, __LINE__, i+1, cFldstr[i]);
			pub_base_strpack(cFldstr[i]);
		}
		vtcp_log("[%s][%d] cFldstr[7]->%s 结束 ", __FILE__, __LINE__,cFldstr[7]);
		memcpy(fdstr, cFldstr[7],  strlen(cFldstr[7]));/**卡号**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+2*150, cFldstr[6],  strlen(cFldstr[6]));/**持卡人证件种类**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+3*150, cFldstr[4],  strlen(cFldstr[4]));/**持卡人证件号码**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+4*150, cFldstr[2],  strlen(cFldstr[2]));/**持卡人客户名**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+14*150, cFldstr[8], strlen(cFldstr[8]));/**持卡人手机号码**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+20*150, cFldstr[5], strlen(cFldstr[5]));/**单位编号**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+21*150, cFldstr[0], strlen(cFldstr[0]));/**个人代理编号itemno**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
		memcpy(fdstr+22*150, cFldstr[3], strlen(cFldstr[3]));/**开户金额opnamt**/
		vtcp_log("[%s][%d] transfer 结束 ", __FILE__, __LINE__);
			
		return 0;
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
			fldstr[22]  开户金额opnamt    ok
		----------------------------------------------*/
