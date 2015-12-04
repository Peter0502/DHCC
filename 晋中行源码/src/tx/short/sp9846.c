/*************************************************
* 文 件 名: sp9846.c
* 功能描述：根据显示账号查询户名和余额[存款]
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

int sp9846()
{
    int ret=0;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[5],cif_type[2],prdt_name[31];
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

    get_zd_data("1011",g_mdm_ac_rel.ac_no);
    get_zd_int("1012",&ac_seqn);
    
    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
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

	switch(prdt_ac_id_c.prdt_no[0])
	{
		case '1':	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_dd_mst.name);
					bal=g_dd_mst.bal;

					ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
									"prdt_no='%s'",g_dd_mst.prdt_no);
					strcpy(prdt_name,g_dd_parm.title);
					strcpy(cur_no,g_dd_parm.cur_no);
					strcpy(cif_type,g_dd_parm.cif_type);
					break;
		case '2':	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_td_mst.name);
					bal=g_td_mst.bal;

					ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
									"prdt_no='%s'",g_td_mst.prdt_no);
					strcpy(prdt_name,g_td_parm.title);
					strcpy(cur_no,g_td_parm.cur_no);
					strcpy(cif_type,g_td_parm.cif_type);
					break;
		case '3':	ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_ln_mst.name);
					bal=g_ln_mst.bal;

					ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,
									"prdt_no='%s'",g_ln_mst.prdt_no);
					strcpy(prdt_name,g_ln_parm.title);
					strcpy(cur_no,g_ln_parm.cur_no);
					break;
		case '9':	ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_in_mst.name);
					bal=g_in_mst.bal;

					ret = In_parm_Sel(g_pub_tx.reply,&g_in_parm,
									"prdt_no='%s'",g_in_mst.prdt_no);
					strcpy(cur_no,g_in_parm.cur_no);
					break;
		default:
				sprintf(acErrMsg,"产品类型错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
	}

    if( ret )
    {
        sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
    
	pub_base_strpack(name);
    set_zd_data("1018",name); 					/*户名*/
    set_zd_data("101A",cur_no); 				/*币种*/
    set_zd_data("101B","1"); 					/*默认现转*/
    set_zd_double("1019",bal); 					/*余额*/
    set_zd_data("1014",note_no); 				/*凭证种类*/
    set_zd_data("1015",g_mdm_ac_rel.note_no); 	/*凭证号码*/
    set_zd_data("101F",prdt_name); 				/*产品名称*/

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
