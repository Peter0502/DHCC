/*************************************************
* 文 件 名: sp9928.c
* 功能描述：根据显示账号和序号查询户名和余额[储蓄部提]
* 作    者:  
* 完成日期：2003年1月17日 
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

int sp9928()
{
    int ret=0;
	int	ac_seqn;
	char name[61],cur_no[3],note_no[5],cif_type[2],prdt_name[31];
	double bal=0.00;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	g_mdm_attr;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	g_reply_int=0;

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    get_zd_int("1022",&ac_seqn);
    
    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
								"ac_no='%s' and ac_seqn=%d", \
								g_mdm_ac_rel.ac_no,ac_seqn);
    if( g_reply_int==100 )
    {
		g_reply_int=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_no='%s' and ac_seqn=%d ",g_mdm_ac_rel.ac_no,ALL_SEQN);
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

	ret=Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret )
	{
       	sprintf(acErrMsg,"查询对方账号错误![%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

 	if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
	{
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 g_mdm_ac_rel.mdm_code);
		if( ret )
		{
		 	sprintf(acErrMsg,"查询介质属性表错误![%d]",ret);
					WRITEMSG
		 	strcpy(g_pub_tx.reply, "B004");
		 	goto ErrExit;
		}
	 	strcpy(note_no,g_mdm_attr.note_type);
 	}
 	else strcpy(note_no,"");

	if( prdt_ac_id_c.prdt_no[0]== '2')
	{
		ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", \
						 prdt_ac_id_c.ac_id,ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"账号不存在!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(name,g_td_mst.name);
		bal=g_td_mst.bal;

		if( g_td_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}

		ret = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
							g_td_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg," 产品不存在!! [%s]",g_td_mst.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if( g_td_parm.wdr_ind[0]=='N' )
		{
			sprintf(acErrMsg,"该账号不允许做部提业务!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O193");
			goto ErrExit;
		}

		strcpy(prdt_name,g_td_parm.title);
		strcpy(cur_no,g_td_parm.cur_no);
		strcpy(cif_type,g_td_parm.cif_type);
	}
	else
	{
		sprintf(acErrMsg,"该帐号不允许作部提业务![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O193");
		goto ErrExit;
	}

	pub_base_strpack(name);
    set_zd_data("102H",name); 						/*户名*/
    set_zd_data("102J",cur_no); 					/*币种*/
    set_zd_double("102I",bal); 						/*余额*/
    set_zd_data("102V",prdt_name); 					/*产品名称*/
    set_zd_data("1023",note_no); 					/*凭证种类*/
    set_zd_data("1024",g_mdm_ac_rel.note_no); 		/*凭证号码*/
    set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn); 

    set_zd_data("1029",g_mdm_ac_rel.draw_id_yn); 	/*证件支取*/
    set_zd_data("102G",g_mdm_ac_rel.id_type); 		/*证件类型*/

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
