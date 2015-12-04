/*************************************************************
* �� �� ��: gD055.c
* ������������������
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
#include "point_point_mst_c.h"
#include "prdt_point_mst_c.h"

gD055()
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

	ret = clr_prt_point_mst();
	if( ret ) 
	{
		sprintf( acErrMsg, "�����Ʒ����ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = clr_ac_point_mst();
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʻ���ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*-----------------------------------------------*
* ����: ���ʻ�����������
*
*
*-----------------------------------------------*/
int clr_ac_point_mst()
{
	int ret = 0;

	struct point_point_mst_c sPoint_point_mst;

	ret = Point_point_mst_Dec_Upd( g_pub_tx.reply , " 1 = 1 ");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Point_point_mst_Dec_Upd����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sPoint_point_mst, 0x00, sizeof(sPoint_point_mst) );

		ret = Point_point_mst_Fet_Upd( &sPoint_point_mst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Fet_Upd����!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		sPoint_point_mst.yt_point = sPoint_point_mst.point + sPoint_point_mst.used_point;
		sPoint_point_mst.point = 0;
		sPoint_point_mst.used_point = 0;

		ret = Point_point_mst_Upd_Upd( sPoint_point_mst, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Point_point_mst_Upd_Upd����!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}


/*-----------------------------------------------*
* ����: ��Ʒ������������
*
*
*-----------------------------------------------*/
int clr_prt_point_mst()
{
	int ret = 0;

	struct prdt_point_mst_c sPrdt_point_mst;

	ret = Prdt_point_mst_Dec_Upd( g_pub_tx.reply , " 1 = 1 ");
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Prdt_point_mst_Dec_Upd����!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sPrdt_point_mst, 0x00, sizeof(sPrdt_point_mst) );

		ret = Prdt_point_mst_Fet_Upd( &sPrdt_point_mst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Fet_Upd����!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		sPrdt_point_mst.yt_point = sPrdt_point_mst.point + sPrdt_point_mst.used_point;
		sPrdt_point_mst.point = 0;
		sPrdt_point_mst.used_point = 0;

		ret = Prdt_point_mst_Upd_Upd( sPrdt_point_mst, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Prdt_point_mst_Upd_Upd����!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

