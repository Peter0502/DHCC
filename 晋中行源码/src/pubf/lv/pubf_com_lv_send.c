/***************************************************************************/
/* �ļ�����: lv_sendcomm.pc                                                */
/* ��������: С��ƴ�����Ͳ�����������                                      */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "dmem.h"
#include "lvnet.h"
#include "lv_pub.h"
#include "public.h"
extern char *pcLvGetDay_work();
extern char *pcLvGetDay();
extern char * zip_space(char *);

int ret=0;
char *lv_date(long);
int iLvSendPackage(TLVPACKAGE *lv);
/******************************************************************************
****                   ƴ����ʱע������(ʯ����)                            ****
****                         (2005.11.27)                                  ****
****  1.������ʱ����ָ�����ڶ�����Ĭ���ǵ���,��ΪС����������ǰ�İ�      ****
****  2.ȡ��ǰ����ʱ�ú���pcLvGetDay,��������it_txcom.tbsdy,��Ϊ�ڱ�������  ****
****    �������ڶ���С��ϵͳ���ڣ��������ۺ�ҵ��ϵͳ����                   ****
****  3.�жϰ����߽��׼�¼��״̬ʱ����ʹ�ú궨�壬����Ҫʹ����ֵ           ****
****  4.��Ҫ��Ҫ�ؼ��Ÿ�ֵ                                                 ****
****  5.��Ҫ��(C15)��Ѻ��ֵ                                                ****
****  6.ע���������ݿ��к��ڱ����ж����Է�Ϊ��λ                         ****
****  7.ע����Dbs����ȡ�����ֶ�,���ȳ���6����������doulbe�ͱ�ʾ��,������   ****
****    int��long��,����ʹ��sprintf����ʱҪע������                        ****
****  8.����Ը���������ֵ,����ĳ��ҵ����������Ѿ����������ĸ����ֶ�  ****
****    ��ô����ֱ�Ӵӱ���ȡ�������ϣ������lv_addit����ȡ����������,ʹ��  ****
****    �����ɲο�����,���߸���iLvGetAddit������ʵ�ַ������Լ�д����      ****
****  9.ʹ��lv_connectʱҪע�����,����������LV_SERVER_BATCH,������������  ****
****    LV_SERVER_RT                                                       ****
****  10.�ѷ��ͺͽ��ձ��ķŵ��������ݿ��֮��                              ****
****  11.����lv_get_ptdj_tag��һ��ƴ��ͨ����PKG001���ĵķ���,���˿��Բο�  ****
****     �����Ѿ�д�õ�ƴ�������ɲ�����д��������������ϵ�ע����������    ****
****     ��Ӧ�޸�                                                          ****
****  12.����д�ĳ������ӱ�ע,�ر����޸�(�������Ӻ�ɾ������)�����Լ�д�� ****
****     ����ʱ,����ע����˭�ں������ԭ�������޸�                         ****
****  ����                                                                 ****
****      1.��ע���ڳ��������˳����쳣�˳�ʱ��Ҫ�ر�SOCKET��destroy���ṹ  ****
****      1.����ǰҪ���뷢�͵Ǽǲ�,����lv_insSendList����                  ****
******************************************************************************/

