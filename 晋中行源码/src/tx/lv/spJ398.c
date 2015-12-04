/******************************************************************
模块名称: 交易汇总信息查询
程序名称: spJ398.c
函数原型: spJ398()
输入参数:
输出参数:
使用说明:
编 写 者: dxy
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "hv_zf_c.h"
   
   



char pcTmpBuf[5];
static struct TIS6560019_GROUP {
	char brno[5];	/**交易机构**/
	char wtday[8];	/**委托日期**/
}tis19;
 struct TQINFO1 {
	char wzlr[4];		/**登记数量**/
	char wzcheck2[4];		/**复核数量**/
	char wzsend[4];		/**发送数量**/
	char wzinqueue[4];		/**排队数量**/
	char wzreject[4];		/**拒绝数量**/
	char wzcancel[4];		/**撤销数量**/
	char wzsuccess[4];	/**往帐扎差数量**/
	char lzsuccess[4];	/**来帐扎差数量**/
	char lzgz[4];		/**来帐挂帐数量**/
	char lzback[4];		/**来帐退回数量**/
};
 struct TQINFO2{
	char wzlr[4];			/**登记数量**/
	char wzcheck2[4];		/**复核数量**/
	char wzsend[4];			/**发送数量**/
	char wzreject[4];		/**拒绝数量**/
	char wzzf[4];			/**止付数量**/
	char wzcz[4];			/**冲正数量**/
	char wzsuccess[4];		/**扎差数量**/
	char lzagree[4];		/**同意数量**/
	char lznotagree[4];		/**不同意数量**/
	char lzother[4];		/**其他**/
};
 struct TQINFO3{
	char wzback[4];		/**往帐已回复**/
	char wznotback[4];	/**往帐未回复**/
	char wzother[4];		/**往帐其他**/
	char lzback[4];		/**来帐已回复**/
	char lznotback[4];	/**来帐未回复**/
	char lzother[4];		/**来帐其他**/
};
/**
 struct TOS6560019_GROUP {
 	
	TQINFO1 ptdj;
	TQINFO1 dqdj;
	TQINFO1 djth;
	TQINFO1 ptjjhz;
	TQINFO1 dqjjhz;
	TQINFO1 ssjjhz;
	TQINFO1 ssdjhz;
	TQINFO2 ptjj;
	TQINFO2 dqjj;
	TQINFO2 ssjj;
	TQINFO2 ssdj;
	TQINFO2 tyxx;
	TQINFO2 ssxx;
	TQINFO3 query;
	TQINFO3 free;
	TQINFO3 askback;	
	TQINFO3 askcancel;	
	TQINFO3 askzf;	
	TQINFO3 askcz;	
	TQINFO3 czmsg;	
	
};	
**/


  
 int iQueryInfo1(char *pkgno,struct TQINFO1 *st);
 int iQueryInfo2(char *pkgno,struct TQINFO2 *st);
 int iQueryInfo3(char *pkgno,struct TQINFO3 *st);
 char * iQueryStatCnt(char *pkgno,int sendflag,char *statlist);
 int dh6560019_Process();
