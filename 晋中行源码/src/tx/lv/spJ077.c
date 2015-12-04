/************************************************************
* 文 件 名:   spJ077.c
* 功能描述：  项目号维护
*
* 作    者:   chenhw
* 完成日期：  2006-9-15
*
* 修改记录：
* 日    期:   2006-12-11 15:14
* 修 改 人:   xyy
* 修改内容:
**************************************************************/
#define MYSQLERR if( ret ) {\
 strcpy( g_pub_tx.reply,"P046" );\
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
#include "lv_xmh_c.h"
#include "lv_define.h"
#include "public.h"


int spJ077()
{
	int ret;

	char cBrno[6];
	char cTbsdy[9];
	char cTlrno[5];
	char cXmno[77];
	char cXmno2[61];
	char cOpcd[2];
	char cQsbrno[6];
	char cYwtype[51];
	char cYwind[13];
	char cXm_ind[2];
	char cActno[20];
	char cName[61];
	char cOqsbrno[13];
	char cFlag[2];
	char cFiller[61];
	char buf[121];

	struct lv_xmh_c g_lv_xmh;

	memset(cBrno,'\0',sizeof(cBrno));
	memset(cTbsdy,'\0',sizeof(cTbsdy));
	memset(cXmno,'\0',sizeof(cXmno));
	memset(cXmno2,'\0',sizeof(cXmno2));
	memset(cOpcd,'\0',sizeof(cOpcd));
	memset(cQsbrno,'\0',sizeof(cQsbrno));
	memset(cYwtype,'\0',sizeof(cYwtype));
	memset(cYwind ,'\0',sizeof(cYwind));
	memset(cXm_ind,'\0',sizeof(cXm_ind));
	memset(cActno,'\0',sizeof(cActno));
	memset(cName,'\0',sizeof(cName));
	memset(cOqsbrno,'\0',sizeof(cOqsbrno));
	memset(cFlag,'\0',sizeof(cFlag));
	memset(cFiller,'\0',sizeof(cFiller));
	memset(buf,'\0',sizeof(buf));

	memset(&g_lv_xmh ,0x00 ,sizeof(struct lv_xmh_c));

	vtcp_log("[%s][%d]spJ077小额项目号维护交易BEGIN",__FILE__,__LINE__);

	/*** 初始化全局变量 ***/
  pub_base_sysinit();

  /** 取得FD数据 **/
  get_fd_data("0030",cBrno);           /** 取得交易机构 **/
	get_fd_data("0050",cTbsdy);          /** 取得交易日期 **/
	get_fd_data("0070",cTlrno);          /** 取得柜员号 **/
	get_fd_data("0690",cXm_ind);         /** 取得项目种类 **/
  get_fd_data("0740",cXmno);           /** 取得项目号 **/
  get_fd_data("0520",cOqsbrno);        /** 取得对方清算行号 **/
  get_fd_data("0330",cActno);          /** 取得帐号号 **/
  get_fd_data("0760",buf);             /** 取得户名附言 **/
  get_fd_data("0270",cYwtype);         /** 取得业务种类 **/
  get_fd_data("0680",cFlag);           /** 取得标志 **/
  get_fd_data("0670",cOpcd);           /** 取得操作类型 **/

	memcpy(cName,buf,sizeof(cName)-1);
	memcpy(cFiller,buf+60,sizeof(cFiller)-1);
	memcpy(cXmno2,cXmno,sizeof(cXmno2)-1);
	memcpy(cYwind,cYwtype,sizeof(cYwind)-1);
	vtcp_log("[%s][%d]cBrno=====[%s]cXm_ind==[%s]",__FILE__,__LINE__,cBrno,cXm_ind);
	vtcp_log("[%s][%d]cFlag=====[%s]cOpcd==[%s]",__FILE__,__LINE__,cFlag,cOpcd);
	vtcp_log("[%s][%d]cXmno ====[%s]",__FILE__,__LINE__,cXmno);
	vtcp_log("[%s][%d]cXmno2 ===[%s]",__FILE__,__LINE__,cXmno2);
  vtcp_log("[%s][%d]cXm_ind===[%s]",__FILE__,__LINE__,cXm_ind);

  zipn_space(cXmno2,sizeof(cXmno2)-1);
	if(cOpcd[0]!='1')/** 查询 修改 **/
	{
		g_reply_int = Lv_xmh_Dec_Upd(g_pub_tx.reply,"xmno='%s' ",cXmno2);
		vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
		if( g_reply_int==100 ) {
        strcpy( g_pub_tx.reply,"D104" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    if( g_reply_int ) {
        strcpy( g_pub_tx.reply,"D102" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }

		g_reply_int = Lv_xmh_Fet_Upd(&g_lv_xmh,g_pub_tx.reply);
		vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
		if( g_reply_int==100 ) {
        strcpy( g_pub_tx.reply,"D104" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    if( g_reply_int ) {
        strcpy( g_pub_tx.reply,"D102" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
		
		if(memcmp(cBrno,g_lv_xmh.br_no,sizeof(cBrno)-1)!=0 &&
			memcmp(cBrno,LV_CZ_QSBRNO,BRNO_LEN)!=0)
		{
			sprintf(acErrMsg,"无权查看修改它行项目号");
	    WRITEMSG
      strcpy(g_pub_tx.reply,"P061");
      set_zd_data(DC_GET_MSG,acErrMsg);
      goto ErrExit;
    }
	}
	else
	{
		memcpy(g_lv_xmh.xmno,cXmno2,sizeof(g_lv_xmh.xmno)-1);
		zipn_space(g_lv_xmh.xmno,sizeof(g_lv_xmh.xmno)-1);
	}
	
	if(cOpcd[0]!='0')
	{
		if(memcmp(cBrno,LV_CZ_QSBRNO,BRNO_LEN)==0)
		{
			  sprintf(acErrMsg,"只有支行可以开项目号");
	      WRITEMSG
        strcpy(g_pub_tx.reply,"P062");
        set_zd_data(DC_GET_MSG,acErrMsg);
        goto ErrExit;
    }

		memcpy(g_lv_xmh.yw_ind,cYwtype,sizeof(g_lv_xmh.yw_ind)-1);
		memcpy(g_lv_xmh.xm_ind,cXm_ind,sizeof(g_lv_xmh.xm_ind)-1);
		memcpy(g_lv_xmh.ac_no,cActno,sizeof(g_lv_xmh.ac_no)-1);
		memcpy(g_lv_xmh.name,cName,sizeof(g_lv_xmh.name)-1);
		memcpy(g_lv_xmh.o_qs_no,cOqsbrno,sizeof(g_lv_xmh.o_qs_no)-1);
		memcpy(g_lv_xmh.br_no,cBrno,sizeof(g_lv_xmh.br_no)-1);
		memcpy(g_lv_xmh.last_date,cTbsdy,sizeof(g_lv_xmh.last_date)-1);
		memcpy(g_lv_xmh.tel,cTlrno,sizeof(g_lv_xmh.tel)-1);
		memcpy(g_lv_xmh.flg,cFlag,sizeof(g_lv_xmh.flg)-1);
		memcpy(g_lv_xmh.brf,cFiller,sizeof(g_lv_xmh.brf)-1);

		zipn_space(g_lv_xmh.yw_ind,sizeof(g_lv_xmh.yw_ind)-1);
		zipn_space(g_lv_xmh.xm_ind,sizeof(g_lv_xmh.xm_ind)-1);
		zipn_space(g_lv_xmh.ac_no,sizeof(g_lv_xmh.ac_no)-1);
		zipn_space(g_lv_xmh.name,sizeof(g_lv_xmh.name)-1);
		zipn_space(g_lv_xmh.o_qs_no,sizeof(g_lv_xmh.o_qs_no)-1);
		zipn_space(g_lv_xmh.br_no,sizeof(g_lv_xmh.br_no)-1);
		zipn_space(g_lv_xmh.last_date,sizeof(g_lv_xmh.last_date)-1);
		zipn_space(g_lv_xmh.tel,sizeof(g_lv_xmh.tel)-1);
		zipn_space(g_lv_xmh.flg,sizeof(g_lv_xmh.flg)-1);
		zipn_space(g_lv_xmh.brf,sizeof(g_lv_xmh.brf)-1);
	
	}

	if(cOpcd[0]=='1')
	{
		if(g_lv_xmh.flg[0]=='1')
		{
			  sprintf(acErrMsg,"添加一个停用的项目号没有意义");
	      WRITEMSG
        strcpy(g_pub_tx.reply,"P063");
        set_zd_data(DC_GET_MSG,acErrMsg);
        goto ErrExit;
    }

		g_reply_int=Lv_xmh_Ins(g_lv_xmh,g_pub_tx.reply);
		vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
		if( g_reply_int ) 
		{
        strcpy( g_pub_tx.reply,"D107" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
	}
	else if(cOpcd[0]=='2')
	{
		g_reply_int=Lv_xmh_Upd_Upd(g_lv_xmh,g_pub_tx.reply);
		vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
		if( g_reply_int ) 
		{
        strcpy( g_pub_tx.reply,"D107" );
        sprintf(acErrMsg,"SQLERR =[%d]",g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
	}
	if(cOpcd[0]!='1')/** 查询 修改 **/
	{
		Lv_xmh_Clo_Upd();
	}

	/* 记流水日志 */
  g_reply_int = pub_ins_trace_log();
  if ( g_reply_int )
  {
  	  sprintf( acErrMsg, "记流水日志错" );
  	  WRITEMSG
  	  strcpy(g_pub_tx.reply,"P031");
  	  set_zd_data(DC_GET_MSG,acErrMsg);
  	  goto ErrExit;
  }

	memset(buf,' ',sizeof(buf)-1);
	memcpy(buf,g_lv_xmh.name,sizeof(g_lv_xmh.name)-1);
	memcpy(buf+60,g_lv_xmh.brf,sizeof(g_lv_xmh.brf)-1);

  set_zd_data( "0670", cOpcd);                   /** 操作类型 **/
  set_zd_data( "0740", g_lv_xmh.xmno);           /** 项目号 **/
  set_zd_data( "0690", g_lv_xmh.xm_ind);         /** 返回项目种类 **/
  set_zd_data( "0520", g_lv_xmh.o_qs_no);        /** 返回对方清算行号 **/
  set_zd_data( "0330", g_lv_xmh.ac_no);         /** 返回帐号号 **/
  set_zd_data( "0760", buf);            /** 返回户名附言 **/
  set_zd_data( "0610", g_lv_xmh.yw_ind);         /** 返回业务种类 **/
  set_zd_data( "0680", g_lv_xmh.flg);            /** 返回标志 **/

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
	

