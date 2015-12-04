/*************************************************
* �� �� ��:  sp8110.c
* ���������� �¾��˺Ŷ��ձ��ѯ
*
* ��    ��:  
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_merge_ac_c.h"
#include "mdm_attr_c.h"

int sp8110()
{
	struct mo_merge_ac_c vmo_merge_ac;
	struct mo_merge_ac_c f_mo_merge_ac;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

		int prt_flag=0; /* �Ƿ���۱�־ 1 ���� 0����*/
		struct mdm_ac_rel_c sMdm_ac_rel;
		struct mdm_attr_c sMdm_attr;

		memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
		memset(&sMdm_attr, 0x0, sizeof(struct mdm_attr_c));

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0640",f_mo_merge_ac.old_br_no );
	get_zd_data("0630",f_mo_merge_ac.old_ac_no );
	get_zd_data("0910",f_mo_merge_ac.br_no );
	get_zd_data("0380",f_mo_merge_ac.ac_no );
	get_zd_data("0700",f_mo_merge_ac.chg_mdm_yn );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_merge_ac.old_br_no );
	if( strlen(f_mo_merge_ac.old_br_no) )
	{
		sprintf( tmpstr," old_br_no='%s' and",f_mo_merge_ac.old_br_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_mo_merge_ac.old_ac_no );
	if( strlen(f_mo_merge_ac.old_ac_no) )
	{
		sprintf( tmpstr," old_ac_no='%s' and",f_mo_merge_ac.old_ac_no );
		strcat( wherelist,tmpstr );
		prt_flag = 1;
	}

	pub_base_strpack( f_mo_merge_ac.br_no );
	if( strlen(f_mo_merge_ac.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_mo_merge_ac.br_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_mo_merge_ac.ac_no );
	if( strlen(f_mo_merge_ac.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_mo_merge_ac.ac_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_mo_merge_ac.chg_mdm_yn );
	if( strlen(f_mo_merge_ac.chg_mdm_yn) )
	{
		sprintf( tmpstr," chg_mdm_yn='%s' and",f_mo_merge_ac.chg_mdm_yn );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_merge_ac" );

	/**��ɲ�ѯ**/
	/***
	strcat(wherelist," br_no='") ;
	strcat( wherelist,g_pub_tx.tx_br_no );
	strcat(wherelist,"'") ;
	***/
	strcat(wherelist," 1=1 ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"old_br_no,old_ac_no,br_no,ac_no,chg_mdm_yn");

	ret=Mo_merge_ac_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_merge_ac_Fet_Sel( &vmo_merge_ac, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**��ʾ�б���**/
			fprintf( fp,"~@�ɻ�����|@����/����|@������|@�˿���|@�Ƿ��Ѹ�������|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */


		fprintf( fp, "%s|%s|%s|%s|%s|\n",vmo_merge_ac.old_br_no,vmo_merge_ac.old_ac_no,vmo_merge_ac.br_no,vmo_merge_ac.ac_no,vmo_merge_ac.chg_mdm_yn);
		ttlnum++;
	}

	ret=Mo_merge_ac_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

	/* add by rob for print old bankbook at 20030804 **/
	if (prt_flag == 1)
	{
		pub_base_strpack(vmo_merge_ac.ac_no);
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",
							 vmo_merge_ac.ac_no );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "��ѯ�����˺Ŷ��ձ����!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "�����˺Ŷ��ձ��в����ڸ��˺ŵļ�¼!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "B117");
			goto ErrExit;
		}

		pub_base_strpack(sMdm_ac_rel.mdm_code);
		ret = Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'",
			sMdm_ac_rel.mdm_code);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "��ѯ�������Ա����!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "N056");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "�������Բ����ڸü�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "N055");
			goto ErrExit;
		}
		if (sMdm_attr.mdm_type[0] == '1') /* ������ */
		{
			sprintf(acErrMsg, "��Ҫ��ӡ���˺�");
			WRITEMSG
			set_zd_data("0670", "Y"); /* �ھɴ����ϴ�ӡ���˺� */
		}
		else
		{
			sprintf(acErrMsg, "����Ҫ��ӡ���˺�");
			WRITEMSG
			set_zd_data("0670", "N"); /* �ھɴ����ϲ���ӡ���˺� */
		}
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
