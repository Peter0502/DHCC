/*************************************************
* 文 件 名:  sp9632.c
* 功能描述： 柜员角色交接
*
* 作    者:  ligl 
* 完成日期： 2008年8月5日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_tel_change_c.h"
#include "com_tel_change_hst_c.h"
#include "com_tel_c.h"
sp9632()
{
	int             ret=0, i = 0, vcnt=0 ,pcnt=0 ,tcnt=0;
	char            tel[5], filename[300];
	FILE           	*fp;
	char       cTo_tel[5];
	char       cStr[5];
	struct com_tel_c gcom_tel,vcom_tel,pcom_tel;
	struct com_tel_change_c 	g_com_tel_change;	/* 产品账户结构 */
	struct com_tel_change_c 	g_com_tel_change1;	/* 产品账户结构 */
	struct com_tel_change_hst_c 	g_com_tel_change_hst;	/* 产品账户结构 */
	/* 清空 */
	memset(&gcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&vcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&pcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&g_com_tel_change	, 0x00, sizeof(struct com_tel_change_c));
	memset(&g_com_tel_change1	, 0x00, sizeof(struct com_tel_change_c));
	memset(&g_com_tel_change_hst	, 0x00, sizeof(struct com_tel_change_hst_c));
	pub_base_sysinit();
	get_zd_data("0910",cStr);/**被交接的角色***/
	get_zd_data("0920",cTo_tel);/***交接对方柜员***/
  pub_base_strpack(cTo_tel);
  pub_base_strpack(cStr);
  vtcp_log("g_pub_tx.tel=[%s],cStr=[%s],cTo_tel=[%s]",g_pub_tx.tel,cStr,cTo_tel);
  /*** 对com_tel_change维护部分 ***/
  {
  /*** 检查com_tel_change ***/
  vcnt=sql_count("com_tel_change"," tel='%s' and to_tel='%s' ",g_pub_tx.tel,cStr);
  pcnt=sql_count("com_tel_change"," tel='%s' and to_tel='%s' ",cTo_tel,cStr);
  
  /*** 插入com_tel_change ***/
  if(strcmp(g_pub_tx.tel,cStr)==0)/*送出权限*/
  	{
  		vtcp_log("%s,%d,权限送出!vcnt=[%d],pcnt=[%d]",__FILE__,__LINE__,vcnt,pcnt);
  		ret=Com_tel_Sel(g_pub_tx.reply,&gcom_tel," tel='%s'",g_pub_tx.tel);
      ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel," tel='%s'",cTo_tel);
  		if(vcnt==0)/* 增加新纪录 */
      	{
      		strcpy(g_com_tel_change.tel   ,g_pub_tx.tel);
          strcpy(g_com_tel_change.br_no ,gcom_tel.br_no);
          strcpy(g_com_tel_change.name  ,gcom_tel.name);
          strcpy(g_com_tel_change.pwd   ,gcom_tel.pwd);
          strcpy(g_com_tel_change.lvl   ,gcom_tel.lvl);
          strcpy(g_com_tel_change.to_tel,gcom_tel.tel);
          strcpy(g_com_tel_change.to_lvl,gcom_tel.lvl);
          strcpy(g_com_tel_change.sts   ,"0");
      		/*插入数据*/
      		ret=Com_tel_change_Ins(g_com_tel_change);
      	}else{
      		    if(vcnt==1)
      		    	{
      		    		/*检查是否已送出权限*/
      	         	tcnt=sql_count("com_tel_change"," tel<>'%s' and to_tel='%s' and sts=1 ",g_pub_tx.tel,cStr);
      	         	if(tcnt==1)
      	         		{
      	         			sprintf( acErrMsg,"[%s]柜员权限已交给其他柜员！",g_pub_tx.tel );
		                   WRITEMSG
		                   strcpy( g_pub_tx.reply,"Z10B" );
		                   goto ErrExit;
      	         		}else
      	         	if(tcnt>0)
      	         		{
      	         			sprintf( acErrMsg,"[%s]柜员权限已交给其他!多个!柜员！",g_pub_tx.tel );
		                   WRITEMSG
		                  strcpy( g_pub_tx.reply,"D106" );
      	         			goto ErrExit;
      	         		}
      	          vtcp_log("%s,%d,权限不能送出!tcnt=[%d]",__FILE__,__LINE__,tcnt);
      		    	}
      		}
      if(pcnt==0)/* 增加新纪录 */
      	{
      		strcpy(g_com_tel_change1.tel   ,cTo_tel);
          strcpy(g_com_tel_change1.br_no ,vcom_tel.br_no);
          strcpy(g_com_tel_change1.name  ,vcom_tel.name);
          strcpy(g_com_tel_change1.pwd   ,vcom_tel.pwd);
          strcpy(g_com_tel_change1.lvl   ,vcom_tel.lvl);
          strcpy(g_com_tel_change1.to_tel,gcom_tel.tel);
          strcpy(g_com_tel_change1.to_lvl,gcom_tel.lvl);
          strcpy(g_com_tel_change1.sts   ,"0");
      		/*插入数据*/
      		ret=Com_tel_change_Ins(g_com_tel_change1);
      	}
  	}
  	else
  if(strcmp(cStr,cTo_tel)==0)/*收回权限*/
  	{
  		vtcp_log("%s,%d,收回权限!vcnt=[%d],pcnt=[%d]",__FILE__,__LINE__,vcnt,pcnt);
  	}
  	else/*移交权限*/
  	{
  		vtcp_log("%s,%d,移交权限!pcnt=[%d]",__FILE__,__LINE__,pcnt);
      ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel," tel='%s'",cTo_tel);
      ret=Com_tel_Sel(g_pub_tx.reply,&pcom_tel," tel='%s'",cStr);
  		if(pcnt==0)/* 增加新纪录 */
      	{
      		strcpy(g_com_tel_change.tel   ,cTo_tel);
          strcpy(g_com_tel_change.br_no ,vcom_tel.br_no);
          strcpy(g_com_tel_change.name  ,vcom_tel.name);
          strcpy(g_com_tel_change.pwd   ,vcom_tel.pwd);
          strcpy(g_com_tel_change.lvl   ,vcom_tel.lvl);
          strcpy(g_com_tel_change.to_tel,cStr);
          strcpy(g_com_tel_change.to_lvl,pcom_tel.lvl);
          strcpy(g_com_tel_change.sts   ,"0");
      		/*插入数据*/
      		ret=Com_tel_change_Ins(g_com_tel_change);
      	}else
      if(pcnt>0)/***主表更新字段***/
      	{
      	}
  	}
  }
  	
  /***主表更新状态***/
  sql_execute2("update com_tel_change set sts='0' where tel='%s' and to_tel='%s'",g_pub_tx.tel,cStr);
  sql_execute2("update com_tel_change set sts='1' where tel='%s' and to_tel='%s'",cTo_tel,cStr);
  /*************/
  strcpy(g_com_tel_change_hst.tel,g_pub_tx.tel);
  strcpy(g_com_tel_change_hst.br_no,g_pub_tx.tx_br_no);
  strcpy(g_com_tel_change_hst.to_tel,cTo_tel);
  strcpy(g_com_tel_change_hst.role_tel,cStr);
  g_com_tel_change_hst.sts[0]='2';
  strcpy(g_com_tel_change_hst.brf,"角色交接");
  g_com_tel_change_hst.tx_date=g_pub_tx.tx_date;
  g_com_tel_change_hst.tx_time=g_pub_tx.tx_time;
  ret=Com_tel_change_hst_Ins(g_com_tel_change_hst);
  /*************/
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
