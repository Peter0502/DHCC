/*************************************************
* �� �� ��: sp6610.c
* ������������������ҵ���ļ�����
*
* ��    ��: jack
* �������: 2004��02��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include <errno.h>

sp6610()
{
	char filename1[248];
	char filename2[248];
	char filename[10];
	char execstr[256];
	char cBr_no[6];
	char cTtyno[5];
	int ret=0;
	FILE *fp=NULL;
	memset(filename,0x0,sizeof(filename));
	memset(filename1,0x0,sizeof(filename1));
	memset(filename2,0x0,sizeof(filename2));
	memset(cBr_no,0x0,sizeof(cBr_no));
	memset(cTtyno,0x0,sizeof(cTtyno));
	get_zd_data("0380",filename);
	get_zd_data("0030",cBr_no);
	get_zd_data("0100",cTtyno);
	pub_base_strpack (filename);
	sprintf(filename1,"%s/tmp/%s",getenv("HOME"),filename);
	fp = fopen(filename1,"r");
	if( fp==NULL)
	{
		sprintf(acErrMsg," open ���� file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fclose(fp);
	pub_base_strpack (cBr_no);
	pub_base_strpack (cTtyno);
	sprintf(filename2,"%s/%s%s",getenv("FILDIR"),cBr_no,cTtyno);			/*�������ļ���  */
	memset( execstr, 0x0, sizeof(execstr));
	sprintf( execstr, "cp %s %s", filename1, filename2);
	ret = system(execstr);
	if ( ret )
	{
		sprintf(acErrMsg,"�����ļ�ʧ��[%s][%s][%d][%s]", filename2, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;		
	}
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����ļ��ɹ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ļ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
