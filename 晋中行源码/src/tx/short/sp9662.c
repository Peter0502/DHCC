/*************************************************
* �� �� ��:    sp9662.c
* ����������   
*              ���ݴ���˺�ȡ���˻���źͶ�Ӧ��Ʒ��Ϣ
*
* ��    ��:    LiuHuafeng 2008-7-14 17:25:56
* ������ڣ�   2008-7-14 17:26:02
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
TX_CODE	TX_NAME	BIT_MAP	LOG_IND	TX_TYPE	WRK_STS	YW_CNT_IND	YW_PERCENT
9662	����˺�ȡ��źͲ�Ʒ	10000000000000000000001111100000001000000000010000000000000000000111110000000000000000000000000000000000000000000000000000000000	0	4	0	3	
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
9662	0	9662	#$
SUB_TX_CODE	SUB_TX_NAME	TX_FUNC	ACCT_IND	CHNL_IND
9662	��ȡ��Ų�Ʒ��Ϣ	sp9662	0	0

*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9662()
{
	int             ret = 0;
	char            ac_no[20];
	int             ac_seqn, back_ac_seqn;
	int             flag = 0, redflag = 0;
	char            tx_code[8];


	struct dd_mst_c dd_mst_tmp;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;

	/** ���ݳ�ʼ�� **/
	memset(&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/

	get_zd_data("0310", ac_no);	/* ����ʺ� */
	pub_base_old_acno(ac_no);	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0310", ac_no);	/* ������ʺ� */

	get_zd_data("0650", tx_code);	/* ���״��� */

	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, " ac_no='%s' ", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "L113");
		ERR_DEAL
	} else if (ret != 0) {
		sprintf(acErrMsg, "ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		ERR_DEAL
	}
	sprintf(acErrMsg, "ȡ�����˻�IDΪ[%ld]", g_mdm_ac_rel.ac_id);
	WRITEMSG
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no);
	/* ���ݴ����˻�ID��ѯ�������ļ����� */
	flag = sql_count("dd_mst", " ac_id=%ld ", g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg, "���ݴ����˻�ID��ѯ�������ļ�����[%d]", flag);
	WRITEMSG

	if (flag == 0) {
		set_zd_int("0660", redflag);	/* �Ƿ�������ű�־ */
		sprintf(acErrMsg, "�������ļ������޴˼�¼[%d]", flag);
		WRITEMSG
			strcpy(g_pub_tx.reply, "L114");
		goto ErrExit;
	}
	if (flag == 1) {
		ret = Dd_mst_Sel(g_pub_tx.reply, &dd_mst_tmp, " ac_id=%ld ", g_mdm_ac_rel.ac_id);
		if (ret == 100) {
			sprintf(acErrMsg, "������ļ������޴˼�¼ erro code=[%s]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "L002");
			ERR_DEAL

		} else if (ret != 0) {
			sprintf(acErrMsg, "ȡ�������ļ��쳣 erro code=[%s]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
			ERR_DEAL
		}
		sprintf(acErrMsg, "ȡ�������ļ�PASS");
		WRITEMSG


		/* �ǿ��Ͽ���ר��ͨ�� */


		/* ���ø��ݴ����ʾ�˺ź��˺����ȡ������ļ����� */
			ret = pub_dd_dddis_file(ac_no, dd_mst_tmp.ac_seqn,
				      &g_mdm_ac_rel, &g_dd_mst, &g_dd_parm);	/* ��ȡ����Ϣ */
		if (ret) {
			sprintf(acErrMsg, "��ȡ�����Ϣerror");
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("aaaaaaaaaaaaaaa[%ld]\n", g_dd_mst.cif_no);
		long            tmpcif_no = 0;
		tmpcif_no = g_dd_mst.cif_no;
		vtcp_log("bbbbbbbbbbbbbbbb[%ld]\n", tmpcif_no);
			sprintf(acErrMsg, "����3312!");
			WRITEMSG

			/* ������� */
			set_zd_data("0250", g_dd_mst.name);	/* ���� */
			set_zd_data("0230", g_dd_mst.prdt_no);	/* ��Ʒ��� */
			set_zd_data("0260", g_dd_parm.title);	/* ��Ʒ���� */
			set_zd_data("0680", g_dd_parm.cif_type);	/* �ͻ����� */
			set_zd_long("0460", g_dd_mst.opn_date);	/* �������� */
		/* ������� */
		sprintf(acErrMsg, "�����������");
		WRITEMSG
			redflag = 1;
		set_zd_long("0350", dd_mst_tmp.ac_seqn);	/* �����˻���� */

		set_zd_int("0660", redflag);	/* �Ƿ�������ű�־
						 * 1-Ψһ��¼ */
		goto OkExit;
	}
	redflag = 9;
	set_zd_int("0660", redflag);	/* �Ƿ�������ű�־ 9-������¼ */

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
