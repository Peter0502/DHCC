/*************************************************
* 文 件 名: sp9952.c
* 功能描述：根据帐号回显账户序号及信息[储蓄取款交易]
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 

#define EXTERN
#include "string.h" 
#include "public.h"
#include "prdt_ac_id_c.h"

#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "mo_prdt_prot_c.h" /****add at 20130519****/

sp9952()  
{ 		
	int i=0,ret=0,flag=0,tag=0;
	long ac_seqn=0;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char tmpname[100],prdt_name[31];
	char cTx_code[5]; /****add at 20130519****/
	double hold_amt=0.00; /* add by martin 2008-11-24 13:00*/
	char ac_sts[2],hold_sts[2]; /* add by martin 2008-11-24 13:00*/
	FILE *fp;

	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct mo_prdt_prot_c g_mo_prdt_prot;

	memset(cTx_code,0x00,sizeof(cTx_code));
	memset(&g_mo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);		/*帐号*/
	get_zd_data("0650",cTx_code);		/****交易代码****//****add at 20130519****/

	pub_base_old_acno( ac_no );						/* 旧账号转化 */
	set_zd_data("1021",ac_no);						/*帐号*/

	if( strlen(ac_no)==13 && ac_no[0]!='1' )
	{
		sprintf(acErrMsg,"请输入储蓄帐号!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

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

	if( g_mdm_ac_rel.note_sts[0]=='1' && memcmp(ac_no,"621223",6) && memcmp(ac_no,"621780",6))/**要求IC和磁条卡挂失后可以查余额**/
	{
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
		sprintf(acErrMsg,"该介质已销户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}

	/****增加关于七天乐签约帐户销户时的判断，若未解约即来销户，让其先去解约  add at 20130519****/
	vtcp_log("[%s][%d]  帐户ID==[%ld] 交易代码==[%s]", __FILE__,__LINE__,g_mdm_ac_rel.ac_id,cTx_code);
	if(memcmp(cTx_code ,"2202",4)==0)
	{
		ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&g_mo_prdt_prot,"main_ac_id = %ld and prot_type ='TK7' and sts != '*' ", \
			g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"该帐户为七天通知签约帐户，请先解约再销户!![%s] ret=[%d]",ac_no,ret);
			set_zd_data("0710","1");
		}else if(ret==100){/****不是签约帐户，继续往下走****/
			vtcp_log("[%s][%d] 该帐户不是七天乐签约帐户!![%s] ret=[%d]",__FILE__,__LINE__,ac_no,ret);
			set_zd_data("0710","0");
		}else if(ret){
			sprintf(acErrMsg,"查询理财协议登记簿异常!!ret=[%d] 帐号=[%s]",ret,ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"X013");
			goto ErrExit;
		}

	}
	/****end at 20130519****/

	if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
	{
		sprintf(acErrMsg,"定期一本通帐户不允许做取款业务!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O210");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
					 	  g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d] [%ld]",
				ret, g_mdm_ac_rel.ac_id);
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

	/*卡的情况*/
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
		{
			ac_seqn = prdt_ac_id_c.ac_seqn;
		}
		else if(ret>1)
		{
			set_zd_long("102N",prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			/*set_zd_int("1022",ac_seqn); mod by chenggongxun 20091018 转移到了当前大括号外
			tag = 1;*/
		}
		set_zd_long("1022",ac_seqn);/* add by chenggongxun 20091018*/
		tag = 1;/* add by chenggongxun 20091018*/
	}
	else
		ac_seqn = g_mdm_ac_rel.ac_seqn;

	memset(cif_type,'0',sizeof(cif_type));

	if( tag==0 )
	{
		if(prdt_ac_id_c.prdt_no[0]=='1')
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%ld", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"read dd_mst error" );
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

			strcpy(name,g_dd_mst.name);
			strcpy(ac_sts,g_dd_mst.ac_sts);
			strcpy(hold_sts,g_dd_mst.hold_sts);
			cif_no = g_dd_mst.cif_no;
			bal = g_dd_mst.bal;
			hold_amt=g_dd_mst.hold_amt;

			if(g_dd_mst.ac_sts[0]=='*')
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
								g_dd_mst.prdt_no);
			if( ret==100 )
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
			cif_no = g_dd_mst.cif_no;
		}
		else if(prdt_ac_id_c.prdt_no[0]=='2')
		{
			ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%ld", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"read dd_mst error" );
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

			strcpy(name,g_td_mst.name);
			strcpy(ac_sts,g_td_mst.ac_sts);
			strcpy(hold_sts,g_td_mst.hold_sts);
			bal=g_td_mst.bal;
			hold_amt=g_td_mst.hold_amt;

			if( g_td_mst.ac_sts[0]=='*' )
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
				sprintf(acErrMsg,"read td_parm error" );
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

			if( g_td_parm.td_type[0]!='2' && g_td_parm.td_type[0]!='3' && \
				g_td_parm.td_type[0]!='4' )
			{
				sprintf(acErrMsg,"该账户不允许作取款交易!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O196");
				goto ErrExit;
			}

			/*** 定活两便,通知存款不能做取款 ***/
			if( !strncmp(g_td_parm.prdt_no,"24",2) )
			{
				sprintf(acErrMsg,"该账户不允许作取款交易!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O196");
				goto ErrExit;
			}

			strcpy(prdt_name,g_td_parm.title);
			strcpy(cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);
			cif_no = g_td_mst.cif_no;
		}
		else 
		{
			sprintf(acErrMsg,"该帐户不能做取款业务![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O196");
			goto ErrExit;
		}

	   	set_zd_data("102M",cif_type);

		if( cif_type[0]!='1' )
		{
			sprintf(acErrMsg,"请输入储蓄账号!! [%s]",cif_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O164");
			goto ErrExit;
		}

		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
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
			sprintf(acErrMsg,"查询错误",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}

		pub_base_strpack(name);
    	set_zd_data("102H",name);						/*户名*/
    	set_zd_data("102V",prdt_name);					/*产品名称*/
		set_zd_data("102J",cur_no);						/*币种*/
		set_zd_data("102K","1");						/*默认现转*/
		set_zd_double("102I",bal);						/*余额*/
		set_zd_data("1024",g_mdm_ac_rel.note_no);		/*凭证号码*/
		set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*密码支取*/
		set_zd_data("102B",ac_sts); 		/*帐户状态*/
    set_zd_data("102O",hold_sts); 		/*冻结状态*/
    set_zd_double("102S",hold_amt); 						/*冻结金额*/

		if(flag==1&&g_mdm_ac_rel.draw_id_yn[0]=='N')
		{
	 		set_zd_data("1029","N");                        /*证件支取*/
			set_zd_data("102G",g_cif_id_code_rel.id_type);  /*证件类型*/
			/*
			set_zd_data("102A",g_cif_id_code_rel.id_no);    *证件号码*
			*/
		}
		else if(flag==1)
		{
			set_zd_data("1029","Y");                        /*证件支取*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*证件类型*/
			/*
			set_zd_data("102A",g_mdm_ac_rel.id_no);         *证件号码*
			*/
		}
		else
		{
			set_zd_data("1029",g_mdm_ac_rel.draw_id_yn);    /*证件支取*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*证件类型*/
			/*
			set_zd_data("102A",g_mdm_ac_rel.id_no);         *证件号码*
			*/
		}
	}

	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

    if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
	{
		ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
				  		   g_mdm_ac_rel.mdm_code);
		if( ret==100 )
		{
			sprintf(acErrMsg,"查询介质属性表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B004");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(note_no,g_mdm_attr.note_type);

		/***选项列表***/
		ret = Dic_data_Dec_Sel( g_pub_tx.reply, \
							 "pr_code='per_dety' and pr_seq='%s'",note_no);
		if( ret ) goto ErrExit;

		while(1)
		{
			ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
			if( ret==100 ) break;
			if( ret ) goto ErrExit;

			pub_base_strpack(vdic_data.pr_seq);
			pub_base_strpack(vdic_data.content);

			if( i==0 )  
			{
				set_zd_data("1023",vdic_data.pr_seq);
				strcpy(note_no,vdic_data.pr_seq);
			}
			fprintf(fp,"1023|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			i++;
		}
		Dic_data_Clo_Sel();
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***选项列表下载完毕***/

	set_zd_data("1021",ac_no);      /*处理旧帐号*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("1021","");      /*处理旧帐号*/
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
