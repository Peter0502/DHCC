/*************************************************
* �� �� ��:  spB413.c
* ���������� ������Ĵ�ӡ������ϸ���ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��1��30��
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
#include "com_branch_c.h"

int spB413()
{
	struct	mo_fax_c	sMoFax;		/* ����ǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ */
	struct	com_branch_c	sComBranch; 	/* ����������� */
	char	cFileName[50];			/* ��ʱ�ļ��� */
	char	cOperCode[6];			/* ҵ����� */
	char	cBrNo[6];			/* ������ */
	int	ret, num=0;
	FILE 	*fp;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cBrNo, 0x00, sizeof( cBrNo ) );
	
	strcpy( cOperCode, "10005" );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0910", cBrNo );
	sprintf( acErrMsg, "aaaaaaacbrno = [%s]", cBrNo );		WRITEMSG
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
		strcpy( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	/* �������������Ƿ�Ϊ�� */
	pub_base_strpack( cBrNo );
	
	sprintf( acErrMsg, "aaaaaaaaaaaaaaaaaaaaaaa" );		WRITEMSG
	/* �����������Ų�Ϊ�գ����ӡ����������Ĵ�����ϸ�����ܱ� */
	if( strlen( cBrNo ) != 0 )
	{
		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaa111111" );		WRITEMSG	
		/* ��������Ļ����š��������ڲ��Ҵ���ǼǱ���д����ʱ�ļ��� */
		/* �������α� */
		ret = Mo_fax_Dec_Upd( g_pub_tx.reply , "tx_br_no='%s' \
				and tx_date=%d ", cBrNo, g_pub_tx.tx_date );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_fax_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaa222222222" );		WRITEMSG	
		fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"��������", "���׻�����", "������", "������ȫ��", \
			"�����˿������к�", "�����˿���������", "�������˺�", \
			"�������˺����", "������ȫ��", "�տ��˿������к�", \
			"�տ��˿���������", "�տ����˺�", "�տ���ȫ��", \
			"������", "����״̬", "�Ѵ�ӡ����" );
	
		while(1)
		{
			memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
			memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
		
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
		
			/* ��齻�׻����Ƿ������Ļ���(��ǩ������) */
			ret = pub_sttl_ChkSignBrno( cOperCode, sMoFax.tx_br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "������Ȩ�޴�!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			
			/* ���ݸ������˺�ID�͸������˺���ŷ��ظ������˺� */
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdmAcRel , "ac_id=%ld \
						and ac_seqn=%d", sMoFax.payer_ac_id, \
						sMoFax.payer_ac_seqn );
			if( ret == 100 )
    			{
        			sprintf(acErrMsg,"ȡ�������˻���ϵ��mdm_ac_rel��!�޴��˻���Ϣ!\
        			ac_id=[%ld], ac_seqn=[%d]", sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
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
	}
	/* ���Ϊ�գ����ӡ��������Ĵ�����ϸ�����ܱ� */
	else
	{
		/* ��ѯ�����������,ȡ�������� */
		ret = Com_branch_Dec_Sel( g_pub_tx.reply, "1='%s'", "1" );
		if ( ret )
    		{
        		sprintf( acErrMsg, "ִ��Com_branch_Dec_Sel����!ret=[%d]", ret );
        		WRITEMSG
        		goto ErrExit;
   		}
   		   		
   		while(1)	/*������ѭ����*/
   		{
   			
   			ret = Com_branch_Fet_Sel ( &sComBranch , g_pub_tx.reply );
   			if ( ret == 100 )
   			{
   				break;
   			}else if ( ret )
   			{
	   			sprintf( acErrMsg, "ִ��Com_branch_Fet_Sel����!ret=[%d]", ret );
	        		WRITEMSG
	        		goto ErrExit;
   			}
   		sprintf( acErrMsg, "aaaaaaaaaaaaaaaaasComBranch.br_no=[%s]",sComBranch.br_no );		WRITEMSG	
   			/* ���ݹ�����������еĻ����š��������ڲ��Ҵ���ǼǱ���д����ʱ�ļ��� */
			/* �������α� */
			ret = Mo_fax_Dec_Upd( g_pub_tx.reply , "tx_br_no='%s' \
						and tx_date=%ld ", sComBranch.br_no,\
						g_pub_tx.tx_date );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_fax_Dec_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

   			num = 0;
   			while(1)
			{
				memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
				memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
		
				ret = Mo_fax_Fet_Upd( &sMoFax, g_pub_tx.reply );
				if ( ret == 100  )
				{
					break;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_fax_Fet_Upd��[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				
				if( num == 0 )
				{
					fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
					"��������", "���׻�����", "������", "������ȫ��", \
					"�����˿������к�", "�����˿���������", "�������˺�", \
					"�������˺����", "������ȫ��", "�տ��˿������к�", \
					"�տ��˿���������", "�տ����˺�", "�տ���ȫ��", \
					"������", "����״̬", "�Ѵ�ӡ����" );				
				}
				
				/* ��齻�׻����Ƿ������Ļ���(��ǩ������) */
				ret = pub_sttl_ChkSignBrno( cOperCode, sMoFax.tx_br_no, g_pub_tx.tx_br_no );
				if ( ret )
				{
					sprintf( acErrMsg, "������Ȩ�޴�!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				
				/* ���ݸ������˺�ID�͸������˺���ŷ��ظ������˺� */
				ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdmAcRel , "ac_id=%ld \
							and ac_seqn=%d", sMoFax.payer_ac_id, \
							sMoFax.payer_ac_seqn );
				if( ret == 100 )
    				{
        				sprintf(acErrMsg,"ȡ�������˻���ϵ��mdm_ac_rel��!�޴��˻���Ϣ!\
        				ac_id=[%ld], ac_seqn=[%d]", sMoFax.payer_ac_id, sMoFax.payer_ac_seqn );
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
			}	/* end while */
   		}	/* end while */
   		Com_branch_Clo_Sel ();
	}	/* end if */
	
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
