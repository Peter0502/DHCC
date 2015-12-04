/***********************************************
* �� �� ��: spC104.c
* �����������ͻ�������Ϣ¼��
*
* ��    ��: andy
* ������ڣ�2004��01��15��
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
#include "cif_alias_inf_c.h"

	static struct cif_basic_inf_c	  cif_basic_inf;
	static struct cif_alias_inf_c     cif_alias_inf;
	
spC104()
{
	int ret=0;
	
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));

	get_zd_long("0280",&cif_alias_inf.cif_no);	/*�ͻ���*/
	get_zd_data("0250",cif_alias_inf.alias);	/*�ͻ�����*/

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** �������� **/
	/* ���ͻ�ID�Ƿ���� */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
					"cif_no=%ld",cif_alias_inf.cif_no);
	if(ret==100)
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

	/*��ѯ������Ϣ���Ƿ���ڼ�¼*/
	ret=sql_count("cif_alias_inf","cif_no=%ld",cif_alias_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_alias_inf.alias_seqn=1;

		/* �Ǽǿͻ�������Ϣ�� */
		ret=Cif_alias_inf_Ins(cif_alias_inf);
		if(ret)
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C021");
			goto ErrExit;
		}
	}
	else 
	{
		ret=sql_max_int("cif_alias_inf","alias_seqn", \
						"cif_no=%ld",cif_alias_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			cif_alias_inf.alias_seqn=ret+1;

			/* �Ǽǿͻ�������Ϣ�� */
			ret=Cif_alias_inf_Ins(cif_alias_inf);
			if(ret)
			{
				sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C021");
				goto ErrExit;
			}
		}
	}

	/* �Ǽǽ�����ˮ */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

	/*�������*/
	set_zd_long("0350",cif_alias_inf.alias_seqn);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ�������Ϣ¼��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ͻ�������Ϣ¼��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
