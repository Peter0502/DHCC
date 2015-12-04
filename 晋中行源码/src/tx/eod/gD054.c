/*************************************************************
* �� �� ��: gD054.c
* ��������������ͳ��
*
* ��    ��: jack
* �������: 2003��03��16��
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
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "point_tx_def_c.h"
#include "point_point_mst_c.h"
#include "point_prdt_def_c.h"
#include "prdt_point_mst_c.h"

gD054()
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

	ret = dd_ac_point();
	if( ret ) 
	{
		sprintf( acErrMsg, "����(����)����ͳ�����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = td_ac_point();
	if( ret ) 
	{
		sprintf( acErrMsg, "����(����)����ͳ�����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = dd_prdt_point();
	if( ret ) 
	{
		sprintf( acErrMsg, "��Ʒ(����)����ͳ�����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = td_prdt_point();
	if( ret ) 
	{
		sprintf( acErrMsg, "��Ʒ(����)����ͳ�����!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͳ�Ƴɹ�!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ͳ��ʧ��!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*---------------------------------------------------*
* ����: ����(����)����ͳ��
*
*
*---------------------------------------------------*/
int dd_ac_point()
{
	int ret = 0;
	int iPoint = 0;

	struct dd_mst_hst_c sDd_mst_hst;
	struct point_tx_def_c sPoint_tx_def;
	struct point_point_mst_c sPoint_point_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret = Dd_mst_hst_Dec_Sel ( g_pub_tx.reply , "tx_date = %ld", g_pub_tx.tx_date);
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_hst_Dec_Selʧ��!!![%s]", \
			g_pub_tx.reply);
		WRITEMSG
	}

	while (1)
	{
		memset( &sDd_mst_hst, 0x00, sizeof(sDd_mst_hst) );

		ret = Dd_mst_hst_Fet_Sel ( &sDd_mst_hst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_hst_Dec_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_tx_def, 0x00, sizeof(sPoint_tx_def) );

		ret = Point_tx_def_Sel( g_pub_tx.reply, &sPoint_tx_def, \
			"tx_code = '%s'", sDd_mst_hst.tx_code);
		if ( ret == 100 )
		{
			continue;		/*---�޶��岻ͳ��---*/
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_tx_def_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (g_pub_tx.tx_date < sPoint_tx_def.beg_date) ||\
				(g_pub_tx.tx_date > sPoint_tx_def.edn_date) )
		{
			continue;	/*---���ڻ��ֲ�ͳ��---*/	
		}

		if ( (sDd_mst_hst.tx_amt > sPoint_tx_def.edn_bal) ||\
				(sDd_mst_hst.tx_amt < sPoint_tx_def.beg_bal) )
		{
			continue;	/*---���ڹ涨���׽�Χ�ڵĽ��ײ�ͳ��---*/	
		}

		iPoint = sDd_mst_hst.tx_amt * sPoint_tx_def.ratio;

		if( iPoint - sPoint_tx_def.max_point > 0.000001) 
		{
			iPoint = sPoint_tx_def.max_point;
		}
		if( iPoint - sPoint_point_mst.point > 0.000001)
		{
			iPoint = sPoint_tx_def.min_point;
		}

		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_id = %d and ac_seqn = %d", sDd_mst_hst.ac_id, sDd_mst_hst.ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Mdm_ac_rel_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_point_mst, 0x00, sizeof(sPoint_point_mst) );

		ret = Point_point_mst_Dec_Upd( g_pub_tx.reply , \
				"ac_no = '%s'", sMdm_ac_rel.ac_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Dec_Updʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Point_point_mst_Fet_Upd( sPoint_point_mst, g_pub_tx.reply );
		if ( ret == 0 )
		{
			if ( (sPoint_point_mst.used_point == 0) && (sPoint_point_mst.used_point == 0) ) 
			{
				sPoint_point_mst.beg_date = g_pub_tx.tx_date;
			}

			sPoint_point_mst.point = sPoint_point_mst.point + iPoint;

			ret = Point_point_mst_Upd_Upd( sPoint_point_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Point_point_mst_Upd_Upd \
					ʧ��!!![%s]", g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret == 100 )
		{
			strcpy( sPoint_point_mst.ac_no, sMdm_ac_rel.ac_no );
			strcpy( sPoint_point_mst.cif_type, "1");
			sPoint_point_mst.beg_date = g_pub_tx.tx_date;
			sPoint_point_mst.point = iPoint;
			sPoint_point_mst.yt_point = 0;
			sPoint_point_mst.used_point = 0;

			ret = Point_point_mst_Ins( sPoint_point_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Point_point_mst_Insʧ��!!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

/*---------------------------------------------------*
* ����: ����(����)����ͳ��
*
*
*---------------------------------------------------*/
int td_ac_point()
{
	int ret = 0;
	int iPoint = 0;

	struct td_mst_hst_c sTd_mst_hst;
	struct point_tx_def_c sPoint_tx_def;
	struct point_point_mst_c sPoint_point_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret = Td_mst_hst_Dec_Sel ( g_pub_tx.reply , "tx_date = %ld", g_pub_tx.tx_date);
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Td_mst_hst_Dec_Selʧ��!!![%s]", \
			g_pub_tx.reply);
		WRITEMSG
	}

	while (1)
	{
		memset( &sTd_mst_hst, 0x00, sizeof(sTd_mst_hst) );

		ret = Td_mst_hst_Fet_Sel ( &sTd_mst_hst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Td_mst_hst_Dec_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_tx_def, 0x00, sizeof(sPoint_tx_def) );

		ret = Point_tx_def_Sel( g_pub_tx.reply, &sPoint_tx_def, \
			"tx_code = '%s'", sTd_mst_hst.tx_code);
		if ( ret == 100 )
		{
			continue;		/*---�޶��岻ͳ��---*/
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_tx_def_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (g_pub_tx.tx_date < sPoint_tx_def.beg_date) ||\
				(g_pub_tx.tx_date > sPoint_tx_def.edn_date) )
		{
			continue;	/*---���ڻ��ֲ�ͳ��---*/	
		}

		if ( (sTd_mst_hst.tx_amt > sPoint_tx_def.edn_bal) ||\
				(sTd_mst_hst.tx_amt < sPoint_tx_def.beg_bal) )
		{
			continue;	/*---���ڹ涨���׽�Χ�ڵĽ��ײ�ͳ��---*/	
		}

		iPoint = sTd_mst_hst.tx_amt * sPoint_tx_def.ratio;

		if( iPoint - sPoint_tx_def.max_point > 0.000001) 
		{
			iPoint = sPoint_tx_def.max_point;
		}
		if( iPoint - sPoint_point_mst.point > 0.000001)
		{
			iPoint = sPoint_tx_def.min_point;
		}

		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_id = %d and ac_seqn = %d", sTd_mst_hst.ac_id, sTd_mst_hst.ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Mdm_ac_rel_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_point_mst, 0x00, sizeof(sPoint_point_mst) );

		ret = Point_point_mst_Dec_Upd( g_pub_tx.reply , \
				"ac_no = '%s'", sMdm_ac_rel.ac_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Dec_Updʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Point_point_mst_Fet_Upd( sPoint_point_mst, g_pub_tx.reply );
		if ( ret == 0 )
		{
			if ( (sPoint_point_mst.used_point == 0) && (sPoint_point_mst.used_point == 0) ) 
			{
				sPoint_point_mst.beg_date = g_pub_tx.tx_date;
			}

			sPoint_point_mst.point = sPoint_point_mst.point + iPoint;

			ret = Point_point_mst_Upd_Upd( sPoint_point_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Point_point_mst_Upd_Upd \
					ʧ��!!![%s]", g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret == 100 )
		{
			strcpy( sPoint_point_mst.ac_no, sMdm_ac_rel.ac_no );
			strcpy( sPoint_point_mst.cif_type, "1");
			sPoint_point_mst.beg_date = g_pub_tx.tx_date;
			sPoint_point_mst.point = iPoint;
			sPoint_point_mst.yt_point = 0;
			sPoint_point_mst.used_point = 0;

			ret = Point_point_mst_Ins( sPoint_point_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Point_point_mst_Insʧ��!!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

/*---------------------------------------------------*
* ����: ��Ʒ(����)����ͳ��
*
*
*---------------------------------------------------*/
int dd_prdt_point()
{
	int ret = 0;
	int iPoint = 0;

	struct dd_mst_hst_c sDd_mst_hst;
	struct dd_mst_c sDd_mst;
	struct point_prdt_def_c sPoint_prdt_def;
	struct prdt_point_mst_c sPrdt_point_mst;

	ret = Dd_mst_hst_Dec_Sel ( g_pub_tx.reply , "tx_date = %ld", g_pub_tx.tx_date);
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_hst_Dec_Selʧ��!!![%s]", \
			g_pub_tx.reply);
		WRITEMSG
	}

	while (1)
	{
		memset( &sDd_mst_hst, 0x00, sizeof(sDd_mst_hst) );

		ret = Dd_mst_hst_Fet_Sel ( &sDd_mst_hst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_hst_Dec_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst,"ac_id = %d \
			and ac_seqn = %d", sDd_mst_hst.ac_id, sDd_mst_hst.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_prdt_def, 0x00, sizeof(sPoint_prdt_def) );
		ret = Point_prdt_def_Sel( g_pub_tx.reply, &sPoint_prdt_def, \
			"prdt_no = '%s'", sDd_mst.prdt_no);
		if ( ret == 100 )
		{
			continue;		/*---�޶��岻ͳ��---*/
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_prdt_def_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (g_pub_tx.tx_date < sPoint_prdt_def.beg_date) ||\
				(g_pub_tx.tx_date > sPoint_prdt_def.edn_date) )
		{
			continue;	/*---���ڻ��ֲ�ͳ��---*/	
		}

		if ( (sDd_mst_hst.tx_amt > sPoint_prdt_def.edn_bal) ||\
				(sDd_mst_hst.tx_amt < sPoint_prdt_def.beg_bal) )
		{
			continue;	/*---���ڹ涨���׽�Χ�ڵĽ��ײ�ͳ��---*/	
		}

		iPoint = sDd_mst_hst.tx_amt * sPoint_prdt_def.ratio;

		if( iPoint - sPoint_prdt_def.max_point > 0.000001) 
		{
			iPoint = sPoint_prdt_def.max_point;
		}
		if( iPoint - sPrdt_point_mst.point > 0.000001)
		{
			iPoint = sPoint_prdt_def.min_point;
		}

		memset( &sPrdt_point_mst, 0x00, sizeof(sPrdt_point_mst) );

		ret = Prdt_point_mst_Dec_Upd( g_pub_tx.reply , \
				"prdt_no = '%s'", sDd_mst.prdt_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Dec_Updʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Prdt_point_mst_Fet_Upd( sPrdt_point_mst, g_pub_tx.reply );
		if ( ret == 0 )
		{
			if ( (sPrdt_point_mst.used_point == 0) && (sPrdt_point_mst.used_point == 0) ) 
			{
				sPrdt_point_mst.beg_date = g_pub_tx.tx_date;
			}

			sPrdt_point_mst.point = sPrdt_point_mst.point + iPoint;

			ret = Prdt_point_mst_Upd_Upd( sPrdt_point_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Prdt_point_mst_Upd_Upd \
					ʧ��!!![%s]", g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret == 100 )
		{
			strcpy( sPrdt_point_mst.prdt_no, sDd_mst.prdt_no );
			sPrdt_point_mst.beg_date = g_pub_tx.tx_date;
			sPrdt_point_mst.point = iPoint;
			sPrdt_point_mst.yt_point = 0;
			sPrdt_point_mst.used_point = 0;

			ret = Prdt_point_mst_Ins( sPrdt_point_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Prdt_point_mst_Insʧ��!!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

/*---------------------------------------------------*
* ����: ��Ʒ(����)����ͳ��
*
*
*---------------------------------------------------*/
int td_prdt_point()
{
	int ret = 0;
	int iPoint = 0;

	struct td_mst_hst_c sTd_mst_hst;
	struct td_mst_c sTd_mst;
	struct point_prdt_def_c sPoint_prdt_def;
	struct prdt_point_mst_c sPrdt_point_mst;

	ret = Td_mst_hst_Dec_Sel ( g_pub_tx.reply , "tx_date = %ld", g_pub_tx.tx_date);
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Td_mst_hst_Dec_Selʧ��!!![%s]", \
			g_pub_tx.reply);
		WRITEMSG
	}

	while (1)
	{
		memset( &sTd_mst_hst, 0x00, sizeof(sTd_mst_hst) );

		ret = Td_mst_hst_Fet_Sel ( &sTd_mst_hst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Td_mst_hst_Dec_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sTd_mst, 0x00, sizeof(sTd_mst) );
		ret = Td_mst_Sel( g_pub_tx.reply, &sTd_mst,"ac_id = %d \
			and ac_seqn = %d", sTd_mst_hst.ac_id, sTd_mst_hst.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Td_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPoint_prdt_def, 0x00, sizeof(sPoint_prdt_def) );
		ret = Point_prdt_def_Sel( g_pub_tx.reply, &sPoint_prdt_def, \
			"prdt_no = '%s'", sTd_mst.prdt_no);
		if ( ret == 100 )
		{
			continue;		/*---�޶��岻ͳ��---*/
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_prdt_def_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( (g_pub_tx.tx_date < sPoint_prdt_def.beg_date) ||\
				(g_pub_tx.tx_date > sPoint_prdt_def.edn_date) )
		{
			continue;	/*---���ڻ��ֲ�ͳ��---*/	
		}

		if ( (sTd_mst_hst.tx_amt > sPoint_prdt_def.edn_bal) ||\
				(sTd_mst_hst.tx_amt < sPoint_prdt_def.beg_bal) )
		{
			continue;	/*---���ڹ涨���׽�Χ�ڵĽ��ײ�ͳ��---*/	
		}

		iPoint = sTd_mst_hst.tx_amt * sPoint_prdt_def.ratio;

		if( iPoint - sPoint_prdt_def.max_point > 0.000001) 
		{
			iPoint = sPoint_prdt_def.max_point;
		}
		if( iPoint - sPrdt_point_mst.point > 0.000001)
		{
			iPoint = sPoint_prdt_def.min_point;
		}

		memset( &sPrdt_point_mst, 0x00, sizeof(sPrdt_point_mst) );

		ret = Prdt_point_mst_Dec_Upd( g_pub_tx.reply , \
				"prdt_no = '%s'", sTd_mst.prdt_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Dec_Updʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Prdt_point_mst_Fet_Upd( sPrdt_point_mst, g_pub_tx.reply );
		if ( ret == 0 )
		{
			if ( (sPrdt_point_mst.used_point == 0) && (sPrdt_point_mst.used_point == 0) ) 
			{
				sPrdt_point_mst.beg_date = g_pub_tx.tx_date;
			}

			sPrdt_point_mst.point = sPrdt_point_mst.point + iPoint;

			ret = Prdt_point_mst_Upd_Upd( sPrdt_point_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Prdt_point_mst_Upd_Upd \
					ʧ��!!![%s]", g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret == 100 )
		{
			strcpy( sPrdt_point_mst.prdt_no, sTd_mst.prdt_no );
			sPrdt_point_mst.beg_date = g_pub_tx.tx_date;
			sPrdt_point_mst.point = iPoint;
			sPrdt_point_mst.yt_point = 0;
			sPrdt_point_mst.used_point = 0;

			ret = Prdt_point_mst_Ins( sPrdt_point_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"ִ��Prdt_point_mst_Insʧ��!!![%s]", \
					g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Selʧ��!!![%s]", \
				g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
