/*************************************************************
* �� �� ��: gD024.c
* �������������ɷ���ǰ�ලϵͳ����
*
* ��    ��: jack
* �������: 2003��3��29��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include "public.h"
#include "com_sys_parm_c.h"
gD024()
{
	int	ret=0;
	char trdt[9],trdt1[5],dbname[10],str[200],str1[50];
    char tmpbuf[128];
    char tmpbuf1[128];
	struct com_sys_parm_c	com_sys_parm_c;

	memset(&com_sys_parm_c,0x00,sizeof(struct com_sys_parm_c));
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"select com_sys_parm error ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(trdt,"%d",com_sys_parm_c.sys_date);

	strcpy(str1,getenv("NBSSDIR"));
	sprintf(str,"cd %s",str1);
	printf("%s\n",str);
	system("qtuld.sh");

	system(str);	
        
    memset(tmpbuf,0x00,sizeof(tmpbuf));
    if ( trdt[7] == NULL || trdt[7] == ' ' ) {
         memcpy(tmpbuf,trdt,8);
    }
    else {
         memcpy(&tmpbuf,&trdt[6],2);
    }
    memset(tmpbuf1,0x00,sizeof(tmpbuf1));
    sprintf(tmpbuf1,"%s/%s",str1,tmpbuf);
        
    if (access(tmpbuf1,0) == 0) {
		sprintf(str,"rm -f %s/*.txt",tmpbuf1);
    }
    else {
	    sprintf(str,"mkdir %s",tmpbuf1);
    }

	vtcp_log("xxx --- look [%s]",tmpbuf1);
	system(str);	

	sprintf(str,"mv %s/*.txt %s",str1,tmpbuf1);
	printf("%s\n",str);
	system(str);	

OkExit:
  sql_commit();   /*--�ύ����--*/
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"�������мලϵͳ����ǰ���ݳɹ�!!!!![%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 0;
ErrExit:
  if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
  {
      strcpy(g_pub_tx.reply,"G009");/* ���޸� */
  }
  sql_rollback(); /*--����ع�--*/
  sprintf(acErrMsg,"�������мලϵͳ����ǰ����ʧ��!!!!![%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 1;
}
