/*************************************************
* �� �� ��: sp9734.c
* ���������������ʺŻ����ʻ�֧ȡ��Ϣ_[֧ȡ��Ϣά������]
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "mob_acct_type_hst_c.h"
#include "mob_acct_type_c.h"

sp9734()  
{ 		
	int ret=0;
	char ac_no[20];
	char mob_sts[8];         /*ǩԼ״̬*/
	char tx_date[16];					/*ǩԼ����*/
	char mobile[50];					/*ǩԼ�ֻ����ɶ��*/
	/*�������ͣ�����ƽ̨�����Ľ���*/
	char type[4];
	struct mob_acct_type_hst_c sMob_acct_type_hst;
	struct mob_acct_type_c sMob_acct_type;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sMob_acct_type_hst,0x00,sizeof(sMob_acct_type_hst));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&ac_no,0x00,sizeof(ac_no));
	memset(&mob_sts,0x00,sizeof(mob_sts));
	memset(&tx_date,0x00,sizeof(tx_date));
	memset(&mobile,0x00,sizeof(mobile));
	memset(&type,0x00,sizeof(type));
  /*���������ѯ����״̬*/
  get_zd_data("0720",type);
  if(0 == strcmp(type,"6"))
  {
  	get_zd_data("0300",ac_no);						/*�ʺ�*/
		pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
		if(ret)
		{
			sprintf(acErrMsg,"���˻������ڣ� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 0;
		}
		ret =  Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no = '%s' ",ac_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"���˻�δǩԼ�� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F038");
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 0;
		}else if( ret )
		{
			sprintf(acErrMsg,"�����ʺų���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		strcpy(g_pub_tx.reply,"F036");
  	set_zd_data(DC_REPLY,g_pub_tx.reply);
  	set_zd_data("0810",sMob_acct_type.mobile);
		return 0;
  }
  
	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);						/*�ʺ�*/
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0300",ac_no);	/* ������ʺ� */
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	/*add by chenggongxun 20091023 ���� beg*/
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*if( g_mdm_ac_rel.note_sts[0]=='1' )20111117 YW  ������ʧ�ж�
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' ) */
	if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѱ���֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����Ѳ��ỻ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	/*add by chenggongxun 20091023 ���� beg*/
	set_zd_data("0670",g_mdm_ac_rel.draw_pwd_yn);	/*�Ƿ�����֧ȡ*/
	set_zd_data("0680",g_mdm_ac_rel.id_type);   /*֤������*/
	set_zd_data("0620",g_mdm_ac_rel.id_no);     /*֤������*/
 
	/* 20110728 add by yanqq */
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no);  /*�����к�*/
	set_zd_data("0490",g_mdm_ac_rel.mdm_code);     /*�˻����ʴ���[0010-���� 0013-�浥 0020-������]*/

	/* ������� */
	if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y')			/*1-�޸�����*/
		set_zd_data("0700","1");
	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N')		/*2-��������*/
		set_zd_data("0700","2");

	/** ��ѯ������ʷǩԼ��Ϣ **/
	ret =  Mob_acct_type_hst_Sel(g_pub_tx.reply,&sMob_acct_type_hst,"ac_no = '%s' order by tx_date desc,trace_no desc,mob_sts",ac_no);
	if(100 == ret)
	{
			ret =  Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no = '%s' ",ac_no);
			if(ret != 0 && ret != 100)
			{
					vtcp_log("[%s][%d]��ѯmob_acct_type�����![%d]\n",__FILE__,__LINE__,ret);
					goto ErrExit;
			}
			else if(100 == ret)
			{
					vtcp_log("[%s][%d]���˻�û��ǩԼ�ֻ�![%d]\n",__FILE__,__LINE__,ret);
					set_zd_data("0250","�ر�");
					goto OkExit;
			}
			set_zd_data("0250","��ͨ");
			set_zd_data("0810",sMob_acct_type.mobile);
			sprintf(tx_date,"%ld",sMob_acct_type.tx_date);			
			set_zd_data("0860",tx_date);
			set_zd_data("0920",sMob_acct_type.tel);							/*������Ա*/
			goto OkExit;
	}
	else if(ret != 0 && ret != 100)
	{
			vtcp_log("[%s][%d]��ѯmob_acct_type_hst�����![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
	}
	if(0 == strcmp(sMob_acct_type_hst.mob_sts,"3"))
	{
		sprintf(tx_date,"%ld",sMob_acct_type_hst.tx_date);
		set_zd_data("0870",tx_date);															/*��Լ����*/
		set_zd_data("0920",sMob_acct_type_hst.lst_tel);							/*������Ա*/
		set_zd_data("0250","�ر�");																/*״̬*/
		goto OkExit;
	}
	
	/** ��ѯ����ǩԼ��Ϣ **/
												
	strcpy(mobile,sMob_acct_type_hst.mobile);
	sprintf(tx_date,"%ld",sMob_acct_type_hst.tx_date);			
	set_zd_data("0860",tx_date);    										/*ǩԼ/��Ч����*/					
	set_zd_data("0920",sMob_acct_type_hst.lst_tel);							/*������Ա*/   
	set_zd_data("0810",mobile);													/*ǩԼ�ֻ�*/
	if(0 == strcmp(sMob_acct_type_hst.mob_sts,"2"))
	{
		set_zd_data("0250","��ͣ");
	}
	set_zd_data("0250","��ͨ"); 

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
