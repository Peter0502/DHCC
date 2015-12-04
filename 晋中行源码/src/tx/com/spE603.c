/****************************************************************************************
* 文 件 名:  spE603.c
* 功能描述： 柜员完工交易
*            检查大小额是否有未发送、复核交易；柜员是否尚有付出但对方尚未收入的交接记录;
*            柜员是否尚有应该收入但尚未收入的交接记录, 是否有付出没有做收入的纪录
*            检查支行不可以记4开头科目, 清算中心只可以记408的科目
*            lvl=6的柜员不必签退
* 作    者:  andy
* 完成日期： 2004年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人: xxx/20040712
*   修改内容: 同城控制
*   日    期: 20070412
*   修 改 人: wanglei 
*   修改内容: 柜员券别总数与帐面金额总数是否相同，不相同不允许签退
*   日    期: 20070428
*   修 改 人: wanglei 
*   修改内容: 要求柜员输入券别张数，与表中张数比较，不相同不允许签退
******************************************************************************************/
#include <stdio.h>
#include <math.h>
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "com_parm_c.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "cash_mst_c.h"
#define  EXTERN


int spE603()
{
	double d_amt=0.00, c_amt=0.00;
	int	ret=0;
	int mcnt=0, zcnt=0;
	double tmp_amt1=0.00, tmp_amt2=0.00, tmp_amt3=0.00, tmp_amt4=0.00;
	double dCert=0.00;
	double dBal=0.00;  /****98柜员的余额****/
	char tmp_str[256],tmp_600[600];

	struct	com_tel_c	g_com_tel;
	struct	com_tel_connect_c	g_com_tel_connect;
	struct  com_parm_c sCom_parm;
	struct  dc_log_c sDc_log;
	struct  trace_log_c sTrace_log;
	struct  cash_mst_c cash_mst;
	struct  st_money_dtl money_box;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(tmp_str, 0x00, sizeof(tmp_str));
	memset(&sDc_log, 0x00, sizeof(struct dc_log_c));
	memset(&sTrace_log, 0x00, sizeof(struct trace_log_c));
	memset(&cash_mst, 0x00, sizeof(struct cash_mst_c));
	memset(&money_box, 0x00, sizeof(struct st_money_dtl));
	memset(tmp_600,'\0',sizeof(tmp_600));

	get_zd_data("0950",tmp_600); /****得到从前台传来的券别张数****/
	strcpy((char *)&money_box,tmp_600);

	/* 初始化结构 */
	pub_base_sysinit();

	/* add by LiuHuafeng 检查是否有未复核未发送的数据 */
	
	{
		char cTel[7];
		memset(cTel, 0 , sizeof(cTel));
		int iCnt=0;
		get_zd_data("0070",cTel);
		iCnt=sql_count("hv_zf","cmtno in ('100','101','102','103','105','108','121') and hv_sts='0' and chk='%s'",cTel);
		if(iCnt>0)
		{
			memset(acErrMsg, 0 , sizeof(acErrMsg));
			sprintf( acErrMsg, 
				"您存在%d笔复核未发送大额业务",iCnt);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O155");
			goto ErrExit;
		}
	}
	
	/* end by LiuHuafeng 20061229 */
	/*检查柜员是否尚有付出但对方尚未收入的交接记录*/
	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,"tel='%s' and ind in('0','A')",g_pub_tx.tel);
	if(ret==0)
	{
		sprintf( acErrMsg, 
				"柜员有付出的交接记录,但对方柜员尚未收入,请先收入!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O048");
		goto ErrExit;
	}else if(ret&&ret!=100){
		goto ErrExit;
	}

	
	/*检查柜员是否尚有应该收入但尚未收入的交接记录*/
	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,"tw_tel='%s' and ind in('0','A')",g_pub_tx.tel);
	if(ret==0)
	{
		sprintf( acErrMsg, "柜员有应该收入的交接记录但尚未收入,请先收入!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O047");
		goto ErrExit;
	}else if(ret && ret!=100){
		goto ErrExit;
	}

	/* add by xxx at 20040907 **/
	/* 检查是否有付出没有做收入的纪录 */
	ret = sql_count("note_mst", "tel='%s' and sts = '2'", g_pub_tx.tel);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计note_mst表错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	if (ret > 0)
	{
		sprintf(acErrMsg, "有调出待收状态的凭证!![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "N095");
		goto ErrExit;
	}

	/* add by xxx end **/
	
	/**add by spj for vtm 20140610 beg**/
	ret = sql_count("vtm_note_mst", "tw_tel='%s' and ind = '2'", g_pub_tx.tel);
	if (0 > ret)
	{
		sprintf(acErrMsg, "操作数据库[vtm_note_mst]错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < ret)
	{
		sprintf(acErrMsg, "该柜员存在未接收的卡!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A096");
		goto ErrExit;
	}
	/**add by spj for vtm 20140610 end**/

	/* wanglei begin 20070412 */
	ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst,"tel='%s'",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "查询现金登记簿表出错!![%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	/****核对券别张数     98柜员不检查张数****/
	if(memcmp(g_pub_tx.tel+2,"98",2) != 0)
	{
		if((atol(money_box.money100) != cash_mst.t10000) || (atol(money_box.money50) != cash_mst.t5000) || \
		   (atol(money_box.money20)  != cash_mst.t2000) || (atol(money_box.money10)  != cash_mst.t1000) || \
		   (atol(money_box.money5)   != cash_mst.t500)  || (atol(money_box.money2)   != cash_mst.t200) || \
		   (atol(money_box.money1)   != cash_mst.t100)  || (atol(money_box.money05)  != cash_mst.t50) || \
		   (atol(money_box.money02)  != cash_mst.t20)   || (atol(money_box.money01)  != cash_mst.t10) || \
		   (atol(money_box.money005) != cash_mst.t5)    || (atol(money_box.money002) != cash_mst.t2) || \
		   (atol(money_box.money001) != cash_mst.t1) )
		{
			vtcp_log("10000[%d],5000[%d],2000[%d],1000[%d],500[%d],200[%d],100[%d],50[%d],20[%d],10[%d],5[%d],2[%d],1[%d]\n",cash_mst.t10000,cash_mst.t5000,cash_mst.t2000,cash_mst.t1000,cash_mst.t500,cash_mst.t200,cash_mst.t100,cash_mst.t50,cash_mst.t20,cash_mst.t10,cash_mst.t5,cash_mst.t2,cash_mst.t1);


				sprintf(acErrMsg,"柜员输入券别张数与后台记录不符!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "M039");
				goto ErrExit;
		}
	}else{/****98柜员签退时，核对余额****/
		get_zd_double("0400",&dBal);
		if(pub_base_CompDblVal(dBal, cash_mst.bal) != 0)
		{
			sprintf(acErrMsg,"98柜员现金余额不符! dbal=[%.2f] bal= [%.2f]",dBal,cash_mst.bal);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M040");
			goto ErrExit;
		}
	}

	dCert = cash_mst.t10000*100.0+cash_mst.t5000*50.0+cash_mst.t2000*20+cash_mst.t1000*10+ \
	cash_mst.t500*5+cash_mst.t200*2+cash_mst.t100*1+cash_mst.t50*0.5+cash_mst.t20*0.2+ \
	cash_mst.t10*0.1+cash_mst.t5*0.05+cash_mst.t2*0.02+cash_mst.t1*0.01;

	/****wanglei 20070426   不控制98柜员券别合计总额与现金余额不相等的情况****/
	if(memcmp(g_pub_tx.tel+2,"98",2) != 0)
	{
		if(pub_base_CompDblVal(dCert, cash_mst.bal) != 0)
		{
			sprintf(acErrMsg,"柜员券别合计总额与现金余额不符! cert=[%.2f] bal= [%.2f]",dCert,cash_mst.bal);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M038");
			goto ErrExit;
		}
	}

	/* wanglei end 20070412 */

  /*****/
  ret=sql_count("com_tel","br_no='%s' and csts='0' and tel not like '%99'",g_pub_tx.tx_br_no,g_pub_tx.tel);
  if(ret==1)
  {
  	set_zd_int("0680",ret);
  }
  /*****/
	/*检查柜员状态*/
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",
					g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "查询柜员信息表出错!!" );
		WRITEMSG
		goto ErrExit;
	}

	if( g_com_tel.lvl[0]=='6' )
	{
		sprintf( acErrMsg, "不必退[%s]!!",g_pub_tx.tel );
		WRITEMSG
		strcpy(g_pub_tx.reply,"S080");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='2')
	{
		sprintf( acErrMsg, "柜员已经签退!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O046");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='3')
	{
		sprintf( acErrMsg, "该柜员已被锁定!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O063");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='4')
	{
		sprintf( acErrMsg, "该柜员已被删除!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O062");
		goto ErrExit;
	}

	/* 检查4开头科目不可以记帐,清算中心可记408 **/
	d_amt=0;
if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,5)!=0)
{
	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and substr(acc_hrt,1,3) in('407','408','416') and tx_opn_br_no='%s' ",g_pub_tx.tx_date, g_pub_tx.tel,g_pub_tx.tx_br_no);
}
else{
	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and substr(acc_hrt,1,3) in('407','416')",g_pub_tx.tx_date, g_pub_tx.tel);
}
	if (ret != 0)
	{
vtcp_log("%d@%s ret=%d",__LINE__, __FILE__, ret);
		sprintf(acErrMsg, "统计4开头科目发生额错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

		/*vtcp_log("d_amt is[%.2f] ",d_amt);*/
	if (pub_base_CompDblVal(d_amt, 0.00) != 0)
	{
		sprintf(acErrMsg, "不可以记4开头科目 [%.2f] [%.2f]",d_amt, c_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P081");
		goto ErrExit;
	}

	/*修改柜员状态为签退*/
	ret=sql_execute("update com_tel set csts='%s' where tel='%s'",
					 "2",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "更新柜员状态出错!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O049");
		goto ErrExit;
	}

	/* add by xxx at 20040820 */
	/* 检查该柜员的会计流水是否平账 */
	/*取今日借方发生额总数*/ 
	tmp_amt1 = 0.00;
	tmp_amt2 = 0.00;
	tmp_amt3 = 0.00;
	tmp_amt4 = 0.00;
	ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='1' and tx_tx_br_no='%s' and \
		sts='0' and acc_hrt[1]<'6' and tel='%s'", g_pub_tx.tx_br_no,
		g_pub_tx.tel); 
	if(ret) 
	{ 
		sprintf( acErrMsg, "Declare dc_log error!!" ); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret=Dc_log_Fet_Sel(&sDc_log, g_pub_tx.reply); 
		if(ret==100)break; 
		else if(ret) 
		{ 
			sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		
		if( sDc_log.ct_ind[0]=='1' ) tmp_amt3+=sDc_log.amt; 
		tmp_amt1+=sDc_log.amt; 
	} 
	Dc_log_Clo_Sel(); 
	
	/*取今日贷方发生额总数*/ 
	ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='2' and tx_tx_br_no='%s' and \
		sts='0' and acc_hrt[1]<'6' and tel='%s'",g_pub_tx.tx_br_no,
		g_pub_tx.tel); 
	if(ret) 
	{ 
		sprintf( acErrMsg, "Declare dc_log error!!" ); 
		WRITEMSG 
		goto ErrExit; 
	}

	while(1) 
	{ 
		ret=Dc_log_Fet_Sel(&sDc_log,g_pub_tx.reply); 
		if(ret==100)break; 
		else if(ret) 
		{ 
			sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		if( sDc_log.ct_ind[0]=='1' ) tmp_amt4+=sDc_log.amt; 
		tmp_amt2+=sDc_log.amt; 
	} 
	Dc_log_Clo_Sel();

	/*检查会计分录借贷是否平衡*/ 
	ret=pub_base_CompDblVal(tmp_amt1,tmp_amt2); 
	if(ret==1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf 差额为%.2lf",tmp_amt1,tmp_amt2,tmp_amt1-tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日借方发生额大于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,差额为%.2lf",tmp_amt1,tmp_amt2,tmp_amt1-tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日借方发生额小于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	}

	/*检查会计分录借贷是否平衡*/ 
	ret=pub_base_CompDblVal(tmp_amt3,tmp_amt4); 
	if(ret==1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,差额为%.2lf",tmp_amt3,tmp_amt4,tmp_amt3-tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日现金借方发生额大于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,差额为%.2lf",tmp_amt3,tmp_amt4,tmp_amt3-tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日现金借方发生额小于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	}
  
	set_zd_data("0670","0");				/*正常签退*/
	
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    vtcp_log("[%s][%d]Before return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    return 1;
}
