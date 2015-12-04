/******************************************************************
ģ������: ����ҵ��ƴ������
��������: ����ҵ��ƴ������
����ԭ��: spJ412 
�������:
�������:
ʹ��˵��:
�� д ��: SSH 2006.10.22
*******************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"

int spJ412()
{
	int iRc=0,recid=0;
	long dqdate;
	char cBrno[6];
	char packcd[2];			/**��׷�ӱ�־**/
	char pkgno[4];			/**���ı��**/
	char packid[9];			/**�����**/
	char wtday[9];			/**����ί������**/
	char operlevel[2];			/**���ȼ�**/
	char cashqsactno[13];		/**�����������к�**/
	char ordersel[2];			/**֧��������ŷ�Χ**/
	char orderno1[9];		/**��ʼ�������**/
	char orderno2[9];		/**�����������**/
	char packend[2];			/**��������־**/
	char hzdate[9];			/**��ִ����**/	
	char sql_orderno[9];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_stat[2];
	PAY_IN_AREA payin;
	struct	lv_wbctl_c lv_wbctl;
	struct	lv_pkgreg_c lv_pkgreg;

	memset(&lvca, 0 , sizeof(lvca));
	memset(cBrno ,'\0',sizeof(cBrno));
	memset(packcd, 	0, sizeof(packcd));
	memset(pkgno, 	0, sizeof(pkgno));
	memset(packid, 	0, sizeof(packid));
	memset(wtday, 	0, sizeof(wtday));
	memset(operlevel, 0, sizeof(operlevel));
	memset(cashqsactno, 0, sizeof(cashqsactno));
	memset(ordersel, 0, sizeof(ordersel));
	memset(orderno1, 0, sizeof(orderno1));
	memset(orderno2, 0, sizeof(orderno2));
	memset(packend, 0, sizeof(packend));
	memset(hzdate, 	'\0', sizeof(hzdate));
	memset(sql_orderno,0, sizeof(sql_orderno));
	memset(sql_orbrno, 0, sizeof(sql_orbrno));
	memset(sql_wtday,  0, sizeof(sql_wtday));
	memset(sql_stat,   0, sizeof(sql_stat));
	memset(&payin,0,sizeof(payin));
	memset(&lv_wbctl,'\0',sizeof(lv_wbctl));
	memset(&lv_pkgreg,'\0',sizeof(lv_pkgreg));

	pub_base_sysinit();
	if (iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
			goto ErrExit;
	}
	get_fd_data("0030", cBrno);       /** ȡ�ý��׻��� **/
	get_zd_data("0210",hzdate);
	get_zd_data("0230",pkgno);
	get_fd_data("0250",packid);
	get_zd_data("0370",cashqsactno);
	vtcp_log("[%s][%d]--------packid=[%s]\n",__FILE__,__LINE__,packid);
	get_zd_data("0450",wtday);
	get_zd_data("0460",orderno1);
	get_zd_data("0470",orderno2);
	get_zd_data("0670",packcd);
	get_zd_data("0680",operlevel);
	get_zd_data("0690",ordersel);
	get_zd_data("0700",packend);
	vtcp_log("[%s][%d]--------pkgno=[%s]\n",__FILE__,__LINE__,pkgno);

	lv_wbctl.pack_date=apatoi(wtday,8);

	memcpy(lv_wbctl.packid,packid,sizeof(lv_wbctl.packid)-1);
	if (packcd[0]=='1')
	{
		/**��ӵ�ԭ�а�**/
		/***********�ĳ���sel
		iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply," packid='%s' and pack_date = '%d' ",lv_wbctl.packid,lv_wbctl.pack_date);
		if(iRc){
		vtcp_log("%s,%d,����lv_wbctl��[%d]\n",__FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"�����������Ʊ��");
		strcpy( g_pub_tx.reply,"P026" );
		WRITEMSG
		goto ErrExit;
		}

		iRc=Lv_wbctl_Fet_Upd(&lv_wbctl,g_pub_tx.reply);
		***/

		iRc=Lv_wbctl_Sel(g_pub_tx.reply,&lv_wbctl," packid='%s' and pack_date = '%d' ",lv_wbctl.packid,lv_wbctl.pack_date);
		if (iRc)
		{
			vtcp_log("%s,%d,fetch lv_wbctl��[%d]\n",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"ȡ���������Ʊ����ݴ�");
			strcpy( g_pub_tx.reply,"P026" );
			WRITEMSG
				goto ErrExit;
		}
		if (memcmp(pkgno,lv_wbctl.pkgno,sizeof(pkgno))!=0)
		{
			vtcp_log("[%s],[%d],�������Ͳ�һ��pkgno==[%s]lv_wbctl.pkgno=[%s]!!!\n",__FILE__,__LINE__,pkgno,lv_wbctl.pkgno);
			sprintf(acErrMsg,"�������Ͳ�һ��");
			strcpy( g_pub_tx.reply,"P023" );
			WRITEMSG
				goto ErrExit;

		}
		/***���״̬�Ƿ�Ϊ����ƴ��***/
		if (lv_wbctl.stat[0]!=PSTAT_PACKING)
		{
			vtcp_log("%s,%d,lv_wbctl״̬[%c]��\n",__FILE__,__LINE__,lv_wbctl.stat[0]);
			sprintf(acErrMsg,"�������Ʊ�״̬��");
			strcpy( g_pub_tx.reply,"P039" );
			WRITEMSG
				goto ErrExit;
		}
	}
	vtcp_log("[%s][%d]--------pkgno=[%s]\n",__FILE__,__LINE__,pkgno);
	if (memcmp(pkgno,PKGNO_PTJJHZ,sizeof(pkgno))==0)
	{
		sql_stat[0]=STAT_WZCHECK2;
	}else
	{
		sql_stat[0]=STAT_WZLR;
	} 
	vtcp_log("[%s][%d]--------sql_stat=[%s]\n",__FILE__,__LINE__,sql_stat);

	if (ordersel[0]=='0')
	{
		vtcp_log("%s,%d here ��ŷ�ΧΪ����",__FL__);
		iRc = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"in_date=%d and cash_qs_no='%.12s' and lv_sts='%s' and br_no ='%s'  and packid is null",
			lv_wbctl.pack_date,cashqsactno,sql_stat,g_pub_tx.tx_br_no);
		if ( iRc )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,iRc);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"���ݿ���Ҵ���");
			WRITEMSG
				goto ErrExit;
		}
	}else
	{
		/****ѡ����ŷ�Χ****/
		vtcp_log("%s,%d here ѡ����ŷ�Χ",__FL__);

		/**
		if(memcmp(pkgno ,"PKGNO_PTJJ", 3) ==0){	
		**/
		vtcp_log("[%s][%d]========��ͨ���\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"in_date=%d and cash_qs_no='%.12s' and orderno>='%08ld' and orderno<='%08ld' and lv_sts='%s' and br_no = '%s' and txnum in(30102,30103,30104,30105) and pkgno='%s' and packid is null  ", lv_wbctl.pack_date,cashqsactno,atol(orderno1),atol(orderno2),sql_stat,cBrno,pkgno);
		vtcp_log("%s,%d here ",__FL__);
		if ( iRc )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,iRc);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"���ݿ���Ҵ���");
			WRITEMSG
				goto ErrExit;
		}
		/****************
		}
		else if(memcmp(pkgno ,"PKGNO_PTJJHZ", 3) ==0){
		vtcp_log("[%s][%d]========��ͨ��ǻ�ִ\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
		"in_date=%d and pay_qs_no='%.12s' and orderno>='%08ld' and orderno<='%08ld' and lv_sts='%s' and br_no = '%s' and txnum in(30102,30103,30104,30105) and pkgno='%s' and packid is null  ", lv_wbctl.pack_date,cashqsactno,atol(orderno1),atol(orderno2),sql_stat,cBrno,pkgno);
		vtcp_log("%s,%d here ",__FL__);
		if( iRc )
		{
		vtcp_log("%s,%d here ret=%d",__FL__,iRc);
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"���ݿ���Ҵ���");
		WRITEMSG
		goto ErrExit;
		}

		}
		*****************/

		vtcp_log("%s,%d �α궨�����!here ",__FL__);
	}
	while (1)
	{
		if (memcmp(pkgno,PKGNO_TYXX,sizeof(pkgno))!=0)
		{
			vtcp_log("%s,%d here ",__FL__);
			memset(&payin,'\0',sizeof(payin));
			iRc = Lv_pkgreg_Fet_Upd(&lv_pkgreg,g_pub_tx.reply);
			recid++;
			if (iRc!=0 && iRc!=100)
			{
				vtcp_log("%s,%d,���ݿ����iRc=%d\n",__FILE__,__LINE__,iRc);
				sprintf(acErrMsg,"�������ݿ����");
				strcpy( g_pub_tx.reply,"P026" );
				WRITEMSG
				goto ErrExit;
			}
			if (iRc==100)
			{
				if (recid==1)
				{
					vtcp_log("%s,%d,���ݿ����sqlcode=%d\n",__FILE__,__LINE__,iRc);
					sprintf(acErrMsg,"�������ݿ����");
					strcpy( g_pub_tx.reply,"P026" );
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("%s,%d,��¼ѭ���������\n",__FILE__,__LINE__);
				break;
			}
			vtcp_log("[%s][%d]===========lv_pkgreg.lw_ind=[%s]\n",__FILE__,__LINE__,lv_pkgreg.lw_ind);
			vtcp_log("[%s][%d]===========payin.F_lv_ind[0]=[%c]\n",__FILE__,__LINE__,payin.F_lw_ind[0]);
			vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
			memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
			memcpy(payin.F_pkgno,pkgno,sizeof(payin.F_pkgno));
			apitoa(lv_pkgreg.wt_date,sizeof(payin.F_wtday),payin.F_wtday);
			memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
			memcpy(payin.F_txnum,lv_pkgreg.txnum,sizeof(payin.F_txnum));
			iLvPayinAutoInfo(&payin);
			vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
			vtcp_log("[%s][%d]===========lv_pkgreg.lw_ind=[%s]\n",__FILE__,__LINE__,lv_pkgreg.lw_ind);
			memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
			payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
			/***iLvFetRecToPayin����һ�������ж�***/
			if (memcmp(payin.F_pkgno,PKGNO_PTJJHZ,sizeof(payin.F_pkgno))==0)
			{
				memset(payin.F_txnum,'\0',sizeof(payin.F_txnum));
			}
			iRc=iLvFetchRecToPayin(&payin);
			if (iRc)
			{
				vtcp_log("%s,%d,δ�ҵ�֧����¼\n",__FILE__,__LINE__);
				sprintf(acErrMsg,"δ�ҵ�֧����¼");
				strcpy( g_pub_tx.reply,"P026" );
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
			iLvResetPayin(&payin);
			vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
			vtcp_log("%s,%d ==========payin.T_txamt=[%.16s]\n ",__FL__,payin.T_txamt);
			vtcp_log("%s,%d ==========payin.F_txamt=[%.16s]\n ",__FL__,payin.F_txamt);
			/** ��ӻ�ִ���޺����� **/
			if (memcmp(pkgno,PKGNO_PTJJ,sizeof(pkgno))==0)
			{
				vtcp_log("[%s][%d]=========hzdate=[%s]\n",__FILE__,__LINE__,hzdate);
				iRc=pub_base_deadlineD(apatoi(payin.F_wtday,8),apatoi(hzdate,2),&dqdate);
				if ( iRc )
				{
					vtcp_log("%s,%d,���ڴ���\n",__FILE__,__LINE__);
					strcpy( g_pub_tx.reply,"P082" );
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("[%s][%d]========������=dqdate=[%ld]\n",__FILE__,__LINE__,dqdate);
				iRc=iCheckHzdate(cashqsactno,apatoi(hzdate,2));
				if (iRc!=0)
				{
					vtcp_log("%s,%d,\n",__FILE__,__LINE__);
					strcpy( g_pub_tx.reply,"P016" );
					WRITEMSG
					goto ErrExit;
				}
				apitoa(dqdate,sizeof(payin.F_respday),payin.F_respday);
				vtcp_log("[%s][%d]=========payin.F_respday=[%s]\n",__FILE__,__LINE__,payin.F_respday);
			}
			if (memcmp(pkgno,"002",3) ==0 || memcmp(pkgno,"006",3)==0)
			{
				if (payin.T_stat[0]!=STAT_WZLR)
				{
					vtcp_log("%s,%d,[%.8s]֧������״̬[%c]����\n",__FILE__,__LINE__,payin.F_orderno,payin.T_stat[0]);
					sprintf(acErrMsg,"֧������״̬����");
					strcpy( g_pub_tx.reply,"P016" );
					WRITEMSG
					goto ErrExit;
				}
			}
			else
			{
				if (payin.T_stat[0]!=STAT_WZCHECK2)
				{
					vtcp_log("%s,%d,[%.8s]֧������״̬[%c]����\n",__FILE__,__LINE__,payin.F_orderno,payin.T_stat[0]);
					sprintf(acErrMsg,"֧������״̬����");
					strcpy( g_pub_tx.reply,"P016" );
					WRITEMSG
					goto ErrExit;
				}
			}
			if (payin.T_packid[0]!='\0')
			{
				vtcp_log("%s,%d,[%.8s]֧�������Ѿ�ƴ����[%.8s]\n",
					__FILE__,__LINE__,payin.F_orderno,payin.T_packid);
				sprintf(acErrMsg,"֧�������Ѿ�ƴ������");
				strcpy( g_pub_tx.reply,"P032" );
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d]---------------packcd=[%s]\n",__FILE__,__LINE__,packcd);
			vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
			if (packcd[0]=='1')
			{
				/****��ӵ�ԭ�а�**/
				vtcp_log("%s,%d ��ӵ�ԭ�а�============\n ",__FL__);
				memcpy(payin.T_packday, payin.A_tbsdy, 	sizeof(payin.T_packday));
				memcpy(payin.T_packid,  packid, 	sizeof(payin.T_packid));
				memcpy(payin.F_packday, payin.A_tbsdy, 	sizeof(payin.F_packday));
				memcpy(payin.F_packid,  packid, 	sizeof(payin.F_packid));

				vtcp_log("[%s][%d]----------wbctl.pay_qs_no=[%s]\n", __FILE__,__LINE__,lv_wbctl.pay_qs_no);
				vtcp_log("%s,%d ==========payin.T_txamt=[%.16s]\n ",__FL__,payin.T_txamt);
				vtcp_log("%s,%d ==========payin.F_txamt=[%.16s]\n ",__FL__,payin.F_txamt);
				iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
				if (iRc!=PCHK_UPDATE)
				{	/** PCHK_UPDATE==2 **/
					vtcp_log("%s,%d,��ӵ�ԭ�а�ʱ[%d]����\n",
						__FILE__,__LINE__,iRc);
					sprintf(acErrMsg,"����֧�����׵Ǽǲ���");
					strcpy( g_pub_tx.reply,"P058" );
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("[%s][%d]----------wbctl.pay_qs_no=[%s]\n", __FILE__,__LINE__,lv_wbctl.pay_qs_no);

				memcpy(lv_pkgreg.packid,  lv_wbctl.packid,  sizeof(lv_pkgreg.packid)-1);
				lv_pkgreg.pack_date=lv_wbctl.pack_date;
				iRc = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
				vtcp_log("%s,%d here ",__FL__);
				if ( iRc )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,iRc);
					strcpy( g_pub_tx.reply,"P058" );
					sprintf(acErrMsg,"SQLERR [%d]",iRc);
					WRITEMSG
					goto ErrExit;
				}
			}
			else
			{
				/****����һ���°�****/
				vtcp_log("%s,%d ����һ���°�===========recid=[%d]\n ",__FL__,recid);
				if (recid==1)
				{
					vtcp_log("%s,%d ==========payin.T_o_packid=[%s]\n ",__FL__,payin.T_o_packid);
					/****�����±��ĵ�һ����¼�������°�����,�Ժ�Ͳ���������*/
					vtcp_log("[%s][%d]----------wbctl.pay_qs_no=[%s]\n", __FILE__,__LINE__,lv_wbctl.pay_qs_no);
					iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
					if (iRc!=PCHK_NEW)
					{
						vtcp_log("%s,%d,�����ɰ�ʱ[%d]����\n",
							__FILE__,__LINE__,iRc);
						sprintf(acErrMsg,"����֧�����׵Ǽǲ���");
						strcpy( g_pub_tx.reply,"P058" );
						WRITEMSG
						goto ErrExit;
					}
					vtcp_log("[%s][%d]----------wbctl.pay_qs_no=[%s]\n", __FILE__,__LINE__,lv_wbctl.pay_qs_no);
					memcpy(lv_pkgreg.packid,  lv_wbctl.packid,  sizeof(lv_pkgreg.packid)-1);
					lv_pkgreg.pack_date=lv_wbctl.pack_date;

					iRc = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
					vtcp_log("%s,%d here ",__FL__);
					if ( iRc )
					{
						vtcp_log("%s,%d here ret=%d",__FL__,iRc);
						strcpy( g_pub_tx.reply,"P058" );
						sprintf(acErrMsg,"SQLERR [%d]",iRc);
						WRITEMSG
						goto ErrExit;
					}
					/************* ���������ã�����
					iRc=iLvUpdRecFromPayin(&payin,0);
					if(iRc){
					vtcp_log("%s,%d,����֧�����׵Ǽǲ���[%d]\n",
					__FILE__,__LINE__,iRc);
					sprintf(acErrMsg,"����֧�����׵Ǽǲ���");
					WRITEMSG
					goto ErrExit;
					}
					**************/
					/****CAUTION:������������Ͱ���������Ҫ����һЩ����ֵ****/

					/********************************************************/
					/**************���Ϊ�����°�,��ʵֻ�е�һ����¼��Ҫ����
					******�Ժ�ļ�¼����׷��,���Ծ��޸�һ��tita�е�ֵ,ʹ��һ��
					******ѭ���ܹ��ߵ�׷�ӵķ�֧*****************************/
					packcd[0]='1';
					memcpy(packid,payin.F_packid,sizeof(packid));

					vtcp_log("%s,%d ===========,packcd=[%s]\n ",__FL__,packcd);

				}
				else
				{
					/****��Ӧ���ߵ������֧****/
					vtcp_log("%s,%d,�����֧\n",__FILE__,__LINE__);
					sprintf(acErrMsg,"�����֧");
					strcpy( g_pub_tx.reply,"P015" );
					WRITEMSG
					goto ErrExit;
				}	
			}
		}
	}
	Lv_pkgreg_Clo_Upd();

	vtcp_log("%s,%d here ",__FL__);
	vtcp_log("[%s][%d]----------wbctl.pack_date=[%d],wbctl.packid=[%s]\n",
		__FILE__,__LINE__,lv_wbctl.pack_date,lv_wbctl.packid);
	vtcp_log("[%s][%d]----------wbctl.o_pack_date=[%d],wbctl.o_packid=[%s]\n",
		__FILE__,__LINE__,lv_wbctl.o_pack_date,lv_wbctl.o_packid);
	if (hzdate[0]!='\0')	/** �����ִ���� **/
	{
		iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date='%d' and packid=%s",lv_wbctl.pack_date,lv_wbctl.packid);
		if ( iRc )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,iRc);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",iRc);
			WRITEMSG
				goto ErrExit;
		}
		vtcp_log("%s,%d here ",__FL__);
		iRc=Lv_wbctl_Fet_Upd(&lv_wbctl,g_pub_tx.reply);
		if ( iRc )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,iRc);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",iRc);
			WRITEMSG
				goto ErrExit;
		}
		vtcp_log("[%s][%d]----------wbctl.pay_qs_no=[%s]\n", __FILE__,__LINE__,lv_wbctl.pay_qs_no);
		lv_wbctl.hz_date=apatoi(hzdate,2);
		iRc=Lv_wbctl_Upd_Upd(lv_wbctl,g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,�����������Ʊ�ʧ��[%d]\n",__FILE__,__LINE__, iRc);
			sprintf(acErrMsg,"�����������Ʊ�ʧ��");
			strcpy( g_pub_tx.reply,"P058" );
			WRITEMSG
				goto ErrExit;
		}
		Lv_wbctl_Clo_Upd();
	}
	if (packend[0]=='1')
	{
		/****�Ѱ���¼���****/
		iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply,"pack_date=%ld and packid='%s'",lv_wbctl.pack_date,lv_wbctl.packid);
		if (iRc)
		{
			strcpy(g_pub_tx.reply,"HV01");
			vtcp_log("[%s]][%d],[%d]\n",__FL__,iRc);
			goto ErrExit;
		}
		iRc=Lv_wbctl_Fet_Upd(&lv_wbctl,g_pub_tx.reply);	
		if (iRc)
		{
			strcpy(g_pub_tx.reply,"HV01");
			vtcp_log("[%s]][%d],[%d]\n",__FL__,iRc);
			goto ErrExit;
		}
		vtcp_log("[%s][%d]---------lv_wbctl.hz_date=[%d]\n",__FILE__,__LINE__,lv_wbctl.hz_date);
		lv_wbctl.stat[0]=PSTAT_PACKED;
		vtcp_log("[%s][%d]=================stat=[%c]\n",__FILE__,__LINE__,lv_wbctl.stat[0]);
		vtcp_log("[%s][%d]=================lv_wbctl.packid=[%s]\n",__FILE__,__LINE__,lv_wbctl.packid);
		vtcp_log("[%s][%d]=================lv_wbctl.packday=[%d]\n",__FILE__,__LINE__,lv_wbctl.pack_date);

		Lv_wbctl_Debug(&lv_wbctl);	
		iRc=Lv_wbctl_Upd_Upd(lv_wbctl,g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,�����������Ʊ�ʧ��[%d]\n",__FILE__,__LINE__, iRc);
			sprintf(acErrMsg,"�����������Ʊ�ʧ��");
			strcpy( g_pub_tx.reply,"P058" );
			WRITEMSG
				goto ErrExit;
		}
		vtcp_log("[%s][%d]================iRc[%d]=stat=[%s]\n",__FILE__,__LINE__,iRc,lv_wbctl.stat);
	}
	Lv_wbctl_Clo_Upd();

	set_zd_long("0280",lv_wbctl.pack_date);
	set_zd_data("0290",lv_wbctl.packid);
	set_zd_long("0340",lv_wbctl.dtlcnt);
	set_zd_double("0400",lv_wbctl.totamt);

	OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
	ErrExit:
		sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
