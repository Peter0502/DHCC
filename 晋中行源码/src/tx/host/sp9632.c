/*************************************************
* �� �� ��:  sp9632.c
* ���������� ��Ա��ɫ����
*
* ��    ��:  ligl 
* ������ڣ� 2008��8��5��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
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
	struct com_tel_change_c 	g_com_tel_change;	/* ��Ʒ�˻��ṹ */
	struct com_tel_change_c 	g_com_tel_change1;	/* ��Ʒ�˻��ṹ */
	struct com_tel_change_hst_c 	g_com_tel_change_hst;	/* ��Ʒ�˻��ṹ */
	/* ��� */
	memset(&gcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&vcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&pcom_tel , 0x00, sizeof(struct com_tel_c));
	memset(&g_com_tel_change	, 0x00, sizeof(struct com_tel_change_c));
	memset(&g_com_tel_change1	, 0x00, sizeof(struct com_tel_change_c));
	memset(&g_com_tel_change_hst	, 0x00, sizeof(struct com_tel_change_hst_c));
	pub_base_sysinit();
	get_zd_data("0910",cStr);/**�����ӵĽ�ɫ***/
	get_zd_data("0920",cTo_tel);/***���ӶԷ���Ա***/
  pub_base_strpack(cTo_tel);
  pub_base_strpack(cStr);
  vtcp_log("g_pub_tx.tel=[%s],cStr=[%s],cTo_tel=[%s]",g_pub_tx.tel,cStr,cTo_tel);
  /*** ��com_tel_changeά������ ***/
  {
  /*** ���com_tel_change ***/
  vcnt=sql_count("com_tel_change"," tel='%s' and to_tel='%s' ",g_pub_tx.tel,cStr);
  pcnt=sql_count("com_tel_change"," tel='%s' and to_tel='%s' ",cTo_tel,cStr);
  
  /*** ����com_tel_change ***/
  if(strcmp(g_pub_tx.tel,cStr)==0)/*�ͳ�Ȩ��*/
  	{
  		vtcp_log("%s,%d,Ȩ���ͳ�!vcnt=[%d],pcnt=[%d]",__FILE__,__LINE__,vcnt,pcnt);
  		ret=Com_tel_Sel(g_pub_tx.reply,&gcom_tel," tel='%s'",g_pub_tx.tel);
      ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel," tel='%s'",cTo_tel);
  		if(vcnt==0)/* �����¼�¼ */
      	{
      		strcpy(g_com_tel_change.tel   ,g_pub_tx.tel);
          strcpy(g_com_tel_change.br_no ,gcom_tel.br_no);
          strcpy(g_com_tel_change.name  ,gcom_tel.name);
          strcpy(g_com_tel_change.pwd   ,gcom_tel.pwd);
          strcpy(g_com_tel_change.lvl   ,gcom_tel.lvl);
          strcpy(g_com_tel_change.to_tel,gcom_tel.tel);
          strcpy(g_com_tel_change.to_lvl,gcom_tel.lvl);
          strcpy(g_com_tel_change.sts   ,"0");
      		/*��������*/
      		ret=Com_tel_change_Ins(g_com_tel_change);
      	}else{
      		    if(vcnt==1)
      		    	{
      		    		/*����Ƿ����ͳ�Ȩ��*/
      	         	tcnt=sql_count("com_tel_change"," tel<>'%s' and to_tel='%s' and sts=1 ",g_pub_tx.tel,cStr);
      	         	if(tcnt==1)
      	         		{
      	         			sprintf( acErrMsg,"[%s]��ԱȨ���ѽ���������Ա��",g_pub_tx.tel );
		                   WRITEMSG
		                   strcpy( g_pub_tx.reply,"Z10B" );
		                   goto ErrExit;
      	         		}else
      	         	if(tcnt>0)
      	         		{
      	         			sprintf( acErrMsg,"[%s]��ԱȨ���ѽ�������!���!��Ա��",g_pub_tx.tel );
		                   WRITEMSG
		                  strcpy( g_pub_tx.reply,"D106" );
      	         			goto ErrExit;
      	         		}
      	          vtcp_log("%s,%d,Ȩ�޲����ͳ�!tcnt=[%d]",__FILE__,__LINE__,tcnt);
      		    	}
      		}
      if(pcnt==0)/* �����¼�¼ */
      	{
      		strcpy(g_com_tel_change1.tel   ,cTo_tel);
          strcpy(g_com_tel_change1.br_no ,vcom_tel.br_no);
          strcpy(g_com_tel_change1.name  ,vcom_tel.name);
          strcpy(g_com_tel_change1.pwd   ,vcom_tel.pwd);
          strcpy(g_com_tel_change1.lvl   ,vcom_tel.lvl);
          strcpy(g_com_tel_change1.to_tel,gcom_tel.tel);
          strcpy(g_com_tel_change1.to_lvl,gcom_tel.lvl);
          strcpy(g_com_tel_change1.sts   ,"0");
      		/*��������*/
      		ret=Com_tel_change_Ins(g_com_tel_change1);
      	}
  	}
  	else
  if(strcmp(cStr,cTo_tel)==0)/*�ջ�Ȩ��*/
  	{
  		vtcp_log("%s,%d,�ջ�Ȩ��!vcnt=[%d],pcnt=[%d]",__FILE__,__LINE__,vcnt,pcnt);
  	}
  	else/*�ƽ�Ȩ��*/
  	{
  		vtcp_log("%s,%d,�ƽ�Ȩ��!pcnt=[%d]",__FILE__,__LINE__,pcnt);
      ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel," tel='%s'",cTo_tel);
      ret=Com_tel_Sel(g_pub_tx.reply,&pcom_tel," tel='%s'",cStr);
  		if(pcnt==0)/* �����¼�¼ */
      	{
      		strcpy(g_com_tel_change.tel   ,cTo_tel);
          strcpy(g_com_tel_change.br_no ,vcom_tel.br_no);
          strcpy(g_com_tel_change.name  ,vcom_tel.name);
          strcpy(g_com_tel_change.pwd   ,vcom_tel.pwd);
          strcpy(g_com_tel_change.lvl   ,vcom_tel.lvl);
          strcpy(g_com_tel_change.to_tel,cStr);
          strcpy(g_com_tel_change.to_lvl,pcom_tel.lvl);
          strcpy(g_com_tel_change.sts   ,"0");
      		/*��������*/
      		ret=Com_tel_change_Ins(g_com_tel_change);
      	}else
      if(pcnt>0)/***��������ֶ�***/
      	{
      	}
  	}
  }
  	
  /***�������״̬***/
  sql_execute2("update com_tel_change set sts='0' where tel='%s' and to_tel='%s'",g_pub_tx.tel,cStr);
  sql_execute2("update com_tel_change set sts='1' where tel='%s' and to_tel='%s'",cTo_tel,cStr);
  /*************/
  strcpy(g_com_tel_change_hst.tel,g_pub_tx.tel);
  strcpy(g_com_tel_change_hst.br_no,g_pub_tx.tx_br_no);
  strcpy(g_com_tel_change_hst.to_tel,cTo_tel);
  strcpy(g_com_tel_change_hst.role_tel,cStr);
  g_com_tel_change_hst.sts[0]='2';
  strcpy(g_com_tel_change_hst.brf,"��ɫ����");
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
