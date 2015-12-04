/*************************************************************************
*�ļ�����:spJ112.c
*��������:С����ܶ��ʽ���
*��������:2011-05-09
*��    ��:shaoliguang
*˵��:���е������������ݵĹ���
insert into tx_def values('5385','��������С������ļ�','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','6','1','3');
insert into tx_flow_def values('5385','0','J112','#$');
insert into tx_sub_def values('J112','��������С������ļ�','spJ112','0','0');
*************************************************************************/
#include <stdio.h>
#define EXTERN
#include <string.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_sysctl_c.h"
#include "lv_chk842_m_c.h"
#include "lv_chk842_d_c.h"
#include "lv_define.h"
#include "syyh_czyh_qs_reg_c.h"
#include "hv_orno_brno_c.h"


spJ112()
{
	struct hv_orno_brno_c sHv_Or_Brno;
	char cBasename[100];
	char cFullname[100];
	char cCmd[300];
	char cTx_br_no[6];
	long lTx_date=0;	/*��������*/
	int  ret=0;
	int iCnt;
	
	
	/***��ʼ��ȫ�ֱ���***/
	pub_base_sysinit();
	
	get_zd_long("0460",&lTx_date);
	get_zd_data("0910",cTx_br_no);
	
	vtcp_log("[%s][%d]ǰ̨����ֵ[%ld][%s]",__FILE__,__LINE__,lTx_date,cTx_br_no);
	/*����ǰ̨���������ںϷ���*/
	ret=pub_base_dayvalicheck(lTx_date);
	if(ret)
	{
		sprintf(acErrMsg,"���ڲ��Ϸ�![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"CUD2");
		goto ErrExit;
	}

	/*ȡ�ļ�·��*/
	sprintf(cFullname,"%s/",(char*)getenv("YQSH_PATH"));
	/****
	sprintf(cCmd,"ftpsend %s",cFullname);
	****/
	
	vtcp_log("[%s][%d]fullname=[%s]",__FILE__,__LINE__,cFullname);
	vtcp_log("[%s][%d]cCmd=[%s]",__FILE__,__LINE__,cCmd);
	
	if(memcmp(cTx_br_no,"aaaaa",5)==0)/***��ȫ�������ڵ��ļ�**/
	{
		ret=Hv_orno_brno_Dec_Sel(g_pub_tx.reply,"br_no like  '9999_' order by  br_no");
		if(ret){
			sprintf(acErrMsg,"�α궨�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"B867");
			goto ErrExit;
		}
		iCnt=0;
		while(1)
		{
			memset(&sHv_Or_Brno,0x00,sizeof(struct hv_orno_brno_c));
			ret=Hv_orno_brno_Fet_Sel(&sHv_Or_Brno,g_pub_tx.reply);
			if(ret==100 && iCnt>=0){
				sprintf(acErrMsg,"δ��ѯ����¼cnt=[%d]ret=[%d]",iCnt,ret);
				WRITEMSG
				break;
			}else if(ret){
				sprintf(acErrMsg,"���Ҵ������г���[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d][%s]�������п�ʼ���ļ�!!",__FILE__,__LINE__,sHv_Or_Brno.br_no);
			/*�ѻ�����׷�ӵ�·��*/
			sprintf(cBasename,"lv_xc_dz_%s_%08ld.txt",sHv_Or_Brno.br_no,lTx_date);
			/**** 20110513
			strcat(cCmd,cBasename);
			****/
			sprintf(cCmd,"ftpsend %s %s",cFullname,cBasename);
			vtcp_log("[%s][%d]cCmd=[%s]",__FILE__,__LINE__,cCmd);
			
			ret=system(cCmd);
			if(ret)
			{
				vtcp_log("%s,%d�´������ļ�����������ʧ��",__FILE__,__LINE__);
				strncpy(g_pub_tx.reply,"P157",4);
				goto ErrExit;
			}
			iCnt++;
			
		}
		Hv_orno_brno_Clo_Sel();

		set_zd_long("0730",iCnt);
		vtcp_log("[%s][%d]  cnt =%d",__FILE__,__LINE__,iCnt);
	}
	else
	{
		memset(&sHv_Or_Brno,0x00,sizeof(struct hv_orno_brno_c));
		ret=Hv_orno_brno_Sel(g_pub_tx.reply,&sHv_Or_Brno,"br_no='%s'",cTx_br_no);
		if(ret){
			sprintf(acErrMsg,"�α궨�����!");
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d][%s]�������п�ʼ���ļ�!!",__FILE__,__LINE__,sHv_Or_Brno.br_no);
		/*�ѻ�����׷�ӵ�·��*/
		sprintf(cBasename,"lv_xc_dz_%s_%08ld.txt",sHv_Or_Brno.br_no,lTx_date);
		/**** 20110513
		strcat(cCmd,cBasename);
		****/
		sprintf(cCmd,"ftpsend %s %s",cFullname,cBasename);
		vtcp_log("[%s][%d]cCmd=[%s]",__FILE__,__LINE__,cCmd);
		ret=system(cCmd);
		if(ret)
		{
			vtcp_log("%s,%d�´������ļ�����������ʧ��",__FILE__,__LINE__);
			strncpy(g_pub_tx.reply,"P157",4);
			goto ErrExit;
		}
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ִ�д��ͳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return (0);
ErrExit:
	sprintf(acErrMsg,"ִ�д���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"J112",4);
	}
	set_zd_data("0120",g_pub_tx.reply);
	return (1);
}
