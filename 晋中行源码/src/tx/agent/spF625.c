/*************************************************
* 文 件 名: spF625.c
* 功能描述：缴税批量导入数据
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
#include "public.h"
#include <errno.h>
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"

spF625()
{ 	
	int ret=0;
	char execstr[256];

	char cUnit_no[5];
	char cSign[2];

	char filename[100];
	char filename1[100];
	char fldstr[12][100];
	char tmpstr[1024];
	FILE *fp;
	FILE *fp1;
	int i=0;

	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	cSign[0] = '\t';
	cSign[1] = '\0';

	get_zd_data( "0920", cUnit_no );

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该单位编号对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	
	memset( filename1, 0x0, sizeof(filename1) );
	sprintf( filename1, "%s/TAX_IN_ERR%s.txt", getenv("FILDIR"), cUnit_no );
	fp1 = fopen( filename1, "w" );
	if ( fp1 == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0; i<9; i++ )
		{
			ret=pub_base_cut_str( tmpstr, fldstr[i], cSign, i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		
		/*-------------一定要建立了对应关系,才能缴税---------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 100 )
		{
			/*fprintf( fp1, "`%s	@对应的信息不存在\n", tmpstr );*/
			fprintf( fp1, "`%s\n", tmpstr );
			fprintf( fp1, "`纳税编号对应的信息不存在\n", tmpstr );
			continue;
		}
		if ( ret )
		{
			sprintf( acErrMsg,"查询ag_peritemmap出错[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sAg_peritemmap.ac_no );
		pub_base_strpack( sAg_peritemmap.name );
		if ( strcmp( fldstr[6], sAg_peritemmap.ac_no ) )
		{
			/*-----------------------------------------------------*
			fprintf( fp1, "`%s	@账号不符,银行账户为[%s]-[%s] \n", \
				tmpstr, sAg_peritemmap.ac_no, sAg_peritemmap.name );
			*-----------------------------------------------------*/
			fprintf( fp1, "`%s\n", tmpstr );
			fprintf( fp1, "`账号不符,银行账户为[%s]-[%s] \n", \
				sAg_peritemmap.ac_no, sAg_peritemmap.name );
		}

		memset( &sPay_agent, 0x00, sizeof(sPay_agent) );
		ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			if ( sPay_agent.result[0] == '0' )
			{
				continue;	/*	该记录已缴费,不用处理	*/
			}

			/*------	以新文本为准	------*/
			ret = sql_execute( "delete from pay_agent where \
				unit_no = '%s' and item_no = '%s'", \
				cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf( acErrMsg,"删除pay_agent记录出错[%s]",fldstr[0] );
				WRITEMSG
				goto ErrExit;
			}
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"查询pay_agent出错[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPay_agent, 0x00, sizeof(sPay_agent) );
		strcpy( sPay_agent.unit_no, cUnit_no );
		strcpy( sPay_agent.item_no, fldstr[0] );
		strcpy( sPay_agent.name, sAg_peritemmap.name );
		strcpy( sPay_agent.id_type, "1" );
		strcpy( sPay_agent.id_no, sAg_peritemmap.id_no );
		strcpy( sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = 0;
		sPay_agent.end_date = atol( fldstr[4] );
		sPay_agent.tx_date = 99999999;
		sPay_agent.tram = atof( fldstr[3] );
		strcpy( sPay_agent.remark, fldstr[1] );
		strcpy( sPay_agent.sata, "1" );
		strcpy( sPay_agent.result, "1" );

		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply );
		if ( ret )
		{
    		sprintf(acErrMsg,"------>插入记录[%s]进pay_agent出错[%d]", fldstr[0], ret ); 
    		WRITEMSG
			goto ErrExit;
		}
	}
	fclose(fp);
	fclose(fp1);

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=1;
	pub_base_AllDwnFilName( filename );

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
    sprintf(acErrMsg,"------>批量导税务数据成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose(fp);
    fclose(fp1);
    sprintf(acErrMsg,"------>批量导税务数据失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
