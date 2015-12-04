/*************************************************
* 文 件 名:  sp9727.c
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
#include "mdm_attr_c.h"

int sp9727()
{
    int ret=0;
	char name[51],cur_no[3],note_no[5];
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

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"ac_no=[%s]!",g_mdm_ac_rel.ac_no);
    WRITEMSG
    
    ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_no='%s' and ac_seqn=0",g_mdm_ac_rel.ac_no);
    if( ret==100 )
    {
		ret=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
			"ac_no='%s' and ac_seqn=%d ",g_mdm_ac_rel.ac_no,ALL_SEQN);
		if( ret==100 )
		{
        	sprintf(acErrMsg,"查询对方账号错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
		}
    }

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,  \
		"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id, \
		g_mdm_ac_rel.ac_seqn);
	if( ret )
	{
       	sprintf(acErrMsg,"查询对方账号错误![%d]ac_id=[%ld] and ac_seqn=[%d]", \
				ret,g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

 	if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
	{
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr, \
			 "mdm_code='%s'",g_mdm_ac_rel.mdm_code);
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

	switch(prdt_ac_id_c.prdt_no[0])
	{
		case '1':
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
						"ac_id=%ld and ac_seqn=%d", \
						g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
				strcpy(name,g_dd_mst.name);
				bal=g_dd_mst.bal;
	
				ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
						"prdt_no='%s'",g_dd_mst.prdt_no);
				strcpy(cur_no,g_dd_parm.cur_no);
		case '2':
				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
						"ac_id=%ld and ac_seqn=%d", \
						g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
				strcpy(name,g_td_mst.name);
				bal=g_td_mst.bal;
	
				ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
						"prdt_no='%s'",g_td_mst.prdt_no);
				strcpy(cur_no,g_td_parm.cur_no);
		default:
       			sprintf(acErrMsg,"帐户产品类型非定活!! [%s]", \
						prdt_ac_id_c.prdt_no);
       			WRITEMSG
       			strcpy(g_pub_tx.reply, "B004");
       			goto ErrExit;
	}

	pub_base_strpack(name);
	vtcp_log("name=[%s],bal=[%lf]",name,bal);
    set_zd_data("102H",name); 					/*户名*/
    set_zd_data("102J",cur_no); 				/*币种*/
    set_zd_double("102I",bal); 					/*余额*/
    set_zd_data("1023",note_no); 				/*凭证种类*/
    set_zd_data("1024",g_mdm_ac_rel.note_no); 	/*凭证号码*/

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
