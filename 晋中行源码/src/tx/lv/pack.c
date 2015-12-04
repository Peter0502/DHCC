/************************************************************
* �� �� ��:   lv_packcomms.c
* ����������  С��ҵ��֧��ƴ����������
*
* ��    ��:   chenhw
* ������ڣ�  2006-9-28 ���Ա������׸�����������
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if( ret ) {\
    strcpy( g_pub_tx.reply,"P026" );\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    return(-1);\
}

#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_lbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"

spJ070()
{
	int ret=0,count=0;
	int iPackid=0;
	int recid=0;
	int sql_cnt=0;
	int iHzdate;

	char cBrno[6];
	char cTbsdy[9];
	char cPackcd[2];
	char cPkgno[4];
	char cPackid[9];
	char cWtday[9];
	char cCashqsactno[13];
	char cOrdersel[2];
	char cOrderno1[9];
	char cOrderno2[9];
	char cPackend[2];
	char cHzdate[3];
	char sql_stat[2];

	struct lv_wbctl_c  	g_lv_wbctl;
	struct lv_pkgreg_c  	g_lv_pkgreg;

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_pkgreg ,0x00 ,sizeof(struct lv_pkgreg_c));

	memset(cBrno ,'\0',sizeof(cBrno));
	memset(cTbsdy ,'\0',sizeof(cTbsdy));
	memset(cPackcd,'\0',sizeof(cPackcd));
	memset(cPkgno,'\0',sizeof(cPkgno));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cWtday,'\0',sizeof(cWtday));
	memset(cCashqsactno,'\0',sizeof(cCashqsactno));
	memset(cOrdersel,'\0',sizeof(cOrdersel));
	memset(cOrderno1,'\0',sizeof(cOrderno1));
	memset(cOrderno2,'\0',sizeof(cOrderno2));
	memset(cPackend,'\0',sizeof(cPackend));
	memset(cHzdate,'\0',sizeof(cHzdate));
	memset(sql_stat,'\0',sizeof(sql_stat));

	vtcp_log("[%s][%d]spJ070С��ҵ��֧��ƴ������BEGIN",__FILE__,__LINE__);

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/** ȡ��FD���� **/
	get_fd_data( "0030", &cBrno);		/** ȡ�ý��׻��� **/
	get_fd_data( "0050", &cTbsdy);		/** ȡ�ý��׻��� **/
	get_fd_data( "0670", &cPackcd);		/**��׷�ӱ�־**/
	get_fd_data( "0230", &cPkgno);		/**���ı��**/
	get_fd_data( "0530", &cPackid);		/**�����**/
	get_fd_data( "0440", &cWtday);		/**����ί������**/
	get_fd_data( "0500", &cHzdate);		/**��ִ����**/
	get_fd_data( "0520", &cCashqsactno);	/**�����������к�**/
	get_fd_data( "0540", &cOrderno1);	/**��ʼ�������**/
	get_fd_data( "0550", &cOrderno2);	/**�����������**/
	get_fd_data( "0680", &cOrdersel);	/**֧��������ŷ�Χ**/
	get_fd_data( "0690", &cPackend);	/**��������־**/

	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cBrno,cPackcd,cPkgno,cPackid,cWtday,cHzdate,cCashqsactno,cOrderno1,cOrderno2,cOrdersel,cPackend);

	if ( cPackcd[0] == '1' )  /*  ����ӵ�ԭ�а����׽��м�� */
	{
		ret = Lv_wbctl_Sel( g_pub_tx.reply,&g_lv_wbctl,"packid = %s and pack_date = %s",cPackid,cWtday);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"С���������Ʊ��в����ڸü�¼");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P025");
			return (-1);
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			return (-1);
		}
		if (memcmp(cPkgno,g_lv_wbctl.pkgno,sizeof(cPkgno)-1)!=0)
		{
			sprintf(acErrMsg,"�������Ͳ�һ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P023");
			return (-1);
		}
		/***���״̬�Ƿ�Ϊ����ƴ��***/
		if (g_lv_wbctl.stat[0]!=PSTAT_PACKING)
		{
			sprintf(acErrMsg,"�������Ʊ�״̬��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P024");
			return (-1);
		}
	}

	/** ���Ǻʹ����˻�Ҫ�󸴺�״̬ ����Ϊ¼�� **/
	if (memcmp(cPkgno,PKGNO_PTDJ,sizeof(cPkgno)-1)==0||
		memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0||
		memcmp(cPkgno,PKGNO_DJTH,sizeof(cPkgno)-1)==0)
	{
		sql_stat[0]=STAT_WZCHECK2;
	}
	else
	{
		sql_stat[0]=STAT_WZLR;
	}
	if ( cOrdersel[0]=='0' )
	{
		/***��ŷ�ΧΪ����***/
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"pkgno='%.3s' and cash_qs_no='%.12s' and lv_sts='%s' and packid is null",
			cPkgno,cCashqsactno,sql_stat);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
	}
	else
	{
		/****ѡ����ŷ�Χ****/
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"pkgno='%.3s' and cash_qs_no='%.12s' and orderno>='%08ld' and orderno<='%08ld' and lv_sts='%s' and packid is null",
			cPkgno,cCashqsactno,atol(cOrderno1),atol(cOrderno2),sql_stat);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
	}

	while (1)
	{
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret = Lv_pkgreg_Fet_Upd(&g_lv_pkgreg,g_pub_tx.reply);
		recid++;
		vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
		if (ret!=0  && ret!=100)
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			vtcp_log("%s,%d here ",__FL__);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			return (-1);
		}
		if ( ret==100 )
		{
			if ( recid==1 )
			{
				sprintf(acErrMsg,"û�п�ƴ���Ľ���[%d]",ret);
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P027");
				return (-1);
			}
			vtcp_log("[%s][%d]spJ030ѭ����¼�������",__FILE__,__LINE__);
			break;
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		/** ��ӻ�ִ���޺����� **/
		if (memcmp(cPkgno,PKGNO_PTJJ,sizeof(cPkgno)-1)==0 || memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)
		{
			vtcp_log("[%s][%d]spJ030=========hzdate=[%s]",__FILE__,__LINE__,cHzdate);
			iHzdate = apatoi(cHzdate,1);
			vtcp_log("[%s][%d]spJ030=========hzdate=[%d]",__FILE__,__LINE__,iHzdate);
			ret=pub_base_deadlineD(g_lv_pkgreg.wt_date,iHzdate,&g_lv_pkgreg.resp_date);			
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			if ( ret!=0 )
			{
				sprintf(acErrMsg,"��ִ���޼��ʧ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P028");
				vtcp_log("%s,%d here ",__FL__);
				return (-1);
			}
			vtcp_log("[%s][%d]spJ030_-------___--cHzdate==[%s]---=====+++",__FILE__,__LINE__,cHzdate);

			ret=iCheckHzdate(cCashqsactno,iHzdate);
			if( ret!=0 )
			{
			sprintf(acErrMsg,"��ִ���޼��ʧ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P029");
			return (-1);
			}

			vtcp_log("[%s][%d]spJ030======lv_pkgreg.resp_date=[%ld]",__FILE__,__LINE__,g_lv_pkgreg.resp_date);
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		/** ��齻��״̬  **/
		if (memcmp(cPkgno,"002",3) ==0 || memcmp(cPkgno,"006",3)==0||memcmp(cPkgno,"012",3)==0)
		{
			if (g_lv_pkgreg.lv_sts[0]!=STAT_WZLR)
			{
				sprintf(acErrMsg,"֧������״̬����!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				return (-1);
			}
		}
		else
		{
			if (g_lv_pkgreg.lv_sts[0]!=STAT_WZCHECK2)
			{
				sprintf(acErrMsg,"֧������״̬����!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				return (-1);
			}
		}
		if (g_lv_pkgreg.packid[0]!='\0')
		{
			sprintf(acErrMsg,"֧�������Ѿ�ƴ����[%.8s]!",g_lv_pkgreg.packid);
			WRITEMSG
			vtcp_log("%s,%d here ",__FL__);
			strcpy(g_pub_tx.reply,"P031");
			return (-1);
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		if (cPackcd[0]=='1')
		{
			/****��ӵ�ԭ�а�**/
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
			ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
				"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,cPackid);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				return (-1);
			}
			vtcp_log("%s,%d here ",__FL__);
			ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				return (-1);
			}
			if(memcmp(cPkgno,PKGNO_TYXX,sizeof(cPkgno)-1)==0)
			if ( memcmp(g_lv_wbctl.txnum, g_lv_pkgreg.txnum , sizeof(g_lv_wbctl.txnum)-1) != 0)
			{
				sprintf(acErrMsg,"����¼ҵ������=[%s],��ҵ������=[%s]",g_lv_pkgreg.txnum,g_lv_wbctl.txnum);
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P031");
				return (-1);
			}
			if (g_lv_wbctl.stat[0] !=PSTAT_PACKING)
			{
				sprintf(acErrMsg,"֧������״̬����!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				return (-1);
			}
			vtcp_log("%s,%d dtlcnt=[%d]",__FL__,g_lv_wbctl.dtlcnt);
			g_lv_wbctl.dtlcnt +=1;
			g_lv_wbctl.totamt +=g_lv_pkgreg.tx_amt;

			memcpy(g_lv_pkgreg.packid,  g_lv_wbctl.packid,  sizeof(g_lv_pkgreg.packid)-1);
			g_lv_pkgreg.pack_date=g_lv_wbctl.pack_date;

			ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				return (-1);
			}
			vtcp_log("%s,%d here ",__FL__);
			Lv_wbctl_Clo_Upd();
			vtcp_log("%s,%d here ",__FL__);
			ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				return (-1);
			}
		}
		else
		{
			/****����һ���°�****/
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
			if (cPackcd[0]=='0')
			{
				/**�����ɰ�***/
				iPackid=iLvGetPackid();
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					return (-1);
				}
			}
			if (recid==1)
			{
				/****�����±��ĵ�һ����¼�������°�����,�Ժ�Ͳ���������*/
				vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);
				memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
				/** ׼���������ݣ��ȳ�ʼ�� **/
				g_lv_wbctl.pack_date= apatoi(pcLvGetDay(), 8);
				apitoa(iPackid,sizeof(g_lv_wbctl.packid)-1,g_lv_wbctl.packid);
				vtcp_log("[%s][%d]spJ030ipackid=[%s]",__FILE__,__LINE__,g_lv_wbctl.packid);
				memcpy(g_lv_wbctl.pkgno, g_lv_pkgreg.pkgno, sizeof(g_lv_wbctl.pkgno)-1);
				memcpy(g_lv_wbctl.pay_qs_no, g_lv_pkgreg.pay_qs_no, sizeof(g_lv_wbctl.pay_qs_no)-1);
				memcpy(g_lv_wbctl.cash_qs_no, g_lv_pkgreg.cash_qs_no, sizeof(g_lv_wbctl.cash_qs_no)-1);
				memcpy(g_lv_wbctl.txnum, g_lv_pkgreg.txnum, sizeof(g_lv_wbctl.txnum)-1);
				g_lv_wbctl.in_date= apatoi(cTbsdy, 8);
				memcpy(g_lv_wbctl.br_no, cBrno, sizeof(g_lv_wbctl.br_no)-1);
				g_lv_wbctl.dtlcnt=1;
				g_lv_wbctl.totamt=g_lv_pkgreg.tx_amt;
				g_lv_wbctl.bfflag[0]='0';
				g_lv_wbctl.stat[0]=PSTAT_PACKING;
				g_lv_wbctl.ffflag[0]='0';      /** Ͻ�ڷַ���־,��ʱ����0 **/
				memcpy(g_lv_wbctl.brprtflag,"000",sizeof(g_lv_wbctl.brprtflag)-1);
				memcpy(g_lv_wbctl.qsprtflag,"000",sizeof(g_lv_wbctl.qsprtflag)-1);
				vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

				ret = Lv_wbctl_Ins(g_lv_wbctl,g_pub_tx.reply);
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					return (-1);
				}
				vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);
				apitoa(iPackid,sizeof(g_lv_wbctl.packid)-1,g_lv_wbctl.packid);

				vtcp_log("[%s][%d]spJ030----cHzdate[%d]=[%s]---",__FILE__,__LINE__,g_lv_wbctl.pack_date,g_lv_wbctl.packid);
				memcpy(g_lv_pkgreg.packid,  g_lv_wbctl.packid,  sizeof(g_lv_pkgreg.packid)-1);
				g_lv_pkgreg.pack_date=g_lv_wbctl.pack_date;

				ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					return (-1);
				}
				vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

				/**************���Ϊ�����°�,��ʵֻ�е�һ����¼��Ҫ����
				******�Ժ�ļ�¼����׷��,���Ծ��޸�һ��tita�е�ֵ,ʹ��һ��
				******ѭ���ܹ��ߵ�׷�ӵķ�֧*****************************/
				cPackcd[0]='1';
				apitoa(iPackid,sizeof(cPackid)-1,cPackid);

			}
			else
			{
				/****��Ӧ���ߵ������֧****/
				sprintf(acErrMsg,"�����ɰ�ʱ����!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P031");
				return (-1);
			}
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	Lv_pkgreg_Clo_Upd();
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0)
	{
		/****���ڴ���ҵ��,����ֻ��һ��������****/
		char cTableBuf[513];
		sql_cnt=0;
		memset(cTableBuf, 0 , sizeof(cTableBuf));
		sprintf(cTableBuf,"(select count(*) from lv_pkgreg where pkgno='005' and pack_date=%d and packid='%s' group by wt_date,pay_ac_no)",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("[%s][%d]cTableBuf=[%s]",__FILE__,__LINE__,cTableBuf);
		sql_cnt=sql_count(cTableBuf,"1=1");
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
/**
		sql_cnt=sql_count("lv_pkgreg","pack_date=%d and packid='%.8s' group by pay_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		ret = sql_execute("select count(*) into sql_cnt from lv_pkgreg where pkgno='005' and pack_date=%d and packid='%s' group by wt_date,pay_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
**/
		
		vtcp_log("[%s][%d]sql_cnt=%d",__FILE__,__LINE__,sql_cnt);
		if ( sql_cnt!=1 )
		{
			sprintf(acErrMsg,"����ֻ��һ��������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P031");
			return (-1);
		}
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)
	{
		/****���ڽ��ҵ��,����ֻ��һ���տ���****/
		char cTableBuf[513];
		sql_cnt=0;
		memset(cTableBuf, 0 , sizeof(cTableBuf));
		sprintf(cTableBuf,"(select count(*) from lv_pkgreg where pkgno='006' and pack_date=%d and packid='%s' group by wt_date,pay_ac_no)",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("[%s][%d]cTableBuf=[%s]",__FILE__,__LINE__,cTableBuf);
		sql_cnt=sql_count(cTableBuf,"1=1");
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
/**
		sql_cnt=0;
		sql_cnt=sql_count("lv_pkgreg","pack_date=%d and packid='%s' group by wt_date,cash_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
**/
		if ( sql_cnt!=1 )
		{
			sprintf(acErrMsg,"����ֻ��һ���տ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P031");
			return (-1);
		}
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	if (cHzdate[0]!='0')	/** �����ִ���� **/
	{
		vtcp_log("[%s][%d]spJ030----cHzdate[0]=[%d]---",__FILE__,__LINE__,cHzdate[0]);
		vtcp_log("[%s][%d]spJ030----cHzdate[%d]=[%s]---",__FILE__,__LINE__,g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
			"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		g_lv_wbctl.hz_date=apatoi(cHzdate,2);
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		vtcp_log("[%s][%d]spJ030----g_lv_wbctl.stat[0]=[%c]---",__FILE__,__LINE__,g_lv_wbctl.stat[0]);
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
		Lv_wbctl_Clo_Upd();
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	/****�Ѱ���¼���****/
	if (cPackend[0]=='1')
	{
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
			"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			return (-1);
		}
		vtcp_log("%s,%d here ",__FL__);
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		g_lv_wbctl.stat[0]=PSTAT_PACKED;
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		vtcp_log("[%s][%d]spJ030----g_lv_wbctl.stat[0]=[%c]---",__FILE__,__LINE__,g_lv_wbctl.stat[0]);
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				return (-1);
			}
			Lv_wbctl_Clo_Upd();
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		return (-1);
	}
	set_zd_data("0530",g_lv_wbctl.packid);
	set_zd_long("0290",g_lv_wbctl.dtlcnt);
	set_zd_double("0330",g_lv_wbctl.totamt);

	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

}



spJ071()
{
	int ret;
	int iCnt;
	int iPackday;
	int iPackid;
	int recid=0;
	int iJzCnt=0;
	int g_reply_int=0;

	char cBrno[6];
	char cTbsdy[9];
	char cTlrno[5];
	char cOptype[2];
	char cOpayqsactno[13];
	char cOpkgno[4];
	char cOpackday[9];
	char cOpackid[9];
	char cPackid[9];
	char cOrderno[9];
	char sql_pkgno[4];
	char sql_stat[2];

	struct lv_wbctl_c 	g_lv_wbctl;
	struct lv_lbctl_c 	g_lv_lbctl;
	struct lv_pkgreg_c 	g_lv_pkgreg;

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_lbctl_c));
	memset(&g_lv_pkgreg ,0x00 ,sizeof(struct lv_pkgreg_c));

	memset(cBrno,'\0',sizeof(cBrno));
	memset(cTbsdy,'\0',sizeof(cTbsdy));
	memset(cTlrno,'\0',sizeof(cTlrno));
	memset(cOptype,'\0',sizeof(cOptype));
	memset(cOpayqsactno,'\0',sizeof(cOpayqsactno));
	memset(cOpkgno,'\0',sizeof(cOpkgno));
	memset(cOpackday,'\0',sizeof(cOpackday));
	memset(cOpackid,'\0',sizeof(cOpackid));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cOrderno,'\0',sizeof(cOrderno));
	memset(sql_pkgno,'\0',sizeof(sql_pkgno));
	memset(sql_stat,'\0',sizeof(sql_stat));

	vtcp_log("[%s][%d]spJ071С���ִҵ��ƴ������BEGIN",__FILE__,__LINE__);

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/** ȡ��FD���� **/
	get_fd_data( "0030", &cBrno);           /** ȡ�ý��׻��� **/
	get_fd_data( "0050", &cTbsdy);          /** ȡ�ý������� **/
	get_fd_data( "0070", &cTlrno);          /** ȡ�ù�Ա�� **/
	get_fd_data( "0670", &cOptype);		/****0-ƴ��,1-����****/
	get_fd_data( "0520", &cOpayqsactno);	/**ԭ���������к�**/
	get_fd_data( "0230", &cOpkgno);		/**���ı��**/
	get_fd_data( "0440", &cOpackday);	/**ԭ������**/
	get_fd_data( "0530", &cOpackid);	/**�����**/

	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cBrno,cOptype,cOpayqsactno,cOpkgno,cOpackday,cOpackid);

	iPackday=apatoi(pcLvGetDay(),8);    /** ���ƴ�����ڼ�ƴ����� **/
	iPackid=iLvGetPackid();
	sprintf( cPackid,"%08d",iPackid);

	memcpy(g_lv_lbctl.pay_qs_no,cOpayqsactno,sizeof(g_lv_lbctl.pay_qs_no));
	memcpy(g_lv_lbctl.packid,cOpackid,sizeof(g_lv_lbctl.packid));
	g_lv_lbctl.pack_date=apatoi(cOpackday,8);
	memcpy(g_lv_lbctl.pkgno,cOpkgno,sizeof(g_lv_lbctl.pkgno));

	ret = Lv_lbctl_Sel( g_pub_tx.reply,&g_lv_lbctl,"pack_date = '%s' and pay_qs_no = '%s' and packid = %s",cOpackday,cOpayqsactno,cOpackid);
	MYSQLERR

	vtcp_log("[%s][%d]spJ071[%s][%ld]--N",__FILE__,__LINE__,g_lv_lbctl.o_packid,g_lv_lbctl.o_pack_date);
	if(g_lv_lbctl.o_packid[0]!='\0'){
		sprintf(acErrMsg,"��ִ[%s]�Ѿ�ƴ������",g_lv_lbctl.o_packid);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P032");
		return (-1);
	}
	vtcp_log("[%s][%d]spJ071----------BEGIN",__FILE__,__LINE__);

	iCnt=0;

	if(memcmp(cOpkgno,PKGNO_PTJJ,sizeof(cOpkgno)-1)==0){
		memcpy(sql_pkgno,PKGNO_PTJJHZ,sizeof(sql_pkgno)-1);
	}else{
		memcpy(sql_pkgno,PKGNO_DQJJHZ,sizeof(sql_pkgno)-1);
	}

	iCnt=sql_count("lv_pkgreg","pkgno=%s and cash_qs_no=%s and o_pack_date=%08d and o_packid=%.8s and lv_sts<>'0'",sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.pack_date,g_lv_lbctl.packid);
	if(iCnt==-1){
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"SQLERR");
		WRITEMSG
		return (-1);
	}
	vtcp_log("[%s][%d]spJ071----[%d][%d]------",__FILE__,__LINE__,iCnt,g_lv_lbctl.dtlcnt);

	if(iCnt!=g_lv_lbctl.dtlcnt){
		sprintf(acErrMsg,"��ִ��¼��[%d]��������¼��[%ld]����!",iCnt,g_lv_lbctl.dtlcnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P033");
		return (-1);
	}
	vtcp_log("[%s][%d]spJ071----------",__FILE__,__LINE__);

	iCnt=0;
	vtcp_log("[%s][%d]spJ071----------",__FILE__,__LINE__);
	iCnt=sql_count("lv_pkgreg","pkgno=%s and cash_qs_no=%s and o_pack_date=%08d and o_packid=%.8s and lv_sts<>'3'",sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.pack_date,g_lv_lbctl.packid);
	if(iCnt==-1){
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"SQLERR");
		WRITEMSG
		return (-1);
	}
	vtcp_log("[%s][%d]spJ071----------BEGIN",__FILE__,__LINE__);

	if(iCnt!=0){
		sprintf(acErrMsg,"����[%d]��û�и��˵Ļ�ִ����!",iCnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P034");
		return (-1);
	}
	
	sql_stat[0]=STAT_WZCHECK2;

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
		"pkgno='%.3s' and cash_qs_no='%.12s' and o_packid =%s and o_pack_date='%08d' and lv_sts ='%s' ",
		sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.packid,g_lv_lbctl.pack_date,sql_stat);
	MYSQLERR

	while(1)
	{

		ret = Lv_pkgreg_Fet_Upd(&g_lv_pkgreg,g_pub_tx.reply);
		recid++;
		if(ret!=0  && ret!=100){
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			return (-1);
                }
		if( ret==100 ){
			if( recid==1 ){
				sprintf(acErrMsg,"û�п�ƴ���Ľ���[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P027");
				return (-1);
			}
			vtcp_log("[%s][%d]spJ030ѭ����¼�������",__FILE__,__LINE__);
			break;
		}

		if(memcmp(g_lv_pkgreg.packid,"        ",sizeof(g_lv_pkgreg.packid)-1)==0)
		{
			sprintf(acErrMsg,"��ִ[%.8s]�Ѿ�ƴ������");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P032");
			return (-1);
		}

		g_lv_pkgreg.pack_date=iPackday;
		memcpy(g_lv_pkgreg.packid,cPackid,sizeof(g_lv_pkgreg.packid)-1);
		
		ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
		MYSQLERR
		
		/** ͳ�Ƴɹ��������� **/
		if(memcmp(g_lv_pkgreg.rcpstat,"00",2)==0){
			g_lv_wbctl.dtlsucamt+=g_lv_pkgreg.tx_amt;
			g_lv_wbctl.dtlsuccnt++;
		}

	}
	Lv_pkgreg_Clo_Sel();

	/****���������¼****/

	g_lv_wbctl.pack_date = iPackday;
	memcpy(g_lv_wbctl.packid,cPackid,sizeof(g_lv_wbctl.packid)-1);
	vtcp_log("[%s][%d]spJ030ipackid=[%s]",__FILE__,__LINE__,g_lv_wbctl.packid);

	memcpy(g_lv_wbctl.pkgno, sql_pkgno, sizeof(g_lv_wbctl.pkgno)-1);
	memcpy(g_lv_wbctl.pay_qs_no, g_lv_pkgreg.pay_qs_no, sizeof(g_lv_wbctl.pay_qs_no)-1);
	memcpy(g_lv_wbctl.cash_qs_no, g_lv_pkgreg.cash_qs_no, sizeof(g_lv_wbctl.cash_qs_no)-1);
	memcpy(g_lv_wbctl.txnum, g_lv_pkgreg.txnum, sizeof(g_lv_wbctl.txnum)-1);
	g_lv_wbctl.in_date= apatoi(cTbsdy, 8);
	memcpy(g_lv_wbctl.br_no, cBrno, sizeof(g_lv_wbctl.br_no)-1);
	g_lv_wbctl.hz_date= apatoi(cTbsdy, 8);
	g_lv_wbctl.o_pack_date= g_lv_lbctl.pack_date;
	memcpy(g_lv_wbctl.o_packid, g_lv_lbctl.packid, sizeof(g_lv_wbctl.o_packid)-1);
	memcpy(g_lv_wbctl.o_pkgno, g_lv_lbctl.pkgno, sizeof(g_lv_wbctl.o_pkgno)-1);
	memcpy(g_lv_wbctl.ornode, g_lv_lbctl.ornode, sizeof(g_lv_wbctl.ornode)-1);
	memcpy(g_lv_wbctl.acnode, g_lv_lbctl.acnode, sizeof(g_lv_wbctl.acnode)-1);
	g_lv_wbctl.dtlcnt=g_lv_lbctl.dtlcnt;
	g_lv_wbctl.totamt=g_lv_lbctl.totamt;
	g_lv_wbctl.bfflag[0]='0';
	g_lv_wbctl.ffflag[0]='0';      /** Ͻ�ڷַ���־,��ʱ����0 **/
	memcpy(g_lv_wbctl.brprtflag,"000",sizeof(g_lv_wbctl.brprtflag)-1);
	memcpy(g_lv_wbctl.qsprtflag,"000",sizeof(g_lv_wbctl.qsprtflag)-1);
	g_lv_wbctl.stat[0]=PSTAT_PACKED;

	vtcp_log("[%s][%d]spJ030[%s][%d]",__FILE__,__LINE__,g_lv_wbctl.packid,g_lv_wbctl.pack_date);

	ret = Lv_wbctl_Ins(g_lv_wbctl,g_pub_tx.reply);
	MYSQLERR
	vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);



	/** ����������¼ **/

	ret=Lv_lbctl_Dec_Upd(g_pub_tx.reply,
		"pack_date = '%s' and pay_qs_no = '%s' and packid = %s",cOpackday,cOpayqsactno,cOpackid);
	MYSQLERR
	ret=Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
	MYSQLERR

	g_lv_lbctl.o_pack_date = iPackday;
	memcpy(g_lv_lbctl.o_packid, cPackid, sizeof(g_lv_lbctl.o_packid)-1);
	vtcp_log("[%s][%d]-----[%s][%d]---",__FILE__,__LINE__,g_lv_lbctl.o_packid,g_lv_lbctl.o_pack_date);

	ret=Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
	MYSQLERR
	Lv_lbctl_Clo_Upd();	

	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		return (-1);
	}

        set_zd_data("0540",g_lv_wbctl.packid);
        set_zd_long("0550",g_lv_wbctl.dtlsuccnt);
        set_zd_double("0330",g_lv_wbctl.dtlsucamt);


	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

}
