/***************************************************************
* �� �� ��:     pubf_acct_redo.c	
* ����������	(�������ǹ��ú���)
*              1������˺š��˺�ID���˺���ŵĺϷ��ԣ�
*              2������Ʒ����������˺����ļ����޸����ļ�ȫ�ֱ�����
*              3���Ǽ���ϸ�ʣ�
*              4���Ǽǽ�����ˮ��
**��־�÷���
**��Ҫ�˶�ƾ֤��** 	g_pub_tx.ac_wrk_ind[0]='1'
**					g_pub_tx.hst_ind[0]=='1'�ռ�����ϸ
**					g_pub_tx.prt_ind[0]!='1'�Ƿ����
*
* ��ڲ���:		err_date ��������
*				C_B_flag �������Ǳ�־1-��2-��
*
* ��    ��:		
* ������ڣ�    2003��03��16��
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
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "com_sect_acm_c.h"
#include "tx_dc_br_rel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

struct mdm_attr_c v_mdm_attr;
struct com_sect_acm_c com_sect_acm_c;

pub_acct_redo(char C_B_flag[2])
{
	int ret=0;
	int	ass_ind=0;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c mdm_attr_c;
	struct note_parm_c note_parm_c;
    int factday,lastday;

	/**��ʼ�����ļ�����**/
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
	memset(&com_sect_acm_c,0x00,sizeof(struct com_sect_acm_c));

	/*�����ø�������*/
	if( C_B_flag[0]=='1' )	g_pub_tx.tx_amt1=-g_pub_tx.tx_amt1;

	/*�����Ŀ�����*/
	pub_base_strpack(g_pub_tx.ac_no);
	ret=strlen(g_pub_tx.ac_no);
	if(ret==3||ret==5||ret==7)
	{
		ret=chongzheng_cash();
		ERR_DEAL
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
		ret=Chg_acchrt_mst(&ass_ind);
		ERR_DEAL
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
	/****�岹��Ŀ��ʱ��Ҳ������ˮ modify by wanglei 20070518****/
	/********* 
		if( ass_ind!=1 )
		{
		}
	*********/ 
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
			if( pub_ins_trace_log1() )
			{
				sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
				WRITEMSG
				goto ErrExit;
			}
	}
	else
	{
		ret=pub_sub_ac1();
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"�ʺŲ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		ERR_DEAL
			sprintf(acErrMsg,"��ʼ�����ļ�����PASS!");
			WRITEMSG

		/***��鴦��ֻ���***/
		ret=pub_sub_mst1();
		ERR_DEAL

		/***�����۴���***/
vtcp_log("-===================g_pub_tx.ac_id_type=[%s]",g_pub_tx.ac_id_type);
		ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
		ERR_DEAL

		/***�Ǽǽ�����ˮ***/
		if( g_pub_tx.ac_id_type[0]!='3' && g_pub_tx.hst_ind[0]!='0' )
		{
			/***�����ر���***/
			ret=pub_sub_spe_do1();
			ERR_DEAL

			if( pub_ins_trace_log1() )
			{
				sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
				WRITEMSG
				goto ErrExit;
			}
		}
		sprintf(acErrMsg,"�Ǽǽ�����ˮPASS!");
		WRITEMSG
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
	sprintf(acErrMsg,"�������Ǽ��˴���ɹ�!");
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"�������Ǵ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

int Chg_dd_mst1()
{
	int ret;
	double acm;		/*�ֶ���Ϣ����*/

	/*** �������ļ���� ***/
	ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**��ȡ��Ʒ����**/
	ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
						" prdt_no='%s' ",g_dd_mst.prdt_no );
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);

	strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;
	
	/*���㻧����ת��,�ǽ��㻧����ת��*/
	/**------- xxx ------ ����������**
	if( g_pub_tx.ct_ind[0]=='2' && g_dd_mst.sttl_type[0]=='S' )
	{
		sprintf(acErrMsg,"�ǽ��㻧������ת��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P105");
		goto ErrExit;
	}
	**------------------------------**/

	if( g_dd_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"�˺�״̬������״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	if( g_dd_mst.hold_sts[0]=='1' )
	{
		sprintf(acErrMsg,"���˺��ѱ���ȫ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P110");
		goto ErrExit;
	}

	/*����/�����ǽ������*/
	if( g_pub_tx.add_ind[0]=='0' )			/*�ǽ�,�൱��ȡ��,�ʻ�������*/
	{
		if( g_dd_parm.draw_ind[0]=='N' )
		{
			sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A023");
			goto ErrExit;
		}
		if( g_dd_parm.thr_draw_ind[0]=='N' 
			&& strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
		{
			sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A011");
			goto ErrExit;
		}

		/*** ��������Ϣ�����ĺ��� ***/
		/***if( g_dd_parm.acm_calc_type[0]!='3')	*�ǵ���*/
		{
			/**����ԭ���Ļ������� add by ligl 2010/7/16 11:22:04**/
			if ( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,g_dd_mst.bal,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
					goto ErrExit;
			}
			/**end**/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
	
			/*����ֶ���Ϣ����*/
			/**------- why ----???? -------------**
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type,
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_dd_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**----------------------------------**/
		}

		g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
		}

		if( g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"���˺��ѱ�ֻ����������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
		if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt )
		{
			sprintf(acErrMsg,"���˺��ѱ����ֶ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
		if(g_dd_mst.od_ind[0] != 'Y')
		{
			if( g_dd_mst.bal<g_dd_mst.ctl_amt+g_dd_mst.hold_amt )
			{
			sprintf(acErrMsg,"���˺������ڿ��ƽ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A030");
			goto ErrExit;
			}
		}
	}
	else if( g_pub_tx.add_ind[0]=='1' )		/*�Ǵ�,�ʻ��������*/
	{
		if( g_dd_parm.dpst_ind[0]=='N' )
		{
			sprintf(acErrMsg,"��������[%s]!",g_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			goto ErrExit;
		}
		if( g_dd_parm.thr_dpst_ind[0]=='N' 
			&& strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
		{
			sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			goto ErrExit;
		}

		/**if( g_dd_parm.acm_calc_type[0]!='3' )			*�ǵ��㻧*/
		{
			/*** ��������Ϣ�����ĺ��� ***/
			/**����ԭ���Ļ������� add by ligl 2010/7/16 11:22:04**/
			if ( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,g_dd_mst.bal,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
					goto ErrExit;
			}
			/**end**/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			/*����ֶ���Ϣ����*
			**------------ why --------????? --------**
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, 
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_dd_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------**/
		}

		g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
		}
	}
	else
	{
		sprintf(acErrMsg,"�����־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_dd_mst.lst_date=g_pub_tx.tx_date;

	if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.min_bal)<0 
		&& g_dd_mst.ac_sts[0]=='1' )
	{
		sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
			g_dd_mst.bal,g_dd_parm.min_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}
	if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_bal)>0 
		&& pub_base_CompDblVal(g_dd_parm.max_bal,0.00) )
	{
		sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
			g_dd_mst.bal,g_dd_parm.max_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A008" );
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	{
		ret=pub_ins_dd_hst1();
		ERR_DEAL
	}
	/*****add zgf 20130725****/

		pub_mob_sendmail( g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );

	/***add end zgf*****/
	ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Dd_mst_Clo_Upd();

			sprintf(acErrMsg,"������ڷֻ���PASS!");
			WRITEMSG
OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_td_mst1()
{
	int ret;
    long l_date;
    long num;
	double acm;

	/*** ���ö��ڲ�Ʒ���������麯�� ***/

	/*** �������ļ���� ***/
	ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	if( g_td_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"�˺�״̬������״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	if( g_td_mst.hold_sts[0]=='1' )
	{
		sprintf(acErrMsg,"���˺��ѱ���ȫ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P110");
		goto ErrExit;
	}

    /* �����ڲ�Ʒ�ṹ��ֵ */
    ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm," prdt_no='%s' ",
                      g_td_mst.prdt_no);
	ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;

	/*** ���ü�����Ϣ�����ĺ��� ***/
    if ( g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' )
        /* �������� ������ȡ */
    {
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}
    }

    if ( g_pub_tx.add_ind[0]=='1') 		/*ȡ��*/
	{
		if( g_pub_tx.ct_ind[0]=='1' && (g_td_parm.cif_type[0]=='1' || \
			g_td_parm.cif_type[0] =='2') )
		{
			sprintf(acErrMsg,"�Թ����ڲ�����ȡ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P113");
			goto ErrExit;
		}

        if (g_pub_tx.ct_ind[0]=='1' && g_td_parm.cash_ind[0] == 'N')
        {
            sprintf(acErrMsg,"�ò�Ʒ����������!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P223");
            goto ErrExit;
        }

		if( g_pub_tx.ct_ind[0]=='1' && g_td_mst.odtt_ind[0]=='1' )
		{
			sprintf(acErrMsg,"�㻧������ȡ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P108");
			goto ErrExit;
		}

        if( g_td_parm.thr_draw_ind[0]=='N' \
              && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
        {
            sprintf(acErrMsg,"����ͨ��[%s]!",g_td_parm.thr_draw_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A011");
            goto ErrExit;
        }

        /* ����ʻ���� */
        if (pub_base_CompDblVal(g_td_mst.bal,g_pub_tx.tx_amt1) == -1)
        {
            sprintf(acErrMsg,"ȡ�������ʻ����!!bal=[%lf] amt1=[%lf]",
                g_td_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P214");
            goto ErrExit;
        }

        if ((g_td_parm.td_type[0] != '3' && g_pub_tx.svc_ind==1001) \
            || g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004)
        {
		    g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
		    g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;
        }

	    if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	    {
	        g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
	    }

		if( g_td_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"���˺��ѱ�ֻ����������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}

		if( g_td_mst.hold_sts[0]=='3' && g_td_mst.bal<g_td_mst.hold_amt )
		{
			sprintf(acErrMsg,"���˺��ѱ����ֶ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
    
        /* ����Ƿ������ȡ��� */
        if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '6')
        /* �������� �������� */
        {
           sprintf(acErrMsg,"�ò�Ʒ��������ȡ���!!");
           WRITEMSG
           strcpy(g_pub_tx.reply,"P219");
           goto ErrExit;
        }

        /* ����Ǽ���Ƿ������� */
        if (g_td_parm.td_type[0] == '0')
        /* ��ͨ������ȡ */
        {
        	/********
			if (g_pub_tx.tx_date >= g_td_mst.mtr_date)
   			{
            	sprintf(acErrMsg,"���˻��Ѿ�����,������������!!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"A034");
				goto ErrExit;
 			}
          	********/
            if (g_td_parm.wdr_ind[0] == 'N')
            {
                sprintf(acErrMsg,"��ͨ������ȡ��������!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P215");
                goto ErrExit;
            }

            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1; /* ������� */

            if (g_td_parm.wdr_lmt_cnt != 0 \
                  && g_td_mst.tx_cnt > g_td_parm.wdr_lmt_cnt)
            {
                sprintf(acErrMsg,"������������涨�Ĵ���!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P216");
                goto ErrExit;
            }
        }

        /* ������ȡ���� */
        if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
            g_td_parm.draw_min_amt) == -1)
        {
            sprintf(acErrMsg,"ȡ����С�����ȡ����!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P217");
            goto ErrExit;
        }

        if (g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0] == '2')
        /* ������ȡ ����ȡϢ */
        {
		   if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 )
			{
			    sprintf(acErrMsg,"ȡ�����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P115");
				goto ErrExit;
			}

       		/*�汾ȡϢÿ��ȡ��Ϣ�����޸���ϸ���*/
            if (g_td_parm.td_type[0] == '3')
            {
                if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
   				{
      				g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
   				}
            }

			g_td_mst.tx_cnt++;
			if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
			{
				sprintf(acErrMsg,"ȡ����ܴ���!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P117");
				goto ErrExit;
			}
            /**********
            if (g_td_mst.ttl_cnt == g_td_mst.tx_cnt)
            {
                sprintf(acErrMsg,"���������һ�ν���Ӧ����������!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P229");
                goto ErrExit;
            }
            ***********/
            switch(g_td_parm.intvl_type[0])
            {
                case 'Y':
                        break;
                case 'Q':
                        break;
                case 'M':
                        num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                		ret = pub_base_deadlineM(g_td_mst.ic_date,num,
                        						 &l_date);
                		if (ret != 0)
                        {
                             sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"P228");
                             goto ErrExit;
                        }  
                        if (g_pub_tx.tx_date < l_date)
                        {
                             sprintf(acErrMsg,"��û�е�ÿ��ȡ���ʱ��!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"P218");
                             goto ErrExit;
                        }
                         break;
                case 'D':
                        num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                        ret = pub_base_deadlineD(g_td_mst.ic_date,num,
                   					             &l_date);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P228");
                            goto ErrExit;
                        }
                        if (g_pub_tx.tx_date < l_date)
                        {
                            sprintf(acErrMsg,"��û�е�ÿ��ȡ���ʱ��!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P218");
                            goto ErrExit;
                        }
                        break;
                 default:
                        break;
              }/*switch����*/
           }

           /* ������Ϣ */
           if (g_td_parm.td_type[0] != '2' && g_td_parm.td_type[0] != '3')
           /* ������ȡ �汾ȡϢ ���ü�����Ϣ */
           {
               ret = pub_base_CalIntst(&g_pub_intst);
               if (ret != 0)
               {
                   sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                   WRITEMSG
                   strcpy(g_pub_tx.reply,"P220");
                   goto ErrExit;
               }
               sprintf(acErrMsg,"ȡ����ϢΪ:,[%lf]",g_pub_intst.factval);
               WRITEMSG
           }
			
		/*����ȡ�����*/
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}

		/*����ֶ���Ϣ����*/
		if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
			g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
			0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
			g_dd_parm.intst_mon,g_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"����ֶ���Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}

		/*�ǼǷֶλ�����*/
		com_sect_acm_c.ac_id=g_pub_tx.ac_id;
		com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
		com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*��������*/
		com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*��������*/
		com_sect_acm_c.rate=g_dd_mst.rate/12;				/*������*/
		com_sect_acm_c.intst_acm=acm;						/*��Ϣ����*/
		strcpy(com_sect_acm_c.sts,"0");						/*״̬*/
		ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
		if(ret==-239)
		{
			sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D108" );
			goto ErrExit;;
		}
		ERR_DEAL
	}
	else if( g_pub_tx.add_ind[0]=='2')						/*�Ǵ�,���*/
	{
        if (g_td_parm.td_type[0] != '1')
        {
            sprintf(acErrMsg,"�ò�Ʒ�Ķ��ڴ�����Ͳ�����������!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P225");
            goto ErrExit;
        }
        if (g_td_parm.dpst_ind[0] == 'N')
        {
            sprintf(acErrMsg,"�ò�Ʒ����������!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P225");
            goto ErrExit;
        }
        if ( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
              && g_td_parm.thr_dpst_ind[0] == 'N')
        {
            sprintf(acErrMsg,"�ò�Ʒ������ͨ��!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P226");
            goto ErrExit;
        }

		g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
		g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
			g_td_mst.hst_cnt++;
		}

		if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 )
		{
			sprintf(acErrMsg,"������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P118");
			goto ErrExit;
		}

        if (pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_parm.dpst_min_amt) == -1)
        {
            sprintf(acErrMsg,"�����С�����������!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P227");
            goto ErrExit;
        }

		/*���������*/
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}

		/*����ֶ���Ϣ����*/
		if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
			g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
			0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
			g_dd_parm.intst_mon,g_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"����ֶ���Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}

		/*�ǼǷֶλ�����*/
		com_sect_acm_c.ac_id=g_pub_tx.ac_id;
		com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
		com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*��������*/
		com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*��������*/
		com_sect_acm_c.rate=g_dd_mst.rate/12;				/*������*/
		com_sect_acm_c.intst_acm=acm;						/*��Ϣ����*/
		strcpy(com_sect_acm_c.sts,"0");						/*״̬*/
		ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
		if(ret==-239)
		{
			sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D108" );
			goto ErrExit;;
		}
		ERR_DEAL
	}
	else
	{
		sprintf(acErrMsg,"�����־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_td_mst.lst_date=g_pub_tx.tx_date;

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_bal)<0 
		&& g_td_mst.ac_sts[0]=='1' )
	{
		sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
			g_td_mst.bal,g_td_parm.min_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}
	if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_bal)>0 
		&& pub_base_CompDblVal(g_td_parm.max_bal,0.00) )
	{
		sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
			g_td_mst.bal,g_td_parm.max_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A008" );
		goto ErrExit;
	}
    if (g_pub_tx.hst_ind[0]== '1') /*�ռ�����ϸ*/
    {
		strcpy(g_pub_tx.name,g_td_mst.name);
		g_pub_tx.cif_no=g_td_mst.cif_no;
		ret=pub_ins_td_hst();
		ERR_DEAL
    }
	ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Td_mst_Clo_Upd();
			sprintf(acErrMsg,"�����ڷֻ���PASS!");
			WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_ln_mst1()
{
	int ret;
	int do_flag=0;
	double acm;

	/*** ���ô����Ʒ���������麯�� ***/

	if( g_pub_tx.ac_get_ind[1]!='1' ) /**δ��ȡln_mst**/
	{
		ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		ERR_DEAL

		ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
			WRITEMSG
			if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}

	ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
	ERR_DEAL

	strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );
	strcpy( g_pub_tx.opn_br_no,g_ln_mst.opn_br_no );
	strcpy( g_pub_tx.name,g_ln_mst.name );
	g_pub_tx.cif_no=g_ln_mst.cif_no;

	/**�˻�״̬���**/
	if( g_ln_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�����˺�������![%s]",g_ln_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P120");
		goto ErrExit;
	}
	if( g_ln_mst.ac_sts[0]=='9' )
	{
		sprintf(acErrMsg,"�����˺��ѳ���![%s]",g_ln_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L036");
		goto ErrExit;
	}

	/*** ���ü�����Ϣ�����ĺ��� ***/
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2",g_ln_mst.bal,g_pub_tx.tx_amt1 , 
							&g_ln_mst.intst_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2",g_ln_mst.in_lo_intst,g_pub_tx.tx_amt2, 
							&g_ln_mst.in_lo_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2" , g_ln_mst.out_lo_intst , g_pub_tx.tx_amt3,
							&g_ln_mst.out_lo_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2" , g_ln_mst.cmpd_lo_intst , g_pub_tx.tx_amt4,
							&g_ln_mst.cmpd_lo_acm ,"0",0,0 );
		ERR_DEAL
	g_ln_mst.lst_date=g_pub_tx.tx_date;

	/**����ǻ�����Ϣ�����������ע���ֶλ����ɸ������Լ���**/

	if( g_pub_tx.add_ind[0]=='0' ) 				/*�ǽ�*/
	{
		if( g_pub_tx.svc_ind==3010 ) 			/*��������ӣ����ſ�*/
		{
			if( g_ln_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg, "״̬����[%s]",
					g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
				WRITEMSG
				strcpy( g_pub_tx.reply,"L067" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
				g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
			{
				sprintf(acErrMsg,
					"���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
					g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P122" );
				goto ErrExit;
			}
			g_ln_mst.bal+=g_pub_tx.tx_amt1;
			g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
			ret=pub_ins_ln_hst( 0 );
			ERR_DEAL
			do_flag=1;
		}

			/*����ֶ���Ϣ����*/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			/*�ǼǷֶλ�����*/
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*��������*/
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*��������*/
			com_sect_acm_c.rate=g_ln_mst.rate/12;				/*������*/
			com_sect_acm_c.intst_acm=acm;						/*��Ϣ����*/
			strcpy(com_sect_acm_c.sts,"0");						/*״̬*/
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			strcpy(g_pub_tx.add_ind,"1");
	}
	else if( g_pub_tx.add_ind[0]=='1' ) 		/*�Ǵ�,�ʻ�������*/
	{
		if( g_pub_tx.svc_ind==3010 ) /*�������٣�������*/
		{
			g_ln_mst.bal-=g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 )
			{
				sprintf(acErrMsg,"����������[%.2lf]>[%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
				WRITEMSG
				strcpy( g_pub_tx.reply,"L039" );
				goto ErrExit;
			}
			ret=pub_ins_ln_hst( 0 );
			ERR_DEAL
			do_flag=1;
		}

			/*����ֶ���Ϣ����*/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			/*�ǼǷֶλ�����*/
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*��������*/
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*��������*/
			com_sect_acm_c.rate=g_ln_mst.rate/12;				/*������*/
			com_sect_acm_c.intst_acm=acm;						/*��Ϣ����*/
			strcpy(com_sect_acm_c.sts,"0");						/*״̬*/
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			strcpy(g_pub_tx.add_ind,"0");
	}
	else
	{
		sprintf(acErrMsg,"�����־����[%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	if( do_flag==0 )
	{
		sprintf(acErrMsg,"��־����[%s][%d]",g_pub_tx.add_ind,g_pub_tx.svc_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	{
		g_ln_mst.hst_bal=g_ln_mst.hst_bal-g_pub_tx.tx_amt1;
		ret=pub_ins_ln_hst1( 1 );
		ERR_DEAL
	}

	ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	Ln_mst_Clo_Upd();
			sprintf(acErrMsg,"�������ֻ���PASS!");
			WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_in_mst1()
{
	struct com_item_c v_com_item;
	int ret;
	double acm;
	/*** �����ڲ���Ʒ���������麯�� ***/

	/*** �ڲ����ļ���� ***/
	ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**��ȡ��Ʒ����**/
	ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",g_in_mst.prdt_no);
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_in_parm.dc_code); 			/*��ƴ���*/

	/**��ȡ��Ŀ����**/
	ret = Com_item_Sel( g_pub_tx.reply,&v_com_item,
						"acc_hrt='%s'",g_in_parm.acc_hrt );
	ERR_DEAL
	if( g_in_mst.sts[0]!='1' && g_in_mst.sts[0]!='2' )
	{
		sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]", \
				g_in_mst.ac_id,g_in_mst.sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	g_in_mst.lst_date=g_pub_tx.tx_date;

	if( g_pub_tx.add_ind[0]=='0' )					/*��*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"��Ŀ����������");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A025");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='2' )
		{
			sprintf(acErrMsg,"��Ʒ����������");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A027");
			goto ErrExit;
		}

		if( v_com_item.dc_ind[0]=='2' )					/*��ĿΪ������*/
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

			/*����ֶ���Ϣ����*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------------------**/
		}
		else											/*��ĿΪ�跽��*/
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;

			/*����ֶ���Ϣ����*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**--------------------------------**/
			strcpy(g_pub_tx.add_ind,"1");
		}
	}
	else if ( g_pub_tx.add_ind[0]=='1' )	/*��*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"��Ŀ�������跽");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A024");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='1' )
		{
			sprintf(acErrMsg,"��Ʒ�������跽");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A026");
			goto ErrExit;
		}
		if( v_com_item.dc_ind[0]=='2' )					/*��ĿΪ������*/
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;

			/*����ֶ���Ϣ����*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, 
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------------**/
		}
		else											/*��ĿΪ�跽��*/
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

			/*����ֶ���Ϣ����*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"����ֶ���Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			*�ǼǷֶλ�����*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*��������*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*��������*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*������*
			com_sect_acm_c.intst_acm=acm;						*��Ϣ����*
			strcpy(com_sect_acm_c.sts,"0");						*״̬*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"�ǼǷֶλ��������!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**-------------------------------------------**/
			strcpy(g_pub_tx.add_ind,"0");
		}
	}
	else
	{
		sprintf(acErrMsg,"�����־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( pub_base_CompDblVal(g_in_mst.bal,0.00)<0 
		&& g_in_mst.od_ind[0]=='N')
	{
		sprintf(acErrMsg,"���˻�������͸֧![%.2f][%.2f]",
				g_in_mst.bal,g_pub_tx.tx_amt1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P124");
		goto ErrExit;
	}

	if( g_in_mst.od_ind[0]=='Y' 
		&& pub_base_CompDblVal(g_in_parm.od_amt,0.00)
		&& pub_base_CompDblVal(-g_in_mst.bal,g_in_parm.od_amt)>0 )
	{
		sprintf(acErrMsg,"͸֧����![%.2f][%.2f]",
				g_in_mst.bal,g_in_parm.od_amt );
		WRITEMSG
		strcpy(g_pub_tx.reply,"T004");
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	{
		ret=pub_ins_in_hst();
		ERR_DEAL
	}

	ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	In_mst_Clo_Upd();

			sprintf(acErrMsg,"�����ڲ��ֻ���PASS!");
			WRITEMSG
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*��Ŀ����*  �ɵ�ʵʱ������**/
int Chg_acchrt_mst1_old()
{
	int ret=0,i=0; 
	double amt;
	char wherelist[200];

	struct gl_mst_c g_gl_mst;
	struct com_item_c g_com_item;
	struct com_item_c f_com_item;
	struct tx_dc_br_rel_c g_tx_dc_br_rel;

	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&f_com_item,0x00,sizeof(struct com_item_c));

	pub_base_strpack(g_pub_tx.cur_no);
	if( !strlen(g_pub_tx.cur_no) ) /**Ĭ��Ϊ�����**/
	strcpy( g_pub_tx.cur_no,"01" );

	/*����Ŀ����*/
	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P100");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	/*��Ŀ���ͱ���Ϊ���ʻ�����*/
	if(g_com_item.acc_knd[0]!='3')
	{
		sprintf(acErrMsg,"�ÿ�Ŀ���Ͳ������ʻ���Ŀ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O073");
		goto ErrExit;
	}

	/**�������¼���Ŀ**/
	if(g_com_item.sub_acc_yn[0]=='Y')
	{
		sprintf(acErrMsg,"�ÿ�Ŀ�����¼���Ŀ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O073");
		goto ErrExit;
	}

	/**ȡ�ö�Ӧ�Ļ�ƻ���**/
	ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&g_tx_dc_br_rel,"tx_br_no='%s'",
						 g_pub_tx.tx_br_no );
	if( ret )
	{
		sprintf(acErrMsg,"���׻����޶�Ӧ��ƻ���[%s]",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S029");
		goto ErrExit;
	}

	memset( wherelist,0,sizeof(wherelist) );
	memcpy( &f_com_item,&g_com_item,sizeof(g_com_item) );
	pub_base_strpack( f_com_item.acc_hrt );
	pub_base_strpack( f_com_item.up_acc_hrt );

	strcpy( g_pub_tx.dc_code,"ACHR" );
	set_zd_data("0152",f_com_item.acc_hrt );

	sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
			 g_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no,f_com_item.acc_hrt );
	/**ȡ���ϼ���Ŀ**/
	for ( i=0;i<2;i++ )
	{
	 	if( !strcmp(f_com_item.acc_hrt,f_com_item.up_acc_hrt) )
	 	break;

		ret=Com_item_Sel(g_pub_tx.reply,&f_com_item,"acc_hrt='%s'",
			 			 f_com_item.up_acc_hrt);
		if( ret )
		{
			sprintf(acErrMsg,"�ÿ�Ŀ�����ֲ�����[%s]",f_com_item.up_acc_hrt);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P102");
		    goto ErrExit;
	    }
	  	pub_base_strpack( f_com_item.acc_hrt );
	  	pub_base_strpack( f_com_item.up_acc_hrt );
	   	strcat( wherelist,",'" );
	    strcat( wherelist,f_com_item.acc_hrt );
		strcat( wherelist,"'" );
	}

	 strcat( wherelist,")" );

	 sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
	 WRITEMSG

	/*�޸�����*/
	ret=Gl_mst_Dec_Upd(g_pub_tx.reply,wherelist);
	if( ret )
	{
		sprintf(acErrMsg,"��ѯ�����쳣!![%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	i=0;
	while(1)
	{
		ret=Gl_mst_Fet_Upd(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 )break;
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�����쳣!![%s]",g_pub_tx.ac_no);
			WRITEMSG
			goto ErrExit;
		}
		i++;

		switch(g_pub_tx.add_ind[0])								/*���׽������*/
		{	
			case '0':											/*��*/
					if(g_com_item.amt_dc_ind[0]=='2')
					{
						sprintf(acErrMsg,"�ÿ�Ŀֻ����Ǵ�!![%s]",g_pub_tx.ac_no);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P102");
						goto ErrExit;
					}

					if(g_com_item.dc_ind[0]=='1')				/*�账��*/
						g_gl_mst.dr_bal+=g_pub_tx.tx_amt1;		/*��ǰ�跽���*/
					else if(g_com_item.dc_ind[0]=='2')			/*������*/
						g_gl_mst.cr_bal-=g_pub_tx.tx_amt1;		/*��ǰ�������*/
					else if(g_com_item.dc_ind[0]=='0')			/*˫����*/
					{
						amt=g_gl_mst.dr_bal-g_gl_mst.cr_bal;
						amt+=g_pub_tx.tx_amt1;
						ret=pub_base_CompDblVal(amt,0.00);
						if(ret==1)
						{
							g_gl_mst.dr_bal=amt;		/*��ǰ�跽���*/
							g_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
						}
						else if(ret==-1)
						{
							g_gl_mst.cr_bal=-amt;		/*��ǰ�������*/
							g_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						}
						else
						{
							g_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
							g_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
						}
					}
					g_gl_mst.rdd_cnt+=1;		/*���ս����*/
					g_gl_mst.rdd_amt+=amt;		/*���ս跢����*/
					g_gl_mst.tddr_cnt+=1;		/*��Ѯ�����*/
					g_gl_mst.tddr_amt+=amt;		/*��Ѯ�跢����*/
					g_gl_mst.mdr_cnt+=1;		/*���½����*/
					g_gl_mst.mdr_amt+=amt;		/*���½跢����*/
					g_gl_mst.qdr_cnt+=1;		/*���������*/
					g_gl_mst.qdr_amt+=amt;		/*�����跢����*/
					g_gl_mst.ydr_cnt+=1;		/*��������*/
					g_gl_mst.ydr_amt+=amt;		/*����跢����*/
					break;
			case '1':												/*���׼Ǵ�*/
					if(g_com_item.amt_dc_ind[0]=='1')
					{
						sprintf(acErrMsg,"�ÿ�Ŀֻ����ǽ�!![%s]",g_pub_tx.ac_no);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P102");
						goto ErrExit;
					}

					if(g_com_item.dc_ind[0]=='1')				/*�账��*/
						g_gl_mst.cr_bal-=g_pub_tx.tx_amt1;		/*��ǰ�������*/
					else if(g_gl_mst.dc_ind[0]=='2')			/*������*/
						g_gl_mst.dr_bal+=g_pub_tx.tx_amt1;		/*��ǰ�������*/
					else if(g_com_item.dc_ind[0]=='0')			/*˫����*/
					{
						amt=g_gl_mst.dr_bal-g_gl_mst.cr_bal;
						amt=amt+g_pub_tx.tx_amt1;
						ret=pub_base_CompDblVal(amt,0.00);
						if(ret==1)
						{
							g_gl_mst.dr_bal=amt;		/*��ǰ�跽���*/
							g_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
						}
						else if(ret==-1)
						{
							g_gl_mst.cr_bal=-amt;		/*��ǰ�������*/
							g_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						}
						else
						{
							g_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
							g_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
						}
					}	
					g_gl_mst.rcd_cnt+=1;					/*���մ�����*/
					g_gl_mst.rcd_amt+=g_pub_tx.tx_amt1;		/*���մ�������*/
					g_gl_mst.tdcr_cnt+=1;					/*��Ѯ������*/
					g_gl_mst.tdcr_amt+=g_pub_tx.tx_amt1;	/*��Ѯ��������*/
					g_gl_mst.mcr_cnt+=1;					/*���´�����*/
					g_gl_mst.mcr_amt+=g_pub_tx.tx_amt1;		/*���´�������*/
					g_gl_mst.qcr_cnt+=1;					/*����������*/
					g_gl_mst.qcr_amt+=g_pub_tx.tx_amt1;		/*������������*/
					g_gl_mst.ycr_cnt+=1;					/*���������*/
					g_gl_mst.ycr_amt+=g_pub_tx.tx_amt1;		/*�����������*/
					break;
			default:
					sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P106");
					goto ErrExit;
					break;
		}
	}

	ret=Gl_mst_Upd_Upd(g_gl_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Update ���ʳ���!![%s]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	Gl_mst_Clo_Upd();

	if( i==0 )
	{
		sprintf(acErrMsg,"�����в����ڸÿ�Ŀg_gl_mst.acc_hrt=[%s]", \
				g_gl_mst.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O075");
		goto ErrExit;
	}

		sprintf(acErrMsg,"�����Ŀ����PASS!");
		WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_ins_ln_trace_log1( double amt )
{
	double v_amt;
	int ret=0;

	v_amt=g_pub_tx.tx_amt1;
	g_pub_tx.tx_amt1=amt;
	ret=pub_ins_trace_log();
	ERR_DEAL
	g_pub_tx.tx_amt1=v_amt;

OkExit:
	return 0;
ErrExit:
	return 1;
}
int pub_ins_trace_log1()
{
	struct trace_log_c	trace_log_c;
	int ret;

	memset(&trace_log_c,0x00,sizeof(struct trace_log_c));
	
	trace_log_c.trace_no=g_pub_tx.trace_no;
	trace_log_c.tx_date=g_pub_tx.tx_date;
	trace_log_c.tx_time=g_pub_tx.tx_time;

	strcpy(trace_log_c.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(trace_log_c.opn_br_no,g_pub_tx.opn_br_no);

	strcpy(trace_log_c.tx_code,g_pub_tx.tx_code);
    strcpy(trace_log_c.sub_tx_code,g_pub_tx.sub_tx_code);
	trace_log_c.svc_ind=g_pub_tx.svc_ind;

	strcpy(trace_log_c.cur_no,g_pub_tx.cur_no);
	strcpy(trace_log_c.ct_ind,g_pub_tx.ct_ind);
	strcpy(trace_log_c.ac_no,g_pub_tx.ac_no);
	trace_log_c.ac_seqn=g_pub_tx.ac_seqn;
	trace_log_c.ac_id=g_pub_tx.ac_id;

	strcpy(trace_log_c.note_type,g_pub_tx.note_type);
	strcpy(trace_log_c.note_no,g_pub_tx.beg_note_no);
	strcpy(trace_log_c.saving_notarize,"0");

	if( g_pub_tx.add_ind[0]=='1' )
		strcpy(trace_log_c.add_ind,g_pub_tx.add_ind);
	else
		strcpy(trace_log_c.add_ind,"0");
	trace_log_c.amt=g_pub_tx.tx_amt1;

	strcpy(trace_log_c.brf,g_pub_tx.brf);
	strcpy(trace_log_c.hst_ind,g_pub_tx.hst_ind);
	strcpy(trace_log_c.tel,g_pub_tx.tel);
	strcpy(trace_log_c.chk,g_pub_tx.chk);
	strcpy(trace_log_c.aut,g_pub_tx.auth);
	strcpy(trace_log_c.sts,"0");
	strcpy(trace_log_c.no_show,"0");

	if( pub_base_CompDblVal(trace_log_c.amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[1]=='1' )
	{
			get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
		g_pub_tx.trace_cnt++;
		trace_log_c.trace_cnt=g_pub_tx.trace_cnt;
			set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
		ret=Trace_log_Ins(trace_log_c,g_pub_tx.reply);
		if( ret )
		{
			vtcp_log("=====[%.2lf]",trace_log_c.amt);
			sprintf(acErrMsg,"�Ǽǽ�����ˮ����[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_dd_hst1()
{
	int ret;

	struct dd_mst_hst_c	dd_mst_hst_c;
	memset(&dd_mst_hst_c,0x00,sizeof(struct dd_mst_hst_c));

	dd_mst_hst_c.trace_no=g_pub_tx.trace_no;
	dd_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(dd_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(dd_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	dd_mst_hst_c.ac_id=g_pub_tx.ac_id;
	dd_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(dd_mst_hst_c.tx_code,g_pub_tx.tx_code);	
	strcpy(dd_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);	

	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' 
		|| g_pub_tx.add_ind[0]=='3' )
		strcpy(dd_mst_hst_c.add_ind,"0");	
	else
		strcpy(dd_mst_hst_c.add_ind,"1");	
    strcpy(dd_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	dd_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	dd_mst_hst_c.bal=g_dd_mst.hst_bal;
	dd_mst_hst_c.acbo_bal=g_dd_mst.acbo_bal;
	dd_mst_hst_c.intst_acm=g_dd_mst.intst_acm;
	dd_mst_hst_c.tx_date=g_pub_tx.tx_date;
	dd_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(dd_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strncpy(dd_mst_hst_c.brf,g_pub_tx.brf,10);	
	strcpy(dd_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(dd_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(dd_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(dd_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_dd_mst.hst_cnt++;
		dd_mst_hst_c.hst_cnt=g_dd_mst.hst_cnt;
		ret=Dd_mst_hst_Ins(dd_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǻ�����ϸ�ʴ���[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P127");
			goto ErrExit;
		}
	}

OkExit:	
	return 0;
ErrExit:
	return 1;
}

int pub_ins_td_hst1()
{
	int ret;
	struct td_mst_hst_c	td_mst_hst_c;
	memset(&td_mst_hst_c,0x00,sizeof(struct td_mst_hst_c));

	td_mst_hst_c.trace_no=g_pub_tx.trace_no;
	td_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(td_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(td_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	td_mst_hst_c.ac_id=g_pub_tx.ac_id;
	td_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(td_mst_hst_c.tx_code,g_pub_tx.tx_code);	
	strcpy(td_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);	
TRACE
	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
		strcpy(td_mst_hst_c.add_ind,"0");	
	else
		strcpy(td_mst_hst_c.add_ind,"1");	
    strcpy(td_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	td_mst_hst_c.bal=g_td_mst.hst_bal;
	td_mst_hst_c.acbo_bal=g_td_mst.acbo_bal;
	td_mst_hst_c.intst_acm=g_td_mst.intst_acm;
	td_mst_hst_c.tx_date=g_pub_tx.tx_date;
	td_mst_hst_c.tx_time=g_pub_tx.tx_time;
TRACE
	strcpy(td_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strcpy(td_mst_hst_c.brf,g_pub_tx.brf);	
	strcpy(td_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(td_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(td_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(td_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_td_mst.hst_cnt++;
		td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
		ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǶ�����ϸ�ʴ���[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P128");
			goto ErrExit;
		}
	}

    sprintf(acErrMsg,"�ǼǶ����ʻ���ϸ�ɹ�!!!");
    WRITEMSG
	return 0;
ErrExit:
    sprintf(acErrMsg,"�ǼǶ����ʻ���ϸʧ��!!!");
    WRITEMSG
	return 1;
}
/* mode 1���� 2���� 3���� 4���� */
int pub_ins_ln_hst1( int mode )
{
	struct ln_mst_hst_c	ln_mst_hst_c;
	int ret=0;

	memset(&ln_mst_hst_c,0x00,sizeof(ln_mst_hst_c));

	ln_mst_hst_c.trace_no=g_pub_tx.trace_no;

	strcpy(ln_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(ln_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
	ln_mst_hst_c.ac_id=g_pub_tx.ac_id;
	ln_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;

	strcpy(ln_mst_hst_c.tx_code,g_pub_tx.tx_code);
	strcpy(ln_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);
	strcpy(ln_mst_hst_c.add_ind,g_pub_tx.add_ind);
    strcpy(ln_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	ln_mst_hst_c.tx_date=g_pub_tx.tx_date;
	ln_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(ln_mst_hst_c.note_type,g_pub_tx.note_type);
	strcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no);

	strncpy(ln_mst_hst_c.brf,g_pub_tx.brf,10);
	strcpy(ln_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(ln_mst_hst_c.chk,g_pub_tx.chk);
	strcpy(ln_mst_hst_c.auth,g_pub_tx.auth);

	if( mode==0 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"0");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
		ln_mst_hst_c.bal=g_ln_mst.bal;
		ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
	}
	else if( mode==1 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"1");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
		ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
	}
	else if( mode==2 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"2");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
		ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
	}
	else if( mode==3 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"3");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
		ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
	}
	else if( mode==4 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"4");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
		ln_mst_hst_c.bal=g_ln_mst.bal;
		ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
	}
	else if( mode==5 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"5");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
		ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
	}
	else if( mode==6 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"6");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
		ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
	}
	else if( mode==7 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"7");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
		ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
	}
	else
	{
		sprintf(acErrMsg,"�ǼǴ�����ϸ��mode����[%d]!",mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P129");
		goto ErrExit;
	}

	ln_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;

	if ( g_pub_tx.hst_ind[0]!='1' ) goto GoodExit;

	if( ( pub_base_CompDblVal(ln_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_ln_mst.hst_cnt++;
		ln_mst_hst_c.hst_cnt=g_ln_mst.hst_cnt;

		ret=Ln_mst_hst_Ins(ln_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǴ�����ϸ�ʴ���[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P129");
			goto ErrExit;
		}
	}

	ret=pub_ins_ln_trace_log(ln_mst_hst_c.tx_amt);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

int pub_ins_in_hst1()
{
	int ret;
	struct in_mst_hst_c	in_mst_hst_c;
	memset(&in_mst_hst_c,0x00,sizeof(struct in_mst_hst_c));

	in_mst_hst_c.trace_no=g_pub_tx.trace_no;
	in_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(in_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(in_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	in_mst_hst_c.tx_date=g_pub_tx.tx_date;
	in_mst_hst_c.tx_time=g_pub_tx.tx_time;
	in_mst_hst_c.ac_id=g_pub_tx.ac_id;
	in_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
		strcpy(in_mst_hst_c.add_ind,"0");	
	else
		strcpy(in_mst_hst_c.add_ind,"1");	
	in_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	in_mst_hst_c.bal=g_in_mst.hst_bal;
	in_mst_hst_c.intst_acm=g_in_mst.intst_acm;
	strcpy(in_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strncpy(in_mst_hst_c.brf,g_pub_tx.brf,10);	
	strcpy(in_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(in_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(in_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(in_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_in_mst.hst_cnt++;
		in_mst_hst_c.hst_cnt=g_in_mst.hst_cnt;

		ret=In_mst_hst_Ins(in_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽ��ڲ���ϸ�ʴ���[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P128");
			goto ErrExit;
		}
	}
	
	return 0;
ErrExit:
	return 1;
}

static pub_sub_ac1()
{
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id_c;
	char v_ac_get_ind[2];
	int ret=0;

	v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(g_pub_tx.crd_no);

	/*���뿨�ŵ����*/
	if( strlen(g_pub_tx.crd_no) && ! strlen(g_pub_tx.ac_no) )
	{
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
	}

	/*�Ѿ���ȡ�˽����˺�*/
	if( g_pub_tx.ac_get_ind[0]=='1' ) 				
	{
		if( g_mdm_ac_rel.ac_id<=0 ) 
		{
			sprintf(acErrMsg,"��ʶac_get_ind ����[%s][%ld]",
					g_pub_tx.ac_get_ind,g_mdm_ac_rel.ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
		strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
	}
	else		/*δ��ȡ�����ʺ�*/
	{
		/*** ����˺š��˺�ID���˺���� ***/
		if( g_pub_tx.ac_no[0]!=NULL )
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
								"ac_no='%s' ",g_pub_tx.ac_no);
			if( ret )
			{
				sprintf(acErrMsg,"��ʾ�˺Ų�����[%s][%s]", \
						g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind);
				WRITEMSG
				if( ret==100 ) 
				{
					if( g_pub_tx.ac_wrk_ind[6]=='1' )
					{
						strcpy(g_pub_tx.reply,"P102");		/*���������ʺ�*/
						goto ErrExit2;
					}	
					strcpy(g_pub_tx.reply,"P102");
				}
				goto ErrExit;
			}
			if( g_pub_tx.ac_id && g_pub_tx.ac_id!=g_mdm_ac_rel.ac_id )
			{
				sprintf(acErrMsg,"�˺�[%s]���˺�ID[%ld]����һ����",
						g_pub_tx.ac_no,g_pub_tx.ac_id);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
			strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
			g_pub_tx.ac_get_ind[0]='1';
		}
		else if( g_pub_tx.ac_id )
		{
			/**û����ȡmdm_ac_rel,����������һ��**/
			g_pub_tx.ac_get_ind[0]='0';
			ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_pub_tx.ac_id);
			ERR_DEAL
			while(1)
			{
				ret=Mdm_ac_rel_Fet_Sel( &v_mdm_ac_rel,g_pub_tx.reply );
				if( ret==100 ) break;
				ERR_DEAL

				if( v_mdm_ac_rel.ac_seqn==g_pub_tx.ac_seqn )
				{
					memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
					g_pub_tx.ac_get_ind[0]='1';
					break;
				}
				else if( v_mdm_ac_rel.ac_seqn==ALL_SEQN && 
					(v_mdm_ac_rel.note_sts[0]=='0' || 
					v_mdm_ac_rel.note_sts[0]=='1') 
					&& g_mdm_ac_rel.ac_id!=g_pub_tx.ac_id )
				{
					g_pub_tx.ac_get_ind[0]='1';
					memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
				}
			}
			if( g_pub_tx.ac_get_ind[0]=='0' )
			{
				sprintf(acErrMsg,"mdm_ac_rel����ƥ���¼acid=[%d]ac_seqn=[%d]",
						g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn );
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
			strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
		}
		else
		{
			sprintf(acErrMsg,"�˺�[%s]���˺�ID[%ld]��Ϊ��",
					g_pub_tx.ac_no,g_pub_tx.ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P101");
			goto ErrExit;
		}
	}
	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

	/**����Ѿ������������ҵ��ıȽ�һ��**/
	if( g_pub_tx.ac_seqn>=0 )
	{
		if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && 
			g_mdm_ac_rel.ac_seqn!=g_pub_tx.ac_seqn )
		{
			sprintf(acErrMsg,"mdm_ac_rel��Ų�ͬacid=[%d]seqn=[%d]in[%d]",
					g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.ac_seqn );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}
	else if( g_pub_tx.ac_seqn==0 )
	{
		if( g_mdm_ac_rel.ac_seqn==ALL_SEQN )
		{
			if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
			  	g_pub_tx.ac_seqn = 200;
			else
			{
				ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
				if( ret ) goto ErrExit;
			}
		}
	    else
		{
			g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
		}
	}

	/**����״̬���**/
	if( g_pub_tx.ac_get_ind[0]=='1' )
	{
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

		if( g_mdm_ac_rel.note_sts[0]=='1' )
		{
			sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ״̬,������Ҵ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M009");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='2' )
		{
			sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ��֤״̬,��������ʧ��֤!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M010");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='3' )
		{
			sprintf(acErrMsg,"�ý����ѻ�֤!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M011");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='4' )
		{
			sprintf(acErrMsg,"���˻���������!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M017");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='5' )
		{
			sprintf(acErrMsg,"���˻���������!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M019");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='*' )
		{
			sprintf(acErrMsg,"�ý���������������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M012");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]!='0' )
		{
			sprintf(acErrMsg,"�ý���״̬������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M013");
			goto ErrExit;
		}
	}

	/*** ȡ�˻�����,��Ʒ���� ����Ѿ�ȷ���˾Ͳ�����***/
	if( g_pub_tx.ac_id_type[0]=='0' || g_pub_tx.ac_id_type[0]==NULL )
	{
		if( g_pub_tx.ac_seqn>0 )
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c, \
								"ac_id=%ld and ac_seqn=%d ", \
								g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		else
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
								"ac_id=%ld ",g_pub_tx.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
		strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
	}
	else
	{
		/**�Ѿ�ȷ������,������ȡ�˻����**/
		if( g_pub_tx.ac_seqn<=0 )
		{
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
								"ac_id=%ld and ac_id_type='%c' ",
								g_pub_tx.ac_id,g_pub_tx.ac_id_type[0]);
			if( ret )
			{
				sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
			strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
		}
	}

	if( g_pub_tx.ac_id_type[0]=='9' ) /**�ڲ��ʣ����������**/
	{
		if( g_pub_tx.ac_seqn==0 )
			g_pub_tx.ac_seqn=SINGLE_SEQN;
	}

	g_pub_tx.ac_get_ind[0]=v_ac_get_ind[0];

GoodExit:
	return 0;
ErrExit:
	return 1;
ErrExit2:
	return 100;
}

pub_sub_mst1()
{
	int ret=0 ;

	/*** ��鴦��ֻ���,��ϸ�� ***/
	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':	
				ret=Chg_dd_mst1();
				ERR_DEAL
				break;
		case '2':	
				/**---- xxx---- ���ڲ������������ ----**
				ret=Chg_td_mst1();
				ERR_DEAL
				**-------------------------------------**/
				sprintf(acErrMsg,"���ڲ������������![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P451");
				goto ErrExit;
		case '3':	
				/**----- xxx ---- �����ô���ĳ������ǽ��� ----**
				ret=Chg_ln_mst1();
				ERR_DEAL
				**---------------------------------------------**/
				sprintf(acErrMsg,"���ô���ĳ�������![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P452");
				goto ErrExit;
				break;
		case '9':	
				ret=Chg_in_mst1();
				ERR_DEAL
				break;
		default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",g_pub_tx.ac_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
		}	

	return 0;
ErrExit:
	return 1;
}
pub_sub_spe_do1()
{
	int ret;

	/*** ����Ƿ�����Ʋ�Ʒ ****/
	if( g_pub_tx.add_ind[0]=='2' )
	{
		ret=sql_count("mo_prdt_prot"," main_ac_id=%ld and sts='1' ", \
						g_pub_tx.ac_id,g_pub_tx.ac_id);
		if( ret!=0 )
		{
			sprintf(acErrMsg,"�û�����ƻ�Э����Ʒ��������![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P182");
			goto ErrExit;
       }
	}

	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='3' )
	{
		ret=pub_base_InsMo_opn_cls(g_pub_tx.name,"","2");
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
			WRITEMSG
			goto ErrExit;
		}
	}

	return 0;
ErrExit:
	return 1;
}
int chongzheng_cash()
{
	int ret;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;

	if(strcmp(g_pub_tx.ac_no,"10101")!=0)
		return(0);

	vtcp_log("%s%d �ֽ�������Ĺ�Աcash_mst \n",__FILE__,__LINE__);
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));

	if( !strlen(g_pub_tx.cur_no) ) /**Ĭ��Ϊ�����**/
		strcpy( g_pub_tx.cur_no,"01" );

	/*** ���ݽ��׻����ź͹�Ա����ȡ�ֽ��˺�ID ***/
	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	/* �������Ƿ���ڣ��Ƿ����� */
	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "�����ִ�" );
		WRITEMSG
		goto ErrExit;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"�ֽ��˺�δ���û�״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit;
	}
	vtcp_log("add_ind is [%s],amt is[%.2f]\n",g_pub_tx.add_ind,g_pub_tx.tx_amt1);
	/*	hst �ĸ����������־��101��Ŀ�Ľ���෴,�������¸�hst��add_ind ��ֵ */
	if( g_pub_tx.add_ind[0]=='0' ){
		cash_mst_c.bal=cash_mst_c.bal+g_pub_tx.tx_amt1;
		strcpy(cash_mst_hst_c.add_ind,"1");
	}
	else if( g_pub_tx.add_ind[0]=='1' ){
		cash_mst_c.bal=cash_mst_c.bal-g_pub_tx.tx_amt1;
		strcpy(cash_mst_hst_c.add_ind,"0");
	}
	else
	{
		sprintf(acErrMsg,"������־����![%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( cash_mst_c.bal<=-0.005)
	{
		sprintf(acErrMsg,"�ֽ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P152");
		goto ErrExit;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Cash_mst_Clo_Upd();

	/*** �Ǽ�Ǯ����ϸ ***/
	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,cash_mst_c.ac_no);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,"��������");
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}
	TRACE

OkExit:
	return 0;
ErrExit:
	return 1;
}
