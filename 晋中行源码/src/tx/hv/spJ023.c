/*************************************************
* �� �� ��:  spJ023.c
* ���������� ���ɸ�ʽ����¼��,�˽���Ϊ��������ĳ�в�ѯϣ��֪������Ϣ
* ��    ��:  xyy 
* ������ڣ� 2006��8��18��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:SSH
* �޸�����:¼���ͬʱ����
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#define loc_t sys_loc_t
#include "find_debug.h"

int iGetHvAddid(char * addid);
int spJ023()
{
	int  	iOrderno=0;
	int     flag=0;
	char  	cTxbank[13];
	char  	cFlag[2];        
	char 	cBuff	[2000];
	char    Cashno   [13];  /** �������к� **/
	char    cContent [512];
	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack303;
	int ret=0;	
	HV_FD123_AREA fd123;
	
	struct hv_fzf_c hv_fzf;
	struct hv_uniontab_c hv_uniontab;
	memset(cTxbank		,0,sizeof(cTxbank));
	memset(cFlag		,0,sizeof(cFlag));
	memset(cBuff		,0,sizeof(cBuff));
	memset(&hv_fzf		,0,sizeof(hv_fzf));
	memset(&hv_uniontab	,0,sizeof(hv_uniontab));
	memset(&hv_notpay, 0 , sizeof(hv_notpay));
   	memset(Cashno		,0, sizeof(Cashno ) );
	memset(cContent,	0x00,	sizeof(cContent));

	memset(&fd123,0x00,sizeof(fd123));

	memset(&pack303, 0 , sizeof(pack303));
	get_zd_data("0680",cFlag);	/*ѡ���ʶ��0-�����кš�1-�ձ�����*/

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);

	/***�õ����֧��ҵ��8583����***/
	vtcp_log("[%s][%d]���ɸ�ʽ����ʼ�spJ023",__FILE__,__LINE__);

	g_reply_int = iHvTistoNotpay(&hv_notpay);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("[%s][%d]������iHvTistoNotpay����",__FILE__,__LINE__);
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);

	/******���Ӫҵ��״̬******/ 

	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"����״̬������![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"���֧��ϵͳ��ǰ״̬�޷����˽���");
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]������iHv_checkstat",__FILE__,__LINE__);
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);


	/** У�鷢�𡢽����к��Ƿ���� **/
	memcpy(Cashno,hv_notpay.F_ac_br_no, sizeof(Cashno) -1 );
	
	if(cFlag[0]=='0')
	{
		if(strlen(Cashno)==0)
		{
			sprintf( acErrMsg,"�������кŲ���Ϊ��,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"�������кŲ���Ϊ��");
			WRITEMSG
			goto ErrExit;
		}
	}
       
       
	GetOr_br_noByBr_no(hv_notpay.F_tx_br_no,cTxbank);/*���ݻ��������к�*/
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);

	/** ������ **/
	if(cFlag[0]=='0') {
		g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no ='%s' ",Cashno);       	
		if (g_reply_int)
		{
			sprintf( acErrMsg,"���������г���,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"���������г���");
			WRITEMSG
			goto ErrExit;
		}
	        
		if(hv_uniontab.br_sts[0]!= '1') {
			sprintf( acErrMsg,"������״̬��,[%s][%d]", __FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"������״̬��");
			goto ErrExit;
		}
		if(memcmp(hv_uniontab.qs_no, HV_QSBRNO,sizeof(cTxbank) -1 ) == 0) {
			sprintf(acErrMsg,"������[%.12s]�뷢����[%.12s]����ͬ��һ��������[%s][%d]!",hv_uniontab.qs_no,HV_QSBRNO,__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"�������뷢���в���ͬ��һ��������");
			goto ErrExit;
		}
	}
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	vtcp_log(" [%s][%d]hv_notpay->F_content==[%s] ",__FILE__,__LINE__,hv_notpay.F_content);
	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);

	g_reply_int=iCharacterType(hv_notpay.T_req_content,sizeof(hv_notpay.T_req_content),'g');
    if(g_reply_int)
    {
        vtcp_log("%s,%d,����ĸ�����[%.60s]��%dλ���ڷǷ��ַ�",__FILE__,__LINE__,hv_notpay.T_req_content,g_reply_int);
        strncpy(acErrMsg,"����ĸ����д��ڷǷ��ַ�",50);
		set_zd_data("0130","���븽�����зǷ��ַ�");
		goto ErrExit;
    }

	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);	
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P018");
		goto ErrExit;
	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	find_qs_no(NULL,hv_notpay.T_pay_qs_no);
	if(cFlag[0]=='0')
	{

	     if((find_qs_no(hv_notpay.T_ac_br_no,hv_notpay.T_cash_qs_no))==NULL)
	     {
	        sprintf(acErrMsg,"û���ҵ������������к�[%s][%d]!",__FILE__,__LINE__);
    	 	WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"û���ҵ������������к�");
    	 	goto ErrExit;
	     }

	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	find_sendco(NULL,hv_notpay.T_receco);

	if(cFlag[0]=='0')
	{
		if((find_sendco(hv_notpay.F_ac_br_no,hv_notpay.T_receco))==NULL)
		{
			sprintf(acErrMsg,"δ�ҵ��ձ����Ĵ���[%s][%d]!",__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"δ�ҵ��ձ����Ĵ���");
			goto ErrExit;
		}
	}
	else
	  memcpy(hv_notpay.T_receco,hv_notpay.F_ac_br_no,sizeof(hv_notpay.T_receco));
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log(" [%s][%d] ",__FILE__,__LINE__);
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;/*����״̬*/
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	vtcp_log(" [%s][%d] ",__FILE__,__LINE__);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvUpdFzfRecFromPayin����,[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	/*** ��ʼд��ʷ��ϸ ***/ 
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	if (g_reply_int)
	{
	   sprintf( acErrMsg,"�������ɸ�ʽ��ϸ�����,[%s][%d]",__FILE__,__LINE__);
	   strcpy( g_pub_tx.reply, "HV01");
	   WRITEMSG
	   goto ErrExit;
	}
	/****Add by SSH,2006.8.28,ֱ�ӷ���****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);

	memcpy(cContent,hv_notpay.T_req_content,sizeof(hv_notpay.T_req_content));

	pub_com_CMT303_T2TAG(&hv_notpay,&pack303);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	ret=hv_gen_pack(&pack303);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack303,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack303)){
		sprintf(acErrMsg,"����303���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*************************************/

	/***����һ�¸���,���ǰ̨��ӡ�������������***/
	deal_half_charcter(cContent);
    vtcp_log("[%s]\n",cContent);
    memcpy(fd123.notpay_content,cContent,strlen(cContent));
    memcpy(fd123.orderno,hv_notpay.T_orderno,sizeof(fd123.orderno));
	
OkExit:
    pub_base_full_space((char *)&fd123,sizeof(fd123));
    set_zd_data("1230",(char *)&fd123);
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}

/****�����ڸ����д��ڻ��з��źͻس����ŵ����****/
int   myfunxier(char *source,char *target)
{
    int i = 0;
    int j = 0;
    vtcp_log("[%s][%d]������ַ���source==[%s]\n",__FILE__,__LINE__,source);
    while(source[i]!=0x00)
    {
    	
        if((int)source[i]==13)/**13��C�еĻ��з���**/
            i = i+2;
        else if((int)source[i]==10)/**10��ʾ���ǻس�����**/
            i = i+1;
        else
        {
            target[j] = source[i];
            i++;
            j++;
        }
    }
    target[j] = 0x00;/**�����***/
    
    vtcp_log("[%s][%d]����õ��ַ���target==[%s]\n",__FILE__,__LINE__,target);
    return 0;
}
int deal_half_charcter(char *_buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		char buf[512];

		memset(buf,0x00,sizeof(buf));
		
		
	    myfunxier(_buf,buf);
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		memcpy(_buf,cBuf,sizeof(cBuf));
		return 0;
}
