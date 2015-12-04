/*************************************************
* 文 件 名: sp9683.c
* 功能描述：根据帐号回显账户信息
*
* 作    者: rob
* 完成日期：2003年12月05日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9683()  
{ 		
	int i=0;
	int ret=0,ac_seqn=0;
	double bal;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char tmp_memo[100],prdt_name[31];
	char filename[100],tmpname[100];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;

	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);					/*帐号*/
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/
	
	if(strlen(ac_no)==19)
	{
		sprintf(acErrMsg,"请输入对公帐号!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.mdm_code[2]=='1')
	{
		sprintf(acErrMsg,"请输入对公帐号!! [%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}
		
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.ac_seqn==9999)
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret<0)
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==1) 
			ac_seqn=prdt_ac_id_c.ac_seqn;
		else 
		{
			set_zd_long("101E",prdt_ac_id_c.ac_seqn);
		 	ac_seqn=0;

			if(prdt_ac_id_c.prdt_no[0]!='1'&&prdt_ac_id_c.prdt_no[0]!='2')
			{
				sprintf(acErrMsg,"非存款类账号!! [%d]",ret);
	 			WRITEMSG
	  			strcpy(g_pub_tx.reply,"O166");
	   			goto ErrExit;
			}
		}
	}
	else if(g_mdm_ac_rel.ac_seqn)
		ac_seqn=g_mdm_ac_rel.ac_seqn;

	/*取信息*/
	if(ac_seqn||(g_mdm_ac_rel.ac_seqn==0&&prdt_ac_id_c.prdt_no[0]=='2'))
	{
		switch(prdt_ac_id_c.prdt_no[0])
		{
			case '1':	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
										"ac_id=%ld and ac_seqn=%d", 
										prdt_ac_id_c.ac_id,ac_seqn);
						strcpy(name,g_dd_mst.name);
						bal=g_dd_mst.bal;

						if(g_dd_mst.ac_sts[0]=='*')	
						{
							sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"P245");
	   						goto ErrExit;
						}

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

						if(g_td_mst.ac_sts[0]=='*')	
						{
							sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"P245");
	   						goto ErrExit;
						}

						ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							  "prdt_no='%s'",g_td_mst.prdt_no);

						if(g_td_parm.dpst_ind[0]=='N')	
						{
							sprintf(acErrMsg,"该定期户不能做存款业务![%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"O181");
	   						goto ErrExit;
						}
						strcpy(prdt_name,g_td_parm.title);
						strcpy(cur_no,g_td_parm.cur_no);
						strcpy(cif_type,g_td_parm.cif_type);
						break;
			default:
						sprintf(acErrMsg,"产品类型错误![%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply, "G102");
						goto ErrExit;
		}

		set_zd_data("101D",cif_type);

		if( cif_type[0]=='1' )
		{
			sprintf(acErrMsg,"请输入对公账号!! [%d]",cif_type);
		 	WRITEMSG
		  	strcpy(g_pub_tx.reply,"O165");
		   	goto ErrExit;
		}
	}

	/*开始下载菜单*
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	***摘要选项列表***
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);
		if(i==0)
		strcpy(tmp_memo,vdic_data.pr_seq);

		fprintf(fp,"101C|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	*对公凭证类型下载*
	i=0;
	ret=Dic_data_Dec_Sel( g_pub_tx.reply, "pr_code='com_deb'");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( i==0 && ret==100 ) 
		{	
			strcpy(note_no,"");
			break;
		}
		else if(ret==100) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( !strncmp(vdic_data.pr_seq,"119",3) )  
		{
			vtcp_log("XXXXXvdic_data.pr_seq===[%s]",vdic_data.pr_seq);
			memset(note_no,0x00,sizeof(note_no));
			strcpy(note_no,vdic_data.pr_seq);
		}
		fprintf(fp,"1014|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	*明细下载完毕*/

	/* 输出变量 */
	pub_base_strpack(name);
	vtcp_log("name=[%s],bal=[%lf]",name,bal);
   	set_zd_data("0260",name); 					/*户名*/
   	set_zd_double("0410",bal); 					/*余额*/


	set_zd_data("1011",ac_no);
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
   
