/*********************************************************************************
***�� �� ��:  sp1715.c                                                         ***
***���������� ����Ϣ��ѯ                                                ***
***                                                                            ***
***��    ��:  wangwenkui                                                      ***
***������ڣ� 2010-12-14 17:23                                                 ***
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
#include "mo_loss_c.h"


char mapstat(char *stat);
int sp1715()
{
	struct mdm_ac_rel_c st_mdm_ac_rel;
	struct dd_mst_c st_dd_mst;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct mo_loss_c	st_mo_loss;
	char cCheckPWD[2];
	char cStat;
	int iCif_no=0;
	int ret=0;
	
	memset( &g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset(&st_dd_mst,0x00,sizeof(st_dd_mst));
	memset(&st_mdm_ac_rel,0x00,sizeof(st_mdm_ac_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&st_mo_loss, 0x00, sizeof(st_mo_loss));
	memset(cCheckPWD, 0, sizeof(cCheckPWD));	
	pub_base_sysinit();

	get_zd_data("0620",st_mdm_ac_rel.ac_no );		/*���Ӻ���*/
	get_zd_data("0690",cCheckPWD);		/**�Ƿ�鴦���룬1 - ��飬0 - �����**/
	
	/**�������Ӻ��ҿͻ�id**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	if(cCheckPWD[0] == '1')
	{/**��֤����**/
		get_zd_data("0790",g_pub_tx.draw_pwd);
		if( pub_base_DesChk(g_pub_tx.tx_date, st_mdm_ac_rel.ac_no, g_pub_tx.draw_pwd, g_mdm_ac_rel.draw_pwd) )
      {
          sprintf(acErrMsg,"���벻��!");
          WRITEMSG
          strcpy(g_pub_tx.reply,"M005");
          return 1;
      }
    }
	st_dd_mst.ac_id=st_mdm_ac_rel.ac_id;
	set_zd_data("0620",st_mdm_ac_rel.id_no);  /**֤������**/
	set_zd_data("0670",st_mdm_ac_rel.id_type); /**֤������**/
	cStat = mapstat(st_mdm_ac_rel.note_sts);
	set_zd_data("0690", &cStat);		/**����״̬**/
	set_zd_long("0450",st_mdm_ac_rel.beg_date);		/**��������**/
	set_zd_data("0730",st_mdm_ac_rel.opn_br_no);	/**����������**/
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
	ret=Mo_loss_Sel(g_pub_tx.reply,&st_mo_loss," ac_no='%s' and unloss_date != 0 ",st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		set_zd_data("0710","0");		/**��ʧ״̬**/
	}
	else if(ret==0)
	{
		if(st_mo_loss.loss_ind[0] == '1')
			{
				set_zd_data("0710","0");		/**��ʧ״̬**/
			}else 
			{
				set_zd_data("0710","0");		/**��ʧ״̬**/
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
char mapstat(char *stat)
{
	if(stat[0] == '*')
		{
			return '3';
		}else if(stat[0] == '0')
		{
			return '1';
		}else if(stat[0] == '1')
		{
			return '6';
		}else if(stat[0] == '2')
		{
			return '6';
		}else if(stat[0] == '3')
		{
			return '9';
		}else if(stat[0] == '4')
		{
			return '2';
		}else 
		{
			return '9';
		}
}
