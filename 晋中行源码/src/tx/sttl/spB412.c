/*************************************************
* �� �� ��:  spB412.c
* ���������� ��ɴ��������ӡ���ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��1��28��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_fax_c.h"

int spB412()
{
	struct	mo_fax_c	sMoFax;		/* ����ǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ */
	char	cFileName[50];			/* ��ʱ�ļ��� */
	int	ret, num=0;
	FILE 	*fp;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( cFileName, 0x00, sizeof( cFileName ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ���ݹ�Ա�š��ն˺�������ʱ�ļ� */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "�����´�ȫ·���ļ���(����)��" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "��ʱ�ļ�����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}
	
	/* ���ݱ�����š��������ڲ��Ҵ���ǼǱ���д����ʱ�ļ��� */
	/* �������α� */
	ret = Mo_fax_Dec_Upd( g_pub_tx.reply , "tx_br_no='%s' \
				and tx_date=%d ", g_pub_tx.tx_br_no, \
				g_pub_tx.tx_date );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_fax_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"��������", "���׻�����", "������", "������ȫ��", \
			"�����˿������к�", "�����˿���������", "�������˺�", \
			"�������˺����", "������ȫ��", "�տ��˿������к�", \
			"�տ��˿���������", "�տ����˺�", "�տ���ȫ��", \
			"������", "����״̬", "�Ѵ�ӡ����" );
	
	while(1)
	{
		memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
		
		ret = Mo_fax_Fet_Upd( &sMoFax, g_pub_tx.reply );
		if ( ret == 100 && num == 0)
		{
			sprintf( acErrMsg, "��ѯ����Ǽǲ����޼�¼!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B081" );
			goto ErrExit;
		}else if ( ret == 100 && num > 0 )
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_fax_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		/* ���ݸ������˺�ID�͸������˺���ŷ��ظ������˺� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdmAcRel , "ac_id=%ld and ac_seqn=%d", \
					sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
		if( ret == 100 )
    		{
        		sprintf(acErrMsg,"ȡ�������˻���ϵ��mdm_ac_rel��!�޴��˻���Ϣ!ac_id=[%ld],\
        			ac_seqn=[%d]", sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "M003" );
        		goto ErrExit;
    		}else if ( ret )
    		{
        		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%d]", ret );
        		WRITEMSG
        		goto ErrExit;
    		}

		fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|%.2f|%s|%d|\n", \
			g_pub_tx.tx_date, g_pub_tx.tx_br_no, sMoFax.fax_no, \
			sMoFax.requ_name, sMoFax.payer_br_no, sMoFax.payer_br_name, \
			sMdmAcRel.ac_no, sMoFax.payer_ac_seqn, sMoFax.payer_name, \
			sMoFax.payee_br_no, sMoFax.payee_br_name, sMoFax.payee_ac_no, \
			sMoFax.payee_name, sMoFax.fax_amt, sMoFax.fax_sts, sMoFax.pr_cnt );

		num++;	
	}

	Mo_fax_Clo_Upd( );
	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

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
