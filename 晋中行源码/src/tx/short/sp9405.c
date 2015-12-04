/*****************************************************
* �� �� ��:  sp9405.c
* ���������� ��ѯ�ʻ�������
* ��    ��:  xyy
* ������ڣ� 2006-9-26 11:07 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_fd123.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_basic_inf_c.h"
int sp9405()
{
	int ret=0,i;
	char cActno[20];
	char cActype[2];
	struct mdm_ac_rel_c  mdm_ac_rel;
	struct dd_mst_c  sDd_mst;
	struct in_mst_c  sIn_mst;
	struct cif_basic_inf_c  cif_basic_inf;
	memset(cActno,0,sizeof(cActno));
	memset(cActype,0,sizeof(cActype));
	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
	vtcp_log("sp9405 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	get_zd_data("0320",cActno);
	pub_base_old_acno(cActno);
	vtcp_log("[%s,%d]cActno=[%s]",__FILE__,__LINE__,cActno);
	zip_space(cActno);
	if(strlen(cActno)==0)
	{
		sprintf(acErrMsg,"�ʺŲ���Ϊ��");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01" );
		goto	ErrExit;
	}
	/***************add by liuyue 20061205 ,����Ҫ����Դӿ�Ŀ��*****/
	
	if(strlen(cActno)!=7 && strlen(cActno)!=5)
	{
		vtcp_log("[%s][%d]����������˺�!\n",__FILE__,__LINE__);
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel, "ac_no='%s'",cActno);
		vtcp_log("sp9405 ��ʼ��[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"�޴˽������˻���ϵ����");
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01" );
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�������˻���ϵ����[%d]",ret);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
		if(mdm_ac_rel.ac_no[0] != '9')
		{
			/*** ˳�㴫���˻����  add by chenchao 20110810 ***/
			ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ�������ʧ��[%d]",ret);
				strcpy( g_pub_tx.reply, "H034" );
				WRITEMSG
				goto ErrExit;
			}
			set_zd_double("0390",sDd_mst.bal);
		}else if(mdm_ac_rel.ac_no[0] == '9')
		{
			ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ�������ʧ��[%d]",ret);
				strcpy( g_pub_tx.reply, "H034" );
				WRITEMSG
				goto ErrExit;
			}
			set_zd_double("0390",sIn_mst.bal);
		}

		/*ȡ�ͻ���Ϣ gujy JinZ 070425*/
		memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
		ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf, "cif_no=(select cif_no from cif_id_code_rel where \
				id_type='%s' and id_no = '%s')", mdm_ac_rel.id_type, mdm_ac_rel.id_no);
		if ( ret==100 )
		{
			;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�������˻���ϵ����[%d]",ret);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
}	
	vtcp_log("[%s][%d]��ѯ�ɹ�!ac_seqn=[%d]",__FILE__,__LINE__,mdm_ac_rel.ac_seqn);
  if(mdm_ac_rel.ac_seqn==0)
  {
  	cActype[0]='6';
  }
  else
  {
  	cActype[0]='1';
  }
  vtcp_log("[%s][%d]cActype[0]==[%c]",__FILE__,__LINE__,cActype[0]);
  set_zd_data("0680",cActype);
  set_zd_data("0330",mdm_ac_rel.ac_no);
  set_zd_data("0250",cif_basic_inf.name);
  set_zd_double("0280",cif_basic_inf.cif_no);

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
