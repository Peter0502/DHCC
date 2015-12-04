/*************************************************
* �� �� ��:  spF064.c
* ���������� �����ն˲��Ǵ���(���ǵ��ͻ����������ҳΪֹ)
*
* ��    ��:  jack
* ������ڣ� 2003��6��9��
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

int spF064()
{

	int ret = 0;
	long num = 0;
	long icount = 0;
	char cAcno[20];

	char filename[128];
	FILE *fp;

	char tmpstr[128];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_unprt_hst_c sMdm_unprt_hst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( filename ,0x00,sizeof(filename));
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sMdm_unprt_hst,0x00,sizeof(sMdm_unprt_hst));
	memset( tmpstr,0x00,sizeof(tmpstr));
	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );

    if ( pub_base_sysinit() )
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

	ret = Mdm_ac_rel_Dec_Upd( g_pub_tx.reply , "ac_no = '%s'", cAcno );
	if ( ret )
	{
        sprintf( acErrMsg, "�����α�Mdm_ac_rel_Dec_Upd��[%d]", ret );
        WRITEMSG
        goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd( &sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
        sprintf( acErrMsg, "��ѯ�˺Ž��ʶ��ձ����[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	if ( sMdm_ac_rel.prt_line % BANK_BOOK_LENG == 0 )
	{
		strcpy( g_pub_tx.reply, "0001" );
        sprintf( acErrMsg, "--------->������ҳ,���������!!!!!" );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_AllDwnFilName( filename );

	sprintf( acErrMsg,"------>���ɲ������ļ���Ϊ[%s]", filename );
	WRITEMSG

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
		fprintf( fp, "%ld,%s,%s,%.2f,%.2f,%s\n", sMdm_unprt_hst.tx_date, sMdm_unprt_hst.brf, sMdm_unprt_hst.add_ind,sMdm_unprt_hst.tx_amt, sMdm_unprt_hst.bal, sMdm_unprt_hst.tel );

		ret = sql_execute( "DELETE FROM mdm_unprt_hst where \
			ac_id = %ld and tx_date = %ld and trace_no = %ld", \
			sMdm_ac_rel.ac_id, sMdm_unprt_hst.tx_date, \
			sMdm_unprt_hst.trace_no );
		if ( ret )
		{
        	sprintf( acErrMsg, "------>ɾ��δ��ӡ��ϸ��[%d]!!!", ret );
        	WRITEMSG
			strcpy( g_pub_tx.reply, "P161" );
        	goto ErrExit;
		}

		num = num +1;	/*	���μ�¼��  */
		icount = num + sMdm_ac_rel.prt_line;	/* ��ӡ��ʼ���� */

		if ( icount % BANK_BOOK_LENG == 0 )
		{
			break;		/* �͵�������ҳ��OK */
		}
	}

	Mdm_unprt_hst_Clo_Sel();
	fclose( fp );

	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", num );
	set_zd_data( "0220", tmpstr );

	num = sMdm_ac_rel.prt_line + 1;
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", num );
	set_zd_data( "0640", tmpstr );

	set_zd_data( "0370", cAcno );

	set_zd_data( "0110", "1" );

	/*	���Ľ����˺Ŷ��ձ��еĴ�ӡ���� */
	sMdm_ac_rel.prt_line = icount;

	ret = Mdm_ac_rel_Upd_Upd( sMdm_ac_rel, g_pub_tx.reply  );
	if ( ret )
	{
        sprintf( acErrMsg, "------>�޸Ľ����˺Ŷ��ձ��[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	ret = Mdm_ac_rel_Clo_Upd( );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf( acErrMsg, "----->���Ǵ��۳ɹ�!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	Mdm_unprt_hst_Clo_Sel();
	Mdm_ac_rel_Clo_Upd();
	fclose( fp );
	sprintf(acErrMsg,"----->���Ǵ���ʧ��[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

