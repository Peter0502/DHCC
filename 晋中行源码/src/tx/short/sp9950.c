/*************************************************
* 文 件 名: sp9950.c
* 功能描述：根据帐号回显对公帐户信息[对公销户交易]
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
#include "mo_infrm_c.h"
#include "paymach_mana_inf_c.h"
#include "ln_mst_c.h"
#include "ag_unitinfo_c.h"
#include "ac_ac_rel_c.h"
sp9950()  
{ 		
	int i=0,ret=0,flag=0,ac_seqn;
	double bal;
	long cif_no=0;
	long sumlong=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char tmp_memo[100],tmpname[100],prdt_name[31];
	char spe_ind[11];
	char ac_type[2];						/***1-活期、2-对公***/
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct mo_infrm_c g_mo_infrm;
	struct paymach_mana_inf_c g_paymach_mana_inf;
	struct ln_mst_c g_ln_mst;
	struct ag_unitinfo_c g_ag_unitinfo;
        struct ac_ac_rel_c g_ac_ac_rel;
	/*** 初始化系统 ***/
	pub_base_sysinit();

	memset( &g_mo_infrm,0x00,sizeof(struct mo_infrm_c) );
	memset( &g_paymach_mana_inf,0x00,sizeof(struct paymach_mana_inf_c) );
	memset( spe_ind, 0, sizeof(spe_ind) );
	memset( spe_ind, '0', sizeof(spe_ind)-1 );
	memset( &g_pub_intst,0,sizeof(g_pub_intst) );
        memset( &g_ln_mst,0,sizeof(struct ln_mst_c));
        memset( &g_ag_unitinfo,0,sizeof(struct ag_unitinfo_c));
        memset( &g_ac_ac_rel,0,sizeof(struct ac_ac_rel_c));
	/*** 初始化全局变量 ***/
	pub_base_sysinit( );

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);					/*帐号*/
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/

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
		sprintf(acErrMsg,"查找出错!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack(g_mdm_ac_rel.opn_br_no);
	pub_base_strpack(g_pub_tx.tx_br_no);

	if( strcmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no) )
	{
		sprintf(acErrMsg,"请在开户行办理此业务! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"D245");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]=='4')
	{
		sprintf(acErrMsg,"该帐户已做部提，请输入新帐户!! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M017");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]=='*')
	{
		sprintf(acErrMsg,"该帐户已销, 请输入新帐户!! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}

	/* 账户是否有密码器 */
	ret = Paymach_mana_inf_Sel( g_pub_tx.reply, &g_paymach_mana_inf, \
								"ac_no='%s' and flag='0'", ac_no );
	if( !ret )
   	{
   		sprintf( acErrMsg, "该帐户有支付密码器，请先作账户与器具删除!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D246");
		goto ErrExit;
	}
	else if( ret && ret!=100 )
   	{
   		sprintf( acErrMsg, "查找错![%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
						   g_mdm_ac_rel.ac_id);
	if( ret==100)
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(prdt_ac_id_c.prdt_no[0]=='1')
	{
		sprintf(acErrMsg,"对公活期,非定期帐号!");
		WRITEMSG
	  strcpy(g_pub_tx.reply, "O186");
		goto ErrExit;
		
		g_pub_tx.ac_id_type[0] = '1';

		ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
						  prdt_ac_id_c.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		/* 去掉零余额销户 add by martin 2008/12/22 16:11:40*/
		if(g_dd_mst.bal>0)
			{
				sprintf(acErrMsg,"该账号余额不为零[%lf]!! ",g_dd_mst.bal);
			  WRITEMSG
			  strcpy(g_pub_tx.reply,"O077");
			  goto ErrExit;
			}

		strcpy(name,g_dd_mst.name);
		cif_no = g_dd_mst.cif_no;
		bal = g_dd_mst.bal;
		g_pub_tx.tx_amt1 = g_dd_mst.bal;

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
			sprintf(acErrMsg,"查找出错!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		/**检查关联帐户开始 modify by ligl 20060912**/
		/**应该从贷款主表里找**/
		ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')",g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"贷款关联帐号不允许销户!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T044");
			goto ErrExit;
		}
		/*	存款帐号不用检查
		ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"save_ac_id='%ld' and ac_sts in('1','2','3','4','5')",g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"贷款关联帐号不允许销户!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T044");
			goto ErrExit;
		}	
		*****/
		/*检查代理单位帐户*/
		ret=Ag_unitinfo_Sel(g_pub_tx.reply,&g_ag_unitinfo,"bank_acct_no='%s'",ac_no);
		if(ret==0)
		{
		   	sprintf(acErrMsg,"代理关联帐户不允许销户",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T045");
			goto ErrExit;
		} 
		/***完毕***/
		
		/* 修改为新的配合取消零余额销户 modify by martin 2008/12/22 16:12:45
		ret=sql_sum_long("Note_cheq_mst","cnt",&sumlong,"ac_id='%ld' and note_type='001'",g_dd_mst.ac_id);
		if(sumlong>0)
		{
			sprintf(acErrMsg,"此帐户拥有两张以上的现金支票,不允许销户",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T043");
			goto ErrExit;
		}*/
		ret=sql_sum_long("Note_cheq_mst","cnt",&sumlong,"ac_id='%ld' ",g_dd_mst.ac_id);
		if(sumlong>0)
		{
			sprintf(acErrMsg,"活期账中存在凭证记录,不允许销户",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N037");
			goto ErrExit;
		}
		ret=Ac_ac_rel_Sel(g_pub_tx.reply,&g_ac_ac_rel,"ac_id1='%d'",g_dd_mst.ac_id);
		if(ret==0)
		{
		   	sprintf(acErrMsg,"关联帐户不允许销户!!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T044");
			goto ErrExit;
		}
		/**modify end**/
		strcpy(g_pub_tx.ac_no,ac_no);
		strcpy(prdt_name,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
		strcpy(cif_type,g_dd_parm.cif_type);
		strcpy(ac_type,"1");
		strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);

		if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
			g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
			g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
			g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

        	/* 调用计息函数 */
        	g_pub_intst.mode = 1;  /* 销户计息 */
		if( g_dd_mst.intst_type[0]!='0' )
		{
        	ret = pub_base_CalIntst(&g_pub_intst);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"计算利息错误!!");
            	WRITEMSG
            	strcpy(g_pub_tx.reply,"A032");
            	goto ErrExit;
        	}
        }
		else
		{
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;
			g_pub_intst.keepval=0.00;
			g_pub_intst.factval=0.00;
			g_pub_intst.val=0.00;
		}
        sprintf(acErrMsg,"利息为 = [%lf]",g_pub_intst.factval);
        WRITEMSG
      /* 去掉零余额销户 add by martin 2008/12/23 14:00:34*/
		if(g_pub_intst.factval>0)
			{
				sprintf(acErrMsg,"该账号利息不为零[%lf]!! ",g_pub_intst.factval);
			  WRITEMSG
			  strcpy(g_pub_tx.reply,"O076");
			  goto ErrExit;
			}
	}
	else if(prdt_ac_id_c.prdt_no[0]=='2')	
	{
		g_pub_tx.ac_id_type[0]='2';

		ret = Td_mst_Sel( g_pub_tx.reply, &g_td_mst, "ac_id=%ld", \
							prdt_ac_id_c.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(name,g_td_mst.name);
		cif_no = g_td_mst.cif_no;
		bal = g_td_mst.bal;
		g_pub_tx.tx_amt1 = g_td_mst.bal;

		if(g_td_mst.ac_sts[0]=='*')
		{
			sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}

		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
						  g_td_mst.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
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

		/***如果需要到期日、且允许提前支取***/
		if( g_td_parm.atterm_date_ind[0]=='Y' && \
			g_td_parm.fwd_draw_ind[0]=='Y' )
		{
			if( g_pub_tx.tx_date < g_td_mst.mtr_date )
				spe_ind[2]='1';
		}

		strcpy(prdt_name,g_td_parm.title);
		strcpy(cur_no,g_td_parm.cur_no);
		strcpy(cif_type,g_td_parm.cif_type);
		strcpy(ac_type,"2");
		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);

		/*** 调用计算利息积数的函数 **** 定额续取  定额续存*/
    	if ( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' ||
			g_td_parm.td_type[0] == '7' )
    	{
			if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
				g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
				g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
				g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}
    	}
       	/* 计算利息 */
       	/* 加判断如果计息方式是不计息 则不去计算利息 wudawei 20140126 */
        g_pub_intst.mode = 1;  /* 销户计息 */
        if( g_td_mst.intst_type[0]!='0' )
				{
	        ret = pub_base_CalIntst(&g_pub_intst);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"计算存本取息利息错误!!");
	            WRITEMSG
	            strcpy(g_pub_tx.reply,"P220");
	            goto ErrExit;
	        }
	      }
	      else
				{
					g_pub_intst.dealval=0.00;
					g_pub_intst.tax_intst=0.00;
					g_pub_intst.keepval=0.00;
					g_pub_intst.factval=0.00;
					g_pub_intst.val=0.00;
				}
				/* 加判断如果计息方式是不计息 则不去计算利息 wudawei 20140126 end */
        sprintf(acErrMsg,"取款利息为:,[%lf]",g_pub_intst.factval);
        WRITEMSG
	}
	else
	{
		sprintf(acErrMsg,"非存取款类账号! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}

   	set_zd_data("102M",cif_type);
   	set_zd_double("0400",g_pub_intst.factval+bal);
	if( cif_type[0]=='1' )
	{
		sprintf(acErrMsg,"请输入对公账号!! [%d]",cif_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code );
	if( ret )
	{	
		sprintf(acErrMsg,"查询介质参数表错误![%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G102");
		goto ErrExit;
	}

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
	/***
   	set_zd_data("102C",g_mdm_ac_rel.pwd_mach_yn);	*支付密码*
	***/
   	set_zd_data("102C","N");						/*支付密码*/
   	set_zd_data("102U",spe_ind);					/*特殊标志*/
   	set_zd_data("102H",name);						/*户名*/
   	set_zd_data("102V",prdt_name);					/*产品名称*/
	set_zd_data("102J",cur_no);						/*币种*/
	set_zd_data("102K","1");						/*默认现转*/
	set_zd_data("102M",ac_type);					/*定活类型*/
	set_zd_double("102I",bal);						/*余额*/

	if(ac_type[0]=='2')								/***定期***/
	{
		set_zd_data("1023",g_mdm_attr.note_type);	/*凭证类型*/
		set_zd_data("1024",g_mdm_ac_rel.note_no);	/*凭证号码*/
	}

	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");

	/***下载对公摘要列表***/
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

	/***产品选项列表***/
	if(ac_type[0]=='1')						/***活期下载凭证类型***/
	{
		i=0;
		ret=Dic_data_Dec_Sel( g_pub_tx.reply, \
							  "pr_code='pub_taty' order by pr_seq");
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
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***选项列表下载完毕***/

	set_zd_data("1021",ac_no);

OkExit:
	ret=sql_rollback(); 	/*--事务回滚--*/
	ret=sql_begin(); 	/*打开事务*/
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
   
