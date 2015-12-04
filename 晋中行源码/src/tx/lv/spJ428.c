/************************************************************
* 文 件 名:   spJ428.c
* 功能描述：  通存通兑账号开通协议
*
* 作    者:
* 完成日期：  2006-9-6
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "tctd_ktzh_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
spJ428()
{
	int ret=0;
	struct   tctd_ktzh_c   tctd_ktzh;
	struct   mdm_ac_rel_c  mdm_ac_rel;
	struct   dd_mst_c      dd_mst;
	char cz_type[2];	/**操作类型**/
	char kt_no[25];		/**开通帐号**/
	char kt_xyno[80];	/**开通协议号**/
	char yw_lx[8];		/**业务类型:通存标志+通兑标志+查询标志+0000**/
	double td_amt_lmt;	/**通兑金额限制**/
	char cCifType[2];
	char cCifNo[21];
	int  nInsFlag=0;
	memset(&tctd_ktzh,0x00 ,sizeof(struct tctd_ktzh_c));
	memset(&mdm_ac_rel,0x00 ,sizeof(struct mdm_ac_rel_c));
	memset(&dd_mst,0x00 ,sizeof(struct dd_mst_c));
	memset(cz_type,0x00 ,sizeof(cz_type));
	memset(kt_no,0x00 ,sizeof(kt_no));
	memset(kt_xyno,0x00 ,sizeof(kt_xyno));
	memset(yw_lx,0x00 ,sizeof(yw_lx));
	memset(cCifType,'\0',sizeof(cCifType));
	memset(cCifNo,'\0',sizeof(cCifNo));
	td_amt_lmt=0.0;	
	vtcp_log("[%s][%d]spJ428小额通存通兑协议BEGIN",__FILE__,__LINE__);
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/** 取得FD数据 **/
	get_zd_data( "0670",cz_type );
	get_zd_data( "0300",kt_no);
	get_zd_data( "0740",kt_xyno );
	get_zd_data( "0650",yw_lx ) ;
	get_zd_data( "0680",cCifType ) ;
	get_zd_data( "0620",cCifNo ) ;
	get_zd_double( "0390",&td_amt_lmt ) ;

	ret = Tctd_ktzh_Sel(g_pub_tx.reply,&tctd_ktzh,"tctd_xyno = '%s'",kt_xyno);
	if (ret!=100 && ret){
		vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
		set_zd_data("0130","查询数据错误");
		goto ErrExit;
	}else if(ret==100){
		nInsFlag=1;
	}
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",kt_no);
	if(ret){
		vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
		set_zd_data("0130","帐号未找到");
		goto ErrExit;
	}
	if(memcmp(mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no))!=0){
		vtcp_log("[%s][%d]只能在开户行做此业务[%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","只能在开户行做此业务");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d]证件类型[%s]号码不符[%s]",__FILE__,__LINE__,mdm_ac_rel.id_type,mdm_ac_rel.id_no);
	if(strcmp(mdm_ac_rel.id_type,cCifType)!=0 ||strcmp(mdm_ac_rel.id_no,cCifNo)!=0){
		vtcp_log("[%s][%d]证件类型或号码不符[%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","输入证件不符");
		goto ErrExit;
	}
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id=%d",mdm_ac_rel.ac_id);
	if(ret){
		vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
		set_zd_data("0130","帐号未找到");
		goto ErrExit;
	}
	if(dd_mst.ac_type[0]!='5'){
		vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
		set_zd_data("0130","账户类型不正确");
		goto ErrExit;
	}
	if(!nInsFlag){
		/****需要更新得情况****/
		ret=Tctd_ktzh_Dec_Upd(g_pub_tx.reply,"tctd_xyno='%s'",kt_xyno);
		if(ret){
			vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","查询数据错误");
			goto ErrExit;
		}
		ret=Tctd_ktzh_Fet_Upd(&tctd_ktzh,g_pub_tx.reply);
		if(ret){
			vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","查询数据错误");
			Tctd_ktzh_Clo_Upd();
			goto ErrExit;
		}
	}
	ret=GetOr_br_noByBr_no(g_pub_tx.tx_br_no,tctd_ktzh.tctd_br_no);
	if(ret){
		vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
		set_zd_data("0130","得到支付系统行号错误");
		goto ErrExit;
	}
	if(cz_type[0]=='1'){
		/**启用**/
		memcpy(tctd_ktzh.br_no,g_pub_tx.tx_br_no,sizeof(tctd_ktzh.br_no)-1);
		tctd_ktzh.begin_date=g_pub_tx.tx_date;
		tctd_ktzh.end_date=0;
		memcpy(tctd_ktzh.tctd_acno,kt_no,sizeof(tctd_ktzh.tctd_acno)-1);
		memcpy(tctd_ktzh.tctd_name,dd_mst.name,sizeof(dd_mst.name)-1);
		if(yw_lx[0]=='1'){
			tctd_ktzh.tc_kt_flag[0]='1';
		}else{
			tctd_ktzh.tc_kt_flag[0]='0';
		}
		if(yw_lx[1]=='1'){
			tctd_ktzh.td_kt_flag[0]='1';
		}else{
			tctd_ktzh.td_kt_flag[0]='0';
		}
		if(yw_lx[2]=='1'){
			tctd_ktzh.cx_kt_flag[0]='1';
		}else{
			tctd_ktzh.cx_kt_flag[0]='0';
		}
		if(tctd_ktzh.td_kt_flag[0]=='1'){
			tctd_ktzh.td_amt_lmt=td_amt_lmt;
		}else{
			tctd_ktzh.td_amt_lmt=0;
		}
		memcpy(tctd_ktzh.tctd_xyno,kt_xyno,sizeof(tctd_ktzh.tctd_xyno)-1);
		memcpy(tctd_ktzh.tel,g_pub_tx.tel,sizeof(tctd_ktzh.tel)-1);
		tctd_ktzh.trace_no=g_pub_tx.trace_no;
			
	}else{
		/**停用**/
		memcpy(tctd_ktzh.br_no,g_pub_tx.tx_br_no,sizeof(tctd_ktzh.br_no)-1);
		tctd_ktzh.end_date=g_pub_tx.tx_date;
		memcpy(tctd_ktzh.tctd_acno,kt_no,sizeof(tctd_ktzh.tctd_acno)-1);
		memcpy(tctd_ktzh.tctd_name,dd_mst.name,sizeof(dd_mst.name)-1);
		tctd_ktzh.tc_kt_flag[0]='0';
		tctd_ktzh.td_kt_flag[0]='0';
		tctd_ktzh.cx_kt_flag[0]='0';
		tctd_ktzh.td_amt_lmt=0;
		memcpy(tctd_ktzh.tctd_xyno,kt_xyno,sizeof(tctd_ktzh.tctd_xyno)-1);
		memcpy(tctd_ktzh.tel,g_pub_tx.tel,sizeof(tctd_ktzh.tel)-1);
		tctd_ktzh.trace_no=g_pub_tx.trace_no;
	}
	if(nInsFlag){
		ret=Tctd_ktzh_Ins(tctd_ktzh,g_pub_tx.reply);
		if(ret){
			vtcp_log("[%s][%d]插入数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","插入数据库错误");
			goto ErrExit;
		}
	}else{
		ret=Tctd_ktzh_Upd_Upd(tctd_ktzh,g_pub_tx.reply);
		if(ret){
			vtcp_log("[%s][%d]更新数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","更新数据库错误");
			Tctd_ktzh_Clo_Upd();
			goto ErrExit;
		}
		Tctd_ktzh_Clo_Upd();
	}
	set_zd_data("1230",tctd_ktzh.tctd_br_no);
	vtcp_log("[%s][%d]--------Mission completed!!!--------------",__FILE__,__LINE__);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;

}
