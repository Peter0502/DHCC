/*************************************************
* 文 件 名: sp9959.c
* 功能描述：根据帐号回显账户序号及信息[储蓄销户交易]
* 作    者: rob
* 完成日期：2003年06月10日
* 修改记录：   
*     1. 日    期:20120613
*        修 改 人:
*        修改内容:使卡号前6位与ICCARDHEADCODE或者CARDHEADCODE相同时可以做该查询交易
                  使67域传值为空时，进行卡状态（销卡，换证）检验，
                  不为空时，跳过检验进行查询
                  使68域为空时进行销卡满足销卡交易的查询，不为空时满足电子现金交易查询的要求
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "card.h"
#include "mdm_attr_c.h"
#include "mo_infrm_c.h"
#include "ln_auth_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ln_mst_c.h"
#include "mo_prdt_prot_c.h" /****20130418****/
sp9959()  
{ 		
	int i=0,ret=0,tag=0;
	long ac_seqn=0;
	double bal;
	double tmp_amt=0.00;
	long cif_no=0;
	long tmp_date=0, end_date=0;/* martin 20080926*/
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61],flag[2],flag1[2];
	char opn_br[8];
	char tmpname[100],prdt_name[31];
	char spe_ind[11];
	char cTx_code[5];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct mo_infrm_c g_mo_infrm;
	struct ln_mst_c g_ln_mst;
	struct mo_prdt_prot_c g_mo_prdt_prot;
	struct ag_peritemmap_c g_ag_peritemmap;
	struct ag_unitinfo_c g_ag_unitinfo;
	long sumlong=0;
	memset(opn_br,0,sizeof(opn_br));
	memset( &g_mo_infrm,0x00,sizeof(struct mo_infrm_c) );
	memset( spe_ind,0x00,sizeof(spe_ind) );
	memset( spe_ind,'0',sizeof(spe_ind)-1 );
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_mo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
	memset(&g_ag_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	memset(&g_ag_unitinfo,0x00,sizeof(struct ag_unitinfo_c));
	memset(cTx_code,0x00,sizeof(cTx_code));
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);						/*帐号*/
	pub_base_old_acno( ac_no );						/**对旧帐号的处理**/
	get_zd_data("0670",flag);
	vtcp_log("[%s][%d]  0670[%s]\n",__FILE__,__LINE__,flag);
	get_zd_data("0680",flag1);
	vtcp_log("[%s][%d]  0680[%s]\n",__FILE__,__LINE__,flag1);                 	
	get_zd_data("0650",cTx_code);              /*七天乐账户销户检查 20130422*/
	vtcp_log("[%s][%d]  0650[%s]\n",__FILE__,__LINE__,cTx_code);                 	

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

	/****增加关于七天乐签约帐户销户时的判断，若未解约即来销户，让其先去解约  add at 20130422****/
	vtcp_log("[%s][%d]  帐户ID==[%ld] 交易代码==[%s]", __FILE__,__LINE__,g_mdm_ac_rel.ac_id,cTx_code);
	if(memcmp(cTx_code ,"2204",4)==0)
	{
		ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&g_mo_prdt_prot,"main_ac_id = %ld and prot_type ='TK7' and sts != '*' ", \
				g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"该帐户为七天通知签约帐户，请先解约再销户!![%s] ret=[%d]",ac_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B361");
			goto ErrExit;
		}else if(ret==100){/****不是签约帐户，继续往下走****/
			vtcp_log("[%s][%d] 该帐户不是七天乐签约帐户!![%s] ret=[%d]",__FILE__,__LINE__,ac_no,ret);
		}else if(ret){
			sprintf(acErrMsg,"查询理财协议登记簿异常!!ret=[%d] 帐号=[%s]",ret,ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"X013");
			goto ErrExit;
		}

	}
	/****end at 20130418****/
	/*YW 20121214 晋中IC卡增加子卡余额回显*/
