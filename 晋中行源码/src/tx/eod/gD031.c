/*************************************************************
* �� �� ��: gD031.c
* �������������ɷ��̺�ලϵͳ����
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
gD031()
{
int	 ret=0;
char trdt[9],trdt1[5],dbname[10],str[200],str1[50],str2[200];
char pass[50];
char tmpbuf[128];
char tmpbuf1[128];

/****JYW add wait change it to DHCC*****/

goto  OkExit;

/********JYW **************************/


	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=sql_max_long("gl_mst","date",&g_pub_tx.tx_date,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"select gl_mst error ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(trdt,"%8d",g_pub_tx.tx_date);

	memset( pass,0x00,sizeof( pass ) );
	strcpy(str1,getenv("NBSSDIR"));
	sprintf(str,"cd %s",str1);    
	printf("%s\n",str);
	sprintf(str2,"qtuld1.sh %s",trdt); 
	printf("%s\n",str2);
	system(str2);                  /* �������ƺ����� */	
	system(str);                   /* �������ݵ���Ŀ¼ $HOME/nbsstxt */	

    /* ��8λ���ڵĺ���λ(��)��Ϊ������Ŀ¼ */
    memset(tmpbuf,0x00,sizeof(tmpbuf));
    if ( trdt[7] == NULL || trdt[7] == ' ' ) {
         memcpy(tmpbuf,trdt,8);
    }
    else {
         memcpy(&tmpbuf,&trdt[6],2);
    }
    printf("tmpbuf=[%s]\n",tmpbuf);

	memset(tmpbuf1,0x00,sizeof(tmpbuf1));
	sprintf(tmpbuf1,"%s/%s",str1,tmpbuf);
	if( access(tmpbuf1,0)!=0 ) {
		sprintf(acErrMsg,"���мලϵͳ��������Ŀ¼������![%s]",tmpbuf1);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(str,"mv -f %s/*.txt %s/%s",str1,str1,tmpbuf);
	printf("%s\n",str);
	system(str);	

	sprintf(pass,"%s/%s",str1,tmpbuf);
	chdir(pass);

	sprintf(str,"wc -m *.txt > wc.txt");
	printf("%s\n",str);
	system(str);	

	sprintf(str,"chmod -R 777 %s/%s",str1,tmpbuf);
	printf("%s\n",str);
	system(str);	

OkExit:
   sql_commit();   /*--�ύ����--*/
   strcpy(g_pub_tx.reply,"0000");
   sprintf(acErrMsg,"�������мලϵͳ���̺����ݳɹ�!!!!![%s]",g_pub_tx.reply);
   WRITEMSG
   set_zd_data("0120",g_pub_tx.reply);
   return 0;
ErrExit:
   if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
   {
       strcpy(g_pub_tx.reply,"G009");/* ���޸� */
   }
   sql_rollback(); /*--����ع�--*/
   sprintf(acErrMsg,"�������мලϵͳ���̺�����ʧ��!!!!![%s]",g_pub_tx.reply);
   WRITEMSG
   set_zd_data("0120",g_pub_tx.reply);
   return 1;
}
