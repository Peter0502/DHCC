/*************************************************
* 文 件 名:  spF070.c
* 功能描述： ATM和POS处理接口/平台帐务处理接口
*
* 作	 者:  rob
* 完成日期： 2003年2月17日
*
* 修改记录： 
*	日	期:
*	修 改 人:
*	修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card.h" /* add by LiuHuafeng 20070127 */
/*add by lifei for nx 2009-8-1 9:13:29 */
#include "march_info_c.h"
#include "march_mst_c.h"
#include "auth_modify_ln_rate_c.h"/**ADD BY ZHGF 20100618*/
#include "mdm_ac_rel_c.h"/**20100618*/
#include "com_parm_c.h"
#include "counter_tlog_c.h"

static char fd24[4]; /* 业务类型 *//*20100618**ADD BY ZHGF **/
int spF070()
{
	int ret=0;
	char flag;
	char crd_no[25],crd2_no[25];
	char cAcc_no[8];
	char cAcno[25];
	char brf10[11],tmp_brf[51],Tmp_str[101];
	char brf20[21];
	char dc_ind[2], ct_ind[2], cz_ind[2],fee_ind[2],ysq_ind[2],sYw_type[4];
	char acc_opnbr[6],acc_no[ACC_NO_LVL3_LEN+1];  /** 20090405 wyw 科目记账用到 **/
	long cx_ind=0; /*撤销标志*/
	char auth_no[8];
	double tx_amt=0.0;
	double tx_amt1=0.0;
	/*add by lifei for nx 2009-8-1 9:13:29 */
	double dFee_amt=0.00;
	long pt_date=0;
	long pt_trace_no=0;
	long cz_date=0;
	long cz_pt_trace_no=0;
	char ls_ac_no[25];
	struct march_info_c sMarch_info;
	struct march_mst_c sMarch_mst;
	memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
	memset(&sMarch_info,0x00,sizeof(sMarch_info));
	memset(ls_ac_no,0x00,sizeof(ls_ac_no));
	/*end by lifei for nx 2009-8-1 9:13:29 */

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct com_parm_c   sCom_parm;
	memset(fd24, 0x0, sizeof(fd24));
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(crd2_no, 0x0, sizeof(crd2_no));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(ysq_ind, 0x0, sizeof(ysq_ind));
	memset(auth_no, 0x0, sizeof(auth_no));
	memset(tmp_brf, 0x0, sizeof(tmp_brf));
	memset(brf10, 0x0, sizeof(brf10));
	memset(brf20, 0x0, sizeof(brf20));
	memset(sYw_type,0,sizeof(sYw_type));
	memset(Tmp_str,0,sizeof(Tmp_str));
	memset(cAcno, 0x0, sizeof(cAcno));
	memset(cAcc_no,0x0,sizeof(cAcc_no));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst,0,sizeof(struct dd_mst_c));
	memset(&sCom_parm,0,sizeof(struct com_parm_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0240",fd24);
	vtcp_log("%s,%d fd24=[%s]\n",__FILE__,__LINE__,fd24);

	/* add by lifei  for nx 2009-8-1 9:53:12*/
	get_zd_long("0520",&pt_trace_no);
	get_zd_long("0440",&pt_date);
	get_zd_double("0410",&dFee_amt);
	get_zd_long("0450",&cz_date);
	get_zd_long("0780",&cz_pt_trace_no);
	/* end by lifei  for nx 2009-8-1 9:53:12*/

	get_zd_data("0660",dc_ind); /*借贷标志*/ 
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	get_zd_data("0680",ct_ind); /*现转标志*/ 
	get_zd_data("0690",fee_ind); /*收费模式*/
	get_zd_data("0700",ysq_ind); /*预授权标志 */

	get_zd_data("0300", crd_no); /*借贷标志相对方*/
	get_zd_data("0300", g_pub_tx.crd_no); 
	get_zd_data("0310", crd2_no);

	get_zd_data("0790",g_pub_tx.draw_pwd);	/* 密码 */
	get_zd_double("0400",&tx_amt);/*交易金额*/
	get_zd_data("0810",tmp_brf);  /*摘要*/
	get_zd_long("0500",&cx_ind); /*撤销标志*/
	get_zd_data("0890",g_pub_tx.note_type);/*凭证种类*/
	get_zd_data("0590",g_pub_tx.beg_note_no);/*凭证号码*/
	get_zd_data("0240",sYw_type);
	vtcp_log("%s,%d,凭证种类[%s],凭证号码[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);
	memcpy(brf10,tmp_brf,10);
	memcpy(brf20,tmp_brf,20);
	strcpy(g_pub_tx.cur_no, "01");
	g_pub_tx.ct_ind[0]=ct_ind[0];
	set_zd_long("0460",g_pub_tx.tx_date);
	get_zd_data("0320",cAcno);/*若fee_ind为i,则32域需要传IC卡电子现金主账号*/
	vtcp_log("%s,%d,帐号为[%s]",__FILE__,__LINE__,cAcno);
	pub_base_old_acno(cAcno);
	/***20120306IC卡电子钱包账户转换成科目号 add by wzw***/
	if(memcmp(cAcno,ICCARDHEADCODE,6) == 0 && fee_ind[0]=='i')
	{
		ret=pub_base_GetParm_Str( "ICACC",1, cAcc_no );
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAcno);
    if (ret)
		{
			vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	  ret=Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='%s' and parm_seqn=11", sMdm_ac_rel.opn_br_no);
    if (ret)
		{
			vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		vtcp_log("%s,%d[%s]",__FILE__,__LINE__,sCom_parm.val);
		if(strlen(crd_no)<13 && memcmp(crd_no,cAcc_no,8)==0)
		{
			strcpy(crd_no, sCom_parm.val);
			strcpy(g_pub_tx.crd_no, sCom_parm.val );
		}
		if(strlen(crd2_no)<13 && memcmp(crd2_no,cAcc_no,8)==0)
		{
			strcpy(crd2_no, sCom_parm.val);
		}
		vtcp_log("%s,%d,crd_no=[%s],g_pub_tx.crd_no=[%s],crd2_no=[%s]",__FILE__,__LINE__,crd_no,g_pub_tx.crd_no,crd2_no);
  }

	vtcp_log("%s,%d,借贷[%s],现转[%s],收费模式[%s],卡号[%s],金额[%.2f]",__FILE__,__LINE__,dc_ind,ct_ind,fee_ind,crd_no,tx_amt);
	vtcp_log("%s,%d,卡号[%s],卡号2[%s]摘要[%s]",__FILE__,__LINE__,crd_no,crd2_no,tmp_brf);
	sprintf(acErrMsg,"交易类型= ysq_ind[%s] ,cz_ind[%s],cx_ind[%d],fee_ind=[%s]",ysq_ind,cz_ind,cx_ind,fee_ind);
	WRITEMSG
		pub_base_old_acno(g_pub_tx.crd_no);
	strcpy(crd_no,g_pub_tx.crd_no);
	pub_base_old_acno(crd2_no);
	/**add by ChenChao 20111010 gmt时,set 1023 1024**/
	char sTmpbuf[128];
	memset(sTmpbuf,0x00,sizeof(sTmpbuf));
	get_zd_data("0240",sTmpbuf);
	vtcp_log("[%s][%d],gmt=[%s]",__FILE__,__LINE__,sTmpbuf);
	if(strcmp(sTmpbuf, "gmt" )==0 && pub_base_CompDblVal(tx_amt,0) > 0 && cz_ind[0] != '1')
	{
		set_zd_data("1023",g_pub_tx.note_type); /*票据种类*/
		set_zd_data("1024",g_pub_tx.beg_note_no); /*票据号码*/
		/*add by wjl _20150605 添加对账记录交易流水等（柜面通对账用）**/
	  struct counter_tlog_c    counter_tlog_c;
	  memset(&counter_tlog_c,0x00,sizeof(struct counter_tlog_c));
	  get_zd_long("0440",&counter_tlog_c.platform_date);
	  counter_tlog_c.trace_no=g_pub_tx.trace_no;
	  get_zd_double("0520",&counter_tlog_c.ptrace_no);/*交易金额*/
	  counter_tlog_c.tx_date=g_pub_tx.tx_date;
	  get_zd_double("0400",&counter_tlog_c.tx_amt);
    get_zd_data("0300",counter_tlog_c.main_ac_no);
   	get_zd_data("0660",counter_tlog_c.dc_ind);
   	get_zd_data("0680",counter_tlog_c.ct_ind);
    get_zd_data("0310",counter_tlog_c.app_ac_no);
     ret=Counter_tlog_Ins(counter_tlog_c,g_pub_tx.reply);
    if ( ret )
    {
        Counter_tlog_Debug(&counter_tlog_c);
        sprintf(acErrMsg,"登记柜面通明细出错[%d]!",ret);
        WRITEMSG
            strcpy(g_pub_tx.reply,"P127");
        goto ErrExit;
    }
    /*end 20150605**/
	}

	if (cz_ind[0] == '1')
	{
		tx_amt = -1 * tx_amt;
		set_zd_data("1026","N");/*冲正不检查密码*/
	}
	if (cx_ind == 1)
	{
		set_zd_data("1026","N");/*撤销不检查密码*/
	}
	g_pub_tx.tx_amt1=tx_amt;
	if (strlen(crd_no)<13)
	{
		ret = pub_base_GetParm_Str(crd_no,1,crd_no);
		if (ret)
		{
			vtcp_log("%s,%d,查找错误!reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			goto ErrExit;
		}
		vtcp_log("%s,%d,crd_no[%s]",__FILE__,__LINE__,crd_no);
		/** 20090405 wyw 若账号是参数的情况,账号改为机构号和科目号共同组成 **/
		if (strlen(crd_no)<13)
		{
			memset(acc_opnbr,0,sizeof(acc_opnbr));
			memset(acc_no,0,sizeof(acc_no));
			memcpy(acc_opnbr,crd_no,5);
			memcpy(acc_no,crd_no+5,ACC_NO_LVL3_LEN);
			memset(crd_no,0,sizeof(crd_no));
			strcpy(crd_no,acc_no);
			set_zd_data("0770",acc_opnbr);
		}else
		{
			pub_base_old_acno(crd_no);
		}
	}
	vtcp_log("%s,%d,crd2_no[%s]",__FILE__,__LINE__,crd2_no);
	if (ct_ind[0]=='1')
	{
		/*第二个账号为现金账号*/
		vtcp_log("%s,%d,若是现金交易，则次账号为空",__FILE__,__LINE__);
	}
	else
	{
		{
			if (strlen(crd2_no)<13)
			{
				ret = pub_base_GetParm_Str(crd2_no,1,crd2_no);
				if (ret)
				{
					vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
					goto ErrExit;
				}
				vtcp_log("%s,%d,crd2_no[%s]",__FILE__,__LINE__,crd2_no);
				/** 20090405 wyw 若账号是参数的情况,账号改为机构号和科目号共同组成 **/
				if (strlen(crd2_no)<13)
				{
					memset(acc_opnbr,0,sizeof(acc_opnbr));
					memset(acc_no,0,sizeof(acc_no));
					memcpy(acc_opnbr,crd2_no,5);
					memcpy(acc_no,crd2_no+5,ACC_NO_LVL3_LEN);
					memset(crd2_no,0,sizeof(crd2_no));
					strcpy(crd2_no,acc_no);
					set_zd_data("0770",acc_opnbr);
					vtcp_log("%s,%d,crd2_no[%s],acc_opnbr[%s]",__FILE__,__LINE__,crd2_no,acc_opnbr);
				}
			}
		}
	}
	if (ct_ind[0]=='1') /*现金*/
	{
		if (dc_ind[0]=='1') /*借*/
		{
			if (strlen(crd_no)>12 && crd_no[0] !='9') /*借:客户,贷:现金*/
			{
				set_zd_double("1194",tx_amt);
				set_zd_data("1191",g_pub_tx.cur_no);
				set_zd_data("1197",brf20);
				set_zd_data("1198",crd_no);

				/*活期户*/
				set_zd_data("0910",g_pub_tx.tx_br_no);	
				set_zd_data("1021",crd_no);
				set_zd_data("1022","1");/*活期序号*/
				set_zd_data("1028",g_pub_tx.draw_pwd);
				set_zd_double("102F",tx_amt);	
				set_zd_data("102K","1");/*现转标志*/
				set_zd_data("102J","01");/*币种*/
				set_zd_data("102Y",brf20);
				strcpy(g_pub_tx.reply,"0001"); /*借客户,贷现金:spD003,spM002*/
				vtcp_log("%s,%d,借客户,贷现金分支!",__FILE__,__LINE__);
			}else if(crd_no[0] !='\0' && (strlen(crd_no)<13 || crd_no[0]=='9'))
			{
				/*贷：现金,借:待清算款项,spM002,spA016*/  
				/*他行卡在此存在取现的情况*/
				set_zd_double("1194",tx_amt);	
				set_zd_data("1191",g_pub_tx.cur_no);
				set_zd_data("1197",brf20);
				set_zd_data("1198",crd_no);
				set_zd_data("1198","2006"); /* 现金收支码 */

				set_zd_data("1201",crd_no);
				set_zd_data("120A",brf20);	
				set_zd_data("1205","1");     /*现金*/
				set_zd_data("1204",g_pub_tx.cur_no);
				set_zd_double("1208",tx_amt);
				strcpy(g_pub_tx.reply,"0005"); /*外行卡取现*/
				vtcp_log("%s,%d,现金交暖气费撤销分支!",__FILE__,__LINE__);
				vtcp_log("%s,%d,借:清算,贷:现金分支!",__FILE__,__LINE__);
			}else
			{
				vtcp_log("%s,%d,卡号不存在!",__FILE__,__LINE__);
				goto ErrExit;
			}
		}else if(dc_ind[0] == '2')/*贷*/
		{
			if (strlen(crd_no)>12 && crd_no[0] !='9') /*贷:客户,借:现金*/
			{
				/*现金存款*/
				set_zd_double("1184",tx_amt);	
				set_zd_data("1181",g_pub_tx.cur_no);
				set_zd_data("1187",brf20);
				set_zd_data("1188",crd_no);

				/*活期现金存款*/
				set_zd_data("1011",crd_no);
				set_zd_data("1012","1"); /*活期序号*/
				set_zd_double("1013",tx_amt);
				set_zd_data("101A",g_pub_tx.cur_no);
				set_zd_data("101B","1"); /*现金*/
				/*set_zd_data("1016",brf10);*/
				set_zd_data("0630",brf20);
				strcpy(g_pub_tx.reply,"0002"); /*借:现金,贷:客户spD099,spM001*/
				vtcp_log("%s,%d,借现金贷客户分支!",__FILE__,__LINE__);
			}else if(crd_no[0] != '\0' && (strlen(crd_no)<13 || crd_no[0] =='9'))
			{   
				/*若是内部账户和科目走spA017*/
				set_zd_double("1184",tx_amt);	
				set_zd_data("1181",g_pub_tx.cur_no);
				set_zd_data("1187",brf20);
				set_zd_data("1188",crd_no);

				set_zd_data("1211",crd_no);
				set_zd_double("1218",tx_amt);
				set_zd_data("121A",brf20);
				set_zd_data("1215","1");/*现转标志*/
				set_zd_data("1214",g_pub_tx.cur_no);
				strcpy(g_pub_tx.reply,"0006");/*借:现金,贷:待清算 sM001,spA017*/
				/*取暖交易现金缴款*/
				vtcp_log("%s,%d,借现金贷取暖内部收款户分支!,现金缴暖气费分支",__FILE__,__LINE__);
				vtcp_log("%s,%d,借现金,贷内部户!",__FILE__,__LINE__);
			}else
			{
				vtcp_log("%s,%d,卡号不存在!",__FILE__,__LINE__);
				goto ErrExit;
			}
		}else
		{
			vtcp_log("%s,%d,借贷标志错!",__FILE__,__LINE__);
			goto ErrExit;
		}
	}
	else if (ct_ind[0] == '2') /*转帐*/
	{
		if (dc_ind[0]=='1') /*借*/
		{
			if (crd_no[0] != '\0' && (strlen(crd_no)<13 ||crd_no[0]=='9'))
			{   /*借内部帐*/
				set_zd_data("1201",crd_no);
				set_zd_data("120A",brf20);	
				set_zd_data("1205","2"); /*转帐*/
				set_zd_data("1204",g_pub_tx.cur_no);	
				set_zd_double("1208",tx_amt);
				if (strlen(crd2_no)>12 && crd2_no[0] != '9')
				{
					set_zd_data("1011",crd2_no);
					set_zd_data("1012","1"); /*活期序号*/
					set_zd_double("1013",tx_amt); /*交易金额*/
					set_zd_data("101A",g_pub_tx.cur_no);
					set_zd_data("101B","2"); /*转帐*/
					/*set_zd_data("1016",brf20);*/
					set_zd_data("0630",brf20);
					strcpy(g_pub_tx.reply,"0004"); /*借:待清算,贷:客户 spA016,spD099*/
					vtcp_log("%s,%d,贷:客户,借:清算分支!",__FILE__,__LINE__);
				}
				else if (crd2_no[0] != '\0' && (strlen(crd2_no)<13 || crd2_no[0]=='9'))
				{
					set_zd_data("1211",crd2_no);
					set_zd_double("1218",tx_amt);
					set_zd_data("121A",brf20);
					set_zd_data("1215","2");/*转帐*/
					set_zd_data("1214",g_pub_tx.cur_no);
					strcpy(g_pub_tx.reply,"0007");/*借:内部户贷:待清,spA016,spA017*/
					vtcp_log("%s,%d,借内部户,贷清算分支!",__FILE__,__LINE__);
				}else
				{
					vtcp_log("%s,%d，卡号不存在请检查!",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
			else if (strlen(crd_no)>12 && crd_no[0]!='9')
			{
				/**先进行授权检查*20100618**ADD BY ZHGF BEGIN*/
				if(memcmp(fd24,"gmt",3)==0 && memcmp(crd_no,"5010119500027",13)!=0 && memcmp(crd_no,"5010300100025",13)!=0 &&
					 memcmp(crd_no,"5010687100010",13)!=0 && memcmp(crd_no,"5010750700027",13)!=0){ /**村镇银行柜面通三方交易商行无需授权**/
					if(pub_base_CompDblVal(g_pub_tx.tx_amt1,50000.00)>=0){
						vtcp_log("%s,%d 金额超过限制先进行授权检查!\n",__FILE__,__LINE__);
						ret=check_gmtauth(crd_no,g_pub_tx.tx_date,tx_amt);
						if(ret){
						      vtcp_log("%s,%d,授权检查失败错误!",__FILE__,__LINE__);
						      strcpy(g_pub_tx.reply,"A040");
						      goto ErrExit;
						}
					}
				}
				/**先进行授权检查*20100618**ADD BY ZHGF END*/
				/*借客户*/
				set_zd_data("1021",crd_no);
				set_zd_data("1022","1");/*活期序号*/
				if (crd_no[0]=='5')
				{
					set_zd_data("1023",g_pub_tx.note_type); /*票据种类*/
					set_zd_data("1024",g_pub_tx.beg_note_no); /*票据号码*/
				}
				set_zd_data("1028",g_pub_tx.draw_pwd);
				set_zd_double("102F",tx_amt);	
				set_zd_data("102K","2");/*现转标志*/
				set_zd_data("102J","01");
				set_zd_data("102Y",brf20);
				if (strlen(crd2_no)>12 && crd2_no[0] !='9')
				{ 
					/*借客户贷客户 spD003,spD099*/
					set_zd_data("1011",crd2_no);
					set_zd_data("1012","1");
					set_zd_double("1013",tx_amt);
					set_zd_data("101A",g_pub_tx.cur_no);
					set_zd_data("101B","2");
					/*set_zd_data("1016",brf20);*/
					set_zd_data("0630",brf20);
					strcpy(g_pub_tx.reply,"0008"); /*spD003,spD099*/
					vtcp_log("%s,%d,借客户贷客户分支!",__FILE__,__LINE__);
				}
				else if (crd2_no[0] != '\0' && (strlen(crd2_no) <13 || crd2_no[0]=='9'))
				{
					/*借客户，贷:待清算*/
					/*借:客户,贷:待清算 spD003,spA017*/
					set_zd_data("1211",crd2_no);
					set_zd_double("1218",tx_amt);
					set_zd_data("121A",brf20);
					set_zd_data("1215","2");
					set_zd_data("1214",g_pub_tx.cur_no);
					strcpy(g_pub_tx.reply,"0003");/*借:客户,贷:待清算 spD003,spA017*/
					vtcp_log("%s,%d,借待清算贷客户分支!",__FILE__,__LINE__);
				}
				else
				{
					vtcp_log("%s,%d，卡号不存在请检查!",__FILE__,__LINE__);
					goto ErrExit;
				}
			}else
			{
				vtcp_log("%s,%d，卡号不存在请检查!",__FILE__,__LINE__);
				goto ErrExit;
			}
		}else if(dc_ind[0] == '2')
		{ /*贷*/
			if (crd_no[0] != '\0' && (strlen(crd_no)<13 || crd_no[0]=='9'))
			{    /*贷内部帐*/
				set_zd_data("1211",crd_no);
				set_zd_double("1218",tx_amt);
				set_zd_data("121A",brf20);
				set_zd_data("1215","2");
				set_zd_data("1214",g_pub_tx.cur_no);
				if (strlen(crd2_no)>12 && crd2_no[0] !='9')/*客户*/
				{
					set_zd_data("0910",g_pub_tx.tx_br_no);	
					set_zd_data("1021",crd2_no);
					set_zd_data("1022","1");/*活期序号*/
					set_zd_data("1028",g_pub_tx.draw_pwd);
					set_zd_double("102F",tx_amt);	
					set_zd_data("102K","2");/*现转标志*/
					set_zd_data("102J","01");
					set_zd_data("102Y",brf20);

					strcpy(g_pub_tx.reply,"0003");/*借:客户,贷:客户 spD003,spD099*/
					vtcp_log("%s,%d,借客户待亲算!",__FILE__,__LINE__);
				}
				else if (strlen(crd2_no)<13 || crd2_no[0]=='9')
				{
					set_zd_data("1201",crd2_no);
					set_zd_data("120A",brf20);	
					set_zd_data("1205","2"); /*转帐*/
					set_zd_data("1204",g_pub_tx.cur_no);	
					set_zd_double("1208",tx_amt);
					strcpy(g_pub_tx.reply,"0007");/*借:内部户,贷:待清算spA016,spA017*/
					vtcp_log("%s,%d,借内部户待内部户!",__FILE__,__LINE__);
				}
				else
				{
					vtcp_log("%s,%d，卡号不存在请检查!",__FILE__,__LINE__);
					goto ErrExit;
				}
			}else if(strlen(crd_no)>12 && crd_no[0] !='9')
			{
				/*贷客户*/
				set_zd_data("1011",crd_no);
				set_zd_data("1012","1"); /*活期序号*/
				set_zd_double("1013",tx_amt); /*交易金额*/
				set_zd_data("101A",g_pub_tx.cur_no);
				set_zd_data("101B","2"); /*转帐*/
				/*set_zd_data("1016",brf20);*/
				set_zd_data("0630",brf20);
				if (strlen(crd2_no)>12 && crd2_no[0] !='9')
				{/*借:客户贷:客户*/
					set_zd_data("1021",crd2_no);
					set_zd_data("1022","1"); /* 活期序号 */
					set_zd_data("1028",g_pub_tx.draw_pwd);
					set_zd_double("102F",tx_amt);	
					set_zd_data("102K","2");/*现转标志*/
					set_zd_data("102J","01");
					set_zd_data("102Y",brf20);
					strcpy(g_pub_tx.reply,"0008"); /*spD003,spD099*/
					vtcp_log("%s,%d,借客户贷客户分支!",__FILE__,__LINE__);
				}
				else if (crd2_no[0] != '\0' && (strlen(crd2_no)<13 || crd2_no[0] =='9'))
				{ /*借内部户,贷客户*/
					/*借:内部户,贷:客户 spA016,spD099*/	
					vtcp_log("%s,%d,帐号[%s]",__FILE__,__LINE__,crd2_no);
					set_zd_data("1201",crd2_no);
					set_zd_data("120A",brf20);	
					set_zd_data("1205","2"); /*转帐*/
					set_zd_data("1204",g_pub_tx.cur_no);	
					set_zd_double("1208",tx_amt);
					strcpy(g_pub_tx.reply,"0004"); /*借:待清算,贷:客户 spA016,spD099*/
					vtcp_log("%s,%d,贷:客户,借:清算分支!",__FILE__,__LINE__);
				}
				else
				{
					vtcp_log("%s,%d，卡号不存在请检查!",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
		}else
		{
			vtcp_log("%s,%d,借贷标志错!",__FILE__,__LINE__);
			goto ErrExit;
		}
	}
	else/*add by chenggx 20091225*/
	{
		strcpy(g_pub_tx.reply,"B099");
		vtcp_log("%s,%d,reply[%s]现转标志错误!",__FILE__,__LINE__,g_pub_tx.reply);
		goto ErrExit;
	}
	OkExit:
	/*strcpy( g_pub_tx.reply, "0002" );*/
		sprintf(acErrMsg,"本行设备外行卡处理!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

	ErrExit:
		sprintf(acErrMsg,"银联帐务处理失败!! ");
	WRITEMSG
	if ( !memcmp(g_pub_tx.reply,"0000",4) || strlen(g_pub_tx.reply) ==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
Ins_trade_self_func()
{
	struct chnl_self_trade_c chnl_self_trade;
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	char fee_ind[2],dc_ind[2],ct_ind[2],cz_ind[2];
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	get_zd_data("0660",dc_ind); /*借贷标志*/ 
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	get_zd_data("0680",ct_ind); /*现转标志*/ 
	get_zd_data("0690",fee_ind); /*收费模式*/
	/* 给自助设备交易日记结构赋值 */
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	/****Add by SSH,20090405,增加记录第二帐号****/
	{
		char sTmp[128];
		memset(sTmp,'\0',sizeof(sTmp));
		get_zd_data("0310",sTmp); 
		strncpy(chnl_self_trade.ac_no1,sTmp,sizeof(chnl_self_trade.ac_no1)-1);
	}
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strncpy(chnl_self_trade.tel,g_pub_tx.tel,6);
	if (fee_ind[0]=='1'||fee_ind[0]=='2'||fee_ind[0]=='3'||fee_ind[0]=='4')
	{
		strncpy(chnl_self_trade.ttyp,"1",1);
	}else if(fee_ind[0]=='0')
	{
		strncpy(chnl_self_trade.ttyp,"2",1);
	}else
	{
		strncpy(chnl_self_trade.ttyp,"5",1);
	}
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	get_zd_long("0520", &chnl_self_trade.stra);	/*平台流水*/
	get_zd_long("0450", &chnl_self_trade.dz_date); /*平台日期*/
	/* 本交易对帐中心流水号 */
	if (cz_ind[0]=='1')
	{
		get_zd_long("0780", &chnl_self_trade.strb);   /*原平台流水*/
		get_zd_long("0440", &chnl_self_trade.dz_date);  /* 平台日期*/
	}
	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	strncpy(chnl_self_trade.dz_flag,"0",1);
	strncpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind,1);
	/*get_zd_data("0810", chnl_self_trade.brf_no);*/
	if (fee_ind[0]=='1'||fee_ind[0]=='2')
	{
		if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0)
		{
			chnl_self_trade.info_source[0] == '0';
		}else
		{
			chnl_self_trade.info_source[0] == '1';
		}
	}else
	{
		chnl_self_trade.info_source[0] == '2';
	}
	if (dc_ind[0]=='1')
	{
		chnl_self_trade.add_ind[0]=='0';
	}else
	{
		chnl_self_trade.add_ind[0]=='1';
	}
	strncpy(chnl_self_trade.batch_flag,"0",1);
	strncpy(chnl_self_trade.stat,"0",1);

	/*Chnl_self_trade_Debug(&chnl_self_trade);*/
	/* 登记自助设备交易日记 */
	g_reply_int = pub_base_Insertselftrade(chnl_self_trade);
	if (g_reply_int != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "AT07");
		return(-1);
	}
	return(0);
}
/**********************************************************************
* 函 数 名：  check_gmtauth
* 函数功能：  柜面通远程授权信息检查
* 作者/时间:  liuyong 20100610
*
* 参数：
*     输入: acno,char    :账号
*txdate,long  :授权日期
txamt,double :交易金额
*     输出: 无
*     返回:
* 修改历史：
*
********************************************************************/
int check_gmtauth(char *acno,long txdate,double txamt)
{
  struct auth_modify_ln_rate_c g_auth_modify_ln_rate;
  struct mdm_ac_rel_c g_mdm_ac_rel;

  int ret = 0;
  long sqDate = 0;/*授权日期*/
  char actno[20];         /*账号*/
  char tel[TEL_LENGTH+1]; /*被授权柜员*/
  char brno[6];

  memset(actno,0x00,sizeof(actno));
  memset(tel, 0x00 , sizeof(tel));
  memset(brno,0x00,sizeof(brno));
  memset(&g_auth_modify_ln_rate,0x00,sizeof(struct auth_modify_ln_rate_c));
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

  get_zd_data(DC_TEL,tel);
  get_zd_data(DC_TX_BR_NO,brno);
  sqDate = g_pub_tx.tx_date;
  memcpy(actno,acno,strlen(acno));

  if(!strlen(actno))
  {
      sprintf(acErrMsg,"账号为空");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }

  /*授权时间检查*/
  /**应该与auth_modify_ln_rate表里的授权日期检查**20100618
  if(txdate != sqDate)
  {
      sprintf(acErrMsg,"授权时间与系统时间不符!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }**/

  ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",actno);
  if(ret)
  {
      sprintf(acErrMsg,"账号不存在!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return ret;
  }
  ret=sql_count("auth_modify_ln_rate","ac_no ='%s' and auth_date=%ld and auth_sts='0' and amt=%lf",actno,txdate,txamt);
  if(ret>1){
	sprintf(acErrMsg,"同笔交易存在多笔授权请检查!");
	set_zd_data("0130",acErrMsg);
	WRITEMSG
	return ret;
  }

  ret = Auth_modify_ln_rate_Sel(g_pub_tx.reply,&g_auth_modify_ln_rate,"ac_no ='%s' and auth_date=%ld and auth_sts='0' and amt=%lf",actno,txdate,txamt);
  if(ret)
  {
      sprintf(acErrMsg,"没有该笔交易的授权信息!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return ret;
  }

  if(g_auth_modify_ln_rate.auth_sts[0]=='1')
  {
      sprintf(acErrMsg,"该笔交易已经发生过了!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }
  char fd60[17]; /* 柜员机构 */
  char fd24[4]; /* 业务类型 */
  char cTxbrno[6];/*对方交易机构**/
  char cTxtel[7];/*对方交易柜员**/
  get_zd_data("0240",fd24);
  if(memcmp(fd24,"gmt",3)==0)
  {
       memset(fd60,0,sizeof(fd60));
       memset(cTxbrno,0,sizeof(cTxbrno));
       memset(cTxtel,0,sizeof(cTxtel));
       get_zd_data("0600",fd60);
       vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
       memcpy(cTxbrno,fd60,5);
       memcpy(cTxtel,fd60+5,6);
  }
  vtcp_log("%s,%d g_pub_tx.tel=[%s]\n",__FILE__,__LINE__,g_pub_tx.tel);
  if(memcmp(g_auth_modify_ln_rate.tel,cTxtel,4)!=0)
  {
      sprintf(acErrMsg,"该笔交易未授给该柜员!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }

  vtcp_log("%s,%d g_auth_modify_ln_rate.brno=[%s]\n",__FILE__,__LINE__,g_auth_modify_ln_rate.brno);
  vtcp_log("%s,%d cTxbrno=[%s]\n",__FILE__,__LINE__,cTxbrno);
  if(memcmp(g_auth_modify_ln_rate.brno,cTxbrno,5)!=0)
  {
      sprintf(acErrMsg,"该笔交易未授给该机构!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }
  vtcp_log("%s,%d 金额转换结果amt=[%lf],amt=[%lf]\n",__FILE__,__LINE__,g_auth_modify_ln_rate.amt,txamt);
  if(pub_base_CompDblVal(g_auth_modify_ln_rate.amt,txamt) !=0)
  {
      sprintf(acErrMsg,"授权金额与发生金额不相符!");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }
  vtcp_log("%s,%d g_auth_modify_ln_rate.auth_date=[%ld]\n",__FILE__,__LINE__,g_auth_modify_ln_rate.auth_date);
  vtcp_log("%s,%d g_pub_tx.tx_date=[%ld]\n",__FILE__,__LINE__,g_pub_tx.tx_date);
  if(g_auth_modify_ln_rate.auth_date!=g_pub_tx.tx_date){
      sprintf(acErrMsg,"授权日期与交易日期不符,授权信息无效");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }

  sprintf(acErrMsg,"柜面远程授权信息检查完成!");
  WRITEMSG
  ret = sql_execute("update auth_modify_ln_rate set auth_sts='1' where ac_no ='%s' and auth_date=%ld and auth_sts='0' and amt=%lf",actno,txdate,txamt);
  if(ret){
      sprintf(acErrMsg,"更新授权信息表错误");
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      return 1;
  }


  return 0;
}

