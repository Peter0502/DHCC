/*************************************************************
* �� �� ��: gD009.c
* �������������ɴ���̬��
*
* ��    ��: jane
* �������: 2003��8��31��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if( g_reply_int ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( g_reply_int ) {\
	sprintf(acErrMsg,"SQLERR [%d]",g_reply_int);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "gl_oprt_dyn_c.h"

gD009()
{
	long last_date;
	double _dd_bal;
	double _td_bal;
	double _ln_bal;
    struct com_sys_parm_c com_sys_parm_c;
    struct gl_oprt_dyn_c sGlOprtDyn;
    struct com_branch_c sComBranch;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
    memset(&sGlOprtDyn, 0x0, sizeof(struct gl_oprt_dyn_c));
    memset(&sComBranch, 0x0, sizeof(struct com_branch_c));

    g_reply_int = sql_begin(); /*������*/
	MYSQLERR

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

    /* ��ѯϵͳ������ */
    g_reply_int = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	MYSQLERR
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* ���̺��� */

	last_date = pub_base_daynumSUB( g_pub_tx.tx_date, 1 );

	g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "1=1 order by br_no" );
	MYSQLERR

	while(1)
	{
		g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
		if( g_reply_int == 100 ) break;
		else
			MYSQLERR
		vtcp_log( "������[%s]", sComBranch.br_no );

		memset( &sGlOprtDyn, 0x00, sizeof( struct gl_oprt_dyn_c ) );
		vtcp_log( "date[%ld],br_no[%s]",sGlOprtDyn.date,sComBranch.br_no);
		g_reply_int = Gl_oprt_dyn_Sel( g_pub_tx.reply, &sGlOprtDyn, "\"date\"='%ld' and br_no='%s'", \
			last_date, sComBranch.br_no );
		if( g_reply_int == 100 )
		{
			strcpy( sGlOprtDyn.br_no,sComBranch.br_no);
			vtcp_log( "������[%s],date[%ld]",sGlOprtDyn.br_no,sGlOprtDyn.date);
		}
		else
			MYSQLERR

		sGlOprtDyn.date = g_pub_tx.tx_date;

		get_lst_bal( &sGlOprtDyn );    /*ȡ�������*/

/*���� ��ͳ�Ʋ�Ʒ135��*/
		g_reply_int = sql_sum_double( "dd_mst_bk", "bal", &_dd_bal, "opn_br_no='%s' and prdt_no not in ('135','142')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst_bk", "bal", &_td_bal, "opn_br_no='%s' and prdt_no not in ('135','142')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal = _dd_bal + _td_bal;
		g_reply_int = sql_sum_double( "dd_mst", "ys_bal", &_dd_bal, "cal_code='%s' and prdt_no in ('142')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst", "ys_bal", &_td_bal, "cal_code='%s' and prdt_no in ('142')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal = sGlOprtDyn.cr_de_bal + _dd_bal + _td_bal;
		vtcp_log( "���մ�����[%lf]", sGlOprtDyn.cr_de_bal );

		g_reply_int = sql_sum_double( "dd_mst", "ys_bal", &_dd_bal, "opn_br_no='%s' and cal_code='1' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst", "ys_bal", &_td_bal, "opn_br_no='%s' and cal_code='1' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal1 = _dd_bal + _td_bal;
		vtcp_log( "���ո��˴�����[%lf]", sGlOprtDyn.cr_de_bal1 );

		g_reply_int = sql_sum_double( "dd_mst", "ys_bal", &_dd_bal, "opn_br_no='%s' and cal_code='2' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst", "ys_bal", &_td_bal, "opn_br_no='%s' and cal_code='2' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal2 = _dd_bal + _td_bal;
		vtcp_log( "���չ�˾������[%lf]", sGlOprtDyn.cr_de_bal2 );

		g_reply_int = sql_sum_double( "dd_mst", "ys_bal", &_dd_bal, "opn_br_no='%s' and cal_code='3' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst", "ys_bal", &_td_bal, "opn_br_no='%s' and cal_code='3' and prdt_no not in ('135','136','131','142','138','139')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal3 = _dd_bal + _td_bal;
		vtcp_log( "���ջ�������ҵ���˴�����[%lf]", sGlOprtDyn.cr_de_bal3 );

		g_reply_int = sql_sum_double( "dd_mst_bk", "bal", &_dd_bal, "opn_br_no='%s' and prdt_no='136'", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst_bk", "bal", &_td_bal, "opn_br_no='%s' and prdt_no='136'", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal4 = _dd_bal + _td_bal;
		vtcp_log( "����ͬҵһ�������[%lf]", sGlOprtDyn.cr_de_bal4 );

		g_reply_int = sql_sum_double( "dd_mst_bk", "bal", &_dd_bal, "opn_br_no='%s' and prdt_no in ('131')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst_bk", "bal", &_td_bal, "opn_br_no='%s' and prdt_no in ('131')", sComBranch.br_no );
		MYSQLERR
		g_pub_tx.tx_amt1 = _dd_bal + _td_bal;
		vtcp_log( "���ճжұ�֤�������[%lf]", g_pub_tx.tx_amt1 );
		g_reply_int = sql_sum_double( "dd_mst", "ys_bal", &_dd_bal, "cal_code='%s' and prdt_no in ('142')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst", "ys_bal", &_td_bal, "cal_code='%s' and prdt_no in ('142')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal5 = _dd_bal + _td_bal;
		vtcp_log( "���յ�����֤�������[%lf]", sGlOprtDyn.cr_de_bal5 );
		sGlOprtDyn.cr_de_bal5 = sGlOprtDyn.cr_de_bal5 + g_pub_tx.tx_amt1;
		vtcp_log( "���ձ�֤�������[%lf]", sGlOprtDyn.cr_de_bal5 );

		g_reply_int = sql_sum_double( "dd_mst_bk", "bal", &_dd_bal, "opn_br_no='%s' and prdt_no in ('138','139')", sComBranch.br_no );
		MYSQLERR
		g_reply_int = sql_sum_double( "td_mst_bk", "bal", &_td_bal, "opn_br_no='%s' and prdt_no in ('138','139')", sComBranch.br_no );
		MYSQLERR
		sGlOprtDyn.cr_de_bal6 = _dd_bal + _td_bal;
		vtcp_log( "������ʱ������[%lf]", sGlOprtDyn.cr_de_bal6 );

/*�����*/
		g_reply_int = sql_sum_double( "ln_mst_bk", "bal", &sGlOprtDyn.cr_ln_bal, "opn_br_no='%s'", sComBranch.br_no );
		MYSQLERR
		vtcp_log( "���մ������[%lf]", sGlOprtDyn.cr_ln_bal );

		g_reply_int = sql_sum_double( "ln_mst", "ys_bal", &sGlOprtDyn.cr_ln_bal1, "opn_br_no='%s' and cal_code='1' and prdt_no not in ('3Z1','3Z2')", sComBranch.br_no );
		MYSQLERR
		vtcp_log( "���ո��˴������[%lf]", sGlOprtDyn.cr_ln_bal1 );

		g_reply_int = sql_sum_double( "ln_mst", "ys_bal", &sGlOprtDyn.cr_ln_bal2, "opn_br_no='%s' and cal_code='2' and prdt_no not in ('3Z1','3Z2')", sComBranch.br_no );
		MYSQLERR
		vtcp_log( "���չ�˾�������[%lf]", sGlOprtDyn.cr_ln_bal2 );

		g_reply_int = sql_sum_double( "ln_mst", "ys_bal", &sGlOprtDyn.cr_ln_bal3, "opn_br_no='%s' and cal_code='3' and prdt_no not in ('3Z1','3Z2')", sComBranch.br_no );
		MYSQLERR
		vtcp_log( "���ջ�������ҵ���˴������[%lf]", sGlOprtDyn.cr_ln_bal3 );

		g_reply_int = sql_sum_double( "ln_mst_bk", "bal", &sGlOprtDyn.cr_ln_bal4, "opn_br_no='%s' and prdt_no in ('3Z1','3Z2')", sComBranch.br_no );
		MYSQLERR
		vtcp_log( "�����������[%lf]", sGlOprtDyn.cr_ln_bal4 );

		g_reply_int = sql_sum_double( "mo_bank_po,mo_bank_acc_po", "mo_bank_po.po_amt", &sGlOprtDyn.cr_ln_bal5, "mo_bank_acc_po.tx_br_no='%s' and mo_bank_po.po_sts in ('1','4') and mo_bank_po.pact_no=mo_bank_acc_po.pact_no", sComBranch.br_no );

		MYSQLERR
		vtcp_log( "����ǩ���жһ�Ʊ���[%lf]", sGlOprtDyn.cr_ln_bal5 );

		g_reply_int = Gl_oprt_dyn_Ins( sGlOprtDyn, g_pub_tx.reply );
		if( g_reply_int )
		{
			vtcp_log( "������[%s],date[%ld]",sGlOprtDyn.br_no,sGlOprtDyn.date);
		}
		MYSQLERR
	}
	Com_branch_Clo_Sel();


OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ҵ��״̬��ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ҵ��״̬��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*����ֵ����*/
int get_lst_bal( struct gl_oprt_dyn_c *gl_oprt_dyn )
{
	gl_oprt_dyn->ys_de_bal = gl_oprt_dyn->cr_de_bal;
	gl_oprt_dyn->ys_de_bal1 = gl_oprt_dyn->cr_de_bal1;
	gl_oprt_dyn->ys_de_bal2 = gl_oprt_dyn->cr_de_bal2;
	gl_oprt_dyn->ys_de_bal3 = gl_oprt_dyn->cr_de_bal3;
	gl_oprt_dyn->ys_de_bal4 = gl_oprt_dyn->cr_de_bal4;
	gl_oprt_dyn->ys_de_bal5 = gl_oprt_dyn->cr_de_bal5;
	gl_oprt_dyn->ys_de_bal6 = gl_oprt_dyn->cr_de_bal6;
	gl_oprt_dyn->ys_ln_bal = gl_oprt_dyn->cr_ln_bal;
	gl_oprt_dyn->ys_ln_bal1 = gl_oprt_dyn->cr_ln_bal1;
	gl_oprt_dyn->ys_ln_bal2 = gl_oprt_dyn->cr_ln_bal2;
	gl_oprt_dyn->ys_ln_bal3 = gl_oprt_dyn->cr_ln_bal3;
	gl_oprt_dyn->ys_ln_bal4 = gl_oprt_dyn->cr_ln_bal4;
	gl_oprt_dyn->ys_ln_bal5 = gl_oprt_dyn->cr_ln_bal5;
}
