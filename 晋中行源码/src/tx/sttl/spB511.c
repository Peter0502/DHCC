/*************************************************
* �� �� ��:  spB511.c
* ���������� ͬҵ���뻹����ϸ��ѯ
*
* ��    ��:  martin
* ������ڣ� 2006��9��24��
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
#include "mo_ibl_ret_c.h"

int spB511()
{
	char	cFileName[50];			/* ��ʱ�ļ��� */
	int	ret, num=0;
	FILE 	*fp;

	struct mo_ibl_ret_c sMo_ibl_ret;
	struct mdm_ac_rel_c sMdm_ac_rel;

	char wherelist[500];
	char tmpstr[50];

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( wherelist, 0x0, sizeof(wherelist));
	memset(&sMo_ibl_ret, 0x0, sizeof(struct mo_ibl_ret_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
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

	get_zd_data("0730", sMo_ibl_ret.pact_no);
	get_zd_double("0390", &sMo_ibl_ret.ret_amt);
	get_zd_long("0440", &sMo_ibl_ret.tx_date);

	strcpy(wherelist, " 1=1 ");
	if (strcmp(sMo_ibl_ret.pact_no, "") != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr," and pact_no = '%s' ", sMo_ibl_ret.pact_no);
		strcat(wherelist, tmpstr);
	}

	if (pub_base_CompDblVal(sMo_ibl_ret.ret_amt, 0.00) == 1)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and ret_amt = %lf ", sMo_ibl_ret.ret_amt);
		strcat(wherelist, tmpstr);
	}

	if (sMo_ibl_ret.tx_date > 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and tx_date = %ld ", sMo_ibl_ret.tx_date);
		strcat(wherelist, tmpstr);
	}
  strcat(wherelist, " order by pact_no , tx_br_no ");
	sprintf(acErrMsg, "-------> wherelist = [%s]", wherelist);
	WRITEMSG

	ret = Mo_ibl_ret_Dec_Upd( g_pub_tx.reply , wherelist);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_ibl_ret_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", 
			"��ͬ��", "��������", "�黹����", "�黹��Ϣ",
			"��������", "������ˮ��", "��־", "����Ա", "����Ա", 
			"��Ȩ��");
	/*
	"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"��ͬ��", "����˺�", "�Է��˺�", "��Ʒ���", "�������", \
			"��軧��", "�������", "�����", "�������", "��������", \
			"��Ƿ���", "��������","��������", "��������", "���״̬");
	*/
	while(1)
	{
		memset( &sMo_ibl_ret, 0x0, sizeof( struct mo_ibl_ret_c ) );
		
		ret = Mo_ibl_ret_Fet_Upd( &sMo_ibl_ret, g_pub_tx.reply );
		if ( ret == 100 && num == 0)
		{
			sprintf( acErrMsg, "ͬҵ���뻹��Ǽǲ����޼�¼!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B254" );
			goto ErrExit;
		}
		else if ( ret == 100 && num > 0 )
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "��ѯmo_ibl_ret����!! [%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
/***12:55 here ���mdm_ac_rel  
		if (sMo_ibl_ret.sts[0] != '1')
		{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and \
			ac_seqn=0", sMo_ibl.ac_id);
		if( ret == 100 ) 
		{
        	sprintf(acErrMsg,"ȡ�������˻���ϵ��mdm_ac_rel��!�޴��˻���Ϣ! \
					ac_id=[%ld]", sMo_ibl.ac_id);
       		WRITEMSG
       		strcpy( g_pub_tx.reply, "M003" );
       		goto ErrExit;
    	}
		else if ( ret )
    	{
       		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%d]", ret );
       		WRITEMSG
       		goto ErrExit;
    	}
		}
***/		
			switch(sMo_ibl_ret.sts[0])
			{
				case '1':
			fprintf(fp,"%s|%s|%.2lf|%.2lf|%ld|%ld|%s|%s|%s|%s\n",
					sMo_ibl_ret.pact_no, sMo_ibl_ret.tx_br_no,
					sMo_ibl_ret.ret_amt,sMo_ibl_ret.ret_rate, sMo_ibl_ret.tx_date,
					sMo_ibl_ret.trace_no, "¼��", sMo_ibl_ret.tel, 
					sMo_ibl_ret.chk,sMo_ibl_ret.auth);
					break;
				case '2':
			fprintf(fp,"%s|%s|%.2lf|%.2lf|%ld|%ld|%s|%s|%s|%s\n",
					sMo_ibl_ret.pact_no, sMo_ibl_ret.tx_br_no,
					sMo_ibl_ret.ret_amt,sMo_ibl_ret.ret_rate, sMo_ibl_ret.tx_date,
					sMo_ibl_ret.trace_no, "�Ѽ���", sMo_ibl_ret.tel, 
					sMo_ibl_ret.chk,sMo_ibl_ret.auth);
					break;
				default:
					sprintf(acErrMsg, "ͬҵ���뻹��Ǽǲ���״̬����!! [%s]",
						sMo_ibl_ret.sts);
					WRITEMSG
					break;
			}
		num++;	
	}

	Mo_ibl_ret_Clo_Upd( );
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
