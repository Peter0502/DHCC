/*************************************************
* �� �� ��: spZHJX.c
* ��������:���˻�������Ϣ
*
*
* ��    ��: HOUYI
* �������: 20101104
*
insert into tx_def values ('2212', '���˻���������', '10000000000000000000000011000000000010010000000000000000010000000000000000000000000000001000000100000000000000000000000000000100', '1', '1', '2', '3', null);
insert into tx_sub_def values  ('ZHJX','�ӻ���Ϣ����','spZHJX','1','0');
tx_flow_def
2212	0	ZHJX	#=0000@1@#$
2212	1	D033	#=0000@2@#$
2212	2	D003	#=0000@3@#$
2212	3	d033	#=0000@4@#$
2212	4	NULL	#=0001@5@#=0002@7@#=0003@6@#=0004@8@#$
2212	5	M002	#$
2212	6	D034	#=0000@7@#$
2212	7	D099	#$
2212	8	A017	#$
tx_sub_rel
2212	D033	1	2	0670	������־
2212	D034	0	0310	0300	�Է��˺�
2212	NULL	0	0710	0120	��ת��ʽ
2212	d033	1	0	0390	���
2212	d033	1	3	0670	����
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "public.h"
#include <errno.h>
#include <stdio.h>
#define EXTERN
#include "mdm_ac_rel_c.h"
#include "sub_dd_mst_c.h"
#include "dd_mst_c.h"
#include "intst_hst_c.h"
#include "com_parm_c.h"
#include "sub_intst_hst_c.h"
#include "com_item_c.h"
#include "sub_com_sect_acm_c.h"

spZHJX()
{
	double acm=0.0;
	double sRate=0.0;
	long sub_ac_seqn=0;
	double sect_intst=0.0;
	double sect_acm=0.0;
	/*-- add begin by wzs 20120227
	     reason: ������ʹ��D099������Ϣ ����Ϣ��ֵ��1013,Ӱ���˺����D099�ӽ��׵�ȡֵ --*/
	double tmp_amt;
	/*-- add end by wzs 20120227 --*/
	struct pub_tx sPub_tx;
	struct dd_mst_c sDd_mst;
	struct sub_dd_mst_c sSub_dd_mst;
	struct sub_com_sect_acm_c sSub_com_sect_acm;
	struct intst_hst_c sIntst_hst;
	/*-- add begin by wzs 20120228 reason: ���� ����Ϣд�����˻���Ϣ�Ǽǲ� --*/
	struct sub_intst_hst_c sSub_intst_hst;
	memset(&sSub_intst_hst,0x00,sizeof(struct sub_intst_hst_c));
	/*-- add end by wzs 20120228 --*/
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	memset(&sIntst_hst,0x00,sizeof(sIntst_hst));

	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_long("0780",&sub_ac_seqn);

	/*-- add begin by wzs 20120227
	     reason: ������ʹ��D099������Ϣ ����Ϣ��ֵ��1013,Ӱ���˺����D099�ӽ��׵�ȡֵ --*/
	get_zd_double("1013",&tmp_amt);
	/*-- add end by wzs 20120227 --*/

	if(sub_ac_seqn<=0)
	{
		goto OkExit;
	}
	g_reply_int=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%ld",g_pub_tx.ac_no,sub_ac_seqn);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] Sub_dd_mst_Dec_Upd���� ���룺%d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	g_reply_int=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if(g_reply_int==100)
	{
		vtcp_log("[%s][%d] Sub_dd_mst_Fet_Upd���� ���룺%d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d] Sub_dd_mst_Fet_Upd���� ���룺%d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(sSub_dd_mst.intst_type[0]=='0')/**����Ϣ**/
	{
		Sub_dd_mst_Clo_Upd();
		goto OkExit;
	}
		/******��������˻��ֶλ���,��δ��Ϣ��¼���н�Ϣ*****/
		g_reply_int = Sub_com_sect_acm_Dec_Upd(g_pub_tx.reply,"sub_ac_no='%s' and sts='0'",sSub_dd_mst.sub_ac_no);
		if( g_reply_int )
		{
			strcpy( g_pub_tx.reply,"AT03" );
			sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		while (1) {
			memset(&sSub_com_sect_acm,0,sizeof(sSub_com_sect_acm));
			g_reply_int = Sub_com_sect_acm_Fet_Upd(&sSub_com_sect_acm, g_pub_tx.reply);
			if (g_reply_int == 100)
				break;
			if( g_reply_int )
			{
				strcpy( g_pub_tx.reply,"AT03" );
				sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			memset(&sSub_intst_hst, 0, sizeof(sSub_intst_hst));
			sSub_intst_hst.bic_acm = sSub_com_sect_acm.intst_acm;
			sSub_intst_hst.ic_date = sSub_com_sect_acm.beg_date;
			sSub_intst_hst.intst_date = sSub_com_sect_acm.end_date;
			strcpy(sSub_intst_hst.br_no, sSub_dd_mst.br_no);
			strcpy(sSub_intst_hst.sub_ac_no, sSub_com_sect_acm.sub_ac_no);

			sSub_intst_hst.bic_bal = sSub_dd_mst.bal+sect_intst;
			sSub_intst_hst.bic_acm = sSub_com_sect_acm.intst_acm;

			sSub_intst_hst.fct_intst = pub_base_PubDround_1(sSub_com_sect_acm.intst_acm * sSub_com_sect_acm.rate /12/30/100.00);
			sSub_intst_hst.intst_tax=0.00;
			sect_intst +=(sSub_intst_hst.fct_intst-sSub_intst_hst.intst_tax);/* �ϼƷֶ���Ϣ */
			sect_acm += sSub_com_sect_acm.intst_acm; /* �ϼƷֶλ��� */
			sSub_intst_hst.aic_bal = sSub_dd_mst.bal + sect_intst;
			sSub_intst_hst.aic_acm = 0.00;
			sSub_intst_hst.sub_ac_seqn = sSub_com_sect_acm.sub_ac_seqn;
			sSub_intst_hst.jx_date = g_pub_tx.tx_date;
			strcpy(sSub_intst_hst.ac_no, sSub_com_sect_acm.ac_no);

			strcpy(sSub_intst_hst.ac_ind, "A");
			strcpy(sSub_intst_hst.pt_ind, "1");
			sSub_intst_hst.intst_flag[0]='2'; /*�����Ƿֶλ���������Ϣ */
			sSub_intst_hst.intst_sts[0]='0';  /* ���� */
			sSub_intst_hst.rate = sSub_com_sect_acm.rate;


			if (pub_base_CompDblVal(sSub_intst_hst.fct_intst, 0.00) != 0) {
				g_reply_int = Sub_intst_hst_Ins(sSub_intst_hst, g_pub_tx.reply);
				if( g_reply_int )
				{
					strcpy( g_pub_tx.reply,"AT03" );
					sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);
					WRITEMSG
					goto ErrExit;
				}
			}
			sSub_com_sect_acm.sts[0] = '1'; /* �ֶλ����ó��ѽ�Ϣ״̬ */
			g_reply_int = Sub_com_sect_acm_Upd_Upd(sSub_com_sect_acm, g_pub_tx.reply);
			if( g_reply_int )
			{
				strcpy( g_pub_tx.reply,"AT03" );
				sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("�ֶ���ϢΪ:[%lf] [%s] [%d]",sect_intst,__FILE__,__LINE__);
		}
		Sub_com_sect_acm_Clo_Upd();
		/******����ֶλ���over*****/
	g_reply_int=pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date, &sRate);
	if(sSub_dd_mst.rate_type[0]=='1')
	{
		sRate=sSub_dd_mst.rate;
	}
	/**���˻�����**/
	acm=sSub_dd_mst.bal*pub_base_CalTrueDays(sSub_dd_mst.lst_date,g_pub_tx.tx_date)+sSub_dd_mst.intst_acm;
	vtcp_log("bal=%.2f,lst_date=%ld,tx_date=%ld, intst_acm=%.2f,acm=%.2f",sSub_dd_mst.bal,sSub_dd_mst.lst_date,g_pub_tx.tx_date,sSub_dd_mst.intst_acm,acm);
	/*-- add begin by wzs 20120228 reason: ���� ����Ϣд�����˻���Ϣ�Ǽǲ� --*/
	strncpy(sSub_intst_hst.ac_no,sSub_dd_mst.ac_no,strlen(sSub_dd_mst.ac_no));
	strncpy(sSub_intst_hst.sub_ac_no,sSub_dd_mst.sub_ac_no,strlen(sSub_dd_mst.sub_ac_no));
	strncpy(sSub_intst_hst.br_no,sSub_dd_mst.br_no,strlen(sSub_dd_mst.br_no));
	sSub_intst_hst.sub_ac_seqn=sSub_dd_mst.sub_ac_seqn;
	sSub_intst_hst.bic_bal = sSub_dd_mst.bal+sect_intst;
	sSub_intst_hst.bic_acm = acm;
	sSub_intst_hst.fct_intst=acm*sRate/L360/L100;
	sSub_intst_hst.intst_tax=0.00;
	sSub_intst_hst.aic_bal=sSub_dd_mst.bal+sSub_intst_hst.fct_intst;
	sSub_intst_hst.aic_acm=0.00;
	sSub_intst_hst.ic_date=sSub_dd_mst.ic_date;
	sSub_intst_hst.intst_date=g_pub_tx.tx_date;
	sSub_intst_hst.ac_ind[0]='A'; /* �����˻� */
	sSub_intst_hst.pt_ind[0]='1';
	sSub_intst_hst.rate=sSub_dd_mst.rate;
	sSub_intst_hst.jx_date=g_pub_tx.tx_date;
	sSub_intst_hst.intst_flag[0]='1'; /*����������Ļ���������Ϣ */
	sSub_intst_hst.intst_sts[0]='0';  /* ���� */
	/*-- add end by wzs 20120228 --*/
	sSub_dd_mst.lst_date=g_pub_tx.tx_date;
	sSub_dd_mst.ic_date=g_pub_tx.tx_date;
	sSub_dd_mst.intst_acm=0.0;
	g_reply_int=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] Sub_dd_mst_Upd_Upd���� ���룺%d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Sub_dd_mst_Clo_Upd();

	g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,\
		"ac_id=(select ac_id from mdm_ac_rel where ac_no='%s')",sSub_dd_mst.ac_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� ���룺%d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}

	strncpy(sIntst_hst.br_no,sDd_mst.opn_br_no,strlen(sDd_mst.opn_br_no));
	sIntst_hst.ac_id=sDd_mst.ac_id;
	sIntst_hst.ac_seqn=sDd_mst.ac_seqn;
	strncpy(sIntst_hst.prdt_no,sDd_mst.prdt_no,strlen(sDd_mst.prdt_no));
	strcpy(sIntst_hst.cur_no,"01");
	sIntst_hst.bic_bal=sDd_mst.bal;
	sIntst_hst.bic_acm=acm+sect_acm;
	sIntst_hst.fct_intst=acm*sRate/L360/L100+sect_intst;
	sIntst_hst.intst_tax=0.00;
	sIntst_hst.aic_bal=sDd_mst.bal+acm*sRate/L360/L100+sect_intst;
	sIntst_hst.aic_acm=0.00;
	sIntst_hst.ic_date=sSub_dd_mst.ic_date;
	sIntst_hst.intst_date=g_pub_tx.tx_date;
	sIntst_hst.ac_ind[0]='0';
	sIntst_hst.pt_ind[0]='1';
	sIntst_hst.rate=sSub_dd_mst.rate;

	g_reply_int=Intst_hst_Ins(sIntst_hst,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("�����Ϣ�Ǽǲ� ����! [%d] [%s] [%d]",g_reply_int,__FILE__,__LINE__);
		Intst_hst_Debug(&sIntst_hst);
		strcpy(g_pub_tx.reply,"AT03");
		goto ErrExit;
	}

	/**��Ϣ����**/
	memset(&sPub_tx,0x00,sizeof(sPub_tx));/****����g_pub_tx***/
	memcpy(&sPub_tx,&g_pub_tx,sizeof(g_pub_tx));
	/**���˻�����**/
	g_pub_tx.add_ind[0]='1';
	g_pub_tx.svc_ind=1201;/**��Ϣ����**/
	g_pub_tx.ct_ind[0]='2';
	g_pub_tx.tx_amt1=acm*sRate/L360/L100+sect_intst;
	strcpy(g_pub_tx.brf, "���˻���Ϣ");
	strcpy(g_pub_tx.cur_no,"01");
	vtcp_log("[%s][%d] ��Ϣ:%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	g_reply_int=pub_base_sub_ac_trance(sub_ac_seqn);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "���ʺŴ�ȡ�����ش���!");
		WRITEMSG
		goto ErrExit;
	}
	/**���˻�����**/
	memcpy(&g_pub_tx,&sPub_tx,sizeof(g_pub_tx));
	g_pub_tx.svc_ind=1005;
	g_pub_tx.ct_ind[0]='2';
	g_pub_tx.tx_amt1=acm*sRate/L360/L100+sect_intst;
	strcpy(g_pub_tx.ac_get_ind, "00");
	strcpy(g_pub_tx.ac_id_type, "1");
	strcpy(g_pub_tx.add_ind, "1");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.ac_wrk_ind, "0000000J");
	strcpy(g_pub_tx.brf, "���˻���Ϣ");
	g_reply_int = pub_acct_trance();
	if (g_reply_int) {
		sprintf(acErrMsg, "����ڶԹ��ʻ�����Ϣ����!!");
		WRITEMSG
			goto ErrExit;
	}
	/**add by wyw haha**/
	set_zd_double("1013", g_pub_tx.tx_amt1);
	/**end**/
	set_zd_data("101A","01");/** add by chenchao 20111230 ***/
	vtcp_log("[%s][%d] zylsee018 D099",__FILE__,__LINE__);
	g_reply_int = pubf_acct_proc("D099");
	if (g_reply_int)
		goto ErrExit;
	/**��Ŀ����**/
	/** ��Ϊ�����
	if (intst_acct(acm*sRate, 0.00))
	{
		goto ErrExit;
	}
	**/
	if (intst_acct(g_pub_tx.tx_amt1, 0.00))
	{
		goto ErrExit;
	}

	/*-- add begin by wzs 20120227
	     reason: ������ʹ��D099������Ϣ ����Ϣ��ֵ��1013,Ӱ���˺����D099�ӽ��׵�ȡֵ --*/
	set_zd_double("1013",tmp_amt);
	/*-- add end by wzs 20120227 --*/

	/*-- add begin by wzs 20120228 reason: ���� ����Ϣд�����˻���Ϣ�Ǽǲ� --*/
	g_reply_int = Sub_intst_hst_Ins(sSub_intst_hst, g_pub_tx.reply);
	if( g_reply_int )
	{
		strcpy( g_pub_tx.reply,"AT03" );
		sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	/*-- add end by wzs 20120228 */

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "�����˼�Ϣ�ɹ�!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0')
		strcpy(g_pub_tx.reply, "G010");
	sprintf(acErrMsg, "�����˼�Ϣʧ��!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

int intst_acct( double sum_intst, double sum_tax)
{
	int             ret = 0;
	char cTmpacchrt[ACC_NO_LVL3_LEN+1];
	struct com_item_c sCom_item;

	memset(cTmpacchrt,0x0,sizeof(cTmpacchrt));
	memset(&sCom_item,0x0,sizeof(struct com_item_c));
	get_zd_data("0153",cTmpacchrt);
	pub_base_strpack( cTmpacchrt );
	vtcp_log("[%s][%d]acc_hrt=%s sum_intst=%.2f tx_br_no=%s opn_br_no=%s",__FILE__,__LINE__,cTmpacchrt,sum_intst,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
	/*-----------------------------------------------------*/

	ret = Com_item_Sel( g_pub_tx.reply, &sCom_item,"acc_hrt='%s' ",cTmpacchrt);
	if( ret )
	{
		sprintf(acErrMsg,"��ѯcom_item�쳣[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"D013");
		goto ErrExit;
	}
	memcpy(g_pub_tx.ac_no,sCom_item.acc_no,sizeof(g_pub_tx.ac_no));

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.ac_get_ind, "00");
	strcpy(g_pub_tx.ac_id_type, "9");
	strcpy(g_pub_tx.add_ind, "0");	/** ���Ǽ��٣��ǽ跽 **/
	strcpy(g_pub_tx.ac_wrk_ind, "0000000J");
	strcpy(g_pub_tx.hst_ind, "1");
	g_pub_tx.svc_ind = 9001;
	g_pub_tx.tx_amt1 = sum_intst;
	strcpy(g_pub_tx.brf, "�Թ���Ϣ");
	ret = pub_acct_trance();
	if (ret) {
		sprintf(acErrMsg, "��Ϣ֧����Ŀ���˴���!!");
		WRITEMSG
			goto ErrExit;
	}
	set_zd_data("1204", g_pub_tx.cur_no);
	set_zd_data("1205", g_pub_tx.ct_ind);
	set_zd_double("1208", g_pub_tx.tx_amt1);
	ret = pubf_acct_proc("A016");
	if (ret != 0) {
		sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
			goto ErrExit;
	}


	/**-------�Թ���Ϣ˰��ʱ����---------------------------**/
	if (sum_tax > 0.005 || sum_tax < -0.005) {
		strcpy(g_pub_tx.reply, "AT03");
		sprintf(acErrMsg, "SQLERR [%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}


