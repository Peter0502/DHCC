/*********************************************************************************
***�� �� ��:  sp1717.c                                                         ***
***����������  ���ݿ��Ż�ÿͻ���Ϣ���˻���У��״̬                        ***
***                                                                            ***
***��    ��:  zhaoxingwen                                                      ***
***������ڣ� 2011-11-15                                                       ***
***                                                                            ***
***��    ¼�� ���շ�ϵͳ�����˴˳���������˸Ķ�������ϵ���Ϊ	       ***
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
#include "card.h"

int sp1717()
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
	char buf[64]={'\0'};
	char qury_lx[64]={'\0'};
	int iCif_no=0;
	int ret=0;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
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

	get_zd_data("0620",st_mdm_ac_rel.ac_no );		/*������*/
	get_zd_data("0270",qury_lx );		/*��ѯ����*/
	vtcp_log( "[%s][%d] ac_no[%s],qury_lx[%s]",__FILE__,__LINE__,st_mdm_ac_rel.ac_no,qury_lx);	
		/**�������Ӻ��ҿͻ�id**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	/*IC�����ײ��ں�������
	get_zd_data("0790",g_pub_tx.draw_pwd);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);
	get_zd_data("0660",g_pub_tx.draw_pwd_yn);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd_yn[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
	if(g_pub_tx.draw_pwd_yn[0] != 'N' && 
	 strcmp(st_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
	{
          sprintf(acErrMsg, "�������[%s]->[%s]!!",st_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
          WRITEMSG
          strcpy(g_pub_tx.reply, "O123");
          goto ErrExit;
	}
        **/
	st_dd_mst.ac_id=st_mdm_ac_rel.ac_id;
	st_dd_mst.ac_seqn=st_mdm_ac_rel.ac_seqn;
	set_zd_data("0200",st_mdm_ac_rel.id_type);/*֤������*/
	vtcp_log( "[%s][%d] ֤�����ࣺ[%s]",__FILE__,__LINE__,st_mdm_ac_rel.id_type);
	set_zd_data("0320",st_mdm_ac_rel.id_no);/*֤������*/
	vtcp_log( "[%s][%d] ֤�����룺[%s]",__FILE__,__LINE__,st_mdm_ac_rel.id_no);
	set_zd_data("0440",st_mdm_ac_rel.opn_br_no);/*����������*/
	vtcp_log( "[%s][%d] ����������[%s]",__FILE__,__LINE__,st_mdm_ac_rel.opn_br_no);
	/**�ж����ǵ����ֽ���˽��׵��ô˳����򲻹��˻�״̬�Ƿ�����Ҳ������ؿͻ���Ϣ 20131225 hzh**/
	if(memcmp(qury_lx,"dzxjtzcx",8) != 0)
	{
		if ('0' != st_mdm_ac_rel.note_sts[0])
		{
			sprintf(acErrMsg,"����״̬������!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D140");
			goto ErrExit;
		}
  	
		if ('0' != st_mdm_ac_rel.coll_sts[0])
		{
			sprintf(acErrMsg,"���ʴ��ڼ�ʰ״̬!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D141");
			goto ErrExit;
		}
	}
	
	/**���ݿͻ�id�ҿͻ���**/
	ret=Dd_mst_Sel(g_pub_tx.reply,&st_dd_mst,"ac_id='%ld'  ", st_dd_mst.ac_id );
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		/**�ж����ǵ����ֽ���˽��׵��ô˳����򲻹��˻�״̬�Ƿ�����Ҳ������ؿͻ���Ϣ 20131225 hzh**/
		if(memcmp(qury_lx,"dzxjtzcx",8) != 0)
		{
			if ('1' != st_dd_mst.ac_sts[0])
			{
				sprintf(acErrMsg,"�˻�״̬������!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"T999");
				goto ErrExit;
			}
			if (('1' == st_dd_mst.hold_sts[0] || '2' == st_dd_mst.hold_sts[0]) && memcmp(st_mdm_ac_rel.ac_no,ICCARDHEADCODE,6))/**����IC����621780���ڴ˴���ѯ���ܶ���״̬**/
			{
				sprintf(acErrMsg,"�˻�������ȫ�����ֻ������״̬!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L098");
				goto ErrExit;
			}

			if (('1' == st_dd_mst.hold_sts[0] || '2' == st_dd_mst.hold_sts[0] || '3' == st_dd_mst.hold_sts[0]) && memcmp(qury_lx,"agent_pt",8) == 0)/**����IC����621780���ڴ˴���ѯ���ܶ���״̬**/
			{
				sprintf(acErrMsg,"�˻�������ȫ�����ֻ������״̬!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L098");
				goto ErrExit;
			}
		}
		iCif_no=st_dd_mst.cif_no;
		vtcp_log( "[%s][%d] �ͻ��ţ�[%ld]",__FILE__,__LINE__,iCif_no);
		set_zd_long("0540",iCif_no);   /**�ͻ���**/
		set_zd_data("0260",st_dd_mst.name);/*����*/
		/**set_zd_double("1213",st_dd_mst.bal);**/
		st_dd_mst.bal*=100 ;
		sprintf(buf,"%.0f",st_dd_mst.bal);
		set_zd_data("0330",buf);/*���*100*/
		vtcp_log( "[%s][%d] ���*100��[%s]",__FILE__,__LINE__,buf);
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
		vtcp_log( "[%s][%d] Cif_basic_inf_Sel run well[%s]",__FILE__,__LINE__);
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
