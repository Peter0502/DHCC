/*************************************************
* �� �� ��:  sp9761.c
* ���������� ���֧����ݲ�ѯ
*				�����кſ�ݲ�ѯ����
*
* ��    ��:  jane
* ������ڣ� 2003��5��06��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9761()
{
	char same_sabk_flg[2];	/*�Ƿ����뱾����ͬһ�����еı�־*/
	struct mbfenetbank_c mbfenetbank_v;
	struct mbfeeisbank_c mbfeeisbank_v;

	memset( &mbfenetbank_v, 0x00, sizeof( struct mbfenetbank_c ) );
	memset( &mbfeeisbank_v, 0x00, sizeof( struct mbfeeisbank_c ) );

	strcpy( same_sabk_flg, "0" );	/*�Ƿ����뱾����ͬһ�����еı�־*/

	get_zd_data( "0580", mbfenetbank_v.nbkcode );	/*�����к�*/
	if( strlen( mbfenetbank_v.nbkcode ) == 0 )
	{
		get_zd_data( "0590", mbfenetbank_v.nbkcode );	/*ǩ���кŻ��ֽ��Ʊ�Ҹ��к�*/
		strcpy( same_sabk_flg, "1" );	/*�Ƿ����뱾����ͬһ�����еı�־*/
	}

	if( strlen( mbfenetbank_v.nbkcode ) == 12 )
	{
		g_reply_int = Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v, 
			"nbkcode='%s'", mbfenetbank_v.nbkcode );
		if ( g_reply_int==100 )
		{
			sprintf( acErrMsg, "���кŲ�����" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "��ѯ�������кű����ݿ��������[%d]",g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfenetbank_v.nbkstate[0] != '1' )
		{
			sprintf( acErrMsg, "���к��Ѿ���ע��" );
			MBFEWRITEMSG
			vtcp_log("�кţ���%s", mbfenetbank_v.nbkcode);
			goto ErrExit;
		}

		if( same_sabk_flg[0] == '0' )
		{
			if( get_sys_par_val( "01", mbfetran.isdficode ) ) return -1;
			if( !strcmp( mbfetran.isdficode, mbfenetbank_v.sabkcode ) )
			{
				sprintf( acErrMsg, "�����뱾��Ϊͬһ�����з�Χ�ڵĻ���" );
				MBFEWRITEMSG
				goto ErrExit;
			}
		}

		set_zd_data( "0963", mbfenetbank_v.nbkname );
	}
	else if( strlen( mbfenetbank_v.nbkcode ) == 6 )
	{
		g_reply_int = Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v, 
			"bnkno='%s'", mbfenetbank_v.nbkcode );
		if ( g_reply_int == 100 )
		{
			sprintf( acErrMsg, "���������кŲ�����" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "��ѯ���������кű����ݿ��������[%d]", g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfeeisbank_v.bnksstat[0] != '1' )
		{
			sprintf( acErrMsg, "���к��Ѿ���ע��" );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfeeisbank_v.cnapsflg[0] == 'N' )
		{
			sprintf( acErrMsg, "�����Ѳ���֧��ϵͳ��ת�߻��ҵ��[%s]", mbfeeisbank_v.cnapsbnkno );
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data( "0963", mbfeeisbank_v.bnklname );
	}
	else
	{
		sprintf( acErrMsg, "�к�λ������ȷ" );
		MBFEWRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
