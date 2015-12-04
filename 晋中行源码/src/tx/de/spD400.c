/***************************************************************
* �� �� ��: spD400.c
* ����������VTM��ʧ����
* ��    ��: �����
* ������ڣ�2014��7��31��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD400()
{
	int		iRet = 0;
	int		mode = 0;
	char	id_own[2];
	char	loss_type[3];
	
	memset(id_own, 0x00, sizeof(id_own));
	memset(loss_type, 0x00, sizeof(loss_type));
	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&g_dd_parm, 0x00, sizeof(struct dd_parm_c));
	memset(&g_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&g_td_parm, 0x00, sizeof(struct td_parm_c));
	memset(&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset(&g_ln_parm, 0x00, sizeof(struct ln_parm_c));
	memset(&g_in_mst, 0x00, sizeof(struct in_mst_c));
	memset(&g_in_parm, 0x00, sizeof(struct in_parm_c));
	
	/**��ʼ��ȫ�ֱ���**/
	pub_base_sysinit();
	get_zd_data("0300", g_pub_tx.ac_no);
	get_zd_data("0220", loss_type);
	get_zd_data("0690",id_own);
	get_zd_data("0680", g_pub_tx.tmp_ind);/**������֤������**/
	get_zd_data("0630", g_pub_tx.cal_code);/**������֤������**/
	get_zd_data("1148", g_pub_tx.id_type);
	get_zd_data("1146", g_pub_tx.id_no);
	get_zd_data("1141", g_pub_tx.draw_uncon_yn);
	get_zd_data("1142", g_pub_tx.draw_pwd_yn);
	get_zd_data("1143", g_pub_tx.qry_pwd);
	get_zd_data("1144", g_pub_tx.draw_pwd);
	get_zd_data("1145", g_pub_tx.draw_id_yn);
	get_zd_data("1146", g_pub_tx.id_no);
	get_zd_data("1147", g_pub_tx.draw_seal_yn);
	get_zd_data("1149", g_pub_tx.qry_pwd_yn);
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("%s %d tmp_ind ===[%s] ",__FILE__,__LINE__,g_pub_tx.tmp_ind);
	vtcp_log("%s %d cal_code ===[%s] ",__FILE__,__LINE__,g_pub_tx.cal_code);
	vtcp_log("%s %d id_type ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_type);
	vtcp_log("%s %d id_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_no);
	vtcp_log("%s %d draw_uncon_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_uncon_yn);
	vtcp_log("%s %d draw_pwd_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	/*vtcp_log("%s %d qry_pwd ===[%s] ",__FILE__,__LINE__,g_pub_tx.qry_pwd);
	vtcp_log("%s %d draw_pwd ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	vtcp_log("%s %d draw_id_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_id_yn);
	vtcp_log("%s %d id_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_no);
	vtcp_log("%s %d draw_seal_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_seal_yn);
	vtcp_log("%s %d qry_pwd_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.qry_pwd_yn);
	
	/**�Ƿ���**/
	pub_base_old_acno(g_pub_tx.ac_no);
	if ('Y' == id_own[0])
	{
		get_zd_data("0250", g_pub_tx.name);
		
		/**У��֤����Ϣ**/
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if (100 == iRet)
		{
			sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]", g_pub_tx.ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if (iRet)
		{
			sprintf(acErrMsg, "�����ʺų���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		
		zip_space(g_mdm_ac_rel.id_type);
		zip_space(g_mdm_ac_rel.id_no);
		zip_space(g_pub_tx.tmp_ind);
		zip_space(g_pub_tx.cal_code);
		if (0 != strcmp(g_mdm_ac_rel.id_type, g_pub_tx.tmp_ind))
		{
			sprintf(acErrMsg, "֤�����ʹ���");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P444");
			goto ErrExit;
		}
		else if (0 != strcmp(g_mdm_ac_rel.id_no, g_pub_tx.cal_code))
		{
			sprintf(acErrMsg, "֤���������");
			WRITEMSG
			strcpy(g_pub_tx.reply, "P445");
			goto ErrExit;
		}
	}
	else
	{
		get_zd_data("0260", g_pub_tx.name);
	}
	vtcp_log("%s %d �Ƿ���id_own ===[%s]����=[%s]",__FILE__,__LINE__,id_own,g_pub_tx.name);
	
	/**��ʧ��Ҵ���**/
	mode = atoi(loss_type);
	if (pub_acct_loss(mode, 0))
	{
		sprintf(acErrMsg, "��ʧ������س���!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_long("0460", g_pub_tx.tx_date);
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ʧ��ҳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ʧ��ҳɹ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}