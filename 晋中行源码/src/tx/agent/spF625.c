/*************************************************
* �� �� ��: spF625.c
* ������������˰������������
*
* ��    ��: jack
* �������: 2004��06��25��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"

spF625()
{ 	
	int ret=0;
	char execstr[256];

	char cUnit_no[5];
	char cSign[2];

	char filename[100];
	char filename1[100];
	char fldstr[12][100];
	char tmpstr[1024];
	FILE *fp;
	FILE *fp1;
	int i=0;

	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	cSign[0] = '\t';
	cSign[1] = '\0';

	get_zd_data( "0920", cUnit_no );

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��Ŷ�Ӧ��¼!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	
	memset( filename1, 0x0, sizeof(filename1) );
	sprintf( filename1, "%s/TAX_IN_ERR%s.txt", getenv("FILDIR"), cUnit_no );
	fp1 = fopen( filename1, "w" );
	if ( fp1 == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0; i<9; i++ )
		{
			ret=pub_base_cut_str( tmpstr, fldstr[i], cSign, i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		
		/*-------------һ��Ҫ�����˶�Ӧ��ϵ,���ܽ�˰---------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 100 )
		{
			/*fprintf( fp1, "`%s	@��Ӧ����Ϣ������\n", tmpstr );*/
			fprintf( fp1, "`%s\n", tmpstr );
			fprintf( fp1, "`��˰��Ŷ�Ӧ����Ϣ������\n", tmpstr );
			continue;
		}
		if ( ret )
		{
			sprintf( acErrMsg,"��ѯag_peritemmap����[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sAg_peritemmap.ac_no );
		pub_base_strpack( sAg_peritemmap.name );
		if ( strcmp( fldstr[6], sAg_peritemmap.ac_no ) )
		{
			/*-----------------------------------------------------*
			fprintf( fp1, "`%s	@�˺Ų���,�����˻�Ϊ[%s]-[%s] \n", \
				tmpstr, sAg_peritemmap.ac_no, sAg_peritemmap.name );
			*-----------------------------------------------------*/
			fprintf( fp1, "`%s\n", tmpstr );
			fprintf( fp1, "`�˺Ų���,�����˻�Ϊ[%s]-[%s] \n", \
				sAg_peritemmap.ac_no, sAg_peritemmap.name );
		}

		memset( &sPay_agent, 0x00, sizeof(sPay_agent) );
		ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			if ( sPay_agent.result[0] == '0' )
			{
				continue;	/*	�ü�¼�ѽɷ�,���ô���	*/
			}

			/*------	�����ı�Ϊ׼	------*/
			ret = sql_execute( "delete from pay_agent where \
				unit_no = '%s' and item_no = '%s'", \
				cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf( acErrMsg,"ɾ��pay_agent��¼����[%s]",fldstr[0] );
				WRITEMSG
				goto ErrExit;
			}
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"��ѯpay_agent����[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPay_agent, 0x00, sizeof(sPay_agent) );
		strcpy( sPay_agent.unit_no, cUnit_no );
		strcpy( sPay_agent.item_no, fldstr[0] );
		strcpy( sPay_agent.name, sAg_peritemmap.name );
		strcpy( sPay_agent.id_type, "1" );
		strcpy( sPay_agent.id_no, sAg_peritemmap.id_no );
		strcpy( sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = 0;
		sPay_agent.end_date = atol( fldstr[4] );
		sPay_agent.tx_date = 99999999;
		sPay_agent.tram = atof( fldstr[3] );
		strcpy( sPay_agent.remark, fldstr[1] );
		strcpy( sPay_agent.sata, "1" );
		strcpy( sPay_agent.result, "1" );

		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply );
		if ( ret )
		{
    		sprintf(acErrMsg,"------>�����¼[%s]��pay_agent����[%d]", fldstr[0], ret ); 
    		WRITEMSG
			goto ErrExit;
		}
	}
	fclose(fp);
	fclose(fp1);

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=1;
	pub_base_AllDwnFilName( filename );

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
    sprintf(acErrMsg,"------>������˰�����ݳɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose(fp);
    fclose(fp1);
    sprintf(acErrMsg,"------>������˰������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
