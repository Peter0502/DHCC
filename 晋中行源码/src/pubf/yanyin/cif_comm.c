/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2006-11-15                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : cif_comm.c                                               ***
 *** �������� : ������ӡ����ķ���˳������蹫������                     ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
 ***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include"time.h"
#include"sys/timeb.h"
#define EXTERN
#include "public.h"
#include "cifserver.h"
#include "cifpack.h"
#include "lv_define.h"
#include "hv_define.h"
#include"lv_pkgreg_c.h"
#include"hv_uniontab_c.h"
#include"hv_orno_brno_c.h"
#include"com_sys_parm_c.h"
#include"lv_sysctl_c.h"
#include"lv_addit_c.h"
#include"lv_chgbank_c.h"
#include "errlog.h"
long iLvGetSeqno(char *brno, char *ac_ind);
char *zip_space(char *s){
	int i,j;
	i=j=0;
	while(s[i]!='\0'){
		if(s[i]!=' ' && s[i]!='\t' && s[i]!=0x0a && s[i]!=0x0d){
			s[j]=s[i];
			j++;
		}
		i++;
	}
	while(j<i){
		s[j++]='\0';
	}	
}
int apatoi(char *s,int len){
	char buf[64];
	memset(buf,'\0',sizeof(buf));
	memcpy(buf,s,len);
	return(atoi(buf));
}
/**********************************************************************
*  char *pcCifGetTime(char  *pcTimeStr);                              *
*  ���ܣ�  ȡ��ϵͳ���ں�ʱ��                                         *
*  ������  ʱ�䴮������                                               *
*  ����ֵ��ʱ�䴮��������ַ                                           *
*                                                                     *
**********************************************************************/         
char  *pcCifGetTime(char *pcTimeStr)
{
	struct tm stTime;
	struct  timeb stTimeB;
	memset((char *)&stTime ,'\0',sizeof(struct tm));
	memset((char *)&stTimeB,'\0',sizeof(struct  timeb));
	ftime(&stTimeB);
#ifdef AIX
	memcpy(&stTime,localtime((time_t *)&(stTimeB.time)),sizeof(struct tm));
#else
	memcpy(&stTime,localtime((long *)&(stTimeB.time)),sizeof(struct tm));
#endif
	strftime(pcTimeStr,(size_t)(14 + 1),"%Y%m%d%H%M%S",&stTime);
	return(pcTimeStr);
}
/******************************************************************/
/* ��������: int iCifGetAddid()    						          */
/* ��������: �õ���һ������������id,�뽻�׻����޹�    			*/
/* ��������: 2005.12.2               			  			*/
/* ����:     SSH                       		  				*/
/* ˵��:          								  				*/
/******************************************************************/
int nCifGetAddid()
{
	return iLvGetSeqno(HV_BR_NO, LV_ADDID_IND);
}
/**********************************************************************
*  int nCifSetAddit(char *txday,char  *name,char *buf,int length)     *
*  ���ܣ����ø���������                                               *
*  ������TODO:ע�����ڵĲ�ͬȡ��                                      *
*        txday-����(�ɰ�Ӧȡ��ǰС��ϵͳ����/�°�ȡ�ۺ�ҵ������)      *
*        addid -������tag������                                        *
*        name -������tag������                                        *
*        buf -������tag������                                         *
*        length -������tag�ĳ���                                      *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nCifSetAddit(long txday,char *addid,char *name,void *buf,int length){
	int iRc;
	struct lv_addit_c wd_lv_addit;
	char cTxday[9];
	memset(cTxday, 0 , sizeof(cTxday));
	sprintf(cTxday,"%08ld",txday);
	zip_space(cTxday);
	memset(&wd_lv_addit,'\0',sizeof(wd_lv_addit));
	if(addid[0]=='\0'){
		int nAddid=nCifGetAddid();
		if(nAddid<=0){
			return(-1);
		}
		sprintf(wd_lv_addit.addid,"%08d",nAddid);
		wd_lv_addit.tx_date=atoi(cTxday);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		memcpy(wd_lv_addit.tagdata,(char *)buf,length);
		wd_lv_addit.taglen=length;/** new add 070520 **/
		iRc=Lv_addit_Ins(wd_lv_addit,g_pub_tx.reply);
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d, Insert lv_addit error=%d\n",__FILE__,__LINE__,iRc);
			Lv_addit_Debug(&wd_lv_addit);
			return(-1);
		}
	}else{
		memcpy(wd_lv_addit.addid,addid,sizeof(wd_lv_addit.addid)-1);
		wd_lv_addit.tx_date=atoi(cTxday);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		wd_lv_addit.taglen=length;/** new add 070520 **/
		iRc=Lv_addit_Dec_Upd(g_pub_tx.reply,"addid='%s' and tx_date='%d' and tagname='%s'",wd_lv_addit.addid,wd_lv_addit.tx_date,wd_lv_addit.tagname);
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"[%s][%d] find addid[%s] \n",__FILE__,__LINE__,wd_lv_addit.addid);
		iRc=Lv_addit_Fet_Upd(&wd_lv_addit,g_pub_tx.reply);
		if(iRc){
			if(iRc==100)
			{
				memcpy(wd_lv_addit.tagdata,(char *)buf,length);
				wd_lv_addit.taglen=length;/** new add 070520 **/
				iRc=Lv_addit_Ins(wd_lv_addit,g_pub_tx.reply);
				if(iRc){
					errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d, Insert lv_addit error=%d\n",__FILE__,__LINE__,iRc);
					Lv_addit_Debug(&wd_lv_addit);
					return(-1);
				}
				return 0;
			}
			else
			{
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d, Insert lv_addit error=%d\n",__FILE__,__LINE__,iRc);
				Lv_addit_Debug(&wd_lv_addit);
				return(-1);
			}
		}
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"[%s][%d] buf==[%s]\n",__FILE__,__LINE__,(char *)buf);
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"[%s][%d] tagdata==[%s]\n",__FILE__,__LINE__,wd_lv_addit.tagdata);
		memcpy(wd_lv_addit.tagdata,(char *)buf,length);
		wd_lv_addit.taglen=length;/** new add 070520 **/
		iRc=Lv_addit_Upd_Upd(wd_lv_addit,g_pub_tx.reply);
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d, Insert lv_addit error=%d\n",__FILE__,__LINE__,iRc);
			Lv_addit_Debug(&wd_lv_addit);
			return(-1);
		}
		Lv_addit_Clo_Upd();
	}
	memcpy(addid,wd_lv_addit.addid,sizeof(wd_lv_addit.addid)-1);
	return(0);
}
/**********************************************************************
*  int nCifGetChgBank(char *cashno,char *chgbank,char *txday)         *
*  ���ܣ�����������ҵ���С��֧��������                               *
*  ������                                                             *
*        cashno  -������к�                                          *
*        chgno   -С��������к�                                      *
*        txday   -��ǰС������                                        *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nCifGetChgBank(char *cashno,char *chgno,char *txday) {
	int iRc;
	struct lv_chgbank_c wd_lv_chgbank;
	memset(&wd_lv_chgbank,'\0',sizeof(wd_lv_chgbank));
	memcpy(wd_lv_chgbank.bakcode,cashno,sizeof(wd_lv_chgbank.bakcode)-1);	
	iRc=Lv_chgbank_Sel(g_pub_tx.reply,&wd_lv_chgbank,"bakcode='%s'",wd_lv_chgbank.bakcode);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,δ�ҵ�Ʊ������[%s]\n",__FILE__,__LINE__,wd_lv_chgbank.bakcode);
		return(-1);
	}
	if(wd_lv_chgbank.stat[0]=='0' && 
		memcmp(wd_lv_chgbank.effday,txday,8)>0){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"����[%s]��������δ����Ч��\n",wd_lv_chgbank.bakcode);
		memcpy(chgno,wd_lv_chgbank.agecode,sizeof(wd_lv_chgbank.agecode)-1);
		return(0);
	}
	if(wd_lv_chgbank.stat[0]=='1'){
		if(memcmp(wd_lv_chgbank.effday,txday,8)>0){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"����[%s]���ڵ�δ����Ч��\n",wd_lv_chgbank.bakcode);
			memcpy(chgno,wd_lv_chgbank.oagecode,sizeof(wd_lv_chgbank.agecode)-1);
			return(-1);
		}else{
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"����[%s]�������ѵ���Ч��\n",wd_lv_chgbank.bakcode);
			memcpy(chgno,wd_lv_chgbank.agecode,sizeof(wd_lv_chgbank.agecode)-1);
			return(0);
		}
	}
	return(-1);
}
void CBexchange(int f,unsigned char s[],unsigned char t[]);
void Shuff_Data(unsigned char *msg,int len1);
void CBshuff(unsigned char s[])
{
  unsigned char tmp[7];

  memcpy(tmp,s,6);
  CBexchange(0,tmp,s);
  Shuff_Data(s,6);
}


void CBunshuff(unsigned char s[])
{
  unsigned char tmp[7];

  Shuff_Data(s,6);
  memcpy(tmp,s,6);
  CBexchange(1,tmp,s);
}


void CBexchange(int f,unsigned char s[],unsigned char t[])
{
  switch (f) {
  case 0:
      t[0] = s[2];
      t[1] = s[5];
      t[2] = s[0];
      t[3] = s[4];
      t[4] = s[3];
      t[5] = s[1];
      break;
  case 1:
      t[0] = s[2];
      t[1] = s[5];
      t[2] = s[0];
      t[3] = s[4];
      t[4] = s[3];
      t[5] = s[1];
      break;
  }
}

void Shuff_Data(unsigned char *msg,int len1)
{
/* shuffle string is 19f2c8276ad0839b */

  int offset = 0;

  switch(len1) {
    case 16 :  
	  msg[offset++]^= 0x01;
    case 15 :
	  msg[offset++]^= 0x09;
    case 14 :
	  msg[offset++]^= 0x0f;
    case 13 :
	  msg[offset++]^= 0x02;
    case 12 :
	  msg[offset++]^= 0x0c;
    case 11 :
	  msg[offset++]^= 0x08;
    case 10 :
	  msg[offset++]^= 0x02;
    case 9 :
	  msg[offset++]^= 0x07;
    case 8 :
	  msg[offset++]^= 0x06;
    case 7 :
	  msg[offset++]^= 0x0a;
    case 6 :
	  msg[offset++]^= 0x0d;
    case 5 :
	  msg[offset++]^= 0x00;
    case 4 :
	  msg[offset++]^= 0x08;
    case 3:
	  msg[offset++]^= 0x03;
    case 2 :
	  msg[offset++]^= 0x09;
    case 1 :
	  msg[offset++]^= 0x0b;
  }
}

