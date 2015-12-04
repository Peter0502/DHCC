/*************************************************
* 文 件 名: sp9788.c
* 功能描述：根据帐号和序号回显相关信息_转存单交易_NO.2
* 作    者: rob
* 完成日期：2003年02月19日
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
#include "prdt_ac_id_c.h"
#include "mdm_prdt_rel_c.h"
#include "mdm_attr_c.h"

sp9788()  
{ 		
	int i=0,ret=0,ac_seqn=0;
	char title[40],cur_no[3];
	char ac_no[20],name[50],tmpname[100];
	char note_type[4];
	double bal;
	FILE *fp;

	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_prdt_rel_c g_mdm_prdt_rel;
	struct mdm_attr_c vmdm_attr;

	memset(&vmdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_prdt_rel,0x00,sizeof(struct mdm_prdt_rel_c));

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);							/*帐号*/
	get_zd_int("0340",&ac_seqn);						/*序号*/
	
	/* 调用根据帐号返回账户序号函数 */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"查找出错=[%ld]",ret );
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/* 查找介质种类 */
	ret = Mdm_attr_Sel( g_pub_tx.reply,&vmdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"查找出错!!ret=[%d]",ret);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy( note_type,vmdm_attr.note_type );
	memset( &vmdm_attr, 0x00, sizeof(struct mdm_attr_c) );

	ret = Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", \
					   g_mdm_ac_rel.ac_id,ac_seqn);
 	if( ret )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
			 	g_mdm_ac_rel.ac_id,ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"查找出错=[%ld]",ret );
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

 	if( g_td_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该账户已销!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;
	}

	ret = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
						g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该产品编号!prdt_no=[%s]",g_td_mst.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P145");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"查找出错=[%ld]",ret );
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	bal=g_td_mst.bal;
	strcpy(name,g_td_mst.name);
	strcpy(title,g_td_parm.title);
	strcpy(cur_no,g_td_parm.cur_no);
	
	/* 下载介质类型菜单 */
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	ret=Mdm_prdt_rel_Dec_Sel(g_pub_tx.reply,"prdt_no='%s'",g_td_parm.prdt_no);
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Mdm_prdt_rel_Fet_Sel(&g_mdm_prdt_rel,g_pub_tx.reply);
		if( ret==100 ) break;
		else if( ret ) goto ErrExit;

		ret = Mdm_attr_Sel(g_pub_tx.reply,&vmdm_attr,"mdm_code='%s' and \
						   mdm_code<>'0016' and mdm_code<>'0020'", \
						   g_mdm_prdt_rel.mdm_code1);

		pub_base_strpack(vmdm_attr.mdm_name);
		pub_base_strpack(vmdm_attr.mdm_code);

		if( i==0 )  set_zd_data("1161",vmdm_attr.mdm_code);
		fprintf(fp,"1161|%s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code);
		i++;
	}

	Mdm_prdt_rel_Clo_Sel();
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/*介质菜单下载完毕*/

	/* 输出变量 */
	set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("1148",g_mdm_ac_rel.id_type);
	set_zd_double("0420",bal);
	set_zd_data("0210",cur_no);
	set_zd_data("0240",g_td_mst.prdt_no);
	set_zd_data("0270",title);
	set_zd_data("1166",name);

	set_zd_data("0890",note_type);

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
