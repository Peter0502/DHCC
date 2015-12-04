/*****************************************************
* �� �� ��:  sp9550.c
* ���������� ������ˮ��,���ҷ�¼��ˮ,�����õ�123����,��ʽ����:
*            ��Ŀ��(2)+��Ŀ1+��Ŀ2+...
*            ÿ����Ŀ�ĸ�ʽ����:
*            ƽ���־(1)+�跽��Ŀ(7)+�跽���(16,Ԫ)+������Ŀ(7)+�������(16,Ԫ)
* ��    ��:  SSH 
* ������ڣ� 2006��9��17�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "dc_log_c.h"
#include "com_item_c.h"
/****����ֵ:Y-��Ŀ����ƽ��,N-��Ŀ������ƽ��****/
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
 	vtcp_log("sp9550��ʼ��[%s][%d]",__FILE__,__LINE__);
	memset(&v_dc_log,'\0',sizeof(v_dc_log));
	memset(pcAcData,'\0',sizeof(pcAcData));
	memset(pcAcno,'\0',sizeof(pcAcno));
	pub_base_sysinit();
	get_zd_long(DC_TRACE_NO,&v_dc_log.trace_no);
	ret=Dc_log_Dec_Sel(g_pub_tx.reply,"tx_date=%d and trace_no=%d order by dc_ind ",
			g_pub_tx.tx_date,v_dc_log.trace_no);
	if(ret){
		set_zd_data(DC_GET_MSG,"�޷�¼��Ϣ");
		goto ErrExit;
	}
	while(1){
		memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));	
		memset(pcAcno,'\0',sizeof(pcAcno));	
		ret=Dc_log_Fet_Sel(&v_dc_log,g_pub_tx.reply);
		if(ret && ret!=100){
			set_zd_data(DC_GET_MSG,"�޷�¼��Ϣ");
			Dc_log_Clo_Sel();
			goto ErrExit;
		}
		if(ret==100){
			break;
		}
		cEqua_ind=iGetAcno(v_dc_log.acc_hrt,pcAcno);
		/***���з�¼����һ��***/
		memset(pcAcData+2+cnt*47,' ',47);
		pcAcData[2+cnt*47]=cEqua_ind;

		sprintf(pcTmpBuf,"%-7s%016.2f",pcAcno,v_dc_log.amt);
		if(v_dc_log.dc_ind[0]=='2'){
			/****����****/
			memcpy(pcAcData+2+cnt*47+24,pcTmpBuf,23);
		}else{
			/****�跽****/
			memcpy(pcAcData+2+cnt*47+1,pcTmpBuf,23);
		}
		cnt++;
	}
	Dc_log_Clo_Sel();
	/****��¼����****/
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

