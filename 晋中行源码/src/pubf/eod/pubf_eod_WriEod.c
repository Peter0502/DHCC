/***********************************
  日终监控
************************************/
#include <stdio.h>
#include <string.h>
#include <varargs.h>
#define EXTERN
#include "public.h"
#include "com_eod_exec_c.h"

int	pub_eod_WriJk( msgstr, va_alist )
	char *msgstr;
	va_dcl
{
	va_list 	ap;
	int		i=1;
	int		j;
	int     year, mon, day, hour, min, sec;
	long	logrq;
	char	lslog[50],logfile[50],tmplog[50];
	char	msg[256];
	FILE 	*lsfp;
	FILE 	*fp;
	FILE 	*_fp;

    	char line[BUFSIZ];

	memset(logfile,0x00,sizeof(logfile));
	memset(tmplog,0x00,sizeof(tmplog));

	va_start(ap);

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);
	logrq=year*10000+mon*100+day;
    	
    	if ( getenv("LOGDIR") )
	{
		strcpy( logfile, getenv("LOGDIR") );
	}
	else
	{
		strcpy(logfile,"/tmp");
	}
	strcat( logfile, "/" );
	strcpy(tmplog,logfile);
	sprintf(lslog,"%sRZJK.%d",logfile,logrq);

	strcat(tmplog,"logjk.tmp");
	strcat(logfile,"RZJK");
	fp=fopen(logfile,"r");

	if ( fp == NULL )
	{
		fclose(fp);
		fp=fopen(logfile,"w");
		if( fp!=NULL )
		{
			fclose(fp);
		}
		fp=fopen(logfile,"r");
	}

	if( fp != NULL )
	{
		_fp=fopen(tmplog,"a");
		lsfp=fopen(lslog,"a");
		if( _fp == NULL )
		{
			perror("fopen");
			return 1;
		}
		fgets(line,BUFSIZ,fp);
		while(!feof(fp))
		{
			i++;
			fgets(line,BUFSIZ,fp);
			if (feof(fp)) break;
			if(i>17) break;
			fputs(line,_fp);
		}
		fclose(fp);
	}
	else
	{

	}

	fprintf(_fp,"  [%04d-%02d-%02d]  [%02d:%02d:%02d]  [",
		year, mon, day, hour, min ,sec,msg);
	fprintf(lsfp,"  [%04d-%02d-%02d]  [%02d:%02d:%02d]  [",
		year, mon, day, hour, min ,sec,msg);

	vfprintf(_fp, msgstr, ap);
	vfprintf(lsfp,msgstr, ap);

	fprintf(_fp, "]\n");
	fprintf(lsfp,"]\n");

	fclose(_fp);
	fclose(lsfp);

	rename(tmplog,logfile);

	return 0;
}

int pub_eod_WriBeg( int do_seq, char *prog )
{
	struct com_eod_exec_c sEodExec;
	int	ret;
	
	memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );
	
	ret = Com_eod_exec_Sel( g_pub_tx.reply, &sEodExec, "do_seq = %d \
				and prog = '%s' and using='Y'", do_seq, prog );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "日终处理程序不存在prog=[%s]", prog );
		WRITEMSG
		pub_eod_WriJk("%-30s",acErrMsg);
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "取日终处理程序错prog=[%s],[%d]", prog, ret );
		WRITEMSG
		pub_eod_WriJk("%-30s",acErrMsg);
		return(-1);	
	}
	
	pub_base_strpack( sEodExec.prog_name );
	sprintf( acErrMsg, "%s开始", sEodExec.prog_name );
	pub_eod_WriJk( "%-30s", acErrMsg );

	return 0;
}

int pub_eod_WriEnd(int do_seq, char  *prog)
{
	struct com_eod_exec_c sEodExec;
	int	ret;
	
	memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );
	
	ret = Com_eod_exec_Sel( g_pub_tx.reply, &sEodExec, "do_seq = %d \
				and prog = '%s' and using='Y'", do_seq, prog );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "日终处理程序不存在prog=[%s]", prog );
		WRITEMSG
		pub_eod_WriJk("%-40s",acErrMsg);
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "取日终处理程序错prog=[%s],[%d]", prog, ret );
		WRITEMSG
		pub_eod_WriJk("%-40s",acErrMsg);
		return(-1);	
	}
	
	pub_base_strpack( sEodExec.prog_name );
	sprintf( acErrMsg, "%s结束", sEodExec.prog_name );
	pub_eod_WriJk( "%-40s", acErrMsg );

	return 0;
}

int pub_eod_WriErr(int do_seq, char	*prog)
{
	struct com_eod_exec_c sEodExec;
	int	ret;
	
	memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );
	
	ret = Com_eod_exec_Sel( g_pub_tx.reply, &sEodExec, "do_seq = %d \
				and prog = '%s' and using='Y'", do_seq, prog );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "日终处理程序不存在prog=[%s]", prog );
		WRITEMSG
		pub_eod_WriJk("%-40s",acErrMsg);
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "取日终处理程序错prog=[%s],[%d]", prog, ret );
		WRITEMSG
		pub_eod_WriJk("%-40s",acErrMsg);
		return(-1);	
	}
	
	pub_base_strpack( sEodExec.prog_name );
	sprintf( acErrMsg, "%s错误退出", sEodExec.prog_name );
	pub_eod_WriJk( "%-40s", acErrMsg );	

	return 0;
}
