/*************************************************************
* �� �� ��: gD015.c
* �������������Ԥ����Ϣ(���ںͶ���)
*
* ��    ��: jack
* �������: 2003��03��17��
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
#include "gl_prdt_dyn_c.h"
#include "mo_put_intst_c.h"

gD015()
{
	int ret = 0;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	ret = dd_put_intst();
	if( ret ) 
	{
		sprintf( acErrMsg, "���ڼ�����Ϣ�ɹ�!!!!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = td_put_intst();
	if( ret ) 
	{
		sprintf( acErrMsg, "���ڼ�����Ϣ�ɹ�!!!!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���Ԥ����Ϣ�ɹ�!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���Ԥ����Ϣʧ��!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*----------------------------------------------------
* ����: ���ڴ��Ԥ����Ϣ
*
*
*---------------------------------------------------*/
int dd_put_intst()
{
	int ret = 0;

	double dIntst = 0.00;
	double dRate_val = 0.00;

	struct dd_parm_c sDd_parm;
	struct gl_prdt_dyn_c sGl_prdt_dyn;
	struct mo_put_intst_c sMo_put_intst;

	ret = Dd_parm_Dec_Sel ( g_pub_tx.reply , "put_ind = 'Y'");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_parm_Dec_Sel����!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while ( 1 )
	{

		memset( &sDd_parm, 0x00, sizeof(sDd_parm) );

		ret = Dd_parm_Fet_Sel ( &sDd_parm, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_parm_Fet_Sel����!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*------------- ȡ����ֵ ------------*/
		dRate_val = 0.00;
		ret = pub_base_getllz( sDd_parm.rate_no, sDd_parm.cur_no, \
				g_pub_tx.tx_date, &dRate_val);
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Com_rate_Sel����!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Gl_prdt_dyn_Dec_Sel ( g_pub_tx.reply , "prdt_no = '%s' \
				order by br_no", sDd_parm.prdt_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Gl_prdt_dyn_Dec_Sel����!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		while ( 1 )
		{
			
			memset( &sGl_prdt_dyn, 0x00, sizeof(sGl_prdt_dyn) );

			ret = Gl_prdt_dyn_Fet_Sel ( &sGl_prdt_dyn, g_pub_tx.reply);
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Gl_prdt_dyn_Fet_Sel����!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			dIntst = 0.00;
			dIntst = sGl_prdt_dyn.intst_acm * dRate_val / 36000;

			/*--------- ������ ---begin--------*/
        	strcpy(g_pub_tx.prdt_code, sDd_parm.prdt_no);
        	set_zd_data("0151", sDd_parm.dc_code);	/*��ƴ���*/
			set_zd_double("0400", dIntst);

			if ( (sDd_parm.cif_type[0] == '5') || \
				(sDd_parm.cif_type[0] == '1') )		/*���˿ͻ�*/
			{
				set_zd_data ( "0640", "52103");
				set_zd_data ( "0650", "26004");
			}
			else
			{
				set_zd_data ( "0640", "52101");
				set_zd_data ( "0650", "26003");
			}
        
        	strcpy(g_pub_tx.sub_tx_code,"g015");

        	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            	WRITEMSG
            	goto ErrExit;
        	}
			/*--------- ������ ---end----------*/

			ret = sql_execute ( "update gl_prdt_dyn set intst_acm \
					= 0.00 where prdt_no = '%s' and br_no = '%s'", \
					sGl_prdt_dyn.prdt_no, sGl_prdt_dyn.br_no );
			if ( ret )
			{
				sprintf(acErrMsg,"�޸Ĳ�Ʒ��̬���л�������!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			strcpy( sMo_put_intst.prdt_no, sDd_parm.prdt_no );
			sMo_put_intst.tx_date = g_pub_tx.tx_date;
			strcpy( sMo_put_intst.acc_ind, "1" );
			strcpy( sMo_put_intst.opn_brno, sGl_prdt_dyn.br_no );
			sMo_put_intst.intst_acm = sGl_prdt_dyn.intst_acm;
			sMo_put_intst.rate_val = dRate_val;
			sMo_put_intst.intst = dIntst;

			/*-------- дԤ����Ϣ�Ǽǲ� ------------*/
			ret = Mo_put_intst_Ins( sMo_put_intst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Mo_put_intst_Ins����!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		Gl_prdt_dyn_Clo_Sel();
	}
	Dd_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	return 1;
}


/*----------------------------------------------------
* ����: ���ڴ��Ԥ����Ϣ
*
*
*---------------------------------------------------*/
int td_put_intst()
{
	int ret = 0;

	double dIntst = 0.00;
	double dRate_val = 0.00;

	struct td_parm_c sTd_parm;
	struct gl_prdt_dyn_c sGl_prdt_dyn;
	struct mo_put_intst_c sMo_put_intst;

	ret = Td_parm_Dec_Sel ( g_pub_tx.reply , "put_ind = 'Y'");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Td_parm_Dec_Sel����!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while ( 1 )
	{

		memset( &sTd_parm, 0x00, sizeof(sTd_parm) );

		ret = Td_parm_Fet_Sel ( &sTd_parm, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Td_parm_Fet_Sel����!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*------------- ȡ����ֵ ------------*/
		dRate_val = 0.00;
		ret = pub_base_getllz( sTd_parm.rate_no, sTd_parm.cur_no, \
				g_pub_tx.tx_date, &dRate_val);
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Com_rate_Sel����!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Gl_prdt_dyn_Dec_Sel ( g_pub_tx.reply , "prdt_no = '%s' \
				order by br_no", sTd_parm.prdt_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Gl_prdt_dyn_Dec_Sel����!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		while ( 1 )
		{
			
			memset( &sGl_prdt_dyn, 0x00, sizeof(sGl_prdt_dyn) );

			ret = Gl_prdt_dyn_Fet_Sel ( &sGl_prdt_dyn, g_pub_tx.reply);
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Gl_prdt_dyn_Fet_Sel����!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			dIntst = 0.00;
			dIntst = sGl_prdt_dyn.intst_acm * dRate_val / 36000;

			/*--------- ������ ---begin--------*/
        	strcpy(g_pub_tx.prdt_code, sTd_parm.prdt_no);
        	set_zd_data("0151", sTd_parm.dc_code);	/*��ƴ���*/
        	set_zd_data("0210", sTd_parm.cur_no);	/*����*/
			set_zd_double("0400", dIntst);			/*���*/

			if ( (sTd_parm.cif_type[0] == '5') || \
				(sTd_parm.cif_type[0] == '1') )		/*���˿ͻ�*/
			{
				set_zd_data ( "0640", "52103");
				set_zd_data ( "0650", "26004");
			}
			else
			{
				set_zd_data ( "0640", "52101");
				set_zd_data ( "0650", "26003");
			}
        
        	strcpy(g_pub_tx.sub_tx_code,"g015");

        	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            	WRITEMSG
            	goto ErrExit;
        	}
			/*--------- ������ ---end----------*/

			ret = sql_execute ( "update gl_prdt_dyn set intst_acm \
					= 0.00 where prdt_no = '%s' and br_no = '%s'", \
					sGl_prdt_dyn.prdt_no, sGl_prdt_dyn.br_no );
			if ( ret )
			{
				sprintf(acErrMsg,"�޸Ĳ�Ʒ��̬���л�������!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			strcpy( sMo_put_intst.prdt_no, sTd_parm.prdt_no );
			sMo_put_intst.tx_date = g_pub_tx.tx_date;
			strcpy( sMo_put_intst.acc_ind, "2" );
			strcpy( sMo_put_intst.opn_brno, sGl_prdt_dyn.br_no );
			sMo_put_intst.intst_acm = sGl_prdt_dyn.intst_acm;
			sMo_put_intst.rate_val = dRate_val;
			sMo_put_intst.intst = dIntst;

			/*-------- дԤ����Ϣ�Ǽǲ� ------------*/
			ret = Mo_put_intst_Ins( sMo_put_intst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Mo_put_intst_Ins����!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		Gl_prdt_dyn_Clo_Sel();
	}
	Td_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	return 1;
}