/* Ϊ�˱����ȥ�����˸��ٵ� */
int lv_Ins_Cmthtr(NOTPAY_IN_AREA *notpayin)
{
	return 0;
}
/****Add by SSH,2006.4.8,�ѷ��ͺͽ��ղ��������*****/
int iLvSendPackage(TLVPACKAGE *lv)
{
	int iRc=0,iSocketID=0;
	char pcCmtno[4];
	char respCode[9];
	char respBuf[512];
	memset(respCode,'\0',sizeof(respCode));
	memset(respBuf,'\0',sizeof(respBuf));
	memset(pcCmtno,'\0',sizeof(pcCmtno));
	memcpy(pcCmtno,lv->CMTCode,3);
	vtcp_log("%s,%d,����[.3s]\n",__FL__,pcCmtno);
	iRc = lv_initPara(NULL);
	if (iRc)
	{
		vtcp_log("%s,%d,��ʼ������������� return = %d!",__FL__,iRc);
		sprintf( acErrMsg,"��ʼ�������������!");
		WRITEMSG
		sprintf( acErrMsg,"��ʼ�������������!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	if (strstr("003,004,009,010,316,321,323,013",pcCmtno)!=NULL)
	{
		iSocketID = lv_connect(LV_SERVER_RT);
	}
	else
	{
		iSocketID = lv_connect(LV_SERVER_BATCH);
	}
	if (iSocketID<=0)
	{
		vtcp_log("%s,%d,����ǰ�û����� return = %d!",__FL__,iSocketID);
		sprintf( acErrMsg,"����ǰ�û�����!");
		WRITEMSG
			sprintf( acErrMsg,"����ǰ�û�����!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,����ǰ�û��ɹ� return = %d!",__FL__,iSocketID);	
	iRc = lv_send(iSocketID,lv);
	if (iRc)
	{
		vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",
			__FL__,iRc);
		lv_netClose(iSocketID);
		sprintf( acErrMsg,"�������ݰ���ǰ�û�����!");
		WRITEMSG
		sprintf( acErrMsg,"�������ݰ���ǰ�û�����!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,�������ݰ���ǰ�û��ɹ� !",__FL__);		
	lv_reset(lv);
	iRc = lv_recv(iSocketID,lv);
	if (iRc)
	{
		vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",
			__FL__,iRc);
		lv_netClose(iSocketID);
		sprintf( acErrMsg,"��ǰ�û��������ݰ�����!");
		WRITEMSG
		sprintf( acErrMsg,"��ǰ�û��������ݰ�����!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,��ǰ�û��������ݰ��ɹ� !",__FL__);		
	iRc = lv_analyze_pack(lv);
	if (iRc)
	{
		vtcp_log("%s,%d,���յ��ı��ĸ�ʽ���� return = %d!",__FL__,iRc);
		sprintf( acErrMsg,"���յ��ı��ĸ�ʽ����!");
		WRITEMSG
		sprintf( acErrMsg,"���յ��ı��ĸ�ʽ����!");
		set_zd_data("0130",acErrMsg);
		lv_netClose(iSocketID);
		return(-1);
	}
	vtcp_log("%s,%d,���յ��ı��ĸ�ʽ��ȷ !",__FL__);		
	memset(respCode,0,sizeof(respCode));
	memset(respBuf,0,sizeof(respBuf));
	lv_get_tag(lv,"C42",respCode);
	lv_get_tag(lv,"72A",respBuf);
	if ( respCode[3]!='I' )
	{
		char cErrmsg[100];
		memset(cErrmsg, 0 , sizeof(cErrmsg));
		WRITEMSG
		vtcp_log("%s,%d,���ķ��͵�ǰ�û�ʧ�� ERRNO=%s\nERRNOINFO=%s!",__FL__,respCode,respBuf);
		memcpy(cErrmsg,"ǰ�û���Ӧʧ��,",15);
		memcpy(cErrmsg+15,respBuf+8,sizeof(cErrmsg)-15);
		memcpy(acErrMsg,cErrmsg, sizeof(acErrMsg)-1);
		set_zd_data("0130",acErrMsg);
		vtcp_log("%s,%d errmsg=[%s]",__FL__,acErrMsg);
		lv_netClose(iSocketID);
		return(-1);
	}
	lv_netClose(iSocketID);  

	vtcp_log("%s,%d,iLvSendPackage() �ɹ� !",__FL__);		
	return(0);
}

struct lv_pkgreg_c lv_pkgreg;

/****Add by SSH,2005.11.27,��һ���������ַ�����ת��Ϊ��\0��β�Ĵ�****/
static char *ADDZERO_a(char *str,int length)
{
	static char buf[513];
	memset(buf,'\0',length+1);
	memcpy(buf,str,length);
	return(buf);
}
/****Add by SSH,2005.11.27,��һ���������ַ�����ת��Ϊ��\0��β�Ĵ�������ո�****/
static char *ADDZERO_b(char *str,int length)
{
	static char buf[513];
	int i=0;
	memset(buf,'\0',length+1);
	memcpy(buf,str,length);
	buf[length]='\0';
	for (i=0;i<length;i++)
	{
		if (buf[i]=='\0')
			buf[i]=' ';
	}
	return(buf);
}
#define ADDZERO(s) ADDZERO_a(s,sizeof(s))
#define ADDSPZ(s) ADDZERO_b(s,sizeof(s))
/****Add by SSH,2005.11.27,һ��ƴ������****/
int lv_Upd_Pkgreg_Stat(char i_stat, char *fmt, ...)
{
	struct  lv_pkgreg_c  wd_lv_pkgreg;
	char sqlbuf[512];
	int i=0;
	va_list vaptr;
	va_start(vaptr, fmt);
	vsprintf(sqlbuf, fmt, vaptr);
	va_end(vaptr);

	memset(&wd_lv_pkgreg, 0x0, sizeof(wd_lv_pkgreg));

	g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "%s", sqlbuf );
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α����Lv_pkgreg - %d ", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	while (1)
	{
		g_reply_int = Lv_pkgreg_Fet_Upd(&wd_lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int==100 && i!=0)break;
		if (g_reply_int)
		{
			sprintf( acErrMsg,"ȡ�α����Lv_pkgreg - %d ", g_reply_int);
			WRITEMSG
				set_zd_data("0130",acErrMsg);
			return(-1);	
		}
		vtcp_log("[%s][%d]i_stat=[%s]",__FILE__,__LINE__,i_stat);
		wd_lv_pkgreg.lv_sts[0] = i_stat;
		vtcp_log("[%s][%d]lv_pkgreg.lv_sts=[%c]",__FILE__,__LINE__,wd_lv_pkgreg.lv_sts[0]);
		g_reply_int = Lv_pkgreg_Upd_Upd(wd_lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"���¼�¼����Lv_pkgreg - %d ", g_reply_int);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
				return(-1);
		}
		/***
		Lv_pkgreg_Sel(g_pub_tx.reply,&wd_lv_pkgreg,sqlbuf);
		***/
		i++;
		vtcp_log("[%s][%d]i==[%d]",__FILE__,__LINE__,i);
		vtcp_log("[%s][%d]lv_pkgreg.lv_sts=[%c]",__FILE__,__LINE__,wd_lv_pkgreg.lv_sts[0]);
	}
	g_reply_int = Lv_pkgreg_Clo_Upd();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�رո����α���� = %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	return 0;
}
int lv_get_ptdj_tag(char *packday,char *packid)
{
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	char cSysday[9];
	struct lv_pkgreg_c lv_pkgreg;
	int iRc=0;
	long txcnt=0;/****��ϸ���ױ���****/
	char tmpBuf[1024];
	PAY_IN_AREA payin;

	TLVPACKAGE pkg001;
	static struct lv_wbctl_c      wd_lv_wbctl;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg001, '\0', sizeof(pkg001));
	memset(cSysday   ,0, sizeof(cSysday));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	lv_create(&pkg001);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	/** �α����ñ��� **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply,"pack_date = %ld and packid = '%s'",	wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		WRITEMSG
			sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg001);
		return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (DB_NOTFOUND == g_reply_int)
	{
		sprintf( acErrMsg,"��lv_wbctl��¼");
		WRITEMSG
			sprintf( acErrMsg,"��lv_wbctl��¼");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}else if(g_reply_int)
	{
		sprintf( acErrMsg,"ȡ��������ʧ��");
		WRITEMSG
			sprintf( acErrMsg,"ȡ��������ʧ��");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		strcpy( g_pub_tx.reply,"P071");
		sprintf( acErrMsg,"����¼״̬����!");
		WRITEMSG
			sprintf( acErrMsg,"����¼״̬����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg001);
		return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	txcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("%s,%d ������",__FL__);
	vtcp_log("[%s][%d]-----dtlcnt=[%d]txcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt,txcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and lw_ind = '1' and pay_qs_no = '%s' and packid = '%s' and pack_date = %ld and lv_sts = '%s' ", PKGNO_PTDJ, wd_lv_wbctl.pay_qs_no , wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, sql_stat);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"ȡlv_pkgreg�α���� %d", g_reply_int);
		WRITEMSG
			sprintf( acErrMsg,"ȡlv_pkgreg�α���� %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	/** ��ʼƴ���� **/
	/****Modified by SSH,2005.11.24,PKG��****/
	pkg001.opeType = '1';	/** С�� **/
	pkg001.pkgType = 1;			/** ��pkg�� **/
	memcpy(pkg001.CMTCode,PKGNO_PTDJ,3);	/** PKG�� **/
	memcpy(pkg001.workDate,pcLvGetDay_work(),8);/**ע��:��С�����ڲ����ۺ�ҵ������*/
	vtcp_log("%s,%d workdate===[%s] ",__FILE__,__LINE__,pkg001.workDate);

	/*******************************************/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK)
		{
			vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����,û�иü�¼!");
			lv_destroy(&pkg001);
			WRITEMSG
				sprintf( acErrMsg,"�������ݿ����,û�иü�¼!");
			set_zd_data("0130",acErrMsg);
			return(-1);
		}
		strcpy(sql_orbrno, lv_pkgreg.or_br_no);

		strcpy(sql_wtday, lv_date(lv_pkgreg.wt_date));

		strcpy(sql_orderno, lv_pkgreg.orderno);

		zip_space(sql_orbrno); zip_space(sql_wtday); zip_space(sql_orderno);

		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,sql_orbrno,sizeof(payin.F_orbrno));

		memcpy(payin.F_wtday,sql_wtday,sizeof(payin.F_wtday));

		memcpy(payin.F_orderno,sql_orderno,sizeof(payin.F_orderno));

		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));

		memcpy(payin.F_pkgno,PKGNO_PTDJ,sizeof(payin.F_pkgno));

		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];

		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];

		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));

		iLvZipAll(&payin);
		iLvResetPayin(&payin);	

		/** Ҫ�ؼ����� **/
		memset(tmpBuf, '\0', sizeof(tmpBuf));
		memcpy(pkg001.CMTNo,"001",3);		/***Ҫ�ؼ���001***/
		pkg001.paymPRI='0';					/***���ȼ�***/
		lv_set_tag(&pkg001,"0BG",ADDZERO(payin.F_txnum));/** ҵ�����ͺ� **/
		lv_set_tag(&pkg001,"52A",ADDZERO(payin.F_orbrno));/** �������к� **/
		lv_set_tag(&pkg001,"58A",ADDZERO(payin.F_acbrno));/** �������к� **/
		lv_set_tag(&pkg001,"30A",ADDZERO(payin.F_wtday));/**	ί������ **/
		lv_set_tag(&pkg001,"0BC",ADDZERO(payin.F_orderno));/**	֧��������� **/
		lv_set_tag(&pkg001,"33G",ADDZERO(payin.F_txamt));	/**	��� **/
		lv_set_tag(&pkg001,"CC4",ADDZERO(payin.F_payno));/**�����˿����к�*/
		lv_set_tag(&pkg001,"50C",ADDZERO(payin.F_payactno));/**�������˺�*/
		lv_set_tag(&pkg001,"50A",ADDZERO(payin.F_payname));	/**����������**/
		lv_set_tag(&pkg001,"50B",ADDZERO(payin.F_payaddress));	/**�����˵�ַ**/
		lv_set_tag(&pkg001,"CC5",ADDZERO(payin.F_cashno));	/**	�տ��˿����к�*/
		lv_set_tag(&pkg001,"59C",ADDZERO(payin.F_cashactno));/**�տ����˺�**/
		lv_set_tag(&pkg001,"59A",ADDZERO(payin.F_cashname));/**�տ������� **/
		lv_set_tag(&pkg001,"59B",ADDZERO(payin.F_cashaddress));/**�տ��˵�ַ**/
		lv_set_tag(&pkg001,"CEG",ADDZERO(payin.F_ywtype));	/**	ҵ������ **/
		lv_set_tag(&pkg001,"72A",ADDZERO(payin.F_content));	/**	���� **/

		vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
		/****����ƴ�����ֶ�****/
		if (memcmp(payin.F_txnum,TXNUM_HD,sizeof(payin.F_txnum))==0)
		{
			/****���ҵ���޸������ݸ�ʽ***/
			lv_set_tag(&pkg001,"B40","00000000");	/**	�������ݳ���**/
		}
		else if (memcmp(payin.F_txnum,TXNUM_WT,sizeof(payin.F_txnum))==0)
		{
			/****ί���տ�ҵ��****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[18]='\0';
			lv_set_tag(&pkg001,"B40","00000018");	/**	�������ݳ���**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	�������� **/
		}
		else if (memcmp(payin.F_txnum,TXNUM_TSCF,sizeof(payin.F_txnum))==0)
		{
			/****���ճи�ҵ��****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[91]='\0';
			lv_set_tag(&pkg001,"B40","00000091");	/**	�������ݳ���**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	�������� **/
		}
		else if (memcmp(payin.F_txnum,TXNUM_GKDJ,sizeof(payin.F_txnum))==0)
		{
			/****�������ҵ��****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[111]='\0';
			lv_set_tag(&pkg001,"B40","00000111");	/**	�������ݳ���**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	�������� **/
		}
		else
		{
			/****����δ����ĸ���������ʹ�ø������ݱ����õ�****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****û�и�������****/
				lv_set_tag(&pkg001,"B40","00000000");	/**	�������ݳ���**/
			}
			else
			{
				/****�и�������****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg001,"B40",len_buf);	/**	�������ݳ���**/
				lv_set_tag(&pkg001,"72C",tmpBuf);		/**	�������� **/
			}
		}
		vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
		lv_batch_end(&pkg001);
	}
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�ر�Lv_pkgreg�α���� %d ", g_reply_int);
		WRITEMSG
			sprintf( acErrMsg,"�ر�Lv_pkgreg�α���� %d ", g_reply_int);
		return(-1);
	}
	/*
	change by liuyue 20061228  26�ŵİ���28�ŷ��͵�������
	memcpy(cSysday,(char *)pcLvGetDay(),8);	
	*/
	sprintf(cSysday,"%8d",wd_lv_wbctl.pack_date);
	/** ��ͷ���� **/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_pkg_batch(&pkg001,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg001,"011",wd_lv_wbctl.pay_qs_no);	/** �������������к� **/
	lv_pkg_batch(&pkg001,"012",wd_lv_wbctl.cash_qs_no);/** �������������к� **/
	lv_pkg_batch(&pkg001,"30E",cSysday);	/** ��ί������ **/
	/*****
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	****/

	vtcp_log("%s,%d pack_date===[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_pkg_batch(&pkg001,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	/****Deleted by SSH,2005.11.24,��Ѻ��Τ�Ѹ�****/
	/****lv_pkg_batch(&pkg001,"C15",wd_lv_wbctl.packmy);** ����Ѻ **/
	/*********/
	/****Modified by SSH,2005.11.24,����6λ�������Ǹ�����****/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);

	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	vtcp_log("%s,%d tmpBuf===%s",__FL__,tmpBuf);
	lv_pkg_batch(&pkg001,"B63",tmpBuf);				/** ��ϸҵ���ܱ��� **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,����Է�Ϊ��λ,15λ***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg001,"32B",tmpBuf);			 /** ��ϸҵ���ܽ�� **/

	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_gen_pack(&pkg001);
	printf("%s,%d ץ��1 \n",__FILE__,__LINE__);
	dm_show(&(pkg001.dm));
	/****ע��: �����ݿ���²����ŵ�����֮ǰ,�������ٲ�һ�µĿ���****/
	/*sql_stat2[0]=STAT_WZSEND;*/
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lw_ind = '1' and pkgno = '%s' and lv_sts ='%s' and packid = '%s' and pack_date = %ld and pay_qs_no = '%s' ", PKGNO_PTDJ, sql_stat, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.pay_qs_no );
	WRITEMSG
	if (g_reply_int !=DB_OK)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg001);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);

	wd_lv_wbctl.stat[0] =PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg001);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	g_reply_int = lv_insSendList(&pkg001);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		WRITEMSG
		sprintf( acErrMsg,"�����������Ʊ����");
		lv_destroy(&pkg001);
		vtcp_log("[%s][%d]errmsg--[%s]\n",__FL__,acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);

	dm_show(&(pkg001.dm));
	vtcp_log("%s,%d,����\n",__FL__);
	iRc=iLvSendPackage(&pkg001);
	if (iRc)
	{
		lv_destroy(&pkg001);
		vtcp_log("[%s][%d]errmsg--[%s]\n",__FL__,acErrMsg);
		return(-1);
	}
	lv_destroy(&pkg001);
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	return(0);
}
/** ƴʵʱ���Ƿ��Ͱ�  pkg003 **/              
int lv_get_ssdj_tag(char *packday, char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];
	char    sql_stat2[2];
	TLVPACKAGE pkg003;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
	char	PAYNO[13];
	char	PAYACTNO[33];
	char	PAYNAME[61];
	char	PAYADDRESS[61];
	char	CASHNO[13];
	char	CASHACTNO[33];
	char	CASHNAME[61];
	char	CASHADDRESS[61];
	char	YWTYPE[13];
	char	CONTENT[61];
	char	ADDID[9];
	memset(&payin, 0 , sizeof(payin)); /* add by LiuHuafeng 20060429 */
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ADDID,    '\0', sizeof(ADDID));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(PAYNO,    '\0', sizeof(PAYNO));
	memset(PAYACTNO, '\0', sizeof(PAYACTNO));
	memset(PAYNAME,  '\0', sizeof(PAYNAME));
	memset(PAYADDRESS,'\0', sizeof(PAYADDRESS));
	memset(CASHNO,    '\0', sizeof(CASHNO));
	memset(CASHACTNO, '\0', sizeof(CASHACTNO));
	memset(CASHNAME,  '\0', sizeof(CASHNAME));
	memset(CASHADDRESS,'\0', sizeof(CASHADDRESS));
	memset(YWTYPE,	 '\0', sizeof(YWTYPE));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));

	lv_load_tagifno();
	lv_create(&pkg003);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	vtcp_log("[%s][%d]packid=[%s]\n",__FL__,packid);
	wd_lv_wbctl.pack_date = apatoi( packday, 8);
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s' ", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α����- %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** �õ�������double�䵽int **/

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, lv_date(wd_lv_wbctl.pack_date), sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg003.opeType = '1';	/** С�� **/
	pkg003.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg003.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg003.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg003.CMTCode,PKGNO_SSDJ,3); 	/** ��Ϊ�� PKG003��**/
	memcpy(pkg003.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	vtcp_log("[%s][%d]-------packid=[%s],packday=[%s],sql_stat=[%s]\n",__FL__,packid,packday,sql_stat);
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "packid = '%s' and pack_date= %s and lv_sts = '%s' and pkgno = '%s' and lw_ind = '1'", packid, packday, sql_stat, PKGNO_SSDJ);
	if (g_reply_int != DB_OK)
	{ sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL DECLARE SSDJ1 CURSOR FOR 
	select txnum,orbrno,acbrno,wtday,orderno,txamt,payno,payactno,payname,payaddress,cashno,cashactno,cashname,
	cashaddress,ywtype,content,addid from lv_ssdjs where packid =:packid and packday=:packday and stat =:sql_stat;

	EXEC SQL OPEN SSDJ1;

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	EXEC SQL FETCH SSDJ1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,:TXAMT,:PAYNO,:PAYACTNO,:PAYNAME,:PAYADDRESS,
	:CASHNO,:CASHACTNO,:CASHNAME,:CASHADDRESS,:YWTYPE,:CONTENT,:ADDID;*/

	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int !=DB_OK && g_reply_int !=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** Ҫ�ؼ����� **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memcpy(pkg003.CMTNo,"001",3);       /** Ҫ�ؼ���001***/
	lv_set_tag(&pkg003,"0BG",lv_pkgreg.txnum);	/** ҵ�����ͺ� **/
	lv_set_tag(&pkg003,"52A",lv_pkgreg.or_br_no);	/** �������к� **/
	lv_set_tag(&pkg003,"58A",lv_pkgreg.ac_br_no);	/** �������к� **/
	lv_set_tag(&pkg003,"30A", lv_date(lv_pkgreg.wt_date));	/**	ί������ **/
	lv_set_tag(&pkg003,"0BC", lv_pkgreg.orderno);	/**	֧��������� **/
	TXAMT = lv_pkgreg.tx_amt*100.0;
	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg003,"33G",tmpBuf);	/**	��� **/
	lv_set_tag(&pkg003,"CC4", lv_pkgreg.pay_opn_br_no);	/**	�����˿������к� **/
	lv_set_tag(&pkg003,"50C", lv_pkgreg.pay_ac_no);	/**	�������˺� **/
	lv_set_tag(&pkg003,"50A", lv_pkgreg.pay_name);	/**	���������� **/
	lv_set_tag(&pkg003,"50B", lv_pkgreg.pay_addr);	/**	�����˵�ַ **/
	lv_set_tag(&pkg003,"CC5", lv_pkgreg.cash_opn_br_no);	/**	�տ��˿������к� **/
	lv_set_tag(&pkg003,"59C", lv_pkgreg.cash_ac_no);	/**	�տ����˺� **/
	lv_set_tag(&pkg003,"59A", lv_pkgreg.cash_name);	/**	�տ������� **/
	lv_set_tag(&pkg003,"59B", lv_pkgreg.cash_addr);	/**	�տ��˵�ַ **/
	lv_set_tag(&pkg003,"CEG", lv_pkgreg.lv_yw_ind);	/**	ҵ������ **/
	lv_set_tag(&pkg003,"72A", lv_pkgreg.lv_brf);	/**	���� **/
	/** ƴ�������� **/
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	apitoa(lv_pkgreg.wt_date,8,payin.F_wtday);
	memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
	memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
	memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(payin.T_addid));

	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	vtcp_log("[%s][%d]==payin.T_addid=[%.8s]\n",__FL__,payin.T_addid);
	vtcp_log("[%s][%d]==payin.T_addid=[%c]\n",__FL__,payin.T_addid[0]);
	if (payin.T_addid[0]=='\0')
	{
		lv_set_tag(&pkg003,"B40","00000000");   /** �������ݳ���**/
		vtcp_log("[%s][%d]==payin.T_addid=[%s]\n",__FL__,payin.T_addid);
	}
	else
	{
		char len_buf[9];
		vtcp_log("[%s][%d]=payin.T_addid=[%s]\n",__FL__,payin.T_addid);
		iLvGetAddit(&payin,"000",tmpBuf,0);
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		lv_set_tag(&pkg003,"B40",len_buf);  /** �������ݳ���**/
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		lv_set_tag(&pkg003,"72C",tmpBuf);       /** �������� **/
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
	}
	lv_batch_end(&pkg003);
	g_reply_int= Lv_pkgreg_Clo_Sel();
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*	EXEC SQL CLOSE SSDJ1;*/

	/** ��ͷ���� **/
	lv_pkg_batch(&pkg003,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg003,"011",wd_lv_wbctl.pay_qs_no);			/** �������������к� **/
	lv_pkg_batch(&pkg003,"012",wd_lv_wbctl.cash_qs_no);		/** �������������к� **/
	lv_pkg_batch(&pkg003,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg003,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	/** lv_pkg_batch(&pkg003,"C15",wd_lv_wbctl.packmy);	** ����Ѻ **/

	/** apitoa(wd_lv_wbctl.dtlcnt,sizeof(Cnt)-1,Cnt); **/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg003,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/

	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg003,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	/** lv_pkg_batch(&pkg003,"72D","");	** ���������� **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	vtcp_log("[%s][%d]==========\n",__FL__);
	lv_gen_pack(&pkg003);
	vtcp_log("[%s][%d]==========\n",__FL__);
	/**
	dm_show(&(pkg003.dm));
	**/
	vtcp_log("[%s][%d]==========\n",__FL__);

	/** ����״̬Ϊ�ѷ��� **/
	vtcp_log("%s,%d,***************************\n",__FL__);
	vtcp_log("stat2=[%s]\n,packid=[%s]\n,packday=[%s]\n,stat=[%s]\n",
		sql_stat2,packid,packday,sql_stat);

	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lw_ind = '1' and pkgno = '%s' and packid = '%s' and pack_date= '%s'  and lv_sts = '%s'", PKGNO_SSDJ, packid, packday, sql_stat);
	if (g_reply_int !=DB_OK && g_reply_int != DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] =PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg003);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** ���� **/
	vtcp_log("%s,%d,����\n",__FL__);
	if (iRc=iLvSendPackage(&pkg003))
	{
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg003);
	return(0);

}
/** ƴʵʱ��Ƿ��Ͱ� pkg004 **/              
int lv_get_ssjj_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024*8];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];	
	char    sql_stat2[2];	
	TLVPACKAGE pkg004;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
	char	PAYNO[13];
	char	PAYACTNO[33];
	char	PAYNAME[61];
	char	PAYADDRESS[61];
	char	CASHNO[13];
	char	CASHACTNO[33];
	char	CASHNAME[61];
	char	CASHADDRESS[61];
	char	YWTYPE[13];
	char	CONTENT[61];
	char	ADDID[9];

	memset(&payin, 0 , sizeof(payin));/* add by lhf0423 */
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ADDID,    '\0', sizeof(ADDID));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(PAYNO,    '\0', sizeof(PAYNO));
	memset(PAYACTNO, '\0', sizeof(PAYACTNO));
	memset(PAYNAME,  '\0', sizeof(PAYNAME));
	memset(PAYADDRESS,'\0', sizeof(PAYADDRESS));
	memset(CASHNO,    '\0', sizeof(CASHNO));
	memset(CASHACTNO, '\0', sizeof(CASHACTNO));
	memset(CASHNAME,  '\0', sizeof(CASHNAME));
	memset(CASHADDRESS,'\0', sizeof(CASHADDRESS));
	memset(YWTYPE,	 '\0', sizeof(YWTYPE));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));	
	memset(sql_stat2, '\0', sizeof(sql_stat2));	

	lv_load_tagifno();
	lv_create(&pkg004);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8);
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;	
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** �õ�������double�䵽int **/

	/** ���α���Ҫ���ֶθ�ֵ **
	memcpy(Txday,wd_lv_wbctl.pack_date, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);
	***********/

	/** ��ʼƴ���� **/
	pkg004.opeType = '1';	/** С�� **/
	pkg004.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg004.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg004.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg004.CMTCode,PKGNO_SSJJ,3);	/** ��Ϊ�� PKG004��**/
	memcpy(pkg004.CMTNo,"001",3);       /***Ҫ�ؼ���001***/
	pkg004.paymPRI='0';                 /***���ȼ�***/
	memcpy(pkg004.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lv_sts = '%s' and lw_ind = '1'", PKGNO_SSJJ, packid, packday, sql_stat);
	if (g_reply_int!=DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if (wd_dtlcnt!=0)                       /**����ѭ��**/
	{
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");

			lv_destroy(&pkg004);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");

			lv_destroy(&pkg004);
			WRITEMSG
				return(-1);
		}
		/** Ҫ�ؼ����� **/
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORDERNO, lv_pkgreg.orderno);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(PAYNO, lv_pkgreg.pay_opn_br_no);
		strcpy(PAYACTNO, lv_pkgreg.pay_ac_no);
		strcpy(PAYNAME, lv_pkgreg.pay_name);
		strcpy(PAYADDRESS, lv_pkgreg.pay_addr);
		strcpy(CASHNO, lv_pkgreg.cash_opn_br_no);
		strcpy(CASHACTNO, lv_pkgreg.cash_ac_no);
		strcpy(CASHNAME, lv_pkgreg.cash_name);
		strcpy(CASHADDRESS, lv_pkgreg.cash_addr);
		strcpy(YWTYPE, lv_pkgreg.lv_yw_ind);
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		lv_set_tag(&pkg004,"0BG",TXNUM);	/** ҵ�����ͺ� **/
		lv_set_tag(&pkg004,"52A",ORBRNO);	/** �������к� **/
		lv_set_tag(&pkg004,"58A",ACBRNO);	/** �������к� **/
		lv_set_tag(&pkg004,"30A",WTDAY);	/**	ί������ **/
		lv_set_tag(&pkg004,"0BC",ORDERNO);	/**	֧��������� **/
		TXAMT = lv_pkgreg.tx_amt*100.0;
		sprintf(tmpBuf,"%015.0f",TXAMT);
		vtcp_log("[%s][%d]====TXAMT=[%f]\n",__FL__,TXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg004,"33G",tmpBuf);	/**	��� **/
		lv_set_tag(&pkg004,"CC4",PAYNO);	/**	�����˿������к� **/
		lv_set_tag(&pkg004,"50C",PAYACTNO);	/**	�������˺� **/
		lv_set_tag(&pkg004,"50A",PAYNAME);	/**	���������� **/
		lv_set_tag(&pkg004,"50B",PAYADDRESS);	/**	�����˵�ַ **/
		lv_set_tag(&pkg004,"CC5",CASHNO);	/**	�տ��˿������к� **/
		lv_set_tag(&pkg004,"59C",CASHACTNO);	/**	�տ����˺� **/
		lv_set_tag(&pkg004,"59A",CASHNAME);	/**	�տ������� **/
		lv_set_tag(&pkg004,"59B",CASHADDRESS);	/**	�տ��˵�ַ **/
		lv_set_tag(&pkg004,"CEG",YWTYPE);	/**	ҵ������ **/
		lv_set_tag(&pkg004,"72A",CONTENT);	/**	���� **/
		/** ƴ�������� **/

		/**   ʵʱ��˰,ͨ�Ҷ�����������ʱ�������   ***/

		memset(tmpBuf,'\0',sizeof(tmpBuf));
		apitoa(lv_pkgreg.wt_date,8,payin.F_wtday);
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(payin.T_addid));
		/****HSYX BEGIN:ƴ001-005�ĸ�����****/
		if (payin.T_addid[0]=='\0')
		{
			lv_set_tag(&pkg004,"B40","00000000");   /** �������ݳ���**/
		}
		else
		{
			iLvGetAddit(&payin,"000",tmpBuf,0);
		}
		if (tmpBuf[0]!='\0' && memcmp(lv_pkgreg.txnum,"30103",sizeof(lv_pkgreg.txnum)-1)!=0)
		{
			/****�и�������****/
			char len_buf[9];
			memset(len_buf, 0 , sizeof(len_buf));
			sprintf(len_buf,"%08d",strlen(tmpBuf));
			lv_set_tag(&pkg004,"B40",len_buf);  /** �������ݳ���**/
			lv_set_tag(&pkg004,"72C",tmpBuf);       /** �������� **/
			vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
		}
		else if (payin.T_addid[0]!='\0')
		{
			vtcp_log("[%s][%d]=============\n",__FILE__,__LINE__);
			/****û�б�׼000��������****/
			/****HSYX BEGIN:ƴ001-005�ĸ�����****/
			int sIndex=0;
			int sAddLen=0;
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			for (sIndex=1;sIndex<=5;sIndex++)
			{
				char pcTagName[4];
				sprintf(pcTagName,"%03d",sIndex);
				iLvGetAddit(&payin,pcTagName,tmpBuf+sAddLen,0);
				printf("%s,%d,Gethere,��[%d]��������\n[%s]\n",__FILE__,__LINE__,sIndex,tmpBuf);
				if (tmpBuf[sAddLen]=='\0')
				{
					/****�޴˸����򣬸�����ѭ������****/
					if (sIndex==2)
					{
						/****�������б�****/
						strcat(tmpBuf+sAddLen,"00");
						sAddLen=strlen(tmpBuf);
					}
					else
					{
						break;
					}
				}
				else
				{
					/****�д˸�����ƴ������****/
					sAddLen=strlen(tmpBuf);
				}
				vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			}
			if (tmpBuf[0]=='\0')
			{
				lv_set_tag(&pkg004,"B40","00000000");   /** �������ݳ���**/
			}else
			{
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg004,"B40",len_buf);  /** �������ݳ���**/
				lv_set_tag(&pkg004,"72C",tmpBuf);       /** �������� **/
				vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			}
			/****HSYX END:ƴ001-005�ĸ�����****/
		}
		lv_batch_end(&pkg004);
	}
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE SSJJ1;*/

	/** ��ͷ���� **/
	lv_pkg_batch(&pkg004,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg004,"011",wd_lv_wbctl.pay_qs_no);			/** �������������к� **/
	lv_pkg_batch(&pkg004,"012",wd_lv_wbctl.cash_qs_no);		/** �������������к� **/
	lv_pkg_batch(&pkg004,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg004,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	/** lv_pkg_batch(&pkg004,"C15",wd_lv_wbctl.packmy);	** ����Ѻ **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg004,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/

	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg004,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	/**    lv_pkg_batch(&pkg004,"72D","");	** ���������� **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	lv_gen_pack(&pkg004);
	/**
	dm_show(&(pkg004.dm));
	**/
	/** ����״̬Ϊ�ѷ��� **/
	/** Ҳ���·�����Ϊ�������У�������Ӧ��ʱ����ס�� dxy 20060206 **/
	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lw_ind = '1' and pkgno = '%s' and packid = '%s' and pack_date = %s and lv_sts = '%s'", PKGNO_SSJJ, packid, packday, sql_stat);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg004); 
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	/** ���� **/
	vtcp_log("%s,%d,����\n",__FL__);
	if (iRc=iLvSendPackage(&pkg004))
	{
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg004);
	return(0);

}
/****Add by SSH,2005.11.28,����ƴ������****/
int lv_get_dqdj_tag_ssh(char *packday,char *packid)
{
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	int iRc=0;
	long txcnt=0;/****��ϸ���ױ���(����)****/
	int listcnt=0;/****�տ����嵥������****/
	char preActno[33];/****����ǰһ�ʵĸ����ʺ�****/
	char preWtday[9];/****����ǰһ�ʵ�ί������****/
	char tmpBuf[1024];
	PAY_IN_AREA payin;

	TLVPACKAGE pkg005;
	static struct lv_wbctl_c      wd_lv_wbctl;
	iRc=0;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg005, '\0', sizeof(pkg005));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	lv_create(&pkg005);
	/** �α����ñ��� **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",
		__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",
		__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int= Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");     

		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"����¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
		lv_destroy(&pkg005);
		return(-1);
	}
	txcnt = wd_lv_wbctl.dtlcnt;

	/** ���α���Ҫ���ֶθ�ֵ **/
	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	/*=====================================================================================
	EXEC SQL DECLARE DQDJ1 CURSOR FOR 
	select orbrno,wtday,orderno
	from lv_dqdjs 
	where 	payqsactno=:wd_lv_wbctl.pay_qs_no and 
	packid =:wd_lv_wbctl.packid and 
	packday=:wd_lv_wbctl.pack_date and 
	stat=:sql_stat;	
	EXEC SQL OPEN DQDJ1;
	*===================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DQDJ, packid, packday, sql_stat);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		WRITEMSG
		return(-1);
	}
	/** ��ʼƴ���� **/
	/****Modified by SSH,2005.11.24,PKG��****/
	pkg005.opeType = '1';	/** С�� **/
	pkg005.pkgType = 1;			/** ��pkg�� **/
	memcpy(pkg005.CMTCode,PKGNO_DQDJ,3);	/** PKG�� **/
	memcpy(pkg005.workDate,pcLvGetDay_work(),8);/**ע��:��С�����ڲ����ۺ�ҵ������*/
	/*******************************************/
	listcnt=0;
	memset(preActno,0x15,sizeof(preActno)-1);
	preActno[sizeof(preActno)-1]='\0';
	memset(preWtday,0x15,sizeof(preWtday)-1);
	preWtday[sizeof(preWtday)-1]='\0';
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int!=DB_OK)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",
				__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");

			lv_destroy(&pkg005);
			WRITEMSG
				return(-1);
		}
		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_wtday,lv_date(lv_pkgreg.wt_date),sizeof(payin.F_wtday));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
		memcpy(payin.F_pkgno,PKGNO_DQDJ,sizeof(payin.F_pkgno));
		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];
		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));
		iLvZipAll(&payin);
		iLvResetPayin(&payin);	
		/** Ҫ�ؼ����� **/
		if (memcmp(payin.F_payactno,preActno,sizeof(preActno)-1)==0 &&
			memcmp(payin.F_wtday,preWtday,sizeof(preWtday)-1)==0)
		{
			/****������һ��ҵ��,ֻ�ı��տ����嵥****/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_orderno));/**	֧��������� **/
			sprintf(tmpBuf,"%08d",listcnt+1);/****�տ���Ϣ���****/
			lv_set_tag_multi(&pkg005,"B81",tmpBuf);
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_acbrno));/*�տ���*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_cashno));/**�տ��˿�����*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashactno));/**�տ����ʺ�*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashname));/*�տ�������*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashaddress));/**�տ��˵�ַ*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_txamt));/**	��� **/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_content));/**���� **/

			/****����ƴ�����ֶ�****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****û�и�������****/
				lv_set_tag_multi(&pkg005,"B81","00000000");	/**	�������ݳ���**/
			}else
			{
				/****�и�������****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag_multi(&pkg005,"B81",len_buf);	/**	�������ݳ���**/
				lv_set_tag_multi(&pkg005,"B81",tmpBuf);		/**	�������� **/
			}
			listcnt++;/****�տ��嵥��ϸ������1****/
		}
		else
		{
			listcnt=0;
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			memcpy(pkg005.CMTNo,"003",3);		/***Ҫ�ؼ���003***/
			pkg005.paymPRI='0';					/***���ȼ�***/
			memcpy(preActno,payin.F_payactno,sizeof(preActno)-1);
			memcpy(preWtday,payin.F_wtday,sizeof(preWtday)-1);
			lv_set_tag(&pkg005,"0BG",ADDZERO(payin.F_txnum));/** ҵ�����ͺ� **/
			lv_set_tag(&pkg005,"CCM",ADDZERO(payin.F_orbrno));/** �������к�**/
			lv_set_tag(&pkg005,"30A",ADDZERO(payin.F_wtday));/**	ί������ **/
			lv_set_tag(&pkg005,"CC4",ADDZERO(payin.F_payno));/**�����˿�����*/
			lv_set_tag(&pkg005,"50C",ADDZERO(payin.F_payactno));/**�������˺�*/
			lv_set_tag(&pkg005,"50A",ADDZERO(payin.F_payname));/**����������**/
			lv_set_tag(&pkg005,"50B",ADDZERO(payin.F_payaddress));/**�����ַ*/
			lv_set_tag(&pkg005,"CEG",ADDZERO(payin.F_ywtype));	/**	ҵ������ **/
			sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);/****�տ��嵥����****/
			lv_set_tag(&pkg005,"B61",tmpBuf);		

			lv_set_tag(&pkg005,"B81",ADDZERO(payin.F_orderno));/**	֧��������� **/
			sprintf(tmpBuf,"%08d",listcnt+1);/****�տ���Ϣ���****/
			lv_set_tag_multi(&pkg005,"B81",tmpBuf);
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_acbrno));/*�տ���*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_cashno));/**�տ��˿�����*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashactno));/**�տ����ʺ�*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashname));/*�տ�������*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashaddress));/**�տ��˵�ַ*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_txamt));/**	��� **/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_content));/**���� **/

			/****����ƴ�����ֶ�****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****û�и�������****/
				lv_set_tag_multi(&pkg005,"B81","00000000");	/**	�������ݳ���**/
			}else
			{
				/****�и�������****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag_multi(&pkg005,"B81",len_buf);	/**	�������ݳ���**/
				lv_set_tag_multi(&pkg005,"B81",tmpBuf);		/**	�������� **/
			}
			listcnt++;/****�տ��嵥��ϸ������1****/
		}
	}
	lv_batch_end(&pkg005);
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
		return(-1);
	}
	/****�����һ�ʸ����¼������****/

	/** ��ͷ���� **/
	vtcp_log("[%s][%d]lv_wbctl.pack_date=[%ld]",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]lvca.lv_date=[%s]",__FILE__,__LINE__,lvca.lv_date);
	lv_pkg_batch(&pkg005,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg005,"011",wd_lv_wbctl.pay_qs_no);	/** �������������к� **/
	lv_pkg_batch(&pkg005,"012",wd_lv_wbctl.cash_qs_no);/** �������������к� **/
	lv_pkg_batch(&pkg005,"30E",packday);	/** ��ί������ **/
	/**
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	**/
	lv_pkg_batch(&pkg005,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	/****Deleted by SSH,2005.11.24,��Ѻ��Τ�Ѹ�****/
	/****lv_pkg_batch(&pkg005,"C15",wd_lv_wbctl.packmy);** ����Ѻ **/
	/*********/
	/****Modified by SSH,2005.11.24,����6λ�������Ǹ�����****/
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg005,"B63",tmpBuf);				/** ��ϸҵ���ܱ��� **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,����Է�Ϊ��λ,15λ***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg005,"32B",tmpBuf);			 /** ��ϸҵ���ܽ�� **/
	lv_gen_pack(&pkg005);
	/****ע��: �����ݿ���²����ŵ�����֮ǰ,�������ٲ�һ�µĿ���****/
	sql_stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' and pay_qs_no = '%s' " , sql_stat, packid, packday, PKGNO_DQDJ, wd_lv_wbctl.pay_qs_no);

	if (g_reply_int!=DB_OK)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg005);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	vtcp_log("%s,%d,����\n",__FL__);
	iRc=iLvSendPackage(&pkg005);
	if (iRc)
	{
		lv_destroy(&pkg005);
		WRITEMSG
		return(-1);
	}
	lv_destroy(&pkg005);
	return(0);

}
int lv_get_dqjj_tag(char *packday,char *packid)
{
	vtcp_log("[%s][%d]===============w\n", __FL__);
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	int iRc=0;
	int	iSocketID=0;
	long txcnt=0;/****��ϸ���ױ���(����)****/
	int listcnt=0;/****�������嵥������****/
	char preActno[33];/****����ǰһ�ʵ��տ��ʺ�****/
	char preWtday[9];/****����ǰһ�ʵ�ί������****/
	char tmpBuf[1024];
	char respCode[9];
	char respBuf[100];
	char pcDfxyNo[60];
	char cHzDate[3];
	memset(cHzDate, 0 , sizeof(cHzDate));

	vtcp_log("[%s][%d]===============w\n", __FL__);
	PAY_IN_AREA payin;

	TLVPACKAGE pkg006;
	static struct lv_wbctl_c      wd_lv_wbctl;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg006, '\0', sizeof(pkg006));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	lv_create(&pkg006);
	/** �α����ñ��� **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n", __FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n", __FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]=w\n", __FL__);
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");

		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"����¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	txcnt = wd_lv_wbctl.dtlcnt;

	vtcp_log("[%s][%d]===============w\n", __FL__);
	/** ���α���Ҫ���ֶθ�ֵ **/
	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	/*==========================================================================================
	EXEC SQL DECLARE DQJJ1 CURSOR FOR 
	select orbrno,wtday,orderno
	from lv_dqjjs 
	where 	payqsactno=:wd_lv_wbctl.pay_qs_no and 
	packid =:wd_lv_wbctl.packid and 
	packday=:wd_lv_wbctl.pack_date and 
	stat=:sql_stat;	
	EXEC SQL OPEN DQJJ1;
	==========================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DQJJ, packid, packday, sql_stat);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/** ��ʼƴ���� **/
	/****Modified by SSH,2005.11.24,PKG��****/
	pkg006.opeType = '1';	/** С�� **/
	pkg006.pkgType = 1;			/** ��pkg�� **/
	memcpy(pkg006.CMTCode,PKGNO_DQJJ,3);	/** PKG�� **/
	memcpy(pkg006.workDate,pcLvGetDay_work(),8);/**ע��:��С�����ڲ����ۺ�ҵ������*/
	/*******************************************/
	listcnt=0;
	memset(preActno,0x15,sizeof(preActno)-1);
	preActno[sizeof(preActno)-1]='\0';
	memset(preWtday,0x15,sizeof(preWtday)-1);
	preWtday[sizeof(preWtday)-1]='\0';
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		/*EXEC SQL FETCH DQJJ1 INTO :sql_orbrno,:sql_wtday,:sql_orderno;*/
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int != DB_OK)
		{
			vtcp_log("%s,%d,���ݿ����,δ�ҵ���¼sqlcode=%d\n",
				__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����,δ�ҵ���¼!");
			WRITEMSG
				lv_destroy(&pkg006);
			return(-1);
		}
		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_wtday,lv_date(lv_pkgreg.wt_date),sizeof(payin.F_wtday));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
		memcpy(payin.F_pkgno,PKGNO_DQJJ,sizeof(payin.F_pkgno));
		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];
		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));
		iLvZipAll(&payin);
		iLvResetPayin(&payin);	
		/** Ҫ�ؼ����� **/
		if (	listcnt==0)
		{
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			memcpy(pkg006.CMTNo,"004",3);		/***Ҫ�ؼ���004***/
			pkg006.paymPRI='0';					/***���ȼ�***/
			memcpy(preActno,payin.F_cashactno,sizeof(preActno)-1);
			memcpy(preWtday,payin.F_wtday,sizeof(preWtday)-1);
			lv_set_tag(&pkg006,"0BG",ADDZERO(payin.F_txnum));/** ҵ�����ͺ� **/
			lv_set_tag(&pkg006,"CCN",ADDZERO(payin.F_orbrno));/** �տ����к�**/
			lv_set_tag(&pkg006,"30A",ADDZERO(payin.F_wtday));/**	ί������ **/
			lv_set_tag(&pkg006,"CC5",ADDZERO(payin.F_cashno));/**�տ��˿�����*/
			lv_set_tag(&pkg006,"59C",ADDZERO(payin.F_cashactno));/*�տ����˺�*/
			lv_set_tag(&pkg006,"59A",ADDZERO(payin.F_cashname));/*�տ�������**/
			lv_set_tag(&pkg006,"59B",ADDZERO(payin.F_cashaddress));/*�տ��ַ*/
			lv_set_tag(&pkg006,"CEG",ADDZERO(payin.F_ywtype));	/**	ҵ������ **/
			sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);/****�����嵥����****/
			lv_set_tag(&pkg006,"B62",tmpBuf);		
			lv_set_tag(&pkg006,"B82",ADDZERO(payin.F_orderno));/**	֧��������� **/
		}
		if (listcnt>0)
		{
			lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_orderno));/**	֧��������� **/
		}
		sprintf(tmpBuf,"%08d",listcnt+1);/****������Ϣ���****/
		lv_set_tag_multi(&pkg006,"B82",tmpBuf);
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_acbrno));/*������*/
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_payno));/**�����˿�����*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payactno));/**�������ʺ�*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payname));/*����������*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payaddress));/**�����˵�ַ*/
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_txamt));/**	��� **/
		/****�෽Э���****/
		memset(pcDfxyNo,'\0',sizeof(pcDfxyNo));
		iLvGetAddit(&payin,"XYH",pcDfxyNo,sizeof(pcDfxyNo));
		if (pcDfxyNo[0]=='\0')
		{
			memset(pcDfxyNo,'0',sizeof(pcDfxyNo));
		}
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(pcDfxyNo));/**�෽Э��� **/
		/****����ƴ�����ֶ�****/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_content));/**���� **/
		memset(tmpBuf,'\0',sizeof(tmpBuf));
		iLvGetAddit(&payin,"000",tmpBuf,0);
		if (tmpBuf[0]=='\0')
		{
			/****û�и�������****/
			lv_set_tag_multi(&pkg006,"B82","00000000");	/**	�������ݳ���**/
		}else
		{
			/****�и�������****/
			char len_buf[9];
			sprintf(len_buf,"%08d",strlen(tmpBuf));
			lv_set_tag_multi(&pkg006,"B82",len_buf);	/**	�������ݳ���**/
			lv_set_tag_multi(&pkg006,"B82",tmpBuf);		/**	�������� **/
		}
		listcnt++;/****�����嵥��ϸ������1****/
	}
	/****�����һ���տ��¼������****/
	lv_batch_end(&pkg006);
	/*	EXEC SQL CLOSE DQJJ1;*/
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		WRITEMSG
			return(-1);
	}
	/** ��ͷ���� **/
	vtcp_log("[%s][%d]lv_wbctl.pack_date=[%ld]",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]lvca.lv_date=[%s]",__FILE__,__LINE__,lvca.lv_date);
	lv_pkg_batch(&pkg006,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg006,"011",wd_lv_wbctl.pay_qs_no);	/** �������������к� **/
	lv_pkg_batch(&pkg006,"012",wd_lv_wbctl.cash_qs_no);/** �������������к� **/
	lv_pkg_batch(&pkg006,"30E",packday);	/** ��ί������ **/
	/**
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	**/
	lv_pkg_batch(&pkg006,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	sprintf(cHzDate,"%02d",wd_lv_wbctl.hz_date);
	lv_pkg_batch(&pkg006,"BS6",cHzDate);	/** ��ִ���ޣ������� **/
	/****Deleted by SSH,2005.11.24,��Ѻ��Τ�Ѹ�****/
	/****lv_pkg_batch(&pkg006,"C15",wd_lv_wbctl.packmy);** ����Ѻ **/
	/*********/
	/****Modified by SSH,2005.11.24,����6λ�������Ǹ�����****/
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg006,"B63",tmpBuf);				/** ��ϸҵ���ܱ��� **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,����Է�Ϊ��λ,15λ***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg006,"32B",tmpBuf);			 /** ��ϸҵ���ܽ�� **/
	lv_gen_pack(&pkg006);
	/****ע��: �����ݿ���²����ŵ�����֮ǰ,�������ٲ�һ�µĿ���****/
	sql_stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' and pay_qs_no = '%s'" , sql_stat, packid, packday, PKGNO_DQJJ, wd_lv_wbctl.pay_qs_no);

	if (g_reply_int != DB_OK)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	g_reply_int= Lv_wbctl_Clo_Upd();
	if (g_reply_int )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int= lv_insSendList(&pkg006);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	vtcp_log("%s,%d,����\n",__FL__);
	iRc=iLvSendPackage(&pkg006);
	if (iRc !=DB_OK)
	{
		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg006);
	return(0);

}
/** ƴ�����˻㷢�Ͱ� **/
/** �������Ϊ��id **/
int lv_get_djth_tag(char *packday,char *packid)
{
	int     ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg007;
	PAY_IN_AREA pay_in;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	char    OTXNUM[6];
	char    OORBRNO[13];
	char    OACBRNO[13];
	char    OWTDAY[9];
	char    OORDERNO[9];
	double	OTXAMT;
	char    OCASHNO[13];
	char    OCASHACTNO[33];
	char    OCASHNAME[61];
	char    OCASHADDRESS[61];
	char    OPAYNO[13];
	char    OPAYACTNO[33];
	char    OPAYNAME[61];
	char    OPAYADDRESS[61];	
	char    RETCODE[3];
	char	CONTENT[61];
	char	YWTYPE[13];
	char    Stat[2];
	char    Stat2[2]; 

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(OTXNUM,  '\0', sizeof(OTXNUM));
	memset(OORBRNO,  '\0', sizeof(OORBRNO));
	memset(OACBRNO,  '\0', sizeof(OACBRNO));
	memset(OWTDAY,  '\0', sizeof(OWTDAY));
	memset(OORDERNO,  '\0', sizeof(OORDERNO));
	memset(OCASHNO,  '\0', sizeof(OCASHNO));
	memset(OCASHACTNO,  '\0', sizeof(OCASHACTNO));
	memset(OCASHNAME,  '\0', sizeof(OCASHNAME));
	memset(OCASHADDRESS,  '\0', sizeof(OCASHADDRESS));
	memset(OPAYNO,    '\0', sizeof(OPAYNO));
	memset(OPAYACTNO, '\0', sizeof(OPAYACTNO));
	memset(OPAYNAME,  '\0', sizeof(OPAYNAME));
	memset(OPAYADDRESS,'\0', sizeof(OPAYADDRESS));
	memset(RETCODE,'\0', sizeof(RETCODE));
	memset(CONTENT,	 '\0', sizeof(CONTENT));
	memset(YWTYPE,	 '\0', sizeof(YWTYPE));
	memset(Stat,   '\0', sizeof(Stat));
	memset(Stat2,   '\0', sizeof(Stat2));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	lv_load_tagifno();
	lv_create(&pkg007);

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld  and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if ( g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		WRITEMSG
			lv_destroy(&pkg007);
		return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
			lv_destroy(&pkg007);
		return(-1);
	}
	wd_dtlcnt =wd_lv_wbctl.dtlcnt;

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg007.opeType = '1';   /** С�� **/
	pkg007.pkgType = 1;       /** ��pkg�� **/
	memcpy(pkg007.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg007.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg007.CMTCode,PKGNO_DJTH,3); /** ��Ϊ��PKG007��**/
	memcpy(pkg007.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	Stat[0] = STAT_WZCHECK2;
	vtcp_log("[%s][%d]========packday=[%s],packid=[%s],stat=[%s]\n",__FL__,packday,packid,Stat);
	/*==========================================================================================
	EXEC SQL DECLARE DJTH1 CURSOR FOR
	select txnum,oacbrno,oorbrno,wtday,orderno,otxnum,oorbrno,oacbrno,owtday,oorderno,otxamt,ocashno,
	ocashactno,ocashname,ocashaddress,opayno,opayactno,opayname,opayaddress,
	retcode,content from lv_djths where packid =:packid  and packday =:packday  and sts =:Stat;

	EXEC SQL OPEN DJTH1;
	==========================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DJTH, packid, packday, Stat);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		/*========================================
		EXEC SQL FETCH DJTH1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,
		:OTXNUM,:OORBRNO,:OACBRNO,:OWTDAY,:OORDERNO,:OTXAMT,:OCASHNO,
		:OCASHACTNO,:OCASHNAME,:OCASHADDRESS,:OPAYNO,:OPAYACTNO,:OPAYNAME,
		:OPAYADDRESS,:RETCODE,:CONTENT;
		*========================================*/
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");

			lv_destroy(&pkg007);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"���ݿ����");

			lv_destroy(&pkg007);
			WRITEMSG
				return(-1);
		}
		/** Ҫ�ؼ����� **/

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		memcpy(pkg007.CMTNo,"002",3);       /***Ҫ�ؼ���002***/
		pkg007.paymPRI='0';                 /***���ȼ�***/
		lv_set_tag(&pkg007,"0BG",lv_pkgreg.txnum);	/** ҵ�����ͺ� **/
		lv_set_tag(&pkg007,"52A",lv_pkgreg.or_br_no);	/** �������к� **/
		lv_set_tag(&pkg007,"58A",lv_pkgreg.ac_br_no);	/** �������к� **/
		lv_set_tag(&pkg007,"30A",lv_date(lv_pkgreg.wt_date));	/**	ί������ **/
		lv_set_tag(&pkg007,"0BC",lv_pkgreg.orderno);	/**	֧��������� **/
		lv_set_tag(&pkg007,"0BH",lv_pkgreg.o_txnum);	/**	ԭ�������ͱ�� **/
		lv_set_tag(&pkg007,"CC1",lv_pkgreg.o_or_br_no);	/**	ԭ�������к� **/
		lv_set_tag(&pkg007,"CC2",lv_pkgreg.or_br_no);	/**	ԭ�������к� **/
		lv_set_tag(&pkg007,"051",lv_date(lv_pkgreg.o_wt_date));	/**	ԭί������ **/
		lv_set_tag(&pkg007,"005",lv_pkgreg.o_orderno);/**	ԭ֧��������� **/
		vtcp_log("[%s][%d]====OTXAMT=[%.2f]\n",__FL__,OTXAMT);
		OTXAMT = lv_pkgreg.tx_amt*100;
		sprintf(tmpBuf,"%015.0f",OTXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg007,"33S",tmpBuf);	/**	ԭ��� **/
		lv_set_tag(&pkg007,"CCK",lv_pkgreg.pay_opn_br_no);/**	ԭ�տ��˿������к� **/
		lv_set_tag(&pkg007,"CQ2",lv_pkgreg.pay_ac_no);	/**	ԭ�տ����˺� **/
		lv_set_tag(&pkg007,"CR2",lv_pkgreg.pay_name);	/**	ԭ�տ������� **/
		lv_set_tag(&pkg007,"59F", lv_pkgreg.pay_addr);	/**	ԭ�տ��˵�ַ **/
		lv_set_tag(&pkg007,"CCL", lv_pkgreg.cash_opn_br_no);	/**	ԭ�����˿������к� **/
		lv_set_tag(&pkg007,"CQ1",lv_pkgreg.cash_ac_no);	/**	ԭ�������˺� **/
		lv_set_tag(&pkg007,"CR1",lv_pkgreg.cash_name);	/**	ԭ���������� **/
		lv_set_tag(&pkg007,"50F",lv_pkgreg.cash_addr);	/**	ԭ�����˵�ַ **/
		lv_set_tag(&pkg007,"CIA",lv_pkgreg.retcode);	/**�˻�ԭ��**/
		lv_set_tag(&pkg007,"72A",lv_pkgreg.lv_brf);	/**	���� **/
		/** ƴ�������� **/
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ORDERNO, lv_pkgreg.orderno);

		if (memcmp(TXNUM,TXNUM_DJTH,sizeof(TXNUM)-1)==0) /** �����˻�ҵ�� **/
		{
			lv_set_tag(&pkg007,"B40","00000000");
		}
		/****************** �����˻�ֻ���������ͣ���û�и���������
		else
		{
		memset(tmpBuf,'\0',sizeof(tmpBuf));
		memcpy(pay_in.F_wtday, WTDAY,sizeof(pay_in.F_wtday));
		memcpy(pay_in.F_orbrno,ORBRNO,sizeof(pay_in.F_orbrno));
		memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
		iLvGetAddit(&pay_in,"000",tmpBuf,0);
		if(tmpBuf[0]=='\0'){
		****û�и�������****
		lv_set_tag(&pkg007,"B40","00000000");   ** �������ݳ���**
		}
		else
		{
		****�и�������****
		char len_buf[9];
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		lv_set_tag(&pkg007,"B40","0");** �������ݳ��� ,����ҵ���޸�ʽ**
		lv_set_tag(&pkg007,"72C","");       ** �������ݣ�����ҵ�������� **
		}
		}
		***************/
		lv_batch_end(&pkg007); 

	}	
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE DJTH1;*/

	/** ��ͷ���� **/
	lv_pkg_batch(&pkg007,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg007,"011",Payqsactno);			/** �������������к� **/
	lv_pkg_batch(&pkg007,"012",Cashqsactno);		/** �������������к� **/
	lv_pkg_batch(&pkg007,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg007,"0BD",wd_lv_wbctl.packid); /** ����� **/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg007,"B63",Cnt);                /** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg007,"32B",tmpBuf);             /** ��ϸҵ���ܽ�� **/
	vtcp_log("[%s][%d]---------Cnt=[%s]tmpBuf=[%s]\n",__FL__,Cnt,tmpBuf);

	lv_gen_pack(&pkg007);
	/**
	dm_show(&(pkg007.dm));
	**/
	/** ����״̬Ϊ�ѷ��� **/
	Stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s'  " , Stat, packid, packday, PKGNO_DJTH);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ����");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_dat=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	if ((g_reply_int = lv_insSendList(&pkg007)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&pkg007))
	{
		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg007);
	return(0);
}
/** pkg008 **/
/** ��ͨ��ǻ�ִҵ���Ͱ� **/
/** �������Ϊ��id **/
int lv_get_ptjjhz_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg008;
	static struct lv_wbctl_c      wd_lv_wbctl;
	/****HSYX BEGIN:���ӱ�������,ƴ֧Ʊ��ִ��****/
	PAY_IN_AREA pay_in;
	char    PAYNO[13];              /**�����˿�����***/
	char    PAYACTNO[33];   /**�������ʺ�**/
	char    PAYNAME[61];    /**����������**/
	char    CASHNO[13];             /**�տ��˿�����**/
	char    CASHACTNO[33];  /**�տ����ʺ�**/
	char    CASHNAME[61];   /**�տ�������**/
	char    PACKDAY[9];             /**������**/
	char    PACKID[9];              /**�����**/
	char    OWTDAY[9];
	/****HSYX END:���ӱ�������,ƴ֧Ʊ��ִ��****/
	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO,  '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	/****HSYX BEGIN:��ʼ������****/
	memset(OWTDAY, 0 , sizeof(OWTDAY));
	memset(PAYNO,'\0',sizeof(PAYNO));
	memset(PAYACTNO,'\0',sizeof(PAYACTNO));
	memset(PAYNAME,'\0',sizeof(PAYNAME));
	memset(CASHNO,'\0',sizeof(CASHNO));
	memset(CASHACTNO,'\0',sizeof(CASHACTNO));
	memset(CASHNAME,'\0',sizeof(CASHNAME));
	memset(PACKDAY,'\0',sizeof(PACKDAY));
	memset(PACKID,'\0',sizeof(PACKID));
	/****HSYX END:��ʼ������****/

	lv_load_tagifno();
	lv_create(&pkg008);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno,  wd_lv_wbctl.pay_qs_no,  sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg008.opeType = '1';	/** С�� **/
	pkg008.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg008.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg008.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg008.CMTCode,PKGNO_PTJJHZ,3);	/** ��Ϊ��PKG008��**/
	memcpy(pkg008.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	/***Modified by SSH,2006.1.9,�����ִ,��ִ���׼�¼�б�����ǾͰ���Ϣ****/
	/****���������Ʊ��opackday��opackid�б����˾���Ϣ���Զ�Ӧ****/
	/*====================================================================
	Stat[0]=STAT_WZCHECK2;
	EXEC SQL DECLARE PTJJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday, content
	FROM  lv_ptjjs
	where payqsactno=:wd_lv_wbctl.cash_qs_no 
	and packid =:wd_lv_wbctl.o_packid 
	and packday =:wd_lv_wbctl.o_pack_date 
	and pkgno=:Pkgno and stat <>'0' ;
	EXEC SQL OPEN PTJJHZ;
	*=====================================================================*/

	/***HSYX BEGIN:���wbctl��opackidΪ99999999��ʾΪ֧Ʊҵ��,����Ϊ��ͨ���ҵ��**/
	if (memcmp(wd_lv_wbctl.o_packid,"99999999",8)==0)
	{
		apitoa(wd_lv_wbctl.pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.packid,sizeof(PACKID)-1);
		vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);
	}else
	{
		/* rem by LiuHuafeng 20070402 NEWYX
		apitoa(wd_lv_wbctl.o_pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.o_packid,sizeof(PACKID)-1);
		*********replace by next tow line *************/
		apitoa(wd_lv_wbctl.pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.packid,sizeof(PACKID)-1);
		vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);
	}
	/****HSYX BEGIN:�Ӱ�����****/
	vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",\
		__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);

	vtcp_log("[%s][%d]packid=[%s]",__FILE__,__LINE__,packid);
	vtcp_log("[%s][%d]packday=[%s]",__FILE__,__LINE__,packday);
	vtcp_log("[%s][%d]wd_lv_wbctl.pay_qs_no=[%s]",__FILE__,__LINE__,wd_lv_wbctl.pay_qs_no);
	/*** dongxy �޸�һ�� 20070320
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, packid, packday, wd_lv_wbctl.pay_qs_no);
	*****************************/
	g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, PACKID, PACKDAY, wd_lv_wbctl.pay_qs_no);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/***HSYX END:���wbctl��opackidΪ�ձ�ʾΪ֧Ʊҵ��,����Ϊ��ͨ���ҵ��**/
	/********** HSYX del begin
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, packid, packday, wd_lv_wbctl.pay_qs_no);
	if (g_reply_int != DB_OK)
	{
	sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
	WRITEMSG
	return(-1);
	}
	HSYX del end*************/

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{ 
		/*EXEC SQL FETCH PTJJHZ INTO :WTDAY,:ORDERNO,NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
		:JZDAY,:CONTENT;*/
		/** dongxy �޸�һ�� 20070320
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		**/
		g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("[%s][%d]cashqsactno=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);
			vtcp_log("[%s][%d]opackid[%s]\n",	__FL__,wd_lv_wbctl.o_packid); 
			vtcp_log("[%s][%d]opackday=[%d]\n",	__FL__,wd_lv_wbctl.o_pack_date );
			vtcp_log("[%s][%d]pkgno=[%s]\n",__FL__,Pkgno);
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"���ݿ����!");
			lv_destroy(&pkg008);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");
			lv_destroy(&pkg008);
			WRITEMSG
				return(-1);
		}
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(INDAY, lv_date(lv_pkgreg.in_date));
		TXAMT = lv_pkgreg.tx_amt*100;
		strcpy(RCPSTAT, lv_pkgreg.rcpstat);
		strcpy(JZDAY, lv_date(lv_pkgreg.jz_date));
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		/****HSYX BEGIN :��ѡ�˼����ֶ�****/
		strncpy(OWTDAY, lv_date(lv_pkgreg.o_wt_date),8);
		strcpy(PAYNO, lv_pkgreg.pay_opn_br_no);
		strcpy(PAYACTNO, lv_pkgreg.pay_ac_no);
		strcpy(PAYNAME, lv_pkgreg.pay_name);
		strcpy(CASHNO, lv_pkgreg.cash_opn_br_no);
		strcpy(CASHACTNO, lv_pkgreg.cash_ac_no);
		strcpy(CASHNAME, lv_pkgreg.cash_name);
		/****HSYX END :��ѡ�˼����ֶ�****/

		/** NORDERNO������ţ�ORDERNO ��ԭ������� **/
		strcpy(NORDERNO, lv_pkgreg.orderno);
		strcpy(ORDERNO, lv_pkgreg.o_orderno);
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
		vtcp_log("[%s][%d]cashqsactno=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);
		vtcp_log("[%s][%d]opackid[%s]\n",	__FL__,wd_lv_wbctl.o_packid); 
		vtcp_log("[%s][%d]opackday=[%d]\n",	__FL__,wd_lv_wbctl.o_pack_date );
		vtcp_log("[%s][%d]pkgno=[%s]\n",__FL__,Pkgno);
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ����!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
		}
		/******************/
		/** Ҫ�ؼ����� **/
		/****HSYX BEGIN:ƴ֧Ʊ��ִ��****/
		if(memcmp(wd_lv_wbctl.o_packid,"99999999",9)==0){
			ZPJL1_ADD_AREA  wd_zp_add1;
			memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
			memcpy(pkg008.CMTNo,"007",3);       /***007��Ʊ����ִ***/
			vtcp_log("[%s][%d]===============\n",__FILE__,__LINE__);
			/*****delete by xyy 2007-4-23 16:22 NEWYX3
			if (memcmp(TXNUM,"30102",5)==0)
			{
				lv_set_tag(&pkg008,"0BG","30104");    ** ֧Ʊҵ���ִ **
			}else
			{
				lv_set_tag(&pkg008,"0BG","30105");    ** ͨ��ҵ���ִ **
			}
			*****************replace by next line**************************/
			lv_set_tag(&pkg008,"0BG",TXNUM);
			
			lv_set_tag(&pkg008,"30A",INDAY);    /** ί������ **/
			/* rem by LiuHuafeng 20070402
			lv_set_tag(&pkg008,"051",WTDAY);
			** replace by next line ******/
			lv_set_tag(&pkg008,"051",OWTDAY);    /** ԭί������ **/
			lv_set_tag(&pkg008,"0BC",NORDERNO);     /**     ֧��������� **/
			lv_set_tag(&pkg008,"005",ORDERNO);  /** ԭ֧��������� **/
			memcpy(pay_in.F_wtday, WTDAY,sizeof(pay_in.F_wtday));
			memcpy(pay_in.F_orbrno,ACBRNO,sizeof(pay_in.F_orbrno));
			/** ӦΪ�������¸�ֵ�ˣ�ORDERNO��ԭ������� dongxy
			memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
			**/
			/****NEWYX2:��������ô�����������****/
			memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
			memcpy(pay_in.F_pkgno,PKGNO_PTJJ,sizeof(pay_in.F_pkgno));
			memcpy(pay_in.F_opcd,OPCD_RECEIVE,sizeof(pay_in.F_opcd));
			memcpy(pay_in.F_wtday,OWTDAY    ,sizeof(pay_in.F_wtday));/***add by xyy 2007-4-15 16:57 NEWYX2***/
			/** ���溯�������������ˣ����Ը�ֵ dongxy **/
			pay_in.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
			iRc=iLvFetchRecToPayin(&pay_in);
			if (iRc)
			{
				vtcp_log("%s,%d,���ݿ����iRc=%d\n",__FILE__,__LINE__,iRc);
				sprintf( acErrMsg,"���ݿ����!");
				lv_destroy(&pkg008);
				WRITEMSG
					return(-1);
			}
			if (memcmp(TXNUM,TXNUM_ZPJLHZ,strlen(TXNUM_ZPJLHZ))==0)/**modify by xyy 2007-4-23 16:23 NEWYX3**/
			{
				ZPJL1_ADD_AREA  wd_zp_add1;
				char pcTmpBuf[32];
				memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
				memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
				iRc=iLvGetAddit(&pay_in,"001",&wd_zp_add1,0);
				memcpy(pcTmpBuf,wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno));
				if (pcTmpBuf[0]=='\0'||pcTmpBuf[0]==' ')
				{
					memset(pcTmpBuf,'0',12);
					pcTmpBuf[12]='\0';
				}else
				{
					sprintf(pcTmpBuf,"%012d",apatoi(wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno)));
				}
				lv_set_tag(&pkg008,"CSH",pcTmpBuf);  /** ֧Ʊ���� **/
			}else if(memcmp(TXNUM,TXNUM_TYJLHZ,strlen(TXNUM_TYJLHZ))==0) /**modify by xyy 2007-4-23 16:23 NEWYX3**/
			{
				TYJL1_ADD_AREA  wd_ty_add1;
				char pcTmpBuf[32];
				memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
				memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
				iRc=iLvGetAddit(&pay_in,"001",&wd_ty_add1,0);
				memcpy(pcTmpBuf,wd_ty_add1.cpno,sizeof(wd_ty_add1.cpno));
				if (pcTmpBuf[0]=='\0'||pcTmpBuf[0]==' ')
				{
					memset(pcTmpBuf,'0',12);
					pcTmpBuf[12]='\0';
				}else
				{
					sprintf(pcTmpBuf,"%012d",apatoi(wd_ty_add1.cpno,sizeof(wd_ty_add1.cpno)));
				}
				lv_set_tag(&pkg008,"CSH",pcTmpBuf);  /** ֧Ʊ����**/
			}
			lv_set_tag(&pkg008,"CC4",PAYNO);    /** �����˿����� **/
			lv_set_tag(&pkg008,"50C",PAYACTNO);    /** �������ʺ� **/
			lv_set_tag(&pkg008,"50A",PAYNAME);    /** ���������� **/
			lv_set_tag(&pkg008,"CC5",CASHNO);    /** �տ��˿�����*/
			lv_set_tag(&pkg008,"59C",CASHACTNO);    /** �տ��ʺ�*/
			lv_set_tag(&pkg008,"59A",CASHNAME);    /** �տ�������*/
			lv_set_tag(&pkg008,"52A",ORBRNO);    /** ������*/
			lv_set_tag(&pkg008,"58A",ACBRNO);    /** ������*/
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			sprintf(tmpBuf,"%015.0f",TXAMT);
			printf("[%s][%d]====tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			lv_set_tag(&pkg008,"33S",tmpBuf);       /** ��� **/
			if (memcmp(RCPSTAT,"00",2)==0)
			{
				lv_set_tag(&pkg008,"CIA","00");         /** ҵ���ִ״̬ **/
				lv_set_tag(&pkg008,"CCA","          ");         /** ҵ���ִ״̬ **/
			}
			else
			{
				char pcReason[11];
				int  iReasonLen;
				memset(pcReason,' ',10);
				pcReason[10]='\0';
				lv_set_tag(&pkg008,"CIA","09");         /** ҵ���ִ״̬ **/
				/***Add by SSH,2006.11.14,֧�ֶ����Ʊ����**/
				memset(pcReason,'\0',sizeof(pcReason));
				iRc=iLvGetAddit(&pay_in,"TPL",pcReason,0);
				/****HSYX BEGIN:�°�3.1.1�����5������!****/
				/***������***/
				vtcp_log("[%s][%d]--------pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
				/**begin: add by wangyongwei 070410***/
			  if(memcmp(pcReason,"00",2)==0)
			  {
			  	memset(pcReason,0,sizeof(pcReason));
			  	pub_base_full_space(pcReason,10);
			  }
			  else
			  {
          zip_space(pcReason);
          iReasonLen=strlen(pcReason);
			  	if(iReasonLen<10)
			  	{
			      vtcp_log("[%s][%d]--------pcReason=[%s]*******!!!!!!!\n",__FILE__,__LINE__,pcReason);
			  		char tmpReason[11];
			  		memset(tmpReason,0,sizeof(tmpReason));
			  		memcpy(tmpReason,pcReason,iReasonLen);
			  		memset(pcReason,' ',sizeof(pcReason)-1);
			  		memcpy(pcReason+10-iReasonLen,tmpReason,iReasonLen);		
			      vtcp_log("[%s][%d]--------pcReason=[%s]*******!!!!!!!\n",__FILE__,__LINE__,pcReason);
				  }
			  }
		    /**end: add by wangyongwei 070410***/
				/***
				memcpy(pcReason,"2122232425",10);***/
				/****HSYX END:�°�3.1.1�����5������****/
				vtcp_log("[%s][%d]--------pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
				lv_set_tag(&pkg008,"CCA",pcReason);     /** ҵ���ִ״̬ **/
			}
			lv_set_tag(&pkg008,"72A",CONTENT);      /** ���� **/
			/****HSYX END:ƴ֧Ʊ��ִҪ�ؼ�****/
		}
		else
		{
			memcpy(pkg008.CMTNo,"006",3);       /***Ҫ�ؼ���006***/
			pkg008.paymPRI='0';                 /***���ȼ�***/
			lv_set_tag(&pkg008,"30A",INDAY);    /** ί������ **/
			lv_set_tag(&pkg008,"BSE",INDAY);    /** �ۿ����� **/

			vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);		
			lv_set_tag(&pkg008,"0BC",NORDERNO);		/**֧��������� **/			

			lv_set_tag(&pkg008,"0BH",TXNUM);	/**	ԭҵ�����ͺ� **/	
			/****NEWYX2:����ͽ����кŷ���****/
			lv_set_tag(&pkg008,"CC1",ACBRNO);   /** ԭ�������к� **/
			lv_set_tag(&pkg008,"CC2",ORBRNO);   /** ԭ�������к� **/
			lv_set_tag(&pkg008,"051",OWTDAY);   /**NEWBP ԭί������ **/
			lv_set_tag(&pkg008,"005",ORDERNO);  /** ԭ֧��������� **/

			vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			sprintf(tmpBuf,"%015.0f",TXAMT);
			vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
			lv_set_tag(&pkg008,"33S",tmpBuf);   	/** ��� **/
			lv_set_tag(&pkg008,"CIA",RCPSTAT);   	/** ҵ���ִ״̬ **/
			/** lv_set_tag(&pkg008,"BSN","");   		������������ **/
			lv_set_tag(&pkg008,"72A",CONTENT);  	/** ���� **/
		}
		if (memcmp(RCPSTAT,"00",2)==0)
		{
			/****ͬ��Ӧ��****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			lv_pkgreg.lv_sts[0] = STAT_YES_SEND;/* ��Ҫ���ʶ����� by LiuHuafeng 2007-4-6 22:51*/
			/** dongxy �޸�һ�� 20070320
			pcStat[0]=STAT_YES_SEND;
			g_reply_int = sql_execute( "update lv_pkgreg set lv_sts='%s' where wt_date='%s' and pkgno = '%s' and lw_ind = '2' and or_br_no='%s' and orderno='%s' ", pcStat ,WTDAY, PKGNO_PTJJ,  ORBRNO, ORDERNO);
			*******************************/
			g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
			if (g_reply_int)
			{
				vtcp_log("%s,%d,fetch lv_wbctl��[%d]\n",__FILE__,__LINE__,g_reply_int);
				strcpy( g_pub_tx.reply,"P058" );
				WRITEMSG
					return(-1);
			}
		}
		else
		{
			/****��ͬ��Ӧ��****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			/** dongxy �޸�һ�� 20070320
			pcStat[0]=STAT_NO_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%s' where pkgno = '%s' and lw_ind = '2' and wt_date='%s' and or_br_no='%s' and orderno='%s'", pcStat, PKGNO_PTJJ, WTDAY, ORBRNO, ORDERNO);
			**************************************/
			lv_pkgreg.lv_sts[0] = STAT_NO_SEND;
			g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
			if (g_reply_int)
			{
				vtcp_log("%s,%d,fetch lv_wbctl��[%d]\n",__FILE__,__LINE__,g_reply_int);
				strcpy( g_pub_tx.reply,"P058" );
				WRITEMSG
					return(-1);
			}
		}
		lv_batch_end(&pkg008);
	}
	/*EXEC SQL CLOSE PTJJHZ;*/
	/** dongxy �޸�һ�� 20070320
	g_reply_int = Lv_pkgreg_Clo_Sel();
	******************************/
	g_reply_int = Lv_pkgreg_Clo_Upd();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/** ��ͷ���� **/
	lv_pkg_batch(&pkg008,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg008,"011",Payqsactno);			/** �������������к� **/
	lv_pkg_batch(&pkg008,"012",Cashqsactno);		/** �������������к� **/
	lv_pkg_batch(&pkg008,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg008,"0BD",wd_lv_wbctl.packid);		/** ����� **/
	/** lv_pkg_batch(&pkg008,"C15","");	����Ѻ **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	vtcp_log("[%s][%d]dtlcnt===[%s]",__FILE__,__LINE__,Cnt);
	lv_pkg_batch(&pkg008,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	vtcp_log("[%s][%d]totamt===[%f]",__FILE__,__LINE__,wd_lv_wbctl.totamt);
	lv_pkg_batch(&pkg008,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/**
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	**/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	vtcp_log("[%s][%d]====Cnt=[%s]\n",__FL__,Cnt);
	lv_pkg_batch(&pkg008,"B41",Cnt);	/** ��ϸҵ��ɹ��ܱ��� **/
	vtcp_log("[%s][%d]dtlsuccnt===[%s]",__FILE__,__LINE__,Cnt);
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	vtcp_log("[%s][%d]dtlsucamt===[%f]",__FILE__,__LINE__,wd_lv_wbctl.dtlsucamt);
	lv_pkg_batch(&pkg008,"32C",tmpBuf);	/** ��ϸҵ��ɹ��ܽ�� **/
	lv_pkg_batch(&pkg008,"CCO",wd_lv_wbctl.cash_qs_no);/** ԭ������������ **/

	/** ����Ҫ��02D�ֶ���"999", ���Լ�һ������ dongxy 20070320 **/
	vtcp_log("[%s][%d]---------txnum=[%s]\n",__FILE__,__LINE__,TXNUM);
	/****NEWYX2:��ϵͳ�и�׼ȷ�����򵥵��жϷ���****/
	if (memcmp(wd_lv_wbctl.o_packid,"99999999",8)==0){
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
		lv_pkg_batch(&pkg008,"02D","999");		/** ԭ�����ͺ� **/
		lv_pkg_batch(&pkg008,"30I",pcLvGetDay_work());	/** ԭ��ί������ Ӱ��Ҫ����С������**/
		vtcp_log("[%s][%d]##################���ﰡ!\n",__FILE__,__LINE__);
	}
	else
	{
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		lv_pkg_batch(&pkg008,"02D",wd_lv_wbctl.o_pkgno);		/** ԭ�����ͺ� **/
		lv_pkg_batch(&pkg008,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** ԭ��ί������ **/
	}
	vtcp_log("%s,%d opackid===[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.o_packid);
	lv_pkg_batch(&pkg008,"0BE",wd_lv_wbctl.o_packid);	/** ԭ����� **/
	lv_pkg_batch(&pkg008,"72D","");		/**���������� ���� **/
	lv_gen_pack(&pkg008);

	Stat2[0] = STAT_WZSEND;

	vtcp_log("[%s][%d]����lv_pkgreg��ʱ����packid=[%s]",__FILE__,__LINE__,wd_lv_wbctl.packid);
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_PTJJHZ);
	vtcp_log("[%s][%d]������lv_pkgreg��״̬lv_sts=[%c]",__FILE__,__LINE__,lv_pkgreg.lv_sts[0]);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ����!");
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg008)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&pkg008))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg008);

	return(0);
}
/** pkg009 **/
/** ʵʱ���ǻ�ִҵ���Ͱ� **/
/** �������Ϊ��id **/
int lv_get_ssdjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg009;
	PAY_IN_AREA pay_in;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char  cOwtday[9];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double	TXAMT;
	double	FEEAMT2;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(cOwtday, 0 , sizeof(cOwtday));
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO,  '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg009);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]===============\n",__FL__);
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]====\n",__FL__);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld  and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg009.opeType = '1';	/** С�� **/
	pkg009.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg009.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg009.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg009.CMTCode,PKGNO_SSDJHZ,3);	/** ��Ϊ��PKG009��**/
	memcpy(pkg009.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	Stat[0]=STAT_WZLR;
	/*======================================================================
	EXEC SQL DECLARE SSDJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday,feeamt2, content
	FROM  lv_ssdjs where payqsactno=:wd_lv_wbctl.cash_qs_no and  packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and pkgno=:Pkgno and stat <>'0' ;

	EXEC SQL OPEN SSDJHZ;

	EXEC SQL FETCH SSDJHZ INTO :WTDAY,:ORDERNO,:NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
	:JZDAY,:FEEAMT2,:CONTENT;
	======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1' and lv_sts <> '0' and cash_qs_no = '%s'", PKGNO_SSDJHZ, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.cash_qs_no);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int != DB_OK && g_reply_int != DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	apitoa(lv_pkgreg.in_date,8,INDAY);
	apitoa(lv_pkgreg.wt_date,8,WTDAY);	
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(cOwtday, lv_date(lv_pkgreg.o_wt_date));
	strcpy(ORDERNO, lv_pkgreg.orderno);
	TXAMT = lv_pkgreg.tx_amt*100.0;
	strcpy(RCPSTAT, lv_pkgreg.rcpstat);
	strcpy(CONTENT, lv_pkgreg.lv_brf);
	FEEAMT2 = lv_pkgreg.df_chrg_amt;

	/* Begin:  Added by chenming   2006-10-11 20:11 Ӧ��������һ��֧��������� */
	vtcp_log("%s,%d, ======ORBRNO=[%s] ",__FILE__,__LINE__,ORBRNO);				
	long lOrderno = iLvGetOrderno();
	vtcp_log("%s,%d, ======lOrderno=[%ld] ",__FILE__,__LINE__,lOrderno);		  
	apitoa(lOrderno, sizeof(NORDERNO)-1, NORDERNO);
	vtcp_log("%s,%d, ======NORDERNO=[%s] ",__FILE__,__LINE__,NORDERNO);	  
	/* End:  Added by chenming   2006-10-11 20:11 */   		

	/** Ҫ�ؼ����� **/
	memcpy(pkg009.CMTNo,"006",3);       /***Ҫ�ؼ���006***/
	pkg009.paymPRI='0';                 /***���ȼ�***/
	lv_set_tag(&pkg009,"30A",INDAY);    /** ί������ **/

	vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
	lv_set_tag(&pkg009,"0BC",NORDERNO);	/**	֧��������� **/	

	lv_set_tag(&pkg009,"0BH",TXNUM);	/**	ԭҵ�����ͺ� **/	
	lv_set_tag(&pkg009,"CC1",lv_pkgreg.o_or_br_no);   /** ԭ�������к� **/
	lv_set_tag(&pkg009,"CC2",lv_pkgreg.o_ac_br_no);   /** ԭ�������к� **/
	lv_set_tag(&pkg009,"051",cOwtday);    /** ԭί������ **/
	lv_set_tag(&pkg009,"005",lv_pkgreg.o_orderno); 		/***ԭ֧��������� **/

	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg009,"33S",tmpBuf);   	/** ��� **/

	lv_set_tag(&pkg009,"CIA",RCPSTAT);   	/** ҵ���ִ״̬ **/
	lv_set_tag(&pkg009,"BSE",INDAY);   	/** �ۿ����� **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",FEEAMT2);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg009,"BSN",tmpBuf);   		/** ������������ **/

	lv_set_tag(&pkg009,"72A",CONTENT);  	/** ���� **/

	lv_batch_end(&pkg009);

	/*EXEC SQL CLOSE SSDJHZ;*/
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** ��ͷ���� **/
	lv_pkg_batch(&pkg009,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg009,"011",Payqsactno);			/** �������������к� **/
	lv_pkg_batch(&pkg009,"012",Cashqsactno);		/** �������������к� **/
	lv_pkg_batch(&pkg009,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg009,"0BD",wd_lv_wbctl.packid);		/** ����� **/
	/** lv_pkg_batch(&pkg009,"C15","");	����Ѻ **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg009,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg009,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/***
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	***/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	lv_pkg_batch(&pkg009,"B41",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	/****NEWYX2:�ɹ������㲻��ȷ****/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	lv_pkg_batch(&pkg009,"32C",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	lv_pkg_batch(&pkg009,"02D",wd_lv_wbctl.o_pkgno);		/** ԭ�����ͺ� **/
	lv_pkg_batch(&pkg009,"CCO",wd_lv_wbctl.cash_qs_no);/** ԭ������������ **/
	lv_pkg_batch(&pkg009,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** ԭ��ί������ **/
	lv_pkg_batch(&pkg009,"0BE",wd_lv_wbctl.o_packid);	/** ԭ����� **/
	lv_pkg_batch(&pkg009,"72D","");		/** ���������� ���� **/
	lv_gen_pack(&pkg009);
	/**
	dm_show(&(pkg009.dm));
	**/
	Stat2[0] = STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_SSDJHZ);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"û�з�������!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg009)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]����\n",__FL__);
	if (iRc=iLvSendPackage(&pkg009))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg009);

	return(0);
}
/** pkg010 **/
/** ʵʱ��ǻ�ִҵ���Ͱ� **/
/** �������Ϊ��id **/
int lv_get_ssjjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg010;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	OWTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	double		FEEAMT2;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO, '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));
	memset(OWTDAY, 0 , sizeof(OWTDAY));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg010);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]===============ִ�е�������!!!======= ",__FILE__,__LINE__);
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d wd_lv_wbctl.o_packid ===[%.8s] ",__FL__,wd_lv_wbctl.o_packid);	
	vtcp_log("%s,%d wd_lv_wbctl.o_pack_date ===[%d] ",__FL__,wd_lv_wbctl.o_pack_date);					
	vtcp_log("%s,%d wd_lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,wd_lv_wbctl.cash_qs_no);							

	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg010.opeType = '1';	/** С�� **/
	pkg010.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg010.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg010.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg010.CMTCode,PKGNO_SSJJHZ,3);	/** ��Ϊ��PKG010��**/
	memcpy(pkg010.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	Stat[0]=STAT_WZCHECK2;
	vtcp_log("[%s][%d]========Stat=[%s]\n",__FL__,Stat);
	vtcp_log("[%s][%d]========opackid=[%s]\n",__FL__,wd_lv_wbctl.o_packid);
	vtcp_log("[%s][%d]========opackday=[%d]\n",__FL__,wd_lv_wbctl.o_pack_date);
	vtcp_log("[%s][%d]========cash_qs_no=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);	
	/*======================================================================
	EXEC SQL DECLARE SSJJHZ CURSOR FOR 
	SELECT  wtday, orderno, norderno, txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday,feeamt2, content
	FROM  lv_ssjjs where payqsactno=:wd_lv_wbctl.cash_qs_no and packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and pkgno=:Pkgno and stat <>'0';

	EXEC SQL OPEN SSJJHZ;

	EXEC SQL FETCH SSJJHZ INTO :WTDAY,:ORDERNO,:NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
	:JZDAY,:FEEAMT2,:CONTENT;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1' and lv_sts <> '0' and cash_qs_no = '%s'", PKGNO_SSJJHZ, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.cash_qs_no);
	if ( g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ���� sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,δ�ҵ���Ӧ�����ݿ��¼ sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"δ�ҵ���Ӧ�����ݿ��¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d or_br_no=[%s] ac_br_no=[%s] o_wt_date=[%d]",__FILE__,__LINE__,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.o_wt_date);
	vtcp_log("%s,%d o_or_br_no=[%s] o_ac_br_no=[%s] o_wt_date=[%d]",__FILE__,__LINE__,lv_pkgreg.o_or_br_no,lv_pkgreg.o_ac_br_no,lv_pkgreg.o_wt_date);
	strcpy(INDAY, lv_date(lv_pkgreg.in_date));
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
	strncpy(OWTDAY,lv_date(lv_pkgreg.o_wt_date),8);
	strcpy(ORDERNO, lv_pkgreg.orderno);
	TXAMT = lv_pkgreg.tx_amt*100.0;
	strcpy(RCPSTAT, lv_pkgreg.rcpstat);
	strcpy(CONTENT, lv_pkgreg.lv_brf);
	FEEAMT2 = lv_pkgreg.df_chrg_amt;
	/****NEWYX2:����ʱ�в����²������****/

	/** Ҫ�ؼ����� **/
	memcpy(pkg010.CMTNo,"006",3);       /***Ҫ�ؼ���006***/
	pkg010.paymPRI='0';                 /***���ȼ�***/
	lv_set_tag(&pkg010,"30A",INDAY);    /** ί������ **/
	lv_set_tag(&pkg010,"BSE",INDAY);   	/** �ۿ����� **/

	vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
	lv_set_tag(&pkg010,"0BC",lv_pkgreg.orderno);	/**	֧��������� **/	

	lv_set_tag(&pkg010,"0BH",TXNUM);	/**	ԭҵ�����ͺ� **/	

	/* rem by LiuHuafeng 2007-4-8 12:22 ���ڲ�ѯ������֯�Ļ�ִ��Ҳ�������ʣ�
		����ԭ�����кź�ԭ�����к�Ӧ���Ƿ������� by LiuHuafeng 2007-4-8 12:23
		ԭ�����к�--->���ν�����,ԭ�����к�-->���η�����
		����ֱ�������ݱ���ԭ�����ݾͿ�����
	lv_set_tag(&pkg010,"CC1",ORBRNO);   ** ԭ�������к� **
	lv_set_tag(&pkg010,"CC2",ACBRNO);   ** ԭ�������к� **
	lv_set_tag(&pkg010,"051",WTDAY);   ** ԭί������ **
	lv_set_tag(&pkg010,"005",ORDERNO); ** ԭ֧��������� **
	***************************************************************/
	lv_set_tag(&pkg010,"CC1",lv_pkgreg.o_or_br_no);   /** ԭ�������к� **/
	lv_set_tag(&pkg010,"CC2",lv_pkgreg.o_ac_br_no);   /** ԭ�������к� **/
	lv_set_tag(&pkg010,"051",OWTDAY);    /** ԭί������ **/
	lv_set_tag(&pkg010,"005",lv_pkgreg.o_orderno); /** ԭ֧��������� **/

	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg010,"33S",tmpBuf);   	/** ��� **/

	lv_set_tag(&pkg010,"CIA",RCPSTAT);   	/** ҵ���ִ״̬ **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",FEEAMT2);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg010,"BSN",tmpBuf);   		/** ������������ **/

	lv_set_tag(&pkg010,"72A",CONTENT);  	/** ���� **/

	lv_batch_end(&pkg010);

	/*	EXEC SQL CLOSE SSJJHZ;*/
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** ��ͷ���� **/
	lv_pkg_batch(&pkg010,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg010,"011",Payqsactno);			/** �������������к� **/
	lv_pkg_batch(&pkg010,"012",Cashqsactno);		/** �������������к� **/
	lv_pkg_batch(&pkg010,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg010,"0BD",wd_lv_wbctl.packid);		/** ����� **/
	/** lv_pkg_batch(&pkg010,"C15","");	����Ѻ **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg010,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg010,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/****NEWYX2:�ɹ������㷨����****/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	lv_pkg_batch(&pkg010,"B41",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	/****NEWYX2:�ɹ������㲻��ȷ****/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	lv_pkg_batch(&pkg010,"32C",tmpBuf);	/** ��ϸҵ���ܽ�� **/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	lv_pkg_batch(&pkg010,"02D",wd_lv_wbctl.o_pkgno);		/** ԭ�����ͺ� **/
	lv_pkg_batch(&pkg010,"CCO",wd_lv_wbctl.cash_qs_no);/** ԭ������������ **/
	lv_pkg_batch(&pkg010,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** ԭ��ί������ **/
	lv_pkg_batch(&pkg010,"0BE",wd_lv_wbctl.o_packid);	/** ԭ����� **/
	/****************
	lv_pkg_batch(&pkg010,"BS1",wd_lv_wbctl.zcnode);		** ����ڵ����� **
	lv_pkg_batch(&pkg010,"BS2",wd_lv_wbctl.zcday);		** �������� **
	lv_pkg_batch(&pkg010,"BS3",wd_lv_wbctl.zcid);		** ����� **
	lv_pkg_batch(&pkg010,"BS4",wd_lv_wbctl.bfflag);		** ������־ **
	lv_pkg_batch(&pkg010,"BS5",wd_lv_wbctl.qsday);		** �������� **
	lv_pkg_batch(&pkg010,"CIB",wd_lv_wbctl.packstat);		** ������״̬ **
	lv_pkg_batch(&pkg010,"72D","");		** ���������� ���� **
	***************/

	lv_gen_pack(&pkg010);
	/**
	dm_show(&(pkg010.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_ssjjs set stat =:Stat2 where packid =:wd_lv_wbctl.o_packid  and packday =:wd_lv_wbctl.o_pack_date and stat <>'0';*/
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_SSJJHZ);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	vtcp_log("[%s][%d]===============ִ�е�������!!!======= ",__FILE__,__LINE__);
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg010)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	pubf_lv_print_dm(&(pkg010.dm));
	if (iRc=iLvSendPackage(&pkg010))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg010);

	vtcp_log("[%s][%d]===============ִ�е�������!!!======= ",__FILE__,__LINE__);

	return(0);
}
/** pkg011 **/
/** ���ڽ�ǻ�ִҵ���Ͱ� **/
/** �������Ϊ��id **/
int lv_get_dqjjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg011;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO, '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg011);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]=wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]=wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg011.opeType = '1';	/** С�� **/
	pkg011.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg011.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg011.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg011.CMTCode,PKGNO_DQJJHZ,3);	/** ��Ϊ��PKG011��**/
	memcpy(pkg011.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	/****Modified by SSH,2006.1.10,�����ִƴ��,��¼�еİ���ϢΪ����Ϣ****/
	Stat[0]=STAT_WZCHECK2;
	/*================================================================================
	EXEC SQL DECLARE DQJJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday, content
	FROM  lv_dqjjs where payqsactno=:wd_lv_wbctl.cash_qs_no and  packid =:wd_lv_wbctl.o_packid and  packday = :wd_lv_wbctl.o_pack_date and stat <>'0';

	EXEC SQL OPEN DQJJHZ;
	*================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1'  and lv_sts ='3' and cash_qs_no = '%s'", PKGNO_DQJJHZ, packid, apatoi( packday, 8 ), wd_lv_wbctl.cash_qs_no);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{ 
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg011);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"���ݿ����!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg011);
			WRITEMSG
				return(-1);
		}
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(INDAY, lv_date(lv_pkgreg.in_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		TXAMT = lv_pkgreg.tx_amt*100.0;
		strcpy(RCPSTAT, lv_pkgreg.rcpstat);
		strcpy(CONTENT, lv_pkgreg.lv_brf);
		strcpy(JZDAY, lv_date(lv_pkgreg.jz_date));
		/****NEWYX2:����ʱ�в����²������****/

		/** Ҫ�ؼ����� **/
		memcpy(pkg011.CMTNo,"006",3);       /***Ҫ�ؼ���006***/
		pkg011.paymPRI='0';                 /***���ȼ�***/
		lv_set_tag(&pkg011,"30A",INDAY);    /** ί������ **/
		lv_set_tag(&pkg011,"BSE",INDAY);   	/** �ۿ����� **/

		vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
		lv_set_tag(&pkg011,"0BC",ORDERNO);	/**	֧��������� **/

		lv_set_tag(&pkg011,"0BH",TXNUM);	/**	ԭҵ�����ͺ� **/	
		/****NEWYX2:����ͽ����з���****/
		lv_set_tag(&pkg011,"CC1",ACBRNO);   /** ԭ�������к� **/
		lv_set_tag(&pkg011,"CC2",ORBRNO);   /** ԭ�������к� **/
		lv_set_tag(&pkg011,"051",WTDAY);    /** ԭί������ **/
		/****NEWYX2:ԭ֧���������****/
		lv_set_tag(&pkg011,"005",lv_pkgreg.o_orderno); 	/** ԭ֧��������� **/
		vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
		memset(tmpBuf, '\0', sizeof(tmpBuf));
		sprintf(tmpBuf,"%015.0f",TXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg011,"33S",tmpBuf);   	/** ��� **/
		lv_set_tag(&pkg011,"CIA",RCPSTAT);   	/** ҵ���ִ״̬ **/
		lv_set_tag(&pkg011,"BSN","");   		/** ������������ **/
		lv_set_tag(&pkg011,"72A",CONTENT);  	/** ���� **/
		if (memcmp(RCPSTAT,"00",2)==0)
		{
			/****ͬ��Ӧ��****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			pcStat[0]=STAT_YES_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%c' where pkgno = '%s' and lw_ind = '2' and  wt_date='%s' and ac_br_no='%s' and o_orderno='%s'", pcStat[0], PKGNO_DQJJ, WTDAY, ORBRNO, ORDERNO);
		}
		else
		{
			/****��ͬ��Ӧ��****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			pcStat[0]=STAT_NO_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%s' where pkgno = '%s' and lw_ind = '2' and  wt_date='%s' and ac_br_no='%s' and o_orderno='%s'", pcStat, PKGNO_DQJJ, WTDAY, ORBRNO, ORDERNO);
		}
		lv_batch_end(&pkg011);
		if ( g_reply_int != DB_OK )
		{
			sprintf( acErrMsg,"�������ݿ�ʧ�� ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
				return(-1);
		}
	}
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int !=DB_OK)
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE DQJJHZ;*/

	/** ��ͷ���� **/
	lv_pkg_batch(&pkg011,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg011,"011",Payqsactno);			/** �������������к� **/
	lv_pkg_batch(&pkg011,"012",Cashqsactno);		/** �������������к� **/
	lv_pkg_batch(&pkg011,"30E",packday);		/** ��ί������ **/
	lv_pkg_batch(&pkg011,"0BD",wd_lv_wbctl.packid);		/** ����� **/
	/** lv_pkg_batch(&pkg011,"C15","");	����Ѻ **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg011,"B63",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg011,"32B",tmpBuf);	/** ��ϸҵ���ܽ�� **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/***
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	****/
	apitoa(wd_lv_wbctl.dtlsuccnt,sizeof(Cnt)-1,Cnt);
	lv_pkg_batch(&pkg011,"B41",Cnt);	/** ��ϸҵ���ܱ��� **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100.0);
	lv_pkg_batch(&pkg011,"32C",tmpBuf);	/** ��ϸҵ���ܽ�� **/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	lv_pkg_batch(&pkg011,"02D",wd_lv_wbctl.o_pkgno);		/** ԭ�����ͺ� **/
	lv_pkg_batch(&pkg011,"CCO",wd_lv_wbctl.cash_qs_no);/** ԭ������������ **/
	lv_pkg_batch(&pkg011,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** ԭ��ί������ **/
	lv_pkg_batch(&pkg011,"0BE",wd_lv_wbctl.o_packid);	/** ԭ����� **/
	/***********
	lv_pkg_batch(&pkg011,"BS1",wd_lv_wbctl.zcnode);		** ����ڵ����� **
	lv_pkg_batch(&pkg011,"BS2",wd_lv_wbctl.zcday);		** �������� **
	lv_pkg_batch(&pkg011,"BS3",wd_lv_wbctl.zcid);		** ����� **
	lv_pkg_batch(&pkg011,"BS4",wd_lv_wbctl.bfflag);		** ������־ **
	lv_pkg_batch(&pkg011,"BS5",wd_lv_wbctl.qsday);		** �������� **
	lv_pkg_batch(&pkg011,"CIB",wd_lv_wbctl.packstat);		** ������״̬ **
	lv_pkg_batch(&pkg011,"72D","");		** ���������� ���� **
	**********/

	lv_gen_pack(&pkg011);
	/**
	dm_show(&(pkg011.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_dqjjs set stat =:Stat2 where packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and stat<>'0'; */
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_DQJJHZ);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݲ�����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg011)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�����������Ʊ����");

		lv_destroy(&pkg011);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&pkg011))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg011);

	return(0);
}
/** ƴͨ����Ϣҵ���Ͱ� PKG012 **/  
int lv_get_tyxx_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[128];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];
	char    sql_stat2[2];
	TLVPACKAGE pkg012;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
	/*char    INFOLEN[9];*/
	char    CONTENT[256];

	memset(&payin, 0 , sizeof(payin));/* add by lhf0423 */
	memset(CONTENT,    '\0', sizeof(CONTENT));
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	/*memset(INFOLEN,  '\0', sizeof(INFOLEN));*/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));

	lv_load_tagifno();
	lv_create(&pkg012);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		strcpy( g_pub_tx.reply,"P071");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** �õ�������double�䵽int **/
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, lv_date(wd_lv_wbctl.pack_date), sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg012.opeType = '1';	/** С�� **/
	pkg012.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg012.packDate , lv_date(wd_lv_wbctl.pack_date), 8);
	pkg012.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg012.CMTCode,PKGNO_TYXX,3);	/** ��Ϊ�� PKG012��**/
	memcpy(pkg012.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	/*======================================================================
	EXEC SQL DECLARE TYXX1 CURSOR FOR 
	select txnum,orbrno,acbrno,wtday,orderno,infolen,content from lv_tyxxs where packid =:packid 
	and packday=:packday and stat =:sql_stat;

	EXEC SQL OPEN TYXX1;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_TYXX, packid, packday, sql_stat);
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		/*		EXEC SQL FETCH TYXX1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,:INFOLEN,:CONTENT;*/

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"�������ݿ����!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg012);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg012);
			WRITEMSG
				return(-1);
		}
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		/*strcpy(INFOLEN, lv_pkgreg.infolen);*/
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		/** Ҫ�ؼ����� **/

		memcpy(pkg012.CMTNo,"005",3);       /***Ҫ�ؼ���005***/
		pkg012.paymPRI='0';                 /***���ȼ�***/

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		lv_set_tag(&pkg012,"0BG",TXNUM);	/** ҵ�����ͺ� **/
		lv_set_tag(&pkg012,"52A",ORBRNO);	/** �������к� **/
		lv_set_tag(&pkg012,"58A",ACBRNO);	/** �������к� **/
		lv_set_tag(&pkg012,"30A",WTDAY);	/**	ί������ **/
		lv_set_tag(&pkg012,"0BC",ORDERNO);	/**	��Ϣҵ�����(֧ͬ���������) **/
		/** ƴ�������� **/
		if (memcmp(TXNUM,TXNUM_DS,sizeof(TXNUM)-1)==0) /** ����ҵ�� **/
		{
			lv_set_tag(&pkg012,"B40","00000000");
		}
		else  if(memcmp(TXNUM,TXNUM_DF,sizeof(TXNUM)-1)==0) /** ����ҵ�� **/
		{
			lv_set_tag(&pkg012,"B40","00000000");
		}
		else     /** һ����Ϣ���� **/
		{
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			memcpy(payin.F_wtday, WTDAY,sizeof(payin.F_wtday));
			memcpy(payin.F_orbrno,ORBRNO,sizeof(payin.F_orbrno));
			memcpy(payin.F_orderno,ORDERNO,sizeof(payin.F_orderno));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****û�и�������****/
				char pcAutoAdd[384];
				memset(pcAutoAdd,' ',sizeof(pcAutoAdd));
				pcAutoAdd[sizeof(pcAutoAdd)-1]='\0';
				memcpy(pcAutoAdd,"NONE",4);
				memcpy(pcAutoAdd+60,CONTENT,strlen(CONTENT));
				if (CONTENT[0]==' '||CONTENT[0]=='\0')
				{
					memcpy(pcAutoAdd+60,"NONE",4);
				}	
				memcpy(pcAutoAdd+375,"00000000",8);

				lv_set_tag(&pkg012,"B40","00000383");   /** �������ݳ���**/
				lv_set_tag(&pkg012,"72C",pcAutoAdd);   /** ��������**/
			}else
			{
				/****�и�������****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg012,"B40",len_buf);  /** �������ݳ���**/
				lv_set_tag(&pkg012,"72C",tmpBuf);       /** �������� **/
			}
		}
		lv_batch_end(&pkg012);
	}
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE TYXX1;*/

	/** ��ͷ���� **/
	lv_pkg_batch(&pkg012,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg012,"C36",wd_lv_wbctl.pay_qs_no);			/** ������ڵ�� **/
	lv_pkg_batch(&pkg012,"C37",wd_lv_wbctl.cash_qs_no);		/** �����սڵ�� **/
	lv_pkg_batch(&pkg012,"30E",packday);/** ��ί������ **/
	lv_pkg_batch(&pkg012,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	lv_pkg_batch(&pkg012,"BS7",wd_lv_wbctl.ffflag);	/** Ͻ�ڷַ���־ **/
	lv_pkg_batch(&pkg012,"0BG",wd_lv_wbctl.txnum);	/** ҵ�����ͺ� **/

	sprintf(Cnt,"%04.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg012,"B42",Cnt);    /** ��ϸҵ���ܱ��� **/

	/** lv_pkg_batch(&pkg012,"72D","");  				** ���������� **/

	vtcp_log("[%s][%d]==========\n",__FL__);

	lv_gen_pack(&pkg012);
	/**
	dm_show(&(pkg012.dm));
	**/
	/** ����״̬Ϊ�ѷ��� **/
	/*EXEC SQL UPDATE lv_tyxxs set stat =:sql_stat2 where packid =:packid and packday=:packday
	and stat =:sql_stat;*/
	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lv_sts='%s' and packid ='%s' and pack_date=%s and lw_ind='1' and pkgno='%s' and pay_qs_no='%s'" , sql_stat, packid, packday, PKGNO_TYXX,wd_lv_wbctl.pay_qs_no);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"û�п�ƴ���ļ�¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"�����������Ʊ����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg012)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���·��͵Ǽǲ�����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	/** ���� **/
	vtcp_log("%s,%d,����\n",__FL__);
	if (iRc=iLvSendPackage(&pkg012))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg012);
	return(0);
}
/** pkg013 **/
/** ʵʱ��Ϣҵ���Ͱ� **/
/** �������Ϊ��id **/
int lv_get_ssxx_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[5];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[840];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg013;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** �α����ñ��� **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	char	ADDID[9];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,	 '\0', sizeof(ORDERNO));
	memset(ADDID,	 '\0', sizeof(ADDID));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg013);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]=wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]=wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"δ�鵽��Ӧ��������¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	/** ״̬��������ſ��Է��� **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** ���α���Ҫ���ֶθ�ֵ **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** ��ʼƴ���� **/
	pkg013.opeType = '1';	/** С�� **/
	pkg013.pkgType = 1;		/** ��pkg�� **/
	memcpy(pkg013.packDate, pcLvGetDay(), sizeof(wd_lv_wbctl.pack_date) -1);
	pkg013.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg013.CMTCode,"013",3);	/** ��Ϊ��PKG013��**/
	memcpy(pkg013.CMTNo,"005",3);       /***Ҫ�ؼ���005***/
	memcpy(pkg013.workDate,pcLvGetDay_work(),8);

	/** ��ʼ���Ҷ�Ӧ��ϸ���еļ�¼ **/
	Stat[0]=STAT_WZLR;
	/*======================================================================
	EXEC SQL DECLARE SSXX CURSOR FOR 
	SELECT  txnum, orbrno,acbrno, wtday,orderno,addid
	FROM  lv_ssxxs where payqsactno=:wd_lv_wbctl.pay_qs_no and  packid =:packid and packday = :packday and sts =Stat;

	EXEC SQL OPEN SSXX;

	EXEC SQL FETCH SSXX INTO :TXNUM, :ORBRNO, :ACBRNO, :WTDAY, :ORDERNO,:ADDID;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s' and pay_qs_no = '%s'", PKGNO_SSXX, packid, packday, Stat, wd_lv_wbctl.pay_qs_no);
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"�����ѯ�α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���ݿ��޴˼�¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
	strcpy(ORDERNO, lv_pkgreg.orderno);
	strcpy(ADDID, lv_pkgreg.addid);

	/** Ҫ�ؼ����� **/
	lv_set_tag(&pkg013,"0BG",TXNUM);	/** ҵ�����ͺ� **/
	lv_set_tag(&pkg013,"52A",ORBRNO);   /** �������к� **/
	lv_set_tag(&pkg013,"58A",ACBRNO);   /** �������к� **/
	lv_set_tag(&pkg013,"30A",WTDAY);    /** ί������ **/
	lv_set_tag(&pkg013,"0BC",ORDERNO);	/**	֧��������� **/	

	/** ƴ�������� **/
	/***
	if(memcmp(TXNUM,TXNUM_ZHCX,sizeof(TXNUM)-1)==0) ** �ͻ��˻���ѯ **
	{
	lv_set_tag(&pkg013,"B40","00000000");
	}
	else  if(memcmp(TXNUM,TXNUM_CXYD,sizeof(TXNUM)-1)==0) ** �ͻ��˻���ѯӦ�� **
	{
	lv_set_tag(&pkg013,"B40","00000000");
	}
	else
	{
	***/
	/** �ʺŲ�ѯ��Ӧ��ssxxs���и����ֶβ�ȫ,��ʱ������ȡֵ **/
	memset(tmpBuf,'\0',sizeof(tmpBuf));
	iLvGetAddit_Notpay(WTDAY,ADDID,"000",tmpBuf,0);
	if (tmpBuf[0]=='\0')
	{
		/****û�и�������****/
		lv_set_tag(&pkg013,"B40","00000000");   /** �������ݳ���**/
	}
	else
	{
		/****�и�������****/
		char len_buf[9];
		vtcp_log("%s,%d �»�Ϊע�����ƴ����������û�и� ",__FILE__,__LINE__);
		vtcp_log("%s,%d tmpbuf=[%s]",__FILE__,__LINE__,tmpBuf);
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		vtcp_log("%s,%d tmpbuf+181=[%s]",__FILE__,__LINE__,tmpBuf+181);
		/** �µİ��ĸ�Ϊ173
		if (memcmp(tmpBuf+181,"04",2)==0)
		**/
		if (memcmp(tmpBuf+173,"04",2)==0)
		{
			/****ģ��֧ƱȦ���Ӱ������****/
			char pcPicture[1024*2];
			int  nBlockCnt=50;
			int i;
			memset(pcPicture,'\0',sizeof(pcPicture));
			sprintf(len_buf,"%08d",strlen(tmpBuf)+nBlockCnt*1000);
			lv_set_tag(&pkg013,"B40",len_buf);  /** �������ݳ���**/
			sprintf(pcPicture+strlen(tmpBuf)-8,"%08d",nBlockCnt*1000);
			lv_set_tag(&pkg013,"72C",tmpBuf);       /** �������� **/
			memset(pcPicture,'0',1000);
			for (i=0;i<nBlockCnt;i++)
				lv_set_tag_multi(&pkg013,"72C",pcPicture);   /** �������� **/
		}
		else
		{ 
			lv_set_tag(&pkg013,"B40",len_buf);  /** �������ݳ���**/
			lv_set_tag(&pkg013,"72C",tmpBuf);       /** �������� **/
		}
	}
	/**	lv_set_tag(&pkg013,"B40",);	��Ϣ���� **/	
	/**	lv_set_tag(&pkg013,"72C",);	��Ϣ���� **/

	lv_batch_end(&pkg013);

	/*EXEC SQL CLOSE SSXX;*/

	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** ��ͷ���� **/
	lv_pkg_batch(&pkg013,"02C",wd_lv_wbctl.pkgno);	/** �����ͺ� **/
	lv_pkg_batch(&pkg013,"C36",Payqsactno);			/** ������ڵ�� **/
	lv_pkg_batch(&pkg013,"C37",Cashqsactno);		/** �����սڵ�� **/
	lv_pkg_batch(&pkg013,"30E",packday);/** ��ί������ **/
	lv_pkg_batch(&pkg013,"0BD",wd_lv_wbctl.packid);	/** ����� **/
	lv_pkg_batch(&pkg013,"BS7",wd_lv_wbctl.ffflag);	/** Ͻ�ڷַ���־ **/
	lv_pkg_batch(&pkg013,"0BG",wd_lv_wbctl.txnum);	/** ҵ�����ͺ� **/
	sprintf(Cnt,"%04d",wd_lv_wbctl.dtlcnt);
	vtcp_log("[%s][%d]========cnt=[%s]\n",__FL__,Cnt);
	lv_pkg_batch(&pkg013,"B42",Cnt);    /** ��ϸҵ���ܱ��� **/
	/** lv_pkg_batch(&pkg013,"72D","");		���������� ���� **/

	lv_gen_pack(&pkg013);
	/**
	dm_show(&(pkg013.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_ssxxs set stat =:Stat2 where packid =:packid and packday =:packday and stat =:Stat;*/

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' " , Stat, packid, packday, PKGNO_SSXX);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"�������ݿ����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"����û���ҵ�!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"������������");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg013)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"���·��͵Ǽǲ�����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&pkg013))
	{
		lv_destroy(&pkg013);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg013);

	return(0);
}
/** ���ɸ�ʽ���ķ��� **/
/** ��������ǲ�ѯ��� **/
int lv_get_cmt303_tag(char *packday,char *orderno)
{
	int     	ret;
	int		iRc=0;
	TLVPACKAGE 	CMT303;

	static	NOTPAY_IN_AREA	notpay_in;
	static struct hv_fzf_c      hv_fzf;

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT303);
	/* cmt��ķ��� hv_fzf ���� key: otype,or_br_no,wt_date,orderno,cmtno*/
	if (GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_fzf.or_br_no))
	{
		vtcp_log("[%s][%d]�õ������ߺŴ���!",__FL__);
		vtcp_log("�õ������ߺŴ���!");
		lv_destroy(&CMT303);
		return(-1);
	}
	hv_fzf.wt_date = apatoi( packday, 8 );
	memcpy(hv_fzf.orderno, orderno, sizeof(hv_fzf.orderno) -1 );
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '303' and lw_ind='1'  ",  hv_fzf.or_br_no,hv_fzf.wt_date, hv_fzf.orderno))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c303s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT303);
		return(-1);
	}
	vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf.hv_fzf_sts[0]);
	/** ״̬�Ǹ��˲ſ��Է��� **/
	if ( hv_fzf.hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf.hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT303);
		return(-1);
	}
	/** ��ʼƴ���� **/

	CMT303.opeType = '1';   /** С�� **/
	CMT303.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT303.packDate , lv_date( hv_fzf.wt_date), 8);
	CMT303.packID = apatoi(hv_fzf.orderno,sizeof(hv_fzf.orderno)-1);

	memcpy(CMT303.CMTCode,"303",3);
	memcpy(CMT303.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	lv_set_tag(&CMT303,"010",hv_fzf.sendco);	/** �������Ĵ��� **/
	lv_set_tag(&CMT303,"011",hv_fzf.pay_qs_no);	/** �����������к� **/
	lv_set_tag(&CMT303,"52A",hv_fzf.or_br_no);	/** �������к� **/
	lv_set_tag(&CMT303,"30A",lv_date(hv_fzf.wt_date));	/** ί������ **/
	lv_set_tag(&CMT303,"0B9",hv_fzf.receco);	/** �ձ����Ĵ��� **/
	if (hv_fzf.cash_qs_no[0]!='\0')	
	{
		lv_set_tag(&CMT303,"012",hv_fzf.cash_qs_no);	/** �����������к�  **/
		lv_set_tag(&CMT303,"58A",hv_fzf.ac_br_no);	/** �������к�  **/
	}
	lv_set_tag(&CMT303,"053",hv_fzf.req_content);	/** ����  **/

	lv_gen_pack(&CMT303);
	dm_show(&(CMT303.dm));

	/** ��notpay_in **/
	memcpy(notpay_in.cmtno,		"303",					sizeof(hv_fzf.cmtno)-1);
	memcpy(notpay_in.orderno,	hv_fzf.orderno,	sizeof(hv_fzf.orderno)-1);
	memcpy(notpay_in.orbrno,	hv_fzf.or_br_no,		sizeof(hv_fzf.or_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf.pay_qs_no,	sizeof(hv_fzf.pay_qs_no)-1);
	memcpy(notpay_in.acbrno,	hv_fzf.ac_br_no,		sizeof(hv_fzf.ac_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf.cash_qs_no,sizeof(hv_fzf.cash_qs_no)-1);
	/** txamt **/
	/** opkgno **/
	/** owtday **/
	/** oorderno **/
	memcpy(notpay_in.content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);

	/** ���뷢�ͼ�¼ **/
	/*****************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	lv_destroy(&CMT303);
	return(-1);
	}

	ret=lv_Recv_Ins_Tab(&CMT303,&notpay_in,1);
	if(ret)
	{ 
	vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
	vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
	lv_destroy(&CMT303);
	return(-1);
	}********************/

	hv_fzf.hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf.sendco, lvca.tlrno, sizeof(hv_fzf.sendco)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT303);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("����\n");
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	dm_show(&CMT303.dm);
	if (iRc=iLvSendPackage(&CMT303))
	{
		lv_destroy(&CMT303);
		return(-1);
	}
	lv_destroy(&CMT303);

	return(0);

}
/** С��ҵ�������ķ��� **/
int lv_get_cmt321_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT321;
	static	NOTPAY_IN_AREA	notpay_in;

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT321);

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '321' and lw_ind='1'  ",  hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT321);
		return(-1);
	}
	/** ״̬�Ǹ��˲ſ��Է��� **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR)
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT321);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/** ��ʼƴ���� **/
	CMT321.opeType = '1';   /** С�� **/
	CMT321.pkgType = 0;     /** ��cmt�� **/

	memcpy(CMT321.CMTCode,"321",3);
	memcpy(CMT321.workDate,pcLvGetDay_work(),8);
	memcpy(CMT321.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT321.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	/** ��ʼ��� **/
	lv_set_tag(&CMT321,"30A",lv_date(hv_fzf->wt_date));		/** ί������ **/
	lv_set_tag(&CMT321,"011",hv_fzf->pay_qs_no);	/** ���������������к� **/
	lv_set_tag(&CMT321,"0BD",hv_fzf->orderno);		/** ����������� **/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	lv_set_tag(&CMT321,"02D",hv_fzf->o_cmtno);		/** ԭ�����ͺ� **/
	lv_set_tag(&CMT321,"30I",lv_date(hv_fzf->o_pack_date));	/** ��ί������ **/
	lv_set_tag(&CMT321,"0BE",hv_fzf->o_packid);		/** ����� **/
	/** lv_set_tag(&CMT321,"C15","");			��Ѻ **/
	lv_set_tag(&CMT321,"72A",hv_fzf->req_content);	/** �������� **/

	lv_gen_pack(&CMT321);
	dm_show(&(CMT321.dm));

	/** ��notpay_in **/
	memcpy(notpay_in.cmtno,     "321",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,	hv_fzf->o_cmtno,		sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,	lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpay_in.oorderno,	hv_fzf->o_orderno,	sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,	hv_fzf->filler,		sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/***************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	lv_destroy(&CMT321);
	return(-1);
	}
	**************************/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	ret=lv_Recv_Ins_Tab(&CMT321,&notpay_in,1);
	if (ret)	
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		lv_destroy(&CMT321);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT321);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT321))
	{
		lv_destroy(&CMT321);
		return(-1);
	}
	lv_destroy(&CMT321);

	return(0);

}
/** ʵʱҵ��������뱨�ķ��� **/
int lv_get_cmt323_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT323;
	static	NOTPAY_IN_AREA	notpay_in;

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT323);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '323' and lw_ind='1' ",  hv_fzf->req_br_no,  hv_fzf->wt_date, hv_fzf->orderno ))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT323);
		return(-1);
	}
	/** ״̬�Ǹ��˲ſ��Է��� **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR)
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT323);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT323.opeType = '1';   /** С�� **/
	CMT323.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT323.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT323.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT323.CMTCode,"323",3);
	memcpy(CMT323.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	lv_set_tag(&CMT323,"30A",lv_date(hv_fzf->wt_date));		/** ����ί������ **/
	lv_set_tag(&CMT323,"011",hv_fzf->pay_qs_no);	/** ���������������к� **/
	lv_set_tag(&CMT323,"0BD",hv_fzf->orderno);		/** ����������� **/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	lv_set_tag(&CMT323,"02D",hv_fzf->o_cmtno);		/** ԭ�����ͺ� **/
	lv_set_tag(&CMT323,"30I",lv_date(hv_fzf->o_pack_date));	/** ԭ��ί������ **/
	lv_set_tag(&CMT323,"0BE",hv_fzf->o_packid);		/** ԭ����� **/
	/** lv_set_tag(&CMT323,"C15","");			��Ѻ **/
	lv_set_tag(&CMT323,"72A",hv_fzf->req_content);	/** �������� **/

	lv_gen_pack(&CMT323);
	dm_show(&(CMT323.dm));

	/** ��notpay_in **/
	memcpy(notpay_in.cmtno,     "323",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,    lv_date(hv_fzf->o_wt_date), 8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/*****
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	lv_destroy(&CMT323);
	return(-1);
	}
	***********************************/
	ret=lv_Recv_Ins_Tab(&CMT323,&notpay_in,1);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		lv_destroy(&CMT323);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT323);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT323))
	{
		lv_destroy(&CMT323);
		return(-1);
	}
	lv_destroy(&CMT323);

	return(0);

}
/** ���ҵ��ֹ�����뱨�ķ��� **/
int lv_get_cmt327_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT327;

	static	NOTPAY_IN_AREA	notpay_in;

	memset(&notpay_in,	0x00,	 sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT327);
	/****NEWYX:����������ʶ****/
	lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '327' and lw_ind='1' ", hv_fzf->or_br_no,  hv_fzf->wt_date, hv_fzf->orderno);
	if (lvca.rtcode )
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT327);
		return(-1);
	}
	/** ״̬�Ǹ��˲ſ��Է��� **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT327);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT327.opeType = '1';   /** С�� **/
	CMT327.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT327.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT327.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT327.CMTCode,"327",3);
	memcpy(CMT327.workDate,pcLvGetDay_work(),8);


	/** ��ʼ��� ����**/
	if (hv_fzf->reqtype[0]=='0')
	{
		vtcp_log("[%s][%d]��ʼ����ƴ��!\n",__FILE__,__LINE__);
		lv_set_tag(&CMT327,"52A",hv_fzf->req_br_no);		/** ֹ�������� **/
		lv_set_tag(&CMT327,"58A",hv_fzf->res_br_no);		/** ֹ��Ӧ���� **/
		lv_set_tag(&CMT327,"0BH",hv_fzf->otxnum);		/** ԭҵ�����ͺ� **/
		lv_set_tag(&CMT327,"051",lv_date(hv_fzf->o_wt_date));		/** ԭί������ **/
		lv_set_tag(&CMT327,"005",hv_fzf->o_orderno);		/** ԭ֧��������� **/

	}/**����**/
	else if (hv_fzf->reqtype[0]=='1')
	{
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
		lv_set_tag(&CMT327,"02D",hv_fzf->o_cmtno);		/** ԭ�����ͺ� **/
		lv_set_tag(&CMT327,"30I",lv_date(hv_fzf->o_pack_date));	/** ԭ��ί������ **/
		lv_set_tag(&CMT327,"0BE",hv_fzf->o_packid);		/** ԭ����� **/
	}
	
	
	lv_set_tag(&CMT327,"30A",lv_date(hv_fzf->wt_date));		/** ֹ��ί������ **/
	lv_set_tag(&CMT327,"011",hv_fzf->pay_qs_no);	/** ֹ������������ **/

	lv_set_tag(&CMT327,"0BD",hv_fzf->orderno);		/** ֹ��������� **/
	lv_set_tag(&CMT327,"012",hv_fzf->cash_qs_no);	/** ֹ��Ӧ�������� **/

	lv_set_tag(&CMT327,"BS8",hv_fzf->reqtype);		/** ֹ������ **/

	/** lv_set_tag(&CMT327,"C15","");			��Ѻ **/
	lv_set_tag(&CMT327,"72A",hv_fzf->req_content);	/** ֹ��Ӧ���� **/

	lv_gen_pack(&CMT327);
	dm_show(&(CMT327.dm));

	/** ��notpay_in **/
	memcpy(notpay_in.cmtno,     "327",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,   	sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no,	sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday , lv_date(    hv_fzf->o_wt_date),  8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	ret=lv_Ins_Cmthtr(&notpay_in);
	if (ret)
	{
		vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
		vtcp_log("���뵥��������ʷ��ϸ����");
		lv_destroy(&CMT327);
		return(-1);
	}
	ret=lv_Recv_Ins_Tab(&CMT327,&notpay_in,1);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		lv_destroy(&CMT327);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT327);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT327))
	{
		lv_destroy(&CMT327);
		return(-1);
	}
	lv_destroy(&CMT327);

	return(0);

}
/** CMT316С������������״̬֪ͨ���ķ��� **/
int lv_get_cmt316_tag(char *packday,int dtlid,char *opackid)
{
	int     ret;
	return(0);

}
/** ��ѯ���ķ��� cmt301  **/
/** ��������ǲ�ѯ��źͲ�ѯ���к� **/
int lv_get_cmt301_tag(struct hv_fzf_c *hv_fzf)
{
	int     	ret;
	int		iRc=0;
	char    	tmpBuf[30];               /* ��������ԭ���ҷ��š���� */
	TLVPACKAGE 	CMT301;

	static  NOTPAY_IN_AREA  	notpayin;

	memset(&notpayin,	0x00 , sizeof(notpayin));

	memset(tmpBuf,		0x00 , sizeof(tmpBuf));

	lv_load_tagifno();
	lv_create(&CMT301);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '301' and lw_ind='1'  ",  hv_fzf->or_br_no,		hv_fzf->req_date,hv_fzf->orderno))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c301s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT301);
		return(-1);
	}
	/** �ж�״̬**/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT301);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT301.opeType = '1';   /** С�� **/
	CMT301.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT301.packDate,lv_date( hv_fzf->wt_date), 8);
	CMT301.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT301.CMTCode,"301",3);
	memcpy(CMT301.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	lv_set_tag(&CMT301,"CJ9",lv_date(hv_fzf->wt_date));   /** ��ѯ���� **/
	lv_set_tag(&CMT301,"010",hv_fzf->sendco);	/** �������Ĵ��� **/
	lv_set_tag(&CMT301,"011",hv_fzf->pay_qs_no);	/** �����������к� **/
	lv_set_tag(&CMT301,"01C",hv_fzf->or_br_no);   /** ��ѯ���к� **/
	lv_set_tag(&CMT301,"056",hv_fzf->orderno);   /** ��ѯ��� **/
	lv_set_tag(&CMT301,"0B9",hv_fzf->receco);   /** �ձ����Ĵ��� **/
	lv_set_tag(&CMT301,"012",hv_fzf->cash_qs_no);  /** �����������к� **/
	lv_set_tag(&CMT301,"01D",hv_fzf->ac_br_no);  /** �鸴���к� **/
	lv_set_tag(&CMT301,"051",lv_date(hv_fzf->o_wt_date));  /** ԭί������ **/
	lv_set_tag(&CMT301,"CC1",hv_fzf->o_or_br_no);  /** ԭ�������к� **/
	lv_set_tag(&CMT301,"CC2",hv_fzf->o_ac_br_no);  /** ԭ�������к� **/
	lv_set_tag(&CMT301,"CE2",hv_fzf->otype);  /** ԭ�������� **/
	lv_set_tag(&CMT301,"005",hv_fzf->o_orderno);  /** ԭ֧��������� **/

	vtcp_log("[%s][%d]o_tx_amt==[%.2f]\n",__FILE__,__LINE__,hv_fzf->o_tx_amt);
	vtcp_log("[%s][%d]o_tx_amt==[%f]\n",__FILE__,__LINE__,hv_fzf->o_tx_amt*100);
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",hv_fzf->o_tx_amt*100);
	vtcp_log("[%s][%d]tmpbuf==[%s]\n",__FILE__,__LINE__,tmpBuf);
	vtcp_log("[%s][%d]tmpbuf==[%s]\n",__FILE__,__LINE__,tmpBuf);

	lv_set_tag(&CMT301,"CND",tmpBuf);  /** ԭ���ҷ��š���� **/
	lv_set_tag(&CMT301,"053",hv_fzf->req_content);	/** ��ѯ����  **/

	lv_gen_pack(&CMT301);
	dm_show(&(CMT301.dm));

	/** ��notpayin **/
	memcpy(notpayin.cmtno,     "301",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->req_br_no,   sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	sprintf(notpayin.otxamt,"%015f",hv_fzf->o_tx_amt);
	/** opkgno **/
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date), 8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/*******
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	lv_destroy(&CMT301);
	return(-1);
	}************************************/
	ret=lv_insSendList(&CMT301); 
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");

		lv_destroy(&CMT301);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf->send_tel, lvca.tlrno, sizeof(hv_fzf->send_tel)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c301 error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT301);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("����\n");

	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	if (iRc=iLvSendPackage(&CMT301))
	{
		lv_destroy(&CMT301);
		return(-1);
	}
	lv_destroy(&CMT301);
	return(0);

}
/** С��ҵ���˻����뱨�� cmt319 **/
/** ��������������к������ **/
int lv_get_cmt319_tag(struct hv_fzf_c *hv_fzf)
{
	int    	 	ret;
	int		iRc=0;
	TLVPACKAGE 	CMT319;

	static  NOTPAY_IN_AREA  notpayin;

	memset(&notpayin,	0x00 , sizeof(notpayin));

	lv_load_tagifno();
	lv_create(&CMT319);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '319' and lw_ind='1'  ",  hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno) )
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT319);
		return(-1);
	}
	/** ״̬�Ǹ��˲ſ��Է��� **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->tx_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT319);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT319.opeType = '1';   /** С�� **/
	CMT319.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT319.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT319.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT319.CMTCode,"319",3);
	memcpy(CMT319.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	if (hv_fzf->reqtype[0]='0')
	{
		vtcp_log("[%s][%d]���ʽ���ƴ��!\n",__FILE__,__LINE__);
		lv_set_tag(&CMT319,"52A",hv_fzf->req_br_no);	/*�˻������� */
		lv_set_tag(&CMT319,"58A",hv_fzf->ac_br_no);    /**    �˻�Ӧ���� **/
		lv_set_tag(&CMT319,"0BH",hv_fzf->otxnum);  /** ԭҵ�����ͺ� **/
		lv_set_tag(&CMT319,"051",lv_date(hv_fzf->o_wt_date));  /**  ԭί������ **/
		lv_set_tag(&CMT319,"005",hv_fzf->o_orderno);  /**  ԭ֧��������� **/

	}
	else if (hv_fzf->reqtype[0]=='1')
	{
		vtcp_log("[%s][%d]������ҵ��ƴ��!\n",__FILE__,__LINE__);
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
		lv_set_tag(&CMT319,"02D",hv_fzf->o_cmtno);  /** ԭ�����ͺ� **/
		lv_set_tag(&CMT319,"30I",lv_date(hv_fzf->o_pack_date));  /** ԭ��ί������ **/
		lv_set_tag(&CMT319,"0BE",hv_fzf->o_packid);  /** ԭ����� **/
	}
	lv_set_tag(&CMT319,"30A",lv_date(hv_fzf->wt_date));   /** ί������ **/
	lv_set_tag(&CMT319,"011",hv_fzf->pay_qs_no);	/** �˻����뷢���� **/
	lv_set_tag(&CMT319,"0BD",hv_fzf->orderno);   /** �˻�������� **/
	lv_set_tag(&CMT319,"012",hv_fzf->cash_qs_no);   /** �˻�Ӧ�������� **/
	lv_set_tag(&CMT319,"BS8",hv_fzf->reqtype);  /** �˻����� **/

	/**  lv_set_tag(&CMT319,"C15",hv_fzf->miya);  ** ��Ѻ **/
	lv_set_tag(&CMT319,"72A",hv_fzf->req_content);	/** �˻����븽�� **/

	lv_gen_pack(&CMT319);
	dm_show(&(CMT319.dm));

	/** ��notpayin **/
	memcpy(notpayin.cmtno,     "319",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpayin.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/****
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret){
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	set_zd_data("0130",acErrMsg);
	lv_destroy(&CMT319);
	return(-1);
	}
	**************************************************************/
	ret=lv_insSendList(&CMT319);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT319);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("[%s][%d]�����������Ʊ����",__FL__);
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT319);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("[%s][%d]�ر��α����",__FL__);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT319))
	{
		lv_destroy(&CMT319);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	lv_destroy(&CMT319);

	return(0);
}
/** ���ҵ��ֹ��Ӧ���� cmt328 **/
/** ��������������к������ **/
int lv_get_cmt328_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT328;

	static  NOTPAY_IN_AREA  notpayin;

	memset(&notpayin, '\0', sizeof(notpayin));

	lv_load_tagifno();
	lv_create(&CMT328);
	/****NEWYX:����������ʶ****/
	if (g_reply_int = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d  and orderno = '%s' and cmtno = '327' and lw_ind='2' ",  hv_fzf->or_br_no,hv_fzf->wt_date, hv_fzf->orderno ))
	{
		sprintf( acErrMsg,"�����α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_ctczs error [%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"δ�鵽��Ӧ�ļ�¼!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	/** ״̬�Ǹ��˲ſ��Է��� **/
	/** if( hv_fzf->hv_fzf_sts[0] != STAT_WZCHECK1 && hv_fzf->hv_fzf_sts[0] != STAT_WZCHECK2 ) **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1 )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		sprintf( acErrMsg,"��¼״̬����!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	/** ��ʼƴ���� **/
	CMT328.opeType = '1';   /** С�� **/
	CMT328.pkgType = 0;     /** ��cmt�� **/

	memcpy(CMT328.CMTCode,"328",3);
	memcpy(CMT328.workDate,pcLvGetDay_work(),8);
	memcpy(CMT328.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT328.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	/** ��ʼ��� **/
	lv_set_tag(&CMT328,"30A",lv_date(hv_fzf->res_date));   /** ί������ **/
	lv_set_tag(&CMT328,"011",hv_fzf->cash_qs_no);	/** ֹ��Ӧ�������� **/
	lv_set_tag(&CMT328,"52A",hv_fzf->res_br_no);	/** ֹ��Ӧ���� **/
	lv_set_tag(&CMT328,"0BD",hv_fzf->res_orderno);   /** ֹ��Ӧ����� **/
	lv_set_tag(&CMT328,"051",lv_date(hv_fzf->req_date));  /** ԭֹ������ί������ **/
	lv_set_tag(&CMT328,"CCO",hv_fzf->pay_qs_no);   /** ԭֹ������������ **/
	lv_set_tag(&CMT328,"CC1",hv_fzf->req_br_no);   /** ԭֹ�������� **/
	lv_set_tag(&CMT328,"0BE",hv_fzf->req_orderno);  /** ԭֹ��������� **/
	lv_set_tag(&CMT328,"BSA",hv_fzf->reqtype);  /** ԭֹ�����ͺ� **/
	/** lv_set_tag(&CMT328,"C15",hv_fzf->miya);  ** ��Ѻ **/
	lv_set_tag(&CMT328,"BSB",hv_fzf->res_sts);  /** ֹ��Ӧ�� **/
	lv_set_tag(&CMT328,"72A",hv_fzf->res_content);	/** ֹ��Ӧ���� **/

	lv_gen_pack(&CMT328);
	dm_show(&(CMT328.dm));

	/** ��notpayin **/
	memcpy(notpayin.cmtno,     "328",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	/** txamt **/
	memcpy(notpayin.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/***********************
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	sprintf( acErrMsg,"���뵥��������ʷ��ϸ����");
	set_zd_data("0130",acErrMsg);
	lv_destroy(&CMT328);
	WRITEMSG
	return(-1);
	}
	*********************************/
	ret=lv_insSendList(&CMT328);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		sprintf( acErrMsg,"����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((g_reply_int = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		set_zd_data("0130",acErrMsg);
		sprintf( acErrMsg,"�����������Ʊ����");
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Hv_fzf_Clo_Upd())
	{
		sprintf( acErrMsg,"�ر��α���� - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT328))
	{
		lv_destroy(&CMT328);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&CMT328);
	return(0);
}
/** �鸴���ķ��� **/
/** ��������ǲ�ѯ��� **/
int lv_get_cmt302_tag(struct hv_fzf_c *hv_fzf )
{
	int		ret;
	int		iRc=0;
	char 		tmpBuf[1024];
	TLVPACKAGE 	CMT302;

	static  NOTPAY_IN_AREA  notpay_in;

	memset(&notpay_in,	0x00, 	sizeof(notpay_in));

	memset(tmpBuf,		0x00,	sizeof(tmpBuf));

	lv_load_tagifno();
	lv_create(&CMT302);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno='301' and lw_ind='2'  ", hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c302s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT302);
		return(-1);
	}
	vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
	/** ״̬�Ǹ��˲ſ��Է��� **change by liuyue 20051126*/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1 )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT302);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT302.opeType = '1';   /** С�� **/
	CMT302.pkgType ='0';     /** ��cmt�� **/
	memcpy(CMT302.packDate, lv_date(hv_fzf->res_date), 8);
	CMT302.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT302.CMTCode,"302",3);
	memcpy(CMT302.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	lv_set_tag(&CMT302,"CJ1",lv_date(hv_fzf->res_date));	/** �鸴���� **/
	lv_set_tag(&CMT302,"010",hv_fzf->receco);	/** �������� **/
	lv_set_tag(&CMT302,"011",hv_fzf->cash_qs_no);	/** ���������� **/
	lv_set_tag(&CMT302,"01D",hv_fzf->res_br_no);	/** �鸴���к� **/
	lv_set_tag(&CMT302,"043",hv_fzf->res_orderno);	/** �鸴��� **/
	lv_set_tag(&CMT302,"0B9",hv_fzf->sendco);	/** �ձ�����  **/
	lv_set_tag(&CMT302,"012",hv_fzf->pay_qs_no);	/** ����������  **/
	lv_set_tag(&CMT302,"01C",hv_fzf->req_br_no);	/** ��ѯ���к�  **/
	lv_set_tag(&CMT302,"CJA",lv_date(hv_fzf->req_date));		/** ԭ��ѯ����  **/
	lv_set_tag(&CMT302,"CP1",hv_fzf->req_orderno);	/** ԭ��ѯ���  **/
	lv_set_tag(&CMT302,"051",lv_date(hv_fzf->o_wt_date));	/** ԭί������  **/
	lv_set_tag(&CMT302,"CC1",hv_fzf->o_or_br_no);	/** ԭ�������к�  **/
	lv_set_tag(&CMT302,"CC2",hv_fzf->o_ac_br_no);	/** ԭ�������к�  **/
	lv_set_tag(&CMT302,"CE2",hv_fzf->otype);	/** ԭ��������  **/
	lv_set_tag(&CMT302,"005",hv_fzf->o_orderno);	/** ԭ֧���������  **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",hv_fzf->o_tx_amt*100);
	lv_set_tag(&CMT302,"CND",tmpBuf);				/** ԭ���ҷ��š����  **/
	lv_set_tag(&CMT302,"053",hv_fzf->res_content);/** �鸴����  **/

	lv_gen_pack(&CMT302);
	dm_show(&(CMT302.dm));

	/** ��notpay_in **/
	memcpy(notpay_in.cmtno,     "302",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->res_br_no,   	sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no,	sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->req_br_no,    	sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/* rem by LiuHuafeng 20060304
	memcpy(notpay_in.otxamt,	tmpBuf,					sizeof(tmpBuf)-1);
	**************************************/
	memcpy(notpay_in.otxamt,	tmpBuf,					sizeof(notpay_in.otxamt));
	/** opkgno **/
	memcpy(notpay_in.owtday,    lv_date(hv_fzf->req_date),8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** ���뷢�ͼ�¼ **/
	/***
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	lv_destroy(&CMT302);
	return(-1);
	}*******by liuyue 20060918 û���������******/
	ret=lv_insSendList(&CMT302);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		lv_destroy(&CMT302);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf->send_tel, lvca.tlrno, sizeof(hv_fzf->send_tel)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT302);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	if (iRc=iLvSendPackage(&CMT302))
	{
		lv_destroy(&CMT302);
		return(-1);
	}
	lv_destroy(&CMT302);

	return(0);

}
/** С��ҵ���˻�Ӧ���ķ��� **/
int lv_get_cmt320_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	char tmpBuf[1024];

	TLVPACKAGE CMT320;
	static  NOTPAY_IN_AREA  notpay_in;

	memset(tmpBuf,		'\0', sizeof(tmpBuf));
	memset(&notpay_in, 	'\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT320);
	/****NEWYX:����������ʶ****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '319' and lw_ind='2' ",  hv_fzf->or_br_no,  hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("�����α���� - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("δ�鵽��Ӧ�ļ�¼!");
		lv_destroy(&CMT320);
		return(-1);
	}
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1  )
	{
		vtcp_log("[%s][%d]-----stat=[%c][%c]\n",__FL__,hv_fzf->hv_fzf_sts[0],STAT_WZLR);
		vtcp_log("��¼״̬����!");
		lv_destroy(&CMT320);
		return(-1);
	}
	/** ��ʼƴ���� **/
	CMT320.opeType = '1';   /** С�� **/
	CMT320.pkgType = 0;     /** ��cmt�� **/
	memcpy(CMT320.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT320.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);
	memcpy(CMT320.CMTCode,"320",3);
	memcpy(CMT320.workDate,pcLvGetDay_work(),8);

	/** ��ʼ��� **/
	lv_set_tag(&CMT320,"30A",lv_date(hv_fzf->res_date));	/** ί������ **/
	lv_set_tag(&CMT320,"011",hv_fzf->cash_qs_no);	/** �˻�Ӧ�������� **/
	lv_set_tag(&CMT320,"52A",hv_fzf->res_br_no);	/** �˻�Ӧ���� **/
	lv_set_tag(&CMT320,"0BD",hv_fzf->res_orderno);	/**�˻�Ӧ����� **/
	lv_set_tag(&CMT320,"CJB",lv_date(hv_fzf->req_date));	/** ԭ�˻�����ί������ **/
	lv_set_tag(&CMT320,"012",hv_fzf->pay_qs_no	);	/** ԭ�˻����������� **/
	lv_set_tag(&CMT320,"58A",hv_fzf->or_br_no);	/** ԭ�˻�������  **/
	lv_set_tag(&CMT320,"0BE",hv_fzf->orderno);	/** ԭ�˻��������  **/
	vtcp_log("[%s][%d]hv_fzf.reqtype[0]===[%c]\n",__FILE__,__LINE__,hv_fzf->reqtype[0]);
	lv_set_tag(&CMT320,"BS8",hv_fzf->reqtype	);	/** ԭ�˻����� **/
	/**    lv_set_tag(&CMT320,"C15",hv_fzf->miya);	��Ѻ **/
	lv_set_tag(&CMT320,"018",hv_fzf->res_sts);	/** �˻�Ӧ�� **/
	lv_set_tag(&CMT320,"72A",hv_fzf->res_content);	/** �˻�Ӧ���� **/

	lv_gen_pack(&CMT320);
	dm_show(&(CMT320.dm));

	/** ��notpay_in
	memcpy(notpay_in.cmtno,     "320",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->res_orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);

	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,   lv_date( hv_fzf->o_wt_date),8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);
	**/

	/** ���뷢�ͼ�¼ **/
	/*********************************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,���뵥��������ʷ��ϸ����\n",__FL__);
	vtcp_log("���뵥��������ʷ��ϸ����");
	return(-1);
	}
	***********************************/
	ret=lv_insSendList(&CMT320);
	if (ret)
	{
		vtcp_log("%s,%d,����С��ķ��ͽ�����ʷ�Ǽǲ�����\n",__FL__);
		vtcp_log("����С��ķ��ͽ�����ʷ�Ǽǲ�����");
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("�����������Ʊ����");
		lv_destroy(&CMT320);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("�ر��α���� - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("����\n");
	if (iRc=iLvSendPackage(&CMT320))
	{
		lv_destroy(&CMT320);
		return(-1);
	}
	lv_destroy(&CMT320);
	return(0);

}
