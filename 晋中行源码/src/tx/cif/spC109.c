/*************************************************
* �� �� ��: spC109.c
* �����������������ڻ�����Ʒ��Ϣ¼��
*
* ��    ��: andy
* ������ڣ�2004��01��20��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_oth_prdt_inf_c.h"

	static struct cif_basic_inf_c	cif_basic_inf;
	static struct cif_oth_prdt_inf_c	cif_oth_prdt_inf;
	static struct cif_oth_prdt_inf_c	g_cif_oth_prdt_inf;
	
spC109()
{
	int ret=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&cif_oth_prdt_inf.cif_no);		/*�ͻ���*/
	get_zd_data("0670",cif_oth_prdt_inf.unit_type);		/*��������*/
	get_zd_data("0910",cif_oth_prdt_inf.unit_no);		/*�������*/
	get_zd_data("0660",cif_oth_prdt_inf.prdt_code);		/*��Ʒ����*/
	get_zd_data("0210",cif_oth_prdt_inf.pro_cur_no);	/*��Ʒ����*/
	get_zd_data("0300",cif_oth_prdt_inf.ac_no);			/*��/����*/
	get_zd_double("0390",&cif_oth_prdt_inf.balance);	/*������*/
	get_zd_double("0400",&cif_oth_prdt_inf.mon_pay);	/*�»����*/
	
	/** �������� **/
	/* ���ͻ����Ƿ���� */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
						 "cif_no=%ld",cif_oth_prdt_inf.cif_no);	
	if(ret==100)
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	ret=Cif_oth_prdt_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld order by inf_no", \
							    cif_oth_prdt_inf.cif_no);
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE cif_oth_prdt_inf�쳣![%s]",
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C039");
		ERR_DEAL;
	}

	while(1)
	{
		ret =Cif_oth_prdt_inf_Fet_Sel(&g_cif_oth_prdt_inf,g_pub_tx.reply);
		if( ret==100 ) 
		{
			cif_oth_prdt_inf.inf_no=g_cif_oth_prdt_inf.inf_no+1;
			break;
		}	
		else if( ret )
		{
			sprintf(acErrMsg,"FETCH cif_oth_prdt_inf�α��쳣![%s]",
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C038");
			ERR_DEAL;
		}
	}

	Cif_lifsty_inf_Clo_Sel ();

	/* �Ǽǿͻ����������ڻ�������Ʒ��Ϣ�� */
	ret=Cif_oth_prdt_inf_Ins(cif_oth_prdt_inf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ʒ��Ϣ��ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C026");
		goto ErrExit;
	}
	
	/* �Ǽǽ�����ˮ */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_long("0350",cif_oth_prdt_inf.inf_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ�������Ʒ��Ϣ¼��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	sprintf(acErrMsg,"�ͻ�������Ʒ��Ϣ¼��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

