/*************************************************
*2104的产品类型快捷查询
* 文 件 名:    sp9784.c
* 功能描述：   
* 作    者:    jack
* 完成日期：   2003年01月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_prdt_rel_c.h"
#include "mdm_attr_c.h"

	
sp9785()  
{ 		
	/** 数据初始化 **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_id_code_rel_c      cif_id_code_rel_tmp; 
	struct cif_basic_inf_c        cif_basic_inf; 
	struct mdm_prdt_rel_c        vmdm_prdt_rel; 
	struct mdm_attr_c        vmdm_attr; 
	struct mdm_attr_c        omdm_attr; 
	int ret=0;
	int vcnt=0;
	char wherelist[40];
	char filename[100];
	FILE *fp;
	int i=0;

	sprintf( wherelist," cif_type in('0','1','5') " );
	
	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	vtcp_log( "(prdt_no in(select prdt_no from dd_parm where %s) or prdt_no in(select prdt_no from td_parm where %s)) and mdm_code2='%s' and mdm_code3='%s' ",
		wherelist,wherelist,NULL_MDM,NULL_MDM);
	/***介质列表***/
	ret=Mdm_prdt_rel_Dec_Sel( g_pub_tx.reply,
		"(prdt_no in(select prdt_no from dd_parm where %s) or prdt_no in(select prdt_no from td_parm where %s)) and mdm_code2='%s' and mdm_code3='%s' order by mdm_code1",
		wherelist,wherelist,NULL_MDM,NULL_MDM);
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Mdm_prdt_rel_Fet_Sel( &vmdm_prdt_rel , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		ret = Mdm_attr_Sel( g_pub_tx.reply,&vmdm_attr,"mdm_code='%s'",
			vmdm_prdt_rel.mdm_code1 );
		if( ret ) continue;

		pub_base_strpack(vmdm_attr.mdm_name);
		if( strcmp(omdm_attr.mdm_code,vmdm_attr.mdm_code) )
		{
			if( i==0 )
				set_zd_data("1161",vmdm_attr.mdm_code);
			fprintf( fp,"1161|%s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code );
			i++;
		}
		strcpy( omdm_attr.mdm_code,vmdm_attr.mdm_code );
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();


	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* 输出变量 */
	set_zd_data( "1166", cif_basic_inf.name );
	vtcp_log("[%s]",cif_basic_inf.name);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
