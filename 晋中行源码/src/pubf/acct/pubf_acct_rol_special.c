/***************************************************************
* �� �� ��:     pubf_acct_rol_special.c
* ����������    �����жԽ��׵����⴦��
**��־�÷���
*
* ��    ��:		
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "tx_dc_br_rel_c.h"
#include "mo_invest_c.h"

int pub_acct_Rol_special( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;

	/*�ӽ���1111�����⴦��**/
	if( !strcmp(ptrace_log->sub_tx_code,"1111") )
	{
	}
	/*����1111�����⴦��**/
	if( !strcmp(ptrace_log->tx_code,"1111") )
	{
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_acct_Rol_spe( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;

	if( !strcmp(ptrace_log->sub_tx_code,"4603") )
	{
		;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