/*************************************************************************
*                                                                        *
*   函数名:         dh6560019_Work()                                      *
*   函数说明:       1.初始化                                             *
*                   2.校验卡号                                 			 *
*                   3.查询                                 			 	 *
*                   4.组织返回包                              		 	 *
*                   5.给前台返回包                                       *
*                                                                        *
*************************************************************************/
int spJ398()
{
	vtcp_log("[%s][%d]汇总交易交易查询\n",__FILE__,__LINE__);
        LV_FD123_AREA fd123;
	int iRc = 0;
	/*struct TOS6560019_GROUP tos19 ;;*/
	struct TQINFO1 ptdj;
	
	memset(&fd123,0x00,sizeof(fd123));
	/*memset(&tos19,0x00,sizeof(tos19));*/
	memset(&tis19,0x00,sizeof(tis19));
	memset(&ptdj,0x00,sizeof(ptdj));
	
	memset(pcTmpBuf,0x00,sizeof(pcTmpBuf));
	
	
	
	get_zd_data("0910",tis19.brno);
	get_zd_data("0440",tis19.wtday);
	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) 
		&& ((tis19.brno[0]=='\0' ||tis19.brno[0]==' ')|| memcmp(g_pub_tx.tx_br_no,tis19.brno,5)))
	{
		vtcp_log("%s,%d,非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d]tis19===[%s]\n",__FILE__,__LINE__,(char *)&tis19);

	vtcp_log("[%s][%d]开始执行汇总查询主程序!\n",__FILE__,__LINE__);
	/*
	if((iRc=iQueryInfo1(PKGNO_PTDJ,&ptdj)))	return(-1);
	vtcp_log("[%s][%d]iRc==[%d]\n",__FILE__,__LINE__,iRc); 
	*/
	vtcp_log("[%s][%d]fuck!\n",__FILE__,__LINE__);
	memcpy(ptdj.wzlr,iQueryStatCnt("001",1,"lv_sts = '4' "),4);
	vtcp_log("[%s][%d]fuck!\n",__FILE__,__LINE__);
	/*
	if((iRc=iQueryInfo1(PKGNO_DQDJ,&tos19.dqdj)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo1(PKGNO_DJTH,&tos19.djth)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo1(PKGNO_PTJJHZ,&tos19.ptjjhz)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo1(PKGNO_DQJJHZ,&tos19.dqjjhz)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo1(PKGNO_SSJJHZ,&tos19.ssjjhz)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo1(PKGNO_SSDJHZ,&tos19.ssdjhz)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_PTJJ,&tos19.ptjj)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_DQJJ,&tos19.dqjj)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_SSJJ,&tos19.ssjj)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_SSDJ,&tos19.ssdj)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_TYXX,&tos19.tyxx)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo2(PKGNO_SSXX,&tos19.ssxx)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("301",&tos19.query)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("303",&tos19.free)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("319",&tos19.askback)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("321",&tos19.askcancel)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("327",&tos19.askzf)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	
	if((iRc=iQueryInfo3("323",&tos19.askcz)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	if((iRc=iQueryInfo3("325",&tos19.czmsg)))	return(-1);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	*/
	vtcp_log("[%s][%d]开始执行汇总查询主程序!\n",__FILE__,__LINE__);
	
	vtcp_log("[%s][%d]ptdj===[%s]\n",__FILE__,__LINE__,(char *)&ptdj);
	
	memcpy(&fd123,&ptdj,sizeof(ptdj));
	
	
	
	
	set_zd_data("1230",(char *)&fd123);
	
	vtcp_log("[%s][%d]fd123====[%s]\n",__FILE__,__LINE__,(char *)&fd123);
	
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"查询汇总交易成功[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

} 



/******************************************************************************
*   函数名称:          dh6560019_Process()                                  	  *
*                                                                             *
*   函数说明:         查询                                    				  *
******************************************************************************/

