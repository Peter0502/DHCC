/*************************************************
* �� �� ��: sp9747.c
* �����������������,����֤�����ͺ�֤����������� ���ز�Ʒѡ���б�
*			����
* ��    ��: lance
* ������ڣ�2003��05��03��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"


sp9747()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_id_code_rel_c      cif_id_code_rel_tmp; 
	struct cif_basic_inf_c        cif_basic_inf; 

	int ret=0, time_type=0;
	int sel_flag=0;
	int vcnt=0;
	char trust_flg[2];
	char ln_type[2];
	char time_typ[2];
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	FILE *fp;
	int i=0;

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_id_code_rel_tmp, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/	
	get_zd_data( "0670" , cif_id_code_rel_tmp.id_type );	/* ֤������ */
	get_zd_data( "0620" , cif_id_code_rel_tmp.id_no );	/* ֤������ */
	get_zd_data( "1152" , trust_flg );	/* ί��Э���־ */
	get_zd_int( "103A" , &time_type ); /* ʱ�� */
	get_zd_data( "1145", ln_type );    /* ���ʽ */

	vtcp_log("[%s][%s][%s][%d}{%s}",cif_id_code_rel_tmp.id_type,cif_id_code_rel_tmp.id_no,trust_flg,time_type,ln_type);
	
	if( time_type<=12 )
	{
		strcpy( time_typ , "1" );
	}else if( time_type<=36 ){
		strcpy( time_typ , "2" );
	}else	if( time_type<=60 ){
		strcpy( time_typ , "3" );
	}else{
		strcpy( time_typ , "4" );
	}
	
	TRACE
			
	/* ����֤�����ͺ�֤������ȡ�ͻ��� */
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
	" id_type='%s' and id_no='%s' " , cif_id_code_rel_tmp.id_type , cif_id_code_rel_tmp.id_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ϣû�еǼ�,�뵽�ͻ����ĵǼ�[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
	
	TRACE
					  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_id_code_rel.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ӧ�Ŀͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}

	/*1�����ˣ� 2����˾ ��3��������ָ�������ص���֯���� 
	4�����ڻ����� 5���ڲ�Ա�� */
	/*0�����ã�1�����ˣ� 2����˾�� */
	if( cif_basic_inf.type[0]=='1' )
		sprintf( wherelist," cif_type in('1','0') " );
	if( cif_basic_inf.type[0]=='2' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='3' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='4' )
		sprintf( wherelist," cif_type in('0','2') " );
	if( cif_basic_inf.type[0]=='5' )
		sprintf( wherelist," cif_type in('1','0') " );

		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG			
	/* �Ƿ�ί��Э����� 
		sprintf( tmpstr," and trust_prot_ind='%s' ",trust_flg );
		strcat( wherelist,tmpstr );
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG
	***/			
	/* �������� */

		sprintf( tmpstr," and time_type='%s' ",time_typ );
		strcat( wherelist,tmpstr );
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG	
	/* ���ʽ */
		sprintf( tmpstr," and ln_type='%s' ",ln_type );
		strcat( wherelist,tmpstr );		
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG	
	/* ������������ */
		sprintf( tmpstr," and beg_date<='%ld' ",g_pub_tx.tx_date );
		strcat( wherelist,tmpstr );		
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG	
	/* ����ֹ������ */
		sprintf( tmpstr," and end_date>='%ld' ",g_pub_tx.tx_date );
		strcat( wherelist,tmpstr );		
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG
				
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

	/***��Ʒ�б�***/
	ret=Ln_parm_Dec_Sel( g_pub_tx.reply,"%s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Ln_parm_Fet_Sel( &g_ln_parm , g_pub_tx.reply );
		if( ret==100 ) break;		
		if( ret ) goto ErrExit;
		
		sel_flag++;
		    
		if( i==0 )
		{
			set_zd_data("0230",g_ln_parm.prdt_no);
			set_zd_data("0810",g_ln_parm.title);
		}

		pub_base_strpack(g_ln_parm.title);
		fprintf( fp,"0230|%-30s|%s|\n",g_ln_parm.title,g_ln_parm.prdt_no );

		i++;
		vcnt++;
	}
	Ln_parm_Clo_Sel();

		if ( sel_flag==0 )
		{    
		    sprintf(acErrMsg,"û���������������ļ�¼![%s]",g_pub_tx.reply);
		    WRITEMSG
		    strcpy(g_pub_tx.reply,"L123");
		    goto ErrExit;	
		}
		
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* ������� */

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
