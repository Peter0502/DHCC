/*************************************************
* �� �� ��: spF680.c
* ����������������������ӡ
*
* ��    ��: jack
* �������: 2004��05��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <sys/types.h>
#include <errno.h>

spF680()  
{ 	
	
	int ret=0;

	char cUnit_no[5];
	char cFlag[2];
	char cDate[9];
	long lTraceno=0;/**�ļ�����������ˮ ADD BY ZHGF**/

	char execstr[256];

	char filename[100];
	char filename1[100];
	int i=0;

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cDate, 0x0, sizeof(cDate) );
	
	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0700", cFlag );
	get_zd_data( "0460", cDate );
	get_zd_long( "0290", &lTraceno);/**�ļ�����������ˮ**/


	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	memset(filename,0x0,sizeof(filename));
	pub_base_AllDwnFilName( filename );

	if ( cFlag[0] != '2' )
	{
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/%s%s%s%s%06d.txt", getenv("FILDIR"), cFlag, cUnit_no, cDate,g_pub_tx.tel,lTraceno);/**�ļ�����������ˮ**/
		vtcp_log("%s,%d filename1==[%s]",__FILE__,__LINE__,filename1);
	}
	else
	{
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/DWHZ%s%s%s.txt", getenv("FILDIR"), cUnit_no, cDate,g_pub_tx.tel);
		vtcp_log("%s,%d filename1==[%s]",__FILE__,__LINE__,filename1);
	}

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]���ɵ��ļ���Ϊ[%s]", filename, filename1 );
	WRITEMSG

	if ( access( filename1, 0) )
	{
		sprintf(acErrMsg,"Ҫ���ʵ��ļ�������[%s]", filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	memset( execstr, 0x0, sizeof(execstr) );
	sprintf( execstr, "cp %s %s", filename1, filename );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
		
	}

	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������´��ļ��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���������´��ļ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
