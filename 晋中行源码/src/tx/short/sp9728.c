/*************************************************
*2103的证件快捷查询
* 文 件 名: sp9728.c
* 功能描述：根据证件类型和证件号码 返回客户姓名
*			返回
* 作    者: lance
* 完成日期：2003年01月13日
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

sp9728()  
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

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_id_code_rel_tmp, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/** 取值、赋值 **/	
	get_zd_data( "1164" , cif_id_code_rel_tmp.id_type);	/* 证件类型 */
	get_zd_data( "1165" , cif_id_code_rel_tmp.id_no );	/* 证件号码 */
	vtcp_log("[%s].........[%s]",cif_id_code_rel_tmp.id_type,cif_id_code_rel_tmp.id_no);
	
	/* 根据证件类型和证件号码取客户号 */
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
	" id_type='%s' and id_no='%s' " , cif_id_code_rel_tmp.id_type , cif_id_code_rel_tmp.id_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件信息没有登记,请到客户中心登记[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
					  
	/* 根据客户号查询客户名称 */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_id_code_rel.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件对应的客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	/*1：个人； 2：公司 ；3：机构（指政府机关等组织）； 
	4：金融机构； 5：内部员工 */
	/*0：公用；1：个人； 2：公司； 5内部员工*/
	if( cif_basic_inf.type[0]=='1' )
		sprintf( wherelist," cif_type in('0','1') " );
	if( cif_basic_inf.type[0]=='2' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='3' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='4' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='5' )
		sprintf( wherelist," cif_type in('0','1','5') " );
	
	vtcp_log("XXXXXXX=[%s]",cif_basic_inf.type);

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

	/***介质列表***/
	ret=Mdm_prdt_rel_Dec_Sel( g_pub_tx.reply,
		"prdt_no in(select prdt_no from td_parm where %s) and mdm_code2='%s' \
		and mdm_code3='%s' order by mdm_code1",wherelist,NULL_MDM,NULL_MDM);
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Mdm_prdt_rel_Fet_Sel( &vmdm_prdt_rel , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
		/*if ((strcmp(vmdm_prdt_rel.mdm_code1,"0012")==0)||(strcmp(vmdm_prdt_rel.mdm_code1,"0014")==0)||(strcmp(vmdm_prdt_rel.mdm_code1,"0015")==0))
	 	continue;*/	

		ret = Mdm_attr_Sel( g_pub_tx.reply,&vmdm_attr,"mdm_code='%s'",
							vmdm_prdt_rel.mdm_code1 );		
		if( ret ) continue;
		/**modify 20060913 屏蔽掉贺卡定期,特种定期,定活两便介质种类**/
		if ((strcmp(vmdm_attr.mdm_code,"0012")==0)||(strcmp(vmdm_attr.mdm_code,"0014")==0)||(strcmp(vmdm_attr.mdm_code,"0015")==0))
	 	continue;
		vtcp_log("!!!!!!!prdt_no[%s],code[%s]\n",vmdm_prdt_rel.prdt_no,vmdm_attr.mdm_code);
	 	/**end**/
		pub_base_strpack(vmdm_attr.mdm_name);
		if( strcmp(omdm_attr.mdm_code,vmdm_attr.mdm_code) )
		{
			if( i==0 )
				set_zd_data("1161",vmdm_attr.mdm_code);
			fprintf( fp,"1161|%-20s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code );
			i++;
		}
		strcpy( omdm_attr.mdm_code,vmdm_attr.mdm_code );
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();

	/***产品列表***
	ret=Td_parm_Dec_Sel( g_pub_tx.reply,"%s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Td_parm_Fet_Sel( &g_td_parm , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1031",g_td_parm.prdt_no);
			set_zd_data("1032",g_td_parm.title);
		}

		pub_base_strpack(g_td_parm.title);
		fprintf( fp,"1031|%-20s|%s|\n",g_td_parm.title,g_td_parm.prdt_no );

		i++;
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();
	***/

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
