/*************************************************
* 文 件 名: sp9731.c
* 功能描述：根据帐号回显账户序号及信息[公共转帐取款快查]
* 作    者: rob
* 完成日期：2003年02月19日
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
#include "com_item_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "gl_mst_c.h"
#include "hv_brno_orno_c.h"/**YHBP**/

sp9731()
{
	int i=0,ret=0,flag=0,tag=0;
	long ac_seqn= 0;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61],tmp_inf[3];
	char ac_type[2],tmp_memo[100],tmpname[100],prdt_name[31];
	char node0[8];   /*初始节点,晋中网银专设 add by hzh 20130604*/
	FILE *fp;
  char in_out='0'; /**0.科目  内部帐 1.对公活期帐户**/

  
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
  struct hv_brno_orno_c hv_brno_orno;/**YHBP**/

	struct com_item_c g_com_item;
	struct gl_mst_c     g_gl_mst;
	char spe_ind[21];
	char opn_br_no[6];/**YHBP**/
	struct mo_prdt_prot_c sMo_prdt_prot;
	struct mo_prot_ac_rel_c sMo_prot_ac_rel;
	struct dd_mst_c sDd_mst_virtual;
	memset(opn_br_no,'\0',sizeof(opn_br_no));
	memset(&hv_brno_orno, 0 , sizeof(hv_brno_orno));
	memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));

	memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
	memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    
	/**YHBP**/
	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(node0, 0x0, sizeof(node0));
	 
	/** 取值、赋值 **/
	get_zd_data("0030",opn_br_no); /**YHBP**/
	get_zd_data("1021",ac_no);
	get_zd_data("0270",node0); /*初始节点,晋中网银专设 add by hzh 20130604 */
	vtcp_log("%s,%d,初始节点[%s]",__FILE__,__LINE__,node0);
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/
  vtcp_log("帐号为：[%s]",ac_no);
	get_zd_data("102L",tmp_inf);				/* 摘要 */
  i=strlen(ac_no);
	if(i==3 || i==5 || i==7)							/*科目与内部帐号*/
	{
    vtcp_log("对科目的处理");
    in_out='0';
    set_zd_data("0690",&in_out);
    /**begin YHBP 根据机构号查询行号**/
    ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",opn_br_no);
    if( ret )
    {
        sprintf(acErrMsg,"查开户行行号记录不存在!![%s]",ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"M003");
        goto ErrExit;
    }
    vtcp_log("行号：[%s]",hv_brno_orno.or_br_no);
    /**end YHBP**/
	 	/*根据科目号查询科目名称*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_no=[%s]", ac_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "查询公共科目表异常!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"该科目下有账户或有子科目!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
		 	goto ErrExit;
		}

	 	/*根据科目号查询余额及借贷方向*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"查询公共科目表异常!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
                set_zd_double("1203",g_gl_mst.dr_bal);   /*余额*/
		/*科目借贷类型*/
		if(g_com_item.dc_ind[0]=='1')
		{
			set_zd_data("0220","借");
			set_zd_double("0410",g_gl_mst.dr_bal);

                        set_zd_double("0390",g_gl_mst.dr_bal);  /*当前借方余额*/
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("0220","贷");
			set_zd_double("0410",g_gl_mst.cr_bal);

                        set_zd_double("0390",g_gl_mst.cr_bal);  /*当前贷方余额*/
         	}
		else if(g_com_item.dc_ind[0]=='0')		/*双方*/
		{
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("0220","借");
				set_zd_double("0410",g_gl_mst.dr_bal);
			}
			else if(ret==-1)
			{
				set_zd_data("0220","贷");
				set_zd_double("0410",g_gl_mst.cr_bal);
			}
		}
		set_zd_data("0250",g_com_item.acc_name);       /*户名*/
		set_zd_data("0210",g_gl_mst.cur_no);           /*币种*/


   /**以下是FD120的赋值**/
                set_zd_data("1201",ac_no);
                set_zd_data("1202",g_com_item.acc_name); /*户名*/
                                                         /*1203余额上面已添加*/
                set_zd_data("1204",g_gl_mst.cur_no);     /*币种*/
                set_zd_data("1205",  "2");          /*现转标志2。转 1 现*/

		/** add.begin.gyf.20060920 为了和检查帐户时保持一致，这里增加对102域部分信息赋值 **/
		set_zd_data("102H",g_com_item.acc_name);
                set_zd_double("102I",g_gl_mst.dr_bal);   /*余额*/
                set_zd_data("102J",g_gl_mst.cur_no);     /*币种*/
                set_zd_data("102K","2");             /*现转标志2。转 1 现*/
		strcpy(cif_type,"2");	/** 当对公户类型处理 **/
		strcpy(ac_type,cif_type);
		set_zd_data("102M",ac_type);
		set_zd_data("1029","N");
		set_zd_data("1026","N");
		strcpy(prdt_name,"科目账:");
		strcat(prdt_name,g_com_item.acc_name);
		set_zd_data("102V",prdt_name);
		/** add.end.gyf.20060920 **/

		set_zd_data("0520",hv_brno_orno.or_br_no);/**YHBP 传送行号 **/
	}
