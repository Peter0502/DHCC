/*************************************************
* �� �� ��:  spB338.c
* ���������� ������ҵ�����еĳжһ�Ʊ����Ѻ,�����еĳжһ�Ʊ�ϵ����ݽ��еǼ� ��Ӧǰ̨5621
*
* ��    ��:  liuxuan
* ������ڣ� 2006��10��7��
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "whwtsk_reg_c.h"
#include "prdt_ac_id_c.h"
#include "dd_parm_c.h" 
#include "dc_acc_rel_c.h"
#include "mdm_ac_rel_c.h"

int spB338()
{
	char	cBailAcNo[25];				/* ��֤���ʺ� */
	int		ret, num;
	char	Tmp_flag[2];
	struct whwtsk_reg_c sWhwtsk_reg,tmp;
	char    opr[2];
	struct  prdt_ac_id_c sPrdt_ac_id;
	struct  dd_parm_c sDd_parm;
	struct  dc_acc_rel_c sDc_acc_rel;
	struct  mdm_ac_rel_c sMdm_ac_rel;

	memset( &sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset( &sDc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset( &sDd_parm,0x00,sizeof(struct dd_parm_c));
	memset( &sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset( &opr,0x0,sizeof(opr));  
	memset( &sWhwtsk_reg,0x00,sizeof(struct whwtsk_reg_c));
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( &tmp,0x00,sizeof(struct whwtsk_reg_c));

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data("0680",opr);
	get_zd_data("0330",sWhwtsk_reg.pact_no);
	pub_base_strpack( sWhwtsk_reg.pact_no );
		/* ��ǰ̨��Ļȡֵ */
		strcpy( sWhwtsk_reg.tx_br_no, g_pub_tx.tx_br_no );/* ���׻��� */
		get_zd_long( "0450", &sWhwtsk_reg.req_date);/* �������� ��������Ϊ��Ϣ�� */
		get_zd_data( "0260", sWhwtsk_reg.payer_name );	/* ������ȫ�� */
		get_zd_data( "0250", sWhwtsk_reg.payer_br_name );/* �����˿��������� */
		get_zd_data( "0270", sWhwtsk_reg.payee_name );	/* �տ���ȫ�� */
		get_zd_data( "0830", sWhwtsk_reg.payee_ac_no );	/* �տ����˺� */
		get_zd_data( "0810", sWhwtsk_reg.payee_br_name );/* �տ��˿��������� */
		get_zd_double( "1001", &sWhwtsk_reg.sign_amt );	/* ǩ��Ʊ���� */
		get_zd_long( "0440", &sWhwtsk_reg.matr_date );	/* �������� */
		get_zd_double( "1002", &sWhwtsk_reg.tot_mort_amt );/* ��Ѻ��� */
		get_zd_double( "0400", &sWhwtsk_reg.bail_amt_ratio );/* ��֤����� */
		strcpy( sWhwtsk_reg.acc_po_ind, "0" );	/* ��Ʊ״̬: 0δ���ڴ���*/
		get_zd_data( "0380", sWhwtsk_reg.payer_ac_no );	/* �������ʺ� */
		get_zd_double( "1003", &sWhwtsk_reg.bail_amt );	/* ��֤���� */
		get_zd_data( "0670", sWhwtsk_reg.acc_type );	/* �ж����� */
		get_zd_data( "0320", sWhwtsk_reg.bail_ac_no);/* �ڱ���������Ϊ������ҵ�ʺ� */
		strcpy( sWhwtsk_reg.acc_po_sts,"9");/* ������ʾ���й�������ѺƱ */
	vtcp_log("Э�����[%s],Ʊ����:[%lf]",sWhwtsk_reg.pact_no,sWhwtsk_reg.sign_amt );
	
	strncpy(g_pub_tx.ac_no,"717",3);
	strncpy(g_pub_tx.ct_ind,"2",1);
	
	if(opr[0]=='1') /*�Ǽ�*/
	{
vtcp_log("����Ǽǲ���:opr=[%s] [%s]--[%d]",opr,__FILE__,__LINE__);
		ret=Whwtsk_reg_Ins(sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸ĳжһ�Ʊ������Ϣ�ǼǱ����!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0210","1"); /*����*/
		set_zd_data("0660","2"); /*1,�� 2,�� �Ǽ�  �Ǵ�*/
		set_zd_data("0670","2"); /*1,�� 2 ת*/
		set_zd_double("0400",sWhwtsk_reg.sign_amt); /*������*/
	strncpy(g_pub_tx.add_ind,"1",1);
	}
	else if(opr[0]=='2') /*����*/
	{
vtcp_log("��������Ĳ���:opr=[%s] [%s]--[%d]",opr,__FILE__,__LINE__);
		ret=Whwtsk_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sWhwtsk_reg.pact_no);
		if(ret && ret != 100)
		{
			sprintf(acErrMsg,"��ѯwhwtsk_reg�����![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
vtcp_log("�������Dec_Upd:=[%s] [%s]--[%d]",sWhwtsk_reg.pact_no,__FILE__,__LINE__);
		ret=Whwtsk_reg_Fet_Upd(&sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"��ѯwhwtsk_reg�����![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		/* ��ǰ̨��Ļȡֵ */
		strcpy( sWhwtsk_reg.tx_br_no, g_pub_tx.tx_br_no );/* ���׻��� */
		get_zd_long( "0450", &sWhwtsk_reg.req_date);/* �������� ��������Ϊ��Ϣ�� */
		get_zd_data( "0260", sWhwtsk_reg.payer_name );	/* ������ȫ�� */
		get_zd_data( "0250", sWhwtsk_reg.payer_br_name );/* �����˿��������� */
		get_zd_data( "0270", sWhwtsk_reg.payee_name );	/* �տ���ȫ�� */
		get_zd_data( "0830", sWhwtsk_reg.payee_ac_no );	/* �տ����˺� */
		get_zd_data( "0810", sWhwtsk_reg.payee_br_name );/* �տ��˿��������� */
		get_zd_double( "1001", &sWhwtsk_reg.sign_amt );	/* ǩ��Ʊ���� */
		vtcp_log("Ʊ����[%.0f]\n",sWhwtsk_reg.sign_amt);
		get_zd_long( "0440", &sWhwtsk_reg.matr_date );	/* �������� */
		get_zd_double( "1002", &sWhwtsk_reg.tot_mort_amt );/* ��Ѻ��� */
		get_zd_double( "0400", &sWhwtsk_reg.bail_amt_ratio );/* ��֤����� */
		strcpy( sWhwtsk_reg.acc_po_ind, "0" );	/* ��Ʊ״̬: 0δ���ڴ���*/
		get_zd_data( "0380", sWhwtsk_reg.payer_ac_no );	/* �������ʺ� */
		get_zd_double( "1003", &sWhwtsk_reg.bail_amt );	/* ��֤���� */
		get_zd_data( "0670", sWhwtsk_reg.acc_type );	/* �ж����� */
		get_zd_data( "0320", sWhwtsk_reg.bail_ac_no);/* �ڱ���������Ϊ������ҵ�ʺ� */
		strcpy( sWhwtsk_reg.acc_po_sts,"9");/* ������ʾ���й�������ѺƱ */
		
		ret=Whwtsk_reg_Upd_Upd(sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"���±����!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		Whwtsk_reg_Clo_Upd();
		
		set_zd_data("0210","1"); /*����*/
		set_zd_data("0660","1"); /*1,�� 2,�� �����ǽ�*/
		set_zd_data("0670","2"); /*1,�� 2 ת*/
		set_zd_double("0400",sWhwtsk_reg.sign_amt); /*������*/
	strncpy(g_pub_tx.add_ind,"0",1);
	}		
	ret=pub_acct_trance();
	if(ret)
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}


	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "��Ѻ�жһ�Ʊ��Ϣ¼��" );	/* ��ע */

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sWhwtsk_reg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}


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
