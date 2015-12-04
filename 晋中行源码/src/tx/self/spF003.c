/*************************************************
* 文 件 名:  spF003.c
* 功能描述： ATM 取款(判断是数据来源)
帐务处理类入口:
*
* 作	者:  rob
* 完成日期： 2003年2月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card_reg_c.h"
#include "card_consum_lmt_c.h"
#include "chnl_self_trade_c.h"
#include "card.h" /* add by LiuHuafeng 20070127 */
/*add by lifei for nx 2009-8-1 9:13:29 */
#include "march_info_c.h"
#include "march_mst_c.h"
#include "march_mst_hst_c.h"
#include "com_parm_c.h"

int spF003()
{
	int ret=0;
	char flag;
	char crd_no[25];
	char tmpstr[25];
	char dc_ind[2], ct_ind[2], cz_ind[2],fee_ind[2],ysq_ind[2],s_ct_ind[2];
	long cx_ind=0;
	char trade_type[4];
	char auth_no[8];
	char Yw_type[4],Check_flag[2];
	char cAcc_no[8];
	char cAcno[25];
	double tx_amt=0.0;
	struct chnl_tradeadd_c chnl_tradeadd;
	struct chnl_self_trade_c chnl_self_trade;
	struct chnl_tradelimit_c chnl_tradelimit;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c v_mdm_ac_rel; 
	struct com_parm_c   sCom_parm;
	struct chnl_self_trade_c  sChnl_self_trade;
	/*add by lifei for nx 2009-8-2 16:45:34 本行pos传商户号*/
	long cx_th;
	int ret1=0;
	long lCount=0;
	long pt_trace_no=0;
	long pt_date=0;
	long old_trace_no=0;
	long old_date=0;
	double fee_amt=0.0;
	double th_amt=0.00;
	double th_amt1=0.00;
	char ls_ac_no[25];
	struct march_info_c sMarch_info;
	struct march_mst_c sMarch_mst;
	struct march_mst_c sMarch_mst_t;
	struct march_mst_hst_c sMarch_mst_hst;
	struct march_mst_hst_c sMarch_mst_hst_t;
	struct march_mst_hst_c sMarch_mst_hst_s;
	/*memset(cx_th,0x00,sizeof(cx_th));*/
	memset(ls_ac_no,0x00,sizeof(ls_ac_no));
	memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
	memset(&sMarch_mst_t,0x00,sizeof(sMarch_mst_t));
	memset(&sMarch_info,0x00,sizeof(sMarch_info));	
	memset(&sMarch_mst_hst,0x00,sizeof(sMarch_mst_hst));	
	memset(&sMarch_mst_hst_t,0x00,sizeof(sMarch_mst_hst_t));
	memset(&sMarch_mst_hst_s,0x00,sizeof(sMarch_mst_hst_s));
	/*end by lifei for nx 2009-8-2 16:45:34 本行pos传商户号*/

	memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	memset(&chnl_tradelimit, 0x0, sizeof(struct chnl_tradelimit_c));	
	memset(&sDd_mst,0,sizeof(struct dd_mst_c));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&v_mdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sCom_parm,0,sizeof(struct com_parm_c));

	memset(crd_no, 0x0, sizeof(crd_no));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(ysq_ind, 0x0, sizeof(ysq_ind));
	memset(s_ct_ind, 0x0, sizeof(s_ct_ind));
	memset(auth_no, 0x0, sizeof(auth_no));
	memset(cAcc_no, 0x0, sizeof(cAcc_no));
	memset(cAcno, 0x0, sizeof(cAcno));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	vtcp_log("%s,%d cz_ind=[%s]\n",__FILE__,__LINE__,cz_ind);
	get_zd_data("0700",ysq_ind); /*预授权标志 */
	vtcp_log("%s,%d ysq_ind=[%s]\n",__FILE__,__LINE__,ysq_ind);
	get_zd_data("0680",ct_ind);  /*现转标志*/
	vtcp_log("%s,%d ct_ind=[%s]\n",__FILE__,__LINE__,ct_ind);
	get_zd_data("0690",fee_ind); /*费率标志*/
	vtcp_log("%s,%d fee_ind=[%s]\n",__FILE__,__LINE__,fee_ind);
	get_zd_data("0660",dc_ind);  /*借贷标志*/
	vtcp_log("%s,%d dc_ind=[%s]\n",__FILE__,__LINE__,dc_ind);
	get_zd_data("0720",s_ct_ind);/*实际现转标志*/
	vtcp_log("%s,%d s_ct_ind=[%s]\n",__FILE__,__LINE__,s_ct_ind);
	get_zd_data("0300", crd_no); /*主帐号*/
	get_zd_data("0300",g_pub_tx.crd_no);
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0790",g_pub_tx.draw_pwd); /*支付密码*/
	get_zd_long("0500",&cx_ind);  /*撤销标志,1:撤销*/
	get_zd_double("0400",&g_pub_tx.tx_amt1);/*交易金额*/
	get_zd_double("0400",&tx_amt);
	vtcp_log("[%s][%d] tx_amt=[%f] g_pub_tx.tx_amt1=[%f]",__FILE__,__LINE__,tx_amt,g_pub_tx.tx_amt1);
	get_zd_data("0240",Yw_type);   /* 业务类型 */
	strcpy(g_pub_tx.cur_no, "01");
	pub_base_old_acno(g_pub_tx.crd_no);
	pub_base_old_acno(crd_no);
	vtcp_log("%s,%d,他代本卡号[%s]",__FILE__,__LINE__,crd_no);
	
	get_zd_data("0320",cAcno);/*若fee_ind为i,则32域需要传IC卡电子现金主账号*/
	vtcp_log("%s,%d,帐号为[%s]",__FILE__,__LINE__,cAcno);
	pub_base_old_acno(cAcno);
	/***20120306IC卡电子钱包账户转换成科目号 add by wzw***/
	if(memcmp(cAcno,ICCARDHEADCODE,6) == 0 && fee_ind[0]=='i')
	{
  /*  ret=pub_base_GetParm_Str( "ICACC",1, cAcc_no );
    strcpy(g_pub_tx.crd_no, cAcc_no );*/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &v_mdm_ac_rel, "ac_no='%s'", cAcno);
    if (ret)
		{
			vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	  ret=Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='%s' and parm_seqn=11", v_mdm_ac_rel.opn_br_no);
    if (ret)
		{
			vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		strcpy(g_pub_tx.crd_no, sCom_parm.val );
  }
	
	if (memcmp(crd_no,CARDHEADCODE,6) == 0 || memcmp(crd_no,ICCARDHEADCODE,6) == 0 )
	{/*****20090729 宁夏卡不校验密码**********/
		g_pub_tx.draw_pwd_yn[0] = 'N'; /*交易不检查密码*//*20090729*/
		set_zd_data("1026","N");
	}else{/**ADD BY ZHGF ELSE 非卡就要检查密码20100528**/

		/*林冬燕添加,柜面通，折贷，不检查密码*/
		if(dc_ind[0]=='2' && !memcmp(Yw_type,"gmt",3)){
			g_pub_tx.draw_pwd_yn[0] = 'N'; 
			set_zd_data("1026","N");
		}else{
			g_pub_tx.draw_pwd_yn[0] = 'Y'; 
			set_zd_data("1026","Y");
			/**科目号不校验 add by zhaodawei 20110603**/
			if(strlen(crd_no)>7)
			{
				/**添加存折密码校验-王文奎-20110112**/
				ret=pub_base_check_draw("101");
				if(ret)
				{
					vtcp_log("%s,%d,密码错误", __FILE__,__LINE__);
					goto	ErrExit;
				}
				/**添加结束-20110112**/
			}
			/**end 科目号不校验 add by zhaodawei 20110603**/
		}
	}
	
	/** 20090407 wyw 修改校验户名和证件类型 **/
	vtcp_log("%s,%d,70域标志[%s]",__FILE__,__LINE__,ysq_ind);
	if (ysq_ind[0]>'4')
	{
		vtcp_log("%s,%d,70域标志[%s]",__FILE__,__LINE__,ysq_ind);
		memset(Check_flag,0,sizeof(Check_flag));
		Check_flag[0]=ysq_ind[0];
		ysq_ind[0]='0';
		set_zd_data("0700","0"); /* 70域大于4说明是校验标志，取出后将其置成0，走账接口用 **/
		if (Check_flag[0]=='5') /*校验第二个账号的户名*/
		{
			vtcp_log("%s,%d,户名[%s]",__FILE__,__LINE__,g_pub_tx.name);
			/* 这里只验证了活期 */	
			get_zd_data("0260",g_pub_tx.name);
			get_zd_data("0310",g_pub_tx.ac_no1);
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no1);
			if (ret)
			{
				vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%d and ac_seqn=1",sMdm_ac_rel.ac_id);
			if (ret)
			{
				vtcp_log("%s,%d,reply[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.reply);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			if (memcmp(g_pub_tx.name,sDd_mst.name,sizeof(sDd_mst.name)))
			{
				vtcp_log("%s,%d,name[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.name);
				strcpy(g_pub_tx.reply,"B005");
				goto ErrExit;
			}
		}else if(Check_flag[0]=='6') /**校验第一个账号的证件号码**/
		{
			get_zd_data("0620",g_pub_tx.id_no);
			get_zd_data("0300",g_pub_tx.ac_no);
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
			if (ret)
			{
				vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			if (strcmp(sMdm_ac_rel.id_no,g_pub_tx.id_no))
			{
				strcpy(g_pub_tx.reply,"M004");
				vtcp_log("%s,%d,id_no[%s],证件号码不符!",__FILE__,__LINE__,g_pub_tx.id_no);
				goto ErrExit;
			}
		}
	}
	/*add by lifei for nx 2009-8-2 16:53:39 本行pos31域传的是商户号，转为帐户*/
	if (fee_ind[0]=='7')
	{
		vtcp_log("%s %d 进入本行pos分支。。。。。。",__FILE__,__LINE__);
		
		get_zd_data("0310",ls_ac_no);
		vtcp_log("%s,%d,商户号[%s]",__FILE__,__LINE__,ls_ac_no);
		get_zd_long("0780",&old_trace_no); /* 原平台流水号 */
		get_zd_long("0450",&old_date);
		get_zd_long("0520",&pt_trace_no);
		get_zd_long("0440",&pt_date);
		get_zd_double("0410",&fee_amt);
		get_zd_long("0500",&cx_th);  /*撤销标志,1:撤销*/
		
		set_zd_data("0320",ls_ac_no);/*先将商户号放在32域*/
		
		if (ysq_ind[0]=='0'  || ysq_ind[0]=='3' || ysq_ind[0]=='4')/*pos消费或ysq完成*/
		{
			vtcp_log("%s,%d,本行POS分支ysq_ind[%s]",__FILE__,__LINE__,ysq_ind);
			ret = March_info_Sel(g_pub_tx.reply,&sMarch_info,"mrchno = '%s'",ls_ac_no);
			if (ret)
			{
				vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"CU03");
				goto ErrExit;			
			}
			
			if (dc_ind[0]=='1' && cz_ind[0]=='0')
			{
				vtcp_log("%s,%d,本行POS分支dc_ind[%s],cz_ind[%s]",__FILE__,__LINE__,dc_ind,cz_ind);
				ret = March_mst_Sel(g_pub_tx.reply,&sMarch_mst,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
				if (ret!=0 && ret!=100)
				{
					vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"CU03");
					goto ErrExit;		
				}			
				if (ret==100)
				{
					vtcp_log("%s,%d,本行POS分支插入商户交易信息",__FILE__,__LINE__);
					strcpy(sMarch_mst.march_code,sMarch_info.mrchno);
					strcpy(sMarch_mst.march_name,sMarch_info.mrchnm);
					strcpy(sMarch_mst.ac_no,sMarch_info.mrchacno_ls);
					sMarch_mst.tx_date = g_pub_tx.tx_date;
					sMarch_mst.tx_cnt = 1;
					sMarch_mst.tx_amt = tx_amt;
					sMarch_mst.fee_amt = fee_amt;
					sMarch_mst.deal_sign[0] = '0';
					/*get_zd_data("1261",&sMarch_mst.tt_no);add by cgx 20090818 nxhhyh */
					ret1 = March_mst_Ins(sMarch_mst,g_pub_tx.reply);
					if (ret1)
					{
						vtcp_log("%s,%d,插入商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
						strcpy(g_pub_tx.reply,"AT03");
						goto ErrExit;					
					}	
				}
				if (!ret)
				{
					vtcp_log("%s,%d,本行POS分支更新商户交易信息",__FILE__,__LINE__);	
					ret1 = March_mst_Dec_Upd(g_pub_tx.reply,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
					if (ret1)
					{
						vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
						strcpy(g_pub_tx.reply,"AT03");
						goto ErrExit;					
					}		
					ret1= March_mst_Fet_Upd(&sMarch_mst_t,g_pub_tx.reply);
					if (ret1)
					{
						vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
						strcpy(g_pub_tx.reply,"AT03");
						goto ErrExit;					
					}	
															
					sMarch_mst_t.tx_cnt++;
					sMarch_mst_t.tx_amt += tx_amt;
					sMarch_mst_t.fee_amt += fee_amt;
					
					ret1 = March_mst_Upd_Upd(sMarch_mst_t,g_pub_tx.reply);
					if (ret1)
					{
						vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
						strcpy(g_pub_tx.reply,"AT03");
						goto ErrExit;					
					}		
					March_mst_Clo_Upd();		
				}
				set_zd_data("0310",sMarch_info.mrchacno_ls);
			}
			if ((dc_ind[0]=='1' && cz_ind[0]=='1') || (dc_ind[0]=='2' && cz_ind[0]=='0'))
			{
				vtcp_log("%s,%d,本行POS分支dc_ind[%s],cz_ind[%s],cx_th[%d]",__FILE__,__LINE__,dc_ind,cz_ind,cx_th);
				
				/**为行内自助POS退费接口添加 begin lzy 20141125 **/
				if (cx_th==3 && dc_ind[0]=='2' && cz_ind[0]=='0') 
				{
					ret = March_mst_Sel(g_pub_tx.reply,&sMarch_mst,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
					if (ret!=0 && ret!=100)
					{
						vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"CU03");
						goto ErrExit;		
					}			
					if (ret==100)
					{
						vtcp_log("%s,%d,本行POS分支插入商户交易信息",__FILE__,__LINE__);
						strcpy(sMarch_mst.march_code,sMarch_info.mrchno);
						strcpy(sMarch_mst.march_name,sMarch_info.mrchnm);
						strcpy(sMarch_mst.ac_no,sMarch_info.mrchacno_ls);
						sMarch_mst.tx_date = g_pub_tx.tx_date;
						sMarch_mst.tx_cnt = 1;
						sMarch_mst.tx_amt = -tx_amt;
						sMarch_mst.fee_amt = -fee_amt;
						sMarch_mst.deal_sign[0] = '0';
						/*get_zd_data("1261",&sMarch_mst.tt_no);add by cgx 20090818 nxhhyh */
						ret1 = March_mst_Ins(sMarch_mst,g_pub_tx.reply);
						if (ret1)
						{
							vtcp_log("%s,%d,插入商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}	
					}
					if (!ret)
					{
						vtcp_log("%s,%d,本行POS分支更新商户交易信息",__FILE__,__LINE__);	
						ret1 = March_mst_Dec_Upd(g_pub_tx.reply,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}		
						ret1= March_mst_Fet_Upd(&sMarch_mst_t,g_pub_tx.reply);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}
						sMarch_mst_t.tx_cnt++;
						sMarch_mst_t.tx_amt  -= tx_amt;
						sMarch_mst_t.fee_amt -= fee_amt;						
						ret1 = March_mst_Upd_Upd(sMarch_mst_t,g_pub_tx.reply);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}		
						March_mst_Clo_Upd();		
					}					
					if (sMarch_mst.deal_sign[0]=='0')
						set_zd_data("0310",sMarch_info.mrchacno_ls);
					else
						set_zd_data("0310",sMarch_info.mrchacno);
				} 
				/**为行内自助POS退费接口添加 end lzy 20141125 **/
				else
				{
					memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
					memset(&sMarch_mst_hst,0x00,sizeof(sMarch_mst_hst));
					ret = March_mst_hst_Sel(g_pub_tx.reply,&sMarch_mst_hst,"march_code= '%s' and pt_trace_no=%ld and pt_date = %ld",sMarch_info.mrchno,old_trace_no,old_date);
					if (ret)
					{
						vtcp_log("%s,%d,没有该笔消费信息!ret[%ld]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"CU03");
						goto ErrExit;		
					}									
					if (cx_th==0 && dc_ind[0]=='2' && cz_ind[0]=='0')	/*退货*/
					{
						vtcp_log("%s,%d,本行POS分支cx_th[%d],dc_ind[%s],cz_ind[%s]",__FILE__,__LINE__,cx_th,dc_ind,cz_ind);
						ret = sql_sum_double("march_mst_hst","tx_amt",&th_amt,"march_code='%s' and pt_trace_no=%ld and pt_date=%ld and cx_ind='0' and dc_ind='1' and cz_ind='0'",sMarch_info.mrchno,old_trace_no,old_date);
						if (ret)
						{
							vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
							strcpy(g_pub_tx.reply,"CU03");
							goto ErrExit;					
						}	
							
						ret = sql_sum_double("march_mst_hst","tx_amt",&th_amt," cz_pt_trace_no in (select pt_trace_no from march_mst_hst \
									 where march_code='%s' and pt_trace_no=%ld and pt_date=%ld and cx_ind='0' and dc_ind='1' and cz_ind='0') \ 
	 									and cz_pt_date in (select pt_date from march_mst_hst \
										 where march_code='%s' and pt_trace_no=%ld and pt_date=%ld and cx_ind='0' and dc_ind='1' and cz_ind='0') \
										and cx_ind='0' and dc_ind='2' and cz_ind='0'	",sMarch_info.mrchno,old_trace_no,old_date,sMarch_info.mrchno,old_trace_no,old_date);
						if (ret)
						{
							vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
							strcpy(g_pub_tx.reply,"CU03");
							goto ErrExit;					
						}						
						
						if (pub_base_CompDblVal(sMarch_mst_hst.tx_amt,tx_amt+th_amt-th_amt1)<0)
						{
							vtcp_log("%s,%d,退货金额超过原消费金额!",__FILE__,__LINE__);
							strcpy(g_pub_tx.reply,"LS51");
							goto ErrExit;						
						}											
					}
					if (cx_th==1 && dc_ind[0]=='2' && cz_ind[0]=='0')	/*撤销*/
					{
						memset(&sMarch_mst_hst_t,0x00,sizeof(sMarch_mst_hst_t));
						memset(&sMarch_mst_hst_s,0x00,sizeof(sMarch_mst_hst_s));
						vtcp_log("%s,%d,本行POS分支撤销",__FILE__,__LINE__);
						if (pub_base_CompDblVal(sMarch_mst_hst.tx_amt,tx_amt)!=0)
						{
							vtcp_log("%s,%d,账面余额不足!",__FILE__,__LINE__);
							strcpy(g_pub_tx.reply,"YYL8");
							goto ErrExit;						
						}
						/*执行撤销交易前的检查 判断 修改 mod beg by ChengGX 2010-2-26 11:06
						lCount=sql_count("march_mst_hst","march_code='%s' and cz_pt_trace_no=%ld and cz_pt_date=%ld and cx_ind='1' and dc_ind='2' and cz_ind='0'",sMarch_info.mrchno,old_trace_no,old_date);
						if (lCount!=0)
						{
							vtcp_log("%s,%d,只能撤销一次!",__FILE__,__LINE__);
							strcpy(g_pub_tx.reply,"YY65");
							goto ErrExit;							
						}*/
						vtcp_log("%s,%d,POS消费撤销交易",__FILE__,__LINE__);
						ret = March_mst_hst_Dec_Sel( g_pub_tx.reply , "march_code='%s' and cz_pt_trace_no=%ld and cz_pt_date=%ld and cx_ind='1' and dc_ind='2' and cz_ind='0'" , sMarch_info.mrchno,old_trace_no,old_date );
						if (ret){
						    vtcp_log("%s,%d,定义查询游标错误!ret[%ld]",__FILE__,__LINE__,ret);
	                        strcpy(g_pub_tx.reply,"CU03");
						    goto ErrExit;
	                    }
	                    while(1){
	                        vtcp_log("%s,%d,POS消费撤销交易",__FILE__,__LINE__);
	    					ret = March_mst_hst_Fet_Sel( &sMarch_mst_hst_t , g_pub_tx.reply );
	    					vtcp_log("%s,%d,POS消费撤销交易ret [%ld]",__FILE__,__LINE__,ret);
	    					if(ret==100){
	    					    vtcp_log("%s,%d,POS消费撤销交易 退出循环",__FILE__,__LINE__);
	    					    break;
	    					}
	                        else if(ret){
	                            vtcp_log("%s,%d,查询商户交易明细错误!ret[%ld]",__FILE__,__LINE__,ret);
	                            strcpy(g_pub_tx.reply,"CU03");
	    					    goto ErrExit;
	                        }
	                        vtcp_log("%s,%d,POS消费撤销交易",__FILE__,__LINE__);
	                        ret=March_mst_hst_Sel1(g_pub_tx.reply,&sMarch_mst_hst_s,"march_code='%s' and cz_pt_trace_no=%ld and cz_pt_date=%ld and cx_ind='1' and dc_ind='2' and cz_ind='1'" , sMarch_info.mrchno,sMarch_mst_hst_t.pt_trace_no,sMarch_mst_hst_t.pt_date);
	                        vtcp_log("%s,%d,POS消费撤销交易 ret[%ld]",__FILE__,__LINE__,ret);
	                        if(ret==100){
	    					    vtcp_log("%s,%d,只能撤销一次!",__FILE__,__LINE__);
	    						strcpy(g_pub_tx.reply,"YY65");
	    						goto ErrExit;
	    					}
	                        else if(ret){
	                            vtcp_log("%s,%d,查询商户交易明细错误!ret[%ld]",__FILE__,__LINE__,ret);
	                            strcpy(g_pub_tx.reply,"CU03");
	    					    goto ErrExit;
	                        }
	                    }
	                    March_mst_hst_Clo_Sel();
						/*执行撤销交易前的检查 判断 mod end by ChengGX 2010-2-26 11:06 */
					}
					else
					{
						if (pub_base_CompDblVal(sMarch_mst_hst.tx_amt,tx_amt)<0)
						{
							vtcp_log("%s,%d,账面余额不足!",__FILE__,__LINE__);
							strcpy(g_pub_tx.reply,"YY13");
							goto ErrExit;						
						}
					}			
					ret = March_mst_Sel(g_pub_tx.reply,&sMarch_mst,"march_code = '%s' and tx_date = %ld",sMarch_info.mrchno,sMarch_mst_hst.tx_date);	
					if (ret)
					{
						vtcp_log("%s,%d,查找商户信息错误!ret[%ld]",__FILE__,__LINE__,ret);
	        					strcpy(g_pub_tx.reply,"CU03");
	        					goto ErrExit;					
					}	
					if (pub_base_CompDblVal(sMarch_mst.tx_amt,tx_amt)<0 || pub_base_CompDblVal(sMarch_mst.fee_amt,fee_amt)<0)
					{
						vtcp_log("%s,%d,账面余额不足!sMarch_mst.tx_amt[%f],tx_amt[%f]",__FILE__,__LINE__);
						vtcp_log("%s,%d,sMarch_mst.tx_amt[%f],tx_amt[%f]",__FILE__,__LINE__,sMarch_mst.tx_amt,tx_amt);
						vtcp_log("%s,%d,sMarch_mst.fee_amt[%f],fee_amt[%f]",__FILE__,__LINE__,sMarch_mst.fee_amt,fee_amt);
						strcpy(g_pub_tx.reply,"LS51");
						goto ErrExit;						
					}
					else
					{
						ret1 = March_mst_Dec_Upd(g_pub_tx.reply,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}		
						ret1= March_mst_Fet_Upd(&sMarch_mst_t,g_pub_tx.reply);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}	
						sMarch_mst_t.tx_cnt++;
						sMarch_mst_t.tx_amt -= tx_amt;
						sMarch_mst_t.fee_amt -= fee_amt;						
						ret1 = March_mst_Upd_Upd(sMarch_mst_t,g_pub_tx.reply);
						if (ret1)
						{
							vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
							strcpy(g_pub_tx.reply,"AT03");
							goto ErrExit;					
						}		
						March_mst_Clo_Upd();									
					}
					if (sMarch_mst.deal_sign[0]=='0')
						set_zd_data("0310",sMarch_info.mrchacno_ls);
					else
						set_zd_data("0310",sMarch_info.mrchacno);					
				}
			}
			if (dc_ind[0]=='2' && cz_ind[0]=='1')
			{
				vtcp_log("%s,%d,本行POS分支dc_ind[%s],cz_ind[%s]",__FILE__,__LINE__,dc_ind,cz_ind);
				memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
				memset(&sMarch_mst_hst_t,0x00,sizeof(sMarch_mst_hst_t));		
				
				ret = March_mst_hst_Sel(g_pub_tx.reply,&sMarch_mst_hst_t,"march_code= '%s' and pt_trace_no=%ld and pt_date = %ld",sMarch_info.mrchno,old_trace_no,old_date);
				if (ret)
				{
					vtcp_log("%s,%d,未作撤销或退货或退费!ret[%ld]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"CU03");
					goto ErrExit;		
				}	
				
				if (cx_th != 3)  /**增加退费接口后,如是退费冲正跳过下面的检查 lzy 20141127**/
				{
					memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
					memset(&sMarch_mst_hst,0x00,sizeof(sMarch_mst_hst));		
					ret = March_mst_hst_Sel(g_pub_tx.reply,&sMarch_mst_hst,"march_code= '%s' and pt_trace_no=%ld and pt_date = %ld",sMarch_info.mrchno,sMarch_mst_hst_t.cz_pt_trace_no,sMarch_mst_hst_t.cz_pt_date);
					if (ret)
					{
						vtcp_log("%s,%d,未作撤销!ret[%ld]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"CU03");
						goto ErrExit;		
					}
				}	

				ret1 = March_mst_Dec_Upd(g_pub_tx.reply,"march_code = '%s' and tx_date =%ld",sMarch_info.mrchno,g_pub_tx.tx_date);
				if (ret1)
				{
					vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
					strcpy(g_pub_tx.reply,"AT03");
					goto ErrExit;					
				}		
				ret1= March_mst_Fet_Upd(&sMarch_mst,g_pub_tx.reply);
				if (ret1)
				{
					vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
					strcpy(g_pub_tx.reply,"AT03");
					goto ErrExit;					
				}	
														
				sMarch_mst.tx_cnt++;
				sMarch_mst.tx_amt += tx_amt;
				sMarch_mst.fee_amt += fee_amt;
				
				ret1 = March_mst_Upd_Upd(sMarch_mst,g_pub_tx.reply);
				if (ret1)
				{
					vtcp_log("%s,%d,更新商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
					strcpy(g_pub_tx.reply,"AT03");
					goto ErrExit;					
				}		
				March_mst_Clo_Upd();					
		
				if (sMarch_mst.deal_sign[0]=='0')
					set_zd_data("0310",sMarch_info.mrchacno_ls);
				else
					set_zd_data("0310",sMarch_info.mrchacno);												
			}
			get_zd_data("0310",ls_ac_no);
			vtcp_log("%s,%d,商户号[%s]",__FILE__,__LINE__,ls_ac_no);
			memset(&sMarch_mst_hst,0x00,sizeof(sMarch_mst_hst));
			strcpy(sMarch_mst_hst.march_code,sMarch_info.mrchno);
			strcpy(sMarch_mst_hst.mch_name,sMarch_info.mrchnm);
			strcpy(sMarch_mst_hst.xf_ac_no,crd_no);
			strcpy(sMarch_mst_hst.ac_no,ls_ac_no);
			sMarch_mst_hst.tx_date=g_pub_tx.tx_date;
			sMarch_mst_hst.trace_no = g_pub_tx.trace_no;
			sMarch_mst_hst.pt_trace_no = pt_trace_no;
			sMarch_mst_hst.pt_date = pt_date;
			sMarch_mst_hst.cz_pt_trace_no = old_trace_no;
			sMarch_mst_hst.cz_pt_date = old_date;
			sMarch_mst_hst.dc_ind[0]=dc_ind[0];
			sMarch_mst_hst.cz_ind[0]=cz_ind[0];
			sMarch_mst_hst.ysq_ind[0]=ysq_ind[0];
			sprintf(sMarch_mst_hst.cx_ind,"%1d",cx_th);
			/*tmpstr*add by chenggh 20091220 小数点后保留两位小数 */
			memset(tmpstr,0,sizeof(tmpstr));
			sprintf(tmpstr,"%016.2f",tx_amt);
			vtcp_log("%s,%d,tmp[%s]",__FILE__,__LINE__,tmpstr);
			sMarch_mst_hst.tx_amt=atof(tmpstr);
			memset(tmpstr,0,sizeof(tmpstr));
			sprintf(tmpstr,"%016.2f",fee_amt);
			vtcp_log("%s,%d,tmp[%s]",__FILE__,__LINE__,tmpstr);
			sMarch_mst_hst.fee_amt=atof(tmpstr);
			March_mst_hst_Debug(&sMarch_mst_hst);
			/*add by chenggh 20091220 小数点后保留两位小数 */
			ret = March_mst_hst_Ins(sMarch_mst_hst,g_pub_tx.reply);
			if (ret)
			{
				vtcp_log("%s,%d,插入商户消费信息错误!ret[%ld]",__FILE__,__LINE__,ret1);
				strcpy(g_pub_tx.reply,"AT03");
				goto ErrExit;						
			}			
		}	
	}
	/*end by lifei for nx 2009-8-2 16:53:39 本行pos31域传的是商户号，转为帐户*/

	if (cz_ind[0]=='1' && ysq_ind[0] == '0')/*冲正交易同向记负*/
	{
		tx_amt = -1 * tx_amt;
		g_pub_tx.draw_pwd_yn[0] = 'N'; /*冲正交易不检查密码*/
		/** 20090512 wyw 若是冲正首先判断原交易是否成功 */	
		memset(&sChnl_self_trade,0,sizeof(struct chnl_self_trade_c));
		get_zd_long("0780",&sChnl_self_trade.stra);
		get_zd_long("0450",&sChnl_self_trade.dz_date);
		get_zd_double("0400",&sChnl_self_trade.tx_amt);
		ret= Chnl_self_trade_Sel(g_pub_tx.reply,&sChnl_self_trade,"dz_date=%ld and stra=%ld and tx_amt=%lf",sChnl_self_trade.dz_date,sChnl_self_trade.stra,sChnl_self_trade.tx_amt);
		if (ret)
		{
			vtcp_log("%s,%d,要冲正的纪录不存在![%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"CU25");
			goto ErrExit;
		}
	}
	if (fee_ind[0]==0)
	{
		memcpy(trade_type,"POS",3);
	}
	else if (fee_ind[0]>'5')
	{
		memcpy(trade_type,"GMM",3);
	}
	else
	{
		memcpy(trade_type,"ATM",3);
	}
	if (strlen(crd_no)>15 && (memcmp(crd_no,CARDHEADCODE,6) != 0 && memcmp(crd_no,ICCARDHEADCODE,6) != 0))/*非本行卡业务不受理*/
	{
		vtcp_log("%s,%d,非本行卡不受理crd_no[%s]",__FILE__,__LINE__,crd_no);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
	}
	/* 检查卡的状态 **/
	if ((memcmp(crd_no,CARDHEADCODE,6) == 0 || memcmp(crd_no,ICCARDHEADCODE,6) == 0) && cz_ind[0]=='0')
	{
		/* 检查卡的合法性 */
		vtcp_log("%s,%d,卡号[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
		if (pub_base_CheckCrdNo())
		{
			sprintf(acErrMsg,"检查卡的合法性错误!!");
			WRITEMSG
				goto ErrExit;
		}
		memset(&sDd_mst,0,sizeof(struct dd_mst_c));
		g_reply_int=pub_base_currentdis_file(crd_no,1,&sDd_mst);		
		if (g_reply_int)
		{
			vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
			goto ErrExit;
		}
		set_zd_data("0250",sDd_mst.name);
		/** add by wyw 验证凭证是否挂失 20090504**/
		if (g_mdm_ac_rel.note_sts[0] != '0' &&  dc_ind[0] =='1')
		{
			vtcp_log("%s,%d,凭证非正常状态不能取款",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU41");
			goto ErrExit;
		}
		/* add by wyw 20090513 */
		if (dc_ind[0] =='1')
		{	
			if (sDd_mst.ac_sts[0] != '1')
			{
				vtcp_log("%s,%d,账户非正常状态不能取款!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"L016");
				goto ErrExit;
			}
			if (sDd_mst.hold_sts[0] == '1' )
			{
				vtcp_log("%s,%d,该账户完全冻结不能取款!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P110");
				goto ErrExit;
			}
			else if (sDd_mst.hold_sts[0] == '2' )
			{
				vtcp_log("%s,%d,该账户只进不出不能取款!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"O161");
				goto ErrExit;
			}
			else if (sDd_mst.hold_sts[0] == '3' && ysq_ind[0]=='0')
			{
				if (sDd_mst.bal-g_pub_tx.tx_amt1 < sDd_mst.hold_amt+sDd_mst.ctl_amt)
				{
					vtcp_log("%s,%d,当前余额不足!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"LS51");
					goto ErrExit;
				}
			}
		}
		/* add by wyw 20090513 */
	}
	/*若是卡则进行卡的基本检查*/
	if ((memcmp(crd_no,CARDHEADCODE,6) == 0 || memcmp(crd_no,ICCARDHEADCODE,6) == 0 ) && ysq_ind[0]=='0')  /*若是本行卡则进行合法行检查*/
	{
		/** 检查当天累计取款金额是否超限 */
		/**if(cz_ind[0] =='0' && atoi(fee_ind) <6 && atoi(fee_ind)>0)**/ /*冲正交易不检查限额*/
		if (cz_ind[0] =='0' && atoi(fee_ind)>0) /*冲正交易不检查限额*/
		{
			if (cx_ind == 1)
			{
				vtcp_log("%s,%d,撤销交易不检查cvn",__FILE__,__LINE__);
			}else
			{
				/** 网银业务不检查卡的CVN **/
				if (dc_ind[0]=='1' && memcmp(Yw_type,"010",3))/*向卡内存钱时不检查cvn*/
				{
					g_reply_int=pubf_card_chk_msr(1,crd_no); /*cvn检查*/
					if (g_reply_int)
					{
						sprintf(acErrMsg,"检查卡磁道或cvn失败!");
						WRITEMSG
							goto ErrExit;
					}
				}
			}
			if (dc_ind[0]=='1')/*取款业务限制金额*/
			{
				if (ret=Check_card_reg_limit(s_ct_ind,trade_type))
				{
					vtcp_log("%s,%d,限制检查错误[%d]",__FILE__,__LINE__,ret);
					goto ErrExit;
				}
				vtcp_log("%s,%d,卡号[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
			}
		}
		vtcp_log("%s,%d",__FILE__,__LINE__);
	}
	vtcp_log("%s,%d,卡号[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
	sprintf(acErrMsg,"交易类型= ysq_ind[%s] ,cz_ind[%s]",ysq_ind,cz_ind);
	WRITEMSG
	if (ysq_ind[0] == '0')
	{
		/*ATM存取款和POS消费类*/
		vtcp_log("%s,%d帐务处理类",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"0001");
	}
	else if (ysq_ind[0] == '1')
	{
		if (pub_base_CompDblVal(g_pub_tx.tx_amt1,10000000.00)>=0)
		{
			vtcp_log("%s,%d,金额超限",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU61");
			goto ErrExit;
		}
		/*POS预授权类(冻结)*/
		if (cz_ind[0] =='0') /*正常 spF036*/
		{
			vtcp_log("%s,%d预授权",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0002");
		}else if(cz_ind[0] == '1') 
		{  /*冲正 spF037*/
			vtcp_log("%s,%d预授权冲正",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0003");
		}
	}
	else if (ysq_ind[0] == '2')
	{
		/*POS预授权撤销(解冻)*/
		if (cz_ind[0] =='0') /*正常 spF038*/
		{
			vtcp_log("%s,%d预授权撤销",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0004");
		}else if(cz_ind[0] == '1') 
		{  /*冲正*/
			vtcp_log("%s,%d预授权撤销冲正",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0005");
		}
	}
	else if (ysq_ind[0] == '3')
	{
		/*POS预授权完成(解冻加扣款)*/
		if (cz_ind[0] =='0') /*正常*/
		{
			vtcp_log("%s,%d预授权完成",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0006");
		}else if(cz_ind[0] == '1') 
		{  /*冲正*/
			vtcp_log("%s,%d预授权完成冲正",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0007");
		}
	}
	else if (ysq_ind[0] == '4')
	{
		/*POS预授权完成撤销*/
		if (cz_ind[0] =='0') /*正常*/
		{
			vtcp_log("%s,%d预授权完成撤销",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0008");
		}else if(cz_ind[0] == '1') 
		{  /*冲正*/
			vtcp_log("%s,%d预授权完成撤销冲正",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"0009");
		}
	}else
	{
		vtcp_log("%s,%d未知交易类型",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU06");
	}
	OkExit:
	/*strcpy( g_pub_tx.reply, "0000" );*/
		sprintf(acErrMsg,"本行设备外行卡处理!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	vtcp_log("%s,%d,检查密码否[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	return 0;

	ErrExit:
	if (strlen(g_pub_tx.reply)==0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy( g_pub_tx.reply, "CU06" );
	}
	sprintf(acErrMsg,"卡业务处理失败! ");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Check_card_reg_limit(char *s_ct_ind,char *trade_type)
{
	/***************20090729*********************************
	int ret =0;
	struct card_reg_c sCard_reg;
	struct card_consum_lmt_c sCard_consum_lmt;
	struct mdm_ac_rel_c      sMdm_ac_rel;
	double max_amt = 0.0;
	long pass_cnt =0;
	int atm_qy_flag,atm_tx_flag,pos_qy_flag,pos_tx_flag;
	memset(&sCard_consum_lmt,0,sizeof(sCard_consum_lmt));
	memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
	memset(&sCard_reg, 0x0, sizeof(struct card_reg_c));	
	vtcp_log("%s,%d,limit分支!",__FILE__,__LINE__);
	ret = pub_card_reg(0.00,"GMM",'0',&atm_qy_flag,&pos_qy_flag,&atm_tx_flag,&pos_tx_flag,0,3);
	if(ret)
	{
	vtcp_log("%s,%d,更新卡登记簿错误!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU06");
	return(-1);
	}
	vtcp_log("%s,%d,limit分支!",__FILE__,__LINE__);
	ret = Card_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.crd_no);	
	if(ret)
	{
	vtcp_log("%s,%d,帐号不存在!ret[%d]",__FILE__,__LINE__,ret);
	strcpy(g_pub_tx.reply,"CU14");
	return(-1);
	}
	ret = Card_reg_Fet_Upd(&sCard_reg,g_pub_tx.reply);	
	if(ret)
	{
	vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU14");
	return(-1);
	}
	*ret = pub_base_GetParm_long("ATMMM",1,&pass_cnt);  开封用自己的参数*
	ret = pub_base_GetParm_long("KFCAR",2,&pass_cnt); 
	if(ret)
	{
	pass_cnt = 3;
	vtcp_log("%s,%d,密码次数不存在!",__FILE__,__LINE__);
	vtcp_log("%s,%d,密码次数不存在,取默认次数[%d]!",__FILE__,__LINE__,pass_cnt);
	}
	*检查交易密码*
	** 开封 wyw ，修改验证密码的函数 20090513
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	***
	* 若是当天ATM上超过3次则不在进行校验 wyw 20090513*
	vtcp_log("%s,%d,ATM标志[%s]pass_cnt[%d][%d]!",__FILE__,__LINE__,trade_type,pass_cnt,sCard_reg.add_pswd_cnt);
	if( g_pub_tx.tx_date == sCard_reg.l_tx_date && sCard_reg.add_pswd_cnt + 1 >pass_cnt && !memcmp(trade_type,"ATM",3))
	{
	vtcp_log("%s,%d,超过ATM的当日可取款次数!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU75");
	return(-1);
	}
	strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
	ret = pub_base_check_draw("101");
	if(ret)
	{
	vtcp_log("%s,%d",__FILE__,__LINE__);
	if(g_pub_tx.tx_date == sCard_reg.l_tx_date)
	{		
	sCard_reg.add_pswd_cnt = sCard_reg.add_pswd_cnt + 1;	
	}else{
	sCard_reg.add_pswd_cnt = 1;	
	}
	if((sCard_reg.add_pswd_cnt > pass_cnt) && !memcmp(trade_type,"ATM",3))
	{
	* 修改介质账号对照表
	ret  = sql_execute2("update mdm_ac_rel set coll_sts='2' where \ 
	ac_no = '%s' ", g_pub_tx.crd_no); 
	if (ret != 0) 
	{ 
	sprintf(acErrMsg, "更新mdm_ac_rel表错误!! [%d]", ret); 
	WRITEMSG 
	strcpy(g_pub_tx.reply,"AT04"); 
	Card_reg_Clo_Upd();
	return(-1);
	}  
	sCard_reg.add_pswd_cnt = 0;
	不设置吞卡标注提示超限* 

	* atm吞卡响应码 * 
	strcpy(g_pub_tx.reply,"CU38"); 
	}else{
	strcpy(g_pub_tx.reply,"CU55");
	}	
	sCard_reg.l_tx_date= g_pub_tx.tx_date;
	ret = Card_reg_Upd_Upd(sCard_reg,g_pub_tx.reply);
	if(ret)
	{
	*当日交易限额*
	vtcp_log("%s,%d,更新密码错误次数!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU14");
	Card_reg_Clo_Upd();
	return(-1);
	}
	Card_reg_Clo_Upd();
	ret = sql_commit(); 
	if (ret != 0) 
	{ 
	sprintf(acErrMsg,"提交事务错误!!"); 
	WRITEMSG 
	} 
	ret = sql_begin(); 
	if (ret != 0) 
	{ 
	sprintf(acErrMsg,"开始事务错误!!"); 
	WRITEMSG 
	} 
	return(-1);
	}
	Card_reg_Clo_Upd();
	if((sCard_reg.add_pswd_cnt > pass_cnt) && !memcmp(trade_type,"ATM",3))
	{
	vtcp_log("%s,%d,密码次数超限!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU38");
	return(-1);
	}

	ret = sql_execute2("update card_reg set add_pswd_cnt=0 where ac_no='%s'",g_pub_tx.crd_no);
	if(ret)
	{
	vtcp_log("%s,%d,更新更新卡登记簿失败![%d],ac_no[%s]",__FILE__,__LINE__,ret,g_pub_tx.crd_no);
	strcpy(g_pub_tx.reply,"CU06");
	return(-1);
	}
	if(memcmp(trade_type,"GMM",3)==0)
	{
	vtcp_log("%s,%d,若是柜面业务验证完密码后退出!",__FILE__,__LINE__);
	return(0);
	}
	if(s_ct_ind[0]=='1')
	{
	** 开封参数控制,每日取现金额两万 20090504 *
	ret = pub_base_GetParm_double("KFCAR",13,&max_amt);
	if(ret)
	{
	*单笔ATM转帐限额*
	max_amt=10000.00;
	vtcp_log("%s,%d,取累计取现金额错误!取默认值[%.2lf]",__FILE__,__LINE__,max_amt);
	}
	if(pub_base_CompDblVal(sCard_reg.atm_td_dr_cash_tx_amt + g_pub_tx.tx_amt1,max_amt) >0)
	{
	*当限额不为0.00,且发生额大额最大限额*
	vtcp_log("%s,%d,累计取现金额超限!,amt1[%lf],atm_td_dr_cash_tx_amt[%lf],max_amt[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
	strcpy(g_pub_tx.reply,"CU61");
	return(-1);
	}
	******add  by wyw 20090610 开封不现在取款次数，只是限制每日金额20000
	if(sCard_reg.atm_td_dr_cash_tx_cnt + 1 > sCard_reg.atm_max_dr_cash_tx_cnt && sCard_reg.atm_max_dr_cash_tx_cnt != 0)
	{
	*当日交易次数*
	vtcp_log("%s,%d,累计取款次数超限!max[%ld],cnt[%ld]",__FILE__,__LINE__,sCard_reg.atm_max_dr_cash_tx_cnt,sCard_reg.atm_td_dr_cash_tx_cnt);
	strcpy(g_pub_tx.reply,"CU65");
	return(-1);
	}
	****
	}else if(s_ct_ind[0]=='2' && !memcmp(trade_type,"ATM",3)){ *POS消费没有限制*
	* 开封要求ATM 每日转账限额 1万 ,20090504*
	ret = pub_base_GetParm_double("KFCAR",15,&max_amt);
	if(ret)
	{
	*单笔ATM转帐限额*
	max_amt=10000.00;
	vtcp_log("%s,%d,取累计取现金额错误!取默认值[%.2lf]",__FILE__,__LINE__,max_amt);
	return(-1);
	}
	if(pub_base_CompDblVal(sCard_reg.atm_td_dr_tran_tx_amt + g_pub_tx.tx_amt1,max_amt) >0)
	{
	*当限额不为0.00,且发生额大额最大限额*
	vtcp_log("%s,%d,累计取现金额超限!,amt1[%lf],atm_td_dr_cash_tx_amt[%lf],max_amt[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
	strcpy(g_pub_tx.reply,"CU61");
	return(-1);
	}
	ret = pub_base_GetParm_double("ATMZZ",1,&max_amt);
	if(ret)
	{
	*单笔ATM转帐限额*
	max_amt=50000.00;
	vtcp_log("%s,%d,取累计取现金额错误!取默认值[%.2lf]",__FILE__,__LINE__,max_amt);
	*strcpy(g_pub_tx.reply,"CU61");
	return(-1);
	*
	}
	if(pub_base_CompDblVal(g_pub_tx.tx_amt1,max_amt) >0)
	{
	*当日交易限额*
	vtcp_log("%s,%d,交易金额超限!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU61");
	return(-1);
	}
	}
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.crd_no);
	if(ret)
	{
	vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU14");
	return(-1);
	}
	if(sMdm_ac_rel.main_ind[0]=='2')
	{
	vtcp_log("%s,%d,该卡为副卡，检查卡限额登记簿!",__FILE__,__LINE__);
	ret = Card_consum_lmt_Sel(g_pub_tx.reply,&sCard_consum_lmt,"ac_no='%s'",g_pub_tx.crd_no);
	if(ret)
	{
	vtcp_log("%s,%d,该卡为副卡，未做卡限额维护，不能取款!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU62");
	return(-1);
	}
	*卡每日取现金额*
	if(s_ct_ind[0]=='1')
	{
	if(pub_base_CompDblVal(sCard_reg.atm_td_dr_cash_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.da_cash_lmt)==1 || pub_base_CompDblVal(sCard_reg.atm_tm_dr_cash_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.mo_cash_lmt)==1)
	{
	vtcp_log("%s,%d,超出此卡每日/月取现金额!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU62");
	return(-1);
	}
	}else if(s_ct_ind[0]=='2' && !memcmp(trade_type,"ATM",3)){
	*副卡转帐限额*
	if(pub_base_CompDblVal(sCard_reg.atm_td_dr_tran_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.da_amt_lmt)==1 || pub_base_CompDblVal(sCard_reg.atm_tm_dr_tran_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.mo_amt_lmt)==1)
	{
	vtcp_log("%s,%d,超出此卡每日/月转帐金额!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU62");
	return(-1);
	}

	}else if(!memcmp(trade_type,"POS",3))
	{
	*副卡消费金额限制*
	if(pub_base_CompDblVal(sCard_reg.pos_td_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.da_con_lmt)==1 || pub_base_CompDblVal(sCard_reg.pos_tm_tx_amt+g_pub_tx.tx_amt1,sCard_consum_lmt.mo_con_lmt)==1)
	{
	vtcp_log("%s,%d,超出此卡每日/月消费金额!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU62");
	return(-1);
	}
	}
	return(0);
	}
	********************************************************/
	return(0);
}

