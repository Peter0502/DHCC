/*************************************************
* �� �� ��: sp9787.c
* �������������ݽ������෵��֧ȡ��ʽ_2106�Թ����ڿ�����ѯ
* ��    ��:    
* ������ڣ�2003��01��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "mdm_prdt_rel_c.h"

sp9787()  
{ 		
	int ret=0,i=0;
	char tmp_type[5];
	FILE *fp;
	char filename[100];
	char wherelist[100];

	struct mdm_attr_c g_mdm_attr;
	struct mdm_prdt_rel_c vmdm_prdt_rel;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&vmdm_prdt_rel,0x00,sizeof(struct mdm_prdt_rel_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1161",tmp_type);					/*��������*/
	
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"a");
	if( fp==NULL )
	{
		 sprintf( acErrMsg,"open file error [%s]",filename );
		 WRITEMSG
		 strcpy( g_pub_tx.reply,"S047");
		 goto ErrExit;
	}

	vtcp_log(
		"mdm_code1='%s' and prdt_no in(select prdt_no from td_parm where \
		cif_type in('2','3,'4') ) and mdm_code2='%s' and mdm_code3='%s' \
		order by mdm_code1",tmp_type,NULL_MDM,NULL_MDM);
 /***������ʽ��Ϊ����Ʒ�� modify  by wanglei 20061009***/
	ret=Mdm_prdt_rel_Dec_Sel(g_pub_tx.reply, \
		"mdm_code1='%s' and prdt_no in(select prdt_no from td_parm where \
		cif_type in('2','3','4') ) and mdm_code2='%s' and mdm_code3='%s' \
		order by prdt_no",tmp_type,NULL_MDM,NULL_MDM);
	if(ret)
	{
		 sprintf( acErrMsg,"YYYYYYYYYYYYYYY [%d]",ret );
		 WRITEMSG
		 goto ErrExit;
	}
TRACE
TRACE
	while(1)
	{
TRACE
		ret = Mdm_prdt_rel_Fet_Sel( &vmdm_prdt_rel , g_pub_tx.reply );
TRACE
		if( ret==100 ) 
		{
		 	sprintf( acErrMsg,"BBBBBBBBBBBBBBB [%d]",ret );
			WRITEMSG
			break;
		}
		else if( ret ) goto ErrExit;
TRACE

		 sprintf( acErrMsg,"AAAAAAAAAAAAAAAAAAAA [%d]",ret );
		 WRITEMSG
		ret = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",
							vmdm_prdt_rel.prdt_no );
		if( ret ) continue;

		pub_base_strpack(g_td_parm.title);
		if( i==0 )
		{
			set_zd_data("1031",vmdm_prdt_rel.prdt_no);
			set_zd_data("1032",g_td_parm.title);
			set_zd_data(DC_FILE_SND,"2");
		}

		fprintf( fp,"1031|%-30s|%s|\n",g_td_parm.title,vmdm_prdt_rel.prdt_no );
		i++;
	}

	Mdm_prdt_rel_Clo_Sel();
	fclose(fp);

	/* ���ݽ�������ȡ��ز���*/
	ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'",tmp_type);
  	if( ret==100 )
	{
	  	sprintf(acErrMsg,"�����ڸ�ƾ֤����!!mdm_code=[%s]",tmp_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H009");
		goto ErrExit;
	}

	/* ������� */
	if(g_mdm_attr.draw_id_yn[0]=='O')
		strcpy(g_mdm_attr.draw_id_yn,"N");

	if(g_mdm_attr.draw_pwd_yn[0]=='O')
		strcpy(g_mdm_attr.draw_pwd_yn,"N");

	if(g_mdm_attr.pwd_mach_yn[0]=='O')
		strcpy(g_mdm_attr.pwd_mach_yn,"N");

	set_zd_data("1155", g_mdm_attr.draw_id_yn);
	set_zd_data("1152", g_mdm_attr.draw_pwd_yn);
	set_zd_data("115A", g_mdm_attr.pwd_mach_yn);
	set_zd_data("1169", g_mdm_attr.no_ind);			/*��������*/

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
   
