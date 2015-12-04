/************************************************************
* �� �� ��:   spJ079.c
* ����������  С����ϸ���ʽ���
*
* ��    ��:   chenhw
* ������ڣ�  2006-9-11
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
 goto ErrExit;\
 }

#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_lbctl_c.h"
#include "lv_chk851_d_c.h"
#include "lv_define.h"

static struct lv_chk851_d_c g_lv_chk851_d;

spJ079()
{
	int ret;
	int flg;
	struct lv_lbctl_c g_lv_lbctl;
	struct lv_wbctl_c g_lv_wbctl;
	int iCntccpc=0;
	int iCntbank=0;
	char cTxday[9];
	memset(cTxday, 0 , sizeof(cTxday));
	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_chk851_d,0x00, sizeof(g_lv_chk851_d));

	vtcp_log("[%s][%d]spJ079С����ϸ���ʽ���BEGIN",__FILE__,__LINE__);

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	get_zd_data(DC_DATE,cTxday);
	/* ��ʼ���254���Ľ������ */
	iCntccpc=sql_count("lv_c254r","qs_date=%d and zc_amt<>0",atoi(cTxday));
	if(iCntccpc<0)
	{
		vtcp_log("[%s][%d] ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntccpc);
		WRITEMSG
		strncpy(acErrMsg, "��254���㱨�ı����!", 40);
		set_zd_data("0130",acErrMsg);
		memcpy(g_pub_tx.reply,"P083",4);
		goto ErrExit;
	}
	iCntbank=sql_count("(select count(*)  from lv_chk842_d  group by zc_jd_lx,zc_date,zc_cc having sum(decode(pkgno,'009',-lb_tx_amt,lb_tx_amt)) <>sum(decode(pkgno,'009',-wb_tx_amt,wb_tx_amt)))","1=1");
	if(iCntbank<0)
	{
		vtcp_log("[%s][%d] ����842ͳ��254���Ĵ��� sqlcode=[%d]\n",__FILE__,__LINE__,iCntbank);
		WRITEMSG
		strncpy(acErrMsg, "��842���ı�ͳ��254��������!", 40);
		set_zd_data("0130",acErrMsg);
		memcpy(g_pub_tx.reply,"P083",4);
		goto ErrExit;
	}
	if(iCntbank!=iCntccpc)
	{
		sprintf(acErrMsg,"����û���յ�254����������յ�%d�ʣ�Ӧ��%d��",iCntccpc,iCntbank);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		memcpy(g_pub_tx.reply,"P083",4);
		goto ErrExit;
	}

	ret = Lv_chk851_d_Dec_Upd(g_pub_tx.reply,"1=1 order by pack_sts");
	MYSQLERR

	flg = 0;
	while(1)
	{
		ret = Lv_chk851_d_Fet_Upd(&g_lv_chk851_d,g_pub_tx.reply);
		if( ret==100 )
		{
			if( flg == 0) /* 851N4JU5=  */
			{
    			ret = sql_count("lv_chk842_d","1=1");
				if(ret)
				{
						vtcp_log("[%s][%d] ;9C;JU5=8511(ND#,2;D\6TCwO8 ret===[%d]\n",__FILE__,__LINE__,ret);
						WRITEMSG
						strncpy(acErrMsg, ";9C;JU5=8511(ND#,2;D\6TCwO8!", 40);
						set_zd_data("0130",acErrMsg);
						memcpy(g_pub_tx.reply,"P083",4);
						goto ErrExit;
				}
			}
			break;
		}
		if( ret!=0 )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			memcpy(g_pub_tx.reply,"P083",4);
			goto ErrExit;
		}

		flg ++;

		/* �ȼ������ */
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		if(g_lv_chk851_d.lw_flg[0]=='1')/* send */
		{
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_wbctl_Dec_Upd(g_pub_tx.reply,
				"pack_date=%d and packid='%s'",g_lv_chk851_d.pack_date,g_lv_chk851_d.packid);
			if(ret)
			{
				vtcp_log("%s,%d ���������α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				sprintf(acErrMsg,"����%s�쳣����,����Ƽ���ϵ",g_lv_chk851_d.packid);
				set_zd_data("0130",acErrMsg);
				goto ErrExit;
			}
			vtcp_log("%s,%dstat==[%c] ",__FILE__,__LINE__,g_lv_wbctl.stat[0]);
			/* begin add by LiuHuafeng 2007-11-13 16:45:50 NEWTCTD8 begin ʵʱҵ��Ҫ���⴦�� */
			if(memcmp(g_lv_chk851_d.pkgno,"009",sizeof(g_lv_chk851_d.pkgno)-1)==0
				 && memcmp(g_lv_wbctl.packstat,"03",sizeof(g_lv_wbctl.packstat)-1)!=0 
				 && memcmp(g_lv_wbctl.packstat,"05",sizeof(g_lv_wbctl.packstat)-1)!=0 
				 )
			{
				Lv_wbctl_Debug(&g_lv_wbctl);
				vtcp_log("[%s][%d] ����ʵʱ����ҵ��δ�յ�����920�������\n",__FILE__,__LINE__);
				strncpy(acErrMsg, "ʵ����ִδ�յ�920��˳�������쳣����", 40);
				strncpy(g_pub_tx.reply,"P085",4);
				WRITEMSG
				goto ErrExit;
			}
			if(memcmp(g_lv_chk851_d.pkgno,"010",sizeof(g_lv_chk851_d.pkgno)-1)==0
				 && memcmp(g_lv_wbctl.packstat,"03",sizeof(g_lv_wbctl.packstat)-1)!=0 
				 && memcmp(g_lv_wbctl.packstat,"05",sizeof(g_lv_wbctl.packstat)-1)!=0 
				 )
			{
				Lv_wbctl_Debug(&g_lv_wbctl);
				vtcp_log("[%s][%d] ����ʵʱ���ҵ��δ�յ�����920�������\n",__FILE__,__LINE__);
				strncpy(acErrMsg, "ʵ���ִδ�յ�920��˳�������쳣����", 40);
				WRITEMSG
				strncpy(g_pub_tx.reply,"P085",4);
				goto ErrExit;
			}
			g_lv_wbctl.qs_date=g_lv_chk851_d.qs_date;
			/* begin add by LiuHuafeng 2009-5-17 18:27:49 for ��������*/
			strncpy(g_lv_wbctl.packstat,PACKSTAT_QS,sizeof(g_lv_wbctl.packstat)-1);
			/* end by LiuHuafeng 2009-5-17 18:27:55 for �������� */
			memcpy(g_lv_wbctl.filler,g_lv_wbctl.packstat,sizeof(g_lv_wbctl.packstat)-1);
			/* NEWTCTD8 end by LiuHuafeng 2007-11-13 16:54:00 */
			
			if(g_lv_wbctl.stat[0]=='4'){
				vtcp_log("%s,%d ",__FILE__,__LINE__);
				memcpy(g_lv_wbctl.packstat,g_lv_chk851_d.pack_sts,sizeof(g_lv_wbctl.packstat)-1);
				/*g_lv_wbctl.dtlsucamt=g_lv_chk851_d.tot_amt;
				g_lv_wbctl.dtlsuccnt=g_lv_chk851_d.dtlcnt;*/
				memcpy(g_lv_wbctl.zcid,g_lv_chk851_d.zc_cc,sizeof(g_lv_wbctl.zcid)-1);
				g_lv_wbctl.zcnode[0]=g_lv_chk851_d.zc_jd_lx[0];
				g_lv_wbctl.zc_date=g_lv_chk851_d.zc_date;
				g_lv_wbctl.qs_date=g_lv_chk851_d.qs_date;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ���������α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				goto ErrExit;
			}
			Lv_wbctl_Clo_Upd();
		}
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		if(g_lv_chk851_d.lw_flg[0]=='2')/* receive */
		{
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_lbctl_Dec_Upd(g_pub_tx.reply,
				"pack_date=%d and packid='%s' and pay_qs_no='%s'",g_lv_chk851_d.pack_date,g_lv_chk851_d.packid,g_lv_chk851_d.pay_qs_no);
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret = Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				sprintf(acErrMsg,"����%sû�������յ�����Ƽ���ϵ",g_lv_chk851_d.packid);
				set_zd_data("0130",acErrMsg);
				goto ErrExit;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			memcpy(g_lv_lbctl.packstat,g_lv_chk851_d.pack_sts,sizeof(g_lv_lbctl.packstat)-1);
			/*g_lv_lbctl.dtlsucamt=g_lv_chk851_d.tot_amt;
			g_lv_lbctl.dtlsuccnt=g_lv_chk851_d.dtlcnt;*/
			memcpy(g_lv_lbctl.zcid,g_lv_chk851_d.zc_cc,sizeof(g_lv_wbctl.zcid)-1);
			g_lv_lbctl.zcnode[0]=g_lv_chk851_d.zc_jd_lx[0];
			g_lv_lbctl.zc_date=g_lv_chk851_d.zc_date;
			g_lv_lbctl.qs_date=g_lv_chk851_d.qs_date;
			ret = Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
			if(ret)
			{
				Lv_lbctl_Debug(&g_lv_lbctl);
				vtcp_log("%s,%d ������������� ret=%d",__FILE__,__LINE__,ret);
				WRITEMSG
				goto ErrExit;
			}

			Lv_wbctl_Clo_Upd();
		}
		
		memcpy(g_lv_chk851_d.dhcc_sts,g_lv_chk851_d.pack_sts,sizeof(g_lv_chk851_d.dhcc_sts));
		ret = Lv_chk851_d_Upd_Upd(g_lv_chk851_d,g_pub_tx.reply);
		if(ret)
		{
			Lv_wbctl_Debug(&g_lv_lbctl);
			vtcp_log("%s,%d ����851�α���� ret=%d",__FILE__,__LINE__,ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	Lv_chk851_d_Clo_Sel();
	
	/**add by ChengGX 20091231 beg ���Ӷ����յ�(254)���㱨��,���յ�(920)����ĵĽ��׵Ĵ���,����״̬lv_lbctl����qs_dateΪ�� packstatΪ03�ļ�¼**/
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_lbctl_c));
	ret = Lv_lbctl_Dec_Upd(g_pub_tx.reply,"in_date<=%d and pack_date<=%d and zc_date<=%d and( pkgno='001' or pkgno='007') and qs_date=0 and packstat='03'",atoi(cTxday),atoi(cTxday),atoi(cTxday));
	/*��ѯ�α�Ķ���Ҫ����ʵ�ʵĻ�������Ȫ��lv_lbctl��������û�з������������ļ�¼
	��׼ȷ���ж�Ҫ���ݵ�������һ���ж�in_date�����塢���������ա���һ���ܶ�������֮���Ҫ�ж���ǰһ����졣
	*/
	if(ret)
	{
		vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	while(1)
	{
	    vtcp_log("%s,%d ChengGX ����Ҫ��������յ�(254)���㱨��,���յ�(920)����ĵĽ���",__FILE__,__LINE__);
		ret = Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
		if(ret==100)
		{
			break;
		}
		if(ret)
		{
			vtcp_log("%s,%d ��ȡ�����α���� ret=%d",__FILE__,__LINE__,ret);
			WRITEMSG
			sprintf(acErrMsg,"����%sû�������յ�����Ƽ���ϵ",g_lv_chk851_d.packid);
			set_zd_data("0130",acErrMsg);
			goto ErrExit;
		}
		vtcp_log("%s,%d ChengGX����ĩ�����ʣ���Ҫ����lv_lbctl����pack_dateΪ�� packstatΪ03 �ļ�¼",__FILE__,__LINE__);
		g_lv_lbctl.qs_date=atoi(cTxday);
		strncpy(g_lv_lbctl.packstat,PACKSTAT_QS,sizeof(g_lv_lbctl.packstat)-1);
		ret = Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
		if(ret)
		{
			Lv_lbctl_Debug(&g_lv_lbctl);
			vtcp_log("%s,%d ������������� ret=%d",__FILE__,__LINE__,ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	Lv_lbctl_Clo_Upd();
	/**end by ChengGX 20091231  ��һ����ʱ �������������� lv_lbctl����qs_dateΪ�� packstatΪ03�ļ�¼ Ϊ qs_dateΪ��һ���� packstatΪ05**/
	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

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


