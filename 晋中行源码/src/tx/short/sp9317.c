/*************************************************
* �� �� ��: sp9317.c
* �����������绰����ȡ������˶�
* ��    ��: ZHGF
* ������ڣ�2008��05��19��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
	  �������ã�
	insert into tx_def values('9317','�绰����ȡ������˶�','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values('9317','0','9317','#$');
	insert into tx_sub_def values('9317','�绰����ȡ������˶�','sp9317','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"

int sp9317()
{
	int ret=0;
	char cAc_no[20];
	char cDraw_pwd[7];
	char filename[100];
	FILE *fp;
	struct mdm_ac_rel_c		z_mdm_ac_rel;
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG
	memset(filename,0x00,sizeof(filename));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
	memset(&z_mdm_ac_rel,0x00,sizeof(z_mdm_ac_rel));
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
	/*****{
		set_zd_data("0300","6229366601000341208");
		set_zd_data("0790","111112");
	}*****/
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0790",g_pub_tx.draw_pwd);
	pub_base_strpack(g_pub_tx.draw_pwd);
	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
	vtcp_log("%s,%d,�˺�Ϊ[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	{/**��ʼ���ȡ������**/
		/*vtcp_log("%s,%d �绰����ȡ������[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&z_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if(ret){
			sprintf(acErrMsg,"��ѯ���ʺ��ʻ���ϵ�����!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			    strcpy(g_pub_tx.reply,"W011");
			ERR_DEAL;
		}
		if(z_mdm_ac_rel.draw_pwd_yn[0]=='N'){
			sprintf(acErrMsg,"���ܻ��뵽��̨����ҵ��!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			    strcpy(g_pub_tx.reply,"D240");
			ERR_DEAL;
		}
		strcpy(g_pub_tx.id_no,z_mdm_ac_rel.id_no);
		strcpy(g_pub_tx.id_type,z_mdm_ac_rel.id_type);
		/**��ʼ���绰����ȡ������**/
		ret = pub_base_check_draw("101");
		if(ret){
			sprintf(acErrMsg,"֧ȡ����������!");
			sprintf(g_pub_tx.reply,"M005");
			goto ErrExit;
		}
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�绰����ȡ������˶Գɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"�绰����ȡ������˶�ʧ�ܣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