int iQueryInfo1(char *pkgno,struct TQINFO1 *st){
	
	
	int iRc = 0;
	char pcStatList[16];
	
	memset(pcStatList,0x00,sizeof(pcStatList));
	
	
	
	vtcp_log("[%s][%d]sizeof(TQINFO1)===[%d]\n",__FILE__,__LINE__,sizeof(*st));
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	
	sprintf(pcStatList," lv_sts  ='%c' ",STAT_WZLR);
	memcpy(st->wzlr,iQueryStatCnt(pkgno,1,pcStatList),4);
	/***
	sprintf(pcStatList," lv_sts = '%c' ",STAT_WZCHECK2);
	memcpy(st->wzcheck2,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	vtcp_log("[%s][%d]iRc===[%d]\n",__FILE__,__LINE__,iRc);
	vtcp_log("[%s][%d]st===[%s]\n",__FILE__,__LINE__,(char *)st);
	
	
	
	sprintf(pcStatList," lv_sts  ='%c' ",STAT_WZSEND);
	memcpy(st->wzsend,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZINQUEUE);
	memcpy(st->wzinqueue,iQueryStatCnt(pkgno,1,pcStatList),4);
	vtcp_log("[%s][%d]iRc===[%d]\n",__FILE__,__LINE__,iRc);

	
	
	
	vtcp_log("[%s][%d]st===[%s]\n",__FILE__,__LINE__,(char *)st);
	

	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZREJECT);
	memcpy(st->wzreject,iQueryStatCnt(pkgno,1,pcStatList),4);
	vtcp_log("[%s][%d]iRc===[%d]\n",__FILE__,__LINE__,iRc);
	
	vtcp_log("[%s][%d]st===[%s]\n",__FILE__,__LINE__,(char *)st);
	
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZCANCEL);
	memcpy(st->wzcancel,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts  in('%c','%c') ",STAT_WZSUCCESS,STAT_WZBACK);
	memcpy(st->wzsuccess,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts  in('%c','%c') ",STAT_LZRECV2,STAT_LZRECV1);
	memcpy(st->lzsuccess,iQueryStatCnt(pkgno,0,pcStatList),4);
	
		
		
	sprintf(pcStatList," lv_sts  in('%c','%c','%c','%c','%c','%c') ",STAT_LZNMAC_ERR,STAT_LZLMAC_ERR,STAT_LZAMAC_ERR,STAT_LZGZ_QS,STAT_LZGZ_BR,STAT_LZBACK_WAIT);
	memcpy(st->lzgz,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_LZBACK);
	memcpy(st->lzback,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	vtcp_log("[%s][%d]iQueryInfo1正常结束!\n",__FILE__,__LINE__);

	vtcp_log("[%s][%d]st===[%s]\n",__FILE__,__LINE__,(char *)st);
*****/
	vtcp_log("[%s][%d]xierixer\n",__FILE__,__LINE__); 
	vtcp_log("[%s][%d]",__FILE__,__LINE__); 
	return 0;	
	
	vtcp_log("[%s][%d]",__FILE__,__LINE__); 	
}
 int iQueryInfo2(char *pkgno,struct TQINFO2 *st){
	/**
	int iRc = 0;	
	char pcStatList[16];
	
	memset(pcStatList,0x00,sizeof(pcStatList));
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZLR);
	memcpy(st->wzlr,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZCHECK2);
	memcpy(st->wzcheck2,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZSEND);
	memcpy(st->wzsend,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZREJECT);
	memcpy(st->wzreject,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZZF);
	memcpy(st->wzzf,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZCZ);
	memcpy(st->wzcz,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts ='%c' ",STAT_WZSUCCESS);
	memcpy(st->wzsuccess,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," lv_sts in('%c','%c') ",STAT_YES_NOTSEND,STAT_YES_SEND);
	memcpy(st->lzagree,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	sprintf(pcStatList," lv_sts in('%c','%c') ",STAT_NO_NOTSEND,STAT_NO_SEND);
	memcpy(st->lznotagree,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	sprintf(pcStatList," lv_sts in('%c','%c','%c') ",STAT_LZRECV1,STAT_LZRECV2,STAT_LZBACK);
	memcpy(st->lzother,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	vtcp_log("[%s][%d]iQueryInfo2正常结束!\n",__FILE__,__LINE__);

	vtcp_log("[%s][%d]st===[%s]\n",__FILE__,__LINE__,(char *)st);
	**/
	
	return(0);
}
 int iQueryInfo3(char *pkgno,struct TQINFO3 *st){
 	/**
	int iRc = 0;	
	char pcStatList[16];
	memset(st,0x00,sizeof(*st));
	memset(pcStatList,0x00,sizeof(pcStatList));
	
	
	sprintf(pcStatList," hv_fzf_sts  ='%c' ",STAT_WZBACK);
	memcpy(st->wzback,iQueryStatCnt(pkgno,1,pcStatList),4);

	sprintf(pcStatList," hv_fzf_sts ='%c' ",STAT_WZSEND);
	memcpy(st->wznotback,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," hv_fzf_sts not in('%c','%c','%c') ",STAT_INVALID,STAT_WZBACK,STAT_WZSEND);
	memcpy(st->wzother,iQueryStatCnt(pkgno,1,pcStatList),4);
	
	sprintf(pcStatList," hv_fzf_sts ='%c' ",STAT_LZBACK);
	memcpy(st->lzback,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	sprintf(pcStatList," hv_fzf_sts ='%c' ",STAT_LZRECV1);
	memcpy(st->lznotback,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	sprintf(pcStatList," hv_fzf_sts not in('%c','%c','%c') ",STAT_INVALID,STAT_LZRECV1,STAT_LZBACK);
	memcpy(st->lzother,iQueryStatCnt(pkgno,0,pcStatList),4);
	
	**/
	return(0);
}

