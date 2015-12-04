/*************************************************
* 文 件 名: sp9616.c
* 功能描述：强行签退柜员
*
* 作    者: rob 
* 完成日期：20030719
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "com_parm_c.h"
#include "dc_log_c.h"
#include "tel_control_c.h"

int sp9616( )
{
	double d_amt=0.00, c_amt=0.00;
	int	ret=0;
	double tmp_amt1=0.00, tmp_amt2=0.00, tmp_amt3=0.00, tmp_amt4=0.00;
	char tmp_str[256];

	struct	com_tel_c				g_com_tel;
	struct	com_tel_connect_c		g_com_tel_connect;
	struct  com_parm_c sCom_parm;
	struct  dc_log_c sDc_log;
	struct tel_control_c sTel_control;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(&sDc_log, 0x0, sizeof(struct dc_log_c));
	memset(&sTel_control, 0x0, sizeof(struct tel_control_c));

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_data("0920", g_pub_tx.tel);

	sprintf(acErrMsg, "强行签退柜员号为:[%s]", g_pub_tx.tel);
	WRITEMSG

	/*检查柜员状态*/
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",
		g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "查询柜员信息表出错!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.tx_br_no, g_com_tel.br_no);
	strcpy(sTel_control.br_no, g_com_tel.br_no);
	strcpy(sTel_control.tel, g_com_tel.tel);
	sTel_control.tx_date = g_pub_tx.tx_date;
	sTel_control.sts[0] = '0';
	strcpy(sTel_control.msg, "该柜员由于昨天未签退而被锁定!!");
	ret = Tel_control_Ins(sTel_control, g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg, "insert into tel_control error [%d]", ret);
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

	/* add by rob at 20030907 **/
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
	/* add by rob end **/

	/* 对同城的控制 add by rob at 20030711 */
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and \
			parm_seqn = 1");
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "公共参数表没有该挂账科目!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "O228");
		goto ErrExit;
	}

	/* 检查会计流水 26102 科目余额是否为零 *
	**/

	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and dc_ind='1' and acc_hrt='%s'", g_pub_tx.tx_date, 
		g_pub_tx.tel, sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计提出挂帐科目借方余额错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &c_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and dc_ind='2' and acc_hrt='%s'", g_pub_tx.tx_date, 
		g_pub_tx.tel, sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计提出挂帐科目贷方余额错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	if (pub_base_CompDblVal(d_amt, c_amt) != 0)
	{
		sprintf(acErrMsg, "同城提出挂帐科目不平 [%lf] [%lf]",d_amt, c_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O230");
		goto ErrExit;
	}

	ret = sql_count("mo_city", "tx_br_no='%s' and tel='%s' and \
		sts='2'", g_pub_tx.tx_br_no, g_pub_tx.tel);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计同城登记簿错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "该柜员还有提出打印票据没有核销!! ");
		WRITEMSG
		set_zd_data("0130", "该柜员还有提出打印票据没有核销!!");
		strcpy(g_pub_tx.reply, "D258");
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

	/* add by rob at 20030820 */
	/* 检查该柜员的会计流水是否平账 */
	/*取今日借方发生额总数*/ 
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
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt1,tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日借方发生额大于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt1,tmp_amt2); 
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
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt3,tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日现金借方发生额大于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt3,tmp_amt4);
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "本日现金借方发生额小于贷方发生额!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	}


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"强行签退柜员成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"强行签退柜员失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
