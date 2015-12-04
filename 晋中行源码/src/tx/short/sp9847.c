/*************************************************
* 文 件 名:  sp9847.c
* 功能描述： 根据显示账号和账户序号查询户名和余额__公共转帐取款方专用
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
#include "mdm_attr_c.h"
#include "cif_id_code_rel_c.h"

int sp9847()
{
    int ret=0;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[3];
	char cif_type[3],prdt_name[31];
	int flag=0;
	double bal=0.00;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	g_mdm_attr;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	g_reply_int=0;

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    get_zd_int("1022",&ac_seqn);
    
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
								"ac_no='%s' and ac_seqn=%d ",
								g_mdm_ac_rel.ac_no,ac_seqn);
    if( g_reply_int==100 )
    {
		g_reply_int=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
							"ac_no='%s' and ac_seqn=%d ",
							 g_mdm_ac_rel.ac_no,ALL_SEQN);
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

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
		"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret )
	{
       	sprintf(acErrMsg,"查询对方账号错误![%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
							"cif_no=%ld",prdt_ac_id_c.cif_no);
	if( ret==100 )
	{
       	sprintf(acErrMsg,"该客户不存在!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "C007");
       	goto ErrExit;
	}

	if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
	{
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,
						"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
		if( ret )
		{
       		sprintf(acErrMsg,"查询介质属性表错误![%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply, "B004");
       		goto ErrExit;
		}
		strcpy(note_no,g_mdm_attr.note_type);
		vtcp_log("WWWWWWWWWWWWWWWWnote_no=[%s]",note_no);
	}
	else strcpy(note_no,"");

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", 
						prdt_ac_id_c.ac_id,ac_seqn);
    	if( ret )
    	{
        	sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "B005"); 
			goto ErrExit; 
		}
		strcpy(name,g_dd_mst.name);
		bal=g_dd_mst.bal;

		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_dd_mst.prdt_no);
    	if( ret )
    	{ 
			sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "B005"); 
			goto ErrExit; 
		}
		strcpy(prdt_name,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
		strcpy(cif_type,g_dd_parm.cif_type);
	}
	else
	{
		sprintf(acErrMsg,"请输入活期帐号，其他账户不能作转帐方!![%s]", \
				prdt_ac_id_c.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O227");
		goto ErrExit;
	}
    
	pub_base_strpack(name);
	vtcp_log("name=[%s],bal=[%lf]",name,bal);
	vtcp_log("pwd_yn=[%s]",g_mdm_ac_rel.draw_pwd_yn);
	vtcp_log("id_type=[%s]",g_mdm_ac_rel.id_type);
	vtcp_log("id_no=[%s]",g_mdm_ac_rel.id_no);
	vtcp_log("mach_yn=[%s]",g_mdm_ac_rel.pwd_mach_yn);

    set_zd_data("102H",name); 							/*户名*/
    set_zd_data("102J",cur_no); 						/*币种*/
    set_zd_data("102V",prdt_name); 						/*产品名称*/
    set_zd_data("102K","1"); 							/*默认现转*/
    set_zd_double("102I",bal); 							/*余额*/
    set_zd_data("1023",note_no); 						/*凭证种类*/
    set_zd_data("1024",g_mdm_ac_rel.note_no); 			/*凭证号码*/
    set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn); 		/*是否密码支取*/

   	set_zd_data("1029",g_mdm_ac_rel.draw_id_yn ); 		/*是否证件支取*/
   	set_zd_data("102G",g_mdm_ac_rel.id_type); 			/*证件类型*/

    set_zd_data("102C",g_mdm_ac_rel.pwd_mach_yn); 		/*是否密码器支取*/
   	set_zd_double("102F",g_td_mst.td_amt); /*存本取息零存整取的每次发生额*/
	
	set_zd_data("102M",cif_type);    		           /*帐户类型*/
		vtcp_log("YYYYYYYYYYYYYYYcif_type=[%s]",cif_type);

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
