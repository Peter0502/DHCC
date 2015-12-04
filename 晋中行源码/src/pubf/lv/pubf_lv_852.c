#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "lv_define.h"
#include "hv_define.h"
#include "public.h"
#include "lv_czyh_dz_c.h"
#include "exitlvpack.h"
extern char *pcLvGetDay();
void str_print852mx_to_dtl852(struct str_lv_852_mx_dtl * pDtl,struct str_lv_852_mx_dtl_print * pPrint)
{
	strncpy(pDtl->lw_ind   , pPrint->lw_ind    , sizeof(pDtl->lw_ind   ));
	strncpy(pDtl->pkg_no   , pPrint->pkg_no    , sizeof(pDtl->pkg_no   ));
	strncpy(pDtl->pay_qs_no, pPrint->pay_qs_no , sizeof(pDtl->pay_qs_no));
	sprintf(pDtl->pack_date, "%08ld",pPrint->pack_date);
	strncpy(pDtl->packid   , pPrint->packid    , sizeof(pDtl->packid   ));
}

void str_print852head_to_head(struct str_lv_852_head * pHead,struct str_lv_852_head_print * pPrint)
{
	sprintf(pHead->wt_date  ,"%08ld", pPrint->wt_date);
	strncpy(pHead->pay_qs_no, pPrint->pay_qs_no , sizeof(pHead->pay_qs_no)-1);
	sprintf(pHead->dz_date  , "%08ld" ,pPrint->dz_date);
	sprintf(pHead->dtlcnt   , "%08ld", pPrint->dtlcnt);
}

int iLvPre852ForCzyh_dz(long dz_date,long wt_date,char * pay_qs_no)
{
	int ret=0;
	struct lv_czyh_dz_c sDz;
	struct str_lv_852_head sHead;
	struct str_lv_852_head_print sHeadPrint;
	struct str_lv_852_mx_dtl sDtl;
	struct str_lv_852_mx_dtl_print sMxPrint;
	char cBuf[32 * 100 +1];
	
	memset(&sHead     , 0 , sizeof(sHead));
	memset(&sDtl      , 0 , sizeof(sDtl));
	memset(&sHeadPrint, 0 , sizeof(sHeadPrint));
	memset(&sMxPrint  , 0 , sizeof(sMxPrint));
	memset(&sDz, 0 , sizeof(sDz));
	memset(cBuf, 0 , sizeof(cBuf));
	ret = Lv_czyh_dz_Dec_Sel(g_pub_tx.reply, " lw_ind='2' and check_flag='2' and tx_date=%08ld",g_pub_tx.tx_date);
	if(ret)
	{
		vtcp_log("%s, %d 定义村镇银行对账表游标错误,ret=%d",__FILE__,__LINE__,ret);
		return ret;
	}
	sHeadPrint.wt_date=wt_date;
	strncpy(sHeadPrint.pay_qs_no,pay_qs_no,12);
	sHeadPrint.dz_date = dz_date;
	sHeadPrint.dtlcnt = 0;
	vtcp_log("%s,%d",__FILE__,__LINE__);
	while(1)
	{
		ret = Lv_czyh_dz_Fet_Sel( &sDz , g_pub_tx.reply );
		if(ret == 100)
			break;
		if(ret)
		{
			vtcp_log("%s, %d 定义村镇银行对账表游标错误,ret=%d",__FILE__,__LINE__,ret);
			return ret;
		}
		
		sMxPrint.lw_ind[0]='2';
		strncpy(sMxPrint.pkg_no,sDz.pack_type,sizeof(sMxPrint.pkg_no)-1);
		sMxPrint.pack_date = sDz.pack_date;
		strncpy(sMxPrint.pay_qs_no,sDz.pay_qs_no,sizeof(sMxPrint.pay_qs_no)-1);
		strncpy(sMxPrint.packid  , sDz.pack_id,sizeof(sMxPrint.packid)-1);
		str_print852mx_to_dtl852(&sDtl,&sMxPrint);
		memcpy(cBuf+sHeadPrint.dtlcnt*32,(char *)&sDtl,32);
		vtcp_log("%s,%d buf == [%s]",__FILE__,__LINE__,cBuf);
		sHeadPrint.dtlcnt++;
	}
	Lv_czyh_dz_Clo_Sel();
	vtcp_log("%s,%d dtlcnt = %d",__FILE__,__LINE__,sHeadPrint.dtlcnt);
	if(sHeadPrint.dtlcnt !=0)
	{
		/* 组织发送报文 */
		TLVPACKAGE lv852;
		int iSocketID=0;
		char respCode[9];
		char respBuf [100];
		memset(&lv852,0,sizeof(TLVPACKAGE));
		
		memset(respCode, 0 , sizeof(respCode));
		memset(respBuf , 0 , sizeof(respBuf));
		
		str_print852head_to_head(&sHead,&sHeadPrint);
		vtcp_log("%s,%d head== dz_date=%s",__FILE__,__LINE__,sHead.dz_date);
		lv_create(&lv852);

		/* 2 处理其他状态的包 */
		vtcp_log("[%s][%d] g_pub_tx.tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);
		lv852.opeType = '1';
		lv852.pkgType = 0;
		memcpy(lv852.CMTCode,"852",3);
		memcpy(lv852.workDate,pcLvGetDay(),8);
		vtcp_log("%s,%d head== dz_date=%s",__FILE__,__LINE__,sHead.dz_date);
		lv_set_tag(&lv852,"30A",sHead.wt_date);
		lv_set_tag(&lv852,"B10",sHead.pay_qs_no);
		lv_set_tag(&lv852,"B11",sHead.dz_date);
		lv_set_tag(&lv852,"B67",sHead.dtlcnt);
		lv_set_tag(&lv852,"B87",cBuf);
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		lv852.opeType = '1';   /** 小额 **/
		lv852.pkgType = 0;     /** 是cmt包 **/
		memcpy(lv852.packDate, pcLvGetDay(), sizeof(lv852.packDate));
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		lv_gen_pack(&lv852);
		ret=lv_initPara(NULL);
		if (ret)
		{
			vtcp_log("%s,%d,初始化网络参数错误 return = %d!",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"初始化网络参数错误!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			return ret;
		}
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		ret = lv_connect(LV_SERVER_BATCH);
		iSocketID = ret;
		if (iSocketID<=0)
		{
			vtcp_log("%s,%d,连接前置机错误 return = %d!",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"连接前置机错误!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			return iSocketID;
		}
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		/* 2 处理其他状态的包 */
		ret = lv_send(ret,&lv852);
		if (ret)
		{
			vtcp_log("%s,%d,发送数据包到前置机出错 return = %d!",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"发送数据包到前置机出错!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			return ret;
		}
		lv_reset(&lv852);

		ret = lv_recv(iSocketID,&lv852);
		if (ret)
		{
			vtcp_log("%s,%d,接收数据包到前置机出错 return = %d!",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"从前置机接收数据包出错!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			return ret;
		}
		lv_get_tag(&lv852,"C42",respCode);
		lv_get_tag(&lv852,"72A",respBuf);
		if ( respCode[3]!='I' )
		{
			vtcp_log("%s,%d,报文发送到前置机失败 ERRNO=%s\nERRNOINFO=%s!",__FILE__,__LINE__,respCode,respBuf);
			sprintf(acErrMsg,respBuf);
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_netClose(iSocketID);
			lv_destroy(&lv852);
			return -1;
		}
		lv_netClose(iSocketID);
		lv_destroy(&lv852);
	}
	vtcp_log("%s,%d",__FILE__,__LINE__);
	return 0;
}
