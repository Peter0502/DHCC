/*************************************************
* �� �� ��:  spW002.c
* ���������� �˻���Ϣ��ѯ
*
* ��    ��:  chengbo 
* ������ڣ� 2009��3��25��
*
* �޸ļ�¼�� 
* ��   ��:	2009��3��31��
* �� �� ��:	gujingyu *Lsy20100928*
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"

spW002()
{
	int             ret, i = 0, vcnt;
	char            id_no[21];
	char            ac_no[21], filename[300];
	char            id_type[2];
	char            ac_std[10];
	int             ttlnum;
	int             cunqi;
	FILE           *fp;
	double          ttbal = 0;
	long             dBeg_num, dEnd_num;
	int             date;

	struct prdt_ac_id_c sPrdt_ac_id;	/* ��Ʒ�˻��ṹ */
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_attr_c sMdm_attr;
	struct td_mst_c sTd_mst;
	struct dd_mst_c sDd_mst;
	struct dd_parm_c sDd_parm;
	struct td_parm_c sTd_parm;
	/* ��� */
	memset(&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&sTd_mst, 0x00, sizeof(struct td_mst_c));
	memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&sMdm_attr, 0x00, sizeof(struct mdm_attr_c));
	memset(&sTd_parm, 0x00, sizeof(struct td_parm_c));
	memset(&sDd_parm, 0x00, sizeof(struct dd_parm_c));
	memset(ac_no, 0x00, sizeof(ac_no));
	memset(filename, 0x00, sizeof(filename));
	memset(ac_std, 0x00, sizeof(ac_std));
	memset(id_type, 0x00, sizeof(id_type));
	memset(id_no, 0x00, sizeof(id_no));
	/* ȡֵ ����ֵ */
	pub_base_sysinit();
	get_zd_data("0300", ac_no);	/* ��/���� */
	vtcp_log("[%s][%d]�õ����ʺ�ac_no=[%s]\n", __FILE__, __LINE__, ac_no);
	pub_base_old_acno(ac_no);	/**�Ծ��ʺŵĴ���**/
	get_zd_data("0680", id_type);	/**֤������**/
	get_zd_data("0630", id_no);	/**֤������**/
	vtcp_log("[%s][%d]�õ����ʺ�ac_no=[%s],id_type[%s],id_no[%s]\n", __FILE__, __LINE__, ac_no, id_type, id_no);
	get_zd_long("0540", &dBeg_num);	/* ��ʼ������ */
	dBeg_num = dBeg_num - 1;
	dEnd_num = dBeg_num + 10 - 1;	/* ���������� */
	vtcp_log("dBeg_num=[%d]dEnd_num=[%d]",dBeg_num,dEnd_num);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "�޴˿��ţ�����!ac_no=[%s]", ac_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "��ȡ�����˻����ձ��쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]�õ���sMdm_ac_rel.opn_br_no=[%s],sMdm_ac_rel.beg_date=[%ld],sMdm_ac_rel.note_sts=[%s]\n", __FILE__, __LINE__, sMdm_ac_rel.opn_br_no, sMdm_ac_rel.beg_date, sMdm_ac_rel.note_sts);
  set_zd_data("0770", sMdm_ac_rel.opn_br_no);
  set_zd_long("0450", sMdm_ac_rel.beg_date);
  set_zd_data("0670", sMdm_ac_rel.note_sts);
	vtcp_log("[%s][%d]�õ���sMdm_ac_rel.opn_br_no=[%s],sMdm_ac_rel.beg_date=[%ld],sMdm_ac_rel.note_sts=[%s]\n", __FILE__, __LINE__, sMdm_ac_rel.opn_br_no, sMdm_ac_rel.beg_date, sMdm_ac_rel.note_sts);
  
	ret = Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", sMdm_ac_rel.mdm_code);
	if (ret == 100) {
		sprintf(acErrMsg, "�����ڴ˽���!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "��ȡmdm_attr���쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "N056");
		goto ErrExit;
	}
	ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "ac_id=%ld", sMdm_ac_rel.ac_id);
	ERR_DEAL

		ttlnum = 0;

	while (1) {
		ret = Prdt_ac_id_Fet_Sel(&sPrdt_ac_id, g_pub_tx.reply);
		if (ret == 100)
			break;
		ERR_DEAL

			if (!ttlnum) {
			/* �����´�ȫ·���ļ���(����) */
			pub_base_AllDwnFilName(filename);

			fp = fopen(filename, "w");
			if (fp == NULL) {
				sprintf(acErrMsg, "open file error [%s]", filename);
				WRITEMSG
					strcpy(g_pub_tx.reply, "S047");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]�´��ļ�Ϊ%.30s",__FILE__,__LINE__,filename);
			/**fprintf(fp, "~�˺�|�˻����|��ǰ���|����|����|��������|������|��������|�˻�״̬|��������|\n");**/
			fprintf(fp, "~�˺�|��ǰ���|����|����|��������|������|��������|�˻�״̬|��������|\n");
			vtcp_log("%s,%d��ʼ������ѯ�ʺ���Ϣ", __FILE__, __LINE__);
		}
		if (sPrdt_ac_id.ac_id_type[0] == '1') {
			/**��ʼ��ѯ���ڲ�Ʒ!***/
			vtcp_log("[%s][%d]��ѯ���ڲ�Ʒ",__FILE__,__LINE__);
			ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld and ac_seqn=%d", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
			if (ret == 100) {
				sprintf(acErrMsg, "���˻�ID������!ac_no=[%s]", ac_no);
				WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "��ȡ�������ļ��쳣![%d]", ret);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W015");
				goto ErrExit;
			}
			ttbal += sDd_mst.bal;

			ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm, "prdt_no='%s'", \
					  sPrdt_ac_id.prdt_no);
			if (ret == 100) {
				sprintf(acErrMsg, "�ò�Ʒ������!prdt_no=[%s]",
					sPrdt_ac_id.prdt_no);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W025");
				goto ErrExit;
			}
			memset(ac_std, 0x00, sizeof(ac_std));
			if (sDd_mst.ac_sts[0] == '1') {
				strcpy(ac_std, "����");
				date = 99999999;
			} else if (sDd_mst.ac_sts[0] == '3') {
				strcpy(ac_std, "��ʧ");
				date = sDd_mst.lst_date;
			} else if (sDd_mst.ac_sts[0] == '*') {
				strcpy(ac_std, "����");
				date = sDd_mst.lst_date;
			} else
			{
				strcpy(ac_std, "�쳣");
				date = sDd_mst.lst_date;
			}
			/**��ʼ���ʺ���Ϣд���ļ�**/
			/*** ���ɲ�Ʒ�б� ***/
			ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);		
			if(ret)
			{
				sprintf(acErrMsg, "��ȡ�������ļ��쳣![%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "W015");
				goto ErrExit;
			}
				vtcp_log("[%s][%d],dBeg_num=[%d]dEnd_num=[%d]", __FILE__, __LINE__,dBeg_num,dEnd_num);
			if (ttlnum >= dBeg_num && ttlnum <= dEnd_num)	/* �����������ļ�¼���� */
				fprintf(fp, "%20s|%15.2f|%20s|%4d|%8d|%8d|%8d|%6s|%5s|\n", ac_no, sDd_mst.bal, sDd_parm.title, 0000, sDd_mst.opn_date, 99999999, date, ac_std, sDd_mst.opn_br_no);
		}
		if (sPrdt_ac_id.ac_id_type[0] == '2') {
			/**��ѯ���ڲ�Ʒ**/
			vtcp_log("[%s][%d]��ѯ���ڲ�Ʒ",__FILE__,__LINE__);
			ret = Td_mst_Sel(g_pub_tx.reply, &sTd_mst, "ac_id=%ld and ac_seqn=%d", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
			if (ret == 100) {
				sprintf(acErrMsg, "���˻������ڶ��ڲ�Ʒ!");
				WRITEMSG
					strcpy(g_pub_tx.reply, "W016");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "��ȡ�������ļ��쳣![%d]", ret);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W017");
				goto ErrExit;
			}
			ttbal += sTd_mst.bal;
			ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm, "prdt_no='%s'", \
					  sPrdt_ac_id.prdt_no);
			if (ret) {
				goto ErrExit;
			}
			memset(ac_std, 0x00, sizeof(ac_std));
			if (sTd_mst.ac_sts[0] == '1') {
				date = 99999999;
				strcpy(ac_std, "����");
			} else if (sTd_mst.ac_sts[0] == '3') {
				date = sTd_mst.lst_date;
				strcpy(ac_std, "��ʧ");
			} else if (sTd_mst.ac_sts[0] == '*') {
				strcpy(ac_std, "����");
				date = sTd_mst.lst_date;
			} else
			{
				strcpy(ac_std, "�쳣");
				date = sTd_mst.lst_date;
			}
			if ('Y' == sTd_parm.term_type[0])
				cunqi = sTd_parm.term * 12;
			else
				cunqi = sTd_parm.term;
				vtcp_log("[%s][%d],dBeg_num=[%d]dEnd_num=[%d]", __FILE__, __LINE__,dBeg_num,dEnd_num);
			if (ttlnum >= dBeg_num && ttlnum <= dEnd_num)	/* �����������ļ�¼���� */
				fprintf(fp, "%20s|%15.2f|%20s|%4d|%8d|%8d|%8d|%6s|%5s|\n", ac_no, sTd_mst.bal, sTd_parm.title, cunqi, sTd_mst.opn_date, sTd_mst.mtr_date, date, ac_std, sTd_mst.opn_br_no);
		}
		ttlnum++;
	}
	if(sDd_mst.name[0]!='\0')
		{
			vtcp_log("[%s][%d],sDd_mst.name=[%s]", __FILE__, __LINE__,sDd_mst.name);
			set_zd_data("0250", sDd_mst.name);
		}
	else
		{
			vtcp_log("[%s][%d],sTd_mst.name=[%s]", __FILE__, __LINE__,sTd_mst.name);
			set_zd_data("0250", sTd_mst.name);
		}
	ret = Prdt_ac_id_Clo_Sel();
	ERR_DEAL

	vtcp_log("[%s][%d]", __FILE__, __LINE__);

	fclose(fp);
	set_zd_data(DC_FILE_SND, "1");
	set_zd_int("0490", ttlnum);
	vtcp_log("[%s][%d],ttlnum=[%d]", __FILE__, __LINE__, ttlnum);
	set_zd_double("0410", ttbal);
	vtcp_log("[%s][%d],ttbal=[%f]", __FILE__, __LINE__, ttbal);

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
