/***************************************************************
* 文 件 名: spD413.c
* 功能描述：卡限额维护
* 作    者: mike
* 完成日期：2003年03月25日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card_consum_lmt_c.h"

spD413()
{
	int ret = 0;
	char tmp_ac_no[20];

	struct  card_consum_lmt_c   g_card_consum_lmt;
	struct  card_consum_lmt_c   card_consum_lmt;
	struct  mdm_ac_rel_c   		f_mdm_ac_rel;

	memset(&card_consum_lmt,0x00,sizeof(struct card_consum_lmt_c));
	memset(&g_card_consum_lmt,0x00,sizeof(struct card_consum_lmt_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("1021",g_pub_tx.ac_no);						/*主卡号*/
	get_zd_data("0310",tmp_ac_no);							/*副卡号*/
	get_zd_double("0390",&card_consum_lmt.da_cash_lmt);    	/*日取现限额*/
	get_zd_double("0400",&card_consum_lmt.da_amt_lmt);     	/*日取款限额*/
	get_zd_double("0410",&card_consum_lmt.da_con_lmt); 	 	/*日消费限额*/
	get_zd_double("0420",&card_consum_lmt.od_cash_lmt);     /*现金透支限额*/
	get_zd_double("0430",&card_consum_lmt.od_amt_lmt);		/*透支限额*/
	get_zd_double("1001",&card_consum_lmt.mo_cash_lmt);    	/*月取现限额*/
	get_zd_double("1002",&card_consum_lmt.mo_amt_lmt);     	/*月取款限额*/
	get_zd_double("1003",&card_consum_lmt.mo_con_lmt);  	/*月消费限额*/

	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1144",g_pub_tx.draw_pwd);
 	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);

	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.ac_no,&g_mdm_ac_rel,"ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
	  	sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P237");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.main_ind[0]!='1')
	{
		sprintf(acErrMsg,"主卡不是主卡!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P236");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]!='0')
	{
		sprintf(acErrMsg,"主卡处于非正常状态!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P239");
		goto ErrExit;
	}

	/*检查支取方式是否合法*/
	ret = pub_base_check_draw("001");
	if( ret )
	{
	 	sprintf(acErrMsg,"调用检查支取方式是否合法函数出错!!");
		WRITEMSG
		goto ErrExit;
	}

/**********************************************************************/

	/*检查副卡是否存在*/
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.ac_no,&f_mdm_ac_rel,"ac_no='%s'", \
								 tmp_ac_no);
	if( g_reply_int==100 )
	{
	  	sprintf(acErrMsg,"介质账户对照表中不存在该副卡记录");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P238");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.note_sts[0]!='0')
	{
		sprintf(acErrMsg,"副卡处于非正常状态!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P240");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.ac_id!=f_mdm_ac_rel.ac_id)
	{
		sprintf(acErrMsg,"副卡与主卡帐号ID不一致!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P235");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.main_ind[0]!='2')
	{
		sprintf(acErrMsg,"副卡不是副卡!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P241");
		goto ErrExit;
	}

	/*操作卡限额登记薄*/
	ret=Card_consum_lmt_Sel(g_pub_tx.reply,&g_card_consum_lmt, \
	  						"ac_no='%s'",tmp_ac_no);
    if( ret==100 )
  	{
		strcpy(card_consum_lmt.ac_no,tmp_ac_no);

		ret = Card_consum_lmt_Ins(card_consum_lmt,g_pub_tx.reply);
		if( ret==-239 )
		{
		  	sprintf(acErrMsg,"登记卡限额登记薄异常[%s]",ret);
		 	WRITEMSG
		 	strcpy( g_pub_tx.reply, "D117" );
		  	goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg, "insert error !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
 	}
  	else if(ret==0)
	{
    	ret=sql_execute("update card_consum_lmt set da_cash_lmt=%.2lf, \
						da_amt_lmt=%.2lf,da_con_lmt=%.2lf,od_cash_lmt=%.2lf, \
						od_amt_lmt=%.2lf,mo_cash_lmt=%.2lf,mo_amt_lmt=%.2lf, \
						mo_con_lmt=%.2lf where ac_no='%s'", \
						card_consum_lmt.da_cash_lmt, \
						card_consum_lmt.da_amt_lmt, \
						card_consum_lmt.da_con_lmt, \
						card_consum_lmt.od_cash_lmt, \
						card_consum_lmt.od_amt_lmt, \
						card_consum_lmt.mo_cash_lmt, \
						card_consum_lmt.mo_amt_lmt, \
						card_consum_lmt.mo_con_lmt, \
						tmp_ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"更新卡限额登记薄异常[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "D118" );
			goto ErrExit;
		}
	}
	else{
		  sprintf(acErrMsg,"查询卡限额登记薄异常[%s]");
		  WRITEMSG
		  strcpy( g_pub_tx.reply, "D119" );
		  goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"卡限额维护成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"卡限额维护失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
