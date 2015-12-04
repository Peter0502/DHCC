/*************************************************
* �� �� ��:  spB510.c
* ���������� ͬҵ���̨�˲�ѯ
*
* ��    ��:  rob
* ������ڣ� 2003��7��07��
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
#include "mo_ibl_c.h"

int spB510()
{
	char	cFileName[50];			/* ��ʱ�ļ��� */
	int	ret, num=0;
	FILE 	*fp;

	struct mo_ibl_c sMo_ibl;
	struct mdm_ac_rel_c sMdm_ac_rel;

	char wherelist[500];
	char tmpstr[50];

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( wherelist, 0x0, sizeof(wherelist));
	memset(&sMo_ibl, 0x0, sizeof(struct mo_ibl_c));
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

	get_zd_data("0730", sMo_ibl.pact_no);
	get_zd_data("0670", sMo_ibl.sign);
	get_zd_double("0390", &sMo_ibl.amt);
	get_zd_long("0440", &sMo_ibl.matr_date);
        get_zd_long("0450",&sMo_ibl.opn_date);
	get_zd_data("0680", sMo_ibl.sts);

	strcpy(wherelist, " 1=1 ");
	if (strcmp(sMo_ibl.pact_no, "") != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr," and pact_no = '%s' ", sMo_ibl.pact_no);
		strcat(wherelist, tmpstr);
	}

	if (strcmp(sMo_ibl.sign, "") != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and sign = '%s' ", sMo_ibl.sign);
		strcat(wherelist, tmpstr);
	}

	if (pub_base_CompDblVal(sMo_ibl.amt, 0.00) == 1)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and amt = %lf ", sMo_ibl.amt);
		strcat(wherelist, tmpstr);
	}

	if (sMo_ibl.matr_date > 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and matr_date = %ld ", sMo_ibl.matr_date);
		strcat(wherelist, tmpstr);
	}

        if (sMo_ibl.opn_date > 0)
        {
                memset(tmpstr, 0x0, sizeof(tmpstr));
                sprintf(tmpstr, " and opn_date = %ld ", sMo_ibl.opn_date);
                strcat(wherelist, tmpstr);
        }
 
	if (strcmp(sMo_ibl.sts, "") != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and sts = '%s' ", sMo_ibl.sts);
		strcat(wherelist, tmpstr);
	}

	sprintf(acErrMsg, "-------> wherelist = [%s]", wherelist);
	WRITEMSG

	ret = Mo_ibl_Dec_Upd( g_pub_tx.reply , wherelist);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_ibl_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"��ͬ��", "�Է��˺�", "��Ʒ���", "�������", \
			"��軧��", "�������", "�����", "�������", "��������", \
			"��Ƿ���", "��������","��������", "��������", "���״̬");
	/*
	"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"��ͬ��", "����˺�", "�Է��˺�", "��Ʒ���", "�������", \
			"��軧��", "�������", "�����", "�������", "��������", \
			"��Ƿ���", "��������","��������", "��������", "���״̬");
	*/
	while(1)
	{
		memset( &sMo_ibl, 0x0, sizeof( struct mo_ibl_c ) );
		
		ret = Mo_ibl_Fet_Upd( &sMo_ibl, g_pub_tx.reply );
		if ( ret == 100 && num == 0)
		{
			sprintf( acErrMsg, "���Ǽǲ����޼�¼!" );
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
			sprintf( acErrMsg, "��ѯmo_ibl����!! [%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		if (sMo_ibl.sts[0] != '1')
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
if ( sMo_ibl.sign[0] == '1')
		{
			switch(sMo_ibl.sts[0])
			{
				case '1':

			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ����¼��");
					break;
				case '2':
			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�������");
					break;
				case '*':
			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�����Ѿ�����");
					break;
				default:
					sprintf(acErrMsg, "ͬ�ǵǼǲ���״̬����!! [%s]",
						sMo_ibl.sts);
					WRITEMSG
					break;
			}
		}
		else
		{
			switch(sMo_ibl.sts[0])
			{
				case '1':
			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ���¼��");
					break;
				case '2':
			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�������");
					break;
				case '*':
			fprintf(fp,"%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ����Ѿ�����");
					break;
				default:
					sprintf(acErrMsg, "ͬ�ǵǼǲ���״̬����!! [%s]",
						sMo_ibl.sts);
					WRITEMSG
					break;
			}
		}
/*		if ( sMo_ibl.sign[0] == '1')
		{
			switch(sMo_ibl.sts[0])
			{
				case '1':

			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, "", sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ����¼��");
					break;
				case '2':
			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMdm_ac_rel.ac_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�������");
					break;
				case '*':
			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMdm_ac_rel.ac_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ����", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�����Ѿ�����");
					break;
				default:
					sprintf(acErrMsg, "ͬ�ǵǼǲ���״̬����!! [%s]",
						sMo_ibl.sts);
					WRITEMSG
					break;
			}
		}
		else
		{
			switch(sMo_ibl.sts[0])
			{
				case '1':
			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, "", sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ���¼��");
					break;
				case '2':
			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMdm_ac_rel.ac_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ�������");
					break;
				case '*':
			fprintf(fp,"%s|%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",
					sMo_ibl.pact_no, sMdm_ac_rel.ac_no, sMo_ibl.op_ac_no,
					sMo_ibl.prdt_no, "ͬҵ���", sMo_ibl.op_name, sMo_ibl.term,
					sMo_ibl.amt, sMo_ibl.rate, sMo_ibl.over_rate, 
					sMo_ibl.no_pay_bal,sMo_ibl.opn_date, sMo_ibl.matr_date, sMo_ibl.wrk_date,
					"ͬҵ����Ѿ�����");
					break;
				default:
					sprintf(acErrMsg, "ͬ�ǵǼǲ���״̬����!! [%s]",
						sMo_ibl.sts);
					WRITEMSG
					break;
			}
		}
*/
		num++;	
	}

	Mo_ibl_Clo_Upd( );
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
