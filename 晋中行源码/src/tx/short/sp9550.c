/*****************************************************
* 文 件 名:  sp9550.c
* 功能描述： 根据流水号,查找分录流水,并放置到123域中,格式如下:
*            条目数(2)+条目1+条目2+...
*            每个条目的格式如下:
*            平衡标志(1)+借方科目(7)+借方金额(16,元)+贷方科目(7)+贷方金额(16,元)
* 作    者:  SSH 
* 完成日期： 2006年9月17日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "dc_log_c.h"
#include "com_item_c.h"
/****返回值:Y-科目参与平衡,N-科目不参与平衡****/
static char iGetAcno(char *acc_hrt,char *acno){
	int iRc;
	char pcResp[5];
	char pcTmpBuf[8];
	struct com_item_c com_item;
	memset(&com_item,'\0',sizeof(com_item));
	iRc=Com_item_Sel(pcResp,&com_item,"acc_hrt='%.5s'",acc_hrt);
	if(iRc){
		sprintf(pcTmpBuf,"[%-5d]",apatoi(acc_hrt,5));
		memcpy(acno,acc_hrt,7);
		return('N');
	}else{
		sprintf(pcTmpBuf,"%-7s",com_item.acc_no);
		memcpy(acno,com_item.acc_no,sizeof(com_item.acc_no)-1);
	}
	if(com_item.equa_ind[0]=='Y'){
		return('Y');	
	}else{
		return('N');
	}
}

int sp9550()
{
	int ret=0;
	int iDcSeqn=-1;
	int cnt=0;
	char cEqua_ind=0;
	char pcAcData[1024];
	char pcTmpBuf[32];
	char pcAcno[8];
	struct dc_log_c v_dc_log;    
 	vtcp_log("sp9550开始！[%s][%d]",__FILE__,__LINE__);
	memset(&v_dc_log,'\0',sizeof(v_dc_log));
	memset(pcAcData,'\0',sizeof(pcAcData));
	memset(pcAcno,'\0',sizeof(pcAcno));
	pub_base_sysinit();
	get_zd_long(DC_TRACE_NO,&v_dc_log.trace_no);
	ret=Dc_log_Dec_Sel(g_pub_tx.reply,"tx_date=%d and trace_no=%d order by dc_ind ",
			g_pub_tx.tx_date,v_dc_log.trace_no);
	if(ret){
		set_zd_data(DC_GET_MSG,"无分录信息");
		goto ErrExit;
	}
	while(1){
		memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));	
		memset(pcAcno,'\0',sizeof(pcAcno));	
		ret=Dc_log_Fet_Sel(&v_dc_log,g_pub_tx.reply);
		if(ret && ret!=100){
			set_zd_data(DC_GET_MSG,"无分录信息");
			Dc_log_Clo_Sel();
			goto ErrExit;
		}
		if(ret==100){
			break;
		}
		cEqua_ind=iGetAcno(v_dc_log.acc_hrt,pcAcno);
		/***所有分录另起一行***/
		memset(pcAcData+2+cnt*47,' ',47);
		pcAcData[2+cnt*47]=cEqua_ind;

		sprintf(pcTmpBuf,"%-7s%016.2f",pcAcno,v_dc_log.amt);
		if(v_dc_log.dc_ind[0]=='2'){
			/****贷方****/
			memcpy(pcAcData+2+cnt*47+24,pcTmpBuf,23);
		}else{
			/****借方****/
			memcpy(pcAcData+2+cnt*47+1,pcTmpBuf,23);
		}
		cnt++;
	}
	Dc_log_Clo_Sel();
	/****分录笔数****/
	apitoa(cnt,2,pcAcData);	
	set_zd_data("1230",pcAcData);
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

