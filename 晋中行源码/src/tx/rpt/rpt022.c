/***************************************************************
* �� �� ��: rpt022.c
* ��������: ATM������ˮ�嵥
*
* ��    ��: rob
* �������: 2003��6��24��
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
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "chnl_self_trade_c.h"

int page = 1;
double	q_amt=0.00, q_amt_w=0.00, z_amt=0.00, c_amt=0.00;
int 	q_cnt=0, q_cnt_w=0, z_cnt=0, c_cnt=0;
struct chnl_self_trade_c sChnl_self_trade;

int get_ratio_atmtrqd(int bli,int bll,char str[100]);
rpt022()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));
	memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));

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

	/* ��ӡatm������ˮ�嵥 */
	ret = atm_trade_list();
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ӡATM������ˮ�嵥!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡATM������ˮ�嵥����ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"��ӡATM������ˮ�嵥����ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int atm_trade_list()
{
	int ret = 0; 
	int opncnt=0; 
	FILE    *fp; 
	int     line=0; 
	int  	total_line=54;
	char fname[100]; 
	char filename[100];
	char vpfm[21]; 
	char old_br_no[6];
	char new_br_no[6];
	int first=0;
	
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));

	strcpy(vpfm, "atmtrqd");
	pub_rpt_rmfile( "" , "BRatmtrqd" , 0 );
TRACE
sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG

	ret = Chnl_self_trade_Dec_Sel(g_pub_tx.reply, "tx_date=%ld and ttyp='1' \
		and tx_code in ('6203', '6205', '6206') and stat='0' \
		order by tx_br_no, tel, tx_date, trace_no", g_pub_tx.tx_date);
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	first=0;
	while(1) 
	{ 
		ret = Chnl_self_trade_Fet_Sel(&sChnl_self_trade, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			if (strcmp(old_br_no, "") != 0)
			{
				pub_rpt_openfile( &fp, old_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_atmtrqd) ) 
					goto ErrExit;

				ret = atm_total(old_br_no);
				if (ret != 0)
				{
					sprintf(acErrMsg, "���ܽ�����!!");
					WRITEMSG
					goto ErrExit;
				}
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","DD",&opncnt, 
						get_ratio_atmtrqd) ) 
					goto ErrExit;
				fclose(fp);
			}
			break; 
		}

		memset(new_br_no, 0x0, sizeof(new_br_no));
		strcpy(new_br_no, sChnl_self_trade.tx_br_no);
		if( first==0 )
		{
			first=1;
			strcpy( old_br_no, new_br_no);
			page=1;
			sprintf( filename,"%s","BRatmtrqd");
			pub_rpt_openfile( &fp, sChnl_self_trade.tx_br_no, filename); 

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_atmtrqd) ) 
				goto ErrExit; 
		
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_atmtrqd) ) 
				goto ErrExit; 
			strcpy(old_br_no, new_br_no);
			fclose(fp);
		}
		
		sprintf( filename,"%s","BRatmtrqd");
	
		if (strcmp(old_br_no, new_br_no) != 0 || line > total_line)
		{
			pub_rpt_openfile( &fp, old_br_no, filename); 
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_atmtrqd) ) 
				goto ErrExit;

			if (strcmp(old_br_no, new_br_no) != 0)
			{
				/* ���� */
				ret = atm_total(old_br_no);
				if (ret != 0)
				{
					sprintf(acErrMsg, "���ܽ�����!!");
					WRITEMSG
					goto ErrExit;
				}
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","DD",&opncnt, 
						get_ratio_atmtrqd) ) 
					goto ErrExit;
			}
			else
			{
				fprintf(fp,"@HY@\n");
			}
			page = page + 1;
			line = 0;
			fclose(fp);

			if (strcmp(old_br_no, new_br_no) != 0)
			{
				page = 1;
			}
			pub_rpt_openfile( &fp, sChnl_self_trade.tx_br_no, filename); 

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_atmtrqd) ) 
				goto ErrExit; 
		
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_atmtrqd) ) 
				goto ErrExit; 
			strcpy(old_br_no, new_br_no);
			fclose(fp);
		}
		pub_rpt_openfile( &fp, sChnl_self_trade.tx_br_no, filename); 
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_atmtrqd) ) 
			goto ErrExit; 
		fclose(fp);
	} 
	
	Chnl_self_trade_Clo_Sel();
	return 0;
