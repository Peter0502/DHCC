/*****************************************************
* 文 件 名:  sp9505.c
* 功能描述： 查找syyh_czyh_qs_reg 返回指定日期下的清算情况
* 作    者:  LiuHuafeng 2009-5-16 8:02:51
* 完成日期： 2009-5-16 8:02:55
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
* insert into tx_def values('9505','查与村镇银行清算数据','10000000000000000000000000000110000000110010000000010000000000000010000000000000000000000000000000000000000000000000000000000000','0','4','2');
* insert into tx_flow_def values('9505','0','9505','#$');
* insert into tx_sub_def values('9505','查与村镇银行清算数据','sp9505','0','0');
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
#include "syyh_czyh_qs_reg_c.h"
int sp9505()
{
	int ret=0,i;
	struct syyh_czyh_qs_reg_c sQsreg;
	long lQsdate=0;
	double dZcamt =0.0;
	char cFlag[2];
	char cDc_ind[2];
	char cKinbr[BRNO_LEN+1];
	char cQs_ac_no[21];   /* 清算资金账户，商行是110，村镇银行是自己一个内部科目 */
	char cCz_ac_no[21]; /* 存放同业 商行是115的一个账户，村镇银行是一个账户 */

	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cFlag , 0 , sizeof(cFlag));
	memset(cDc_ind , 0 , sizeof(cDc_ind));
	memset(&sQsreg , 0 , sizeof(sQsreg));
	memset(cQs_ac_no , 0 , sizeof(cQs_ac_no));
	memset(cCz_ac_no , 0 , sizeof(cCz_ac_no));

	
	
	vtcp_log("sp9505 开始！[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_long("0440",&lQsdate);
	get_zd_data("0680", cFlag);
	get_zd_data("0910",cKinbr);
	vtcp_log("[%s][%d] cFlag=[%s] cKinbr=[%s]",__FILE__,__LINE__,cFlag,cKinbr);
	
	ret = Syyh_czyh_qs_reg_Dec_Sel( g_pub_tx.reply , "tx_date=%ld and flag='%s' and qs_flag='1' and br_no='%s' " ,lQsdate,cFlag,cKinbr);
	if(ret)
	{
		vtcp_log("%s,%d 定义游标出现错误 ret = %ld",__FILE__,__LINE__,ret);
		goto ErrExit;
	}
	memset(&sQsreg , 0 , sizeof(sQsreg));
	ret = Syyh_czyh_qs_reg_Fet_Sel( &sQsreg , g_pub_tx.reply );
	if(ret!=0 && ret !=100)
	{
		vtcp_log("%s,%d 定义游标出现错误 ret = %ld",__FILE__,__LINE__,ret);
		goto ErrExit;
	}
	set_zd_double("0390",sQsreg.wz_amt);
	set_zd_double("0400",sQsreg.lz_amt);
	dZcamt +=sQsreg.zc_amt;
	Syyh_czyh_qs_reg_Clo_Sel();

	if(pub_base_CompDblVal(dZcamt,0.0)>0)
	{
		cDc_ind[0]='1';
	}
	else
	{
		cDc_ind[0]='2';
		dZcamt = 0 - dZcamt;
	}
	set_zd_data("0670",cDc_ind);
	set_zd_double("0430",dZcamt);
	if(sQsreg.flag[0]=='1')
	{
		/* 得到对应大小额需要清算的会计分录或账户 */
		/* 得到大额的 */
		if((char*)getenv("CZYH")!=NULL && strncmp((char *)getenv("CZYH"),"YES_CZYH",8)==0)
		{
			/* 村镇银行大额 */
			/* 暂时按村镇银行不需要清算考虑， */
		}
		else
		{
			ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HVQS,cQs_ac_no);
			if(ret)
			{
				vtcp_log("%s,%d 得到110清算账户错误%d",__FILE__,__LINE__,ret);
				strncpy(g_pub_tx.reply,"P094",4);
				goto ErrExit;
			}
			ret = pub_base_GetParm_Str(cKinbr,PARM_CZYH_QS_NO,cCz_ac_no);
			if(ret)
			{
				vtcp_log("%s,%d 得到115与村镇银行清算账户错误%d",__FILE__,__LINE__,ret);
				strncpy(g_pub_tx.reply,"P094",4);
				goto ErrExit;
			}
		}
	}
	else
	{
		/* 得到对应大小额需要清算的会计分录或账户 */
		/* 得到小额的 */
		if((char*)getenv("CZYH")!=NULL && strncmp((char *)getenv("CZYH"),"YES_CZYH",8)==0)
		{
			/* 村镇银行小额 */
			/* 暂时按村镇银行不需要清算考虑， */
		}
		else
		{
			/* 小额用待清算款项进行轧差处理
			ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HVQS,cQs_ac_no);
			if(ret)
			{
				vtcp_log("%s,%d 得到110清算账户错误%d",__FILE__,__LINE__,ret);
				strncpy(g_pub_tx.reply,"P094",4);
				goto ErrExit;
			}
			****/
			strncpy(cQs_ac_no,LV_WAIT_QS,sizeof(cQs_ac_no)-1);
			ret = pub_base_GetParm_Str(cKinbr,PARM_CZYH_QS_NO,cCz_ac_no);
			if(ret)
			{
				vtcp_log("%s,%d 得到115与村镇银行清算账户错误%d",__FILE__,__LINE__,ret);
				strncpy(g_pub_tx.reply,"P094",4);
				goto ErrExit;
			}
		}
	}
	set_zd_data("0300",cQs_ac_no);
	set_zd_data("0310",cCz_ac_no);
	/******获取三个索引的值*******/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
