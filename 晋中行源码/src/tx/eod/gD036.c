/*************************************************************
* �� �� ��: gD036.c
* �����������ڲ��ʻ���Ϣ���ʳ���
*
* ��    ��: jack
* �������: 2003��3��12��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "intst_hst_c.h"

gD036()
{
	int ret = 0;
    struct intst_hst_c intst_hst_c;
    memset(&intst_hst_c, 0x0, sizeof(struct intst_hst_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
	/**----- xxx----- @20030702@ ----**/
	strcpy(g_pub_tx.tx_code,"Z036");
	strcpy(g_pub_tx.sub_tx_code,"Z036");
	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.trace_cnt=0;
	/**-------------------------------**/

    ret = Intst_hst_Dec_Upd(g_pub_tx.reply,"ac_ind='2' and pt_ind='0'");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Intst_hst_Fet_Upd(&intst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if (ret == 100)
        {
            break;
        }

		/**----- xxx ----- @20030702@ ----**/
		strcpy(g_pub_tx.tx_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.opn_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.cur_no,intst_hst_c.cur_no);
		/**-------------------------------**/
        memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
        g_pub_tx.ac_id = intst_hst_c.ac_id;
        g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.add_ind,"1");
        strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
		/**----- xxx ----- @20030702@ ----**
        strcpy(g_pub_tx.ac_id_type,"1");
        g_pub_tx.svc_ind=1001;
        strcpy(g_pub_tx.brf,"��Ϣ");
		**-------------------------------**/
        strcpy(g_pub_tx.ac_id_type,"9");
        g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.brf,"�ڲ���Ϣ����");
        g_pub_tx.tx_amt1 = intst_hst_c.fct_intst;

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"���ڲ��ʻ�����Ϣ����!!");
            WRITEMSG
            goto ErrExit;
        }
        /**------ xxx ------ @20030702@ ------ ���л�Ƽ��� ----**/
        set_zd_data("1214",g_pub_tx.cur_no);
        set_zd_data("1215",g_pub_tx.ct_ind);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A017");
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
		/**----------------------------------------------------**/

        /* ������Ϣ�Ǽǲ� */
        strcpy(intst_hst_c.pt_ind,"1");
        ret = Intst_hst_Upd_Upd(intst_hst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������Ϣ�Ǽǲ�����!!");
            WRITEMSG
            goto ErrExit;
        }

		/**----- xxx ----- @20030702@ ---- ��Ϣ֧����Ŀ���� -----*/
		get_zd_data("0153",g_pub_tx.ac_no);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.add_ind,"0");		/** ���Ǽ��٣��ǽ跽 **/
        strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=9001;
        g_pub_tx.tx_amt1 = intst_hst_c.fct_intst;
		strcpy(g_pub_tx.brf,"�ڲ��ʽ�Ϣ��Ϣ֧��");

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"��Ϣ֧����Ŀ���˴���!!");
            WRITEMSG
            goto ErrExit;
        }

        set_zd_data("1204",g_pub_tx.cur_no);
        set_zd_data("1205",g_pub_tx.ct_ind);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A016");
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
		/**---------------------------------------------------**/
    }
    Eod_Dd_mst_Clo_Upd();
    set_zd_data("1214","");
    set_zd_data("1215","");
    set_zd_double("1218",0.00);
    set_zd_data("1204","");
    set_zd_data("1205","");
    set_zd_double("1208",0.00);

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Թ����ڻ���Ϣ���˳���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�Թ����ڻ���Ϣ���˳���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
