/*************************************************
* �� �� ��:    sp4518.c
* ����������   
*              �޸ĵ�ѺƷ����
*
* ��    ��:    
* ������ڣ�   
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_gage_reg_c.h"

sp4518()
{ 		
	int ret;
	char  var1[20];
	struct ln_mst_c   s_ln_mst;
	struct mdm_ac_rel_c    s_mdm_ac_rel;
	struct ln_gage_reg_c    s_ln_gage_reg;
	
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
	memset( &s_mdm_ac_rel , 0x00 , sizeof(struct mdm_ac_rel_c) );
	memset( &s_ln_gage_reg , 0x00 , sizeof(struct ln_gage_reg_c) );
	
	
	memset(var1,0x00,sizeof(var1));
	
	
	get_zd_data("0300",var1);
	vtcp_log("[%s][%d]var1===[%s]\n",__FILE__,__LINE__,var1);
	
	
	/** ȡֵ����ֵ **/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",var1);/* ��ȡ�����ʺ� */
	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]�����ʺŲ�����!!ac_no=[%s]",__FILE__,__LINE__,var1);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"HV01");
		 	set_zd_data("0130","�����ʺŲ�����");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"��ѯ�������˻���ϵ���쳣!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
	
	
	/****��������Ǽ�����****/
	ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_mdm_ac_rel.ac_id);/* ��ȡ�����ʺ� */
	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]�����ʺŲ�����!!ac_id=[%ld]",__FILE__,__LINE__,s_ln_mst.ac_id);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"��������Ǽ�����!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
	 	
	 	vtcp_log("[%s][%d]��ȡ���ĺ�ͬ��====[%s]\n",__FILE__,__LINE__,s_ln_mst.pact_no);
	 	
	 	/*******���������ѺƷ�Ǽǲ�******/
	 	
	 ret = Ln_gage_reg_Sel(g_pub_tx.reply,&s_ln_gage_reg,"ac_id=%ld and  pact_no='%s' ",s_ln_mst.ac_id,s_ln_mst.pact_no);/* ��ȡ�����ʺ� */
	 if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]���������ѺƷ�Ǽǲ�����!!ac_id=[%ld]",__FILE__,__LINE__,s_ln_mst.ac_id);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"HV01");
		 	set_zd_data("0130","���������ѺƷ�Ǽǲ�����");    
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"���������ѺƷ�Ǽǲ�����!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}



	  set_zd_data("0250",s_ln_mst.name);
	  set_zd_data("0630",s_ln_mst.pact_no);
	  set_zd_double("0410",s_ln_mst.bal);
	  set_zd_double("0400",s_ln_mst.gage_amt);
		set_zd_data("0220",s_ln_gage_reg.gage_type);
		set_zd_double("0420",s_ln_gage_reg.amt);
		set_zd_data("0830",s_ln_gage_reg.gage_inf);
		


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