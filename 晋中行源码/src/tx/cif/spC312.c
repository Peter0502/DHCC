/*************************************************
* �� �� ��: spC312.c
* �����������ͻ����Ŷ��ά��
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
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

static struct cif_crdt_inf_c		g_cif_crdt_inf;
static struct cif_crdt_inf_c		cif_crdt_inf;
static struct cif_crdt_code_c		g_cif_crdt_code;
	
char old_alias[30],tbwhere[50],memo[41];
char sts[2];

spC312()
{
	int ret=0;
	int cmp_code=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));
	memset(&g_cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));
	memset(&g_cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));

	/** ȡֵ����ֵ **/
	get_zd_data("0670",sts);								/*��������*/
	get_zd_long("0280",&g_cif_crdt_code.cif_no);			/*�ͻ���*/
	get_zd_data("0580",g_cif_crdt_inf.credit_no);			/*��ͬ���*/
	get_zd_data("0680",g_cif_crdt_inf.cyc_ind);   			/*ѭ����־*/
	get_zd_data("0210",g_cif_crdt_inf.cur_no);  			/*����*/
	get_zd_double("0400",&g_cif_crdt_inf.bal); 			    /*���Ž��*/

	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
				ret=Cif_crdt_inf_Dec_Upd(g_pub_tx.reply,
				 	"credit_no='%s'",g_cif_crdt_inf.credit_no);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE �α��쳣![%s]",g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_crdt_inf_Fet_Upd(&cif_crdt_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_crdt_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				/* 20061010 ����rowid */
				memcpy(g_cif_crdt_inf.rowid, cif_crdt_inf.rowid,sizeof(g_cif_crdt_inf.rowid));
				ret=Cif_crdt_inf_Upd_Upd(g_cif_crdt_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_crdt_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_crdt_inf_Clo_Upd( );
				break;
		case '2':										/*2-ɾ��*/
				/*ɾ��ɾ���ͻ����Ŵ��Ŷ��ձ�����*/
				sprintf(tbwhere,"cif_no=%ld and credit_no='%s'",
						g_cif_crdt_code.cif_no,g_cif_crdt_inf.credit_no);
				ret =deldb("cif_crdt_code",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ����Ŵ��Ŷ��ձ��쳣!![%s]",ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}

				memset(tbwhere,0x00,sizeof(tbwhere));

				/*ɾ���ͻ�������Ϣ������*/
				sprintf(tbwhere,"credit_no='%s'",g_cif_crdt_inf.credit_no);
				ret =deldb("cif_crdt_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ�������Ϣ���쳣!![%s]",ret);
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
    sprintf(acErrMsg,"�ͻ����Ŷ��ά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ����Ŷ��ά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

