/***************************************************************************/
/* �ļ����ƣ�С��֧��������������֧�����ף���                              */
/*                                                                         */
/* ����������                                                              */
/*                                                                         */
/* ���ڣ�  2005.07.20                                                      */
/* ���ߣ�  WenShuanghua                                                    */
/***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define EXTERN

#include "hv_define.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "hv_addit_c.h"
#include "hv_fd123.h"
#include "seqctl_c.h"
#include "lv_define.h"
#include "lv_tito.h"
#include "lv_lbctl_c.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "errlog.h"
#include "lvpack.h"
#include "public.h"

extern char * pcLvGetDay();

/**�ж�һ�ʽ����Ƿ��ͽ��׻��ǽ��ս���**/
int lv_get_not_orient(NOTPAY_IN_AREA *wp_notpayin)
{
	if (memcmp(wp_notpayin->optype,OPCD_LR_QUERY,sizeof(wp_notpayin->optype))==0||
		memcmp(wp_notpayin->optype,OPCD_LR,sizeof(wp_notpayin->optype))==0||
		memcmp(wp_notpayin->optype,OPCD_CHECK,sizeof(wp_notpayin->optype))==0||
		memcmp(wp_notpayin->optype,OPCD_SEND_QUERY,sizeof(wp_notpayin->optype))==0||
		memcmp(wp_notpayin->optype,OPCD_SEND,sizeof(wp_notpayin->optype))==0)
	{
		/*
		||memcmp(wp_notpayin->optype,OPCD_RESP,sizeof(wp_notpayin->optype))==0)
		*/
		return(TX_SEND);
	}else
	{
		return(TX_RECEIVE);
	} 
}

