/*************************************************************
* �� �� ��: spF888.c
* ��������������ҵ�����(����ATM,POS,����ͨ��ҵ��)
* ��    ��: jack
* ������ڣ�2003��8��18��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

#define MAX 10000
#include "four_tradelog_c.h"
#include "chnl_self_trade_c.h"
#include "six_dz_log_c.h"

long lDate;
char cFlag[2];
char cMessage[81];

char   shmx[MAX][81], ylmx[MAX][81];
char   hdbz[MAX] ;
char   sh_corr[MAX][81];

spF888()
{
	int ret = 0;
	char buf[128];
	char filename[128];
	
	ret = dz_process();
	
	set_zd_data( "0760", cMessage );
	set_zd_data( "0700", cFlag );
	set_zd_long( "0440", lDate );

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "%ld.txt", lDate );

	memset( buf, 0x00, sizeof(buf) );
	sprintf( buf, "%s/bin/ftp_to_tfiof.sh %s %s", getenv("HOME"), filename );
	system( buf );

	if ( ret )
	{
		goto ErrExit;
	}


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ҵ����ʽ��׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ҵ����ʽ��׳���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int dz_process()
{
	FILE    *fr, *fw;
	int     flag=0,i=0,j=0,n=0, len=0 ; /* 0:t_lc_trans_hist   1:t_lc_transflow */
	int     num_bd=0, num_yl=0, num_sh=0, num_corr=0, num_bs=0; /* ���ر������������� */
	char    lsbuf[51], ret_code[3];
	
	char   buf[1024],  filename[61], jdbz[2];
	char   trn_date[9], date[9], srsn[5], otsn[5], seq_dl[11];
	char   seq_kh[11], seq_zx[11], czbz[3], oper[9];
	char   trcd[5], amcr[20], idno[21],wsno[11], time[9], trn_flag[2];
	double tram , zje_sxf ,zje_sh, zje_yd, zje_yl;
	double  sxfje, tzje;
	int     dl_kh ; /* 0-������ 1-������ 2-������ */
	int     bdt_bs_sh, tdb_bs_sh ;
	double  bdt_jffs_sh, bdt_dffs_sh, tdb_jffs_sh, tdb_dffs_sh ;
	int     bdt_bs_zx, tdb_bs_zx ;
	double  bdt_jffs_zx, bdt_dffs_zx, tdb_jffs_zx, tdb_dffs_zx ;
	int     bdt_bs_hj, tdb_bs_hj ;
	double  bdt_jffs_hj, bdt_dffs_hj, tdb_jffs_hj, tdb_dffs_hj , qsje_hj;
	
	char fldstr[35][128];	/*���ѭ��������ʼ�¼*/
	int  ret = 0;
	char tmpstr[128];

	char filename1[128];
	char filename2[128];
	
	struct four_tradelog_c sFour_tradelog;
	struct chnl_self_trade_c sChnl_self_trade;
	struct six_dz_log_c sSix_dz_log;
	
	memset( cFlag, 0x0, sizeof(cFlag) );

	get_zd_long( "0440", &lDate );

	sprintf( acErrMsg, "����������������>��������Ϊ[%ld]", lDate );
	WRITEMSG
	
	ret = sql_execute( "delete from six_dz_log where tx_date = %ld", lDate );
	
	/*--------------�ж��Ƿ��Ѿ��Թ���---------------*/
	sprintf( trn_date, "%ld", lDate );
	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/%s.txt", trn_date );
	fw = fopen( filename, "r" );	
	if(fw != NULL)
	{
		fclose(fw);
		sprintf( cMessage, "�����Ѿ�����![%s]", trn_date );
		sprintf( acErrMsg, "�����Ѿ�����![%s]", trn_date );
		WRITEMSG
		strcpy( cFlag, "0" );
		return(0);
	}	
	
	/*-------ȡ�����ļ�,���ж�ȡ�����ļ��Ƿ�ɹ�----------*/
	memset( filename1, 0x0, sizeof(filename1) );
	memset( filename2, 0x0, sizeof(filename2) );
	sprintf( filename1, "ZXLS_4302_%s.txt", trn_date );
	sprintf( filename2, "HZQC_4302_%s.txt", trn_date );
	
	memset( buf, 0x00, sizeof(buf) );
	sprintf( buf, "%s/bin/ftp_lc_dz.sh %s %s %s", getenv("HOME"), filename1, filename2, trn_date );
	system( buf );

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/HZQC_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if(fr==NULL)
	{
		sprintf( cMessage, "����û�����ɶ����ļ� %s !", filename );
		sprintf( acErrMsg, "����û�����ɶ����ļ�[%s]", filename );
		WRITEMSG
		goto ErrExit;
	}
	fclose(fr);	

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/ZXLS_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "����û�����ɶ����ļ� %s !", filename );
		sprintf( acErrMsg, "����û�����ɶ����ļ� %s !", filename );
		WRITEMSG
		goto ErrExit;
	}
	fclose(fr);	
	
	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/%s.txt", trn_date );
	fw = fopen( filename, "w" );
	fprintf( fw, "��������: %s\n", trn_date );	
		
	tdb_bs_sh = bdt_bs_sh = 0 ;
	tdb_jffs_sh = tdb_dffs_sh = 0.0 ;
	bdt_jffs_sh = bdt_dffs_sh = 0.0 ;
	memset( jdbz, 0x0, sizeof(jdbz) );	
	
	/*--------------����������ش�ȡ���--------------*/
	ret = Four_tradelog_Dec_Sel( g_pub_tx.reply , "tx_date = %ld and tx_stat = '0' \
		and tx_code in ('7701', '7702', '7801', '7802', '7711', '7713') order by trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "ִ��Four_tradelog_Dec_Sel��[%s]!!", g_pub_tx.reply );
		sprintf( acErrMsg, "ִ��Four_tradelog_Dec_Sel��[%s]!!", g_pub_tx.reply );
		WRITEMSG
		return 1;
	}
		
	while(1)
	{
		memset( &sFour_tradelog, 0x0, sizeof(sFour_tradelog) );
		ret = Four_tradelog_Fet_Sel( &sFour_tradelog, g_pub_tx.reply ); 
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "ִ��Four_tradelog_Fet_Sel��[%s]!!", g_pub_tx.reply );
			sprintf( acErrMsg, "ִ��Four_tradelog_Fet_Sel��[%s]!!", g_pub_tx.reply );
			WRITEMSG
			return 1;
		}
		
		pub_base_strpack( sFour_tradelog.tx_code );
		
	 	if( sFour_tradelog.tx_code[1] == '7' )	/* ������ */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sFour_tradelog.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sFour_tradelog.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4302" );
	 		strcpy( sSix_dz_log.mb_bank, "4301" );
	 		sSix_dz_log.tx_amt = sFour_tradelog.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sFour_tradelog.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sFour_tradelog.chang_no );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );	 		
	 		
	 		if ( strcmp( sFour_tradelog.tx_code, "7711" ) && strcmp( sFour_tradelog.tx_code, "7713" ) )
	 		{	/* 7711 ���ճ忪�����ʻ�  7713 ���ղ��������ʻ� */
	 			bdt_bs_sh++ ;
	 	  	
	 	  		if ( memcmp( sFour_tradelog.tx_code, "7701", 4 ) == 0 )
	 	    			bdt_jffs_sh += sFour_tradelog.tx_amt ;
	 	  		else
	 	  			bdt_dffs_sh+= sFour_tradelog.tx_amt;
	 		}
	 	}
	 	else	/* ������ */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sFour_tradelog.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sFour_tradelog.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4301" );
	 		strcpy( sSix_dz_log.mb_bank, "4302" );
	 		sSix_dz_log.tx_amt = sFour_tradelog.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sFour_tradelog.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sFour_tradelog.chang_no );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 		
	 		tdb_bs_sh++;
	 		
	 		if( memcmp( sFour_tradelog.tx_code, "7801", 4) ==0 )
	 	 		tdb_dffs_sh += sFour_tradelog.tx_amt;
	 	  	else
	 	  		tdb_jffs_sh += sFour_tradelog.tx_amt;
	 	}	 	
	 	
	 	++num_sh;
	 	
	 	if ( num_sh >= MAX )
	 	{
	 		sprintf( cMessage, "���ݿռ�̫С!" );
	 		sprintf( acErrMsg, "���ݿռ�̫С!" );
	 		WRITEMSG
	 	  	goto ErrExit;
	 	}	  
	}
	Four_tradelog_Clo_Sel();
	
	/*-----------------ATMȡ��,POS���ѽ���----------------*/
	ret = Chnl_self_trade_Dec_Sel( g_pub_tx.reply , "tx_date = %ld and stat = '0' \
		and ( ( tx_code = '6203' and info_source in ('1', '2') ) or \
		tx_code = '6301' )  order by trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "ִ��Four_tradelog_Dec_Sel��[%s]!!", g_pub_tx.reply );
		sprintf( acErrMsg, "ִ��Four_tradelog_Dec_Sel��[%s]!!", g_pub_tx.reply );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset( &sChnl_self_trade, 0x0, sizeof(sChnl_self_trade) );
		ret = Chnl_self_trade_Fet_Sel( &sChnl_self_trade, g_pub_tx.reply ); 
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "ִ�� chnl_self_trade��[%s]!!", g_pub_tx.reply );
			sprintf( acErrMsg, "ִ�� chnl_self_trade��[%s]!!", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		/*-------------ATM,POS�����ڴ��--------------*/
		if( !strcmp( sChnl_self_trade.tx_code, "6203") && sChnl_self_trade.info_source[0] == '2' )	/* ������ */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sChnl_self_trade.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sChnl_self_trade.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4302" );
	 		strcpy( sSix_dz_log.mb_bank, "4301" );
	 		sSix_dz_log.tx_amt = sChnl_self_trade.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sChnl_self_trade.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sChnl_self_trade.stra );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 			
	 	  	bdt_bs_sh++ ;
	 	  	bdt_dffs_sh+= sChnl_self_trade.tx_amt;	/*����*/
	 	}
	 	else	/* ������ */
	 	{ 
	 		 			
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sChnl_self_trade.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sChnl_self_trade.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4301" );
	 		strcpy( sSix_dz_log.mb_bank, "4302" );
	 		sSix_dz_log.tx_amt = sChnl_self_trade.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sChnl_self_trade.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sChnl_self_trade.stra );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 		
	 		tdb_bs_sh++;
	 	  	tdb_jffs_sh += sChnl_self_trade.tx_amt;	/*�跽*/
	 	}
	 		 	
	 	++num_sh;
	 	
	 	if ( num_sh >= MAX )
	 	{
	 		sprintf( cMessage, "���ݿռ�̫С!" );
	 		sprintf( acErrMsg, "���ݿռ�̫С!" );
	 		WRITEMSG
	 	  	goto ErrExit;
	 	}		
	}
	Chnl_self_trade_Clo_Sel();
		
	/*-------------��ȡ�����ļ�--------------*/
	sprintf( filename, "/backup/data/HZQC_4302_%s.txt",  trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "Can't open file %s !", filename );
		sprintf( acErrMsg, "Can't open file %s !", filename );
		WRITEMSG
		fclose(fw);
		goto ErrExit;
	}
	
	while( fgets( buf, 1023, fr) != NULL)
	{
		/*--------�ж��Ƿ��м�¼--------*/
		if( memcmp( buf+9, "4302", 4 ) !=0 ) 
		{
			continue;
		}
		else
		{
	    		if( memcmp( buf, trn_date, 8 ) !=0 )
	    		{
	      			buf[8]=0;
	      			sprintf( cMessage,"���ڲ���ȷ! %s" , buf);
	      			sprintf( acErrMsg, "���ڲ���ȷ! %s", buf );
	      			WRITEMSG
	      			fclose( fr );
	      			goto ErrExit;
	    		}
	    		
	    		memset(fldstr,0x0,sizeof(fldstr));
			for( i=0;i<15;i++ )
			{
				ret=pub_base_cut_str( buf, fldstr[i], "|", i+1 );
					vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
				pub_base_strpack( fldstr[i] );
			}
	    		  		
	    		/* ���������� */
	    		bdt_bs_hj = atoi(fldstr[6]);
	    		/* �������跽������ */
	    		bdt_jffs_hj = atof(fldstr[7]);
	    		/* ���������������� */
	    		bdt_dffs_hj = atof(fldstr[8]);
	    		
	    		/* ���������� */
	    		tdb_bs_hj = atoi(fldstr[9]);
	    		/* �������跽������ */
	    		tdb_jffs_hj = atof(fldstr[10]);
	    		/* ���������������� */
	    		tdb_dffs_hj = atof(fldstr[11]);
	    		
	    		/* �����־ */
	    		jdbz[0] = fldstr[13][0];
	    		/* ������ */
	    		qsje_hj = atof(fldstr[14]);
	    		
	    		break;
		}
	}
	fclose(fr);
	
	tdb_bs_zx = bdt_bs_zx = 0 ;
	tdb_jffs_zx = tdb_dffs_zx = 0.0 ;
	bdt_jffs_zx = bdt_dffs_zx = 0.0 ;
	
	sprintf( filename, "/backup/data/ZXLS_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "Can't open file %s !", filename );
		sprintf( acErrMsg, "Can't open file %s !", filename );
		WRITEMSG
		fclose(fw);
		goto ErrExit;
	}
	
	num_yl = 0 ;
	while( fgets( buf, 1023, fr) != NULL )
	{
		n = strlen(buf);
		dl_kh = 0 ; /* ����Ϊ������ */
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<35;i++ )
		{
			ret=pub_base_cut_str( buf, fldstr[i], "|", i+1 );
				/*vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);*/
			pub_base_strpack( fldstr[i] );
		}		
		
		/*01: ������ */
		strcpy( trcd, fldstr[0] );
		
		/*---------��ؿ���ѯ����¼ȥ��----------*/
		if( memcmp(trcd,"7795",4) == 0  || memcmp(trcd,"7895",4) == 0 )
		{
			continue;
		}
		
		/* 02: �������� */
		strcpy(date , fldstr[1] );
		
		/* 03: ����ʱ�� */ 
		strcpy(time , fldstr[2] );
		
		/* 04�������� */
		strcpy(srsn , fldstr[3] );
		if( memcmp(srsn , "4302",4) == 0 ) 
		{
			dl_kh = 1;
		}
		
		/* 05�������� */
		strcpy( otsn , fldstr[4] );
		if ( memcmp( otsn ,"4302",4 ) == 0 ) 
		{
			dl_kh = 2 ;
		}
		
		/* �����������ѭ��, �����´��� */
		if(dl_kh == 0)  
		{
			continue; 
		}
		
		/* ��Ϊ�����У��򽫽������Ϊ78XX */
		if(dl_kh==2) 
		{
			trcd[1]='8';  
		}
		
		/* 06�����׹�Ա */
		strcpy(oper, fldstr[5]);
		
		/* 07�����׻��� */
		
		/* --- 08���ʻ��� */
		
		/* --- 09��ƾ֤���� */
		
		/*10������ 16λ��19λ */
		strcpy( amcr, fldstr[9] );
		if( memcmp( lsbuf, "69430", 5) == 0 )
		{
			memcpy( amcr, lsbuf+2, 16 ); 
			amcr[16]=0;
		}
		
		/* 11�����׽�� */
		tram = atof(fldstr[10])/1.00 ;
		
		/* 12�����׷�ʽ  0-�� 1-�� */
		
		/* 13������(����)����ˮ ����Ϊ10λ��ǰ���� */
		strcpy(seq_dl, fldstr[12]);
		
		/* 14������(����)����ˮ ����Ϊ10λ��ǰ���� */
		strcpy(seq_kh, fldstr[13]);
		
		/* 15��������ˮ ����Ϊ10λ��ǰ���� */
		strcpy(seq_zx, fldstr[14]);
		
		/* 16�������ʺ� POS����ʱ����̻��� */
		
		/* 17�����ý�� ��������Ϣ*/
		
		/* 18�����б�־ 1-��ʾ���Ľ��յ����� 2-��ʾ���׳ɹ����  */
		
		/* --- 19�������ж��ʱ�־ -���ֶ�ֹͣʹ�� */
		
		/* --- 20�������ж��ʱ�־ ���ֶ�ֹͣʹ�� */
		
		/* 21�������־ 0-��ʾ�跽  1-��ʾ���� */
		strcpy(jdbz, fldstr[20]);
		
		/* ---22���������� */
		
		/* ---23���������� */
		
		/* 24��ԭ����������ˮ  ��������ʱ�ɴ������ύ��ԭ����������ˮ */
		
		/* 25����Ȩ���� ��Ŵ����е���Ȩ���� */
		
		/* 26���ͻ�����  �����з��صĿͻ�����Ϣ */
		
		/* 27�����Ҵ��� 01-����� */
		
		/* 28����������  0-�����з��� 1-���ķ��� */
		
		/* --- 29����ע1 */
		
		/* --- 30����ע2 */
		
		/* 31����Ӧ�� 00-�ɹ� */
		if(fldstr[30][0]==10) 
			fldstr[30][0]=' '; /* �����и�Ϊ�ո� */
		
		strcpy( ret_code, fldstr[30] );
		
		/* 32�������ʱ�־  0-�������� 1-���������� */
		strcpy( czbz, fldstr[31] );
		
		/* 33���ն˺� */
		if( memcmp( ret_code, "00",2)==0 && czbz[0]!='1' && czbz[0]!='0' )
		{
						
			memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "1" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, trcd );
	 		strcpy( sSix_dz_log.ac_no, amcr );
	 		strcpy( sSix_dz_log.sl_bank, srsn );
	 		strcpy( sSix_dz_log.mb_bank, otsn );
	 		sSix_dz_log.tx_amt = tram;
	 		sprintf( sSix_dz_log.sl_trace_no, "%s", seq_dl );
	 		sprintf( sSix_dz_log.center_trace_no, "%s", seq_zx );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", seq_kh );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
				
			++num_yl;
			if( dl_kh == 1 )	/* ������ */
			{ 
		    		bdt_bs_zx++ ;
		    		if( memcmp( trcd, "7701", 4 ) == 0 )
		      			bdt_jffs_zx += tram ;
		    		else 
		    			bdt_dffs_zx+= tram;
		  	}
		  	else 	/* ������ */
		  	{
		    		tdb_bs_zx++ ;
		    		if(memcmp(trcd,"7801",4)==0)
		      			tdb_dffs_zx += tram ;
		    		else
		    			tdb_jffs_zx += tram;
		  	}
		}
		
		if( num_yl >= MAX )
		{
			sprintf( cMessage, "���ݿռ����С!" );
		  	sprintf( acErrMsg, "���ݿռ����С!" );
		  	WRITEMSG
		  	fclose(fr);
		  	goto ErrExit;
		}
	}
		
	fclose(fr);

	fprintf(fw,"��������:\n");
	fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n",\
	      bdt_bs_hj, bdt_jffs_hj, bdt_dffs_hj );
	fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n",\
	      tdb_bs_hj, tdb_jffs_hj, tdb_dffs_hj );
	fprintf(fw,"    ������:%.2f\n", qsje_hj);
	
	/*----------------����������������ϸ�Ա�----------------*/
	if (bdt_bs_zx != bdt_bs_hj || tdb_bs_zx!=tdb_bs_hj)
	{
		fprintf(fw,"������ϸ:\n");
		fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n",\
		   bdt_bs_zx, bdt_jffs_zx, bdt_dffs_zx );      
		fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n", \
		   tdb_bs_zx, tdb_jffs_zx, tdb_dffs_zx );
	}
	
	/*-----------------�����������������ʶԱ�---------------*/
	if (bdt_bs_sh != bdt_bs_hj || tdb_bs_sh!=tdb_bs_hj ||
	fabs(bdt_jffs_sh-bdt_jffs_hj)>0.001||fabs(bdt_dffs_sh-bdt_dffs_hj)>0.001||
	fabs(tdb_jffs_sh-tdb_jffs_hj)>0.001||fabs(tdb_dffs_sh-tdb_dffs_hj)>0.001)
	{
		fprintf(fw,"���л���:\n");
		fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n",\
		   bdt_bs_sh, bdt_jffs_sh, bdt_dffs_sh );
		fprintf(fw,"    ���������� %4d   �跽:%12.2f   ����:%12.2f\n", \
		   tdb_bs_sh, tdb_jffs_sh, tdb_dffs_sh );
	}
	else
	{
		fprintf( fw, "��������˶���ȷ!\n" );
	}
	
	/*---------------������������-----------------*/
	ret =  Six_dz_log_Dec_Sel( g_pub_tx.reply , "inf_source = '0' and tx_date = %ld \
		order by center_trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "ִ��Six_dz_log_Dec_Sel��!" );
		sprintf( acErrMsg, "ִ��Six_dz_log_Dec_Sel��!" );
		WRITEMSG
		fclose(fr);
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
		ret = Six_dz_log_Fet_Sel( &sSix_dz_log, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "ִ��Six_dz_log_Fet_Sel��!" );
			sprintf( acErrMsg, "ִ��Six_dz_log_Fet_Sel��!" );
			WRITEMSG
			fclose(fr);
			goto ErrExit;
		}
		
		ret = sql_count( "six_dz_log", "inf_source = '1' and tx_date = %ld \
			and ac_no = '%s' and tx_amt = %lf and center_trace_no = '%s'", \
			lDate, sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		if ( ret != 1 )
		{
			fprintf( fw, "���� %4s %4s %4s %-19s %10.2f %s\n",\
	 			sSix_dz_log.tx_code, sSix_dz_log.sl_bank, sSix_dz_log.mb_bank, \
	 			sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		}
	}
	
	Six_dz_log_Clo_Sel();
	
	/*---------------������������-----------------*/
	ret =  Six_dz_log_Dec_Sel( g_pub_tx.reply , "inf_source = '1' and tx_date = %ld \
		order by center_trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "ִ��Six_dz_log_Dec_Sel��!" );
		sprintf( acErrMsg, "ִ��Six_dz_log_Dec_Sel��!" );
		WRITEMSG
		fclose(fr);
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
		ret = Six_dz_log_Fet_Sel( &sSix_dz_log, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "ִ��Six_dz_log_Fet_Sel��!" );
			sprintf( acErrMsg, "ִ��Six_dz_log_Fet_Sel��!" );
			WRITEMSG
			fclose(fr);
			goto ErrExit;
		}
		
		ret = sql_count( "six_dz_log", "inf_source = '0' and tx_date = %ld \
			and ac_no = '%s' and tx_amt = %lf and center_trace_no = '%s'", \
			lDate, sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		if ( ret != 1 )
		{
			fprintf( fw, "���� %4s %4s %4s %-19s %10.2f %s\n",\
	 			sSix_dz_log.tx_code, sSix_dz_log.sl_bank, sSix_dz_log.mb_bank, \
	 			sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		}
	}
	
	Six_dz_log_Clo_Sel();	
	
	fclose(fw);
		
OkExit:
	strcpy( cFlag, "0" );
	strcpy( cMessage, "�������!" );
	return 0;
ErrExit:
	fclose( fw );
	strcpy( cFlag, "1" );
	return 0;
}
