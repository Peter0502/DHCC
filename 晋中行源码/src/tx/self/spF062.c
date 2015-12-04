/*************************************************
* �� �� ��:  spF062.c
* ���������� �����ն˲�ѯ�˻���ϸ
*
* ��    ��:  jack
* ������ڣ� 2003��6��8��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"

int spF062()
{

	int ret = 0;
	int num = 0;
	char cAcno[20];
	char cPass[7];
	long lBegdate= 0;
	long lEnddate= 0;
	char filename[128];
	FILE *fp;
	char tmpstr[128];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_hst_c sDd_mst_hst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cPass, 0x0, sizeof(cPass) );

	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst_hst, 0x00, sizeof(sDd_mst_hst));
	memset( tmpstr, 0x00,sizeof(tmpstr));
	memset( filename,0x00,sizeof(filename));
	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	get_zd_data( "0790", cPass );
	get_zd_long( "0440", &lBegdate );
	get_zd_long( "0450", &lEnddate );

    sprintf( acErrMsg, "ȡ�������ں��˺�Ϊ[%ld][%ld][%s]",
			lBegdate, lEnddate, cAcno);
    WRITEMSG

    if ( pub_base_sysinit() )
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
			"ac_no = '%s'", cAcno );
	if ( ret )
	{
        sprintf( acErrMsg, "��ѯ�˺Ž��ʶ��ձ����[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_EnDes( g_pub_tx.tx_date, "", cPass );

	if ( strcmp( cPass, sMdm_ac_rel.draw_pwd ) )
	{
		strcpy( g_pub_tx.reply, "M005" );
        sprintf( acErrMsg, "------>���������!!!" );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf( acErrMsg,"�����ļ�����![%s]", filename );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P157");
		goto ErrExit;
	}

	ret = ret = Dd_mst_hst_Dec_Sel( g_pub_tx.reply , \
			"ac_id = %ld and tx_date >= %ld and tx_date <= %ld \
			order by tx_date,trace_no", sMdm_ac_rel.ac_id, \
			lBegdate, lEnddate );
	if ( ret )
	{
        sprintf( acErrMsg, "------>���������ϸ�α��[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	while (1)
	{
		memset( &sDd_mst_hst, 0x00, sizeof(sDd_mst_hst) );
		ret = Dd_mst_hst_Fet_Sel( &sDd_mst_hst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			/*	Ŀǰ��ɳ������--------------------------
			if ( num == 0 )
			{
        		sprintf( acErrMsg, "------>�޻�����ϸ[%d]!!!", ret );
        		WRITEMSG
				strcpy( g_pub_tx.reply, "W121");
        		goto ErrExit;
			}
			------------------------------------------*/

			break;
		}
		else if ( ret )
		{
        	sprintf( acErrMsg, "------>��ѯ������ϸ��[%d]!!!", ret );
        	WRITEMSG
        	goto ErrExit;
		}

		fprintf( fp, "%ld,%s,%.2f,%.2f\n", sDd_mst_hst.tx_date, sDd_mst_hst.tx_code, sDd_mst_hst.tx_amt, sDd_mst_hst.bal );

		num = num +1;
	}

	Dd_mst_hst_Clo_Sel();
	fclose( fp );

	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", num );

	set_zd_data( "0650", tmpstr );	/* �ܼ�¼�� */
	set_zd_data( "0220", tmpstr );	/* ���μ�¼�� */

	set_zd_data( "0370", cAcno );
	set_zd_data( "0700", "0" );		/* �Ƿ��м�¼ */

	set_zd_data( "0110", "1" );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"----->��ѯ������ϸ�ɹ�!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	Dd_mst_hst_Clo_Sel();
	fclose( fp );
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

