#include "find_debug.h" 
/*************************************************
* �� �� ��:  pubf_base_product.c
* ����������
*           ���� ���ڲ�Ʒ���Լ�麯�� pub_base_check_current();	
* 				 ���ڲ�Ʒ���Լ�麯�� pub_base_check_terminate();			
*                ���ڲ�Ʒ������� pub_base_change_current();	
* 				 ���ͻ������빺���Ʒ�����Ƿ�������� pub_base_check_cltprdt;
* 				 �����ʻ�ID������˻����ID���� pub_base_get_cltid();			
* 				 �����ڲ����ʺ�ID���� pub_base_get_inid();			
*		 		 ȡ�ͻ��˻���� pub_base_get_cltidseq();
*				 ���ݲ�Ʒ����ȡ��Ʒ��������	pub_base_get_prdtparm();�
*				 ��ʾ�˺�ת�����˺�ID�ĺ��� pub_base_disid_cltid();
*				 ���˺�ת�������˺ź�����pub_base_oldac_newac();
*                ���˻�ID���˻���Ų�ѯ��Ʒ����    pub_base_get_prdt_name
* 				 �ɲ�Ʒ�Ų�ѯ��Ʒ����	pub_base_prdt_name    
* 				 ���ݿ�Ŀ������ȡ��Ŀ��Ϣ	pub_base_item_inf    
* 				 ����ƾ֤����ȡƾ֤����    	pub_base_get_note_name   
* ��    ��: rob
* ������ڣ�2003/12/27
* �޸ļ�¼��  
* 1. ��   ��:
*    �� �� ��:
*    �޸�����:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"
#include "clt_id_seq_c.h"
#include "in_id_seq_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mo_merge_ac_c.h"
#include "mdm_unprt_hst_c.h"
#include "ac_seqn_id_c.h"
#include "com_item_c.h"
#include "note_parm_c.h"

struct  pub_tx f_pub_tx;
struct  dd_parm_c f_dd_parm;
struct  dd_mst_c f_dd_mst;
struct  td_mst_c f_td_mst;
struct  td_mst_hst_c f_td_mst_hst;
struct  clt_id_seq_c g_clt_id_seq;					/*�ͻ��˻�id�ṹ*/
struct  in_id_seq_c g_in_id_seq;					/*�ڲ��˻�id�ṹ*/
struct  cif_basic_inf_c g_cif_basic_inf;			/*�ͻ�������Ϣ�ṹ*/
struct  cif_id_code_rel_c g_cif_id_code_rel;		/*�ͻ�֤����ͻ��Ŷ��սṹ*/
struct  prdt_ac_id_c g_prdt_ac_id;					/*��Ʒ�˺Ŷ��սṹ*/
struct  mo_merge_ac_c g_mo_merge_ac;				/*�¾��˺Ŷ��սṹ*/
struct  mdm_unprt_hst_c g_mdm_unprt_hst;			/*δ��ӡ��ϸ�˽ṹ*/
struct  ac_seqn_id_c ac_seqn_id;					/*�˻����ID��ṹ*/
struct  com_item_c com_item;						/* ������Ŀ�� */
struct	note_parm_c	note_parm;						/* ƾ֤������ */

static int ret=0;

