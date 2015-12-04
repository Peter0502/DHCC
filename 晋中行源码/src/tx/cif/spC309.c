/*************************************************
* �� �� ��: spC309.c
* ����������
		�ͻ�������ʷ��Ϣά��
*
* ��    ��: 
* ������ڣ�2004��01��26��
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

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_wrk_hst_inf_c		cif_wrk_hst_inf;
	static struct cif_wrk_hst_inf_c		g_cif_wrk_hst_inf;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC309()
{
	int ret=0;
	int cmp_code=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));
	memset(&g_cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_data("0700",sts);								/*��������*/
	get_zd_long("0280",&g_cif_wrk_hst_inf.cif_no);			/*�ͻ���*/
	get_zd_long("0490",&g_cif_wrk_hst_inf.wrk_no);			/*�������*/

	get_zd_data("0670",g_cif_wrk_hst_inf.wrk_ind);			/*����״̬*/
	get_zd_data("0220",g_cif_wrk_hst_inf.industry_no);		/*��ҵ����*/
	get_zd_data("0680",g_cif_wrk_hst_inf.vocation_no);		/*ְҵ����*/
	get_zd_data("0690",g_cif_wrk_hst_inf.position_no);		/*ְλ����*/

	get_zd_data("0260",g_cif_wrk_hst_inf.unit_name);		/*��λ����*/
	get_zd_long("0440",&g_cif_wrk_hst_inf.beg_date);		/*��ְ����*/
	get_zd_long("0450",&g_cif_wrk_hst_inf.end_date);		/*��ְ����*/
	get_zd_double("0410",&g_cif_wrk_hst_inf.income);		/*��н����*/

	sprintf(acErrMsg,"%s",g_cif_wrk_hst_inf.unit_name);
	WRITEMSG	
	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
				ret=Cif_wrk_hst_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and wrk_no=%d",g_cif_wrk_hst_inf.cif_no,
					g_cif_wrk_hst_inf.wrk_no);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_wrk_hst_inf_rel�α��쳣![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_wrk_hst_inf_Fet_Upd(&cif_wrk_hst_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_wrk_hst_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				
				
			memcpy(g_cif_wrk_hst_inf.rowid, cif_wrk_hst_inf.rowid,sizeof(g_cif_wrk_hst_inf.rowid));
				ret=Cif_wrk_hst_inf_Upd_Upd(g_cif_wrk_hst_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_wrk_hst_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_wrk_hst_inf_Clo_Upd( );
				break;
		case '2':										/*2-ɾ��*/
				/*ɾ���ͻ�������ʷ��Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld and wrk_no=%d",
						g_cif_wrk_hst_inf.cif_no,g_cif_wrk_hst_inf.wrk_no);
				ret =deldb("cif_wrk_hst_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ�������ʷ��Ϣ���¼�쳣!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

    /* �Ǽǽ�����ˮ */
    if( pub_ins_trace_log() )
    {
         sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
         WRITEMSG
         goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�������Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ�������Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

