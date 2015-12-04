/*************************************************
* �� �� ��: sp8903.c
* �����������Թ�����ǩԼ״̬��ѯ
* ��    ��: likd
* ������ڣ�
* �޸ļ�¼��.....................................................
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
* 2. 
*�������ã�
insert into tx_def values('8903','�Թ����Ų�ѯҵ��','00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');

insert into tx_sub_def values('8903','�Թ����Ų�ѯҵ��','sp8903','0','0');

insert into tx_flow_def values ('8903', 0, '8903', '#$');

*************************************************/

#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"

int sp8903()
{
	int  ret=-1;
	int ttlnum=0;						/**��ȡ��������**/
	char wherelist[1024];   /**��ѯ����**/
	struct mob_acct_type_c sMob ;
	long ldate=0;
	long hdate=0;
	char ac_no[20];
	char br_no[6];
	char type[10];
	char filename[100];

	FILE *fp;

	pub_base_sysinit();

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset(&sMob,0,sizeof(&sMob));
	memset(br_no,0,sizeof(br_no));
	memset(ac_no,0,sizeof(ac_no));
	memset(type,0,sizeof(type));
	memset(filename,0,sizeof(filename));
	
	get_zd_data("0380",ac_no );
	get_zd_long("0290",&ldate );
	get_zd_long("0440",&hdate );
	get_zd_data("0030",br_no );

	
	
	if(strlen(ac_no))
	{
		sprintf(wherelist,"ac_no = '%s'",ac_no);	
	}
	else if(ldate)
	{
		if(hdate)
		{
			sprintf(wherelist,"tx_date >= %ld and tx_date <= %ld and br_no = '%s' and ac_no like '5%%%%' order by tx_date desc",ldate,hdate,br_no);
		}
		else
		{
			sprintf(wherelist,"tx_date = %ld and br_no = '%s' and ac_no like '5%%%%' order by tx_date desc",ldate,br_no);
		}
	}
	else
	{
		sprintf(wherelist,"br_no = '%s' and ac_no like '5%%%%' order by tx_date desc",br_no);
		vtcp_log("%s,%d,��֤�û���Ϣʧ��![%s]",__FILE__,__LINE__,wherelist);
	}
	ret = Mob_acct_type_Dec_Sel(g_pub_tx.reply,wherelist);
	if(ret)
	{
		sprintf( acErrMsg, "sql error" );
		WRITEMSG 
		goto ErrExit; 
	}
	ttlnum=0;
	while(1)
	{
		memset(&sMob,0,sizeof(&sMob));
		ret=Mob_acct_type_Fet_Sel( &sMob, g_pub_tx.reply );
		if( ret==100 ) break;
		else if(ret)
		{
			sprintf( acErrMsg, "sql error" );
			WRITEMSG 
			goto ErrExit; 
		}

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			/**��ʾ�б���**/
			fprintf( fp,"~@  ��  ��  ��  ��|���|@    �ֻ�����|@   ǩԼ����|ǩԼ����|@ǩԼ״̬|@����Ա|\n" );
		}
		if(sMob.mob_sts[0]=='1')
		{
			strcpy(type,"��ͨ");	
		}
		else if(sMob.mob_sts[0]=='3')
		{
			strcpy(type,"�ر�");
		}
		fprintf(fp,"%s|%d|%s|%s|%d|%s|%s|\n",sMob.ac_no,ttlnum,sMob.mobile,sMob.br_no,sMob.tx_date,type,sMob.tel);
		ttlnum++;
	}
	ret = Mob_acct_type_Clo_Sel();
	if(ret)
	{
		sprintf( acErrMsg, "sql error" );
		WRITEMSG 
		goto ErrExit; 
	}
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
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