/***************************************************************
* 文 件 名: spD610.c
* 功能描述：改对公活期积数
* 作    者: mike
* 完成日期：2003年3月9日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"

spD610()
{   
	int ret; 
	char ac_no[20],ac_type[2],intst_type[2];
	char name[51];
	char cFlt_ratio[9];                     /****浮动比率****/
	long tmp_date1,tmp_date2;
	double tmp_amt=0,intst_acm=0;

	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_c     g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	
	memset(cFlt_ratio,0x00,sizeof(cFlt_ratio));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* 账号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			/* 账户序号 */
	get_zd_double("0560",&intst_acm);  			    /* 调整后的积数 */
	get_zd_data("0290",cFlt_ratio);  		    /* 浮动比率 */
	vtcp_log("[%s][%d]  flt_ratio = %.2f",__FILE__,__LINE__,atof(cFlt_ratio));

	/*根据账号查找账号ID*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
						 g_pub_tx.ac_no);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	
				ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				
				ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"活期文件中无此记录");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_dd_mst.ac_id;
				g_pub_tx.tx_amt1=intst_acm;
				g_dd_mst.intst_acm=intst_acm;
				vtcp_log("新积数[%.2f]",g_dd_mst.intst_acm);
				g_dd_mst.lst_date=g_pub_tx.tx_date;
				/*g_dd_mst.flt_ratio=atof(cFlt_ratio);**更新浮动比率 不在这里更新了有单独交易**/

				ret = Dd_mst_Upd_Upd( g_dd_mst,g_pub_tx.reply );
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				Dd_mst_Clo_Upd();
    g_pub_tx.hst_ind[0]='1';
    strcpy(g_pub_tx.brf,"调积数");
    /*** 按财务部要求不登记交易明细 20121106 wudw
    if(pub_ins_dd_hst())     
        goto ErrExit;
	****/
    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"对公积数调整成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"对公积数调整失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