/**********************************************************************
*  int nDealCifPack(TCIFPACKA *)                                      *
*  ���ܣ�����һ��CIF����                                              *
*  ������CIF���Ľṹ                                                  *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nDealCifPack(TCIFPACK *pack){
	struct lv_pkgreg_c wd_lv_ptjjr;
	struct lv_pkgreg_c wd_lv_ptjjs;
	struct hv_uniontab_c wd_hvuniontab;
	struct hv_orno_brno_c wd_hv_orno_brno;
	struct com_sys_parm_c wd_com_sys_parm;
	struct lv_sysctl_c    wd_lv_sysctl;
	int  iRc=0;
	char pcGlobalTbsdy[9];	/**�ۺ�ҵ������**/
	char pcLvTxday[9];		/**С��ϵͳ����**/
	char pcTimeStr[32];
	char pcTmpStr[256];
	char pcTxcode[5];
	char pcTmpBuf[128];
	int  nId;
	memset(pcGlobalTbsdy,'\0',sizeof(pcGlobalTbsdy));
	memset(pcLvTxday,'\0',sizeof(pcLvTxday));
	memset(pcTimeStr,'\0',sizeof(pcTimeStr));
	memset(pcTxcode,'\0',sizeof(pcTxcode));
	memset(&wd_lv_ptjjr	,'\0',sizeof(wd_lv_ptjjr));
	memset(&wd_lv_ptjjs,'\0',sizeof(wd_lv_ptjjs));
	memset(&wd_hvuniontab	,'\0',sizeof(wd_hvuniontab));
	memset(&wd_com_sys_parm	,'\0',sizeof(wd_com_sys_parm));
	memset(&wd_lv_sysctl	,'\0',sizeof(wd_lv_sysctl));
	memset(&wd_hv_orno_brno	,'\0',sizeof(wd_hv_orno_brno));
	memset(pcTmpBuf	,'\0',sizeof(pcTmpBuf));
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,Gethere,[����],�յ��ı���:\n%s\n",__FILE__,__LINE__,pack);
	pcCifGetTime(pcTimeStr);
	iRc=Com_sys_parm_Sel(g_pub_tx.reply,&wd_com_sys_parm,"1=1");
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"ȡ�ۺ�ҵ�����ڴ�[%d]\n",iRc);
		return(-1);
	}
	sprintf(pcGlobalTbsdy,"%08ld",wd_com_sys_parm.sys_date);
	iRc=Lv_sysctl_Sel(g_pub_tx.reply,&wd_lv_sysctl,"1=1");
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"ȡС��ϵͳ���ڴ�[%d]\n",iRc);
		return(-1);
	}
	sprintf(pcLvTxday,"%08ld",wd_lv_sysctl.tx_date);

	/****TODO:���ڴ˴�����һЩ����Ҫ�ļ��****/
	;;;;;;;;;;;;;
	/****TODO:���ڴ˴�����һЩ����Ҫ�ļ��****/
	vGetCifTag(pack,"TXC",pcTxcode);/****���״���****/
	/***���⽻��****/
	if(memcmp(pcTxcode,"0500",4)==0){
		/****TODO:������֤,��֮�ܸ�ʽ�޴˽���****/
		return(0);
	}


	memcpy(wd_lv_ptjjr.pkgno,PKGNO_PTJJ,sizeof(wd_lv_ptjjr.pkgno)-1);	
	wd_lv_ptjjr.jz_date=atoi(pcGlobalTbsdy);	
	/****TODO:�°�˴�ȡ�ۺ�ҵ������****/
	wd_lv_ptjjr.in_date=atoi(pcGlobalTbsdy);
	memset(pcTmpBuf,'\0',9);
	vGetCifTag(pack,"30A",pcTmpBuf);
	wd_lv_ptjjr.wt_date=atoi(pcTmpBuf);
	wd_lv_ptjjr.pack_date=wd_lv_ptjjr.wt_date;
	vGetCifTag(pack,"0BC",wd_lv_ptjjr.orderno);
	wd_lv_ptjjr.tx_time=atoi(pcTimeStr+8);
	if(memcmp(pack->head.cifno,"100",3)==0){
		/****֧Ʊ����****/
		memcpy(wd_lv_ptjjr.txnum,TXNUM_ZPJL,sizeof(wd_lv_ptjjr.txnum)-1);	
	}else{
		/****ͨ�ý���****/
		memcpy(wd_lv_ptjjr.txnum,TXNUM_TYJL,sizeof(wd_lv_ptjjr.txnum)-1);	
	}	
	vGetCifTag(pack,"CC5",wd_lv_ptjjr.cash_opn_br_no);/****�����****/
	/****���������ͨ��lv_chgbank�ҵ�������****/
	iRc=nCifGetChgBank(wd_lv_ptjjr.cash_opn_br_no,wd_lv_ptjjr.or_br_no,pcLvTxday);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,nCifGetChgBank��ERR\n",__FILE__,__LINE__);
		return(-1);
	}		
	vGetCifTag(pack,"D21",wd_lv_ptjjr.pay_opn_br_no);/****������****/
	vGetCifTag(pack,"D21",wd_lv_ptjjr.ac_br_no);/****��������������****/
	vGetCifTag(pack,"D23",wd_lv_ptjjr.pay_ac_no);/****��Ʊ���ʺ�****/
	zip_space(wd_lv_ptjjr.pay_ac_no);
	/****���ݷ��𡢽����кš������ʺ�ȡ�����кš�ccpc����ͱ��ػ�����****/
	memset(&wd_hvuniontab	,'\0',sizeof(wd_hvuniontab));
	
	memcpy(wd_hvuniontab.or_br_no,wd_lv_ptjjr.or_br_no,sizeof(wd_hvuniontab.or_br_no)-1);
			
	iRc=Hv_uniontab_Sel(g_pub_tx.reply,&wd_hvuniontab,"or_br_no='%s'",wd_hvuniontab.or_br_no);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,δ�ҵ�������\n",__FILE__,__LINE__);
		Hv_uniontab_Debug(&wd_hvuniontab);
		return(-1);
	}
	if(wd_hvuniontab.br_sts[0]!='1'){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,������[%s]δ��Ч\n",__FILE__,__LINE__,wd_hvuniontab.or_br_no);
		return(-1);
	}
	memcpy(wd_lv_ptjjr.pay_qs_no,wd_hvuniontab.qs_no,sizeof(wd_lv_ptjjr.pay_qs_no)-1);
	memcpy(wd_lv_ptjjr.sendco,wd_hvuniontab.sendco,sizeof(wd_lv_ptjjr.sendco)-1);
	memset(&wd_hvuniontab	,'\0',sizeof(wd_hvuniontab));
	memcpy(wd_hvuniontab.or_br_no,wd_lv_ptjjr.ac_br_no,sizeof(wd_hvuniontab.or_br_no)-1);
	iRc=Hv_uniontab_Sel(g_pub_tx.reply,&wd_hvuniontab,"or_br_no='%s'",wd_hvuniontab.or_br_no);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,δ�ҵ�������,������������,�ٸ����ʺŲ���\n",__FILE__,__LINE__);
		Hv_uniontab_Debug(&wd_hvuniontab);
		memset(&wd_hvuniontab	,'\0',sizeof(wd_hvuniontab));
		memcpy(wd_hvuniontab.or_br_no,LV_QSBRNO,sizeof(wd_hvuniontab.or_br_no)-1);
		iRc=Hv_uniontab_Sel(g_pub_tx.reply,&wd_hvuniontab,"or_br_no='%s'",wd_hvuniontab.or_br_no);
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,δ�ҵ�����������\n",__FILE__,__LINE__);
			Hv_uniontab_Debug(&wd_hvuniontab);
			return(-1);
		}
	}
	if(memcmp(wd_hvuniontab.or_br_no,LV_QSBRNO,sizeof(wd_hvuniontab.or_br_no)-1)==0){
		/****���������к�,�ٸ����ʺŲ��ҽ����к�****/
		/****�϶��ǶԹ���****/
		/****TODO:�ݲ�����,�Ժ����****/
	}
	memcpy(wd_hv_orno_brno.or_br_no,wd_hvuniontab.or_br_no,sizeof(wd_hv_orno_brno.or_br_no)-1);
	iRc=Hv_orno_brno_Sel(g_pub_tx.reply,&wd_hv_orno_brno,"or_br_no='%s'",wd_hv_orno_brno.or_br_no);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,δ�ҵ�������\n",__FILE__,__LINE__);
		Hv_orno_brno_Debug(&wd_hvuniontab);
		return(-1);
	}
	memcpy(wd_lv_ptjjr.br_no,wd_hv_orno_brno.br_no,sizeof(wd_lv_ptjjr.br_no)-1);
	memcpy(wd_lv_ptjjr.ac_br_no,wd_hvuniontab.or_br_no,sizeof(wd_lv_ptjjr.ac_br_no)-1);
	memcpy(wd_lv_ptjjr.cash_qs_no,wd_hvuniontab.qs_no,sizeof(wd_lv_ptjjr.cash_qs_no)-1);
	memcpy(wd_lv_ptjjr.receco,wd_hvuniontab.sendco,sizeof(wd_lv_ptjjr.receco)-1);
	memset(pcTmpStr,'\0',sizeof(pcTmpStr));
	vGetCifTag(pack,"32A",pcTmpStr);/****���ҷ��ţ����****/
	wd_lv_ptjjr.tx_amt=atof(pcTmpStr+3)/100.0; /****��ϵͳ��ԪΪ��λ****/
	vGetCifTag(pack,"D22",wd_lv_ptjjr.pay_name);/****��Ʊ������****/
	/**add by wyw �������еİ�Ǳ�ȫ��**/
	memcpy(wd_lv_ptjjr.pay_name,(char*)DBC2SBC(wd_lv_ptjjr.pay_name,sizeof(wd_lv_ptjjr.pay_name)-1),sizeof(wd_lv_ptjjr.cash_name)-1);
	zip_space(wd_lv_ptjjr.pay_name);
	vGetCifTag(pack,"59C",wd_lv_ptjjr.cash_ac_no);/****�տ����ʺ�****/
	zip_space(wd_lv_ptjjr.pay_ac_no);
	vGetCifTag(pack,"59A",wd_lv_ptjjr.cash_name);/****�տ�������****/
	zip_space(wd_lv_ptjjr.cash_name);
	/**add by wyw �������еİ�Ǳ�ȫ��**/
	memcpy(wd_lv_ptjjr.cash_name,(char*)DBC2SBC(wd_lv_ptjjr.cash_name,sizeof(wd_lv_ptjjr.cash_name)-1),sizeof(wd_lv_ptjjr.cash_name)-1);
	vtcp_log("%s,%d ȫ��cash_name[%s]\n",__FL__,(char *)DBC2SBC(wd_lv_ptjjr.cash_name,sizeof(wd_lv_ptjjr.cash_name)));
	vGetCifTag(pack,"D37",wd_lv_ptjjr.lv_brf);/****��;****/
	zip_space(wd_lv_ptjjr.lv_brf);
	wd_lv_ptjjr.lv_sts[0]=STAT_LZRECV1;	
	wd_lv_ptjjr.beg_sts[0]=STAT_LZRECV1;	
	memset(pcTmpBuf,'\0',9);
	vGetCifTag(pack,"BS6",pcTmpBuf);/****��ִ����****/
	wd_lv_ptjjr.resp_date=atoi(pcTmpBuf);/****��ִ����****/
	if(memcmp(pcTxcode,"0200",4)==0){
		/****TODO:�ڴ���д������ɵ���ش���****/	
		/****��֮�ܸ�ʽû�д˽���****/
		return(0);
	}else if(memcmp(pcTxcode,"0400",4)==0){
		/****TODO:�ڴ���д��ӡ֪ͨ����ش���****/	
		/****�϶��Ǵ���ӡϵͳȡ�����ˣ�����������֧****/
		/****����ӡϵͳȡ֧������****/
		wd_lv_ptjjr.lw_ind[0]='2';		/****����****/
		sprintf(wd_lv_ptjjr.addid,"%08d",nCifGetAddid());
		iRc=Lv_pkgreg_Ins(wd_lv_ptjjr,g_pub_tx.reply);
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"[%s][%d],sqlcode=[%d]\n",__FILE__,__LINE__,iRc);
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,������ҵ���¼ʧ��!\n",__FILE__,__LINE__);
			Lv_pkgreg_Debug(&wd_lv_ptjjr);
			return(-1);
		}
		/****�����������,�������ñ�׼������****/
		if(memcmp(wd_lv_ptjjr.txnum,"30102",5)==0){
			/****֧Ʊ����ҵ��****/
			ZPJL1_ADD_AREA wd_add1;
			ZPJL2_ADD_AREA wd_add2;
			ZPJL3_ADD_AREA wd_add3;
			
			memset(&wd_add1,' ',sizeof(wd_add1));
			memset(&wd_add2,'\0',sizeof(wd_add2));
			memset(&wd_add3,' ',sizeof(wd_add3));
			vGetCifTag(pack,"D35",wd_add1.cpday);/****��Ʊ����****/
			vGetCifTag(pack,"D01",wd_add1.cpno); /****Ʊ�ݺ���****/
			memcpy(wd_add1.payno,wd_lv_ptjjr.pay_opn_br_no,sizeof(wd_add1.payno));
			memcpy(wd_add1.cpactno,wd_lv_ptjjr.pay_ac_no,sizeof(wd_add1.cpactno));
			memcpy(wd_add1.cashactno,wd_lv_ptjjr.cash_ac_no,sizeof(wd_add1.cashactno));
			sprintf(pcTmpStr,"%015.0f",wd_lv_ptjjr.tx_amt*100);
			memcpy(wd_add1.cpamt,pcTmpStr,sizeof(wd_add1.cpamt));
			memcpy(wd_add1.use,wd_lv_ptjjr.lv_brf,sizeof(wd_add1.use));
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"001",&wd_add1,sizeof(wd_add1));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����֧Ʊҵ�񸽼���(1)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			vGetCifTag(pack,"D41",wd_add2.bscnt);/****�������****/
			if(wd_add2.bscnt[0]==' ' ||wd_add2.bscnt[0]=='\0'){
				/****��Ϊ��������Ǳ�����,����Ҫ�ڴ˴���һ��****/
				memcpy(wd_add2.bscnt,"00",2);
			}
			vGetCifTag(pack,"D42",(char *)wd_add2.bslist); /****�����嵥****/
			nId=apatoi(wd_add2.bscnt,sizeof(wd_add2.bscnt));
			if(nId<=10){
				wd_add2.bslist[nId][0]='\0';
			}
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"002",&wd_add2,sizeof(wd_add2));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����֧Ʊҵ�񸽼���(2)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			memset(&wd_add3	,'0',sizeof(wd_add3));
			vGetCifTag(pack,"D30",wd_add3.bscheckpswd);/****У������****/
			if(wd_add3.bscheckpswd[0]=='\0'){
				memset(&wd_add3	,' ',sizeof(wd_add3.bscheckpswd));
			}
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"003",&wd_add3,sizeof(wd_add3));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����֧Ʊҵ�񸽼���(3)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
		}else{
			/****ͨ�ý���ҵ��****/
			TYJL1_ADD_AREA wd_add1;
			TYJL2_ADD_AREA wd_add2;
			TYJL3_ADD_AREA wd_add3;
			TYJL4_ADD_AREA wd_add4;
			TYJL5_ADD_AREA wd_add5;
			memset(&wd_add1,' ',sizeof(wd_add1));
			memset(&wd_add2,'\0',sizeof(wd_add2));
			memset(&wd_add3,' ',sizeof(wd_add3));
			memset(&wd_add4,' ',sizeof(wd_add4));
			memset(&wd_add5,' ',sizeof(wd_add5));
			vGetCifTag(pack,"D35",wd_add1.cpday);/****��Ʊ����****/
			vGetCifTag(pack,"D01",wd_add1.cpno+8); /****Ʊ�ݺ���****/
			memset(wd_add1.cpno,'0',8);				/**ͨ�ý���������Ʊ�ݺ���20λ**/
			memcpy(wd_add1.payno,wd_lv_ptjjr.pay_opn_br_no,sizeof(wd_add1.payno));
			memcpy(wd_add1.dlpayno,wd_lv_ptjjr.pay_opn_br_no,sizeof(wd_add1.dlpayno));
			sprintf(pcTmpStr,"%015.0f",wd_lv_ptjjr.tx_amt*100);
			memcpy(wd_add1.cpamt,pcTmpStr,sizeof(wd_add1.cpamt));
			memcpy(wd_add1.use,wd_lv_ptjjr.lv_brf,sizeof(wd_add1.use));
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"001",&wd_add1,sizeof(wd_add1));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(1)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			vGetCifTag(pack,"D41",wd_add2.bscnt);/****�������****/
			if(wd_add2.bscnt[0]==' ' ||wd_add2.bscnt[0]=='\0'){
				/****��Ϊ��������Ǳ�����,����Ҫ�ڴ˴���һ��****/
				memcpy(wd_add2.bscnt,"00",2);
			}
			vGetCifTag(pack,"D42",(char*)wd_add2.bslist);/****�����嵥****/
			nId=apatoi(wd_add2.bscnt,sizeof(wd_add2.bscnt));
			if(nId<10){
				wd_add2.bslist[nId][0]='\0';
			}
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"002",&wd_add2,sizeof(wd_add2));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(2)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			memset(&wd_add3	,'0',sizeof(wd_add3));
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"003",&wd_add3,sizeof(wd_add3));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(3)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			vGetCifTag(pack,"D32",wd_add4.ptype);/****Ʊ������****/
			vGetCifTag(pack,"D36",wd_add4.tsday);/****��ʾ��������****/
			vGetCifTag(pack,"D51",wd_add4.mac);/****��Ʊ��Ѻ****/
			if(wd_add4.mac[0]=='\0' ||wd_add4.mac[0]==' '){
				memset(wd_add4.mac,' ',sizeof(wd_add4.mac));
			}
			vGetCifTag(pack,"D61",wd_add4.stopday);/****��Ʊ������****/
			vGetCifTag(pack,"D62",wd_add4.xyno);	/****�ж�Э����***/
			vGetCifTag(pack,"D63",wd_add4.htno);	/****���׺�ͬ���***/
			vGetCifTag(pack,"D64",wd_add4.cdday);	/****�ж�����  ***/
			vGetCifTag(pack,"D66",wd_add4.cdname);	/****�ж��� ***/
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"004",&wd_add4,sizeof(wd_add4));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(4)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			memset(&wd_add5,'0',sizeof(wd_add5));
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"005",&wd_add5,sizeof(wd_add5));
			if(iRc){
				errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(5)ʧ��!\n",__FILE__,__LINE__);
				return(-1);
			}
			
		}
		/****�����Զ��帽����****/
		memset(pcTmpStr,'\0',sizeof(pcTmpStr));
		vGetCifTag(pack,"SHR",pcTmpStr);
		if(memcmp(pcTmpStr,"00",2)==0){
			/**Ʊ�����ͨ��***/
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"SHR","00",2);
		}else{
			/**Ʊ�����δͨ��***/
			iRc=nCifSetAddit(wd_lv_ptjjr.in_date,wd_lv_ptjjr.addid,"SHR",pcTmpStr,strlen(pcTmpStr));
		}
		if(iRc){
			errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,����ͨ��ҵ�񸽼���(SHR)ʧ��!\n",__FILE__,__LINE__);
			return(-1);
		}
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,���ݴ���ɹ�!",__FILE__,__LINE__);
		return(0);
	}
}