else {
					
         ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
         if( ret )
         {
                sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"M003");
                goto ErrExit;
         }
         /**begin YHBP 根据机构号查行号**/
         set_zd_data("0880",g_mdm_ac_rel.draw_pwd_chg);
         
        /** begin 为网银还贷交易特别处理,lzy 20141226**/
				char net_ind[10]={0};
				get_zd_data("0100",net_ind); /**lzy 20141226**/
				vtcp_log("9731net_ind[%s]",net_ind);
				if ( memcmp(net_ind,"netb",4) == 0 )
				{
					set_zd_data("0860",g_mdm_ac_rel.opn_br_no);
				}
        /** end 为网银还贷交易特别处理,lzy 20141226**/

        ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
        if( ret )
        {
            sprintf(acErrMsg,"查开户行行号记录不存在!![%s]",ac_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M003");
            goto ErrExit;
        }
        vtcp_log("行号: [%s]",hv_brno_orno.or_br_no);
        /**end YHBP 结束**/
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

         ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
                                              g_mdm_ac_rel.ac_id);
         if( ret )
         {
                 sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"M003");
                 goto ErrExit;
         }

         if(prdt_ac_id_c.ac_id_type[0] == '1')
         {
                 in_out='2';
                 set_zd_data("0690",&in_out);
         }
         else if(prdt_ac_id_c.ac_id_type[0] == '9')
         {
                 in_out='1';
                 set_zd_data("0690",&in_out);
         }
/*	set_zd_int("1022",prdt_ac_id_c.ac_seqn);* 长治加 */

