/*************************************************
* �� �� ��:  sp8701.c
* ���������� ��Ŀ���ѯ
*
* ��    ��:  rob
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
#include "com_item_c.h"

int sp8701()
{
	struct com_item_c vcom_item;
	struct com_item_c f_com_item;
	char f_sub_acc_yn2[41];
	char f_acc_lvl3[41];
	char f_dc_ind4[41];
	char f_roll_ind5[41];
	char f_acc_knd6[41];
	char f_fore_ind7[41];
	char f_equa_ind8[41];
	char f_amt_dc_ind9[41];
	char f_od_ind10[41];
	char f_in_ind11[41];
	char f_scope12[41];
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

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0640",f_com_item.acc_no );
	get_zd_data("0750",f_com_item.acc_name );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_com_item.acc_no );
	if( strlen(f_com_item.acc_no) )
	{
		sprintf( tmpstr," acc_no='%s' and",f_com_item.acc_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_item.acc_name );
	if( strlen(f_com_item.acc_name) )
	{
		sprintf( tmpstr," acc_name like '%%%%%s%%%%' and",f_com_item.acc_name );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"com_item" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 order by acc_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"acc_no,acc_name,sub_acc_yn,acc_lvl,dc_ind,roll_ind,acc_knd,fore_ind,equa_ind,amt_dc_ind,od_ind,in_ind,scope");

	ret=Com_item_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Com_item_Fet_Sel( &vcom_item, g_pub_tx.reply );
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
			fprintf( fp,"~@��Ŀ��|@��Ŀ����|@�ӿ�Ŀ��־|@��Ŀ����|@�����־|@�����־|@��Ŀ����|@����ұ�־|@ƽ���־|@�������|@͸֧��־|@�ڲ���Ŀ��־|@���跶Χ|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_dic_show_str(f_sub_acc_yn2,"sub_acc_yn",vcom_item.sub_acc_yn);
		pub_base_dic_show_str(f_acc_lvl3,"acc_lvl",vcom_item.acc_lvl);
		pub_base_dic_show_str(f_dc_ind4,"accdc_ind",vcom_item.dc_ind);
		pub_base_dic_show_str(f_roll_ind5,"roll_ind",vcom_item.roll_ind);
		pub_base_dic_show_str(f_acc_knd6,"acc_knd",vcom_item.acc_knd);
		pub_base_dic_show_str(f_fore_ind7,"cur_ind",vcom_item.fore_ind);
		pub_base_dic_show_str(f_equa_ind8,"equa_ind",vcom_item.equa_ind);
		pub_base_dic_show_str(f_amt_dc_ind9,"accdc_ind",vcom_item.amt_dc_ind);
		pub_base_dic_show_str(f_od_ind10,"allow_ind",vcom_item.od_ind);
		pub_base_dic_show_str(f_in_ind11,"in_ind",vcom_item.in_ind);
		pub_base_dic_show_str(f_scope12,"scope",vcom_item.scope);

		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",vcom_item.acc_no,vcom_item.acc_name,f_sub_acc_yn2,f_acc_lvl3,f_dc_ind4,f_roll_ind5,f_acc_knd6,f_fore_ind7,f_equa_ind8,f_amt_dc_ind9,f_od_ind10,f_in_ind11,f_scope12);
		ttlnum++;
	}

	ret=Com_item_Clo_Sel( );
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
