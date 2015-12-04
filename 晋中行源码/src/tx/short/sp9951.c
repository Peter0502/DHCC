/*************************************************
* 文 件 名: sp9951.c
* 功能描述：根据帐号回显账户序号及信息[储蓄存款交易]
*
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:20110608
*        修 改 人:zhaodawei
*        修改内容:增加开户行和开户日期 用于6223卡信息查询交易显示
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9951()  
{ 		
	int i=0, ret=0, ac_seqn=0 ,tag=0;
	double bal;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61],prdt_name[31];
	char tmpname[100];
	char cOpn_br_no[11]; /**增加开户行 20110608**/
	long lOpn_date=0;    /**增加开户日期 20110608**/
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;

	memset(cOpn_br_no,0,sizeof(cOpn_br_no));
	
	/** 取值、赋值 **/	
	get_zd_data("1011",ac_no);					/*帐号*/
	pub_base_old_acno( ac_no );   				/**对旧帐号的处理**/

	if( strlen(ac_no)==13 && ac_no[0]!='1' )
	{
		sprintf(acErrMsg,"请输入储蓄帐号!! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!! [%d]",ret);
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

	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		/**先与判断该账户是否已销户 20130226 解决挂失销户后提示不准确的问题**/
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if(g_reply_int)
		{
			sprintf(acErrMsg, "[%s][%d]未找到ac_id[%ld]",__FILE__,__LINE__,g_mdm_ac_rel.ac_id );
			WRITEMSG
			strcpy(g_pub_tx.reply, "B117");
			goto ErrExit;
		}
		if(g_dd_mst.ac_sts[0] == '*')
		{
		sprintf(acErrMsg, "[%s][%d]已销户ac_sts[%s]",__FILE__,__LINE__,g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P245");
		goto ErrExit;
		}
		/**先与判断该账户是否已销户 20130226 解决挂失销户后提示不准确的问题**/
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D140");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已被换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已部提换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已做销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
	{
		sprintf(acErrMsg,"定期一本通帐户不允许存款!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O209");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr, "mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质属性表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
						 g_mdm_ac_rel.ac_id);
	if( ret==100 )
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

	vtcp_log("g_mdm_ac_rel_.ac_seqn=[%d]", g_mdm_ac_rel.ac_seqn);
	if( g_mdm_ac_rel.ac_seqn==9999 )
	{
		ret = sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 ) 
			ac_seqn = prdt_ac_id_c.ac_seqn;
		else if( ret>1 )
		{
			set_zd_long("101E",prdt_ac_id_c.ac_seqn);
		 	ac_seqn = prdt_ac_id_c.ac_seqn;
			tag = 1;
		}
	}
	else if(g_mdm_ac_rel.ac_seqn)
		ac_seqn = g_mdm_ac_rel.ac_seqn;

	vtcp_log("tag=[%d]", tag);
	/*取信息*/
	if(tag==0)
	{
		if(prdt_ac_id_c.prdt_no[0]=='1')
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
							prdt_ac_id_c.ac_id);
			if( ret==100 )
			{
				sprintf(acErrMsg,"帐号不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(name,g_dd_mst.name);
			bal=g_dd_mst.bal;

			if(g_dd_mst.ac_sts[0]=='*')	
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
 				WRITEMSG
  				strcpy(g_pub_tx.reply,"P245");
   				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						 	  g_dd_mst.prdt_no);
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);
			/**增加开户行和开户日期 20110608**/
			strcpy(cOpn_br_no,g_dd_mst.opn_br_no);
			lOpn_date=g_dd_mst.opn_date;
		}
		else if(prdt_ac_id_c.prdt_no[0]=='2')
		{
			ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
						 	 prdt_ac_id_c.ac_id);
			if( ret==100 )
			{
				sprintf(acErrMsg,"帐号不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(name,g_td_mst.name); 
			bal=g_td_mst.bal;

			if(g_td_mst.ac_sts[0]=='*')	
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
 				WRITEMSG
  				strcpy(g_pub_tx.reply,"P245");
   				goto ErrExit;
			}

			ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
								g_td_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"产品不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			if(g_td_parm.dpst_ind[0]=='N')	
			{
				sprintf(acErrMsg,"该定期户不能做存款业务![%d]",ret);
 				WRITEMSG
  				strcpy(g_pub_tx.reply,"O181");
   				goto ErrExit;
			}
			/***add by wanglei 20060915**/
			/***增加对零整帐户续存次数的控制***/
			if(memcmp(prdt_ac_id_c.prdt_no,"209",3)==0 || memcmp(prdt_ac_id_c.prdt_no,"20A",3)==0 \
			|| memcmp(prdt_ac_id_c.prdt_no,"20B",3)==0)
			{
				vtcp_log("[%s][%d] td_mst.lst_date = [%ld] g_pub_tx.tx_date = [%ld]\n", \
				__FILE__,__LINE__,g_td_mst.lst_date,g_pub_tx.tx_date);
				if(pub_base_CalMouths(g_td_mst.lst_date,g_pub_tx.tx_date) >=3)
				{
					sprintf(acErrMsg,"零存整取已经漏存两次以上,不允许续存!");
 					WRITEMSG
  					strcpy(g_pub_tx.reply,"P209");
   					goto ErrExit;
				}
			}
			/***end by wanglei 20060915**/	
			strcpy(prdt_name,g_td_parm.title);
			strcpy(cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);
			/**增加开户行和开户日期 20110608**/
			strcpy(cOpn_br_no,g_td_mst.opn_br_no);
			lOpn_date=g_td_mst.opn_date;
		}
		else
		{
			sprintf(acErrMsg,"非村取款类帐户![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O166");
			goto ErrExit;
		}

		if( cif_type[0]!='1' )
		{
			sprintf(acErrMsg,"请输入储蓄账号[%s][%s]",cif_type, \
					prdt_ac_id_c.prdt_no);
	 		WRITEMSG
	  		strcpy(g_pub_tx.reply,"O164");
	   		goto ErrExit;
		}

		/***下载凭证类型***
		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");

		ret = Dic_data_Sel( g_pub_tx.reply, &vdic_data, \
							"pr_code='note_type' and pr_seq='%s'", 
						 	g_mdm_attr.note_type);
		if( ret==100 )
		{
			sprintf(acErrMsg,"TRANSFER Dic_data ERROR!ret==[%d]",ret);
	 		WRITEMSG
	  		strcpy(g_pub_tx.reply,"O164");
	   		goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		fprintf(fp,"1014|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
   		set_zd_data("1014",vdic_data.pr_seq);

		fclose(fp);
		set_zd_data(DC_FILE_SND,"2");
		***结束***/

		/*返回值*/	
		pub_base_strpack(name);
		vtcp_log("name=[%s],bal=[%lf]",name,bal);
   		set_zd_data("1018",name); 					/*户名*/
   		set_zd_data("101A",cur_no); 				/*币种*/
   		set_zd_data("101B","1"); 					/*默认现转*/
   		set_zd_double("1019",bal); 					/*余额*/
   		set_zd_data("1014",g_mdm_attr.note_type); 	/*凭证种类*/
   		set_zd_data("1015",g_mdm_ac_rel.note_no); 	/*凭证号码*/
   		set_zd_data("101F",prdt_name);			 	/*产品名称*/
   		/**增加开户行和开户日期 20110608**/
   		set_zd_data("0870",cOpn_br_no);			 	/*开户行*/
   		set_zd_long("0470",lOpn_date);			 	/*开户日期*/
	}

	set_zd_long("1012",ac_seqn);
	set_zd_data("1011",ac_no);		/*处理旧帐号*/

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
   
