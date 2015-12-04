/*************************************************************
* �� �� ��: rpt004.c
* �����������������ձ���
* ��    ��: rob
* �������: 2003��4��29��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_parm_c.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct com_sys_parm_c vcom_sys_parm;
struct com_branch_c com_branch_c;

int rpt104()
{
	struct com_parm_c com_parm;
	char	l_tmp[101];
	long	l_date;
	int		l_day;
	int		l_month;
	int		flag = 0;
	char	prog_name[256];
	char	pathname[256];
	char	filename[256];
	char	filename2[256];
	char 	vparm[21];
	int 	ii=0;
	FILE	*fp;
	int ret=0;

	memset(prog_name, 0x00, sizeof(prog_name));
	memset(pathname, 0x00, sizeof(pathname));
	memset(filename, 0x00, sizeof(filename));
	memset(vparm, 0x00, sizeof(vparm));

	ret=sql_begin();

	/**�ڶ�λ0��ʾ��ĩ��һ������**/
	/**�ڶ�λ1��ʾ�ճ�����**/
	get_zd_data("0670",vparm);
	strcpy( vparm, "01000" );

	ret=Com_parm_Dec_Upd( g_pub_tx.reply,
		"parm_code='NJJXB' and parm_seqn=1" );
	if( ret ) {TRACE goto ErrExit; }
	ret=Com_parm_Fet_Upd( &com_parm,g_pub_tx.reply );
	if( ret ) {TRACE goto ErrExit; }
	if( com_parm.val[0]!='0' )
	{
		com_parm.val[0]='0';
		ret=Com_parm_Upd_Upd( com_parm,g_pub_tx.reply );
		if( ret ) {TRACE goto ErrExit; }
		Com_parm_Clo_Upd();
		sprintf(acErrMsg,"���ʱ�Ѿ����ɹ���!");
		WRITEMSG
		goto GoodExit;
	}
	else
	{
		Com_parm_Clo_Upd();
	}

	Com_sys_parm_Sel(g_pub_tx.reply,&vcom_sys_parm,"1=1");

	l_day = vcom_sys_parm.sys_date%100;
	l_month = vcom_sys_parm.sys_date%10000;

	vtcp_log("date[%d] [%s]",l_month,vparm);
	
	strcpy(pathname, getenv("PFMDIR"));

		sprintf(filename,"%s/readrfc_nm.txt",pathname);

	vtcp_log("UUUUUUUUU[%s]",filename);

	fp = fopen( filename, "r" );
	if( fp == NULL )
	{
		vtcp_log("open file readrfc.txt error!\n");
		TRACE
		goto ErrExit;
	}
	
	while( !feof(fp) )
	{
		memset(l_tmp, 0x00, sizeof(l_tmp));

		fgets(l_tmp, 100, fp);
		cls_after_space(l_tmp);

		if(!strlen(l_tmp)) continue;

		memset( filename2, 0x00, sizeof( filename2 ) );
		get_filename(l_tmp, filename2);
		sprintf(prog_name,"%s %s %s","bbxxxx",filename2,vparm);
		vtcp_log(" prog[%s]",prog_name );

		system(prog_name);
	}
	fclose(fp);

GoodExit:
	strcpy(g_pub_tx.reply,"0000");
	TRACE
	set_zd_data("0120",g_pub_tx.reply);
	sql_commit();
	return 0;
ErrExit:
	strcpy(g_pub_tx.reply,"D103");
	TRACE
	set_zd_data("0120",g_pub_tx.reply);
	sql_rollback();
	return 1;
}


int get_filename(char  *_str, char *_sub_str)
{
	int     i, k = 0;
	int     str_len;

	/***  ȥ���ַ���ǰ��Ŀո� �ͺ���ĺ��� ***/
	str_len = strlen(_str);

	for(i = 0; i<=str_len; i++)
	{
		if( _str[i] == ' ' || _str[i] == '\t' )
			_str[i] = '\0';
		if( _str[i] == '\0' || _str[i] == '\n') break;
			_sub_str[k] = _str[i];
			k++;
	}
	_sub_str[k] = '\0';
	return 0;
}
int cls_after_space(char *_str)
{
	int     i, k = 0;
	int     str_len;

	str_len = strlen(_str);

	for(i = str_len-1; i>0; i-- )
	{
		if( _str[i] == ' ' || _str[i] == '\n') continue;
		else    break;
	}
	_str[i+1] = '\0';

	return 0;
}


