/*************************************************
*2104����ת�浥�Ľ���
* �� �� ��:    sp9786.c
* ����������   
* ��    ��:    rob
* ������ڣ�   2003��01��13��
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
#include "mdm_prdt_rel_c.h"
#include "mdm_attr_c.h"

	
sp9786()
{
	/** ���ݳ�ʼ�� **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_id_code_rel_c      cif_id_code_rel_tmp; 
	struct cif_basic_inf_c        cif_basic_inf; 
	struct mdm_prdt_rel_c        vmdm_prdt_rel; 
	struct mdm_attr_c        vmdm_attr; 
	struct mdm_attr_c        omdm_attr; 
	int ret=0;
	int vcnt=0;
	char wherelist[40];
	char filename[100];
	FILE *fp;
	int i=0;

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

	/***�����б�***/
	ret=Mdm_attr_Dec_Sel( g_pub_tx.reply,"mdm_type='2' order by mdm_code",
						  wherelist,wherelist,NULL_MDM,NULL_MDM);
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Mdm_attr_Fet_Sel( &vmdm_attr , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vmdm_attr.mdm_name);
			if( i==0 )
				set_zd_data("1161",vmdm_attr.mdm_code);
			fprintf( fp,"1161|%s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code );
		i++;
		vcnt++;
	}
	Mdm_attr_Clo_Sel();


	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* ������� */
	set_zd_data( "1166", cif_basic_inf.name );
	vtcp_log("[%s]",cif_basic_inf.name);

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
