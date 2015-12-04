/*************************************************
* 文 件 名: sp9955.c
* 功能描述：根据帐号回显对公帐户信息[对公取款交易]
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include "public.h"
#include "prdt_ac_id_c.h"

#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"

sp9955()  
{ 		
	int i=0,ret=0,flag=0,ac_seqn;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char tmp_memo[100],tmpname[100],prdt_name[31];
	FILE *fp;
	
	vtcp_log("Oooooooooooooooooooooooo");
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	pub_base_sysinit();
	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);					/*帐号*/
 	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/

	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXXac_no=[%s]",ac_no);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/****若存款户为贷款的还款户，返回标志以提示柜员 add by wanglei 20070621****/
	ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id = %ld and ac_sts in('1','2','3','4','5') \
		and (bal >0 or in_lo_intst >0 or out_lo_intst >0 or cmpd_lo_intst >0) ", g_mdm_ac_rel.ac_id);
	if(ret == 0)
	{
		sprintf(acErrMsg,"贷款还款帐号且有本金或利息未还!!");
		WRITEMSG
		set_zd_data("0710","1"); /****返回提示标志****/
	}
	/****ended 20070621****/
	
	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXX");
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
					   g_mdm_ac_rel.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXX");
	if(prdt_ac_id_c.prdt_no[0]=='1')
	{
		ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",prdt_ac_id_c.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找帐号出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(name,g_dd_mst.name);
		cif_no = g_dd_mst.cif_no;
		bal = g_dd_mst.bal;

		if(g_dd_mst.ac_sts[0]=='*')
		{
			sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}
		/*****add by ligl 2007-1-29 13:09******/
		int ret1=0,ret2=0;
		long days=0;
		ret=pub_base_CalDays(g_dd_mst.opn_date,g_pub_tx.tx_date,"0",&days);
		/******  根据管总要求  对公开户去掉 开户3天内不能取款的限制
		if(days<3 && g_dd_mst.bal>0)
		{
  			 ret2=sql_count("ln_mst","repay_ac_id=%ld or save_ac_id=%ld",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_id);
  			 if(ret2==0)
  			 	{
						sprintf(acErrMsg,"该账号开户不满3日不允许取现!! [%d]",ret2);
						WRITEMSG
						strcpy(g_pub_tx.reply,"T067");
						goto ErrExit;
				}
  		
  	}
	********/
  	/********end**********/
    /*if(g_dd_mst.ac_type[0]=='2')
    {
    	sprintf(acErrMsg,"一般结算户不能取现!![%d]",ret);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"T047");
    	goto ErrExit;
    }*/
		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_dd_mst.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(prdt_name,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
		strcpy(cif_type,g_dd_parm.cif_type);
	}
	else if(prdt_ac_id_c.prdt_no[0]=='2')	
	{
		sprintf(acErrMsg,"该帐户不能做此业务，只能部提! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O197");
		goto ErrExit;
	}
	else
	{
		sprintf(acErrMsg,"非存取款类账号! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}

   	set_zd_data("102M",cif_type);
																					if( cif_type[0]=='1' )
	{
		sprintf(acErrMsg,"请输入对公账号!! [%d]",cif_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
							   "cif_no=%ld",cif_no);
	if( ret==100 )
	{	
		sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack(name);
   	set_zd_data("102C","N");						/*支付密码*/
   	set_zd_data("102H",name);						/*户名*/
   	set_zd_data("102V",prdt_name);					/*产品名称*/
	set_zd_data("102J",cur_no);						/*币种*/
	set_zd_data("102K","1");						/*默认现转*/
	set_zd_double("102I",bal);						/*余额*/
	set_zd_data("1024",g_mdm_ac_rel.note_no);		/*凭证号码*/
	set_zd_data("102O",g_dd_mst.ac_type);						/*帐户类型*/

	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");

	/***下载对公摘要列表  不用了，统一通过现转标志后选摘要
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
		{
			set_zd_data("102L",vdic_data.pr_seq);
			strcpy(tmp_memo,vdic_data.pr_seq);
		}

		fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();
	*/

	/***凭证类型选项列表***/
	i=0;
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='pub_taty' order by pr_seq");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		else if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )  
		{
			strcpy(note_no,vdic_data.pr_seq);
			set_zd_data("1023",vdic_data.pr_seq);
		}
		fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/***选项列表下载完毕***/

	set_zd_data("1021",ac_no);
  set_zd_data("0680",g_dd_mst.ac_type);
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
   
