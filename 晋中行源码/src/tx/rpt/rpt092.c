/*************************************************************
* �� �� ��: rpt092.c
* ����������
*		Ϊ�����ṩ���ݣ���ͣ��ϴǮ��ȡ���ݣ���ϴǮĿǰͨ��ODS�ṩ����
* ��    ��: 
* �������: 
* �޸ļ�¼��
* ��    ��: 2007��10��29��
* �� �� ��: chenhw
* �޸�����:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "com_sys_parm_c.h"

extern char *get_env_info(char *infoname);

int rpt092()
{
	int ret=0;
	char	comm[1024];
	char filename[64],path[64],dbuser[16],dbpasswd[16];
	struct com_sys_parm_c	s_com_sys_parm;
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));

	/* ȡǰһ�콻������ */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "1=1" );
	ERR_DEAL
	
	memset(comm,0,sizeof(comm));
  strcpy(path,getenv("HOME"));

  strcpy(dbuser,get_env_info("DB_USER"));
  strcpy(dbpasswd,get_env_info("DB_PASSWD"));
/*  
  sprintf(comm,"%s/%s/",path,"fxmoneydata");
  ret = chdir(comm);
  sprintf(acErrMsg,"comm[%s]",comm);
  WRITEMSG
  if ( ret )
  {
      sprintf(acErrMsg,"����Ŀ¼ʧ��[%d]",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"O141");
      goto ErrExit;
  }
	**/
	
 /*Ϊ����ϵͳ��ȡ���� begin 20120628 */
	sprintf(comm,"ftp_to_cis.sh %s %s %s %s %d",get_env_info("CIS_USER"),get_env_info("CIS_PWD"),get_env_info("DB_USER"),get_env_info("DB_PASSWD"),s_com_sys_parm.lst_date);
	sprintf(acErrMsg,"comm[%s]",comm);
	ret=system(comm);
	if ( ret )
	{
	    sprintf(acErrMsg,"Ϊ����ϵͳ��ȡ���ݴ���[%d],[%s]",ret,comm);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"AT13");
	    goto ErrExit;
	}
	sprintf(acErrMsg,"Ϊ����ϵͳ��ȡ�������[%s]",comm);
	WRITEMSG
   /*Ϊ����ϵͳ��ȡ���� end  20120628 */

GoodExit:
	strcpy(g_pub_tx.reply,"0000");
	return 0;
ErrExit:
	return 1;
}