int notpayin_printf(NOTPAY_IN_AREA *wp_notpayin)
{
	vtcp_log("[%s][%d]optype     =[%2s ]\n" , __FILE__,__LINE__,wp_notpayin->optype     );
	vtcp_log("[%s][%d]pkgno      =[%3s ]\n" , __FILE__,__LINE__,wp_notpayin->pkgno      );
	vtcp_log("[%s][%d]cmtno      =[%3s ]\n" , __FILE__,__LINE__,wp_notpayin->cmtno      );
	vtcp_log("[%s][%d]orderno    =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->orderno    );
	vtcp_log("[%s][%d]txnum      =[%5s ]\n" , __FILE__,__LINE__,wp_notpayin->txnum      );
	vtcp_log("[%s][%d]acbrno     =[%12s]\n" , __FILE__,__LINE__,wp_notpayin->acbrno     );
	vtcp_log("[%s][%d]rettype    =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->rettype    );
	vtcp_log("[%s][%d]respcode   =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->respcode   );
	vtcp_log("[%s][%d]opackday   =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->opackday   );
	vtcp_log("[%s][%d]opkgno     =[%3s ]\n" , __FILE__,__LINE__,wp_notpayin->opkgno     );
	vtcp_log("[%s][%d]opackid    =[%3s ]\n" , __FILE__,__LINE__,wp_notpayin->opackid    );
	vtcp_log("[%s][%d]otxnum     =[%5s ]\n" , __FILE__,__LINE__,wp_notpayin->otxnum     );
	vtcp_log("[%s][%d]owtday     =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->owtday     );
	vtcp_log("[%s][%d]oorderno   =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->oorderno   );
	vtcp_log("[%s][%d]qorderno  =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->qorderno  );
	vtcp_log("[%s][%d]qwtday      =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->qwtday      );
	vtcp_log("[%s][%d]oorbrno    =[%12s]\n" , __FILE__,__LINE__,wp_notpayin->oorbrno    );
	vtcp_log("[%s][%d]oacbrno    =[%12s]\n" , __FILE__,__LINE__,wp_notpayin->oacbrno    );
	vtcp_log("[%s][%d]otxtype    =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->otxtype    );
	vtcp_log("[%s][%d]cour       =[%3s ]\n" , __FILE__,__LINE__,wp_notpayin->cour       );
	vtcp_log("[%s][%d]otxamt     =[%15s]\n" , __FILE__,__LINE__,wp_notpayin->otxamt     );
	vtcp_log("[%s][%d]unit       =[%60s]\n" , __FILE__,__LINE__,wp_notpayin->unit       );
	vtcp_log("[%s][%d]belongcyc  =[%16s]\n" , __FILE__,__LINE__,wp_notpayin->belongcyc  );
	vtcp_log("[%s][%d]sftype     =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->sftype     );
	vtcp_log("[%s][%d]pdate      =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->pdate      );
	vtcp_log("[%s][%d]pnum       =[%20s]\n" , __FILE__,__LINE__,wp_notpayin->pnum       );
	vtcp_log("[%s][%d]paybrname  =[%60s]\n" , __FILE__,__LINE__,wp_notpayin->paybrname  );
	vtcp_log("[%s][%d]actno      =[%32s]\n" , __FILE__,__LINE__,wp_notpayin->actno      );
	vtcp_log("[%s][%d]name       =[%60s]\n" , __FILE__,__LINE__,wp_notpayin->name       );
	vtcp_log("[%s][%d]reason     =[%60s]\n" , __FILE__,__LINE__,wp_notpayin->reason     );
	vtcp_log("[%s][%d]bsnum      =[%2s ]\n" , __FILE__,__LINE__,wp_notpayin->bsnum      );
	vtcp_log("[%s][%d]passwd     =[%60s]\n" , __FILE__,__LINE__,wp_notpayin->passwd     );
	vtcp_log("[%s][%d]datalen    =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->datalen    );
	vtcp_log("[%s][%d]acttype    =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->acttype    );
	vtcp_log("[%s][%d]pswdcd     =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->pswdcd     );
	vtcp_log("[%s][%d]qtype      =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->qtype      );
	vtcp_log("[%s][%d]orbrno     =[%12s]\n" , __FILE__,__LINE__,wp_notpayin->orbrno     );
	vtcp_log("[%s][%d]payqsactno =[%12s]\n" , __FILE__,__LINE__,wp_notpayin->payqsactno );
	vtcp_log("[%s][%d]sendco     =[%4s ]\n" , __FILE__,__LINE__,wp_notpayin->sendco     );
	vtcp_log("[%s][%d]cashqsactno=[%12s]\n" , __FILE__,__LINE__,wp_notpayin->cashqsactno);
	vtcp_log("[%s][%d]receco     =[%4s ]\n" , __FILE__,__LINE__,wp_notpayin->receco     );
	vtcp_log("[%s][%d]wssrno     =[%6s ]\n" , __FILE__,__LINE__,wp_notpayin->wssrno     );
	vtcp_log("[%s][%d]txday      =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->txday      );
	vtcp_log("[%s][%d]stat       =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->stat       );
	vtcp_log("[%s][%d]datatype   =[%c  ]\n" , __FILE__,__LINE__,wp_notpayin->datatype   );
	vtcp_log("[%s][%d]addid      =[%8s ]\n" , __FILE__,__LINE__,wp_notpayin->addid      );

	vtcp_log("[%s][%d]content   =[%255s]\n" , __FILE__,__LINE__,wp_notpayin->content    );
	vtcp_log("[%s][%d]pascode   =[%128s]\n" , __FILE__,__LINE__,wp_notpayin->pascode    );
	return 0;
}

int iModifySorceTableStatFor328(NOTPAY_IN_AREA *wp_notpayin)
{
	int ret=0;

	struct hv_fzf_c       sHvFzf;
	struct lv_pkgreg_c    sLvPkgreg;
	memset(&sHvFzf, '\0', sizeof(struct hv_fzf_c));
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

	ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf," otype = '1' and or_br_no = '%s' and wt_date = '%d' and orderno = '%s' and cmtno = '327'", wp_notpayin->acbrno,wp_notpayin->owtday,wp_notpayin->oorderno);
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (wp_notpayin->rettype!='0') return 0;
	printf("[%s][%d] pkgno===[%.3s] \n",__FILE__,__LINE__,wp_notpayin->pkgno);
  /****NEWYX ALL:�޸���if����,����ͨ�ý���****/
	if ( !memcmp(sHvFzf.otxnum,TXNUM_GKJJ,sizeof(sHvFzf.otxnum)-1)  
		||!memcmp(sHvFzf.otxnum,TXNUM_QTJJ,sizeof(sHvFzf.otxnum)-1)  
		||!memcmp(sHvFzf.otxnum,TXNUM_ZJZF,sizeof(sHvFzf.otxnum)-1)  
		||!memcmp(sHvFzf.otxnum,TXNUM_SQZF,sizeof(sHvFzf.otxnum)-1)  
		||!memcmp(sHvFzf.otxnum,TXNUM_GZJJ,sizeof(sHvFzf.otxnum)-1)
		||!memcmp(sHvFzf.otxnum,TXNUM_TYJL,sizeof(sHvFzf.otxnum)-1) 
		||!memcmp(sHvFzf.otxnum,TXNUM_ZPJL,sizeof(sHvFzf.otxnum)-1)) 
	{ 
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date = '%s' and orderno = '%s' ", sHvFzf.or_br_no,sHvFzf.o_wt_date,sHvFzf.o_orderno);
		if (ret)
		{
			vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
			WRITEMSG
				return -1;
		}
		ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
		if ( ret == 1403 )
		{
			sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
			WRITEMSG
				return -1;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
			WRITEMSG
				return -1;
		}
		if (sLvPkgreg.lv_sts[0]!=STAT_LZRECV1)
		{
			vtcp_log("[%s][%d] ���е�״̬��ʱ����״̬������ֹ��==[%c] \n",
				__FILE__,__LINE__,sLvPkgreg.lv_sts[0]);
			sprintf( acErrMsg, "״̬��![%d]", ret );
			WRITEMSG
				return -1;
		}
		if (wp_notpayin->respcode=='1')/******add by xyy 20060328****/
		{
			sLvPkgreg.lv_sts[0]=STAT_LZZF;
		}	
		printf("[%s][%d]sLvPkgreg.stat=[%c]\n",__FILE__,__LINE__,sLvPkgreg.lv_sts[0]);

		ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
			WRITEMSG
				return -1;
		}
		Lv_pkgreg_Clo_Upd();
		return 0;
	}
	if ( !memcmp(sHvFzf.otxnum,TXNUM_DQJJ,sizeof(sHvFzf.otxnum)-1)  
		||!memcmp(sHvFzf.otxnum,TXNUM_PLKS,sizeof(sHvFzf.otxnum)-1)) 
	{
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date = '%s' and orderno = '%s' ", sHvFzf.or_br_no, sHvFzf.o_wt_date,sHvFzf.o_orderno);
		if (ret)
		{
			vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
			WRITEMSG
				return -1;
		}
		ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
		if ( ret == 1403 )
		{
			sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
			WRITEMSG
				return -1;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
			WRITEMSG
				return -1;
		}
		if (sLvPkgreg.lv_sts[0]!=STAT_LZRECV1)
		{
			vtcp_log("[%s][%d] ���е�״̬��ʱ����״̬������ֹ��==[%c] \n",
				__FILE__,__LINE__,sLvPkgreg.lv_sts[0]);
			sprintf( acErrMsg, "״̬��![%d]", ret );
			WRITEMSG
				return -1;
		}
		sLvPkgreg.lv_sts[0]=STAT_LZZF;
		ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
			WRITEMSG
				return -1;
		}
		Lv_pkgreg_Clo_Upd();
		return 0;
	}
	return 0;
}

/********************************************************/
/*	������:Rebk_Mul_Pkg_Djth			*/
/*  ����:Ҫ���˻ش����˻�				*/
/********************************************************/
int Rebk_Mul_Pkg_Djth(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	struct lv_pkgreg_c              sLvPkgreg;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));
  /****NEWYX BEGIN:����pkgno����,�ϱ߻���****/
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply," pkgno = '%s' and  or_br_no = '%s' and wt_date=%d and orderno = '%s' ",PKGNO_DJTH, wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
  /****NEWYX END:����pkgno����,�ϱ߻���****/
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.txnum, wp_notpayin.otxnum, 5)!=0)
	{
		vtcp_log("[%s][%d]ԭҵ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.otxnum,lv_pkgreg_c.txnum);
		sprintf( acErrMsg, "ԭҵ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND && lv_pkgreg_c.lv_sts[0]!=STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]====stat=[%c]����Ϊ4,7\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();
	return(0);
}

/********************************************************/
/*	������:Rebk_Mul_Pkg_Dqdj()							*/
/*  ����:Ҫ���˻ض��ڴ���ҵ��							*/
/********************************************************/
int Rebk_Mul_Pkg_Dqdj(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	struct lv_pkgreg_c              sLvPkgreg;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));
	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.txnum, wp_notpayin.otxnum, 5)!=0)
	{
		vtcp_log("[%s][%d]ԭҵ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.otxnum,lv_pkgreg_c.txnum);
		sprintf( acErrMsg, "ԭҵ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"319", 3)==0)	/** �˻� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSUCCESS && lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{  	 
			vtcp_log("[%s][%d]====stat=[%c]����Ϊ6��4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	else if (memcmp(wp_notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND && lv_pkgreg_c.lv_sts[0]!=STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]====stat=[%c]����Ϊstat=4,7\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg_Dqjj()							*/
/*  ����:Ҫ���˻ض��ڽ��ҵ��							*/
/********************************************************/
int Rebk_Mul_Pkg_Dqjj(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	struct lv_pkgreg_c              sLvPkgreg;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	/*�Ƚ�ԭ�����ͺţ���ί�����ڣ�����ţ�ҵ�����ͺ�*/
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.txnum, wp_notpayin.otxnum, 5)!=0)
	{
		vtcp_log("[%s][%d]ԭҵ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.otxnum,lv_pkgreg_c.txnum);
		sprintf( acErrMsg, "ԭҵ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND && lv_pkgreg_c.lv_sts[0]!=STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]====stat=[%c]����Ϊ4,7\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	else if (memcmp(wp_notpayin.cmtno ,"327", 3)==0)    /** ֹ�� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{
			vtcp_log("[%s][%d]====stat=[%c]����Ϊ4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg_Ssdj()							*/
/*  ����:Ҫ���˻�ʵʱ����ҵ��							*/
/********************************************************/
int Rebk_Mul_Pkg_Ssdj(NOTPAY_IN_AREA *notpayin)
{
	int ret,date;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c              sLvPkgreg;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));
	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));

	date = apatoi(wp_notpayin.owtday,8);
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", wp_notpayin.orbrno,date,wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	/*�Ƚ�ԭ�����ͺţ���ί�����ڣ�����ţ�ҵ�����ͺ�*/
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"323", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{
			vtcp_log("[%s][%d]====stat=[%c]����Ϊstat=4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg_Ssjj()			*/
/*  ����:Ҫ���˻�ʵʱ���ҵ��				*/
/********************************************************/
int Rebk_Mul_Pkg_Ssjj(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	struct lv_pkgreg_c              sLvPkgreg;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	/*�Ƚ�ԭ�����ͺţ���ί�����ڣ�����ţ�ҵ�����ͺ�*/
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"323", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{
			vtcp_log("[%s][%d]ԭҵ��״̬����!stat=[%c]����Ϊ4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);
}

/********************************************************/
/*	������:Rebk_Sig_Pkg()				*/
/*  ����:Ҫ���˻ص���ҵ��				*/
/********************************************************/
int Rebk_Sig_Pkg(NOTPAY_IN_AREA *notpayin)
{
	int iTxnum=0,ret=0;
	char	cTxnum[6];
	char	cPackid[9];

	struct lv_wbctl_c  sLvWbctl;
	struct lv_wbctl_c  lv_wbctl_c;
	NOTPAY_IN_AREA wp_notpayin;

	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));
	memset(&sLvWbctl,    '\0', sizeof(struct lv_wbctl_c));
	memset(&lv_wbctl_c,  '\0', sizeof(struct lv_wbctl_c));
	memset(cPackid, '\0', sizeof(cPackid));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));
	memcpy(cPackid,notpayin->opackid,sizeof(cPackid)-1);

	ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLvWbctl," pack_date=%d and packid = '%s'", apatoi(notpayin->opackday,8), cPackid);
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
		WRITEMSG
			return -1;
	}
	/**************�����������Ʊ�*****************************************/

	if (memcmp(sLvWbctl.packstat, PACKSTAT_SEND, 2)!=0 && memcmp(sLvWbctl.packstat, PACKSTAT_SUCCESS, 2)!=0 &&
		memcmp(sLvWbctl.packstat, PACKSTAT_QS, 2)!=0)
	{
		vtcp_log("[%s][%d]��¼״̬����!״̬=[%s]����Ϊ=02��03,05\n",__FILE__,__LINE__,sLvWbctl.packstat);
		sprintf( acErrMsg, "��¼״̬�����!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(sLvWbctl.pkgno,notpayin->opkgno,sizeof(sLvWbctl.pkgno)-1)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ʹ���!\n",__FILE__,__LINE__);
		sprintf( acErrMsg, "ԭ�����ʹ�!" );
		WRITEMSG
			return -1;
	}
	iTxnum = atoi(cTxnum);
	vtcp_log("[%s][%d]========iTxnum=[%d]\n",__FILE__,__LINE__,iTxnum);
	switch(iTxnum)	
	{
	case 100: /*���*/
	case 101: /*���ճи�*/
	case 102: /*ί���տ�*/
	case 103: /*�����ʽ���ǻ���*/
	case 106: /*��������ҵ��*/
	case 20001: /*��˰*/
	case 20002: /*��˰����*/ 
	case 20003: /*����ֱ��֧���˻�*/ 
	case 20004: /*������Ȩ֧���˻�*/ 
	case 20005: /*��ծ�Ҹ����ǻ���*/ 
	case 30002: /*�ɷ�*/ 
		ret = Rebk_Mul_Pkg_Ptdj(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]������ͨ���Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "������ͨ���Ǵ�!" );
			WRITEMSG
				return -1;
		}
		vtcp_log("[%s][%d]=otxamt=[%s]\n",__FILE__,__LINE__,wp_notpayin.otxamt);
		memcpy(notpayin->otxamt,wp_notpayin.otxamt,sizeof(notpayin->otxamt));
		break;
	case 30001:
		ret = Rebk_Mul_Pkg_Ssdj(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]����ʵʱ���Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "����ʵʱ���Ǵ�!" );
			WRITEMSG
				return -1;
		}
		break;
	case 104:
		ret = Rebk_Mul_Pkg_Dqdj(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]���Ҷ��ڴ��Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "���Ҷ��ڴ��Ǵ�!" );
			WRITEMSG
				return -1;
		}
		break;
	case 113: /*�����ʽ��ǻ���*/
	case 119: /*�������ҵ��*/
	case 20103: /*����ֱ��֧��*/
	case 20104: /*������Ȩ֧��*/
	case 30102:
		ret = Rebk_Mul_Pkg_Ptjj(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]������ͨ��Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "������ͨ��Ǵ�!" );
			WRITEMSG
				return -1;
		}
		break;
	case 114:
	case 20102:
		ret = Rebk_Mul_Pkg_Dqjj(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]���Ҷ��ڽ�Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "���Ҷ��ڽ�Ǵ�!" );
			WRITEMSG
				return -1;
		}
		break;
	case 20101:
	case 30101:
		ret = Rebk_Mul_Pkg_Ssjj(&wp_notpayin);
		if (ret!=0)	
		{
			vtcp_log("[%s][%d]����ʵʱ��Ǳ����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "����ʵʱ��Ǵ�!" );
			WRITEMSG
				return -1;
		}
		break;
	case 108:
		ret = Rebk_Mul_Pkg_Djth(&wp_notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]���Ҵ����˻�����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "���Ҵ����˻��!" );
			WRITEMSG
				return -1;
		}
	}
	return(0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg()				*/
/*  ����:Ҫ���˻�����ҵ��				*/
/********************************************************/
/****Modidified by SSH,2006.02.06,�������¼��ʱ���ı�ԭ����״̬****/ 
int Rebk_Mul_Pkg(NOTPAY_IN_AREA *wp_notpayin)
{
	int iPkgno=0,ret=0;
	char	cPackid[9];
	char	cPackday[9];
	char	cOrderno[9];
	char	cOrbrno[13]; /***���Ӷ������˻ػ����ŵĴ���***/
	char	pcQsactno[13];
	char	cBrno[6];
	char	Stat='0';
	char OldStat = '0';
	char OldStat2 = '0';
	NOTPAY_IN_AREA notpayin;
	struct	lv_wbctl_c	sLvWbctl;
	struct  lv_wbctl_c  lv_wbctl_c;

	memset(&sLvWbctl, '\0', sizeof(struct lv_wbctl_c));
	memset(&lv_wbctl_c, '\0', sizeof(struct lv_wbctl_c));

	memset(pcQsactno,'\0', sizeof(pcQsactno));
	memset(cPackid,  '\0', sizeof(cPackid));
	memset(cPackday, '\0', sizeof(cPackday));
	memset(cOrderno, '\0', sizeof(cOrderno));
	memset(cOrbrno,  '\0', sizeof(cOrbrno));
	memset(cBrno,    '\0', sizeof(cBrno));
	memset(&notpayin, 0 , sizeof(notpayin));

	memcpy(&notpayin,wp_notpayin,sizeof(notpayin));

	/* ���׻��� */
	get_fd_data("0030",cBrno);

	memcpy(pcQsactno,LV_QSBRNO,sizeof(pcQsactno)-1);
	if (GetOr_br_noByBr_no(cBrno,cOrbrno)==NULL)
	{
		vtcp_log("[%s][%d]�õ������߻����Ŵ���!",__FILE__,__LINE__);
		sprintf( acErrMsg, "�õ������߻����Ŵ�!" );
		WRITEMSG
			return -1;
	}
	ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLvWbctl," pack_date=%d and packid = '%s'",
		apatoi(notpayin.opackday,8),notpayin.opackid);
	if ( ret == 1403 )
	{
		vtcp_log("[%s][%d]ԭ��ѯ��¼������!",__FILE__,__LINE__);
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		vtcp_log("[%s][%d]ִ��Lv_wbctl_Sel��!",__FILE__,__LINE__);
		sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_wbctl_c.pkgno,"003",3)==0 
		|| memcmp(lv_wbctl_c.pkgno,"004",3)==0)
	{
		if (lv_wbctl_c.stat[0]!=PSTAT_SEND && lv_wbctl_c.stat[0]!=PSTAT_WRESP)
		{
			vtcp_log("[%s][%d]��¼״̬����!״̬=[%c]����Ϊ=3\n",
				__FILE__,__LINE__,lv_wbctl_c.stat[0]);
			sprintf( acErrMsg, "��¼״̬����!" );
			WRITEMSG
				return -1;
		}
	}else
	{
		if (lv_wbctl_c.stat[0] != PSTAT_WRESP)
		{
			vtcp_log("[%s][%d]��¼״̬����!״̬=[%c]����Ϊ=4\n",
				__FILE__,__LINE__,lv_wbctl_c.stat[0]);
			sprintf( acErrMsg, "��¼״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	if (memcmp(lv_wbctl_c.packstat, PACKSTAT_SEND, 2)!=0 && memcmp(lv_wbctl_c.packstat, PACKSTAT_SUCCESS, 2)!=0
		&&memcmp(lv_wbctl_c.packstat, PACKSTAT_QS, 2)!=0 && memcmp(lv_wbctl_c.packstat, PACKSTAT_INQUEUE, 2)!=0)
	{
		vtcp_log("[%s][%d]��¼״̬����!״̬=[%s]����Ϊ=02��03\n",__FILE__,__LINE__,lv_wbctl_c.packstat);
		sprintf( acErrMsg, "��¼״̬����!" );
		WRITEMSG
			return -1;
	}	
	if (memcmp(lv_wbctl_c.pkgno,notpayin.opkgno,3) != 0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!wbctl.pkgno=[%s]notpayin.opkgno=[%s]\n",
			__FILE__,__LINE__,lv_wbctl_c.pkgno,notpayin.opkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	memcpy(cPackid,  lv_wbctl_c.packid,  sizeof(cPackid)-1);
	apitoa(lv_wbctl_c.pack_date, sizeof(cPackday),cPackday);

	if (memcmp(notpayin.cmtno ,"319", 3)==0)	/** �˻� **/
	{
		OldStat = STAT_WZSUCCESS;
		OldStat2 = STAT_WZSEND;
	}
	else if (memcmp(notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		OldStat = STAT_WZSEND;
		OldStat2 = STAT_WZINQUEUE;
	}
	else if (memcmp(notpayin.cmtno ,"323", 3)==0)    /** ���� **/
	{
		OldStat = STAT_WZSEND;
		OldStat2 = STAT_WZSEND;
	}
	else if (memcmp(notpayin.cmtno ,"327", 3)==0)    /** ֹ�� **/
	{
		OldStat = STAT_WZSEND;
		OldStat2 = STAT_WZSEND;
	}
	/* �����еĽ�����ӵ�notpay_in�е�totamt�� */
	dbl2str(&lv_wbctl_c.totamt,0,sizeof(notpayin.totamt),0,notpayin.totamt);
	dbl2str(&lv_wbctl_c.dtlcnt,0,sizeof(notpayin.totcnt),0,notpayin.totcnt);
	memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(notpayin.totamt));
	memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(notpayin.totcnt));

	vtcp_log("[%s][%d]=========notpayin->opkgno=[%s]\n",__FILE__,__LINE__,notpayin.opkgno);
	iPkgno=apatoi(notpayin.opkgno,3);
	vtcp_log("[%s][%d]=========iPkgno=[%d]\n",__FILE__,__LINE__,iPkgno);
	vtcp_log("[%s][%d]===========stat=[%c]\n",__FILE__,__LINE__,Stat);
	return(0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg_Ptdj()			*/
/*  ����:Ҫ���˻���ͨ����ҵ��				*/
/********************************************************/
int Rebk_Mul_Pkg_Ptdj(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c	sLvPkgreg;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));
	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));

	memcpy(&wp_notpayin, notpayin,sizeof(wp_notpayin));

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.pkgno, wp_notpayin.opkgno, 3)!=0)
	{
		vtcp_log("[%s][%d]ԭ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.opkgno,lv_pkgreg_c.pkgno);
		sprintf( acErrMsg, "ԭ�����ͺŴ���![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(lv_pkgreg_c.txnum, wp_notpayin.otxnum, 5)!=0) 
	{
		vtcp_log("[%s][%d]ԭҵ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.otxnum,lv_pkgreg_c.txnum);
		sprintf( acErrMsg, "ԭҵ�����ͺŴ���![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"319", 3)==0)	/** �˻� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSUCCESS && lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{   
			vtcp_log("[%s][%d]====��lv_sts=[%c]����stat=6��4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭ��¼״̬����![%d]", ret );
			WRITEMSG
				return -1;
		}
	}
	else if (memcmp(wp_notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND && lv_pkgreg_c.lv_sts[0]!=STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]====stat=[%c]����stat=4��7\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭ��¼״̬����![%d]", ret );
			WRITEMSG
				return -1;
		}
	}
	dbl2str(&lv_pkgreg_c.tx_amt,0,sizeof(notpayin->otxamt),0,notpayin->otxamt);
	vtcp_log("[%s][%d] otxamt===[%.15s] \n",__FILE__,__LINE__,notpayin->otxamt);

	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return (0);
}
/********************************************************/
/*	������:Rebk_Mul_Pkg_Ptjj()							*/
/*  ����:Ҫ���˻���ͨ���ҵ��							*/
/********************************************************/
int Rebk_Mul_Pkg_Ptjj(NOTPAY_IN_AREA *notpayin)
{
	int ret;
	NOTPAY_IN_AREA wp_notpayin;
	struct lv_pkgreg_c              sLvPkgreg;
	struct lv_pkgreg_c	lv_pkgreg_c;

	memset(&lv_pkgreg_c, '\0', sizeof(struct lv_pkgreg_c));

	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));
	memset(&wp_notpayin, '\0', sizeof(wp_notpayin));

	memcpy(&wp_notpayin,notpayin,sizeof(wp_notpayin));

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply," or_br_no = '%s' and wt_date = %d and orderno = '%s' ", wp_notpayin.orbrno,apatoi(wp_notpayin.owtday,8),wp_notpayin.oorderno);
	if (ret)
	{
		vtcp_log("[%s][%d]����pkg�Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_pkgreg_Fet_Upd��![%d]", ret );
		WRITEMSG
			return -1;
	}
	/*�Ƚ�ԭ�����ͺţ���ί�����ڣ�����ţ�ҵ�����ͺ�*/
	if (memcmp(lv_pkgreg_c.txnum, wp_notpayin.otxnum, 5)!=0)
	{
		vtcp_log("[%s][%d]ԭҵ�����ͺŴ���!����=[%s],����=[%s]", \
			__FILE__,__LINE__,wp_notpayin.otxnum,lv_pkgreg_c.txnum);
		sprintf( acErrMsg, "ԭҵ�����ͺŴ���!" );
		WRITEMSG
			return -1;
	}
	if (memcmp(wp_notpayin.cmtno ,"321", 3)==0)    /** ���� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND && lv_pkgreg_c.lv_sts[0]!=STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]====stat=[%c]����stat=4��7\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	else if (memcmp(wp_notpayin.cmtno ,"327", 3)==0)    /** ֹ�� **/
	{
		if (lv_pkgreg_c.lv_sts[0]!=STAT_WZSEND)
		{
			vtcp_log("[%s][%d]====stat=[%c]����stat=4\n",__FILE__,__LINE__,lv_pkgreg_c.lv_sts[0]);
			sprintf( acErrMsg, "ԭҵ��״̬����!" );
			WRITEMSG
				return -1;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�֧���ǼǱ���[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);
}


/*���ǰ̨�������Ƿ�Ϸ�*/
int check_notpayin(NOTPAY_IN_AREA *wp_notpayin)
{
	char	cCmtno[4];
	char	cPkgno[4];
	char	cTxnum[6];
	char	sOpackday[9];
	char	sOorderno[9];
	char	sBakcode[13];
	char	sOwtday[9];
	int	iTxnum=0;
	int     iPkgno=0;
	int	iCmtno=0;
	int	ret=0;

	NOTPAY_IN_AREA notpayin;
	static struct  hv_fzf_c hv_fzf_c;
	static struct  hv_fzf_c sHvfzf;

	memset(&notpayin, '\0', sizeof(notpayin));
	memset(&hv_fzf_c, '\0', sizeof(hv_fzf_c));
	memset(&sHvfzf, 0x00, sizeof( struct hv_fzf_c ) );

	memset(cCmtno, 0, sizeof(cCmtno));
	memset(cPkgno, 0, sizeof(cPkgno));
	memset(cTxnum, 0, sizeof(cTxnum));
	memset(sOpackday, 0, sizeof(sOpackday));
	memset(sOorderno, 0, sizeof(sOorderno));
	memset(sBakcode, 0, sizeof(sBakcode));
	memset(sOwtday, 0, sizeof(sOwtday));

	memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
	memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);
	memcpy(cTxnum, wp_notpayin->txnum, sizeof(cTxnum)-1);
	memcpy(&notpayin,wp_notpayin,sizeof(notpayin));

	iCmtno = atoi(cCmtno);
	iPkgno = atoi(cPkgno);
	iTxnum = atoi(cTxnum);
	vtcp_log("[%s][%d]=========wp_notpayin->pkgno==[%s]\n",__FILE__,__LINE__,wp_notpayin->pkgno);
	vtcp_log("[%s][%d]=========iPkgno=[%s]\n",__FILE__,__LINE__,iPkgno);
	vtcp_log("[%s][%d]=========iTxnum=[%d]\n",__FILE__,__LINE__,iTxnum); /*modify xyy 20060322***/

	if (!memcmp(wp_notpayin->pkgno, PKGNO_TYXX, 3))
	{/* ͨ����Ϣ */
		if (!memcmp(wp_notpayin->txnum, TXNUM_TYXX, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_DS, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_DF, TXNUM_LEN))
		{
			vtcp_log("[%s][%d]ҵ�����ʹ���!\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"ҵ�����ʹ���![%s][%d]",__FILE__,__LINE__);
			WRITEMSG
				return -1;
		}
	}
	if (!memcmp(wp_notpayin->pkgno, PKGNO_SSXX, 3))
	{/*ʵʱ��Ϣ*/
		if (!memcmp(wp_notpayin->txnum, TXNUM_ZPQC, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_ZHCX, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_CXYD, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_ZPYD, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_ZSCX, TXNUM_LEN)
			&&!memcmp(wp_notpayin->txnum, TXNUM_ZSYD, TXNUM_LEN))
		{
			vtcp_log("[%s][%d]ҵ�����ʹ���!\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"ҵ�����ʹ���![%s][%d]",__FILE__,__LINE__);
			WRITEMSG
				return -1;
		}
	}
	/*�жϼ�¼״̬�Ƿ���ȷ*/

	/** �����кͽ����в�����ͬһ�� **/
	vtcp_log("[%s][%d],����������:[%s],����������:[%s]\n",__FILE__,__LINE__,
		wp_notpayin->payqsactno,wp_notpayin->cashqsactno);

	if (memcmp(wp_notpayin->cashqsactno,wp_notpayin->payqsactno,12) == 0)
	{
		vtcp_log("[%s][%d]����ͽ��������в�����ͬһ��,����=[%s],����=[%s]\n", \
			__FILE__,__LINE__,wp_notpayin->payqsactno,wp_notpayin->cashqsactno);
		sprintf( acErrMsg,"����ͽ����в�����ͬһ��![%s][%d]",__FILE__,__LINE__);
		WRITEMSG
		return -1;
	}
	if (memcmp(wp_notpayin->acbrno,wp_notpayin->orbrno,12) == 0)
	{
		vtcp_log("[%s][%d]����ͽ����в�����ͬһ��,����=[%s],����=[%s]\n", \
			__FILE__,__LINE__,wp_notpayin->orbrno,wp_notpayin->acbrno);
		sprintf( acErrMsg,"����ͽ����в�����ͬһ��![%s][%d]",__FILE__,__LINE__);
		WRITEMSG
		return -1;
	}
	/** �ж�ԭί�����ڵĺϷ��� **/
	memcpy(sOwtday, wp_notpayin->owtday, sizeof(sOwtday)-1);
	if (zip_space(sOwtday)!=0)
	{
		if (memcmp(sOwtday,wp_notpayin->txday,8) > 0)
		{
			vtcp_log("[%s][%d]ԭί�����ڲ��Ϸ�,=[%s]\n", \
				__FILE__,__LINE__,sOwtday);
			sprintf( acErrMsg,"ԭί�����ڲ��Ϸ�![%s][%d]",__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "EP006" );
			return -1;
		}
	}
	/** �ж�ԭ��ί�����ڵĺϷ��� **/
	memcpy(sOpackday, wp_notpayin->opackday,sizeof(sOpackday)-1);
	if (zip_space(sOpackday)!=0)
	{
		if (memcmp(sOpackday,wp_notpayin->txday,12) > 0)
		{
			vtcp_log("[%s][%d]ԭ��ί�����ڲ��Ϸ�,=[%s]\n", \
				__FILE__,__LINE__,wp_notpayin->opackday);
			sprintf( acErrMsg,"ԭ��ί�����ڲ��Ϸ�![%s][%d]",__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "EP006" );
			return -1;
		}
	}
	/*���濪ʼ�ж�ԭ֧�������Ƿ���ڣ��Լ���ǰ״̬�Ƿ��������˽���*/
	switch(iCmtno)
	{
	case 301:/*��ѯ����*/
		if (((memcmp(wp_notpayin->oacbrno,wp_notpayin->acbrno,12) == 0)
			||(memcmp(wp_notpayin->oorbrno,wp_notpayin->acbrno,12) == 0))
			&&(memcmp(wp_notpayin->oorbrno,wp_notpayin->oacbrno,12) != 0))
		{
			/****do nothing continue**********************/

		}
		else
		{
			vtcp_log("[%s][%d]����������кŴ�����!,����=[%s],ԭ����=[%s]!ԭ������=[%s]\n", \
				__FILE__,__LINE__,wp_notpayin->orbrno,wp_notpayin->oacbrno,wp_notpayin->oorbrno);
			sprintf( acErrMsg,"����������кŴ���![%s][%d]",__FILE__,__LINE__);
			WRITEMSG
				return -1;
		}
		break; 
	case 302:/*�鸴����*/
		memcpy(sOorderno, wp_notpayin->qorderno, sizeof(sOorderno)-1);
		memcpy(sBakcode, wp_notpayin->acbrno, sizeof(sBakcode)-1);
		ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '302'", sBakcode,apatoi(wp_notpayin->qwtday,8),sOorderno);
		if (ret)
		{
			vtcp_log("[%s][%d]������ѯ���Ľ��յǼǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "EP020" );
			return -1;
		}
		ret = Hv_fzf_Fet_Upd( &sHvfzf, g_pub_tx.reply );
		if ( ret == 1403 )
		{
			sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
			WRITEMSG
			return -1;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
			WRITEMSG
			return -1;
		}
		if (sHvfzf.hv_fzf_sts[0] != STAT_LZRECV1)
		{
			vtcp_log("[%s][%d]ԭ��ѯ����״̬����!stat=[%c]��Ҫ��״̬��H\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
			WRITEMSG
			return -1;
		}
		sHvfzf.hv_fzf_sts[0]=STAT_LZBACK;	/** ��Ӧ�� **/
		sHvfzf.res_date = apatoi(pcLvGetDay(), 8);
		ret = Hv_fzf_Upd_Upd( sHvfzf, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
			WRITEMSG
			return -1;
		}
		Hv_fzf_Clo_Upd();

		break;
	case 303:
		/*���ɸ�ʽ����*/
		break;
	case 319:
	case 321:
		/*�˻����뱨��*/
		/*�ɲ�ͬ��ҵ��������������ͬ�ı�*/
		/*��ҵ��ֻ�Ƕ�������ҵ��*/
		/*add by liuyue 051129����������Ľ����н���һ�����ж�,�Լ������������ж�
		����һ������û���ѡ���������ߵ���ʱ�����ʱ,�ڿ�ʼ�ͱ��������������õȵ�ȥ����������packid,
		packday ���������õ�������� **************************/

		if (wp_notpayin->rettype=='1')	/** �˻����� **/
		{	
			vtcp_log("[%s][%d]=======wp_notpayin->opkgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
			vtcp_log("[%s][%d]=======notpayin->opkgno=[%s]\n",__FILE__,__LINE__,notpayin.opkgno);
			ret=Rebk_Mul_Pkg(&notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]========����pkg�����,ret=[%d]\n",__FILE__,__LINE__,ret);
				WRITEMSG
				return -1;
			}
			memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(wp_notpayin->totamt));
			memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(wp_notpayin->totcnt));
		}
		else if (wp_notpayin->rettype=='0')		/** �˻ص��� **/
		{	
			vtcp_log("[%s][%d]==notpayin--otxnum====[%s]\n",__FILE__,__LINE__,wp_notpayin->otxnum);
			ret=Rebk_Sig_Pkg(&notpayin);
			vtcp_log("[%s][%d]otxamt==[%s]=\n",__FILE__,__LINE__,notpayin.otxamt);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]========����pkg�����,ret=[%d]\n",__FILE__,__LINE__,ret);
				WRITEMSG
				return -1;
			}
			memcpy(wp_notpayin->otxamt,notpayin.otxamt,sizeof(wp_notpayin->otxamt));
			memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(wp_notpayin->totamt));
			memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(wp_notpayin->totcnt));
		}
		break;
	case 320:
		/*����ԭ���ҵ���˻�Ӧ��Ǽǲ�*/
		memcpy(sOorderno, wp_notpayin->oorderno, sizeof(sOorderno)-1);
		memcpy(sBakcode, wp_notpayin->acbrno, sizeof(sBakcode)-1);
		ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '319'", sBakcode,apatoi(wp_notpayin->owtday,8),sOorderno);
		if (ret)
		{
			vtcp_log("[%s][%d]������֧���Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "������֧���Ǽǲ���![%d]", ret );
			WRITEMSG
			return -1;
		}
		ret = Hv_fzf_Fet_Upd( &sHvfzf, g_pub_tx.reply );
		if ( ret == 1403 )
		{
			vtcp_log("[%s][%d]ԭ��ѯ��¼������\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
			WRITEMSG
			return -1;
		}else if( ret )
		{
			vtcp_log("[%s][%d]ִ��Hv_fzf_Fet_Upd��\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
			WRITEMSG
			return -1;
		}
		if (sHvfzf.hv_fzf_sts[0]!=STAT_LZRECV1)
		{
			vtcp_log("[%s][%d]Ӧ���ʼ״̬����!��״̬��[%c]��Ҫ��״̬��H\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
			sprintf( acErrMsg, "Ӧ���ʼ״̬�����![%d]", ret );
			WRITEMSG
			return -1;
		}
		sHvfzf.hv_fzf_sts[0] = STAT_LZBACK;
		vtcp_log("[%s][%d]====================sHvfzf.hv_fzf_sts=[%c]\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
		ret = Hv_fzf_Upd_Upd( sHvfzf, g_pub_tx.reply );
		if ( ret )
		{
			vtcp_log("[%s][%d]�޸ķ�֧���ǼǱ���\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
			WRITEMSG
			return -1;
		}
		Hv_fzf_Clo_Upd();

		break;

	case 323:
		/*ʵʱҵ��������뱨��*/
		/*ʵʱ��(��)��ҵ��ķ�����û���յ������еĻ�ִ���ȴ���ʱʱ���ɷ���ʵʱҵ���������*/
		vtcp_log("[%s][%d]+++++pkgno==[%s]+++++\n",__FILE__,__LINE__,iPkgno);
		vtcp_log("[%s][%d]+++++rettype==[%c]+++++\n",__FILE__,__LINE__,wp_notpayin->rettype);
		vtcp_log("[%s][%d]==============wp_notpayin->opkgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
		vtcp_log("[%s][%d]==============notpayin->opkgno=[%s]\n",__FILE__,__LINE__,notpayin.opkgno);
		ret=Rebk_Mul_Pkg(&notpayin);
		if (ret!=0)
		{
			vtcp_log("[%s][%d]========����pkg�����,ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
			WRITEMSG
			return -1;
		}
		memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(wp_notpayin->totamt));
		memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(wp_notpayin->totcnt));

		break;
	case 327:
		/*���ҵ��ֹ�����뱨��*/
		/*��ʵʱ���ҵ��ķ�����û���յ������еĻ�ִǰ��
		�ɷ���ֹ�����뱨�ģ�ֹ������������һ�ʽ��ҵ��*/
		vtcp_log("[%s][%d]======[%c] \n",__FILE__,__LINE__,wp_notpayin->rettype);
		if (wp_notpayin->rettype=='1')  /** �˻����� **/
		{
			ret=Rebk_Mul_Pkg(&notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]========����pkg�����,ret=[%d]\n",__FILE__,__LINE__,ret);
				sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
				WRITEMSG
				return -1;
			}
			memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(wp_notpayin->totamt));
			memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(wp_notpayin->totcnt));
		}
		else if (wp_notpayin->rettype=='0')     /** �˻ص��� **/
		{
			vtcp_log("[%s][%d]==notpayin--otxnum=======[%s]\n",__FILE__,__LINE__,wp_notpayin->otxnum);
			ret=Rebk_Sig_Pkg(&notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]========����pkg�����,ret=[%d]\n",__FILE__,__LINE__,ret);
				sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
				WRITEMSG
				return -1;
			}
			memcpy(wp_notpayin->totamt,notpayin.totamt,sizeof(wp_notpayin->totamt));
			memcpy(wp_notpayin->totcnt,notpayin.totcnt,sizeof(wp_notpayin->totcnt));
		}
		break;
	case 328:
		/*���ҵ��ֹ��Ӧ����*/
		memcpy(sOorderno, wp_notpayin->oorderno, sizeof(sOorderno)-1);
		memcpy(sBakcode, wp_notpayin->acbrno, sizeof(sBakcode)-1);
		ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '327'", sBakcode,apatoi(wp_notpayin->owtday,8),sOorderno);
		if (ret)
		{
			vtcp_log("[%s][%d]������֧���Ǽǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "������֧���Ǽǲ���![%d]", ret );
			WRITEMSG
			return -1;
		}
		ret = Hv_fzf_Fet_Upd( &sHvfzf, g_pub_tx.reply );
		if ( ret == 1403 )
		{
			vtcp_log("[%s][%d]ԭ��ѯ��¼������\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
			WRITEMSG
			return -1;
		}else if( ret )
		{
			vtcp_log("[%s][%d]ִ��Hv_fzf_Fet_Upd��\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
			WRITEMSG
			return -1;
		}
		/*��ԭҵ���Ƿ��Ѿ�����Ӧ��*/
		if (sHvfzf.hv_fzf_sts[0] !=STAT_LZRECV1)
		{
			vtcp_log("[%s][%d]ֹ��Ӧ��,״̬����!����ΪH ����Ϊ[%c]\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts [0] );
			sprintf( acErrMsg, "ֹ��Ӧ��״̬��![%d]", ret );
			WRITEMSG
			return -1;
		}
		sHvfzf.hv_fzf_sts[0] = STAT_LZBACK;

		ret = Hv_fzf_Upd_Upd( sHvfzf, g_pub_tx.reply );
		if ( ret )
		{
			vtcp_log("[%s][%d]�޸ķ�֧���ǼǱ���\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
			WRITEMSG
			return -1;
		}
		Hv_fzf_Clo_Upd();

		break;
	}
	vtcp_log("[%s][%d]====================sHvfzf.hv_fzf_sts=[%c]\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
	/*����ͨ����Ϣҵ�����ʵʱ��Ϣҵ���*/
	switch(iPkgno)
	{
	case 12:
		break;
	case 13:
		switch(iTxnum)
		{
		case 30503:
			/*������ʻ���ѯӦ���Ƿ�������ԭ�ʻ���ѯ����*/
		#if 0	/****Delete by SSH,2006.1.26,�����ԭ����*********/
			{
				struct wd_lv_ssxxr_area  wd_lv_ssxxr;
				memset(&wd_lv_ssxxr, 0, sizeof(wd_lv_ssxxr));
				memcpy(wd_lv_ssxxr.wtday,   wp_notpayin->owtday,  sizeof(wd_lv_ssxxr.wtday)-1);
				memcpy(wd_lv_ssxxr.orbrno,  wp_notpayin->oorbrno, sizeof(wd_lv_ssxxr.orbrno)-1);
				memcpy(wd_lv_ssxxr.orderno, wp_notpayin->oorderno,sizeof(wd_lv_ssxxr.orderno)-1);
				if ((it_txcom.rtncd = DbsLV_SSXXR(DBS_LOCK,&wd_lv_ssxxr)) != DB_OK)
				{
					vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
					WRITEMSG
					DbsLV_SSXXRDebug(&wd_lv_ssxxr);
					memcpy(it_totw.labtex.text,"����ʵʱ��Ϣҵ����յǼǲ�����!",40);
					return it_txcom.rtncd;
				}
				if (TITA.hcode == TX_HCODE_NORMAL)
				{
					if (wd_lv_ssxxr.stat[0] != STAT_LZRECV1)
					{
						vtcp_log("[%s][%d]״̬����!����Ϊ=[%c]����ΪH\n",__FILE__,__LINE__,wd_lv_ssxxr.stat[0]);
						WRITEMSG
						memcpy(it_totw.labtex.text,"״̬����!",40);
						return -1;
					}
					wd_lv_ssxxr.stat[0] = STAT_LZBACK;
					if (memcmp(wd_lv_ssxxr.txnum, TXNUM_ZHCX, TXNUM_LEN)!=0)
					{
						vtcp_log("[%s][%d] txnum====[%s] \n",__FILE__,__LINE__,wd_lv_ssxxr.txnum);
						vtcp_log("[%s]TXNUM_ZHCX==== \n",TXNUM_CXYD);
						vtcp_log("[%s][%d]�������ʹ���!\n",__FILE__,__LINE__);
						WRITEMSG
						memcpy(it_totw.labtex.text,"�������ʹ���!",40);
						return -1;
					}
					if (memcmp(wd_lv_ssxxr.qactno, wp_notpayin->actno, sizeof(wd_lv_ssxxr.qactno)-1) !=0)
					{
						vtcp_log("[%s][%d]ԭ��ѯ�ʺŴ���!����=[%s],����=[%s]\n",__FILE__,__LINE__,wp_notpayin->actno,wd_lv_ssxxr.qactno);
						WRITEMSG
						memcpy(it_totw.labtex.text,"ԭ��ѯ�ʺŴ���!",40);
						return -1;
					}
					if (memcmp(wd_lv_ssxxr.qname, wp_notpayin->name, sizeof(wd_lv_ssxxr.qname)-1) !=0)
					{
						vtcp_log("[%s][%d]ԭ��ѯ��������!����=[%s],����=[%s]\n",__FILE__,__LINE__,wp_notpayin->name,wd_lv_ssxxr.qname);
						WRITEMSG
						memcpy(it_totw.labtex.text,"ԭ��ѯ��������!",40);
						return -1;
					}
				}
				if ((it_txcom.rtncd = DbsLV_SSXXR(DBS_UPDATE,&wd_lv_ssxxr)) != DB_OK)
				{
					vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
					WRITEMSG
					DbsLV_SSXXRDebug(&wd_lv_ssxxr);
					memcpy(it_totw.labtex.text,"����ʵʱ��Ϣҵ����յǼǲ�����!",40);
					return it_txcom.rtncd;
				}
				/*���������Ʊ�*/
				memcpy(wd_lv_lbctl.packday,  wp_notpayin->opackday, sizeof(wd_lv_lbctl.packday)-1);
				vtcp_log("[%s][%d]=pakcday==[%s]\n",__FILE__,__LINE__,wd_lv_lbctl.packday);
				vtcp_log("[%s][%d]=pakcid==[%s]\n",__FILE__,__LINE__,wd_lv_lbctl.packid);
				memcpy(wd_lv_lbctl.packid,   wp_notpayin->opackid,sizeof(wd_lv_lbctl.packid)-1);
				if (find_qsactno(wp_notpayin->oorbrno,wd_lv_lbctl.payqsactno,0)==NULL)
				{
					vtcp_log("[%s][%d]�õ����������д���!",__FILE__,__LINE__);
					WRITEMSG
					memcpy(it_totw.labtex.text,"�õ����������д���!",40);
					return -1;
				}
				if ((it_txcom.rtncd = DbsLV_LBCTL(DBS_LOCK,&wd_lv_lbctl)) != DB_OK)
				{
					vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
					DbsLV_LBCTLDebug(&wd_lv_lbctl);
					WRITEMSG
					memcpy(it_totw.labtex.text,"�����������Ʊ����!",40);
					return -1;
				}
				if (TITA.hcode == TX_HCODE_NORMAL)
				{
					if (wd_lv_lbctl.stat[0] != PSTAT_RECEIVE)
					{
						vtcp_log("[%s][%d]״̬����,��=[%c]����Ϊ��5\n",__FILE__,__LINE__,wd_lv_lbctl.stat[0]);
						WRITEMSG
						memcpy(it_totw.labtex.text,"����״̬����!",40);
						return -1;
					}
					wd_lv_lbctl.stat[0] = PSTAT_LRESP;
				}
				if ((it_txcom.rtncd = DbsLV_LBCTL(DBS_UPDATE,&wd_lv_lbctl)) != DB_OK)
				{
					vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
					DbsLV_LBCTLDebug(&wd_lv_lbctl);
					WRITEMSG
					memcpy(it_totw.labtex.text,"�����������Ʊ����!",40);
					return -1;
				}
			}
		#endif/******Delete by SSH,2006.1.26,�����ԭ����******/
			break;
		}
		break;
	}
	vtcp_log("[%s][%d]====================sHvfzf.hv_fzf_sts=[%c]\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
	vtcp_log("[%s][%d]====================sHvfzf.hv_fzf_sts=[%c]\n",__FILE__,__LINE__,sHvfzf.hv_fzf_sts[0]);
	return 0;
}


/*ע�����ʱ����״̬*/
/*�ӱ��еõ��������notpayin�ṹ�壬����notpay_out���������и�ֵ*/
/*ע�����ڵ�ȡֵ*/
int table_to_notpayin(NOTPAY_IN_AREA *wp_notpayin,NOTPAY_OUT_AREA *wp_notpay_out)
{
	char	cCmtno[4];
	char	cPkgno[4];
	int     iPkgno;
	int	iCmtno;
	int	ret;
	NOTPAY_IN_AREA	wd_notpayin;
	NOTPAY_OUT_AREA	wd_notpay_out;
	struct hv_fzf_c		sHvFzf;
	struct hv_fzf_c		hv_fzf_c;
	memset(&sHvFzf, '\0', sizeof(struct hv_fzf_c));
	memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));

	iCmtno = 0;
	iPkgno = 0;
	memset(cCmtno, 0, sizeof(cCmtno));
	memset(cPkgno, 0, sizeof(cPkgno));
	memset(&wd_notpayin, 0, sizeof(wd_notpayin));
	memset(&wd_notpay_out, 0, sizeof(wd_notpay_out));

	memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
	memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);
	iCmtno = atoi(cCmtno);
	iPkgno = atoi(cPkgno);

	switch(iCmtno)
	{
	case 319:
	case 320:
	case 321:
	case 322:
	case 323:
	case 324:
	case 325:
	case 327:
	case 328:
		/*�ӳ����˻س���ֹ�����͵Ǽǲ��еõ���¼*/
		{
			ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '%s'", wp_notpayin->orbrno,apatoi(wp_notpayin->txday,8),wp_notpayin->orderno,wp_notpayin->cmtno);
			if ( ret == 1403 )
			{
				sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "EP020" );
				return -1;
			}else if( ret )
			{
				sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
				WRITEMSG
				return -1;
			}
			/*��������*/
			apitoa(hv_fzf_c.wt_date, sizeof(wd_notpay_out.txday),wd_notpay_out.txday);
			memcpy(wd_notpay_out.brno,    hv_fzf_c.br_no,    sizeof(wd_notpay_out.brno));
			memcpy(wd_notpay_out.orderno, hv_fzf_c.orderno, sizeof(wd_notpay_out.orderno));
			/*ҵ�����ͣ�txnum������*/
			memcpy(wd_notpay_out.orbrno, hv_fzf_c.or_br_no,  sizeof(wd_notpay_out.orbrno));
			memcpy(wd_notpay_out.acbrno, hv_fzf_c.ac_br_no, sizeof(wd_notpay_out.acbrno));
			if (iCmtno==319 || iCmtno==321 || iCmtno==323 || iCmtno==327)
				memcpy(wd_notpay_out.content, hv_fzf_c.req_content, sizeof(wd_notpay_out.content));
			if (iCmtno==320 || iCmtno==328)
				memcpy(wd_notpay_out.content, hv_fzf_c.res_content, sizeof(wd_notpay_out.content));
		}
		break;
	case 301:
		/*�Ӳ�ѯ���ķ��͵Ǽǲ��еõ���¼*/
		{
			ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf," otype = '1' and or_br_no = '%s' and wt_date = '%d' and orderno = '%s' and cmtno = '301'",wp_notpayin->orbrno,apatoi(wp_notpayin->txday,8),wp_notpayin->orderno);
			if ( ret == 1403 )
			{
				sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "EP020" );
				return -1;
			}else if( ret )
			{
				sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
				WRITEMSG
				return -1;
			}
			apitoa(hv_fzf_c.wt_date, sizeof(wd_notpay_out.txday),wd_notpay_out.txday);
			memcpy(wd_notpay_out.brno,    hv_fzf_c.br_no,   sizeof(wd_notpay_out.brno));
			memcpy(wd_notpay_out.orderno, hv_fzf_c.orderno,sizeof(wd_notpay_out.orderno));
			/*ҵ�����ͣ�txnum������*/
			memcpy(wd_notpay_out.orbrno,  hv_fzf_c.or_br_no,  sizeof(wd_notpay_out.orbrno));
			memcpy(wd_notpay_out.acbrno,  hv_fzf_c.ac_br_no, sizeof(wd_notpay_out.acbrno));
			memcpy(wd_notpay_out.content, hv_fzf_c.req_content, sizeof(wd_notpay_out.content));
		}
		break;
	case 302:
		/*�Ӳ鸴���ķ��͵Ǽǲ��еõ���¼*/
		{
			ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '302'",wp_notpayin->orbrno,apatoi(wp_notpayin->txday,8),wp_notpayin->orderno);
			if ( ret == 1403 )
			{
				sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "EP020" );
				return -1;
			}else if( ret )
			{
				sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
				WRITEMSG
				return -1;
			}
			apitoa(hv_fzf_c.req_date, sizeof(wd_notpay_out.txday),wd_notpay_out.txday);
			memcpy(wd_notpay_out.brno,    hv_fzf_c.br_no,    sizeof(wd_notpay_out.brno));
			memcpy(wd_notpay_out.orderno, hv_fzf_c.orderno, sizeof(wd_notpay_out.orderno));
			/*ҵ�����ͣ�txnum������*/
			memcpy(wd_notpay_out.orbrno,  hv_fzf_c.or_br_no,   sizeof(wd_notpay_out.orbrno));
			memcpy(wd_notpay_out.acbrno,  hv_fzf_c.ac_br_no,    sizeof(wd_notpay_out.acbrno));
			memcpy(wd_notpay_out.content, hv_fzf_c.res_content,sizeof(wd_notpay_out.content));
		}
		break;
	case 303:
		/*�����ɸ�ʽ���ķ��͵Ǽǲ��еõ���¼*/
		{
			ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf," otype = '1' and or_br_no = '%s' and wt_date =%d and orderno = '%s' and cmtno = '303'",wp_notpayin->orbrno,apatoi(wp_notpayin->txday,8),wp_notpayin->orderno);
			if ( ret == 1403 )
			{
				sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "EP020" );
				return -1;
			}else if( ret )
			{
				sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
				WRITEMSG
				return -1;
			}
			apitoa(hv_fzf_c.wt_date, sizeof(wd_notpay_out.txday),wd_notpay_out.txday);
			memcpy(wd_notpay_out.brno,    hv_fzf_c.br_no,    sizeof(wd_notpay_out.brno));
			memcpy(wd_notpay_out.orderno, hv_fzf_c.orderno, sizeof(wd_notpay_out.orderno));
			/*ҵ�����ͣ�txnum������*/
			memcpy(wd_notpay_out.orbrno,  hv_fzf_c.or_br_no,  sizeof(wd_notpay_out.orbrno));
			memcpy(wd_notpay_out.acbrno,  hv_fzf_c.ac_br_no,  sizeof(wd_notpay_out.acbrno));
			memcpy(wd_notpay_out.content, hv_fzf_c.req_content, sizeof(wd_notpay_out.content));
		}
		break;
	}
	/**********************δ�ĵĶ���ע�͵�
	switch(iPkgno)
	{
	case 12:
	*��ͨ����Ϣ���͵Ǽǲ����ҵ���¼*
	{
	struct wd_lv_tyxxs_area		wd_lv_tyxxs;
	memset(&wd_lv_tyxxs, 0, sizeof(wd_lv_tyxxs));
	memcpy(wd_lv_tyxxs.orbrno,  wp_notpayin->orbrno,  sizeof(wd_lv_tyxxs.orbrno)-1);
	memcpy(wd_lv_tyxxs.wtday,   wp_notpayin->txday,   sizeof(wd_lv_tyxxs.wtday)-1);
	memcpy(wd_lv_tyxxs.orderno, wp_notpayin->orderno, sizeof(wd_lv_tyxxs.orderno)-1);
	if ((it_txcom.rtncd = DbsLV_TYXXS(DBS_FIND,&wd_lv_tyxxs)) != DB_OK)
	{
	printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
	WRITEMSG
	DbsLV_TYXXSDebug(&wd_lv_tyxxs);
	memcpy(it_totw.labtex.text,"����ͨ����Ϣ���͵Ǽǲ�����!",40);
	return it_txcom.rtncd;
	}
	memcpy(wd_notpay_out.txday,   wd_lv_tyxxs.wtday,   sizeof(wd_notpay_out.txday));
	memcpy(wd_notpay_out.brno,    wd_lv_tyxxs.brno,    sizeof(wd_notpay_out.brno));
	memcpy(wd_notpay_out.orderno, wd_lv_tyxxs.orderno, sizeof(wd_notpay_out.orderno));
	memcpy(wd_notpay_out.orbrno,  wd_lv_tyxxs.orbrno,  sizeof(wd_notpay_out.orbrno));
	memcpy(wd_notpay_out.acbrno,  wd_lv_tyxxs.acbrno,  sizeof(wd_notpay_out.acbrno));
	memcpy(wd_notpay_out.content, wd_lv_tyxxs.content, sizeof(wd_notpay_out.content));
	if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_DF,TXNUM_LEN))
	{
	if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN))
	wd_notpay_out.sfflag = '0';
	else
	wd_notpay_out.sfflag = '1';
	memcpy(wd_notpay_out.unit,   wd_lv_tyxxs.sfunit, sizeof(wd_notpay_out.unit));
	memcpy(wd_notpay_out.sftype, wd_lv_tyxxs.sftype, sizeof(wd_notpay_out.sftype));
	}
	wp_notpayin->stat = wd_lv_tyxxs.stat[0];
	}
	break;
	case 13:
	{
	struct wd_lv_ssxxs_area		wd_lv_ssxxs;
	memset(&wd_lv_ssxxs, 0, sizeof(wd_lv_ssxxs));
	memcpy(wd_lv_ssxxs.orbrno,  wp_notpayin->orbrno,  sizeof(wd_lv_ssxxs.orbrno)-1);
	memcpy(wd_lv_ssxxs.wtday,   wp_notpayin->txday,   sizeof(wd_lv_ssxxs.wtday)-1);
	memcpy(wd_lv_ssxxs.orderno, wp_notpayin->orderno, sizeof(wd_lv_ssxxs.orderno)-1);
	if ((it_txcom.rtncd = DbsLV_SSXXS(DBS_FIND,&wd_lv_ssxxs)) != DB_OK)
	{
	printf("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,it_txcom.rtncd);
	WRITEMSG
	DbsLV_SSXXSDebug(&wd_lv_ssxxs);
	memcpy(it_totw.labtex.text,"����ʵʱ��Ϣ���͵Ǽǲ�����!",40);
	return it_txcom.rtncd;
	}
	memcpy(wd_notpay_out.txday, wd_lv_ssxxs.wtday, sizeof(wd_notpay_out.txday));
	memcpy(wd_notpay_out.brno, 	wd_lv_ssxxs.brno,  sizeof(wd_notpay_out.brno));
	memcpy(wd_notpay_out.orderno, wd_lv_ssxxs.orderno, sizeof(wd_notpay_out.orderno));
	memcpy(wd_notpay_out.orbrno, wd_lv_ssxxs.orbrno, sizeof(wd_notpay_out.orbrno));
	memcpy(wd_notpay_out.acbrno, wd_lv_ssxxs.acbrno, sizeof(wd_notpay_out.acbrno));
	memcpy(wd_notpay_out.content, wd_lv_ssxxs.content, sizeof(wd_notpay_out.content));
	wd_notpay_out.acttype = wd_lv_ssxxs.qacttype[0];
	memcpy(wd_notpay_out.actno, wd_lv_ssxxs.qactno, sizeof(wd_notpay_out.actno));
	memcpy(wd_notpay_out.name,  wd_lv_ssxxs.qname,  sizeof(wd_notpay_out.name));
	wd_notpay_out.pswdcd = wd_lv_ssxxs.qpswdcd[0];
	memcpy(wd_notpay_out.passwd, wd_lv_ssxxs.qpasswd, sizeof(wd_notpay_out.passwd));
	wd_notpay_out.qtype = wd_lv_ssxxs.qtype[0];
	memcpy(wd_notpay_out.qcontent, wd_lv_ssxxs.qcontent, sizeof(wd_notpay_out.qcontent));
	memcpy(wd_notpay_out.opackday, wd_lv_ssxxs.opackday, sizeof(wd_notpay_out.opackday));
	memcpy(wd_notpay_out.opackid,  wd_lv_ssxxs.opackid,  sizeof(wd_notpay_out.opackid));
	dbl2str(&wd_lv_ssxxs.qavbal,0,sizeof(wd_notpay_out.avbal),0,wd_notpay_out.avbal);
	wp_notpayin->stat = wd_lv_ssxxs.stat[0];
	}
	break;
	}
	**********************/
	memcpy(wp_notpayin, &wd_notpayin, sizeof(wd_notpayin));
	return 0;
}

