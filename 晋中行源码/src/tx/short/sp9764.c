/*************************************************
* 文 件 名: sp9764.c
* 功能描述：根据帐号回显相关信息_增加卡折交易
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

sp9764()  
{ 		
	int i=0,ret=0,ac_seqn;
	char title[40],cur_no[3];
	char ac_no[21],name[50],tmpname[100];
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
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/
	
	/* 调用根据帐号返回账户序号函数 */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( strncmp(g_mdm_ac_rel.mdm_code,"0010",4) && \
		strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
	    sprintf(acErrMsg,"非卡折类帐号,请重新输入!g_mdm_ac_rel.mdm_code=[%s]", \
				g_mdm_ac_rel.mdm_code);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"O188");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]=='*')
	{
    	sprintf(acErrMsg,"该介质已作销质!mdm_code=[%s]",g_mdm_ac_rel.mdm_code);
    	WRITEMSG
 		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	/*卡类帐号检查主副卡标志*/
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
		if(g_mdm_ac_rel.main_ind[0]!='1')
		{
	    	sprintf(acErrMsg,"请输入主卡号，副卡不能做此业务!mdm_code=[%s]", \
					g_mdm_ac_rel.mdm_code);
	    	WRITEMSG
	 		strcpy(g_pub_tx.reply,"O198");
			goto ErrExit;
		}
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld and ac_seqn=%d",\
					     g_mdm_ac_rel.ac_id,1);
   	if( ret )
	{
		 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
		 		 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D197");
		 goto ErrExit;
	}

	if(g_mdm_ac_rel.ac_seqn==9999)
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld and ac_seqn>=200", \
					  g_mdm_ac_rel.ac_id);
		if(ret<0)
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
			ac_seqn=g_prdt_ac_id.ac_seqn;
		else  
			ac_seqn=g_prdt_ac_id.ac_seqn;
	}
	else if(g_mdm_ac_rel.ac_seqn)
		ac_seqn=g_mdm_ac_rel.ac_seqn;
	else 
		ac_seqn=0;

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		 case '1':                               /*1-活期*/
				 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
					 "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret )
				 {
					 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

			 	 if(g_dd_mst.ac_sts[0]=='*')
				 {
					 sprintf(acErrMsg,"该帐户已作销户处理!");
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P245");
					 goto ErrExit;
				 }

				 ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
									g_prdt_ac_id.prdt_no);
				 if( ret )
				 {
					 sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 bal=g_dd_mst.bal;
				 strcpy(name,g_dd_mst.name);
				 strcpy(title,g_dd_parm.title);
				 strcpy(cur_no,g_dd_parm.cur_no);
				 break;
		 case '2':                               /*2-定期*/
				 ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
						 "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret )
				 {
					 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	 }

			 	 if(g_td_mst.ac_sts[0]=='*')
				 {
					 sprintf(acErrMsg,"该帐户已作销户处理!");
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P245");
					 goto ErrExit;
				 }

				 ret = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
									g_prdt_ac_id.prdt_no);
				 if( ret )
				 {
					 sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 bal=g_td_mst.bal;
				 strcpy(name,g_td_mst.name);
				 strcpy(title,g_td_parm.title);
				 strcpy(cur_no,g_td_parm.cur_no);
				 break;
		default:
				 break;
	}
	
	/* 下载介质类型菜单 */
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	ret=Mdm_attr_Dec_Sel(g_pub_tx.reply,"1=1");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Mdm_attr_Fet_Sel(&vmdm_attr,g_pub_tx.reply);
		if( ret==100 ) break;
		else if( ret ) goto ErrExit;

vtcp_log("IIII[%d]",i);
vtcp_log("JJJJ[%s][%s][%s][%s]\n",
	g_prdt_ac_id.prdt_no,g_mdm_ac_rel.mdm_code,vmdm_attr.mdm_code,NULL_MDM );
		
		if( pub_base_check_promedi(g_prdt_ac_id.prdt_no,g_mdm_ac_rel.mdm_code, \
										   vmdm_attr.mdm_code,NULL_MDM) )
		{
TRACE
		continue;
		}

		pub_base_strpack(vmdm_attr.mdm_name);
		pub_base_strpack(vmdm_attr.mdm_code);

		if( i==0 )  set_zd_data("1161",vmdm_attr.mdm_code);
vtcp_log("1161|%s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code);
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
	/*
	set_zd_data("1146",g_mdm_ac_rel.id_no);
	*/
	set_zd_double("0420",bal);
	set_zd_data("0210",cur_no);
	set_zd_data("1166",name);

	set_zd_data("1021",ac_no);

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
 
