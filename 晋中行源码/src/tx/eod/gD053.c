/*************************************************************
* �� �� ��: gD053.c
* ����������ץȡ���ݵ�����
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
#include "mo_jl_bal_c.h"

gD053()
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

	ret = dd_bal_sum();
	if ( ret )
	{
		sprintf(acErrMsg,"�������ͳ�Ƴɹ����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = td_bal_sum();
	if ( ret )
	{
		sprintf(acErrMsg,"�������ͳ�Ƴɹ����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = ln_bal_sum();
	if ( ret )
	{
		sprintf(acErrMsg,"�������ͳ�Ƴɹ����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = in_bal_sum();
	if ( ret )
	{
		sprintf(acErrMsg,"�ڲ������ͳ�Ƴɹ����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}





OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ͳ�Ƴɹ����!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ͳ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*--------------------------------------------*
* ����: ��ɻ�������ͳ��
*
*
*--------------------------------------------*/
int dd_bal_sum()
{
	int ret = 0;

	struct dd_mst_c sDd_mst;
	struct mo_jl_bal_c sMo_jl_bal;
	struct dd_parm_c sDd_parm;

	ret = Dd_mst_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by ac_id, ac_seqn");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_Dec_Selʧ��!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );

		ret = Dd_mst_Fet_Sel ( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Fet_Selʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sDd_parm, 0x00, sizeof(sDd_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply , &sDd_parm, \
				"prdt_no = '%s'",sDd_mst.prdt_no);
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (sDd_mst.bal - sDd_mst.ys_bal > 0.000001) ||\
				(sDd_mst.ys_bal - sDd_mst.bal > 0.000001) ) 
		{
			sMo_jl_bal.date = sDd_mst.lst_date;
			strcpy( sMo_jl_bal.acc_ind, "1" );
			strcpy( sMo_jl_bal.opn_brno, sDd_mst.opn_br_no );
			strcpy( sMo_jl_bal.cur_no, sDd_parm.cur_no );
			sMo_jl_bal.ac_id = sDd_mst.ac_id;
			sMo_jl_bal.ac_seqn = sDd_mst.ac_seqn;
			sMo_jl_bal.ys_bal = sDd_mst.ys_bal;
			sMo_jl_bal.bal = sDd_mst.bal;
			strcpy( sMo_jl_bal.name, sDd_mst.name);

			ret = Mo_jl_bal_Ins( sMo_jl_bal, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Dd_parm_Insʧ��!![%s]ac_id[%d] \
					ac_seqn[%d]", g_pub_tx.reply, sDd_mst.ac_id, \
					sDd_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Dd_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	Dd_parm_Clo_Sel();
	return 1;
}

/*--------------------------------------------*
* ����: ��ɶ�������ͳ��
*
*
*--------------------------------------------*/
int td_bal_sum()
{
	int ret = 0;

	struct td_mst_c sTd_mst;
	struct mo_jl_bal_c sMo_jl_bal;
	struct td_parm_c sTd_parm;

	ret = Dd_mst_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by ac_id, ac_seqn");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_Dec_Selʧ��!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sTd_mst, 0x00, sizeof(sTd_mst) );

		ret = Dd_mst_Fet_Sel ( &sTd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Fet_Selʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sTd_parm, 0x00, sizeof(sTd_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply , &sTd_parm, \
				"prdt_no = '%s'",sTd_mst.prdt_no);
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (sTd_mst.bal - sTd_mst.ys_bal > 0.000001) ||\
				(sTd_mst.ys_bal - sTd_mst.bal > 0.000001) ) 
		{
			sMo_jl_bal.date = sTd_mst.lst_date;
			strcpy( sMo_jl_bal.acc_ind, "2" );
			strcpy( sMo_jl_bal.opn_brno, sTd_mst.opn_br_no );
			strcpy( sMo_jl_bal.cur_no, sTd_parm.cur_no );
			sMo_jl_bal.ac_id = sTd_mst.ac_id;
			sMo_jl_bal.ac_seqn = sTd_mst.ac_seqn;
			sMo_jl_bal.ys_bal = sTd_mst.ys_bal;
			sMo_jl_bal.bal = sTd_mst.bal;
			strcpy( sMo_jl_bal.name, sTd_mst.name);

			ret = Mo_jl_bal_Ins( sMo_jl_bal, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Dd_parm_Insʧ��!![%s]ac_id[%d] \
					ac_seqn[%d]", g_pub_tx.reply, sTd_mst.ac_id, \
					sTd_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Dd_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	Dd_parm_Clo_Sel();
	return 1;
}



/*--------------------------------------------*
* ����: ��ɴ�������ͳ��
*
*
*--------------------------------------------*/
int ln_bal_sum()
{
	int ret = 0;

	struct ln_mst_c sLn_mst;
	struct mo_jl_bal_c sMo_jl_bal;
	struct ln_parm_c sLn_parm;

	ret = Dd_mst_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by ac_id, ac_seqn");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_Dec_Selʧ��!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sLn_mst, 0x00, sizeof(sLn_mst) );

		ret = Dd_mst_Fet_Sel ( &sLn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Fet_Selʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sLn_parm, 0x00, sizeof(sLn_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply , &sLn_parm, \
				"prdt_no = '%s'",sLn_mst.prdt_no);
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (sLn_mst.bal - sLn_mst.ys_bal > 0.000001) ||\
				(sLn_mst.ys_bal - sLn_mst.bal > 0.000001) ) 
		{
			sMo_jl_bal.date = sLn_mst.lst_date;
			strcpy( sMo_jl_bal.acc_ind, "3" );
			strcpy( sMo_jl_bal.opn_brno, sLn_mst.opn_br_no );
			strcpy( sMo_jl_bal.cur_no, sLn_parm.cur_no );
			sMo_jl_bal.ac_id = sLn_mst.ac_id;
			sMo_jl_bal.ac_seqn = sLn_mst.ac_seqn;
			sMo_jl_bal.ys_bal = sLn_mst.ys_bal;
			sMo_jl_bal.bal = sLn_mst.bal;
			strcpy( sMo_jl_bal.name, sLn_mst.name);

			ret = Mo_jl_bal_Ins( sMo_jl_bal, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Dd_parm_Insʧ��!![%s]ac_id[%d] \
					ac_seqn[%d]", g_pub_tx.reply, sLn_mst.ac_id, \
					sLn_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Dd_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	Dd_parm_Clo_Sel();
	return 1;
}


/*--------------------------------------------*
* ����: ����ڲ�������ͳ��
*
*
*--------------------------------------------*/
int in_bal_sum()
{
	int ret = 0;

	struct in_mst_c sIn_mst;
	struct mo_jl_bal_c sMo_jl_bal;
	struct in_parm_c sIn_parm;

	ret = Dd_mst_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by ac_id, ac_seqn");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_Dec_Selʧ��!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sIn_mst, 0x00, sizeof(sIn_mst) );

		ret = Dd_mst_Fet_Sel ( &sIn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Fet_Selʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sIn_parm, 0x00, sizeof(sIn_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply , &sIn_parm, \
				"prdt_no = '%s'",sIn_mst.prdt_no);
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (sIn_mst.bal - sIn_mst.ys_bal > 0.000001) ||\
				(sIn_mst.ys_bal - sIn_mst.bal > 0.000001) ) 
		{
			sMo_jl_bal.date = sIn_mst.lst_date;
			strcpy( sMo_jl_bal.acc_ind, "9" );
			strcpy( sMo_jl_bal.opn_brno, sIn_mst.opn_br_no );
			strcpy( sMo_jl_bal.cur_no, sIn_parm.cur_no );
			sMo_jl_bal.ac_id = sIn_mst.ac_id;
			sMo_jl_bal.ac_seqn = sIn_mst.ac_seqn;
			sMo_jl_bal.ys_bal = sIn_mst.ys_bal;
			sMo_jl_bal.bal = sIn_mst.bal;
			strcpy( sMo_jl_bal.name, sIn_mst.name);

			ret = Mo_jl_bal_Ins( sMo_jl_bal, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Dd_parm_Insʧ��!![%s]ac_id[%d] \
					ac_seqn[%d]", g_pub_tx.reply, sIn_mst.ac_id, \
					sIn_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Dd_parm_Clo_Sel();

OkExit:
	return 0;
ErrExit:
	Dd_parm_Clo_Sel();
	return 1;
}

