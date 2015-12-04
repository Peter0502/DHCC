/***************************************************************
* 文 件 名:     pubf_acct_opn_ac.c
* 功能描述：	开账号
*              	1、生成账户序号；
*              	2、登记产品账号对照表；
*              	3、登记主文件；
*              	4、登记开销户登记簿；
*              	5、登记交易流水；
*
* 作    者:     xxx
* 完成日期：    2003年1月6日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ac_seqn_id_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "cif_prdt_rel_c.h"
#include "com_item_c.h"
#include "ac_ac_rel_c.h"

char cif_type;

pub_acct_opn_ac()
{
	int ret;

/*	char cif_type[2];
*/
	struct ac_seqn_id_c	ac_seqn_id_c;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	mdm_attr_c;
	struct dd_mst_c	v_dd_mst;
	struct cif_prdt_rel_c	g_cif_prdt_rel;
	memset(&ac_seqn_id_c,0x00,sizeof(struct ac_seqn_id_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_prdt_rel,0x00,sizeof(struct cif_prdt_rel_c));
	ret=0;

	/***介质账号检查***/
	ret=pub_sub_init();
	if( ret ) goto ErrExit;
	
	sprintf(acErrMsg,"账号[%s]",g_pub_tx.ac_no);
	WRITEMSG	/**取介质账号表**/
	
	ret=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",
		g_pub_tx.ac_no);
	if( ret ) 
	{
		sprintf(acErrMsg,"取介质账号对照表错[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s %d ",__FILE__,__LINE__);
	ret=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"显示账号不存在![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( ret ) 
	{ 
		sprintf(acErrMsg," 取显示账号出错![%s][%d]",g_pub_tx.ac_no,ret);
		WRITEMSG
		goto ErrExit; 
	}

	ret=pub_sub_chk_mdm();
	if( ret )
	{
		sprintf(acErrMsg," ------检查介质出错------ " );
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

	/*** 读取介质属性 ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s'", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该介质属性不存在![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取介质属性异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** Add By xxx **/
	/*** 多账户介质判断状态 ***/
	if( mdm_attr_c.ac_num_ind[0]=='N' )
	{
		if( g_mdm_ac_rel.main_ind[0]!='1' && mdm_attr_c.mdm_type[0]=='3' )
		{
			sprintf(acErrMsg,"副卡不允许卡下开户!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D220");
			goto ErrExit;
		}

		if( g_mdm_ac_rel.coll_sts[0]=='1' )
		{
			if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
			{
				sprintf(acErrMsg,"调用捡拾检查函数出错");
				WRITEMSG
				goto ErrExit;
			}
		}

		switch( g_mdm_ac_rel.note_sts[0] )
		{
			case '1':
				sprintf(acErrMsg,"该介质已挂失!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M009");
				goto ErrExit;
			case '2':
				sprintf(acErrMsg,"该介质已挂失换证!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P171");
				goto ErrExit;
			case '3':
				sprintf(acErrMsg,"该介质已作换证处理!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M022");
				goto ErrExit;
			case '4':
				sprintf(acErrMsg,"该介质已被部提!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O217");
				goto ErrExit;
			case '5':
				sprintf(acErrMsg,"该介质已撤销!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M023");
				goto ErrExit;
			case '*':
				sprintf(acErrMsg,"该介质已销!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P172");
				goto ErrExit;
			default:
				break;
		}
	}

	/***定期一本通打印行数加3***/
	if( !strncmp(mdm_attr_c.mdm_code,"0016",4) && !strncmp(g_pub_tx.tx_code,"2104",4) )
	{	
	vtcp_log("aaaaaaaaa[%d]\n",g_mdm_ac_rel.prt_line);
	g_mdm_ac_rel.prt_line+=3;
	vtcp_log("aaaaaaaaaa[%d]\n",g_mdm_ac_rel.prt_line);
	}
	/****
	else if( !strncmp(mdm_attr_c.mdm_code,"0016",4) )
	{	
	vtcp_log("bbbbbbbbb[%d]\n",g_mdm_ac_rel.prt_line);
	g_mdm_ac_rel.prt_line++;
	vtcp_log("bbbbbbbbb[%d]\n",g_mdm_ac_rel,prt_line);
	}
***/
	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.beg_note_no,g_mdm_ac_rel.note_no);
	strcpy(g_pub_tx.end_note_no,g_mdm_ac_rel.note_no);

	if(mdm_attr_c.ac_num_ind[0]=='Y')
	{
		g_reply_int= Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
									g_pub_tx.ac_id);
		if( !g_reply_int )
		{
			sprintf(acErrMsg,"该介质只允许开一个账户![%d]",g_reply_int);
			WRITEMSG
			sprintf(acErrMsg,"ac_id[%d]",g_pub_tx.ac_id);
			WRITEMSG
						
			strcpy(g_pub_tx.reply,"P244");
			goto ErrExit;
		}
		else if( g_reply_int&&g_reply_int!=100 )
		{
			sprintf(acErrMsg,"查找错误![%d]",g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}
	/***FINISH***/

	/*** 生成账户序号 ***/
	if( g_pub_tx.prdt_code[0]=='1' ) /**活期，每户仅一个活期**/
	{
		ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
		if( ret ) 
		{
			sprintf(acErrMsg,"转序号出错 1 " );
			WRITEMSG
			goto ErrExit;
		}
		ret=Dd_mst_Sel(g_pub_tx.reply,&v_dd_mst,
			"ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn );
		if( ret && ret!=100 ) goto ErrExit;
		else if( ret==0 )
		{
			sprintf(acErrMsg,"账号已经存在id[%ld]seqn[%d]!ret[%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P164" );
			goto ErrExit;
		}
		if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN ) /**非多户介质使用序号**/
		{
			g_mdm_ac_rel.ac_seqn=g_pub_tx.ac_seqn;
		}
	}
	else if( g_mdm_ac_rel.ac_seqn==ALL_SEQN ) /**多户介质使用序号**/
	{
		ret=pub_base_CrtAcSeqn( g_pub_tx.ac_id , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"账号序号ID表异常错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P140");
			goto ErrExit;
		}
		/***定期一本通每个存折最多可开26个子帐户，超过则开新定期一本通帐户***/
		if(!strcmp(mdm_attr_c.mdm_code, "0016") && g_pub_tx.ac_seqn == 226)
		{
			sprintf(acErrMsg,"定期一本通存折已超过最大开户数，请开新一本通帐户!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P510");
			goto ErrExit;
		}
	}
	else if( ac_seqn_id_c.ac_seqn_id!=ZERO_SEQN ) /**单介质统一序号**/
	{
		g_pub_tx.ac_seqn=SINGLE_SEQN;
		g_mdm_ac_rel.ac_seqn=g_pub_tx.ac_seqn;
	}
	else
	{
		sprintf(acErrMsg,"该介质不允许开多个账户![%d]",ac_seqn_id_c.ac_seqn_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P143");
		goto ErrExit;
	}

	if(mdm_attr_c.ac_num_ind[0]=='Y')
		g_mdm_ac_rel.book_bal=g_pub_tx.tx_amt1;
	else
		g_mdm_ac_rel.book_bal=0.00;
	ret=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret ) 
	{
		sprintf(acErrMsg,"修改介质账号对照表出错" );
		WRITEMSG
		goto ErrExit;
	}

	ret=Mdm_ac_rel_Clo_Upd();
	if( ret ) 
		goto ErrExit;

	sprintf(acErrMsg,"生成了序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG
	
	/*** 登记产品账户对照表 ***/
	/****已经改为视图**
	prdt_ac_id_c.ac_id=g_pub_tx.ac_id;
	prdt_ac_id_c.ac_seqn=g_pub_tx.ac_seqn;
	strncpy(prdt_ac_id_c.ac_id_type,g_pub_tx.prdt_code,1);
	strcpy(prdt_ac_id_c.prdt_no,g_pub_tx.prdt_code);
	ret=Prdt_ac_id_Ins(prdt_ac_id_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记产品账户ID对照表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	*********/

	/*** 登记主文件 ***
	if( g_pub_tx.add_ind[0]!='1' )
	{
		sprintf(acErrMsg,"存取标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}
	*/

	/**** ADD BY xxx.05.03.23 ****/
	/**** 登记客户产品关系表 ****/
	if( g_pub_tx.prdt_code[0]=='1'||g_pub_tx.prdt_code[0]=='2'|| \
		g_pub_tx.prdt_code[0]=='3' )
	{
		g_cif_prdt_rel.cif_no=g_pub_tx.cif_no;
		strcpy(g_cif_prdt_rel.prdt_code,g_pub_tx.prdt_code);
		g_cif_prdt_rel.ac_id=g_pub_tx.ac_id;
		strcpy(g_cif_prdt_rel.relation_no,"1");			/*关系代码1-户主*/
		strcpy(g_cif_prdt_rel.relation_flag,"1");		/*状态1-正常2-解除*/
		g_cif_prdt_rel.addr_seri=0;						/*账单地址*/
		g_cif_prdt_rel.crt_date=g_pub_tx.tx_date;		/*建立日期*/
		g_cif_prdt_rel.cls_date=0;						/*解除日期*/

		ret = Cif_prdt_rel_Ins(g_cif_prdt_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg, "Insert cif_prdt_rel error !! [%d]", ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "C080" );
			goto ErrExit;
		} 
	}
	/****FINISH****/

	switch(g_pub_tx.prdt_code[0])
	{
		case '1' :	
			ret=pub_ins_dd_mst();
			if( ret )
			{
				sprintf(acErrMsg,"登记活期主文件错误![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '2' :	
			ret=pub_ins_td_mst();
			if( ret )
			{
				sprintf(acErrMsg,"登记定期主文件错误![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '3' :	
			ret=pub_ins_ln_mst();
			if( ret )
			{
				sprintf(acErrMsg,"登记贷款主文件错误![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '9' :	
			ret=pub_ins_in_mst();
			if( ret )
			{
				sprintf(acErrMsg,"登记内部主文件错误![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		default  :	
			sprintf(acErrMsg,"账户类型错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
	}
	if((memcmp(g_pub_tx.tx_code,"2104",4)==0)&&strlen(g_mdm_ac_rel.ac_no)==13)
	{
	vtcp_log("-=========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
		ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
		if(ret)
		{
			sprintf(acErrMsg,"ERROR");
			WRITEMSG 
			goto ErrExit;
		}
	}
/*** 补登折处理 ***/
	if(g_pub_tx.prt_ind[0]=='1')
	{
	ret=pub_base_patch_book(mdm_attr_c);
		if( ret<0 )
		{
			sprintf(acErrMsg,"补登折错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*** 登记交易流水 ***/
	if( g_pub_tx.prdt_code[0]!='3' )
	{
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"登记交易流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开账户成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开账户失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_ins_dd_mst()
{
	int ret;
	double rate_val;

	g_dd_mst.ac_id=g_pub_tx.ac_id;
	vtcp_log("-==========ac_type=[%s]",g_dd_mst.ac_type);
	g_dd_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_dd_mst.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(g_dd_mst.prdt_no,g_pub_tx.prdt_code);
	vtcp_log("-==========g_pub_tx.prdt_code=[%s]",g_pub_tx.prdt_code);
	g_dd_mst.bal=g_pub_tx.tx_amt1;
	g_dd_mst.acbo_bal=g_pub_tx.tx_amt1;
	g_dd_mst.ys_bal=0.00;	
	g_dd_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_dd_mst.rate=g_pub_tx.rate;
	g_dd_mst.flt_ratio=0.00; /**谁加的??**/
	g_dd_mst.opn_date=g_pub_tx.tx_date;
	g_dd_mst.ic_date=g_pub_tx.tx_date;
	g_dd_mst.lst_date=g_pub_tx.tx_date;

	/***Add by xxx***/
	/***对公基本户可取现、对公一般户不能取现***/
	if( g_dd_parm.cif_type[0]=='1' || g_pub_tx.ac_type[0]=='1' )
		g_dd_mst.cash_ind[0]='Y';
	else if(g_dd_parm.cif_type[0]!='1' && g_pub_tx.ac_type[0]=='2')
		g_dd_mst.cash_ind[0]='N';
	else 
		g_dd_mst.cash_ind[0]=g_dd_parm.cash_ind[0];
	/***Finish***/

	g_dd_mst.odtt_ind[0]='0';
	strcpy( g_dd_mst.cal_code,g_pub_tx.cal_code );
/****for xxxxxxxx****/
	if( !strncmp(g_dd_mst.prdt_no,"142",3) )
		strcpy( g_dd_mst.cal_code,"61099" );
	else if( cif_type=='1' || cif_type=='5' )
		g_dd_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_dd_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_dd_mst.cal_code[0]='3';
	else 
		g_dd_mst.cal_code[0]='2';


	/***参数检查**/
	/*检查开户日期是否在起用日期和终止日期之间*/	
	if ( (g_dd_parm.beg_date > g_dd_mst.opn_date) || \
		(g_dd_mst.opn_date > g_dd_parm.end_date))
	{	
		sprintf(acErrMsg,"超有效期[%d]not in[%d][%d]",
			g_dd_mst.opn_date,g_dd_parm.beg_date,g_dd_parm.end_date);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A001" );
		goto ErrExit;
	}

	if( (g_dd_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
		(g_dd_parm.cif_type[0]=='2' 
			&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_dd_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
	{
		sprintf( acErrMsg,"客户类型不符parm[%s] [%c]",
			g_dd_parm.cif_type,cif_type );
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D112" );
		goto ErrExit;
	}

	/**账户类型**/
	if( g_pub_tx.ac_type[0]=='S' || g_pub_tx.ac_type[0]=='D' )
	{
		strcpy(g_dd_mst.sttl_type,g_pub_tx.ac_type);
		strcpy(g_dd_mst.ac_type,"5");
		vtcp_log("-=666666==g_dd_mst.ac_type=[%s]",g_dd_mst.ac_type);
		
	}
	else
	{
		strcpy(g_dd_mst.ac_type,g_pub_tx.ac_type);
		strcpy(g_dd_mst.sttl_type,"D");
	}

	/*检查该产品最低开户金额和最高开户金额*/
	if ( g_pub_tx.ac_wrk_ind[4]=='1' && 
		pub_base_CompDblVal(g_dd_mst.bal,0.00)==0 )
	{
		;
	}
	else if ( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.min_opn_amt)<0 )
	{
		sprintf(acErrMsg,"low [%.2lf]<[%.2lf]! ",
			g_dd_mst.bal,g_dd_parm.min_opn_amt);
		WRITEMSG
		if( pub_base_CompDblVal(g_dd_mst.bal,0.00)==0 )
		{
			sprintf(acErrMsg,"活期可以零金额开户");
			WRITEMSG
		}
		else
		{
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
	}
	else if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_opn_amt)>0
		&& pub_base_CompDblVal(g_dd_parm.max_opn_amt,0.00)!=0 )
	{
		sprintf(acErrMsg,"open bal overflow!! ");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A004" );
		goto ErrExit;
	}

	/*检查开户通兑标志,通开只业务机构级，于会计机构无关*/
	if ( g_dd_parm.thr_opn_ind[0]=='N' && 
		strcmp(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no ) )
	{
		sprintf(acErrMsg,"thr_opn_ind is N,open failed!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A002" );
		goto ErrExit;
	}

	/*检查利率是否超过规定利率上浮和下浮后的界限*/
	/***取利率编号对应的利率***/
	ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,
		g_pub_tx.tx_date,&rate_val);
	if( ret ) goto ErrExit;

	if( g_dd_parm.rate_type[0]=='0' && g_pub_tx.ac_type[0]!='6' )/**挂牌利率**/
		g_dd_mst.rate=rate_val;
	else if( g_dd_parm.rate_type[0]=='2' || g_pub_tx.ac_type[0]=='6' ) /**手输利率(协定产品)**/
		g_dd_mst.rate=g_pub_tx.rate;
	else /**手输利率检查上浮下浮比率**/
	{
		if( pub_base_CompDblVal(g_dd_mst.rate,
			rate_val*(1+g_dd_parm.min_flt_ratio) )>0 )
		{
			sprintf(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_dd_mst.rate,
			rate_val*(1-g_dd_parm.min_flt_ratio) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
	}

	/* 检查开户限制条件编码 (调用函数) *
	g_dd_parm.opn_lmt */

	/**参数检查完成**/

	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_dd_parm.intst_type);
	}
	strcpy(g_dd_mst.intst_type,g_pub_tx.intst_type);

	/*** 计算利息积数的函数 ***/
	g_dd_mst.intst_acm=0.00;
	if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
		g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
		0.00,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
		g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
	{
		sprintf(acErrMsg,"计算利息积数错误!");
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"intst_acm=[%lf]",g_dd_mst.intst_acm);
WRITEMSG
vtcp_log("-==========ac_type=[%s]",g_dd_mst.ac_type);

	g_dd_mst.hst_cnt=0;
	g_dd_mst.hst_pg=0;
	/** 钞汇标志付值 **/
	if( g_dd_parm.opn_aplic_ind[0]=='Y' && \
		(g_dd_mst.ac_type[0]=='1' || g_dd_mst.ac_type[0]=='2' || \
		 g_dd_mst.ac_type[0]=='3' || g_dd_mst.ac_type[0]=='4') )
			strcpy(g_dd_mst.ac_sts,"0");
	else
		strcpy(g_dd_mst.ac_sts,"1");
	if(memcmp(g_dd_parm.prdt_no,"145",3)==0)/***验资户 2007-6-8 9:32*/
	{
		strcpy(g_dd_mst.ac_sts,"1");
	}
	strcpy(g_dd_mst.hold_sts,"0");
	g_dd_mst.hold_amt=0.00;
	g_dd_mst.ctl_amt=0.00;
	strcpy(g_dd_mst.od_ind,g_dd_parm.od_aplic_ind);	
	strcpy(g_dd_mst.draw_pwd,"N");

	g_dd_mst.cif_no=g_pub_tx.cif_no;
	strcpy(g_dd_mst.name,g_pub_tx.name);
	/**使对公基本户已经销户的客户,可以再开基本户 modify by wanglei 20061120**/
	if( g_pub_tx.ac_type[0]=='1' )
	{
		ret=sql_count("dd_mst","cif_no=%ld and ac_type='1' and ac_sts!='*'",g_pub_tx.cif_no,g_dd_mst.ac_sts);
		if( ret )
		{
			sprintf(acErrMsg,"该客户已存在基本账户[%ld][%d]", \
					g_pub_tx.cif_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C082");
			goto ErrExit;
		}
	}

	ret=pub_ins_dd_hst();
	if( ret )
	{
		sprintf(acErrMsg,"登记活期明细文件错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"金额为:[%lf]",g_dd_mst.bal);
WRITEMSG

	ret=Dd_mst_Ins(g_dd_mst,g_pub_tx.reply);	
	vtcp_log("debug begin\n");
		 Dd_mst_Debug(&g_dd_mst);
	if( ret )
	{
		sprintf(acErrMsg,"登记活期产品主文件错误![%d]",ret);
		WRITEMSG
		 Dd_mst_Debug(&g_dd_mst);
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	/*** 登记开销户登记簿 ***/
	if( pub_base_InsMo_opn_cls(g_dd_mst.name,"","1") )
	{
		sprintf(acErrMsg,"登记开销户登记簿错误!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_td_mst()
{
	int ret;
    int dedr_intvl;
	double rate_val;
	long l_term=0;
        struct ac_ac_rel_c      g_ac_ac_rel;
        struct mdm_ac_rel_c     t_mdm_ac_rel;
	memset(&g_ac_ac_rel,0x00,sizeof(struct ac_ac_rel_c));
	memset(&t_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

vtcp_log("INNNNNNNNNNNNNNNNNNNNNNNNN");
	/*** 前台只控制了单账户介质的金额 ***/
	/*** 储蓄定期10万以上必须开特种存单 ***/
	/*if( (!strcmp(g_td_parm.dc_code,"2151") || !strcmp(g_td_parm.dc_code,"2158"))
		&& strcmp(g_td_parm.prdt_no,"20C")
		&& pub_base_CompDblVal(g_pub_tx.tx_amt1,100000.00)>=0 
		&& strcmp(g_pub_tx.mdm_code,"0014") )
	{
		sprintf(acErrMsg,"储蓄整存整取超过十万元必须用储蓄特种存单![%s][%lf]%s",
			g_td_parm.prdt_no,g_pub_tx.tx_amt1,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "P419" );
		goto ErrExit;
	}
*/
	/*** 储蓄定期10万以下不得用特种存单 ***/
	/*if( (!strcmp(g_td_parm.dc_code,"2151") || !strcmp(g_td_parm.dc_code,"2158"))
		&& strcmp(g_td_parm.prdt_no,"20C")
		&& pub_base_CompDblVal(g_pub_tx.tx_amt1,100000.00)<0 
		&& !strcmp(g_pub_tx.mdm_code,"0014") )
	{
		sprintf(acErrMsg,"储蓄整存整取小于十万元不得用储蓄特种存单![%s][%lf]%s",
			g_td_parm.prdt_no,g_pub_tx.tx_amt1,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "P423" );
		goto ErrExit;
	}*/

	g_td_mst.ac_id=g_pub_tx.ac_id;
	g_td_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_td_mst.opn_br_no,g_pub_tx.opn_br_no);
	pub_base_strpack(g_td_parm.br_no);
	if( strlen(g_td_parm.br_no)==5 )
	{
		strcpy(g_td_mst.opn_br_no,g_td_parm.br_no);
		strcpy(g_pub_tx.opn_br_no,g_td_parm.br_no);
	}
	strcpy(g_td_mst.prdt_no,g_pub_tx.prdt_code);
	g_td_mst.bal=g_pub_tx.tx_amt1;
	g_td_mst.acbo_bal=g_pub_tx.tx_amt1;
	g_td_mst.ys_bal=0.00;	
	g_td_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_td_mst.rate=0.00;
	g_td_mst.flt_ratio=0.00;

	g_td_mst.opn_date=g_pub_tx.tx_date;
	g_td_mst.ic_date=g_pub_tx.tx_date;
	g_td_mst.lst_date=g_pub_tx.tx_date;
	g_td_mst.book_line=g_mdm_ac_rel.prt_line;

		/**计算到期日期**/
	if(memcmp(g_td_parm.prdt_no,"25A",3)==0)/*晋中协定同业存款定期 add by martin 2009/7/22 10:59:38*/
		{
			/*定期直接复制到期日*/
		}
	else
		{/* begin by martin 2009/7/22 11:00:30*/
	if (g_td_parm.td_type[0] != '4')
	{
		if( g_td_parm.term_type[0]=='D' )
		{
			l_term=g_td_parm.term;
			pub_base_deadlineD(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
		}
		if( g_td_parm.term_type[0]=='Y' )
		{
			g_td_parm.term_type[0]='M';
			g_td_parm.term*=12;
		}
		if( g_td_parm.term_type[0]=='M' )
		{
			l_term=g_td_parm.term;
			pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
		}
	}
	else
	{
		g_td_mst.mtr_date = 0;
	}
   }/* end by martin 2009/7/22 11:00:30*/
	g_pub_tx.mach_date=g_td_mst.mtr_date;

	sprintf(acErrMsg,"term_type[%s] term[%d][%d]opn[%d]mtr[%d]",
		g_td_parm.term_type,l_term,g_td_parm.term,
		g_td_mst.opn_date,g_td_mst.mtr_date);
	WRITEMSG

	/***参数检查**/
		/*检查开户日期是否在起用日期和终止日期之间*/	
		if ( (g_td_parm.beg_date > g_td_mst.opn_date) || \
			(g_td_mst.opn_date > g_td_parm.end_date))
		{	
			sprintf(acErrMsg,"超有效期[%d]not in[%d][%d]",
				g_td_mst.opn_date,g_td_parm.beg_date,g_td_parm.end_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}
TRACE
		if( (g_td_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
			(g_td_parm.cif_type[0]=='2' 
				&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_td_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
		{
			sprintf( acErrMsg,"客户类型不符parm[%s] [%c]",
				g_td_parm.cif_type,cif_type );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D112" );
			goto ErrExit;
		}

TRACE
		/*检查该产品最低开户金额和最高开户金额*/
		if ( g_pub_tx.ac_wrk_ind[4]=='1' && 
			pub_base_CompDblVal(g_td_mst.bal,0.00)==0 )
		{
			;
		}
TRACE
		if ( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_opn_amt)<0 )
		{
			sprintf(acErrMsg,"open bal is too low!!  \
			g_td_mst.bal==================================[%.2lf], \
			g_td_parm.min_opn_amt=========================[%.2lf]", \
			g_td_mst.bal,g_td_parm.min_opn_amt);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_opn_amt)>0
			&& pub_base_CompDblVal(g_td_parm.max_opn_amt,0.00)!=0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}

TRACE
		/*检查开户通兑标志,通开只业务机构级，于会计机构无关*/
		if ( g_td_parm.thr_opn_ind[0]=='N' && 
			strcmp(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no ) )
		{
			sprintf(acErrMsg,"thr_opn_ind is N,open failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A002" );
			goto ErrExit;
		}

TRACE
		/*检查利率是否超过规定利率上浮和下浮后的界限*/
		/***取利率编号对应的利率***/
		ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		if( g_td_parm.rate_type[0]=='0' ) /**挂牌利率**/
			g_td_mst.rate=rate_val;
		else /**手输利率检查上浮下浮比率**/
		{
			g_td_mst.rate=g_pub_tx.rate;
			vtcp_log("jack -- look g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);

			/*if( pub_base_CompDblVal(g_td_mst.rate,
				rate_val*(1+g_td_parm.min_flt_ratio) )>0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_td_mst.rate,
				rate_val*(1-g_td_parm.min_flt_ratio) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! \
						g_td_mst.rate=====================[%.2lf], \
						rate_val==========================[%.2lf]", \
						g_td_mst.rate, rate_val);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}*/
		}

TRACE
		/* 检查开户限制条件编码 (调用函数) *
		g_td_parm.opn_lmt */

	/**参数检查完成**/
    if (g_td_parm.td_type[0]=='1' || g_td_parm.td_type[0]=='2'
        || g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0]=='7' )
    {
        /* 零存整取 整存零取 存本取息 计算总发生次数 */
        if (g_td_parm.intvl_type[0] == 'Y')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 12;
        }
        else if (g_td_parm.intvl_type[0] == 'Q')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 4;
        } 
        else if (g_td_parm.intvl_type[0] == 'M')
        {
            dedr_intvl=g_td_parm.dedr_intvl;
        }
        else if (g_td_parm.intvl_type[0] == 'D')
        {
            /* 只考虑了月的整数倍 */
            dedr_intvl = g_td_parm.dedr_intvl / 30;
        }
        else 
        {
            sprintf(acErrMsg,"定期产品表中的间隔类型错误!!");
            WRITEMSG
            goto ErrExit;
        }

        if (g_td_parm.term_type[0] == 'Y')
        {
            g_td_mst.ttl_cnt = g_td_parm.term * 12 / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'M')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'D')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / 30 / dedr_intvl;
        }
    }
    
  /*** begin.gyf.20060918 因为整存零取计算利息需要积数，所以下面计算积数模块提前 **/
	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);
	}
	strcpy(g_td_mst.intst_type,g_pub_tx.intst_type);

	/* 计算利息积数的函数 NEED算积数需要改进：因为不同方式要求不同日期传入 */
	g_td_mst.intst_acm=0.00;
TRACE
    if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' ||
		g_td_parm.td_type[0] == '7' )
    /* 零存整取 整存零取 */
    {
		if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}
    }
   /*** end.gyf.20060918 ****/
    
	/**账户类型**/
	switch( g_td_parm.td_type[0] )
	{
		case '1': /*零存整取*/
		case '7': /*教育储蓄*/
			g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
            sprintf(acErrMsg,"tx_cnt=[%d] ",g_td_mst.tx_cnt);
            WRITEMSG
			break;
		case '2': /*整存零取*/
			/**NEED 需要计算**/
			      g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
            
            /* begin.gyf.20060918 */
            /* 计算利息 */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算存本取息利息错误!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P220");
                goto ErrExit;
            }
            sprintf(acErrMsg,"存本取息利息为:[%lf]",g_pub_intst.factval);
            WRITEMSG
            g_td_mst.td_amt+=g_pub_intst.factval ;
            g_td_mst.td_amt/=g_td_mst.ttl_cnt;
            break;
            /* end.gyf.20060918 */
		case '3': /*存本取息*/

			/**NEED 需要计算**/
            /* 计算利息 */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算存本取息利息错误!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P220");
                goto ErrExit;
            }
            sprintf(acErrMsg,"存本取息利息为:[%lf]",g_pub_intst.factval);
            WRITEMSG
			g_td_mst.td_amt=g_pub_intst.factval / g_td_mst.ttl_cnt;

			break;
		default :
			g_td_mst.td_amt=0.00;
			break;
	}

  /*** begin.gyf.20060918 因为整存零取计算利息需要积数，所以下面计算积数模块提前 
	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);
	}
	strcpy(g_td_mst.intst_type,g_pub_tx.intst_type);

	/ 计算利息积数的函数 NEED算积数需要改进：因为不同方式要求不同日期传入/
	g_td_mst.intst_acm=0.00;
TRACE
    if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' ||
		g_td_parm.td_type[0] == '7' )
    /零存整取 整存零取 /
    {
		if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}
    }
    end.gyf.20060918 ****/

	g_td_mst.hst_cnt=0;
	/** 钞汇标志付值 **/
	strcpy(g_td_mst.ac_sts,"1");
	strcpy(g_td_mst.hold_sts,"0");
	g_td_mst.hold_amt=0.00;
	g_td_mst.ctl_amt=0.00;
	strcpy(g_td_mst.tfr_ind,g_pub_tx.tmp_ind);
	g_td_mst.cif_no=g_pub_tx.cif_no;
	strcpy(g_td_mst.name,g_pub_tx.name);
/****for xxxxxxxx****/
	if( cif_type=='1' || cif_type=='5' )
		g_td_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_td_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_td_mst.cal_code[0]='3';
	else 
		g_td_mst.cal_code[0]='2';

vtcp_log("INNNNNNNNNNNNNNNNNNNNNNNNN");
TRACE
		ret=pub_ins_td_hst();
		if( ret )
		{
			sprintf(acErrMsg,"登记定期明细文件错误![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

TRACE
	ret=Td_mst_Ins(g_td_mst,g_pub_tx.reply);	
	if( ret )
	{
		sprintf(acErrMsg,"登记定期产品主文件错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
        /***********如果为协定存款帐号需关联一个对公活期户 add by ligl 20060915**************/
        if(g_td_parm.td_type[0]=='5')/*协议存款*/
        {
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
          	
          	ret=Mdm_ac_rel_Sel(&g_pub_tx.reply,&t_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no1);
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%ld][%s][%d]",t_mdm_ac_rel.ac_id,__FILE__,__LINE__);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"介质帐号关系表不存在该记录![%d][%s]",ret,g_pub_tx.ac_no1);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B117");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"取介质帐号关系表异常![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		}
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
		g_ac_ac_rel.ac_id=g_td_mst.ac_id;
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
		g_ac_ac_rel.ac_id1=t_mdm_ac_rel.ac_id;
		ret=Ac_ac_rel_Ins(g_ac_ac_rel,g_pub_tx.reply);	
		if( ret )
		{
			sprintf(acErrMsg,"登记帐户关联表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P144");
			goto ErrExit;
		}	
        }
        /******************end******************/
	/*** 登记开销户登记簿 ***/
	if( pub_base_InsMo_opn_cls(g_td_mst.name,"","1") )
	{
		sprintf(acErrMsg,"登记开销户登记簿错误!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

/*****贷款分户处理*************/
int pub_ins_ln_mst()
{
	int ret;
	int pay_ac_seqn;
	long cif_no,ac_id;
	char v_cif_type[2];
	double rate_val,over_rate_val,fine_rate_val;
	double min_rate , max_rate , rate;
	long min_day , max_day ;
	
	g_ln_mst.ac_id=g_pub_tx.ac_id;
	g_ln_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_ln_mst.opn_br_no,g_pub_tx.opn_br_no); 	/**开户机构**/
	strcpy(g_ln_mst.prdt_no,g_pub_tx.prdt_code);
	/*g_ln_mst.amt_lmt			*贷款限额*/
	g_ln_mst.bal=g_pub_tx.tx_amt1;
	g_ln_mst.ys_bal=0.00;	
	g_ln_mst.hst_bal=0.00;	
	g_ln_mst.ttl_prvd_amt=g_pub_tx.tx_amt1; /*累计发放金额*/

	/*g_ln_mst.rate;  利率*/
	/*g_ln_mst.over_rate;  逾期利率*/
	g_ln_mst.intst_acm=0.00;

	g_ln_mst.in_lo_intst=0.00;  /**欠息**/
	g_ln_mst.in_lo_acm=0.00;
	g_ln_mst.out_lo_intst=0.00;
	g_ln_mst.out_lo_acm=0.00;
	g_ln_mst.cmpd_lo_intst=0.00;
	g_ln_mst.cmpd_lo_acm=0.00;

	/**g_ln_mst.cal_code 指标体系代码付值 **/
/****for xxxxxxxx****/
	if( cif_type=='1' || cif_type=='5' )
		g_ln_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_ln_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_ln_mst.cal_code[0]='3';
	else 
		g_ln_mst.cal_code[0]='2';

	g_ln_mst.opn_date=g_pub_tx.tx_date;
	g_ln_mst.ic_date=g_pub_tx.tx_date;
	g_ln_mst.lst_date=g_pub_tx.tx_date;
	/**g_ln_mst.mtr_date ; 到期日期**/

	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_ln_parm.intst_type);
	}
	strcpy(g_ln_mst.intst_type,g_pub_tx.intst_type); 

    if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_ln_mst.repay_type,g_ln_parm.ln_pay_type);
	}
	g_ln_mst.hst_cnt=0;
	g_ln_mst.hst_pg=0;
	/** 还款账号付值 后边**/

	/** 展期付值 **/
	strcpy(g_ln_mst.exp_ind,"0");
	g_ln_mst.exp_rate=0.00;
	g_ln_mst.exp_mtr_date=0;

	strcpy(g_ln_mst.ac_sts,g_ln_parm.ac_sts);
	strcpy(g_ln_mst.five_sts,"1");

	g_ln_mst.cif_no=g_pub_tx.cif_no;  /** 客户号付值 **/
	strcpy(g_ln_mst.name,g_pub_tx.name);
	/**g_ln_mst.pact_no 合同号付值 **/
	strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );

		/* 检查 合同号 */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , 
					" pact_no='%s' and ac_sts!='9' " , g_ln_mst.pact_no );
		if( ret && ret!=100)
		{
			sprintf(acErrMsg,"读取贷款主文件信息异常![%s][%s]",g_ln_mst.pact_no,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0 )
		{
			sprintf(acErrMsg,"合同号已存在![%s][%s]",g_ln_mst.pact_no,g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L020");
			goto ErrExit;
		}

	/***参数检查**/
		/*检查开户日期是否在起用日期和终止日期之间*/	
		if ( (g_ln_parm.beg_date > g_ln_mst.opn_date) || \
			(g_ln_mst.opn_date > g_ln_parm.end_date))
		{	
			sprintf(acErrMsg,"超有效期[%d]not in[%d][%d]",
				g_ln_mst.opn_date,g_ln_parm.beg_date,g_ln_parm.end_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}

		if( (g_ln_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
			(g_ln_parm.cif_type[0]=='2' 
				&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_ln_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
		{
			if( cif_type!='\0' )
			{
			sprintf( acErrMsg,"NEED客户类型不符parm[%s] [%c]",
				g_ln_parm.cif_type,cif_type );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D112" );
			goto ErrExit;
			}
		}

		/***期限检查***/
		/* 检查贷款的最小期限，最大期限 */	
		
		if ( g_ln_parm.term_type[0]=='Y' )      /* 期限类型：年 */
		{	
			g_ln_parm.min_term = g_ln_parm.min_term*12;
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* 期限类型：月 */
         	 {
			 TRACE
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' )  /* 期限类型：日 */
		  {
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		  sprintf(acErrMsg,"开户日[%ld]到期日[%ld]",g_ln_mst.opn_date,g_ln_mst.mtr_date);
		  WRITEMSG		  
		  sprintf(acErrMsg,"最小日期[%ld]最大日期[%ld]",min_day,max_day);
		  WRITEMSG
		  /*lpj20151123 因为新增加的对公贷款产品不区分期限类型，所以对公贷款开户不进行期限检查*/
		if(strncmp(g_ln_mst.prdt_no,"3G",2)!=0)
		{
		if ( g_ln_mst.mtr_date<min_day )
		  {
			sprintf(acErrMsg,"[%ld][%ld]",min_day,max_day);
			WRITEMSG
			strcpy( acErrMsg,"贷款期限小于规定的最小期限!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L003" );
			goto ErrExit;
		  }
       		 if ( g_ln_mst.mtr_date>max_day )
		  {
                strcpy( acErrMsg,"贷款期限大于规定的最大期限!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }
		sprintf(acErrMsg,"检查贷款的最小期限，最大期限(开户) PASS!");
		WRITEMSG	
	}
		/*检查该产品最低金额和最高金额，检查时用限额*/
		if ( pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_parm.min_bal)<0 )
		{
			sprintf(acErrMsg,"low [%.2lf]<[%.2lf]! ",
				g_ln_mst.amt_lmt,g_ln_parm.min_bal);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_parm.max_bal)>0
			&& pub_base_CompDblVal(g_ln_parm.max_bal,0.00)!=0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}

		/** 还款账号付值 必须是活期**/
		/* 根据币种生成账户序号 */
		/**ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &pay_ac_seqn );	***/	
		ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &pay_ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			goto ErrExit;
		}
		g_ln_mst.repay_ac_id=0;
		pub_base_strpack( g_pub_tx.ac_no1);
		if( strlen(g_pub_tx.ac_no1) )
		{
			ret = pub_ln_AcCheck( g_pub_tx.ac_no1,pay_ac_seqn,
				&cif_no,&g_ln_mst.repay_ac_id );
			sprintf(acErrMsg,"还款帐号!!!![%s][%d]",g_pub_tx.ac_no1,pay_ac_seqn);
   			WRITEMSG	
			sprintf(acErrMsg,"还款帐号id!!!![%ld]",g_ln_mst.repay_ac_id);
   			WRITEMSG
			if( ret )
			{
				sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
				WRITEMSG
				goto ErrExit;
			}
		/** 不检查是否一致了 **/
		/*
			if ( cif_no != g_pub_tx.cif_no )
			{
				sprintf(acErrMsg,"根据卡的客户号[%d]与还息户客户号[%d]不符",
					g_pub_tx.cif_no,cif_no );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L019");
				goto ErrExit;
			 }*/
		}
		/**检查输入的证件和卡的是否相符**/
		if( strlen(g_pub_tx.id_type) )
		{
			if( pub_cif_CifCheck(g_pub_tx.id_type,g_pub_tx.id_no,
				&cif_no,v_cif_type) )
			{
				sprintf(acErrMsg,"该客户在客户中心未登记![%s][%s]",
					g_pub_tx.id_type,g_pub_tx.id_no);
				WRITEMSG
				goto ErrExit;
			}
			/**不检查是否一致了**/
			/*if ( cif_no != g_pub_tx.cif_no )
			{
				sprintf(acErrMsg,"根据证件的客户号[%d]与还息户客户号[%d]不符",
					g_pub_tx.cif_no,cif_no );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L019");
				goto ErrExit;
			}*/
		}
	   /***取利率编号对应的利率***/
	   ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
					g_pub_tx.tx_date,&rate_val);
	   if( ret ) goto ErrExit;
	   sprintf(acErrMsg,"转换前利率[%lf]",rate_val);
	   WRITEMSG

	   rate_val = rate_val/1.2;/* 转换成月利率 */ 
	   sprintf(acErrMsg,"转换结束后利率[%lf]",rate_val);
	   WRITEMSG

	   
	   if( g_ln_parm.over_rate_type[0]=='0' )/* 取逾期利率编号 */
	   {
	   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&over_rate_val);
	   		if( ret ) goto ErrExit;					
	   		over_rate_val = over_rate_val/1.2;/* 转换成月利率 */
	   }else{
			over_rate_val = g_ln_mst.rate ;
	   }
	   /** 取罚息利率 **/
	   if( g_ln_parm.fine_rate_type[0]=='0' )/* 取逾期利率编号 */
	   {
	   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&fine_rate_val);
	   		if( ret ) goto ErrExit;					
	   		fine_rate_val = fine_rate_val/1.2;/* 转换成月利率 */
	   }else{
			fine_rate_val = g_ln_mst.rate ;
	   }
		sprintf(acErrMsg,"逾期利率[%s]",g_ln_parm.over_rate_no);
		WRITEMSG
		sprintf(acErrMsg,"罚息利率[%s]",g_ln_parm.fine_rate_no);
		WRITEMSG
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG
	 	sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG


		if( g_ln_parm.get_rate_type[0]=='0' ) /**挂牌利率**/
		{
			g_ln_mst.rate=rate_val;
		}else{		/**手输利率检查上浮下浮比率**/  
			/*正常利率检查*/
			/* modified by liuxuan 2010-4-16 信贷管利率综合业务不管
			if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
					rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
							,g_ln_parm.max_flt_ratio );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
            sprintf(acErrMsg,"最高利率正确时[%lf][%lf][%d] ", rate_val,
			rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
			WRITEMSG

			if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
			{
			sprintf(acErrMsg,"最低利率错误时[%lf][%lf][%d] ", rate_val,
		 rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
						 WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
			sprintf(acErrMsg,"最低利率正确时[%lf][%lf][%d] ", rate_val,
		rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			*/
			/**逾期利率检查**/
			/*
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
					over_rate_val*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.over_rate,g_ln_parm.over_rate_max,
				over_rate_val*(100+g_ln_parm.over_rate_max)/100.00
							 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L062" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
					over_rate_val*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf][%lf]!!",
						g_ln_mst.over_rate,
						over_rate_val*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L061" );
				goto ErrExit;
			}			
			*/
			/**罚息利率检查**/
			/*
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
				fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00
							 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L163" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L162" );
				goto ErrExit;
			}
			*/
		}

		/* 检查开户限制条件编码 (调用函数) *
		g_dd_parm.opn_lmt */
		TRACE
		if( pub_ins_ln_hst( 0 ) )
		{
			sprintf( acErrMsg,"登记贷款产品明细错误!" );
			WRITEMSG
			goto ErrExit;
		}
		TRACE
	/**参数检查完成**/
	ret=Ln_mst_Ins(g_ln_mst,g_pub_tx.reply);	
	if( ret )
	{
		Ln_mst_Debug(&g_ln_mst);



		sprintf(acErrMsg,"登记贷款产品主文件错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L220");
		goto ErrExit;
	}
TRACE
	/*** 登记开销户登记簿 ***/
	if( pub_base_InsMo_opn_cls(g_ln_mst.name,"","1") )
	{
		sprintf(acErrMsg,"登记开销户登记簿错误!");
		WRITEMSG
		goto ErrExit;
	}
TRACE
	return 0;
ErrExit:
	return 1;
}

int pub_ins_in_mst()
{
	int ret;
	double rate_val;
	struct com_item_c v_com_item;
	memset(&v_com_item,0x00,sizeof(struct com_item_c));

	g_in_mst.ac_id=g_pub_tx.ac_id;
	g_in_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_in_mst.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(g_in_mst.prdt_no,g_pub_tx.prdt_code);
	g_in_mst.bal=g_pub_tx.tx_amt1;
	g_in_mst.ys_bal=0.00;
	g_in_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_in_mst.rate=g_pub_tx.rate;
	g_in_mst.over_rate=g_pub_tx.over_rate;
	if( g_in_mst.over_rate<0.005 && g_in_mst.over_rate>-0.005 )
		g_in_mst.over_rate=g_in_mst.rate;
	g_in_mst.opn_date=g_pub_tx.tx_date;
	g_in_mst.ic_date=g_pub_tx.tx_date;
	g_in_mst.lst_date=g_pub_tx.tx_date;
	g_in_mst.end_date=g_pub_tx.mach_date;
	strcpy(g_in_mst.intst_knd,g_pub_tx.tmp_ind);

	/***参数检查**/
		/*检查开户日期是否在起用日期和终止日期之间*/	
		if ( (g_in_parm.star_date > g_in_mst.opn_date) || \
			(g_in_mst.opn_date > g_in_parm.stop_date))
		{	
			sprintf(acErrMsg,"超有效期[%d]not in[%d][%d]",
				g_in_mst.opn_date,g_in_parm.star_date,g_in_parm.stop_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}

		/*检查该产品最低金额和最高金额*/
		/****NEED开户是否要？**
		if( pub_base_CompDblVal(g_in_mst.bal,g_in_parm.min_amt)<0 )
		{
			sprintf(acErrMsg,"open bal is too low!! ========================
					g_in_mst.bal=[%.2lf],g_in_parm.min_amt=[%.2lf]", \
					g_in_mst.bal,g_in_parm.min_amt);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_in_mst.bal,g_in_parm.max_amt)>0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}
		**/

		/***最低最高期限的检查***/
		/***NEED***/
		if( g_in_mst.end_date<=g_in_mst.opn_date && g_in_mst.end_date!=0 )
		{
			sprintf(acErrMsg,"到期日错[%d] [%d]! ",
				g_in_mst.end_date,g_in_mst.opn_date );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "B028" );
			goto ErrExit;
		}

		/*检查利率是否超过规定利率上浮和下浮后的界限*/
		/***取利率编号对应的利率***/
		sprintf(acErrMsg,"利率rate_no[%s]cur_no[%s]tx_date[%ld]",
				g_in_parm.rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date );
			WRITEMSG		
		ret = pub_base_getllz(g_in_parm.rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		/**内部帐利率全部手输，利率检查上浮下浮比率**
			if( pub_base_CompDblVal(g_in_mst.rate,
				rate_val*(1+g_in_parm.max_flot_rate) )>0 )
			{
				sprintf(acErrMsg,"rate[%lf] is too high [%lf][%lf]!! ",
					g_in_mst.rate,rate_val,g_in_parm.max_flot_rate);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_in_mst.rate,
				rate_val*(1-g_in_parm.min_flot_rate) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
sprintf(acErrMsg,"g_in_mst.rate=[%lf],min_flot_rate=[%lf],rate_val=[%lf]",
        g_in_mst.rate,g_in_parm.min_flot_rate,rate_val);
WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
			**********/

		/***取罚息利率编号对应的利率***/
		ret = pub_base_getllz(g_in_parm.fine_rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		/**内部帐利率全部手输，利率检查上浮下浮比率**/
		sprintf(acErrMsg, "-----> max=[%lf], min=[%lf] , current=[%lf]",
			rate_val*(1+g_in_parm.max_flot_rate), 
			rate_val*(1-g_in_parm.min_flot_rate), g_in_mst.over_rate);
		WRITEMSG

		/*********
			if( pub_base_CompDblVal(g_in_mst.over_rate,
				rate_val*(1+g_in_parm.max_flot_rate) )>0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_in_mst.over_rate,
				rate_val*(1-g_in_parm.min_flot_rate) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
		**************/

		/* 检查开户限制条件编码 (调用函数) *
		g_in_parm.opn_lmt */

	/** xxx 检查该科目是否是多用户科目**/
	ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_hrt='%s'",g_in_parm.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"com_item_sel erro [%s][%d]!!",g_in_parm.acc_hrt,ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "O074" );
		goto ErrExit;
	}
	if( v_com_item.acc_knd[0]!='2' )
	{
		sprintf(acErrMsg,"该科目不允许开账号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P302");
		goto ErrExit;
	}
	if(v_com_item.sub_acc_yn[0]=='Y')
	{
		sprintf(acErrMsg,"该科目还有下级科目!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P301");
		goto ErrExit;
	}
	if( strlen(g_pub_tx.name)==0 )
		strcpy(g_pub_tx.name,v_com_item.acc_name);
	/**参数检查完成**/

	strcpy(g_pub_tx.add_ind,"1");
	/*** 计算利息积数的函数 默认为0按实际天数2变动累积数 ***/
	g_in_mst.intst_acm=0.00;
	if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,"0","2",
		0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
		g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
	{
		sprintf(acErrMsg,"计算利息积数错误!");
		WRITEMSG
		goto ErrExit;
	}
	/***/

	/**** add by xxx 20030715 ***
	if (g_pub_tx.add_ind[0] == '0') * 借 *
	{
		if( v_com_item.dc_ind[0]=='2' ) 
		{ 
			*** 调用计算利息积数函数 ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"计算利息积数错误!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
			
			strcpy(g_pub_tx.add_ind,"0"); 
		} 
		else
		{
			*** 调用计算利息积数函数 *** 
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"计算利息积数错误!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
			
			strcpy(g_pub_tx.add_ind,"1");
		}

	}
	else if (g_pub_tx.add_ind[0] == '1') * 贷 *
	{
		if( v_com_item.dc_ind[0]=='2' ) 
		{ 
			*** 调用计算利息积数函数 ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"计算利息积数错误!"); 
				WRITEMSG 
				goto ErrExit; 
			} 

			strcpy(g_pub_tx.add_ind,"1"); 
		} 
		else 
		{
			*** 调用计算利息积数函数 ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"计算利息积数错误!"); 
				WRITEMSG 
				goto ErrExit; 
			} 

			strcpy(g_pub_tx.add_ind,"0");
		}
	}
	else 
	{ 
		sprintf(acErrMsg,"存取标志错误g_pub_tx.add_ind=[%s]!",
				g_pub_tx.add_ind); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"P106"); 
		goto ErrExit; 
	}
	*** add by xxx end ****/


	g_in_mst.hst_cnt=0;
	g_in_mst.hst_pg=0;
	strcpy(g_in_mst.sts,"1");
	g_in_mst.hold_amt=0.00;
	/* modified by liuxuan 是否可透支取科目的性质 以前是直接付的N*/
	g_in_mst.od_ind[0]=v_com_item.od_ind[0];
	/* modify over */
	strcpy(g_in_mst.name,g_pub_tx.name);

	ret=pub_ins_in_hst();
	if( ret )
	{
		sprintf(acErrMsg,"登记内部明细文件错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	ret=In_mst_Ins(g_in_mst,g_pub_tx.reply);	
	if( ret )
	{
		sprintf(acErrMsg,"登记内部产品主文件错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}

	/*** 登记开销户登记簿 ***/
	if( pub_base_InsMo_opn_cls(g_in_mst.name,"",g_in_mst.sts) )
	{
		sprintf(acErrMsg,"登记开销户登记簿错误!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

static int pub_sub_init()
{
	int ret;
	/**读取产品参数**/
	switch(g_pub_tx.prdt_code[0])
	{
		case '1' :	
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_dd_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_dd_parm.dc_code);
			break;
		case '2' :	
			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_td_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_td_parm.dc_code);
			break;
		case '3' :	
			ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_ln_parm.dc_code);
			break;
		case '9' :	
			ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_in_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_in_parm.dc_code);
			break;
		default  :	
			sprintf(acErrMsg,"账户类型错[%s]!",g_pub_tx.prdt_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
	}
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品代码不存在![%s]",g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P145");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取产品参数表错误![%s][%d]",g_pub_tx.prdt_code,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P146");
		goto ErrExit;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code );

	return 0;
ErrExit:
	return 1;
}
static int pub_sub_chk_mdm()
{
	char v_cif_type[2];
	int ret;

	/****介质****/
	if( g_pub_tx.tx_date>g_mdm_ac_rel.end_date )
	{
		sprintf(acErrMsg,"该介质已经过期，请销户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P148");
		goto ErrExit;
	}

	/*** 介质产品检查函数,检查该产品与介质是否匹配 ***/
	/* if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) ) */
	if( g_pub_tx.ac_wrk_ind[0]=='9' )
		;
	else
	{
		if( pub_base_check_promedi(g_pub_tx.prdt_code,
			g_mdm_ac_rel.mdm_code,NULL_MDM,NULL_MDM) )
		{
			sprintf(acErrMsg,"该介质不能此产品[%s][%s]!",
				g_mdm_ac_rel.mdm_code,g_pub_tx.prdt_code);
			WRITEMSG
			goto ErrExit;
		}
	}

	/**客户中心检查**/
	if( g_pub_tx.prdt_code[0]!='9' )
	{
		vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,g_mdm_ac_rel.id_type);
		pub_base_strpack( g_mdm_ac_rel.id_type );
		if( strlen(g_mdm_ac_rel.id_type) )
		{
			if( pub_cif_CifCheck(g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no,
				&g_pub_tx.cif_no,v_cif_type) )
			{
				sprintf(acErrMsg,"该客户在客户中心未登记![%s][%s]",
					g_pub_tx.id_type,g_pub_tx.id_no);
				WRITEMSG
				goto ErrExit;
			}
			 vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);

		}
		else
			g_pub_tx.cif_no=0;
		vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);
	}
	else
		g_pub_tx.cif_no=0;

	cif_type=v_cif_type[0];
	if(g_pub_tx.cif_no==0)/*验资户2007-6-8 9:33**/
  {
  	cif_type='2';
  }
	g_pub_tx.ac_id_type[0]=g_pub_tx.prdt_code[0];
	g_pub_tx.ac_id_type[1]='\0';
 vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);
	return 0;
ErrExit:
	return 1;
}
