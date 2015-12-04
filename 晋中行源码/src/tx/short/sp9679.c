/*************************************************
* 文 件 名:  sp9679.c
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
#include "prdt_ac_id_c.h"
#include "cif_addr_inf_c.h"
#include "sub_dd_mst_c.h"

int sp9679()
{
    int ret;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[3];
	char ac_type[2];
	int flag=0;
	double bal;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct cif_addr_inf_c	cif_addr_inf_v;
	struct prdt_ac_id_c prdt_ac_id_v;
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct sub_dd_mst_c sub_dd_mst_c;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&cif_addr_inf_v,0x00,sizeof(struct cif_addr_inf_c));
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	ret=0;
	g_reply_int=0;
	bal=0.00;

    get_zd_data("0300",g_mdm_ac_rel.ac_no);
	pub_base_old_acno(g_mdm_ac_rel.ac_no);
    set_zd_data("0300",g_mdm_ac_rel.ac_no);
	if( !strlen( g_mdm_ac_rel.ac_no ) )
	{
    	get_zd_data("0310",g_mdm_ac_rel.ac_no);
		pub_base_old_acno(g_mdm_ac_rel.ac_no);
		set_zd_data("0310",g_mdm_ac_rel.ac_no);
	}
    strcpy(g_pub_tx.cur_no,"01");

	if(strlen(g_mdm_ac_rel.ac_no)>13)
		strcpy(ac_type,"1");
	else if(strlen(g_mdm_ac_rel.ac_no)==13)
	{
		if(g_mdm_ac_rel.ac_no[0]<'5') 
			strcpy(ac_type,"1");
		else if(g_mdm_ac_rel.ac_no[0]>='5'&& g_mdm_ac_rel.ac_no[0]<'9')
			strcpy(ac_type,"2");
	}
	set_zd_data("0670",ac_type);
    

sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_mdm_ac_rel.ac_no,g_pub_tx.ac_seqn);
WRITEMSG

    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",
				g_mdm_ac_rel.ac_no);
	if( g_reply_int ) 
	{
       	sprintf(acErrMsg,"查询对方账号错误!![%s] [%d]", g_mdm_ac_rel.ac_no, g_reply_int);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]!='0' &&
		g_mdm_ac_rel.note_sts[0]!='1' )
	{
       	strcpy(g_pub_tx.reply, "P172");
		TRACE;
       	goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_v,"ac_id=%ld and \ 
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
		g_pub_tx.ac_seqn = prdt_ac_id_v.ac_seqn;
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

	/******
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
	*******/
    
	if (atoi(g_pub_tx.ac_type) == 1) 
	{ 
		set_zd_data("0250",g_dd_mst.name); 
		set_zd_data("0270",g_dd_mst.name); 
		pub_base_strpack(g_dd_mst.prdt_no);
		if (strcmp(g_dd_mst.prdt_no, "131") == 0)
		{
			set_zd_data("0130","该帐号是承兑保证金账号不能入账");
			goto ErrExit;
		}
	} 
	else if (atoi(g_pub_tx.ac_type) == 2) 
	{ 
		set_zd_data("0250",g_td_mst.name); 
		set_zd_data("0270",g_td_mst.name); 
	} 
	else if (atoi(g_pub_tx.ac_type) == 3) 
	{ 
		set_zd_data("0250",g_ln_mst.name); 
		set_zd_data("0270",g_ln_mst.name); 
		/* 不能入贷款账号 */
		set_zd_data("0130","该帐号是贷款账号不能入账");
		goto ErrExit;
	} 
	else if (atoi(g_pub_tx.ac_type) == 4) 
	{ 
		sprintf(acErrMsg,"透支账户主文件没有户名"); 
		WRITEMSG 
	} 
	else if (atoi(g_pub_tx.ac_type) == 9) 
	{ 
		set_zd_data("0250", g_in_mst.name); 
		set_zd_data("0270", g_in_mst.name); 
	}

	set_zd_data("0690","0");
	
	/* 
	g_reply_int=Prdt_ac_id_Sel( g_pub_tx.reply , &prdt_ac_id_v , "ac_id=%ld" , 
							  g_mdm_ac_rel.ac_id);
    if(g_reply_int)
	{
		sprintf( acErrMsg, "查询产品账号关系视图表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
	    goto ErrExit;
	}
	*/

	g_reply_int=Cif_addr_inf_Sel( g_pub_tx.reply, &cif_addr_inf_v,
		"cif_no=%ld and addr_seqn=1", prdt_ac_id_v.cif_no);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询客户地址信息表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0962",cif_addr_inf_v.addr);

	g_reply_int=Sub_dd_mst_Sel( g_pub_tx.reply,&sub_dd_mst_c,
		"ac_no='%s'",g_mdm_ac_rel.ac_no );
	if( g_reply_int==0 )
	{
		vtcp_log(" is subdd");
		set_zd_data("0680","1");
	}
	else if(g_reply_int==100)
	{
		vtcp_log(" is't subdd");
		set_zd_data("0680","0");
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询客户地址信息表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data("0690","1");
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
