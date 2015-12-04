/*************************************************
* 文 件 名:  spB564.c
*	撤票通知
*	票据承兑后，提示收票前，如果出票人请求撤票并撤票成功，
*	作为承兑行会收到本通知，收到本通知后将进行账务处理。
*
* 作	者:  chenchao
* 完成日期： 2010-4-6 9:37:33
*
* 修改记录：
* 日	期:
* 修 改 人:
* 修改内容:
insert into tx_def values ('4964','撤票通知','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null);
insert into tx_flow_def  values ('4964',0,'B564','#=0001@1@#$');
insert into tx_flow_def  values ('4964',1,'D003','#=0000@2@#$');
insert into tx_flow_def  values ('4964',2,'D099','#$');
insert into tx_sub_def values ('B564','撤票通知','spB564','0','0');

insert into tx_sub_rel (tx_code, in_sub_tx, out_ind, out_data, in_data, memo) values ('4964', 'D003', '1', '1', '0191', null);

insert into dc_entry  values ('B56400', 1, '0210', '52107', '1', '1', '1', '0660', '0440', '利息支出');

insert into tx_dc_rel (sub_tx_code, dc_code, entry_code) values ('B564', '0000', 'B56400');

*************************************************/
#define MYSQLERR if( ret ) \
{\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit11;\
}
#define ERR_DEAL if( ret ) \
{\
	sprintf( acErrMsg, "sql error" ); \
	WRITEMSG \
	goto ErrExit; \
}
#define EXTERN
#include "public.h" 
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "hv_uniontab_c.h"
#include "ecds_recv_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "intst_hst_c.h"
#include "com_sys_parm_c.h"
#include "com_sect_acm_c.h"
#include "com_item_c.h"
#include "mo_bail_rate_c.h"

