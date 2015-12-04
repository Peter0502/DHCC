/*************************************************
* 文 件 名: spF624.c
* 功能描述：缴税批量开户业务
*
* 作    者: jack
* 完成日期: 2004年06月25日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*--- 证件类型默认为1 ---*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "note_min_no_c.h"

spF624()
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
	double dTx_amt = 0.00;
	double dT_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;

	char filename[100];
	char filename1[100];
	char fldstr[6][100];
	char cSign[2];
	char tmpstr[1024];
	FILE *fp;
	FILE *fp1;
	int i=0;
	
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	long lBeg_note_no = 0;
	char cNote_tmp[16];
	char cNote_tmp1[9];
	int iLen = 0;

	long lCount = 0;
	long lRnum = 0;

	char cMessage[41];
	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct note_min_no_c sNote_min_no;

	memset(cPrdt_code,0x0,sizeof(cPrdt_code));
	memset(cBeg_card_no,0x0,sizeof(cBeg_card_no));
	memset(cEnd_card_no,0x0,sizeof(cEnd_card_no));
	memset(cCard_no,0x0,sizeof(cCard_no));
	memset(cNote_no,0x0,sizeof(cNote_no));
	memset(cz_acno,0x0,sizeof(cz_acno));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cNote_type,0x0,sizeof(cNote_type));

	memset(cMessage, 0x0, sizeof(cMessage) );

	get_zd_data("0230",cPrdt_code);
	get_zd_data("0920",cUnit_no);
	get_zd_data("0900",cNote_type);

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/*------------------	分隔符	----------------*/
	memset( cSign, 0x0, sizeof(cSign) );
	cSign[0] = '\t';
	cSign[1] = '\0';
	
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "010") ) /* 存折 */
	{
		strcpy( cRctp, "存折" );
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
	
	memset(filename1,0x0,sizeof(filename1));
    sprintf( filename1, "%s/Banksyyh%ld0001.txt", getenv("FILDIR"), \
		g_pub_tx.tx_date );
    fp1 = fopen( filename1,"w" );   /*该文件用来记录开户正确处理结果*/
    if( fp1==NULL )
    {
       	sprintf(acErrMsg," open file [%s] error ",filename1 );
       	WRITEMSG
       	strcpy( g_pub_tx.reply,"S047" );
       	goto ErrExit;
    }
    	
    pub_base_strpack( sAg_unitinfo.unit_name );

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg,"----->打开开户资料文件[%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
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
		for( i=0;i<5;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],cSign,i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-	fldstr[0] 税号 ---------------------------------
			fldstr[1] 纳税名称
			fldstr[2] 别名
			fldstr[3] 开户人名称
			fldstr[4] 证件号码
		--------------------------------------------------*/

		lCount = lCount + 1;
	}
	fclose ( fp );
	
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

	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<5;i++ )
		{
			ret=pub_base_cut_str( tmpstr, fldstr[i], cSign, i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-	fldstr[0] 税号 ---------------------------------
			fldstr[1] 纳税名称
			fldstr[2] 别名
			fldstr[3] 开户人名称
			fldstr[4] 证件号码
		--------------------------------------------------*/
		
		/**------------ 每次循环都重新初始化全局变量 -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		dTx_amt = 0.00;

		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			/*--------待确认是否允许开户-------------------
			pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[4] ) == 0 )
			{
				sprintf(acErrMsg,"该客户[%s]记录已存在不能重新开户", fldstr[0] );
				WRITEMSG
				goto ErrExit;
			}
			*---------------------------------------------*/
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

		strcpy(cif_id_code_rel.id_type, cIdtp);/* 证件类型 */
		strcpy(cif_id_code_rel.id_no,fldstr[4]);  /* 证件号码 */

		strcpy(cif_basic_inf.type,"1");		      /* 客户类型 */
		strcpy(cif_basic_inf.name, fldstr[3]);	  /* 客户名称 */
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
			sprintf(acErrMsg,"该客户已经拥有客户ID![%s][%s]",cIdtp,fldstr[4]);
			WRITEMSG
			cif_no = cif_id_code_rel.cif_no; /*对已存在的客户号赋值*/
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
		ret = Cif_id_code_rel_Ins( cif_id_code_rel );
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
		strcpy( cif_basic_inf.sts , "1" ) ;  /* 客户状态：1正常 2注销 */
		cif_basic_inf.cif_seqn = 1 ;                /* 客户序号 */

		ret = Cif_basic_inf_Ins( cif_basic_inf );
		if( ret )
		{
			sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
	
		sprintf(acErrMsg,"建立客户信息成功,客户号[%ld]",cif_no);/*生成客户号*/
		WRITEMSG

OK_cif_no:

/*--------------------开介质------------------------*
		ret = pub_base_CrtAcChkb( cBeg_card_no, cCard_no, 3, 1);
		if( ret )
		{
			sprintf(acErrMsg,"生成卡号[%s]校验位出错![%d]",cBeg_card_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F005");
			goto ErrExit;
		}
		*------------------------------------------*/

		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		ret = Note_min_no_Sel( g_pub_tx.reply, &sNote_min_no, \
			"br_no = '%s' and tel = '%s' and note_type = '%s'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type );
		if( ret )
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
		if ( !strcmp(cNote_type, "020") ) /* 卡 */
		{
			strcpy(g_pub_tx.mdm_code,"0020");	
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );

			memset( cNote_no, 0x0, sizeof(cNote_no) );
			memcpy( cNote_no, sNote_min_no.note_min_no+6, 10 );

			strcpy(g_pub_tx.crd_no, "940071733" );
			strcat(g_pub_tx.crd_no, cNote_no );
		}

		strcpy(g_pub_tx.id_type, cIdtp);	
		strcpy(g_pub_tx.id_no, fldstr[4]);
		strcpy(g_pub_tx.name, fldstr[3]);

		strcpy(g_pub_tx.draw_uncon_yn,"N");
		strcpy(g_pub_tx.draw_pwd_yn,"Y");
		strcpy(g_pub_tx.qry_pwd,"000000");
		strcpy(g_pub_tx.draw_pwd,"000000");
		strcpy(g_pub_tx.draw_id_yn,"N");
		strcpy(g_pub_tx.draw_seal_yn,"N");

		strcpy(g_pub_tx.brf,"开介质");

		pub_base_strpack(g_pub_tx.crd_no);
		pub_base_strpack(g_pub_tx.beg_note_no);

		ret = pub_acct_opn_cif_mdm();
		if( ret )
		{
			sprintf(acErrMsg,"开介质错!!![%d][%s][%s][%s]",ret, cIdtp, fldstr[4],fldstr[3]);
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
		g_pub_tx.tx_amt1 = 0.00;		/*	开户金额为0	*/
		strcpy(g_pub_tx.name,fldstr[3]);
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.brf,"活期批量开户");
		
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
		set_zd_double( "1037", 0.00 );
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

		/*--------------- 写代理对应记录 ---------------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

		strcpy( sAg_peritemmap.bat_flag, "0" );
		strcpy( sAg_peritemmap.remark, "代缴税款" );

		strcpy( sAg_peritemmap.unit_no, cUnit_no );
		strcpy( sAg_peritemmap.item_no, fldstr[0] );
		strcpy( sAg_peritemmap.name, fldstr[3] );
		strcpy( sAg_peritemmap.id_type, cIdtp );
		strcpy( sAg_peritemmap.id_no, fldstr[4] );
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
				fldstr[3], fldstr[4], xs_acno, g_pub_tx.tx_date, \
				g_pub_tx.tel, cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf(acErrMsg,"修改ag_peritemmap失败[%s][%d]!!!",fldstr[0],ret );
				WRITEMSG
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
		
		fprintf( fp1, "%s\t%s\t%s\t%s\t%s\t%s\t%ld\n", fldstr[0], \
			fldstr[1], fldstr[2], xs_acno, fldstr[3], fldstr[4], \
			g_pub_tx.tx_date );
	}
	
	fclose(fp);
	fclose(fp1);
	
	if ( access( filename1, 0) )
	{
		sprintf(acErrMsg,"要访问的文件不存在[%s]", filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	memset( execstr, 0x0, sizeof(execstr) );
	sprintf( execstr, "cp %s %s", filename1, filename );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"更改下传文件名失败[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
		
	}

	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"批量开户成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose(fp);
    fclose(fp1);
    sprintf(acErrMsg,"批量开户失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
