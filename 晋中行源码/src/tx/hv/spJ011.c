/*************************************************
* �� �� ��:  spJ011.c
* ���������� ��Ʊ�������㱨��¼��
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
int spJ011()
{
	char  cFlag[2];
	char cBuff	[2000];
	char cOrderno[9];
	HV_721_ADD    hv_721_add;
	THVPACKAGE pack721;
	HV_NOTPAY_AREA hv_notpay;
	int ret=0;	
	struct hv_poinfo_c hv_poinfo;
	struct hv_fzf_c hv_fzf;
	memset(cOrderno, 0 , sizeof(cOrderno));
	memset(cBuff  ,0,sizeof(cBuff));
	memset(&hv_poinfo, 0 ,sizeof(hv_poinfo));
	memset(cFlag,0,sizeof(cFlag));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_poinfo,0,sizeof(hv_poinfo));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&hv_721_add,0,sizeof(hv_721_add));
	memset(&pack721, 0 , sizeof(pack721));
	
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
	/*begin add by LiuHuafeng 2006-10-2 22:33 ������������ 
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
	*/
	/* end by LiuHuafeng 2006-10-2 22:44 */
	vtcp_log("[%s][%d]orderno=====[%s]",__FILE__,__LINE__,hv_notpay.F_orderno);
	/******���Ӫҵ��״̬******/ 
	cFlag[0] = hv_notpay.F_hp_add_pwd[0];
	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"����״̬������![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]������iHv_checkstat",__FILE__,__LINE__);
	
	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����iHvfFrontToTable����![%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P018" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]   cBuff  =[%s]",__FILE__,__LINE__,cBuff);
	MEMCPY_DEBUG((char *)&hv_721_add,cBuff,sizeof(HV_721_ADD));/*�õ��ļ��е�����*/
	vtcp_log("[%s][%d]   hv_721_add  =[%s]",__FILE__,__LINE__,&hv_721_add);
		
	g_reply_int = cIns_hp(hv_notpay,hv_721_add);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"��cIns_hp����[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/******************************
	*    ���濪ʼ�����Ʊ����     *
	******************************/	
	
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"��iHvUpdFzfRecFromPayin����[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/*** ��ʼд��ʷ��ϸ ***/ 
	   
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����Ʊ����������ϸ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/*¼�뽻��Ҫд��ˮ*/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"���ͽ���д��ˮ����!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****Add by SSH,2006.8.28,ֱ�ӷ���****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	pub_com_CMT721_T2TAG(&hv_notpay,&pack721);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	ret=hv_gen_pack(&pack721);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack721,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack721)){
		sprintf(acErrMsg,"����721���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}

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
