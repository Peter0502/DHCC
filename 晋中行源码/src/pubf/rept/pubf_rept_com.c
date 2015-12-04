/*************************************************
* �� �� ��: pubf_rept_com.c
* ��������: �ձ����û�������
*			pub_rept_com_syer		ȡ�������ڵ���
*			pub_rept_com_smon		ȡ�������ڵ���
*			pub_rept_com_sday		ȡ�������ڵ���
*			pub_rept_com_ssea		ȡ�������ڵļ���
*
* �������:   2004��05��04
* �޸ļ�¼:
*	 1. ��    ��:
*		�� �� ��:
*		�޸�����:
*	 2.
*************************************************/

#define EXTERN
#define EXTERNREPT
#include <string.h>
#include "public.h"
#include "pubf_rept_fun.h"

char *pub_rept_com_syer()
{
	char	l_return[256];
	sprintf(l_return, "%04d", g_rept_date/100/100);
	return l_return;
}
char *pub_rept_com_smon()
{
	char	l_return[256];
	sprintf(l_return, "%02d", g_rept_date/100%100);
	return l_return;
}
char *pub_rept_com_sday()
{
	char	l_return[256];
	sprintf(l_return, "%02d",g_rept_date%100);
	return l_return;
}
char *pub_rept_com_ssea()
{
	char	l_return[256];
	int		l_month;

	l_month = g_rept_date/100%100;

	if( l_month>=1 && l_month <=3 )
		strcpy( l_return, "һ");
	else if( l_month>=4 && l_month <=6 )
		strcpy( l_return, "��");
	else if( l_month>=6 && l_month <=9 )
		strcpy( l_return, "��");
	else if( l_month>=10 && l_month <=12 )
		strcpy( l_return, "��");
	else
		strcpy( l_return, "  ");
	return l_return;
}
char	*pub_rept_com_sbrname()
{
	char	l_name[41];
	struct com_branch_c vcom_branch;

	memset(l_name, 0x00, sizeof(l_name));
	memset(&vcom_branch, 0x00, sizeof(struct com_branch_c));

	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&vcom_branch, \
					  "br_no='%s'",g_brno_tmp.br_no);
	if( 100 == g_reply_int )	
	{
		sprintf(acErrMsg,"no find BRNO[%s]",g_brno_tmp.br_no);
		WRITEMSG
		strcpy(l_name," ");
	}

	strcpy(l_name,vcom_branch.br_name);
	
	TRACE
	return l_name;
ErrExit:
	TRACE
	strcpy(l_name, " ");
	return l_name;
}
