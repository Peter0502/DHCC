/******************************************************************
ģ������: δ��ɽ�����Ϣ��ѯ
��������: spJ400.pc
����ԭ��: spJ400()
�������:
�������:
ʹ��˵��:
�� д ��: dxy
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "hv_zf_c.h"






static struct Tis_Search_Unanswer {
	char brno[5];	/**���׻���**/
	char wt_date[8];
}tis22;

typedef struct{
	char wznotback[4];		/***����δ�ظ�***/
	char lznotback[4];		/***����δ�ظ�***/
	char lzbacknotsend[4];		/***���ʻظ�δ����***/
}TQINFO;
struct {
	TQINFO query;		/***��ѯ**/
	TQINFO askback;		/***�����˻�***/
	TQINFO askzf;		/***����֧��***/
	TQINFO ptjj;		/***��ͨ���***/
	TQINFO dqjj;		/***���ڽ��***/
}tos22;	



static int iSetNum4(char *buf,int num);
static int iQueryInfo_wznotback(char *pkgtag,char *pkgno,char *stat_name,char *otype_name);
static int iQueryInfo_lznotback(char *pkgtag,char *pkgno,char *stat_name,char *otype_name);
static int iQueryInfo_lzbacknotsend(char *pkgtag,char *pkgno,char *stat_name,char *otype_name);
/*************************************************************************
*                                                                        *
*   ������:         spJ400()                                      *
*   ����˵��:       1.��ѯû�б��ظ��ļ�¼����                          *
*************************************************************************/
int spJ400()
{
	vtcp_log("[%s][%d]��ʼ��ѯδ��ɽ���!\n",__FILE__,__LINE__);
	LV_FD123_AREA fd123;
	char cWt_date[9];
	int ret = 0;
	int iWt_date;


	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	memset(&fd123,0x00,sizeof(fd123));
	memset(&tos22,0x00,sizeof(tos22));
	memset(cWt_date,0x00,sizeof(cWt_date));
	
	get_zd_data("0910",tis22.brno);
	get_zd_data("0440",cWt_date);

	zip_space(cWt_date);

	iWt_date = atoi(cWt_date);
	if(iWt_date==0)
	{
		memcpy(tis22.wt_date,"",sizeof(tis22.wt_date));
	}

		
	vtcp_log("[%s][%d]��ѯ���к�Ϊ===[%s]\n",__FILE__,__LINE__,tis22.brno);
	
	memcpy(tis22.wt_date,cWt_date,8);
	
	
	ret = dh6560022_Process(tos22);
	if(ret)
	{
		vtcp_log("[%s][%d]ִ�в�ѯ����������!\n",__FILE__,__LINE__);
		goto ErrExit;
	}


	vtcp_log("[%s][%d]tos22===[%s]\n",__FILE__,__LINE__,(char*)&tos22);	
	memcpy(&fd123,&tos22,sizeof(tos22));
	vtcp_log("[%s][%d]fd123===[%s]\n",__FILE__,__LINE__,(char *)&fd123);

	set_zd_data("1230",(char *)&fd123);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"��ѯδ��ɽ��׳ɹ�[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
	
	
}



