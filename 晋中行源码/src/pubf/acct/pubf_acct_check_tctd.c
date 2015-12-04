/***************************************************************
* 文 件 名:     pubf_acct_check_tctd.c
* 功能描述: 	检查帐户是否开通通存通兑业务
*
* 作    者:     wl
* 完成日期:     2007年3月31日
*
* 修改记录:
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "tctd_ktzh_c.h"

int pub_acct_check_tctd(char * ac_no)
{
	int ret = 0;
	struct tctd_ktzh_c    g_tctd_ktzh;
	
	memset(&g_tctd_ktzh,0x00,sizeof(struct tctd_ktzh_c));
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",ac_no);
	if ( ret == 100) 
	{
		sprintf(acErrMsg,"帐号在介质账号对照表不存在! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查找介质账号对照表出错! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	vtcp_log("%s %d ",__FILE__,__LINE__);
	
	ret=Tctd_ktzh_Sel(g_pub_tx.reply,&g_tctd_ktzh,"tctd_acno = %s", ac_no);
	if ( ret == 100) 
	{
		sprintf(acErrMsg,"帐号在通存通兑协议表中不存在! [%s]",ac_no);
		strcpy(g_pub_tx.reply,"0000");
		WRITEMSG
		return 0; /****若此表中没有，则此帐号没有登记协议****/
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查找介质账号对照表出错! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		return -1;
	}
	
	if(g_tctd_ktzh.tc_kt_flag[0] == '1')/****只开通通存****/
	{
		return 1;
	}
	else if(g_tctd_ktzh.td_kt_flag[0] == '1')/****只开通通兑****/
	{
		return 2;
	}
	else if(g_tctd_ktzh.cx_kt_flag[0] == '1')/****只开通查询****/
	{
		return 3;
	}
	else if(g_tctd_ktzh.tc_kt_flag[0] == '1' && g_tctd_ktzh.td_kt_flag[0] == '1')
	{/****通存通兑都开通****/
		return 4;
	}
	else if(g_tctd_ktzh.tc_kt_flag[0] == '1' && g_tctd_ktzh.td_kt_flag[0] == '1' && g_tctd_ktzh.cx_kt_flag[0] == '1')
	{/****通存通兑查询都开通****/
		return 5;
	}
	else
	{
		return 0;
	}	
	
	ErrExit:
		return -1;
}
/**********************************************************************
* 函 数 名：  pub_dd_dddis_file
* 函数功能：  根据存款显示账号和账号序号取存款主文件
* 作    者：  rob
* 完成时间：  2004年12月30日
*
* 参    数： 
*     输  入：  ac_no       char(19)  显示账号  	
*               ac_seqn     smallint  账号序号	       
*
*     输  出:                         介质帐号对照文件结构
*                                     贷款主文件结构
*                                     贷款参数文件结构
*                           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_dd_dddis_file(char *ac_no,int ac_seqn , 
struct mdm_ac_rel_c *mdm_ac_rel,struct dd_mst_c *dd_mst,
struct dd_parm_c *dd_parm )
{
	int ret = 0 ;
	
	/* 根据显示账号，在凭证(介质)与账户关系mdm_ac_rel表中查询，取出账户ID */
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	    
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_no='%s'", ac_no );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录[%d][%s]",ret,ac_no);
		WRITEMSG
		    strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常[%d][%s]",ret,ac_no);
		WRITEMSG
		    strcpy( g_pub_tx.reply ,"D103" );
		return -1;
	}

	if( mdm_ac_rel->ac_seqn ==9999)
	{
	sprintf( acErrMsg,"卡账户不可以转换mdm[%d]input[%d]",
	ac_seqn,mdm_ac_rel->ac_seqn);
	WRITEMSG
	strcpy( g_pub_tx.reply ,"D033" );
	return -1;
	}
	sprintf( acErrMsg,"看看看 1111账号=[%s],ID==[%d],seqn==[%d]",ac_no,mdm_ac_rel->ac_id,ac_seqn);
	WRITEMSG
	/* 取贷款主文件信息 */
	ret = Dd_mst_Sel(g_pub_tx.reply, dd_mst , "ac_id=%ld and ac_seqn=%d" ,
	    mdm_ac_rel->ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
		
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		return -1;
	}
	sprintf( acErrMsg,"看看看 111111=[%s]",dd_mst->prdt_no);
	WRITEMSG
	/* 取贷款参数信息 */
	ret = Dd_parm_Sel(g_pub_tx.reply, dd_parm , " prdt_no='%s' " ,
	    dd_mst->prdt_no );
	if ( ret==100 )
	{
		sprintf(acErrMsg,"贷款参数文件中无此记录 [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"取贷款参数文件异常 [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return -1;
	}
	return 0 ;
}

