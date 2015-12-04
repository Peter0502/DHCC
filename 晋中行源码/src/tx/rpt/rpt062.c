/***************************************************************
* �� �� ��: rpt062.c
* ��������: ��ҵ�����м�ҵ��ͳ�Ʊ�
*
* ��    ��: rob
* �������: 2003��12��19��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "trace_log_bk_c.h"
#include "note_parm_c.h"
#include "tx_def_c.h"

int page = 1;
struct trace_log_bk_c sTrace_log_bk;
double dwjsje=0.00, dwjssr=0.00, grjsje=0.00, grjssr=0.00;
double jjkxfje=0.00, jjkzzje=0.00, jjkcxje=0.00, jjkqxje=0.00;
double dfgzje=0.00, dsdxfje=0.00, qtdlsfje=0.00, cdhpje=0.00;
long dfbs=0, dkbs=0;

int get_ratio_middle(int bli,int bll,char str[100]);
rpt062()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯϵͳ���������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;

	/* ��ӡ��ҵ�����м�ҵ��ͳ�Ʊ� */
	ret = print_middle();
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ӡ����ҵ�񱨱����!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡ����ҵ�񱨱���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"��ӡ����ҵ�񱨱���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int print_middle()
{
	int ret = 0; 
	int opncnt=0; 
	FILE    *fp; 
	int     line=0; 
	char filename[100];
	char vpfm[21]; 
	
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));

	sprintf( filename,"%s","BMmiddle");
	pub_rpt_rmfile( "" , filename , 0 );
	strcpy(vpfm, "middle");

sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG
	
	ret = get_middle_data();
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�������м�ҵ�����ݴ���");
		WRITEMSG
		goto ErrExit;
	}

	pub_rpt_openfile( &fp, "61000", filename);

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt,
			get_ratio_middle) )
		goto ErrExit;
	fclose(fp);

	return 0;
ErrExit:
	return 1;
}