/******************************************************************************
*   ��������:          dh6560022_Process()                                  	  *
*                                                                             *
*   ����˵��:         ��ѯ                                    				  *
******************************************************************************/
int dh6560022_Process(){
	int iRc;
	if((iRc=iQueryInfo_wznotback("cmtno","301","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.query.wznotback,iRc);
	if((iRc=iQueryInfo_wznotback("cmtno","319","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askback.wznotback,iRc);
	if((iRc=iQueryInfo_wznotback("cmtno","327","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askzf.wznotback,iRc);
	if((iRc=iQueryInfo_wznotback("pkgno",PKGNO_PTJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.ptjj.wznotback,iRc);
	if((iRc=iQueryInfo_wznotback("pkgno",PKGNO_DQJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.dqjj.wznotback,iRc);
	/**************************************************/
	if((iRc=iQueryInfo_lznotback("cmtno","301","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.query.lznotback,iRc);
	if((iRc=iQueryInfo_lznotback("cmtno","319","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askback.lznotback,iRc);
	if((iRc=iQueryInfo_lznotback("cmtno","327","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askzf.lznotback,iRc);
	if((iRc=iQueryInfo_lznotback("pkgno",PKGNO_PTJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.ptjj.lznotback,iRc);
	if((iRc=iQueryInfo_lznotback("pkgno",PKGNO_DQJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.dqjj.lznotback,iRc);
	/**************************************************/
	if((iRc=iQueryInfo_lzbacknotsend("cmtno","301","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.query.lzbacknotsend,iRc);
	if((iRc=iQueryInfo_lzbacknotsend("cmtno","319","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askback.lzbacknotsend,iRc);
	if((iRc=iQueryInfo_lzbacknotsend("cmtno","327","hv_fzf_sts","otype"))<0)	return(-1);
	iSetNum4(tos22.askzf.lzbacknotsend,iRc);
	if((iRc=iQueryInfo_lzbacknotsend("pkgno",PKGNO_PTJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.ptjj.lzbacknotsend,iRc);
	if((iRc=iQueryInfo_lzbacknotsend("pkgno",PKGNO_DQJJ,"lv_sts","2"))<0)	return(-1);
	iSetNum4(tos22.dqjj.lzbacknotsend,iRc);
	return(0);
}
static int iQueryInfo_wznotback(char *pkgtag,char *pkgno,char *stat_name,char *otype_name){
	vtcp_log("[%s][%d]tos22===[%s]\n",__FILE__,__LINE__,(char*)&tos22);	
	int iRc;	
	int sql_cnt = 0;
	char cTable_name[20];
    char sql_condition[256];
	char sql_buf[256];

	memset(sql_condition,0x00,sizeof(sql_condition));
	memset(sql_buf,		 0x00,sizeof(sql_buf));

	memset(cTable_name,0x00,sizeof(cTable_name));
	
	if(tis22.brno[0]=='\0' ||tis22.brno[0]==' '){
		if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
			sprintf(sql_condition," %s='%.3s' and %s='%c' and %s='%c' and wt_date like '%%%%%s%%%%'  ",pkgtag,pkgno,stat_name,STAT_WZSEND,otype_name,'2' ,tis22.wt_date );
		}else{
			vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P075");
			return(-1);
		}	
	}else{
		if(memcmp(g_pub_tx.tx_br_no,tis22.brno,5)==0){
			sprintf(sql_condition," br_no='%.*s' and %s='%.3s' and %s='%c' and %s='%c' and wt_date like '%%%%%s%%%%'  ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_WZSEND,otype_name,'2' ,tis22.wt_date );
		}else{
			if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
				sprintf(sql_condition," br_no='%.*s' and %s='%.3s' and %s='%c'  and %s='%c' and wt_date like '%%%%%s%%%%' ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_WZSEND,otype_name,'2',tis22.wt_date );
			}else{
				vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P075");
				return(-1);
			}	
		}
	}
	if(!memcmp(pkgno,PKGNO_PTJJ,3)||!memcmp(pkgno,PKGNO_DQJJ,3))
	{
		/**��ͨ���***/
		/**���ڽ��**/
		strcpy(cTable_name,"lv_pkgreg");
		
	}else if(!memcmp(pkgno,"301",3) ||!memcmp(pkgno,"319",3) ||!memcmp(pkgno,"327",3))
	{
		/**��ѯ**/
		/**�����˻�**/
		/**ֹ������**/
		strcpy(cTable_name,"hv_fzf");
		
	}
	
   vtcp_log("[%s][%d]sql_buf===[%s]\n",__FILE__,__LINE__,sql_condition);
   vtcp_log("[%s][%d]sql_buf===[%s]\n",__FILE__,__LINE__,sql_condition);
	
	sql_cnt = sql_count(cTable_name,sql_condition);
	vtcp_log("[%s][%d]iQueryInfo_wznotback%s==[%d]\n",__FILE__,__LINE__,pkgno,sql_cnt);
	return(sql_cnt);	
}
static int iQueryInfo_lznotback(char *pkgtag,char *pkgno,char *stat_name,char *otype_name){
	int iRc;	
	int sql_cnt=0;
	char cTable_name[20];
	
    char sql_condition[256];
	char sql_buf[256];

	memset(sql_condition,0x00,sizeof(sql_condition));
	memset(sql_buf,		 0x00,sizeof(sql_buf));
	memset(cTable_name,0x00,sizeof(cTable_name));
	
	if(tis22.brno[0]=='\0' ||tis22.brno[0]==' '){
		if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
			sprintf(sql_condition," %s='%.3s' and %s ='%c' and %s='%c' and wt_date like '%%%%%s%%%%' ",pkgtag,pkgno,stat_name,STAT_LZRECV1,otype_name,'2',tis22.wt_date );
		}else{
			vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P075");
			return(-1);
		}	
	}else{
		if(memcmp(g_pub_tx.tx_br_no,tis22.brno,5)==0){
			sprintf(sql_condition," br_no='%.*s'and %s='%.3s' and %s='%c' and %s='%c' and wt_date like '%%%%%s%%%%'  ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_LZRECV1 ,otype_name,'2' ,tis22.wt_date);
		}else{
			if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
				sprintf(sql_condition," br_no='%.*s' and %s='%.3s' and %s='%c' and %s='%c'  and wt_date like '%%%%%s%%%%' ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_LZRECV1,otype_name,'2',tis22.wt_date );
			}else{
				vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P075");
				return(-1);
			}	
		}
	}
	if(!memcmp(pkgno,PKGNO_PTJJ,3)||!memcmp(pkgno,PKGNO_DQJJ,3))
	{
		/**��ͨ���***/
		/**���ڽ��**/
		strcpy(cTable_name,"lv_pkgreg");
		
	}else if(!memcmp(pkgno,"301",3) ||!memcmp(pkgno,"319",3) ||!memcmp(pkgno,"327",3))
	{
		/**��ѯ**/
		/**�����˻�**/
		/**ֹ������**/
		strcpy(cTable_name,"hv_fzf");
		
	}
	
	
	sql_cnt = sql_count(cTable_name,sql_condition);
	vtcp_log("[%s][%d]iQueryInfo_lznotback%s==[%d]\n",__FILE__,__LINE__,pkgno,sql_cnt);
	
	return(sql_cnt);
}
static int iQueryInfo_lzbacknotsend(char *pkgtag,char *opkgno,char *stat_name,char *otype_name){
	int iRc;	
	char pkgno[4];
	int sql_cnt=0;
	char cTable_name[20];
	
    char sql_condition[256];
	char sql_buf[256];

	memset(sql_condition,0x00,sizeof(sql_condition));
	memset(sql_buf,		 0x00,sizeof(sql_buf));
	
	memset(pkgno,'\0',sizeof(pkgno));
	memset(cTable_name,0x00,sizeof(cTable_name));
	
	memcpy(pkgno,opkgno,sizeof(pkgno)-1);
	
		
	if(tis22.brno[0]=='\0' ||tis22.brno[0]==' '){
		if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
			sprintf(sql_condition," %s='%.3s' and %s in('%c','%c','%c') and %s='%c' and wt_date like '%%%%%s%%%%' ",pkgtag,pkgno,stat_name,STAT_WZLR,STAT_WZCHECK1,STAT_WZCHECK2,otype_name,'2' ,tis22.wt_date );
		}else{
			vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P075");
			return(-1);
		}	
	}else{
		if(memcmp(g_pub_tx.tx_br_no,tis22.brno,5)==0){
			sprintf(sql_condition," br_no='%.*s' and %s='%.3s' and %s in('%c','%c','%c') and %s = '%c' and wt_date like '%%%%%s%%%%'  ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_WZLR,STAT_WZCHECK1,STAT_WZCHECK2,otype_name,'2' ,tis22.wt_date );
		}else{
			if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)==0){
				sprintf(sql_condition," br_no='%.*s' and %s='%.3s' and %s in('%c','%c','%c')  and %s = '%s' and wt_date like '%%%%%s%%%%'  ",5,tis22.brno,pkgtag,pkgno,stat_name,STAT_WZLR,STAT_WZCHECK1,STAT_WZCHECK2 ,otype_name,'2',tis22.wt_date);
			}else{
				vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P075");
				return(-1);
			}	
		}
	}
	if(!memcmp(pkgno,PKGNO_PTJJ,3)||!memcmp(pkgno,PKGNO_DQJJ,3))
	{
		/**��ͨ���***/
		/**���ڽ��**/
		strcpy(cTable_name,"lv_pkgreg");
		
	}else if(!memcmp(pkgno,"301",3) ||!memcmp(pkgno,"319",3) ||!memcmp(pkgno,"327",3))
	{
		/**��ѯ**/
		/**�����˻�**/
		/**ֹ������**/
		strcpy(cTable_name,"hv_fzf");
		
	}
	
	
	sql_cnt = sql_count(cTable_name,sql_condition);
	vtcp_log("[%s][%d]iQueryInfo_lzbacknotsend_%s==[%d]\n",__FILE__,__LINE__,pkgno,sql_cnt);
	return(sql_cnt);	
}
static int iSetNum4(char *buf,int num){
	char pcTmpBuf[5];
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));

	vtcp_log("[%s][%d]num===[%d]\n",__FILE__,__LINE__,num);
	if(num<=0){
		memset(buf,' ',4);
	}else{
		sprintf(pcTmpBuf,"%4d",num);
		memcpy(buf,pcTmpBuf,4);
	}
	vtcp_log("[%s][%d]buf====[%s]\n",__FILE__,__LINE__,*buf);
	return(0);
}



