/*************************************************
*2104�Ĳ�Ʒ���Ϳ�ݲ�ѯ
* �� �� ��:    sp9784.c
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

	
sp9784()  
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
	char wherelist[400];
	char filename[100];
	FILE *fp;
	int i=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	sprintf( wherelist," cif_type in('0','1','5') and (prdt_no[1,3]>='201' and prdt_no[1,3]<='208') or (prdt_no[1,2]='22' or prdt_no[1,2]='23') and beg_date<%ld and end_date>%ld" ,g_pub_tx.tx_date,g_pub_tx.tx_date);

	vtcp_log("WHERE [%s]",wherelist);

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
	ret=Td_parm_Dec_Sel( g_pub_tx.reply,"%s order by prdt_no",wherelist );
	if( ret ) 
	{
		sprintf( acErrMsg,"�α궨�����[%d]",ret );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	i=0;
	while(1)
	{
		ret = Td_parm_Fet_Sel( &g_td_parm , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1031",g_td_parm.prdt_no);
			set_zd_data("1032",g_td_parm.title);
		}

		pub_base_strpack(g_td_parm.title);
		fprintf( fp,"1031|%s|%s|\n",g_td_parm.title,g_td_parm.prdt_no );

		i++;
		vcnt++;
	}
	Td_parm_Clo_Sel();

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