char * iQueryStatCnt(char *pkgno,int sendflag,char *statlist){

	
	char sql_condition[256];
	int sql_cnt = 0;
	char cTmplist[100];
	char cRecordNum[4];
	char cBr_no[6];
	
	memset(cBr_no,0x00,sizeof(cBr_no));
	memset(cRecordNum,0x00,sizeof(cRecordNum));
	memset(sql_condition,0x00,sizeof(sql_condition));
	memset(cTmplist,0x00,sizeof(cTmplist));
	
	
	memcpy(cBr_no,tis19.brno,sizeof(tis19.brno));
	zip_space(cBr_no);
	
	vtcp_log("[%s][%d]开始报文【%s】的处理，sql==[%s]\n",__FILE__,__LINE__,pkgno,statlist);

	sql_cnt=0;
	if(pkgno[0]=='0')
	{
		sprintf(sql_condition," wt_date ='%.8s' and pkgno='%.3s' and br_no like '%%%s%%' and ",tis19.wtday,pkgno,cBr_no);
	}
	else
	{/****cmtno业务***/
		sprintf(sql_condition," otype = '2' and wt_date='%.8s' and  br_no like '%%%s%%' and ",tis19.wtday,cBr_no);
		if(apatoi(pkgno,3)%2!=0)/***申请类交易***/
		{
			sprintf(cTmplist,"cmtno = '%.3s' ",pkgno);
			strcat(sql_condition,cTmplist);
		}
		else if(apatoi(pkgno,3)%2==0)/**应答类交易**/
		{
			sprintf(cTmplist,"cmtno = '%d' and res_orderno is not null and res_date <>0 ",apatoi(pkgno)-1);
			strcat(sql_condition,cTmplist);
		}
			
	}
	
	if(sendflag)/***如果是往帐****/
		strcat(sql_condition," and lw_ind = '1' and  ");
	else 
		strcat(sql_condition," and lw_ind = '2' and ");
	
	
	strcat(sql_condition,statlist);
		
	vtcp_log("[%s][%d]sql_condition==[%s]\n",__FILE__,__LINE__,sql_condition);
	vtcp_log("[%s][%d]pkgno===[%s]\n",__FILE__,__LINE__,(char *)pkgno);

	if(pkgno[0]=='0')/***是pkg报文的查询***/
	{
		vtcp_log("[%s][%d]funck!\n",__FILE__,__LINE__);
		 sql_cnt = sql_count("lv_pkgreg",sql_condition);
		
	}
	else 
	{
		vtcp_log("[%s][%d]funck!\n",__FILE__,__LINE__);
		sql_cnt = sql_count("hv_fzf",sql_condition);
	}
	vtcp_log("[%s][%d]liuyue\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]sql_cnt==[%d]\n",__FILE__,__LINE__,sql_cnt);
	
	if(sql_cnt<=0)
		memset(cRecordNum,'0',4);
	else 
		sprintf(cRecordNum,"%04d",sql_cnt);
	return cRecordNum;
		
}

 int iSetNum4(char *buf,int num){
	
	/**memset(buf,0x00,4);
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	if(num<=0){
		memset(buf,'0',4);
	}else{
		sprintf(pcTmpBuf,"%04d",num);
		memcpy(buf,pcTmpBuf,4);
	}
	vtcp_log("[%s][%d]buf====[%s]\n",__FILE__,__LINE__,*buf);**/
	return(0);
}