struct mo_bank_acc_po_c	sMo_bank_acc_po;
int 
spB564()
{ 
	struct ecds_recv_reg_c	sEcds_recv_reg,sEcdsrecvreg;	
	struct mo_bank_po_c	sMo_bank_po;
	struct com_parm_c	sCom_parm;
	struct mdm_ac_rel_c	sMdm_ac_rel;

	/*接受的报文内容*/
	long	Dt=0;				/*日期*/
	char	tranSeqNb[28+1];		/*交易流水号*/
	char	drftCls[1+1] ;			/*票据分类*/
	char	drftTyp[4+1] ;			/*票据类型*/
	char	drftNb[30+1];			/*票据号*/
	long	isseDt=0 ;			/*出票日*/
	long	dueDt=0 ;			/*到期日*/
	double	drftAmt=0.00;			/*票面金额*/
	char	bailAcct[32+1];		 	/*保证金账号*/
	double	bailAmt=0.00;			/*保证金*/
	/*end*/
	
	char		cTmpBuff[101];
	char		cAcc_hrt[9];
	long		l_date=0;
	int		 ttlnum = 0;	/**读取的总条数**/
	char		wherelist[1024];
	int		cnt=0;
	int		 i = 0;
	double		intst_amt=0.00;
	int         ret = 0,pub_ret=0,ins_flag=0;
	vtcp_log("%s.%d", __FILE__,__LINE__);  
	memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
	memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c)); 
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));

	memset(tranSeqNb , 0x00,sizeof( tranSeqNb));	 /*交易流水号*/ 
	memset(drftCls   , 0x00,sizeof( drftCls  ));	 /*票据分类  */ 
	memset(drftTyp   , 0x00,sizeof( drftTyp  ));	 /*票据类型  */ 
	memset(drftNb	, 0x00,sizeof( drftNb   ));	 /*票据号	*/ 
	memset(bailAcct  , 0x00,sizeof( bailAcct ));	 /*保证金账号*/ 

	vtcp_log("%s.%d", __FILE__,__LINE__);
	pub_base_sysinit();

	get_zd_long("0440",  &Dt	);	/*日期*/
	get_zd_long("0460",  &isseDt	);	/*出票日*/  
	get_zd_long("0450",  &dueDt	 );	/*到期日*/  
	get_zd_double("0390",  &drftAmt   );	  /*票面金额  */
	get_zd_double("0420",  &bailAmt   );	  /*保证金	*/	  
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("101F",  cTmpBuff );	/*交易流水号*/ 
	zip_space(cTmpBuff);
	memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
	
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0670",  drftCls   );	/*票据分类*/
	zip_space(cTmpBuff);
	memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0910",  cTmpBuff   );	/*票据类型*/
	zip_space(cTmpBuff);
	memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
	
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("1032",  cTmpBuff	);	/*票据号*/
	zip_space(cTmpBuff);
	memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0820",  cTmpBuff  );	/*保证金账号*/
	zip_space(cTmpBuff);
	memcpy(bailAcct , cTmpBuff , sizeof(bailAcct)-1);  
	  
	/*ret=sysinit();
	if (ret<0)
	{
		vtcp_log("系统初始化错误[%s][%d]\n",__FILE__,__LINE__);
		vtcp_log("rc=[%d]",ret);
		goto ErrExit;
	}*/
	vtcp_log("[%s][%d]日期=[%d]",__FILE__,__LINE__,Dt);
	vtcp_log("[%s][%d]交易流水号=[%s]",__FILE__,__LINE__,tranSeqNb);
	vtcp_log("[%s][%d]票据分类=[%s]",__FILE__,__LINE__,drftCls);
	vtcp_log("[%s][%d]票据类型=[%s]",__FILE__,__LINE__,drftTyp);
	vtcp_log("[%s][%d]票据号=[%s]",__FILE__,__LINE__,drftNb);
	vtcp_log("[%s][%d]出票日=[%d]",__FILE__,__LINE__,isseDt);
	vtcp_log("[%s][%d]到期日=[%d]",__FILE__,__LINE__,dueDt);
	vtcp_log("[%s][%d]票面金额=[%lf]",__FILE__,__LINE__,drftAmt);
	vtcp_log("[%s][%d]保证金账号=[%s]",__FILE__,__LINE__,bailAcct);
	vtcp_log("[%s][%d]保证金=[%lf]",__FILE__,__LINE__,bailAmt);

	vtcp_log("%s.%d   -------drftNb=%s  ...drftTyp=%s  ", __FILE__,__LINE__,drftNb,drftTyp );
	
	ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",Dt , tranSeqNb);
	if(ret == 0)
	{
		if(sEcdsrecvreg.bf_flag[0] == '0' || sEcdsrecvreg.bf_flag[0] == '2')
		{
			sprintf(acErrMsg,"该笔票据已经处理!! sEcdsrecvreg.bf_flag=[%s]",sEcdsrecvreg.bf_flag);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "NXDP" );
			goto ErrExit;
		}
		else if(sEcdsrecvreg.bf_flag[0] == '1')
		{
			pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",Dt , tranSeqNb);
			if(pub_ret)
			{
				sprintf(acErrMsg, "update ecds_recv_reg error ,pub_ret [%d]", pub_ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}   
			ins_flag=1;
			
		}
		else
		{
			sprintf(acErrMsg,"补发标志错误,检查[%s]",sEcdsrecvreg.bf_flag);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D999" );
			goto ErrExit;
		}
		}
	else if (ret && ret !=100)
	{
        	sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT03" );
            goto ErrExit;
	}
	ret = Mo_bank_po_Sel( g_pub_tx.reply,&sMo_bank_po,"po_no = '%s' ",drftNb);  /*有个编号问题没有确定*/
	if (ret)
	{
		sprintf(acErrMsg, "查询mo_bank_po错!!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B012");
		goto ErrExit;
	}
	if( sMo_bank_po.po_sts[0] == '3' )
       {
               sprintf( acErrMsg, "此承兑汇票已做退票!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B212" );
               goto ErrExit;
       }else if( sMo_bank_po.po_sts[0] == '2' )
       {
               sprintf( acErrMsg, "此承兑汇票已核销，不允许退票!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B228" );
               goto ErrExit;
       }else if( sMo_bank_po.po_sts[0] == '4' )
       {
               sprintf( acErrMsg, "此承兑汇票已托收，不允许退票!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B229" );
               goto ErrExit;
       }
       if(sMo_bank_po.po_ind[0]=='0')
       {
               sprintf( acErrMsg, "此承兑汇票已挂失，不允许退票!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B031" );
               goto ErrExit;
       }
	sprintf(wherelist, " pact_no='%s' " ,sMo_bank_po.pact_no );  

	vtcp_log("WHERE [%s]", wherelist);

	ret = Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply, wherelist);
	ERR_DEAL
	ttlnum = 0;

	while (1) 
	{
		ret = Mo_bank_acc_po_Fet_Upd(&sMo_bank_acc_po, g_pub_tx.reply);
		if (ret == 100 )
			break;
		vtcp_log("%s.%d", __FILE__,__LINE__);	
		if (ret)
		{
			sprintf(acErrMsg, "查询mo_bank_acc_po错!!ret=[%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "AT06");
			goto ErrExit;
		}	
		ttlnum++;
		/*****add by ligl 20100508 保证金账号从协议表里找****/
		memcpy(bailAcct,sMo_bank_acc_po.bail_ac_no,sizeof(sMo_bank_acc_po.bail_ac_no)-1);
		pub_base_strpack(bailAcct);
		if(sMo_bank_acc_po.acc_type[0]=='3')
		{
			bailAmt=sMo_bank_po.po_amt;
		}
		else if(sMo_bank_acc_po.acc_type[0]=='1')
		{
			bailAmt=sMo_bank_po.po_amt*sMo_bank_acc_po.bail_amt_ratio/100;
		}
		else if(sMo_bank_acc_po.acc_type[0]=='2')
		{
			bailAmt=0.00;	
		}
		/*****/
		sMo_bank_acc_po.bail_amt=sMo_bank_acc_po.bail_amt-bailAmt;
		strcpy(sMo_bank_acc_po.acc_po_ind, "3");
		ret = Mo_bank_acc_po_Upd_Upd(sMo_bank_acc_po, g_pub_tx.reply);
		if (ret) 
		{
			sprintf(acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		
	}
	ret= Mo_bank_acc_po_Clo_Upd();
	ERR_DEAL

	if (!ttlnum) 
	{
		strcpy(g_pub_tx.reply, "S049");
		goto ErrExit;
	} 
	vtcp_log("%s.%d---- ", __FILE__,__LINE__);
	
	if(ins_flag == 0) 
	{
		sEcds_recv_reg.ecds_tx_date		=Dt;
		memcpy(sEcds_recv_reg.ecds_trace_no,	tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
		sEcds_recv_reg.trace_no 		= 0;
		sEcds_recv_reg.tx_date  		= g_pub_tx.tx_date;	
		memcpy(sEcds_recv_reg.drftcls,		drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);	 
		memcpy(sEcds_recv_reg.drfttyp ,		drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);	 
		memcpy(sEcds_recv_reg.drftnb ,		drftNb,	sizeof(sEcds_recv_reg.drftnb)-1);   
		sEcds_recv_reg.issedt 			= Dt ;	 					
		sEcds_recv_reg.duedt	 		= dueDt ;  
		memcpy(sEcds_recv_reg.tag_name,		"CBS1000160",sizeof( sEcds_recv_reg.tag_name)-1);	  /*报文类型*/
		sEcds_recv_reg.sts[0]='1';
		sEcds_recv_reg.check_flag[0]='1';
		Ecds_recv_reg_Debug(&sEcds_recv_reg);
		ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
		if (ret )
		{
			sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "NXC8");
			goto ErrExit;
		}
	}
	ret = sql_execute("update mo_bank_po set po_sts='3' where po_no = '%s' ",drftNb ); /*置成退票状态*/
	if (ret)
	{
		sprintf(acErrMsg, "update mo_bank_po出错!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B017");
		goto ErrExit;
	}
	
	ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",\
	PARM_ECDS_TR_PAY_AC,sMo_bank_acc_po.payer_br_no);
	if (ret)
	{
		sprintf(acErrMsg,"查询Com_parm表错误!!ret  [%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "AT06" );
		goto ErrExit;
	}
 
	/*计息函数*/
	set_zd_data("1021",bailAcct);
	ret = pubf_intst(&intst_amt);
	if (ret)
	{
		sprintf(acErrMsg, "结息失败!!!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G010");
		goto ErrExit;
	} 
	vtcp_log("[%s][%d]bailAcct=%s",__FILE__,__LINE__,bailAcct);
	/*************************************利息记账部分******************************/
	memcpy(g_pub_tx.ac_get_ind,"000",3);
	g_pub_tx.tx_amt1=intst_amt;
	strcpy(g_pub_tx.ac_wrk_ind, "000000001");
	memcpy(g_pub_tx.ac_no, "52207",sizeof(g_pub_tx.ac_no));/* 账户 */
	strcpy(g_pub_tx.intst_type, "1");/* 计息类型 */
	strcpy(g_pub_tx.add_ind, "0");  /* 增减:0减1加2销户 */
	strcpy(g_pub_tx.prt_ind, "0");  /* 是否打印存折(有折无折:1是0否 */
	strcpy(g_pub_tx.hst_ind, "1");  /* 入明细:1日间入2日终单 日终汇总入 */
	strcpy(g_pub_tx.brf, "保证金利息");
	sprintf(acErrMsg, "Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 交易记帐处理 ***/
	if (pub_acct_trance())
	{
		sprintf(acErrMsg, "交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "交易记帐处理 PASS!");
	WRITEMSG
	set_zd_data("0210", "01");
	set_zd_double("0440",intst_amt);
	strcpy(g_pub_tx.sub_tx_code, "B564");
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (ret != 0)
	{
		vtcp_log("会计记帐不成功[%s][%ld]",__FILE__,__LINE__);
		sprintf(acErrMsg, "会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
	
	/***********************************为D099准备着*******保证金+利息******************/
	set_zd_data("1011", sMo_bank_acc_po.payer_ac_no);
	/*set_zd_int("1012", &g_pub_tx.ac_seqn);*/
	set_zd_data("101A", "01");
	set_zd_data("101B", "2");
	set_zd_data("1016", "保证金");      /***摘要***/
	set_zd_data("0630","保证金");
	set_zd_double("1013", bailAmt+intst_amt);
	set_zd_data("1017", "0");
	set_zd_data("0192", "000000001");
	set_zd_int("0193", 1001);
	/*********************************为D003准备数据*********保证金****************/
	set_zd_data("1021",bailAcct);
	set_zd_double("102F",bailAmt);
	set_zd_data("102J","01");
	set_zd_data("102K","2");
	set_zd_data("102Y","保证金");
	set_zd_data("102O","0");			/*是否打折标志*/
	set_zd_data("0810","保证金");
	set_zd_data("0192","000000001");
	set_zd_int("0193",1001);

GoodExit:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
GoodExit1:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"B564",4);
	}
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

int pubf_intst( double *HQdealval,char *cAcc_hrt)
{
	long		HQday=0;
	int		ret =0;
	double		HQ_rate=0.00;
	struct		mo_bail_rate_c		sMoBailRate1,sMoBailRate;
	memset(&sMoBailRate1,0,sizeof(struct mo_bail_rate_c));
	memset(&sMoBailRate,0,sizeof(struct mo_bail_rate_c));
	memset(cAcc_hrt,0,sizeof(cAcc_hrt));
	
	ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply, " bail_type='1' and pact_no='%s' and sts='1' ",sMo_bank_acc_po.pact_no);
	if (ret) {
		sprintf(acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d]sMo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	ret = Mo_bail_rate_Fet_Upd(&sMoBailRate, g_pub_tx.reply);
	if (ret && ret!=100) {
		sprintf(acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d]sMo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	else if(ret==100)/**没有进行利率维护***/
	{
		*HQdealval = 0.00;
		Mo_bail_rate_Clo_Upd();
		return 0;
	}
	else
	{
	memcpy(&sMoBailRate1, &sMoBailRate, sizeof(struct mo_bail_rate_c));

	pub_base_strpack(sMoBailRate.ac_no);
	
	ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date, &HQ_rate);
	if (ret) {
		sprintf(acErrMsg, "取利率错误!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O195");
		return ret;
	}
	
	/** 计算天数 **/
	if (pub_base_CalDays(sMoBailRate.beg_date, g_pub_tx.tx_date, "0",&HQday)) {
		sprintf(acErrMsg, "计算天数错误!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O108");
		return ret;
	}
	vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
	 
	sprintf(acErrMsg, "HQday=[%ld]HQ_rate=[%lf],ic_rate=[%lf]", HQday, HQ_rate,sMoBailRate.ic_rate);
	WRITEMSG
	/** 计算利息 **/
	
	*HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
	vtcp_log("[%s][%d] HQdealval=[%f]",__FILE__,__LINE__,*HQdealval);
	*HQdealval = pub_base_PubDround_1(*HQdealval);
	if (pub_base_CompDblVal(HQdealval,0.00) < 0)
	{
		sprintf(acErrMsg, "计算利息错误错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D158");
		return ret;
	}

	/** 更新保证金计息金额 **/
	sMoBailRate.intst_date = g_pub_tx.tx_date;
	sMoBailRate.end_trace_no = g_pub_tx.trace_no;
	sMoBailRate.intst_amt = *HQdealval;
	
	/**更新保证金状态**/
	strcpy(sMoBailRate.sts, "*");
	ret = Mo_bail_rate_Upd_Upd(sMoBailRate, g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d]Mo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	Mo_bail_rate_Clo_Upd();
	}	
	vtcp_log("[%s][%d] 该保证金总共产生了[%f]利息",__FILE__,__LINE__,*HQdealval);
	
	/**全部利息转给原收息帐户**/
	
	/**----------------检查保证金利息需要从哪个科目取值---------------------------**/
	vtcp_log("[%s][%d] bail_ac_no =[%s]",__FILE__,__LINE__,sMoBailRate.bail_ac_no);
	return 0;
}

