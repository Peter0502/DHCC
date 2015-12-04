/***************************************************************
* 文 件 名:     spJ005.c
* 功能描述：    银行汇票信息录入			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-17 13:01
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"


#define LIMIT_MONEY 100.00


/*****将从前台得到的值放到我自己的数据结构中来*************/
int iGet_br_name(char *or_br_no, char *br_name);

spJ005()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	double money = 0.0;
	char cDeal_mode[2];/***转出类型***/
	HV_FD123_AREA fd123;
	int    iExit = 0 ;	/**记录是否存在，1-存在  0－不存在****/
	long ori_ac_id;
	
	vtcp_log("+++++++++[%s][%d]++++++++++\n",__FILE__,__LINE__);
	vtcp_log("+++++++++[%s][%d]FUCK!!!!\n",__FILE__,__LINE__);
	ori_ac_id=g_pub_tx.ac_id;
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	vtcp_log("+++++++++[%s][%d]shit!!!!\n",__FILE__,__LINE__);
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&fd123,     0x00 ,  sizeof(fd123));

	/***********************
	 *PART 1 从前台得到数据*
	 ***********************/
	vtcp_log("+++++++++[%s][%d]+++++++\n",__FILE__,__LINE__);
	/**得到大额支付业务8583数据***/
	/**由于其中涉及到了帐号方面字段采用payin结构体****/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"从前台获取数据失败!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG	
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/**********************
        *PART 2 基本判断      *
 	**********************/
	 /**1.输入的金额不能为0 或者某个限度******/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&money);
	if(money<LIMIT_MONEY)
	{
		sprintf(acErrMsg,"[%s][%d]输入金额不能少于[%lf]!\n"
				,__FILE__,__LINE__,LIMIT_MONEY);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S042");
		goto ErrExit;
	}
	/****判断该是否登记过*****/
	/***由于payin/notpayin无法处理po_no,po_type字段,所以将需要再读取123域*/
	get_fd_data("1230",(char *)&fd123);
	get_zd_data("0720",cDeal_mode);

	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);
	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);
	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);

	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));	
	/**票据号码**/
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_payin.F_pay_opn_br_no);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	
	ret = Hv_poinfo_Dec_Sel(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",			hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"操作数据库错误!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
	if(ret!=100&&ret)
	{
		sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/********录入状态取消的汇票依然可以再来录入
	else if(ret!=100)
	{
		sprintf(acErrMsg,"该记录已经存在!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	**********************************/
	if(ret==100 &&!ret)
		iExit = 0;
	else if(ret==0)
		iExit = 1;
		
     /************************
     *PART 3 准备写数据表   *
     ************************/	

	/****记录不存在,开始写汇票信息表****/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&hv_poinfo.po_bal);	/***出票金额**/
	MEMCPY_DEBUG(hv_poinfo.br_no,g_pub_tx.tx_br_no,sizeof(hv_poinfo.br_no)-1);
	/***签票机构号码**/
	hv_poinfo.bank_type[0] = '0';
        /**本行汇票**/
	MEMCPY_DEBUG(hv_poinfo.or_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.or_br_no)-1);/***发起行行号**/
	iGet_full_hv_no(hv_payin.F_pay_opn_br_no,"",hv_poinfo.pay_qs_no);
	vtcp_log("[%s][%d]hv_poinfo.pay_qs_no=[%s]\n",__FILE__,__LINE__,hv_poinfo.pay_qs_no);
	/***付款清算行号**/
	vtcp_log("[%s][%d]wt_date=[%s]\n",__FILE__,__LINE__,hv_payin.F_wt_date);
	hv_poinfo.l_tx_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/***货币种类***/
	MEMCPY_DEBUG(hv_poinfo.cur_id,"RMB",sizeof(hv_poinfo.cur_id)-1);
	/***上次交易日期,放当前日期***/
	hv_poinfo.td_cnt    = 1;
	/***当日交易笔数*/
	hv_poinfo.lost_cnt  = 0;
	/***挂失次数***/
	MEMCPY_DEBUG(hv_poinfo.pay_ac_no,hv_payin.F_pay_ac_no,sizeof(hv_poinfo.pay_ac_no)-1);	/***付款人帐号**/
	MEMCPY_DEBUG(hv_poinfo.pay_name,hv_payin.F_pay_name,sizeof(hv_poinfo.pay_name)-1);	/***付款人姓名**/
	MEMCPY_DEBUG(hv_poinfo.pay_addr,hv_payin.F_pay_addr,sizeof(hv_poinfo.pay_addr)-1);   /***付款人地址***/
	MEMCPY_DEBUG(hv_poinfo.cash_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.cash_ac_no)-1);/***收款人帐号**/
	MEMCPY_DEBUG(hv_poinfo.cash_name,hv_payin.F_cash_name,sizeof(hv_poinfo.cash_name)-1);/***收款人姓名**/
	MEMCPY_DEBUG(hv_poinfo.hold_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);/**当前持票人帐号**/
	MEMCPY_DEBUG(hv_poinfo.hold_name,hv_payin.F_cash_name,sizeof(hv_poinfo.hold_name)-1);  /**当前持票人姓名**/
	hv_poinfo.reg_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/***登记日期***/
	hv_poinfo.po_ind[0]       = fd123.po_type[0];
	/***汇票类型***/
	hv_poinfo.deal_mode[0]    =cDeal_mode[0];
	/***加核押标志***/
	hv_poinfo.add_pwd_ind[0] = '0';
	/***转帐现金处理方式***/
	MEMCPY_DEBUG(hv_poinfo.cash_br_no,hv_payin.F_cash_opn_br_no,sizeof(hv_poinfo.cash_br_no)-1);/***兑付行行号**/
	MEMCPY_DEBUG(hv_poinfo.op_tel,hv_payin.F_tel,sizeof(hv_poinfo.op_tel)-1);
	/*** 开户操作员**/
	MEMCPY_DEBUG(hv_poinfo.brf,   hv_payin.F_hv_brf,sizeof(hv_poinfo.brf)-1);
	/***附言***/


	vtcp_log("[%s][%d]开始插入数据表!\n",__FILE__,__LINE__);
	if(!iExit)/***如果是新录入的汇票需要插入记录***/
	{
			vtcp_log("[%s][%d]记录是不存在的!\n",__FILE__,__LINE__);
			hv_poinfo.po_sts[0] = HV_HP_REG;
			/***上次帐户状态***/
			hv_poinfo.l_po_sts[0] =  HV_HP_UNREG;/***设置为无效状态,便于取消时候用***/
			/***当前汇票的状态***/

			ret = Hv_poinfo_Ins(hv_poinfo,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"写汇票基本信息表错误ret=[%d]\n",ret);
				set_zd_data("0130",acErrMsg);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P009");
				goto ErrExit;
			}
	}
	else if(iExit==1)/***说明了汇票是存在的***/
	{
			vtcp_log("[%s][%d]记录是存在的!\n",__FILE__,__LINE__);
			if((hv_poinfo.l_po_sts[0]==HV_HP_REG || hv_poinfo.l_po_sts[0]==HV_HP_SIGN)  &&hv_poinfo.po_sts[0]==HV_HP_UNREG)
			{
					hv_poinfo.l_po_sts[0] = HV_HP_UNREG;
					/***上次帐户状态***/
					hv_poinfo.po_sts[0] =HV_HP_REG;
					/***当前汇票的状态***/
					ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
					if(ret)
					{
						vtcp_log("[%s][%d]登记取消状态的汇票重新登记错误!\n",__FILE__,__LINE__);
						sprintf(acErrMsg,"登记取消状态的汇票重新登记错误",50);
						set_zd_data("0130",acErrMsg);
						strcpy(g_pub_tx.reply,"P009");
						goto ErrExit;	
					}
			}	
			else
			{
					vtcp_log("[%s][%d]该状态的汇票不是登记取消，无法重新登记!\n",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P001");
					sprintf(acErrMsg,"该状态的汇票不是登记取消，无法重新登记",50);
					set_zd_data("0130",acErrMsg);
					goto ErrExit;
			}
	}
	/**********************
     *PART 4 写流水及返回 *
     **********************/ 
	/* 写入业务流水 */
	vtcp_log("+++++++++[%s][%d]++++++++++\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/* 增加对业务委托凭证的处理  added by liuxuan 2007-3-31*/
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_int("1022",&g_pub_tx.ac_seqn);
	g_pub_tx.ac_id=ori_ac_id;
	vtcp_log("开始凭证处理 ac_no[%s] ac_id[%ld] type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);

	/******** 这里换成马丁同志在001里写的函数  20090910 chenhw 
	if(pub_sub_note())
	{
		sprintf(acErrMsg,"处理凭证出错!");
		WRITEMSG
		goto ErrExit;
	}**/

	vtcp_log("开始凭证处理 type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	if ((memcmp(g_pub_tx.note_type,"005",3)==0) )
	{
		ret=J005_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
		if(ret)
		{
			sprintf(acErrMsg,"处理凭证出错!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add by gong 2011/9/30 15:22:03 晋中商行，因信贷系统修改 */
	char m_draw_no[25];
	memset(m_draw_no,'\0',sizeof(m_draw_no));
	WRITEMSG
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	vtcp_log("[%s][%d],draw_no=[%s]",__FILE__,__LINE__,m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='YH%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_date,hv_poinfo.po_no,m_draw_no);/*关联交易序号*/
		if( ret < 0 )
		{
				sprintf( acErrMsg, "更改draw_loan_hst表出错!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
		}
	}

	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int J005_com_NoteUse(
 long    ac_id,
 int     ac_seqn,
 char    *note_type,
 char    *beg_no,
 char    *end_no,
 char    *tel)
{
	struct note_parm_c sNoteParm;

	int     ret;

	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );

	/* 取凭证参数表:根据凭证类型取凭证结构 */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
		 sprintf(acErrMsg,"取凭证参数note_parm表错!note_type=[%s]",note_type);
		 WRITEMSG
		 strcpy( g_pub_tx.reply, "N001" );
		 return (-1);
	 }else if ( ret )
	 {
		 sprintf( acErrMsg, "执行Note_parm_Sel错误![%d]", ret );
		 WRITEMSG
		  return(-1);
	}

        /* 判断是否管理，如果不管理直接返回正确 */
        if ( sNoteParm.ctl_no_ind[0] == 'N' )
        {
                return(0);      /* 不管理号码，直接返回正确0 */
        }
        sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
        WRITEMSG
        sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
        WRITEMSG

        /* 判断是否允许卖给客户 */
        if ( sNoteParm.sell_ind[0] == 'N' )     /* 不允许出售给客户 */
        {
                sprintf( acErrMsg, "凭证不能出售给客户" );
                        WRITEMSG
                        return(-1);

        }else if ( sNoteParm.sell_ind[0] == 'Y' )
        {
                ret = J005_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
                if ( ret )
                {
                        sprintf( acErrMsg, "销客户凭证错" );
                        WRITEMSG
                        return(-1);
                }
        }else
        {
                sprintf( acErrMsg, "凭证种类出售标志错，sell_ind=[%s]",sNoteParm.sell_ind );
                WRITEMSG
                return(-1);
        }

	return(0);
}


int J005_com_CheqClsNo(long ac_id, int ac_seqn, char *note_type, char *cheq_beg_no, char *cheq_end_no)
{
        struct  note_cheq_mst_c sCheqMst;       /* 记录客户凭证台帐 */
        struct  note_mst_c      sNoteMst;
        char    sts[51];        /* 支票状态 */
        long    tx_date;        /* 交易日期 */
        char    tw_br_no[6];/* 对方机构号 */
        char    tw_tel[6];      /* 对方柜员号 */
        int     ret;

        MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
        MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );

        /* 检查起始号码、终止号码的连续性,冠子头是否相同 */
        ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
        if ( ret == -1 )
        {
                sprintf( acErrMsg, "检查连续性,冠子头失败!ret=[%d]",ret );
                WRITEMSG
                return(-1);
        }
        /*******************************/

        /* 根据帐户ID检查支票是否为该帐号所有 */
        ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
        if( ret == -1 )
        {
                sprintf( acErrMsg, "客户无此凭证或凭证状态不一致!ac_id=[%d],\
                         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
                         ac_id,note_type,cheq_beg_no, cheq_end_no );
                WRITEMSG
                return(-1);
        }

        if ( sts[0] == '0' )
        {
                sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N080" );
                return(-1);
        }else if ( sts[0] == '2' )
        {
                sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N081" );
                return(-1);
        }else if ( sts[0] == '3' )
        {
                sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N082" );
                return(-1);
        }else if ( sts[0] == '4' )
        {
                sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N083" );
                return(-1);
        }else if ( sts[0] == '5' )
        {
                sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N084" );
                return(-1);
        }

        /*****************************/
	  /* 将客户凭证台帐中该支票号码标志置为'5'（使用状态） */
 strcpy( sNoteMst.note_type, note_type );
 strcpy( sNoteMst.beg_note_no, cheq_beg_no );
 strcpy( sNoteMst.end_note_no, cheq_end_no );
 tx_date=g_pub_tx.tx_date; /*JYW JINZHONG20070421*/

  ret=pub_com_RegCheq( sNoteMst, ac_id, ac_seqn, tx_date, "G" );
 if( ret == -1 )
 {
                sprintf( acErrMsg, "修改支票状态位错误!ret=[%d]",ret );
                WRITEMSG
                return(-1);
        }


        /* 计算凭证数量 */
        ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
        if ( ret == -1 )
        {
                sprintf( acErrMsg, "计算凭证数量错，ret = [%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N010" );
                return(-1);
       }
        sCheqMst.cnt = ret ;
        sCheqMst.ac_id = g_pub_tx.ac_id;
        sCheqMst.ac_seq = g_pub_tx.ac_seqn;
        strcpy( sCheqMst.note_type, note_type );
        strcpy( sCheqMst.beg_note_no, cheq_beg_no );
        strcpy( sCheqMst.end_note_no ,cheq_end_no );

        /* 登记客户凭证台帐明细 C表示客户使用凭证 */
        strcpy( tw_br_no , "" );
        strcpy( tw_tel, "" );

        ret=pub_com_RegCheqHst( "C", sCheqMst, tw_br_no, tw_tel );
        if( ret == -1 )
        {
                sprintf( acErrMsg, "登记客户凭证台帐明细失败!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N021" );
                return(-1);
        }


        return 0;
}
