/***************************************************************************/
/* 文件名称: spJ105.c                                                      */
/* 功能描述: 商行支付系统与村镇银行清算交易                                */
/* 创建日期: 2009-5-15 22:05:41                                            */
/* 作者:     LiuHuafeng                                                    */
/* 说明:                                                                   */
/* insert into tx_def values('5206','与村镇银行清算','10000000000000000000000000000110000000110010000000010000000000000010000000000000000000000000000000000000000000000000000000000000','0','4','2'); */
/* insert into tx_flow_def values('5206','0','5206','#=0000@1@#$');        */
/* insert into tx_flow_def values('5206','1','A016','#=0000@2@#$');        */
/* insert into tx_flow_def values('5206','2','A017','#$');                 */
/* insert into tx_sub_def values('5206','与村镇银行清算','spJ105','0','0');*/
/***************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_sysctl_c.h"
#include "lv_chk842_m_c.h"
#include "lv_chk842_d_c.h"
#include "lv_define.h"
#include "syyh_czyh_qs_reg_c.h"
int spJ105(){
	char cAcno110[25];
	char cAcno115[25];
	char cKinbr[BRNO_LEN+1];
	double dQsamt=0;
	char cDc_ind[2];
	double dTxamt=0.0;
	struct syyh_czyh_qs_reg_c sQsreg;
	int ret = 0;
	long lQsdate=0;
	char cZf_type[2];
	memset(cZf_type, 0 , sizeof(cZf_type));
	memset(cDc_ind , 0 , sizeof(cDc_ind));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cAcno110 , 0 , sizeof(cAcno110));
	memset(cAcno115 , 0 , sizeof(cAcno115));
	memset(&sQsreg , 0 , sizeof(sQsreg));
	pub_base_sysinit();
	get_zd_data("0300",cAcno110);
	get_zd_data("0310",cAcno115);
	get_zd_double("0430",&dQsamt);
	get_zd_data("0670",cDc_ind);
	get_zd_data("0680",cZf_type);
	get_zd_data("0910",cKinbr);
	get_zd_long("0440", &lQsdate);
	/* 针对商行110账户的dc_ind 如果村镇银行来帐比往帐多，dc_ind=2,反之 dc_ind=1 */
	ret = Syyh_czyh_qs_reg_Dec_Upd( g_pub_tx.reply , "tx_date=%ld and qs_flag='1' and flag='%s' and br_no='%s' " ,lQsdate,cZf_type,cKinbr);
	if(ret)
	{
		vtcp_log("%s,%d 定义游标出现错误 ret = %ld",__FILE__,__LINE__,ret);
		goto ErrExit;
	}
	while(1)
	{
		ret = Syyh_czyh_qs_reg_Fet_Upd ( &sQsreg,g_pub_tx.reply);
		if(ret == 100)
		{
			break;
		}
		if(ret)
		{
			vtcp_log("%s,%d 查询错误 ret = %ld",__FILE__,__LINE__,ret);
			goto ErrExit;
		}
		sQsreg.qs_flag[0]='2';
		dTxamt += sQsreg.zc_amt;
		sQsreg.qs_trace_no=g_pub_tx.trace_no;
		sQsreg.qs_time = g_pub_tx.tx_time;
		strncpy(sQsreg.tel,g_pub_tx.tel,sizeof(sQsreg.tel)-1);
		ret = Syyh_czyh_qs_reg_Upd_Upd( sQsreg, g_pub_tx.reply);
		if(ret)
		{
			Syyh_czyh_qs_reg_Debug( sQsreg );
			vtcp_log("%s,%d 更新错误 ret = %ld",__FILE__,__LINE__,ret);
			goto ErrExit;
		}
	}
	Syyh_czyh_qs_reg_Clo_Upd( );
	/* 根据借贷方向确定传入的清算金额站在往帐金额-来帐金额的角度上的正负关系*/
	if(cDc_ind[0]=='1')
	{
		/* 往帐笔来帐多 */
	}
	else
	{
		/* 来帐比往帐多 */
		dQsamt = 0 - dQsamt;
	}
	vtcp_log("[%s][%d] 金额的和= %.2f,传来的清算金额 =%.2f",__FILE__,__LINE__,dTxamt,dQsamt);
	if(pub_base_CompDblVal(dTxamt,dQsamt)!=0)
	{
		vtcp_log("%s,%d 清算表中往帐-来帐金额的和= %.2f,传来的清算金额 =%.2f",__FILE__,__LINE__,dTxamt,dQsamt);
		strncpy(g_pub_tx.reply,"P092",4);
		goto ErrExit;
	}
	if(pub_base_CompDblVal(dTxamt,0.00)==0)
	{
		vtcp_log("%s,%d 清算金额 =%.2f 不需要清算",__FILE__,__LINE__,dTxamt,dQsamt);
		strncpy(g_pub_tx.reply,"P093",4);
		goto ErrExit;
	}
	vtcp_log("%s,%d ",__FILE__,__LINE__);
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