vtcp_log("in_out is [%c]",in_out);

	/** delete.begin.gyf.20060920
	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') )
	delete.end.gyf.20060920 **/

	/** modify.gyf.20060920 增加转帐对内部帐户的支持 **/
	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') && prdt_ac_id_c.prdt_no[0]!='9' )
	{
		sprintf(acErrMsg,"非存取款类账号! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}
	set_zd_data("0930",prdt_ac_id_c.ac_id_type);
	set_zd_data("0240",prdt_ac_id_c.prdt_no); /*新增返回产品号*/
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
			ac_seqn = prdt_ac_id_c.ac_seqn;
		}
		else if(ret>1)
		{
			set_zd_long("102N",prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			set_zd_long("1022",ac_seqn);
			tag = 1;
		}
	}
	else
		ac_seqn = g_mdm_ac_rel.ac_seqn;

	if( tag==0 )
	{
		/** begin.gyf.20060920
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

			if(g_dd_mst.ac_sts[0]=='*')
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
			sprintf(acErrMsg,"请输入活期帐号，其他账户不能作转帐方![%s]", \
					prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O227");
			goto ErrExit;
		}

		strcpy(ac_type,cif_type);
		set_zd_data("102M",ac_type);

		ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
									"cif_no=%ld",cif_no);
		if( ret )
		{
			sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "G102");
			goto ErrExit;
		}

		end.gyf.20060920 ***/

		/** add.begin.gyf.20060920 增加对内部帐户的支持 **/
		switch( prdt_ac_id_c.prdt_no[0])
		{
		case '1':
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

			if(g_dd_mst.ac_sts[0]=='*')
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
				
	             /**1.检查.是否是七天乐账户.**/
             vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_dd_mst.ac_id);
             ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_dd_mst.ac_id);
             if(ret == 100)
             {
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**无七天乐签约.继续原程序.**/
             }
             else if(ret)
             {
                 sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
                 WRITEMSG
                 strcpy( g_pub_tx.reply,"D103");
                 goto ErrExit;
             }
             else
             {
                 vtcp_log("[%s][%d]主账户 zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);

                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_dd_mst.ac_id);
                 if(ret == 100)
                 {
                   /**七天乐签约信息存在.与虚拟账户对照关系不存在.**/
                     sprintf(acErrMsg,"七天乐签约信息存在.与虚拟账户对照关系不存在.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 
                 /**2.七天乐账户信息..签约账户,虚拟账户信息已取得.**/
                 ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"dd_mst中无虚拟账户信息.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 bal=g_dd_mst.bal+sDd_mst_virtual.bal;
                               vtcp_log("[%s][%d]虚拟账户余额bal=[%17.2f]",__LINE__,__FILE__,bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
             }
             
                 /* end 20130306*/
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);

			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",bal);
			set_zd_data("0730",g_dd_mst.od_ind);          /*透支标志*/

			ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
										"cif_no=%ld",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
			}
			set_zd_data("0720",g_dd_mst.hold_sts);/* 是否冻结 */
			vtcp_log("[%s][%d]玩笑g_dd_mst.hold_sts==[%s]",__FILE__,__LINE__,g_dd_mst.hold_sts);
			break;
		case '2':
				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_td_mst.name);
			cif_no = g_td_mst.cif_no;
			bal = g_td_mst.bal;

			if(g_td_mst.ac_sts[0]=='*')
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
							"prdt_no='%s'",g_td_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_td_parm.title);
			strcpy(cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);

			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",g_td_mst.bal);

			ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
										"cif_no=%ld",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
			}
			set_zd_data("0720",g_td_mst.hold_sts);/* 是否冻结 */
			vtcp_log("[%s][%d]玩笑g_td_mst.hold_sts==[%s]",__FILE__,__LINE__,g_td_mst.hold_sts);
			break;
		case '9':
			ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_in_mst.name);
			bal = g_in_mst.bal;

			if(g_in_mst.sts[0]=='*')
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = In_parm_Sel(g_pub_tx.reply,&g_in_parm,
							"prdt_no='%s'",g_in_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_in_parm.prdt_mo);
			strcpy(cur_no,g_in_parm.cur_no);
			strcpy(cif_type,"2");	/** 当对公户类型处理 **/
			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",g_in_mst.bal);
      set_zd_data("0730",g_in_mst.od_ind);          /*透支标志*/
			g_mdm_ac_rel.draw_id_yn[0] = 'N';	/** 因为内部帐户没有值，所以赋一个 **/
			set_zd_data("0720","0");/* 是否冻结 */
			vtcp_log("[%s][%d]玩笑g_td_mst.hold_sts==[%s]",__FILE__,__LINE__,g_td_mst.hold_sts);
			break;
		default:
			sprintf(acErrMsg,"请输入活期帐号，其他账户不能作转帐方![%s]", \
					prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O227");
			goto ErrExit;
		}


		/*** add.end.gyf.20060920 ***/
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);

		pub_base_strpack(name);
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);

       set_zd_data("0250",name);
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);
        /**对于网银交易这里不需要改变39域的值，保证平台从39域传来的交易金额继续往后传 20130604 hzh**/
        if(memcmp(node0, "nbanklvhv", 9)) 
		{
        set_zd_double("0390",bal);
    }
        /**对于网银交易这里不需要改变39域的值，保证平台从39域传来的交易金额继续往后传 20130604 hzh**/
    	set_zd_data("102C","N");						/*是否支付密码器*/
    	set_zd_data("102H",name);						/*户名*/
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);
    	set_zd_data("102V",prdt_name);					/*产品名称*/
		set_zd_data("102J",cur_no);						/*币种*/
		set_zd_data("102K","1");						/*默认现转*/
                set_zd_data("0680","1");

		set_zd_double("102I",bal);						/*余额*/
		set_zd_data("1024",g_mdm_ac_rel.note_no);		/*凭证号码*/
                set_zd_data("1025",g_mdm_ac_rel.draw_uncon_yn); /*是否任意支取*/
		set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*密码支取*/
                set_zd_data("1027",g_mdm_ac_rel.qry_pwd);  /*查询密码*/
		vtcp_log("[%d]SPE_IND[%s]",__LINE__,spe_ind);
		set_zd_data("1028",g_mdm_ac_rel.draw_pwd); /*支取密码*/
		set_zd_data("102U",spe_ind);
  		/*特殊标志*/
                set_zd_data("102B",g_mdm_ac_rel.draw_seal_yn); /*是否凭印鉴支取*/

		if(flag==1&&g_mdm_ac_rel.draw_id_yn[0]=='N')
		{
	 		set_zd_data("1029","N");                        /*证件支取*/
			set_zd_data("102G",g_cif_id_code_rel.id_type);  /*证件类型*/
			set_zd_data("102A",g_cif_id_code_rel.id_no);    /*证件号码*/
		}
		else if(flag==1)
		{
			set_zd_data("1029","Y");                        /*证件支取*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*证件类型*/
			set_zd_data("102A",g_mdm_ac_rel.id_no);         /*证件号码*/
		}
		else
		{
			set_zd_data("1029",g_mdm_ac_rel.draw_id_yn);    /*证件支取*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*证件类型*/
			set_zd_data("102A",g_mdm_ac_rel.id_no);         /*证件号码*/
		}
        set_zd_data("0250",name);
        set_zd_data("0620",g_mdm_ac_rel.id_no);
        set_zd_data("0520",hv_brno_orno.or_br_no);/**YHBP 传送行号 **/
	}

}

	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	if( ac_type[0]=='2' )
	{
		if( strcmp(tmp_inf,"01" ) )	/* 有初值 */
		{
			ret=Dic_data_Sel( g_pub_tx.reply,&vdic_data,"pr_code='tran_memo' and pr_seq='%s' " , tmp_inf);
			if( ret==100 )
			{
				strcpy( tmp_inf , "01" );
			}else if(ret){
				goto ErrExit;
			}else{
				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);
				set_zd_data("102L",vdic_data.pr_seq);
				fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			}
		}
		if( strcmp(tmp_inf,"01")==0)
		{
			/* 若无初值 */
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
		}
	}
    if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) && ac_type[0]=='1')
	{
		memset( &g_mdm_attr, 0x00, sizeof(struct mdm_attr_c) );
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

		/***选项列表***/
		ret = Dic_data_Dec_Sel( g_pub_tx.reply, \
							 "pr_code='per_dety' and pr_seq='%s'",note_no);
		if( ret ) goto ErrExit;
		i=0;
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
			fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			i++;
		}
		Dic_data_Clo_Sel();
	}
	else
	{
		if( ac_type[0]=='2' )			/*下载对公取款凭证类型*/
		{
			/***选项列表***/
			i=0;
			ret=Dic_data_Dec_Sel( g_pub_tx.reply, \
				"pr_code='pub_taty' and pr_seq[1,3]<>'001' order by pr_seq");
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
					strcpy(note_no,vdic_data.pr_seq);
					set_zd_data("1023",vdic_data.pr_seq);
				}
				fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***选项列表下载完毕***/

	set_zd_data("1021",ac_no);

    double tmp_bal=0;
    get_zd_double("0410",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!注意  余额[%.2f]小于0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("0410",tmp_bal);
    }
    tmp_bal=0;
    get_zd_double("0390",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!注意  余额[%.2f]小于0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("0390",tmp_bal);
    }
    tmp_bal=0;
    get_zd_double("102I",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!注意  余额[%.2f]小于0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("102I",tmp_bal);
    }

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

