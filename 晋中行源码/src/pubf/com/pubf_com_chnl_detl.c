/************************************************************************
* �� �� ��:  pubf_com_chnl_detl.c
* ���������� �����������Ǽ���ؽ�����ϸ��Ϣ
* ��  ��:    chenggx
* ������ڣ� 20130325
* ��    ���� ������õ����ݴ� ���ʼ���ͨ��125�򴫣��������˷ŵ���ϸ���
* �����޸ģ� Modified by YangMeng 20130425 
*************************************************************************/
#include "find_debug.h" 
#include <stdio.h>
#include "public.h"
#include "chnl_tx_detl_c.h"

int strsplit(char *str,char *des,int k);

int pubf_com_checkin_txdetail(char *sDetail)
{
	int iRet = 0;
	char *ps=NULL;
	char sTmp[128] = {0};
	struct chnl_tx_detl_c stChnl_tx_detl;
	memset(sTmp,0x00,sizeof(sTmp));
	memset(&stChnl_tx_detl,0x00,sizeof(stChnl_tx_detl));/**/
	
	strcpy(stChnl_tx_detl.syscode,g_pub_tx.tty);
	strcpy(stChnl_tx_detl.tx_code,g_pub_tx.tx_code);
	stChnl_tx_detl.tx_date = g_pub_tx.tx_date;
	/*stChnl_tx_detl.tx_time = g_pub_tx.tx_time;*/
	sprintf(stChnl_tx_detl.tx_time,"%6ld", g_pub_tx.tx_time);    /** ϵͳʱ��  **/
	stChnl_tx_detl.trace_no = g_pub_tx.trace_no;
	stChnl_tx_detl.trace_cnt = g_pub_tx.trace_cnt;/**/
	
	/*ѭ��ȡ��õĴ��е����� ���ṹ�帳ֵ*/
	/*ƽ̨����|ƽ̨��ˮ|ί������|֧�����|�������к�|������ʾ|�����˺������˻�|���˺�������־ 0�� 1��|��ת��־ 1�� 2ת|���׽��|�Է����к�|�Է��˺�|�Է�����|�Է�id|�Է�֤������|ҵ�����ͱ��|ҵ������|�������ͣ�1:������ 2:ũ���� 3:����֧�� 4:��������ƽ̨ 5:����ũ����*/
	ps = sDetail;
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,0);
	zip_space(sTmp);
	stChnl_tx_detl.pt_date = atoi(sTmp); /*ƽ̨����*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,1);
	zip_space(sTmp);
	stChnl_tx_detl.pt_trace_no=atoi(sTmp); /*ƽ̨��ˮ*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,2);
	zip_space(sTmp);
	stChnl_tx_detl.wt_date=atoi(sTmp); /*ί������*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,3);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.orderno,sTmp); /*֧�����*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,4);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.or_br_no,sTmp); /*�������к�*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,5);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.sndrcvflag,sTmp); /*������ʾ*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,6);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.ac_no,sTmp); /*�����˺� �����˻�*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,7);
	vtcp_log("[%s][%d],�˻�����ac_name=[%s]",__FILE__,__LINE__,sTmp);
	zip_space(sTmp);
	vtcp_log("[%s][%d],�˻�����ac_name=[%s]",__FILE__,__LINE__,sTmp);
	vtcp_log("[%s][%d],�˻���������strlen=[%d]",__FILE__,__LINE__,strlen(sTmp));
	memcpy(stChnl_tx_detl.ac_name,sTmp,strlen(sTmp)+1); /*�Է�����*/
	vtcp_log("[%s][%d],�˻�����ac_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.ac_name);
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,8);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.add_ind,sTmp); /*���˺�������־ 0�� 1��*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,9);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.ct_ind,sTmp); /*��ת��־ 1�� 2ת */
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,10);
	zip_space(sTmp);
	stChnl_tx_detl.tx_amt = strtod(sTmp,NULL); /*���׽��*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,11);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_br_no,sTmp); /*�Է����к�*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,12);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_ac_no,sTmp); /*�Է��˺�*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,13);
	vtcp_log("[%s][%d],�Է�����df_name=[%s]",__FILE__,__LINE__,sTmp);
	zip_space(sTmp);
	vtcp_log("[%s][%d],�Է�����df_name=[%s]",__FILE__,__LINE__,sTmp);
	vtcp_log("[%s][%d],�Է����Ƴ���strlen=[%d]",__FILE__,__LINE__,strlen(sTmp));
	memcpy(stChnl_tx_detl.df_name,sTmp,strlen(sTmp)+1); /*�Է�����*/
	vtcp_log("[%s][%d],�Է�����df_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.df_name);
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,14);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_id_type,sTmp); /*�Է�id*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,15);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_id_no,sTmp); /*�Է�֤������*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,16);
	zip_space(sTmp);
	memcpy(stChnl_tx_detl.txnum,sTmp,4); /*ҵ�����ͱ��*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,17);
	zip_space(sTmp);
	memcpy(stChnl_tx_detl.yw_type,sTmp,5); /*ҵ������*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,18);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.chnl_type,sTmp); /*�������ͣ�1:������ 2:ũ���� 3:����֧�� 4:��������ƽ̨ 5:����ũ����*/
	strcpy(stChnl_tx_detl.dz_ind,"0");/*����״̬*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,19);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.filler1,sTmp);/*filler1���ڵǼǸ���*/
	/*���������˺Ż��� add by xk 2014/6/13 10:38:34 begin*/
	/*ͨ��ͨ���ֽ���ʱ �����˺�Ϊ0*/
	if(strcmp(stChnl_tx_detl.ac_no,"0")!=0 && stChnl_tx_detl.chnl_type[0]=='3'){
		iRet = pub_base_acnogetname(stChnl_tx_detl.ac_no,stChnl_tx_detl.ac_name);
		if(iRet)
		{
			sprintf(acErrMsg,"�����˺Ų��һ���ʧ��!!!");
			WRITEMSG
			/*strcpy(g_pub_tx.reply, "D104");
			goto ErrExit;�˴�����Ҫ��Ӵ����������Ϊ���˵�ʱ���˺��п�����������룬ȥ������*/
		}
		vtcp_log("[%s][%d],stChnl_tx_detl.ac_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.ac_name);
	}
	/*���������˺Ż��� add by xk 2014/6/13 10:38:34 end*/
	
	Chnl_tx_detl_Debug(&stChnl_tx_detl);
	
	iRet = Chnl_tx_detl_Ins(stChnl_tx_detl,g_pub_tx.reply);
	if(iRet)
	{
		sprintf(acErrMsg,"�Ǽ�������������ϸ�Ǽǲ�ʧ��!!!");
		WRITEMSG
		Chnl_tx_detl_Debug(&stChnl_tx_detl);
		strcpy(g_pub_tx.reply, "D107");
		goto ErrExit;
	}
	
OKExit:
	return 0;
ErrExit:
	return 1;
}

/**************************************************************************
* FUNCTION  : ȡ�ض�λ���ַ���
* Input     : ԭ����Ŀ�괮��λ��
* OUTPUT    :
* RETURN    : 0(�ɹ�)   ��-1(ʧ��)
**************************************************************************/
int strsplit(char *str,char *des,int k)
{	
	if(str == NULL || des == NULL)
	{
		return -1;	
	}
	int i=0;
	int t=0; /*��¼��һ��λ��*/
	int j=0;
	while(*(str+i)!='\0')
	{
		if(*(str+i)=='|')
		{
			if(k==j++)
			{
				memcpy(des,str+t,i-t);	
			}	
			t=i+1;
		}	
		i++;
	}
	return 0;
}

