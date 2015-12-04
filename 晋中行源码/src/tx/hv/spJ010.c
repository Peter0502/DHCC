/*************************************************
* �� �� ��:  spJ010.c
* ���������� ���л�Ʊ�����˻ر���¼��
* ��    ��:  xyy 
* ������ڣ� 2006��8��17��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hv_addit_c.h"
#include "hv_poinfo_c.h"
#define loc_t sys_loc_t
#include "hvpack.h"
#include "find_debug.h"
int iGetHvAddid(char * addid);
int spJ010()
{
	int ret=0;	          
	char  cPassed[11];
	char  cPjnum[9];
	char  cTxbank[13];
	char  cFilename[100];
	char  cBuff	[2000];
	char cOrderno[9];
	memset(cOrderno, 0 , sizeof(cOrderno));
	FILE  *fpr;
	HV_724_ADD    hv_724_add;
	THVPACKAGE   pack724;
	HV_NOTPAY_AREA hv_notpay;
	struct hv_addit_c hv_addit;	
	struct hv_fzf_c hv_fzf;
	struct hv_poinfo_c hv_poinfo;

	memset(cFilename, 0, sizeof(cFilename));
	memset(cPassed,0,sizeof(cPassed)); 
	memset(cBuff  ,0,sizeof(cBuff));
	memset(cTxbank,0,sizeof(cTxbank));
	memset(cPjnum,0,sizeof(cPjnum));
	memset(&hv_addit,0,sizeof(hv_addit));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_poinfo,0,sizeof(hv_poinfo));
	memset(&hv_724_add,0,sizeof(hv_724_add));
	memset(&pack724, 0 , sizeof(pack724));

	vtcp_log("spJ010�[%s][%d]",__FILE__,__LINE__);

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/***�õ����֧��ҵ��8583����***/
	g_reply_int = iHvTistoNotpay(&hv_notpay);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"����iHvTistoNotpay����![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	/*begin add by LiuHuafeng 2006-10-2 22:33 ������������ */
	g_reply_int=iGetHvOrderno(cOrderno);
	if(g_reply_int)
	{
		vtcp_log("%s,%d get orderno error sqlcode=%d",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "P018" );
		sprintf( acErrMsg,"�õ�������Ŵ���![%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	MEMCPY_DEBUG(hv_notpay.F_orderno,cOrderno,sizeof(hv_notpay.F_orderno));
	/* end by LiuHuafeng 2006-10-2 22:44 */
	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����iHvfFrontToTable����![%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P018" );
		WRITEMSG
		goto ErrExit;
	}
	MEMCPY_DEBUG((char *)&hv_724_add,cBuff,sizeof(HV_724_ADD));
	MEMCPY_DEBUG(cPjnum,hv_724_add.pj_num,sizeof(cPjnum)-1);
	vtcp_log(" [%s][%d]cPjnum=[%s]",__FILE__,__LINE__,cPjnum);
	/*�õ��û�Ʊ�Ļ�����Ϣ*/			
	vtcp_log("[%s][%d]�����α�Hv_poinfo_Dec_Sel",__FILE__,__LINE__);
	MEMCPY_DEBUG(cTxbank,hv_notpay.F_or_br_no,sizeof(cTxbank)-1);
	g_reply_int =  Hv_poinfo_Dec_Sel ( g_pub_tx.reply ,"po_no='%s' and pay_br_no ='%s' ",cPjnum,cTxbank);
	if( g_reply_int){
		sprintf(acErrMsg,"�����α����![%s][%d]",__FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"�����α����");
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		return (NULL);
	}
	vtcp_log("[%s][%d]�����α�Hv_poinfo_Dec_Sel OVER",__FILE__,__LINE__);
	g_reply_int = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
	vtcp_log("[%s][%d]g_reply_int=========================[%d]",__FILE__,__LINE__,g_reply_int);
	if( g_reply_int){
		sprintf( acErrMsg,"FECHTһ����¼����[%s][%d]",__FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"FECHTһ����¼����");
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;		
	}
	if (hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER && hv_poinfo.po_sts[0]!=HV_HP_ASKCASH)
	{
		sprintf(acErrMsg,"�����ƴ�״̬�Ļ�Ʊ���������˻�[%s][%d]po_sts=[%c]!"
			,__FILE__,__LINE__,hv_poinfo.po_sts[0]);
		set_zd_data(DC_GET_MSG,"�����ƴ�״̬�Ļ�Ʊ���������˻�");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;                                                      
	}
	MEMCPY_DEBUG(cPassed,hv_724_add.hp_miya,sizeof(cPassed)-1);
	/*�ж���Ѻ�Ϸ��� */      
	if (memcmp(cPassed,hv_poinfo.pwd,sizeof(cPassed)-1)!=0)
	{
		sprintf(acErrMsg,"�������Ѻ��ԭ��Ʊ��Ѻ����[%s][%d]!",__FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"�������Ѻ��ԭ��Ʊ��Ѻ����");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;                                                       
	}
	hv_poinfo.po_sts[0] = HV_HP_ASKRETURN;
	vtcp_log("[%s][%d]��ʼ����Hv_poinfo_Upd_Upd",__FILE__,__LINE__);
	g_reply_int = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);	
	vtcp_log("[%s][%d]����Hv_poinfo_Upd_Upd����[%d]",__FILE__,__LINE__,g_reply_int);
	if(g_reply_int){   
		sprintf(acErrMsg,"���»�Ʊ������Ϣ�����[%s][%d][%d]!",__FILE__,__LINE__,g_reply_int);
		set_zd_data(DC_GET_MSG,"���»�Ʊ������Ϣ�����");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	Hv_poinfo_Clo_Upd( );
	vtcp_log("[%s][%d]iHvUpdFzfRecFromPayin",__FILE__,__LINE__);
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvUpdFzfRecFromPayin����[%s][%d]",__FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"���ú���iHvUpdFzfRecFromPayin����");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	/*** ��ʼд��ʷ��ϸ ***/ 

	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����֧����ϸ�����,[%s][%d]", __FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"�����֧����ϸ�����");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	/*¼�뽻��Ҫд��ˮ*/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"���ͽ���д��ˮ����!\n");
		set_zd_data(DC_GET_MSG,"�����֧����ϸ�����");
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	/****Add by SSH,2006.8.28,ֱ�ӷ���****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	pub_com_CMT724_T2TAG(&hv_notpay,&pack724);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	ret=hv_gen_pack(&pack724);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack724,hv_notpay.F_tx_br_no,hv_notpay.F_tel,
		hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack724)){
		sprintf(acErrMsg,"����724���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*************************************/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
