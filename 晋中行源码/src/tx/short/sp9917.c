/*************************************************
* 文 件 名: sp9917.c
* 功能描述：根据介质种类返回支取方式,下载能使用的产品
*
* 作    者:    
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
#include "mdm_attr_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"

sp9917()  
{ 		
	int ret=0;
	int vcnt=0;
	int i=0;
	char mdm_code[5];
	char filename[150];
	char wherelist[150];
	FILE *fp;

	struct mdm_attr_c g_mdm_attr;
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_id_code_rel_c      cif_id_code_rel_tmp; 
	struct cif_basic_inf_c        cif_basic_inf; 

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset (&cif_id_code_rel_tmp, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

	/** 取值、赋值 **/	
	get_zd_data("1161",mdm_code);							/* 介质种类 */
	get_zd_data( "1164" , cif_id_code_rel_tmp.id_type );	/* 证件类型 */
	get_zd_data( "1165" , cif_id_code_rel_tmp.id_no );		/* 证件号码 */
	
	/* 根据介质种类取相关参数*/
	ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'",mdm_code);
  	if( ret==100 )
	{
	  	sprintf(acErrMsg,"不存在该凭证种类!!mdm_code=[%s]",mdm_code);
	  	WRITEMSG
		strcpy(g_pub_tx.reply,"H009");
		goto ErrExit;
	}
	vtcp_log("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%s][%s][%s]\n",g_mdm_attr.draw_pwd_yn,g_mdm_attr.draw_id_yn,g_mdm_attr.pwd_mach_yn,g_mdm_attr.qry_pwd_yn);
	/* 输出变量 */
	/***将密码支取方式默认为'Y'**modify by wanglei 20060916***/
	if(g_mdm_attr.draw_pwd_yn[0]=='O')
		strcpy(g_mdm_attr.draw_pwd_yn,"Y");

	if(g_mdm_attr.draw_id_yn[0]=='O')
		strcpy(g_mdm_attr.draw_id_yn,"N");

	if(g_mdm_attr.pwd_mach_yn[0]=='O')
		strcpy(g_mdm_attr.pwd_mach_yn,"N");

	set_zd_data("1155", g_mdm_attr.draw_id_yn);
	set_zd_data("1159", g_mdm_attr.qry_pwd_yn);
	set_zd_data("1152", g_mdm_attr.draw_pwd_yn);
	set_zd_data("115A", g_mdm_attr.pwd_mach_yn);

	sprintf(acErrMsg,"AZZ=================g_mdm_attr.no_ind[%s]", \
			g_mdm_attr.no_ind);
	WRITEMSG
	set_zd_data("1169", g_mdm_attr.no_ind);			/*介质种类*/

	/* 根据证件类型和证件号码取客户号 */
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
		  "id_type='%s' and id_no='%s'",cif_id_code_rel_tmp.id_type, \
		  cif_id_code_rel_tmp.id_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件信息没有登记,请到客户中心登记[%s]", \
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C002");
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
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}

	/*1：个人； 2：公司 ；3：机构（指政府机关等组织）； 
	4：金融机构； 5：内部员工 */
	/*0：公用；1：个人； 2：公司； 5内部员工*/
	if( cif_basic_inf.type[0]=='1' )
		sprintf( wherelist," cif_type in('0','1') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') ", mdm_code,mdm_code,mdm_code );
	if( cif_basic_inf.type[0]=='2' )
		sprintf( wherelist," cif_type in('0','2') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') ", mdm_code,mdm_code,mdm_code );
	if( cif_basic_inf.type[0]=='3' )
		sprintf( wherelist," cif_type in('0','2') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') ", mdm_code,mdm_code,mdm_code );
	if( cif_basic_inf.type[0]=='4' )
		sprintf( wherelist," cif_type in('0','2') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') ", mdm_code,mdm_code,mdm_code );
	
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

	/*** 生成产品列表 ***/
	ret=Dd_parm_Dec_Sel( g_pub_tx.reply," %s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Dd_parm_Fet_Sel( &g_dd_parm , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1031",g_dd_parm.prdt_no);
			set_zd_data("1032",g_dd_parm.title);
		}
    /**add by zyl 晋中前台3101用.屏蔽prdt_no=107 七天乐虚拟账户.**/
    if(strcmp(g_dd_parm.prdt_no,"107") == 0)
    	continue;
    /**end by zyl**/
		pub_base_strpack(g_dd_parm.title);
		fprintf( fp,"1031|%-20s|%-3s|\n",g_dd_parm.title,g_dd_parm.prdt_no );

		i++;
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

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
