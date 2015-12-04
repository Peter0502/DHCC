/*************************************************
* 文 件 名:  sp9776.c
* 功能描述： 根据显示账号查询户名
*
* 作    者:  rob
* 完成日期： 2003年3月30日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "prdt_ac_id_c.h"

int sp9776()
{
    int ret;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[3];
	int flag=0;
	double bal;
	char br_no[6];

	struct com_branch_c com_branch_c;
	struct prdt_ac_id_c sPrdt_ac_id;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(br_no, 0x0, sizeof(br_no));
	ret=0;
	g_reply_int=0;
	bal=0.00;

    get_zd_data("0300",g_mdm_ac_rel.ac_no);
	pub_base_old_acno( g_mdm_ac_rel.ac_no );
    set_zd_data("0300",g_mdm_ac_rel.ac_no);
    strcpy(g_pub_tx.cur_no,"01");
    

sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_mdm_ac_rel.ac_no,g_pub_tx.ac_seqn);
WRITEMSG

    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",
				g_mdm_ac_rel.ac_no);
	if( g_reply_int ) 
	{
       	sprintf(acErrMsg,"查询对方账号错误!! [%d]",g_reply_int);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

 	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and \ 
		ac_seqn=%d", g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn); 
	if (ret != 0 && ret != 100) 
	{ 
		sprintf(acErrMsg,"查询产品帐号对照表错误!! [%d]",ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	else if (ret == 100) 
	{ 
		ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn); 
		if ( ret != 0) 
		{ 
			sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]", 
				g_pub_tx.cur_no); 
			WRITEMSG 
			goto ErrExit; 
		} 
	} 
	else 
	{ 
		g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn; 
	}

	strcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
	sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG

	ret = pub_base_disac_file(g_pub_tx.ac_no, g_pub_tx.ac_seqn, 
		g_pub_tx.ac_type); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"查询账号错误!! "); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"D169"); 
		goto ErrExit; 
	}

	/** 
	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d", 
		  g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	strcpy(name,g_dd_mst.name);
    if( ret )
    {
        sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
	**/

 	if (atoi(g_pub_tx.ac_type) == 1) 
	{ 
		set_zd_data("0250",g_dd_mst.name); 
		strcpy(br_no, g_dd_mst.opn_br_no);
	} 
	else if (atoi(g_pub_tx.ac_type) == 2) 
	{ 
		set_zd_data("0250",g_td_mst.name); 
		strcpy(br_no, g_td_mst.opn_br_no);
	} 
	else if (atoi(g_pub_tx.ac_type) == 3) 
	{ 
		set_zd_data("0250",g_ln_mst.name); 
		strcpy(br_no, g_ln_mst.opn_br_no);
	} 
	else if (atoi(g_pub_tx.ac_type) == 4) 
	{ 
		sprintf(acErrMsg,"透支账户主文件没有户名"); 
		WRITEMSG 
	} 
	else if (atoi(g_pub_tx.ac_type) == 9) 
	{ 
		set_zd_data("0250", g_in_mst.name); 
		strcpy(br_no, g_in_mst.opn_br_no);
	}


	ret = Com_branch_Sel(g_pub_tx.reply, &com_branch_c, "br_no = '%s'",
			br_no);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"查询公共机构编码表错误!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D172");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg,"没有该机构编码!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D173");
		goto ErrExit;
	}

    
	set_zd_data("0830",com_branch_c.br_name);
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	/*** delete by rob
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data("0690","1");
	****/
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
