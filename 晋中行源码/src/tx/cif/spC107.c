/*************************************************
* �� �� ��: spC107.c
* �����������ͻ�������Ϣ¼��
*
* ��    ��: andy
* ������ڣ�2004��01��17��
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
#include "cif_wrk_hst_inf_c.h"

static struct cif_basic_inf_c	cif_basic_inf;
static struct cif_wrk_hst_inf_c	cif_wrk_hst_inf;

spC107()
{
	int ret=0;

	/** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
    WRITEMSG

    /** ���ݳ�ʼ�� **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

    /** ȡֵ����ֵ **/
	get_zd_long("0280",&cif_wrk_hst_inf.cif_no);		/*�ͻ�ID*/
	get_zd_data("0670",cif_wrk_hst_inf.wrk_ind);		/*����״̬*/
	get_zd_data("0220",cif_wrk_hst_inf.industry_no); 	/*��ҵ������*/
	get_zd_data("0680",cif_wrk_hst_inf.vocation_no); 	/*ְҵ����*/
	get_zd_data("0690",cif_wrk_hst_inf.position_no); 	/*ְλ����*/

	get_zd_data("0260",cif_wrk_hst_inf.unit_name);		/*��λ*/	
	get_zd_long("0440",&cif_wrk_hst_inf.beg_date);		/*��ְ����*/
	get_zd_long("0450",&cif_wrk_hst_inf.end_date); 		/*��ְ����*/
	get_zd_double("0410",&cif_wrk_hst_inf.income);		/*��н����*/
	
	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����� */
	ret=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,
							"cif_basic_inf.cif_no=%ld",cif_wrk_hst_inf.cif_no);
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
	
	/*���ͻ������Ǹ��˻��ǹ�˾*/
	if(cif_basic_inf.type[0]!='1')
	{
 		sprintf(acErrMsg,"�ÿͻ�Ϊ�Թ��ͻ������õǼ�!![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C081");
        ERR_DEAL;
    }

	/* ���ͻ�������ʷ��Ϣ���Ƿ���ڼ�¼*/
	ret=sql_count("cif_wrk_hst_inf","cif_no=%ld",cif_wrk_hst_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_wrk_hst_inf.wrk_no=1;

		/* �Ǽǿͻ�������ʷ��Ϣ�� */
		ret=Cif_wrk_hst_inf_Ins(cif_wrk_hst_inf);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������ʷ��Ϣ��ʧ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C022");
			goto ErrExit;
		}
	}
	else
	{
		ret=sql_max_int("cif_wrk_hst_inf","wrk_no",\
						"cif_no=%ld",cif_wrk_hst_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			cif_wrk_hst_inf.wrk_no=ret+1;

			/* �Ǽǿͻ�������ʷ��Ϣ�� */
			ret=Cif_wrk_hst_inf_Ins(cif_wrk_hst_inf);
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽǿͻ�������ʷ��Ϣ��ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C022");
				goto ErrExit;
			}

		}
	}
				
	set_zd_long("0350",cif_wrk_hst_inf.wrk_no);

	/*�Ǽǽ�����ˮ*/
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ�������Ϣ¼��ɹ�!!�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ�������Ϣ¼��ʧ��!!�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

