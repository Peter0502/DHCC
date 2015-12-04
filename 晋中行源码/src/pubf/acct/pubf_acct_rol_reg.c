/***************************************************************
* �� �� ��:     pubf_acct_rol_reg.c
* ����������
**��־�÷���
*
* ��    ��:
* ������ڣ�    2003��11��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "tx_dc_br_rel_c.h"
#include "mdm_unprt_hst_c.h"
#include "mo_city_c.h"
#include "mo_ibl_c.h"
#include "mo_ibl_ret_c.h"
#include "mo_invest_c.h"
#include "mo_righ_invest_c.h"
#include "mbfe_pub.h"
#include "mo_stok_rebuy_prot_c.h"
#include "cash_book_c.h"
#include "mo_discnt_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "mo_sg_cash_po_bill_c.h"
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"
#include "com_sys_parm_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "mo_bank_split_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_bail_rate_c.h"
#include "mo_po_co_c.h"
#include "protect_note_c.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include "accountreg_c.h"
#include "ass_mst_c.h"
#include "ass_mst_hst_c.h"
#include "mo_int_tax_hst_c.h"
#include "cashier_c.h"  /**YHBP**/
/**do_mode����ʽ��1ɾ2��
***reg_no�Ǽǲ����
**/
int pub_acct_Rol_reg_sub( ptrace_log,do_mode,reg_no )
 struct trace_log_c *ptrace_log;
 char do_mode;
 int reg_no;
{
	int ret=0;

	sprintf( acErrMsg, "---------->��ʼ����ǼǼ�¼!!!" );
	WRITEMSG

	if( do_mode=='0' )
	{
		sprintf( acErrMsg,"������" );
		WRITEMSG
		goto OkExit;
	}
	vtcp_log("%s,%d regno==%d",__FILE__,__LINE__,reg_no);
	switch( reg_no )
	{
		case 2:
			/*֪ͨ�Ǽǲ�*/
			ret=Rol_mo_infrm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 3:
			/*��ʧ�Ǽǲ�*/
			ret=Rol_mo_loss( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 4:
			/*�����Ǽǲ�*/
			ret=Rol_mo_unchange( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 5:
			/*���޶�Ǽǲ�*/
			ret=Rol_card_consum_lmt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 6:
			/*����չ�ڵǼǲ�*/
			ret=Rol_ln_expan( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 7:
			/*����Э��*/
			ret=Rol_prdt_prot_parm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 8:
			/*����Э��Ǽǲ�*/
			ret=Rol_mo_prdt_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 9:
			/*����Э���˻����ձ�*/
			ret=Rol_mo_prot_ac_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 10:
			/*ί��Э��Ǽǲ�*/
			ret=Rol_mo_trust_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 11:
			/*͸֧����ƻ���*/
			ret=Rol_od_pay_pln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 12:
			/*Ǯ����ϸ*/
			ret=Rol_cash_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 13:
			/*֧Ʊ͸֧�Ǽǲ�*/
			ret=Rol_mo_cheq_od( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 14:
			/*���̿�Ǽǲ�*/
			ret=Rol_mo_rmny_smny( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 15:
			/*�ͻ�ƾ̨֤����ϸ*/
			ret=Rol_note_cheq_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 16:
			/*�ͻ�ƾ̨֤��*/
			ret=Rol_note_cheq_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 17:
			/*��Աƾ̨֤��*/
			ret=Rol_note_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 18:
			/*ƾ̨֤����ϸ*/
			ret=Rol_note_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 19:
			/*ƾ֤��С��*/
			ret=Rol_note_min_no( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 20:
			/*�ֶλ���*/
			ret=Rol_com_sect_acm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 21:
			/*�������ǼǱ�*/
			TRACE
			ret=Rol_mo_opn_cls( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 22:
			/*��ʰ�ǼǱ�*/
			ret=Rol_mo_coll( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 23:
			/*��֤�Ǽǲ�*/
			ret=Rol_mo_rpl_note( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 24:
			/*�ڲ�̨����ϸ*/
			ret=Rol_in_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 25:
			/*���ֵǼǱ�*/
			ret=Rol_mo_discnt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 26:
			/*���ֻ���ǼǱ�*/
			ret=Rol_mo_pay_discnt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 27:
			/*���ǼǱ�*/
			ret=Rol_mo_ibl( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 28:
			/*Ͷ�ʵǼǲ�*/
			ret=Rol_mo_invest( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 29:
			/*Ȩ����Ͷ�ʵǼǱ�*/
			ret=Rol_mo_righ_invest( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 30:
			/*֤ȯ�ع�Э��ǼǱ�*/
			ret=Rol_mo_stok_rebuy_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 31:
			/*���ҲֵǼǱ�*/
			ret=Rol_mo_sg_cash_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 32:
			/*���гжһ�Ʊ�ǼǱ�*/
			ret=Rol_mo_bank_acc_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 33:
			/*���л�Ʊ��ѯ�鸴�Ǽ�*/
			ret=Rol_mo_po_qry( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 34:
			/*���л�Ʊ�ǼǱ�*/
			ret=Rol_mo_bank_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 35:
			/*���б�Ʊ*/
			ret=Rol_mo_cashier( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 36:
			/*ͬ�ǵǼǱ�*/
			ret=Rol_mo_city( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 37:
			/*����ǼǱ�*/
			ret=Rol_mo_fax( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 38:
			/*�������д����*/
			ret=Rol_mo_fax_code( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 39:
			/*ί���嵥*/
			ret=Rol_mo_trust_list( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 40:
			/*ί���տ�ǼǱ�*/
			ret=Rol_mo_trust_pay( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 41:
			/*�ٴ���ǼǱ�*/
			ret=Rol_mo_again_ln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 42:
			/*�ٴ����ǼǱ�*/
			ret=Rol_mo_pay_again_ln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 43:
			/*�����ʣ���������*/
			ret=Rol_point_point_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 44:
			/*����ʹ����ϸ��*/
			ret=Rol_point_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 45:
			/*��Ʒ��������*/
			ret=Rol_prdt_point_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 46:
			/*��Ʒ����ʹ����ϸ��*/
			ret=Rol_prdt_point_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 47:
			/*��������*/
			ret=Rol_ass_gl( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 48:
			/*�������ļ�*/
			ret=Rol_ass_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 49:
			/*������ϸ*/
			ret=Rol_ass_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 50:
			/*���˷ֶμ�Ϣ��*/
			ret=Rol_ass_subs_intst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 51:
			/*��Ȩ������*/
			ret=Rol_auth_code( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 52:
			/*��Ϣ�Ǽǲ�*/
			ret=Rol_intst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 53:
			/*���ڴ����ϸ*/
			ret=Rol_dd_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 54:
			/*������ϸ*/
			ret=Rol_ln_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 55:
			/*͸֧�ʵ��ļ�*/
			ret=Rol_od_bill( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 56:
			/*͸֧��ϸ*/
			ret=Rol_od_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 57:
			/*���ڴ����ϸ*/
			ret=Rol_td_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 58:
			/*���Ҵ���Ƿ���*/
			ret=Rol_ln_lo( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 59:
			/*����ſ��ƻ���*/
			ret=Rol_ln_pay_pln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 60:
			/*�����˳���*/
			ret=Rol_ln_pay_seq( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 61:
			/*����Ǽǲ�*/
			ret=Rol_ln_reg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 62:
			/*�������˻���ϵ*/
			ret=Rol_mdm_ac_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 63:
			/*δ������ϸ��*/
			ret=Rol_mdm_unprt_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 64:
			/*�˻������ʷ*/
			ret=Rol_mo_ac_chg_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 65:
			/* �շ���ϸ */
			ret = Rol_com_chrg_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 66:
			/* �ͻ���Ʒ��ϵ�� */
			ret = Rol_cif_prdt_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 67:
			/*����Ǽǲ�*/
			ret=Rol_mo_hold( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 68:
			/*���֧�������ˮ��*/
			ret=Rol_Mbfeghvsendjrn( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 69:
			/*���֧�������嵥*/
			ret=Rol_Mbfeghvrecvlist( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 70:
			/*�ֽ��ո��Ǽǲ�*/
			ret=Rol_Cash_book( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 71: /*���Ҳֿ�������ǼǱ�*/
			ret=Rol_mo_sg_cash_po_bill( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 72: /*�����������*/
			ret=Rol_Mbfeghvsendlist( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 73: /*������˹���ҵ������*/
			ret=Rol_Mbfeghvrecvlist1( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 74: /*���ڻ�Ʊ�Ҹ�*/
			ret=Rol_Mbfebillopnreg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 79: /*��֤��ע��*/
			ret=Rol_mo_sg_protect_note( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 80: /*���ʺŴ�ȡ������*/
			ret=Rol_sub_dd_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 81: /*�жһ�Ʊ��ֵǼǱ�*/
			ret=Rol_mo_bank_split_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 82: /*��Ѻ�浥¼��*/
			ret=Rol_impa_dep_reg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 83: /*��֤���Ϣ�ǼǱ�*/
			ret=Rol_mo_bail_rate( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 84: /*�ͣɣӽ�ݺŶ��ձ�ǼǱ�*/
			ret=Rol_mo_po_co( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 85:/* ���Ǽǲ� */
			/** add by LiuHuafeng 20061107 **/
			{
				char cCmtno[4];
				char cTxday[9];
				char cOrderno[9];
				char cWssrno[7];
				memset(cCmtno  ,0,sizeof(cCmtno)-1);
				memset(cTxday  ,0,sizeof(cTxday)-1);
				memset(cOrderno,0,sizeof(cOrderno)-1);
				memset(cWssrno ,0,sizeof(cWssrno)-1);
				sprintf(cWssrno,"%-6d",ptrace_log->trace_no);
				get_zd_data(DC_DATE,cTxday);
				zip_space(cWssrno);
				vtcp_log("%s,%d  wssrno=[%s]",__FILE__,__LINE__,cWssrno);
				ret=iFindSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno);
				if(ret)
				{
					set_zd_data("0130","�ñ�ҵ���Ѿ����Ͳ���ȡ��");
					strncpy(g_pub_tx.reply,"P088",4);
					ERR_DEAL
					goto ErrExit;
				}
			}
			/** end by LiuHuafeng 20061107 **/
			ret=Rol_Hv_zf(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P088",4);
			ERR_DEAL
			break;
		case 86:/* ��Ʊ�Ǽǲ�ȡ�� */
			ret=Rol_Hv_Poinfo(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P085",4);
			ERR_DEAL;
			break;
		case 87:/* С����Ǽǲ� */
			ret=Rol_Lv_Pkgreg(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P085",4);
			ERR_DEAL;
			break;
		case 88:/* MBFE��֧����Ǽǲ� */
			/**
			ret=Rol_mbfe_fzfreg(ptrace_log, do_mode );
			**/
			ERR_DEAL;
			break;
    case 89:/**�����ʴ���ȡ���Ǽǲ� **/
      ret=Rol_Hv_zf_Errdeal(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 90:/**С����ʴ���ȡ���Ǽǲ�**/
      ret=Rol_Lv_pkgreg_Errdeal(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 91:/**��������ȡ���ֶ���Ϣ˰�Ǽǲ�������� */
      ret=Rol_Mo_int_tax_hst(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 92:/* ���б�Ʊǩ��ȡ�� add by xyy YHBP*/
      ret=Rol_Cashier(ptrace_log, do_mode );
      if(ret)
          strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;

    default:
	  break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/**
mo_hold            ����Ǽǲ�
mo_infrm           ֪ͨ�Ǽǲ�
mo_loss            ��ʧ�Ǽǲ�
mo_unchange        �����Ǽǲ�
card_consum_lmt    ���޶�Ǽǲ�
ln_expand          ����չ�ڵǼǲ�
prdt_prot_parm     ����Э��
mo_prdt_prot       ����Э��Ǽǲ�
mo_prot_ac_rel     ����Э���˻����ձ�
mo_trust_prot      ί��Э��Ǽǲ�
od_pay_pln         ͸֧����ƻ���
cash_mst_hst       Ǯ����ϸ
mo_cheq_od         ֧Ʊ͸֧�Ǽǲ�
mo_rmny_smny       ���̿�Ǽǲ�
note_cheq_hst      �ͻ�ƾ̨֤����ϸ
note_cheq_mst      �ͻ�ƾ̨֤��
note_mst           ��Աƾ̨֤��
note_mst_hst       ƾ̨֤����ϸ
note_min_no        ƾ֤��С��
com_sect_acm       �ֶλ���
mo_opn_cls         �������ǼǱ�
mo_coll            ��ʰ�ǼǱ�
mo_rpl_note        ��֤�Ǽǲ�
com_tel_connect    ��Ա���ӵǼǱ�
in_mst_hst         �ڲ�̨����ϸ
mo_discnt          ���ֵǼǱ�
mo_pay_discnt      ���ֻ���ǼǱ�
mo_ibl             ���ǼǱ�
mo_invest          Ͷ�ʵǼǲ�
mo_righ_invest     Ȩ����Ͷ�ʵǼǱ�
mo_stok_rebuy_prot ֤ȯ�ع�Э��ǼǱ�
mo_sg_cash_po      ���ҲֵǼǱ�
mo_bank_acc_po     ���гжһ�Ʊ�ǼǱ�
mo_po_qry          ���л�Ʊ��ѯ�鸴�Ǽ�
mo_bank_po         ���л�Ʊ�ǼǱ�
mo_cashier         ���б�Ʊ
mo_city            ͬ�ǵǼǱ�
mo_fax             ����ǼǱ�
mo_fax_code        �������д����
mo_trust_list      ί���嵥
mo_trust_pay       ί���տ�ǼǱ�
mo_again_ln        �ٴ���ǼǱ�
mo_pay_again_ln    �ٴ����ǼǱ�
point_point_mst    �����ʣ���������
point_mst_hst      ����ʹ����ϸ��
prdt_point_mst     ��Ʒ��������
prdt_point_hst     ��Ʒ����ʹ����ϸ��
ass_gl             ��������
ass_mst            �������ļ�
ass_mst_hst        ������ϸ
ass_subs_intst     ���˷ֶμ�Ϣ��
auth_code          ��Ȩ������
intst_hst          ��Ϣ�Ǽǲ�
dd_mst             ���ڴ�����ļ�
dd_mst_hst         ���ڴ����ϸ
ln_mst             �������ļ�
ln_mst_hst         ������ϸ
od_mst             ͸֧���ļ�
od_bill            ͸֧�ʵ��ļ�
od_mst_hst         ͸֧��ϸ
td_mst             ���ڴ�����ļ�
td_mst_hst         ���ڴ����ϸ
ln_lo              ���Ҵ���Ƿ���
ln_pay_pln         ����ſ��ƻ���
ln_pay_seq         �����˳���
ln_reg             ����Ǽǲ�
mdm_ac_rel         �������˻���ϵ
mdm_prdt_rel       ���ʲ�Ʒ��ϵ��
mdm_unprt_hst      δ������ϸ��
mo_ac_chg_hst      �˻������ʷ
com_chrg_hst	   �շ���ϸ
cash_book		   �ֽ��ո��Ǽǲ�
protect_note       ��֤���ʻ�
*/
/*֪ͨ�Ǽǲ�*/
static int Rol_mo_infrm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��ʧ�Ǽǲ�*/
static int Rol_mo_loss( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�����Ǽǲ�*/
static int Rol_mo_unchange( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���޶�Ǽǲ�*/
static int Rol_card_consum_lmt( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����չ�ڵǼǲ�*/
static int Rol_ln_expan( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����Э��*/
static int Rol_prdt_prot_parm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����Э��Ǽǲ�*/
static int Rol_mo_prdt_prot( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����Э���˻����ձ�*/
static int Rol_mo_prot_ac_rel( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ί��Э��Ǽǲ�*/
static int Rol_mo_trust_prot( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*͸֧����ƻ���*/
static int Rol_od_pay_pln( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*Ǯ����ϸ*/
static int Rol_cash_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*֧Ʊ͸֧�Ǽǲ�*/
static int Rol_mo_cheq_od( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���̿�Ǽǲ�*/
static int Rol_mo_rmny_smny( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '1':
			ret=sql_execute( "delete from mo_rmny_smny where tx_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�ͻ�ƾ̨֤����ϸ*/
static int Rol_note_cheq_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�ͻ�ƾ̨֤��*/
static int Rol_note_cheq_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��Աƾ̨֤��*/
static int Rol_note_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ƾ̨֤����ϸ*/
static int Rol_note_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ƾ֤��С��*/
static int Rol_note_min_no( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
static int Rol_com_sect_acm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�������ǼǱ�*/
static int Rol_mo_opn_cls( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	sprintf(acErrMsg,"���뿪�����Ǽǲ�[%c]",do_mode);
	WRITEMSG
	switch(do_mode)
	{
		case '1':  /**���������ļ�¼**/
			ret=sql_execute("delete from mo_opn_cls where ac_no='%s' and ac_seqn=%d and wrk_sts='1' ",
				ptrace_log->ac_no,ptrace_log->ac_seqn );
			ERR_DEAL
			break;
		case '2':  /**���������ļ�¼**/
			ret=sql_execute("delete from mo_opn_cls where ac_no='%s' and ac_seqn=%d and wrk_sts='2' ",
				ptrace_log->ac_no,ptrace_log->ac_seqn );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��ʰ�ǼǱ�*/
static int Rol_mo_coll( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '0':
			sprintf( acErrMsg,"������" );
			WRITEMSG
			break;
		default:
			ret=sql_execute( "delete from mo_coll where tx_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��֤�Ǽǲ�*/
static int Rol_mo_rpl_note( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '1':
			ret=sql_execute( "delete from mo_rpl_note where rpl_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�ڲ�̨����ϸ*/
static int Rol_in_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ֵǼǱ�*/
static int Rol_mo_discnt( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mo_discnt_c	Tmp_mo_discnt;
	int	ret=0;
	int i=0;

	switch(do_mode)
	{
		case '1':		/** ¼�볷�� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"pact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��ǵǼ�״̬�����ȳ����������˽���![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			/***
			if( Tmp_mo_discnt.pn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"�ǽ���¼������ֽ��ף����ܳ���![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			****/
			ret=sql_execute( "delete from mo_discnt where pact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case '2':		/** �������� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='2' )
			{
				sprintf(acErrMsg,"���ֵǼǱ����Ѿ�����״̬�����ȳ������˽���![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B279" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '3':		/** ���ּ��˳��� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='3' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��Ǽ���״̬����������![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"2");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '4':		/** ���ֻ���� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			vtcp_log("%s %d ��־�еĽ��===[%f]",__FILE__,__LINE__,ptrace_log->amt);
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '5':		/** ת���ּ��˳��� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='3' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��Ǽ���״̬����������![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '6':		/** ���ת����ת��¼�볷�� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"note_type='%s' and pnote_no='%s' ",ptrace_log->note_type,ptrace_log->note_no);
			ERR_DEAL
			for( i=1;i<3;i++ )
			{
				ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
				ERR_DEAL
				pub_base_strpack(Tmp_mo_discnt.pact_no);
				pub_base_strpack(ptrace_log->ac_no);
				if( !strcmp(Tmp_mo_discnt.pact_no,ptrace_log->ac_no) )	/*** ԭת������ֵļ�¼ ***/
				{
					if( Tmp_mo_discnt.sts[0]!='*' )
					{
						sprintf(acErrMsg,"���ֵǼǱ��Ǽ���״̬����������![%s]",Tmp_mo_discnt.sts);
						WRITEMSG
						strcpy( g_pub_tx.reply,"B280" );
						goto ErrExit;
					}
					strcpy(Tmp_mo_discnt.sts,"3");
					ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
					ERR_DEAL
				}
				if( strlen(Tmp_mo_discnt.pact_no)==0 )
				{
					ret=Mo_discnt_Del_Upd(Tmp_mo_discnt,g_pub_tx.reply);
					ERR_DEAL
				}
			}
			Mo_discnt_Clo_Upd();
			break;
		case '7':		/** ���ת����ת�����˳��� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='*' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��Ǽ���״̬����������![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			Mo_discnt_Clo_Upd();
			break;
		case '8':		/** �ع�ʽת����ת��¼�볷�� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"tpact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��ǵǼ�״̬�����ȳ����������˽���![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			if( Tmp_mo_discnt.tpn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"�ǽ���¼������ֽ��ף����ܳ���![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			ret=sql_execute( "delete from mo_discnt where tpact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case '9':		/** ת����ת�����ڻع������ **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply," tpact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case 'A':		/** �ع�ʽ��������Ϣ¼�� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"rpact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"���ֵǼǱ��ǵǼ�״̬�����ȳ����������˽���![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			if( Tmp_mo_discnt.rtpn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"�ǽ���¼������ֽ��ף����ܳ���![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			ret=sql_execute( "delete from mo_discnt where rpact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case 'B':		/** �����ֻع����� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply," rpact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case 'C':		/** ���ʽ�����ּ��˳��� **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
		/*
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
		*/
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='*' )
			{
				sprintf(acErrMsg,"���ֵǼǱ�������״̬����������![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ֻ���ǼǱ�*/
static int Rol_mo_pay_discnt( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ǼǱ�*/
static int Rol_mo_ibl( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;
	 struct mo_ibl_c sMo_ibl;
	 struct mo_ibl_ret_c sMo_ibl_ret;

	 switch(do_mode)
	 {
		case '1': /* ͬҵ����/������˳��� */
			memset(&sMo_ibl, 0x0, sizeof(struct mo_ibl_c));
			ret = Mo_ibl_Sel(g_pub_tx.reply, &sMo_ibl, "ac_id=%ld",
				ptrace_log->ac_id);
			ERR_DEAL

			if (pub_base_CompDblVal(sMo_ibl.amt, sMo_ibl.no_pay_bal) != 0)
			{
				sprintf(acErrMsg, "�ñʲ����Ѿ����ֻ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"S057");
				goto ErrExit;
			}

			if (sMo_ibl.sign[0] == '1')
			{
				ret = sql_count("mo_ibl_ret", "pact_no='%s'", sMo_ibl.pact_no);
				if (ret < 0)
				{
					sprintf(acErrMsg, "sql_count error !! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				if (ret > 0)
				{
					sprintf(acErrMsg, "�ʽ�Ӫҵ���Ѿ�¼�뻹����Ϣ!!", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "S058");
					goto ErrExit;
				}
			}

			ret=sql_execute("update mo_ibl set sts='1',ac_id=0, opn_date=0 \
				where ac_id=%ld", ptrace_log->ac_id );
			ERR_DEAL
			break;
		case '2': /* ͬҵ���뻹�� */
			memset(&sMo_ibl, 0x0, sizeof(struct mo_ibl_c));
			memset(&sMo_ibl_ret, 0x0, sizeof(struct mo_ibl_ret_c));

			ret = Mo_ibl_Sel(g_pub_tx.reply, &sMo_ibl, "ac_id=%ld",
				ptrace_log->ac_id);
			ERR_DEAL

			ret = Mo_ibl_ret_Dec_Upd(g_pub_tx.reply, "pact_no='%s' and \
				ret_amt=%lf and sts='2'", sMo_ibl.pact_no, ptrace_log->amt);
			ERR_DEAL

			ret = Mo_ibl_ret_Fet_Upd(&sMo_ibl_ret, g_pub_tx.reply);
			ERR_DEAL

			strcpy(sMo_ibl_ret.sts, "1");

			ret = Mo_ibl_ret_Upd_Upd(sMo_ibl_ret, g_pub_tx.reply);
			ERR_DEAL

			Mo_ibl_ret_Clo_Upd();

			ret=sql_execute("update mo_ibl set sts='2', \
				no_pay_bal=no_pay_bal+%lf where ac_id=%ld",
				ptrace_log->amt, ptrace_log->ac_id);
			ERR_DEAL
			break;
		case '3': /* ������� */
			ret=sql_execute("update mo_ibl set sts='2', \
				no_pay_bal=no_pay_bal+%lf where ac_id=%ld",
				ptrace_log->amt, ptrace_log->ac_id);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

/*Ͷ�ʵǼǲ�*/
static int Rol_mo_invest( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_invest_c	sMo_invest;
	memset(&sMo_invest,0x00,sizeof(struct mo_invest_c));

	if( do_mode=='1' )		/*** ɾ�� ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** �޸� ***/
	{
		if( !strncmp("4602",ptrace_log->sub_tx_code,4) )	/** ծȯͶ�ʼ��� **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )	goto OkExit;
			ERR_DEAL
			if( pub_base_CompDblVal(sMo_invest.sol_amt,0.00)!=0
				|| sMo_invest.sts[0]!='2' )
			{
				vtcp_log("sts=[%s],[%d]",sMo_invest.sts,pub_base_CompDblVal(sMo_invest.sol_amt,0.00));
				sprintf(acErrMsg,"�ñ�ծȯͶ�������ծȯ�Ѿ�����,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			strcpy(sMo_invest.sts,"1");
			sMo_invest.ac_id=0;
			sMo_invest.overflow_ac_id=0;
			sMo_invest.abate_ac_id=0;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->tx_code,"4603") )	/** ծȯ��Ϣ **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.pre_intst = sMo_invest.pre_intst + ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"4604") )	/** ծȯ���� **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto AAAAA;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			strcpy(sMo_invest.sts,"2");
			sMo_invest.par_bal = sMo_invest.par_bal + ptrace_log->amt;
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
			goto OkExit;

AAAAA:		ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"abate_ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto BBBBB;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.abate_amt = sMo_invest.abate_amt + ptrace_log->amt;
			strcpy(sMo_invest.sts,"2");
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
			goto OkExit;

BBBBB:		ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"overflow_ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.overflow_amt = sMo_invest.overflow_amt + ptrace_log->amt;
			strcpy(sMo_invest.sts,"2");
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"A016") && !strcmp(ptrace_log->tx_code,"4604") )
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.sol_amt = sMo_invest.sol_amt - ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"A017") && !strcmp(ptrace_log->tx_code,"4604") )
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"�ñ�ծȯͶ��̨��״̬������,�޷�����!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.pre_intst = sMo_invest.pre_intst + ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
	}
	else
	{
		sprintf(acErrMsg,"�������̶������[%c]",do_mode);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S054" );
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*Ȩ����Ͷ�ʵǼǱ�*/
static int Rol_mo_righ_invest( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_righ_invest_c	sMo_righ_invest;
	memset(&sMo_righ_invest,0x00,sizeof(struct mo_righ_invest_c));

	if( do_mode=='1' )		/*** ɾ�� ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** �޸� ** Ȩ����Ͷ�ʼ��� ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( pub_base_CompDblVal(sMo_righ_invest.amt,ptrace_log->amt)!=0 || sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s],[%lf][%lf]",sMo_righ_invest.sts,sMo_righ_invest.amt,ptrace_log->amt);
			sprintf(acErrMsg,"�ñʹ�ȨͶ���Ѿ�����,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		strcpy(sMo_righ_invest.sts,"1");
		sMo_righ_invest.ac_id=0;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='3' && !strcmp(ptrace_log->tx_code,"4607") )	/*** �޸� ** Ȩ����Ͷ�ʼ��� ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"�ñʹ�ȨͶ���Ѿ�����,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.incm_bal=sMo_righ_invest.incm_bal-ptrace_log->amt;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='3' && !strcmp(ptrace_log->tx_code,"4608") )	/*** �޸� ** Ȩ����Ͷ������ ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"�ñʹ�ȨͶ���Ѿ�����,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.incm_bal=sMo_righ_invest.incm_bal-ptrace_log->amt;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='4' && !strcmp(ptrace_log->sub_tx_code,"4608") )	/*** �޸� ** Ȩ����Ͷ�ʳ��� ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld ",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"�ñʹ�ȨͶ���Ѿ�����,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.amt=sMo_righ_invest.amt+ptrace_log->amt;
		sMo_righ_invest.wrk_date=ptrace_log->tx_date;
		sMo_righ_invest.trace_no=ptrace_log->trace_no;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*֤ȯ�ع�Э��ǼǱ�*/
static int Rol_mo_stok_rebuy_prot( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_stok_rebuy_prot_c	sMo_stok_rebuy_prot;
	memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));

	if( do_mode=='1' )		/*** ɾ�� ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** �޸� ** ֤ȯ�ع����������� ***/
	{
		ret=Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if(pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,ptrace_log->amt)!=0||sMo_stok_rebuy_prot.sts[0]!='2' )
		{
			vtcp_log("sts=[%s],[%lf][%lf]",sMo_stok_rebuy_prot.sts,sMo_stok_rebuy_prot.bal,ptrace_log->amt);
			sprintf(acErrMsg,"�ñ�֤ȯ�ع�̨��״̬����ȷ,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P470" );
			goto ErrExit;
		}
		strcpy(sMo_stok_rebuy_prot.sts,"1");
		sMo_stok_rebuy_prot.ac_id=0;
		ret=Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
		ERR_DEAL
		Mo_stok_rebuy_prot_Clo_Upd();
	}
	else if( do_mode=='3' )	/*** �޸� ** ���������� ***/
	{
		ret=Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if(sMo_stok_rebuy_prot.sts[0]!='*' && sMo_stok_rebuy_prot.sts[0]!='3' )
		/* 3 ��״̬������δ�ջ� */
		{
			vtcp_log("sts=[%s]",sMo_stok_rebuy_prot.sts);
			sprintf(acErrMsg,"�ñ�֤ȯ�ع�̨��״̬����ȷ,�޷�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P470" );
			goto ErrExit;
		}
		strcpy(sMo_stok_rebuy_prot.sts,"2");
		sMo_stok_rebuy_prot.bal=sMo_stok_rebuy_prot.bal+ptrace_log->amt;
		ret=Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
		ERR_DEAL
		Mo_stok_rebuy_prot_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ҲֵǼǱ�*/
static int Rol_mo_sg_cash_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_sg_cash_po_c	T_mo_sg_cash_po;
	memset(&T_mo_sg_cash_po,0x00,sizeof(struct mo_sg_cash_po_c));

	switch( do_mode )
	{
		case '1' :	/*** ��֤����� ***/
			ret=Mo_sg_cash_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_Fet_Upd(&T_mo_sg_cash_po,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_sg_cash_po_Clo_Upd();
				break;
			}
			ERR_DEAL

			if( ptrace_log->add_ind[0]=='1' )	/** �ɴ汣֤�� **/
			{
				ret=Mo_sg_cash_po_Del_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else if( ptrace_log->note_type[0]=='4' )	/** ���Ҳ� ***/
			{
				T_mo_sg_cash_po.safeg_sum=T_mo_sg_cash_po.safeg_sum-ptrace_log->amt;
				ret=Mo_sg_cash_po_Upd_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_sg_cash_po_Clo_Upd();
			break;
		case '2' :	/*** ����������� ***/
			ret=Mo_sg_cash_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_Fet_Upd(&T_mo_sg_cash_po,g_pub_tx.reply);
			ERR_DEAL
			T_mo_sg_cash_po.pickup_sum=T_mo_sg_cash_po.pickup_sum-ptrace_log->amt;
			if( pub_base_CompDblVal(T_mo_sg_cash_po.pickup_sum,0.00)<0 )
			{
				sprintf(acErrMsg,"������ǼǱ���������Ϊ�������ܳ���!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"B284" );
				goto ErrExit;
			}
			ret=Mo_sg_cash_po_Upd_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_sg_cash_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���гжһ�Ʊ�ǼǱ�*/
static int Rol_mo_bank_acc_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_acc_po_c	T_mo_bank_acc_po;
	memset(&T_mo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));

	switch( do_mode )
	{
		case '1' :	/*** ��֤����� ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Debug(&T_mo_bank_acc_po);
			if( ptrace_log->add_ind[0]=='1' )	/** �ɴ汣֤�� **/
			{
				vtcp_log("kkkkkkkkkkkkkkkkkkkkkkk\n");
				if( T_mo_bank_acc_po.acc_po_sts[0]!='A' )
				{
					sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬����[%s]",T_mo_bank_acc_po.acc_po_sts);
					WRITEMSG
					strcpy( g_pub_tx.reply,"B281" );
					goto ErrExit;
				}
				ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else 								/** ���ɱ�֤�� **/
			{
				T_mo_bank_acc_po.bail_amt=T_mo_bank_acc_po.bail_amt-ptrace_log->amt;
				ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_bank_acc_po_Clo_Upd();
			break;
		case '2' :	/*** �ж�Э��Ǽǳ��� ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='0' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬������״̬[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			if( ptrace_log->add_ind[0]=='1' )
			{
				ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else
			{
				strcpy(T_mo_bank_acc_po.acc_po_sts,"A");
				ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '3' :	/*** �ж�Э���������� ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬������״̬[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_acc_po.acc_po_sts,"0");
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '4' :	/*** �ж�Э��ǩ������ ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and acc_po_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			strcpy(T_mo_bank_acc_po.acc_po_sts,"1");
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '5' :	/*** ���Ʊ�ݹ����� ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='' and bill_ac_no='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬����[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			break;
		case '6' :	/*** ��֤��֧ȡ���� ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' and acc_po_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			T_mo_bank_acc_po.bail_amt=T_mo_bank_acc_po.bail_amt+ptrace_log->amt;
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���л�Ʊ��ѯ�鸴�Ǽ�*/
static int Rol_mo_po_qry( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���л�Ʊ�ǼǱ�*/
static int Rol_mo_bank_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_po_c	T_mo_bank_po;
	memset(&T_mo_bank_po,0x00,sizeof(struct mo_bank_po_c));

	switch( do_mode )
	{
		case '1' :	/*** �жһ�Ʊǩ������ ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬����[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_po_Del_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			/*** ��Ʊ״̬��Ϊ����״̬ ***/
			break;
		case '2' :	/*** �жһ�Ʊ���ճ��� ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='4' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬������״̬[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"1");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		case '3' :	/*** �жһ�Ʊ�������� ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='2' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬�Ǻ���״̬[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"4");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		case '4' :	/*** �жһ�Ʊ��Ʊ���� ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='3' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬����Ʊ״̬[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B302" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"1");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->brf);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�жһ�ƱЭ��ǼǱ�״̬����[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_po_Del_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���б�Ʊ*/
static int Rol_mo_cashier( struct trace_log_c *ptrace_log, char do_mode )
{
	pub_base_strpack(ptrace_log->ac_no);
	switch( do_mode )
	{
		case '1':	/*��Ʊ��Ϣ¼�볷��*/
			g_reply_int=sql_execute("delete from mo_cashier where pact_no='%s' and cashier_sts='0'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		case '2':	/*��Ʊǩ������*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='0',cashier_no=NULL,sign_date=0,sign_trace_no=0,sign_tel=NULL,sign_br_no=NULL where cashier_no='%s'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		case '3':	/*��Ʊ���峷��*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='1',use_ind='0',cash_amt=0,rem_amt=0,pay_ac_no=NULL,rec_ac_no=NULL,rem_ac_no=NULL,can_date=0,can_trace_no=0 where cashier_no='%s'",
				ptrace_log->note_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			if( ptrace_log->ct_ind[0] == '1' )    /*ͬ�����뷽ʽ����,�賷��ͬ�ǵǼǲ�*/
			{
				g_reply_int=sql_execute( "update mo_city set sts='C' where payer_ac_no='%s' and bill_amt='%lf' and sts='O'",
					ptrace_log->ac_no,ptrace_log->amt );
				if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			}
			break;
		case '4':	/*��Ʊ�˻س���*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='1',use_ind='0',cash_amt=0,rem_amt=0,pay_ac_no=NULL,rec_ac_no=NULL,rem_ac_no=NULL,can_date=0,can_trace_no=0 where cashier_no='%s'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		default :
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ͬ�ǵǼǱ� add by liuxuan 2006-10-12 */
static int Rol_mo_city( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct accountreg_c accountreg;
/*
	if(do_mode!='1'){
		vtcp_log("��ϵͳ��ǰ������׶�ͬ�ǵǼǲ�����,����ֻ��ͬ�ǽ��״���\n",__FILE__,__LINE__);
		return(0);
	}
*/
	if(memcmp(ptrace_log->sub_tx_code,"B901",4)!=0){
		vtcp_log("��ϵͳ��ǰ������׶�ͬ�ǵǼǲ�����,����ֻ��ͬ�ǽ��״���\n",__FILE__,__LINE__);
		return(0);
	}
		vtcp_log("�����ͬ�ǵǼǲ�����\n",__FILE__,__LINE__);
	memset(&accountreg, 0x0, sizeof(struct accountreg_c));
	vtcp_log("Ʊ����Ϣtx_date=%ld and trace_no=%ld and br_no=%s accountreg.stat[%s]",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat);
/**
	ret = Accountreg_Sel(g_pub_tx.reply, &accountreg, "tx_date=%ld and \
		trace_no=%ld and br_no='%s'",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no);
***/
	ret = Accountreg_Sel(g_pub_tx.reply, &accountreg, "tx_date=%ld and \
		trace_no=%ld ",ptrace_log->tx_date, ptrace_log->trace_no);
	vtcp_log("Ʊ����Ϣtx_date=%ld and trace_no=%ld and br_no=%s accountreg.stat[%s]",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "ͬ�ǵǼǲ���û�еǼǸñ�ҵ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "D122");
				goto ErrExit;
			}
			if (accountreg.stat[0] != '2' && accountreg.stat[0] != '3')
			{
				sprintf(acErrMsg, "�ñ�Ʊ��trace_no[%ld],br_no[%s]���Ǽ��ʻ򸴺�״̬[%s]���ܳ���!!,ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat");
				WRITEMSG
				strcpy(g_pub_tx.reply, "P004");
				goto ErrExit;
			}
			ret = sql_execute("update accountreg set stat='9' where trace_no = %ld and \
				tx_date=%ld ", \
				ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
OkExit:
	return 0;
ErrExit:
	return 1;
}
/* ��ǰ��ͬ�����ڲ��������Ը��˸��� _ori */
static int Rol_mo_city_ori( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_city_c sMo_city;

	switch(do_mode)
	{
		case '1': /* ���ȡ��� D096*/
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_date=%ld and \
				tx_no=%ld",ptrace_log->tx_date,ptrace_log->trace_no);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��![%ld]",ptrace_log->trace_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
				break;
			}
			if (sMo_city.sts[0] != '0')
			{
				sprintf(acErrMsg, "�ñ�Ʊ��״̬��������Ǽ�״̬���ܳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O244");
				goto ErrExit;
			}
			ret=sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '2': /* �ֹ�¼�볷�� D211*/
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_date=%ld and \
				tx_no=%ld",ptrace_log->tx_date, ptrace_log->trace_no);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}
			if (sMo_city.sts[0] != '1')
			{
				sprintf(acErrMsg, "�ñ�Ʊ�ݲ���¼��״̬���ܳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O245");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='0',tx_no=%ld, \
				payee_ac_no='', bill_type='' where tx_no = %ld and \
				tx_date=%ld ", \
				ptrace_log->ac_id,ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '3': /* �ֹ������ӡ���� D212 */
			ret = sql_count("mo_city", "bat_no=%ld and sts='3' and \
				in_out_ind='1' and trans_mode='1'", ptrace_log->ac_seqn);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�ƴ���!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "�����κ�Ʊ�������Ѿ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D230");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='1', pr_cnt=0 ,\
				bat_no=0 where bat_no=%ld and sts='2' and in_out_ind='1' and \
				trans_mode='1'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '4': /* �ֹ������������ */
			/* �ý��ײ��ܳ��� */
			strcpy(g_pub_tx.reply, "S053");
			goto ErrExit;
			ret = sql_execute("update mo_city set sts='2'  where \
				bat_no=%ld and sts='3'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '5': /* �Զ����¼�볷�� */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}
			if (sMo_city.sts[0] != '6')
			{
				sprintf(acErrMsg, "�ñ�Ʊ���Ѿ���ӡ���ܳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O246");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='0',tx_no=%ld, \
				payee_ac_no='', bill_type='',bat_no=0 where tx_no = %ld \
				and tx_date=%ld ",
				ptrace_log->ac_seqn,ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '6': /* �ֹ�/�Զ�����¼�볷�� */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.sts[0] != 'A' && sMo_city.sts[0] != 'H')
			{
				sprintf(acErrMsg, "�ñ�Ʊ�ݲ���¼��״̬���ܳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O244");
				goto ErrExit;
			}

			ret = sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '7': /* �ֹ������ӡ���� */
			ret = sql_count("mo_city", "bat_no>=%ld and bat_no<=%ld \
				and sts in ('C', 'O','Q') and in_out_ind='2' and \
				trans_mode='1'", ptrace_log->ac_seqn, ptrace_log->svc_ind);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�ƴ���!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "�˴δ�ӡ��Ʊ����״̬����'B'�Ĳ��ܳ���!!\
					[%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "S053");
				goto ErrExit;
			}

			ret = sql_execute("update mo_city set sts='A', pr_cnt=0 ,\
				bat_no=0 where bat_no>=%ld  and bat_no <= %ld and \
				sts='B' and in_out_ind='2' and trans_mode='1'",
				ptrace_log->ac_seqn, ptrace_log->svc_ind);
			ERR_DEAL
			break;
		case '8': /* �ֹ�������� */
			ret = sql_count("mo_city", "trans_mode='1' and in_out_ind='2' \
				and bat_no=%ld and sts in ('O','Q')",
				ptrace_log->ac_seqn);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�ƴ���!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "�˴κ�����Ʊ�����Ѿ����ʻ���Ʊ�Ĳ��ܳ���\
					[%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "S053");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='B'  where \
				in_out_ind='2' and trans_mode='1' and bat_no=%ld and \
				sts='C'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '9': /* ����������� D251 D256(�跽) */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				sts='O' and tx_date=%8.0lf", ptrace_log->ac_id,
				ptrace_log->amt);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯ���׼�¼����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.trans_mode[0] == '1')
			{
				ret = sql_execute("update mo_city set sts='C' where \
					tx_no=%ld and sts='O' and tx_date=%8.0f ", \
					ptrace_log->ac_id,ptrace_log->amt);
				ERR_DEAL
			}
			else
			{
				ret = sql_execute("update mo_city set sts='I' where \
					tx_no=%ld and sts='O' and tx_date=%8.0f ", \
					ptrace_log->ac_id,ptrace_log->amt);
				ERR_DEAL
			}
			break;
		case 'A': /* ����������Ʊ */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ", ptrace_log->trace_no, ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯ���׼�¼����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.sts[0] != '1' && sMo_city.sts[0] != '6')
			{
				sprintf(acErrMsg, "����Ʊ����¼��״̬���ܳ���!! [%s]",
					sMo_city.sts);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D231");
				goto ErrExit;
			}

			ret  = sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL

			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				sts='Q' and tx_date=%8.0lf ", ptrace_log->ac_id, \
				ptrace_log->amt);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯ���׼�¼����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "û�и���Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.trans_mode[0] == '1')
			{
				ret = sql_execute("update mo_city set sts='C' where \
					tx_no=%ld and sts='Q' and tx_date=%8.0lf",
					ptrace_log->ac_id, ptrace_log->amt);
				ERR_DEAL
			}
			else
			{
				ret = sql_execute("update mo_city set sts='I' where \
					tx_no=%ld and sts='Q' and tx_date=%8.0lf",
					ptrace_log->ac_id, ptrace_log->amt);
				ERR_DEAL
			}
			break;
		case 'B':
			break;
		case 'C':	/*** Ͷ��ҵ���� *** �޷��ж��Ƿ����� ***
					**** ����ɾ��ֻ���޸�״̬ ***************/
			if( !strncmp(ptrace_log->tx_code,"46",2)
				|| !strncmp(ptrace_log->tx_code,"47",2) )
			{
				memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and tx_no=%ld",
					ptrace_log->tx_date,ptrace_log->trace_no);
				if (ret)
				{
					sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O242");
					goto ErrExit;
				}
				ret = Mo_city_Fet_Upd(&sMo_city,g_pub_tx.reply);
				if (ret == 100)		break;
				if (ret)
				{
					sprintf(acErrMsg, "��ѯͬ�ǵǼǲ�����!! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O243");
					goto ErrExit;
				}
				strcpy(sMo_city.sts,"*");
				ret = Mo_city_Upd_Upd(sMo_city,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg, "û�и���Ʊ��![%ld]",ptrace_log->trace_no);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O243");
					goto ErrExit;
					break;
				}
				Mo_city_Clo_Upd();
			}
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����ǼǱ�*/
static int Rol_mo_fax( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�������д����*/
static int Rol_mo_fax_code( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ί���嵥*/
static int Rol_mo_trust_list( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*ί���տ�ǼǱ�*/
static int Rol_mo_trust_pay( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�ٴ���ǼǱ�*/
static int Rol_mo_again_ln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_again_ln_c	T_mo_again_ln;
	memset(&T_mo_again_ln,0x00,sizeof(struct mo_again_ln_c));

	switch( do_mode )
	{
		case '1' :	/*** �ٴ���¼�볷�� ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='0' )
			{
				sprintf(acErrMsg,"�ñ��ٴ����¼��״̬�����ȳ������˽���!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B285" );
				goto ErrExit;
			}
			ret=Mo_again_ln_Del_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '2' :	/*** �ٴ���ſ�(����)���� ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' )
			{
				sprintf(acErrMsg,"�ñ��ٴ���״̬����ȷ!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"0");
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '3' :	/*** �ٴ����¼�볷�� ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' )
			{
				sprintf(acErrMsg,"�ñ��ٴ���״̬����ȷ!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"1");
			T_mo_again_ln.sum_amt=T_mo_again_ln.sum_amt-ptrace_log->amt;
			if( T_mo_again_ln.sum_amt<0.005 )
			{
				sprintf(acErrMsg,"�ٴ���ǼǱ���������Ϊ�������ܳ���!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"B287" );
				goto ErrExit;
			}
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '4' :	/*** �ٴ������˳��� ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' && T_mo_again_ln.sts[0]!='2' )
			{
				sprintf(acErrMsg,"�ñ��ٴ���״̬����ȷ!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"1");
			T_mo_again_ln.sum_amt -=ptrace_log->amt;
			T_mo_again_ln.cur_amt=0;
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�ٴ����ǼǱ�*/
static int Rol_mo_pay_again_ln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_pay_again_ln_c	T_mo_pay_again_ln;
	char tmp_str[17];
	memset(&T_mo_pay_again_ln,0x00,sizeof(struct mo_pay_again_ln_c));

	switch( do_mode )
	{
		case '1' :	/*** �ٴ����¼�볷�� ***/
			ret=Mo_pay_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' and sts='0' and amt=%lf ",ptrace_log->ac_no,ptrace_log->amt);
			ERR_DEAL
			while(1)
			{
				ret=Mo_pay_again_ln_Fet_Upd(&T_mo_pay_again_ln,g_pub_tx.reply);
				ERR_DEAL
				if( T_mo_pay_again_ln.sts[0]!='0' )
				{
					sprintf(acErrMsg,"�ñ��ٴ�����¼��¼��״̬�����ȳ������˽���!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"B288" );
					goto ErrExit;
				}
				sprintf(tmp_str,"%.2f",T_mo_pay_again_ln.intst_amt);
				pub_base_strpack(ptrace_log->note_no);
				if( strcmp(ptrace_log->note_no,tmp_str) )
					continue;
				else
					break;
			}
			ret=Mo_pay_again_ln_Del_Upd(T_mo_pay_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_pay_again_ln_Clo_Upd();
			break;
		case '2' :	/*** �ٴ������˳��� ***/
			ret=Mo_pay_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' and sts='1' and amt=%lf ",ptrace_log->ac_no,ptrace_log->amt);
			ERR_DEAL
			while(1)
			{
				ret=Mo_pay_again_ln_Fet_Upd(&T_mo_pay_again_ln,g_pub_tx.reply);
				ERR_DEAL
				if( T_mo_pay_again_ln.sts[0]!='1' )
				{
					sprintf(acErrMsg,"�ñ��ٴ�����¼״̬����ȷ!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"B289" );
					goto ErrExit;
				}
				sprintf(tmp_str,"%.2f",T_mo_pay_again_ln.intst_amt);
				pub_base_strpack(ptrace_log->note_no);
				if( strcmp(ptrace_log->note_no,tmp_str) )
					continue;
				else
				{
					strcpy(T_mo_pay_again_ln.sts,"0");
					break;
				}
			}
			ret=Mo_pay_again_ln_Upd_Upd(T_mo_pay_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_pay_again_ln_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�����ʣ���������*/
static int Rol_point_point_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����ʹ����ϸ��*/
static int Rol_point_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��Ʒ��������*/
static int Rol_prdt_point_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��Ʒ����ʹ����ϸ��*/
static int Rol_prdt_point_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��������*/
static int Rol_ass_gl( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�������ļ�*/
static int Rol_ass_mst( struct trace_log_c *ptrace_log, char do_mode )
{
	struct ass_mst_c g_ass_mst;
	struct ass_mst_hst_c ass_mst_hst_g,tmp_ass_mst_hst;
	int ret=0;
    switch(do_mode)
    {
        case '1':			/* �ǵǼǲ���:������ϴ� */
			vtcp_log("[%s][%d]��ʼɾ��ass_mst��¼[%s]\n",__FILE__,__LINE__,ptrace_log->note_no);
            ret=sql_execute("delete from ass_mst where pact_no=%s",
                ptrace_log->note_no);
            ERR_DEAL
            break;
		case '2':			/* �������:��軹����ϴ�֧ȡ */
	if(ptrace_log->trace_cnt!=1)	/* ��һ���������Ĳ��Ƿ�����ظ�ȡ���Ǽǲ����� */
		return(0);
	vtcp_log("��ʼȡ��ass_mst�Ǽǲ�[%s][%d]\n",__FILE__,__LINE__);
	memset(&g_ass_mst,'\0',sizeof(g_ass_mst));
	memset(&ass_mst_hst_g,'\0',sizeof(ass_mst_hst_g));
	ret=Ass_mst_hst_Sel(g_pub_tx.reply,&ass_mst_hst_g,"tx_date=%ld and trace_no=%ld ", \
		ptrace_log->tx_date,ptrace_log->trace_no);
	ERR_DEAL

	ret=Ass_mst_Dec_Upd(g_pub_tx.reply,"pact_no=%ld",ass_mst_hst_g.pact_no);
	ERR_DEAL

	ret=Ass_mst_Fet_Upd(&g_ass_mst,g_pub_tx.reply);
	ERR_DEAL
	g_ass_mst.bal +=ass_mst_hst_g.tx_amt;
	g_ass_mst.intst_acm +=ass_mst_hst_g.intst_acm;
	g_ass_mst.intst=0;
	strcpy(g_ass_mst.sts,"1");

	ret=Ass_mst_Upd_Upd(g_ass_mst,g_pub_tx.reply);
	ERR_DEAL
	Ass_mst_Clo_Upd();

    /* �Ǽǿ�����ϸ */
	memset(&tmp_ass_mst_hst,'\0',sizeof(tmp_ass_mst_hst));
    strcpy(tmp_ass_mst_hst.br_no,g_ass_mst.br_no);
    tmp_ass_mst_hst.tx_date = g_pub_tx.tx_date;
    tmp_ass_mst_hst.trace_no = g_pub_tx.trace_no;
    strcpy(tmp_ass_mst_hst.sub_br_no,g_ass_mst.sub_br_no);
    tmp_ass_mst_hst.pact_no = g_ass_mst.pact_no;
    tmp_ass_mst_hst.tx_amt = 0 - ass_mst_hst_g.tx_amt;
    tmp_ass_mst_hst.bal = g_ass_mst.bal;
    tmp_ass_mst_hst.tx_time = g_pub_tx.tx_time;
    strcpy(tmp_ass_mst_hst.brf,"֧ȡ����");
    tmp_ass_mst_hst.hst_cnt++;
    strcpy(tmp_ass_mst_hst.tel,g_pub_tx.tel);
    strcpy(tmp_ass_mst_hst.chk,g_pub_tx.chk);
    strcpy(tmp_ass_mst_hst.auth,g_pub_tx.auth);

    ret = Ass_mst_hst_Ins(tmp_ass_mst_hst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽǿ�����ϸ����!!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O103");
        goto ErrExit;
    }
	break;
    	default:
            sprintf(acErrMsg,"�������̶������[%c]",do_mode);
            WRITEMSG
            strcpy( g_pub_tx.reply,"S054" );
            goto ErrExit;
    }

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*������ϸ*/
static int Rol_ass_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���˷ֶμ�Ϣ��*/
static int Rol_ass_subs_intst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*��Ȩ������*/
static int Rol_auth_code( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
static int Rol_intst_hst( struct trace_log_c *ptrace_log, char do_mode )
/*��Ϣ�Ǽǲ�*/
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ڴ����ϸ*/
static int Rol_dd_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*������ϸ*/
static int Rol_ln_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*͸֧�ʵ��ļ�*/
static int Rol_od_bill( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*͸֧��ϸ*/
static int Rol_od_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ڴ����ϸ*/
static int Rol_td_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���Ҵ���Ƿ���*/
static int Rol_ln_lo( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����ſ��ƻ���*/
static int Rol_ln_pay_pln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	sprintf(acErrMsg,"����ſ��ƻ���[%c]",do_mode);
	WRITEMSG
	switch(do_mode)
	{
		case '1':
			ret=sql_execute("delete from ln_pay_pln where wrk_date=%ld and pay_ind='0' and ac_id=%ld and ac_seqn=%d",
				ptrace_log->tx_date,ptrace_log->ac_id,ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�����˳���*/
static int Rol_ln_pay_seq( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����Ǽǲ�*/
static int Rol_ln_reg( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�������˻���ϵ*/
static int Rol_mdm_ac_rel( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mdm_ac_rel_c vmdm_ac_rel;
	int ret=0;

	sprintf(acErrMsg,"MDM_AC_REL[%c]",do_mode);
	WRITEMSG

	switch(do_mode)
	{
		case '1': /**������**/
			if ( strcmp(ptrace_log->tx_code, "2104") == 0 )
			{
				break;
			}

			ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'",
				ptrace_log->ac_no );
			ERR_DEAL
			ret=Mdm_ac_rel_Fet_Upd( &vmdm_ac_rel,g_pub_tx.reply );
			ERR_DEAL
			if( vmdm_ac_rel.note_sts[0]!='0' || vmdm_ac_rel.coll_sts[0]!='0' )
			{
				strcpy( g_pub_tx.reply,"L016" );
				goto ErrExit;
			}
			vmdm_ac_rel.note_sts[0]='5';
			ret=Mdm_ac_rel_Upd_Upd( vmdm_ac_rel,g_pub_tx.reply );
			ERR_DEAL
			Mdm_ac_rel_Clo_Upd( );
			break;
		case '2': /**������**/
			ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'",
				ptrace_log->ac_no );
			ERR_DEAL
			ret=Mdm_ac_rel_Fet_Upd( &vmdm_ac_rel,g_pub_tx.reply );
			if( ret!=100 )
			{
				ERR_DEAL
			}
			if( ret==0 )
			{
				if( vmdm_ac_rel.note_sts[0]=='*' )
				{
					vmdm_ac_rel.note_sts[0]='0';
					ret=Mdm_ac_rel_Upd_Upd( vmdm_ac_rel,g_pub_tx.reply );
					ERR_DEAL
				}
			}
			Mdm_ac_rel_Clo_Upd( );
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*δ������ϸ��*/
static int Rol_mdm_unprt_hst( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret;
	struct mdm_unprt_hst_c mdm_unprt_hst_c;
	struct mdm_ac_rel_c mdm_ac_rel_c;
	struct pub_tx v_pub_tx;
	int i=0;
vtcp_log("ROLLBACK unprt_hst");

	switch(do_mode)
	{
		case '1':
			ret=Mdm_unprt_hst_Dec_Sel( g_pub_tx.reply,
			"ac_id=%ld and ac_seqn=%d and tx_date=%ld order by trace_no DESC",
				ptrace_log->ac_id, ptrace_log->ac_seqn,
				ptrace_log->tx_date );
			ERR_DEAL

			ret=Mdm_unprt_hst_Fet_Sel( &mdm_unprt_hst_c,g_pub_tx.reply );
			if( ret==100 )
			{
vtcp_log("WRITe ���� hst");
					memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
					strcpy( g_pub_tx.brf,"����" );
					pub_unprt_hst_ins(g_pub_tx.ac_id_type);
					memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
			}
			else if( ret )
			{
				ERR_DEAL
			}
			else
			{
				if( mdm_unprt_hst_c.trace_no==ptrace_log->trace_no )
				{
					ret=sql_execute("delete from mdm_unprt_hst where ac_id=%ld and ac_seqn=%d and tx_date=%ld and trace_no=%ld",
						ptrace_log->ac_id, ptrace_log->ac_seqn,
						ptrace_log->tx_date, ptrace_log->trace_no );
					ERR_DEAL
				}
				else
				{
vtcp_log("WRITE ���� hst");
					memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
					strcpy( g_pub_tx.brf,"����" );
					pub_unprt_hst_ins(g_pub_tx.ac_id_type);
					memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
				}
			}

			Mdm_unprt_hst_Clo_Sel( );
			break;
		case '2':
			memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
			strcpy( g_pub_tx.brf,"����" );
			pub_unprt_hst_ins(g_pub_tx.ac_id_type);
			memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
			break;
		default:
			break;
	}
	Mdm_ac_rel_Clo_Upd( );
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*�˻������ʷ*/
static int Rol_mo_ac_chg_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}

/* �շ���ϸ */
static int Rol_com_chrg_hst( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;

	 sprintf(acErrMsg,"�����շ���ϸ[%c]",do_mode);
	 WRITEMSG
	 switch(do_mode)
	 {
		case '1':
			ret=sql_execute("delete from com_chrg_hst where tx_date=%ld and \
					trace_no=%ld and ac_no='%s'", ptrace_log->tx_date,
					ptrace_log->trace_no,ptrace_log->ac_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/* �ͻ���Ʒ��ϵ�� */
static int Rol_cif_prdt_rel( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;

	 switch(do_mode)
	 {
		case '1':
			ret=sql_execute("delete from cif_prdt_rel where ac_id=%ld",
				ptrace_log->ac_id );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*����Ǽǲ�*/
static int Rol_mo_hold( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*���֧�������ˮ��*/
static int Rol_Mbfeghvsendjrn( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mbfeghvsendjrn_c	sMbfeghvsendjrn;
	int ret=0;
	memset(&sMbfeghvsendjrn,0x00,sizeof(struct mbfeghvsendjrn_c));
	if( do_mode=='1' )	/** ɾ�� **/
	{
		ret=Mbfeghvsendjrn_Dec_Sel(g_pub_tx.reply," consigndate=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ���֧�������ˮ�����ret=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P459" );
			goto ErrExit;
		}

		while(1)
		{
			ret=Mbfeghvsendjrn_Fet_Sel(&sMbfeghvsendjrn,g_pub_tx.reply);
			if( ret==100 )	break;
			if( ret )
			{
				sprintf(acErrMsg,"ȡ���֧�������ˮ�����ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P459" );
				goto ErrExit;
			}
			if( sMbfeghvsendjrn.procstate[0]=='1' )
			{
				sprintf(acErrMsg,"�ñ�ҵ���Ѿ�ͨ�����֧��ϵͳ������ܳ���!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P460" );
				goto ErrExit;
			}
		}
		Mbfeghvsendjrn_Clo_Sel();
		ret=sql_execute("delete from mbfeghvsendjrn where consigndate=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		if( ret )
		{
			sprintf(acErrMsg,"ɾ�����֧�������ˮ�����!ret=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P461" );
			goto ErrExit;
		}
	}
	else if( do_mode=='2' )	/** �޸� **/
	{
		;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*���֧�������嵥*/
static int Rol_Mbfeghvrecvlist( struct trace_log_c *ptrace_log, char do_mode )
{
	char tx_date[9];
	int ret=0;
	struct mbfeghvrecvlist_c	sMbfeghvrecvlist;
	memset(&sMbfeghvrecvlist,0x00,sizeof(struct mbfeghvrecvlist_c));
	if( do_mode=='1' )	/** ɾ�� **/
	{
		;
	}
	else if( do_mode=='2' )	/** �޸� **/
	{
			sprintf(tx_date,"%8ld",ptrace_log->tx_date);
			ret=Mbfeghvrecvlist_Dec_Upd(g_pub_tx.reply," incometime[1,8]='%s' and incomeflowno=%ld ",tx_date,ptrace_log->trace_no);
			if( ret )
			{
				sprintf(acErrMsg,"ȡ���֧��������ˮ�����ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P466" );
				goto ErrExit;
			}

			ret=Mbfeghvrecvlist_Fet_Upd(&sMbfeghvrecvlist,g_pub_tx.reply);
			if( ret==100 )
			{
				Mbfeghvsendjrn_Clo_Upd();
				return 0;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ���֧��������ˮ�����ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P466" );
				goto ErrExit;
			}
			if( sMbfeghvrecvlist.procstate[0]!='7' )
			{
				sprintf(acErrMsg,"���֧��������ˮ״̬����ȷ�����ܳ���!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P467" );
				goto ErrExit;
			}
			strcpy(sMbfeghvrecvlist.incomeacno,"");
			strcpy(sMbfeghvrecvlist.incomebkcode,"");
			strcpy(sMbfeghvrecvlist.incometime,"");
			sMbfeghvrecvlist.incomeflowno=0;
			strcpy(sMbfeghvrecvlist.incometel,"");
			sMbfeghvrecvlist.procstate[0]='5';

			ret=Mbfeghvrecvlist_Upd_Upd(sMbfeghvrecvlist,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"���´��֧��������ˮ�����!ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P468" );
				goto ErrExit;
			}
			Mbfeghvsendjrn_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}


/*�ֽ��ո��Ǽǲ� add by xxx*/
static int Rol_Cash_book( struct trace_log_c *ptrace_log, char do_mode )
{
	int		ret=0;
	struct	cash_book_c		sCashBook;
	struct	com_sys_parm_c		com_sys_parm;
	long 	l_trace_no=0;
	char add_ind[2];

sprintf( acErrMsg,"zyzyzyz ptrace_log->tx_date[%ld],ptrace_log->trace_no[%ld]",
	ptrace_log->tx_date, ptrace_log->trace_no );
WRITEMSG

	if( !strcmp(ptrace_log->sub_tx_code,"M202") )
	{
		if( ptrace_log->add_ind[0]=='1' )
			strcpy( add_ind,"0" );
		else
			strcpy( add_ind,"1" );
	}
	else
	{
		strcpy( add_ind,ptrace_log->add_ind );
	}
	l_trace_no=ptrace_log->trace_no;
vtcp_log("ZYZYZYZ[%c][%d] [%s]",do_mode,l_trace_no,add_ind);

	switch(do_mode)
	{
		case '1': /* �����ֽ��ո����� */
			memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
			if (add_ind[0] == '0')
			{
			}
			else if (add_ind[0] == '1')
			{
vtcp_log("Cash2: tx_date=%ld and wrk_no=%ld",ptrace_log->tx_date,l_trace_no );
			ret = Cash_book_Sel( g_pub_tx.reply, &sCashBook,
				"tx_date=%ld and wrk_no=%ld",ptrace_log->tx_date,l_trace_no );
			}
			if(ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "��ѯ�ֽ��ո��Ǽǲ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "M028");
				goto ErrExit;
			}
			else if(ret == 100)
			{
				goto OkExit;
				sprintf(acErrMsg, "û�иñʸ�����¼![%ld]",ptrace_log->trace_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "M027");
				goto ErrExit;
				break;
			}

			pub_base_strpack(ptrace_log->brf);
			if( !strcmp(ptrace_log->brf,"Ԥ��Ʊ") )
			{
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and trace_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Dec_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "�ֽ��ո��Ǽǲ��в����ڴ˱������Ӧ�ĸ�����¼!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}else if ( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Fet_Upd��!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}
				if( sCashBook.ind[0]!='2' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M034" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "*" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
				ret=sql_execute("delete from cash_book where trace_no=%ld and tx_date=%ld and ind='*'",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
			}
			else if( !strcmp(ptrace_log->brf,"����Ԥ��Ʊ") )
			{
				/**ȡ�����ո���**/
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and out_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				ERR_DEAL

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "�ֽ��ո��Ǽǲ��в����ڴ˱������Ӧ�ĸ�����¼!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}
				ERR_DEAL

				if( sCashBook.ind[0]!='0' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M031" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "*" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
				ret=sql_execute("delete from cash_book where out_no=%ld and tx_date=%ld and ind='*'",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
				/**ȡ������Ԥ��**/
				ret=Com_sys_parm_Sel( g_pub_tx.reply,&com_sys_parm,"1=1" );
				ERR_DEAL
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and out_no=%ld and ind='3'",
					com_sys_parm.lst_date, l_trace_no );
				ERR_DEAL

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "�ֽ��ո��Ǽǲ��в����ڴ˱������Ӧ�ĸ�����¼!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}
				ERR_DEAL

				if( sCashBook.ind[0]!='3' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M031" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "2" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
			}
			else if( add_ind[0] == '0' )			/* �ֽ𸶳�ʱ */
			{
vtcp_log("Cash1: tx_date=%ld and trace_no=%ld",ptrace_log->tx_date,l_trace_no );
			ret = Cash_book_Sel( g_pub_tx.reply, &sCashBook,
				"tx_date=%ld and out_no=%ld",ptrace_log->tx_date,l_trace_no );
				if( sCashBook.ind[0] == '1' )
				{
					sprintf(acErrMsg, "�ñ��ֽ𸶳���¼�������벻�ܳ���!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "M029");
					goto ErrExit;
				}

				ret=sql_execute("delete from cash_book where out_no=%ld and tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
			}
			else										/* �ֽ�����ʱ */
			{
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and wrk_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Dec_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "�ֽ��ո��Ǽǲ��в����ڴ˱������Ӧ�ĸ�����¼!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}else if ( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Fet_Upd��!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "0" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
			}
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*���ҲֵǼǱ�*/
static int Rol_mo_sg_cash_po_bill( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_sg_cash_po_bill_c	T_mo_sg_cash_po_bill;
	memset(&T_mo_sg_cash_po_bill,0x00,sizeof(struct mo_sg_cash_po_bill_c));

	switch( do_mode )
	{
		case '1' :	/*** ����������� ***/
			ret=Mo_sg_cash_po_bill_Dec_Upd(g_pub_tx.reply," lading_bill='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_bill_Fet_Upd(&T_mo_sg_cash_po_bill,g_pub_tx.reply);
			ERR_DEAL
			ret=Mo_sg_cash_po_bill_Del_Upd(T_mo_sg_cash_po_bill,g_pub_tx.reply);
			ERR_DEAL
			Mo_sg_cash_po_bill_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/* ������֤��Ǽǲ� */
Rol_mo_sg_protect_note(struct trace_log_c *ptrace_log, char do_mode)
{
	int	ret=0;
	struct	protect_note_c pro_note;
	memset( &pro_note,0x00,sizeof(struct protect_note_c));

sprintf(acErrMsg,"������֤����[%s][%s][%c]",ptrace_log->tx_code,ptrace_log->note_no,do_mode);
WRITEMSG
	if ( strncmp("6321",ptrace_log->tx_code,4) &&
		strncmp("6322",ptrace_log->tx_code,4) &&
		strncmp("6323",ptrace_log->tx_code,4) &&
		strncmp("6325",ptrace_log->tx_code,4) &&
		strncmp("6326",ptrace_log->tx_code,4) )
	{
		sprintf(acErrMsg,"�ǵ�����֤����[%s]",ptrace_log->tx_code);
		WRITEMSG
		goto OkExit;
	}
	pub_base_strpack(ptrace_log->note_no);
	switch( do_mode )
	{
		case '1' :	/*** ������������ ***/
			/**
			ret = sql_execute("UPDATE protect_note SET protect_sts='*' WHERE pact_no='%s'",ptrace_log->note_no);
			**/
			ret=Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			if( pro_note.protect_sts[0]!='0' )
			{
				sprintf(acErrMsg,"����Э��ǼǱ�״̬����[%s]",pro_note.protect_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B349" );
				goto ErrExit;
			}
			strcpy(pro_note.protect_sts,"A");
			ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
			ERR_DEAL
			Protect_note_Clo_Upd();
			break;
			ERR_DEAL
			break;
		case '2' :	/*** �����������⳥���� ***/
			ret = sql_execute("UPDATE protect_note SET protect_sts='0' WHERE pact_no='%s'",ptrace_log->note_no);
			ERR_DEAL
			break;
		case '3' :	/*** ��֤����� ***/
			ret=Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			if( ptrace_log->add_ind[0]=='1' )	/** �ɴ汣֤�� **/
			{
				if( pro_note.protect_sts[0]!='A' )
				{
					sprintf(acErrMsg,"����Э��ǼǱ�״̬����[%s]",pro_note.protect_sts);
					WRITEMSG
					strcpy( g_pub_tx.reply,"B349" );
					goto ErrExit;
				}
				ret=Protect_note_Del_Upd(pro_note,g_pub_tx.reply);
				ERR_DEAL
			}
			else 								/** ���ɱ�֤�� **/
			{
				pro_note.bail_amt=pro_note.bail_amt-ptrace_log->amt;
				ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
				ERR_DEAL
			}
			Protect_note_Clo_Upd();
			break;
		case '6' :	/*** ��֤��֧ȡ���� ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Protect_note_Dec_Upd(g_pub_tx.reply," pact_no='%s' and protect_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			pro_note.bail_amt=pro_note.bail_amt+ptrace_log->amt;
			ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
			ERR_DEAL
			Protect_note_Clo_Upd();
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*�����������*/
Rol_Mbfeghvsendlist(struct trace_log_c *ptrace_log, char do_mode)
{
	g_reply_int = sql_execute("update mbfeghvsendlist set procstate='30',incomeacno='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�������׸��´�����˵Ǽǲ���¼����![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}

/*������˹���ҵ������*/
Rol_Mbfeghvrecvlist1(struct trace_log_c *ptrace_log, char do_mode )
{
	g_reply_int = sql_execute("update mbfeghvrecvlist set procstate[1]='5',incomeacno='',incomebkcode='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�������׸��´�����˵Ǽǲ���¼����![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/* add by LiuHuafeng 2006-10-4 10:16
 * ���Ǽǲ�
 */
Rol_Hv_zf(struct trace_log_c *ptrace_log, char do_mode )
{
	int 	iRet=0;
	struct 	hv_zf_c sHv_zf;
	struct 	hv_zf_c sHv_zf_th;/***רΪ�˻�ʹ�õĽṹ��****/
	char   	cWz_sts[2];	 /**���ڶ�ԭ���״����ڶ��˻��¼����֮��,��Ҫ���˻��¼��״̬���б���**/
	char 	cReply[5];

	memset(cReply, 0 , sizeof(cReply));
	memset(&sHv_zf, 0 , sizeof(sHv_zf));
	memset(&sHv_zf_th, 0 ,sizeof(sHv_zf_th));
	memset(cWz_sts, 0 , sizeof(cWz_sts));

	iRet=Hv_zf_Dec_Upd(cReply,"lw_ind=1 and ( (chk_trace_no=%d and chk='%s') or (trace_no=%d and tel='%s') ) and wt_date=%d",ptrace_log->trace_no,ptrace_log->tel,ptrace_log->trace_no,ptrace_log->tel,ptrace_log->tx_date);
	if(iRet)
	{
		vtcp_log("%s,%d ������ҵ��Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	iRet=Hv_zf_Fet_Upd( &sHv_zf, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d û���ҵ�Ҫ�����Ĵ��ҵ��Ǽǲ�[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	/***׼���ĸ��ֶ�,���������˻㽻��ʱ������ԭ֧����****/
	memcpy(sHv_zf_th.or_br_no,	sHv_zf.ac_br_no,	sizeof(sHv_zf.ac_br_no)-1);
	memcpy(sHv_zf_th.orderno,	sHv_zf.o_orderno,	sizeof(sHv_zf.o_orderno)-1);
	sHv_zf_th.wt_date	=	sHv_zf.o_wt_date;
	memcpy(sHv_zf_th.cmtno,		sHv_zf.o_cmtno,		sizeof(sHv_zf.o_cmtno)-1);
	/****���ݸñʼ�¼��״̬,רΪ�˻�ʱ����ԭ֧�����ʽ���***/
	cWz_sts[0]	=	sHv_zf.hv_sts[0];


	if(sHv_zf.hv_sts[0]!=SEND_ACCT_CHECK && sHv_zf.hv_sts[0]!=SEND_REGIST)
	{
		vtcp_log("%s,%d ���״̬=[%c]������ȡ��",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
		goto ErrExit;
	}
	if(sHv_zf.hv_sts[0]==SEND_ACCT_CHECK)
	{
		if(ptrace_log->trace_no!=sHv_zf.chk_trace_no)
		{
			vtcp_log("%s,%d ���״̬=[%c]�����ȳ���������ˮ",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
			strcpy(g_pub_tx.reply,"P074");
			goto ErrExit;
		}
		if(memcmp(sHv_zf.cmtno,HV_HPYC,3))
		{
			sHv_zf.hv_sts[0]=SEND_REGIST;
		}
		else
		{
			sHv_zf.hv_sts[0]=SEND_REGIST;
		}
	}
	else
	{
		sHv_zf.hv_sts[0]=SEND_REVERSE_REG;
	}
	vtcp_log("%s,%d hvsts==[%c]",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
 	iRet=Hv_zf_Upd_Upd( sHv_zf, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P055");
		goto ErrExit;
	}

	Hv_zf_Clo_Upd();


	vtcp_log("[%s][%d]cmtno==[%.3s],hv_sts=[%c]\n",__FILE__,__LINE__,sHv_zf.cmtno,cWz_sts[0]);


	if(!memcmp(sHv_zf.cmtno,HV_TH,3) && cWz_sts[0]==SEND_ACCT_CHECK)
	{
		vtcp_log("[%s][%d]ȡ�������˻�Ľ�����Ҫ��ԭ���ʵ�״̬���и���!\n",__FILE__,__LINE__);
		iRet = Hv_zf_Dec_Upd(cReply,"or_br_no='%.12s' and orderno= '%.8s' and cmtno='%.3s' and wt_date=%d ",
				sHv_zf_th.or_br_no,sHv_zf_th.orderno,sHv_zf_th.cmtno,sHv_zf_th.wt_date);
		if(iRet)
		{
				vtcp_log("%s,%d ������ҵ��Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}
		iRet=Hv_zf_Fet_Upd( &sHv_zf_th, cReply );
		if(iRet)
		{
				vtcp_log("%s,%d û���ҵ�Ҫ�����Ĵ��ҵ��Ǽǲ�[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}

	/*	sHv_zf_th.hv_sts[0] = RECEIVE_ACCT_QS;**����Ϊ���ʵǼ�״̬***/
		sHv_zf_th.hv_sts[0] = sHv_zf_th.hv_osts[0];/*�ָ���ԭ����״̬**/


		iRet=Hv_zf_Upd_Upd( sHv_zf_th, cReply);
		if(iRet)
		{
			vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
			goto ErrExit;
		}

	}

OkExit:
	return 0;
ErrExit:
	return -1;
}

/*
 * ��Ʊ�Ǽǲ�
 */
Rol_Hv_Poinfo(struct trace_log_c *ptrace_log, char do_mode )
{
	struct hv_poinfo_c sPoinfo;
	struct hv_brno_orno_c sHv_brno_orno;
	char cReply[5];
	char l_postat[1];

	int iRet=0;
	int iPo_no = 0;
	char cPo_no[9];
	memset(cPo_no,0x00,sizeof(cPo_no));
	iPo_no = atoi(ptrace_log->note_no);
	sprintf(cPo_no,"%08d",iPo_no);
	memset(&sHv_brno_orno, 0 , sizeof(sHv_brno_orno));
	memset(cReply, 0 , sizeof(cReply));
	memset(&sPoinfo, 0 , sizeof(sPoinfo));
	l_postat[0] = 0x00;



	if(ptrace_log->svc_ind==7021)
	{
		vtcp_log("[%s][%dƾ֤��鲻Ҫ���޸ĵǼǱ�\n",__FILE__,__LINE__);
		goto OkExit;
	}
	/* �ҵ������׻�����Ӧ��֧��ϵͳ�к� */
	vtcp_log("[%s][%d]�����Ʊȡ������!\n",__FILE__,__LINE__);
	iRet=Hv_brno_orno_Sel(cReply,&sHv_brno_orno,"br_no='%s'",ptrace_log->tx_br_no);
	if(iRet)
	{
		vtcp_log("%s,%d �õ����׻�����Ӧ��֧��ϵͳ�кŴ���[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}


	iRet=Hv_poinfo_Dec_Upd(cReply,"pay_br_no='%s' and po_no='%s' ",sHv_brno_orno.or_br_no,cPo_no);
	if(iRet)
	{
		vtcp_log("%s,%d �����Ʊ�Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	iRet=Hv_poinfo_Fet_Upd( &sPoinfo, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d û���ҵ�Ҫ�����Ļ�Ʊ��Ϣ[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	Hv_poinfo_Debug(&sPoinfo);

	vtcp_log("%s,%d ��Ʊ״̬=[%s]�����Գ���[%d]",__FILE__,__LINE__,sPoinfo.po_sts,iRet);
	/****ӪҵԱ���ǩ���Ļ�Ʊ����ȡ��***/
	if(  sPoinfo.po_sts[0]==HV_HP_CASH
	 || sPoinfo.po_sts[0]==HV_HP_CASH || sPoinfo.po_sts[0]==HV_HP_JQ
	 || sPoinfo.po_sts[0]==HV_HP_PARTBACK || sPoinfo.po_sts[0]==HV_HP_FULLBACK
	 || sPoinfo.po_sts[0]==HV_HP_TOCUST || sPoinfo.po_sts[0]==HV_HP_UNSIGN
	 || sPoinfo.po_sts[0]==HV_HP_SEND || sPoinfo.po_sts[0]==HV_HP_LOSS
	 || sPoinfo.po_sts[0]==HV_HP_UNLOSS || sPoinfo.po_sts[0]==HV_HP_OVERDATE_UNTREAD
	 || sPoinfo.po_sts[0]==HV_HP_ASKRETURN || sPoinfo.po_sts[0]==HV_HP_CANCEL
	)
	{
		vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���[%d]",__FILE__,__LINE__,sPoinfo.po_sts[0],iRet);
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���[%d]",__FILE__,__LINE__,sPoinfo.po_sts[0],iRet);

	/****add by liuyue ���������ȡ���Ǽ� ��ȡ���ƴ������*****/
	vtcp_log("[%s][%d]ptrace_log->tx_code=[%s],Poinfo.po_sts[0]==[%c]\n",__FILE__,__LINE__,
		ptrace_log->tx_code,sPoinfo.po_sts[0]);
	if(!memcmp(ptrace_log->tx_code,"5805",4) && sPoinfo.po_sts[0]!=HV_HP_REG)
	{
		strcpy(g_pub_tx.reply,"P074");
		if(sPoinfo.po_sts[0]==HV_HP_MV_CENTER)
			set_zd_data("0130","��ȡ�����ʽ��ƴ���ʡ���ˮ->��ǩ����ˮ��");
		else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST)
			set_zd_data("0130","��ȡ����δ�ý⸶����ˮ->��ǩ����ˮ��");
		else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
			set_zd_data("0130","��ȡ����δ���˻س��ʡ���ˮ->��ǩ����ˮ��");
		else if(sPoinfo.po_sts[0]==HV_HP_SIGN)
			set_zd_data("0130","��ȡ������Ʊǩ������ˮ");
		goto ErrExit;
	}
	if(!memcmp(ptrace_log->tx_code,"5807",4) && sPoinfo.po_sts[0]!=HV_HP_SIGN)
	{
			strcpy(g_pub_tx.reply,"P074");
			if(sPoinfo.po_sts[0]==HV_HP_MV_CENTER)
			set_zd_data("0130","��ȡ�����ʽ��ƴ���ʡ���ˮ");
			else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST)
			set_zd_data("0130","��ȡ����δ�ý⸶����ˮ");
			else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
			set_zd_data("0130","��ȡ����δ���˻س��ʡ�");
			goto ErrExit;
	}

	if(sPoinfo.po_sts[0]==HV_HP_REG)
	{
		  l_postat[0] = sPoinfo.po_sts[0];
		  sPoinfo.po_sts[0]=HV_HP_UNREG;
		  sPoinfo.l_po_sts[0] = l_postat[0];

	}
	/***add by liuyue 20070109***/
	if(sPoinfo.po_sts[0]==HV_HP_SIGN)
	{
		  vtcp_log("[%s][%d]��Ʊ״̬��ǩ��״̬,po_sts=[%c],l_po_sts=[%c]\n",__FILE__,__LINE__,sPoinfo.po_sts[0],sPoinfo.l_po_sts[0]);
		  l_postat[0] = sPoinfo.po_sts[0];
		  sPoinfo.po_sts[0]=HV_HP_REG;
		  sPoinfo.l_po_sts[0] = l_postat[0];
		  vtcp_log("[%s][%d]��Ʊ״̬��ǩ��״̬,po_sts=[%c],l_po_sts=[%c]\n",__FILE__,__LINE__,sPoinfo.po_sts[0],sPoinfo.l_po_sts[0]);

	}
	if( sPoinfo.po_sts[0]==HV_HP_MV_CENTER || sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST ||sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
	  {
	  	/**���ȡ���ɹ�,����ǰ״̬���浽l_po_sts������Ǽ�/ǩ��ȡ��ʱ���ٵǼ�����**/
		vtcp_log("[%s][%d]xiersPoinfo.l_posts=[%c] po_sts=[%c]\n", __FILE__,__LINE__,sPoinfo.l_po_sts[0],sPoinfo.po_sts[0]);
		/*
	  	l_postat[0] = sPoinfo.po_sts[0];
		sPoinfo.po_sts[0]=sPoinfo.l_po_sts[0];
		sPoinfo.l_po_sts[0] = l_postat[0];
		*/
		sPoinfo.l_po_sts[0] =sPoinfo.po_sts[0];
		sPoinfo.po_sts[0]=HV_HP_SIGN;
		vtcp_log("[%s][%d]xiersPoinfo.l_posts=[%c] po_sts=[%c]\n", __FILE__,__LINE__,sPoinfo.l_po_sts[0],sPoinfo.po_sts[0]);
	}
 	iRet=Hv_poinfo_Upd_Upd(sPoinfo, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]�޸Ļ�Ʊ״̬�ɹ���!\n",__FILE__,__LINE__);
	Hv_poinfo_Debug(&sPoinfo);
OkExit:
	vtcp_log("[%s][%d]��Ʊȡ���ɹ� �ǼǱ�!\n",__FILE__,__LINE__);
	return 0;
ErrExit:
	return -1;
}
/* end by LiuHuafeng 2006-10-4 11:22 */

/*���ڻ�Ʊ�Ҹ�*/
Rol_Mbfebillopnreg( struct trace_log_c *ptrace_log, char do_mode )
{
	g_reply_int = sql_execute("update mbfebillopnreg set useind='5',incomeacno='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�������׸��´�����˵Ǽǲ���¼����![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/***ȡ��С��֧������****/
Rol_Lv_Pkgreg(struct trace_log_c *ptrace_log, char do_mode )
{
	vtcp_log("[%s][%d]����С��֧������ȡ������!\n",__FILE__,__LINE__);
	int 	iRet=0;
	struct 	lv_pkgreg_c sLv_pkgreg;
	struct 	lv_pkgreg_c sLv_pkgreg_th;/***רΪ�˻�ʹ�õĽṹ��****/
	char   	cWz_sts[2];	 /**���ڶ�ԭ���״����ڶ��˻��¼����֮��,��Ҫ���˻��¼��״̬���б���**/
	char 	cReply[5];
	char    cWssrno[7];

	memset(cReply, 0 , sizeof(cReply));
	memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
	memset(&sLv_pkgreg_th, 0 ,sizeof(sLv_pkgreg_th));
	memset(cWz_sts, 0 , sizeof(cWz_sts));
	memset(cWssrno, 0 , sizeof(cWssrno));

	sprintf(cWssrno,"%d",ptrace_log->trace_no);

	iRet=Lv_pkgreg_Dec_Upd(cReply,"lw_ind=1 and ( (check_trace_no='%s' and check_tel='%s') or (input_trace_no='%s' and input_tel='%s') ) and wt_date=%d",cWssrno,ptrace_log->tel,cWssrno,ptrace_log->tel,ptrace_log->tx_date);
	if(iRet)
	{
		vtcp_log("%s,%d ������ҵ��Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d û���ҵ�Ҫ�����Ĵ��ҵ��Ǽǲ�[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	/***׼���ĸ��ֶ�,���������˻㽻��ʱ������ԭ֧����****/
	memcpy(sLv_pkgreg_th.or_br_no,	sLv_pkgreg.ac_br_no,	sizeof(sLv_pkgreg.ac_br_no)-1);
	memcpy(sLv_pkgreg_th.orderno,	sLv_pkgreg.o_orderno,	sizeof(sLv_pkgreg.o_orderno)-1);
	sLv_pkgreg_th.wt_date	=	sLv_pkgreg.o_wt_date;
	/****���ݸñʼ�¼��״̬,רΪ�˻�ʱ����ԭ֧�����ʽ���***/
	cWz_sts[0]	=	sLv_pkgreg.lv_sts[0];


	if(sLv_pkgreg.lv_sts[0]!=STAT_WZCHECK2 && sLv_pkgreg.lv_sts[0]!=STAT_WZLR)
	{
		vtcp_log("%s,%d ���״̬=[%c]������ȡ��",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
		strcpy(g_pub_tx.reply,"P057");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]sLv_pkgreg.lv_sts[0]=[%c]=STAT_WZCHECK2=[%c]\n",__FILE__,__LINE__,
		sLv_pkgreg.lv_sts[0],STAT_WZCHECK2);

	/****����󲻿�������������  add at 20120105 begin****/
	if ( strlen(sLv_pkgreg.packid)!=0 )
	{
		vtcp_log("%s,%d С������󲻿�����ȡ��:sLv_pkgreg.packid=[%s] ",__FILE__,__LINE__,sLv_pkgreg.packid);
		strcpy(g_pub_tx.reply,"P057");
		goto ErrExit;
	}
	/****����󲻿�������������  add at 20120105 end****/

	if(sLv_pkgreg.lv_sts[0]==STAT_WZCHECK2)
	{
		vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sLv_pkgreg.check_trace_no=[%ld]\n",
			__FILE__,__LINE__,ptrace_log->trace_no,apatoi(sLv_pkgreg.check_trace_no,sizeof(sLv_pkgreg.check_trace_no)-1));
		if(ptrace_log->trace_no!=apatoi(sLv_pkgreg.check_trace_no,sizeof(sLv_pkgreg.check_trace_no)-1))
		{
		vtcp_log("%s,%d ���״̬=[%c]�����ȳ���������ˮ",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
		strcpy(g_pub_tx.reply,"P074");
		goto ErrExit;
		}
		sLv_pkgreg.lv_sts[0]=STAT_WZLR;
	}
	else
	{
		sLv_pkgreg.lv_sts[0]=STAT_INVALID;
	}
	vtcp_log("%s,%d hvsts==[%c]",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
 	iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P055");
		goto ErrExit;
	}

	Lv_pkgreg_Clo_Upd();


	vtcp_log("[%s][%d]cmtno==[%.3s],lv_sts=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno,cWz_sts[0]);


	if(!memcmp(sLv_pkgreg.pkgno,PKGNO_DJTH,3) && cWz_sts[0]==STAT_WZCHECK2)
	{
		vtcp_log("[%s][%d]ȡ�������˻�Ľ�����Ҫ��ԭ���ʵ�״̬���и���!\n",__FILE__,__LINE__);
		iRet = Lv_pkgreg_Dec_Upd(cReply,"or_br_no='%.12s' and orderno= '%.8s'  and wt_date=%d ",
				sLv_pkgreg_th.or_br_no,sLv_pkgreg_th.orderno,sLv_pkgreg_th.wt_date);
		if(iRet)
		{
				vtcp_log("%s,%d ������ҵ��Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}
		iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg_th, cReply );
		if(iRet)
		{
				vtcp_log("%s,%d û���ҵ�Ҫ�����Ĵ��ҵ��Ǽǲ�[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}

		/*sLv_pkgreg_th.lv_sts[0] = STAT_LZRECV2;*����Ϊ���ʵǼ�״̬***/
		sLv_pkgreg_th.lv_sts[0] = sLv_pkgreg_th.lv_o_sts[0];


		iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg_th, cReply);
		if(iRet)
		{
			vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
			goto ErrExit;
		}

	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/*���ʺ����ļ���ȡ������*/
Rol_sub_dd_mst( struct trace_log_c *ptrace_log, char do_mode )
{
	int	ret=0;

	int iCount = 0;		/* ��������ˮ��ı��� */
	double dAcbl = 0.00;	/* ���ĺ����� */
	double sRate=0.00;    /**��������**/
	double acm=0.00;      /**��Ϣ����**/

	double dTram = 0.00;	/* ������ˮ��Ӧ��� */
	char cAdd_ind[2];	/* 0����;1���� */
	char cSub_ac_no[25];    /* �ݴ����˺��� chenchao 20110117 */

	struct  sub_dd_mst_c	T_sub_dd_mst;
	struct  sub_dd_mst_hst_c	T_sub_dd_mst_hst;
	struct  sub_dd_mst_hst_c  sHst;
	memset(&T_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&T_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
	memset(cAdd_ind,0,sizeof(cAdd_ind));
	/** add by chenchao 20110117 **/
	memset(cSub_ac_no,0,sizeof(cSub_ac_no));
	/* modify begin by wzs 20120514 reason:���˻��������8λ����trace_log��ac_no������,���˺ŵ������trace_log��ac_id�еǼ�*/
	/* memcpy(cSub_ac_no,ptrace_log->ac_no,sizeof(cSub_ac_no)-1); */
	sprintf(cSub_ac_no,"%s-%ld",ptrace_log->ac_no,ptrace_log->ac_id);
	/* modify end by wzs 20120514 */
	memset(ptrace_log->ac_no,0,sizeof(ptrace_log->ac_no));
	pub_base_cut_str(cSub_ac_no,ptrace_log->ac_no,"-",1);
	vtcp_log(" %s, %d, ac_no=[%s]",__FILE__,__LINE__,ptrace_log->ac_no);

	switch( do_mode )
	{
	case '2' :	/*** ���ʺŴ�ȡ�� ***/
		if((ptrace_log->svc_ind==1201)&& (strcmp(ptrace_log->sub_tx_code,"ZHJX")==0))      /**������Ϣ ���˻����� ����**/
		{
			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&T_sub_dd_mst,"ac_no='%s' and sub_ac_no='%s'",ptrace_log->ac_no,cSub_ac_no);
			if(ret)
			{
				vtcp_log(" %s, %d, ��ѯ�����˻�ʧ�ܣ�",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			if(T_sub_dd_mst.intst_type[0]!='0' && T_sub_dd_mst.rate_type[0]=='0')
			{
				g_reply_int=pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date, &sRate);
				acm=ptrace_log->amt/sRate*L360*L100;
			}
			else if(T_sub_dd_mst.intst_type[0]!='0' && T_sub_dd_mst.rate_type[0]=='1')
			{
				sRate=T_sub_dd_mst.rate;
				acm=ptrace_log->amt/sRate*L360*L100;
			}
			else
			{
				acm=0.00;
			}
			ret=sql_execute("update sub_dd_mst set intst_acm=intst_acm+%lf where ac_no='%s' and sub_ac_no='%s'",acm,ptrace_log->ac_no,cSub_ac_no);
			if(ret)
			{
				sprintf(acErrMsg,"���ʺų������������˻������޸Ĵ���!");
				WRITEMSG
				    strcpy(g_pub_tx.reply,"L310");
				goto ErrExit;
			}
		}
		ret=Sub_dd_mst_hst_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld and trace_cnt=%d and ac_no='%s' and tel='%s'",g_pub_tx.tx_date,ptrace_log->trace_no,ptrace_log->trace_cnt,ptrace_log->ac_no,g_pub_tx.tel);
		if(ret)
		{
			vtcp_log(" %s, %d, Sub_dd_mst_hst_Dec_Sel����%d��",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		while(1)
		{
			memset(&T_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
			memset(&T_sub_dd_mst_hst,0,sizeof(T_sub_dd_mst_hst));
			ret=Sub_dd_mst_hst_Fet_Upd(&T_sub_dd_mst_hst,g_pub_tx.reply);
			if(ret==100)
			{
				break;
			}
			else if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Fet_Upd����%d��",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}
			ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%ld ",T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Dec_Upd����%d��",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
			ret=Sub_dd_mst_Fet_Upd(&T_sub_dd_mst,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Dec_Upd����%d��",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"");
				goto ErrExit;
			}
			if ( ptrace_log->svc_ind==1201 && T_sub_dd_mst_hst.add_ind[0]=='1' )	/** ��� **/
			{
				T_sub_dd_mst.bal-=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='1' )
				{
					sprintf(acErrMsg,"�����ʺ�״̬����ȷ�����ܳ���[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
			}
			else if ( ptrace_log->svc_ind==1201 && T_sub_dd_mst_hst.add_ind[0]=='0' )	/** ȡ�� **/
			{
				T_sub_dd_mst.bal+=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='1' )
				{
					sprintf(acErrMsg,"�����ʺ�״̬����ȷ�����ܳ���[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
			}
			else if ( ptrace_log->svc_ind==1202 )				/** ���� **/
			{
				T_sub_dd_mst.bal+=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='*' )
				{
					sprintf(acErrMsg,"�����ʺ�״̬����ȷ�����ܳ���[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
				strcpy(T_sub_dd_mst.ac_sts,"1");
			}
			else
			{
				sprintf(acErrMsg,"�޴���������������ܳ���[%s]",T_sub_dd_mst.ac_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D253" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(T_sub_dd_mst.bal,0.00)<0 )
			{
				sprintf(acErrMsg,"���������Ϊ��[%c]",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P214" );
				goto ErrExit;
			}
			iCount=0;
			iCount = sql_count("sub_dd_mst_hst","tx_date=%ld and trace_no=%ld and ac_no='%s' and sub_ac_seqn=%ld",g_pub_tx.tx_date, T_sub_dd_mst_hst.trace_no,T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			if(iCount >0)
			{
				if(T_sub_dd_mst_hst.add_ind[0]=='1')
				{
					ret = sql_execute("update sub_dd_mst_hst set bal=bal-%lf \
						 where tx_date=%ld and trace_no>%ld and ac_no='%s'  \
						 and sub_ac_seqn=%d",T_sub_dd_mst_hst.tx_amt, \
						g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no, \
						T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
				}else if(T_sub_dd_mst_hst.add_ind[0]=='0')
				{
					ret = sql_execute("update sub_dd_mst_hst set bal=bal+%lf \
						 where tx_date=%ld and trace_no>%ld and ac_no='%s'  \
						 and sub_ac_seqn=%d",T_sub_dd_mst_hst.tx_amt, \
						g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no, \
						T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
				}else{
					vtcp_log("%s,%d,������־����!",__FILE__,__LINE__);
					strcpy( g_pub_tx.reply,"H034" );
					goto ErrExit;

				}
				if(ret)
				{
						vtcp_log("%s,%d,���¼�¼����!",__FILE__,__LINE__);
						strcpy( g_pub_tx.reply,"D013" );
						goto ErrExit;
				}
			}
			T_sub_dd_mst.hst_cnt--;
			ret=Sub_dd_mst_Upd_Upd(T_sub_dd_mst,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Upd_Upd����%d��",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			Sub_dd_mst_Clo_Upd();
			/* ���������ˮ�ȳ�����ˮ����������Ҫ������ˮ���by Wang Yongwei 20101215*/
			/* delete begin by wzs 20120321 reason: DBS����ʹ�ò��� */
			/* ret=Sub_dd_mst_hst_Del_Upd(g_pub_tx.reply); */
			/* delete end by wzs 20120321 */
			sql_execute("delete sub_dd_mst_hst where tx_date=%ld and trace_no=%ld and ac_no='%s' and sub_ac_seqn=%d and trace_cnt=%d", \
									g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no,T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn,ptrace_log->trace_cnt);
			/****������ɾ��ԭ���ļ�¼****/
			/******ɾ����ǰ��¼ by Wang Yongwei 20101215
			ret=sql_execute( "delete from sub_dd_mst_hst where tx_date=%ld and  trace_no=%ld and trace_cnt=%d and tel='%s' and ac_no='%s' and sub_ac_seqn=%d",g_pub_tx.tx_date,ptrace_log->trace_no,ptrace_log->trace_cnt,ptrace_log->tel,ptrace_log->ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			******/
			if ( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		Sub_dd_mst_hst_Clo_Upd();
		memcpy(ptrace_log->ac_no,cSub_ac_no,sizeof(ptrace_log->ac_no)-1);
		break;
	default:
		sprintf(acErrMsg,"�������̶������[%c]",do_mode);
		WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
		goto ErrExit;
		break;
	}
	OkExit:
		return 0;
	ErrExit:
		Sub_dd_mst_Clo_Upd();
		return -1;
}

/*�жһ�Ʊ��ֵǼǱ�*/
static int Rol_mo_bank_split_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_split_po_c	T_mo_bank_split_po;
	memset(&T_mo_bank_split_po,0x00,sizeof(struct mo_bank_split_po_c));

	switch( do_mode )
	{
		case '1' :	/*** ���Ʊ�ݹ����� ***/
			ret=Mo_bank_split_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_split_po_Fet_Upd(&T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_split_po.sts[0]!='1' )
			{
				sprintf(acErrMsg,"��¼��״̬����������¼��[%s]",T_mo_bank_split_po.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D254" );
				goto ErrExit;
			}
			ret=Mo_bank_split_po_Del_Upd(T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_split_po_Clo_Upd();
			break;
		case '2' :	/*** ���Ʊ�ݹ����ջس��� ***/
			ret=Mo_bank_split_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_split_po_Fet_Upd(&T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_split_po.sts[0]!='2' )
			{
				sprintf(acErrMsg,"���ջ�״̬�����������ջ�[%s]",T_mo_bank_split_po.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D255" );
				goto ErrExit;
			}
			ret=Mo_bank_split_po_Del_Upd(T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_split_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*�жһ�Ʊ��Ѻ�浥�ǼǱ�*/
static int Rol_impa_dep_reg( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	switch( do_mode )
	{
		case '1' :	/*** ���Ʊ�ݹ����� ***/
			pub_base_strpack(ptrace_log->note_no);
			ret = sql_execute("delete  from impa_dep_reg WHERE pact_no='%s'",ptrace_log->note_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*��֤���Ϣ�ǼǱ�*/
static int Rol_mo_bail_rate( struct trace_log_c *ptrace_log, char do_mode )
{
	struct	mo_bail_rate_c	T_mo_bail_rate;
	struct	mo_bail_rate_c	T_mo_bail_rate1;
	char	flag[2];
	int	ret=0;

	strcpy(flag,"");
	switch( do_mode )
	{
		case '2' :	/*** �жұ�֤��֧ȡ���� ***/
			if( ptrace_log->note_type[0]=='Y' )	/** ��Ϣ֧ȡ **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='1' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='1' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->ac_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='1' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		case '3' :	/*** ������֤��֧ȡ���� ***/
			if( ptrace_log->note_type[0]=='Y' )	/** ��Ϣ֧ȡ **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->ac_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		case '4' :	/*** ���������⳥���� ***/
			if( ptrace_log->note_type[0]=='Y' )	/** ��Ϣ֧ȡ **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->note_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->note_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->note_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*�ͣɣӽ�ݺŶ��ձ�ǼǱ�*/
static int Rol_mo_po_co( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	switch( do_mode )
	{
		case '1' :	/*** �жһ�Ʊ¼�볷�� ***/
			pub_base_strpack(ptrace_log->note_no);
			ret = sql_execute("delete  from mo_po_co WHERE pact_no='%s'",ptrace_log->ac_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"�������̶������[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
int iFindSendRecord(char* cmtno,char * txday,char *orderno,char *brno,char *wssrno)
{
	FILE * fpr;
	char cFilename[513];
	char cBasename[15];
	char cRecordLine[MAXLINE+1];
	char *p;
	char cTxday[9];
	struct STR_BaseSend str_sendRecord;
	int iLine=0;
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(cTxday,txday,sizeof(cTxday)-1);
	memset(&str_sendRecord, 0 , sizeof(str_sendRecord));
	memset(cFilename, 0 ,sizeof(cFilename));
	memset(cBasename, 0 , sizeof(cBasename));
	memset(cRecordLine, 0 ,sizeof(cRecordLine));

	memcpy(cBasename,"sd",2);
	memcpy(cBasename+2,cTxday,8);
	memcpy(cBasename+10,".txt",4);
	sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
	vtcp_log("[%s][%d] r+ file=[%s] \n",__FILE__,__LINE__,cFilename);
	fpr=fopen(cFilename,"a+");
	if(fpr==NULL)
	{
		vtcp_log("[%s][%d] open [%s] error\n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	iLine=0;
	while(!feof(fpr))
	{
		memset(cRecordLine, 0 , sizeof(cRecordLine));
		memset(&str_sendRecord, 0 , sizeof(str_sendRecord));
		fgets(cRecordLine,MAXLINE+1,fpr);
		memcpy((char *)&str_sendRecord,cRecordLine,MAXLINE);
		vtcp_log("%s",(char*)&str_sendRecord);
		vtcp_log("wssrno=[%s]",wssrno);
		if(!memcmp(str_sendRecord.wssrno,wssrno,sizeof(str_sendRecord.wssrno)))
		{
			memcpy(orderno,str_sendRecord.orderno,sizeof(str_sendRecord.orderno));
			vtcp_log("[%s][%d] �ñ�brno=[%s]wssrno[%s]txday[%s]cmtno[%s]orderno=%s \nҵ���Ѿ����͹���������ȡ��,line=[%d]\n",
				__FILE__,__LINE__,brno,wssrno,txday,cmtno,orderno,iLine);
			fclose(fpr);
			return 1;
		}
		/*
		vtcp_log("[%d][%s]\n",iLine,cRecordLine);
		*/
		iLine++;
		continue;
	}
	fclose(fpr);
	return 0;
}

/***С����ʴ���ȡ���Ǽǲ�***/
Rol_Lv_pkgreg_Errdeal(struct trace_log_c *ptrace_log, char do_mode )
{
  vtcp_log("[%s][%d]����С����ʴ�����ȡ������!\n",__FILE__,__LINE__);
  int   iRet=0;
  struct   lv_pkgreg_c sLv_pkgreg;
  char    cReply[5];
  char    cWssrno[7];

  memset(cReply, 0 , sizeof(cReply));
  memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
  memset(cWssrno, 0 , sizeof(cWssrno));

  sprintf(cWssrno,"%d",ptrace_log->trace_no);

  iRet=Lv_pkgreg_Dec_Upd(cReply,"lw_ind=2 and deal_tel ='%s' and deal_trace_no='%s' and filler like '%%%d%%' ",ptrace_log->tel,cWssrno,ptrace_log->tx_date);
  if(iRet)
  {
    vtcp_log("%s,%d û���ҵ��ñ���ˮ��Ӧ��С����ʴ���ҵ��[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }
  iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg, cReply );
  if(iRet)
  {
    vtcp_log("%s,%d û���ҵ��ñ���ˮ��Ӧ��С����ʴ���ҵ��[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }

  vtcp_log("[%s][%d]sLv_pkgreg.lv_sts[0]=[%c]=STAT_LZRECV2=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0],STAT_LZRECV2);


  if(sLv_pkgreg.lv_sts[0]!=STAT_LZRECV2)
  {
    vtcp_log("%s,%d �ñ�С���״̬=[%c]������ȡ��",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
    strcpy(g_pub_tx.reply,"P057");
    goto ErrExit;
  }
  else
  {
    vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sLv_pkgreg.deal_trace_no=[%ld]\n",
      __FILE__,__LINE__,ptrace_log->trace_no,apatoi(sLv_pkgreg.deal_trace_no,sizeof(sLv_pkgreg.deal_trace_no)-1));
    if(ptrace_log->trace_no!=apatoi(sLv_pkgreg.deal_trace_no,sizeof(sLv_pkgreg.deal_trace_no)-1))
    {
        vtcp_log("%s,%d ��С���״̬=[%c]��ˮ�Ŵ�",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
        strcpy(g_pub_tx.reply,"P074");
        goto ErrExit;
    }
    sLv_pkgreg.lv_sts[0]=sLv_pkgreg.beg_sts[0];
  }
  vtcp_log("%s,%d lvsts==[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
   iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg, cReply);
  if(iRet)
  {
    vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
    strcpy(g_pub_tx.reply,"P055");
    goto ErrExit;
  }
  Lv_pkgreg_Clo_Upd();
  vtcp_log("[%s][%d]cmtno==[%.3s],lv_sts=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno,sLv_pkgreg.lv_sts[0]);

OkExit:
  return 0;
ErrExit:
  return -1;
}
Rol_Hv_zf_Errdeal(struct trace_log_c *ptrace_log, char do_mode)
{
  vtcp_log("[%s][%d]��������ʴ�����ȡ������!\n",__FILE__,__LINE__);
  int   iRet=0;
  struct   hv_zf_c sHv_zf;
  char    cReply[5];
  char    cWssrno[7];

  memset(cReply, 0 , sizeof(cReply));
  memset(&sHv_zf, 0 , sizeof(sHv_zf));
  memset(cWssrno, 0 , sizeof(cWssrno));

  sprintf(cWssrno,"%d",ptrace_log->trace_no);

  iRet=Hv_zf_Dec_Upd(cReply,"lw_ind=2 and chk ='%s' and chk_trace_no='%s' and dealerr_date = '%d' ",ptrace_log->tel,cWssrno,ptrace_log->tx_date);
  if(iRet)
  {
    vtcp_log("%s,%d û���ҵ��ñ���ˮ��Ӧ��С����ʴ���ҵ��[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }
  iRet=Hv_zf_Fet_Upd( &sHv_zf, cReply );
  if(iRet)
  {
    vtcp_log("%s,%d û���ҵ��ñ���ˮ��Ӧ��С����ʴ���ҵ��[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }

  vtcp_log("[%s][%d]sHv_zf.hv_sts[0]=[%c]=RECEIVE_ACCT_QS=[%c]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0],RECEIVE_ACCT_QS);


  if(sHv_zf.hv_sts[0]!=RECEIVE_ACCT_QS)
  {
    vtcp_log("%s,%d �ñʴ���״̬=[%c]������ȡ��",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
    strcpy(g_pub_tx.reply,"P057");
    goto ErrExit;
  }
  else
  {
    vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sHv_zf.chk_trace_no=[%ld]\n",
      __FILE__,__LINE__,ptrace_log->trace_no,sHv_zf.chk_trace_no);
    if(ptrace_log->trace_no!=sHv_zf.chk_trace_no)
    {
        vtcp_log("%s,%d �ô���״̬=[%c]��ˮ�Ŵ�[%ld]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0],sHv_zf.chk_trace_no);
        strcpy(g_pub_tx.reply,"P074");
        goto ErrExit;
    }
    sHv_zf.hv_sts[0]=sHv_zf.beg_sts[0];
  }
  vtcp_log("%s,%d lvsts==[%c]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
  iRet=Hv_zf_Upd_Upd( sHv_zf, cReply);
  if(iRet)
  {
    vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
    strcpy(g_pub_tx.reply,"P055");
    goto ErrExit;
  }
  Hv_zf_Clo_Upd();
  vtcp_log("[%s][%d]cmtno==[%.3s],hv_sts=[%c]\n",__FILE__,__LINE__,sHv_zf.cmtno,sHv_zf.hv_sts[0]);

OkExit:
  return 0;
ErrExit:
  return -1;
}
Rol_Mo_int_tax_hst(struct trace_log_c *ptrace_log, char do_mode)
{
	int   ret=0;
	struct	mo_int_tax_hst_c mo_int_tax_hst;
	char    cReply[5];

	vtcp_log("[%s][%d]ȡ����Ϣ˰�Ǽǲ��������!\n",__FILE__,__LINE__);
	memset(cReply, 0 , sizeof(cReply));
	memset(&mo_int_tax_hst,'\0',sizeof(mo_int_tax_hst));
	ret=Mo_int_tax_hst_Dec_Upd(cReply,"trace_no='%d' and tx_date='%ld' ",ptrace_log->trace_no,ptrace_log->tx_date);
  if(ret && ret!=100)
  {
    vtcp_log("%s,%d û���ҵ��ñ���ˮ��Ӧ�ļ�¼[%d]",__FILE__,__LINE__,ret);
    goto ErrExit;
  }
	while(1)
	{
		ret=Mo_int_tax_hst_Fet_Upd(&mo_int_tax_hst,g_pub_tx.reply);
		if(ret==100)
			break;
		Mo_int_tax_hst_Del_Upd(mo_int_tax_hst,g_pub_tx.reply);
	}
	Mo_int_tax_hst_Clo_Upd();

OkExit:
  return 0;
ErrExit:
  return -1;
}
/** ���б�Ʊ�Ǽǲ�  YHBP **/
Rol_Cashier(struct trace_log_c *ptrace_log, char do_mode )
{
    int iRet=0;
    char cPono [13];
    char cTemp [21];
    char cReply[5 ];
    struct cashier_c sCashier;

    memset(cPono , 0 , sizeof(cPono));
    memset(cTemp , 0 , sizeof(cTemp));
    memset(cReply, 0 , sizeof(cReply));
    memset(&sCashier,0 ,sizeof(sCashier));

    memcpy(cPono,ptrace_log->note_no,sizeof(ptrace_log->note_no));
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+8,cPono,sizeof(cPono)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);

    if(ptrace_log->svc_ind==7021)
    {
        vtcp_log("[%s][%dƾ֤��鲻Ҫ���޸ĵǼǱ�\n",__FILE__,__LINE__);
        goto OkExit;
    }
    /* �ҵ������׻�����Ӧ��֧��ϵͳ�к� */
    vtcp_log("[%s][%d]�������б�Ʊȡ������!\n",__FILE__,__LINE__);

    iRet=Cashier_Dec_Upd(cReply," pono='%s' ",cTemp);/** ��Ʊ����Ψһ�����Ը���ôд��������**/
    if(iRet)
    {
        vtcp_log("%s,%d ���屾Ʊ�Ǽǲ��α����[%d]",__FILE__,__LINE__,iRet);
        goto ErrExit;
    }
    iRet=Cashier_Fet_Upd( &sCashier, cReply );
    if(iRet)
    {
        vtcp_log("%s,%d û���ҵ�Ҫ�����Ļ�Ʊ��Ϣ[%d]",__FILE__,__LINE__,iRet);
        strcpy(g_pub_tx.reply,"P001");
        goto ErrExit;
    }
    Cashier_Debug(&sCashier);

    vtcp_log("[%s][%d]tx_code=[%s]stat[0]==[%c]\n",__FILE__,__LINE__,ptrace_log->tx_code,sCashier.stat[0]);
    if(!memcmp(ptrace_log->tx_code,"6343",4) )/**��Ʊǩ������**/
    {
        if(sCashier.stat[0]!='1')/**�������ǩ��״̬**/
        {
             vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���",__FILE__,__LINE__,sCashier.stat[0]);
             strcpy(g_pub_tx.reply,"P000");
             goto ErrExit;
        }
        else
        {
             sCashier.stat[0]='7';    /**ǩ��ȡ��**/
             sCashier.lactcls[0]='1'; /**ǩ��״̬**/
        }
     }
     else if(!memcmp(ptrace_log->tx_code,"6341",4) || !memcmp(ptrace_log->tx_code,"6342",4))/**��Ʊ�Ǽǽ���**/
     {
        if(sCashier.stat[0]!='0')
        {
            vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���",__FILE__,__LINE__,sCashier.stat[0]);
            strcpy(g_pub_tx.reply,"P000");
            goto ErrExit;
        }
        else
        {
            sCashier.stat[0]='8';    /**�Ǽ�ȡ��**/
            sCashier.lactcls[0]='0'; /**�Ǽ�״̬**/
        }
    }
    /******add by NEWBP3*****/
    else if (!memcmp(ptrace_log->tx_code,"6344",4))
	  {
	  	  if(sCashier.stat[0]!='2')/**������ǶҸ�״̬**/
	  	  {
	  	  	  vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	sCashier.stat[0]='1';    /**�Ҹ�ȡ�� modified by robbit 20080405**/
	  	  	sCashier.lactcls[0]='0'; /**¼��״̬**/
	  	  }
	  }
	  else if (!memcmp(ptrace_log->tx_code,"6346",4))
	  {
	  	  if(sCashier.stat[0]!='6')/**���������Ʊ״̬**/
	  	  {
	  	  	  vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	  sCashier.stat[0]='1';    /**��Ʊȡ�� modified by robbit 20080405**/
	  	  	  sCashier.lactcls[0]='0'; /**¼��״̬**/
	  	  }
	  }
	  else if (!memcmp(ptrace_log->tx_code,"6345",4))
	  {
	  	  if(sCashier.stat[0]!='3')/**������ǽ⸶״̬**/
	  	  {
	  	  	  vtcp_log("%s,%d ��Ʊ״̬=[%c]�����Գ���",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	  sCashier.stat[0]='2';    /**��Ʊȡ�� modified by robbit 20080405**/
	  	  	  sCashier.lactcls[0]='1'; /**¼��״̬**/
	  	  }
	  }
    /****end by NEWBP3***/
    else
    {
        vtcp_log("%s,%d �����б�Ʊҵ��[%s]",__FILE__,__LINE__,ptrace_log->tx_code);
        goto ErrExit;
    }
    iRet=Cashier_Upd_Upd(sCashier, cReply);
    if(iRet)
    {
        vtcp_log("%s,%d ���´��Ǽǲ�����[%d]",__FILE__,__LINE__,iRet);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]�޸Ļ�Ʊ״̬�ɹ���!\n",__FILE__,__LINE__);
OkExit:
    vtcp_log("[%s][%d]��Ʊȡ���ɹ� �ǼǱ�!\n",__FILE__,__LINE__);
    return 0;
ErrExit:
    return -1;
}
