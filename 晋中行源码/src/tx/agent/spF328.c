/*************************************************
* 文 件 名: spF328.c
* 功能描述：保函赔偿处理
*
* 作    者: mike
* 完成日期: 2004年08月16日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"
#include "mo_bail_rate_c.h"

spF328()  
{ 	
	double	tx_amt1,tx_amt2,tx_amt3,tx_amt4;
	struct	protect_note_c pro_note;
	int	ret=0;
	int	flag;
	char	ys_ac_no[24];
	struct	mo_bail_rate_c		sMoBailRate;	/* 保证金利率登记薄 */
	struct	mo_bail_rate_c		sMoBailRate1;	/* 保证金利率登记薄 */
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	HQdealval=0.00;
	char	intst_flag[2];

	/** 数据初始化 **/
	flag=1;
	tx_amt1=0.00;
	tx_amt2=0.00;
	tx_amt3=0.00;
	tx_amt4=0.00;
	memset( &pro_note, 0x00, sizeof(struct protect_note_c));
	memset( ys_ac_no , 0x00, sizeof( ys_ac_no ));
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );

	pub_base_sysinit();

	pro_note.trace_no=g_pub_tx.trace_no;
	pro_note.tx_date=g_pub_tx.tx_date;
	strcpy(pro_note.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(pro_note.tel,g_pub_tx.tel);

	get_zd_data  ("0300", pro_note.pro_ac_no);
	get_zd_long  ("0280",&pro_note.pro_cif_no);
	get_zd_data  ("0310", pro_note.ben_ac_no);
	get_zd_long  ("0290",&pro_note.ben_cif_no);
	get_zd_data  ("0320", pro_note.bail_ac_no);
	get_zd_data  ("0330", pro_note.pact_no);
	get_zd_long  ("0440",&pro_note.opn_date);
	get_zd_long  ("0480",&pro_note.time_limt);
	get_zd_long  ("0450",&pro_note.end_date);
	get_zd_double("0390",&pro_note.sum_amt);
	get_zd_double("0400",&pro_note.amt);
	get_zd_double("0410",&pro_note.bail_amt);
	get_zd_double("0850",&pro_note.ratio);
	get_zd_data  ("0220", pro_note.mort_typ);
	get_zd_data  ("0370", pro_note.mort_ac_no);
	get_zd_data  ("0961", pro_note.pro_name);
	get_zd_data  ("0962", pro_note.pro_addr);
	get_zd_data  ("0963", pro_note.ben_name);
	get_zd_data  ("0964", pro_note.ben_addr);
	get_zd_double("0420",&pro_note.amends_amt);
	get_zd_data  ("1231", ys_ac_no);
	get_zd_double("0840",&pro_note.over_rate);
	strcpy(g_pub_tx.ac_no,pro_note.pro_ac_no);
	g_pub_tx.cif_no=pro_note.pro_cif_no;
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	g_pub_tx.tx_amt1=pro_note.sum_amt;
	g_pub_tx.tx_amt2=pro_note.amt;
	g_pub_tx.tx_amt3=pro_note.bail_amt;
	g_pub_tx.tx_amt4=pro_note.amends_amt;
	g_pub_tx.rate=pro_note.ratio;
	strcpy(g_pub_tx.brf, "保函赔偿");
vtcp_log("RATIO ratio = [%f] ratio=[%f][%s->%d]",pro_note.ratio,g_pub_tx.rate,__FILE__,__LINE__);

	/**---- hao ---- 20051113 ----- 增加担保保证金计息 ---**/
	strcpy(intst_flag,"0");
	if( pub_base_CompDblVal(pro_note.bail_amt,0.00)>0 )
	{
	   ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",pro_note.pact_no);
   		if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret==100 )	
		{
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;					/** 应税利息 **/
			g_pub_intst.factval=0.00;					/** 实付利息 **/
			g_pub_intst.val=0.00;						/** 利息税 **/
			g_pub_intst.keepval=0.00;					/** 保值利息 **/
			set_zd_data("0153","52101");                /** 保证金利息 **/
			set_zd_double("102P",g_pub_intst.dealval);
			strcpy(intst_flag,"0");
		}
   		else if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		else
		{
			strcpy(intst_flag,"1");
			memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );
			ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
			if( ret )
			{
				sprintf( acErrMsg, "取利率错误!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "O195" );
				goto ErrExit;	
			}

			/** 判断是否到期 **/
			if( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** 提前支取 **/
			{
				/** 计算天数 **/
				if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
				day=0;
			}
			else
			{
				/** 计算天数 **/
				if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
				if( pub_base_CalDays(sMoBailRate.end_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
   			}

			sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);
			/** 计算利息 **/
			if( pub_base_CompDblVal(pro_note.bail_amt,sMoBailRate.amt) > 0 )
			{
       			HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=0.00;
			}
			else
			{
       			HQdealval = HQ_rate * pro_note.bail_amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * pro_note.bail_amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=sMoBailRate.amt-pro_note.bail_amt;
				sMoBailRate.amt=pro_note.bail_amt;
			}
			g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);

			/** 更新保证金计息金额 **/
			sMoBailRate.intst_date=g_pub_tx.tx_date;
			sMoBailRate.end_trace_no=g_pub_tx.trace_no;
			sMoBailRate.intst_amt=g_pub_intst.dealval;
			strcpy(sMoBailRate.sts,"*");
			ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
   			{
   				sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,pro_note.pact_no);
       		 	WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
       		 	goto ErrExit;
   			}
			Mo_bail_rate_Clo_Upd();

			/** 插入新的保证金利息登记薄 **/
			if( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
			{
				ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
				if( ret )
   				{
   					sprintf( acErrMsg, "执行Mo_bail_rate_Ins错![%d]",ret);
        			WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
        			goto ErrExit;
   				}
			}
			g_pub_intst.tax_intst=g_pub_intst.dealval;	/** 应税利息 **/
			g_pub_intst.factval=g_pub_intst.dealval;	/** 实付利息 **/
			g_pub_intst.val=0.00;						/** 利息税 **/
			g_pub_intst.keepval=0.00;					/** 保值利息 **/

			/*pro_note.bail_amt=pro_note.bail_amt+g_pub_intst.dealval;*/
			set_zd_data("0153","52101");                /** 保证金利息 **/
			set_zd_double("102P",g_pub_intst.dealval);
		}
	}
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	if( intst_flag[0]=='1' )		/*** 保证金计息了 ***/
	{
		strcpy(g_pub_tx.note_type,"Y");
		if ( pub_base_CompDblVal(g_pub_intst.dealval)<0 )
		{
			sprintf(acErrMsg,"计算利息错误错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}
		if ( pubf_acct_proc("F326") )
		{
			sprintf(acErrMsg,"登记担保保证金会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/**---------- hao ---------------- end ------------**/

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		goto ErrExit;
	}

	/**---- hao ---- 20051113 ----- 增加担保保证金利息入客户帐 ---**/
	if( intst_flag[0]=='1' )		/*** 保证金计息了 ***/
	{
		set_zd_data("1011",sMoBailRate.ac_no);
		set_zd_int ("1012",1);
		set_zd_data("1016","贷 收息帐户");
		set_zd_data("101A","01");
		set_zd_data("101B","2");                /*转帐*/
		set_zd_double("1013",g_pub_intst.dealval);
		if ( pub_base_my_func("D099","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"贷 申请人帐户错![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

	sprintf(acErrMsg," ac_no1=[%s]",ys_ac_no);
	WRITEMSG

	/*** 业务处理开始 ***/
	sprintf(acErrMsg,"赔偿金额保证金金额 amends_amt=[%lf] bail_amt=[%lf]",
		pro_note.amends_amt,pro_note.bail_amt);
	WRITEMSG

	/** 赔偿金额大于保证金金额，需要检查分户帐 **/
	if ( pro_note.amends_amt > pro_note.bail_amt )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",pro_note.pro_ac_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"介质账号对照表中不存在此记录!!erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}
		else if( ret )
		{
				sprintf( acErrMsg,"查询介质帐号对照表异常!!erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}

		if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)g_mdm_ac_rel.ac_seqn=1;

		/* 根据显示帐号和账号序号查询主文件 */
		ret=pub_base_disac_file(pro_note.pro_ac_no, g_mdm_ac_rel.ac_seqn, g_pub_tx.ac_type);
		if( ret )
		{
			sprintf( acErrMsg,"根据显示帐号和账号序号查询主文件erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}
		tx_amt1 = g_dd_mst.bal - ( pro_note.amends_amt - pro_note.bail_amt ) -1 ;

		sprintf(acErrMsg," amends_amt=[%lf] bail_amt=[%lf] bal=[%lf] tx_amt1=[%lf]",
			pro_note.amends_amt,pro_note.bail_amt,g_dd_mst.bal,tx_amt1);
		WRITEMSG

		/** 申请人帐户余额不够扣 需要垫款  **/
		if ( tx_amt1 > 0.00 )
		{
			tx_amt1 = pro_note.amends_amt - pro_note.bail_amt; /* 借申请人201发生额 */
			tx_amt2 = 0.00;
			flag=2; /* 保证金不够扣、申请人201帐户够扣 */
		}
		else
		{
			tx_amt1 = g_dd_mst.bal - 1; /* 借申请人201发生额 */
			tx_amt2 = pro_note.amends_amt - pro_note.bail_amt - g_dd_mst.bal + 1; /* 垫付金额 */
			flag=3; /* 保证金与申请人201帐户都不够扣 需要垫付 */
		}
		tx_amt3 = pro_note.amends_amt; /* 26203 发生额 */
		tx_amt4 = 0.00; /* 贷申请人201发生额 */
	}
	else
	{
		tx_amt3 = pro_note.bail_amt; /* 26203 发生额 */
		tx_amt4 = pro_note.bail_amt - pro_note.amends_amt; /* 贷申请人201发生额 */
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	ret = sql_execute("UPDATE protect_note SET protect_sts='1' WHERE pact_no='%s'",pro_note.pact_no);
	if ( ret )
	{
		sprintf(acErrMsg,"UPDATE ERROR pact_no=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H020");
		goto ErrExit;
	}

	/*************************
	*  1 借25102 
	*************************/
	set_zd_data("1021",pro_note.bail_ac_no);
	set_zd_int ("1022",1);
	set_zd_data("102Y","借 保证金帐户");
	set_zd_data("102J","01");
	set_zd_data("102K","2");					/*转帐*/
	set_zd_data("102O","0");					/*是否打印存折(有折无折标志):1是0否*/
	set_zd_double("102F",pro_note.bail_amt);
	if ( pub_base_my_func("D003","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"借 保证金帐户D003错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG

	/*************************
	*  2 借申请人201 
	*************************/
	if ( flag == 2 || flag == 3 )
	{
		set_zd_data("1021",pro_note.pro_ac_no);
		set_zd_data("102Y","借 申请人帐户");
		set_zd_double("102F",tx_amt1);
		if ( pub_base_my_func("D003","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"借 申请人帐户D003错![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/***********************
	*  3 借垫款13601 
	************************/
	if ( flag == 3 )
	{
		set_zd_data("1201","13601");
		set_zd_data("120A","借 垫款");
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",tx_amt2);
		if ( pub_base_my_func("A016","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"借 垫款A016错![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/**************************
	*  4 贷其他应受应付款26203 
	***************************/
	sprintf(acErrMsg,"QTYS ac_no1=[%s]",ys_ac_no);
	WRITEMSG
	set_zd_data("1211","26203");
	set_zd_data("121A","贷 其他应受应付");
	set_zd_data("1214","01");
	set_zd_data("1215","2");
    set_zd_double("1218",tx_amt3);
	if ( pub_base_my_func("A017","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"贷 其他应受应付A017错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/**************************
	*  5 借其他应受应付款26203 
	***************************/
	set_zd_data("1201","26203");
	set_zd_data("120A","借 其他应受应付");
	set_zd_data("1204","01");
	set_zd_data("1205","2");
    set_zd_double("1208",tx_amt3);
	if ( pub_base_my_func("A016","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"借 其他应受应付错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	/************************
	*  6 贷申请人帐户201
	*************************/
	if ( flag == 1 && tx_amt4 !=0.00 )
	{
		set_zd_data("1011",pro_note.pro_ac_no);
		set_zd_int ("1012",1);
		set_zd_data("1016","贷 申请人帐户");
		set_zd_data("101A","01");
		set_zd_data("101B","2");                /*转帐*/
		set_zd_double("1013",tx_amt4);
		if ( pub_base_my_func("D099","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"贷 申请人帐户错![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	/*****************************
	*  7 贷受益人帐户201 赔偿金额
	******************************/
	set_zd_data("1011",pro_note.ben_ac_no);
	set_zd_int ("1012",1);
	set_zd_data("1016","贷 受益人帐户");
	set_zd_data("101A","01");
	set_zd_data("101B","2");                /*转帐*/
	set_zd_double("1013",pro_note.amends_amt);
	if ( pub_base_my_func("D099","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"贷 受益人帐户错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}


	if ( flag == 3 )
	{
		/*****************************
		*  8 借逾期贷款--担保申请人13001
		******************************/
		if ( yq_chk(tx_amt3,pro_note,tx_amt2))
		{
			sprintf(acErrMsg,"借逾期贷款--担保申请人![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*****************************
		*  9 贷担保垫付13601
		******************************/
		set_zd_data("1211","13601");
		set_zd_data("121A","贷担保垫付");
		set_zd_data("1214","01");
		set_zd_data("1215","2");
   		set_zd_double("1218",tx_amt2);
		if ( pub_base_my_func("A017","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"担保垫付A016错![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	set_zd_double("1208",pro_note.amt);
	set_zd_data("1201","618");			
	set_zd_data("120A","担保表外注销");
	set_zd_data("1204","01");			
	set_zd_data("1205","2");			
	if ( pub_base_my_func("A016","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"借 担保表外注销错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"保函赔偿处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"保函赔偿ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

int yq_chk(double amt_lmt,struct protect_note_c pro_note,double tx_amt )
{
	/* 第二部分：银行垫款同时开逾期贷款户 */
	double	basic_rate=0.00;
	char	cCifNo[9];
	char	cAcNo[25];
	int		ret;

vtcp_log("0  OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);
	memset( cCifNo, 0x00, sizeof( cCifNo ) );
vtcp_log(" OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);

	/* 取承兑逾期贷款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	ret = pub_base_GetParm_Str( "BHYQ", 1, g_pub_tx.prdt_code );
	if( ret )
	{
		sprintf( acErrMsg, "取承兑垫款转逾期贷款产品编号错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B265" );
		goto ErrExit;
	}
			
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
	if(ret)	
	{
		sprintf(acErrMsg,"取贷款参数信息失败!");
		WRITEMSG
		goto ErrExit;	
	}
			
	/* 截取出票人账号的前八位，客户号 */
	strncpy( cCifNo, pro_note.pro_ac_no, 8 );
	cCifNo[8] = '\0';
	g_pub_tx.cif_no = atol( cCifNo );

sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
	g_ln_mst.opn_date = g_pub_tx.tx_date;			/* 开户日期 */		
	strcpy( g_ln_mst.pact_no, pro_note.pact_no );	/* 合同号 */	
	g_ln_mst.amt_lmt = tx_amt;				/* 合同总额  */
vtcp_log(" OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);
	g_ln_mst.over_rate = pro_note.over_rate;		/* 逾期利率:录入时输入的罚息利率 */
	g_ln_mst.fine_rate = pro_note.over_rate;		/* 罚息利率:录入时输入的罚息利率 */
	strcpy( g_pub_tx.cur_no, "01");				/* 币种 */
	strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );	/* 贷款支付方式 */	
	g_ln_mst.mtr_date = g_pub_tx.tx_date;			/* 到期日期:逾期当天日期 */
	strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );	/* 计息类型 */	
	strcpy( g_pub_tx.name, pro_note.pro_name );	/* 贷款户名:承兑汇票出票人户名 */		
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
			
	ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
		WRITEMSG
		return -1;	
	}
	basic_rate = basic_rate/1.2;/* 转换成月利率 */
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	/* 利率浮动比例计算 */
	g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
	g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);
		
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );		/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy( g_pub_tx.brf,"贷款开户开介质" );		
	
	TRACE
sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
	g_pub_tx.ac_id =0 ;
	/***生成账号***/
	if( pub_acct_opn_cif_mdm() )
	{
		sprintf(acErrMsg,"生成账号错误!");
		WRITEMSG
		goto ErrExit;
	}
sprintf(acErrMsg,"贷款账号[%s]",g_pub_tx.ac_no);
WRITEMSG			
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
			
	strcpy(cAcNo, g_pub_tx.ac_no);			/* 登记承兑汇票基本信息登记簿用 */
	TRACE
	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );		/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"贷款开户" );
	TRACE
sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
sprintf( acErrMsg, "ac_id=[%ld]", g_pub_tx.ac_id );
WRITEMSG
	g_pub_tx.tx_amt1 = 0.00;
	/***生成账户ID***/
	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"生成账号ID错误!");
		WRITEMSG
		goto ErrExit;
	}
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
	/* 第三部分：逾期贷款放款，借：13001逾期贷款 */
	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	g_pub_tx.tx_amt1 = g_ln_mst.amt_lmt;	/* 发生额 */
	strcpy( g_pub_tx.ct_ind, "2");						/* 现转标志 */
	sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
	WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3010 ;
	strcpy( g_pub_tx.brf,"保函逾期" );
	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	/*** 交易记帐处理 ***/
	strcpy( g_pub_tx.sub_tx_code, "L104" );
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	
	/* 登记贷款登记薄 */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_ln_mst.bal,0.00,"0",0 );	
	if( ret )
	{
	    sprintf(acErrMsg,"登记贷款登记薄失败");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L082");
		goto ErrExit;
	}						
	sprintf(acErrMsg,"登记贷款登记薄 PASS!");
	WRITEMSG
			
	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值，记会计账时在dc_entry中用到 */
			
	/* 借130－会计记帐 */
	set_zd_data( "0210", "01" );    
	set_zd_double( "0430", g_pub_tx.tx_amt1 );
	strcpy( g_pub_tx.sub_tx_code, "L104" );
	sprintf(acErrMsg,"__zyzyzy__sub_tx_code=[%s]",g_pub_tx.sub_tx_code);
	WRITEMSG
	ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
	if (ret != 0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
	return 0;
ErrExit:
	return 1;
}
