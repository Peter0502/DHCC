/*****************************************************
* �� �� ��:  spJ407.c
* ���������� С��ҵ�����ϸ�˶��������뱨��852��������
* ��    ��:  LiuHuafeng
* ������ڣ� 2008-11-12 16:12
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
* �е���ǰ�汾
* insert into tx_def values('5831','�ֹ�����852','10000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000100000','1','3','2');
* �е��Ժ�汾
* insert into tx_def values('5831','�ֹ�����852','10000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000100000','1','3','2','3',null);
* ����
* insert into tx_flow_def values('5831',0,'J407','#$');
* insert into tx_sub_def values('J407','�ֹ�����852','spJ407','0','0');
* 
*****************************************************/
#include <stdlib.h>
#include <stdio.h>
#define EXTERN
#include "string.h"
#include "lv_define.h"
#include "hv_define.h"
#include "exitlvpack.h"
#include "lv_pkgreg_c.h"
#include "lv_sysctl_c.h"
#include "public.h"
extern char * pcLvGetDay();
#define MXHD850_MAX_ZCCC 10
char cDzday[9];
int spJ407()
{
	int iRc,i,iQscnt=0;
	int iSocketID=0;
	char cQscnt[9];/* ��ϸ������Ŀ */
	char cBrno[BRNO_LEN+1];

	char cTxday[9],cDzday[9];
	char respCode[9];
	char respBuf [100];
	char cQs_br_no[13];
	char cOneBuf[33];
	char cLw_ind    [2];
	char cPkgno     [4];
	char cOr_br_no  [13];/* ������������*/
	char cPack_date [9];
	char cPack_id   [9];
	char cTmpbuf[100];
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	memset(cQs_br_no , 0 , sizeof(cQs_br_no));
	find_qs_no((char*)NULL,cQs_br_no);
	memset(cDzday,0 , sizeof(cDzday));
	memset(cDzday,0x00,sizeof(cDzday));
	memset(cOneBuf,0 ,sizeof(cOneBuf));
	TLVPACKAGE lv852;
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	memset(cLw_ind   ,0 , sizeof(cLw_ind   ));
	memset(cPkgno    ,0 , sizeof(cPkgno    ));
	memset(cOr_br_no ,0 , sizeof(cOr_br_no ));
	memset(cPack_date,0 , sizeof(cPack_date));
	memset(cPack_id  ,0 , sizeof(cPack_id  ));

	iQscnt=0;


	memset(respCode , 0 , sizeof(respCode));
	memset(respBuf  , 0 , sizeof(respBuf ));
	memset(cTxday   , 0 , sizeof(cTxday));
	memset(cBrno    , 0 , sizeof(cBrno));
	get_fd_data("0030",cBrno);
	get_fd_data("0470",cDzday);
	get_fd_data("0670",cLw_ind);
	get_fd_data("0240",cPkgno);
	get_fd_data("0580",cTmpbuf);
	memcpy(cOr_br_no,cTmpbuf,sizeof(cOr_br_no)-1);
	get_fd_data("0440",cPack_date);
	get_fd_data("0450",cPack_id);
	sprintf(cPack_id,"%08ld",atol(cPack_id));
	sprintf(cOneBuf,"%.1s%.3s%.12s%.8s%.8s",cLw_ind,cPkgno,cOr_br_no,cPack_date,cPack_id);
	vtcp_log("[%s][%d] dzday==[%s]",__FILE__,__LINE__,cDzday);
	vtcp_log("[%s][%d] cLw_ind==[%s]",__FILE__,__LINE__,cLw_ind);
	vtcp_log("[%s][%d] cPkgno==[%s]",__FILE__,__LINE__,cPkgno);
	vtcp_log("[%s][%d] cOr_br_no==[%s]",__FILE__,__LINE__,cOr_br_no);
	vtcp_log("[%s][%d] cPack_date==[%s]",__FILE__,__LINE__,cPack_date);
	vtcp_log("[%s][%d] cPack_id==[%s]",__FILE__,__LINE__,cPack_id);
	vtcp_log("[%s][%d] cOneBuf==[%s]",__FILE__,__LINE__,cOneBuf);
	vtcp_log("%s,%d qs_br_no===%s br_no=%s",__FILE__,__LINE__,cQs_br_no,cBrno);
	if(memcmp(cBrno,QS_BR_NO,BRNO_LEN))
	{
		vtcp_log("[%s][%d] ���������Ĳ�������ҵ��",__FILE__,__LINE__);
		sprintf(acErrMsg,"���������Ĳ�������ҵ��");
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		    goto ErrExit;
	}
	memcpy(cTxday,pcLvGetDay(),sizeof(cTxday)-1);
	/**	memcpy(cTxday,cDzday,sizeof(cTxday)-1);**/

	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	memset(cQscnt , 0 , sizeof(cQscnt));
	memcpy(cQscnt,"00000001",8);
	{
		/*********���ݳ�ʼ��******************/
		memset(&lv852,0,sizeof(TLVPACKAGE));
		lv_create(&lv852);

		lv_set_tag(&lv852,"B11",cTxday); /**today****/
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		/* 2 ��������״̬�İ� */
		apitoa(g_pub_tx.tx_date, 8, cTxday);
		vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);
		vtcp_log("[%s][%d] g_pub_tx.tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);
		vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);
		lv852.opeType = '1';
		lv852.pkgType = 0;
		memcpy(lv852.CMTCode,"852",3);
		memcpy(lv852.workDate,pcLvGetDay(),8);
		lv_set_tag(&lv852,"30A",cTxday);
		lv_set_tag(&lv852,"B10",cQs_br_no);
		lv_set_tag(&lv852,"B11",cDzday);
		lv_set_tag(&lv852,"B67",cQscnt);
		lv_set_tag(&lv852,"B87",cOneBuf);

		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		lv852.opeType = '1';   /** С�� **/
		lv852.pkgType = 0;     /** ��cmt�� **/
		memcpy(lv852.packDate, pcLvGetDay(), sizeof(lv852.packDate));
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		lv_gen_pack(&lv852);
		iRc=lv_initPara(NULL);
		if (iRc)
		{
			vtcp_log("%s,%d,��ʼ������������� return = %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"��ʼ�������������!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			goto ErrExit;
		}
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		iRc = lv_connect(LV_SERVER_BATCH);
		iSocketID = iRc;
		if (iSocketID<=0)
		{
			vtcp_log("%s,%d,����ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"����ǰ�û�����!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			goto ErrExit;
		}
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		/* 2 ��������״̬�İ� */
		iRc = lv_send(iRc,&lv852);
		if (iRc)
		{
			vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"�������ݰ���ǰ�û�����!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			goto ErrExit;
		}
		lv_reset(&lv852);

		iRc = lv_recv(iSocketID,&lv852);
		if (iRc)
		{
			vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"��ǰ�û��������ݰ�����!");
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_destroy(&lv852);
			lv_netClose(iSocketID);
			goto ErrExit;
		}
		lv_get_tag(&lv852,"C42",respCode);
		lv_get_tag(&lv852,"72A",respBuf);
		if ( respCode[3]!='I' )
		{
			vtcp_log("%s,%d,���ķ��͵�ǰ�û�ʧ�� ERRNO=%s\nERRNOINFO=%s!",__FILE__,__LINE__,respCode,respBuf);
			sprintf(acErrMsg,respBuf);
			WRITEMSG
			    strcpy( g_pub_tx.reply, "HV01" );
			set_zd_data("0130",acErrMsg);
			lv_netClose(iSocketID);
			lv_destroy(&lv852);
			goto ErrExit;
		}
		lv_netClose(iSocketID);
		lv_destroy(&lv852);
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
