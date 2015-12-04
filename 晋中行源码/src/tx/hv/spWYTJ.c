/**************************************************************
* 文 件 名:  spWYTJ.c
* 功能描述:  网银用,数据库提交                     090815 LOONG
* 修改说明:  数据库提交之前新增手续费计算以及余额信息的检查,
*            对于扣除手续费之后余额不足的情况将终止该笔操作.
*            Mod by hxc 20110114
**************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>
#define EXTERN
#include "svrpub_c.h"
#include "public.h"
#include "com_def_chrg_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "hv_define.h"

int spWYTJ()
{
	int ret = 0;
	double	amt,chrg_amt,ttlamt,available_amt;
	char vtx_code[5];
	struct	com_def_chrg_c	def_chrg;
	struct	mo_def_rate_c	mo_def_rate;
	struct	com_chrg_rate_c	com_chrg_rate;
	struct  mdm_ac_rel_c sMdm_ac_rel;
	struct  dd_mst_c sDd_mst;
	char tmpstr[100];
	int need=0,needauto=0;
	char ind[2];
	char cQuickFlag[2];/* 加急标志 1-加急 其他正常*/
	char ac_no[33];
	HV_FD123_AREA   fd1230;
	
	memset(&def_chrg,0,sizeof(def_chrg));
	memset(&mo_def_rate,0,sizeof(mo_def_rate));
	memset(&com_chrg_rate,0,sizeof(com_chrg_rate));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst,0,sizeof(struct dd_mst_c));
	
	memset(&fd1230, 0, sizeof(fd1230));
	memset(vtx_code, 0 , sizeof(vtx_code));
	memset(tmpstr, 0 , sizeof(tmpstr));
	memset(ind, 0 , sizeof(ind));
	memset(cQuickFlag, 0 ,sizeof(cQuickFlag));
	memset(ac_no, 0x00, sizeof(ac_no));
	
	get_zd_data( "0093",ind );			/*收费手段*/
	vtcp_log(" GET 0093 ind=[%s]",ind );
	get_zd_data("122F",cQuickFlag);
	vtcp_log(" GET 122F cQuickFlag=[%s]",cQuickFlag );
	
	if(ind[0] == '3')
	{
		vtcp_log("[%s],[%d] GET 0093 ind=[%s]",__FILE__,__LINE__,ind );
		goto OkExit;
	}

	get_zd_data( DC_TX_CODE,vtx_code );
	
	ttlamt=0.00;
	vtcp_log( "TJ:tx_code='%s' order by seqn",vtx_code );
	
	ret=Com_def_chrg_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by seqn", vtx_code );
	if ( ret )
	{
		goto ErrExit;
	}
	while ( 1 )
	{
		vtcp_log("[%s,%d]xyy111111-----------------> need===[%d]",__FILE__,__LINE__,need);
		ret=Com_def_chrg_Fet_Sel( &def_chrg,g_pub_tx.reply );
		if ( ret==100 ) break;
		if ( ret ) goto ErrExit;
		
		pub_base_strpack( def_chrg.amt_no );
		pub_base_strpack( def_chrg.mo );
		pub_base_strpack( def_chrg.cond );
		
		sprintf(acErrMsg,"收费CHRG[%s][%s]",def_chrg.cond,def_chrg.chrg_mode);
		WRITEMSG
		    /**条件不符合**/
		if ( pub_comm_auth_condchk(def_chrg.cond) )
		{
			continue;
		}
		if ( def_chrg.chrg_mode[0]=='A' || def_chrg.chrg_mode[0]=='B' )
		{
			needauto++;
			continue;
		}
		vtcp_log("[%s],[%d] Enter pubf_def_chrg_proc!!!!",__FILE__,__LINE__);
		ret=pubf_def_chrg_proc( def_chrg.chrg_rate_code, mo_def_rate.chrg_rate_no,def_chrg.amt_no,&amt );
		if ( ret ) goto ErrExit;
		
		vtcp_log("[%s],[%d] amt===[%f],Enter pubf_acct_chrg_proc!!!!",__FILE__,__LINE__,amt);
		ret=pubf_acct_chrg_proc( mo_def_rate.chrg_rate_no, com_chrg_rate.dc_code,&chrg_amt,amt );
		if ( ret ) goto ErrExit;
		
		    if ( chrg_amt>0.005 )
		{
			vtcp_log("[%s,%d]xyy2222-----------------> need===[%d]",__FILE__,__LINE__,need);
			if ( !need )
			{
				ind[0]=def_chrg.chrg_mode[0];
			}
			need++;
			ttlamt+=chrg_amt;
		}
		vtcp_log("[%s,%d]xyy3333-----------------> need===[%d]",__FILE__,__LINE__,need);
	}
	Com_def_chrg_Clo_Sel();
	set_zd_double( "1227",ttlamt );
	vtcp_log("[%s],[%d]手续费总额为ttlamt=[%.2lf]",__FILE__,__LINE__,ttlamt);
	
	get_fd_data("1230",(char*)&fd1230);
	vtcp_log("%s %d, GET pay_ac_no=[%s]",__FILE__,__LINE__,fd1230.pay_ac_no );
	memcpy(ac_no,fd1230.pay_ac_no,32);
	pub_base_strpack_all(ac_no);
	vtcp_log("%s %d, GET ac_no=[%s]",__FILE__,__LINE__,ac_no );
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_no='%s' ", ac_no);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Mdm_ac_rel_Sel数据库操作失败[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		
	ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id='%d'", sMdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Dd_mst_Sel数据库操作失败[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		available_amt=0.00;
                /**晋中商行网银去掉10元留存金额 20130934 hzh**/
		/**available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt-10.00;**/
		available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt;
		if(pub_base_CompDblVal(available_amt,ttlamt)<0)
			{
				 vtcp_log("[%s],[%d]账户可用余额不足,可用余额available_amt=[%.2lf],手续费ttlamt=[%.2lf]",__FILE__,__LINE__,available_amt,ttlamt);
				 strcpy(g_pub_tx.reply, "A030");
			   goto ErrExit;
			}
	
OkExit:
	    sprintf(acErrMsg,"收费检查子交易成功!%d,%d",need,needauto);
	    WRITEMSG
		  /***set_zd_data( "0093","2" );***/
		  vtcp_log("[%s,%d]xyy3333-----------------> need===[%d]",__FILE__,__LINE__,need);
		
		  ret = sql_commit();
		  if(ret)
		  {
				sprintf(acErrMsg, "数据库提交失败![%s]", g_pub_tx.reply);
				strcpy(g_pub_tx.reply, "D104");
				WRITEMSG
				goto ErrExit;    	 
		  }
		  sprintf(acErrMsg, "数据库提交成功![%s]", g_pub_tx.reply);
		  strcpy(g_pub_tx.reply, "0000");
		  WRITEMSG
		  set_zd_data(DC_REPLY, g_pub_tx.reply);
	    return 0;
ErrExit:
	    strcpy( RtCode,g_pub_tx.reply );
	    set_zd_data(DC_REPLY,RtCode);
	    return 1;

}
