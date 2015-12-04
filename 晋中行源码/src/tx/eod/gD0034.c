/*************************************************
* �� �� ��:     gD0034.c
* ����������    �������˻��ּܷ��
* ��    ��:
* ������ڣ�    2012��03��12��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"

int gD0034()
{
	int ret=-1;
	int  t_num=0;
	double sum_sub_bal=0.0;
	double sum_jamt=0.0;
	double sum_damt=0.0;
	double sum_sub_jamt=0.0;
	double sum_sub_damt=0.0;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_mst_c s_dd_mst;
	struct sub_dd_mst_c s_sub_dd_mst;

	memset(&s_mdm_ac_rel ,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	vtcp_log("---�������˻��ּܷ�鿪ʼ---");

	ret=sql_begin(); /* ������ */
	if ( ret )
	{
		vtcp_log("������ʧ��! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT03");
		goto ErrExit;
	}

	pub_base_sysinit(); /* ��ʼ��ȫ�ֱ��� */

	if(pub_base_PubQlsh( &g_pub_tx.trace_no, g_pub_tx.reply))
	{
		vtcp_log("ȡ��������ˮ���� [%d] [%s]",g_pub_tx.trace_no,g_pub_tx.reply);
		strcpy(g_pub_tx.reply,"P298");
		goto ErrExit;
	}

	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply, "draw_pwd_chg='1' and substr(ac_no,1,1)='5' and note_sts='0'");
	if (ret)
	{
		vtcp_log("��mdm_ac_rel��ѯ�ʺų��� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D169");
	}

	while(1)
	{
		sum_sub_bal=0.0;
		memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
		ret=Mdm_ac_rel_Fet_Sel(&s_mdm_ac_rel,g_pub_tx.reply);
		if(ret==100)
		{
			vtcp_log("��ȡ��¼���� �ܹ�:[%d] [%s] [%d]",t_num,__FILE__,__LINE__);
			break;
		}
		else if(ret)
		{
			vtcp_log("��ȡ���ݳ���! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}

		/* �Ƚ�����Ƿ���� */
		ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst, "ac_id=%ld", s_mdm_ac_rel.ac_id);
		if(ret)
		{
			vtcp_log("��ѯ�������ļ�����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("��ʼ��ѯsub_dd_mst�иöԹ��˻��Ƿ��������˻�");
		/**BEGIN ��ѯsub_dd_mst�иöԹ��˻��Ƿ��������˻�����������˻������������ּܷ�� BY LZY 20140310**/
		ret=Sub_dd_mst_Sel( g_pub_tx.reply, &s_sub_dd_mst, "ac_no='%s' and ac_sts='1'", s_mdm_ac_rel.ac_no);
		vtcp_log("ret:[%d]",ret);
		if( ret && ret!=100)
		{
			vtcp_log("��ѯSub_dd_mst���� �ʺ�:[%s] [%d] [%s] [%d]", s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}
		else if(ret==100 )
		{
			vtcp_log("��ѯSub_dd_mst�޴��˺�,�˺�:[%s] [%s] [%d]",s_mdm_ac_rel.ac_no,__FILE__,__LINE__);
			continue;
		}
		/**END ��ѯsub_dd_mst�иöԹ��˻��Ƿ��������˻�����������˻������������ּܷ�� BY LZY 20140310**/
		vtcp_log("������ѯsub_dd_mst�иöԹ��˻��Ƿ��������˻�");

		ret=sql_sum_double( "sub_dd_mst","bal", &sum_sub_bal, "ac_no='%s' and ac_sts='1'", s_mdm_ac_rel.ac_no);
		if( ret )
		{
			vtcp_log("��ѯ���˻����ϼƳ��� �ʺ�:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("���ʺ�:[%s] ���ʺŽ��:[%lf] ���˻��ϼƽ��:[%lf]",s_mdm_ac_rel.ac_no,s_dd_mst.bal,sum_sub_bal);
		if(s_dd_mst.bal != sum_sub_bal)
		{
			vtcp_log("���˻���� �� ���˺źϼƽ�� ���� [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H019");
			goto ErrExit;
		}

		/* �Ƚϵ�����ˮ�Ľ����� */
		ret=sql_sum_double( "dd_mst_hst","tx_amt",&sum_jamt, "ac_id=%ld and ac_seqn=%ld and tx_date=%ld and add_ind='0'",\
			s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("��ѯ�������˻�������ϸ����! �ʺ�:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "dd_mst_hst","tx_amt",&sum_damt, "ac_id=%ld and ac_seqn=%ld and tx_date=%ld and add_ind='1'",\
			s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("��ѯ�������˻�������ϸ����! �ʺ�:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "sub_dd_mst_hst","tx_amt",&sum_sub_jamt, "ac_no=%s and tx_date=%ld and add_ind='0'",\
			s_mdm_ac_rel.ac_no,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("��ѯ�������˻�������ϸ����! �ʺ�:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "sub_dd_mst_hst","tx_amt",&sum_sub_damt, "ac_no=%s and tx_date=%ld and add_ind='1'",\
			s_mdm_ac_rel.ac_no,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("��ѯ�������˻�������ϸ����! �ʺ�:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("���˻����� �跽���:[%lf] �������:[%lf] ���˻����� �跽���:[%lf] �������:[%lf]",\
			sum_jamt,sum_damt,sum_sub_jamt,sum_sub_damt);

		if( (sum_jamt - sum_damt) != (sum_sub_jamt - sum_sub_damt))
		{
			vtcp_log("���˻������˻� ������ϸ������� ����! ���˻�:[%lf] ���˻�[%lf] [%s] [%d]",\
				(sum_jamt - sum_damt),(sum_sub_jamt - sum_sub_damt),__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H019");
			goto ErrExit;
		}

		t_num++;
	}

	vtcp_log("---�������˻��ּܷ�����---");

OkExit:
	strcpy(g_pub_tx.reply,"0000");
  vtcp_log("�������˻��ּܷ��ɹ�!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(0);
ErrExit:
	if(strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply, "G010");
	}
	sql_rollback();		/* ����ع� */
	vtcp_log("�������˻��ּܷ��ʧ��!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(1);
}






