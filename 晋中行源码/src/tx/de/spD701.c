/***************************************************************
* �� �� ��: spD701.c
* ��������: ͸֧Э��Ǽ�
* ��    ��: jack
* ������ڣ�2003��3��31��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "od_dd_rel_c.h"
#include "cif_basic_inf_c.h"

spD701()
{
	char tmp_ac[21];
	int ret=0,flag=0;

	struct	mdm_ac_rel_c	f_mdm_ac_rel;
	struct	od_dd_rel_c		g_od_dd_rel;
	struct	od_mst_c		f_od_mst;
	struct	od_mst_c		h_od_mst;
	struct	dd_mst_c		f_dd_mst;
	struct	cif_basic_inf_c	g_cif_basic_inf;

	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&f_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&h_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_dd_rel,0x00,sizeof(struct od_dd_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*ȡֵ*/
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0230",g_od_mst.prdt_no);
	get_zd_double("0390",&g_od_mst.max_bal);
	get_zd_double("0400",&g_od_mst.cash_max_bal);
	get_zd_double("0410",&g_od_mst.min_pay_amt);
	get_zd_double("0420",&g_od_mst.min_pay_ratio);
	g_od_mst.min_pay_ratio=g_od_mst.min_pay_ratio/100;
	get_zd_data("0310",tmp_ac);

	/*�ʺż��*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"���ʻ�������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"���˻�����״̬������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D140");
    	goto ErrExit;
 	}

	if(g_mdm_ac_rel.coll_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"���˻����ʴ��ڼ�ʰ״̬");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
    	goto ErrExit;
 	}

	ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
 	if( ret )
 	{
 		sprintf(acErrMsg,"�α궨�����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
    	goto ErrExit;
 	}

	ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"�ʺŲ������ڻ����ļ�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_dd_mst.ac_sts[0]!='1')
 	{
 		sprintf(acErrMsg,"�ʻ�״̬������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
    	goto ErrExit;
 	}

	if(g_dd_mst.hold_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"�ʻ�������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
    	goto ErrExit;
 	}

	strcpy(g_dd_mst.od_ind,"Y");			/*�޸�Ϊ��͸֧��Ʒ*/

	ret = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
 	if( ret )
 	{
 		sprintf(acErrMsg,"�޸Ļ����ļ�����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
    	goto ErrExit;
 	}

	Dd_mst_Clo_Upd();

	/*�����ʺż��*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&f_mdm_ac_rel,"ac_no='%s'",tmp_ac);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"�����ʻ�������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D137");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.note_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"�����ʺŽ���״̬������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D139");
    	goto ErrExit;
 	}

	if(f_mdm_ac_rel.coll_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"�����ʺŽ��ʴ��ڼ�ʰ״̬");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D138");
    	goto ErrExit;
 	}

	ret = Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst,"ac_id=%ld",f_mdm_ac_rel.ac_id);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"�����ʺŲ�����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D137");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_dd_mst.ac_sts[0]!='1')
 	{
 		sprintf(acErrMsg,"�����ʻ�״̬������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D136");
    	goto ErrExit;
 	}

	if(f_dd_mst.hold_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"�����ʻ��ѱ�����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D135");
    	goto ErrExit;
 	}

	/*���ڲ�Ʒ��͸֧��Ʒ��ϵ�Ƿ����*/
	ret = Od_dd_rel_Sel(g_pub_tx.reply,&g_od_dd_rel, \
						"dd_prdt_code='%s' and od_prdt_code='%s'", \
						g_dd_mst.prdt_no,g_od_mst.prdt_no);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"���ڲ�Ʒ��͸֧��Ʒ��ϵ������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D134");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*�����˻��Ƿ��Ѵ���͸֧��Ʒ��¼*/
	ret = Od_mst_Sel(g_pub_tx.reply,&f_od_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( !ret && f_od_mst.ac_sts[0]!='*' )flag=1;
	else if( !ret && f_od_mst.ac_sts[0]=='*' )flag=2;
	else if( ret&&ret!=100 )
 	{
 		sprintf(acErrMsg,"��ѯ͸֧�ļ��쳣ret=[%d]",ret);
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D133");
    	goto ErrExit;
 	}

	/*���ͻ����ͺ��޶�*/
	ret =Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'",g_od_mst.prdt_no);
	if( ret==100 )
 	{
 		sprintf(acErrMsg,"��͸֧��Ʒ������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D129");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							g_dd_mst.cif_no);
	if(ret==100)
 	{
 		sprintf(acErrMsg,"�����ڸÿͻ�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"C107");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(strcmp(g_cif_basic_inf.type,g_od_parm.cif_type))
 	{
 		sprintf(acErrMsg,"�ÿͻ����Ͳ��ɵǼǴ�͸֧��Ʒ");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D143");
    	goto ErrExit;
 	}

	if(g_od_mst.max_bal>g_od_parm.max_amt)
 	{
 		sprintf(acErrMsg,"���ɳ���͸֧����޶�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D132");
    	goto ErrExit;
 	}

	if(g_od_mst.cash_max_bal>g_od_parm.cash_amt)
 	{
 		sprintf(acErrMsg,"���ɳ����ֽ����͸֧�޶�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D131");
    	goto ErrExit;
 	}

	g_od_mst.ac_id=g_dd_mst.ac_id;
	g_od_mst.ac_seqn=g_dd_mst.ac_seqn;
	strcpy(g_od_mst.opn_br_no,g_pub_tx.tx_br_no);
	g_od_mst.bal=0.00;
	g_od_mst.drt_bal=0.00;
	g_od_mst.intst_amt=0.00;
	g_od_mst.intst_bal=0.00;
	/*g_od_mst.drt_intst_amt=0.00;*/
	g_od_mst.cash_bal=0.00;
	g_od_mst.ys_bal=0.00;
	g_od_mst.hst_bal=0.00;
	g_od_mst.intst_acm=0.00;
	g_od_mst.drt_intst_acm=0.00;
	g_od_mst.hst_cnt=0;
	g_od_mst.hst_pg=0;
	strcpy(g_od_mst.ac_sts,"1");
	g_od_mst.pay_ac_id=f_dd_mst.ac_id;
	g_od_mst.pay_ac_seqn=f_dd_mst.ac_seqn;
	strcpy(g_od_mst.mac,"");

	switch(flag)
	{
		case 0:
			/*�Ǽ�͸֧�ļ�*/
			ret=Od_mst_Ins(g_od_mst,g_pub_tx.reply);
			if(ret)
			{
				sprintf(acErrMsg,"�Ǽ�͸֧�ļ�ʧ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D142");
				goto ErrExit;
			}
			break;
		case 1:
			if( strncmp(f_od_mst.prdt_no,g_od_mst.prdt_no,3) )
			{
				sprintf(acErrMsg,"͸֧��Ʒ���ø���!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D142");
				goto ErrExit;
			}

			g_reply_int = Od_mst_Dec_Upd(g_pub_tx.reply, \
										 "ac_id=%ld and ac_seqn=%d", \
			  							 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
		  	if( g_reply_int )
			{
			   	sprintf(acErrMsg,"�����α����!");
				strcpy(g_pub_tx.reply, "P162");
				return -1;
			}

			g_reply_int = Od_mst_Fet_Upd(&h_od_mst,g_pub_tx.reply);
			if( g_reply_int==100 )
			{
				sprintf(acErrMsg,"��¼������[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( g_reply_int )
			{
			  	sprintf(acErrMsg,"ȡ��¼����!");
				strcpy(g_pub_tx.reply, "P162");
				return -1;
			}

			h_od_mst.max_bal=g_od_mst.max_bal;
			h_od_mst.cash_max_bal=g_od_mst.cash_max_bal;
			h_od_mst.pay_ac_id=g_od_mst.pay_ac_id;
			h_od_mst.pay_ac_seqn=g_od_mst.pay_ac_seqn;

			g_reply_int=Od_mst_Upd_Upd(h_od_mst,g_pub_tx.reply);
		  	if( g_reply_int )
			{
			   	sprintf(acErrMsg,"�����α����!");
				strcpy(g_pub_tx.reply, "P162");
				return -1;
			}

			Od_mst_Clo_Upd( );
			break;
		case 2:
			/*�޸�͸֧�ļ�*/
			memset( &f_od_mst,0x00,sizeof(struct od_mst_c) );
			ret = Od_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
								 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 			if( ret )
 			{
 				sprintf(acErrMsg,"�α궨�����");
  				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
    			goto ErrExit;
 			}

			ret = Od_mst_Fet_Upd(&f_od_mst,g_pub_tx.reply);
 			if( ret==100 )
 			{
 				sprintf(acErrMsg,"�ʺŲ�������͸֧�ļ�");
  				WRITEMSG
				strcpy(g_pub_tx.reply,"D148");
    			goto ErrExit;
 			}
			else if( ret )
			{
				sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			ret=Od_mst_Upd_Upd(g_od_mst,g_pub_tx.reply);
 			if( ret )
 			{
 				sprintf(acErrMsg,"�޸�͸֧�ļ�����");
  				WRITEMSG
				strcpy(g_pub_tx.reply,"D105");
    			goto ErrExit;
 			}

			Od_mst_Clo_Upd();
			break;
		default:
 			sprintf(acErrMsg,"FLAG=[%d]ֵ�쳣",flag);
  			WRITEMSG
			break;
	}

	/*�Ǽ�͸֧��ʷ�ļ�
	ret = pubf_od_hst(g_od_mst,0.00,"1","͸֧Э��Ǽ�");
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�͸֧��ʷ�ļ�ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"T028");
		goto ErrExit;
	}
	*/

	if( pub_ins_trace_log() )
	{
	  	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
	 	goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"͸֧Э��Ǽǳɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"͸֧Э��Ǽ�ʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
