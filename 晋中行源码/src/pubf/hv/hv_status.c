/***************************************************************************/
/* �ļ����ƣ����֧��Ӫҵ״̬����������                                    */
/*                                                                         */
/* ����������                                                              */
/*                                                                         */
/* ���ڣ�  2005.1.20                                                       */
/* ���ߣ�ʯ����                                                            */
/***************************************************************************/
#include"stdio.h"
#include"stdlib.h"
#include"hvstat_c.h"
#include"com_sys_parm_c.h"
#include"hv_define.h"
#include "find_debug.h"
#define	HV_HD_MIN_MONEY 20000.00;
/*************************************************************************/
/* �������ƣ�char hv_getstat()                                           */
/*                                                                       */
/* �����������õ���ǰӪҵ״̬                                            */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� Ӫҵ״̬                                                     */
/*************************************************************************/
char hv_getstat(){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ��Ӫҵ״̬����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(HV_STAT_UNKNOWN);
	}
	return(hv_stat.stat[0]);
}
/*************************************************************************/
/* �������ƣ�char *hv_getday()                                           */
/*                                                                       */
/* �����������õ���ǰӪҵ����                                            */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� Ӫҵ����                                                    */
/*************************************************************************/
int hv_getday(){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ��Ӫҵʱ�����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(-1);
	}
	return(hv_stat.tx_date);
}
/*************************************************************************/
/* �������ƣ�char hv_getmsg()                                           */
/*                                                                       */
/* �����������õ���ǰ����                                         */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� ����                                                */
/*************************************************************************/
char *hv_getmsg(){
	int ret=0;
	static char pmsg[31];
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(pmsg,'\0',sizeof(pmsg));

	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));

	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ�õ�ǰ��Ϣ��������,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(NULL);
	}
	MEMCPY_DEBUG(pmsg,hv_stat.message,sizeof(pmsg)-1);
	return(pmsg);
}
/*************************************************************************/
/* �������ƣ�char hv_getfiller1()                                           */
/*                                                                       */
/* �����������õ���ǰ��ע�ֶ�1                                         */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� ��ע�ֶ�1                                                 */
/*************************************************************************/
char *hv_getfiller1(){
	int ret=0;
	char pfiller1[31];
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(pfiller1,'\0',sizeof(pfiller1));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));

	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ�õ�ǰ��ע����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(NULL);
	}
	MEMCPY_DEBUG(pfiller1,hv_stat.filler1,sizeof(pfiller1)-1);
	return(pfiller1);
}
/*************************************************************************/
/* �������ƣ�void hv_setstat(char newstat)                               */
/*                                                                       */
/* �������������õ�ǰӪҵ״̬                                            */
/*                                                                       */
/* ���룺Ҫ����Ϊ��Ӫҵ״̬                                              */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/*************************************************************************/
int hv_setstat(char newstat){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,׼������hv_stat���α����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,׼��hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	hv_stat.stat[0]=newstat;
	/* Begin   Added by ChenMing   2006-10-31 14:33 */
	switch(newstat)
	{
		case '0':  MEMCPY_DEBUG(hv_stat.message,"Ӫҵ׼��",sizeof(hv_stat.message)-1); break;
		case '1':  MEMCPY_DEBUG(hv_stat.message,"�ռ䴦��",sizeof(hv_stat.message)-1); break;
		case '2':  MEMCPY_DEBUG(hv_stat.message,"����׼��",sizeof(hv_stat.message)-1); break;
		case '3':  MEMCPY_DEBUG(hv_stat.message,"ҵ�����",sizeof(hv_stat.message)-1); break;
		case '4':  MEMCPY_DEBUG(hv_stat.message,"���㴰��",sizeof(hv_stat.message)-1); break;
		case '5':  MEMCPY_DEBUG(hv_stat.message,"���մ���ʼ",sizeof(hv_stat.message)-1); break;
		case '6':  MEMCPY_DEBUG(hv_stat.message,"���ն��˲�ƽ",sizeof(hv_stat.message)-1); break;
		case '7':  MEMCPY_DEBUG(hv_stat.message,"���ն����Ѿ�ƽ",sizeof(hv_stat.message)-1); break;
		case 'F':  MEMCPY_DEBUG(hv_stat.message,"δ֪״̬",sizeof(hv_stat.message)-1); break;
		default:
        sprintf(acErrMsg,"û��������״̬!");
        WRITEMSG
        return -1;			
	}
	/* End   Added by ChenMing   2006-10-31 14:33 */	
	
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,����hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* �������ƣ�int hv_setday(int new_date)                                 */
/*                                                                       */
/* �������������õ�ǰӪҵ����                                            */
/*                                                                       */
/* ���룺Ҫ����Ϊ��Ӫҵ����                                              */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/*************************************************************************/
int hv_setday(int new_date){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,׼������hv_stat���α����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,׼��hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	hv_stat.tx_date=new_date;
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,����hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* �������ƣ�void hv_setmsg(char newmsg)                                 */
/*                                                                       */
/* �������������õ�ǰӪҵ״̬��������                                    */
/*                                                                       */
/* ���룺Ҫ����Ϊ��Ӫҵ״̬��������                                      */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/*************************************************************************/
int hv_setmsg(char *newmsg){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,׼������hv_stat���α����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,׼��hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	MEMCPY_DEBUG(hv_stat.message,newmsg,sizeof(hv_stat.message)-1);
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,����hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* �������ƣ�void hv_setfiller1(char newfiller1)                                 */
/*                                                                       */
/* �������������õ�ǰ��ע�ֶ�1                                    */
/*                                                                       */
/* ���룺Ҫ����Ϊ�ı�ע�ֶ�1                                      */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/*************************************************************************/
int hv_setfiller1(char *newfiller1){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,׼������hv_stat���α����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,׼��hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	MEMCPY_DEBUG(hv_stat.filler1,newfiller1,sizeof(hv_stat.filler1)-1);
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,����hv_stat���ݴ�,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* �������ƣ�void hv_checkstat()                                         */
/*                                                                       */
/* ������������鵱ǰӪҵ״̬                                            */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/* ˵����  ����TITA��鵱ǰ״̬�Ƿ��������ˣ���ҵ���������������*/
/*         it_txcom.txrsut�ʹ�����������������gs99��gs100                */
/*************************************************************************/
int iHv_checkstat(char * cmtno,char * yw_type){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	struct com_sys_parm_c com_sys_parm;
	int iTx_tdat=0;
	char cStat;

	int  txno;
	int  txtype;
	char tmpBuf[3];
	char *cnapsday;
	cStat=txno=txtype=0;
	MEMSET_DEBUG(tmpBuf,'\0',sizeof(tmpBuf));

	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	vtcp_log("%s,%d����������1===1\n",__FILE__,__LINE__);
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ��Ӫҵ״̬����,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"ȡ��Ӫҵ״̬����",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	MEMSET_DEBUG(&com_sys_parm, 0 , sizeof(com_sys_parm));
	ret=Com_sys_parm_Sel(cReply,&com_sys_parm,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,ȡ�ù���ϵͳ���������,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"ȡ�ù���ϵͳ���������",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	if(com_sys_parm.sys_sts!=0)
	{
		vtcp_log("%s,%d,ȡ�ù���ϵͳ���������,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"ȡ�ù���ϵͳ���������",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}

	if(com_sys_parm.sys_date!=hv_stat.tx_date){
		vtcp_log("%s,%d,�ۺ�ҵ����֧��ϵͳ�������ڲ�һ��[%d][%d]!\n",
			__FILE__,__LINE__,com_sys_parm.sys_date,hv_stat.tx_date);
		strncpy(acErrMsg,"�ۺ�ҵ����֧��ϵͳ�������ڲ�һ��",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	}
	switch(hv_stat.stat[0]){
	case HV_STAT_PREPARE:
		/*Ӫҵ׼��״̬,ֻ�ܽ��գ����ܷ���ҵ��*/
		vtcp_log("%s,%d,Ӫҵ׼��״̬���ܷ�����\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"Ӫҵ׼��״̬���ܷ�����",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_WORK:
		/*�ռ�Ӫҵ״̬*/
		vtcp_log("%s,%d,�ռ�Ӫҵ״̬�ܷ������н���\n",__FILE__,__LINE__);
		return 0;
	case HV_STAT_WARN:
		/*��������״̬*/
		vtcp_log("%s,%d,ҵ���������״̬�ܷ������Խ���\n",__FILE__,__LINE__);
		return 0;
	case HV_STAT_STOP:
		/*ҵ�����״̬*/
		vtcp_log("%s,%d,ҵ�����״̬���ܷ�����\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"ҵ���ֹ״̬���ܷ�����",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_QSWIN:
		/*���㴰��״ֻ̬����֧��ϵͳ�����ֲ�ͷ�粻��֮��Ĵ��֧��ҵ��
		 �����ҵ���е��ʽ�������Լ�ͬҵ���ҵ��ȣ��������˻���Ϣ�����ɸ�ʽ����
		*/
		if(!memcmp(cmtno,HV_HD,3)){
			/*���ҵ��*/
			if(!memcmp(yw_type,"40",2)){
				/*�ʽ����*/
				return 0;
			}else{
				vtcp_log("%s,%d,���㴰��״̬���ܷ�����,���ҵ��,txtype=%02d\n",__FILE__,__LINE__,txtype);
				strncpy(acErrMsg,"���㴰��״̬���ܷ�����",40);
				set_zd_data("0130",acErrMsg);
				return 1;
			}
		}else if(!memcmp(cmtno,HV_CJ,3)){
			/*���ҵ��*/
			return 0;
		}else if(memcmp(cmtno,"303",3)==0){
			/*���ɸ�ʽ*/
			return 0;
		}else if(memcmp(cmtno,"301",3)==0){
			/* ��ѯ*/
			return 0;
		}else if(memcmp(cmtno,"302",3)==0){
			/* �鸴*/
			return 0;
		}else if(memcmp(cmtno,"311",3)==0){
			/*���볷����ʽ*/
			return 0;
		}else{
			vtcp_log("%s,%d,���㴰��״̬���ܷ�����,txno=%02d,txtype=%02d\n",__FILE__,__LINE__,txno,txtype);
			strncpy(acErrMsg,"���㴰��״̬���ܷ�����",40);
			return 1;
		}
	case HV_STAT_WORKEND:
		vtcp_log("%s,%d,����״̬���ܷ�����\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"����״̬���ܷ�����",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_UNKNOWN:
	default:
		vtcp_log("%s,%d,��ǰӪҵ״̬����[%d]\n",__FILE__,__LINE__,cStat);
		strncpy(acErrMsg,"��ǰӪҵ״̬����",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	}
}
double dGetHvMinTxamt( )
{
	double dMinTxamt=0;
	char cMinTxamt[12];
	char cTxday[9];
	char cSqlbuf[1024];
	int iRet=0;
	MEMSET_DEBUG(cSqlbuf, 0 , sizeof(cSqlbuf));
	MEMSET_DEBUG(cTxday, 0 , sizeof(cTxday));
	MEMSET_DEBUG(cMinTxamt, 0 , sizeof(cMinTxamt));
	get_zd_data(DC_DATE,cTxday);
	sprintf(cSqlbuf,"(select max(sdate) from hv_syspara where sdate<='%s' and datatype='2')",cTxday);
	iRet=sql_count_sum_double ( "hv_syspara" , "startamt" , &dMinTxamt , "sdate=%s" , cSqlbuf );
	if(dMinTxamt==0){
		dMinTxamt=HV_HD_MIN_MONEY;
	}
	vtcp_log("%s,%d �õ��������===[%f]",__FILE__,__LINE__,dMinTxamt);
	return dMinTxamt;
}
