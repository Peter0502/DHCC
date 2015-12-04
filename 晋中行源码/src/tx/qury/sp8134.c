/*************************************************
* �� �� ��:  sp8134.c
* ���������� ����atm����ϸ,�����˺Ų�ѯ���ʮ�ʵĽ�����ϸ
*
* ��    ��:  huzhaohua
* ������ڣ� 20131030

*insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
 values ('8134', '����atm����ϸ', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');

 insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
 values ('8134', 0, '8134', '#$');

 insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
 values ('8134', '����atm����ϸ', 'sp8134', '0', '0');
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"

int sp8134()
{
	
	char sAc_no[25];
	char sTemp[125];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[512];
	char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
	char tablelist[128];   /**Ҫ��ѯ�ı���**/
	int ttlnum=0;			/**��ȡ��������**/
	int ret=0;
	char sTx_date[9];
	char filename[100];
	char sTx_amt[13];
	char sBal[13];
	struct dd_mst_hst_c vdd_mst_hst;
	struct mdm_ac_rel_c vmdm_ac_rel;
	FILE *fp;

	memset(sAc_no,0x00,sizeof(sAc_no));
	memset(sTemp,0x00,sizeof(sTemp));
	memset(sTx_date,0x00,sizeof(sTx_date));
	memset(filename,0x00,sizeof(filename));
	memset(sTx_amt,0x00,sizeof(sTx_amt));
	memset(sBal,0x00,sizeof(sBal));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
	memset( &vmdm_ac_rel,0,sizeof(vmdm_ac_rel));


  vtcp_log( "����atm����ϸ...");   
  
	pub_base_sysinit();
  get_zd_data("0300",sAc_no );
  vtcp_log( "0300==>sAc_no [%s]",sAc_no );


	/**��ɲ�ѯ����**/
	pub_base_strpack( sAc_no );
	if( strlen(sAc_no) )
	{
		if ( pub_base_acno_acid(&vdd_mst_hst.ac_id,sAc_no) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",vdd_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	else
	{
    vtcp_log( "ƽ̨����30�򿨺�Ϊ��!" );
		goto ErrExit;
	}

	strcpy( tablelist,"dd_mst_hst" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY tx_date desc,trace_no desc,trace_cnt desc");

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"tx_amt,tx_date,brf");

	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
  
  ttlnum=0;
  
	while(1)
	{
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 || ttlnum==10) break;
		ERR_DEAL
		if( !ttlnum )
		{
		pub_base_AllDwnFilName( filename );
		vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,filename );
		
		fp = fopen( filename,"w" );
		if( fp==NULL )
			{
			sprintf(acErrMsg," open file [%s] error ",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
			}
		}
		vdd_mst_hst.tx_amt=vdd_mst_hst.tx_amt*100;
		if(vdd_mst_hst.tx_amt<0.00)
			{
			vdd_mst_hst.tx_amt=0-vdd_mst_hst.tx_amt;	
			}
		sprintf( sTx_amt,"%012.0f",vdd_mst_hst.tx_amt);/*���׽��*/
		vdd_mst_hst.bal=vdd_mst_hst.bal*100;
		sprintf( sBal,"%012.0f",vdd_mst_hst.bal);/*���*/
		fprintf(fp, "%ld|%s|%s|%s|\n",vdd_mst_hst.tx_date,sTx_amt,sBal,vdd_mst_hst.brf);
		ttlnum++;
	}	
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_int("0220",ttlnum);
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
