/*********************************************************************************
***�� �� ��:  sp1713.c                                                         ***
***���������� ���˿ͻ����ϲ�ѯ                                                 ***
***                                                                            ***
***��    ��:  C K                                                       ***
***������ڣ� 2010-02-26                                                      ***
***                                                                            ***
***��    ¼�� �������Ӻ�����mdm_ac_rel���в�ѯ���ͻ��Ŀͻ��ţ�  ***
***           Ȼ����ݿͻ��ŵ������ͻ������Ϣ���в�ѯ���ͻ����������Ϣ       ***
**********************************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_wrk_hst_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_hold_c.h"

int sp1713()
{
	struct mdm_ac_rel_c st_mdm_ac_rel;
	struct dd_mst_c st_dd_mst;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct cif_email_inf_c st_cif_email_inf;
	struct cif_addr_inf_c st_cif_addr_inf;
	struct cif_per_inf_c st_cif_per_inf;
	struct cif_wrk_hst_inf_c st_cif_wrk_hst_inf;
	struct cif_id_code_rel_c st_cif_id_code_rel;
	struct mo_hold_c st_mo_hold;
	int iCif_no=0;
	int ret=0;
	
	memset( &g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset(&st_dd_mst,0x00,sizeof(st_dd_mst));
	memset(&st_mdm_ac_rel,0x00,sizeof(st_mdm_ac_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&st_cif_email_inf,0x00,sizeof(st_cif_email_inf));
	memset(&st_cif_addr_inf,0x00,sizeof(st_cif_addr_inf));
	memset(&st_cif_per_inf,0x00,sizeof(st_cif_per_inf));
	memset(&st_cif_wrk_hst_inf,0x00,sizeof(st_cif_wrk_hst_inf));
	memset(&st_cif_id_code_rel,0x00,sizeof(st_cif_id_code_rel));
	memset(&st_mo_hold, 0x00, sizeof(st_mo_hold));
		
	pub_base_sysinit();

	get_zd_data("0620",st_mdm_ac_rel.ac_no );		/*���Ӻ���*/
		
		/**�������Ӻ��ҿͻ�id**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	get_zd_data("0790",g_pub_tx.draw_pwd);
	/*vtcp_log( "%s,%d===g_pub_tx.draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	get_zd_data("0660",g_pub_tx.draw_pwd_yn);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd_yn[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
	if(g_pub_tx.draw_pwd_yn[0] != 'N' && 
	 strcmp(st_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
	{
          sprintf(acErrMsg, "�������!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "O123");
          goto ErrExit;
	}
    
	st_dd_mst.ac_id=st_mdm_ac_rel.ac_id;
	set_zd_data("0620",st_mdm_ac_rel.id_no);
	vtcp_log( "[%s][%d] id_no[%s]",__FILE__,__LINE__, st_mdm_ac_rel.id_no);
	set_zd_data("0670",st_mdm_ac_rel.id_type);
	vtcp_log( "[%s][%d] id_type[%s]",__FILE__,__LINE__, st_mdm_ac_rel.id_type);
	set_zd_double("0390",st_mdm_ac_rel.book_bal);
	vtcp_log( "[%s][%d] book_bal[%f]",__FILE__,__LINE__, st_mdm_ac_rel.book_bal);
	set_zd_data("0910",st_mdm_ac_rel.opn_br_no);	/**��������**/
	vtcp_log( "[%s][%d] ��������[%s]",__FILE__,__LINE__, st_mdm_ac_rel.opn_br_no);
	set_zd_long("0840",st_mdm_ac_rel.beg_date);		/**��������**/
	vtcp_log( "[%s][%d] ��������[%d]",__FILE__,__LINE__, st_mdm_ac_rel.beg_date);
	set_zd_data("0680",st_mdm_ac_rel.note_sts);		/**����״̬**/
	vtcp_log( "[%s][%d] ����״̬[%s]",__FILE__,__LINE__, st_mdm_ac_rel.note_sts);
	
	/**���ݿͻ�id�ҿͻ���**/
	ret=Dd_mst_Sel(g_pub_tx.reply,&st_dd_mst,"ac_id='%ld'", st_dd_mst.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		iCif_no=st_dd_mst.cif_no;
		vtcp_log( "[%s][%d] �ͻ��ţ�[%ld]",__FILE__,__LINE__,iCif_no);
		set_zd_long("0540",iCif_no);   /**�ͻ���**/
	}

	/**���ݿͻ��ŵ�cif_basic_inf���в�ѯ�ͻ������Ϣ**/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&st_cif_basic_inf," cif_no=%ld ",iCif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else if(ret==0)
		{
			set_zd_data("0260",st_cif_basic_inf.name);           /**�ͻ���**/
			set_zd_data("0340",st_cif_basic_inf.lvl);            /**�����ȼ�**/
		}
		/**���ݿͻ��ŵ�cif_per_inf���в�ѯ�ͻ������Ϣ**/
		ret=Cif_per_inf_Sel(g_pub_tx.reply,&st_cif_per_inf," cif_no=%ld ",iCif_no);
		if(ret==0)
		{
			set_zd_data("0230",st_cif_per_inf.sex);              /**�Ա�**/
			set_zd_long("0280",st_cif_per_inf.g_bir);            /**��������**/
			set_zd_double("0370",st_cif_per_inf.mon_income);     /**������**/
			set_zd_data("0240",st_cif_per_inf.marriage);         /**����״��**/
			vtcp_log( "[%s][%d] st_cif_per_inf.sex[%s]",__FILE__,__LINE__,st_cif_per_inf.sex);
			vtcp_log( "[%s][%d] st_cif_per_inf.g_bir[%ld]",__FILE__,__LINE__,st_cif_per_inf.g_bir);
			vtcp_log( "[%s][%d] st_cif_per_inf.mon_income[%f]",__FILE__,__LINE__,st_cif_per_inf.mon_income);
			vtcp_log( "[%s][%d] st_cif_per_inf.marriage[%s]",__FILE__,__LINE__,st_cif_per_inf.marriage);
		}

		/**���ݿͻ��ŵ�cif_addr_inf���в�ѯ�ͻ������Ϣ**/
		ret=Cif_addr_inf_Sel(g_pub_tx.reply,&st_cif_addr_inf," cif_no=%ld and addr_seqn=1",iCif_no);
		if (ret==0)
		{
			set_zd_data("0250",st_cif_addr_inf.addr);                /**��ͥסַ**/
			set_zd_data("0290",st_cif_addr_inf.post_code);           /**��ͥ�ʱ�**/
			vtcp_log( "[%s][%d] ��ͥסַst_cif_addr_inf.addr[%s]",__FILE__,__LINE__,st_cif_addr_inf.addr);
			vtcp_log( "[%s][%d] ��ͥ�ʱ�st_cif_addr_inf.post_code[%s]",__FILE__,__LINE__,st_cif_addr_inf.post_code);
		}
		/**���ݿͻ��ŵ�cif_email_inf���в�ѯ�ͻ������Ϣ**/
		ret=Cif_email_inf_Sel(g_pub_tx.reply,&st_cif_email_inf," cif_no=%ld and addr_seqn=1",iCif_no);
		if (ret==0)
		{
			set_zd_data("0300",st_cif_email_inf.email);  /**�ֻ�����**/
			vtcp_log( "[%s][%d] �ֻ�����st_cif_email_inf.email[%s]",__FILE__,__LINE__,st_cif_email_inf.email);
		}
		
		/**���ݿͻ��ŵ�cif_wrk_hst_inf���в�ѯ�ͻ������Ϣ**/
		ret=Cif_wrk_hst_inf_Sel(g_pub_tx.reply,&st_cif_wrk_hst_inf," cif_no=%ld ",iCif_no);
		if (ret==0)
		{
			set_zd_data("0270",st_cif_wrk_hst_inf.unit_name);   /**��λ����**/
			vtcp_log( "[%s][%d] ��λ����[%s]",__FILE__,__LINE__, st_cif_wrk_hst_inf.unit_name);
		}	
		ret=Mo_hold_Sel(g_pub_tx.reply, &st_mo_hold, "ac_id='%d' and hold_sts='0'", st_mdm_ac_rel.ac_id);
		if (ret==0)
		{
			set_zd_data("0690",st_mo_hold.hold_typ);   /**��������**/
			vtcp_log( "[%s][%d] ��������[%s]",__FILE__,__LINE__, st_mo_hold.hold_typ);
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
