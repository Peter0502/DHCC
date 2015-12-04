/*************************************************
* �� �� ��: spC110.c
* �����������ͻ����Ŷ��¼��
*
* ��    ��: andy
* ������ڣ�2004��01��24��
* �޸ļ�¼��
*     1. ��    ��:
*�� �� ��:
*�޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

	static struct	cif_basic_inf_c		cif_basic_inf;
	static struct	cif_crdt_inf_c		cif_crdt_inf;
	static struct	cif_crdt_code_c		cif_crdt_code;

spC110()
{
	int ret=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ���PASS!");
	WRITEMSG

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));
	memset(&cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));

	/** ȡֵ����ֵ**/
	get_zd_long("0280",&cif_crdt_code.cif_no);
	get_zd_data("0580",cif_crdt_code.credit_no);
	get_zd_data("0580",cif_crdt_inf.credit_no);
	get_zd_data("0210",cif_crdt_inf.cur_no);
	get_zd_double("0400",&cif_crdt_inf.bal);
	get_zd_data("0670",cif_crdt_inf.cyc_ind);

	/** ���̴��� **/
	/* ���ͻ����Ƿ���� */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",
					cif_crdt_code.cif_no);
	if( ret==100)
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	/* �Ǽǿͻ����Ŵ��Ŷ��ձ� */
	ret=Cif_crdt_code_Ins(cif_crdt_code);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ����Ŵ��Ŷ��ձ�ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C028");
		ERR_DEAL;
	}

	/* �Ǽǿͻ�������Ϣ�� */
	ret=Cif_crdt_inf_Ins(cif_crdt_inf);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ��ʧ��![%d]");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C029");
		ERR_DEAL;
	}

	/* �Ǽǽ�����ˮ */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
  	sprintf(acErrMsg,"�Ǽǿͻ�������Ϣʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