/**********************************************************************
 * �� �� ����pub_base_check_current();
 * �������ܣ����ڲ�Ʒ���Լ�麯��
 *           
 * ����/ʱ�䣺
 * 
 * ������
 *    ���룺
 *       	��������		char(1);	
 *		
 *     ���: 
 *           
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_check_current(tran_type)
	char *tran_type;
{
	/*���ݻ��ڲ�Ʒ���ȡ���ڲ�����*/
	MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ�������в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*���ݽ�������,�����ز���*/
	if (tran_type[0]=='1')							/*1-����*/
	{
		/*��鿪�������Ƿ�����ʼ���ں���ֹ����֮��*/	
		if ( (g_dd_parm.beg_date > g_dd_mst.opn_date) || \
			(g_dd_mst.opn_date > g_dd_parm.end_date))
		{	
			strcpy(acErrMsg,"open date is not between begin and end date!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			return 1;
		}

		/*��鿪��ͨ�ұ�־*/
		if (g_dd_parm.thr_opn_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A002" );
			return 1;
		}

		/*���ò�Ʒ��Ϳ���������߿������*/
		if ( g_dd_mst.bal<g_dd_parm.min_opn_amt )
		{
			strcpy(acErrMsg,"open bal is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			return 1;
		}
		else if ( g_dd_mst.bal>g_dd_parm.max_opn_amt)
		{
			strcpy(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			return 1;
		}

		/*��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
		if ( g_dd_mst.rate < g_dd_mst.rate - g_dd_parm.min_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			return 1;
		}
		else if ( g_dd_mst.rate > g_dd_mst.rate + g_dd_parm.max_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			return 1;
		}

		/* ��鿪�������������� (���ú���) */
		if ( g_dd_parm.opn_lmt == 000 )
		{
			return 1;
		}
 	}
	else if(tran_type[0]=='2')						/*2-����*/
	{
		/*�������ͨ�ұ�־*/
		if ( !strcmp( g_dd_parm.thr_cls_ind,"N") )
		{	
			strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A007" );
			return 1;
		}
		/* ������������������� (���ú���) */
		if ( g_dd_parm.cls_lmt == 000 )
		{
			
		}
	}
	else if (tran_type[0]=='3')				/*3-���*/
	{
		MEMSET_DEBUG( &f_dd_mst, 0x00, sizeof(struct dd_mst_c) );

		ret = Dd_mst_Sel(g_pub_tx.reply, &f_dd_mst,
						"prdt_no='%s' and ac_id=%d and ac_seqn=%d",
						g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
		if ( ret==100 )
		{
			sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W014" );
			return 1;
		}
		else if ( ret!=0 )
		{
			sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W015" );
			return 1;
		}

		/*���������Ƿ񳬹������*/
		if( ( g_dd_mst.bal+g_pub_tx.tx_amt1) > g_dd_parm.max_bal )	
		{
			strcpy(acErrMsg,"amt is too high!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A008" );
			return 1;
		}

		/*��������־*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*���ͨ���־*/
		if ( g_dd_parm.thr_dpst_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A010" );
			return 1;
		}

		/* ����������������� (���ú���)
		if ( g_dd_parm.cls_lmt == 000 )
		{

		} */
	}
	else if(tran_type[0]=='4')							/*4-ȡ��*/
	{
		/*�����ȡ��־*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*���ͨ�ұ�־*/
		if (g_dd_parm.thr_draw_ind[0]=='N')
		{
			strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A011" );
			return 1;
		}

		/*������ֱ�־*/
		if (g_pub_tx.ct_ind[0]=='1')
		{
			if(g_dd_parm.cash_ind[0]=='N')
			{
				strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A012" );
				return 1;
			}
		}
	}

   return 0;
}

/**********************************************************************
 * �� �� ����pub_base_check_terminate();
 * �������ܣ����ڲ�Ʒ���Լ�麯��
 *           
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺
 *           �������� 				tran_type(2);
 *     ���: 
 *         	 ��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_check_terminate(tran_type)
	char *tran_type;
{
	int days;

	/*���ݶ��ڲ�Ʒ���ȡ���ڲ�Ʒ����*/
	MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
					"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ�������в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W026" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W027" );
		return 1;
	}

	/*�����˻�id,�˻����ȡ���ڲ�Ʒ���ļ�*/
	MEMSET_DEBUG( &f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret = Td_mst_Sel(g_pub_tx.reply, &f_td_mst,
 					"prdt_no='%s' and ac_id=%ld and ac_seqn=%d",
					g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ���ļ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W016" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ���ļ��쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W017" );
		return 1;
	}

	/*�����˻�id,�˻����ȡ���ڴ����ϸ��*/
	MEMSET_DEBUG( &f_td_mst_hst, 0x00, sizeof(struct td_mst_hst_c) );

	ret = Td_mst_hst_Sel(g_pub_tx.reply, &f_td_mst_hst,
						"ac_id=%ld and ac_seqn=%d and max(trace_no)",
						g_pub_tx.ac_id , g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ��ϸ���в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W028" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ��ϸ���쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W029" );
		return 1;
	}

	switch( atoi(tran_type) )
	{
	  case 1:										/*1-����*/
			/*��鿪�������Ƿ�����ʼ���ں���ֹ����֮��*/	
			if ( (g_td_parm.beg_date > g_td_mst.opn_date) || \
				(g_td_mst.opn_date > g_td_parm.end_date))
			{	
				strcpy(acErrMsg,"open date is not between begin and end date!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A001" );
				break;
			}

			/*���ͨ�ҿ�����־*/
			if (g_td_parm.thr_opn_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A002" );
				break;
			}

			/*���ò�Ʒ��Ϳ���������߿������*/
			if ( g_td_mst.bal<g_td_parm.min_opn_amt )
			{
				strcpy(acErrMsg,"open bal is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A003" );
				break;
			}
			else if ( g_td_mst.bal>g_td_parm.max_opn_amt)
			{
				strcpy(acErrMsg,"open bal overflow!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A004" );
				break;
			}

			/*��������Ƿ񳬹��涨�����ϸ����¸���Ľ��� */
			if ( g_td_mst.rate < g_td_mst.rate - g_td_parm.min_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				break;
			}
			else if ( g_td_mst.rate > g_td_mst.rate + g_td_parm.max_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				break;
			}

			/* ��鿪�������������� */
			if ( g_td_parm.opn_lmt == 000 )
			{
				break;
			}
			break;
	case 2:										/*2-����*/
			/*���ԤԼת���־*/
			if (g_td_parm.tfr_lmt_ind[0]=='N')
			{	
				if (g_td_mst.tfr_ind[0]=='Y')
				{
					strcpy(acErrMsg,"tfr_ind is N,transfer failed!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A013" );
					break;
				}
			}
			
			/*����Ƿ���ǰ֧ȡ*/
			if (g_td_parm.fwd_draw_ind[0]=='N')
			{
				if ( g_pub_tx.tx_date < f_td_mst.mtr_date )
				{
					strcpy(acErrMsg,"advance take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A014" );
					break;
				}
			}	

			/*�������ͨ�ұ�־*/
			if (g_td_parm.thr_cls_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A007" );
				break;
			}

			/*�������������������*/
			if ( g_td_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	 case 3:									/*3-���*/
			/*���������Ƿ񳬹������*/
			if( ( g_td_mst.bal + g_pub_tx.tx_amt1) > g_td_parm.max_bal )	
			{
				strcpy(acErrMsg,"amt is too high!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A008" );
				break;
			}

			/*��������־*/
			if (g_td_parm.dpst_ind[0]=='N')
			{
				strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A009" );
				break;
			}
			else if ( (g_td_parm.dpst_ind[0]=='Y') || \
					 (g_td_parm.dedr_intvl != 0) )
			{
				if( g_td_parm.intvl_type[0]=='Y')	
				{
					days = 365 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='Q')
				{
					days = 90 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='M')
				{
					days = 30 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( (g_pub_tx.tx_date - f_td_mst_hst.tx_date) < days )	
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*������������*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.dpst_min_amt )
			{
				strcpy(acErrMsg,"save money is too low,save failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*���ͨ���־*/
			if (g_dd_parm.thr_dpst_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A010" );
				break;
			}

			/* �����������������*/
			if ( g_dd_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	case 4:									/*4-ȡ��*/
			/*����˺�֧ȡ������Ƿ���ڲ�Ʒ������*/
			if ( (g_td_mst.bal - g_pub_tx.tx_amt1) < g_td_parm.max_bal )
			{
				strcpy(acErrMsg,"after take,remain is not enough!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A017" );
				break;
			}

			/*��������ȡ���*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.draw_min_amt )
			{
				strcpy(acErrMsg,"draw cash is too low,draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*�����ȡ����*/
			if ( g_td_parm.dedr_intvl != 0 )
			 {
				if(g_td_parm.intvl_type[0]=='Y')
				{
				 	days = 365 * g_td_parm.dedr_intvl;
				}
			    else if(g_td_parm.intvl_type[0]=='Q')
			  	{
				    days = 90 * g_td_parm.dedr_intvl;
				}
		 		else if(g_td_parm.intvl_type[0]=='M')
			    {
				   	days = 30 * g_td_parm.dedr_intvl;
				}
				else if(g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( ( g_pub_tx.tx_date - f_td_mst_hst.tx_date )<days )
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*���ͨ�ұ�־*/
			if ( g_td_parm.thr_draw_ind[0]=='N')
			{
				strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A011" );
				break;
			}

			/*������ֱ�־*/
			if (g_pub_tx.ct_ind[0]=='1')
			{
				if(g_td_parm.cash_ind[0]=='N')
				{
					strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A012" );
					break;
				}
			}	
			break;
	case 5:									/*5-����*/
			/*��鲿���־*/
			if(g_td_parm.wdr_ind[0]=='N')
			{
				strcpy(acErrMsg,"take part cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A018" );
				break;
			}

			/*��鲿�����*/
			if (g_td_parm.wdr_lmt_cnt != 0 && \
				g_td_mst.tx_cnt == g_td_parm.wdr_lmt_cnt)
			{
				strcpy(acErrMsg,"take part cash times is full!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A019" );
				break;
			}

	default:break;		

	}
 return 0;
}

/**********************************************************************
 * �� �� ����pub_base_change_current();
 * �������ܣ����ڲ�Ʒ�������
 *           ���˻���Ӧ�Ļ��ڲ�Ʒ�ı�
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺 �˻�id		long;
 *     		  �˻����		int;
 *     ���: 
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_change_current( ac_id, ac_seq )
	long ac_id;
	int ac_seq; 
{
	char tran_tp[2];

	/*�����ʻ�ID���˻�����ҵ�ԭ���ڲ�Ʒ���*/
	MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );

	ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seq=%d",
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ���ļ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W014" );
		return 1;
	}
	else if (ret && ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ���ļ��쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W015" );
		return 1;
	}

	/*����ԭ���ڲ�Ʒ���ȡ��ԭ���ڲ�Ʒ�����ṹ*/
	MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
						g_dd_mst.prdt_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ�������в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*���ɻ��ڲ�Ʒ�Ƿ�����»��ڲ�Ʒ����*/
	tran_tp[0]='1';
	ret = pub_base_check_current(tran_tp);

	/*���ݲ�Ʒ���ȡ�²�Ʒ����*/
	MEMSET_DEBUG( &f_dd_parm, 0x00, sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &f_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ�������в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}
	
	/*�޸Ļ��ڲ�Ʒ���ļ�*/
	ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d", 
							g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret )
	{
		sprintf(acErrMsg,"declare Dd_mst_Dec_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D101" );
		return 1;
	}

	ret = Dd_mst_Fet_Upd( &g_dd_mst,g_pub_tx.reply );
	if ( ret&&ret!=100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D102" );
		return 1;
	}
	else if ( ret==100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D104" );
		return 1;
	}

	strcpy( g_dd_mst.prdt_no,g_pub_tx.prdt_code );

	ret = Dd_mst_Upd_Upd( g_dd_mst, g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"update Dd_mst_Upd_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D105" );
		return 1;
	}

	Dd_mst_Clo_Upd( );

	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_check_cltprdt();
 * �������ܣ����ͻ������빺���Ʒ�����Ƿ��������
 *			 ����˿ͻ����ܹ���Թ���Ʒ���Թ��ͻ����ܹ�����˲�Ʒ��
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺	�ͻ�����		clt_type	char(2);
 *          	��Ʒ���		prdt_no		char(4);
 *     ���: 
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_check_cltprdt( clt_type, prdt_no )
	char *clt_type;
	char *prdt_no;
{
	char prdt_seqn[2];
	char str_tt[41];

	struct ln_parm_c	ln_parm_cif;
	prdt_seqn[0]=prdt_no[0];

	MEMSET_DEBUG( &ln_parm_cif, 0x00, sizeof(struct ln_parm_c) );
	MEMSET_DEBUG( str_tt, 0x0 , sizeof( str_tt ) );
	
	switch(atoi(prdt_seqn))
	{
		case 1:
				/*��ѯ���ڲ�����*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"prdt_no='%s' and cif_type='%s'",
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"���ڲ�Ʒ�������в����ڸü�¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					return 1;
				}
				else if ( ret && ret!=0 )
				{
					sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					return 1;
				}
				break;
		case 2:
				/*��ѯ���ڲ�����*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );
				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
									"prdt_no='%s' and cif_type='%s'", 
									prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"���ڲ�Ʒ�����в����ڸü�¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�������쳣[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 3:
				/*��ѯ���������*/
				if( clt_type[0]=='1' )
				strcpy( str_tt," cif_type in('1','0') " );
				if( clt_type[0]=='2' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='3' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='4' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='5' )
				strcpy( str_tt," cif_type in('1','0') " );

				sprintf(acErrMsg,"str_tt![%s]",str_tt);
				WRITEMSG
				
				ret =Ln_parm_Sel(g_pub_tx.reply, &ln_parm_cif, 
								"prdt_no='%s' and %s ", prdt_no , str_tt );
				if ( ret==100 )
				{
					sprintf(acErrMsg,"�ÿͻ��ͻ�����������Ʒ�ʺϵĿͻ����Ͳ���![%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L146" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"��ѯ�����Ʒ�������쳣[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 4:
				/*��ѯ͸֧������*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );
				ret =Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
								"prdt_no='%s' and cif_type='%s'", 
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"͸֧��Ʒ�������в����ڸü�¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"��ѯ͸֧��Ʒ�������쳣[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		default:
				break;
		}
return 0;
}

/**********************************************************************
 * �� �� ����pub_base_get_cltid();
 * �������ܣ����ɿͻ��ʻ�ID������������˻����ID��
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *				�ͻ��˻�ID		long;
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��lance/20050112
 * �޸�ԭ�����Ӷ�"�˻����ID��"����Ӳ���
 *          
********************************************************************/
int pub_base_get_cltid( cltid )
	long	*cltid;
{
	/*ȡ���ͻ��˺�id,���޸ģ����ؿ������ͻ��˺�id*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	/* ����˻����ID�� */
	ac_seqn_id.ac_id = *cltid;	/* �˺�ID */
	ac_seqn_id.ac_seqn_id = 200;	/* ��ʼ���˻���� */

	ret = Ac_seqn_id_Ins( ac_seqn_id );
	if( ret )
	{
		sprintf(acErrMsg,"����˻����ID��ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W042");
		return 1;
	}
	sprintf(acErrMsg,"����˻����ID�� PASS!");
	WRITEMSG

	return 0;

}

int pub_base_crt_cltid( cltid )
	long	*cltid;
{
	/*ȡ���ͻ��˺�id,���޸ģ����ؿ������ͻ��˺�id*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_get_inid();
 * �������ܣ������ڲ��ʻ�ID����
 *			 
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 
 *			�ڲ��˻�ID		long;
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_get_inid( inid )
	long *inid;
{
	/*ȡ��С�ڲ��˺�id,���޸ģ����ؿ�����С�ڲ��˺�id*/
	ret = In_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = In_id_seq_Fet_Upd (&g_in_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_in_id_seq.ac_seqn = g_in_id_seq.ac_seqn + 1;

	ret = In_id_seq_Upd_Upd (g_in_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	In_id_seq_Clo_Upd( );

	*inid = g_in_id_seq.ac_seqn;

	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_get_cltidseq();
 * �������ܣ�ȡ�ͻ��˻���ź���
 *			 
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺֤������		
 *           ֤������
 *     ���: 
 *			�ͻ��˻����
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_get_cltidseq(id_type,id_num,cif_seq)
	char *id_type;	
	char *id_num;
	long *cif_seq;
{
	/*ȡ�ͻ�����*/
	MEMSET_DEBUG( &g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c) );

	ret = Cif_id_code_rel_Sel (g_pub_tx.reply, &g_cif_id_code_rel, 
					 		"id_type='%s' and id_no='%s'", id_type , id_num);
	if ( ret && ret!=100)
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return 1;
	}
	else if( ret==100 )
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	/*ȡ�ͻ��˺�����޸�,������*/
	ret = Cif_basic_inf_Dec_Upd (g_pub_tx.reply, " cif_no=%d ",
									g_cif_id_code_rel.cif_no);
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Cif_basic_inf_Fet_Upd (&g_cif_id_code_rel,g_pub_tx.reply);
	if (ret && ret!=100)
	{
		sprintf(acErrMsg,"fetch cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_cif_basic_inf.cif_seqn++;

	ret = Cif_basic_inf_Upd_Upd (g_cif_basic_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Cif_basic_inf_Clo_Upd( );

	*cif_seq = g_cif_basic_inf.cif_seqn;

	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_get_prdtparm();
 * �������ܣ����ݲ�Ʒ����ȡ��Ʒ��������
 *			 
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺��Ʒ����		
 *          
 *     ���: 
 *			��Ʒ�������ݽṹ
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_get_prdtparm( char *prdt_no )
{
	char prdt_type[2];

	/*ȡ��Ʒ����*/

	strncpy(prdt_type,prdt_no,1);

    sprintf(acErrMsg,"prdt_type[0]=[%c],[%s]",prdt_type[0],prdt_no);
    WRITEMSG

	switch ( prdt_type[0] )
	{
		case '1':										/*1-����*/
				/*ȡ���ڲ�����*/
				MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
									"prdt_no='%s' order by dc_code", prdt_no);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				break;
		case '2':										/*2-����*/
				/*ȡ���ڲ�����*/
			    sprintf(acErrMsg, "ȡ���ڲ�Ʒ������");
				WRITEMSG
				MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm,"prdt_no='%s' order by dc_code",prdt_no);

				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				break;
		case '3':										/*3-����*/
				/*ȡ���������*/
				MEMSET_DEBUG( &g_ln_parm,0x00,sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm,"prdt_no='%s'", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"SELECT ln_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				break;
		case '4':										/*4-͸֧*/
				/*ȡ͸֧������*/
				MEMSET_DEBUG( &g_od_parm,0x00,sizeof(struct od_parm_c) );

				ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
								prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,"SELECT od_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				break;
		case '9':										/*5-�ڲ���Ʒ*/
				/*ȡ�ڲ���Ʒ������*/
				MEMSET_DEBUG( &g_in_parm,0x00,sizeof(struct in_parm_c) );
				/****Ϊʲô��Dd_parm_Sel  20121128
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
				****/
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				break;
		default:
			    sprintf(acErrMsg, "ȡ��Ʒ���������");
				WRITEMSG
				break;
	}
	return 0;
}
/**********************************************************************
 * �� �� ����pub_base_disid_cltid();
 * �������ܣ���ʾ�˺�ת�����˺�ID�ĺ���
 *			 
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺��ʾ�˺�		
 *          
 *     ���: 
 *			�˺�ID
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_disid_cltid( ac_no , ac_id )
	char *ac_no;
	long *ac_id;
{
	/*������ʾ�˺�ת�����˺�ID*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, 
						"ac_no='%s'", ac_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mdm_ac_rel, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W010" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W011" );
		return 1;
	}

	*ac_id = g_mdm_ac_rel.ac_id;

	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_oldac_newac();
 * �������ܣ����˺�ת�������˺ź����
 *			 
 *
 * ����/ʱ�䣺rob/2003/12/31
 * 
 * ������
 *     ���룺���˺�		
 *          
 *     ���: 
 *			���˺�
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_oldac_newac( old_ac,new_ac )
	char *old_ac;
	char *new_ac;
{
	/*������ʾ�˺�ת�����˺�ID*/
	MEMSET_DEBUG( &g_mo_merge_ac, 0x00, sizeof(struct mo_merge_ac_c) );

	ret = Mo_merge_ac_Sel(g_pub_tx.reply, &g_mo_merge_ac, 
							"old_ac_no='%s'", old_ac);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mo_merge_ac, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W036" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,
		"SELECT mo_merge_ac error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W037" );
		return 1;
	}
	strcpy (new_ac , g_mo_merge_ac.ac_no);

	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_wrt_unprt();
 * �������ܣ��Ǽ�δ��ӡ��ϸ�ʺ���
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺pub_tx 		�������˽ṹ		
 *          
 *     ���: 
 *         	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_wrt_unprt( f_pub_tx )
	struct pub_tx f_pub_tx;
{
	char prdt_type[2];

	g_mdm_unprt_hst.trace_no = f_pub_tx.trace_no;			/*��ˮ��*/
	g_mdm_unprt_hst.trace_cnt = f_pub_tx.trace_cnt;			/*��ˮ�ʴ�*/
	strcpy (g_mdm_unprt_hst.tx_br_no,f_pub_tx.tx_br_no);	/*���׻�����*/	
	g_mdm_unprt_hst.ac_id = f_pub_tx.ac_id;					/*�˺�ID*/
	g_mdm_unprt_hst.ac_seqn = f_pub_tx.ac_seqn;				/*�˻����*/
	strcpy (g_mdm_unprt_hst.add_ind , f_pub_tx.add_ind);	/*������־*/	
	g_mdm_unprt_hst.tx_amt = f_pub_tx.tx_amt1;				/*���׽��*/
	g_mdm_unprt_hst.tx_date = f_pub_tx.tx_date;				/*��������*/
	strcpy (g_mdm_unprt_hst.tel , f_pub_tx.tel);			/*����Ա*/
	strcpy (g_mdm_unprt_hst.brf , f_pub_tx.tel);			/*ժҪ*/

	/*�����˺�id���˻����ID���Ҳ�Ʒ����*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, 
							"ac_id=%ld and ac_seqn=%d", 
							f_pub_tx.ac_id,f_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table prdt_ac_id, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W012" );
		return 1;
	}
	else if ( ret!=0 )
	{
		sprintf(acErrMsg,
		"SELECT prdt_ac_id error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W013" );
		return 1;
	}

	/*ȡ��Ʒ����*/
	strncpy ( prdt_type,g_prdt_ac_id.ac_id_type,1 );

	switch ( atoi(prdt_type) )
	{
		case 1:										/*1-����*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"ac_id='%d' and ac_seqn='%d'", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				g_mdm_unprt_hst.bal = g_dd_mst.bal;				/*���*/
				g_mdm_unprt_hst.hst_cnt = g_dd_mst.hst_cnt;		/*��ϸ����*/

				break;
		case 2:										/*2-����*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				g_mdm_unprt_hst.bal = g_td_mst.bal;				/*���*/
				g_mdm_unprt_hst.hst_cnt = g_td_mst.hst_cnt;		/*��ϸ����*/

				break;
		case 3:										/*3-����*/
				MEMSET_DEBUG( &g_ln_parm, 0x00, sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT ln_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				g_mdm_unprt_hst.bal = g_ln_mst.bal;				/*���*/
				g_mdm_unprt_hst.hst_cnt = g_ln_mst.hst_cnt;		/*��ϸ����*/

				break;
		case 4:										/*4-͸֧*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );

				ret = Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
				"ac_id=%ld and ac_seqn=%d", f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT od_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				g_mdm_unprt_hst.bal = g_od_mst.bal;				/*���*/
				g_mdm_unprt_hst.hst_cnt = g_od_mst.hst_cnt;		/*��ϸ����*/

				break;
		case 9:										/*9-�ڲ���Ʒ*/
				MEMSET_DEBUG( &g_in_parm, 0x00, sizeof(struct in_parm_c) );

				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				g_mdm_unprt_hst.bal = g_in_mst.bal;				/*���*/
				g_mdm_unprt_hst.hst_cnt = g_in_mst.hst_cnt;		/*��ϸ����*/

				break;
		default:
				break;
	}

	return 0;
}

/**********************************************************************
* ��������     	pub_base_get_prdt_name
* �������ܣ�    ���˻�ID���˻���Ų�ѯ��Ʒ����
* ����/ʱ�䣺   lance/2005��07��04��
* 
* ������
*     ���룺 ac_id       ���˻�ID
*            ac_seqn     : �˻����
*           
*     ����� prdt_name   : ��Ʒ����
*
* �� �� ֵ: 0 �ɹ�,1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int pub_base_get_prdt_name( long ac_id, int ac_seqn, char *prdt_name )
{    
	
	MEMSET_DEBUG( &g_prdt_ac_id , 0x00 ,sizeof( struct prdt_ac_id_c ));
	
	g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*��Ʒ����*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
						"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*��Ʒ����*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*��Ʒ����*/							
			return 0;
		default:
			break;
	}/*switch����*/

	return 1;
}
/**********************************************************************
* ��������     	pub_base_prdt_name
* �������ܣ�    �ɲ�Ʒ�Ų�ѯ��Ʒ����
* ����/ʱ�䣺   lance/2005��07��04��
* 
* ������
*     ���룺 prdt_no     ����Ʒ����
*           
*     ����� prdt_name   : ��Ʒ����
*
* �� �� ֵ: 0 �ɹ�,1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int pub_base_prdt_name( char *prdt_no, char *prdt_name )
{    
	
	sprintf( acErrMsg,"prdt_no[%s],prdt_name[%s]",prdt_no,prdt_name);
	WRITEMSG
	if( strcmp(prdt_no,"��")==0 )
	{
		strcpy( prdt_name , "��ʱ����" );
		return 0;
	}
	switch(prdt_no[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"���ڲ����ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*��Ʒ����*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
									prdt_no);
			if( g_reply_int==100 )
			{
				sprintf(acErrMsg,"���ڲ����ļ������ڸü�¼![%s]",prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*��Ʒ����*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"��������ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*��Ʒ����*/							
			return 0;
		case '4':
			g_reply_int=Od_parm_Sel(g_pub_tx.reply,&g_od_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"͸֧�����ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_od_parm.title);			/*��Ʒ����*/							
			return 0;
		case '9':
			g_reply_int=In_parm_Sel(g_pub_tx.reply,&g_in_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"�ڲ��˲����ļ������ڸü�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_in_parm.prdt_mo);			/*��Ʒ����*/							
			return 0;
		default:
			strcpy( g_pub_tx.reply,"D103" );
			break;
	}/*switch����*/

	return 1;
}
/**********************************************************************
* ��������     	pub_base_item_inf
* �������ܣ�    ���ݿ�Ŀ������ȡ��Ŀ��Ϣ
* ����/ʱ�䣺   lance/2005��07��05��
* 
* ������
*     ���룺 	acc_hrt   	   ����Ŀ������
*           
*     ����� 	struct	com_item   : ��Ŀ��Ϣ�ṹ
*
* �� �� ֵ: 0 �ɹ�,1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int pub_base_item_inf( char *acc_hrt, struct com_item_c	*com_item )
{    
	int ret=0;
	
	MEMSET_DEBUG( com_item , 0x00 , sizeof(struct com_item_c) );
	
	sprintf( acErrMsg,"acc_hrt[%s]",acc_hrt);
	WRITEMSG


	ret = Com_item_Sel( g_pub_tx.reply , com_item , " acc_hrt='%s' " , acc_hrt );
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ��Ŀ����Ϣ�쳣����!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );			
		return 1;
	}
	
	return 0;
}

/**********************************************************************
* ��������     	pub_base_get_note_name
* �������ܣ�    ����ƾ֤����ȡƾ֤����
* ����/ʱ�䣺   lance/2005��07��06��
* 
* ������
*     ���룺 	note_type   	   ��ƾ֤����
*           
*     ����� 	note_name	   : ƾ֤����
*
* �� �� ֵ: 0 �ɹ�,1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int pub_base_get_note_name( char *note_type, char *note_name )
{    
	int ret=0;
	
	MEMSET_DEBUG( &note_parm , 0x00 , sizeof( struct note_parm_c ) );
	
	sprintf( acErrMsg,"note_type[%s]",note_type);
	WRITEMSG

	ret = Note_parm_Sel( g_pub_tx.reply , &note_parm , "note_type='%s'" , note_type );
	if( ret==100 )
	{
		sprintf(acErrMsg,"ƾ֤���������޴�ƾ֤!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N085" );			
		return 1;
	}else if( ret ){
		sprintf(acErrMsg,"��ѯƾ֤�������쳣����,�������Ļ�����ϵ!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N086" );			
		return 1;
	}else if( ret==0 ){
		
		/* ���ƾ֤�Ƿ���ʹ��״̬ */
		if( note_parm.use_ind[0] == 'N' )
		{
			sprintf( acErrMsg,"ƾ֤����ͣ��״̬!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N025" );
			return 1;
		}		
	}
	strcpy( note_name , note_parm.name );
	
	return 0;
}
