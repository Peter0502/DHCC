/*************************************************
* 文 件 名:  sp9851.c
* 功能描述： 根据显示账号查询户名和余额
*
* 作    者:  
* 完成日期： 2003年1月17日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9851()
{
	int ret;
	char name[51];
	double dBal=0.0;  /****返回帐户余额  20121030****/
	struct prdt_ac_id_c	prdt_ac_id_c;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	ret=0;
	g_reply_int=0;

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"ac_no=[%s]!",g_mdm_ac_rel.ac_no);
    WRITEMSG
    
    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' and ac_seqn=%d ",g_mdm_ac_rel.ac_no,ALL_SEQN);
    if( g_reply_int==100 )
    {
		g_reply_int=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",g_mdm_ac_rel.ac_no);
		if( ret==100 )
		{
        	sprintf(acErrMsg,"查询对方账号错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
		}
    }
	if( g_reply_int || ret ) 
	{
       	sprintf(acErrMsg,"查询对方账号错误!! [%d][%d]",g_reply_int,ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	if( ret )
	{
       	sprintf(acErrMsg,"查询对方账号错误![%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	while(1)
	{
		ret=Prdt_ac_id_Fet_Sel(&prdt_ac_id_c,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
   	    	sprintf(acErrMsg,"查询对方账号错误![%d]",ret);
   	    	WRITEMSG
   	    	strcpy(g_pub_tx.reply, "B004");
   	    	goto ErrExit;
		}

		switch(prdt_ac_id_c.prdt_no[0])
		{
		case '1':	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ", prdt_ac_id_c.ac_id);
					strcpy(name,g_dd_mst.name);
					dBal=g_dd_mst.bal;
					break;
		case '2':	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ", prdt_ac_id_c.ac_id);
					strcpy(name,g_td_mst.name);
					dBal=g_td_mst.bal;
					break;
		case '3':	ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld ", prdt_ac_id_c.ac_id);
					strcpy(name,g_ln_mst.name);
					dBal=g_ln_mst.bal;
					break;
		case '9':	ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld ", prdt_ac_id_c.ac_id);
					strcpy(name,g_in_mst.name);
					dBal=g_in_mst.bal;
					break;
		default:
				sprintf(acErrMsg,"产品类型错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
		}
		if( ret==0 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B005");
			goto ErrExit;
		}
    }
	Prdt_ac_id_Clo_Sel();
    
	pub_base_strpack(name);
	vtcp_log("name=[%s]",name);
	vtcp_log("uncon_yn=[%s]",g_mdm_ac_rel.draw_uncon_yn);
	vtcp_log("pwd_yn=[%s]",g_mdm_ac_rel.draw_pwd_yn);
	vtcp_log("id_type=[%s]",g_mdm_ac_rel.id_type);
	vtcp_log("id_no=[%s]",g_mdm_ac_rel.id_no);
	vtcp_log("seal_yn=[%s]",g_mdm_ac_rel.draw_seal_yn);
	vtcp_log("mach_yn=[%s]",g_mdm_ac_rel.pwd_mach_yn);
	vtcp_log("余额=[%.2f]",dBal);
        sprintf(acErrMsg,"查询账号余额为[%.2f]",dBal);
    set_zd_data("102H",name); 	/*户名*/
    set_zd_data("1025",g_mdm_ac_rel.draw_uncon_yn); 	/*是否任意支取*/
    set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn); 		/*是否密码支取*/
    set_zd_data("1029",g_mdm_ac_rel.draw_id_yn); 		/*是否证件支取*/
    set_zd_data("102G",g_mdm_ac_rel.id_type); 			/*证件类型*/
    set_zd_data("102A",g_mdm_ac_rel.id_no); 			/*证件号码*/
    set_zd_data("102B",g_mdm_ac_rel.draw_seal_yn); 		/*是否印鉴支取*/
    set_zd_data("102C",g_mdm_ac_rel.pwd_mach_yn); 		/*是否密码器支取*/
    set_zd_double("0410",dBal); 		/*帐户余额*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
