/***************************************************************
* �� �� ��: sp4541.c
* ��    ��: ��ѯ�����ѺƷ�Ǽǲ������ص�Ѻ���͹��ƽ�
	    ��ѯ��ͬ�µĵ�ѺƷ��Ϣ��
* ��    ��: chenchao
* ������ڣ�2011-8-5 16:01:25
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
insert into tx_def values('4541','���ͬ�µĵ�ѺƷ��Ϣ','10000000000000000000000000000000000000111110000000000000000000001010000000000000000000000000000000000000000000000000000000000000','0','6','2');
insert into tx_sub_def values('4541','���ͬ�µĵ�ѺƷ��Ϣ','sp4541','0','0');
insert into tx_flow_def values('4541','0','4541','#$');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "pact_gaga_rel_c.h"

int sp4541()
{
	int	ret=0;
	int	iUpd_flag=0;
	char	cPact_no[22];/*��ͬ��*/
	char	cTxno[9];
	char	filename[300];
	char	cOutOrIn[2];	/**������־ **/
	FILE	*fp = NULL;
	double	dTot_outamt=0.00; /* ����ܽ�� */
	double	dTot_val=0.00;	/** �ܼ�ֵ **/
	char	gaga_sts[16];

	struct ln_gage_reg_c sLn_gage_reg;
	struct pact_gaga_rel_c sPact_gaga_rel;

	memset(&sPact_gaga_rel, 0x00, sizeof(sPact_gaga_rel));
	memset(&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	memset(cPact_no, 0x00, sizeof(cPact_no));
	memset(cTxno, 0x00, sizeof(cTxno));
	memset(cOutOrIn, 0x00, sizeof(cOutOrIn));
	memset(gaga_sts, 0x00, sizeof(gaga_sts));
	memset(filename, 0x00, sizeof(filename));

	pub_base_sysinit();

	get_zd_data("0370", cPact_no); /**��ͬ�� */
	get_zd_data("0650", cTxno);/* ���״��� */

	vtcp_log(" %s, %d, ��ͬ��[%s] ���״���[%s] ",__FILE__,__LINE__,cPact_no,cTxno);
	if( memcmp( cTxno,"4543",4 ) == 0 )
	{
		cOutOrIn[0] = '0';
	}else{
		cOutOrIn[0] = 'X';
	}
	ret = Ln_gage_reg_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and sts='%s' ",cPact_no,cOutOrIn);
	if(ret)
	{
		sprintf(acErrMsg,"�����α�ʧ��!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����α�ʧ��!!!");
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	ret = Ln_gage_reg_Fet_Sel(&sLn_gage_reg,g_pub_tx.reply);
	if(ret != 100 && ret != 0)
	{
		sprintf(acErrMsg,"��ѯ�����ѺƷ�Ǽǲ�����");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��ѯ�����ѺƷ�Ǽǲ�����");
		strcpy(g_pub_tx.reply, "D102");
		goto ErrExit;
	}
	else if(ret == 100)
	{
		sprintf(acErrMsg,"�����ѺƷ�Ǽǲ����޴��˻���¼");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����ѺƷ�Ǽǲ����޴��˻���¼");
		strcpy(g_pub_tx.reply, "L175");
		goto ErrExit;
	}
	vtcp_log(" %s, %d, ��Ѻ���[%lf] ",__FILE__,__LINE__,sLn_gage_reg.amt);

	pub_base_AllDwnFilName(filename);
	fp = fopen(filename,"w");
	if(fp == NULL)
	{
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	if( memcmp(cTxno,"4543",4) == 0 )
	{
		ret = Pact_gaga_rel_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and stsvar = '1'",cPact_no);
	}
	else
	{
		ret = Pact_gaga_rel_Dec_Sel(g_pub_tx.reply,"pact_no = '%s' and stsvar = 'X'",cPact_no);
	}
	if(ret)
	{
		sprintf(acErrMsg, "�����ͬ�ź͵���ѺƷ��ϸ������ϵ���α����!cPact_no=[%s]",cPact_no);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����ͬ�ź͵���ѺƷ��ϸ������ϵ���α����!");
		strcpy(g_pub_tx.reply, "L328");
		goto ErrExit;
	}
	while(1)
	{
		memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
		ret = Pact_gaga_rel_Fet_Sel(&sPact_gaga_rel, g_pub_tx.reply);
		if(ret == 100)
		{
			if(iUpd_flag)
			{
	 			break;
	 		}
	 		sprintf(acErrMsg, "û���ҵ���Ѻ����Ϣ");
			WRITEMSG
			set_zd_data(DC_GET_MSG, "û���ҵ���Ѻ����Ϣ");
			strcpy(g_pub_tx.reply, "L329");
			goto ErrExit;
		}
		else if(ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "ȡ�ý�ݺź͵���ѺƷ��ϸ������ϵ���α���󣡣�");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ȡ�ý�ݺź͵���ѺƷ��ϸ������ϵ���α���󣡣�");
			strcpy(g_pub_tx.reply, "L329");
			goto ErrExit;
		}
		pub_base_strpack(sPact_gaga_rel.bill_no);
		if( sPact_gaga_rel.pact_type[0] == '2' && strlen(sPact_gaga_rel.bill_no) >0 && memcmp(sPact_gaga_rel.io_ind,"120",3)==0)
		{
			if( memcmp(cTxno,"4543",4) == 0 )
			{
				memcpy(gaga_sts,"������״̬",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				fprintf(fp,"%s|%s|%lf|%lf|%s|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts,sPact_gaga_rel.bill_no);
			}else{
				memcpy(gaga_sts,"�����״̬",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				/* ���״̬�������ֵ�Ѻ������� */
				fprintf(fp,"%s|%s|%lf|%lf|%s|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts,sPact_gaga_rel.bill_no);
			}
			set_zd_data("0670","1");/*** �˴�����жҵ���Ѻ�浥���,��Ҫ������Ѻ�浥¼������ **/
		}else{
			if( memcmp(cTxno,"4543",4) == 0 )
			{
				memcpy(gaga_sts,"������״̬",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				fprintf(fp,"%s|%s|%lf|%lf|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts);
			}else{
				memcpy(gaga_sts,"�����״̬",12);
				dTot_outamt += sPact_gaga_rel.gaga_bal;
				dTot_val += sPact_gaga_rel.gaga_val;
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				/* ���״̬�������ֵ�Ѻ������� */
				fprintf(fp,"%s|%s|%lf|%lf|%s|\n",sPact_gaga_rel.gaga_id,sPact_gaga_rel.name,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val,gaga_sts);
			}
		}
		iUpd_flag++;
	}
	Pact_gaga_rel_Clo_Sel();

	set_zd_double("0420",dTot_outamt);/*����� */
	set_zd_double("0430",dTot_val);/*����ܼ�ֵ*/

	/*4543����ѺƷ���ⲻ��Ҫ���ļ�*/
	if( fp!=NULL )
	{
		set_zd_data(DC_FILE_SND, "1");
		vtcp_log("[%s][%d]���ļ��������ر��ļ�",__FILE__,__LINE__);
		fclose(fp);
	}

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "���׳ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}	
	sprintf(acErrMsg, "����ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
