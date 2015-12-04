/**************************************************************
* 文 件 名: sp8924.c
* 功能描述：可用资金管理状态表查询
* 数据库表：zjgl_mst
*
* 作    者: jack
* 完成日期：2004年3月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "zjgl_mst_c.h"
#include "com_item_c.h"
sp8924()
{
	int ret = 0;
	char flag[2];        
	char wherelist[128];    /***查询条件***/

	char cOpn_br_no[6];   /***开户机构***/
	char cCur_no   [3];   /***币种***/
	char cAcc_no   [8];   /***科目号***/
	char tmpstr[128];
	char cFlag[2];
	struct zjgl_mst_c zjgl_mst;  /*可用资金管理状态表*/
	struct com_item_c com_item;
	long lOpn_date=0;
	long lLim_beg_date=0;
	long lLim_end_date=0;
	char cName[51];
	char cLim_term[3];
	double fLim_bal=0.0;
	double lRate=0.0;
	double lOverrate=0.0;
	char cIntst_knd[11];
	char tbwhere[128];
	char crdb[2];

	memset(crdb,0x00,sizeof(crdb));
	memset(cIntst_knd,0x0,sizeof(cIntst_knd));
	memset(tbwhere,0x0,sizeof(tbwhere));
	memset(&com_item,0x0,sizeof(struct com_item_c));
	memset(cLim_term,0x0,sizeof(cLim_term));
	memset(cName,0x0,sizeof(cName));
  memset(tmpstr,0x0,sizeof(tmpstr));
	memset(&zjgl_mst,0x0,sizeof(zjgl_mst));
  memset(cFlag,0x0,sizeof(cFlag));
	memset(cOpn_br_no,0x0,sizeof(cOpn_br_no));
	memset(cCur_no,0x0,sizeof(cCur_no));
	memset(cAcc_no,0x0,sizeof(cAcc_no));
	pub_base_sysinit();
  
	/*-------- 取输入变量  ---------*/

	get_zd_data("0910",cOpn_br_no);
	get_zd_data("0210",cCur_no);
	get_zd_data("0650",cAcc_no);
	get_zd_data("0670",cFlag);
	get_zd_data("0250",cName);
	get_zd_long("0460",&lOpn_date);
	get_zd_long("0450",&lLim_beg_date);
	get_zd_long("0440",&lLim_end_date);
	get_zd_data("0220",cLim_term);
	get_zd_double("0400",&fLim_bal);
	get_zd_data("0530",cIntst_knd);
	get_zd_double("0840",&lRate);
	get_zd_double("0850",&lOverrate);
	get_zd_data("0660",crdb);
	
