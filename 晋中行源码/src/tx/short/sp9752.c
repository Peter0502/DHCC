/*************************************************
* �� �� ��:    sp9752.c
* ����������   
*              ���ڲ�Ʒ���뷵�ز�Ʒ��Ϣ_�Թ�
*
* ��    ��:    lance
* ������ڣ�   2003��02��01��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:	Azz
*        �޸�����:	
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9752()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	char filename[100];
	FILE *fp;
	
	
	/** ���ݳ�ʼ�� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "1031" , prod_no );	/* ��Ʒ���� */
	
	/* ���ݲ�Ʒ���� ���ز�Ʒ���� */
	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"���ø��ݲ�Ʒ����ȡ��Ʒ������������");
		WRITEMSG
		goto ErrExit;
	}		  

	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/* ������� */
	switch( prod_no[0] )
	{
		case '1':
			if(g_dd_parm.ac_type[0]=='5')
			{
				if(g_dd_parm.sttl_type[0]=='D')
					strcpy(g_dd_parm.ac_type,"5");
				else if(g_dd_parm.sttl_type[0]=='S')
					strcpy(g_dd_parm.ac_type,"6");
			}
			set_zd_data( "1032", g_dd_parm.title );	/* ��Ʒ���� */
			set_zd_data( "1033", g_dd_parm.cur_no );
			set_zd_data( "1034", g_dd_parm.ac_type );
			if( g_dd_parm.cif_type[0]=='0' || g_dd_parm.cif_type[0]=='1' 
				|| g_dd_parm.cif_type[0]=='5' )
			{
				fprintf(fp,"1034|���˽��㻧|5|\n");
				fprintf(fp,"1034|���˴��|6|\n");
			}
			if( g_dd_parm.cif_type[0]=='0' || g_dd_parm.cif_type[0]=='2' )
			{
				fprintf(fp,"1034|������|1|\n");
				fprintf(fp,"1034|һ�㻧|2|\n");
				fprintf(fp,"1034|ר�û�|3|\n");
				fprintf(fp,"1034|��ʱ��|4|\n");
			}
			set_zd_data( DC_FILE_SND,"2" );
			set_zd_data( "1036", g_dd_parm.intst_type );
			break;
		case '2':
			set_zd_data( "1032", g_td_parm.title );	/* ��Ʒ���� */
			set_zd_data( "1033", g_td_parm.cur_no );
			/* set_zd_data( "1034", g_td_parm.ac_type ); */
			set_zd_data( "1036", g_td_parm.intst_type );
			set_zd_data( "103C", g_td_parm.td_type );
			set_zd_long( "103A", g_td_parm.term );
			if(g_td_parm.term_type[0]=='Y')
			{
				set_zd_data( "103B","��" );
			}
			else if(g_td_parm.term_type[0]=='M')
			{
				set_zd_data( "103B", "��" );
			}
			
	sprintf(acErrMsg,"����[%s][%d][%s] [%s]",
		g_td_parm.title,g_td_parm.term,g_td_parm.term_type,g_td_parm.intst_type);
	WRITEMSG
			break;
	}

	fclose(fp);
		
	set_zd_data( "103E", "1" );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
