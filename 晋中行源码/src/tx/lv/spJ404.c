/*****************************************************
* �� �� ��:  spJ404.c
* ���������� ��������С��ҵ�����ϸ�˶����뱨��850
* ��    ��:  xyy
* ������ڣ� 2006-11-26 23:15
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
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
char cQry_date[9];
int spJ404()
{
	int iRc,i,iQscnt=0;
	int cnt=0;
	int recnt,index;
	int iSocketID=0;
	char cQscnt[7];/* �Ѿ�����״̬��Ŀ */
	char cBrno[BRNO_LEN+1];

	char cTxday[9];
	char respCode[9]; 
	char cPackdate[9];
	char respBuf [100];
	


	memset(cQry_date,0x00,sizeof(cQry_date));
	TLVPACKAGE lv850;
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

	iQscnt=0;

	memset(cPackdate, 0 , sizeof(cPackdate));
	memset(respCode , 0 , sizeof(respCode));
	memset(respBuf  , 0 , sizeof(respBuf ));
	memset(cTxday   , 0 , sizeof(cTxday));
	memset(cBrno    , 0 , sizeof(cBrno));
	get_fd_data("0030",cBrno);

	get_fd_data("0470",cQry_date);

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
	
	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d] cTxday==[%s]",__FILE__,__LINE__,cTxday);
	cnt = sql_count("lv_chk842_d","zc_date=%s",cTxday);
  if(!cnt)
  {
  	  vtcp_log("[%s][%d] û���յ�842���Ĳ�������850",__FILE__,__LINE__);
	    sprintf(acErrMsg,"û���յ�842���Ĳ�������850");
	    strcpy( g_pub_tx.reply, "HV01" );
	    set_zd_data("0130",acErrMsg);
	    WRITEMSG
	    goto ErrExit;
  }
  else
  {
	    /*********���ݳ�ʼ��******************/
	    memset(&lv850,0,sizeof(TLVPACKAGE));	
	    lv_create(&lv850);
	    
	    memcpy(cTxday,pcLvGetDay(),sizeof(cTxday)-1);  /***liuyue  30A  need this day***/

	    lv_set_tag(&lv850,"30A",cTxday);
	    lv_set_tag(&lv850,"B10",LV_QSBRNO);
	    memset(cTxday, 0 , sizeof(cTxday));
      
	    vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);
	    vtcp_log("[%s][%d] g_pub_tx.tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);

	    /*******
	    memcpy(cTxday,cQry_date,8);
      *******/
	    vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);
      
	    apitoa(g_pub_tx.tx_date, 8, cTxday);	
	    vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);
	    lv_set_tag(&lv850,"B11",cTxday); /**today****/
	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    /* 2 ��������״̬�İ� */
      
	    lv850.opeType = '1';
	    lv850.pkgType = 0;
	    memcpy(lv850.CMTCode,"850",3);
	    memcpy(lv850.workDate,pcLvGetDay(),8);
      
	    memset(cQscnt , 0 , sizeof(cQscnt));
	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    /* 2 ��������״̬�İ� */
	    g_reply_int=iPrepareMxdzDtl(&lv850,&iQscnt);
	    if (g_reply_int)
	    {
	    	vtcp_log("[%s][%d] ׼��850���Ĵ���errcode=[%d]",__FILE__,__LINE__,g_reply_int);
	    	sprintf(acErrMsg,"׼��850���Ĵ���!");
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	WRITEMSG
	    	lv_destroy(&lv850);
	    	goto ErrExit;
	    }
	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    sprintf(cQscnt,"%06d",iQscnt);
	    lv_set_tag(&lv850,"B65",cQscnt);
	    lv_set_tag(&lv850,"B66","000000");
	    lv850.opeType = '1';   /** С�� **/
	    lv850.pkgType = 0;     /** ��cmt�� **/
	    memcpy(lv850.packDate, pcLvGetDay(), sizeof(lv850.packDate));

	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    /* 2 ��������״̬�İ� */
	    lv_gen_pack(&lv850);
	    iRc=lv_initPara(NULL);
	    if (iRc)
	    {
	    	vtcp_log("%s,%d,��ʼ������������� return = %d!",__FILE__,__LINE__,iRc);
	    	sprintf(acErrMsg,"��ʼ�������������!");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	lv_destroy(&lv850);
	    	goto ErrExit;
	    }
	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    /* 2 ��������״̬�İ� */
	    iRc = lv_connect(LV_SERVER_BATCH);
	    iSocketID = iRc;
	    if (iSocketID<=0)
	    {
	    	vtcp_log("%s,%d,����ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
	    	sprintf(acErrMsg,"����ǰ�û�����!");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	lv_destroy(&lv850);
	    	lv_netClose(iSocketID);
	    	goto ErrExit;
	    }
	    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	    /* 2 ��������״̬�İ� */
	    iRc = lv_send(iRc,&lv850);
	    if (iRc)
	    {
	    	vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
	    	sprintf(acErrMsg,"�������ݰ���ǰ�û�����!");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	lv_destroy(&lv850);
	    	lv_netClose(iSocketID);
	    	goto ErrExit;
	    }
	    lv_reset(&lv850);
      
	    iRc = lv_recv(iSocketID,&lv850);
	    if (iRc)
	    {
	    	vtcp_log("%s,%d,�������ݰ���ǰ�û����� return = %d!",__FILE__,__LINE__,iRc);
	    	sprintf(acErrMsg,"��ǰ�û��������ݰ�����!");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	lv_destroy(&lv850);
	    	lv_netClose(iSocketID);
	    	goto ErrExit;
	    }
	    lv_get_tag(&lv850,"C42",respCode);
	    lv_get_tag(&lv850,"72A",respBuf);
	    if ( respCode[3]!='I' )
	    {
	    	vtcp_log("%s,%d,���ķ��͵�ǰ�û�ʧ�� ERRNO=%s\nERRNOINFO=%s!",__FILE__,__LINE__,respCode,respBuf);
	    	sprintf(acErrMsg,respBuf);
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	set_zd_data("0130",acErrMsg);
	    	lv_netClose(iSocketID);
	    	lv_destroy(&lv850);
	    	goto ErrExit;
	    }
	    lv_netClose(iSocketID);
	    lv_destroy(&lv850);
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
int iPrepareMxdzDtl(TLVPACKAGE *lv,int *qscnt)
{
	char h_zcnode[2];
	char h_zc_date [9];
	char h_zcid  [3];
	char h_pkgno [4];
	char h_lw_flg  [2];
	char h_pack_sts[3];
	MXHD_850_QS str_mxhd_850_qs;
	MXHD_850_QT str_mxhd_850_qt;
	int iQscnt=0;

	memset(&str_mxhd_850_qs, 0 , sizeof(str_mxhd_850_qs));

	memset(h_zcid  , 0,sizeof(h_zcid  ));
	memset(h_pkgno , 0,sizeof(h_pkgno ));
	memset(h_lw_flg  , 0,sizeof(h_lw_flg  ));
	memset(h_pack_sts, 0 , sizeof(h_pack_sts));

	vtcp_log("[%s][%d]code=[%s] \n",__FILE__,__LINE__,lv->CMTNo);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	
	char cBuff850_qs[15*28*(MXHD850_MAX_ZCCC+1)+1];
	memset(cBuff850_qs, 0 ,sizeof(cBuff850_qs));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	vCreateMxhd850_qs_buff(cBuff850_qs);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	vtcp_log("[%s][%d] buff=[%s]\n",__FILE__,__LINE__,cBuff850_qs);
	lv_set_tag(lv,"B85",cBuff850_qs);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	*qscnt=28*MXHD850_MAX_ZCCC;

	/*�ύ����ȫ���ı��� */
	lv_set_tag(lv,"B86","\0");
	return 0;
}
vCreateMxhd850_qs_buff(char * buff)
{
	/* ��Ƶ�����İ��� ���� 001 005 007 008 010 011 
	������ 001 005 007 008 009 010 011
	*/
	char cBuff[15*28*MXHD850_MAX_ZCCC+1];
	int i=0;
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	memset(cBuff, 0 ,sizeof(cBuff));
	for (i=0;i<MXHD850_MAX_ZCCC;i++)
	{
		char czc_cc[3];
		memset(czc_cc, 0 , sizeof(czc_cc));
		sprintf(czc_cc,"%02d",i+1);
		vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'1','1');
		vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'2','1');
		vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'2','1');

		vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'1','2');
		vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'1','2');	
		vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'2','2');
		vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'2','2');
	}
	vtcp_log("[%s][%d]cBuff=[%s] \n",__FILE__,__LINE__,cBuff);
	memcpy(buff,cBuff,strlen(cBuff));
	buff[strlen(cBuff)]='\0';
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	return 0;
}
vSetMxhd850_qt(int buf_len,char * pkgno,char * pack_sts,char * cBuff,char lwbs)
{
	MXHD_850_QT str_mxhd_850_qt;
	memset(&str_mxhd_850_qt,0,sizeof(str_mxhd_850_qt));
	str_mxhd_850_qt.lwbs[0]=lwbs;
	memcpy(str_mxhd_850_qt.pkgno,pkgno,3);
	memcpy(str_mxhd_850_qt.packstat,pack_sts,2);
	memcpy(cBuff+buf_len,(char*)&str_mxhd_850_qt,sizeof(str_mxhd_850_qt));
	return 0;
}

vSetMxhd850_qs(int buf_len,char * pkgno,char * zc_cc,char * cBuff,char zc_jd_lx,char lwbs)
{
	MXHD_850_QS str_mxhd_850_qs;
	memset(&str_mxhd_850_qs,0,sizeof(str_mxhd_850_qs));
	str_mxhd_850_qs.lwbs[0]=lwbs;
	str_mxhd_850_qs.zcjdlx[0]=zc_jd_lx;

	memcpy(str_mxhd_850_qs.zcday,cQry_date,sizeof(str_mxhd_850_qs));
	memcpy(str_mxhd_850_qs.zccc,zc_cc,sizeof(str_mxhd_850_qs.zccc));
	memcpy(str_mxhd_850_qs.pkgno,pkgno,sizeof(str_mxhd_850_qs.pkgno));
	memcpy(cBuff+buf_len,(char*)&str_mxhd_850_qs,sizeof(str_mxhd_850_qs));
	return 0;
}