vtcp_log("不会吧!! lRate=[%.4lf] lOverrate=[%.4lf]  [%s]--[%f]",lRate,lOverrate,__FILE__,__LINE__);
if(cFlag[0]=='2')
{
		strcpy(g_pub_tx.brf,"上存管理状态修改");
		ret=Zjgl_mst_Dec_Upd( g_pub_tx.reply,"opn_br_no='%s' and cur_no='%s' and acc_no='%s'",cOpn_br_no,cCur_no,cAcc_no);
				if (ret != 0)
		{
			sprintf(acErrMsg,"修改信息错1[%s]",cOpn_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Zjgl_mst_Fet_Upd(&zjgl_mst, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改信息错2[%s]",cOpn_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
			memset(&com_item,0x0,sizeof(struct com_item_c));

		ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",cAcc_no);

		strcpy(zjgl_mst.acc_hrt,com_item.acc_hrt);
		zjgl_mst.opn_date=lOpn_date;
		strcpy( zjgl_mst.name, cName);
		zjgl_mst.lim_beg_date=lLim_beg_date;
		zjgl_mst.lim_end_date=lLim_end_date;
		strcpy( zjgl_mst.lim_term, cLim_term);
		strcpy( zjgl_mst.intst_knd, cIntst_knd);
		if(crdb[0]=='1')
		{
		  if(com_item.dc_ind[0]=='1')
			zjgl_mst.lim_amt=fLim_bal;
		  else if(com_item.dc_ind[0]=='2')	  
			zjgl_mst.lim_amt= -fLim_bal;
		}
		else if(crdb[0]=='2')
		{
		  if(com_item.dc_ind[0]=='1')
			zjgl_mst.lim_amt= -fLim_bal;
		  else if(com_item.dc_ind[0]=='2')
			zjgl_mst.lim_amt=fLim_bal;
		}
		zjgl_mst.rate=lRate;
		zjgl_mst.over_rate=lOverrate;
		ret = Zjgl_mst_Upd_Upd(zjgl_mst, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改信息错[%s]",cOpn_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
		ret = Zjgl_mst_Clo_Upd();	
}
if(cFlag[0]=='3')
{
			strcpy(g_pub_tx.brf,"上存管理状态删除");
			sprintf(tbwhere,"opn_br_no = '%s' and cur_no = '%s' and acc_no = '%s'", cOpn_br_no ,cCur_no ,cAcc_no);
			ret = deldb("zjgl_mst",tbwhere);
			if (ret != 0)
			{
				sprintf(acErrMsg,"删除记录错[%s]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
}
if(cFlag[0]=='1')
{
		strcpy(zjgl_mst.opn_br_no,cOpn_br_no);
		strcpy(zjgl_mst.cur_no,cCur_no);
		strcpy(zjgl_mst.acc_no,cAcc_no);
		strcpy(g_pub_tx.brf,"上存管理状态增加");

		memset(&com_item,0x0,sizeof(struct com_item_c));

		ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",cAcc_no);

		strcpy(zjgl_mst.acc_hrt,com_item.acc_hrt);
		zjgl_mst.opn_date=lOpn_date;
		strcpy( zjgl_mst.name, cName);
		zjgl_mst.lim_beg_date=lLim_beg_date;
		zjgl_mst.lim_end_date=lLim_end_date;
		strcpy( zjgl_mst.lim_term, cLim_term);
		strcpy( zjgl_mst.intst_knd, cIntst_knd);
		if(crdb[0]=='1')
		{
		  if(com_item.dc_ind[0]=='1')
			zjgl_mst.lim_amt=fLim_bal;
		  else if(com_item.dc_ind[0]=='2')	  
			zjgl_mst.lim_amt= -fLim_bal;
		}
		else if(crdb[0]=='2')
		{
		  if(com_item.dc_ind[0]='1')
		  {
		vtcp_log("[%s][%d] &&&&&& lim_amt = [%.2lf] fLim_bal = [%.2f]对不对!",__FILE__,__LINE__,zjgl_mst.lim_amt,fLim_bal);
			zjgl_mst.lim_amt= -fLim_bal;
		vtcp_log("[%s][%d] &&&&&& lim_amt = [%.2lf] fLim_bal = [%.2f]对不对!",__FILE__,__LINE__,zjgl_mst.lim_amt,fLim_bal);
		  }
		  else if(com_item.dc_ind[0]=='2')
		  {
		vtcp_log("[%s][%d] &&&&&& lim_amt = [%.2lf] fLim_bal = [%.2f]对不对!",__FILE__,__LINE__,zjgl_mst.lim_amt,fLim_bal);
			zjgl_mst.lim_amt=fLim_bal;
		vtcp_log("[%s][%d] &&&&&& lim_amt = [%.2lf] fLim_bal = [%.2f]对不对!",__FILE__,__LINE__,zjgl_mst.lim_amt,fLim_bal);
		  }
		}
		zjgl_mst.rate=lRate;
		zjgl_mst.over_rate=lOverrate;
		zjgl_mst.ic_date=lOpn_date;
		zjgl_mst.sts[0]='1';
		vtcp_log("[%s][%d] &&&&&& zjgl_mst.lim_amt = [%.2lf] fLim_bal = [%.2f]对不对!",__FILE__,__LINE__,zjgl_mst.lim_amt,fLim_bal);
		ret=Zjgl_mst_Ins(zjgl_mst,g_pub_tx.reply);
		if( ret )
        {
                sprintf(acErrMsg,"Insert!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
        }
    
}
g_pub_tx.ac_wrk_ind[1]='1';/**强制记流水**/
    
    strcpy(g_pub_tx.ac_no,cAcc_no);
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
    	strcpy( g_pub_tx.reply, "P126" );
        goto ErrExit;
    }
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
