/*************************************************
* 文 件 名: sp9989.c
* 功能描述：根据帐号回显账户序号及信息[贷款扣划取款快查]
* 作    者: lance
* 完成日期：2003年08月29日
* 修改记录：仿sp9731.c
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"

sp9989()  
{ 		
	int i=0,ret=0,flag=0,tag=0,ac_seqn=0;
	double bal=0.00;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51],tmp_inf[3];
	char ac_type[2],tmp_memo[100],tmpname[100],prdt_name[31];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;

	char spe_ind[21];

	/** 取值、赋值 **/	
	get_zd_data("1201",ac_no);						/*帐号*/
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					    g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质不存在! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** 多账户介质的情况 ***/
	if( g_mdm_attr.ac_num_ind[0]=='N' )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret<0)
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==1) 
		{
			TRACE
			sprintf(acErrMsg,"ac_seqn=[%d]",ac_seqn);
			WRITEMSG
			ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c," ac_id=%ld ",g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]ac_id=[%ld]",ret,g_mdm_ac_rel.ac_id);
		 		WRITEMSG
		  		strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			ac_seqn = prdt_ac_id_c.ac_seqn;
			sprintf(acErrMsg,"ac_seqn=[%d]",ac_seqn);
			WRITEMSG
		}
		else if(ret>1)
		{
			/*多介质只可能存在于个人户,ac_seqn强行赋值 1*/
			TRACE
			ac_seqn = 1;
			/*****
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
			********/
		}
	}else{
		ac_seqn = g_mdm_ac_rel.ac_seqn;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]ac_id=[%ld]ac_seqn=[%d]",ret,g_mdm_ac_rel.ac_id,ac_seqn);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') )
	{
		sprintf(acErrMsg,"非存取款类账号! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}

	if( tag==0 )
	{
		if( prdt_ac_id_c.prdt_no[0]=='1' )
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_dd_mst.name);
			cif_no = g_dd_mst.cif_no;
			bal = g_dd_mst.bal;

			if( g_dd_mst.ac_sts[0]=='*' )
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);
		}
		else
		{
			sprintf(acErrMsg,"请输入活期帐号，其他账户不能作扣划账号![%s]", \
					prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O257");
			goto ErrExit;
		}

		strcpy(ac_type,cif_type);
		set_zd_data("102M",ac_type);
		set_zd_data("0240",prdt_ac_id_c.prdt_no); /*新增返回产品号*/

		ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
									"cif_no=%ld",cif_no);
		if( ret )
		{	
			sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "G102");
			goto ErrExit;
		}

		pub_base_strpack(name);

	   	set_zd_data("1202",name);						/*户名*/
		set_zd_data("1204",cur_no);						/*币种*/
		set_zd_double("1203",bal);						/*扣款账号余额*/
	}
	
	set_zd_data("1201",ac_no);

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
