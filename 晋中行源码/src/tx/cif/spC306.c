/*************************************************
* �� �� ��: spC306.c
* ����������
		�ͻ�������Ϣά��
*
* ��    ��:    
* ������ڣ�   2004��01��26��
* �޸ļ�¼��
*     1. ��    ��:2003-05-31
*        �� �� ��:andy
*        �޸�����:���һ����¼����ɾ��
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_inf_chg_log_c.h"

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_alias_inf_c		cif_alias_inf;
	static struct cif_alias_inf_c		g_cif_alias_inf;
	static struct cif_inf_chg_log_c		cif_inf_chg_log;
	static struct cif_inf_chg_log_c		cif_inf_chg;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC306()
{
	int ret=0;
	int cmp_code=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));
	memset(&g_cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));
	memset(&cif_inf_chg_log,0x00,sizeof(struct cif_inf_chg_log_c));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&g_cif_alias_inf.cif_no);
	get_zd_data("0670",sts);
	get_zd_long("0360",&g_cif_alias_inf.alias_seqn);
	get_zd_data("0270",old_alias);						/*ԭ����*/
	get_zd_data("0250",g_cif_alias_inf.alias);			/*�±���*/
	get_zd_data("0810",memo);

	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
				/* ���ͻ�״̬�Ƿ�ı� */
				if(!strcmp(old_alias,g_cif_alias_inf.alias))
				{
					sprintf(acErrMsg,"δ�����κ��޸ģ�������ύ![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C012");
					ERR_DEAL;
				}

				ret=Cif_alias_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and alias_seqn=%d",g_cif_alias_inf.cif_no,
					g_cif_alias_inf.alias_seqn);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_alias_inf_rel�α��쳣![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_alias_inf_Fet_Upd(&cif_alias_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_alias_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				vtcp_log("rowid==[%20s]\n",cif_alias_inf.rowid);
		memcpy(g_cif_alias_inf.rowid, cif_alias_inf.rowid,sizeof(g_cif_alias_inf.rowid));
				ret=Cif_alias_inf_Upd_Upd(g_cif_alias_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_alias_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_alias_inf_Clo_Upd( );
				break;
		case '2':										/*2-ɾ��*/
				/*ɾ���ͻ�������Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld and alias_seqn=%d",
						g_cif_alias_inf.cif_no,g_cif_alias_inf.alias_seqn);
				ret =deldb("cif_alias_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ�������Ϣ���¼�쳣!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

	/* �Ǽǿͻ���Ϣ�޸���־ */
	cif_inf_chg_log.cif_no=g_cif_alias_inf.cif_no;				/*�ͻ�ID*/
	memcpy(cif_inf_chg_log.chg_tablename,"cif_alias_inf",sizeof(cif_inf_chg_log.chg_tablename));		/*���޸ĵĿͻ���Ϣ����*/
	memcpy(cif_inf_chg_log.chg_columnname,"alias",sizeof(cif_inf_chg_log.chg_columnname));		/*���޸Ŀͻ���Ϣ����*/
	cif_inf_chg_log.chg_date=g_pub_tx.tx_date;					/*�޸�����*/
	cif_inf_chg_log.chg_time=g_pub_tx.tx_time;					/*�޸�ʱ��*/
	memcpy(cif_inf_chg_log.bef_content,old_alias,sizeof(cif_inf_chg_log.bef_content));				/*�޸�ǰ����*/
	memcpy(cif_inf_chg_log.aft_content,g_cif_alias_inf.alias,sizeof(cif_inf_chg_log.aft_content));	/*�޸ĺ�����*/
	memcpy(cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(cif_inf_chg_log.clerk_no));        		/*ְԱ���*/
  memcpy(cif_inf_chg_log.memo,memo,sizeof(cif_inf_chg_log.memo));   						/*����˵��*/

    ret = Cif_inf_chg_log_Ins( cif_inf_chg_log );
	if( cmp_code )
    {
         sprintf(acErrMsg,"�Ǽǿͻ���Ϣ�޸���־ʧ��![%d]",ret);
         WRITEMSG
         strcpy(g_pub_tx.reply,"C013");
         goto ErrExit;
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

