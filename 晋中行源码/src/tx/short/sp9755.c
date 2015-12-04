/*************************************************
* �� �� ��: sp9755.c
* �������������ݿͻ�����ʾ��Ӧ��֤�����͵Ŀ�ݲ�ѯ
* ��    ��: lance
* ������ڣ�2003��05��30��
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
	
sp9755()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct dic_data_c        vdic_data; 
	struct cif_basic_inf_c       cif_basic_inf; 	
	int ret=0;
	char wherelist[40];
	char filename[100];
	char type[2],tmp[5];
	FILE *fp;
	int i=0;
	long cif_no;

	memset (&vdic_data, 0x00, sizeof(struct dic_data_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

	/** ȡֵ����ֵ **/	
	get_zd_long( "0280" , &cif_no );	/* �ͻ��� */
	
	/* ���ݿͻ����жϿͻ����� */
	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿͻ��Ų�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C097");
		ERR_DEAL;
	}else if( ret ){
		sprintf(acErrMsg,"���ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	
	WRITEMSG	
	strcpy( type , cif_basic_inf.type );
	sprintf(acErrMsg,"����type![%s]",type);
	WRITEMSG	
	set_zd_data( "0260" , cif_basic_inf.name );
	set_zd_data( "0720" , cif_basic_inf.type );
	set_zd_data( "0710" , cif_basic_inf.sts );

	if(type[0]=='1')
		strcpy(wherelist,"per_id");
	else if(type[0]=='2'||type[0]=='4'||type[0]=='3')
		strcpy(wherelist,"com_id");

/*1�����ˣ�2����˾ ��3��������ָ�������ص���֯����4��ͬҵ */
	
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

	/***ѡ���б�***/
	if(type[0]=='3')
	{
		ret=Dic_data_Dec_Sel(g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','B')",wherelist);
		if( ret ) goto ErrExit;
	}
	else if(type[0]=='2'){                /** ���ӹ�˾��֤�������޸ķǸ���֤��ѡ�� add by martin 20080619 **/
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','9','A','C','D') order by pr_seq",wherelist);
		if( ret ) goto ErrExit;
	}
	else if(type[0]!='1'){               /** ���ӹ�˾��֤�������޸ķǸ���֤��ѡ�� add by martin 20080619 **/ 
		/*ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','9','A','B','C')",wherelist);*/
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','9','A')",wherelist);
		if( ret ) goto ErrExit;
	}else{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' ",wherelist);
		if( ret ) goto ErrExit;
	}	

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
	
		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )	set_zd_data("0680",vdic_data.pr_seq);
		fprintf( fp,"0680|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq );
		i++;
	}

	Dic_data_Clo_Sel();

	if( i )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
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
