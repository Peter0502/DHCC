/*************************************************
* �� �� ��: sp9751.c
* �������������ز�Ʒѡ���б�
*			����
* ��    ��: lance
* ������ڣ�2003��05��16��
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


sp9751()  
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
	
	/** ȡֵ����ֵ **/	
		
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

	strcpy( wherelist , "1=1" );
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
		    
		/****
		if( i==0 )
		{
			set_zd_data("0240",g_ln_parm.prdt_no);
			set_zd_data("0250",g_ln_parm.title);
		}
		*************/
		pub_base_strpack(g_ln_parm.title);
		fprintf( fp,"0240|%s|%s|\n",g_ln_parm.title,g_ln_parm.prdt_no );

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

