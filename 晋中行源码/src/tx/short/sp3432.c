/*************************************************
* �� �� ��:  sp3432.c
* ���������� ����˻�����ת��
*
* ��    ��:  ChenChao
* ������ڣ� 2012/3/6 10:23:15
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
insert into tx_def (tx_code, tx_name, bit_map, log_ind, tx_type, wrk_sts, yw_cnt_ind, yw_percent) values ('3432', '����˻�����ת��', '10000000000010000000000000000000000000000000000000000000000000000001000000000000000000000000000000000100000000000000000000000100', '1', '1', '2', '3', null);
insert into tx_sub_def (sub_tx_code, sub_tx_name, tx_func, acct_ind, chnl_ind) values ('3432', '����˻�����Ǩ��', 'sp3432', '0', '0');
insert into tx_flow_def (tx_code, flow_code, sub_tx_code, sub_tx_rslt) values ('3432', 0, '3432', '#$');
insert into tx_dc_rel (sub_tx_code, dc_code, entry_code) values ('3432', '0000', '343200');
insert into dc_entry (entry_code, seqn, cur_no, acc_hrt, dc_ind, use_br_ind, ct_type, ct_ind, amt_no, brf) values ('343200', 1, '0210', '@0152', '0680', '1', '1', '0670', '0400', '������Ǩ��');
*************************************************/
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_c.h"
#include "prdt_ac_id_c.h"
#include "td_parm_c.h"
#include "dd_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "acno_hst_log_c.h"
int sp3432()
{
	int	iRet = 0;
	int	iJz_flag = 0;
	char	cAc_no[22];
	char	cBr_no[6];
	char	cBrNO[6];
	char	cWherelist[60];
	double	dTx_amt = 0.00;
	char	cTx_brNo[9];
	long	lAc_seqn = 0;
	long	lTmp_date=0;
	
	struct	dd_mst_c	sDd_mst;
	struct	td_mst_c	sTd_mst;
	struct	td_parm_c	sTd_parm;
	struct	dd_parm_c	sDd_parm;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	struct	mdm_ac_rel_c	sMdm_ac_rel;
	struct	dd_mst_hst_c	sDd_mst_hst;
	struct	acno_hst_log_c	sAcno_hst_log;
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cBr_no,0x00,sizeof(cBr_no));
	memset(cTx_brNo,0x00,sizeof(cTx_brNo));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sTd_mst,0x00,sizeof(sTd_mst));
	memset(&sTd_parm,0x00,sizeof(sTd_parm));
	memset(&sDd_parm,0x00,sizeof(sDd_parm));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	
	pub_base_sysinit();
	
	get_zd_data("0300",cAc_no);
	pub_base_old_acno(cAc_no);
	get_zd_data("0910",cBr_no);
	get_zd_long("0530",&lAc_seqn);
	get_zd_data("0640",cTx_brNo);
	
	memcpy(g_pub_tx.tx_br_no,cTx_brNo,sizeof(g_pub_tx.tx_br_no)-1);
	
	vtcp_log("[%s][%d]ac_no=[%s]cBr_no=[%s]ac_seqn=[%d]",__FILE__,__LINE__,cAc_no,cBr_no,lAc_seqn);
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
	if(iRet)
	{
		sprintf(acErrMsg," ���˺Ų�����![%s] ",cAc_no);
		WRITEMSG
		goto ErrExit;
	}
	if(lAc_seqn >0 )
	{
		sprintf(cWherelist," and ac_seqn=%ld",lAc_seqn);
	}
	iRet = Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld %s",sMdm_ac_rel.ac_id,cWherelist);
	if(iRet)
	{
		sprintf(acErrMsg," �����α����![%s] ",cAc_no);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		dTx_amt = 0.00;
		memset(cBrNO,0x00,sizeof(cBrNO));
		memset(&sTd_parm,0x00,sizeof(sTd_parm));
		memset(&sDd_parm,0x00,sizeof(sDd_parm));
		memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
		iRet = Prdt_ac_id_Fet_Sel(&sPrdt_ac_id,g_pub_tx.reply);
		if(iRet == 100)
		{
			break;
		}else if(iRet)
		{
			sprintf(acErrMsg," ȡ���α����![%s] ",cAc_no);
			WRITEMSG
			goto ErrExit;
		}
		switch (sPrdt_ac_id.prdt_no[0])
		{
		case '1':
			iRet = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%ld and opn_br_no<>'%s' and ac_sts<>'*'", sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn,cBr_no);
			if(iRet)
			{
				sprintf(acErrMsg, "�������α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			iRet = Dd_mst_Fet_Upd(&sDd_mst, g_pub_tx.reply);
			if(iRet == 100)
			{
				break;
			}else if(iRet)
			{
				sprintf(acErrMsg, "�������α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			dTx_amt = sDd_mst.bal;
			memcpy(cBrNO,sDd_mst.opn_br_no,sizeof(cBrNO)-1);
			memcpy(sDd_mst.opn_br_no,cBr_no,sizeof(sDd_mst.opn_br_no)-1);
			iRet = Dd_mst_Upd_Upd(sDd_mst, g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg, "�����α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			Dd_mst_Clo_Upd();
			memcpy(g_pub_tx.prdt_code,sDd_mst.prdt_no,sizeof(g_pub_tx.prdt_code)-1);			
			iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
			if(iRet)
			{
				sprintf(acErrMsg, "��ѯ���ڲ��������! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			set_zd_data(DC_CODE,sDd_parm.dc_code);
			iJz_flag ++;		/** ��־�ҵ���¼Ҫ����  **/
			break;
		case '2':
			iRet = Td_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%ld and opn_br_no<>'%s' and ac_sts<>'*'", sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn,cBr_no);
			if(iRet)
			{
				sprintf(acErrMsg, "�������α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			iRet = Td_mst_Fet_Upd(&sTd_mst, g_pub_tx.reply);
			if(iRet == 100)
			{
				break;
			}else if(iRet)
			{
				sprintf(acErrMsg, "�������α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			dTx_amt = sTd_mst.bal;
			memcpy(cBrNO,sTd_mst.opn_br_no,sizeof(cBrNO)-1);
			memcpy(sTd_mst.opn_br_no,cBr_no,sizeof(sTd_mst.opn_br_no)-1);
			iRet = Td_mst_Upd_Upd(sTd_mst, g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg, "�����α�����������Ļ�����ϵ!! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			Td_mst_Clo_Upd();
			memcpy(g_pub_tx.prdt_code,sTd_mst.prdt_no,sizeof(g_pub_tx.prdt_code)-1);
			iRet = Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
			if(iRet)
			{
				sprintf(acErrMsg, "��ѯ���ڲ��������! ret=[%d]", iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			set_zd_data(DC_CODE,sTd_parm.dc_code);
			iJz_flag ++;		/** ��־�ҵ���¼Ҫ����  **/
			break;
		default:
			sprintf(acErrMsg, "��Ʒ���ʹ���![%d]", iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply, "G102");
			goto ErrExit;
		}
		if(pub_base_CompDblVal(dTx_amt,0.00)>0)
		{
			set_zd_data("0670","2");	/** ��ת��־  ***/
			set_zd_data("0210","01");
			
			memcpy(g_pub_tx.opn_br_no,cBrNO,sizeof(g_pub_tx.opn_br_no)-1);
			set_zd_data("0020",cBrNO);
			set_zd_data("0680","1");	/** �����־  ***/
			set_zd_double("0400",dTx_amt);	/***   ��ԭ������ת�� �ó�ķ�ʽ ***/
			iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
			if (iRet != 0)
			{
				sprintf(acErrMsg, "�ǿ�Ŀ����ʴ���!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
			memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
			g_pub_tx.tx_amt1 = dTx_amt;
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '0';
			memcpy(g_pub_tx.cur_no,"01",2);
			g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;
			g_pub_tx.ac_id = sPrdt_ac_id.ac_id;
			memcpy(g_pub_tx.brf,"����˻�Ǩ�ƻ���",sizeof(g_pub_tx.brf)-1);
			iRet = pub_ins_trace_log();
			if (iRet != 0)
			{
				sprintf(acErrMsg, "�Ǽ���ˮʧ��!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
			memcpy(g_pub_tx.opn_br_no,cBr_no,sizeof(g_pub_tx.opn_br_no)-1);
			set_zd_data("0020",cBr_no);
			set_zd_data("0680","2");	/** �����־  ***/
			set_zd_double("0400",dTx_amt);	/***   ��ԭ������ת�� �ó�ķ�ʽ ***/
			iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
			if (iRet != 0)
			{
				sprintf(acErrMsg, "�ǿ�Ŀ����ʴ���!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d]ac_id=��%ld��ac_seqn=��%d��������ɣ�����",__FILE__,__LINE__,sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
			g_pub_tx.tx_amt1 = dTx_amt;
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '1';
			memcpy(g_pub_tx.cur_no,"01",2);
			g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;  
			g_pub_tx.ac_id = sPrdt_ac_id.ac_id;
			memcpy(g_pub_tx.brf,"����˻�Ǩ�ƻ���",sizeof(g_pub_tx.brf)-1);
			iRet = pub_ins_trace_log();
			if (iRet != 0)
			{
				sprintf(acErrMsg, "�Ǽ���ˮʧ��!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	if(iJz_flag > 0)
	{
		lTmp_date=g_pub_tx.tx_date/100*100+1;
		vtcp_log(" %s, %d, tmp_date ",__FILE__,__LINE__,lTmp_date);
		/* ���½��ʹ�ϵ��  */
		iRet=sql_execute("update mdm_ac_rel set opn_br_no='%s' where ac_id=%ld and note_sts in('0','1','7')",cBr_no,sMdm_ac_rel.ac_id);
		if(iRet)
		{
			sprintf(acErrMsg, "����mdm_ac_rel�л�������!! ret=[%d]", iRet);
			WRITEMSG
			goto ErrExit;
		}
		/* ���±��µ���ϸ  */
		iRet=Dd_mst_hst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and tx_date>=%ld",sMdm_ac_rel.ac_id,lTmp_date);
		if(iRet)
		{
			sprintf(acErrMsg, "��ѯdd_mst_hst����!! ret=[%d]", iRet);
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset(&sDd_mst_hst,0x00,sizeof(sDd_mst_hst));
			iRet=Dd_mst_hst_Fet_Upd(&sDd_mst_hst,g_pub_tx.reply);
			if(iRet == 100)
				break;
			else if(iRet)
			{
				sprintf(acErrMsg, "Dd_mst_hst_Fet_Upd����!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
			memcpy(sDd_mst_hst.opn_br_no,cBr_no,sizeof(sDd_mst_hst.opn_br_no));
			vtcp_log(" %s, %d, opn_br_no[%s] [%s] ",__FILE__,__LINE__,sDd_mst_hst.opn_br_no,cBr_no);
			iRet=Dd_mst_hst_Upd_Upd(sDd_mst_hst,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg, "Dd_mst_hst_Upd_Upd����!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
		}
		Dd_mst_hst_Clo_Upd();
		iRet=Acno_hst_log_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and tx_date>=%ld",sMdm_ac_rel.ac_id,lTmp_date);
		if(iRet)
		{
			sprintf(acErrMsg, "��ѯdd_mst_hst����!! ret=[%d]", iRet);
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset(&sAcno_hst_log,0x00,sizeof(sAcno_hst_log));
			iRet = Acno_hst_log_Fet_Upd(&sAcno_hst_log,g_pub_tx.reply);
			if(iRet == 100)
				break;
			else if(iRet)
			{
				sprintf(acErrMsg, "Dd_mst_hst_Fet_Upd����!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
			memcpy(sAcno_hst_log.br_no,cBr_no,sizeof(sAcno_hst_log.br_no));
			iRet = Acno_hst_log_Upd_Upd(sAcno_hst_log,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg, "Dd_mst_hst_Upd_Upd����!! ret=[%d]", iRet);
				WRITEMSG
				goto ErrExit;
			}
		}
		Acno_hst_log_Clo_Upd();
	}
	else
	{
		sprintf(acErrMsg, "û�з��ϵ�Ҫ��!! iJz_flag=[%d]", iJz_flag);
		WRITEMSG
		goto ErrExit;
	}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"H034",4);
	}
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
