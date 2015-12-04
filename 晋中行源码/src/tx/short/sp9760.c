/*************************************************
* �� �� ��:  sp9760.c
* ���������� ���֧����ݲ�ѯ
*			 ��ѡ��ת�˻��ʱ�����ʺźͽ���ڻ�ҵǼǲ�(mbfeghvsendjrn)
*			 ��ȡһ����¼�����Ի����͵�ַ
*
* ��    ��:  jane
* ������ڣ� 2003��6��3��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "prdt_ac_id_c.h"
#include "cif_addr_inf_c.h"

int sp9760()
{
	char remit_flag[2];
	char oprttype[3];
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mdm_ac_rel_c mdm_ac_rel_v;
	struct prdt_ac_id_c prdt_ac_id_v;
	struct cif_addr_inf_c cif_addr_inf_v;

	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&mdm_ac_rel_v,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	memset(&cif_addr_inf_v,0x00,sizeof(struct cif_addr_inf_c));

	get_zd_char("0670",remit_flag);
	get_zd_data("0300",mbfeghvsendjrn_v.payeracc);
	get_zd_double("0390",&mbfeghvsendjrn_v.amount);

	if( remit_flag[0]=='2' )
		goto OkExit;

	pub_base_sysinit();

	sprintf(acErrMsg, 
		"payeracc='%s' and amount='%f' and procstate='0' and tx_br_no='%s'",
		mbfeghvsendjrn_v.payeracc, mbfeghvsendjrn_v.amount, g_pub_tx.tx_br_no);
		WRITEMSG
	g_reply_int=Mbfeghvsendjrn_Dec_Sel( g_pub_tx.reply, 
		"payeracc='%s' and amount='%f' and procstate='0' and tx_br_no='%s'",
		mbfeghvsendjrn_v.payeracc, mbfeghvsendjrn_v.amount, g_pub_tx.tx_br_no);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ��ҵǼǲ����ݿ���������α����[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfeghvsendjrn_Fet_Sel(&mbfeghvsendjrn_v, g_pub_tx.reply);
	if ( g_reply_int==100 )
	{
		sprintf( acErrMsg, "û�еǼǸñʻ���¼!");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ���Ǽǲ����ݿ����ȡ�α����[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	Mbfeghvsendjrn_Clo_Sel();

	set_zd_data("0250",mbfeghvsendjrn_v.payername);

	g_reply_int=Mdm_ac_rel_Sel( g_pub_tx.reply, &mdm_ac_rel_v, "ac_no='%s'", mbfeghvsendjrn_v.payeracc);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ�ʺŽ��ʶ��ձ����ݿ��������[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Prdt_ac_id_Sel( g_pub_tx.reply , &prdt_ac_id_v , "ac_id=%ld" , 
							  mdm_ac_rel_v.ac_id);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ��Ʒ�˺Ź�ϵ��ͼ�����ݿ��������[%d]",g_reply_int );
		MBFEWRITEMSG
	    goto ErrExit;
	}
	
	g_reply_int=Cif_addr_inf_Sel( g_pub_tx.reply, &cif_addr_inf_v,
		"cif_no='%ld' and addr_seqn=1", prdt_ac_id_v.cif_no);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ�ͻ���ַ��Ϣ�����ݿ��������[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	set_zd_data("0260",cif_addr_inf_v.addr);

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
