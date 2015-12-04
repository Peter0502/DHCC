/*************************************************
* �� �� ��:  spF061.c
* ���������� �����ն˲�ѯδ���ۼ�¼
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
#include "mdm_unprt_hst_c.h"

int spF061()
{

	int ret = 0;
	int num = 0;
	char cAcno[20];
	char cPass[7];
	char filename[128];
	FILE *fp;
	char tmpstr[128];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_unprt_hst_c sMdm_unprt_hst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cPass, 0x0, sizeof(cPass) );
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sMdm_unprt_hst,0x00,sizeof(sMdm_unprt_hst));
	memset( cPass,0x00,sizeof(cPass));
	get_zd_data( "0370", cAcno );
	get_zd_data( "0790", cPass );

    sprintf(acErrMsg,"---------->Ҫ��ѯ���˺�Ϊ[%s]", cAcno );
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

	ret = Mdm_unprt_hst_Dec_Sel( g_pub_tx.reply , \
			"ac_id = %ld order by tx_date,trace_no", \
			sMdm_ac_rel.ac_id );
	if ( ret )
	{
        sprintf( acErrMsg, "------>����δ������ϸ�α��[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	while (1)
	{
		memset( &sMdm_unprt_hst, 0x00, sizeof(sMdm_unprt_hst) );
		ret = Mdm_unprt_hst_Fet_Sel( &sMdm_unprt_hst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			if ( num == 0 )
			{
        		sprintf( acErrMsg, "------>��δ������ϸ[%d]!!!", ret );
        		WRITEMSG
				strcpy( g_pub_tx.reply, "O190");
        		goto ErrExit;
			}

			break;
		}
		else if ( ret )
		{
        	sprintf( acErrMsg, "------>��ѯδ������ϸ��[%d]!!!", ret );
        	WRITEMSG
        	goto ErrExit;
		}

		pub_base_strpack( sMdm_unprt_hst.brf );
		fprintf( fp, "%ld,%s,%.2f,%.2f\n", sMdm_unprt_hst.tx_date, sMdm_unprt_hst.brf, sMdm_unprt_hst.tx_amt, sMdm_unprt_hst.bal );

		num = num +1;
	}

	Mdm_unprt_hst_Clo_Sel();
	fclose( fp );

	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", num );

	set_zd_data( "0650", tmpstr );
	set_zd_data( "0220", tmpstr );

	set_zd_data( "0370", cAcno );
	set_zd_data( "0700", "0" );

	set_zd_data( "0110", "1" );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"----->��ѯδ������ϸ�ɹ�!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	Mdm_unprt_hst_Clo_Sel();
	fclose( fp );
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