int get_middle_data()
{
	int ret=0;
	long day=0, mon=0, year=0;
	double z_amt=0.00, j_amt=0.00;
	long bz=0;
	long mc=0, mm=0;
	char tmpstr[20];

	memset(tmpstr, 0x0, sizeof(tmpstr));

	pub_base_get_ymd(&year, &mon, &day, g_pub_tx.tx_date, &bz);

	sprintf(tmpstr, "%04ld%02ld", year, mon);
	mc = atol(tmpstr) * 100;
	mm = mc + 32;
	yc = year * 10000 + 0100;
	ym = year * 10000 + 1232;

	/* ͳ�Ƶ�λ������ */
	ret = sql_sum_double("trace_log_rz", "amt", &z_amt, "ac_no[1]='5' and \
		add_ind='1' and sts='0' and tx_date>%ld and tx_date<%ld", mc,mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ������������ [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("trace_log_rz", "amt", &j_amt, "ac_no[1]='5' and \
		add_ind='0' and sts='0' and tx_date>%ld and tx_date<%ld", mc,mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dwjsje = ( z_amt - j_amt ) / 10000;

	/* ͳ�����ۼ� */
	ret = sql_sum_double("trace_log_rz", "amt", &z_amt, "ac_no[1]='5' and \
		add_ind='1' and sts='0' and tx_date>%ld and tx_date<%ld", yc,ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ������������ [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("trace_log_rz", "amt", &j_amt, "ac_no[1]='5' and \
		add_ind='0' and sts='0' and tx_date>%ld and tx_date<%ld", yc,ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dwjsje_y = ( z_amt - j_amt ) / 10000;

	/* ͳ�Ƶ�λ�������� */
	ret = sql_sum_double("com_chrg_hst", "chrg_amt", &j_amt, "ac_no[1]='5' \
		and sts='Y' and tx_date>%ld and tx_date<%ld ", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dwjssr = j_amt / 10000;

	/* �굥λ���� */
	ret = sql_sum_double("com_chrg_hst", "chrg_amt", &j_amt, "ac_no[1]='5' \
		and sts='Y' and tx_date>%ld and tx_date<%ld ", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dwjssr_y = j_amt / 10000;

	/* ͳ�Ƹ��˽����� */
	ret = sql_sum_double("trace_log_rz", "amt", &z_amt, "ac_no[1]='1' and \
		add_ind='1' and sts='0' and tx_date>%ld and tx_date<%ld", mc,mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƹ��˽����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("trace_log_rz", "amt", &j_amt, "ac_no[1]='1' and \
		add_ind='0' and sts='0' and tx_date>%ld and tx_date<%ld", mc,mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƹ��˽���������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	grjsje = ( z_amt - j_amt ) / 10000;

	/* ����˽��� */
	ret = sql_sum_double("trace_log_rz", "amt", &z_amt, "ac_no[1]='1' and \
		add_ind='1' and sts='0' and tx_date>%ld and tx_date<%ld", yc,ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƹ��˽����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("trace_log_rz", "amt", &j_amt, "ac_no[1]='1' and \
		add_ind='0' and sts='0' and tx_date>%ld and tx_date<%ld", yc,ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƹ��˽���������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	grjsje_y = ( z_amt - j_amt ) / 10000;

	/* ͳ�Ƹ��˽������� */
	ret = sql_sum_double("com_chrg_hst", "chrg_amt", &j_amt,
		"(ac_no[1]='1' or ac_no[1,4]='9400' or ac_no[1,4]='4302') and \
		sts='Y' and tx_date>%ld and tx_date<%ld ", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	grjssr = j_amt / 10000;

	/* ��������� */
	ret = sql_sum_double("com_chrg_hst", "chrg_amt", &j_amt,
		"(ac_no[1]='1' or ac_no[1,4]='9400' or ac_no[1,4]='4302') and \
		sts='Y' and tx_date>%ld and tx_date<%ld ", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�Ƶ�λ����������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	grjssr_y = j_amt / 10000;

	/* ͳ�ƽ�ǿ����� 
	ret = sql_count("mdm_ac_rel", "mdm_code='0020' and beg_date>%ld and \
		beg_date<%ld", mc, mm)
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƽ�ǿ������쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	jjkzs = ret;*/

	/* ͳ�ƿ�pos���ѽ�� */
	ret = sql_sum_double("chnl_self_trade", "tx_amt", &jjkxfje, 
		"tx_date>%ld and tx_date<%ld \
		and stat='0' and tx_code='6301'", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ����ѽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkxfje = jjkxfje / 10000;

	/* �����ѽ�� */
	ret = sql_sum_double("chnl_self_trade", "tx_amt", &jjkxfje_y, 
		"tx_date>%ld and tx_date<%ld \
		and stat='0' and tx_code='6301'", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ����ѽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkxfje_y = jjkxfje_y / 10000;

	/* ͳ�ƿ�ת�ʽ�� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkzzje, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='2' and tx_date>%ld \
		and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ�ת�ʽ����� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkzzje = jjkzzje / 10000;

	/* ��ת�� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkzzje_y, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='2' and tx_date>%ld \
		and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ�ת�ʽ����� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkzzje_y = jjkzzje_y / 10000;

	/* ͳ�ƿ����ֽ�� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkcxje, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='1' and add_ind='1' \
		and tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ����ֽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkcxje = jjkcxje / 10000;

	/* ����� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkcxje_y, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='1' and add_ind='1' \
		and tx_date>%ld and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ����ֽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkcxje_y = jjkcxje_y / 10000;

	/* ͳ�ƿ�ȡ�ֽ�� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkqxje, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='1' and add_ind='0' \
		and tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ�ȡ�ֽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkqxje = jjkqxje / 10000;

	/* ��ȡ�� */
	ret = sql_sum_double("trace_log_rz", "amt", &jjkqxje_y, "sts='0' and \
		ac_no[1,4] in ('9400', '4302') and ct_ind='1' and add_ind='0' \
		and tx_date>%ld and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƿ�ȡ�ֽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	jjkqxje_y = jjkqxje_y / 10000;

	/* ͳ�ƴ������ʽ�� */
	ret = sql_sum_double("pay_agent_bk", "tram", &dfgzje, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='1') \
		and tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƴ������ʽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dfgzje = dfgzje / 10000;

	/* ��������� */
	ret = sql_sum_double("pay_agent_bk", "tram", &dfgzje_y, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='1') \
		and tx_date>%ld and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƴ������ʽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dfgzje_y = dfgzje_y / 10000;

	/* ͳ�ƴ��յ�Ѷ�ѽ�� */
	ret = sql_sum_double("pay_agent_bk", "tram", &dsdxfje, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='0' and \
		pay_type in ('47','48')) and tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƴ��յ�Ѷ�ѽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dsdxfje = dsdxfje / 10000;

	/* ����յ�Ѷ�� */
	ret = sql_sum_double("pay_agent_bk", "tram", &dsdxfje_y, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='0' and \
		pay_type in ('47','48')) and tx_date>%ld and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƴ��յ�Ѷ�ѽ���쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dsdxfje_y = dsdxfje_y / 10000;

	/* ͳ�����������ո���� */
	ret = sql_sum_double("pay_agent_bk", "tram", &qtdlsfje, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='0' and \
		pay_type in ('40','41','42','43','44','45','46')) and \
		tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�����������ո�����쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	qtdlsfje = qtdlsfje / 10000;

	/* ���������� */
	ret = sql_sum_double("pay_agent_bk", "tram", &qtdlsfje_y, "result='0' and \
		unit_no in (select unit_no from ag_unitinfo where opr_attri='0' and \
		pay_type in ('40','41','42','43','44','45','46')) and \
		tx_date>%ld and tx_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�����������ո�����쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	qtdlsfje_y = qtdlsfje_y / 10000;

	/* ͳ�ƴ������� */
	ret = sql_count("pay_agent_bk", "result='0' and unit_no in \
		(select unit_no from ag_unitinfo where opr_attri='1') and \
		tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret < 0 )
	{
		sprintf(acErrMsg, "ͳ�ƴ��������쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dfbs = ret;

	/* ͳ�ƴ��۱��� */
	ret = sql_count("pay_agent_bk", "result='0' and unit_no in \
		(select unit_no from ag_unitinfo where opr_attri='0') and \
		tx_date>%ld and tx_date<%ld", mc, mm);
	if ( ret < 0 )
	{
		sprintf(acErrMsg, "ͳ�ƴ��������쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	dkbs = ret;

	/* ͳ�����гжһ�Ʊ��� */
	ret = sql_sum_double("mo_bank_po", "po_amt", &cdhpje, "po_ind='1' \
		and sign_date>%ld and sign_date<%ld", mc, mm);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�����гжһ�Ʊ����쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	cdhpje = cdhpje / 10000;

	/* ��жһ�Ʊ */
	ret = sql_sum_double("mo_bank_po", "po_amt", &cdhpje_y, "po_ind='1' \
		and sign_date>%ld and sign_date<%ld", yc, ym);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�����гжһ�Ʊ����쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	cdhpje_y = cdhpje_y / 10000;

	return 0;
ErrExit:
	return 1;
}

int get_ratio_middle( bli,bll,str )
int bli,bll;
char str[100];
{
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 
	struct tx_def_c sTx_def;
	struct note_parm_c sNote_parm;

	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, dwjsje);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, dwjsje_y);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, dwjssr);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, dwjssr_y); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, grjsje); 
			break; 
		case 'F':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, grjsje_y); 
			break; 
		case 'G':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, grjsje); 
			break; 
		case 'H': 
			sprintf(fmt, "%%%d.2lf", bll); 
			sprintf(str, fmt, grjsje_y); 
			break;
		case 'I':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkxfje); 
			break; 
		case 'J':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkxfje_y); 
			break; 
		case 'K':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkzzje); 
			break; 
		case 'L':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkzzje_y); 
			break; 
		case 'M':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkcxje); 
			break; 
		case 'N':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkcxje_y); 
			break; 
		case 'O':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkqxje); 
			break; 
		case 'P':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, jjkqxje_y); 
			break; 
		case 'Q':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, dfgzje); 
			break; 
		case 'R':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, dfgzje_y); 
			break; 
		case 'S':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, dsdxfje); 
			break; 
		case 'T':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, dsdxfje_y); 
			break; 
		case 'U':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, qtdlsfje); 
			break; 
		case 'V':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, qtdlsfje_y); 
			break; 
		case 'W':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, dfbs); 
			break; 
		case 'X':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, dkbs); 
			break; 
		case 'Y':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, cdhpje); 
			break; 
		case 'Z':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, cdhpje_y); 
			break; 
		case 'a': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