ErrExit:
	return 1;
}

/* ���� */
int atm_total(char br_no[6])
{
	int ret=0;

	q_amt = 0.00;
	q_cnt = 0;
	q_amt_w = 0.00;
	q_cnt_w = 0.00;
	z_amt = 0.00;
	z_cnt = 0;
	c_amt = 0.00;
	c_cnt = 0;

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &q_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6203' and stat='0' and \
		tx_br_no='%s' and info_source='0'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ�������!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6203' and stat='0' and tx_br_no='%s' and info_source='0'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ���������!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	q_cnt = ret;

	sprintf(acErrMsg, "ATMȡ����Ϊ:[%lf], ȡ�����:[%ld]", q_amt, q_cnt);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &q_amt_w, 
		"tx_date=%ld and ttyp='1' and tx_code = '6203' and stat='0' and \
		tx_br_no='%s' and info_source='2'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ�������!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6203' and stat='0' and tx_br_no='%s' and info_source='2'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ���������!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	q_cnt_w = ret;

	sprintf(acErrMsg, "ATMȡ��(����)���Ϊ:[%lf], ����ȡ�����:[%ld]", 
		q_amt_w, q_cnt_w);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &z_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6205' and stat='0' and \
		tx_br_no='%s'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ�������!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6205' and stat='0' and tx_br_no='%s'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ���������!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	z_cnt = ret;

	sprintf(acErrMsg, "ATMת�ʽ��Ϊ:[%lf], ת�ʱ���:[%ld]", z_amt, z_cnt);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &c_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6206' and stat='0' and \
		tx_br_no='%s'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ�������!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6206' and stat='0' and tx_br_no='%s'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ATMȡ���������!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	c_cnt = ret;

	sprintf(acErrMsg, "ATM�����Ϊ:[%lf], ������:[%ld]", c_amt, c_cnt);
	WRITEMSG

	return 0;
}

int get_ratio_atmtrqd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 

	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt,"%%%ds",bll); 
			/******
			if (sChnl_self_trade.info_source[0] == '2')
			{
				sprintf( vstr,fmt, sChnl_self_trade.ac_no1);
			}
			else
			{
				sprintf( vstr,fmt, sChnl_self_trade.ac_no);
			}
			*******/
			sprintf( vstr,fmt, sChnl_self_trade.ac_no);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.tx_code);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.tel);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, sChnl_self_trade.tx_amt); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sChnl_self_trade.tx_br_no); 
			break; 
		case 'F':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, sChnl_self_trade.trace_no); 
			break; 
		case 'G':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt); 
			break; 
		case 'H':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt); 
			break; 
		case 'I':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, z_amt); 
			break; 
		case 'J':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, z_cnt); 
			break; 
		case 'K':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, c_amt); 
			break; 
		case 'L':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, c_cnt); 
			break; 
		case 'M':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt_w); 
			break; 
		case 'N':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt_w); 
			break; 
		case 'Q': 
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", 
					sChnl_self_trade.tx_br_no); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret); 
				WRITEMSG 
				goto ErrExit; 
			} 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf(str, fmt, sCom_branch.br_name); 
			break;
		case 'T':
			sprintf(fmt, "%%%ds", bll); 
			sprintf(str, fmt, "ATM"); 
			break;
		case '1': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		case '2': 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf( str, fmt, sChnl_self_trade.tx_br_no ); 
			break; 
		case '3': 
			sprintf( str, "%02ld", page); 
			break; 
		case '7': 
			sprintf( str, "%04s", "6203"); 
			break; 
		case '8': 
			sprintf( str, "%04s", "6205"); 
			break; 
		case '9': 
			sprintf( str, "%04s", "6206"); 
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
