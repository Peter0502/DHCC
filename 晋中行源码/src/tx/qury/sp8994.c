/*************************************************
* �� �� ��:  sp8994.c
* ���������� ί��Э����Ϣ��ѯ
*
* ��    ��:  ligl
* ������ڣ� 2006-09-28
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

int sp8994()
{
		char ac_no[25];	
		char br_no[6];
		char trust_no[21];
		int i=0;
		int ret=0;
		int ttlnum=0;
		FILE *fp;		
		memset(&g_pub_tx,0,sizeof(g_pub_tx));
		memset( ac_no,0,sizeof(ac_no) );
		memset( br_no,0,sizeof(br_no) );
		memset( trust_no,0,sizeof(trust_no));
		pub_base_sysinit();	
		get_zd_data("0860",br_no);
		get_zd_data("0310",ac_no);
		get_zd_data("0630",trust_no);
 		pub_base_strpack(ac_no);
 		pub_base_strpack(trust_no);
		if(strlen(ac_no))
		{
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�˺Ų�����" );
				WRITEMSG
				strcpy( g_pub_tx.reply,"M003" );
				goto ErrExit;
			}
			else if(ret)
			{
				sprintf(acErrMsg,"��ѯ����ac_no=[%s]ret=[%d]",ac_no,ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D103" );
				goto ErrExit;
			}
			ret = Ln_mst_Sel( g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d", \
						  g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�˺Ų�����" );
				WRITEMSG
				strcpy( g_pub_tx.reply,"L114" );
				goto ErrExit;
			}
			else if(ret)
			{
				sprintf(acErrMsg,"��ѯ����");
				WRITEMSG
				strcpy( g_pub_tx.reply,"D103" );
				goto ErrExit;
			}
			
  		ttlnum=sql_count("mo_trust_prot" ,"trust_no='%s' and ln_ac_id=%ld and ln_ac_seqn=%d and tx_br_no='%s'",trust_no,g_ln_mst.ac_id,g_ln_mst.ac_seqn,br_no);
    	vtcp_log("[%s][%d]ttlnum=====[%d]",__FILE__,__LINE__,ttlnum);
    	set_zd_int("0840",ttlnum);  
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
