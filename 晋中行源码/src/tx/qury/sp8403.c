/*************************************************
* �� �� ��:  sp8403.c
* ���������� ������ʾ��Ϣ��ѯ
*
* ��    ��:  lance
* ������ڣ� 20030617
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"

int sp8403()
{
	char brno[6];
	long inf_date;
	
	char mailname[51];
	char filename[100];
	char tmpstr[512];

	int i=0;
	int ret=0;
	FILE *fp;

	pub_base_sysinit();

	get_zd_data("0910",brno );
	sprintf(acErrMsg,"��������[%s]",brno);
	WRITEMSG	
	get_zd_long("0290",&inf_date );

	/* ����ļ��� */
	if(!inf_date) inf_date=g_pub_tx.tx_date;
	if( !strlen(brno) ) strcpy( brno,g_pub_tx.tx_br_no );
		
	sprintf(mailname, "%s/%ld/%s%ld" ,getenv("MAILDIR"), inf_date , brno , inf_date ); 
	sprintf(acErrMsg,"�ļ�����[%s]",mailname);
	WRITEMSG
					      	
	fp = fopen( mailname,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",mailname );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S055" );
		goto ErrExit;
	}else{
		fclose(fp);
		pub_base_AllDwnFilName( filename );
		vtcp_log(filename);		
		
		sprintf(tmpstr,"cp %s %s 1>/dev/null 2>/dev/null",mailname, filename);
		system(tmpstr);
		vtcp_log(tmpstr);

		set_zd_data( DC_FILE_SND,"1" );
	}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
