#include "bal.h"
#include "global.h"
#include"pub_tx.h"
extern struct pub_tx g_pub_tx;              /*ͨ�ü��ʽṹ*/
#define TRACE vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
int mobdo(){
	/****Add by SSH,20080909,֪ͨmobsend���Ͷ���Ϣ****/
	vtcp_log("ȡ����ˮ��:[%d]\n",g_pub_tx.trace_no);
	if(g_pub_tx.trace_no>0){
		char buf[128];
		/****TODO:
			���Ȳ���ÿ�ζ�������Ϣ����,�������г���ʱ����������������
		   ���Ч���ܵ�����Ӱ����Ӧ�ڳ�ʼ��ʱ����
		****/
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"%d|%d",g_pub_tx.tx_date,g_pub_tx.trace_no);
		if(nMobMqOpen()<0){
			vtcp_log("���Ӷ��Ŷ���ʧ��,���������![%d]\n",errno);
			return(0);
		}
		
		vtcp_log("���Ӷ��Ŷ���open,�������!\n");
		if(nMobMqPut(buf,strlen(buf),1)<0){
			vtcp_log("�������ݵ����Ŷ���ʧ��,���������![%d]\n",errno);
			return(0);
		}
		vtcp_log("�������ݵ����Ŷ��гɹ�\n");
	}		
	return(0);
}

