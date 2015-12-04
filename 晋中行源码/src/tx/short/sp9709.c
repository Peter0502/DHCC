/*************************************************
* 文 件 名:    sp9709.c
* 功能描述：   根据帐号回显相关信息_卡限额维护NO.1
* 作    者:    rob
* 完成日期：   2003年02月19日
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

sp9709()  
{ 		
	int ret=0,ac_seqn;
	char title[40],cur_no[3];
	char ac_no[20],name[50];
	char ind[2];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;
	struct cif_basic_inf_c g_cif_basic_inf;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);							/*主卡帐号*/
	get_zd_data("0670",ind);							/*处理类型*/
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/
	
	/* 调用根据帐号返回账户序号函数 */
	ret=pub_base_actoseqn(ac_no,&ac_seqn);          	/*账户序号*/
	if(ret)
	{
	 	sprintf( acErrMsg, "调用函数pub_base_actoseqn出错!!ret=[%d]", ret );
				 WRITEMSG
	 	goto ErrExit;
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
	    sprintf(acErrMsg,"请输入卡号!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"O191");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, \
						"ac_id=%ld", g_mdm_ac_rel.ac_id);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,
			   				"cif_no=%d",g_prdt_ac_id.cif_no);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该客户!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	/* 输出变量 */
	if(ind[0]=='1')
	{
		set_zd_data("1141",g_mdm_ac_rel.draw_uncon_yn);
		set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
		set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
		set_zd_data("1147",g_mdm_ac_rel.draw_seal_yn);
		set_zd_data("1148",g_mdm_ac_rel.id_type);
		set_zd_data("0250",g_cif_basic_inf.name);
	}
	else 
		set_zd_data("0250",g_cif_basic_inf.name);

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
 
