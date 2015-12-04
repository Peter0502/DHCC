/*************************************************
* �� �� ��:  sp1716.c
* ���������� 4.7.2.7	֤����ѯ���ۺ�
*
* ��    ��: ���Ŀ� 
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
#include "mdm_ac_rel_c.h"
#include "mo_hold_c.h"

int sp1716()
{
	char filename[128];
	struct mdm_ac_rel_c stMdm_ac_rel;
	struct mo_hold_c		stMo_hold;
	
	int ttlnum=0;			/**��ȡ��������**/
	int i=0;
	int ret=0;
	FILE *fp;
	
	memset(filename, 0, sizeof(filename));
	memset( &stMdm_ac_rel,0,sizeof(stMdm_ac_rel));
	memset( &stMo_hold,0,sizeof(stMo_hold));
    
	pub_base_sysinit();
  get_zd_data("0660", stMdm_ac_rel.id_type);
	vtcp_log( "%s,%d===stMdm_ac_rel.id_type [%s]",__FILE__,__LINE__,stMdm_ac_rel.id_type);
	get_zd_data("0630", stMdm_ac_rel.id_no);
	vtcp_log( "%s,%d===stMdm_ac_rel.id_no[%s]",stMdm_ac_rel.id_no);
	
  /**��ɲ�ѯ����**/
	ret= Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"id_type='%s' and id_no='%s' and mdm_code in (select mdm_code from mdm_attr where mdm_type='1')",stMdm_ac_rel.id_type,stMdm_ac_rel.id_no);
	ERR_DEAL  
	while(1)
	{
		memset(&stMdm_ac_rel, 0, sizeof(stMdm_ac_rel));
		ret=Mdm_ac_rel_Fet_Sel( &stMdm_ac_rel, g_pub_tx.reply );
		if( ret==100 ) break;
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
			
			/**��ʾ�б���**/
			/**fprintf( fp,"~@��������|@����ʱ��|@ժ    Ҫ|$���׽��|@���|@��ת|$�������|@ƾ֤����|@ƾ֤����|@����������|��ϸ����|@����Ա|@����Ա|@��Ȩ��|��ˮ��|�ʴ�|@�ʺ�|���|\n" );
			**/
		/**fprintf( fp,"~@���ۺ�|@��������|@��������|@����״̬|@����״̬|\n" );
		**/
		}
		Mo_hold_Sel(g_pub_tx.reply, &stMo_hold, "ac_id='%d' and ac_seqn='%d' and hold_sts='0'", stMdm_ac_rel.ac_id, stMdm_ac_rel.ac_seqn);
   	fprintf(fp, "%-19s|%08d|%-5s|%-1s|%-1s|\n",stMdm_ac_rel.ac_no,
   	stMdm_ac_rel.beg_date,stMdm_ac_rel.opn_br_no,stMdm_ac_rel.note_sts,stMo_hold.hold_typ);
		ttlnum++;
		vtcp_log( "%s,%d,ttlnum [%d]", __FILE__, __LINE__, ttlnum );
	}
	ret=Mdm_ac_rel_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
GoodExit:
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
