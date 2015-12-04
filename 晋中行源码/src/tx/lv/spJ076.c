/************************************************************
* 文 件 名:   spJ076.c
* 功能描述：  付款人开户行多方协议号维护
*
* 作    者:   chenhw
* 完成日期：  2006-9-7
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if( ret ) {\
 strcpy( g_pub_tx.reply,"P026" );\
 sprintf(acErrMsg,"SQLERR [%d]",ret);\
 WRITEMSG\
 goto ErrExit;\
 }

#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_newxyh_c.h"
#include "lv_define.h"
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"


static struct lv_newxyh_c g_lv_newxyh;
static char pcPayno[13];
static char pcName[61];
int iZeroToSp(char *str,int len);

 spJ076()
{
	int ret=0;

	char cBrno[6];
	char cTbsdy[9];
	char cTlrno[5];
	char cXyno[61];
	char cPayactno[20];
	char cPayactno_f[20];
	char cOptype[2];	
	char cPayname[61];
	char cCashxyno[61];
	char cCashno[13];
	char cCashactno[33];
	char cCashname[61];
	char cYwtype[6];
	char cYwlist[26];
	char buf[31];

	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;

	
	memset(cBrno,'\0',sizeof(cBrno));
	memset(cTbsdy,'\0',sizeof(cTbsdy));
	memset(cTlrno,'\0',sizeof(cTlrno));
	memset(cXyno,'\0',sizeof(cXyno));
	memset(cPayactno,'\0',sizeof(cPayactno));
	memset(cPayactno_f,'\0',sizeof(cPayactno_f));
	memset(cOptype,'\0',sizeof(cOptype));
	memset(cPayname,'\0',sizeof(cPayname));
	memset(cCashxyno,'\0',sizeof(cCashxyno));
	memset(cCashno,'\0',sizeof(cCashno));
	memset(cCashactno,'\0',sizeof(cCashactno));
	memset(cCashname,'\0',sizeof(cCashname));
	memset(cYwtype,'\0',sizeof(cYwtype));
	memset(cYwlist,'\0',sizeof(cYwlist));
	memset(pcPayno,'\0',sizeof(pcPayno));
	memset(pcName,'\0',sizeof(pcName));
	memset(buf,'\0',sizeof(buf));

	memset(&g_lv_newxyh ,0x00 ,sizeof(struct lv_newxyh_c));

	vtcp_log("[%s][%d]spJ076小额多方协议号维护交易BEGIN",__FILE__,__LINE__);

	/*** 初始化全局变量 ***/
        pub_base_sysinit();

        /** 取得FD数据 未配**/
	get_fd_data( "0030", &cBrno);           /** 取得交易机构 **/
	get_fd_data( "0050", &cTbsdy);		/** 取得交易日期 **/
	get_fd_data( "0070", &cTlrno);		/** 取得柜员号 **/
	get_fd_data( "0961", &cXyno);         /** 取得协议号 **/
	get_fd_data( "0370", &cPayactno_f);         /** 取得付款帐号 **/
	get_fd_data( "0670", &cOptype);         /** 取得操作类型 **/
	get_fd_data( "0962", &cPayname);         /** 取得付款户名 **/
	get_fd_data( "0963", &cCashxyno);         /** 取得收款协议号 **/
	get_fd_data( "0520", &cCashno);         /** 取得收款行号 **/
	get_fd_data( "0750", &cCashactno);         /** 取得收款帐号 **/
	get_fd_data( "0964", &cCashname);         /** 取得收款户名 **/
	get_fd_data( "0810", &buf);         /** 取得业务种类 **/

	memcpy(cYwtype,buf,sizeof(cYwtype)-1);
	memcpy(cYwlist,buf+5,sizeof(cYwlist)-1);
	memcpy(cPayactno,cPayactno_f,sizeof(cPayactno)-1);
	memcpy(pcName,cPayname,sizeof(cPayname)-1);
	zipn_space(pcName,sizeof(pcName)-1);

	vtcp_log("[%s][%d]spJ072[%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s]",__FILE__,__LINE__,cBrno,cXyno,cPayactno,cOptype,cPayname,cCashxyno,cCashno,cCashactno,cCashname,cYwtype,cYwlist);


	if(cOptype[0]=='1' || cOptype[0]=='2'){
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no=%s",cPayactno);
		if( ret )
		{
			sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",cPayactno);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
				g_mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		if( prdt_ac_id_c.ac_id_type[0]=='1')
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
                        		prdt_ac_id_c.ac_id,g_mdm_ac_rel.ac_seqn);
                	if( ret )
                	{
                        	sprintf(acErrMsg,"read dd_mst error" );
                        	WRITEMSG
                        	goto ErrExit;
                	}

			if(strcmp(pcName,g_dd_mst.name)!=0){
				sprintf(acErrMsg,"付款帐号户名不符!!! [%s][%s]",pcName,g_dd_mst.name);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P042");
				goto ErrExit;
			}
		}else{
			ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d", \
                        		prdt_ac_id_c.ac_id,g_mdm_ac_rel.ac_seqn);
                	if( ret )
                	{
                        	sprintf(acErrMsg,"付款帐号类型不符!!!" );
                        	WRITEMSG
				strcpy(g_pub_tx.reply,"P043");
                        	goto ErrExit;
                	}
			
			if(strcmp(pcName,g_in_mst.name)!=0){
				sprintf(acErrMsg,"付款帐号户名不符!!! [%s][%s]",pcName,g_in_mst.name);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P042");
				goto ErrExit;
			}
		}

		if(memcmp(cBrno,g_mdm_ac_rel.opn_br_no,sizeof(cBrno)-1) !=0){
                        sprintf(acErrMsg,"付款帐号非本行帐号!!!!" );
                        WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
                        goto ErrExit;
                }
	}

	ret = GetOr_br_noByBr_no(cBrno, pcPayno );
	if( ret!=0 ){
		sprintf(acErrMsg,"得到行号错误!!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P044");
		goto ErrExit;
	}

	switch(cOptype[0]){
	case '0':
		/***查询***/
		pub_base_rtrim(cXyno);
		ret = Lv_newxyh_Sel( g_pub_tx.reply,&g_lv_newxyh,
			"xyno='%s' and pay_ac_no=%s",cXyno,cPayactno);
		MYSQLERR

		if(memcmp(cBrno,LV_CZ_QSBRNO,sizeof(cBrno)-1)!=0){
			if(memcmp(cBrno,g_lv_newxyh.br_no,sizeof(cBrno)-1)){
				sprintf(acErrMsg,"付款帐号非本行帐号!!!!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"P044");
				goto ErrExit;
			}
		}
        	vtcp_log("[%s][%d]查询完毕-------",__FILE__,__LINE__);

		break;
	case '1':
		/***增加***/
		if(memcmp(cBrno,LV_CZ_QSBRNO,sizeof(cBrno)-1)==0) {
			sprintf(acErrMsg,"只有支行可开协议号明细!!!!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
			goto ErrExit;
		}
		memcpy(g_lv_newxyh.xyno,cXyno,sizeof(g_lv_newxyh.xyno));
		memcpy(g_lv_newxyh.pay_ac_no,cPayactno,sizeof(g_lv_newxyh.pay_ac_no));
		memcpy(g_lv_newxyh.pay_name,cPayname,sizeof(g_lv_newxyh.pay_name));
		memcpy(g_lv_newxyh.pay_no,pcPayno,sizeof(g_lv_newxyh.pay_no));
		memcpy(g_lv_newxyh.cashxyno,cCashxyno,sizeof(g_lv_newxyh.cashxyno));
		memcpy(g_lv_newxyh.cash_ac_no,cCashactno,sizeof(g_lv_newxyh.cash_ac_no));
		memcpy(g_lv_newxyh.cash_name,cCashname,sizeof(g_lv_newxyh.cash_name));
		memcpy(g_lv_newxyh.cash_no,cCashno,sizeof(g_lv_newxyh.cash_no));
		memcpy(g_lv_newxyh.yw_type,cYwtype,sizeof(g_lv_newxyh.yw_type));
		memcpy(g_lv_newxyh.ywlist,cYwlist,sizeof(g_lv_newxyh.ywlist));
		memcpy(g_lv_newxyh.br_no,cBrno,sizeof(g_lv_newxyh.br_no));
		g_lv_newxyh.last_date=atoi(cTbsdy);
		g_lv_newxyh.limitamt=99999999.00;/***不限制金额***/
		g_lv_newxyh.flag[0]='1';
		memcpy(g_lv_newxyh.tel,cTlrno,sizeof(g_lv_newxyh.tel));

		zipn_space(g_lv_newxyh.xyno,sizeof(g_lv_newxyh.xyno)-1);
		zipn_space(g_lv_newxyh.pay_ac_no,sizeof(g_lv_newxyh.pay_ac_no)-1);
		zipn_space(g_lv_newxyh.pay_name,sizeof(g_lv_newxyh.pay_name)-1);
		zipn_space(g_lv_newxyh.cashxyno,sizeof(g_lv_newxyh.cashxyno)-1);
		zipn_space(g_lv_newxyh.cash_ac_no,sizeof(g_lv_newxyh.cash_ac_no)-1);
		zipn_space(g_lv_newxyh.cash_name,sizeof(g_lv_newxyh.cash_name)-1);

		ret=Lv_newxyh_Ins(g_lv_newxyh,g_pub_tx.reply);
		MYSQLERR
        	vtcp_log("[%s][%d]查询完毕-------",__FILE__,__LINE__);

		break;
	case '2':
		/***修改***/
		if(memcmp(cBrno,LV_CZ_QSBRNO,sizeof(cBrno)-1)==0) {
			sprintf(acErrMsg,"只有支行可开协议号明细!!!!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
			goto ErrExit;
		}

		ret = Lv_newxyh_Dec_Upd(g_pub_tx.reply,
			"xyno=%s and pay_ac_no=%s",cXyno,cPayactno);
		MYSQLERR

		ret = Lv_newxyh_Fet_Upd(&g_lv_newxyh,g_pub_tx.reply);
		MYSQLERR

		if(memcmp(g_lv_newxyh.br_no,cBrno,sizeof(cBrno)-1)!=0){
			sprintf(acErrMsg,"只能由录入行修改!!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
			goto ErrExit;
		}

		memcpy(g_lv_newxyh.xyno,cXyno,sizeof(g_lv_newxyh.xyno));
		memcpy(g_lv_newxyh.pay_ac_no,cPayactno,sizeof(g_lv_newxyh.pay_ac_no));
		memcpy(g_lv_newxyh.pay_name,cPayname,sizeof(g_lv_newxyh.pay_name));
		memcpy(g_lv_newxyh.pay_no,pcPayno,sizeof(g_lv_newxyh.pay_no));
		memcpy(g_lv_newxyh.cashxyno,cCashxyno,sizeof(g_lv_newxyh.cashxyno));
		memcpy(g_lv_newxyh.cash_ac_no,cCashactno,sizeof(g_lv_newxyh.cash_ac_no));
		memcpy(g_lv_newxyh.cash_name,cCashname,sizeof(g_lv_newxyh.cash_name));
		memcpy(g_lv_newxyh.cash_no,cCashno,sizeof(g_lv_newxyh.cash_no));
		memcpy(g_lv_newxyh.yw_type,cYwtype,sizeof(g_lv_newxyh.yw_type));
		memcpy(g_lv_newxyh.ywlist,cYwlist,sizeof(g_lv_newxyh.ywlist));
		memcpy(g_lv_newxyh.br_no,cBrno,sizeof(g_lv_newxyh.br_no));
		g_lv_newxyh.last_date=atoi(cTbsdy);
		g_lv_newxyh.limitamt=99999999.00;/***不限制金额***/
		g_lv_newxyh.flag[0]='1';
		memcpy(g_lv_newxyh.tel,cTlrno,sizeof(g_lv_newxyh.tel));

		zipn_space(g_lv_newxyh.xyno,sizeof(g_lv_newxyh.xyno)-1);
		zipn_space(g_lv_newxyh.pay_ac_no,sizeof(g_lv_newxyh.pay_ac_no)-1);
		zipn_space(g_lv_newxyh.pay_name,sizeof(g_lv_newxyh.pay_name)-1);
		zipn_space(g_lv_newxyh.cashxyno,sizeof(g_lv_newxyh.cashxyno)-1);
		zipn_space(g_lv_newxyh.cash_ac_no,sizeof(g_lv_newxyh.cash_ac_no)-1);
		zipn_space(g_lv_newxyh.cash_name,sizeof(g_lv_newxyh.cash_name)-1);

		ret = Lv_newxyh_Upd_Upd(g_lv_newxyh,g_pub_tx.reply);
		MYSQLERR

		Lv_newxyh_Clo_Upd();
		break;
	case '3':
		/***删除***/
		if(memcmp(cBrno,LV_CZ_QSBRNO,sizeof(cBrno)-1)==0) {
			sprintf(acErrMsg,"只有支行可开协议号明细!!!!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
			goto ErrExit;
		}

		ret = Lv_newxyh_Dec_Upd(g_pub_tx.reply,
			"xyno=%s and pay_ac_no=%s",cXyno,cPayactno);
		MYSQLERR

		ret = Lv_newxyh_Fet_Upd(&g_lv_newxyh,g_pub_tx.reply);
		MYSQLERR

		if(memcmp(g_lv_newxyh.br_no,cBrno,sizeof(cBrno)-1)!=0){
			sprintf(acErrMsg,"只能由录入行删除!!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P044");
			goto ErrExit;
		}
		ret = Lv_newxyh_Del_Upd(g_lv_newxyh,g_pub_tx.reply);
		MYSQLERR

		Lv_newxyh_Clo_Upd();

		break;
	default:
		sprintf(acErrMsg,"非法的操作代码!!!!!" );
		WRITEMSG
		goto ErrExit;
	}	

	/* 记流水日志 */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		goto ErrExit;
	}

	memcpy(buf,g_lv_newxyh.yw_type,sizeof(cYwtype)-1);
	memcpy(buf+5,g_lv_newxyh.ywlist,sizeof(cYwlist)-1);

        /** 取得FD数据 未配**/
	set_zd_data( "0961", g_lv_newxyh.xyno);         /** 返回协议号 **/
	set_zd_data( "0370", g_lv_newxyh.pay_ac_no);         /** 返回付款帐号 **/
	set_zd_data( "0962", g_lv_newxyh.pay_name);         /** 返回付款户名 **/
	set_zd_data( "0963", g_lv_newxyh.cashxyno);         /** 返回收款协议号 **/
	set_zd_data( "0520", g_lv_newxyh.cash_no);         /** 返回收款行号 **/
	set_zd_data( "0750", g_lv_newxyh.cash_ac_no);         /** 返回收款帐号 **/
	set_zd_data( "0964", g_lv_newxyh.cash_name);         /** 返回收款户名 **/
	set_zd_data( "0810", buf);         /** 返回业务种类 **/

        vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

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

/*****子功能函数****/
int iZeroToSp(char *str,int len){
	int i,j;
	for(i=0;i<len;i++){
		if(str[i]=='\0'){
			for(j=i;j<len;j++){
				str[j]=' ';
			}
			break;
		}
	}
	return(0);
}