/*	if(!strcmp(g_mdm_ac_rel.mdm_code,ICCARDMDMCODE)){
		ret = sql_sum_double( "dd_mst","bal",&tmp_amt,"ac_id in(select ac_id from mdm_ac_rel where ac_no in (select app_ac_no from mo_ic_ac_rel where main_ac_no='%s'))",g_mdm_ac_rel.ac_no );
		if(ret)
		{
			sprintf(acErrMsg,"汇总子账户余额出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		set_zd_double("0430",tmp_amt);
	}*/
	if(!strlen(flag))
{/** 不卡挂失状态
	if( g_mdm_ac_rel.note_sts[0]=='1' && memcmp(ac_no,ICCARDHEADCODE,6)!=0  && memcmp(ac_no,CARDHEADCODE,6)!=0)
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else 
	**/
		if( g_mdm_ac_rel.note_sts[0]=='2' )
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
		sprintf(acErrMsg,"该介质已做销户处理![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}
}

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

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
			 		     g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!!");
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
	if(!strlen(flag1)){
	/**检查关联帐户开始 modify by ligl 20060912**/
	ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')",g_mdm_ac_rel.ac_id);
	if(ret==0)
	{
		sprintf(acErrMsg,"贷款关联帐号不允许销户!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"T044");
		goto ErrExit;
	}
  }
	/*	存款帐户不用检查
	ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"save_ac_id='%ld' and ac_sts in('1','2','3','4','5')",g_mdm_ac_rel.ac_id);
	if(ret==0)
	{
		sprintf(acErrMsg,"贷款关联帐号不允许销户!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"T044");
		goto ErrExit;
	}
	*/
	/*使68域为空时进行销卡满足销卡交易的查询，不为空时满足电子现金交易查询的要求*/
	if(!strlen(flag1))
	{
	/*检查代理单位帐户*/
	ret=Ag_unitinfo_Sel(g_pub_tx.reply,&g_ag_unitinfo,"bank_acct_no='%s'",ac_no);
	if(ret==0)
	{
		sprintf(acErrMsg,"代理关联帐户不允许销户",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"T045");
		goto ErrExit;
	}
	ret=Ag_peritemmap_Sel(g_pub_tx.reply,&g_ag_peritemmap,"ac_no='%s'",ac_no);
	if(ret==0)
	{
		if(g_ag_peritemmap.stat[0]=='0')
		{
			sprintf(acErrMsg,"代理关联帐户不允许销户",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T045");
			goto ErrExit;
		}
	} 
	/***完毕***/
	}
	sumlong=0;
	ret=sql_sum_long("Note_cheq_mst","cnt",&sumlong,"ac_id='%ld' and note_type='001'",g_mdm_ac_rel.ac_id);
		if(sumlong>1)
		{
			sprintf(acErrMsg,"此帐户拥有两张以上的现金支票,不允许销户",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T043");
			goto ErrExit;
		}
		/**modify end**/
	/*** 多账户介质的情况 ***/
	if( g_mdm_attr.ac_num_ind[0]=='N' )
	{
		ret = sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 ) 
		{
			ac_seqn = prdt_ac_id_c.ac_seqn;
		}
		else if( ret>1 )
		{
			set_zd_long("102N",prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			set_zd_long("1022",ac_seqn);
			tag = 1;
			/****增加一卡通销户 20140228 张国锋 start**
			tag = 0;
			strncpy(prdt_ac_id_c.prdt_no,"2",1);
			ac_seqn = 200;
			****增加一卡通销户 20140228 张国锋 end***/
		}
	}
	else
	{
		ac_seqn = g_mdm_ac_rel.ac_seqn;
		/**--- xxxx ---- 20030818@ ---**
		set_zd_long("1022",ac_seqn);
		**/
	}

vtcp_log("SSSSS[%d][%s][%d]",tag,prdt_ac_id_c.prdt_no,ac_seqn );
	if( tag==0 )
	{
		if( prdt_ac_id_c.prdt_no[0]=='1' )
		{
			/*** 存折销户检查未登明细 ***/
			if( !strncmp(g_mdm_ac_rel.mdm_code,"0010",4) )
			{
				if(strcmp(g_mdm_ac_rel.note_sts,"1")!=0)
				{
					ret = sql_count("mdm_unprt_hst","ac_id=%ld and ac_seqn=%d", \
									prdt_ac_id_c.ac_id,ac_seqn );
					if( ret<0 )
					{
						sprintf(acErrMsg,"TRANSFER sql_count error" );
						WRITEMSG
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					else if( ret )
					{
						sprintf(acErrMsg,"该帐户存在未登明细，请先补登存折!" );
						WRITEMSG
						strcpy(g_pub_tx.reply,"D239");
						goto ErrExit;
					}
				}
			}

			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret==100 )
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
			if(!strlen(flag))
				{
					if( g_dd_mst.ac_sts[0]=='*' )
					{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
					}
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
				sprintf(acErrMsg,"查找帐号出错!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(name,g_dd_mst.name);
			cif_no = g_dd_mst.cif_no;
			bal = g_dd_mst.bal;
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);
			strcpy(opn_br,g_dd_mst.opn_br_no);
		}
		else if(prdt_ac_id_c.prdt_no[0]=='2')
		{
			ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
						    "ac_id=%ld and ac_seqn=%d", \
						    prdt_ac_id_c.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"td_mst [%d][%d]", prdt_ac_id_c.ac_id,ac_seqn);
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

			/*** 通知存款检查通知登记薄 ***/
			if( g_td_parm.td_type[0]=='4' )
			{
				ret = Mo_infrm_Sel(g_pub_tx.reply,&g_mo_infrm,"ac_id=%ld and \
								   ac_seqn=%d and sts='0'", g_td_mst.ac_id, \
								   g_td_mst.ac_seqn );
				if( ret==100 )
				{
					strcpy( spe_ind ,"0001" );			/*** 未登记支取 ***/
				}
				else if( ret==0 )
				{
					if( g_pub_tx.tx_date < g_mo_infrm.prm_draw_date )
						strcpy( spe_ind , "0002");			/*** 提前支取 ***/
					if( g_pub_tx.tx_date > g_mo_infrm.prm_draw_date )
						strcpy( spe_ind , "0003");			/*** 逾期支取 ***/
					if( pub_base_CompDblVal( g_td_mst.bal,g_mo_infrm.prm_draw_amt )>0 )
						/*** 通知金额小于账户余额 ***/
						strcpy( spe_ind , "0004");	
				}
				else if( ret )
				{
					sprintf(acErrMsg,"read dd_mst error" );
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
			}

			/*** 如果需要到期日、且允许提前支取 ***/
			if( g_td_parm.atterm_date_ind[0]=='Y' && \
				g_td_parm.fwd_draw_ind[0]=='Y' )
			{
				if( g_pub_tx.tx_date < g_td_mst.mtr_date )
				{
					sprintf(acErrMsg,"Azz-->Glimpse-->g_pub_tx.tx_date==[%ld], \
							g_td_mst.mtr_date==[%ld]", \
							g_pub_tx.tx_date,g_td_mst.mtr_date);
					WRITEMSG
					spe_ind[2]='1';
				}else/***增加到期后转存的提前支取情况 martin 20080926***/
					/***增加&&  g_td_parm.tfr_lmt_ind[0]=='Y' 这个条件  把零存整去等产品的提示提前支取去掉 wudawei 20140305***/
				if( g_pub_tx.tx_date > g_td_mst.mtr_date &&
					  g_td_mst.tfr_ind[0]=='Y' &&  g_td_parm.tfr_lmt_ind[0]=='Y')
					{
						tmp_date=g_td_mst.mtr_date;

						while(1)
						{
							if( g_pub_tx.tx_date < end_date )
								{
									sprintf(acErrMsg,"Azz-->Glimpse-->g_pub_tx.tx_date==[%ld], \
					      		end_date==[%ld]", \
					      		g_pub_tx.tx_date,end_date );
					        WRITEMSG
					        vtcp_log( "%d@%s nj td_parm.term=[%d],term_type=[%s]",__LINE__, __FILE__,g_td_parm.term,g_td_parm.term_type);
			            vtcp_log( "%d@%s njnj td_mst.mtr_date[%ld],tfr_ind=[%s] ",__LINE__, __FILE__,g_td_mst.mtr_date,g_td_mst.tfr_ind);
					        spe_ind[2]='1';
									break;
								}else
							if( g_pub_tx.tx_date == end_date)
								{
									sprintf(acErrMsg,"Azz-->Glimpse-->g_pub_tx.tx_date==[%ld], \
					      		end_date==[%ld]", \
					      		g_pub_tx.tx_date,end_date );
					        WRITEMSG
									break;
								}
								
							if( g_td_parm.term_type[0]=='Y'  )
			      	  {
			      	  	end_date=tmp_date+(g_td_parm.term*10000);
			      	  }else
			      	if( g_td_parm.term_type[0]=='M' )
			      	  {
							    pub_base_deadlineM(tmp_date,g_td_parm.term,&end_date);
						    }else
			      	if( g_td_parm.term_type[0]=='D' )
			      		{
			      			pub_base_deadlineD(tmp_date,g_td_parm.term,&end_date);
			      		}
							tmp_date=end_date;
						}
					}/***增加到期后转存的提前支取情况 martin 20080926 end***/
			}

			/*** 教育储蓄户置特殊标志 ***/
			if(!strncmp(g_td_parm.prdt_no,"21",2))
				spe_ind[1]='1';

			strcpy(name,g_td_mst.name);
			cif_no=g_td_mst.cif_no;
			bal=g_td_mst.bal;
			strcpy(prdt_name,g_td_parm.title);
			strcpy(cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);
			strcpy(opn_br,g_td_mst.opn_br_no);
		}
		else 
		{
			sprintf(acErrMsg,"非存取款类帐户![%s]",prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O166");
			goto ErrExit;
		}

	   	set_zd_data("102M",cif_type);

		if( cif_type[0]!='1' && cif_type[0]!='0' )
		{
			sprintf(acErrMsg,"请输入储蓄账号!! [%s]",cif_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O164");
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
		set_zd_data("0020",opn_br);				/*开户机构**/
		set_zd_data("0670",g_dd_mst.sttl_type);/*活期帐户结算类型**/
		set_zd_data("102U",spe_ind);					/*特殊标志*/
    	set_zd_data("102H",name);						/*户名*/
    	set_zd_data("102V",prdt_name);					/*产品名称*/
		set_zd_data("102J",cur_no);						/*币种*/
		set_zd_data("102K","1");						/*默认现转*/
		set_zd_data("0720",g_mdm_ac_rel.note_sts);						/*介质状态*/
		set_zd_double("102I",bal);						/*余额*/
	{
		double d102;
		get_zd_double("102I", &d102);
        vtcp_log( "%d@%s  bal [%lf] d102[%lf]",__LINE__, __FILE__, bal ,d102);
	}
		set_zd_data("1024",g_mdm_ac_rel.note_no);		/*凭证号码*/
		set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*密码支取*/

		if( g_mdm_ac_rel.draw_id_yn[0]=='N' )
		{
	 		set_zd_data("1029","N");                        /*证件支取*/
			set_zd_data("102G",g_cif_id_code_rel.id_type);  /*证件类型*/
		}
		else
		{
			set_zd_data("1029",g_mdm_ac_rel.draw_id_yn);    /*证件支取*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*证件类型*/
		}
	}

	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	ret = Dic_data_Sel(g_pub_tx.reply,&vdic_data, \
					   "pr_code='note_type' and pr_seq='%s'", \
					   g_mdm_attr.note_type);
	if( ret ) goto ErrExit;

	pub_base_strpack(vdic_data.pr_seq);
	pub_base_strpack(vdic_data.content);

	set_zd_data("1023",vdic_data.pr_seq);
	fprintf(fp,"1023|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
	Dic_data_Clo_Sel();
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***选项列表下载完毕***/

	set_zd_data("1021",ac_no);

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
   
